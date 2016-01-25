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
#include "ui_ca_public.h"
#include "ui_email_picture.h"
#include "ui_ca_card_update.h"
#include "ui_ca_curtain.h"
#include "ui_ca_card_info.h"

//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;
static u8 *g_ca_feed_status = NULL;
static cas_sid_t g_ca_sid;

#ifdef WIN32
static cas_operators_info_t xxx;
static cas_operators_info_t *g_oper_info = &xxx;
static  cas_card_work_time_t *g_worktime_info = NULL;
#else
static cas_operators_info_t *g_oper_info = NULL;
static  cas_card_work_time_t *g_worktime_info = NULL;
#endif

#ifdef WIN32
static BOOL g_is_smart_card_insert = 0;
#else
static BOOL g_is_smart_card_insert = 1;
#endif
static u32 g_smart_card_rate = 0;
static BOOL g_email_full_flag = FALSE;
static u32 start_ticks = 0;
static u8 g_card_upg_is_open = 0;
/*
enum ca_btn_id
{
  IDC_BTN_CA_CARD_INFO = 1,
  IDC_BTN_CA_INFO2,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_STB_PAIR,
  IDC_BTN_CARD_MANAGE,
};
*/
//#define YXSB_CONDITIONAL_ACCEPT_BTN_CNT      7

u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
}

void g_ca_set_feed_status(u8 *status)
{
  g_ca_feed_status = status;
}


void ui_reset_ca_last_msg(void)
{
    g_ca_last_msg = INVALID_U32;
}

BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
  cmd_t cmd = {CAS_CMD_SID_SET_ANSYNC};
  UI_PRINTF("CA: set sid pgid: %d\n", p_ca_sid->pgid);

  memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

  cmd.data1 = (u32)(&g_ca_sid);
  
  ap_frm_do_command(APP_CA, &cmd);

}



void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
  #if 0
   /***if no ca zone check function,it must send cas zone check end msg itself***/
  control_t *p_ctrl = NULL;
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  if(NULL != p_ctrl)
  {
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
  }
  
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
  OS_PRINTF("#@@@@on_ca_zone_check_end !!! \n ");
  ui_desktop_start();
  return SUCCESS;
}
void on_ca_zone_check_stop(void)
{
#if 1
  /**no zone check funtion,it will set msg self**/
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
  cmd_t cmd = {0};

  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_TF);
  OS_PRINTF("\r\n***ui_cas_factory_set cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                    cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
}
/*
control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{

#define SN_SUBMENU_W          640
#define SN_SUBMENU_H          480
#define SN_SUBMENU_TITLE_H	  57
#define SN_HELP_H			  43
#define SN_SUBMENU_BTNICON_X(x) 	(x+(SN_SUBMENU_BTN_W/2)/2)
#define SN_SUBMENU_BTNICON_Y(y)	(y+(SN_SUBMENU_BTN_H/2)/2)
#define SN_SUBMENU_BTNICON_W		30
#define SN_SUBMENU_BTNICON_H	  30
#define SN_SUBMENU_TITILHELP	102
#define SN_SUBMENU_BTN_W 	((SN_SUBMENU_W-20)/2)
#define SN_SUBMENU_BTN_H 	61
#define SN_SUBMENU_BTN_X1 	0
#define SN_SUBMENU_BTN_Y 	0
#define SN_CA_INFO_CNT	7

  u16 x;
  u8 i =0;
  u8 accept_bit_num = YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
  control_t *p_btn[YXSB_CONDITIONAL_ACCEPT_BTN_CNT];
  u16 stxt_btn[YXSB_CONDITIONAL_ACCEPT_BTN_CNT] =
  { 
    IDS_CA_CARD_INFO,IDS_SP_INFO,IDS_WATCH_LEVE, IDS_PIN_MODIFY,
    IDS_WOR_DURATION_SET, IDS_STB_PAIR, IDS_CARD_MANAGE
  };
----------------------------------------------------------------------------
  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
    //QQ  ctrl_set_rstyle(p_btn[i], RSI_SUB_BUTTON_SH, RSI_COMMON_BTN_HL, RSI_SUB_BUTTON_SH);
	ctrl_set_rstyle(p_btn[i], RSI_SN_BG, RSI_ORANGE, RSI_SN_BG);
	text_set_font_style(p_btn[i], FSI_COMM_TXT_N, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], stxt_btn[i]);
    y += (35+4);
    ctrl_set_related_id(p_btn[i],
                        0,                                     //left 
                        (u8)((i - 1 +
                              accept_bit_num) %
                             accept_bit_num + 1),          // up 
                        0,                                         // right 
                        (u8)((i + 1) % accept_bit_num + 1)); // down 
     if(i >= YXSB_CONDITIONAL_ACCEPT_BTN_CNT)
    {
        ctrl_set_attr(p_btn[i], OBJ_ATTR_INACTIVE);
    }
  }

------------------------------------------------------------------------------


  for(i = 0; i < SN_CA_INFO_CNT; i++)
  {
      if((i <= SN_CA_INFO_CNT/2))
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2;
        y = SN_SUBMENU_BTN_Y + i*SN_SUBMENU_BTN_H ;
      }
      else
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2+SN_SUBMENU_BTN_W;
        y = SN_SUBMENU_BTN_Y + (((i - 1) - SN_CA_INFO_CNT/2)*SN_SUBMENU_BTN_H);
      }

      //button
      p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1 + i),
                                x, y, SN_SUBMENU_BTN_W,SN_SUBMENU_BTN_H,
                                p_ca_frm, 0);
      ctrl_set_rstyle(p_btn[i], RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECTH,RSI_SN_SET_BUTTON_SELECT);
     // ctrl_set_keymap(p_item, card_info_item_keymap);
     // ctrl_set_proc(p_item, card_info_item_proc);
      text_set_font_style(p_btn[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
      text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
       text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
       text_set_content_by_strid(p_btn[i], stxt_btn[i]);
	       ctrl_set_related_id(p_btn[i],
                        0,                                     //left 
                        (u8)((i - 1 +
                              accept_bit_num) %
                             accept_bit_num + 1),          // up 
                        0,                                         // right 
                        (u8)((i + 1) % accept_bit_num + 1)); // down 
	 if(i >= YXSB_CONDITIONAL_ACCEPT_BTN_CNT)
    {
        ctrl_set_attr(p_btn[i], OBJ_ATTR_INACTIVE);
    }
  	
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
      manage_open_menu(ROOT_ID_CA_CARD_INFO,(u32)para1, 0);
    break;
    
    case IDC_BTN_CA_INFO2:
     manage_open_menu(ROOT_ID_CA_OPE, (u32)para1, 0);
    break;

    case IDC_BTN_CA_LEVEL_CONTROL:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, (u32)para1, 0);
      break;

    case IDC_BTN_CA_PIN_MODIFY:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, (u32)para1, 0);
      break;
      
    case IDC_BTN_CA_WORK_DURATION:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, (u32)para1, 0);
      break;
      
    case IDC_BTN_STB_PAIR:
    manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, (u32)para1, 0);
    break;
    
    case IDC_BTN_CARD_MANAGE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, (u32)para1, 0);
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
  static u8 ca_card_remove = 0;
  #if 1
  rect_t ca_notify_rect =
  {
    250, 125,
    260 + NOTIFY_CONT_W,
    125 + NOTIFY_CONT_H,
  };
  #endif
  OS_PRINTF("\r\n*** ca_message_update  g_ca_last_msg[0x%x],event_id[0x%x]\r\n",
                  g_ca_last_msg,event_id);
  
  if(g_ca_last_msg == event_id) //same msg
  {
      return SUCCESS;
  }
  
  if(event_id != CAS_S_ZONE_CHECK_END)
  {
    g_ca_last_msg = event_id;
  }
  switch(event_id)
  {
    case CAS_S_ZONE_CHECK_END:
      OS_PRINTF("\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
      #ifndef WIN32
        on_ca_zone_check_end();
      #endif
      break;
    case CAS_C_SHOW_NEW_EMAIL:
      OS_PRINTF("新邮件\n");
    //QQ  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_SHOW_EMAIL);
      update_email_picture(EMAIL_PIC_SHOW);
      break;
    case CAS_C_HIDE_NEW_EMAIL:
      OS_PRINTF("隐藏新邮件\n");
      update_email_picture(EMAIL_PIC_HIDE);
      break;
    case CAS_C_SHOW_SPACE_EXHAUST:
      OS_PRINTF("邮件已满\n");
      UI_PRINTF("CAS_C_SHOW_SPACE_EXHAUST\n");
  //QQ    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SHOW_EMAIL, 1000, TRUE);
      start_ticks = mtos_ticks_get();
      g_email_full_flag = TRUE;
      //update_email_picture(EMAIL_PIC_FLASH);
      break;
    case CAS_E_CARD_REGU_INSERT:
      update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);
      OS_PRINTF("加扰节目，请插入智能卡 \n");
      break;
    //case CAS_E_ADPT_ILLEGAL_CARD:
      //if(ui_get_curpg_scramble_flag())
      //{
        //update_ca_message(IDS_INSERT_SMC);
      //}
      //break;
    case CAS_S_ADPT_CARD_REMOVE:
      if(ui_get_curpg_scramble_flag())
      {
        ca_card_remove = 1;
        ui_stop_play(STOP_PLAY_BLACK,TRUE);
        //update_ca_message(IDS_INSERT_SMC);
      }
      break;
    case CAS_E_CARD_INIT_FAIL:
      update_ca_message(IDS_CA_CHECK_FAILED); //QQ IDS_CA_CHECK_FAILED
      OS_PRINTF("智能卡校验失败，请联系运营商 ");
      break;
    case CAS_E_PROG_UNAUTH: //智能卡用户无授权
      update_ca_message(IDS_NO_ENTITLEMENT);
      OS_PRINTF(" 没有授权 \n");
      break;
    //case CAS_C_DETITLE_RECEIVED://收到反授权确认码
      //update_ca_message(IDS_NO_ENTITLEMENT);
      //OS_PRINTF(" 收到反授权确认码 \n");
      //break;
    case CAS_E_ZONE_CODE_ERR:
      update_ca_message(IDS_AREA_ERROR);
      OS_PRINTF("区域不正确 \n");
      break;
    case CAS_E_ILLEGAL_CARD://无法识别卡 
      update_ca_message(IDS_CA_UNIDENTIFIED_CARD);//QQ  IDS_CA_UNIDENTIFIED_CARD
      OS_PRINTF("无法识别卡\n");
      break;
    case CAS_E_UNKNOW_ERR://未知错误
      update_ca_message(IDS_CAS_E_UNKNOWN_ERROR);
      OS_PRINTF("未知错误!\n");
      break;
    case CAS_S_CLEAR_DISPLAY:
      OS_PRINTF("CAS_S_CLEAR_DISPLAY!!!ui_get_curpg_scramble_flag = %d\n",ui_get_curpg_scramble_flag());
      ui_play_curn_pg();
      update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_S_ADPT_CARD_INSERT:
      ca_card_remove = 0;
      //update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_E_CARD_DIS_PARTNER:
      update_ca_message(IDS_CA_NOT_PAIRED);//QQ IDS_CA_NOT_PAIRED
      OS_PRINTF("智能卡与本机顶盒不对应\n");
      break;
    case CAS_E_CARD_CHILD_UNLOCK:
      update_ca_message(IDS_CA_OUT_RATING);//QQ  IDS_CA_OUT_RATING
      OS_PRINTF("节目级别高于设定的观看级别\n");
      break;
    case CAS_E_CARD_OUT_WORK_TIME:
      update_ca_message(IDS_CA_OUT_WORKING_HOURS);//QQ  IDS_CA_OUT_WORKING_HOURS
      OS_PRINTF("当前时段被设定为不能观看\n");
      break;
    case CAS_E_VIEWLOCK:
      update_ca_message(IDS_CA_SWITCH_FREQ);//QQ IDS_CA_SWITCH_FREQ
      OS_PRINTF("请勿频繁切换频道 \n");
      break;
    case CAS_E_BLACKOUT:
      update_ca_message(IDS_CONDITIAN_BAN);
      OS_PRINTF("条件禁播 \n");
      break;
    case CAS_E_CADR_NO_MONEY:
      update_ca_message(IDS_NO_ENOUGH_MONEY);
      OS_PRINTF("卡内金额不足\n");
      break;
    case CAS_E_CARD_STBLOCKED:
      update_ca_message(IDS_CA_STB_REBOOT);//QQ IDS_CA_STB_REBOOT
      OS_PRINTF(" 请重启机顶盒 \n");
      break;
    case CAS_E_CARD_OVERDUE:
      update_ca_message(IDS_CA_CARD_OVER_DATE);
      OS_PRINTF("智能卡过期，请更换新卡\n");
      break;
    case CAS_E_NO_OPTRATER_INFO:
      update_ca_message(IDS_NO_SP_IN_SMC);
      OS_PRINTF("卡中不存在节目运营商 \n");
      break;
    case CAS_E_CW_INVALID:
      update_ca_message(IDS_DECRYPTION_FAIL);   
      OS_PRINTF(" 节目解密失败 \n");
      break; 
    case CAS_S_MOTHER_CARD_REGU_INSERT:
      //ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_SLAVE_NEED_PAIR);
      update_ca_message(IDS_CA_INSERT_PARENT_CARD); //QQ  IDS_CA_INSERT_PARENT_CARD
      OS_PRINTF("请插入母卡  \n");
      break;
    case CAS_S_SON_CARD_REGU_INSERT:
      if(ui_is_fullscreen_menu(fw_get_focus_id()))
        ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_INSERT_CHILD_CARD);//QQ  IDS_CA_INSERT_CHILD_CARD
      //update_ca_message(IDS_CA_INSERT_CHILD_CARD);
      OS_PRINTF("请插入子卡  \n");
      break;
    case CAS_E_FEED_DATA_ERROR:
      ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_INVALID_PARENT_CARD);//QQ  IDS_CA_INVALID_PARENT_CARD
      //update_ca_message(IDS_CA_INVALID_PARENT_CARD);
      OS_PRINTF("读取母卡数据失败\n");
      break;
    case CAS_S_CARD_UPGRADING:
      update_ca_message(IDS_CA_CARD_UPGRADING);  //QQ  IDS_CA_CARD_UPGRADING
      OS_PRINTF(" 智能卡升级中，请不要拔卡或者关机  \n");
      break;
    case CAS_S_CARD_NEED_UPG:
      update_ca_message(IDS_CA_CARD_UPGRAD); //QQ  IDS_CA_CARD_UPGRAD
      OS_PRINTF(" 请升级智能卡 \n");
      break;
    case CAS_E_CARD_SLEEP:
      update_ca_message(IDS_CA_CARD_SLEEP); //QQ  IDS_CA_CARD_SLEEP
      OS_PRINTF(" 智能卡暂时休眠，请5分钟后重新开机 \n");
      break;
    case CAS_E_CARD_FREEZE:
      update_ca_message(IDS_CA_CARD_FROZEN);//QQ IDS_CA_CARD_FROZEN
      OS_PRINTF(" 智能卡已冻结，请联系运营商 \n");
      break;
    case CAS_E_CARD_PAUSE:
      update_ca_message(IDS_CA_CARD_STOPED);//QQ IDS_CA_CARD_STOPED
      OS_PRINTF(" 智能卡已暂停，请回传收视记录给运营商 \n");
      break;;
    case CAS_E_CARD_CURTAIN:
      update_ca_message(IDS_CA_CARD_PREVIEW);//QQ IDS_CA_CARD_PREVIEW
      OS_PRINTF(" 高级预览节目，该阶段不能免费观看 \n");
      break;
    case CAS_E_CARD_TESTSTART:
      update_ca_message(IDS_CA_CARD_UPG_TEST);//QQ IDS_CA_CARD_UPG_TEST
      OS_PRINTF(" 升级测试卡测试中...  \n");
      break;
    case CAS_E_CARD_TESTFAILD:
      update_ca_message(IDS_CA_CARD_UPG_TEST_FAIL);//QQ  IDS_CA_CARD_UPG_TEST_FAIL
      OS_PRINTF("  升级测试卡测试失败，请检查机卡通讯模块 \n");
      break;
    case CAS_E_CARD_TESTSUCC:
      update_ca_message(IDS_CA_CARD_UPG_TEST_SUCCESS);//QQ IDS_CA_CARD_UPG_TEST_SUCCESS
      OS_PRINTF("  升级测试卡测试成功 \n");
      break;
    case CAS_E_CARD_NOCALIBOPER:
      update_ca_message(IDS_CA_CARD_CUSTOM_OPER);//QQ  IDS_CA_CARD_CUSTOM_OPER
      OS_PRINTF(" 卡中不存在移植库定制运营商 \n");
      break;
    case CAS_E_CARD_STBFREEZE:
      update_ca_message(IDS_CA_STB_FROZEN); //QQ  IDS_CA_STB_FROZEN
      OS_PRINTF(" 机顶盒被冻结 \n");
      break;
    case CAS_E_CARD_CURTAIN_OK:
      OS_PRINTF("打开窗帘 \n");
   //   manage_open_menu(ROOT_ID_CA_CARD_CURTAIN, 0, 0); 
      break;
    case CAS_E_CARD_CURTAIN_CANCLE:
      OS_PRINTF("关闭窗帘\n");
      close_ca_card_curtain();
      break;
    default:
      //g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
      OS_PRINTF("default evernt == %d!!!\n",event_id);
      break;
  }

   return SUCCESS;
}

/*
RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{
  return SUCCESS;
}
RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2)
{
  return SUCCESS;
}
RET_CODE open_conditional_accept_pin(u32 para1, u32 para2)
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

RET_CODE open_conditional_accept_level(u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_conditional_accept_info(u32 para1, u32 para2)
{
  return SUCCESS;
}*/
RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2)
{
  return SUCCESS;
}

void g_set_operator_info(cas_operators_info_t *p)
{
  g_oper_info = p;
}

cas_operators_info_t *g_get_operator_info()
{
  #ifdef WIN32
  xxx.max_num = 2;
  xxx.p_operator_info[0].operator_count = 2;
  xxx.p_operator_info[0].operator_id = 1;
  strcpy(xxx.p_operator_info[0].operator_name,"sxsxsxsxsxsxsxsxsxsx");
  xxx.p_operator_info[1].operator_count = 2;
  xxx.p_operator_info[1].operator_id = 2;
  strcpy(xxx.p_operator_info[1].operator_name,"WWWWWWWWWWWWWWWWWWWx");
  #endif
  return g_oper_info;
}

void g_set_worktime_info(cas_card_work_time_t *p)
{
  g_worktime_info = p;
}

cas_card_work_time_t *g_get_worktime_info()
{
  return g_worktime_info;
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

void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  p_info->ecm_num = (pg.ecm_num < 8 ? (u8)pg.ecm_num : 8);
  p_info->emm_num = (tp.emm_num < 8 ? (u8)tp.emm_num : 8);

  memcpy(&p_info->ecm_info, &pg.cas_ecm, p_info->ecm_num * sizeof(cas_desc_t));

  memcpy(&p_info->emm_info, &tp.emm_info, p_info->emm_num * sizeof(ca_desc_t));
}

static RET_CODE ui_force_play_by_ca(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
  cmd_t cmd;
  static play_param_t g_pb_info = {0};
  cas_force_channel_t *p_force_channel_info = NULL;
  u32 video_pid = 0;
  u16 audio_pid = 0;
  
  p_force_channel_info = (cas_force_channel_t *) para2;
  
  UI_PRINTF("@@@@@ ui_force_play_by_ca\n");
  if(p_force_channel_info->CompArr[0].comp_type == 1)
  {
    video_pid = p_force_channel_info->CompArr[0].comp_PID;
  }
  else if(p_force_channel_info->CompArr[0].comp_type == 3)
  {
    audio_pid = p_force_channel_info->CompArr[0].comp_PID;
  }
  
  if(p_force_channel_info->CompArr[1].comp_type == 1)
  {
    video_pid = p_force_channel_info->CompArr[1].comp_PID;
  }
  else if(p_force_channel_info->CompArr[1].comp_type == 3)
  {
    audio_pid = p_force_channel_info->CompArr[1].comp_PID;
  }
  ui_stop_play(STOP_PLAY_BLACK,TRUE);
  
  g_pb_info.lock_mode = SYS_DVBC;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.freq=  p_force_channel_info->frequency*100;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.sym= p_force_channel_info->symbol_rate/10;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.nim_modulate = p_force_channel_info->modulation;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.is_on22k = 0;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.nim_type = 0;
  g_pb_info.inner.dvbs_lock_info.tp_rcv.polarity = 0;
  
  g_pb_info.pg_info.s_id = 0x1fff;
  g_pb_info.pg_info.tv_mode= sys_status_get()->av_set.tv_mode;
  g_pb_info.pg_info.v_pid = video_pid;
  g_pb_info.pg_info.a_pid = audio_pid;
  g_pb_info.pg_info.pcr_pid = p_force_channel_info->pcr_pid;
  g_pb_info.pg_info.pmt_pid = 0x1fff;
  g_pb_info.pg_info.audio_track = 0x1;
  g_pb_info.pg_info.audio_volume = 31;
  g_pb_info.pg_info.audio_type = 0x2;
  g_pb_info.pg_info.is_scrambled = TRUE;
  g_pb_info.pg_info.video_type = 0x0;
  g_pb_info.pg_info.aspect_ratio = 0x1;
  g_pb_info.pg_info.context1 = 0x1;
  g_pb_info.pg_info.context2 = 0x0;
  UI_PRINTF("@@@@@ frequency:%d, symbol_rate:%d, modulation:%d, serv_id:%d, video_id:%d, audio_id:%d, pcr_pid:%d\n",
                  g_pb_info.inner.dvbs_lock_info.tp_rcv.freq, g_pb_info.inner.dvbs_lock_info.tp_rcv.sym, p_force_channel_info->modulation, g_pb_info.pg_info.s_id,
                  video_pid, audio_pid, p_force_channel_info->pcr_pid);
  //===============================
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_pb_info;
  cmd.data2 = 0;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID),g_pb_info.pg_info.audio_volume);
  {
    cas_sid_t ca_sid;
    memset(&ca_sid, 0, sizeof(cas_sid_t));   
    ca_sid.num = 1;
    ca_sid.pg_sid_list[0] = g_pb_info.pg_info.s_id;
    ca_sid.tp_freq = g_pb_info.inner.dvbs_lock_info.tp_rcv.freq;
    ca_sid.a_pid = g_pb_info.pg_info.a_pid;
    ca_sid.v_pid = g_pb_info.pg_info.v_pid;
    ca_sid.pmt_pid = g_pb_info.pg_info.pmt_pid;
    ca_sid.ca_sys_id = 0x1FFF;
    ui_ca_set_sid(&ca_sid);
  }
  update_signal();
  return SUCCESS;
}

RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 view_count = 0, pg_id = 0;
  u16 cur_pg_id = 0;
  u8 view_id;
  u8 i = 0;
  u8 focus_id = 0;
  u8 find_pg_flag = 0;
  u32 video_pid = 0;
  u16 audio_pid = 0;
  dvbs_tp_node_t tp = {0};
  RET_CODE ret = ERR_FAILURE;
  dvbs_prog_node_t pg = {0};
  cas_force_channel_t *p_force_channel_info = NULL;
  p_force_channel_info = (cas_force_channel_t *) para2;
  UI_PRINTF("@@@@@ msg:%d \n", msg);
  //enable uio
  if(msg == MSG_CA_UNFORCE_CHANNEL_INFO)
  {
    set_uiokey_status(FALSE);
    return SUCCESS;
  }
   
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  UI_PRINTF("@@@@@ cur_pg_id:%d, view_count:%d\n", cur_pg_id, view_count);
  for(i=0; i<view_count; i++)
  {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
     {
        
        if(p_force_channel_info->CompArr[0].comp_type == 1)
        {
          video_pid = p_force_channel_info->CompArr[0].comp_PID;
        }
        else if(p_force_channel_info->CompArr[0].comp_type == 3)
        {
          audio_pid = p_force_channel_info->CompArr[0].comp_PID;
        }
        
        if(p_force_channel_info->CompArr[1].comp_type == 1)
        {
          video_pid = p_force_channel_info->CompArr[1].comp_PID;
        }
        else if(p_force_channel_info->CompArr[1].comp_type == 3)
        {
          audio_pid = p_force_channel_info->CompArr[1].comp_PID;
        }
        
        ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
        //MT_ASSERT(ret == SUCCESS);
       UI_PRINTF("@@@@@ freq1:%d, sym1:%d, freq1:%d, sym1:%d\n", 
                       tp.freq, tp.sym, p_force_channel_info->frequency, p_force_channel_info->symbol_rate);
        
       UI_PRINTF("@@@@@ pcr_pid:%d, video_pid:%d, audio_pid::0x%x, pcr_pid:%d, video_pid:%d, audio_pid:0x%x\n", 
                       pg.pcr_pid, pg.video_pid, pg.audio[0].p_id, p_force_channel_info->pcr_pid, video_pid, audio_pid);
        if(pg.pcr_pid== p_force_channel_info->pcr_pid && pg.video_pid== video_pid && pg.audio[0].p_id == audio_pid
          && tp.freq == p_force_channel_info->frequency*100 && tp.sym== p_force_channel_info->symbol_rate/10)
        {  
            find_pg_flag = 1;
            UI_PRINTF("@@@@@ enter force!!!!!!!!!!pg_id == %d\n",pg_id);
            focus_id = fw_get_focus_id();
            if(!ui_is_fullscreen_menu(focus_id))
            {
              ui_close_all_mennus();
            }
            ui_play_prog(pg_id, FALSE);
            fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, pg_id);
            
            if(pg_id != cur_pg_id)
              manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
            ret = SUCCESS;
            break;
        }
     }
  }

  if(find_pg_flag == 0)
  {
    UI_PRINTF("@@@@@ find_pg_flag == 0!!!!!!!!!!\n");
    ui_force_play_by_ca(p_ctrl, msg, para1, para2);
  }
  //disable uio
  if(msg == MSG_CA_FORCE_CHANNEL_INFO)
  {
    set_uiokey_status(TRUE);
  }
  
  update_signal();
  return ret;
}

RET_CODE on_ca_card_update_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;
  u8 percent = ((para2 & 0XFF));
  u8 title_type = ((para2 >> 8) & (0XFF));
  utc_time_t card_upg_start_time = {0};
  
  UI_PRINTF("@@@@@ on_ca_card_update percent:%d\n", percent);
  if(title_type == 1 && percent == 0)
  {
    time_get(&card_upg_start_time, FALSE);
    UI_PRINTF("@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
                 card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
                 card_upg_start_time.hour, card_upg_start_time.minute);
    sys_status_set_card_upg_time(card_upg_start_time);
    sys_status_set_card_upg_state(0);
  }
  
  //card upg
  if(title_type == 2)
  {
    if(percent == 0)
    {
      time_get(&card_upg_start_time, FALSE);
      UI_PRINTF("@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
                 card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
                 card_upg_start_time.hour, card_upg_start_time.minute);
      sys_status_set_card_upg_time(card_upg_start_time);
      sys_status_set_card_upg_state(0);
    }
    if(percent == 100)
    {
      time_get(&card_upg_start_time, FALSE);
      UI_PRINTF("@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
                 card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
                 card_upg_start_time.hour, card_upg_start_time.minute);
      sys_status_set_card_upg_time(card_upg_start_time);
      sys_status_set_card_upg_state(1);
    }
  }

  if((percent == 0) || (percent == 1))
  {
    if(!g_card_upg_is_open)
    {
      ui_close_all_mennus();
      open_ca_card_update_info(0, para2);
      g_card_upg_is_open = 1;
    }
  }
  else if(percent > 100)
  {
    if(fw_find_root_by_id(ROOT_ID_CA_CARD_UPDATE) != NULL)
    {
      fw_destroy_mainwin_by_id(ROOT_ID_CA_CARD_UPDATE);
      g_card_upg_is_open = 0;
    }
  }

  return ret;
}

RET_CODE on_mail_full(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(((mtos_ticks_get() - start_ticks)/100) % 2 ==0 )
  {
    update_email_picture(EMAIL_PIC_HIDE);
  }
  else
  {
    update_email_picture(EMAIL_PIC_SHOW);
  }

  return SUCCESS;
}

RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{

  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  u16 sid;
  u16 curn_id = sys_status_get_curn_group_curn_prog_id();
  cas_sid_t ca_sid;

  if (db_dvbs_get_pg_by_id(curn_id, &pg) != DB_DVBS_OK)
  {
    return SUCCESS;
  }

  if (db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    return SUCCESS;
  }


    OS_PRINTF("CA: on ca init ok, set sid : %d\n", pg.s_id);
    sid = (u16)pg.s_id;

    memset(&ca_sid, 0, sizeof(cas_sid_t));

    ca_sid.num = 1;
    ca_sid.pgid = (u16)pg.id;
    ca_sid.pg_sid_list[0] = sid;
    ca_sid.tp_freq = tp.freq;
    ca_sid.v_pid = (u16)pg.video_pid;
    ca_sid.a_pid = pg.audio[pg.audio_channel].p_id;
    ca_sid.pmt_pid = (u16)pg.pmt_pid;
    ca_sid.ca_sys_id = 0x1FFF;
    load_desc_paramter_by_pgid(&ca_sid, ca_sid.pgid);
    
    ui_ca_set_sid(&ca_sid);
  return SUCCESS;
}
