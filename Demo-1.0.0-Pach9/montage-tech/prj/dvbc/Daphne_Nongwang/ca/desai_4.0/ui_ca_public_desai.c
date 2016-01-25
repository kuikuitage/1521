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
#include "ui_ca_entitle_info.h"
#include "ui_channel_warn.h"
#include "ui_nvod_api.h"
#include "ui_browser_api.h"
#include "ui_email_picture.h"
#include "ui_ca_finger.h"
#include "ui_ca_public.h"
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif
//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;
static cas_sid_t g_ca_sid;
static BOOL g_ca_check_start = FALSE;
extern void set_full_scr_ad_status(BOOL is_show);
enum ca_desai_btn_id
{
  IDC_BTN_CA_INFO = 1,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_CA_ENTTITLE_INFO,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_BURSE,
  IDC_BTN_CA_OPERATE,
};



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
  set_full_scr_ad_status(FALSE);
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
  /*stop all ap task,wait ca zone check end*/

  cmd_t cmd = {0};
  dvbc_lock_t tp_set = {0};
  static nim_channel_info_t nim_channel_info = {0};

  sys_status_get_main_tp1(&tp_set);
  nim_channel_info.frequency = tp_set.tp_freq;
  nim_channel_info.param.dvbc.symbol_rate = tp_set.tp_sym;
  nim_channel_info.param.dvbc.modulation = tp_set.nim_modulate;

  ui_release_ap();
  g_ca_check_start = TRUE;
  
  cmd.id = cmd_id;
  cmd.data1 = (u32)(&nim_channel_info);
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_DS);
  OS_PRINTF("\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);

}

RET_CODE on_ca_zone_check_end(void)
{
  /*restart the ap task*/
  ui_activate_ap();
  g_ca_check_start = FALSE;
  
  ui_desktop_start();
  return SUCCESS;
}
void on_ca_zone_check_stop(void)
{
    /**no zone check funtion,it will set msg self**/
    /*todo: need dsca zone check end msg to active ap*/
#if 0
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        /*simulator msg :ca zone check end*/
        ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
    }
  #endif

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

static RET_CODE on_ca_prog_oper(u32 event_id)
{
  switch(event_id)
  {
    case CAS_C_IEXIGENT_PROG:
      UI_PRINTF("[cas stop play]..... play current programe \n");
      if(!g_ca_check_start)
        ui_play_curn_pg();
    break;
    default:
      UI_PRINTF("[cas stop play].....do nothing \n");
    break;
}
  return SUCCESS;
}

control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  u8 i =0;
  u8 accept_bit_num = 7;
  control_t *p_btn[7];
  u16 stxt_btn[7] =
  { 
    IDS_SMARTCARD_INFO, IDS_CA_LEVEL_CONTROL_SET,
    IDS_WOR_DURATION_SET, IDS_ACCREDIT_PRODUCK_INFO, IDS_PWD_MODIFY,
    IDS_SLOT_INFO, IDS_SP_INFO
  };   


  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
//rsc    ctrl_set_rstyle(p_btn[i], RSI_SUB_BUTTON_SH, RSI_COMMON_BTN_HL, RSI_SUB_BUTTON_SH);
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
  switch(ctrlID)
  {
    case IDC_BTN_CA_INFO:
      manage_open_menu(ROOT_ID_CA_CARD_INFO, (u32)para1, 0);
      break;
    case IDC_BTN_CA_LEVEL_CONTROL:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, (u32)para1, 0);
      break;
    case IDC_BTN_CA_WORK_DURATION:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, (u32)para1, 0);
      break;
    case IDC_BTN_CA_ENTTITLE_INFO:
      manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, (u32)para1, 0);
      break;
    case IDC_BTN_CA_PIN_MODIFY:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, (u32)para1, 0);
      break;
    case IDC_BTN_CA_BURSE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, (u32)para1, 0);
      break;
    case IDC_BTN_CA_OPERATE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO, (u32)para1, 0);
      break;  
    default:
      break;
    }
}


RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;

  if(g_ca_last_msg == event_id) //same msg
  {
      return SUCCESS;
  }
  if(event_id != CAS_S_ZONE_CHECK_END &&
      event_id != CAS_C_IEXIGENT_PROG_STOP &&
      event_id != CAS_C_IEXIGENT_PROG)
  {
    g_ca_last_msg = event_id;
  }
  OS_PRINTF("..................................event_id is %ld\n",event_id);
  switch(event_id)
  {
  #ifndef WIN32
    case CAS_S_ZONE_CHECK_END:
      OS_PRINTF("\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
      on_ca_zone_check_end();
      break;
  #endif
    case CAS_C_SHOW_NEW_EMAIL:
      update_email_picture(EMAIL_PIC_SHOW);
      break;
    case CAS_C_HIDE_NEW_EMAIL:
      update_email_picture(EMAIL_PIC_HIDE);
      break;
    case CAS_C_SHOW_SPACE_EXHAUST:
      update_email_picture(EMAIL_PIC_FLASH);
      break;
    case CAS_E_CARD_REGU_INSERT:
      update_ca_message(IDS_CAS_E_CARD_REGU_INSERT);
      break;
    case CAS_E_ILLEGAL_CARD: 
      update_ca_message(IDS_UNKNOW_SMC);
      break;
    case CAS_E_CARD_OVERDUE:
      update_ca_message(IDS_CA_CARD_OVER_DATE);
      break;
    case CAS_E_CARD_INIT_FAIL:
      update_ca_message(IDS_CAS_E_CARD_INIT_FAIL);
      break;
    case CAS_E_NO_OPTRATER_INFO:
      update_ca_message(IDS_NO_SP_IN_SMC);
      break;
    case CAS_E_CARD_FREEZE:
      update_ca_message(IDS_CA_CARD_STOP);
      break;
    case CAS_E_CARD_OUT_WORK_TIME:
      update_ca_message(IDS_CAS_E_CARD_OUT_WORK_TIME);
      break;
    case CAS_E_CARD_CHILD_UNLOCK:
      update_ca_message(IDS_CA_PROG_HIGH_WATCH_LEVEL);
      break;
    case CAS_E_CARD_MODULE_FAIL:
      update_ca_message(IDS_CA_SW1SW2_STB_SN_NOT_MATCH);
      break;
    case CAS_E_PROG_UNAUTH:
      update_ca_message(IDS_CA_NO_PURCH_PROG);
      #if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
        OS_PRINTF("[ca public]Enter CAS_E_PROG_UNAUTH channel \n");
        set_unauth_ad_status(TRUE);
      #endif
      break;  
    case CAS_E_CW_INVALID:
      update_ca_message(IDS_DECRYPTION_FAIL);   
      break; 
    case CAS_E_CADR_NO_MONEY:
      update_ca_message(IDS_CA_NO_MONEY);
      break; 
    case CAS_E_ZONE_CODE_ERR:
      update_ca_message(IDS_AREA_ERROR);
      break; 
    case CAS_E_CARD_SLEEP:
      update_ca_message(IDS_CA_NO_ENABLE);
      break; 
    case CAS_E_CARD_REG_FAILED:
      update_ca_message(IDS_CA_DELETE_CARD);
      break; 
    case CAS_S_CARD_PARTNER_SUCCESS:
      update_ca_message(IDS_CAS_C_MASTERSLAVE_PAIRED_OK);
      break; 
    case CAS_E_CARD_PARTNER_FAILED:
      update_ca_message(IDS_CA_CARD_PARTNER_FAILED_XSD);
      break; 
    case CAS_E_CARD_DIS_PARTNER:
      update_ca_message(IDS_CA_SLAVE_NEED_MASTER_XSD);
      break; 
    case CAS_E_CARD_MATCH_OK:
      update_ca_message(IDS_CAS_S_CARD_PARTNER_SUCCESS);
      break; 
    case CAS_E_CARD_REMATCH:
      update_ca_message(IDS_CA_REPAIR_STB);
      break;   
    case CAS_E_CARD_REMATCH_CANCEL:
      update_ca_message(IDS_CA_CARD_REMATCH_CANCEL);
      break; 
    case CAS_C_EMAIL_FULL:
      update_ca_message(IDS_CA_EMAIL_NO_SPACE);
      break; 
    case CAS_C_CARD_UNLOCK_OK:
      update_ca_message(IDS_CA_SMART_UNLOCK);
      break; 
    case CAS_C_PURSER_UPDATE:
      update_ca_message(IDS_CA_WALLET_RENEW);
      break; 
    case CAS_E_CARD_AUTHEN_OVERDUE:
      update_ca_message(IDS_CA_CARD_NO_MONEY);
      break; 
    case CAS_C_IPP_OVERDUE:
      update_ca_message(IDS_CAS_E_CARD_IPPV_OUT_TIME);
      break; 
     case CAS_S_CLEAR_DISPLAY:
      ui_set_playpg_scrambled(FALSE);
      update_ca_message(RSC_INVALID_ID);
      #if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
        UI_PRINTF("CAS_S_CLEAR_DISPLAY,set unauth ad status to false!! \n");
        set_unauth_ad_status(FALSE);
      #endif
      break;
    case MSG_CA_DS_CLEAR_UNAUTH:
      #if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
        update_ca_message(RSC_INVALID_ID);
      #endif
      break;
    case CAS_S_ADPT_CARD_INSERT:
      g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
      update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_C_IEXIGENT_PROG:
    //case CAS_C_IEXIGENT_PROG_STOP:
      on_ca_prog_oper(event_id);
      break;
    case CAS_S_CARD_UPG_START:
      update_ca_message(IDS_CAS_E_CARD_INIT);
      break;
    case CAS_C_CARD_NEED_UPDATE:
      update_ca_message(IDS_CA_CARD_NEED_UPDATE);
      break;	
    case CAS_E_CARD_NOT_FOUND:
	  if(ui_is_playpg_scrambled())
	  {
      	update_ca_message(IDS_CA_CARD_NOT_INSERT);
	  }
      break;		
    case CAS_E_AUTHEN_UPDATE:
      update_ca_message(IDS_CAS_E_CARD_ENTITLE_UPDATE);
      break;
    case CAS_E_NO_AUTH_DATA:
      update_ca_message(IDS_CAS_E_CARD_CONDITIONAL_LIMIT);
      break;
    case CAS_C_PNET_ERR_LOCK:
      update_ca_message(IDS_CAS_E_CARD_NET_ERR_LOCK);
      break;
    case  CAS_C_SYSTEM_ERR_LOCK:
      update_ca_message(IDS_CAS_E_CARD_SYS_ERR_LOCK);
      break;
    case  CAS_C_PNET_ERR_UNLOCK:
      update_ca_message(IDS_CAS_E_CARD_NET_ERR_UNLOCK);
      break;
    case CAS_C_SYSTEM_ERR_UNLOCK:
      update_ca_message(IDS_CAS_E_CARD_SYS_ERR_UNLOCK);
      break;
    case  CAS_E_AUTHEN_EXPIRED:
      update_ca_message(IDS_CAS_E_CARD_ENTITLE_EXPIRED);
      #if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
        UI_PRINTF("[ca public]Enter CAS_E_AUTHEN_EXPIRED channel \n");
        set_unauth_ad_status(TRUE);
      #endif
      break; 
    case CAS_C_CARD_ADDR_CTL_OPEN:
      update_ca_message(IDS_CAS_E_CARD_ADDR_CTL_OPEN);
      break;
    case CAS_C_CARD_ADDR_CTL_CLOSE:
      update_ca_message(IDS_CAS_E_CARD_ADDR_CTL_CLOSE);
      break;
    case CAS_E_INVALID_PRICE:
      update_ca_message(IDS_CAS_E_CARD_WRITEPURSER_ERROR);
      break;
    case CAS_E_CARD_MONEY_LACK:
      update_ca_message(IDS_CAS_E_CARD_IPPV_NO_MONEY);
      break;
	case CAS_S_ADPT_CARD_REMOVE:
	  if(ui_is_playpg_scrambled())
	  {
	  	update_ca_message(IDS_CA_CARD_NOT_INSERT);
	  }
	  break;
    default:
      //g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
      break;
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

