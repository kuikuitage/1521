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
#include "mtos_event.h"
#include "charsto.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "drv_svc.h"
#include "cas_adapter.h"
#include "dmx.h"

#include "desai_ca_api_v5.0.h"
#include "ds_cas_include_v5.0.h"


//#define DESAI_DRV_DEBUG_EN

#ifdef DESAI_DRV_DEBUG_EN
#define DESAI_DRV_DEBUG OS_PRINTF
#else
#define DESAI_DRV_DEBUG(...) do{}while(0)
#endif


#define DS_CAS_TASK_NUM (8)
#define DS_TASK_PRIORITY_0 (35)
#define DS_FILTER_MAX_NUM (10)
#define DS_FILTER_DEPTH_SIZE (8)
#define DS_FILTER_BUFFER_SIZE (16 * (KBYTES) + 188)
#define DS_DATA_BUFFER_SIZE (1024)
#define DS_CAS_SECTION_TASK_PRIO        49



typedef enum _ds_task_status
{
    DS_TASK_STATUS_FREE,
    DS_TASK_STATUS_BUSY
}ds_task_status_e;

typedef struct _s_ds_task_struct
{
    u32 task_prio;
    ds_task_status_e task_status;
}s_ds_task_struct_t;

static s_ds_task_struct_t ds_task_record[DS_CAS_TASK_NUM] =
{
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
    {0, DS_TASK_STATUS_FREE},
};


typedef struct _s_ds_filter_struct
{
    e_ds_filter_status_t filter_status;
    u8 req_id;
    u16 pid;
    u8 filter_data[DS_FILTER_DEPTH_SIZE];
    u8 filter_mask[DS_FILTER_DEPTH_SIZE];
    u16 req_handle;
    u32 start_ms;
    u32 req_timeout;
    u8 *p_buf;
}s_ds_filter_struct_t;

typedef struct _s_ds_data_got
{
    u8 req_id;
    u8 u_ok;
    u16 pid;
    u8 *p_data;
    u16 length;
}s_ds_data_got_t;



static s_ds_filter_struct_t ds_filter[DS_FILTER_MAX_NUM] = {{0,},};
static u8 ds_filter_buf[DS_FILTER_MAX_NUM][DS_FILTER_BUFFER_SIZE] = {{0,},};
static s_ds_data_got_t ds_data[DS_FILTER_MAX_NUM] = {{0,},};
static u8 ds_data_buf[DS_FILTER_MAX_NUM][DS_DATA_BUFFER_SIZE] = {{0,},};



extern e_ds_smartcard_status_t s_ds_uScStatus;
extern cas_adapter_priv_t g_cas_priv;
extern UINT16 ds_ecm_pid;
extern UINT16 ds_service_id;

UINT8 gpucMessage[4] = {0};


extern scard_device_t *p_ds_scard_dev;

finger_msg_t ds_msgFinger = {{0,},};
msg_info_t ds_OsdMsg = {0, {0,},};
u8 ds_watch_level = 0;
u8 ds_msg_show_type = 0;
ST_CA_EXIGENT_PROGRAM ds_Exigent = {0};
ST_IPPV_INFO ds_ippv = {{0,},};



static u16 v_channel = 0xffff;
static u16 a_channel = 0xffff;
static u16 old_v_channel = 0xffff;
static u16 old_a_channel = 0xffff;
static u8 ecm_flag = 0;
static u32 ecm_uspid = 0;
static u8 reset_flag = 0;


os_sem_t ca_filter_lock[10] = {0};
os_sem_t ca_osd_lock = 0;
os_sem_t ca_finger_lock = 0;


static void data_dump(u8 *p_addr, u32 size)
{
   u32 i = 0;

   for(i = 0; i < size; i++)
   {
      DESAI_DRV_DEBUG("%02x ", p_addr[i]);
      if ((0 == ((i + 1) % 20)) && (i != 0))
     {
         DESAI_DRV_DEBUG("\n");
     }
   }
   DESAI_DRV_DEBUG("\n");
}


void DS_STB_Drv_CaReqLock(os_sem_t *pSemaphore)
{
    BOOL ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)pSemaphore, 0);
    if(ret == FALSE)
      DESAI_DRV_DEBUG("[DS]SEM TAKE ERROR\n");
}
void DS_STB_Drv_CaReqUnLock(os_sem_t *pSemaphore)
{
    BOOL ret = FALSE;
    ret = mtos_sem_give((os_sem_t *)pSemaphore);
   if(ret == FALSE)
      DESAI_DRV_DEBUG("[DS]SEM GIVE ERROR\n");
}


static RET_CODE ds_filter_free(s_ds_filter_struct_t *filter)
  {
      s32 ret = SUCCESS;
      dmx_device_t *p_dev = NULL;

      DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
      p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != p_dev->p_base);

      ret = dmx_chan_stop(p_dev, filter->req_handle);
      MT_ASSERT(ret == SUCCESS);

      ret = dmx_chan_close(p_dev, filter->req_handle);
      MT_ASSERT(ret == SUCCESS);

      return SUCCESS;
  }




/******************1:flash interface**********************************/
void DSCA_GetBuffer(OUT UINT32 * puiStartAddr,  OUT UINT32 * puiSize)
{
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    p_cas_ds_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    MT_ASSERT(NULL != puiSize);
    MT_ASSERT(NULL != puiStartAddr);


    *puiStartAddr = p_cas_ds_priv->flash_start_add;
    *puiSize = p_cas_ds_priv->flash_size;
}

#if 0
void DSCA_FlashRead(UINT32 uiStartAddr,  OUT UINT8 * pucData,  IN UINT32 * puiLen)
{
   UINT32 length = *puiLen;
   UINT32 start_address = uiStartAddr;
   RET_CODE ret;
   s_cas_ds_priv_t *p_cas_ds_priv = NULL;

   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

   p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;


  //-------- param  identfy------------------
   if ((start_address < p_cas_ds_priv->flash_start_add)
            || (start_address > p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, invalid start address pointer param!\n", __FUNCTION__, __LINE__);
    }
     if ((start_address + length) > (p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, data length exceed the max space!\n", __FUNCTION__, __LINE__);
    }

   MT_ASSERT(pucData != NULL);
   //-------- param  identfy------------------
    if ((NULL != p_cas_ds_priv->nvram_read) && (0 != length))
    {
        ret = p_cas_ds_priv->nvram_read((u32)uiStartAddr, (u8 *)pucData, (u32 *)&length);
        if(SUCCESS != ret)
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Read data from nvram failed!\n", __FUNCTION__, __LINE__);
        }
    }
}


void DSCA_FlashWrite(UINT32 uiStartAddr, IN UINT8* pucData, IN UINT32 uiLen)
{
   UINT32 length = uiLen;
   UINT32 start_address = uiStartAddr;
   RET_CODE ret;
   s_cas_ds_priv_t *p_cas_ds_priv = NULL;

   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

   p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    ////-------- param  identfy------------------
     if ((start_address < p_cas_ds_priv->flash_start_add)
              || (start_address > p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
      {
          DESAI_DRV_DEBUG("[DS]%s:LINE:%d, invalid start address pointer param!\n", __FUNCTION__, __LINE__);
      }
       if ((start_address + length) > (p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
      {
          DESAI_DRV_DEBUG("[DS]%s:LINE:%d, data length exceed the max space!\n", __FUNCTION__, __LINE__);
      }

     MT_ASSERT(pucData != NULL);
  //-------- param  identfy------------------
    if ((NULL != p_cas_ds_priv->nvram_write) && (0 != length))
    {
        ret = p_cas_ds_priv->nvram_write(uiStartAddr, (u8 *)pucData, length);
        if(SUCCESS != ret)
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d,Write data to nvram failed!\n", __FUNCTION__, __LINE__);
        }
    }
}
#endif

void DSCA_FlashRead(UINT32 uiStartAddr,  OUT UINT8 * pucData,  IN UINT32 * puiLen)
{
   UINT32 length = *puiLen;
   UINT32 start_address = uiStartAddr;
   void *p_dev = NULL;
   RET_CODE ret;
   s_cas_ds_priv_t *p_cas_ds_priv = NULL;

   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   //mtos_printk("---->DSCA_FlashRead START!!!\n");

   p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);


  //-------- param  identfy------------------
   if ((start_address < p_cas_ds_priv->flash_start_add)
            || (start_address > p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
  {
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, invalid start address pointer param!\n", __FUNCTION__, __LINE__);
  }
  if ((start_address + length) > (p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
  {
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, data length exceed the max space!\n", __FUNCTION__, __LINE__);
  }

  MT_ASSERT(pucData != NULL);
  //-------- param  identfy------------------
  if (0 != length)
  {
    ret = charsto_read(p_dev, (u32)uiStartAddr, (u8 *)pucData, length);
    if(SUCCESS != ret)
    {
      DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Read data from nvram failed!\n", __FUNCTION__, __LINE__);
    }
  }
    //mtos_printk("---->DSCA_FlashRead OVER\n!!!");
}


void DSCA_FlashWrite(UINT32 uiStartAddr, IN UINT8* pucData, IN UINT32 uiLen)
{
   UINT32 length = uiLen;
   UINT32 start_address = uiStartAddr;
   void *p_dev = NULL;
   RET_CODE ret;
   s_cas_ds_priv_t *p_cas_ds_priv = NULL;

   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   //mtos_printk("---->DSCA_FlashWrite START!!!\n");

   p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);

    ////-------- param  identfy------------------
   if ((start_address < p_cas_ds_priv->flash_start_add)
            || (start_address > p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
    {
      DESAI_DRV_DEBUG("[DS]%s:LINE:%d, invalid start address pointer param!\n", __FUNCTION__, __LINE__);
    }
     if ((start_address + length) > (p_cas_ds_priv->flash_start_add + p_cas_ds_priv->flash_size))
    {
      DESAI_DRV_DEBUG("[DS]%s:LINE:%d, data length exceed the max space!\n", __FUNCTION__, __LINE__);
    }

   MT_ASSERT(pucData != NULL);
  //-------- param  identfy------------------
    if (0 != length)
    {
      ret = charsto_writeonly(p_dev, uiStartAddr, (u8 *)pucData, length);
      if(SUCCESS != ret)
      {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d,Write data to nvram failed!\n", __FUNCTION__, __LINE__);
      }
    }
    //mtos_printk("---->DSCA_FlashWrite OVER!!!\n");
}

#if 0
void DSCA_FlashErase(void)
{
    u8 ca_buf[CHARSTO_SECTOR_SIZE] = {0};
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;
    RET_CODE ret;

   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

   p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    memset(ca_buf, 0xFF, CHARSTO_SECTOR_SIZE);

    if(NULL != p_cas_ds_priv->nvram_write)
    {
        ret = p_cas_ds_priv->nvram_write(p_cas_ds_priv->flash_start_add, ca_buf, CHARSTO_SECTOR_SIZE);
        if(SUCCESS != ret)
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d,Write data to nvram failed!\n", __FUNCTION__, __LINE__);
        }
    }
}
#endif

void DSCA_FlashErase(void)
{
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;
    void *p_dev = NULL;
    RET_CODE ret;

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);


    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    //mtos_printk("---->DSCA_FlashErase START!!!\n");

    p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;
    //mtos_printk("flash_addr:0x%x, size = 0x%x\n", p_cas_ds_priv->flash_start_add, p_cas_ds_priv->flash_size);

    ret = charsto_erase(p_dev, p_cas_ds_priv->flash_start_add, 1);
    if(ret != SUCCESS)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    }
    //mtos_printk("---->DSCA_FlashErase OVER!!!\n");
}



/******************2:memory interface**********************************/
 void* DSCA_Malloc(UINT16 usBufferSize)
{
   void *p_buf = NULL;
   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   p_buf = mtos_malloc(usBufferSize);
   MT_ASSERT(NULL != p_buf);
   memset(p_buf, 0, usBufferSize);
   return p_buf;
}

void DSCA_Free(IN void* pucBuffer)
{
   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   if(NULL == pucBuffer)
     return;
   mtos_free(pucBuffer);
}

void DSCA_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize)
{
 //  DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   memset(pucBuffer, ucValue, uiSize);
}

 void DSCA_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize)
 {
 //   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    memcpy(pucDestBuffer, pucSourceBuffer, uiSize);
 }

 INT32 DSCA_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize)
 {
 //   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    return memcmp(pucDestBuffer, pucSourceBuffer, uiSize);
 }

 INT32 DSCA_StrLen(IN const UINT8* pucFormatBuffer)
 {
 //   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    return strlen((const char *)pucFormatBuffer);
 }



/******************3:os interface**************************************/
 UINT8 DSCA_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun )
{
    u8 ret = 0;
    u8 uIndex = 0;
    u32 byPriority = 0;
    u32 *p_stack = NULL;
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d,create task ! Name[%s], Priority[%d]\n", __FUNCTION__, __LINE__, pucName, ucPriority);

    p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    MT_ASSERT(NULL != pucName);
    MT_ASSERT(NULL != pTaskFun);

    for(uIndex = 0; uIndex < DS_CAS_TASK_NUM; uIndex++)
   {
     if(DS_TASK_STATUS_FREE == ds_task_record[uIndex].task_status)
     {
       ds_task_record[uIndex].task_status = DS_TASK_STATUS_BUSY;
       break;
     }
   }

    if(uIndex >= DS_CAS_TASK_NUM)
    {
       OS_PRINTF("[DS]TFSTBCA_RegisterTask too many task or invalid task priority!\n");
       return DS_TASK_FAIL ;
     }

    if (0 != p_cas_ds_priv->task_prio)
    {
        byPriority = p_cas_ds_priv->task_prio;
    }
    else
    {
        byPriority = DS_TASK_PRIORITY_0;
    }

    ds_task_record[uIndex].task_prio = byPriority + uIndex;

    p_stack = (u32 *)mtos_malloc(DS_COMMON_STACK_SIZE);
    MT_ASSERT(NULL != p_stack);

    ret = mtos_task_create((u8 *)pucName,
        (void *)pTaskFun,
        NULL,
        ds_task_record[uIndex].task_prio,
        p_stack,
        DS_COMMON_STACK_SIZE);

    if (!ret)
    {
        OS_PRINTF("[DS]%s:LINE:%d create task error = 0x%08x!\n",__FUNCTION__, __LINE__, ret);
        ds_task_record[uIndex].task_status = DS_TASK_STATUS_FREE;
        return DS_TASK_FAIL;
    }

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d,create task success! Name[%s], Priority[%d]\n", __FUNCTION__, __LINE__, pucName, ds_task_record[uIndex].task_prio);

    return DS_TASK_SUCESS;
}


 void DSCA_Sleep(UINT16 usMilliSeconds)
 {
   DESAI_DRV_DEBUG("[DS]DSCA_Sleep:%dms\n", usMilliSeconds);
   mtos_task_sleep(usMilliSeconds);
 }

 void DSCA_SemaphoreInit(IN DSCA_Semaphore* puiSemaphore , UINT8 ucInitVal)
 {

  DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

  MT_ASSERT(puiSemaphore != NULL);
  ucInitVal = ucInitVal ? 1 : 0;
  if(mtos_sem_create((os_sem_t *)puiSemaphore, ucInitVal))
  {
      DESAI_DRV_DEBUG("[DS]DESAICA_SemaphoreInit OK!\n");
  }
  else
  {
  DESAI_DRV_DEBUG("[DS]CAS create semphore failed!\n");
   MT_ASSERT(0);
  }
 }


 void DSCA_ReleaseSemaphore(IN DSCA_Semaphore* puiSemaphore)
 {
//   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   MT_ASSERT(puiSemaphore != NULL);
   if(TRUE != mtos_sem_give((os_sem_t *)puiSemaphore))
    {
        DESAI_DRV_DEBUG("[DS]CAS release semphore failed!\n");
        MT_ASSERT(0);
    }
 }

 void DSCA_WaitSemaphore(IN DSCA_Semaphore* puiSemaphore)
 {
//   DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
   MT_ASSERT(puiSemaphore != NULL);
   if(TRUE != mtos_sem_take((os_sem_t *)puiSemaphore, 0))
    {
        DESAI_DRV_DEBUG("[DS]CAS waite semphore failed!\n");
        MT_ASSERT(0);
    }
 }


 UINT8 DSCA_MsgQueueInit(IN INT8* pucName, IN UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_CA_MSG_QUEUE_MODE enMode)
 {
     INT32 err ;
     UINT32 message_id = 0;
     void *p_quenue_buf = NULL;
     UINT32 quenue_buf_size = 0;

     DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueInit \n");
     DESAI_DRV_DEBUG("[DS]queue, name:%s, maxQueueLength=%d, taskWaitMode=%d, \n",pucName,uiMaxMsgLen,enMode);

     MT_ASSERT(uiMsgQueueHandle != NULL);
     MT_ASSERT(pucName != NULL);


    message_id = mtos_messageq_create(uiMaxMsgLen,(UINT8 *)pucName);
    MT_ASSERT(message_id != INVALID_MSGQ_ID);


    quenue_buf_size = sizeof(ST_CA_MSG_QUEUE) * uiMaxMsgLen;
    p_quenue_buf = mtos_malloc(quenue_buf_size);
    DESAI_DRV_DEBUG(" [DS]message_id=%d, buf_addr=0x%x \n",message_id,p_quenue_buf);
    MT_ASSERT(p_quenue_buf != NULL);
    memset(p_quenue_buf,0,quenue_buf_size);

    err = mtos_messageq_attach(message_id,p_quenue_buf,sizeof(ST_CA_MSG_QUEUE),uiMaxMsgLen);
    if(err == FALSE)
    {
         mtos_messageq_release(message_id);
         mtos_free(p_quenue_buf);
         DESAI_DRV_DEBUG("[DS]mtos_messageq_attach err ! \n");
         return DS_QUEUE_INIT_FAIL;
    }

    *uiMsgQueueHandle = message_id;
    DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueInit  0K\n");

    return DS_QUEUE_INIT_SUCCESS;
}

 UINT8 DSCA_MsgQueueGetMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg, EN_CA_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond)
 {
    s32 err;
    UINT32 message_id = 0;
    //UINT32 size = 0;
    os_msg_t msg = {0};

    DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueGetMsg START\n");

     MT_ASSERT(pstMsg != NULL);

    message_id = uiMsgHandle;

//    DESAI_DRV_DEBUG("[DS]queueId = %d, queue_waite_mode = %d, queue_waite_time = %d   \n",uiMsgHandle,enMode,uiWaitMilliSecond);

    if(enMode == CA_MSG_QUEUE_NOWAIT)
          err = mtos_messageq_receive(message_id,&msg,10);
    else
         err = mtos_messageq_receive(message_id,&msg,0);
    if(err == FALSE)
    {
         DESAI_DRV_DEBUG("[DS]DESAI_DRV_DEBUG,message_id=%d, receive msg error ! \n",message_id);
         return DS_QUEUE_GET_FAIL;
    }

     pstMsg = (ST_CA_MSG_QUEUE *)msg.para1;
     DESAI_DRV_DEBUG("[DS]queueId = %d , msg.para1(addr)=0x%x, msg.para2(size)=0x%x \n\n",message_id,msg.para1,msg.para2);
     DESAI_DRV_DEBUG("[DS]message receive data:0X%08X,0X%08X,0X%08X,0X%08X \n",pstMsg->uiFirstPara,pstMsg->uiSecondPara,
                                                                                pstMsg->uiThirdPara,pstMsg->uiFourthPara);
     DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueGetMsg  OVER\n");

    return DS_QUEUE_GET_SUCCESS;
}


 UINT8 DSCA_MsgQueueSendMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg)
{
      s32 ret = 0;
      os_msg_t message = {0};
      UINT32 message_id = 0;

      DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueSendMsg START\n");

      MT_ASSERT(NULL != pstMsg);

      message_id = uiMsgHandle;

      message.is_ext = 1;
      message.para1 = (u32)pstMsg;
      message.para2 = sizeof(ST_CA_MSG_QUEUE);

      DESAI_DRV_DEBUG("[DS]message receive data:0X%08X,0X%08X,0X%08X,0X%08X \n",pstMsg->uiFirstPara,pstMsg->uiSecondPara,
                                                                                pstMsg->uiThirdPara,pstMsg->uiFourthPara);


      ret = mtos_messageq_send(message_id, &message);

      if(ret != TRUE)
      {
          DESAI_DRV_DEBUG("[DS]DS send message failed.\n");
          return DS_QUEUE_SEND_FAIL;
      }

      DESAI_DRV_DEBUG("[DS]message receive data:0X%08X,0X%08X,0X%08X,0X%08X \n",pstMsg->uiFirstPara,pstMsg->uiSecondPara,
                                                                                pstMsg->uiThirdPara,pstMsg->uiFourthPara);

      DESAI_DRV_DEBUG("[DS]DSCA_MsgQueueSendMsg OVER\n");

      return DS_QUEUE_SEND_SUCESS;
 }



/******************4:smc interface**************************************/
UINT8 DSCA_SCReset(UINT8 *pucAtr, UINT8 *pucLen)
{
    scard_atr_desc_t atr = {0};
    u8 index = 0;

    s16 ret = 0;
    u8 buf[32];

    if(pucAtr == NULL)
    {
        DESAI_DRV_DEBUG("[DS_RESET] NULL POINTER!!!! \n");
        return DS_REST_ERROR;
    }

    atr.p_buf = buf;
    memset(buf, 0, sizeof(buf));
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    for (index = 0; index < 3; index ++)
    {
        ret = scard_active(p_ds_scard_dev, &atr);
        if (SUCCESS == ret)
        {
          *pucLen = atr.atr_len;
          memcpy(pucAtr, atr.p_buf, *pucLen);
          break;
        }
    }
    if (SUCCESS != ret)
    {
        pucAtr = NULL;
        *pucLen = 0;
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, active smart card failed!\n", __FUNCTION__, __LINE__);
        return DS_REST_ERROR;
    }

    DESAI_DRV_DEBUG("[DS]DSCA_SCReset OVER\n");

    reset_flag = 1;

    return DS_REST_OK;
}


 void DSCA_SendDataToSmartCard(IN UINT8* pucSend, IN UINT8 *pucSendLen, OUT UINT8* pucReceive,
					OUT UINT8 *pucReceiveLen, UINT32 uiTimeout, OUT UINT8* pucStatusWord )
{
  scard_opt_desc_t opt_desc = {0};
  RET_CODE ret = ERR_FAILURE;
  s8 retry_times = 3;
  s8 index = 0;
  scard_atr_desc_t atr = {0};
  static u8 card_update_flag = 0;

  MT_ASSERT(pucSend != NULL);
  MT_ASSERT(pucSendLen != NULL);
  MT_ASSERT(pucReceive != NULL);
  MT_ASSERT(pucReceiveLen != NULL);
  MT_ASSERT(pucStatusWord != NULL);

  DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

   DESAI_DRV_DEBUG("Tx:\n");
   data_dump(pucSend, *pucSendLen);

  opt_desc.p_buf_out = pucSend;
  opt_desc.bytes_out = *pucSendLen;
  opt_desc.p_buf_in = pucReceive;
  opt_desc.bytes_in_actual = 0;

   if(pucSend[0] == 0x74 && pucSend[1] == 0xf5 && pucSend[4] == 0x02 && *pucSendLen == 5)
    retry_times = 1;

 if(reset_flag == 1)
  {
  mtos_task_lock();
  do
    {
        ret = scard_pro_rw_transaction(&opt_desc);
        if(ret == SUCCESS)
        {
            DESAI_DRV_DEBUG("[DS]T0 transaction success!\n");
            break;
        }
        retry_times--;
    }while (retry_times > 0);
  mtos_task_unlock();
  }

   //read_bytes= data + sw1+sw2
   if(ret == SUCCESS)
    {
       if(opt_desc.bytes_in_actual > 2)
      {
          pucStatusWord[0] = pucReceive[opt_desc.bytes_in_actual - 2];
          pucStatusWord[1] = pucReceive[opt_desc.bytes_in_actual - 1];
          *pucReceiveLen = opt_desc.bytes_in_actual - 2;
      }
      else if(opt_desc.bytes_in_actual == 2)
      {
          pucStatusWord[0] = pucReceive[0];
          pucStatusWord[1] = pucReceive[1];
          *pucReceiveLen = 0;
      }
      else
      {
          DESAI_DRV_DEBUG("[DS]DSCA_SendDataToSmartCard error! \n");
          MT_ASSERT(0);
      }

      #ifdef DESAI_DRV_DEBUG_EN
      DESAI_DRV_DEBUG("Rx:\n");
      data_dump(opt_desc.p_buf_in, opt_desc.bytes_in_actual);
      #endif
    }
    else
    {
        DESAI_DRV_DEBUG("T0 transaction failed!\n");
       data_dump(pucSend, *pucSendLen);
       *pucReceiveLen = 0;
       if(card_update_flag == 1)
       {
           for (index = 0; index < 3; index ++)
          {
              ret = scard_active(p_ds_scard_dev, &atr);
              if (SUCCESS == ret)
              {
                 DESAI_DRV_DEBUG("card reset ok: %d!\n", index);
                 break;
              }
              else
                DESAI_DRV_DEBUG("card reset failed!\n");
           }
           card_update_flag = 0;
       }
   }


    if(pucSend[0] == 0x74 && pucSend[1] == 0xf0 && pucSend[4] == 0x01 && *pucSendLen == 5)
   {
      mtos_task_delay_ms(200);
      DESAI_DRV_DEBUG("卡升级开始\n");
  }

   if(pucSend[0] == 0x74 && pucSend[1] == 0xf5 && pucSend[4] == 0x02 && *pucSendLen == 5)
  {
      mtos_task_delay_ms(1000);
      DESAI_DRV_DEBUG("卡升级结束复位\n");
      card_update_flag = 1;
  }
}


/******************5:filter interface**************************************/
 UINT8 DSCA_SetFilter(EN_CA_DATA_ID enDataID, IN UINT8* pucFilter, IN UINT8* pucMask,
				UINT8 ucFilterLen, UINT16 usPID, UINT8 ucWaitSeconds)
{
    u8 index = 0;
    s16 ret = ERR_FAILURE;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;

    DESAI_DRV_DEBUG("[DS]设置过滤表DSCA_SetFilter start\n");
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in, byReqId=%d, wPID=0x%x, ucFilterLen = %d,waitsecs=%d!\n", __FUNCTION__, __LINE__, enDataID, usPID,ucFilterLen,ucWaitSeconds);

    MT_ASSERT(pucFilter != NULL);
    MT_ASSERT(pucMask != NULL);

    for(index = 0; index < ucFilterLen; index ++)
    {
         DESAI_DRV_DEBUG("FilterData[%d]:0X%X  ",index,pucFilter[index]);
         DESAI_DRV_DEBUG("FilterMask[%d]:0X%X \n",index,pucMask[index]);
    }


    for (index = 0; index < DS_FILTER_MAX_NUM; index ++)
    {
        DS_STB_Drv_CaReqLock(&ca_filter_lock[index]);
        if ((DS_FILTER_STATUS_BUSY == ds_filter[index].filter_status)
                && (enDataID == ds_filter[index].req_id)
                &&(ECM_DATA_ID!= enDataID))
        {
            if((memcmp((void *)pucFilter, ds_filter[index].filter_data, ucFilterLen) == 0)
                && (memcmp((void *)pucMask, ds_filter[index].filter_mask, ucFilterLen) == 0)
                && (ds_filter[index].pid == usPID))
             {
                  DESAI_DRV_DEBUG("[DS]Emm filter is the same, don't request new filter!!!!\n");
                   DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
                 return DS_FILTER_SET_OK;
             }
            else
            {
                DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Filter is used, cancel old filter and request new filter!!!!\n",
                            __FUNCTION__, __LINE__);
               ret = ds_filter_free(&ds_filter[index]);
               if (SUCCESS != ret)
              {
                  DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Free filter failed!!!!\n", __FUNCTION__, __LINE__);
                   DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
                  return DS_FILTER_SET_FAIL;
              }
              memset((void *)&ds_filter[index], 0, sizeof(s_ds_filter_struct_t));
           }
        }
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
    }

    for (index = 0; index < DS_FILTER_MAX_NUM; index ++)
    {
        DS_STB_Drv_CaReqLock(&ca_filter_lock[index]);
        if (DS_FILTER_STATUS_FREE == ds_filter[index].filter_status)
        {
            DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
            break;
        }
         DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
    }
    if (index >= DS_FILTER_MAX_NUM)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, filter is full!!!!!\n", __FUNCTION__, __LINE__);
        return DS_FILTER_SET_FAIL;
    }

    if(ECM_DATA_ID == enDataID)
    {
      ecm_flag = 0;
      ecm_uspid = usPID;
    }

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    p_slot.in = DMX_INPUT_EXTERN0;
    p_slot.pid = usPID;
    p_slot.type = DMX_CH_TYPE_SECTION;
    DS_STB_Drv_CaReqLock(&ca_filter_lock[index]);
    ret = dmx_si_chan_open(p_dev, &p_slot, &ds_filter[index].req_handle);
    if (SUCCESS != ret)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, open chan failed, ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
        return DS_FILTER_SET_FAIL;
    }

    ds_filter[index].p_buf = ds_filter_buf[index];

    ret = dmx_si_chan_set_buffer(p_dev, ds_filter[index].req_handle, ds_filter[index].p_buf, DS_FILTER_BUFFER_SIZE);
    if (SUCCESS != ret)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, chan set buffer failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
        return DS_FILTER_SET_FAIL;
    }

    p_param.continuous = TRUE;
    p_param.en_crc = FALSE;
    memset(p_param.mask, 0, DMX_SECTION_FILTER_SIZE);
    memset(p_param.value, 0, DMX_SECTION_FILTER_SIZE);
    p_param.mask[0] = pucMask[0];
    p_param.value[0] = pucFilter[0];
    if(ucFilterLen > 1)
    {
      memcpy(&p_param.mask[1], &pucMask[1], ucFilterLen-1);
      memcpy(&p_param.value[1], &pucFilter[1], ucFilterLen-1);
    }


    ret = dmx_si_chan_set_filter(p_dev, ds_filter[index].req_handle, &p_param);
    if (SUCCESS != ret)
    {
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
        OS_PRINTF("[DS]set filter failed!!!!!!!!!\n");
        return DS_FILTER_SET_FAIL;
    }

    ret = dmx_chan_start(p_dev, ds_filter[index].req_handle);
    DESAI_DRV_DEBUG("[DS]dmx chan start\n");
    if (SUCCESS != ret)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, chan start failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
        return DS_FILTER_SET_FAIL;
    }

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, nWaitSeconds=%d!\n", __FUNCTION__, __LINE__, ucWaitSeconds);
    if (0 == ucWaitSeconds)
    {
        ds_filter[index].req_timeout = 0xffffffff;
    }
    else
    {
        ds_filter[index].req_timeout = ucWaitSeconds * 1000;
    }
    ds_filter[index].filter_status = DS_FILTER_STATUS_BUSY;
    ds_filter[index].req_id = enDataID;
    ds_filter[index].pid = usPID;
    memcpy(ds_filter[index].filter_data, pucFilter, ucFilterLen);
    memcpy(ds_filter[index].filter_mask, pucMask, ucFilterLen);
    ds_filter[index].start_ms = mtos_ticks_get();
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, ===filter data:index=%d,ds_filter[index].req_id=%d,ds_filter[index].pid=0x%x!\n",
            __FUNCTION__, __LINE__, index,ds_filter[index].req_id,ds_filter[index].pid);

      DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);

    DESAI_DRV_DEBUG("[DS]DSCA_SetFilter OK\n");

    return DS_FILTER_SET_OK;
}

 void DSCA_SetCW(UINT16 usEcmPID, IN UINT8* pucOddKey, IN UINT8* pucEvenKey, IN UINT8* pucKeyLen)
 {
    u16 v_pid = 0;
    u16 a_pid = 0;
    RET_CODE ret ;
    dmx_device_t *p_dev = NULL;
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;

    DESAI_DRV_DEBUG("[DS]DSCA_SetCW start\n");

    MT_ASSERT(pucOddKey != NULL);
    MT_ASSERT(pucEvenKey != NULL);

    DESAI_DRV_DEBUG("[DS]DSCA_SetCW pucEvenKey :%d, pucKeyLen:%d\n",*pucEvenKey,*pucKeyLen);

    DESAI_DRV_DEBUG(
    "OddKey:0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\tEvenKey:0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\n"
    , pucOddKey[0], pucOddKey[1], pucOddKey[2], pucOddKey[3]
    , pucOddKey[4], pucOddKey[5], pucOddKey[6], pucOddKey[7]
    , pucEvenKey[0], pucEvenKey[1], pucEvenKey[2], pucEvenKey[3]
    , pucEvenKey[4], pucEvenKey[5], pucEvenKey[6], pucEvenKey[7]);

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    v_pid = p_cas_ds_priv->v_pid;
    a_pid = p_cas_ds_priv->a_pid;

    if (old_v_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, old_v_channel, FALSE);
        old_v_channel = 0xffff;
    }
    if (old_a_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, old_a_channel, FALSE);
        old_a_channel = 0xffff;
    }

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, v_pid=%d, a_pid=%d!\n", __FUNCTION__, __LINE__, v_pid, a_pid);
    ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if (ret != SUCCESS)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get chan id by v_pid failed!!!!\n", __FUNCTION__, __LINE__);
        return ;
    }

    ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if (ret != SUCCESS)
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get chan id by a_pid failed!!!!\n", __FUNCTION__, __LINE__);
        return ;
    }
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, set key v_channel= %x  a_channel = %x\n", __FUNCTION__, __LINE__, v_channel, a_channel);
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, set key old_v_channel= %x  old_a_channel = %x\n",  __FUNCTION__, __LINE__, old_v_channel, old_a_channel);

     if ((a_channel != 0xffff) && (a_pid != 0x1fff))
    {
        ret = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
        if (ret != SUCCESS)
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d, enable audio descrambler failed!!!!\n", __FUNCTION__, __LINE__);
            MT_ASSERT(0);
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, a_channel, (u8 *)pucOddKey, 8);
        ret = dmx_descrambler_set_even_keys(p_dev, a_channel, (u8 *)pucEvenKey, 8);
    }
    else
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, No audio prpgram playing now!!!!\n", __FUNCTION__, __LINE__);
    }

    if (v_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, v_channel, TRUE);
        if (ret != SUCCESS)
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d, enable video descrambler failed!!!!\n", __FUNCTION__, __LINE__);
            MT_ASSERT(0);
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, v_channel, (u8 *)pucOddKey, 8);
        ret = dmx_descrambler_set_even_keys(p_dev, v_channel, (u8 *)pucEvenKey, 8);
    }
    else
    {
        DESAI_DRV_DEBUG("[DS]%s:LINE:%d, No video prpgram playing now!!!!\n", __FUNCTION__, __LINE__);
    }


    old_v_channel = v_channel;
    old_a_channel = a_channel;

    DESAI_DRV_DEBUG("[DS]DSCA_SetCW over\n");

 }


 UINT8 DSCA_StopFilter(EN_CA_DATA_ID enDataID, UINT16 usPID)
 {
     u8 index = 0;
     u32 ret;
     DESAI_DRV_DEBUG("[DS]DSCA_StopFilter start\n");
     for (index = 0; index < DS_FILTER_MAX_NUM; index ++)
    {
       DS_STB_Drv_CaReqLock(&ca_filter_lock[index]);
        if ((enDataID == ds_filter[index].req_id) && (ds_filter[index].pid = usPID))
        {
            DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Filter is used, free filter!\n",
                            __FUNCTION__, __LINE__);
            ret = ds_filter_free(&ds_filter[index]);
            if (SUCCESS != ret)
            {
                DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Free filter failed!!!!\n", __FUNCTION__, __LINE__);
                DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
                return DS_FILTER_SET_FAIL;
            }
            memset((void *)&ds_filter[index], 0, sizeof(s_ds_filter_struct_t));
        }
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[index]);
    }
    DESAI_DRV_DEBUG("[DS]DSCA_StopFilter OK\n");
    return DS_FILTER_SET_OK;
 }


 static void DSCAS_SendEvent(s_cas_ds_priv_t *p_priv, u32 event, UINT8 * pucMessage)
 {
     DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

     if (event == 0)
     {
         return;
     }
     cas_send_event(p_priv->slot, CAS_ID_DS, event, (u32)pucMessage);
 }


 void DSCA_StopCurrentProgram(void)
 {
     s_cas_ds_priv_t *p_priv = NULL;
     //UINT8 gpucMessage[4] = {0};
     gpucMessage[0] = ds_ecm_pid >> 8;
     gpucMessage[1] = ds_ecm_pid & 0xff;
     gpucMessage[2] = ds_service_id>> 8;
     gpucMessage[3] = ds_service_id & 0xff;
     p_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;
     DESAI_DRV_DEBUG("[DS]%s:LINE:%d,ds_ecm_pid =%d,ds_service_id = %d \n", __FUNCTION__, __LINE__,ds_ecm_pid,ds_service_id);
     DSCAS_SendEvent(p_priv, CAS_C_IEXIGENT_PROG_STOP, gpucMessage);
 }

 void DSCA_PlayCurrentProgram(void)
 {
    s_cas_ds_priv_t *p_priv = NULL;
    UINT8 * pucMessage = NULL;
    p_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d\n", __FUNCTION__, __LINE__);
    DSCAS_SendEvent(p_priv, CAS_C_IEXIGENT_PROG, pucMessage);
 }



/******************6:message interface**************************************/
void DSCA_ShowMessage(UINT8 ucMessageType, EN_CAS_SHOW_MSG_TYPE  enShowType, IN UINT8 * pucMessage)
  {
      u32 event = 0;
      s_cas_ds_priv_t *p_priv = NULL;
      ST_FINGER_SHOW_INFO *p_ds_finger = NULL;
      ST_OSD_SHOW_INFO *p_ds_osd = NULL;
      ST_CA_EXIGENT_PROGRAM *p_exigent = NULL;
      ST_IPPV_INFO *p_ippv = NULL;

      DESAI_DRV_DEBUG("[DS]DSCA_ShowMessage,ucMessageType: 0x%x, enShowType: %d, pucMessage: %s \n", ucMessageType,enShowType,pucMessage);

      p_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;

      ds_msg_show_type = enShowType;

      switch (ucMessageType)
      {
         case MESSAGE_CANCEL_TYPE:  /* 取消显示 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, cancle display!\n", __FUNCTION__, __LINE__);
              event = CAS_S_CLEAR_DISPLAY;
              break;
          case MESSAGE_BADCARD_TYPE:  /* 无法识别卡  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, can't identify the card!\n", __FUNCTION__, __LINE__);
              event = CAS_E_ILLEGAL_CARD;
              break;
           case MESSAGE_EXPICARD_TYPE:  /*智能卡已经过期，请更换新卡 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Card overdue!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_OVERDUE;
              break;
           case MESSAGE_CARD_INIT_TYPE: /*智能卡正在初始化 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡正在初始化!\n", __FUNCTION__, __LINE__);
              event = CAS_S_CARD_UPG_START;
              break;
           case MESSAGE_INSERTCARD_TYPE:  /*加密节目，请插入智能卡 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, the card requested insert to the stb!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_REGU_INSERT;
              break;
           case MESSAGE_CARD_INIT_ERR_TYPE:  /*智能卡初始化失败 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡初始化失败!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_INIT_FAIL;
              break;
           case MESSAGE_STB_NO_CARD_TYPE:  /*机顶盒中不存在智能卡 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 机顶盒中不存在智能卡!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_NOT_FOUND;
              break;
           case MESSAGE_NOOPER_TYPE: /*卡中不存在节目运营商 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 卡中不存在节目运营商!\n", __FUNCTION__, __LINE__);
              event = CAS_E_NO_OPTRATER_INFO;
              break;
           case MESSAGE_CARDSTOP_TYPE: /*智能卡已停用 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡已停用!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_FREEZE;
              break;
           case MESSAGE_OUTWORKTIME_TYPE: /*不在工作时段内 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 不在工作时段内!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_OUT_WORK_TIME;
              break;
           case MESSAGE_WATCHLEVEL_TYPE: /*节目级别高于设定观看级别 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 节目级别高于设定观看级别!\n", __FUNCTION__, __LINE__);
              ds_watch_level = *pucMessage;
              DESAI_DRV_DEBUG("[DS] ds_watch_level : %d!\n",ds_watch_level);
              event = CAS_E_CARD_CHILD_UNLOCK;
              break;

           case MESSAGE_PAIRING_TYPE: /*机卡没有对应 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 机卡没有对应!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_MODULE_FAIL;
              break;
           case MESSAGE_NOENTITLE_TYPE: /*您没有购买该节目 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 您没有购买该节目!\n", __FUNCTION__, __LINE__);
              event = CAS_E_PROG_UNAUTH;
              break;
           case MESSAGE_DECRYPTFAIL_TYPE: /*节目解密失败 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 节目解密失败!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CW_INVALID;
              break;
           case MESSAGE_NOMONEY_TYPE: /*卡内金额不足   */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 卡内金额不足!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CADR_NO_MONEY;
              break;

           case MESSAGE_ERRREGION_TYPE: /*区域不正确 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 区域不正确!\n", __FUNCTION__, __LINE__);
              event = CAS_E_ZONE_CODE_ERR;
              break;
           case MESSAGE_FINGER_SHOW_TYPE: /*指纹显示 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 指纹显示!\n", __FUNCTION__, __LINE__);
              p_ds_finger  = (ST_FINGER_SHOW_INFO *)pucMessage;
              ds_msgFinger.show_postion = p_ds_finger->enShowPostion;
              memcpy(ds_msgFinger.show_color, p_ds_finger->aucFontShowColor, 4);
              memcpy(ds_msgFinger.show_color_back, p_ds_finger->aucBackShowColor, 4);
              ds_msgFinger.show_postion_x = p_ds_finger->usShowXPos;
              ds_msgFinger.show_postion_y = p_ds_finger->usShowYPos;
              ds_msgFinger.show_front = p_ds_finger->ucShowFont;
              ds_msgFinger.show_front_size = p_ds_finger->ucShowFontSize;
              memcpy(ds_msgFinger.data, p_ds_finger->pucShowContent, 20);
              DESAI_DRV_DEBUG("[DS] show_postion: %d!\n",ds_msgFinger.show_postion);
              DESAI_DRV_DEBUG("[DS] show_color: %s!\n",ds_msgFinger.show_color);
              DESAI_DRV_DEBUG("[DS] show_back_color: %s!\n",ds_msgFinger.show_color_back);
              DESAI_DRV_DEBUG("[DS] show_postion_X: %d!\n",ds_msgFinger.show_postion_x);
              DESAI_DRV_DEBUG("[DS] show_postion_Y: %d!\n",ds_msgFinger.show_postion_y);
              DESAI_DRV_DEBUG("[DS] show_front: %d!\n",ds_msgFinger.show_front);
              DESAI_DRV_DEBUG("[DS] show_front_size: %d!\n",ds_msgFinger.show_front_size);
              DESAI_DRV_DEBUG("[DS] data: %s!\n",ds_msgFinger.data);
              event = CAS_C_SHOW_NEW_FINGER;
              break;
            case MESSAGE_FINGER_DISAPPEAR_TYPE: /*指纹隐藏 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 指纹隐藏!\n", __FUNCTION__, __LINE__);
              event = CAS_C_HIDE_NEW_FINGER;
              break;
           case MESSAGE_CARDNOTACTIVE_TYPE: /*智能卡未激活 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡未激活!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_SLEEP;
              break;

            case MESSAGE_CARDLOGOUT_TYPE: /*智能卡已注销 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡已注销!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_REG_FAILED;
              break;
           case MESSAGE_CONDITION_CTL_TYPE: /*该产品被限播 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 该产品被限播\n", __FUNCTION__, __LINE__);
              event = CAS_E_NO_AUTH_DATA;
              break;
            case MESSAGE_DESCRA_NOENTITLE_TYPE: /*授权已到期，请联系运营商 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 授权已到期，请联系运营商\n", __FUNCTION__, __LINE__);
              event = CAS_E_AUTHEN_EXPIRED;
              break;
            case MESSAGE_ENTITLE_UPDATE_TYPE: /*授权更新中，请稍等！ */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 授权更新中，请稍等！\n", __FUNCTION__, __LINE__);
              event = CAS_E_AUTHEN_UPDATE;
              break;
             case MESSAGE_CARD_UNWONTED_TYPE: /*智能卡内部异常 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 授权更新中，请稍等！\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_FAILED;
              break;
           case MESSAGE_MS_PAIR_OK_TYPE: /* 子母卡配对成功  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 子母卡配对成功!\n", __FUNCTION__, __LINE__);
              event = CAS_S_CARD_PARTNER_SUCCESS;
              break;

            case MESSAGE_MS_PAIR_FAILED_TYPE: /* 子母卡配对失败,请重新插入正确的母卡  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 子母卡配对失败,请重新插入正确的母卡 !\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_PARTNER_FAILED;
              break;
           case MESSAGE_MS_REPAIR_TYPE:/* 子卡需重新和母卡配对  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 子卡需重新和母卡配对!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_DIS_PARTNER;
              break;
            case MESSAGE_CARD_MATCH_OK_TYPE: /*机卡配对成功 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 机卡配对成功!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_MATCH_OK;
              break;
           case MESSAGE_CARD_REMATCH_TYPE: /*重新机卡配对 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 重新机卡配对!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_REMATCH;
              break;
          case MESSAGE_CANCEL_MATCH_TYPE: /*取消机卡配对 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 取消机卡配对!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_REMATCH_CANCEL;
              break;
           case MESSAGE_NEWEMAIL_TYPE:/* 显示新邮件图标  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 显示新邮件图标!\n", __FUNCTION__, __LINE__);
              event = CAS_C_SHOW_NEW_EMAIL;
              break;
            case MESSAGE_OVEREMAIL_TYPE: /* 显示邮件存满  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 显示邮件存满 \n", __FUNCTION__, __LINE__);
              event = CAS_C_EMAIL_FULL;
              break;
           case MESSAGE_HIDE_NEWEMAIL_TYPE:
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 隐藏新邮件图标!\n", __FUNCTION__, __LINE__);
              event = CAS_C_HIDE_NEW_EMAIL;
              break;

           case MESSAGE_EXIGENT_PROG_TYPE: /*紧急广播 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 紧急广播!\n", __FUNCTION__, __LINE__);
              p_exigent = (ST_CA_EXIGENT_PROGRAM *)pucMessage;
              memcpy(&ds_Exigent, p_exigent, sizeof(ST_CA_EXIGENT_PROGRAM));
              DESAI_DRV_DEBUG("[DS] usNetWorkID: %d!\n",ds_Exigent.usNetWorkID);
              DESAI_DRV_DEBUG("[DS] usServiceID: %d!\n",ds_Exigent.usServiceID);
              DESAI_DRV_DEBUG("[DS] usTransportStreamID: %d!\n",ds_Exigent.usTransportStreamID);
              event = CAS_C_FORCE_CHANNEL;
              break;
           case MESSAGE_EXIGENT_PROG_STOP_TYPE:/*停止紧急广播 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 停止紧急广播!\n", __FUNCTION__, __LINE__);
              event = CAS_C_UNFORCE_CHANNEL;
              break;
            case MESSAGE_CARD_UNLOCK_OK_TYPE: /*智能卡密码解锁成功   */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡密码解锁成功!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_UNLOCK_OK;
              break;
           case MESSAGE_PURSER_UPDATE_TYPE: /*电子钱包更新 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 电子钱包更新!\n", __FUNCTION__, __LINE__);
              event = CAS_C_PURSER_UPDATE;
              break;
           case MESSAGE_NET_ERR_LOCK_TYPE: /*网络异常，卡被锁定 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 网络异常，卡被锁定!\n", __FUNCTION__, __LINE__);
              event = CAS_C_PNET_ERR_LOCK;
              break;
           case MESSAGE_SYSTEM_ERR_LOCK_TYPE: /*系统异常，卡被锁定 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d,系统异常，卡被锁定!\n", __FUNCTION__, __LINE__);
              event = CAS_C_SYSTEM_ERR_LOCK;
              break;
           case MESSAGE_NET_ERR_UNLOCK_TYPE: /*网络异常解锁 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 电子钱包更新!\n", __FUNCTION__, __LINE__);
              event =  CAS_C_PNET_ERR_UNLOCK;
              break;
           case MESSAGE_SYSTEM_ERR_UNLOCK_TYPE: /*系统异常解锁 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 系统异常解锁!\n", __FUNCTION__, __LINE__);
              event = CAS_C_SYSTEM_ERR_UNLOCK;
              break;
           case MESSAGE_SHOWOSD_TYPE: /*显示OSD消息 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 显示OSD消息!\n", __FUNCTION__, __LINE__);
              p_ds_osd = (ST_OSD_SHOW_INFO *)pucMessage;
              ds_OsdMsg.display_pos = p_ds_osd->ucDisplayPos;
              ds_OsdMsg.display_front = p_ds_osd->ucFontType;
              ds_OsdMsg.display_front_size = p_ds_osd->ucFontSize;
              memcpy(ds_OsdMsg.display_back_color, p_ds_osd->aucBackgroundColor, 4);
              memcpy(ds_OsdMsg.display_color, p_ds_osd->aucDisplayColor, 4);
              ds_OsdMsg.content_len = p_ds_osd->usContentSize;
              ds_OsdMsg.data_len = p_ds_osd->usGetBufferSize;
             memset(ds_OsdMsg.data, 0, CAS_MAX_MSG_DATA_LEN);
             memcpy(ds_OsdMsg.data, p_ds_osd->pucDisplayBuffer, ds_OsdMsg.content_len);
             DESAI_DRV_DEBUG("[DS] display_pos: %d!\n",ds_OsdMsg.display_pos);
             DESAI_DRV_DEBUG("[DS] display_front: %d!\n",ds_OsdMsg.display_front);
             DESAI_DRV_DEBUG("[DS] display_front_size: %d!\n",ds_OsdMsg.display_front_size);
             DESAI_DRV_DEBUG("[DS] display_back_color: 0x%x-%x-%x-%x !\n",ds_OsdMsg.display_back_color[0],ds_OsdMsg.display_back_color[1],ds_OsdMsg.display_back_color[2],ds_OsdMsg.display_back_color[3]);
             DESAI_DRV_DEBUG("[DS] display_color: 0x%x-%x-%x-%x!\n",ds_OsdMsg.display_color[0],ds_OsdMsg.display_color[1],ds_OsdMsg.display_color[2],ds_OsdMsg.display_color[3]);
             DESAI_DRV_DEBUG("[DS] content_len: %d!\n",ds_OsdMsg.content_len);
             DESAI_DRV_DEBUG("[DS] data_len: %d!\n",ds_OsdMsg.data_len);
             DESAI_DRV_DEBUG("[DS] data: %s!\n",ds_OsdMsg.data);
              event = CAS_C_SHOW_OSD;
              break;
           case MESSAGE_HIDEOSD_TYPE: /*隐藏OSD消息 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 隐藏OSD消息!\n", __FUNCTION__, __LINE__);
              event = CAS_C_HIDE_OSD_UP;
              break;
            case MESSAGE_CARDISDUE_TYPE: /*智能卡处于欠费催缴 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡处于欠费催缴!\n", __FUNCTION__, __LINE__);
              event = CAS_E_CARD_AUTHEN_OVERDUE;
              break;
           case MESSAGE_IPPVBUY_TYPE: /*IPPV购买提示框  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, PPV购买提示框 !\n", __FUNCTION__, __LINE__);
              p_ippv = (ST_IPPV_INFO *)pucMessage;
              memcpy(&ds_ippv, p_ippv, sizeof(ST_IPPV_INFO));
              DESAI_DRV_DEBUG("[DS] ippv name: %s!\n",ds_ippv.aucIppvName);
              DESAI_DRV_DEBUG("[DS] ippv event id: 0x%x-%x-%x-%x!\n",ds_ippv.aucEventID[0], ds_ippv.aucEventID[1], ds_ippv.aucEventID[2], ds_ippv.aucEventID[3]);
              DESAI_DRV_DEBUG("[DS] ippv provid id: %d!\n",ds_ippv.ucProviderID);
              DESAI_DRV_DEBUG("[DS] ippv price: %d!\n",ds_ippv.usPrice);
              DESAI_DRV_DEBUG("[DS] ippv start time: %d-%d-%d-%d-%d-%d!\n",ds_ippv.stBeginTime.usYear, ds_ippv.stBeginTime.ucMonth, ds_ippv.stBeginTime.ucDay,
                                                                                                                         ds_ippv.stBeginTime.ucHour, ds_ippv.stBeginTime.ucHour, ds_ippv.stBeginTime.ucSecond);
              DESAI_DRV_DEBUG("[DS] ippv end time: %d-%d-%d-%d-%d-%d!\n",ds_ippv.stEndTime.usYear, ds_ippv.stEndTime.ucMonth, ds_ippv.stEndTime.ucDay,
                                                                                                                         ds_ippv.stEndTime.ucHour, ds_ippv.stEndTime.ucHour, ds_ippv.stEndTime.ucSecond);
              event = CAS_C_IPP_BUY_OR_NOT;
              break;

           case MESSAGE_IPPV_OUTTIME_TYPE: /*IPPV节目过期 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, IPPV节目过期!\n", __FUNCTION__, __LINE__);
              event = CAS_C_IPP_OVERDUE;
              break;
            case MESSAGE_HIDE_IPPVBUY_TYPE: /*隐藏 IPPV购买提示框  */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 隐藏 IPPV购买提示框 !\n", __FUNCTION__, __LINE__);
              event = CAS_C_HIDE_IPPV;
              break;
           case MESSAGE_CONDITION_SEARCH_TYPE: /*条件NIT表搜索节目 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 条件NIT表搜索节目!\n", __FUNCTION__, __LINE__);
              event = CAS_C_REQU_NIT_SECITON;
              break;
           case MESSAGE_CARD_UPDATE_BEGIN_TYPE: /*智能卡开始更新，显示进度条 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡开始更新，显示进度条!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_UPDATE_BEGIN;
              break;
           case MESSAGE_CARD_UPDATE_PROGRESS_TYPE: /*更新进度 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 更新进度!\n", __FUNCTION__, __LINE__);
              mtos_task_delay_ms(50);
              event = CAS_C_CARD_UPDATE_PROGRESS;
              break;
           case MESSAGE_CARD_UPDATE_ERR_TYPE: /*更新出错，错误代码 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 更新出错，错误代码!\n", __FUNCTION__, __LINE__);
             // mtos_printk("[DS]%s:LINE:%d, 更新出错，错误代码!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_UPDATE_ERR;
              break;
           case MESSAGE_CARD_UPDATE_END_TYPE: /*更新成功，进度条消失 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 更新成功，进度条消失!\n", __FUNCTION__, __LINE__);
              mtos_printk("[DS]%s:LINE:%d, 更新成功，进度条消失!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_UPDATE_END;
              break;
            case MESSAGE_CARD_NEED_UPDATE_TYPE: /*智能卡需要更新，请稍后 */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 条件NIT表搜索节目!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_NEED_UPDATE;
              break;
            case MESSAGE_CHANGEPRO_TOO_FAST_TYPE: /*请不要频繁切台   */
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 条件NIT表搜索节目!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_CHANGEPRO_TOO_FAST;
              break;
            case MESSAGE_CARD_ADDR_CTL_OPEN_TYPE:
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡区域控制打开!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_ADDR_CTL_OPEN;
              break;
            case MESSAGE_CARD_ADDR_CTL_CLOSE_TYPE:
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 智能卡区域控制关闭!\n", __FUNCTION__, __LINE__);
              event = CAS_C_CARD_ADDR_CTL_CLOSE;
              break;
            default:
              DESAI_DRV_DEBUG("[DS]%s:LINE:%d, 当前库消息 ucMessageType = 0x%x !\n", __FUNCTION__, __LINE__,ucMessageType);
              break;
      }

      DSCAS_SendEvent(p_priv, event, pucMessage);
  }



/******************7:program interface**************************************/
 UINT8 DSCA_QueryProgram(UINT16 usNetWorkID, UINT16 usTransportStreamID, UINT16 usServiceID)
{
  RET_CODE ret = 0;

  s_cas_ds_priv_t *p_cas_ds_priv = NULL;
  p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

  ret = p_cas_ds_priv->query_check(usNetWorkID, usTransportStreamID, usServiceID);

  if(ret == SUCCESS)
      return DS_PROGRAM_EXIST;
  else
    return DS_NOPROGRAM;

}



/******************8:program interface**************************************/
 typedef char    *ck_va_list;
 typedef unsigned int  CK_NATIVE_INT;
 typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
 extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);


 void DSCA_printf(OUT const INT8 *string,...)
{
  DESAI_DRV_DEBUG("#######desai debug ######\n");

    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, string);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)string, p_args);

    CK_VA_END(p_args);
    DESAI_DRV_DEBUG(printk_buf);
    DESAI_DRV_DEBUG("\n");
}

#if 1
 INT32 DSCA_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...)
{
    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;

    CK_VA_START(p_args, pucFormatBuffer);
    printed_len = ck_vsnprintf((char *)pucDestBuffer, 256, (char *)pucFormatBuffer, p_args);
    CK_VA_END(p_args);

//   DESAI_DRV_DEBUG("****pucDestBuffer: %s\n!", pucDestBuffer);
   return printed_len;
}
#endif


/******************9:new add**************************************/

static void ds_nit_parse_descriptor(UINT8* pucDataBuffer, UINT32 uiLen)
{
  UINT8* pucdata = pucDataBuffer;
  UINT8 ucDescriptor_tag = 0;
  UINT8 ucDescriptor_length = 0;
  UINT32 uiLoop =  uiLen;
  UINT8 aucAddrCode[7];
  UINT8 temp = 0;

  s_cas_ds_priv_t *p_priv = NULL;
  UINT8 * pucMessage = NULL;
  p_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;

  while (uiLoop > 0)
  {
    ucDescriptor_tag = pucdata[0];
    ucDescriptor_length = pucdata[1];
    pucdata += 1;
    uiLoop -= 1;
    if (ucDescriptor_tag == 0xa5 && ucDescriptor_length == 7)
    {
        memcpy(aucAddrCode,  & pucdata [1],  ucDescriptor_length);
        DESAI_DRV_DEBUG("[DS]区域设置:");
        for(temp = 0 ; temp < 7; temp++)
          DESAI_DRV_DEBUG("0x%x-",aucAddrCode[temp]);
        DESAI_DRV_DEBUG("\n");
        DSCA_SetCardAddressCode(aucAddrCode);
        break;
     }
  }
  DSCAS_SendEvent(p_priv, CAS_S_ZONE_CHECK_END, pucMessage);
}


 RET_CODE DS_STB_Drv_FreeAllEcm(void)
 {
     RET_CODE ret = SUCCESS;
     u8 uIndex = 0;

     DESAI_DRV_DEBUG("[DS]DS_STB_Drv_FreeAllEcm\n");

     for (uIndex = 0; uIndex < DS_FILTER_MAX_NUM; uIndex++)
     {
         DS_STB_Drv_CaReqLock(&ca_filter_lock[uIndex]);
         if (DS_FILTER_STATUS_BUSY == ds_filter[uIndex].filter_status
                 && ECM_DATA_ID == ds_filter[uIndex].req_id)
         {

            // DSCA_GetPrivateData(ds_filter[uIndex].pid, ds_filter[uIndex].req_id, NULL, 0);
             ret = ds_filter_free(&ds_filter[uIndex]);
             if (SUCCESS != ret)
             {
                 DESAI_DRV_DEBUG("[DS]error_code = 0x%x !!!!\n", ret);
             }
             else
             {
                 memset((void *)&ds_filter[uIndex], 0, sizeof(s_ds_filter_struct_t));
             }
         }
        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
     }

     return SUCCESS;
 }


 static void DS_STB_Drv_SendDataToCA(u8 uDataNum)
 {
     u8 uIndex = uDataNum;

     if (uDataNum > DS_FILTER_MAX_NUM)
     {
         DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Too much data, uDataNum = %d!!!!\n", __FUNCTION__, __LINE__, uDataNum);
         uDataNum = DS_FILTER_MAX_NUM;
     }

//     for (uIndex = 0; uIndex < uDataNum; uIndex ++)
     {
         DESAI_DRV_DEBUG("[DS]%s:LINE:%d, =====Send data to CA: uIndex=%d,ds_data[uIndex].req_id=%d,ds_data[uIndex].pid=0x%x ds_data[uIndex].p_data[0]=0x%x \n",
                 __FUNCTION__, __LINE__, uIndex, ds_data[uIndex].req_id, ds_data[uIndex].pid, ds_data[uIndex].p_data[0]);

         DSCA_GetPrivateData(ds_data[uIndex].pid,ds_data[uIndex].req_id, ds_data[uIndex].p_data, ds_data[uIndex].length);
         if (NULL != ds_data[uIndex].p_data)
         {
             ds_data[uIndex].p_data = NULL;
         }
         memset(&ds_data[uIndex], 0, sizeof(s_ds_data_got_t));
     }
 }


 static void DS_STB_Drv_CopyData(u8 req_id, u8 u_ok, u16 pid, u8 *p_data, u16 length, u8 uIndex)
 {
     if (uIndex >= DS_FILTER_MAX_NUM)
     {
         DESAI_DRV_DEBUG("[DS]%s:LINE:%d, Too much data, uIndex = %d!!!!\n", __FUNCTION__, __LINE__, uIndex);
         return;
     }

     DESAI_DRV_DEBUG("[DS]%s:LINE:%d, req_id = %d, table_id = 0x%x\n", __FUNCTION__, __LINE__, req_id, p_data[0]);
     ds_data[uIndex].req_id = req_id;
     ds_data[uIndex].u_ok = u_ok;
     ds_data[uIndex].pid = pid;

     if ((NULL == p_data) || (0 == length))
     {
         ds_data[uIndex].p_data = NULL;
     }
     else
     {
         memset(ds_data_buf[uIndex],0,DS_DATA_BUFFER_SIZE);   // need improved
         ds_data[uIndex].p_data = ds_data_buf[uIndex];
         memcpy(ds_data[uIndex].p_data, p_data, length);
     }
     ds_data[uIndex].length = length;

   //  DS_STB_Drv_SendDataToCA(uIndex);
 }


 void DS_STB_Drv_CaDataMonitor(void *pData)
{
    u8 aFilterIndex[DS_FILTER_MAX_NUM] = {0};
    u8 uValidFilterNum = 0;
    u8 uIndex = 0;
    s32 nRet = SUCCESS;
    u32 uDataLen = 0;
    u8 uDataNum = 0;
    u32 ecm_time = 0;

     s_cas_ds_priv_t *p_priv = NULL;
     UINT8 * pucMessage = NULL;
     p_priv = (s_cas_ds_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;


   // s_ds_filter_struct_t *p_s_ds_filter_struct = NULL;
    u8 *p_GotData = NULL;
    dmx_device_t *p_dev = NULL;

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    while (1)
    {
        uValidFilterNum = 0;
        memset(aFilterIndex, 0, DS_FILTER_MAX_NUM);

        /* find valid filter */
        for (uIndex = 0; uIndex < DS_FILTER_MAX_NUM; uIndex++)
        {
            DS_STB_Drv_CaReqLock(&ca_filter_lock[uIndex]);
            if (DS_FILTER_STATUS_BUSY == ds_filter[uIndex].filter_status)
            {
                aFilterIndex[uValidFilterNum] = uIndex;
                uValidFilterNum ++;
            }
            DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
        }

        /* no valid filter */
        if (0 == uValidFilterNum)
        {
            mtos_task_sleep(20);
            continue;
        }

        if(ecm_flag == 1)
        {
           if (mtos_ticks_get() - ecm_time> 300)
           {
              DESAI_DRV_DEBUG("[DS]*** ecm data time out!!!!!\n");
              OS_PRINTF("[DS]*** ecm data time out!!!!!\n");
           //   DSCA_GetPrivateData(ecm_uspid, 1,NULL, 0);
          //    DESAI_DRV_DEBUG("[DS]*** req = 1, ecm_uspid = 0x%x \n", ecm_uspid);
              DSCA_SetEcmPid(ds_ecm_pid);
           }
        }

        /* read data */
        uDataNum = 0;
        for (uIndex = 0; uIndex < uValidFilterNum; uIndex++)
        {
            DS_STB_Drv_CaReqLock(&ca_filter_lock[uIndex]);
            if (DS_FILTER_STATUS_BUSY == ds_filter[aFilterIndex[uIndex]].filter_status)
            {
                nRet = dmx_si_chan_get_data(p_dev, ds_filter[aFilterIndex[uIndex]].req_handle,
                            &p_GotData, &uDataLen);

                if (nRet != SUCCESS)        /* no data and time out */
                {

                    if (mtos_ticks_get() - ds_filter[aFilterIndex[uIndex]].start_ms
                            > ds_filter[aFilterIndex[uIndex]].req_timeout)

                    {
                        DESAI_DRV_DEBUG("[DS]#####data time out!!!!!\n");
                        DS_STB_Drv_CopyData(ds_filter[ aFilterIndex[uIndex] ].req_id, FALSE,
                                        ds_filter[ aFilterIndex[uIndex] ].pid, NULL, 0, uDataNum);
                        uDataNum++;
                        DESAI_DRV_DEBUG("****data time out: %d\n", ds_filter[aFilterIndex[uIndex]].req_id);
                        if((NIT_TABLE_ID == ds_filter[aFilterIndex[uIndex]].req_id))
                        {
                             nRet = ds_filter_free(&ds_filter[aFilterIndex[uIndex]]);
                             if (SUCCESS != nRet)
                            {
                                 DESAI_DRV_DEBUG("[DS]filter free error,error_code = 0x%08x!!!! \n", nRet);
                            }
                            else
                            {
                                 memset((void *)&ds_filter[aFilterIndex[uIndex]], 0, sizeof(s_ds_filter_struct_t));
                                 DESAI_DRV_DEBUG("[DS]ecm filter ree \n");
                             }
                            DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
                            DSCAS_SendEvent(p_priv, CAS_S_ZONE_CHECK_END, pucMessage);
                            continue;
                        }
                        else if(ECM_DATA_ID == ds_filter[aFilterIndex[uIndex]].req_id)
                        {
                          nRet = ds_filter_free(&ds_filter[aFilterIndex[uIndex]]);
                          if (SUCCESS != nRet)
                          {
                              DESAI_DRV_DEBUG("[DS]filter free error,error_code = 0x%08x !!!!\n",nRet);
                          }
                          else
                          {
                              memset((void *)&ds_filter[aFilterIndex[uIndex]], 0, sizeof(s_ds_filter_struct_t));
                          }
                        }
                       DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
                       DS_STB_Drv_SendDataToCA(uDataNum -1);
                    }
                    else
                    {
                       DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
                    }
                }
                else                /* get data */
                {

                    DESAI_DRV_DEBUG("[DS]CA get data  = 0x%x  len=%d\n", p_GotData[0] , uDataLen);
                    DESAI_DRV_DEBUG("****get data: %d\n", ds_filter[aFilterIndex[uIndex]].req_id);
                    if((NIT_TABLE_ID == ds_filter[aFilterIndex[uIndex]].req_id))
                    {
                        ds_nit_parse_descriptor(p_GotData, uDataLen);
                        nRet = ds_filter_free(&ds_filter[aFilterIndex[uIndex]]);
                        if (SUCCESS != nRet)
                        {
                            DESAI_DRV_DEBUG("[DS]filter free error,error_code = 0x%08x!!!! \n", nRet);
                        }
                        else
                        {
                            memset((void *)&ds_filter[aFilterIndex[uIndex]], 0, sizeof(s_ds_filter_struct_t));
                            DESAI_DRV_DEBUG("[DS]ecm filter ree \n");
                        }
                        DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
                        continue;
                    }
                    else if ((ECM_DATA_ID == ds_filter[aFilterIndex[uIndex]].req_id))
                    {
                        if (SC_INSERT_OVER == s_ds_uScStatus)
                        {
                            DS_STB_Drv_CopyData(ds_filter[aFilterIndex[uIndex]].req_id, TRUE,
                                        ds_filter[aFilterIndex[uIndex]].pid, p_GotData, uDataLen, uDataNum);
                            uDataNum++;
                        }
                        else
                        {
                            DS_STB_Drv_CopyData(ds_filter[aFilterIndex[uIndex]].req_id, FALSE,
                                        ds_filter[aFilterIndex[uIndex]].pid, NULL, 0, uDataNum);
                            uDataNum++;
                        }
                    }
                    else
                    {
                        DS_STB_Drv_CopyData(ds_filter[aFilterIndex[uIndex]].req_id, TRUE,
                                        ds_filter[aFilterIndex[uIndex]].pid, p_GotData, uDataLen, uDataNum);
                        uDataNum++;
                    }

                    if (ECM_DATA_ID == ds_filter[aFilterIndex[uIndex]].req_id)
                    {
                        ecm_flag= 1;
                        ecm_time = mtos_ticks_get();
                        nRet = ds_filter_free(&ds_filter[aFilterIndex[uIndex]]);
                        if (SUCCESS != nRet)
                        {
                            DESAI_DRV_DEBUG("[DS]filter free error,error_code = 0x%08x!!!! \n", nRet);
                        }
                        else
                        {
                            memset((void *)&ds_filter[aFilterIndex[uIndex]], 0, sizeof(s_ds_filter_struct_t));
                            DESAI_DRV_DEBUG("[DS]ecm filter ree \n");
                        }
                    }
                   DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
                   DS_STB_Drv_SendDataToCA(uDataNum -1);
                }
            }
            else
            {
              DS_STB_Drv_CaReqUnLock(&ca_filter_lock[uIndex]);
            }
        }
//        DS_STB_Drv_SendDataToCA(uDataNum);
        mtos_task_sleep(10);
    }
}


RET_CODE DS_STB_Drv_AdtInit(void)
{
    u32 *p_stack = NULL;
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;
    BOOL nRet = 0;
    u8 task_name[] = "DSFilterData_Monitor";
    u8 i = 0;

    p_cas_ds_priv = g_cas_priv.cam_op[CAS_ID_DS].p_priv;

    DESAI_DRV_DEBUG("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    for(i = 0; i < 10; i++)
     MT_ASSERT(TRUE == mtos_sem_create(&ca_filter_lock[i], TRUE));
    MT_ASSERT(TRUE == mtos_sem_create(&ca_osd_lock, TRUE));
    MT_ASSERT(TRUE == mtos_sem_create(&ca_finger_lock, TRUE));

    p_stack = (u32 *)mtos_malloc(DSCA_SECTION_TASK_STACK_SIZE);
    MT_ASSERT(NULL != p_stack);

    if(p_cas_ds_priv->task_prio_monitor == 0)
        nRet = mtos_task_create(task_name,
                  DS_STB_Drv_CaDataMonitor,
                  (void *)0,
                  DS_CAS_SECTION_TASK_PRIO,
                  p_stack,
                  DSCA_SECTION_TASK_STACK_SIZE);
   else
	nRet = mtos_task_create(task_name,
                  DS_STB_Drv_CaDataMonitor,
                  (void *)0,
                  p_cas_ds_priv->task_prio_monitor,
                  p_stack,
                  DSCA_SECTION_TASK_STACK_SIZE);

    if (nRet == FALSE)
    {
        DESAI_DRV_DEBUG("[DS]Create data monitor task error = 0x%08x!\n", nRet);
        return ERR_FAILURE;
    }

    DESAI_DRV_DEBUG("****DS_STB_Drv_AdtInit ok!\n");
    return SUCCESS;

}


