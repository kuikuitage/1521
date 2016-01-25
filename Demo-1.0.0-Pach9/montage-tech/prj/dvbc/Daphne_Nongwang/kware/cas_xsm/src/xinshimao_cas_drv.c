/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "string.h"
#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"
#include "stdio.h"
#include "stdlib.h"

#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_msg.h"


#include "drv_dev.h"
#include "charsto.h"
#include "drv_svc.h"
#include "dmx.h"

#include "lib_bitops.h"


#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"
#include "nim.h"
#include "smc_op.h"


#include "xinshimao_ca.h"
#include "xinshimao_cas_include.h"


////////////////////////////////////////////////////////////////////////////////////////

//#define CAS_XSM_DEBUG

#ifdef CAS_XSM_DEBUG
#define CAS_XSM_PRINTF OS_PRINTF
#else
#define CAS_XSM_PRINTF DUMMY_PRINTF
#endif

#define _MSG_SIZE() \
   (UINT32)sizeof(xsm_queue_t);

static s_xsm_task_struct_t s_task_record[XSM_CAS_TASK_NUM] =
{
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_0},
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_1},
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_2},
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_3},
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_4},
  {XSM_FREE, 0, XSM_CAS_TASK_PRIO_5}
};

u8 g_data_temp[4096] = {0};

static xsm_queue_t       xsm_queue_array[XSM_CAS_QUEUE_MAX_NUM] = {{0,0,{0,}},};
static xsm_filter_struct_t s_tFilter[XSM_FILTER_MAX_NUM] = {{0,},};
static xsm_data_got_t  s_tData[XSM_FILTER_MAX_NUM] = {{0,},};
static UINT8 s_tFilterBuf[XSM_FILTER_MAX_NUM][XSM_FILTER_BUFFER_SIZE] = {{0,},};
static UINT32 s_ca_lock = 0;
static UINT8 s_private_data[XSM_FILTER_MAX_NUM][XSM_PRIVATE_DATA_LEN_MAX] = {{0,},};
static UINT16 v_channel = 0xffff;
static UINT16 a_channel = 0xffff;
static UINT16 old_v_channel = 0xffff;
static UINT16 old_a_channel = 0xffff;


extern int dmx_jazz_wait_for_data(int timeout);
extern int dmx_jazz_wait_for_data_stop(void);
extern cas_adapter_priv_t g_cas_priv;
extern BOOL s_xsm_entitle_flag; //0:entitle  1:no entitle



typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);
extern void set_event(cas_xsm_priv_t *p_priv, u32 event);
extern UINT8 cas_xsm_get_scramble_flag(void);

/*!
    lock
*/
static void xsm_ca_lock(void)
{
    //CAS_XSM_PRINTF("[xsm],xsm_ca_lock:%d \n",s_ca_lock);
    mtos_sem_take((os_sem_t *)&s_ca_lock, 0);
}

/*!
    unlock
*/
static void xsm_ca_unlock(void)
{
    //CAS_XSM_PRINTF("[xsm],xsm_ca_unlock:%d \n",s_ca_lock);
    mtos_sem_give((os_sem_t *)&s_ca_lock);
}


static void xsm_filter_free(xsm_filter_struct_t *s_tFilter)
{
    CAS_XSM_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    dmx_device_t *p_dev = NULL;
    u32 ret = 0;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, s_tFilter->m_dwReqHandle);
    MT_ASSERT(ret == CAS_SUCCESS);

    ret = dmx_chan_close(p_dev, s_tFilter->m_dwReqHandle);
    MT_ASSERT(ret == CAS_SUCCESS);

}

/*!
  free all ecm request filter 
  
  */
void xsm_cas_all_ecm_req_free()
{
     UINT8 index = 0;

     CAS_XSM_PRINTF("[xsm],enter xsm_cas_all_ecm_req_free \n");
     xsm_ca_lock();
     for(index = 0; index < XSM_FILTER_MAX_NUM; index++)
     {
          if(XSM_USED == s_tFilter[index].m_FilterStatus &&
              CA_STB_FILTER_4 == s_tFilter[index].m_xsmReqId)
          {
             xsm_filter_free(&s_tFilter[index]);
             XinShiMao_TableReceived(s_tFilter[index].m_xsmReqId,FALSE,s_tFilter[index].m_PID,NULL,0);
             memset((void *)&s_tFilter[index], 0, sizeof(xsm_filter_struct_t));
          }
     }
     xsm_ca_unlock();
     CAS_XSM_PRINTF("[xsm],leave xsm_cas_all_ecm_req_free \n");

     return ;
}

/*!
  free all emm request filter 
  
  */
void xsm_cas_all_emm_req_free()
{
     UINT8 index = 0;

     CAS_XSM_PRINTF("[xsm],enter xsm_cas_all_emm_req_free \n");
     xsm_ca_lock();
     for(index = 0; index < XSM_FILTER_MAX_NUM; index++)
     {
          if(XSM_USED == s_tFilter[index].m_FilterStatus &&
              (CA_STB_FILTER_1 == s_tFilter[index].m_xsmReqId ||
                CA_STB_FILTER_2 == s_tFilter[index].m_xsmReqId ||
                CA_STB_FILTER_3 == s_tFilter[index].m_xsmReqId))
         {
           xsm_filter_free(&s_tFilter[index]);
           XinShiMao_TableReceived(s_tFilter[index].m_xsmReqId,FALSE,s_tFilter[index].m_PID,NULL,0);
           memset((void *)&s_tFilter[index], 0, sizeof(xsm_filter_struct_t));
         }
     }
     xsm_ca_unlock();
     CAS_XSM_PRINTF("[xsm],leave xsm_cas_all_emm_req_free \n");

     return ;
}
  /*!
    ABC
  */
void xsm_add_data(UINT8 req_id, UINT8 u_ok, UINT16 pid, 
                                UINT8 *p_data, UINT16 length,UINT8 uIndex)
{
  if(uIndex >= XSM_FILTER_MAX_NUM)
  {
    CAS_XSM_PRINTF("[xsm],xsm_add_data: Too much data, uIndex = %d\n", uIndex);
    return;
  }

  s_tData[uIndex].req_id = req_id;
  s_tData[uIndex].u_ok = u_ok;
  s_tData[uIndex].pid = pid;
  if(NULL == p_data || 0 == length)
  {
    s_tData[uIndex].p_data = NULL;
  }
  else
  {
     if(length > XSM_PRIVATE_DATA_LEN_MAX)
     {
        //CAS_XSM_PRINTF("-----------length > 256-------\n");
        s_tData[uIndex].p_data = mtos_malloc(length);
        MT_ASSERT(s_tData[uIndex].p_data != NULL);
        memcpy(s_tData[uIndex].p_data, p_data, length);
     }
     else
     {
        //CAS_XSM_PRINTF("[xsm] length = %d \n",length);
        memset(s_private_data[uIndex],0,XSM_PRIVATE_DATA_LEN_MAX);
        s_tData[uIndex].p_data = s_private_data[uIndex];
        memcpy(s_tData[uIndex].p_data, p_data, length);
     }
  }
  s_tData[uIndex].length = length;
  
}

  /*!
    ABC
  */
void xsm_send_all_data(UINT8 uDataNum)
{
  u8 uIndex = 0;

  if(uDataNum > XSM_FILTER_MAX_NUM)
  {
    CAS_XSM_PRINTF("[xsm],gt_send_all_data: Too much data, uDataNum = %d\n", uDataNum);
    uDataNum = XSM_FILTER_MAX_NUM;
  }

  //CAS_XSM_PRINTF("Call xsm_send_all_data and uDataNum = %d\n",uDataNum);
  
  for(uIndex = 0; uIndex < uDataNum; uIndex++)
  {
      CAS_XSM_PRINTF("[xsm] send  %d %x %x \n", s_tData[uIndex].req_id, s_tData[uIndex].pid,
                s_tData[uIndex].p_data[0]);

    XinShiMao_TableReceived(s_tData[uIndex].req_id, s_tData[uIndex].u_ok, 
                                               s_tData[uIndex].pid,s_tData[uIndex].p_data,
                                               s_tData[uIndex].length);

    if(NULL != s_tData[uIndex].p_data && s_tData[uIndex].length > XSM_PRIVATE_DATA_LEN_MAX)
    {
      mtos_free(s_tData[uIndex].p_data);
      s_tData[uIndex].p_data = NULL;
    }

    memset(&s_tData[uIndex], 0, sizeof(xsm_data_got_t));
  }
}

extern UINT8 XinShiMao_GetEntitleRemainDay(void);

static void xsm_overdue_data_monitor(void *p_data)
{
    UINT8 RemainDay = 0;
    UINT32 event = 0;
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
 
    while(1)
    {
      mtos_task_sleep(1*60*1000);//30*min

      if(XSM_CA_CARD_INSERT == xsm_get_card_status())
      {
        RemainDay = XinShiMao_GetEntitleRemainDay();
        OS_PRINTF("\n=================RemainDay:%d===============================\n",RemainDay);

        if(RemainDay <= 15)
        {
           event = CAS_E_CARD_OVERDUE;
           cas_send_event(p_priv->slot, CAS_ID_XSM, event, RemainDay);
        }      
      }       
    }
}

  /*!
    ABC
  */
static void xsm_ca_data_monitor(void *p_data)
{
    UINT32 err = 0;
    UINT8 index = 0;
    UINT8 filter_index[XSM_FILTER_MAX_NUM] = {0,};
    UINT8 valid_filter_num = 0;
    UINT8 data_index = 0;
    UINT16 network_id = 0;
    INT32 region_id = -1;
    UINT32 region_event = 0;
    UINT8 *p_got_data = NULL;
    UINT32 got_data_len = 0;
    dmx_device_t *p_dev = NULL;
    
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    while(1)
    {
       //ECM && EMM
       valid_filter_num = 0;
       memset(filter_index,0,XSM_FILTER_MAX_NUM);

       //find free filter
       xsm_ca_lock();
       for(index = 0; index < XSM_FILTER_MAX_NUM; index++)
       {
            if(XSM_USED == s_tFilter[index].m_FilterStatus)
            {
                 filter_index[valid_filter_num] = index;
                 valid_filter_num++;
            }
       }
       xsm_ca_unlock();

       //valid filter doesn't exist
       if(0 == valid_filter_num)
       {
            mtos_task_sleep(20);
            continue;
       }

       //read data
       xsm_ca_lock();
       data_index = 0;
       for(index = 0; index < valid_filter_num; index++)
       {
            err = dmx_si_chan_get_data(p_dev, s_tFilter[filter_index[index]].m_dwReqHandle,
                                                            &p_got_data, &got_data_len);
            
            if(0 != err)
            {

                 if(mtos_ticks_get() - s_tFilter[filter_index[index]].m_dwStartMS
                     > s_tFilter[filter_index[index]].m_timeout)
                 {

                      if(s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_4)
                      {
                         CAS_XSM_PRINTF("\n [xsm], ecm timeout free ecm  filter \n");

                         xsm_filter_free(&s_tFilter[filter_index[index]]);
                      }
                      
                      CAS_XSM_PRINTF("[xsm], get data timeout ! \n");
                      xsm_add_data(s_tFilter[filter_index[index]].m_xsmReqId, TRUE,
                                            s_tFilter[filter_index[index]].m_PID, p_got_data, 
                                            got_data_len, data_index);
                      data_index++;


                      if(s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_4)
                      {
                          memset((void *)&s_tFilter[filter_index[index]], 0, 
                                                                 sizeof(xsm_filter_struct_t));
                      }
                 }
            }
            else
            {              
                 if(s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_4)
                 {
                      //ecm data
                      CAS_XSM_PRINTF("\n\n --------------[xsm],Ecm data -------------------\n");

                      xsm_filter_free(&s_tFilter[filter_index[index]]);
                      
                      if(XSM_CA_CARD_INSERT == xsm_get_card_status())
                      {
                           xsm_add_data(s_tFilter[filter_index[index]].m_xsmReqId, TRUE,
                                              s_tFilter[filter_index[index]].m_PID, p_got_data,
                                              got_data_len, data_index);
                           data_index++;
                      }
                      else
                      {
                           xsm_add_data(s_tFilter[filter_index[index]].m_xsmReqId, FALSE,
                                                 s_tFilter[filter_index[index]].m_PID, 
                                                 NULL, 0, data_index);
                           data_index++;
                      }

                 }
                 else if(s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_1
                            || s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_2
                            || s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_3)
                 {
                      //emm data
                      CAS_XSM_PRINTF("\n\n --------------[xsm],Emm data -------------------\n");
                      
                      xsm_add_data(s_tFilter[filter_index[index]].m_xsmReqId, TRUE,
                                            s_tFilter[filter_index[index]].m_PID,  p_got_data, 
                                            got_data_len, data_index);
                      data_index++;
                 }
                 else if(s_tFilter[filter_index[index]].m_xsmReqId == NIT_TABLE_ID)
                 {
                      network_id = p_got_data[3];
                      network_id = (network_id << 8) | p_got_data[4];
                      CAS_XSM_PRINTF("[xsm],table_id=0x%x, ",p_got_data[0]);
                      memset((void *)&s_tFilter[filter_index[index]], 0, 
                                                             sizeof(xsm_filter_struct_t));
                      
                      cas_xsm_set_network_id(network_id);
                      region_id = XinShiMao_GetCardRegionID();
                      CAS_XSM_PRINTF("network_id=0x%x,region_id=0x%x \n",network_id,region_id);
                      if(network_id == region_id)
                      {
                           region_event = CAS_C_CARD_ZONE_CHECK_OK;
                      }
                      else
                      {
                           region_event = CAS_C_CARD_ZONE_INVALID;
                           CAS_XSM_PRINTF("-------for test fix region_event--------\n");
                      }
                      cas_xsm_zone_check_end(region_event);
                      
                      xsm_ca_unlock();
                      mtos_task_sleep(10);
                      continue;
                 }
                 else
                 {
                      CAS_XSM_PRINTF("\n [xsm],request filter isn't emm/ecm/nit !!! \n");
                      memset((void *)&s_tFilter[filter_index[index]], 0, 
                                                             sizeof(xsm_filter_struct_t));
                      break;
                 }

                 if(s_tFilter[filter_index[index]].m_xsmReqId == CA_STB_FILTER_4)
                 {
                       //free emm data filter
                      CAS_XSM_PRINTF("\n [xsm],free ecm filter \n");

                      //xsm_filter_free(&s_tFilter[filter_index[index]]);
                      memset((void *)&s_tFilter[filter_index[index]], 
                                                     0, sizeof(xsm_filter_struct_t));
                 }
              
            }
       }
       xsm_ca_unlock();

       //send data
       xsm_send_all_data(data_index);
       mtos_task_sleep(10);
    }

}





/*!
  get free queue
  
  \param[in] queue_id.
  \return p_message ,message pointer.
  */
xsm_queue_t *xsm_free_queue_get(UINT32 queue_id)
{
    UINT32 index = 0;
    xsm_queue_t *p_message = NULL;

    for(index = 0; index < XSM_CAS_QUEUE_MAX_NUM; index++)
    {
         if(XSM_FREE == xsm_queue_array[index].queue_status)
         {
              xsm_queue_array[index].queue_id = queue_id;
              xsm_queue_array[index].queue_status = XSM_USED;
              p_message = &xsm_queue_array[index];
              break;
         }
    }

    CAS_XSM_PRINTF("[xsm], index=%d, message addr=0x%x \n",index, (UINT32)p_message);
    return p_message;
}
void XinShiMao_memcpy(void* pDestBuf,const void* pSrcBuf,UINT32  wSize)
{
  CAS_XSM_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
  memcpy(pDestBuf, pSrcBuf, wSize);
}


void XinShiMao_memset(void* pDestBuf,UINT8 c,UINT32 wSize)
{
  CAS_XSM_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
  memset(pDestBuf, c, wSize);
}

INT32  XinShiMao_memcmp(const void *buf1, const void *buf2, UINT32 count)
{
  CAS_XSM_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
  return memcmp(buf1, buf2, count);
}

msg_info_t xsm_OsdMsg = {0, {0,},};
/*提 供 者： 机顶盒

输入参数： event 为消息内容1: 为OSD消息；4为邮件消息
				2,3 大画面的消息提示:param1 为消息提示的内容 */
void  XinShiMao_EventHandle(UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3)
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_PRINTF("XinShiMao_EventHandle event = %d, param1 = %d, param2 = %d, param3 = %d\n",
              event,param1,param2,param3);
    
    if(1 == event)//OSD
    {        //这是滚动消息
      OS_PRINTF(" message_len=%d \n",param2);
      OS_PRINTF("[xsm]osd message:%s \n ",(char *)param1);
      OS_PRINTF(" param3=0x%x \n",param3);

      memcpy(xsm_OsdMsg.data,(UINT8 *)param1,param2);


      OS_PRINTF("[xsm] OsdMsg.data:%s \n\n ",xsm_OsdMsg.data);

      /*OsdMsg.osd_display.roll_value = (UINT8)(param3 & 0x0f);
      OsdMsg.display_pos = (UINT8)(param3 >> 4);

      CAS_XSM_PRINTF("[gt] OsdMsg.display_pos:%d \n ",OsdMsg.display_pos);
      CAS_XSM_PRINTF("[gt] OsdMsg.osd_display.roll_value:%d \n ",
                             OsdMsg.osd_display.roll_value);*/

      set_event(p_priv, CAS_C_SHOW_OSD);
      return ;
    }
    else if(4 == event)//EMAIL
    {         //新邮件
       OS_PRINTF("[xsm],email_id:%d \n",param1);
       set_event(p_priv, CAS_C_SHOW_NEW_EMAIL);

      return;
    }

    switch(param1)
  	{
  		case  SC_NORMAL:/*正常状态*/ 
            event = CAS_S_CLEAR_DISPLAY;
            break;
  		case  SC_NO_CARD: /*未插卡*/
            event = CAS_S_CARD_REMOVE;
            break;
  		case  SC_NO_PPV_RIGHT:/*没有PPV授权*/
            //event = CAS_S_CARD_REMOVE;
            break;
  		case  SC_NO_PPC_RIGHT: /*没有PPC授权*/
            event = CAS_C_ENTITLE_CLOSED;
            break;
  		case  SC_PARENT_CTRL : /*家长控制*/
            break;
  		case  SC_NO_CONDITION :/*条件限播*/
            break;
  		case  SC_INVALID_CARD :   /*无效卡*/
            event = CAS_E_ILLEGAL_CARD;
            break;
  		case  SC_TYPEERROR: /*子母卡喂养失败,插入智能卡类型错误*/
            break;
  		case  SC_ZONEERROR :/*区域错误*/
            break;
  		case  SC_FEEDTIMEERROR: /*喂养时间错误*/
            break;
  		case  SC_FEEDDATEERROR: /*喂养日期错误*/
            break;
  		case  SC_FEEDSYSTEMTIMEERROR: /*系统时间没有正确获取,喂养失败*/
            break;
  		case  SC_ACSEVICEIDEERROR: /*加扰机AC 错误*/
            break;
  		case  SC_CARDSTBNOTPAIR:
            break;
  		case SC_NEEDFEED:  /*子卡需要与母卡对应,请喂养子卡*/
            event = CAS_S_MOTHER_CARD_REGU_INSERT;
            break;
  		default:
            CAS_XSM_PRINTF("wDemux:%d \n", param1);
  			    break;
  	}

     cas_send_event(p_priv->slot, CAS_ID_XSM, event, 0);

}


#define XSM_MAX_SEM_NUM (16)

u32 xsm_sem_list[XSM_MAX_SEM_NUM] = {0, };

static s32 XSMCAS_SEM_Search_Free(void)
{
    int i;

    for(i = 0; i < XSM_MAX_SEM_NUM; i++)
    {
        if(xsm_sem_list[i] == 0)
            return i;
    }
    return -1;
}

static void XSMCAS_SEM_Set(s32 sem_id, u32 semaphore)
{
    xsm_sem_list[sem_id] = semaphore;
}

static s32 XSMCAS_SEM_Check(u32 semaphore)
{
    int i;

    for(i = 0; i < XSM_MAX_SEM_NUM; i++)
    {
        if(xsm_sem_list[i] == semaphore)
            return 0;
    }
    for(i = 0; i < XSM_MAX_SEM_NUM; i++)
    {
        OS_PRINTF("Ori Sem:%x XSM Sem:%x\n", xsm_sem_list[i], semaphore);
    }
    return -1;
}

UINT32   XinShiMao_OS_CreateSemaphore(UINT32 ulInitialCount)  /*创建信号量*/
{
    s32 sem_id = 0;
    u32 semaphore;

    MT_ASSERT(ulInitialCount >= 0);
    sem_id = XSMCAS_SEM_Search_Free();
    MT_ASSERT(0 <= sem_id);
	/* We need to modify mtos_sem_create function to support the second parameter */
    if(mtos_sem_create((os_sem_t *)&semaphore, ulInitialCount) == TRUE)
    {
        XSMCAS_SEM_Set(sem_id, semaphore);
        return semaphore;
    }
    OS_PRINTF("CAS create semphore failed. \n");
    return 0;
}

void  XinShiMao_OS_WaitSemaphore(UINT32 ulSemaphore)       /*等待信号量*/
{
    OS_PRINTF("TRDRV_OS_WaitSemaphore:0x%x\n", ulSemaphore);
    MT_ASSERT((-1) != XSMCAS_SEM_Check(ulSemaphore));
    mtos_sem_take((os_sem_t *)&ulSemaphore, 0);
    
}

void  XinShiMao_OS_SignalSemaphore(UINT32 ulSemaphore)       /*释放信号量*/
{
    MT_ASSERT((-1) != XSMCAS_SEM_Check(ulSemaphore));
    mtos_sem_give((os_sem_t *)&ulSemaphore);
    OS_PRINTF("TRDRV_OS_SignalSemaphore:0x%x\n", ulSemaphore);
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建任务
输入参数：
		name[]， 4个字节的任务名称。
		stackSize， 任务所使用的堆栈的大小。
		entryPoint， 任务的入口地址。
		priority， 任务的优先级。
		arg1， 传递给任务的第一个参数。
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。
输出参数： 无。
返 回 值：
		CAS_SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 XinShiMao_OSPTaskCreate(char name[],UINT32 stackSize,
                               void (*entryPoint)(void*),
                               INT32 priority,
                               UINT32 arg1,
                               UINT32 arg2,
                               UINT32 * taskId)
{
    INT32 err = CAS_FAILTURE;

    UINT32 arg[2] = {0};
    UINT32 *p_stack = NULL;
    UINT8   index = 0;

    CAS_XSM_PRINTF("[xsm],XinShiMao_OSPTaskCreate \n");
    if(NULL == name || NULL == entryPoint)
    {
         CAS_XSM_PRINTF("XinShiMao_OSPTaskCreate parameter error ! \n");
         return CAS_FAILTURE;
    }

    CAS_XSM_PRINTF("name:%s, stacksize:%d, priority:%d, arg1:%d, arg2:%d \n",
                                                              name,stackSize,priority,arg1,arg2);

    for(index = 0; index < XSM_CAS_TASK_NUM; index++)
    {
        if(XSM_FREE == s_task_record[index].m_TaskStatus)
        {
            s_task_record[index].m_TaskStatus = XSM_USED;
            break;
        }
    }

    if(index >= XSM_CAS_TASK_NUM)
    {
        CAS_XSM_PRINTF("XinShiMao_OSPTaskCreate too many task or invalid task priority!\n");
        return CAS_FAILTURE;
     }

    priority = s_task_record[index].m_uTaskPrio;
    arg[0] = arg1;
    arg[1] = arg2;
    p_stack = (UINT32 *)mtos_malloc(XSM_CAS_TASK_STKSIZE);
    MT_ASSERT(p_stack != NULL);
    
    err = mtos_task_create((u8 *)name,
                            (void (*)(void *))entryPoint,
                            (void *)arg,
                            priority,
                            p_stack,
                            XSM_CAS_TASK_STKSIZE);

    if(!err)
    {
        CAS_XSM_PRINTF("XinShiMao_OSPTaskCreate error = 0x%08x!\n", err);
        s_task_record[index].m_TaskStatus = XSM_FREE;
        return CAS_FAILTURE;
     }

    s_task_record[index].m_uTaskId = *taskId;
    s_task_record[index].m_TaskStatus = XSM_USED;

    CAS_XSM_PRINTF("XinShiMao_OSPTaskCreate success Name[%s]  ",name);
    CAS_XSM_PRINTF("Priority[%d] \n",priority);

    return CAS_SUCCESS;
}


INT32 XinShiMao_OSPTaskTemporarySleep(UINT32 milliSecsToWait)
{
  CAS_XSM_PRINTF("call XinShiMao_OSPTaskTemporarySleep !\n");
  mtos_task_sleep(milliSecsToWait);
  return CAS_SUCCESS;
}

/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建一个消息队列。 
输入参数： 
		name[]， 4个字节的队列名称。 
		maxQueueLength，消息队列中可以存放的消息的数量。当消息队列中该数量达到 该数量时，再往该消息队列发消息将会失败。 
		taskWaitMode:可以不管
		queueId， 消息队列的ID。 
输出参数： 无。 
返 回 值： 
		CAS_SUCCESS：成功；
		FAILURE： 发生错误*/
INT32 XinShiMao_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId)
{
   INT32 err = CAS_FAILTURE;
   UINT32 message_id = 0;
   void *p_quenue_buf = NULL;
   UINT32 quenue_buf_size = 0;

   CAS_XSM_PRINTF("[xsm],XinShiMao_OSPQueueCreate \n");

   if(NULL == queueId || NULL == name)
   {
       CAS_XSM_PRINTF("XinShiMao_OSPQueueCreate, parameter error ! \n");
       MT_ASSERT(0);
   }

  message_id = mtos_messageq_create(XSM_CAS_QUEUE_MAX_DEPTH,(UINT8 *)name);
  MT_ASSERT(message_id != INVALID_MSGQ_ID);

  quenue_buf_size = sizeof(XinShiMao_Queue_message) *XSM_CAS_QUEUE_MAX_DEPTH;
  p_quenue_buf = mtos_malloc(quenue_buf_size);

  MT_ASSERT(p_quenue_buf != NULL);
  memset(p_quenue_buf,0,quenue_buf_size);

  err = mtos_messageq_attach(message_id,p_quenue_buf,
                                                  sizeof(XinShiMao_Queue_message),
                                                  XSM_CAS_QUEUE_MAX_DEPTH);
  if(!err)
  {
       mtos_free(p_quenue_buf);
       CAS_XSM_PRINTF("[xsm], mtos_messageq_attach err ! \n");
       return CAS_FAILTURE;
  }

  *queueId = message_id;

  CAS_XSM_PRINTF("*queueId=%d ,name=%s \n\n",*queueId,name);

  return CAS_SUCCESS;
}


/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 
输入参数： 
		queueId， 所要取得的消息的ID。 
		messsage， 为消息的格式。参照XINSHIMAO_QUEUE_MESSAGE。 
		waitMode:目前只用到，等待直到得到消息 

		CAS_SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 XinShiMao_OSPQueueGetMessage(UINT32 queueId,XinShiMao_Queue_message * message,UINT32 waitMode,UINT32 milliSecsToWait)
{
    INT32 err = CAS_FAILTURE;
    UINT32 message_id = 0;
    os_msg_t msg = {0};
    xsm_queue_t *p_message = NULL;

    CAS_XSM_PRINTF("\n\nXinShiMao_OSPQueueGetMessage queueId = %d ,message=%0x%x\n",queueId,message);

    if(NULL == message)
    {
         CAS_XSM_PRINTF("XinShiMao_OSPQueueGetMessage, parameter error ! \n");
         MT_ASSERT(0);
    }

    message_id = queueId;


    err = mtos_messageq_receive(message_id,&msg,0);
    if(!err)
    {
         CAS_XSM_PRINTF("[xsm]message_id=%d, receive msg error ! \n",message_id);
         return CAS_FAILTURE;
    }
    
    CAS_XSM_PRINTF("[xsm],free message addr=0x%x \n",msg.para1);
    p_message = (xsm_queue_t *)msg.para1;
    memcpy(message,&p_message->message,sizeof(XinShiMao_Queue_message));
    memset(p_message,0,sizeof(xsm_queue_t));
    p_message = NULL;
         

    return CAS_SUCCESS;
}


/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 发送消息到消息队列。 
输入参数： 
		queueId， 消息队列ID。 
		message， 要发送的消息。其格式见XINSHIMAO_QUEUE_MESSAGE结构。 
输出参数： 无。 
返 回 值： 
		CAS_SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 XinShiMao_OSPQueueSendMessage(UINT32 queueId,XinShiMao_Queue_message * message)
{
    INT32 err = CAS_FAILTURE;
    UINT32 message_id = 0;
    os_msg_t msg = {0};
    xsm_queue_t *p_message = NULL;

    CAS_XSM_PRINTF("[xsm],XinShiMao_OSPQueueSendMessage queueid = %d,message=0x%x \n",
                        queueId,message);
    
    if(NULL == message)
    {
         CAS_XSM_PRINTF("[xsm]XinShiMao_OSPQueueSendMessage, parameter error ! \n");
         MT_ASSERT(0);
    }

    message_id = queueId;
    p_message = xsm_free_queue_get(message_id);
    if(p_message == NULL)
    {
        MT_ASSERT(0);
        //CAS_XSM_PRINTF("[xsm], message queuen full !!!!!");
        //return CAS_FAILTURE;
    }

    memcpy(&p_message->message,message,sizeof(XinShiMao_Queue_message));
    CAS_XSM_PRINTF("send:0x%08x ",p_message->message.q1stWordOfMsg);
    CAS_XSM_PRINTF("0x%08x ",p_message->message.q2ndWordOfMsg);
    CAS_XSM_PRINTF("0x%08x ",p_message->message.q3rdWordOfMsg);
    CAS_XSM_PRINTF("0x%08x \n",p_message->message.q4thWordOfMsg);
    
    msg.is_ext = 0;
    msg.para1 = (UINT32)p_message;
    msg.para2 = _MSG_SIZE();

    CAS_XSM_PRINTF("[xsm]queueId=%d,msg.para1=0x%x,msg.para2=0x%x\n",
                                                                 queueId,msg.para1,msg.para2);
    

    err = mtos_messageq_send(message_id, &msg);
    if(!err)
    {
         CAS_XSM_PRINTF("XinShiMao_OSPQueueSendMessage, send msg error ! \n");
         MT_ASSERT(0);
    }
        
    return CAS_SUCCESS;
}


UINT32 XinShiMao_Printf(const char * fmt,...)
{
    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, fmt);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)fmt, p_args);

    CK_VA_END(p_args);
    CAS_XSM_PRINTF(printk_buf);
    return 1;

}

//设置机顶盒时间，机顶盒无需过滤TDT表，由CA来提供时间基准
void	XinShiMao_SetStbTime(TXinShiMao_DATETIME* ca_time)
{
     CAS_XSM_PRINTF("call XinShiMao_SetStbTime !\n");
}


/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 获得机顶盒提供给CA模块保存信息的起始地址和空间大小 
输入参数： 
输出参数： 
		lStartAddr 机顶盒分配空间的起始地址 
		lSize 机顶盒分配空间的大小 
返 回 值： 
		TRUE 成功 
		FALSE 失败 */
CAS_BOOL XinShiMao_GetBuffer(UINT32 *lStartAddr,UINT32 * lSize)
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    *lSize = p_priv->flash_size;
    #if 0
    *lStartAddr = p_priv->flash_start_adr;
    #else
    *lStartAddr = 0;
    #endif
    CAS_XSM_PRINTF("[xsm] GUOTONG_GetBuffer %x   %x\n", *lStartAddr, *lSize);

    return TRUE;
}



/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 从机顶盒分配空间的指定起始地址读指定长度的数据。 
输入参数： 
		lStartAddr： 要读取数据的存储空间的地址。 
		pbData： 被读取数据的存放地址指针。 
		nLen： 要读的数据的长度 
输出参数： 
		pbData： 被读出的数据。 
返 回 值： 
		返回实际读到的字节数*/
UINT32 XinShiMao_ReadBuffer(const UINT8 *lStartAddr,UINT8 *pData,INT32 nLen)
{
    UINT32 offset = 0;
    UINT32 size = (UINT32)nLen;
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_PRINTF("[xsm],XinShiMao_ReadBuffer \n");
    CAS_XSM_PRINTF(" XinShiMao_ReadBuffer:0x%x   0x%x   0x%x   0x%x\n",
                 lStartAddr, *lStartAddr, pData, size);
    #if 0
    p_priv->nvram_read((UINT32)lStartAddr, pData, &size);
    #else
    offset = p_priv->flash_start_adr + (UINT32)lStartAddr;
    p_priv->nvram_read(offset, pData, &size);
    #endif

  return size;
}

/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 向机顶盒的存储空间写信息 
输入参数： 
		lStartAddr： 要写的存储空间的目标地址。 
		pData： 要写的数据 
		nLen： 要写的数据的长度 
输出参数： 无。 
返 回 值： 
		TRUE：成功
		FALSE： 失败。*/


CAS_BOOL XinShiMao_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen)
{
    UINT32 offset = 0;
    cas_xsm_priv_t *p_priv =  (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_PRINTF("[xsm],XinShiMao_WriteBuffer \n");
    CAS_XSM_PRINTF(" [xsm] XinShiMao_WriteBuffer:0x%x  0x%x  0x%x  0x%x\n",
              lStartAddr, *(UINT32 *)lStartAddr, pData, nLen);

    #if 0
    p_priv->nvram_write((UINT32)lStartAddr, (UINT8 *)pData, nLen);
    #else
    offset = p_priv->flash_start_adr + lStartAddr;
    p_priv->nvram_write(offset, (UINT8 *)pData, nLen);
    #endif

    return TRUE;
}



/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 启动过滤器去接收CA的数据。 
输入参数： 
		iRequestID     ,     为如下几个值之一
						CA_STB_FILTER_1,
						CA_STB_FILTER_2
						CA_STB_FILTER_3
						CA_STB_FILTER_4
		pbFilterMatch1， 如果是过滤Ecm数据，则该参数为Ecm filter的数据；
						 如果是过滤Emm数据，则该参数为Emm filter1的数据；
		pbFilterMask1，  如果是过滤Ecm数据，则该参数为Ecm filter的mask；
						 如果是过滤Emm数据，则该参数为Emm filter1的mask；
		bLen， filter的长度。 
		wPid， 通道的PID值。 
		bWaitTime， 通道过期时间，即从多长时间内通道无法接收到与 
	       if ( wPid <0 && wPid >0x1fff ) 提示机顶盒DEMMUX需要释放过滤器空间，EMM可以不释放，ECM必须释放
返 回 值： 
		CAS_SUCCESS：成功， 
		FAILURE： 失败。 */
INT32 XinShiMao_TableStart(UINT8 iRequestID,
						  const UINT8 * pbFilterMatch1,
	                                     const UINT8 * pbFilterMask1,
	                                     UINT8 bLen,
	                                     UINT16 wPid,
	                                     UINT8 bWaitTime)
	                                     
{
    UINT32 err = CAS_FAILTURE;
    UINT8   index = 0;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;
    UINT8 filter_byte_num = 0;  


    CAS_XSM_PRINTF("[xsm],XinShiMao_TableStart ,wPid= 0x%x \n",wPid);

    CAS_XSM_PRINTF("[xsm], iRequestID=0x%x ",iRequestID);
    CAS_XSM_PRINTF("bLen=%d  ",bLen);
    CAS_XSM_PRINTF("wPid= 0x%x  ",wPid);
    CAS_XSM_PRINTF("bWaitTime=%d \n",bWaitTime);

    for(index = 0; index < bLen; index ++)
    {
         CAS_XSM_PRINTF("FilterData[%d]:0X%X  ",index,pbFilterMatch1[index]);
         CAS_XSM_PRINTF("FilterMask[%d]:0X%X \n",index,pbFilterMask1[index]);
    }


    if(XSM_CA_CARD_INSERT != xsm_get_card_status())
    {

      CAS_XSM_PRINTF("[xsm], card not insert ! \n");

      return CAS_FAILTURE;
    }
    
    if(wPid > 0x1fff || wPid < 0 )//释放之前的filter资源
    {
      OS_PRINTF("[xsm], wPID=%d is error ! \n",wPid);

       for(index = 0; index < XSM_FILTER_MAX_NUM; index++)
       {
            if(XSM_USED == s_tFilter[index].m_FilterStatus &&
                CA_STB_FILTER_4 == s_tFilter[index].m_xsmReqId)
            {
               xsm_filter_free(&s_tFilter[index]);
               memset((void *)&s_tFilter[index], 0, sizeof(xsm_filter_struct_t));
            }
       }
        
      return CAS_FAILTURE;
    }

    xsm_ca_lock();

          
    filter_byte_num = bLen;
    #if 0
    if(filter_byte_num > DMX_SECTION_FILTER_SIZE)
    {
           filter_byte_num = DMX_SECTION_FILTER_SIZE;
           CAS_XSM_PRINTF("[xsm], filter byte filter_byte_num=%d \n",filter_byte_num);
    }
    #endif
    
    
    for(index = 0; index < XSM_FILTER_MAX_NUM; index++)
    {
         /*filter EMM packet, CA_STB_FILTER_4 is ECM packet*/
         if(XSM_USED == s_tFilter[index].m_FilterStatus
            && iRequestID == s_tFilter[index].m_xsmReqId
            && CA_STB_FILTER_4 != iRequestID
            && NIT_TABLE_ID != iRequestID)
         {

              if((memcmp(pbFilterMatch1, s_tFilter[index].m_uFilterData, filter_byte_num) == 0)
                  && (memcmp(pbFilterMask1, s_tFilter[index].m_uFilterMask, filter_byte_num) == 0)
                  && (s_tFilter[index].m_PID == wPid))
              {
                  CAS_XSM_PRINTF("[xsm],Emm filter is the same, don't request new filter!\n");
                  xsm_ca_unlock();
                  return CAS_SUCCESS;
              }
              else
              {
                   CAS_XSM_PRINTF("[xsm],Emm filter is different, cancel old filter !\n");
                   xsm_filter_free(&s_tFilter[index]);
                   memset((void *)&s_tFilter[index], 0, sizeof(xsm_filter_struct_t));
               }
         }

    }
    
    /*get free filter */
    for(index = 0; index < XSM_FILTER_MAX_NUM; index ++)
    {
      if(XSM_FREE == s_tFilter[index].m_FilterStatus)
      {
        break;
      }
    }

    /*can't get free filter*/
    if(index >= XSM_FILTER_MAX_NUM)
    {
        CAS_XSM_PRINTF("[xsm],Filter array is full!\n");
        //MT_ASSERT(0);                
        xsm_ca_unlock();
        return CAS_SUCCESS;
    }

      p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != p_dev->p_base);

      p_slot.in = DMX_INPUT_EXTERN0;
      p_slot.pid = wPid;
      p_slot.type = DMX_CH_TYPE_SECTION;

      err = dmx_si_chan_open(p_dev,&p_slot,&s_tFilter[index].m_dwReqHandle);
      if (0 != err)
      {
          CAS_XSM_PRINTF("\r\n [xsm],%s : %d : error_code = 0x%08x \n", __FUNCTION__, __LINE__, err);
          xsm_ca_unlock();
          return CAS_FAILTURE;
      }
      CAS_XSM_PRINTF("[xsm],index=%d, wPid=0x%X, m_dwReqHandle=0x%X \n",
                                                 index,wPid,s_tFilter[index].m_dwReqHandle);

      s_tFilter[index].p_buf = s_tFilterBuf[index];
      err = dmx_si_chan_set_buffer(p_dev,s_tFilter[index].m_dwReqHandle,s_tFilter[index].p_buf,
                                                        XSM_FILTER_BUFFER_SIZE);
      if(0 != err)
      {
          CAS_XSM_PRINTF("\r\n[xsm],set filter buffer failed!\n");
          xsm_ca_unlock();
          return CAS_FAILTURE;
      }

      p_param.continuous = 1;
      p_param.en_crc = FALSE;//DISABLE FOR CA DATA
      
       //memcpy(p_param.value, pbFilterMatch1, 12);
       //memcpy(p_param.mask, pbFilterMask1, 12);
       p_param.value[0] = pbFilterMatch1[0];
       p_param.value[1] = pbFilterMatch1[3];
       p_param.value[2] = pbFilterMatch1[4];
       p_param.value[3] = pbFilterMatch1[5];
       p_param.value[4] = pbFilterMatch1[6];
       p_param.value[5] = pbFilterMatch1[7];
       p_param.value[6] = pbFilterMatch1[8];
       p_param.value[7] = pbFilterMatch1[9];
       p_param.value[8] = pbFilterMatch1[10];
       p_param.value[9] = pbFilterMatch1[11];
       p_param.value[10] = pbFilterMatch1[12];



       p_param.mask[0] = pbFilterMask1[0];
       p_param.mask[1] = pbFilterMask1[3];
       p_param.mask[2] = pbFilterMask1[4];
       p_param.mask[3] = pbFilterMask1[5];
       p_param.mask[4] = pbFilterMask1[6];
       p_param.mask[5] = pbFilterMask1[7];
       p_param.mask[6] = pbFilterMask1[8];
       p_param.mask[7] = pbFilterMask1[9];
       p_param.mask[8] = pbFilterMask1[10];
       p_param.mask[9] = pbFilterMask1[11];
       p_param.mask[10] = pbFilterMask1[12];
       
       err = dmx_si_chan_set_filter(p_dev,s_tFilter[index].m_dwReqHandle,&p_param);
       if(0 != err)
       {
           CAS_XSM_PRINTF("\n[xsm],set filter failed!\n");
           xsm_ca_unlock();
           return CAS_FAILTURE;
       }

       err = dmx_chan_start(p_dev,s_tFilter[index].m_dwReqHandle);
       if(0 != err)
       {
            CAS_XSM_PRINTF("\n[xsm],start channel  failed!\n");
            xsm_ca_unlock();
            return CAS_FAILTURE;
       }

       if(0 == bWaitTime || CA_STB_FILTER_4 != iRequestID)
       {
            s_tFilter[index].m_timeout = 0xffffffff;
       }
       else
       {
            s_tFilter[index].m_timeout = bWaitTime * 1000;
       }

        s_tFilter[index].m_FilterStatus = XSM_USED;
        s_tFilter[index].m_xsmReqId = iRequestID;
        s_tFilter[index].m_PID = wPid;
        memcpy(s_tFilter[index].m_uFilterData, pbFilterMatch1, filter_byte_num);
        memcpy(s_tFilter[index].m_uFilterMask, pbFilterMask1, filter_byte_num);
        s_tFilter[index].m_dwStartMS = mtos_ticks_get();
        CAS_XSM_PRINTF("\n [XSM], FILTER setting ok! \n ");

        xsm_ca_unlock();

        return CAS_SUCCESS;
}


/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 设置解扰通道的控制字。 
输入参数： 
		wEcmPid， 控制字的所在的ecm包的PID值。 
		szOddKey， 奇控制字数据的指针。 
		szEvenKey， 偶控制字数据的指针。 
		bKeyLen， 控制字长度。 
		bReservedFlag, 保留。 
*/
void XinShiMao_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag)
{
    UINT16 v_pid = 0;
    UINT16 a_pid = 0;
    dmx_device_t *p_dev = NULL;
    INT32 err = CAS_FAILTURE;

    if((NULL == szOddKey) || (NULL == szEvenKey))
    {
        CAS_XSM_PRINTF("[xsm],XinShiMao_SetCW paramter error ! \n");
        MT_ASSERT(0);
    }

    CAS_XSM_PRINTF("[xsm],XinShiMao_SetCW \n");
    CAS_XSM_PRINTF("\nOddKey %02x %02x %02x %02x %02x %02x %02x %02x ticks=%d \n"
    , szOddKey[0], szOddKey[1], szOddKey[2], szOddKey[3]
    , szOddKey[4], szOddKey[5], szOddKey[6], szOddKey[7],mtos_ticks_get());
    CAS_XSM_PRINTF("EvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n"
    , szEvenKey[0], szEvenKey[1], szEvenKey[2], szEvenKey[3]
    , szEvenKey[4], szEvenKey[5], szEvenKey[6], szEvenKey[7]);

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
    , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    if(old_v_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,old_v_channel,FALSE);
        old_v_channel = 0xffff;
    }
    if(old_a_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,old_a_channel,FALSE);
        old_a_channel = 0xffff;
    }

    v_pid = cas_xsm_get_v_pid();
    a_pid = cas_xsm_get_a_pid();
    #if 0
    v_pid = 0x201;
    a_pid = 0x294;
    #endif

    if(v_pid == 0 || a_pid == 0)
    {
         return;
    }

    CAS_XSM_PRINTF("\n set key v_pid= 0x%x  a_pid = 0x%x\n", v_pid, a_pid);
    err = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if(err != SUCCESS)
    {
        CAS_XSM_PRINTF("v_pid=0x%x \n",v_pid);
        return;
    }

    err = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if(err != SUCCESS)
    {
        CAS_XSM_PRINTF("a_pid=0x%x ,dmx_get_chanid_bypid error !\n",a_pid);
         //MT_ASSERT(0);
         return;
    }
    CAS_XSM_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

    if(v_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,v_channel,TRUE);
       if(err != SUCCESS)
       {
            //MT_ASSERT(0);
            CAS_XSM_PRINTF("[gt],dmx_descrambler_onoff error ! \n");
            return;
       }
       err = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)szOddKey,8);
       err = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)szEvenKey,8);
    }

    if (a_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
        if(err != SUCCESS)
        {
            //MT_ASSERT(0);
            return;
        }
        err = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)szOddKey,8);
        err = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)szEvenKey,8);
    }

    old_v_channel = v_channel;
    old_a_channel = a_channel;
}





/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前节目的ServiceID 
输入参数： 无
输出参数： 无。 
返 回 值： 当前节目的ServiceID*/
UINT16 XinShiMao_GetCurr_ServiceID(void)
{
  UINT16 service_id = 0;

  service_id = cas_xsm_get_server_id();
  CAS_XSM_PRINTF("[xsm],XinShiMao_GetCurr_ServiceID:%d  \n",service_id);

  return service_id;
}


/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Emm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Emm Pid*/
UINT16 XinShiMao_GetCurr_EmmID(void)
{
  UINT16 emm_pid = 0;

  emm_pid = cas_xsm_get_emm_pid();
  CAS_XSM_PRINTF("[XSM],XinShiMao_GetCurr_EmmID:0x%x \n",emm_pid);

  //emm_pid = 0x30;
  return emm_pid;

}


/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Ecm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Ecm Pid*/
UINT16 XinShiMao_GetCurr_EcmID(void)
{

  UINT16 ecm_pid = 0;

  ecm_pid = cas_xsm_get_ecm_pid();
  CAS_XSM_PRINTF("[XSM],XinShiMao_GetCurr_EcmID:0x%x \n",ecm_pid);

  return ecm_pid;
}


/*提 供 者： 机顶盒。 
输入参数： 
		bCardNumber： 暂时保留不用； 
		bLength： pabMessage的长度； 
		pabMessage： 发送命令的消息指针； 
		pabResponse： 接收响应结果的数据块的指针； 
		bRLength： 响应结果的数据块长度指针。 
输出参数： 
		pabResponse： 响应结果的数据块； 
		bRLength： 响应结果的数据块长度。 
		pbSW1 智能卡状态字节1 
		pbSW2 智能卡状态字节2 
返 回 值： 
		CAS_TRUE：成功
		CAS_FALSE：失败 */
INT32 XinShiMao_SC_DRV_SendDataEx(UINT8 bCardNumber,UINT8 bLength,
	                                                      UINT8 * pabMessage,
	                                                      UINT8 * pabResponse,
	                                                      UINT8 * bRLength,
	                                                      UINT8 * pbSW1,
	                                                      UINT8 * pbSW2)
{
    RET_CODE ret = CAS_SUCCESS;
    scard_opt_desc_t rwopt = {0};
    UINT8 read_bytes[255] = {0};
    INT8 retry_times = 0;
    UINT8 i = 0;

    mtos_task_lock();
    mtos_task_delay_ms(10);

    CAS_XSM_PRINTF("[XSM],XinShiMao_SC_DRV_SendDataEx \n");
    if(pabMessage == NULL || pabResponse == NULL || bRLength == NULL
       || pbSW1 == NULL || pbSW2 == NULL)
    {
         CAS_XSM_PRINTF("XinShiMao_SC_DRV_SendDataEx parameter error! \n");
         mtos_task_unlock();
         return CAS_FALSE;
    }


    CAS_XSM_PRINTF("send data to card: ");
    for(i = 0; i < bLength; i++)
    {
         CAS_XSM_PRINTF(" %X ",*(pabMessage + i));
    }
    CAS_XSM_PRINTF("\n");

    
    rwopt.p_buf_out = pabMessage;
    rwopt.bytes_out = bLength;
    rwopt.p_buf_in   = read_bytes;
    rwopt.bytes_in_target = 0;
    rwopt.bytes_in_actual = 0;

    do
    {
        
        ret = scard_pro_rw_transaction(&rwopt);
        CAS_XSM_PRINTF("scard_pro_rw_transaction Ret = %d\n",ret);
        retry_times++;
    }while((0 != ret) && (retry_times <= 3));
    
    if(retry_times > 3)
    {
         OS_PRINTF("Send data to card error ! \n");
          mtos_task_unlock();
         return CAS_FALSE;
    }


    CAS_XSM_PRINTF("receive data to card [%d]: ",rwopt.bytes_in_actual);
    for(i = 0; i < rwopt.bytes_in_actual; i++)
    {
         CAS_XSM_PRINTF(" %X ",read_bytes[i]);
    }
    CAS_XSM_PRINTF("\n");

    if(rwopt.bytes_in_actual > 2)
    {
         memcpy(pabResponse,read_bytes,rwopt.bytes_in_actual - 2);
         *pbSW1 = read_bytes[rwopt.bytes_in_actual - 2];
         *pbSW2 = read_bytes[rwopt.bytes_in_actual - 1];
         *bRLength = rwopt.bytes_in_actual - 2;
    }
    else if(rwopt.bytes_in_actual == 2)
    {
         *pbSW1 = read_bytes[0];
         *pbSW2 = read_bytes[1];
         *bRLength = 0;
    }
    else
    {
        OS_PRINTF("[XSM],XinShiMao_SC_DRV_SendDataEx error! \n");
        mtos_task_unlock();
        return CAS_FALSE;
    }

    CAS_XSM_PRINTF("[XSM],*bRLength=0x%x,*pbSW1=0x%x,*pbSW2=0x%x \n",*bRLength,*pbSW1,*pbSW2);


    CAS_XSM_PRINTF("return data to lib: ");
    for(i = 0; i < *bRLength; i++)
    {
         CAS_XSM_PRINTF(" %X ",*(pabResponse + i));
    }
    CAS_XSM_PRINTF("\n\n");

    mtos_task_unlock();
    return CAS_TRUE;
}


INT32 XinShiMao_SC_DRV_Initialize(void)
{
    INT32 err = 0;
    UINT32 *p_stack = NULL;

    err = mtos_sem_create((os_sem_t *)&s_ca_lock,1);
    if(!err)
    {
         CAS_XSM_PRINTF("XinShiMao_SC_DRV_Initialize, create sem error ! \n");
         MT_ASSERT(0);
    }

    p_stack = (u32 *)mtos_malloc(XSM_MONITOR_OVERDUE_SECTION_TASK_STACK_SIZE);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)"Overdue_Monitor",
            xsm_overdue_data_monitor,
            (void *)0,
            XSM_MONITOR_OVERDUE_SECTION_TASK_PRIO,
            p_stack,
            XSM_MONITOR_OVERDUE_SECTION_TASK_STACK_SIZE);

    if (!err)
    {
      CAS_XSM_PRINTF("Create Overdue_Monitor task error = 0x%08x!\n", err);
      return 1;
    }

    p_stack = (u32 *)mtos_malloc(XSM_CAS_SECTION_TASK_STACK_SIZE);

    err = mtos_task_create((u8 *)"CA_Monitor",
            xsm_ca_data_monitor,
            (void *)0,
            XSM_CAS_SECTION_TASK_PRIO,
            p_stack,
            XSM_CAS_SECTION_TASK_STACK_SIZE);

    if (!err)
    {
      CAS_XSM_PRINTF("Create CA_Monitor task error = 0x%08x!\n", err);
      return 1;
    }

    return 0;
}

//bCardNumber 默认0
void  XinShiMao_SC_DRV_ResetCard(UINT8 bCardNumber)
{
  scard_device_t *p_smc_dev = NULL;

  //Delay_US(1000);
  
  OS_PRINTF("--------[xsm],XinShiMao_SC_DRV_ResetCard ---------\n");
  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);

  OS_PRINTF("--------[xsm],Start scard_reset ---------\n");

  scard_reset(p_smc_dev);

  OS_PRINTF("--------[xsm],Leave scard_reset---------\n");

}

void	XinShiMao_UpStbId(char* p_stbid)
{

}


