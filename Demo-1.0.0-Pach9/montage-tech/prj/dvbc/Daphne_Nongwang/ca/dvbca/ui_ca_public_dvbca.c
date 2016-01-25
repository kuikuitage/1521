/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_prog_bar.h"
#include "ui_channel_warn.h"
#include "ui_nvod_api.h"
#include "ui_browser_api.h"
#include "ui_email_picture.h"
#include "ui_ca_finger.h"
#include "ui_ca_public.h"
#include "ui_ca_ppv_icon.h"
#include "ui_conditional_entitle_expire.h"
#include "ui_ca_pair.h"
#include "ui_ca_burse_charge.h"
#include "ui_dvb_conditional_accept_level.h"
#include "ui_dvb_conditional_accept_worktime.h"
#include "ui_dvb_conditional_accept_pin.h"
#include "ui_ca_urgent.h"



//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;
static cas_sid_t g_ca_sid;

enum ca_desai_btn_id
{
  IDC_BTN_CA_INFO = 1,
  IDC_BTN_CA_WORKTIME,
  IDC_BTN_CA_ACCREDIT_INFO,
  IDC_BTN_CA_PIN,
  IDC_BTN_CA_WATCH_LEVEL,
  IDC_BTN_SP_INFO,
};

#ifdef WIN32
static BOOL g_is_smart_card_insert = 1;
#else
static BOOL g_is_smart_card_insert = 0;
#endif

static BOOL g_is_ecm_msg = 0;

BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

void ui_set_smart_card_insert(BOOL p_card_status)
{
  g_is_smart_card_insert = p_card_status;
}

BOOL ui_get_ecm_msg(void)
{
  return g_is_ecm_msg;
}

void ui_set_ecm_msg(BOOL p_ecm_msg)
{
  g_is_ecm_msg = p_ecm_msg;
}


void ui_set_ca_last_msg_valid(u32 i)
{
    g_ca_last_msg = i;
}

u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
}

void ui_reset_ca_last_msg(void)
{
    g_ca_last_msg = INVALID_U32;
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
 cmd_t cmd = {CAS_CMD_PLAY_INFO_SET_ANSYNC};

  OS_PRINTF("CA: set sid pgid: %d\n", p_ca_sid->pgid);

  memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

  cmd.data1 = (u32)(&g_ca_sid);
  
  ap_frm_do_command(APP_CA, &cmd);
}

static void ui_release_ap(void)
{
  ui_time_release();
  ui_epg_stop();
  ui_epg_release();
  ui_release_signal();
  ui_enable_uio(FALSE);
}

static void ui_activate_ap(void)
{
  ui_time_init();
  ui_epg_init();
  ui_epg_start();
  ui_init_signal();
  ui_enable_uio(TRUE);
}
void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
   ui_release_ap();
  #if 0
  cmd_t cmd = {0};
  dvbc_lock_t tp_set = {0};
  static nim_channel_info_t nim_channel_info = {0};

  sys_status_get_main_tp1(&tp_set);
  nim_channel_info.frequency = tp_set.tp_freq;
  nim_channel_info.param.dvbc.symbol_rate = tp_set.tp_sym;
  nim_channel_info.param.dvbc.modulation = tp_set.nim_modulate;
  /*stop all ap task,wait ca zone check end*/
  ui_release_ap();
  
    cmd.id = cmd_id;
    cmd.data1 = (u32)(&nim_channel_info);
    cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_DS);
    OS_PRINTF("\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                      cmd.id,cmd.data1,cmd.data2);
    ap_frm_do_command(APP_CA, &cmd);
    #endif

}

RET_CODE on_ca_zone_check_end(void)
{
  /*restart the ap task*/
 ui_activate_ap();
 ui_desktop_start();
  return SUCCESS;
}
void on_ca_zone_check_stop(void)
{
    /**no zone check funtion,it will set msg self**/
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        /*simulator msg :ca zone check end*/
        ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
    }
}
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{
  #if 0
  cmd_t cmd = {0};

  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_BY);
  OS_PRINTF("\r\n***ui_cas_factory_set cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                    cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
  #endif
}

control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  u8 i =0;
  u8 accept_bit_num = 6;
  control_t *p_btn[6];
  u16 stxt_btn[6] =
  { 
      IDS_SMARTCARD_INFO,
	  IDS_CA_WORK_TIME_SET,
	  IDS_ACCREDIT_INFO,
      IDS_PIN_MODIFY,
      IDS_WATCH_LEVE,
      IDS_SP_INFO,
  };   


  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_BTN_CA_INFO+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
    ctrl_set_rstyle(p_btn[i], RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECTH, RSI_SN_SET_BUTTON_SELECT);
    text_set_font_style(p_btn[i], FSI_COMM_TXT_N, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], stxt_btn[i]);
    y += (35+4);
    ctrl_set_related_id(p_btn[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              accept_bit_num) %
                             accept_bit_num + 1),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % accept_bit_num + 1));/* down */
  }
  return (p_btn[0]);
}

void on_ca_frm_btn_select_number(u8 ctrlID, u32 para1 )
{
  if(ui_is_smart_card_insert() == FALSE)
  {
    ui_comm_cfmdlg_open2(NULL,IDS_CA_CARD_NOT_INSERT, NULL,0);
    return;
  }
  switch(ctrlID)
  {
    case IDC_BTN_CA_INFO:
      manage_open_menu(ROOT_ID_CA_CARD_INFO, 0, 0);
      break;
      
    case IDC_BTN_CA_WORKTIME:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
      break;

    case IDC_BTN_CA_ACCREDIT_INFO:
      manage_open_menu(ROOT_ID_SP_LIST, 0, 0);
      break;

    case IDC_BTN_CA_PIN:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
      break;

    case IDC_BTN_CA_WATCH_LEVEL: 
     	manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
      break;

    case IDC_BTN_SP_INFO:
      manage_open_menu(ROOT_ID_SP_LIST2,0,0);
    default:
      break;
    }
}

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;
  BOOL card_is_removed = FALSE;
  
  rect_t ca_notify_rect =
  {
    250, 125,
    260 + NOTIFY_CONT_W,
    125 + NOTIFY_CONT_H,
  };
  if((g_ca_last_msg == event_id) && (event_id != CAS_S_CLEAR_DISPLAY)) //same msg
  {
      return SUCCESS;
  }
  if(event_id != CAS_S_ZONE_CHECK_END)
  {
    g_ca_last_msg = event_id;
  }
  OS_PRINTF("(ui_get_curpg_scramble_flag()) : %d\n",ui_get_curpg_scramble_flag());
  switch(event_id)//from CCTB_HUNAN
  {
    case CAS_S_ADPT_CARD_INSERT://card insert
      OS_PRINTF("CAS_S_ADPT_CARD_INSERT\n");
      ui_set_smart_card_insert(TRUE);
	  update_ca_message(RSC_INVALID_ID);
  	  ui_reset_ca_last_msg();
      ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CAS_S_CARD_INSERT);
      card_is_removed = FALSE;
      break;
    case CAS_S_ADPT_CARD_REMOVE://���Ƴ�
      OS_PRINTF("CAS_S_ADPT_CARD_REMOVE\n");
      ui_set_smart_card_insert(FALSE);
    {
      control_t *p_cont = NULL;
	  update_ca_message(RSC_INVALID_ID);
  	  ui_reset_ca_last_msg();
      if((p_cont = fw_find_root_by_id(ROOT_ID_CA_FINGER)) != NULL)
      {
        ui_set_finger_show(FALSE);
        close_ca_finger_menu();
      }
      if((p_cont = fw_find_root_by_id(ROOT_ID_DIALOG)) != NULL)
      {
        ui_comm_dlg_close();
      }
      if((p_cont = fw_find_root_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG)) != NULL)
      {
        close_ca_entitle_expire_dlg();
      }
      if((p_cont = fw_find_root_by_id(ROOT_ID_CA_BURSE_CHARGE)) != NULL)
      {
        close_ca_burse_dlg();
      }
      card_is_removed = TRUE;
      ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CAS_S_CARD_REMOVE);
    }
    set_uiokey_status(FALSE);
    break;
    case CAS_C_EMM_APP_STOP://���ܿ�ֹͣ����
      OS_PRINTF("CAS_C_EMM_APP_STOP\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_APP_STOP);
      break;
    case CAS_C_EMM_APP_RESUME://���ܿ��ָ�����
      OS_PRINTF("CAS_C_EMM_APP_RESUME\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_APP_RESUME);
      break;
    case CAS_C_EMM_PROG_UNAUTH://ȡ����Ŀ����
      OS_PRINTF("CAS_C_EMM_PROG_UNAUTH\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_PROG_UNAUTH);
      break;
    case CAS_C_EMM_PROG_AUTH://���ӽ�Ŀ����
      OS_PRINTF("CAS_C_EMM_PROG_AUTH\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_PROG_AUTH);
      break;
    case CAS_C_EMM_UPDATE_GNO://���ܿ���Ÿ���
      OS_PRINTF("CAS_C_EMM_UPDATE_GNO\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_GNO);
      break;
    case CAS_S_CARD_NEED_UPG://���ܿ�������β忨
      OS_PRINTF("CAS_S_CARD_NEED_UPG\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_GK);
      break;
    case CAS_S_MODIFY_ZONECODE://���������
      OS_PRINTF("CAS_S_MODIFY_ZONECODE\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_CARD_AREA_CODE_UPDATE);
      break;
    case CAS_C_NOTIFY_UPDATE_CTRLCODE://���ܿ��޲������
      OS_PRINTF("���ܿ��޲������!\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_MODIFY_CTRLCODE);
      break;
    case CAS_C_NOTIFY_UPDATE_CRT://��Ȩ����
      OS_PRINTF("��Ȩ����!\n");
      close_notify();
      ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID,IDS_CA_CAS_CARD_ENTITLE_UPDATE);
      break;
    case CAS_C_SHOW_NEW_EMAIL://���ʼ�
      OS_PRINTF("CAS_C_SHOW_NEW_EMAIL\n");
      update_email_picture(EMAIL_PIC_SHOW);
      break;
    case CAS_C_EN_PARTNER://���û���ƥ��
      OS_PRINTF("CAS_C_EN_PARTNER\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_START_PAIR);
      break;
    case CAS_C_DIS_PARTNER://ȡ������ƥ��
      OS_PRINTF("CAS_C_DIS_PARTNER\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_CARD_REMATCH_CANCEL);
      break;
    case CAS_C_SET_SON_CARD://������ĸ��
      OS_PRINTF("������ĸ��!\n");
      ui_set_notify(&ca_notify_rect,NOTIFY_TYPE_STRID,IDS_CA_SET_MON_SON_CARD);
      break;
    case CAS_S_ZONE_CHECK_END:
      OS_PRINTF("\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
	  #ifndef WIN32
      on_ca_zone_check_end();//rsc
      #else
	  ui_desktop_start();
	  #endif
      break;
    case CAS_C_HIDE_NEW_EMAIL:
      OS_PRINTF("CAS_C_HIDE_NEW_EMAIL\n");
      update_email_picture(EMAIL_PIC_HIDE);
      break;
    case CAS_S_CLEAR_DISPLAY:
      OS_PRINTF("CAS_S_CLEAR_DISPLAY\n");
      ui_set_playpg_scrambled(FALSE);
      update_ca_message(RSC_INVALID_ID);
      ui_set_ecm_msg(FALSE);
      break;
    default:
      OS_PRINTF("on_ca_message_update :   default : event_id = [%d]\n",event_id);
      break;
  }

  if(ui_get_curpg_scramble_flag()) 
  {
    ui_set_ecm_msg(TRUE);
    switch(event_id)//from CCTB_HUNAN
    {
      case CAS_E_CADR_NOT_ACT://�뼤���ӿ�
        OS_PRINTF("CAS_E_CADR_NOT_ACT\n");
        update_ca_message(IDS_CAS_SON_CADR_NOT_ACT);
        break;
	  case CAS_E_CARD_REGU_INSERT:/*���Ž�Ŀ����������ܿ�*/
	  	OS_PRINTF("���Ž�Ŀ����������ܿ�\n");
      	update_ca_message(IDS_CAS_E_CARD_REGU_INSERT);
     	break;
      case CAS_C_AUTH_LOGO_DISP://��ĿԤ��Ȩ
        OS_PRINTF("��ĿԤ��Ȩ!\n");
        manage_open_menu(ROOT_ID_CA_PRE_AUTH, 0, 0);
        break;
	  case CAS_E_NO_OPTRATER_INFO:/*���в����ڽ�Ŀ��Ӫ��*/
		OS_PRINTF("���в����ڽ�Ŀ��Ӫ��\n");
		update_ca_message(IDS_NO_SP_IN_SMC);
		break;
	  case CAS_E_ZONE_DISPLAY:/*�������*/
		OS_PRINTF("�������\n");
		update_ca_message(IDS_CA_AREA_DISPLAY);
		break;
	  case CAS_E_PASSWORD_ERROR:/*��Ӫ���������*/
		OS_PRINTF("��Ӫ���������\n");
		update_ca_message(IDS_CA_SP_ERR_PWD);
		break;
	  case CAS_E_CARD_CHILD_UNLOCK:/*��Ŀ��������趨�ۿ�����*/
	  	OS_PRINTF("��Ŀ��������趨�ۿ�����\n");
      	update_ca_message(IDS_CA_PROG_HIGH_WATCH_LEVEL);
      	break;
	  case CAS_E_CARD_OVERDUE:/*���ܿ�����*/
	  	OS_PRINTF("���ܿ�����\n");
      	update_ca_message(IDS_CA_CARD_OVER_DATE);
      	break;
	  case CAS_E_CW_INVALID:/*��Ŀ����ʧ��*/
	  	OS_PRINTF("��Ŀ����ʧ��\n");
        update_ca_message(IDS_DECRYPTION_FAIL);   
        break; 
	  case CAS_E_CARD_OUT_WORK_TIME:/*���ڹ���ʱ����*/
	  	OS_PRINTF("���ڹ���ʱ����\n");
      	update_ca_message(IDS_CAS_E_CARD_OUT_WORK_TIME);
      	break;
	  case CAS_C_IPP_OVERDUE:/*PPV����*/
	  	OS_PRINTF("PPV����\n");
      	update_ca_message(IDS_CAS_E_CARD_IPPV_OUT_TIME);
      	break;
	  case CAS_C_PPC_LESS:/*PPVʣ�಻��10��*/
	  	OS_PRINTF("PPVʣ�಻��10��\n");
      	update_ca_message(IDS_CA_AUTH_LESS_TEN_DAY);
      	break;
      case CAS_C_UPDATE_BALANCE://IPPV��Ŀ
        OS_PRINTF("IPPV��Ŀ\n");
        //manage_open_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
        manage_open_menu(ROOT_ID_CA_PPV_ICON, 0, 0);
        break;
      case CAS_E_ZONE_CODE_ERR://�������
        OS_PRINTF("CAS_E_ZONE_CODE_ERR\n");
        update_ca_message(IDS_CA_AREA_CODE_ERR);
        break;
      case CAS_E_CADR_NO_MONEY://���ܿ�����
        OS_PRINTF("CAS_E_CADR_NO_MONEY\n");
        update_ca_message(IDS_CA_CARD_BALANCE_LESS);
        break;
      case CAS_E_PROG_UNAUTH://Ƶ��δ��Ȩ
        OS_PRINTF("CAS_E_PROG_UNAUTH\n");
		OS_PRINTF("error num %d\n",__LINE__);
      	update_ca_message(IDS_CA_SW1SW2_ECM_NOT_AUTHORIZED);
        break;
      case CAS_E_CARD_AUTH_FAILED:
        OS_PRINTF("���ܿ���Ȩ����\n");
        update_ca_message(IDS_CA_ILLEGAL_AUTH);
        break;
      case CAS_E_CARD_LOCKED://���ܿ�����
        OS_PRINTF("CAS_E_CARD_LOCKED\n");
        update_ca_message(IDS_CAS_E_CARD_LOCKED);
        break;
      case CAS_E_SERVICE_LOCKED://�����ѱ���
        OS_PRINTF("CAS_E_SERVICE_LOCKED\n");
        update_ca_message(IDS_CAS_E_SERVICE_LOCKED);
        break;
      case CAS_E_EXTER_AUTHEN://���ܿ���֤ʧ��
        OS_PRINTF("CAS_E_EXTER_AUTHEN\n");
        update_ca_message(IDS_CAS_E_CARD_PARTNER_FAILED);
        break;
      case CAS_C_IPPT_BUY_OR_NOT://IPP��Ϣ
        OS_PRINTF("CAS_C_IPPT_BUY_OR_NOT\n");
        update_ca_message(IDS_IPPV_PROG);
		#ifndef WIN32
        ui_ca_get_info(CAS_CMD_IPPV_INFO_SYNC_GET, 0,0);
		#endif
        break;
      case CAS_E_CARD_NOT_FOUND://������ƥ��
        OS_PRINTF("CAS_E_CARD_NOT_FOUND\n");
        update_ca_message(IDS_CA_E31);
        break;
      case CAS_E_CARD_FAILED://���ܿ�����ʧ��
        OS_PRINTF("CAS_E_CARD_FAILED\n");
        update_ca_message(IDS_CAS_E_CARD_UPG_FAILED);
        break;
      case CAS_E_NOT_REG_CARD://��������ܿ�
        OS_PRINTF("CAS_E_NOT_REG_CARD\n");
        update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);
        break;
      case CAS_S_ADPT_CARD_INSERT://���ܿ�����,�����ʾ��Ϣ
        OS_PRINTF("CAS_S_ADPT_CARD_INSERT\n");
        update_ca_message(RSC_INVALID_ID);
        break;
      case CAS_C_ENTITLE_CLOSED://��Ȩ�ر�
        update_ca_message(IDS_CA_AUTH_CLOSED);
        break;
      case CAS_S_CARD_AUTHEN_UPDATING://��Ȩˢ��
        update_ca_message(IDS_CA_AUTH_UPDATE);
        break;
      case CAS_E_CARD_RENEW_RESET_STB://���ܿ�״̬���£�������
        update_ca_message(IDS_CA_STATUS_RENEW);
        break;
      case CAS_E_CARD_AUTHEN_OVERDUE:
        OS_PRINTF("CAS_E_CARD_AUTHEN_OVERDUE\n");
        update_ca_message(IDS_CA_SW1SW2_EXPIRED);
        break;
      case CAS_E_CARD_DATA_GET:// ���ܿ����ݴ���
        OS_PRINTF("���ܿ����ݴ���!\n");      
        update_ca_message(IDS_CA_CARD_DATA_ERROR);
        break;
      case CAS_E_ILLEGAL_CARD://�޷�ʶ�𿨣�����ʹ��
        OS_PRINTF("�޷�ʶ�𿨣�����ʹ��!\n");
        update_ca_message(IDS_UNKNOW_SMC);
        break;
      case CAS_C_USERCDN_LIMIT_CTRL_ENABLE://���û��������޲�
        OS_PRINTF("���û��������޲�!\n");
        update_ca_message(IDS_CAS_E_CARD_CONDITIONAL_LIMIT);
        break;
      case CAS_E_CARD_OPERATE_FAILE://���ܿ�����ʧ��
        OS_PRINTF("���ܿ�����ʧ��!\n");
        update_ca_message(IDS_CA_OPERATE_CARD_FAIL);
        break;
      case CAS_E_AUTHEN_EXPIRED://�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����
        OS_PRINTF("�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����\n");
        if(ui_is_fullscreen_menu(fw_get_focus_id()))
        {
           manage_open_menu(ROOT_ID_CA_ENTITLE_EXP_DLG, CAS_E_AUTHEN_EXPIRED, 0);
        }
        break;
      default:
        ui_set_ecm_msg(FALSE);
        OS_PRINTF("on_ca_message_update : ui_get_curpg_scramble_flag == 1  default : event_id = [%d]\n",event_id);
        break;
      }
  }
  else
  {
    g_ca_last_msg = INVALID_U32;
  }
  return SUCCESS;
}

RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{
  return SUCCESS;
}


RET_CODE open_conditional_accept_pair(u32 para1, u32 para2)
{
return SUCCESS;
}
RET_CODE open_conditional_accept_mother(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_conditional_accept_info(u32 para1, u32 para2)
{
  return SUCCESS;
}
RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2)
{
  return SUCCESS;
}
