/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
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
#include "mtos_timer.h"
#include "charsto.h"

#include "mem_manager.h"
#include "nim.h"
#include "trdef.h"
#include "dmx.h"
#include "ads_ware.h"

//#define ADS_TASK_PRIORITY_S 27 //refer sys_cfg.h 
//#define ADS_TASK_PRIORITY_E 50
//#define TR_AD_DATA_TASK_PRIORITY    52

#define TR_ADS_MAX_MSG_QUEUE 16
//#define TR_AD_BASE_NVRAM_ADDR   0xe0000
//#define TR_AD_MAX_ALLOWED_NVRAM_ADDR 0x18ffff

#define TR_AD_TASK_ID_BASE 100
#define TR_AD_FILTER_NUM 31
#define TR_AD_MAX_SEM_NUM 16
#define TR_AD_MAX_CHANNEL_NUM 8

#define TR_AD_INVALID_CHANNEL_ID 0xffff
#define TR_AD_BASE_QUEUE_ID 10
#define TR_AD_STACK_SIZE       64 * KBYTES
#define TR_AD_BASE 1
#define TR_AD_DMX_BUF_SIZE  32 * KBYTES + 188

#define AD_DEBUG

/*!
   ADS filter info
  */
typedef struct{
    dmx_filter_setting_t  alloc_param;
    dmx_chanid_t handle;
    u8 *p_data_buf;
    u8 *p_data_buf_for_free;
}tr_ad_filter_info_t;

/*!
   ADS channel state
  */
typedef enum tr_ad_channel_state
{
    TR_AD_CHAN_DISABLED = 0,
    TR_AD_CHAN_ENABLED,
}tr_ad_channel_state_t;


static u32 ads_svc_msgq[TR_ADS_MAX_MSG_QUEUE] = {
	                  INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID,
                         INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID,
                         INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID,
                         INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID, INVALID_MSGQ_ID};

/*!
   for topreal channel information store
  */
typedef struct{
    tr_ad_channel_state_t state;  /* 1: start  0: stop */
    tr_ad_filter_info_t *filter_info[TR_AD_FILTER_NUM];
    dmx_slot_setting_t slot_t;
    u16 max_filter_num;
    u16 max_filter_size;
    u16 actual_filter_num;
    u32 buffer_size;
    u16 chan_pid;
    void (*upcall_func)(u32 nChannelId,u8 *p_data,u32 nlength);
}tr_ad_channel_info_t;

/*!
   ADS timer list
  */
typedef struct
{
    BOOL used;
    u32 tr_handle;
    s32 os_handle;
} tr_timer_list_t;

extern void ads_send_event(u32 slot, u32 event, u32 param);
extern ads_module_priv_t *ads_priv;

static tr_timer_list_t g_tr_timer_list[MTOS_MAX_SYSTIMER];
static u8 g_priority = 0;//ADS_TASK_PRIORITY_S;

tr_ad_channel_info_t *p_tr_ad_channel[TR_AD_MAX_CHANNEL_NUM];

u32 tr_ad_sem_list[TR_AD_MAX_SEM_NUM] = {0, };


static int detect_task = 0;
dmx_device_t *tr_ad_dmx_dev = NULL;

os_sem_t  g_sem_tr_adslock = {0};
#define TR_ADS_LOCK()      mtos_sem_take(&g_sem_tr_adslock, 0)
#define TR_ADS_UNLOCK()    mtos_sem_give(&g_sem_tr_adslock)

#ifdef AD_DEBUG
#if 0
static void data_dump(u8 *p_addr, u32 size)
{
  u32 i = 0;

  for(i = 0; i < size; i++)
  {
    OS_PRINTF("%02x ", p_addr[i]);
  }
  OS_PRINTF("\n");
}
#endif
#endif

static u8 *dmx_memory_malloc(u32 buf_size)
{
   u32 buf = 0;
#ifdef  WARRIORS
   buf = (u32)mtos_malloc(buf_size + 7);    //memory align
   hal_dcache_flush_all();
   buf |= 0xa0000000;
#elif ORCHID
   buf = (u32)mtos_malloc(buf_size + 7);    //memory align
#else
   buf = (u32)mtos_align_malloc(buf_size, 16); 
#endif

   return (u8 *)buf;
}

static void dmx_memory_free(u8 *addr)
{
#ifdef WARRIORS
    u8 *buf = NULL;
    buf = (u8 *)(((u32)addr) & 0xdfffffff);
    mtos_free(buf);
#elif ORCHID
   mtos_free(addr);
#else
    mtos_align_free(addr);
#endif
}

static u8 MTR_Parse_Qam(u16 value) 
{
   u8 n_qam = NIM_MODULA_QAM32;
   switch(value)
   {
        case 16:
           n_qam = NIM_MODULA_QAM16;
           break;
        case 32:
           n_qam = NIM_MODULA_QAM32;
           break;
        case 64:
           n_qam = NIM_MODULA_QAM64;
           break;
        case 128:
           n_qam = NIM_MODULA_QAM128;
           break;
        case 256:
           n_qam = NIM_MODULA_QAM256;
           break;
        default:
           break;
   }
   return n_qam;
}

/***************************************************************************************/
/*--------- The blow functions are accomplished by user, and called by MTR ----------*/
/***************************************************************************************/
/*API*/
/*-----------------OS--------------------------------------------------------*/
/*!
 * This function is used to allocate memory from the heap. 
 *
 * \param[in] size - The memory size.
 *
 * Return
 *    Allocate memory pointer - Successful
 *    NULL - Failed
 */
MTR_OS_VOID *MTR_DRV_OS_MemoryAllocate(MTR_DRV_OS_U32 size)
#if 1
{
   void *p_memory = NULL;
#ifdef AD_DEBUG
   OS_PRINTF("\n%s %d size:%d\n",__FUNCTION__, __LINE__, size);
#endif
   p_memory = mtos_malloc(size);
   if(NULL == p_memory)
   {
       OS_PRINTF("\n%s %d alloc memory error. size:%d\n",__FUNCTION__, __LINE__, size);
   }
   OS_PRINTF("\n%s %d addr:%d\n",__FUNCTION__, __LINE__, (u32)p_memory);
   return p_memory;
}
#else
{
	 void *p_addr = NULL;
	 mem_mgr_alloc_param_t param = {0};
	 param.id = MEM_ADS_PARTITION;
	 param.size = size;
	 param.user_id = 18;
	 OS_PRINTF("\n%s %d size:%d\n",__FUNCTION__, __LINE__, size);
	 p_addr = mem_mgr_alloc(&param);
	 OS_PRINTF("\n%s %d addr:%d\n",__FUNCTION__, __LINE__, (u32)p_addr);
	 MT_ASSERT(p_addr != NULL);
	// OS_PRINTF("alloc mem addr 0x%x\n",p_addr);
	 memset((u8 *)p_addr,0,param.size);
	 return p_addr;

}
#endif
/*!
 * This function is called to release the previously allocated memory. A release
 * of NULL pointer must be safe.
 * 
 * \param[in] paddress - The previously allocated memory pointer.
 *
 * Return
 *    0 - Successful
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_MemoryFree(MTR_OS_VOID *paddress)
#if 1
{
   s32 ret = 0;
#ifdef AD_DEBUG
   OS_PRINTF("\n%s %d paddress:%d\n",__FUNCTION__, __LINE__, paddress);
#endif
   if(NULL == paddress)
   {
       return ret;
   }
   mtos_free(paddress);
   return ret;
}
#else
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_ADS_PARTITION;
  param.p_addr = (void *)paddress;
 // OS_PRINTF("free mem addr 0x%x\n",p_addr);
  param.user_id = 18;
  OS_PRINTF("\n%s %d paddress:%d\n",__FUNCTION__, __LINE__, paddress);
  mem_mgr_free(&param);

  return 0;
}

#endif
/*!
 * This function is called to set the allocated memory.
 * 
 * \param[in] mem - The allocated memory pointer.
 * \param[in] value - The set value.
 * \param[in] bytes - The memory size.
 *
 * Return
 *    The memory pointer
 */
void *MTR_DRV_OS_MemorySet(void *mem, MTR_OS_U8 value, MTR_DRV_OS_U32 bytes)
{
   if(NULL == mem)
   {
       return NULL;
   }
   memset(mem, value, bytes);
   return mem;
}

/*!
 * This function is called to copy memory.
 * 
 * \param[in] pdst - The copy memory destination pointer.
 * \param[in] psrc - The copy memory source pointer.
 * \param[in] bytes - The memory size.
 *
 * Return
 *    The destination memory pointer
 */
void *MTR_DRV_OS_MemoryCopy(void *pdst,
                             void *psrc, MTR_DRV_OS_U32 bytes)
{
   if(NULL == pdst || NULL == psrc)
   {
       return NULL;
   }
   memcpy(pdst, psrc, bytes);
   return pdst;
}

/*!
 * This function will create a message. 
 *
 * \param[in] queue_element - The max msg num
 * 
 * Return
 *    Message id - Successful
 *    0 - Failed
 */
MTR_DRV_OS_QUEUEID MTR_DRV_OS_QueueCreate(MTR_DRV_OS_U32 queue_element)
{
   u32 ret = 0;
   u8 i = 0;
   char name_buf[20] = "";
   void *p_sec_buf = NULL;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d queue_element=%x\n",__FUNCTION__, __LINE__,queue_element);
#endif
   for(i = 0; i < TR_ADS_MAX_MSG_QUEUE; i++)
   {
       if(ads_svc_msgq[i] == INVALID_MSGQ_ID)
         break;
   }
   MT_ASSERT(i < TR_ADS_MAX_MSG_QUEUE);

   sprintf(name_buf, "ad_svc_%d", i);
   ads_svc_msgq[i] = mtos_messageq_create(queue_element, (u8 *)name_buf);

   MT_ASSERT(ads_svc_msgq[i] != INVALID_MSGQ_ID);
   p_sec_buf = mtos_malloc(sizeof(MTR_OS_Msg) * queue_element);
   MT_ASSERT(p_sec_buf != NULL);
   memset(p_sec_buf, 0 ,sizeof(MTR_OS_Msg) * queue_element);

   ret = mtos_messageq_attach(ads_svc_msgq[i],
                       p_sec_buf,
                       sizeof(MTR_OS_Msg),
                       queue_element);
   
   if(ret == FALSE)
   {
       ads_svc_msgq[i] = INVALID_MSGQ_ID;
       mtos_free(p_sec_buf);
       OS_PRINTF("ADS attach message queue failed.\n");
       return 0;
   }
   if(ads_svc_msgq[i] >= 0)
     ret = ads_svc_msgq[i] + TR_AD_BASE_QUEUE_ID;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d ret=%x\n",__FUNCTION__, __LINE__,ret);
#endif   
   return ret;
}

/*!
 * This function will delete a message. (Now reserved)
 *
 * \param[in] queueid - The queue id
 * 
 * Return
 *    0 - Successful
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid)
{
   u32 ret = 0;
   mtos_messageq_release(queueid);
   return ret;
}

/*!
 * This function is used to send message to a given task.
 *
 * \param[in] queueid - msg identifier of the destination msg queue.
 * \param[in] pmessage - Message data pointer.
 *
 * Return
 *    0 - Successful
 *    -1 - Failed
 * 
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueSendMessage(MTR_DRV_OS_QUEUEID queueid,MTR_OS_Msg *pmessage)
{
   s32 ret = 0;
   os_msg_t message = {0};
 #ifdef AD_DEBUG
   OS_PRINTF("\n%s %d queueid=%x, pmessage->type=%x,pmessage->length=%d\n",
                                    __FUNCTION__, __LINE__,queueid,pmessage->Type,pmessage->Length);
   //data_dump(pmessage->DataBuffer,10);
 #endif
   MT_ASSERT(NULL != pmessage);
   message.is_ext = 0;
   message.para1 = (u32)pmessage;
   message.para2 = sizeof(MTR_OS_Msg);

   MT_ASSERT(queueid -TR_AD_BASE_QUEUE_ID != INVALID_MSGQ_ID);
   MT_ASSERT(pmessage != NULL);

   ret = mtos_messageq_send(queueid - TR_AD_BASE_QUEUE_ID, &message);

   if(ret != TRUE)
       OS_PRINTF("ADS send message failed.\n");

   return (ret == TRUE?0:-1);
}

/*!
 * This function will receive a message from one given task. The relevant task
 * should be blocked while waiting for message.
 *
 * \param[in] queueid - msg identifier of the destination msg queue..
 * \param[in] wait_ornot - wait if failed
 * \param[in] wait_time - dwTimeoutMessage
 *
 * Return
 *    memory address - Successful
 *    NULL - Failed
 * 
 */
void *MTR_DRV_OS_QueueReceiveMessage(u32 queueid,
                                                   MTR_DRV_OS_WAITMODE wait_ornot,u32 wait_time)
{
    os_msg_t p_msg = {0};
    u32 ret = 0;

#ifdef AD_DEBUG
    MTR_OS_Msg *p_m_msg = NULL;
    OS_PRINTF("%s %d queueid=%x,wait_ornot=%d,wait_time=%x\n",__FUNCTION__,
                    __LINE__,queueid,wait_ornot,wait_time);
#endif
    MT_ASSERT((queueid - TR_AD_BASE_QUEUE_ID) != INVALID_MSGQ_ID);

    ret = mtos_messageq_receive(queueid - TR_AD_BASE_QUEUE_ID, &p_msg, wait_time);
    if(ret != TRUE)
    {
        OS_PRINTF("ADS receive message failed.\n");
        return NULL;
    }
#ifdef AD_DEBUG
   p_m_msg = (MTR_OS_Msg *)p_msg.para1;
   OS_PRINTF("\n%s %d p_msg->type=%x,p_msg->length=%d\n",__FUNCTION__,
   	           __LINE__,p_m_msg->Type,p_m_msg->Length);
   //data_dump(p_m_msg->DataBuffer,10);
#endif
   
    return  (PMTR_DRV_OS_VOID)(p_msg.para1);
}

static s32 MTR_Search_Free_Semaphore(void)
{
    int i = 0;

    for(i = 0; i < TR_AD_MAX_SEM_NUM; i++)
    {
        if(tr_ad_sem_list[i] == 0)
            return i;
    }
    return -1;
}

static void MTR_Set_Semaphore(s32 sem_id, u32 semaphore)
{
    tr_ad_sem_list[sem_id] = semaphore;
}

static s32 MTR_Check_Semaphore(u32 semaphore)
{
    int i = 0;

    for(i = 0; i < TR_AD_MAX_SEM_NUM; i++)
    {
        if(tr_ad_sem_list[i] == semaphore)
            return 0;
    }
    return -1;
}

/*!
 * This function allows a semphore to be created and initialized.
 *
 * \param[in] semphore_init_no - initial value of the created semphore.
 *           0 indicates not available; >0 indicates available.
 *
 * Return
 *    semaphore - Success
 *    0 - Failed
 * 
 */
MTR_DRV_OS_SEMPHORE MTR_DRV_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no)
{
    s32 sem_id = 0;
    u32 semaphore = 0;

    MT_ASSERT(semphore_init_no >= 0);
    sem_id = MTR_Search_Free_Semaphore();
    MT_ASSERT(0 <= sem_id);

    if(mtos_sem_create((os_sem_t *)&semaphore, semphore_init_no) == TRUE) 
    {
        MTR_Set_Semaphore(sem_id, semaphore);
    }
    else
    {
        OS_PRINTF("ADS create semphore failed. \n");
        return 0;
    }
#ifdef AD_DEBUG
    OS_PRINTF("%s %d semaphore:%x\n",__FUNCTION__, __LINE__,semaphore);
#endif
    return semaphore;
}

/*! 
 * This function is used to destroy a specific sem.(Now reserved)
 *
 * \param[in] semphoreid - The specific sem handle
 * 
 * Return
 *    0 - Successful
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid)
{
   s32 ret = 0;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d semphoreid=%x\n",__FUNCTION__, __LINE__,semphoreid);
#endif
   MT_ASSERT((-1) != MTR_Check_Semaphore(semphoreid));
   if(mtos_sem_destroy((os_sem_t *)&semphoreid, MTOS_DEL_ALWAYS) != TRUE)
      ret = -1;
   return ret;
}

/*!
 * This function will wait for the semaphore to be signaled. If the semaphore is available,
 * decrement the counter by one.
 *
 * \param[in] Semaphore - The specific sem handle
 * \param[in] dwTimeout - The timeout
 *
 * Return
 *    semaphore - Success
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,
                                            MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time)
{
   s32 ret = 0;
   MT_ASSERT((-1) != MTR_Check_Semaphore(semphoreid));
   if(mtos_sem_take((os_sem_t *)&semphoreid, wait_time) != TRUE)
   {
#ifdef AD_DEBUG
      OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
      ret = MTR_DRV_OS_ERR;
   }
   else
   {
      ret = semphoreid;
   }
   return ret;
}

/*! 
 * This function is used to release a specific sem.
 *
 * \param[in] semphoreid - The specific sem handle
 * 
 * Return
 *    0 - Successful
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreRelease(MTR_DRV_OS_SEMPHORE semphoreid)
{
   s32 ret = MTR_DRV_OS_OK;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d semphoreid=%x\n",__FUNCTION__, __LINE__,semphoreid);
#endif
   MT_ASSERT((-1) != MTR_Check_Semaphore(semphoreid));
   if(mtos_sem_give((os_sem_t *)&semphoreid) != TRUE)
      ret = MTR_DRV_OS_ERR;
   return ret;
}

static u32 MTR_Parse_Timer_Mode(u32 timer_type)
{
  switch(timer_type)
  {
    case MTR_DRV_TIMER_ONE_SHOT:
      return 0;

    case MTR_DRV_TIMER_REPEAT:
      return 1;
      
    default:
      break;
  }
  return 0;
}

/*!
 * This function allows a timer to be created and initialized.
 * We real creat timer in SetTimer func
 *
 * \param[out] handle - The timer handle
 * 
 * Return
 *    0 - Successful
 */
MTRDRV_S32	MTR_DRV_TIMER_CreateTimer(MTRDRV_HANDLE *handle)
{
   s32 ret = MTR_DRV_TIMER_CREATE_ERROR;
   int i = 0;
   u32 *p_handle = NULL;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d handle=%x\n",__FUNCTION__, __LINE__,*(u32 *)handle);
#endif
   p_handle = (u32 *)handle;

   for(i = 0; i < MTOS_MAX_SYSTIMER; i++)
   {
      if(g_tr_timer_list[i].used == FALSE)
      {
         g_tr_timer_list[i].used = TRUE;
         g_tr_timer_list[i].os_handle = -1;
         g_tr_timer_list[i].tr_handle = i + TR_AD_TASK_ID_BASE;
         *p_handle = g_tr_timer_list[i].tr_handle;
#ifdef AD_DEBUG
         OS_PRINTF("%s %d handle=%x\n",__FUNCTION__, __LINE__,*(u32 *)handle);
#endif
         ret = MTR_DRV_TIMER_OK;
         break;
      }
   }
   return ret;
}

/*!
 * This function set timeout.
 *
 * \param[in] handle - The timer handle
 * \param[in] TimeValue - The timeout.
 * \param[in] TimerMode - The timer mode.
 * \param[in] CallBackFunc - The timercallback function entry
 * 
 * Return
 *    0 - Successful
 */
s32	MTR_DRV_TIMER_SetTimer(MTRDRV_HANDLE *handle,
                                MTRDRV_U32 TimeValue, MTRDRV_U32 TimerMode, void (*CallBackFunc)())
{
   s32 ret = MTR_DRV_TIMER_ERROR; 
   s32 tmp_timer_id = -1;
   int i = 0;
   u32 cycle_mode = 0;
   cycle_mode = MTR_Parse_Timer_Mode(TimerMode);
   for(i = 0; i <MTOS_MAX_SYSTIMER; i++)
   {
      if(g_tr_timer_list[i].tr_handle == *handle)
      {
        if(g_tr_timer_list[i].os_handle < 0)
        {
          tmp_timer_id = mtos_timer_create(TimeValue * 1000, (void (*)(u32))CallBackFunc, 0, cycle_mode);
          if(tmp_timer_id >= 0)
          {
            g_tr_timer_list[i].os_handle = tmp_timer_id;
            ret = MTR_DRV_TIMER_OK;
            break;
          }
        }
        else
        {
          mtos_timer_reset(g_tr_timer_list[i].os_handle,TimeValue * 1000);
          ret = MTR_DRV_TIMER_OK;
        }
      }
   }

   return ret;
}

/*!
 * This function to start timer.
 *
 * \param[in] handle - The timer handle
 * 
 * Return
 *    0 - Successful
 */
s32	MTR_DRV_TIMER_StartTimer(MTRDRV_HANDLE *handle)
{
   s32 ret = MTR_DRV_TIMER_ERROR; 
   int i = 0;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d handle=%x\n",__FUNCTION__, __LINE__,*(u32 *)handle);
#endif
   for(i = 0; i < MTOS_MAX_SYSTIMER; i++)
   {
      if((*handle == g_tr_timer_list[i].tr_handle) && (g_tr_timer_list[i].os_handle != -1))
      {
        mtos_timer_start(g_tr_timer_list[i].os_handle);
        ret = MTR_DRV_TIMER_OK;
        break;
      }
   }
   return ret;
}

/*!
 * This function to stop timer.
 *
 * \param[in] handle - The timer handle
 * 
 * Return
 *    0 - Successful
 */
s32	MTR_DRV_TIMER_StopTimer(MTRDRV_HANDLE *handle)
{
   s32 ret = MTR_DRV_TIMER_ERROR;
   int i = 0;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d handle=%x\n",__FUNCTION__, __LINE__,*(u32 *)handle);
#endif
   for(i = 0; i < MTOS_MAX_SYSTIMER; i++)
   {
      if((*handle == g_tr_timer_list[i].tr_handle) && (g_tr_timer_list[i].os_handle != -1))
      {
        if(mtos_timer_stop(g_tr_timer_list[i].os_handle) >= 0)
          ret = MTR_DRV_TIMER_OK;
        break;
      }
   }
   return ret;
}

/*!
 * This function to destroy timer.
 *
 * \param[in] handle - The timer handle
 * 
 * Return
 *    0 - Successful
 *    1 - Failed
 */
s32	MTR_DRV_TIMER_FreeTimer(MTRDRV_HANDLE *handle)
{
   s32 ret = MTR_DRV_TIMER_FREE_ERROR;
   int i = 0;
   u32 temp_handle = *handle;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d handle=%x\n",__FUNCTION__, __LINE__,*(u32 *)handle);
#endif

   for(i = 0; i < MTOS_MAX_SYSTIMER; i++)
   {
      if((temp_handle == g_tr_timer_list[i].tr_handle) && (g_tr_timer_list[i].os_handle != -1))
      {
        mtos_timer_delete(g_tr_timer_list[i].os_handle);
        g_tr_timer_list[i].used = FALSE;
        g_tr_timer_list[i].os_handle = -1;
        g_tr_timer_list[i].tr_handle = 0;
        ret = MTR_DRV_TIMER_OK;
        break;
      }
   }
   return ret;
}

/*!
 * This function will create a task used by MTR. All created tasks must be able
 * to receive or send message mutually, and all messages use same memory to pass
 * pointer to data.
 *
 * \param[in] func - The task function entry
 * \param[in] arg - The task function parameter
 * \param[in] stack_size - Size of stack required of task.
 * \param[in] priority - Priority of the task to be created.
 * \param[in] stack - The stack start address.
 * 
 * Return
 *    Task handle
 */
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskCreate(MTR_OD_ThreadEntryFunc func,
                      PMTR_DRV_OS_VOID arg, MTR_DRV_OS_PRIORITY priority,
                      PMTR_DRV_OS_VOID stack, MTR_DRV_OS_U32 stack_size)
{
   u8 ret = 0;
   char name_buf[20] = "";
   u32 handle = 0;
   g_priority = ads_priv->task_prio_start;
   stack = (u32 *)mtos_malloc(2 * stack_size);
#ifdef AD_DEBUG
   OS_PRINTF("%s %d priority=%x,stack_size=%x\n",__FUNCTION__, __LINE__,priority,stack_size);
#endif

   MT_ASSERT(stack != NULL);

   sprintf(name_buf, "ad_%d", g_priority);
   if(priority >= ads_priv->task_prio_start && priority <= ads_priv->task_prio_end && priority != g_priority)
   {
     ret = mtos_task_create((u8 *)name_buf, func, arg, priority, stack, stack_size);
     handle = priority;
     g_priority = priority;
   }
   else if(g_priority >= ads_priv->task_prio_start && g_priority <= ads_priv->task_prio_end)
   {

     ret = mtos_task_create((u8 *)name_buf, func, arg, g_priority, stack, stack_size);
     handle = g_priority;
     g_priority++;
   }
   if(ret == FALSE)
   {
       mtos_free(stack);
       OS_PRINTF("ADS create task failed.\n");
       return 0;
   }
   return handle;
}


/*! 
 * This function is used to destroy a specific task.(Now reserved)
 *
 * \param[in] taskid - The specific task handle
 * 
 * Return
 *    0 - Successful
 *    -1 - Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskDelete(MTR_DRV_OS_THREADID taskid)
{
   s32 ret = 0;
   //mtos_task_exit();
   return ret;
}

/*! 
 * This function is used to get current task priority
 *
 * Return
 *    Task priority
 */
MTR_DRV_OS_PRIORITY MTR_DRV_OS_TaskGetCurTaskPriority(MTRDRV_VOID)
{
  return g_priority;
}

/*! 
 * This function is used to set  task priority
 *
 * \param[in] taskid - The specific task handle
 * \param[in] priority - The new priority of task
 *
 * Return
 *    0 -Set Success
 *    -1 -Set Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid,
                                                        MTR_DRV_OS_PRIORITY priority)
{
  if((priority != g_priority) && priority >= ads_priv->task_prio_start && priority <= ads_priv->task_prio_end)
  {
    mtos_task_change_prio(g_priority,priority);
    g_priority = priority;
  }
  return MTR_DRV_OS_OK;
}

/*! 
 * This function is used to get current  task id.(Now reserved)
 *
 *
 * Return
 *    Task Handle id
 */
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskGetCurTaskId(MTRDRV_VOID)
{
  return 0;
}

/*! 
 * This function is used to set task suspend.(Now reserved)
 *
 * \param[in] taskid - The specific task handle
 *
 * Return
 *    0 -Set Success
 *    -1 -Set Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid)
{
    mtos_task_suspend(taskid);
    return MTR_DRV_OS_OK;
}

/*! 
 * This function is used to set task resume.(Now reserved)
 *
 * \param[in] taskid - The specific task handle
 *
 * Return
 *    0 -Set Success
 *    -1 -Set Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskResume(MTR_DRV_OS_THREADID taskid)
{
    mtos_task_resume(taskid);
    return MTR_DRV_OS_OK;
}

/*! 
 * This function is used to os sleep.
 *
 * \param[in] dMilliSeconds - os sleep time
 *  
 * Return
 *    0 -Set Success
 *    -1 -Set Failed
 */
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds)
{
  mtos_task_delay_ms(dMilliSeconds);
  return MTR_DRV_OS_OK;
}

#if 0
MTRDRV_SINT MTR_DRV_API_printf(MTRDRV_CHAR *format, ...)
{
    MTRDRV_SINT ret = MTR_DRV_OS_OK;
    char *p_args = NULL;
	u8 dd = 0x5a;
    OS_PRINTF("tr print: %d", dd);
    OS_PRINTF(format,...);
    OS_PRINTF("\n");

    return ret;
}
#else

typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;

/*
 * Storage alignment properties
 */
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define  CK_ADNBND         (sizeof (CK_NATIVE_INT) - 1)

extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);

#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_ARG(ap, T)      (*(T *)(((ap) += (CK_BND (T, CK_AUPBND))) - (CK_BND (T,CK_ADNBND))))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern void OS_PutChar(u8 c);
MTRDRV_SINT MTR_DRV_API_printf(MTRDRV_CHAR *format, ...)
{
#ifdef PRINT_ON  
	ck_va_list p_args = NULL;
	unsigned int  printed_len = 0;
	char *p_p = NULL;
//	  static char	 printk_buf[256];
	char	printk_buf[200];

	p_p = printk_buf;

	/* Emit the output into the temporary buffer */
	CK_VA_START(p_args, format);
	printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);
	CK_VA_END(p_args);
#ifndef WIN32
	while(printed_len != 0)
	{
	  OS_PutChar(*p_p++);
	  printed_len--;
	}
#endif
#endif
  return 0;
}


#endif

/*-----------------DEMUX------------------------------------------------------*/
u8 g_data_temp[4096] = {0};
extern int dmx_jazz_wait_for_data(int timeout);
extern int dmx_jazz_wait_for_data_stop(void);

void MTR_ChannelData_Detect(void *param)
{
    tr_ad_channel_info_t *p_chan = NULL;
    tr_ad_filter_info_t *p_filter = NULL;
    s16 ret = 0;
    u8 *p_data = NULL;
    u32 data_len = 0;
    u16 chan_idx = 0;
    u16 filter_idx = 0;

    MT_ASSERT(NULL != tr_ad_dmx_dev);    
    while(1)
    {
        
        dmx_jazz_wait_for_data(50);
        TR_ADS_LOCK();
        if(0 == detect_task)
        {
            OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
            dmx_jazz_wait_for_data_stop();
            mtos_task_exit();
            TR_ADS_UNLOCK();
            return;
        }
        
        for(chan_idx = 0; chan_idx < TR_AD_MAX_CHANNEL_NUM; chan_idx++)
        {
            p_chan = p_tr_ad_channel[chan_idx];
            if(NULL == p_chan || TR_AD_CHAN_DISABLED == p_chan->state)
                continue;
            for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
            {
                if(1 == detect_task)
                {
                  p_data = NULL;
                  p_filter = p_chan->filter_info[filter_idx];
                  MT_ASSERT(NULL != p_filter);
                  ret = dmx_si_chan_get_data(tr_ad_dmx_dev, p_filter->handle, &p_data, &data_len);
                }

                if(SUCCESS == ret)
                {
                    if(NULL != p_chan->upcall_func && data_len > 0 && p_data != NULL)
                    {
                        memcpy(g_data_temp, p_data, data_len);
#ifdef AD_DEBUG
                        //data_dump(p_temp_data, data_len);
#endif
                        TR_ADS_UNLOCK();
                        if(1 == detect_task)
                          p_chan->upcall_func(chan_idx,g_data_temp,data_len);
                        TR_ADS_LOCK();
                        OS_PRINTF("%s %d data_len=%d\n",__FUNCTION__, __LINE__,data_len);
                    }
                }
            }
        }
        TR_ADS_UNLOCK();
    }
} 

/*!
 *  This function will allocate a section channel to filter data from mpeg TS.
 * 
 *  \param[in] nFilterNumber - Maximum number of the section filters on this channel.
 *  \param[in] nChannelBufferSize - The buffer size of the channel.
 *
 *  Return
 *     The channel number will be returned. It is on identifier of this channel.
 *     The value is from 1 to ADS_DEMUX_INVALID_CHANNEL_ID
 */
MTRDRV_U32 MTR_DRV_DMX_AllocateSectionChannel(MTRDRV_U32 nFilterNumber,
                                                     MTRDRV_U32 nChannelBufferSize)
{
    u32 chan_idx = 0;
    tr_ad_channel_info_t *p_chan = NULL;
    u32 stack_size = TR_AD_STACK_SIZE;
    u8 dmx_ret = 0;
    u32 *p_stack = NULL;

    if(0 == detect_task)
    {
       detect_task = 1;
       if(tr_ad_dmx_dev == NULL)
          tr_ad_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

       MT_ASSERT(NULL != tr_ad_dmx_dev);
       MT_ASSERT(NULL != tr_ad_dmx_dev->p_base);

       p_stack = (u32 *)mtos_malloc(stack_size);
	     MT_ASSERT(p_stack != NULL);
       MT_ASSERT(TRUE == mtos_sem_create(&g_sem_tr_adslock, TRUE));
       dmx_ret = mtos_task_create((u8 *)"MTR_ChannelData_Detect", 
                  MTR_ChannelData_Detect, NULL, g_priority, p_stack, stack_size);
       MT_ASSERT(TRUE == dmx_ret);
      g_priority++;
    }
    
    TR_ADS_LOCK();
    for(chan_idx = 0; chan_idx < TR_AD_MAX_CHANNEL_NUM; chan_idx++)
    {
        if(NULL == p_tr_ad_channel[chan_idx])
        {
            OS_PRINTF("Allocated channel ID is %d.\n Max filter number: %d, Buffer size: %d\n", 
                chan_idx, nFilterNumber, nChannelBufferSize);
            break;
         }
    }
    if(TR_AD_MAX_CHANNEL_NUM <= chan_idx)
    {
        OS_PRINTF("All channel are busy, allocate section channel failed.\n");
        TR_ADS_UNLOCK();
        return 0;
    }
    
    p_chan = mtos_malloc(sizeof(tr_ad_channel_info_t));
    MT_ASSERT(NULL != p_chan);
    memset(p_chan, 0, sizeof(tr_ad_channel_info_t));

    p_chan->buffer_size = nChannelBufferSize;
    p_chan->max_filter_num = nFilterNumber;
    p_chan->max_filter_size = 0; //Set in MTR_DRV_DMX_AllocateFilter
    p_chan->actual_filter_num = 0;
    p_chan->state = TR_AD_CHAN_DISABLED;
    p_chan->upcall_func = NULL;
    p_chan->chan_pid = 0;
    p_tr_ad_channel[chan_idx] = p_chan;
    TR_ADS_UNLOCK();
    
#ifdef AD_DEBUG    
    OS_PRINTF("%s %d chan_idx:%x\n", __FUNCTION__, __LINE__, chan_idx);
#endif

    return chan_idx;
}

/*!
 * This function will free one section channel. It will be called after calling MTR_DRV_DMX_FreeFilter.
 *
 * Param[in] nChannelId - The idetifier of the channel to be released.
 */
MTRDRV_U32 MTR_DRV_DMX_FreeChannel(MTRDRV_U32 nChannelId)
{
    u32 ret = 1;
    tr_ad_channel_info_t *p_chan = NULL;
    u16 filter_idx = 0;
#ifdef AD_DEBUG
    OS_PRINTF("%s %d nChannelId=%x\n",__FUNCTION__, __LINE__,nChannelId);
#endif
    MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > nChannelId);
    p_chan = p_tr_ad_channel[nChannelId];
    if(NULL != p_chan)
    {

        for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
        {
            ret = MTR_DRV_DMX_FreeFilter(nChannelId, filter_idx);
        }
        mtos_free(p_chan);
        p_tr_ad_channel[nChannelId] = NULL;
     }
    return 0;
}

/*!
 * This function will allocate a section filter on one given channel.
 * 
 * \param[in] nChannelId - The channel identifier to be allocate filter.
 * \param[in] nFilterSize - The size of filter.
 *
 * Return
 *    Return the identifier of the filter if it is successfully allocated.
 *    Multiple section filters can be allocated on one channel.
 */
MTRDRV_U32 MTR_DRV_DMX_AllocateFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterSize)
{
    tr_ad_channel_info_t *p_chan = NULL;
    tr_ad_filter_info_t *p_filter = NULL;
    u16 filter_idx = 0;
#ifdef AD_DEBUG
    OS_PRINTF("%s %d nChannelId=%x,nFilterSize=%x\n",__FUNCTION__, __LINE__,nChannelId,nFilterSize);
#endif
    TR_ADS_LOCK();
    MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > nChannelId);
    MT_ASSERT(NULL != p_tr_ad_channel[nChannelId]);
    p_tr_ad_channel[nChannelId]->max_filter_size = nFilterSize;
    p_chan = p_tr_ad_channel[nChannelId];  

    while(NULL != p_chan->filter_info[filter_idx])
    {
        filter_idx++;
    }
    MT_ASSERT(TR_AD_FILTER_NUM > filter_idx);
    
    p_filter = mtos_malloc(sizeof(tr_ad_filter_info_t));
    MT_ASSERT(NULL != p_filter);
    memset(p_filter, 0, sizeof(tr_ad_filter_info_t));
    p_filter->p_data_buf = NULL;
    p_filter->handle = TR_AD_INVALID_CHANNEL_ID;

    p_chan->filter_info[filter_idx] = p_filter;
    p_chan->actual_filter_num++;
    TR_ADS_UNLOCK();
#ifdef AD_DEBUG
    OS_PRINTF("%s %d filter_idx=%x\n",__FUNCTION__, __LINE__,filter_idx);
#endif
    return filter_idx;
}

/*!
 * This function will free a section filter on the given channel.
 * 
 * \param[in] nChannelId - The identifier of the channel associated with this filter.
 * \param[in] nFilterId - The identifier of the filter to be released.
 */
MTRDRV_U32 MTR_DRV_DMX_FreeFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId)
{
    tr_ad_channel_info_t *p_chan = NULL;
    tr_ad_filter_info_t *p_filter = NULL;

#ifdef AD_DEBUG
    OS_PRINTF("%s %d nChannelId=%x,nFilterId=%x\n",__FUNCTION__, __LINE__,nChannelId,nFilterId);
#endif

    MT_ASSERT(NULL != tr_ad_dmx_dev);
    MT_ASSERT(NULL != tr_ad_dmx_dev->p_base);
    TR_ADS_LOCK();
    MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > (nChannelId) && TR_AD_FILTER_NUM > (nFilterId));
    p_chan = p_tr_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);
    
    if(0 >= p_chan->actual_filter_num)
    {
        OS_PRINTF("Filter %d does not exist.\n",nFilterId);
        TR_ADS_UNLOCK();
        return 0;
    }
    p_filter = p_chan->filter_info[nFilterId];
    if(NULL == p_filter)
    {
        OS_PRINTF("Filter [%d] of channel [%d] has been freed.\n", nFilterId, nChannelId);
        TR_ADS_UNLOCK();
        return 0;
    }
    if(TR_AD_INVALID_CHANNEL_ID != p_filter->handle)
    {
        dmx_chan_close(tr_ad_dmx_dev, p_filter->handle);
        p_filter->handle = TR_AD_INVALID_CHANNEL_ID;
    }
    if(NULL != p_filter->p_data_buf)
    {
        dmx_memory_free(p_filter->p_data_buf_for_free);
        p_filter->p_data_buf = NULL;
    }

    mtos_free(p_filter);
    p_chan->filter_info[nFilterId]= NULL;
    p_chan->actual_filter_num--;
    TR_ADS_UNLOCK();
    return 0;
}

/*!
 * This function will set PID to a section channel which has been allocated.
 * The channel should be disabled before calling the MTR_DRV_DMX_ControlChannel.
 *
 * Param[in] nChannelId - The identifier of the channel to allocate PID.
 * Param[in] nPid - The PID value.
 * 
 */
MTRDRV_U32 MTR_DRV_DMX_SetChannelPid(MTRDRV_U32 nChannelId, MTRDRV_U32 nPid)
{
    tr_ad_channel_info_t *p_chan = NULL;
#ifdef AD_DEBUG
    OS_PRINTF("%s %d nChannelId=%x,nPid=%x\n",__FUNCTION__, __LINE__,nChannelId,nPid);
#endif
    MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > nChannelId);
    TR_ADS_LOCK();
    p_chan = p_tr_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);

    p_chan->chan_pid = nPid;
    TR_ADS_UNLOCK();
    return 0;
}

static RET_CODE MTR_Set_Filter(tr_ad_filter_info_t *p_filter, u16 pid)
{
    RET_CODE ret = ERR_FAILURE;
    dmx_chanid_t si_channel_id = TR_AD_INVALID_CHANNEL_ID;
    dmx_slot_setting_t slot;
    u8 *p_data = NULL;
    u8 *p_buf_align = NULL;

    MT_ASSERT(NULL != tr_ad_dmx_dev);
    MT_ASSERT(NULL != tr_ad_dmx_dev->p_base);  
#ifdef AD_DEBUG
    //OS_PRINTF("Mask: \n");
    //data_dump(p_filter->alloc_param.mask, DMX_SECTION_FILTER_SIZE);
    //OS_PRINTF("Data: \n");
    //data_dump(p_filter->alloc_param.value, DMX_SECTION_FILTER_SIZE);
#endif
     memset(&slot, 0, sizeof(dmx_slot_setting_t));     
     slot.pid   = pid;
     slot.type = DMX_CH_TYPE_SECTION;

     if(TR_AD_INVALID_CHANNEL_ID == p_filter->handle)
     {
         ret = dmx_si_chan_open(tr_ad_dmx_dev, &slot, &si_channel_id);
         MT_ASSERT(SUCCESS == ret);
         p_filter->handle = si_channel_id;
     }
     
     if(NULL == p_filter->p_data_buf)
     {
         p_data = (u8 *)dmx_memory_malloc(TR_AD_DMX_BUF_SIZE);
         MT_ASSERT(NULL != p_data);
         p_buf_align = (u8 *)(((u32)p_data + 7) & 0xfffffff8);
         p_filter->p_data_buf = p_buf_align;
         p_filter->p_data_buf_for_free = p_data;
         memset(p_filter->p_data_buf,0,TR_AD_DMX_BUF_SIZE);
     }
     ret = dmx_si_chan_set_buffer(tr_ad_dmx_dev, p_filter->handle, 
            p_filter->p_data_buf, TR_AD_DMX_BUF_SIZE);
     MT_ASSERT(SUCCESS == ret);

     p_filter->alloc_param.continuous = TRUE;
     p_filter->alloc_param.en_crc = TRUE;
     /*for ads it must be CONTINUOUS_SECTION*/
     p_filter->alloc_param.req_mode = DMX_REQ_MODE_SECTION_DOUBLE;

     p_filter->alloc_param.ts_packet_mode = DMX_ONE_MODE;
     ret = dmx_si_chan_set_filter(tr_ad_dmx_dev, p_filter->handle, &(p_filter->alloc_param));
     MT_ASSERT(SUCCESS == ret);
#ifdef AD_DEBUG
     OS_PRINTF("%s PID:%x handle:%x\n", __FUNCTION__, pid, p_filter->handle);
#endif
     return SUCCESS;
}

static RET_CODE MTR_Enable_Channel(u32 channelID)
{
     tr_ad_channel_info_t *p_chan = NULL;
     tr_ad_filter_info_t *p_filter = NULL;
     u16 filter_idx = 0;
     RET_CODE ret = ERR_FAILURE;

     MT_ASSERT(NULL != tr_ad_dmx_dev);
     MT_ASSERT(NULL != tr_ad_dmx_dev->p_base);
     p_chan = p_tr_ad_channel[channelID];
     MT_ASSERT(NULL != p_chan);

     for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
     {
         p_filter = p_chan->filter_info[filter_idx];
         MTR_Set_Filter(p_filter, p_chan->chan_pid);
         
         MT_ASSERT(NULL != p_filter);
         if(TR_AD_INVALID_CHANNEL_ID != p_filter->handle)
         {
             ret = dmx_chan_start(tr_ad_dmx_dev, p_filter->handle);
             MT_ASSERT(SUCCESS == ret);
         }
     }
    return SUCCESS;
}

static RET_CODE MTR_Disable_Channel(u32 channelID)
{
     tr_ad_channel_info_t *p_chan = NULL;
     tr_ad_filter_info_t *p_filter = NULL;
     u16 filter_idx = 0;
     RET_CODE ret = ERR_FAILURE;

     MT_ASSERT(NULL != tr_ad_dmx_dev);
     MT_ASSERT(NULL != tr_ad_dmx_dev->p_base);
     p_chan = p_tr_ad_channel[channelID];
     MT_ASSERT(NULL != p_chan);

     for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
     {
         p_filter = p_chan->filter_info[filter_idx];
         if(NULL == p_filter) 
             continue;
         if(TR_AD_INVALID_CHANNEL_ID != p_filter->handle)
         {
             ret = dmx_chan_stop(tr_ad_dmx_dev, p_filter->handle);
             MT_ASSERT(SUCCESS == ret);
             /*!
               Add these because topreal change pid, but only call disable channel
               We should close channel here
               */	 
             if(0 >= p_chan->actual_filter_num)
             {
                OS_PRINTF("%s %d All filter in channel %d are freed.\n",__FUNCTION__, __LINE__);
                return 0;
             }
             dmx_chan_close(tr_ad_dmx_dev, p_filter->handle);
             p_filter->handle = TR_AD_INVALID_CHANNEL_ID;
             if(NULL != p_filter->p_data_buf)
             {
                memset(p_filter->p_data_buf,0,TR_AD_DMX_BUF_SIZE);
             }
         }
     }

     return SUCCESS;
}

/*!
 * This function will control a section channel. The state of the channel include enable, disable and reset.
 * 
 * Param[in] nChannelId - The identifier of the channel to be controlled.
 * Param[in] channelAction - The input control type.  DISABLE, ENABLE, RESET
 * 
 * Return 0 Successed
 *            1 Failed
 */
MTRDRV_U32 MTR_DRV_DMX_ControlChannel(MTRDRV_U32 nChannelId,
                                             MTR_DRV_DMX_ChannelAction_t channelAction)
{
    RET_CODE tmp_ret = 1;
    u32 ret = 0;
    tr_ad_channel_info_t *p_chan = NULL;
    TR_ADS_LOCK();
    //MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > nChannelId);
    if(TR_AD_MAX_CHANNEL_NUM > nChannelId)
        p_chan = p_tr_ad_channel[nChannelId];
    if(NULL != p_chan)
    {
      switch(channelAction)
      {
        case MTR_DRV_DMX_START_CHANNEL:       /*start*/
          if(TR_AD_CHAN_ENABLED == p_chan->state)
             break;
          tmp_ret = MTR_Enable_Channel(nChannelId);
          MT_ASSERT(-1 != tmp_ret);
          if(tmp_ret < 0)
             ret = 1;
          p_chan->state = TR_AD_CHAN_ENABLED;
          break;
        case MTR_DRV_DMX_STOP_CHANNEL:      /*stop*/
          if(TR_AD_CHAN_DISABLED == p_chan->state)
             break;
          p_chan->state = TR_AD_CHAN_DISABLED;
          tmp_ret = MTR_Disable_Channel(nChannelId);
          MT_ASSERT(-1 != tmp_ret);
          if(tmp_ret < 0)
             ret = 1;
          break;
        case MTR_DRV_DMX_RESET_CHANNEL:         /*reset*/
          if(TR_AD_CHAN_ENABLED == p_chan->state)
          {
             p_chan->state = TR_AD_CHAN_DISABLED;
             tmp_ret = MTR_Disable_Channel(nChannelId);
          }
          tmp_ret = MTR_Enable_Channel(nChannelId);
          MT_ASSERT(-1 != tmp_ret);
          if(tmp_ret < 0)
             ret = 1;
          p_chan->state = TR_AD_CHAN_ENABLED;
          break;
       default:
          OS_PRINTF("%s %d Invalid channelAction=%d\n",__FUNCTION__, __LINE__,channelAction);
          break;
      }
    }
    TR_ADS_UNLOCK();
    return ret;
}

/*!
 * This function will be set filter mask and match bytes of one given filter. The number of 
 * mask and match bytes depends on the wMaxFilterSize parameter TRDRV_DEMUX_AllocateSectionChannel.
 *
 * \param[in] nChannelId - The identifier of the channel associated with this filter.
 * \param[in] nFilterId - The identifier of the filter to set mask and match bytes.
 * \param[in] Table_id - The table id.
 * \param[in] Table_idMask - The mask bytes of table id.
 * \param[in] puiMatch - The pointer of the match value bytes.
 * \param[in] puiMask - The pointer of the match mask bytes.
 * \param[in] DataLen - The data length.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 */
MTRDRV_U32 MTR_DRV_DMX_SetFilter(MTRDRV_U32 nChannelId,
                          MTRDRV_U32 nFilterId,MTRDRV_U8 Table_id,MTRDRV_U8 Table_idMask,
                          MTRDRV_U8 *puiMatch ,MTRDRV_U8 *puiMask, MTRDRV_U8 DataLen)
{
    tr_ad_channel_info_t *p_chan = NULL;
    tr_ad_filter_info_t *p_filter = NULL;
    u16 idx = 0;

    MT_ASSERT(TR_AD_MAX_CHANNEL_NUM > (nChannelId) && TR_AD_FILTER_NUM > (nFilterId));
    MT_ASSERT(NULL != puiMatch && NULL != puiMask);
    TR_ADS_LOCK();
    p_chan = p_tr_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);

    if(0 == p_chan->actual_filter_num)
    {
        OS_PRINTF("Actual allocated filter number is 0.\n");
        TR_ADS_UNLOCK();
        return 1;
    }
    p_filter = p_chan->filter_info[nFilterId];
    MT_ASSERT(NULL != p_filter);

    p_filter->alloc_param.mask[0] = Table_idMask;
    p_filter->alloc_param.value[0] = Table_id;

    //for(idx = 0; idx < p_chan->max_filter_size; idx++)
    //{
        //OS_PRINTF("idx=%d,puiMask=%x,puiMatch=%x\n",idx,puiMask[idx],puiMatch[idx]);
    //}

    /* puiMatch[0] --> table[3] means >> 3 */
    for(idx = 0; idx < p_chan->max_filter_size; idx++)
    {
       p_filter->alloc_param.mask[idx + 1] = puiMask[idx];
       p_filter->alloc_param.value[idx + 1] = puiMatch[idx];
    }
    TR_ADS_UNLOCK();
    return 0;
}


/*!
 * This fucntion will associate a given channel with one callback function,
 * which will be called when the channel gets setion data. The given channel
 * identifier will be a parameter of the up-call function.
 *
 * Param[in] nChannelId - The channel identifier of the allocated channel that
 *           must register the up-call function.
 * Param[in] pfnCallBack - The function to be called when the channel gets data.
 * 
 */
MTRDRV_U32 MTR_DRV_DMX_RegisterChannelCallback(MTRDRV_U32 nChannelId,
           MTRDRV_VOID (*pfnCallBack)(MTRDRV_U32 nChannelId,MTRDRV_U8 *data,MTRDRV_U32 len))
{
    tr_ad_channel_info_t *p_chan = NULL;

#ifdef AD_DEBUG
    OS_PRINTF("%s %d nChannelId=%x\n",__FUNCTION__, __LINE__,nChannelId);
#endif
    TR_ADS_LOCK();
    if(NULL != pfnCallBack && TR_AD_MAX_CHANNEL_NUM > nChannelId)
    {
       p_chan = p_tr_ad_channel[nChannelId];
       MT_ASSERT(NULL != p_chan);
       p_chan->upcall_func = pfnCallBack;
    }
    TR_ADS_UNLOCK();
    return 0;  
}

/*-----------------AP--------------------------------------------------------*/

/*!
 * This function will do nothing, you should creat task in ap_init
 */
MTRDRV_VOID MTR_AD_Creat_Drv_Task(MTRDRV_VOID)
{
  return;
}

/*!
 * This function to get channel_info.(AP porting)
 *
 * \param[out] _chanFrequency - The Frequency of channel
 * \param[out] _chanSymbolRate - The SymbolRate of channel
 * \param[out] _Qammode - The Qam mode 
 *
 */
MTRDRV_VOID MTR_DRV_TUNER_GetMainFreqParams(MTRDRV_U32 *_chanFrequency,
                  MTRDRV_U32 *_chanSymbolRate,MTRDRV_U8 *_Qammode)
{
  *((u32 *)_chanFrequency) = ads_priv->channel_frequency;
  *((u32 *)_chanSymbolRate) = ads_priv->channel_symbolrate;
  *((u16 *)_Qammode) = ads_priv->channel_qam;
#ifdef AD_DEBUG
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
}

MTRDRV_VOID MTR_AD_SearchOver_Process(MTRDRV_VOID)
{
  
#ifdef AD_DEBUG
   OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
  TR_ADS_LOCK();
  detect_task = 0;
#ifdef AD_DEBUG
   OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
  ads_send_event(ads_priv->id, ADS_S_ADSERACH_FINISHED, 1);
#ifdef AD_DEBUG
   OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
  TR_ADS_UNLOCK();
}

/*-----------------STB--------------------------------------------------------*/

MTRDRV_U8 MTR_DRV_TUNER_GetLK(void)
{
  nim_device_t *p_nim_dev = NULL;
  u8 is_locked = 0;
  p_nim_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_nim_dev);
  dev_io_ctrl(p_nim_dev, NIM_IOCTRL_CHECK_LOCK, (u32)&is_locked);
#ifdef AD_DEBUG
  OS_PRINTF("%s %d is_locked:%d\n",__FUNCTION__, __LINE__,is_locked);
#endif
  return is_locked;
}

/*!
 * This function to channel_connect.
 *
 * \param[in] _chanFrequency - The Frequency of channel
 * \param[in] _chanSymbolRate - The SymbolRate of channel
 * \param[in] _Qammode - The Qam mode 
 *
 * Return
 *    0 - Unlocked
 *    1 - Locked
 */
MTRDRV_S8 MTR_DRV_TUNER_SetParams (MTRDRV_U32 _chanFrequency,
                  MTRDRV_U32 _chanSymbolRate,MTRDRV_U8 _Qammode)
{
  RET_CODE ret = 0;
  nim_channel_info_t channel_info = {0};
  nim_device_t *p_nim_dev = NULL;

  p_nim_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_nim_dev);

  channel_info.frequency = ads_priv->channel_frequency;
  channel_info.param.dvbc.symbol_rate = ads_priv->channel_symbolrate;
  channel_info.param.dvbc.modulation = MTR_Parse_Qam(ads_priv->channel_qam);
  
  /* For dvb-s */
  /*channel_info.param.dvbs.symbol_rate = ads_priv->channel_symbolrate;//_chanSymbolRate;
  dev_io_ctrl(nim_dev, NIM_IOCTRL_SET_LNB_ONOFF, 1);
  dev_io_ctrl(nim_dev, NIM_IOCTRL_SET_PORLAR, 0);
  dev_io_ctrl(nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 0);*/
  
  ret = nim_channel_connect(p_nim_dev, &channel_info, FALSE); 

  MT_ASSERT(SUCCESS == ret);
  OS_PRINTF("%s %d is_locked:%d\n",__FUNCTION__, __LINE__,channel_info.lock);
  
  return channel_info.lock;
}

/*-----------------OSD--------------------------------------------------------*/

/*!
 * This function read specific screen data to buffer.
 *
 * \param[in] x - The screen x coordinate.
 * \param[in] y - The screen y coordinate.
 * \param[in] width - The screen width.
 * \param[in] height - The screen height.
 * \param[in] destination - The pointer to buffer.
 *
 */
MTRDRV_VOID TRADReadRectangle(MTRDRV_U32 x,MTRDRV_U32 y,
             MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *destination)
{
  s16 ret = 0;
#ifdef AD_DEBUG
  OS_PRINTF("%s %d x=%x,y=%x,width=%x,height=%x\n",__FUNCTION__, __LINE__,x,y,width,height);
#endif
  if(NULL != ads_priv->read_rec)
  {
     ret = ads_priv->read_rec(x, y, width, height, destination);
     if(SUCCESS != ret)
       OS_PRINTF("%s %d Read rect failed addr:%x\n",__FUNCTION__, __LINE__,destination);
  }
}

/*!
 * This function write data from to specific buffer to screen.
 *
 * \param[in] x - The screen x coordinate.
 * \param[in] y - The screen y coordinate.
 * \param[in] width - The screen width.
 * \param[in] height - The screen height.
 * \param[in] source - The pointer to buffer.
 *
 */
MTRDRV_VOID TRADWriteRectangle(MTRDRV_U32 x,MTRDRV_U32 y,
                   MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *source)
{
  s32 ret = 0;
  //ads_rec_info_t rec_info = {0};
#ifdef AD_DEBUG
  OS_PRINTF("%s %d x=%x,y=%x,width=%x,height=%x\n",__FUNCTION__, __LINE__,x,y,width,height);
  //data_dump(source,width*height);
#endif
  /*
  rec_info.p_rec_addr = source;
  rec_info.rec_x = x;
  rec_info.rec_y = y;
  rec_info.rec_width = width;
  rec_info.rec_height = height;*/
  if(NULL != ads_priv->write_rec)
  {
     ret = ads_priv->write_rec(x, y, width, height, source);
     if(SUCCESS != ret)
       OS_PRINTF("%s %d write rect failed addr:%x\n",__FUNCTION__, __LINE__,source);
  }
}

/*!
 * This function to display I frame.
 *
 * \param[in] source - The I frame data
 * \param[in] length - The I frame data size
 *
 */
MTRDRV_VOID TRAD_Display_Logo(MTRDRV_U8 *source,MTRDRV_U32 length)
{
  s32 ret = 0;
  //ads_pic_info_t logo_info = {0};
#ifdef AD_DEBUG
  OS_PRINTF("%s %d length=%x\n",__FUNCTION__, __LINE__,length);
  //data_dump(source,length);
#endif
  //logo_info.p_pic_src_addr = source;
  //logo_info.pic_length = length;
  if(NULL != ads_priv->read_gif)
  {
        ret = ads_priv->read_gif((u32)length, source);
        if(SUCCESS != ret)
          OS_PRINTF("%s %d Send gif data failed.\n",__FUNCTION__, __LINE__);
  }
}

/*!
 * This function to display gif picture.
 *
 * \param[in] GifCoordinate - The coordinate of pic
 * \param[in] GifData - The I frame data
 * \param[in] gif_length - The pic data size
 * \param[in] gif_idx - Default 0
 * \param[in] refresh - Default 0
 *
 *Return MTRDRV_TRUE Successed
 *          MTRDRV_FALSE failed
 */
MTRDRV_BOOL MTR_AD_Show_Gif(MTR_DRV_OSD_DOTCOORDINATE GifCoordinate,
             MTRDRV_U8 *GifData , MTRDRV_HANDLE gif_length, MTRDRV_U8 gif_idx,MTRDRV_U8 refresh)
{
  s32 ret = 0;
  //ads_pic_info_t pic_info = {0};
#ifdef AD_DEBUG
  OS_PRINTF("%s %d x=%x,y=%x;gif_length=%x;gif_idx=%d;refresh=%d\n",
                         __FUNCTION__, __LINE__,GifCoordinate.X_x,
                         GifCoordinate.Y_y,gif_length,gif_idx,refresh);
  //data_dump(GifData,gif_length);
#endif
  /*
  pic_info.pic_x = GifCoordinate.X_x;
  pic_info.pic_y = GifCoordinate.Y_y;
  pic_info.p_pic_src_addr = GifData;
  pic_info.pic_index = gif_idx;
  pic_info.pic_length = gif_length;
  pic_info.pic_refresh = refresh;*/
  if(NULL != ads_priv->read_gif)
  {
        ret = ads_priv->read_gif(gif_length, GifData);
        if(SUCCESS != ret)
        {
          OS_PRINTF("%s %d Send gif data failed.\n",__FUNCTION__, __LINE__);
          return MTRDRV_FALSE;
        }
  }
  return MTRDRV_TRUE;
}


/*-----------------FLASH--------------------------------------------------------*/
/*!
 * This function send the NVRAM start address and size to MTR .
 *
 * Return
 *    NVRAM size - Successful
 * 
 */
MTRDRV_U32 MTR_DRV_NVRAM_GetFlashAllocateSize(MTRDRV_VOID)
{
#if 0
   return TR_AD_MAX_ALLOWED_NVRAM_ADDR - TR_AD_BASE_NVRAM_ADDR + 1; 
#endif
#ifdef AD_DEBUG
    OS_PRINTF("%s %d flash_size=%x\n",__FUNCTION__, __LINE__,ads_priv->flash_size);
#endif
   return ads_priv->flash_size;
}

/*!
 * This function allows the MTR read access to the NVRAM.
 *
 * Param[in] nvrOffset - The offset of NVRAM address. 
 * Param[in] extBuffer - Memory location to which read data should be copied.
 * Param[in] size - Number of bytes to read.
 *
 * 
 */
MTRDRV_VOID MTR_DRV_NVRAM_ReadData(MTRDRV_U32 nvrOffset, MTRDRV_U8 *extBuffer, MTRDRV_U32 size)
{
    s16 ret = ERR_FAILURE;
    u32 size_back = size;
    u32 nvroffset = nvrOffset;
#ifdef AD_DEBUG
    OS_PRINTF("\n%s %d nvrOffset=%x,size=%x\n",__FUNCTION__, __LINE__,nvrOffset,size);
#endif
#if 0
    charsto_device_t *p_charsto_dev = NULL;
    p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(p_charsto_dev != NULL);
#endif
    MT_ASSERT(extBuffer != NULL);
    if(NULL != ads_priv->nv_read && size_back != 0)
    {
        ret = ads_priv->nv_read(nvroffset, extBuffer, (u32 *)&size_back);
        if(SUCCESS != ret)
          OS_PRINTF("Read NVRAM data failed.\n");
#ifdef AD_DEBUG
	   //data_dump(extBuffer,size);
#endif
        return;
    }
#if 0
    nvroffset += TR_AD_BASE_NVRAM_ADDR;
    if((nvroffset + size_back) > TR_AD_MAX_ALLOWED_NVRAM_ADDR)
    {
        OS_PRINTF("%d data length exceed the allowed space.", size_back);
        return;
    }

    if(size_back != 0)
       ret = charsto_read(p_charsto_dev, nvroffset, extBuffer, size_back);

    if(ret != SUCCESS)
       OS_PRINTF("ADS read NVRAM data failed.\n");
#endif
}

/*!
 * This function allows the MTR write access to the NVRAM.
 * 
 * Param[in] nvrOffset - The offset of NVRAM address. 
 * Param[in] extBuffer - Memory location to which write data should be copied.
 * Param[in] size - Number of bytes to write.
 * 
 */
MTRDRV_VOID MTR_DRV_NVRAM_WriteData(MTRDRV_U32 nvrOffset, MTRDRV_U8 * extBuffer, MTRDRV_U32 size)
{
    s16 ret = ERR_FAILURE;
    u32 size_back = size;
    u32 nvroffset = nvrOffset;
#ifdef AD_DEBUG
    OS_PRINTF("%s %d nvrOffset=%x,extBuffer=%x,size=%x\n",
           __FUNCTION__, __LINE__,nvrOffset,extBuffer,size);
#endif
#if 0
    charsto_device_t *p_charsto_dev = NULL;
    p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(p_charsto_dev != NULL);
#endif
    MT_ASSERT(extBuffer != NULL);
#ifdef AD_DEBUG
//	 data_dump(extBuffer,size);
#endif
    if(NULL != ads_priv->nv_write && size_back != 0)
    {
        ret = ads_priv->nv_write(nvroffset, extBuffer, size_back);
        if(SUCCESS != ret)
          OS_PRINTF("Write NVRAM data failed.\n");
        return;
    }
#if 0
    nvroffset += TR_AD_BASE_NVRAM_ADDR;
    if((nvroffset + size_back) > TR_AD_MAX_ALLOWED_NVRAM_ADDR)
    {
        OS_PRINTF("Write data length exceed the allowed space.");
        return;
    }
    if(size_back != 0)
        ret = charsto_writeonly(p_charsto_dev, nvroffset, extBuffer, size_back);

    if(ret != SUCCESS)
        OS_PRINTF("Write NVRAM data failed.\n");
#endif
}

/*!
 * This function allows the MTR erase the NVRAM.
 * 
 * Param[in] length - Number of bytes to erase.
 *
 */
MTRDRV_VOID MTR_DRV_NVRAM_Erase(MTRDRV_U32 AD_length)
{
    s16 ret = ERR_FAILURE;
    u32 elength = AD_length;
#if 0
    charsto_device_t *p_charsto_dev = NULL;
    p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(p_charsto_dev != NULL);
#endif
#ifdef AD_DEBUG
    OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
    MT_ASSERT(elength != 0);
    if(NULL != ads_priv->nv_erase)
    {
        ret = ads_priv->nv_erase(elength);
        if(SUCCESS != ret)
          OS_PRINTF("Erase NVRAM data failed.\n");
        return;
    }
#if 0
    ret =  charsto_erase(p_charsto_dev, TR_AD_BASE_NVRAM_ADDR,
             (elength > CHARSTO_SECTOR_SIZE)?(elength / CHARSTO_SECTOR_SIZE -1)
             :(elength / CHARSTO_SECTOR_SIZE));
    if(ret != SUCCESS)
        OS_PRINTF("Erase NVRAM data failed.\n");
#endif
}

