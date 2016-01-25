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
#include "ui_new_mail.h"
#include "ui_auto_feed.h"
#include "ui_ca_public.h"
#include "sys_dbg.h"

static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;


#define TENGRUI_CONDITIONAL_ACCEPT_BTN_CNT  5
enum tr_ca_btn_id
{
  IDC_BTN_CA_CARD_INFO = 1,
  IDC_BTN_CA_INFO ,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_MOTHER,
};
static BOOL g_is_smart_card_insert = FALSE;
static u32 g_smart_card_rate = 0;
static cas_sid_t g_ca_sid;

extern BOOL g_son_card_need_feed_msg; 

u16 pwdlg_keymap_in_ca(u16 key);                            

RET_CODE pwdlg_proc_in_ca(control_t *ctrl, u16 msg, u32 para1, u32 para2);

comm_pwdlg_data_t pwdlg_data =
{
  ROOT_ID_BACKGROUND,
  (PWD_DLG_FOR_PLAY_X + 120),
  (PWD_DLG_FOR_PLAY_Y + 100),
  IDS_PLS_INPUT_PIN,
  PWDLG_T_CA,
  pwdlg_keymap_in_ca,
  pwdlg_proc_in_ca,
  //0,
  //6,
  //0,
  //PWDLG_T_CA
}; 

u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
}
BOOL ui_set_smart_card_insert(BOOL status)
{
    g_is_smart_card_insert = status;
    return 0;
    
}
BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

u32 ui_get_smart_card_rate(void)
{
  return g_smart_card_rate;
}

void ui_set_smart_card_rate(u32 level)
{
  g_smart_card_rate = level;
}

RET_CODE on_conditional_accept_level_update_d(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if (para1 != SUCCESS)
  {
    return ERR_FAILURE;
  }
  g_smart_card_rate = para2;
  
  return SUCCESS;
}

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
     /***if no ca zone check function,it must send cas zone check end msg itself***/
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
    }
}
void ui_reset_ca_last_msg(void)
{
   g_ca_last_msg = INVALID_U32;
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

void on_ca_zone_check_end(void)
{
#if 0
TRCA
    /****make sure into ota time out to start play*****/
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        ctrl_process_msg(p_ctrl, MSG_CA_ZONE_CHECK_END, 0, 0);
    }
#endif
	ui_desktop_start();
}

void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{

}
static void on_auto_feed_msg(void)
{
  u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  if(ui_is_fullscreen_menu(curn_root_id))
  {
    open_auto_feed(0, 0);
  }
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
 cmd_t cmd = {CAS_CMD_PLAY_INFO_SET_ANSYNC};

  OS_PRINTF("CA: set sid pgid: %d\n", p_ca_sid->pgid);

  memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

  cmd.data1 = (u32)(&g_ca_sid);
  
  ap_frm_do_command(APP_CA, &cmd);
}

/*
control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  u8 i =0;
  u8 accept_bit_num = TENGRUI_CONDITIONAL_ACCEPT_BTN_CNT;
  control_t *p_btn[TENGRUI_CONDITIONAL_ACCEPT_BTN_CNT];
  u16 stxt_btn[TENGRUI_CONDITIONAL_ACCEPT_BTN_CNT] =
  { 
    IDS_CA_CARD_INFO,IDS_CA_INFO2, IDS_LEVEL_CONTROL_INFO,
    IDS_PIN_MODIFY, IDS_SMARTCARD_FEED,
  };;


  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
    ctrl_set_rstyle(p_btn[i], RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_SUB_BUTTON_SH, RSI_COMMON_BTN_HL, RSI_SUB_BUTTON_SH);
    text_set_font_style(p_btn[i], FSI_COMM_TXT_N, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], stxt_btn[i]);
    y += (35+4);
    ctrl_set_related_id(p_btn[i],
                        0,                                    //left 
                        (u8)((i - 1 +
                              accept_bit_num) %
                             accept_bit_num + 1),           // up 
                        0,                                     // right 
                        (u8)((i + 1) % accept_bit_num + 1)); // down 

  }
  return (p_btn[0]);
}

*/
/*
void on_ca_frm_btn_select_number(u8 ctrlID, u32 para1 )
{
  switch(ctrlID)
  {
    case IDC_BTN_CA_CARD_INFO:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO, 0, 0); 
      break;
    case IDC_BTN_CA_INFO:
      manage_open_menu(ROOT_ID_PROVIDER_INFO, 0, 0);
      break;

    case IDC_BTN_CA_LEVEL_CONTROL:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
      break;

    case IDC_BTN_CA_PIN_MODIFY:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
      break;

    case IDC_BTN_CA_MOTHER:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
      break;

    default:
      break;
  }

}

*/
RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
   u32 event_id = para2;
   cas_module_id_t ca_module = para1;
    
   //prompt_type_t prompt_type;
   //u8 focus_id = 0;
   OS_PRINTF("wwwww event_id[%d]\n",event_id);
   switch(ca_module)
   {
     case CAS_ID_TR:
       OS_PRINTF("on_ca_message_update CAS_ID_TR    event_id[%d]\n",event_id);
       break;
  
     case CAS_UNKNOWN:
       OS_PRINTF("on_ca_message_update CAS_UNKNOWN  event_id[%d]\n",event_id);
       break;
  
     default:
       OS_PRINTF("on_ca_message_update CAS_OTHER    event_id[%d]\n",event_id);
       break;
   }
   
   if(TRUE == g_son_card_need_feed_msg)
   {
      on_auto_feed_msg();
      return SUCCESS;
   }

   if(event_id != CAS_S_ZONE_CHECK_END)
  {
    g_ca_last_msg = event_id;
  }
   
   switch(event_id)
   {
     case CAS_E_CARD_REGU_INSERT:
       OS_PRINTF("CAS_E_CARD_REGU_INSERT\n");
      // update_ca_message(IDS_CAS_E_CARD_REGU_INSERT);
       break;
     case CAS_S_ZONE_CHECK_END:
      on_ca_zone_check_end();
      break;
     case CAS_C_NOTIFY_EXPIRY_STATE:
       OS_PRINTF("CAS_C_NOTIFY_EXPIRY_STATE\n");
       update_ca_message(IDS_CARD_BECOME_DUE);
       break;
     case CAS_E_CARD_UPG_FAILED:
       OS_PRINTF("CAS_E_CARD_UPG_FAILED\n");
       update_ca_message(IDS_CAS_E_CARD_UPG_FAILED);
       break;  
     case CAS_E_CARD_RST_FAIL:
       OS_PRINTF("CAS_E_CARD_RST_FAIL\n");
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_CAS_E_CARD_RST_FAIL);
       break; 
     //case CAS_E_CARD_CHILD_UNLOCK:
       //OS_PRINTF("CAS_E_CARD_CHILD_UNLOCK\n");
       //update_ca_message(IDS_CAS_E_CARD_CHILD_UNLOCK);
       //break;
     //case CAS_E_CARD_OUT_WORK_TIME:
       //OS_PRINTF("CAS_E_CARD_OUT_WORK_TIME\n");
       //update_ca_message(IDS_CAS_E_CARD_OUT_WORK_TIME);
       //break;
     //case CAS_E_NO_OPTRATER_INFO:
       //OS_PRINTF("CAS_E_NO_OPTRATER_INFO\n");
       //update_ca_message(IDS_CAS_E_NO_OPTRATER_INFO);
       //break;
     case CAS_E_CADR_NO_MONEY:
       OS_PRINTF("CAS_E_CADR_NO_MONEY\n");
       update_ca_message(IDS_CAS_E_CADR_NO_MONEY);
       break;
     case CAS_E_SERVICE_LOCKED:
       OS_PRINTF("CAS_E_SERVICE_LOCKED\n");
       update_ca_message(IDS_CAS_E_SERVICE_LOCKED);
       break;
     case CAS_E_CADR_NOT_ACT:
       OS_PRINTF("CAS_E_CADR_NOT_ACT\n");
       update_ca_message(IDS_CAS_E_CADR_NOT_ACT);
       break;
     case CAS_E_ZONE_CODE_ERR:
       OS_PRINTF("CAS_E_ZONE_CODE_ERR\n");
       update_ca_message(IDS_CAS_E_ZONE_CODE_ERR);
       break;
     case CAS_E_NOT_RECEIVED_SPID:
       OS_PRINTF("CAS_E_NOT_RECEIVED_SPID\n");
       update_ca_message(IDS_CAS_E_NOT_RECEIVED_SPID);
       break;
     //case CAS_E_IPPV_CANNTDEL:
       //OS_PRINTF("CAS_E_IPPV_CANNTDEL\n");
       //update_ca_message(IDS_CAS_E_IPPV_CANNTDEL);
       //break;  
     //case CAS_E_CW_INVALID:
       //OS_PRINTF("CAS_E_CW_INVALID\n");
       //update_ca_message(IDS_CAS_E_CW_INVALID);
       //break;  
     case CAS_E_PIN_INVALID:
       OS_PRINTF("CAS_E_PIN_INVALID\n");
       update_ca_message(IDS_CAS_E_PIN_INVALID);
       break;   
     //case CAS_E_IC_CMD_FAIL:
       //OS_PRINTF("CAS_E_IC_CMD_FAIL\n");
       //update_ca_message(IDS_CAS_E_IC_CMD_FAIL);
       //break; 
     //case CAS_E_SYSTEM_ID_INVALID:
       //OS_PRINTF("CAS_E_SYSTEM_ID_INVALID\n");
       //update_ca_message(IDS_CAS_E_SYSTEM_ID_INVALID);
       //break;  
     //case CAS_E_CARD_PAIROTHER:
       //OS_PRINTF("CAS_E_CARD_PAIROTHER\n");
       //update_ca_message(IDS_CAS_E_CARD_PAIROTHER);
       //break;   
     case CAS_ERROR_CARD_NOTSUPPORT:
       OS_PRINTF("CAS_ERROR_CARD_NOTSUPPORT\n");
       update_ca_message(IDS_CAS_ERROR_CARD_NOTSUPPORT);
       break;  
     //case CAS_E_FEEDTIME_NOT_ARRIVE:
       //OS_PRINTF("CAS_E_FEEDTIME_NOT_ARRIVE\n");
       //update_ca_message(IDS_CAS_E_FEEDTIME_NOT_ARRIVE);
       //break; 
     case CAS_E_IPP_INFO_ERROR:
       OS_PRINTF("CAS_E_IPP_INFO_ERROR\n");
       update_ca_message(IDS_CAS_E_IPP_INFO_ERROR);
       break;   
     case CAS_E_FEED_DATA_ERROR:
       OS_PRINTF("CAS_E_FEED_DATA_ERROR\n");
       update_ca_message(IDS_CAS_E_FEED_DATA_ERROR);
       break;   
     //case CAS_E_ACCESS_CARD_ERROR:
       //OS_PRINTF("CAS_E_ACCESS_CARD_ERROR\n");
       //update_ca_message(IDS_CAS_E_ACCESS_CARD_ERROR);
       //break; 
     case CAS_S_CARD_RST_SUCCESS:
       OS_PRINTF("CAS_S_CARD_RST_SUCCESS\n");
       update_ca_message(IDS_CAS_S_CARD_RST_SUCCESS);
       break;   
     case CAS_S_CARD_UPG_SUCCESS:
       OS_PRINTF("CAS_S_CARD_UPG_SUCCESS\n");
       update_ca_message(IDS_CAS_S_CARD_UPG_SUCCESS);
       break;   
     case CAS_S_ADPT_CARD_REMOVE:
       OS_PRINTF("CAS_S_ADPT_CARD_REMOVE\n");
       g_is_smart_card_insert = FALSE;
       g_smart_card_rate = 0;
       //update_ca_message(IDS_CAS_S_CARD_REMOVE);
       if(ui_is_playpg_scrambled())
       {
         update_ca_message(IDS_E04);
		 OS_PRINTF("----CAS_S_ADPT_CARD_REMOVE\n");
       }
       break;  
     case CAS_S_ADPT_CARD_INSERT: 	
       OS_PRINTF("CAS_S_ADPT_CARD_INSERT\n");
       g_is_smart_card_insert = TRUE;   
       break;  
     case CAS_S_MODIFY_ZONECODE:
       OS_PRINTF("CAS_S_MODIFY_ZONECODE\n");
       update_ca_message(IDS_CAS_S_MODIFY_ZONECODE);
       break;  
     //case CAS_S_CARD_UPDATE_BALANCE:
       //OS_PRINTF("CAS_S_CARD_UPDATE_BALANCE\n");
       //update_ca_message(IDS_CAS_S_CARD_UPDATE_BALANCE);
       //break;   
     //case CAS_S_MOTHER_CARD_REGU_INSERT:
       //OS_PRINTF("CAS_S_MOTHER_CARD_REGU_INSERT\n");
       //update_ca_message(IDS_CAS_S_MOTHER_CARD_REGU_INSERT);
       //break;   
     //case CAS_S_SON_CARD_REGU_INSERT:
       //OS_PRINTF("CAS_S_SON_CARD_REGU_INSERT\n");
       //update_ca_message(IDS_CAS_S_SON_CARD_REGU_INSERT);
     //case CAS_S_CLEAR_DISPLAY:
       //OS_PRINTF("CAS_S_CLEAR_DISPLAY\n");
       //ui_set_playpg_scrambled(FALSE);
       //update_signal();
       //break;
     case CAS_S_SEVICE_NOT_SCRAMBLED:
       OS_PRINTF("CAS_S_SEVICE_NOT_SCRAMBLED\n");
        update_ca_message(IDS_E00);
       break;
     case CAS_S_CARD_MODULE_REMOVE:
       OS_PRINTF("CAS_S_CARD_MODULE_REMOVE\n");
        update_ca_message(IDS_E01);  
       break;
     case CAS_E_CARD_MODULE_EEPROM_FAILED:
       OS_PRINTF("CAS_E_CARD_MODULE_EEPROM_FAILED\n");
        update_ca_message(IDS_E02);  
       break;
       case CAS_E_CARD_MODULE_FAIL:
       OS_PRINTF("CAS_E_CARD_MODULE_FAIL\n");
        update_ca_message(IDS_E03);  
       break;
     case CAS_S_CARD_REMOVE:
        OS_PRINTF("CAS_S_CARD_REMOVE\n");
        update_ca_message(IDS_E04);  
        break;      
     case CAS_E_ILLEGAL_CARD:
       OS_PRINTF("CAS_E_ILLEGAL_CARD\n");  
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_E05);
       break;
     case CAS_E_CARD_INIT_FAIL:
       OS_PRINTF("CAS_E_CARD_INIT_FAIL\n");  
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_E06);
       break;
     case CAS_S_CARD_CHECKING:
       OS_PRINTF("CAS_S_CARD_CHECKING\n");  
       update_ca_message(IDS_E07);
       break;
     case CAS_E_CARD_EEPROM_FAILED:
       OS_PRINTF("CAS_E_CARD_EEPROM_FAILED\n");  
       update_ca_message(IDS_E09);
       break;
     case CAS_E_PMT_FORMAT_WRONG:
       OS_PRINTF("CAS_E_PMT_FORMAT_WRONG\n");  
       update_ca_message(IDS_E10);
       break;
     case CAS_E_CARD_DIS_PARTNER:
       OS_PRINTF("CAS_E_CARD_DIS_PARTNER\n");  
       update_ca_message(IDS_E11);
       break;
     case CAS_C_SON_CARD_NEED_FEED:
       OS_PRINTF("CAS_C_SON_CARD_NEED_FEED\n");  
       g_son_card_need_feed_msg = TRUE;
       //update_ca_message(IDS_E12);
       on_auto_feed_msg();
       break;
     case CAS_E_SEVICE_NOT_AVAILABLE:
       OS_PRINTF("CAS_E_SEVICE_NOT_AVAILABLE\n");  
       update_ca_message(IDS_E13);
       break;
     case CAS_E_PROG_UNAUTH:
       OS_PRINTF("CAS_E_PROG_UNAUTH\n");  
       update_ca_message(IDS_E14);
       break;
     case CAS_S_PROG_AUTH_CHANGE:
       OS_PRINTF("CAS_S_PROG_AUTH_CHANGE\n");  
       update_ca_message(IDS_E15);
       break;
     case CAS_E_SEVICE_SCRAMBLED:
       OS_PRINTF("CAS_E_SEVICE_SCRAMBLED\n");  
       update_ca_message(IDS_E16);
       break;
     case CAS_E_SEVICE_SCRAMBLED2:
       OS_PRINTF("CAS_E_SEVICE_SCRAMBLED2\n");  
       update_ca_message(IDS_E17);
       break;
     case CAS_E_ERROR_E18:
       OS_PRINTF("CAS_E_ERROR_E18\n");  
       update_ca_message(IDS_E18);
       break;
     case CAS_E_ERROR_E19:
       OS_PRINTF("CAS_E_ERROR_E19\n");  
       update_ca_message(IDS_E19);
       break;
     case CAS_E_NOT_ALLOWED_BROADCASTER:
       OS_PRINTF("CAS_E_NOT_ALLOWED_BROADCASTER\n");  
       update_ca_message(IDS_E20);
       break;
     case CAS_E_ERROR_E21:
       OS_PRINTF("CAS_E_ERROR_E21\n");  
       update_ca_message(IDS_E21);
       break;
     case CAS_E_ERROR_E22:
       OS_PRINTF("CAS_E_ERROR_E22\n");  
       update_ca_message(IDS_E22);
       break;
     case CAS_S_SERVICE_DISCRAMBLING:
       OS_PRINTF("CAS_S_SERVICE_DISCRAMBLING\n");  
       update_ca_message(IDS_E23);
       break;
     case CAS_E_NOT_ALLOWED_REGION:
       OS_PRINTF("CAS_E_NOT_ALLOWED_REGION\n");  
       update_ca_message(IDS_E24);
       break;
     case CAS_E_CARD_PARTNER_FAILED:
       OS_PRINTF("CAS_E_CARD_PARTNER_FAILED\n");  
       update_ca_message(IDS_E25);
       break;
     case CAS_E_SEVICE_UNKNOWN:
       OS_PRINTF("CAS_E_SEVICE_UNKNOWN\n");  
       update_ca_message(IDS_E26);
       break;
     case CAS_E_SEVICE_NOT_RUNNING:
       OS_PRINTF("CAS_E_SEVICE_NOT_RUNNING\n");  
       update_ca_message(IDS_E27);
       break;
     case CAS_E_CARD_LOCKED:
       OS_PRINTF("CAS_E_CARD_LOCKED\n");  
       update_ca_message(IDS_E28);
       break;
     case CAS_E_ERROR_E29:
       OS_PRINTF("CAS_E_ERROR_E29\n");  
       update_ca_message(IDS_E29);
       break;
     case CAS_E_ERROR_E30:
       OS_PRINTF("CAS_E_ERROR_E30\n");  
       update_ca_message(IDS_E30);
       break;
     case CAS_S_ADPT_CARD_RST_SUCCESS:
       OS_PRINTF("CAS_S_ADPT_CARD_RST_SUCCESS\n");
       ui_ca_get_info(CAS_CMD_RATING_GET, 0, 0);
       break;
     case CAS_E_PARENTAL_CTRL_LOCKED:
       //manage_close_menu();
       OS_PRINTF("CAS_E_PARENTAL_CTRL_LOCKED\n");
       update_ca_message(IDS_E31);
//don't open pwd dialog
#if 0
       if(fw_get_focus_id() == ROOT_ID_BACKGROUND
        || fw_get_focus_id() == ROOT_ID_PROG_LIST
        ||fw_get_focus_id() == ROOT_ID_FAV_LIST)
       {
         if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL) 
           ui_comm_pwdlg_open(&pwdlg_data);
       }
       else if(fw_get_focus_id() == ROOT_ID_PROG_BAR)
       {
         manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
         if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL)
           ui_comm_pwdlg_open(&pwdlg_data);
       }
#endif
       break;
     case CAS_E_NOT_ALLOWED_COUNTRY:
       OS_PRINTF("CAS_E_NOT_ALLOWED_COUNTRY\n");  
       update_ca_message(IDS_E32);
       break;
     case CAS_E_NO_AUTH_DATA:
       OS_PRINTF("CAS_E_NO_AUTH_DATA\n");  
       update_ca_message(IDS_E33);
       break;
      case CAS_E_NO_AUTH_STB:
       OS_PRINTF("CAS_E_NO_AUTH_STB\n");  
       update_ca_message(IDS_E34);
       break;
      case CAS_E_NO_SIGNAL:
       OS_PRINTF("CAS_E_NO_SIGNAL\n");  
       update_ca_message(IDS_E35);
       break;
     case CAS_E_ADPT_ILLEGAL_CARD:
       OS_PRINTF("CAS_E_ADPT_ILLEGAL_CARD\n");  
       g_is_smart_card_insert = FALSE;
       if(ui_is_playpg_scrambled())
       {
         update_ca_message(IDS_E05);
       }
       break;
     case CAS_C_SHOW_NEW_EMAIL:
       OS_PRINTF("CAS_C_SHOW_NEW_EMAIL\n");
       ui_set_new_mail(TRUE);
       open_ui_new_mail(0, 0);
  /*no need to show mail in background
      prompt_type.message_type = 2;
       prompt_type.index = 0;
       
       focus_id = fw_get_focus_id();
     
       if((focus_id == ROOT_ID_BACKGROUND || focus_id == ROOT_ID_PROG_BAR) &&
         (focus_id != ROOT_ID_CA_PROMPT ))
       {
         manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, 0);
       }
  */
       break;
     default:
       OS_PRINTF("default CAS_EVENT\n");
       break;
   }
  
    return SUCCESS;

}


void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
 }


RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  OS_PRINTF("on_ca_init_ok\n");
  ui_set_smart_card_insert(TRUE);
  fw_tmr_create(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE, 10*1000, FALSE);
  return SUCCESS;
}

RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2)
{
 return SUCCESS;
}
RET_CODE open_conditional_accept_mother(u32 para1, u32 para2)
{
  return SUCCESS;
}
RET_CODE open_conditional_accept_pair(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_ca_info(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_ca_operate(u32 para1, u32 para2)
{
  return SUCCESS;
}


