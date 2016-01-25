/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "string.h"
#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"
#include "stdio.h"
#include "stdlib.h"

#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"

#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"

#include "xinshimao_ca.h"
#include "xinshimao_cas_include.h"
#include "nim.h"
#include "smc_op.h"

//#define CAS_XSM_ADT_DEBUG

#ifdef CAS_XSM_ADT_DEBUG
#define CAS_XSM_ADT_PRINTF OS_PRINTF
#else
#define CAS_XSM_ADT_PRINTF DUMMY_PRINTF
#endif


BOOL s_xsm_entitle_flag = 0; //0:entitle  1:no entitle

extern cas_adapter_priv_t g_cas_priv;
static xsm_ca_card_status_t s_card_status = XSM_CA_CARD_UNKNOWN;
static os_sem_t s_pid_lock = 0;

char  gs_stb_sn[20] = {0};
char  gs_cw_mask[8] = {0};
char  gs_smartcard_id[3] = {0};
char  gs_super_cas_id[4] = {0};

static UINT8 s_mail_flag[MAX_EMAIL_NUM] = {0};

u8 gs_ca_nit_flag = 0;

u8 gs_ca_nit_state = 0;

u16 gs_ca_area_code = 0xffff;

u16 gs_card_area_code = 0xffff;

extern msg_info_t xsm_OsdMsg;
/*!
  abc
  */
void set_event(cas_xsm_priv_t *p_priv, u32 event)
{
  CAS_XSM_ADT_PRINTF("gscas: set_event  event  =%d  \n", event);
  if(event == 0)
  {
    return;
  }

 cas_send_event(p_priv->slot, CAS_ID_XSM, event, 0);

}
/*!
    lock
*/
static void xsm_pid_lock(void)
{
    mtos_sem_take((os_sem_t *)&s_pid_lock, 0);
}

/*!
    unlock
*/
static void xsm_pid_unlock(void)
{
    mtos_sem_give((os_sem_t *)&s_pid_lock);
}

/*!
    get card status
*/
xsm_ca_card_status_t xsm_get_card_status()
{
     xsm_ca_card_status_t status = XSM_CA_CARD_UNKNOWN;

     xsm_pid_lock();
     status = s_card_status;
     xsm_pid_unlock();

     CAS_XSM_ADT_PRINTF("[xsm],get status=%d \n",status);
     return status;
}

/*!
    set card status
*/
void xsm_set_card_status(xsm_ca_card_status_t status)
{
     CAS_XSM_ADT_PRINTF("[xsm],set status=%d \n",status);

     xsm_pid_lock();
     s_card_status = status;
     xsm_pid_unlock();
}

/*!
    get video pid
*/
UINT16 cas_xsm_get_v_pid(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  //CAS_XSM_ADT_PRINTF("p_priv addr:0x%x, p_priv->v_pid=0x%x \n",p_priv,p_priv->v_pid);

  return p_priv->v_pid;
}

/*!
    get audio pid
*/
UINT16 cas_xsm_get_a_pid(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  //CAS_XSM_ADT_PRINTF("p_priv addr:0x%x,p_priv->a_pid=0x%x \n",p_priv,p_priv->a_pid);
  return p_priv->a_pid;
}

/*!
  Set scramble flag

  \param[in] scramble_flag.
  */
void cas_xsm_set_scramble_flag(UINT8 scramble_flag)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  xsm_pid_lock();
  p_priv->scramble_flag = scramble_flag;
  xsm_pid_unlock();

  CAS_XSM_ADT_PRINTF("[xsm],cas_xsm_set_scramble_flag scramble_flag=0x%x \n",p_priv->scramble_flag);
}

/*!
  Get scramble flag

  \return scramble_flag.
  */
UINT8 cas_xsm_get_scramble_flag(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
  UINT16 scramble_flag = 0;

  xsm_pid_lock();
  scramble_flag = p_priv->scramble_flag;
  xsm_pid_unlock();

  CAS_XSM_ADT_PRINTF("[xsm],cas_xsm_get_scramble_flag scramble_flag=0x%x \n",p_priv->scramble_flag);
  return scramble_flag;
}

/*!
  Set ecm pid

  \param[in] ecm_pid.
  */
void cas_xsm_set_ecm_pid(UINT16 ecm_pid)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  xsm_pid_lock();
  p_priv->ecm_pid = ecm_pid;
  xsm_pid_unlock();

  CAS_XSM_ADT_PRINTF("[xsm],cas_xsm_set_ecm_pid ecm_pid=0x%x \n",p_priv->ecm_pid);
}

/*!
  Get ecm pid

  \return ecm_pid.
  */
UINT16 cas_xsm_get_ecm_pid(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
  UINT16 ecm_pid = 0;

  xsm_pid_lock();
  ecm_pid = p_priv->ecm_pid;
  xsm_pid_unlock();

  return ecm_pid;
}

/*!
  Set emm pid

  \param[in] emm_pid.
  */
void cas_xsm_set_emm_pid(UINT16 emm_pid)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  xsm_pid_lock();
  p_priv->emm_pid = emm_pid;
  xsm_pid_unlock();

  CAS_XSM_ADT_PRINTF("[xsm],cas_xsm_set_emm_pid emm_pid=0x%x \n",p_priv->emm_pid);
}

/*!
  Get emm pid

  \return emm_pid.
  */
UINT16 cas_xsm_get_emm_pid(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
  UINT16 emm_pid = 0;

  xsm_pid_lock();
  emm_pid = p_priv->emm_pid;
  xsm_pid_unlock();

  return emm_pid;
}

/*!
  Set server id

  \param[in] server_id.
  */
void cas_xsm_set_server_id(UINT16 server_id)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  xsm_pid_lock();
  p_priv->server_id = server_id;
  xsm_pid_unlock();
}

/*!
  Get network id

  \return network id.
  */
UINT16 cas_xsm_get_server_id(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
  UINT16 server_id = 0;

  xsm_pid_lock();
  server_id = p_priv->server_id;
  xsm_pid_unlock();

  return server_id;
}

/*!
  Set network id

  \param[in] network_id.
  */
void cas_xsm_set_network_id(UINT16 network_id)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

  CAS_XSM_ADT_PRINTF("[xsm], set network_id = 0x%x \n",network_id);
  //xsm_pid_lock();
  p_priv->network_id = network_id;
  //xsm_pid_unlock();
}

/*!
  Get network id

  \return network id.
  */
UINT16 cas_xsm_get_network_id(void)
{
  cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
  UINT16 network_id = 0;

  //xsm_pid_lock();
  network_id = p_priv->network_id;
  //xsm_pid_unlock();

  return network_id;
}



void cas_xsm_zone_check_end(UINT32 event)
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_ADT_PRINTF("\r\n*** cas_xsm_zone_check_end *** \r\n");
    set_event(p_priv, event);
    return;
}



/*!
  send message to hide no entitle message
  */
void cas_xsm_cancel_entitle_message()
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_ADT_PRINTF("\r\n*** xsm_cancel_entitle_message *** \r\n");
    set_event(p_priv, CAS_C_ENTITLE_LIMIT);
    return;
}



/*!
  abc
  */
static RET_CODE cas_xsm_init(void)
{
  static u32 init_flag = 0;
  INT32 ret = 0;

  if(!init_flag)
  {
    XinShiMao_Initialize(0);
    CAS_XSM_ADT_PRINTF("cas_xsm_init\n");

    ret = mtos_sem_create((os_sem_t *)&s_pid_lock,1);
    if(!ret)
    {
       CAS_XSM_ADT_PRINTF("[xsm], cas_xsm_init  sem create error ! \n");
       MT_ASSERT(0);
    }

    XinShiMao_SC_DRV_Initialize();


    init_flag = 1;
    mtos_task_sleep(300);
    //cas_init_nvram_data(CAS_ID_XSM);
  }
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_xsm_deinit(void)
{
  return SUCCESS;
}

/*!
  check guotong ca_sys_id

  \param[in] ca_sys_id.
  \return success if success else ERR_FAILURE
  */
RET_CODE cas_xsm_identify(UINT16 ca_sys_id)
{
     RET_CODE ret = 0;

     ret = XinShiMao_IsMatchCAID(ca_sys_id);
     if(ret != TRUE)
     {
             CAS_XSM_ADT_PRINTF("[xsm] No xsm CA card ! \n");
             return ERR_FAILURE;
     }

    return SUCCESS;
}



/*++
功能：通知上层区域检测结束
参数：
--*/
static void cas_xsm_ZoneCheckEnd(void)
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    CAS_XSM_ADT_PRINTF("\r\n*** cas_xsm_ZoneCheckEnd*** \r\n");
    set_event(p_priv, CAS_S_ZONE_CHECK_END);
    return;
}

INT32 xsm_get_mail_header(UINT16 *max_mail_num, cas_mail_header_t *mail_header)
{
     TXinShiMao_EmailHead xsm_email_head[MAX_EMAIL_NUM] = {{{0},0,0,0,{0}},};
     UINT8 email_head_num = 0;
     UINT8 email_head_len = 0;
     UINT8 i = 0;

     if(max_mail_num == NULL || mail_header == NULL)
     {
          CAS_XSM_ADT_PRINTF("[gt], gt_get_mial_header paramter error ! \n");
          MT_ASSERT(0);
     }

     email_head_num = XinShiMao_GetEmailHeads(xsm_email_head, MAX_EMAIL_NUM);
     CAS_XSM_ADT_PRINTF("[xsm],email_head_num=%d \n",email_head_num);
     for(i = 0; i < email_head_num; i++)
     {
          mail_header[i].m_id = xsm_email_head[i].m_bEmailID;
          mail_header[i].index = i;
          mail_header[i].new_email = xsm_email_head[i].m_bNewEmail;
          mail_header[i].priority = xsm_email_head[i].m_bEmail_Level;
          email_head_len = strlen((char *)xsm_email_head[i].m_szEmailTitle);
          CAS_XSM_ADT_PRINTF("[xsm],email_head_len=%d \n",email_head_len);
          strncpy((void *)mail_header[i].subject,(char *)xsm_email_head[i].m_szEmailTitle,email_head_len);
          CAS_XSM_ADT_PRINTF("year=%04d %04d %02d %02d %02d %02d \n",
                                                               xsm_email_head[i].m_tCreateTime.Y,
                                                               xsm_email_head[i].m_tCreateTime.M,
                                                               xsm_email_head[i].m_tCreateTime.D,
                                                               xsm_email_head[i].m_tCreateTime.H,
                                                               xsm_email_head[i].m_tCreateTime.MI,
                                                               xsm_email_head[i].m_tCreateTime.S);

          mail_header[i].send_date[0] = (UINT8)((xsm_email_head[i].m_tCreateTime.Y) / 100);
          mail_header[i].send_date[1] = (UINT8)((xsm_email_head[i].m_tCreateTime.Y) % 100);
          mail_header[i].send_date[2] = (UINT8)((xsm_email_head[i].m_tCreateTime.M) & 0xff);
          mail_header[i].send_date[3] =  xsm_email_head[i].m_tCreateTime.D;
          mail_header[i].send_date[4] =  xsm_email_head[i].m_tCreateTime.H;
          mail_header[i].send_date[5] =  xsm_email_head[i].m_tCreateTime.MI;
          mail_header[i].send_date[6] =  xsm_email_head[i].m_tCreateTime.S;
          s_mail_flag[mail_header[i].m_id] = 1;

          CAS_XSM_ADT_PRINTF("sendtime: %02d %02d %02d %02d %02d %02d %02d \n",
                                                               mail_header[i].send_date[0],
                                                               mail_header[i].send_date[1],
                                                               mail_header[i].send_date[2],
                                                               mail_header[i].send_date[3],
                                                               mail_header[i].send_date[4],
                                                               mail_header[i].send_date[5],
                                                               mail_header[i].send_date[6]);

          CAS_XSM_ADT_PRINTF("[gt],index:%d \n",mail_header[i].index);
          CAS_XSM_ADT_PRINTF("[gt],email id:%d \n",mail_header[i].m_id);
          CAS_XSM_ADT_PRINTF("[gt],new_email:%d \n",mail_header[i].new_email);
          CAS_XSM_ADT_PRINTF("[gt],priority:%d \n",mail_header[i].priority);
          //CAS_XSM_ADT_PRINTF("[gt],creat date:%s \n",mail_header[i].send_date);
          CAS_XSM_ADT_PRINTF("[gt],subject:%s \n",mail_header[i].subject);

          CAS_XSM_ADT_PRINTF("[gt],s_mail_flag[%d]=%d \n",mail_header[i].m_id,
                                            s_mail_flag[mail_header[i].m_id]);
     }

     *max_mail_num = email_head_num;

     return SUCCESS;
}

/*!
  get mail body

  \param[in] mail_id.
  \param[out] p_mail_content .
  */
INT32 xsm_get_mail_body(u32 mail_id,cas_mail_body_t *p_mail_content)
{
     BOOL err = FALSE;
     UINT8 email_body_len = 0;
     TXinShiMao_EmailContent email_content = {{0}};

     if(p_mail_content == NULL)
     {
          CAS_XSM_ADT_PRINTF("[gt], gt_get_mail_body paramter error ! \n");
          MT_ASSERT(0);
     }

      err = XinShiMao_GetEmailContent(mail_id,&email_content);
      if(err == FALSE)
      {
           CAS_XSM_ADT_PRINTF("[gt], gt_get_mail_body, not get mail body ! \n");
           return ERR_FAILURE;
      }

      CAS_XSM_ADT_PRINTF(" content:%s \n",&email_content.m_szEmail);

      p_mail_content->mail_id = mail_id;
      email_body_len = strlen((char *)email_content.m_szEmail);
      CAS_XSM_ADT_PRINTF("[gt],email_body_len:%d \n",email_body_len);
      strncpy((void *)p_mail_content->data,(char *)email_content.m_szEmail,email_body_len);

      CAS_XSM_ADT_PRINTF("[gt],content:%s \n",(char *)p_mail_content->data);

      return SUCCESS;
}
/*!
  abc
  */
static RET_CODE cas_xsm_io_ctrl(u32 cmd, void *param)
{
    cas_xsm_priv_t *p_priv = (cas_xsm_priv_t *)g_cas_priv.cam_op[CAS_ID_XSM].p_priv;
    INT32 ret = SUCCESS;
    UINT8 i = 0;
    cas_card_info_t *p_card_info = NULL;
    UINT8 card_sn[XSM_CARD_SN_LEN] = {0};
    UINT8 entitle_status = 0;
    UINT8 card_type = 0;
    UINT16 card_region_id = 0;
    //chlid_card_status_info *p_feed_info = NULL;
    cas_table_info_t *p_Cas_table_info = NULL;
    XinShiMao_CAServiceInfo xsm_ecm_info = {0};

    cas_mail_headers_t *p_mail_headers = NULL;
    cas_mail_body_t *p_mailcontent = NULL;
//    UINT16 email_count = 0;
 //   UINT16 new_email_count = 0;
    UINT32 mail_index = 0;
    UINT32 err = 0;
    chlid_card_status_info *p_child_satus =  NULL;

    //msg_info_t *p_msg_info = NULL;
    //nim_channel_info_t *p_nim_info = NULL;
    //nim_device_t *p_nim_dev = NULL;
    //UINT8 nit_match[XSM_FILTER_DEPTH_SIZE] = {NIT_TABLE_ID,0,0,0,0,0,0,0,0,0,0,0,0};
    //UINT8 nit_mask[XSM_FILTER_DEPTH_SIZE] =  {0xff,0,0,0,0,0,0,0,0,0,0,0,0};;
    //INT32 wt_len = 0;
    //INT32 rd_len = 0;
    //UINT8 cas_ver[8] = "V000001";
    msg_info_t *p_msg_info = NULL;

    CAS_XSM_ADT_PRINTF("[xsm] ---cas_xsm_io_ctrl---  \n");

     switch(cmd)
     {
          case CAS_IOCMD_STOP:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_STOP \n");
            break;

          case CAS_IOCMD_SET_ECMEMM:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_SET_ECMEMM \n");
            #if 1

            p_Cas_table_info = (cas_table_info_t *)param;

            XinShiMao_SetEcmPID(CA_INFO_CLEAR,NULL);


            for(i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
            {
                 CAS_XSM_ADT_PRINTF("index = %d, ca_system_id = %d, ecm_id = %d, servie_id = %d\n",
                        i,p_Cas_table_info->ecm_info[i].ca_system_id,
                        p_Cas_table_info->ecm_info[i].ecm_id,p_Cas_table_info->service_id);

                 if(TRUE == XinShiMao_IsMatchCAID(p_Cas_table_info->ecm_info[i].ca_system_id))
                 {
                      xsm_ecm_info.m_wEcmPid = p_Cas_table_info->ecm_info[i].ecm_id;
                      xsm_ecm_info.m_bServiceCount = 1;
                      xsm_ecm_info.m_wServiceId[0] = p_Cas_table_info->service_id;

                      //CAS_XSM_ADT_PRINTF("[xsm],11111111111ecm pid=0X%X \n",xsm_ecm_info.m_wEcmPid);
                      //CAS_XSM_ADT_PRINTF("[xsm],service_id=0x%X \n",xsm_ecm_info.m_wServiceId[0]);
                      
                      CAS_XSM_ADT_PRINTF("[xsm] 1111111111XinShiMao_SetEcmPID:0x%x, service id=0x%x \n", 
                                xsm_ecm_info.m_wEcmPid,xsm_ecm_info.m_wServiceId[0]);

                      cas_xsm_set_server_id(p_Cas_table_info->service_id);
                      cas_xsm_set_ecm_pid(xsm_ecm_info.m_wEcmPid);
                      XinShiMao_SetEcmPID(CA_INFO_ADD,&xsm_ecm_info);
                      break;
                      //cas_xsm_get_v_pid();
                 }
            }

            if(i == CAS_MAX_ECMEMM_NUM)
            {
                xsm_ecm_info.m_wEcmPid = 0x1fff;
                xsm_ecm_info.m_bServiceCount = 1;
                xsm_ecm_info.m_wServiceId[0] = p_Cas_table_info->service_id;

                CAS_XSM_ADT_PRINTF("[xsm],ecm pid=0X%X \n",xsm_ecm_info.m_wEcmPid);
                CAS_XSM_ADT_PRINTF("[xsm],service_id=0x%X \n",xsm_ecm_info.m_wServiceId[0]);
                cas_xsm_set_ecm_pid(xsm_ecm_info.m_wEcmPid);
                XinShiMao_SetEcmPID(CA_INFO_ADD,&xsm_ecm_info);
             }

            #endif
            break;

          case CAS_IOCMD_VIDEO_PID_SET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_VIDEO_PID_SET \n");
            p_priv->v_pid = *((UINT16 *)param);
            CAS_XSM_ADT_PRINTF("v_pid=0x%x \n",p_priv->v_pid);

            break;

          case CAS_IOCMD_AUDIO_PID_SET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_AUDIO_PID_SET \n");
            p_priv->a_pid = *((UINT16 *)param);
            CAS_XSM_ADT_PRINTF("a_pid=0x%x ,v_pid=0x%x \n",p_priv->a_pid,p_priv->v_pid);

            break;

          case CAS_IOCMD_CARD_INFO_GET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_CARD_INFO_GET \n");
            p_card_info = (cas_card_info_t *)param;
            if(XinShiMao_GetSMCNO(card_sn) != SUCCESS)
            {
                memset(card_sn,0,XSM_CARD_SN_LEN);
                CAS_XSM_ADT_PRINTF("[xsm], get card id error ! \n");
            }

            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[0]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[1]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[2]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[3]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[4]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[5]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[6]);
            CAS_XSM_ADT_PRINTF("[xsm],CardID:%X \n",card_sn[7]);

            memcpy((char *)(p_card_info->sn),card_sn,XSM_CARD_SN_LEN);

            XinShiMao_GetSMCEntitle(&entitle_status);
            p_card_info->card_state = entitle_status;
            CAS_XSM_ADT_PRINTF("entitle_status = %d\n",entitle_status);

            XinShiMao_GetSMCType(&card_type);
            p_card_info->card_type = card_type;
            CAS_XSM_ADT_PRINTF("card_type = %d\n",card_type);

            card_region_id = XinShiMao_GetCardRegionID();
            p_card_info->card_ctrl_code = card_region_id;
            CAS_XSM_ADT_PRINTF("card_ctrl_code = %d\n",card_region_id);

            break;
         case CAS_IOCMD_MON_CHILD_STATUS_SET:
            p_child_satus = (chlid_card_status_info *)param;
            //ret: 1:invailed card 2:mother card and son card is not paired

            ret = XinShiMao_WriteFeedDataToChild(p_child_satus->feed_data, p_child_satus->length);
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MON_CHILD_STATUS_SET \n");

            break;
         case CAS_IOCMD_READ_FEED_DATA:
            p_child_satus = (chlid_card_status_info *)param;
            ret = XinShiMao_ReadFeedDataFromParent(p_child_satus->feed_data, &p_child_satus->length);
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MON_CHILD_STATUS_SET \n");

            //if return success ret == 0
            break;
          case CAS_IOCMD_CA_OVERDUE:
            ret = XinShiMao_GetEntitleRemainDay();
            break;
          case CAS_IOCMD_MAIL_HEADER_GET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MAIL_HEADER_GET \n");
            for(i = 1; i <= MAX_EMAIL_NUM; i++)
            {
                s_mail_flag[i] = 0;
            }

            p_mail_headers = (cas_mail_headers_t *)param;
            err = xsm_get_mail_header(&p_mail_headers->max_num, p_mail_headers->p_mail_head);
            CAS_XSM_ADT_PRINTF("[xsm] get mail header num: %d \n",p_mail_headers->max_num);
            set_event(p_priv, CAS_C_HIDE_NEW_EMAIL);
            break;

          case CAS_IOCMD_MAIL_BODY_GET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MAIL_BODY_GET \n");
            p_mailcontent = (cas_mail_body_t *)param;
            CAS_XSM_ADT_PRINTF("[xsm],mail_id = %d \n",p_mailcontent->mail_id);
            err = xsm_get_mail_body(p_mailcontent->mail_id, p_mailcontent);
            break;

          case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MAIL_DELETE_BY_INDEX \n");
            mail_index = *(u32 *)param;
            err = XinShiMao_DelEmail(mail_index);
            CAS_XSM_ADT_PRINTF("[delete],mail_id=%d, err = %d \n",mail_index,err);
            s_mail_flag[mail_index] = 0;
            CAS_XSM_ADT_PRINTF("[delete],s_mail_flag[%d]=%d \n",mail_index,s_mail_flag[mail_index]);
            break;

          case CAS_IOCMD_MAIL_DELETE_ALL:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_MAIL_DELETE_ALL \n");
            for(i = 1; i <= MAX_EMAIL_NUM; i++)
            {
                if(s_mail_flag[i] == 1)
                {
                    XinShiMao_DelEmail(i);
                    CAS_XSM_ADT_PRINTF("[xsm] Delete mail id = %d \n",i);
                }
            }
            break;
          case CAS_IOCMD_OSD_MSG_GET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_OSD_MSG_GET \n");
            //break;

          case CAS_IOCMD_OSD_GET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_OSD_GET \n");
            p_msg_info = (msg_info_t *)param;
            memcpy(p_msg_info, &xsm_OsdMsg, sizeof(xsm_OsdMsg));
            //OS_PRINTF("[adt]osd message:%s \n ",(char *)p_msg_info->data);
            break;
          case CAS_IOCMD_ZONE_CHECK:

            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_ZONE_CHECK \n");

            #if 0
            p_nim_info = (nim_channel_info_t *)param;
            CAS_XSM_ADT_PRINTF("\n set tuner_lock_delivery %d\n",p_nim_info->frequency);
            p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                                DEV_IDT_TYPE,
                                                                SYS_DEV_TYPE_NIM);
            //lock main freq
            nim_channel_connect(p_nim_dev, p_nim_info, 0);
            if(p_nim_info->lock != 1)
            {
                  CAS_XSM_ADT_PRINTF("[xsm], nim unlock ! \n");
                  set_event(p_priv, CAS_S_NIM_LOCK_FAIL);
                  set_event(p_priv, CAS_C_CARD_ZONE_CHECK_OK);
                  cas_xsm_ZoneCheckEnd();
            }
            else
            {

                  //filter nit table
                  /*
                  XinShiMao_TableStart(NIT_TABLE_ID,(const UINT8 *)nit_match,
                                                    (const UINT8 *)nit_mask,1,NIT_PID,0);
                                                    */

                  cas_xsm_ZoneCheckEnd();
            }
            #else
                  cas_xsm_ZoneCheckEnd();
            #endif

            break;

          case CAS_IOCMD_FACTORY_SET:
            CAS_XSM_ADT_PRINTF("[xsm] CAS_IOCMD_FACTORY_SET \n");
            xsm_cas_all_ecm_req_free();
            xsm_cas_all_emm_req_free();
            XinShiMao_Initialize(1);
            break;

     }

         return ret;
}

static void cat_parse_descriptor(UINT8 *buf,s16 length)
{
  UINT8 *p_data = buf;
  UINT8  descriptor_tag = 0;
  UINT8  descriptor_length = 0;
  UINT16  ca_system_id = 0;
  UINT16  emm_pid = 0;

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];
    p_data += 2;
    length -= 2;
    if(descriptor_length == 0)
    {
      continue;
    }
    switch(descriptor_tag)
    {
      case 0x09://DVB_XSM_CA:

        ca_system_id = MAKE_WORD(p_data[1], p_data[0]);
        emm_pid = MAKE_WORD(p_data[3],p_data[2] & 0x1F);
        CAS_XSM_ADT_PRINTF("ca_system_id:0x%X  \n",ca_system_id);
        if(CAS_TRUE == XinShiMao_IsMatchCAID(ca_system_id))
        {
          //mtos_task_sleep(2000);
          CAS_XSM_ADT_PRINTF("\n[xsm] EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n",
          emm_pid,ca_system_id);
          cas_xsm_set_emm_pid(emm_pid);
          //XinShiMao_SetEmmPID(CA_INFO_CLEAR,0);
          XinShiMao_SetEmmPID(CA_INFO_ADD,emm_pid);
        }
        
        break;
      default:
        break;
    }
    length -= descriptor_length;
    p_data = p_data + descriptor_length;
  }
}

/*!
  parse ca system id

  \param[in] p_buf , the data pointer.
  \param[in] nDescLen , the data len.
  \param[out] pDesc , ca info desc pointer.
  */
U32 ca_desc_parse(UINT8 *pBuf, INT32 nDescLen, ca_desc_t *pDesc)
{
  UINT8 *p_data = pBuf;
  UINT32 nErr = FALSE;

  if(NULL != pBuf && NULL != pDesc)
  {
      pDesc->CA_system_id = p_data[0]<<8|p_data[1];
      CAS_XSM_ADT_PRINTF("[xsm] CA_system_id = 0x%x \n",pDesc->CA_system_id);
      pDesc->CA_PID = (p_data[2]&0x1F)<<8|p_data[3];
      nErr = TRUE;
  }
    return nErr;
}

/*!
  abc
  */
static void cas_xsm_parse_cat(u8 *p_buf)
{
  u8 *p_data = p_buf;
  u8 version_number = 0;
  u16 length = 0;

  version_number = (u8)((p_data[5] & 0x3E) >> 1);
  length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);


  CAS_XSM_ADT_PRINTF("[CAT] length=%d,version_number=%d\n",
                    length,version_number);
  p_data += 8;

  cat_parse_descriptor(p_data, length - 9);

  CAS_XSM_ADT_PRINTF("\r\nparsing cat over.");
  return;
}

/*!
  parse pmt desc

  \param[in] p_buf , the data pointer.
  \param[in] length , the data len.
  \param[out] program_number .
  */
void pmt_parse_descriptor(UINT8 *p_buf, INT16 length,UINT16 program_number)
{
   UINT8 *p_data = p_buf;
   UINT8 descriptor_tag = 0;
   UINT8 descriptor_length = 0;
   XinShiMao_CAServiceInfo xsmServiceInfo = {0};

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];

    if(0 == descriptor_length)
    {
      break;
    }

    length -= (descriptor_length + 2);
    p_data += 2;
    switch(descriptor_tag)
    {
      case 0x09:
      {
        ca_desc_t Desc = {0};
        ca_desc_parse(p_data,
        descriptor_length,
        &Desc);

        CAS_XSM_ADT_PRINTF("[xsm] ecm_pid = 0x%x \n",Desc.CA_PID);
        if(TRUE == XinShiMao_IsMatchCAID(Desc.CA_system_id) && 
           (cas_xsm_get_ecm_pid() != Desc.CA_PID))
        {
              //XinShiMao_SetEcmPID(CA_INFO_CLEAR, NULL);
              xsmServiceInfo.m_wEcmPid = Desc.CA_PID;
              xsmServiceInfo.m_bServiceCount = 1;
              xsmServiceInfo.m_wServiceId[0] = program_number;

              CAS_XSM_ADT_PRINTF("[xsm] 22222222222XinShiMao_SetEcmPID:0x%x, service id=0x%x \n", 
                    Desc.CA_PID,xsmServiceInfo.m_wServiceId[0]);

              cas_xsm_set_ecm_pid(Desc.CA_PID);

              XinShiMao_SetEcmPID(CA_INFO_ADD, &xsmServiceInfo);
              cas_xsm_set_scramble_flag(1);
         }
         else
         {
              CAS_XSM_ADT_PRINTF("[xsm] not xsmcas \n");
         }
      }
      default:
        break;
    }
    p_data = p_data + descriptor_length;
  }
}


/*!
  parse pmt table

  \param[in] p_buf , the data pointer.
  */
static void cas_xsm_parse_pmt(UINT8 *p_buf)
{
    UINT8   table_id = 0;
    INT16   section_lenxsmh = 0;
    UINT16 program_number = 0;
    UINT8   version_number = 0;
    UINT16 PCR_PID = 0;
    UINT16 program_info_lenxsmh = 0;

    UINT8   stream_type = 0;
    UINT16 elementary_PID = 0;
    UINT8   ES_info_lenxsmh = 0;
    UINT8   *p_data = NULL;

    //GUOTONG_CAServiceInfo xsmServiceInfo = {0};


    if(NULL == p_buf)
    {
         CAS_XSM_ADT_PRINTF("[xsm],xsm_cas_parse_pmt,param is err! \n");
         MT_ASSERT(0);
    }
    p_data = p_buf;

    if(NULL != p_data)
    {
         table_id = p_data[0];
         section_lenxsmh = (((U16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
         if(section_lenxsmh > 0)
         {
              program_number = (U16)p_data[3] << 8 | p_data[4];
              version_number = (p_data[5] & 0x3E) >> 1;
              PCR_PID = ((U16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

              program_info_lenxsmh = ((U16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
              p_data += 12;
              //XinShiMao_SetEcmPID(CA_INFO_CLEAR, NULL);

              cas_xsm_set_scramble_flag(0);

              if(s_xsm_entitle_flag == 1)
              {
                  cas_xsm_cancel_entitle_message();
              }
              s_xsm_entitle_flag = 0;
              pmt_parse_descriptor(p_data, program_info_lenxsmh,program_number);

              p_data += program_info_lenxsmh;
              section_lenxsmh -= (13 + program_info_lenxsmh);//head and crc
              CAS_XSM_ADT_PRINTF("stream_type=0x%x \n",p_data[0]);

              while(section_lenxsmh > 0)
              {
                    stream_type = p_data[0];
                    elementary_PID = ((U16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
                    CAS_XSM_ADT_PRINTF("[xsm] stream_type=0x%x\n",stream_type);
                    CAS_XSM_ADT_PRINTF("[xsm] elementary_PID=0x%x\n",elementary_PID);
                    ES_info_lenxsmh = ((U16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
                    p_data += 5;
                    pmt_parse_descriptor(p_data, ES_info_lenxsmh,program_number);
                    p_data += ES_info_lenxsmh;
                    section_lenxsmh -= (ES_info_lenxsmh + 5);
              }

             cas_xsm_set_server_id(program_number);
    }
  }
}

/*!
  parse table

  \param[in] t_id , table type.
  \param[in] p_buf , data pointer.
  \param[in] p_result , no use.
  */
RET_CODE cas_xsm_table_prase(UINT32 t_id, UINT8 *p_buf, UINT32 *p_result)
{
    UINT16 section_len =  MAKE_WORD(p_buf[2], p_buf[1] & 0x0f) + 3;

    CAS_XSM_ADT_PRINTF("\n  111[xsm]  t_id = %d  table_id = %d \n",t_id, p_buf[0]);
    CAS_XSM_ADT_PRINTF("\n  [xsm]  section_len = %d \n",section_len);
    
    if(t_id == CAS_TID_CAT)
    {
         CAS_XSM_ADT_PRINTF("cas_xsm_table_prase, get cat ! \n");
         memcpy(g_cas_priv.cat_data, p_buf, section_len);
         g_cas_priv.cat_length = section_len;
         cas_xsm_parse_cat(g_cas_priv.cat_data);
    }
    else if(t_id == CAS_TID_PMT)
    {
         CAS_XSM_ADT_PRINTF("cas_xsm_table_prase, get pmt ! \n");
         memcpy(g_cas_priv.pmt_data, p_buf, section_len);
         g_cas_priv.pmt_length = section_len;
         cas_xsm_parse_pmt(g_cas_priv.pmt_data);
    }
    else if(t_id == CAS_TID_NIT)
    {
         CAS_XSM_ADT_PRINTF("cas_xsm_table_prase, get nit ! \n");
    }
    else
    {
         CAS_XSM_ADT_PRINTF("cas_xsm_table_prase, other table ! \n");
    }

    return SUCCESS;
}

/*!
  parse table

  */
RET_CODE cas_xsm_table_resent()
{
    CAS_XSM_ADT_PRINTF(" [xsm] cas_xsm_table_resent ! \n");

    if(g_cas_priv.pmt_length != 0)
    {
        CAS_XSM_ADT_PRINTF("[xsm], g_cas_priv.pmt_length != 0 \n");
    }
    if(g_cas_priv.cat_length != 0)
    {
        CAS_XSM_ADT_PRINTF("[xsm], g_cas_priv.cat_length != 0 \n");
    }
    if(g_cas_priv.nit_length != 0)
    {
        CAS_XSM_ADT_PRINTF("[xsm], g_cas_priv.nit_length != 0 \n");
    }
    return SUCCESS;
}

/*!
  remove card

  */
RET_CODE cas_xsm_card_remove()
{
    CAS_XSM_ADT_PRINTF(" [xsm] xsm ca card remove !! \n");

    xsm_set_card_status(XSM_CA_CARD_REMOVE);

    XinShiMao_SCStatusChange(XSM_CA_CARD_REMOVE);

    xsm_cas_all_emm_req_free();
    xsm_cas_all_ecm_req_free();



    return SUCCESS;
}

RET_CODE cas_xsm_card_reset(UINT32 slot, card_reset_info_t *p_info)
{
    cas_xsm_priv_t *p_priv = NULL;

    CAS_XSM_ADT_PRINTF("[xsm]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    if (NULL == p_info)
    {
        CAS_XSM_ADT_PRINTF("[xsm]%s:LINE:%d, p_info is NULL!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }
    p_priv = g_cas_priv.cam_op[CAS_ID_XSM].p_priv;

    //p_ds_scard_dev = (scard_device_t *)p_info->p_smc;
    scard_pro_register_op((scard_device_t *)p_info->p_smc);

    p_priv->slot = slot;

    //p_priv->card_status = SMC_CARD_INSERT;
    CAS_XSM_ADT_PRINTF("\n [xsm] XSM_CA_CARD_INSERT ! \n");
    xsm_set_card_status(XSM_CA_CARD_INSERT);

    XinShiMao_SCStatusChange(XSM_CA_CARD_INSERT);


    return SUCCESS;
}


/*!
  abc
  */
RET_CODE cas_xsm_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
    cas_xsm_priv_t *p_priv = NULL;

   g_cas_priv.cam_op[CAS_ID_XSM].attached = 1;

   CAS_XSM_ADT_PRINTF("[xsm], attached=%d,  cas_xsm_attach ok ! \n",
                                      g_cas_priv.cam_op[CAS_ID_XSM].attached);

   g_cas_priv.cam_op[CAS_ID_XSM].func.init
    = cas_xsm_init;
   g_cas_priv.cam_op[CAS_ID_XSM].func.deinit
    = cas_xsm_deinit;
   g_cas_priv.cam_op[CAS_ID_XSM].func.identify
    = cas_xsm_identify;
   g_cas_priv.cam_op[CAS_ID_XSM].func.io_ctrl
    = cas_xsm_io_ctrl;
   g_cas_priv.cam_op[CAS_ID_XSM].func.table_process
    = cas_xsm_table_prase;
   g_cas_priv.cam_op[CAS_ID_XSM].func.table_resent
    = cas_xsm_table_resent;

  g_cas_priv.cam_op[CAS_ID_XSM].func.card_remove = cas_xsm_card_remove;
  g_cas_priv.cam_op[CAS_ID_XSM].func.card_reset = cas_xsm_card_reset;

  p_priv = mtos_malloc(sizeof(cas_xsm_priv_t));
  if(p_priv ==  NULL)
  {
      CAS_XSM_ADT_PRINTF("[xsm],p_priv malloc error ! \n");
      return ERR_FAILURE;
  }
  g_cas_priv.cam_op[CAS_ID_XSM].p_priv = (cas_xsm_priv_t *)p_priv;
  memset(p_priv, 0x00, sizeof(cas_xsm_priv_t));
  CAS_XSM_ADT_PRINTF("[xsm],p_priv add:0x%x \n",p_priv);

  p_priv->flash_size = p_cfg->flash_size;
  p_priv->flash_start_adr = p_cfg->flash_start_adr;
  p_priv->nvram_read = p_cfg->nvram_read;
  p_priv->nvram_write = p_cfg->nvram_write;


  *p_cam_id = (UINT32)(&g_cas_priv.cam_op[CAS_ID_XSM]);


#if 0
  if(XinShiMao_Initialize(0) != SUCCESS)
  {
      CAS_XSM_ADT_PRINTF("[xsm], cas_xsm_init error ! \n");
      return ERR_FAILURE;
  }
#endif

  return SUCCESS;
}

