#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "charsto.h"
#include "smc_op.h"

#include "smc_pro.h"

#include "toprealdef.h"
#include "dmx.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "nit.h"
#include "toprealdef.h"
#include "sys_dbg.h"

#define JAZZ_MINGXIN

#define CAS_TASK_PRIORITY_0 33 //refer sys_cfg.h MDL_CAS_TASK_PRIO_0
//#define CA_DEBUG
#define TR_CA_MSG_DEPTH (64)
#define TR_CA_MAX_MSG_QUEUE (16)
#define TR_BASE_NVRAM_ADDR   0x1ffc00
#define TR_MAX_ALLOWED_NVRAM_ADDR 0x1fffff
#define TR_MAX_SECTION_LENGTH                (12)
#define TASK_ID_BASE 100
#define FILTER_NUM 28
#define TR_MAX_DESC_NUM 0x08
#define TR_MAX_SEM_NUM (16)
#define DMX_MAX_SLOT_NUM                          28
#define CA_DMX_TASK_PRIORITY                         51
#define   STACK_SIZE       (2 * KBYTES)
#define TRCAS_CHAN_ENABLED (1)
#define TRCAS_CHAN_DISABLED (0)
#define TR_DESC_ON (1)
#define TR_DESC_OFF (0)
#define TR_INVALID_CHANNEL_ID (0xffff)
#define TR_FILTER_BUFFER_SIZE (16*1024 + 188)
#define TR_MAX_TASK_NUM (16)

#define TR_MAX_CHANNEL_NUM 8
#define TR_MAX_DATA_NUM (100)
#define TR_STATUS_FREE (0)
#define TR_STATUS_BUSY (1)
#define TR_MAX_BUFFER_NUM (32)

os_sem_t  g_sem_tr_caslock = {0};
#define MG_CAS_LOCK()      mtos_sem_take(&g_sem_tr_caslock, 0)
#define MG_CAS_UNLOCK()    mtos_sem_give(&g_sem_tr_caslock)

typedef struct {
   u32 msg_queue_id;
   u32 task_pri;
   u8 status;
}tr_task_struct_t;
tr_task_struct_t tr_tsk_record[TR_MAX_TASK_NUM] =
{
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE},
  {INVALID_MSGQ_ID, 0, TR_STATUS_FREE}
};
typedef struct
{
    dmx_chanid_t  ca_chan;
    U16 pid;
    U32  id;
    BOOL desc_on;
          /*!
        ABC
        */
    BOOL used;
}desc_attr_t;
typedef struct{
    u32 used;
    u8 filter_buf[TR_FILTER_BUFFER_SIZE];
}tr_buffer_t;
typedef struct tr_data_node_t{
    u8 data[1024];
    u16 len;
}tr_data_node_t;

typedef struct{
    u8 used;
    dmx_filter_setting_t  alloc_param;
    dmx_chanid_t handle;
    tr_buffer_t *data_buf;
}tr_filter_info_t;
/*!
   for topreal channel information store
  */
typedef struct{
    u8 used;
    u8 state;  /* 1: start  0: stop */
    tr_filter_info_t filter_info[FILTER_NUM];
    dmx_slot_setting_t slot_t;
    u16 max_filter_num;
    u16 max_filter_size;
    u16 actual_filter_num;
    u32 buffer_size;
    u16 chan_pid;
    void (*upcall_func)(u32 channel_id);
    tr_data_node_t data[TR_MAX_DATA_NUM];
    u8 data_head;
    u8 data_tail;
}tr_channel_info_t;

typedef struct tr_area_code_s{
    u16 area_code_nit_first;
    u16 area_code_nit_second;
    u16 area_code_card;
    BOOL area_code_nit_tag;
    BOOL first_nit_tag;
}tr_area_code_t;

S16 (* SC_ATR_Notify_fun)(U8, U8, U8 *, U8);
static desc_attr_t tr_desc[TR_MAX_DESC_NUM];
u32 tr_sem_list[TR_MAX_SEM_NUM] = {0, };
tr_channel_info_t tr_channel[TR_MAX_CHANNEL_NUM];
u32 tr_cas_lock = 0;
dmx_device_t *tr_dmx_dev = NULL;
scard_device_t *tr_smc_dev = NULL;
charsto_device_t *tr_charsto_dev = NULL;
extern cas_tr_priv_t *tr_priv;
//extern u32 cas_card_status;
tr_buffer_t tr_buffer[TR_MAX_BUFFER_NUM];

tr_area_code_t tr_area_code = {0 , 0 , 0, FALSE, FALSE};
extern void cas_tr_ca_notify(CAS_MESSAGE_CONTROL eCaMsg, U16 wDemux, U32 ulParam);
//#define CAS_S_CARD_REMOVE (cas_card_status)
/*
static u8* dmx_memory_malloc(u32 buf_size)
{
   return (u8 *)mtos_align_malloc(buf_size , 16);

}
*/
//#ifdef CA_DEBUG
static void data_dump(u8 *p_addr, u32 size)
{
  u32 i = 0;

  for(i = 0; i < size; i++)
  {
    OS_PRINTF("%02x ", p_addr[i]);
    if ((0 == ((i + 1) % 20)) && (i != 0))
    {
        OS_PRINTF("\n");
    }
  }
  OS_PRINTF("\n");
}
//#endif

static RET_CODE TRCAS_SMC_State_Access(U8 *state, U8 op)
{
    static U8 smc_state = 0;
    if(op == STATE_READ)
	 *state = smc_state;
    else if(op == STATE_WRITE)
	 smc_state = *state;
    return SUCCESS;
}

/***************************************************************************************/
/*--------- The blow functions are accomplished by user, and called by MCELL ----------*/
/***************************************************************************************/
/*API*/
/*-----------------OS--------------------------------------------------------*/
/*!
 * This function is used to allocate memory from the heap. A NULL pointer must be returned if
 * no enough memory of requested size can be allocated or if the requested size is zero.
 *
 * \param[in] ulSize - The memory size.
 *
 * Return
 *    The allocated memory pointer or NULL if no memory could be allocated.
 */
PVOID TRDRV_OS_AllocateMemory(U32 ulSize)
{
	char *p_buf = NULL;
	p_buf = mtos_malloc(ulSize);
	if(NULL == p_buf)
	{
		OS_PRINTF("%s alloc memory error. size:%d\n", ulSize);
	}
	return p_buf;
}

/*!
 * This function is called to release the previously allocated memory. A release
 * of NULL pointer must be safe.
 *
 * \param[in] pvMemory - The previously allocated memory pointer.
 *
 */
void  TRDRV_OS_FreeMemory(IN PVOID pvMemory)
{
	if(NULL == pvMemory)
		return;
	mtos_free(pvMemory);
}

/*!
 * This function will create a task used by MCELL. All created tasks must be able
 * to receive or send message mutually, and all messages use same memory to pass
 * pointer to data.
 *
 * \param[in] task - The task function entry, the parameter is optional, currently not used.
 * \param[in] iPriority - Priority of the task to be created. Priority range from 0 to 15.
 * \param[in] ulStackSize - Size of stack required of task.
 *
 * Return
 *    Return the task identifier.
 *
 */
U32   TRDRV_OS_CreateTask(IN void(*task)(void *arg), S16 iPriority, U32 ulStackSize)
{
	/*!
	  *  In this function, it return the message queue id since os does not support task id.
	  *
	  *  At the same time, the priority vendor provided does not meet out needs, so need to adjust
	  *  the priority to comply our priority mechanism.
	  *
	  */
	void *p_sec_buf = NULL;
	char name_buf[20] = "";
	U8 ret = 0;
	u32 queue_id = 0;
	U32* p_stack = NULL;
#ifdef CA_DEBUG
	OS_PRINTF( "%s : line: %d\n", __FUNCTION__, __LINE__);
#endif
	ulStackSize *= 2;
	p_stack = (U32*)mtos_nc_malloc(ulStackSize);
		
	OS_PRINTF("TRDRV_OS_CreateTask iPriority = %d\n",iPriority);
	OS_PRINTF("TRDRV_OS_CreateTask ulStackSize = %d\n",ulStackSize);

	MT_ASSERT(p_stack != NULL);
	if(TR_STATUS_BUSY == tr_tsk_record[iPriority].status)
	{
		OS_PRINTF("%s failed. Task priority %d duplicated.\n", __FUNCTION__, iPriority);
		return 0;
	}

	/* We support at most 10 tasks/queues, one queue correspond to one task,
	 so the task priority related to the queue. */
	sprintf(name_buf, "ca_svc_%d", iPriority);
	queue_id = mtos_messageq_create(TR_CA_MSG_DEPTH, (u8 *) name_buf);
	MT_ASSERT(queue_id != INVALID_MSGQ_ID);
	p_sec_buf = mtos_malloc(TR_MAX_SECTION_LENGTH * TR_CA_MSG_DEPTH);
	MT_ASSERT(p_sec_buf != NULL);

	ret = mtos_messageq_attach(queue_id,
	                   p_sec_buf,
	                   TR_MAX_SECTION_LENGTH,
	                   TR_CA_MSG_DEPTH);

	if(ret == FALSE)
	{
	   mtos_messageq_release(queue_id);
	   mtos_free(p_sec_buf);
	   mtos_nc_free(p_stack);
	   OS_PRINTF("CAS attach message queue failed.\n");
	return 0;
	}
	sprintf(name_buf, "ca_%d", iPriority);
	tr_tsk_record[iPriority].msg_queue_id =  queue_id;
	tr_tsk_record[iPriority].status= TR_STATUS_BUSY;
	if(0 != tr_priv->task_pri)
	   tr_tsk_record[iPriority].task_pri = tr_priv->task_pri + TR_MAX_TASK_NUM - iPriority;
	else
	   tr_tsk_record[iPriority].task_pri = CAS_TASK_PRIORITY_0 + TR_MAX_TASK_NUM -iPriority;
	DEBUG(TRCA, INFO, "adjust task_pri:%d\n", tr_tsk_record[iPriority].task_pri);
	ret = mtos_task_create((u8 *)name_buf, task, NULL, tr_tsk_record[iPriority].task_pri, p_stack, ulStackSize);
	if(ret == FALSE)
	{
		   mtos_messageq_release(queue_id);
	   mtos_free(p_sec_buf);
	   mtos_nc_free(p_stack);
	   tr_tsk_record[iPriority].msg_queue_id = INVALID_MSGQ_ID;
	   tr_tsk_record[iPriority].task_pri= 0;
	   tr_tsk_record[iPriority].status= TR_STATUS_FREE;
	   OS_PRINTF("CAS create task failed.\n");
		return 0;
	}
#ifdef CA_DEBUG
		OS_PRINTF( "%s : line: %d\n", __FUNCTION__, __LINE__);
#endif

	return tr_tsk_record[iPriority].msg_queue_id + TASK_ID_BASE;
	}


/*!
 * This function is used to suspend a task for the input period time. And the time of one
 * unit is 1 millisecond.
 *
 * \param[in] ulDelayTime - The reequest delay time(ms)
 *
 */
void  TRDRV_OS_DelayTask(U32 ulDelayTime)
{
    mtos_task_sleep(ulDelayTime);
}

/*!
 * This function is used to send message to a given task.
 *
 * \param[in] tTaskId - Task identifier of the destination task.
 * \param[in] psMsg - Message data pointer.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 *
 */
S16   TRDRV_OS_SendMessage(U32 tTaskId, IN CAS_OS_MESSAGE *psMsg)
{
    os_msg_t message = {0};
    s16 ret;

    message.is_ext = 1;
    message.para1 = (u32)psMsg;
    message.para2 = TR_MAX_SECTION_LENGTH;

    MT_ASSERT(tTaskId != INVALID_MSGQ_ID); // or tTaskId
    MT_ASSERT(psMsg != NULL);

    ret = mtos_messageq_send(tTaskId - TASK_ID_BASE, &message);
    if(ret != TRUE)
        OS_PRINTF("CAS send message failed.\n");

    return (ret == TRUE?0:1);
}

/*!
 * This function will receive a message from one given task. The relevant task
 * should be blocked while waiting for message.
 *
 * \param[in] tTaskId - Task identifier.
 *
 * Return
 *    Return the pointer to CAS_OS_MESSAGE memory.
 *
 */
CAS_OS_MESSAGE *TRDRV_OS_ReceiveMessage(U32 tTaskId)
{
    static os_msg_t p_msg = {0};
    s16 ret;

    MT_ASSERT(tTaskId != INVALID_MSGQ_ID);
    memset(&p_msg, 0, sizeof(os_msg_t));

    ret = mtos_messageq_receive(tTaskId - TASK_ID_BASE, &p_msg, 0);
    if(ret != TRUE)
    {
        OS_PRINTF("CAS receive message failed.\n");
        return NULL;
    }
    return ret?((CAS_OS_MESSAGE *)(p_msg.para1)):NULL;
}

static s32 TRCAS_SEM_Search_Free(void)
{
    int i;

    for(i = 0; i < TR_MAX_SEM_NUM; i++)
    {
        if(tr_sem_list[i] == 0)
            return i;
    }
    return -1;
}

static void TRCAS_SEM_Set(s32 sem_id, u32 semaphore)
{
    tr_sem_list[sem_id] = semaphore;
}

static s32 TRCAS_SEM_Check(u32 semaphore)
{
    int i;

    for(i = 0; i < TR_MAX_SEM_NUM; i++)
    {
        if(tr_sem_list[i] == semaphore)
            return 0;
    }
    for(i = 0; i < TR_MAX_SEM_NUM; i++)
    {
        OS_PRINTF("Ori Sem:%x TR Sem:%x\n", tr_sem_list[i], semaphore);
    }
    return -1;
}
/*!
 * This function allows a semphore to be created and initialized.
 *
 * \param[in] ulInitialCount - initial value of the created semphore.
 *           0 indicates not available; >0 indicates available.
 *
 * Return
 *    The semaphore identifier.
 *
 */
U32   TRDRV_OS_CreateSemaphore(U32 ulInitialCount)
{
    s32 sem_id = 0;
    u32 semaphore;

    MT_ASSERT(ulInitialCount >= 0);
    sem_id = TRCAS_SEM_Search_Free();
    MT_ASSERT(0 <= sem_id);
	/* We need to modify mtos_sem_create function to support the second parameter */
    if(mtos_sem_create((os_sem_t *)&semaphore, ulInitialCount) == TRUE)
    {
        TRCAS_SEM_Set(sem_id, semaphore);
        return semaphore;
    }
    OS_PRINTF("CAS create semphore failed. \n");
    return 0;
}

/*!
 * This function is used to signals the semaphore, it increment
 * the counter by one.
 *
 * Param[in] ulSemaphore - The semaphore identifier.
 * mtos_sem.h
 */
void  TRDRV_OS_SignalSemaphore(U32 ulSemaphore)
{
    MT_ASSERT((-1) != TRCAS_SEM_Check(ulSemaphore));
    mtos_sem_give((os_sem_t *)&ulSemaphore);
    OS_PRINTF("TRDRV_OS_SignalSemaphore:0x%x\n", ulSemaphore);
}

/*!
 * This function will wait for the semaphore to be signaled. If the semaphore is available,
 * decrement the counter by one.
 *
 * ulSemaphore - The semaphore identifier.
 * mtos_sem.h
 */
void  TRDRV_OS_WaitSemaphore(U32 ulSemaphore)
{
    OS_PRINTF("TRDRV_OS_WaitSemaphore:0x%x\n", ulSemaphore);
    MT_ASSERT((-1) != TRCAS_SEM_Check(ulSemaphore));
    mtos_sem_take((os_sem_t *)&ulSemaphore, 0);

}



/*-----------------SC--------------------------------------------------------*/
void TRCAS_SMC_Notify(scard_notify_t *p_event)
{
  /*!
    * Need to check how to get pbATR and bProtocol, maybe we can get the attribute by IOCTL.
    */
    U8 smc_state = 0;

    RET_CODE ret = ERR_FAILURE;
    u8 atr_buf[32] = "";
    scard_device_t *p_smc_dev = NULL;
    scard_atr_desc_t atr_desc = {0};
    //u16 area_code = 0;

    p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
    MT_ASSERT(p_smc_dev != NULL);
    MT_ASSERT(SC_ATR_Notify_fun != NULL);

    p_event->p_atropt = &atr_desc;
    switch(p_event->card_stat)
    {
        case SMC_CARD_INSERT:
            OS_PRINTF("cas: card inserted\n");
            smc_state = SMC_CARD_INSERT;
            ret = scard_reset(p_smc_dev);
            if(ret != SUCCESS)
            {
                OS_PRINTF("Smart card reset failed. ERR:%d\n", ret);
                return;
            }
            atr_desc.p_buf = atr_buf;
            mtos_task_sleep(100);
            ret = scard_active(p_smc_dev, &atr_desc);
            mtos_task_sleep(500);
            if(ret != SUCCESS)
            {
                OS_PRINTF("Smart card active failed. ERR:%d\n", ret);
                return;
            }

            OS_PRINTF("Smart card actived.\n");
            ret = SC_ATR_Notify_fun(0, SMC_CARD_INSERT, p_event->p_atropt->p_buf, p_event->m_protocol);
            /*
            if(MC_OK == MC_SCARD_GetRegionCode(&area_code))
            {
                tr_area_code.area_code_card = area_code;
            }
            */
            break;
        case SMC_CARD_REMOVE:
            OS_PRINTF("cas: card removed\n");
            smc_state = SMC_CARD_REMOVE;
            /*
            ret = scard_deactive(p_smc_dev);
            if(ret != SUCCESS)
                break;
            */
            ret = SC_ATR_Notify_fun(0, SMC_CARD_REMOVE, p_event->p_atropt->p_buf, p_event->m_protocol);
            break;

        default:
            break;
    }
    TRCAS_SMC_State_Access(&smc_state, STATE_WRITE);
    if(ret != SUCCESS)
        OS_PRINTF("Smart cart notify function return failed. ERR:%d\n", ret);

    return;
}

static s16 TRCAS_RCV_Data_Add(tr_channel_info_t *chan, u8 *data, u32 data_size)
{
    MT_ASSERT(NULL != chan && NULL != data && data_size > 0);
    memcpy(chan->data[chan->data_tail].data,data,data_size);
    chan->data[chan->data_tail].len = data_size;
    chan->data_tail++;
    if(TR_MAX_DATA_NUM <= chan->data_tail)
        chan->data_tail = 0;
    if(chan->data_head == chan->data_tail)
        chan->data_head++;

    return 0;
}
#if 0
static void TRCAS_RCV_Data_Del(tr_channel_info_t *chan, u32 data_size)
{
    if(chan->data_head == chan->data_tail)
        return;
    chan->data_head++;
    if(TR_MAX_DATA_NUM <= chan->data_head)
        chan->data_head = 0;
    return;
}
#endif
/*
static BOOL TRCAS_Check_area_code(void)
{
  u32 tr_area_code_nit = 0;
  OS_PRINTF("@@@@@ area_code_nit_tag:%d", tr_area_code.area_code_nit_tag);
  if(tr_area_code.area_code_nit_tag == FALSE)
    return TRUE;
  if(((tr_area_code.area_code_nit_second >> 8) & 0xff) == 1)
  {
    if((tr_area_code.area_code_nit_second & 0xff) != tr_area_code.area_code_card)
    {
      OS_PRINTF("ERR area code, nit:%x,%x card:%d\n",
                  tr_area_code.area_code_nit_first,
                  tr_area_code.area_code_nit_second,
                  tr_area_code.area_code_card);
      tr_area_code_nit = (tr_area_code.area_code_nit_first << 16
                          | tr_area_code.area_code_nit_second);
      cas_tr_ca_notify(CAS_MC_NOTIFY_CURRENT_STATE, CAS_STATE_E24, tr_area_code_nit);
      return FALSE;
    }
  }
  return TRUE;
}
*/
void TRCAS_Parse_nit(handle_t handle, dvb_section_t *p_sec)
{
  handle_t org_handle = handle;
  dvb_section_t *p_org_sec = p_sec;
  u8 *p_buf = p_sec->p_buffer;
  s16 network_desc_len = 0;
  s16 desc_len = 0;
  u16 offset = 0;
  //u16 area_code = 0;
  S16 ret  = SUCCESS;

  if(tr_area_code.first_nit_tag == TRUE)
  {
    parse_nit(org_handle, p_org_sec);
    return;
  }
  tr_area_code.first_nit_tag = TRUE;
  OS_PRINTF("TRCAS_Parse_nit!!!!!!!!!!!!!!!!!!!!!!!!\n");
  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    parse_nit(org_handle, p_org_sec);
    return;
  }

  if((p_buf[5] & 0x01)== 0)
  {
    parse_nit(org_handle, p_org_sec);
    return;
  }

  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  //OS_PRINTF("%d,%d,%d,%d,%d,%d,%d, %d, %d, %d, %d \n",
  // p_buf[10], p_buf[11], p_buf[12], p_buf[13], p_buf[14], p_buf[15], p_buf[16], p_buf[17], p_buf[18], p_buf[19],p_buf[20]);

  offset = 10 + 2+ p_buf[11];
  OS_PRINTF("network_desc_len[%d]\n", network_desc_len);

  if(network_desc_len > 0)
  {
    //net_name_des_len = p_buf[offset + 1];
    if(p_buf[offset] == 0x5f)
    {
      OS_PRINTF("\n");
      tr_area_code.area_code_nit_tag = TRUE;
      //Skip tag
      offset ++;
      network_desc_len --;

      //Fetch desc length
      desc_len = p_buf[offset];
      OS_PRINTF("desc_len:%d\n", desc_len);
      //Skip desc length
      offset ++;
      network_desc_len --;

      tr_area_code.area_code_nit_first = MAKE_WORD(p_buf[offset + 1], p_buf[offset]);
      tr_area_code.area_code_nit_second = MAKE_WORD(p_buf[offset + 3], p_buf[offset + 2]);
    }
  }

  if(tr_area_code.area_code_nit_tag)
  {
    ret = MC_PostRegionCode(tr_area_code.area_code_nit_second & 0xff);
  }
  if(MC_OK != ret)
  {
     OS_PRINTF("Set region code failed. Error code:%d\n", ret);
  }
  parse_nit(org_handle, p_org_sec);
}

static u8 *TRCAS_RCV_Data_Get(tr_channel_info_t *chan, u32 *data_size)
{
    u8 idx = 0;
    if(chan->data_head == chan->data_tail)
        return NULL;

    *data_size = chan->data[chan->data_head].len;
    idx = chan->data_head;
    chan->data[idx].len = 0;

    chan->data_head++;
    if(TR_MAX_DATA_NUM <= chan->data_head)
        chan->data_head = 0;


    return chan->data[idx].data; //chan->data[chan->data_head].data;
}


unsigned long m_Table[256] =
{
	0x00000000, 0x04C11DB7, 0x9823B6E, 0xD4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2,
	0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3,
	0x3C8EA00A, 0x384FBDBD, 0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
	0x5BD4B01B, 0x569796C2, 0x52568B75,  0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011,
	0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,  0x9823B6E0, 0x9CE2AB57, 0x91A18D8E,
	0x95609039,0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,  0xBE2B5B58, 0xBAEA46EF,
	0xB7A96036,0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,  0xD4326D90,
	0xD0F37027,0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
	0xF23A8028,0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A,
	0xEC7DD02D,0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C,
	0x2E003DC5,0x2AC12072,  0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x18AEB13,
	0x54BF6A4,0x808D07D, 0xCC9CDCA,  0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB,
	0x6F52C06C, 0x6211E6B5, 0x66D0FB02,  0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066,
	0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,  0xACA5C697, 0xA864DB20, 0xA527FDF9,
	0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,  0x8AAD2B2F, 0x8E6C3698,
	0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,  0xE0B41DE7,
	0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
	0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED,
	0xD8FBA05A,  0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85,
	0x738AAD5C, 0x774BB0EB,  0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
	0x58C1663D, 0x558240E4, 0x51435D53,  0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47,
	0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,  0x315D626, 0x7D4CB91, 0xA97ED48,
	0xE56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,  0xF12F560E, 0xF5EE4BB9,
	0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,  0xD727BBB6,
	0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
	0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC,
	0xA379DD7B,  0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD,
	0x81B02D74, 0x857130C3,  0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645,
	0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,  0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
	0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,  0x119B4BE9, 0x155A565E, 0x18197087,
	0x1CD86D30, 0x29F3D35, 0x65E2082, 0xB1D065B, 0xFDC1BEC,  0x3793A651, 0x3352BBE6,
	0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,  0xC5A92679,
	0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
	0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673,
	0xFDE69BC4,  0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
	0x933EB0BB, 0x97FFAD0C,  0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,0xBCB4666D,
	0xB8757BDA, 0xB5365D03, 0xB1F740B4,
};

unsigned long CRC32(unsigned char* buffer, unsigned long size)
{
	unsigned long Result = 0xFFFFFFFF;

	while (size--)
	{
		Result = (Result << 8) ^ m_Table[(Result >> 24) ^ *buffer++];
	}

	return Result;
}
static u8 section_filter_crc32_checkout(U32 length,U8 *data)
{
	U32 nCrc32Result = 0;
	U32 nCrc32 = 0;
	U8  *pdata =NULL;

	length = length - 4;
	nCrc32Result = CRC32(data, length);

	pdata = data + length;
	nCrc32 = ((pdata[0]<<24)&0xff000000) | ((pdata[1]<<16)&0x00ff0000)
		| ((pdata[2]<<8)&0x0000ff00) | ((pdata[3]<<0)&0x000000ff);

	if(nCrc32Result == nCrc32)
	{
		return  0;
	}
	else
	{
		return  1;
	}


}


 void TRCAS_ChannelData_Detect(void *param)
{
    tr_channel_info_t *chan = NULL;
    tr_filter_info_t *filter = NULL;
    s16 ret;
    u8 *p_data = NULL;
    U32 data_len = 0;
    u16 chan_idx, filter_idx;


    while(1)
    {
        for(chan_idx = 0; chan_idx < TR_MAX_CHANNEL_NUM; chan_idx++)
        {
            mtos_task_sleep(2);
            chan = &tr_channel[chan_idx];

            if(NULL == chan || TRCAS_CHAN_DISABLED == chan->state)
                continue;
            for(filter_idx = 0; filter_idx < chan->actual_filter_num; filter_idx++)
            {
                p_data = NULL;
                filter = &chan->filter_info[filter_idx];
                MT_ASSERT(NULL != filter);
                ret = dmx_si_chan_get_data(tr_dmx_dev, filter->handle, &p_data, &data_len);
                
                if(data_len > 1024)
                  continue;
                
                if(ERR_NO_RSRC != ret)
                {

                  if(p_data[0] >= 0x80 && p_data[0] <= 0x8f)
                  {
                      if(section_filter_crc32_checkout(data_len,p_data) == 0)
                      {
                        ret = TRCAS_RCV_Data_Add(chan, p_data, data_len);
                        if(SUCCESS == ret && NULL != chan->upcall_func)
                        {
                    #ifdef CA_DEBUG
                            OS_PRINTF("TR Channel[%d] Filter[%d] Handle[%d] Len:%d\nRCV Data:\n", chan_idx, filter_idx, filter->handle, data_len);
                            data_dump(p_data, chan->max_filter_size);
                     #endif
                            chan->upcall_func(chan_idx);
                        }
                      }
                      else
                      {
                        mtos_printk("\ncrc error!!!!!!!tableid=0x%02x\n",p_data[0]);
                      }
                  }
                  else
                  {
                    mtos_printk("\nerror!!!!!!!tableid=0x%02x\n",p_data[0]);
                  }


                }
            }
        }
		mtos_task_sleep(20);
    }
}
/*!
 * This function is called once after system boot up to initialize the driver.
 *
 *
 * \param[in] A notify function pointer. it must called when smart card is inserted
 *           or removed, Please set ATR data to 0 if the card is removed.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 */
S16   TRDRV_SCARD_Initialise(IN S16 (*SC_ATR_Notify)(U8 bCardNumber, U8 bStatus, U8 *pbATR, U8 bProtocol))
{
    RET_CODE ret = ERR_FAILURE;
    U32 ulStackSize = STACK_SIZE;
    U8 dmx_ret = 0;
    U32* p_stack = (U32*)mtos_malloc(ulStackSize);
	u32 task_pri = 0;

#ifdef CA_DEBUG
		 OS_PRINTF( "%s : line: %d\n", __FUNCTION__, __LINE__);
#endif
    MT_ASSERT(p_stack != NULL && SC_ATR_Notify != NULL);
    tr_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
    MT_ASSERT(NULL != tr_smc_dev);
    SC_ATR_Notify_fun = SC_ATR_Notify;
    ret = scard_pro_register_op(tr_smc_dev);
    if(SUCCESS != ret)
    {
        OS_PRINTF("Smart card register notify function failed. ERR:%d\n", ret);
        return ERR_FAILURE;
    }
    ret = mtos_sem_create((os_sem_t *)&tr_cas_lock, 1);
    if (!ret)
        OS_PRINTF("Create semaphore failed\n");


	if(0 != tr_priv->task_pri)
		task_pri = tr_priv->task_pri + TR_MAX_TASK_NUM - 1;//YANGYF
	else
		task_pri = CAS_TASK_PRIORITY_0 + TR_MAX_TASK_NUM - 1;//YANGYF
	DEBUG(TRCA, INFO, "task_pri:%d\n",task_pri);
    dmx_ret = mtos_task_create((u8 *)"TRCAS_ChannelData_Detect", TRCAS_ChannelData_Detect, NULL, task_pri, p_stack, ulStackSize);
    MT_ASSERT(TRUE == dmx_ret);
    ret = scard_reset(tr_smc_dev);
    if(SUCCESS != ret)
    {
        OS_PRINTF("Smart card reset failed. ERR:%d\n", ret);
        return ERR_FAILURE;
    }

    return 0;
}

/*!
 * This function used to reset the smart card, and then
 * the driver must call SC_ATR_Notify to notify the MCELL
 * the card status and ATR sequence.
 *
 * Param[in] The smart card number of the card reset.
 */
void  TRDRV_SCARD_ResetCard(U8 bCardNumber)
{
    scard_device_t *p_smc_dev = NULL;

    p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
    MT_ASSERT(NULL != p_smc_dev);
    mtos_task_delay_ms(10);
    scard_reset(p_smc_dev);
}
/*!
 * This function is used to transfer data to smart card and return the response
 * data from smart card for t = 0 protocol.
 *
 * \param[in] bCardNumber - The card number of the smart card to be sent data.
 * \param[in] pbTxData - Pointer of the data buffer to send to smart card, This data
 *           include T=0 "command header" and the data to be sent.
 * \param[in] ulTxLength - The length of the data to be sent to smart card.
 * \param[out] pbRxData - Pointer of the received data returned from smart card(SW1, SW2)
 * \param[out] pulRxLength - The total length of the data return from smart card. It includes
 *            the 2 bytes of SW1 and SW2.
 *
 * Return
 *   0 - Successful
 *   1 - Failed
 */
S16   TRDRV_SCARD_AdpuFunction(U8 bCardNumber, IN U8 *pbTxData, U32 ulTxLength, OUT U8 *pbRxData, OUT U32 *pulRxLength)
{

  scard_opt_desc_t rwopt;
  RET_CODE ret= SUCCESS;
#ifndef JAZZ_MINGXIN
  u8 status[2] = {0, 0};
#endif
  s8 retry_times = 1;

  rwopt.p_buf_out = pbTxData;
  rwopt.bytes_out = ulTxLength;
  rwopt.p_buf_in = pbRxData;
  rwopt.bytes_in_actual = 0;
  rwopt.bytes_in_target = 0;
  if(pbTxData[0] == 0 && pbTxData[1] == 0x3e &&
  		pbTxData[2] == 0 && pbTxData[3] == 0 && pbTxData[4] == 0xbd)
  {
		return 1;
  }

  do
  {
#ifdef CA_DEBUG
      OS_PRINTF("Tx:\n");
      data_dump(pbTxData, 10 > ulTxLength ? ulTxLength:10);
#endif
#ifndef JAZZ_MINGXIN
      mtos_task_lock();
      ret = scard_pro_t0_transaction(&rwopt, status);
      mtos_task_unlock();
#else
      ret = scard_pro_rw_transaction(&rwopt);
#endif
      if(ret != SUCCESS)
          OS_PRINTF("++++ret=%d\n",ret);
      if(ret == SUCCESS)
         break;
      retry_times--;
  }while(retry_times >= 0);
#ifndef JAZZ_MINGXIN
  memcpy(rwopt.p_buf_in + rwopt.bytes_in_actual, status, sizeof(status));
  *pulRxLength = rwopt.bytes_in_actual + sizeof(status);
#else
  *pulRxLength = rwopt.bytes_in_actual;// + sizeof(status);
#endif


#ifdef CA_DEBUG
      OS_PRINTF("Rx:\n");
      data_dump(rwopt.p_buf_in, *pulRxLength);
#endif
  if(ret != SUCCESS)
  {
      OS_PRINTF("%s transaction data failed. ERR:%d\n", __FUNCTION__, ret);
      OS_PRINTF("Tx:\n");
      data_dump(pbTxData, ulTxLength);
      mtos_task_delay_ms(4);
      return 1;
  }
  mtos_task_delay_ms(4);
  return 0;
}
/*!
 * This function is used to get the inserted/removed state of the smart card in one card slot.
 *
 * Param[in] bCardNumber - The smart card number to get state.
 * Param[out] The pointer of the returned state. 0 is inserted, 1 is removed.
 *
 * Return
 *    0 - Successful
 *    1 - Failed to get state
 */
S16   TRDRV_SCARD_GetSlotState(U8 bCardNumber, OUT U8 *pbSlotState)
{
    TRCAS_SMC_State_Access(pbSlotState, STATE_READ);
    return 0;
}

/*----------------DEMUX------------------------------------------------------*/
/*!
 * This function is used to initialize the DEMUX driver.
 *
 * Return
 *     0 - Successful
 *     1 - Failed
 */
S16   TRDRV_DEMUX_Initialise(void)
{
    u32 chan_idx;

    tr_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != tr_dmx_dev);
    MT_ASSERT(NULL != tr_dmx_dev->p_base);
    MT_ASSERT(TRUE == mtos_sem_create(&g_sem_tr_caslock, TRUE));
    for(chan_idx = 0; chan_idx < TR_MAX_CHANNEL_NUM; chan_idx++)
    {
        tr_channel[chan_idx].used = 0;
    }

    for(chan_idx = 0; chan_idx < TR_MAX_DESC_NUM; chan_idx++)
    {
         memset(&(tr_desc[chan_idx]), 0, sizeof(desc_attr_t));
         tr_desc[chan_idx].ca_chan = TR_INVALID_CHANNEL_ID;
    }
    for(chan_idx = 0; chan_idx < TR_MAX_BUFFER_NUM; chan_idx++)
    {
        memset(&tr_buffer[chan_idx], 0, sizeof(tr_buffer_t));
        tr_buffer[chan_idx].used = 0;
    }

    return 0;
}
/*!
 *  This function will allocate a section channel to filter data from mpeg TS.
 *
 *  \param[in] wMaxFilterNumber - Maximum number of the section filters on this channel.
 *  \param[in] wMaxFilterSize - Maximum number of the match and mask bytes in the filter.
 *             It indicates the depth of the filter.
 *  \param[in] ulBufferSize - The buffer size of the channel.
 *
 *  Return
 *     The channel number will be returned. It is on identifier of this channel.
 *     The value is from 0 to CAS_DEMUX_INVALID_CHANNEL_ID
 */
U32   TRDRV_DEMUX_AllocateSectionChannel(U16 wMaxFilterNumber, U16 wMaxFilterSize, U32 ulBufferSize)
{
    u32 chan_idx;
    tr_channel_info_t *chan;
    MG_CAS_LOCK();
    for(chan_idx = 0; chan_idx < TR_MAX_CHANNEL_NUM; chan_idx++)
    {
        if(tr_channel[chan_idx].used == 0)
        {
            //OS_PRINTF("Allocated channel ID is %d.\n Max filter number: %d, Max filter size: %d, Buffer size: %d\n",
             //   chan_idx, wMaxFilterNumber, wMaxFilterSize, ulBufferSize);
            break;
         }
    }
    if(TR_MAX_CHANNEL_NUM <= chan_idx)
    {
        OS_PRINTF("All channel are busy, allocate section channel failed.\n");
        MG_CAS_UNLOCK();
        return 0;
    }
    MT_ASSERT(DMX_SECTION_FILTER_SIZE >= wMaxFilterSize);

    chan = &tr_channel[chan_idx]; //mtos_malloc(sizeof(tr_channel_info_t));
    //chan = TRDRV_OS_AllocateMemory(sizeof(tr_channel_info_t));
    MT_ASSERT(NULL != chan);
    memset(chan, 0, sizeof(tr_channel_info_t));

    chan->buffer_size = ulBufferSize;
    chan->max_filter_num = wMaxFilterNumber;
    chan->max_filter_size = wMaxFilterSize;
    chan->actual_filter_num = 0;
    chan->state = TRCAS_CHAN_DISABLED;
    chan->upcall_func = NULL;
    chan->chan_pid = 0;
    chan->data_head = 0;
    chan->data_tail = 0;
    chan->used = 1;
    //tr_channel[chan_idx] = chan;

#ifdef CA_DEBUG
    OS_PRINTF( "%s : line: %d ch idx:%d, addr:0x%x addr:0x%x\n", __FUNCTION__, __LINE__, chan_idx, &tr_channel[chan_idx], chan);
#endif
    MG_CAS_UNLOCK();
    return chan_idx;
}
/*!
 * This function will set PID to a section channel which has been allocated.
 * The channel should be disabled before calling the TRDRV_DEMUX_ControlChannel.
 *
 * Param[in] ulChannelId - The identifier of the channel to allocate PID.
 * Param[in] wChannelPid - The PID value.
 *
 */

void  TRDRV_DEMUX_SetChannelPid(U32 ulChannelId, U16 wChannelPid)
{
    tr_channel_info_t *chan = NULL;

#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d : CIP:%d PID:%d\n", __FUNCTION__, __LINE__, ulChannelId, wChannelPid);
#endif
    MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
    MG_CAS_LOCK();
    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);
    chan->chan_pid = wChannelPid;
    MG_CAS_UNLOCK();
    return;
}
static u16 TRCAS_GET_EmptyBuffer(void)
{
     u16 idx;
     for(idx = 0; idx < TR_MAX_BUFFER_NUM; idx++)
     {
     	    if(tr_buffer[idx].used == 0)
                break;
     }
     return idx;
}

static s16 TRCAS_SET_Filter(tr_filter_info_t *filter, u16 pid)
{
     s16 ret = ERR_FAILURE;
     dmx_chanid_t si_channel_id = TR_INVALID_CHANNEL_ID;
     dmx_slot_setting_t slot;
     tr_buffer_t *data_buf = NULL;

#ifdef CA_DEBUG
    OS_PRINTF("channelid:0x%x \n", filter->handle);
    OS_PRINTF("Mask: \n");
    data_dump(filter->alloc_param.mask, DMX_SECTION_FILTER_SIZE);
    OS_PRINTF("Data: \n");
    data_dump(filter->alloc_param.value, DMX_SECTION_FILTER_SIZE);
#endif
     memset(&slot, 0, sizeof(dmx_slot_setting_t));
     slot.pid   = pid;
     slot.type = DMX_CH_TYPE_SECTION;
     if(TR_INVALID_CHANNEL_ID == filter->handle)
     {
         ret = dmx_si_chan_open(tr_dmx_dev, &slot, &si_channel_id);
         MT_ASSERT(SUCCESS == ret);
         filter->handle = si_channel_id;
     }

     if(NULL == filter->data_buf)
     {
         ret = TRCAS_GET_EmptyBuffer();
         if(ret < TR_MAX_BUFFER_NUM)
             data_buf = &tr_buffer[ret];//(u8 *)mtos_nc_malloc(TR_FILTER_BUFFER_SIZE);
         else
             OS_PRINTF("No empty buffer use.\n");

         MT_ASSERT(NULL != data_buf);
         data_buf->used = 1;
         filter->data_buf = data_buf;
     }
     ret = dmx_si_chan_set_buffer(tr_dmx_dev, filter->handle,
            filter->data_buf->filter_buf, TR_FILTER_BUFFER_SIZE);
     MT_ASSERT(SUCCESS == ret);

     filter->alloc_param.continuous = TRUE;
     filter->alloc_param.en_crc = TRUE;
	 //filter->alloc_param.req_mode = DMX_REQ_MODE_CONTINUOUS_SECTION;

     ret = dmx_si_chan_set_filter(tr_dmx_dev, filter->handle, &(filter->alloc_param));
     MT_ASSERT(SUCCESS == ret);
#ifdef CA_DEBUG
     OS_PRINTF("%s PID:%d handle:%d\n", __FUNCTION__, pid, filter->handle);
#endif
     return SUCCESS;
}
static s16 TRCAS_EnableChannel(u32 channelID)
{
     tr_channel_info_t *chan = NULL;
     tr_filter_info_t *filter = NULL;
     u16 filter_idx;
     s16 ret = ERR_FAILURE;

     chan = &tr_channel[channelID];
     for(filter_idx = 0; filter_idx < chan->actual_filter_num; filter_idx++)
     {
         filter = &chan->filter_info[filter_idx];
         TRCAS_SET_Filter(filter, chan->chan_pid);

         MT_ASSERT(NULL != filter);
         if(TR_INVALID_CHANNEL_ID != filter->handle)
         {
#ifdef CA_DEBUG
             OS_PRINTF("Enable channel [%d], filter [%d] Handle:%d\n", channelID, filter_idx, filter->handle);
#endif
             ret = dmx_chan_start(tr_dmx_dev, filter->handle);
             MT_ASSERT(SUCCESS == ret);
         }
     }

    return SUCCESS;
}

static void  TRCAS_DESC_SetDescramblerEvenKey(U32 ulDescId, IN U8 *pbEvenKey, U8 bEvenLen)
{
    RET_CODE pti_ret;
    u32 value;
/*
  In common condition, the CW forth byte equals the sum of 0-2 bytes.
  the eight byte equals the sum of 4-6 bytes.
  But in some special condition, it doesn't comply the rule, here blow
  will change 3rd and 7th CW to follow the common rule.
 */
    value = pbEvenKey[0] + pbEvenKey[1] + pbEvenKey[2];
    pbEvenKey[3] = value & 0xff;
    value = pbEvenKey[4] + pbEvenKey[5] + pbEvenKey[6];
    pbEvenKey[7] = value & 0xff;

#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d : dID:%d CID:%d\n", __FUNCTION__, __LINE__, ulDescId, tr_desc[ulDescId].ca_chan);
#endif
    if(TR_DESC_OFF == tr_desc[ulDescId].desc_on || TR_INVALID_CHANNEL_ID == tr_desc[ulDescId].ca_chan)
    {
         OS_PRINTF("Channel does not allocated for descrambler %d.\n", ulDescId);
         return;
    }

    pti_ret = dmx_descrambler_set_even_keys(tr_dmx_dev, tr_desc[ulDescId].ca_chan, pbEvenKey, bEvenLen);
    if(SUCCESS != pti_ret)
    {
        OS_PRINTF("Set descrambler [%d] even key failed.\n", ulDescId);
        return;
    }
    //cas_tr_send_desc_event(TR_DESC_EVENT_KEY_SET);

    return;
}

static void  TRCAS_DESC_SetDescramblerOddKey(U32 ulDescId, IN U8 *pbOddKey, U8 bOddLen)
{
    RET_CODE pti_ret;
    u32 value;

    /*
      In common condition, the CW forth byte equals the sum of 0-2 bytes.
      the eight byte equals the sum of 4-6 bytes.
      But in some special condition, it doesn't comply the rule, here blow
      will change 3rd and 7th CW to follow the common rule.
     */
    value = pbOddKey[0] + pbOddKey[1] + pbOddKey[2];
    pbOddKey[3] = value & 0xff;
    value = pbOddKey[4] + pbOddKey[5] + pbOddKey[6];
    pbOddKey[7] = value & 0xff;

#ifdef CA_DEBUG
   OS_PRINTF( "%s : %d : DID:%d CID:%d\n", __FUNCTION__, __LINE__, ulDescId, tr_desc[ulDescId].ca_chan);
#endif

    if(TR_DESC_OFF == tr_desc[ulDescId].desc_on || TR_INVALID_CHANNEL_ID == tr_desc[ulDescId].ca_chan)
    {
         OS_PRINTF("Channel does not allocated for descrambler %d.\n", ulDescId);
         return;
    }

    pti_ret = dmx_descrambler_set_odd_keys(tr_dmx_dev, tr_desc[ulDescId].ca_chan, pbOddKey, bOddLen);
    if(SUCCESS != pti_ret)
    {
        OS_PRINTF("Set descrambler [%d] odd key failed.\n", ulDescId);
        return;
    }
    cas_tr_send_desc_event(TR_DESC_EVENT_KEY_SET);

    return;
}

s16   TRCAS_DEMUX_FreeFilter(U32 ulChannelId, U32 ulFilterId)
{
    tr_channel_info_t *chan = NULL;
    tr_filter_info_t *filter = NULL;
    tr_buffer_t *data_buf = NULL;
    if(TR_MAX_CHANNEL_NUM <= ulChannelId || FILTER_NUM <= ulFilterId)
		  OS_PRINTF("%s %d CID:%d FID:%d\n", __FUNCTION__, __LINE__, ulChannelId, ulFilterId);
    MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId && FILTER_NUM > ulFilterId);

    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);

    if(0 >= chan->actual_filter_num)
    {
        OS_PRINTF("All filter in channel %d are freed. Filter %d does not exist.\n", ulChannelId, ulFilterId);
        return 0;
    }
    filter = &chan->filter_info[ulFilterId];
    if(NULL == filter)
    {
        OS_PRINTF("Filter [%d] of channel [%d] has been freed.\n", ulFilterId, ulChannelId);
        return 0;
    }
    if(TR_INVALID_CHANNEL_ID != filter->handle)
    {
        dmx_chan_close(tr_dmx_dev, filter->handle);
        filter->handle = TR_INVALID_CHANNEL_ID;
    }
    if(NULL != filter->data_buf)
    {
        //mtos_nc_free(filter->data_buf);
        data_buf = filter->data_buf;
        data_buf->used = 0;
        memset(data_buf, 0, sizeof(tr_buffer_t));
        filter->data_buf = NULL;
    }
    //mtos_free(filter);
    //TRDRV_OS_FreeMemory(filter);
    memset(filter, 0, sizeof(tr_filter_info_t));
    filter->used = 0;
    //chan->filter_info[ulFilterId]= NULL;
    chan->actual_filter_num--;
    return 0;
}

static s16 TRCAS_DisableChannel(u32 channelID)
{
     tr_channel_info_t *chan = NULL;
     tr_filter_info_t *filter = NULL;
     u16 filter_idx;
     s16 ret = ERR_FAILURE;

     chan = &tr_channel[channelID];
     for(filter_idx = 0; filter_idx < chan->actual_filter_num; filter_idx++)
     {
         filter = &chan->filter_info[filter_idx];
         if(NULL == filter)
             continue;
         if(TR_INVALID_CHANNEL_ID != filter->handle)
         {
#ifdef CA_DEBUG
             OS_PRINTF("Disable channel [%d], filter [%d] Handle:%d\n", channelID, filter_idx, filter->handle);
#endif
             ret = dmx_chan_stop(tr_dmx_dev, filter->handle);
             MT_ASSERT(SUCCESS == ret);
         }
     }
     OS_PRINTF( "%s : %d \n", __FUNCTION__, __LINE__);
     return SUCCESS;
}


/*!
 * This function will control a section channel. The state of the channel include enable, disable and reset.
 *
 * Param[in] ulChannelId - The identifier of the channel to be controlled.
 * Param[in] eAction - The input control type.  DISABLE, ENABLE, RESET
 *
 */
void  TRDRV_DEMUX_ControlChannel(U32 ulChannelId, CAS_DEMUX_CTRL eAction)
{
     tr_channel_info_t *chan = NULL;

#ifdef CA_DEBUG
     OS_PRINTF( "%s : %d CID:%d Act:%d\n", __FUNCTION__, __LINE__, ulChannelId, eAction);
#endif
    if(TR_MAX_CHANNEL_NUM <= ulChannelId)
         OS_PRINTF( "%s : %d CID:%d Act:%d\n", __FUNCTION__, __LINE__, ulChannelId, eAction);

     MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
     MG_CAS_LOCK();
     chan = &tr_channel[ulChannelId];
     MT_ASSERT(NULL != chan);

     switch(eAction)
     {
         case CAS_DEMUX_ENABLE_CHANNEL:
             if(TRCAS_CHAN_ENABLED == chan->state)
                 break;
             TRCAS_EnableChannel(ulChannelId);
             chan->state = TRCAS_CHAN_ENABLED;
             break;
         case CAS_DEMUX_DISABLE_CHANNEL:
             if(TRCAS_CHAN_DISABLED == chan->state)
                 break;
             chan->state = TRCAS_CHAN_DISABLED;
             TRCAS_DisableChannel(ulChannelId);
             break;
         case CAS_DEMUX_RESET_CHANNEL:
             if(TRCAS_CHAN_ENABLED == chan->state)
             {
                 chan->state = TRCAS_CHAN_DISABLED;
                 TRCAS_DisableChannel(ulChannelId);
             }
             TRCAS_EnableChannel(ulChannelId);
             chan->state = TRCAS_CHAN_ENABLED;
             break;
         default:
             OS_PRINTF("\"%d\" is invalid channel action.\n", eAction);
     }
     OS_PRINTF( "%s : %d \n", __FUNCTION__, __LINE__);
     MG_CAS_UNLOCK();
     return;
}
/*!
 * This fucntion will associate a given channel with one up-call function,
 * which will be called when the channel gets setion data. The given channel
 * identifier will be a parameter of the up-call function.
 *
 * Param[in] ulChannelId - The channel identifier of the allocated channel that
 *           must register the up-call function.
 * Param[in] The function to be called when the channel gets data.
 *
 * Return
 *    The channel number will be returned. It is one identifier of this channel.
 */
U32  TRDRV_DEMUX_RegisterChannelUpcallFct(U32 ulChannelId, IN void (*chUpcallFct)(U32 ulChannelId))
{
    tr_channel_info_t *chan = NULL;

    if(TR_MAX_CHANNEL_NUM <= ulChannelId)
        OS_PRINTF("%s %d channel %d.\n", __FUNCTION__, __LINE__, ulChannelId);

    MT_ASSERT(NULL != chUpcallFct && TR_MAX_CHANNEL_NUM > ulChannelId);
    MG_CAS_LOCK();
    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);
#ifdef CA_DEBUG
    OS_PRINTF("#### %s %d channel %d.\n", __FUNCTION__, __LINE__, ulChannelId);
#endif
    chan->upcall_func = chUpcallFct;
    MG_CAS_UNLOCK();
    return ulChannelId;
}

/*!
 * This function will free one section channel. It will be called after calling TRDRV_DEMUX_FreeFilter.
 *
 * Param[in] ulChannelId - The idetifier of the channel to be released.
 */
void  TRDRV_DEMUX_FreeSectionChannel(U32 ulChannelId)
{
    tr_channel_info_t *chan = NULL;
    u16 filter_idx;

    if(TR_MAX_CHANNEL_NUM <= ulChannelId)
	  OS_PRINTF("%s %d channel %d.\n", __FUNCTION__, __LINE__, ulChannelId);

    MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
    MG_CAS_LOCK();
    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);
#ifdef CA_DEBUG
    OS_PRINTF("#### %s %d CID:%d\n", __FUNCTION__, __LINE__, ulChannelId);
#endif
    for(filter_idx = 0; filter_idx < chan->actual_filter_num; filter_idx++)
    {
        TRCAS_DEMUX_FreeFilter(ulChannelId, filter_idx);
    }
    //OS_PRINTF( "%s : line: %d ch idx:%d, addr:0x%x addr:0x%x\n", __FUNCTION__, __LINE__, ulChannelId, tr_channel[ulChannelId], chan);
    //mtos_free(chan);
    //TRDRV_OS_FreeMemory(chan);
    //tr_channel[ulChannelId] = NULL;
    memset(&tr_channel[ulChannelId], 0, sizeof(tr_channel_info_t));
    tr_channel[ulChannelId].used = 0;
    MG_CAS_UNLOCK();
    return;
}
/*!
 * This function will allocate a section filter on one given channel.
 *
 * \param[in] ulChannelId - The channel identifier to be allocate filter.
 *
 * Return
 *    Return the identifier of the filter if it is successfully allocated.
 *    Multiple section filters can be allocated on one channel.
 */
U32   TRDRV_DEMUX_AllocateFilter(U32 ulChannelId)
{
    tr_channel_info_t *chan = NULL;
    tr_filter_info_t *filter = NULL;
    u16 filter_idx = 0;

    if(TR_MAX_CHANNEL_NUM <= ulChannelId)
		OS_PRINTF("#### %s %d CID:%d\n", __FUNCTION__, __LINE__, ulChannelId);


    MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
    MG_CAS_LOCK();
    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);

    while(chan->filter_info[filter_idx].used == 1)
    {
        filter_idx++;
    }
    MT_ASSERT(FILTER_NUM > filter_idx);

    filter = &chan->filter_info[filter_idx]; //mtos_malloc(sizeof(tr_filter_info_t));
    //filter = TRDRV_OS_AllocateMemory(sizeof(tr_filter_info_t));
    MT_ASSERT(NULL != filter);
    memset(filter, 0, sizeof(tr_filter_info_t));
    filter->data_buf = NULL;
    filter->handle = TR_INVALID_CHANNEL_ID;
    filter->used = 1;

    //chan->filter_info[filter_idx] = filter;
    chan->actual_filter_num++;
    //mtos_task_delay_ms(20);
#ifdef CA_DEBUG
    OS_PRINTF("%s %d CID:%d idx:%d addr:0x%x, addr:0x%x num:%d\n", __FUNCTION__, __LINE__, ulChannelId, filter_idx, &chan->filter_info[filter_idx], filter, chan->actual_filter_num);
#endif
    MG_CAS_UNLOCK();
    return filter_idx;
}
/*!
 * This function will be set filter mask and match bytes of one given filter. The number of
 * mask and match bytes depends on the wMaxFilterSize parameter TRDRV_DEMUX_AllocateSectionChannel.
 *
 * \param[in] ulChannelId - The identifier of the channel associated with this filter.
 * \param[in] ulFilterId - The identifier of the filter to set mask and match bytes.
 * \param[in] pbMask - The pointer of the mask bytes.
 * \param[in] pbData - The pointer of the match bytes.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 */

S16   TRDRV_DEMUX_SetFilter(U32 ulChannelId, U32 ulFilterId, IN U8 *pbMask, IN U8 *pbData)
{
    tr_channel_info_t *chan = NULL;
    tr_filter_info_t *filter = NULL;
    u16 idx = 0;

    if(TR_MAX_CHANNEL_NUM <= ulChannelId || FILTER_NUM <= ulFilterId)
    {
	    OS_PRINTF("#### %s %d CID:%d FID:%d\n", __FUNCTION__, __LINE__, ulChannelId, ulFilterId);
    }
    MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId && FILTER_NUM > ulFilterId);
    MT_ASSERT(NULL != pbMask && NULL != pbData);
    MG_CAS_LOCK();
    chan = &tr_channel[ulChannelId];
    MT_ASSERT(NULL != chan);


    //mtos_printk("\n\nTRDRV_DEMUX_SetFilter  0x%02x  0x%02x\n",pbMask[0],pbData[0]);
    if(0 == chan->actual_filter_num)
    {
        OS_PRINTF("Actual allocated filter number is 0.\n");
        MG_CAS_UNLOCK();
        return 1;
    }
    filter = &chan->filter_info[ulFilterId];
    MT_ASSERT(NULL != filter);

    filter->alloc_param.mask[0] = *pbMask;
    filter->alloc_param.value[0] = *pbData;

    for(idx = 3; idx < chan->max_filter_size; idx++)
    {
        filter->alloc_param.mask[idx - 2] = pbMask[idx];
        filter->alloc_param.value[idx - 2] = pbData[idx];
    }
    MG_CAS_UNLOCK();
    return 0;
}

/*!
 * This function will free a section filter on the given channel.
 *
 * \param[in] ulChannelId - The identifier of the channel associated with this filter.
 * \param[in] ulFilterId - The identifier of the filter to be released.
 */
s16   TRDRV_DEMUX_FreeFilter(U32 ulChannelId, U32 ulFilterId)
{
    s16 ret = 0;
#ifdef CA_DEBUG
    OS_PRINTF("%s %d CID:%d FID:%d\n", __FUNCTION__, __LINE__, ulChannelId, ulFilterId);
#endif
    MG_CAS_LOCK();
    ret = TRCAS_DEMUX_FreeFilter(ulChannelId, ulFilterId);
    MG_CAS_UNLOCK();
    return ret;
}

/*!
 * This function will return a pointer of the section data buffer of the given channel.
 *
 * \param[in] ulChannelId - The identifier of the channel to get section data.
 * \param[in] pulSectionSize - A pointer of the buffer length of section data.
 *
 * Return
 *    Pointer to the buffer of section data, and it is NULL if no section data.
 *
 */
U8   *TRDRV_DEMUX_GetSectionData(U32 ulChannelId, OUT U32 *pulSectionSize)
{
   tr_channel_info_t *chan = NULL;
   U8 *pBuf = NULL;
//   u16 len = 0;
   if(TR_MAX_CHANNEL_NUM <= ulChannelId)
	   OS_PRINTF("%s %d CID:%d \n", __FUNCTION__, __LINE__, ulChannelId);
   MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
   MG_CAS_LOCK();
   chan = &tr_channel[ulChannelId];
   MT_ASSERT(NULL != chan);
   pBuf = TRCAS_RCV_Data_Get(chan, pulSectionSize);
/*   
   if(pBuf[0] == 0x80 ||pBuf[0] == 0x81)
   {
     mtos_printk("ECM data:\n");
     for(len = 0; len < *pulSectionSize;len++)
     {
      mtos_printk(" %02x",pBuf[len]);
     }
     mtos_printk("\n\n");
   }
   else
   {
    mtos_printk("\nemm get 0x%02x\n",pBuf[0]);
   }
*/
   MG_CAS_UNLOCK();


   return pBuf;
}

/*!
 * This function will release the buffer pointer of the section channel, and allocate
 * new section data enter the buffer. This function will always be called after calling
 * TRDRV_DEMUX_GetSectionData.
 *
 * \param[in] ulChannelId - The identifier of the channel to release pointer.
 * \param[in] ulSectionSize - The section length to be released.
 *
 */
void  TRDRV_DEMUX_FreeSectionData(U32 ulChannelId, U32 ulSectionSize)
{
#if 0
   tr_channel_info_t *chan = NULL;

   MT_ASSERT(TR_MAX_CHANNEL_NUM > ulChannelId);
   chan = tr_channel[ulChannelId];
   if(NULL == chan)
   {
       OS_PRINTF("Channel [%d] list is empty.\n", ulChannelId);
       return;
   }
#ifdef CA_DEBUG
   OS_PRINTF("#### %s %d CID:%d Size:%d\n", __FUNCTION__, __LINE__, ulChannelId, ulSectionSize);
#endif
   TRCAS_RCV_Data_Del(chan, ulSectionSize);
#endif
    return;
}

/*---------------DESC--------------------------------------------------------*/
/*!
 * This function is used to open a descrambler.
 *
 * Return
 *    Return descrambler identifier or CAS_DEMUX_INVALID_DESCRAMBLER_ID.
 * dmx.h
 */
U32   TRDRV_DESC_OpenDescrambler(void)
{
    U32 index = -1;
    MG_CAS_LOCK();
    for(index = 0;index <TR_MAX_DESC_NUM; index++)
    {
        if(0 == tr_desc[index].used)
            break;
    }
    MT_ASSERT(TR_MAX_DESC_NUM > index);
    tr_desc[index].id = index;
    tr_desc[index].used = 1;
    tr_desc[index].ca_chan = TR_INVALID_CHANNEL_ID;
#ifdef CA_DEBUG
   OS_PRINTF( "%s : %d : idx:%d\n", __FUNCTION__, __LINE__, index);
#endif
    MG_CAS_UNLOCK();
    return index;
}
/*!
 * This function is used to close a descrambler opened before.
 *
 * \param[in] ulDescrId - The identifier of the descrambler to be closed.
 *
 */
void  TRDRV_DESC_CloseDescrambler(U32 ulDescId)
{
    RET_CODE ret;
    u8 cw_defaut[8];

#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d : DID:%d CID:%d\n", __FUNCTION__, __LINE__, ulDescId, tr_desc[ulDescId].ca_chan);
#endif
    MG_CAS_LOCK();
    if(TR_DESC_OFF == tr_desc[ulDescId].desc_on
      || tr_desc[ulDescId].used == 0
      || tr_desc[ulDescId].ca_chan == TR_INVALID_CHANNEL_ID)
    {
        memset(&(tr_desc[ulDescId]), 0, sizeof(desc_attr_t));
        tr_desc[ulDescId].ca_chan = TR_INVALID_CHANNEL_ID;
        MG_CAS_UNLOCK();
        return;
    }
    memset(cw_defaut, 0, sizeof(cw_defaut));
    TRCAS_DESC_SetDescramblerEvenKey(ulDescId, cw_defaut, sizeof(cw_defaut));
    TRCAS_DESC_SetDescramblerOddKey(ulDescId, cw_defaut, sizeof(cw_defaut));
    ret = dmx_descrambler_onoff(tr_dmx_dev, tr_desc[ulDescId].ca_chan, FALSE);
    MT_ASSERT(SUCCESS == ret);

    //cas_tr_send_desc_event(TR_DESC_EVNET_KEY_CLEAR);
    memset(&(tr_desc[ulDescId]), 0, sizeof(desc_attr_t));
    tr_desc[ulDescId].ca_chan = TR_INVALID_CHANNEL_ID;
    tr_desc[ulDescId].desc_on = TR_DESC_OFF;
    tr_desc[ulDescId].used = 0;
    MG_CAS_UNLOCK();
}
/*!
 * This function is used to associate one PID with one descrambler.
 *
 * \param[in] ulDescId - Descrambler identifier to set PID.
 * \param[in] wPid - The PID to descrambler.
 *
 */
void  TRDRV_DESC_SetDescramblerPid(U32 ulDescId, U16 wPid)
{
    RET_CODE pti_ret;
    dmx_chanid_t dmx_handle = TR_INVALID_CHANNEL_ID;
    MG_CAS_LOCK();
    MT_ASSERT(1 == tr_desc[ulDescId].used);

    if(TR_DESC_ON == tr_desc[ulDescId].desc_on && wPid == tr_desc[ulDescId].pid)
    {
        OS_PRINTF("The pid [%d] has been set on descrambler [%d].\n", wPid, ulDescId);
        MG_CAS_UNLOCK();
        return;
    }

    tr_desc[ulDescId].pid = wPid;
    pti_ret = dmx_get_chanid_bypid( tr_dmx_dev, tr_desc[ulDescId].pid, &dmx_handle);
    if(SUCCESS != pti_ret)
    {
         OS_PRINTF("Search channel ID failed for descrambler %d.\n", ulDescId);
         tr_desc[ulDescId].used = 0;
         MG_CAS_UNLOCK();
         return;
    }
    MT_ASSERT(SUCCESS == pti_ret);
    tr_desc[ulDescId].ca_chan = dmx_handle;
#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d : DID:%d PID:%d ChanID:%d\n", __FUNCTION__, __LINE__, ulDescId, wPid, dmx_handle);
#endif
    pti_ret = dmx_descrambler_onoff( tr_dmx_dev, dmx_handle, TRUE);
    if(SUCCESS != pti_ret)
    {
        OS_PRINTF("Descrambler [%d] on failed. ret:%d\n", ulDescId, pti_ret);
        MG_CAS_UNLOCK();
        return;
    }
    tr_desc[ulDescId].desc_on = TR_DESC_ON;
    MG_CAS_UNLOCK();
}
/*!
 * This function is used to set the even keys on a decrambler.
 *
 * \param[in] ulDescId - The descrambler identifier to set CW keys.
 * \param[in] pbEvenKey - Pointer of the even keys.
 * \param[in] bEvenLen - The length of the keys buffer, and it must be 8 bytes.
 *
 */
void  TRDRV_DESC_SetDescramblerEvenKey(U32 ulDescId, IN U8 *pbEvenKey, U8 bEvenLen)
{
#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d\n", __FUNCTION__, __LINE__);
#endif
    MG_CAS_LOCK();
    TRCAS_DESC_SetDescramblerEvenKey(ulDescId, pbEvenKey, bEvenLen);
    MG_CAS_UNLOCK();
}
/*!
 * This function is used to set the odd keys on a decrambler.
 *
 * \param[in] ulDescId - The descrambler identifier to set CW keys.
 * \param[in] pbEvenKey - Pointer of the odd keys.
 * \param[in] bEvenLen - The length of the keys buffer, and it must be 8 bytes.
 *
 */
void  TRDRV_DESC_SetDescramblerOddKey(U32 ulDescId, IN U8 *pbOddKey, U8 bOddLen)
{
#ifdef CA_DEBUG
    OS_PRINTF( "%s : %d\n", __FUNCTION__, __LINE__);
#endif
    MG_CAS_LOCK();
    TRCAS_DESC_SetDescramblerOddKey(ulDescId, pbOddKey, bOddLen);
    MG_CAS_UNLOCK();
}


/*---------------NVRAM-------------------------------------------------------*/
/*!
 * This function is used to initialize the NVRAM driver. It will be called once after in MCELL.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 *
 */

S16   TRDRV_NVRAM_Initialise(void)
{
    /* We need define which part of the NVRAM can be read/write by customer code.
      * So need to redirect customer specified offset to make sure it just can access
      * part of whole NVRAM space.
      */
    tr_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != tr_charsto_dev);
    return 0;
}

/*!
 * This function allows the MCELL read access to the NVRAM.
 *
 * Param[in] ulOffset - Offset from the start of NVRAM.
 * Param[in] pbData - Memory location to which read data should be copied.
 * Param[in] wLength - Number of bytes to read.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 *
 */
S16   TRDRV_NVRAM_Read(U32 ulOffset, OUT U8 *pbData, U16 wLength)
{
    /* We need define which part of the NVRAM can be read/write by customer code.
      * So need to redirect customer specified offset to make sure it just can access
      * part of whole NVRAM space.
      */
    s16 ret;
    u32 length = wLength;

    MT_ASSERT(NULL != tr_charsto_dev);
    MT_ASSERT(NULL != pbData);
    MT_ASSERT(0 != wLength);
    if(NULL != tr_priv->nv_read)
    {
        ret = tr_priv->nv_read(ulOffset, pbData, &length);
        if(SUCCESS != ret)
            return 1;
        return 0;
    }
    ulOffset += TR_BASE_NVRAM_ADDR;
    ret = charsto_read(tr_charsto_dev, ulOffset, pbData, wLength);
    if(ret != SUCCESS)
        OS_PRINTF("CAS read NVRAM data failed.\n");
#ifdef CA_DEBUG
    OS_PRINTF("Read NVRAM data:%s len:%d \n", pbData, wLength);
#endif
    return ret == SUCCESS?0:1;
}
/*!
 * This function allows the MCELL write access to the NVRAM.
 *
 * Param[in] ulOffset - Offset from the start of NVRAM.
 * Param[in] pbData - Memory location to which data should be copied.
 * Param[in] wLength - Number of bytes to write.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 *
 */
S16   TRDRV_NVRAM_Write(U32 ulOffset, IN U8 *pbData,  U16 wLength)
{
    s16 ret;
    u32 length = wLength;

    MT_ASSERT(pbData != NULL);
    MT_ASSERT(wLength != 0);
    MT_ASSERT(tr_charsto_dev != NULL);

    if(NULL != tr_priv->nv_write)
    {
        ret = tr_priv->nv_write(ulOffset, pbData, length);
        if(SUCCESS != ret)
            return 1;
        return 0;
    }
    ulOffset += TR_BASE_NVRAM_ADDR;
    /* Check the init flag and check whether the offset exceed the boundary */

    if((ulOffset + wLength) > TR_MAX_ALLOWED_NVRAM_ADDR)
    {
        OS_PRINTF("Write data length exceed the allowed space.");
        return  1;
    }

    ret = charsto_writeonly(tr_charsto_dev, ulOffset, pbData, wLength);
    if(ret != SUCCESS)
        OS_PRINTF("Write NVRAM data failed.\n");
    return ret == SUCCESS?0:1;
}

/*---------------PRINT-------------------------------------------------------*/
/*!
  Where should the messages output, serail console or TV screen.
  */
typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);
void  MC_Printf(char *p_fmt, ...)
{
    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, p_fmt);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)p_fmt, p_args);

    CK_VA_END(p_args);
    //mtos_printk("\nca core printf: ");
   // mtos_printk(printk_buf);
    return;

}

