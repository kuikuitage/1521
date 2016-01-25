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
#include "ui_ca_public.h"

//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;
static cas_sid_t g_ca_sid;
static BOOL g_is_smart_card_insert = 0;

enum ca_ql_btn_id
{
  IDC_BTN_CA_INFO = 1,
  IDC_BTN_CA_ENTTITLE_INFO,
};

BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

void ui_set_smart_card_insert(BOOL p_card_status)
{
  g_is_smart_card_insert = p_card_status;
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
  /**boyuan CA don't to do**/
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
    cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_QL);
    OS_PRINTF("\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                      cmd.id,cmd.data1,cmd.data2);
    ap_frm_do_command(APP_CA, &cmd);

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
    /**boyuan ca will set message to do **/
}
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{
  cmd_t cmd = {0};

  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_QL);
  OS_PRINTF("\r\n***ui_cas_factory_set cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                    cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
}

control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  u8 i =0;
  u8 accept_bit_num = 2;
  control_t *p_btn[2];
  u16 stxt_btn[2] =
  { 
    IDS_SMARTCARD_INFO, IDS_ACCREDIT_INFO
  };


  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
    ctrl_set_rstyle(p_btn[i], RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECT);
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
	   
	    case IDC_BTN_CA_ENTTITLE_INFO:
	      manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, (u32)para1, 0);
	      break;
	    
	    default:
	      break;
	    }
}


RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;

  //OS_PRINTF("\r\n*** ca_message_update  g_ca_last_msg[0x%x],event_id[0x%x]\r\n",
  //                g_ca_last_msg,event_id);

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
      on_ca_zone_check_end();
      break;
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
      update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);
      break;
    case CAS_E_CARD_INIT_FAIL:
      update_ca_message(IDS_CAS_E_CARD_INIT_FAIL);
      break;
    case CAS_E_PROG_UNAUTH: //智能卡用户无授权
      update_ca_message(IDS_CAS_E_PROG_UNAUTH);  
      break;  
    case CAS_E_ZONE_CODE_ERR:
      update_ca_message(IDS_CAS_E_ZONE_CODE_ERR);
      break;
    case CAS_E_CARD_DATA_GET:// 智能卡读卡号命令错误
      update_ca_message(IDS_CAS_E_READ_CARD_NUMBER_ERROR);
      break;
    case CAS_E_EMM_ERROR://EMM错误
      update_ca_message(IDS_CAS_E_EMM_ERROR);
      break;
    case CAS_E_ECM_ERROR://ECM错误
      update_ca_message(IDS_CAS_E_ECM_ERROR);
      break;
    case CAS_E_ECM_RETURN_ERROR://智能卡返回ECM数据错误
      update_ca_message(IDS_CAS_E_RET_ECM_DATA_ERROR);
      break;
    case CAS_E_ECMEMM_RETURN_P1P2_ERROR://智能卡返回ECM EMM P1P2数据错误
      update_ca_message(IDS_CAS_E_RET_ECM_EMM_P1P2_ERROR);
      break;
    case CAS_E_ECMEMM_RETURN_P3_ERROR://智能卡返回ECM EMM P3数据错误
      update_ca_message(IDS_CAS_E_RET_ECM_EMM_P3_ERROR);
      break;
    case CAS_E_CARD_REG_FAILED://智能卡返回错误
      update_ca_message(IDS_CAS_E_RET_ERROR);
      break;
    case CAS_E_ECMEMM_RETURN_ERROR://智能卡ECM EMM返回未知错误
      update_ca_message(IDS_CAS_E_ECM_EMM_RET_UNKNOWN_ERROR);
      break;
    case CAS_E_CARD_DIS_PARTNER://节目要求机卡配对
      update_ca_message(IDS_CAS_E_NEED_STB_PAIR);
      break;
    case CAS_S_CARD_STB_PARTNER://节目机卡配对成功
      update_ca_message(IDS_CAS_S_STB_CARD_PAIR_SUCCESS);
      break;
    case CAS_E_NO_ECM://没有ECM流信息
      update_ca_message(IDS_CAS_E_NO_ECM_INFO);
      break;
    case CAS_C_USERCDN_LIMIT_CTRL_ENABLE://该用户被条件限播
      update_ca_message(IDS_CAS_C_USERCDN_LIMIT_CTRL_ENABLE);
      break;
    case CAS_C_USERCDN_LIMIT_CTRL_DISABLE://该用户被取消条件限播
      update_ca_message(IDS_CAS_C_USERCDN_LIMIT_CTRL_DISABLE);
      break;
    case CAS_C_ZONECDN_LIMIT_CTRL_ENABLE://该区域被条件限播
      update_ca_message(IDS_CAS_C_ZONECDN_LIMIT_CTRL_ENABLE);
      break;
    case CAS_C_ZONECDN_LIMIT_CTRL_DISABLE://该区域被取消条件限播
      update_ca_message(IDS_CAS_C_ZONECDN_LIMIT_CTRL_DISABLE);
      break;
    case CAS_E_PAIRED_OTHERCARD_ERROR://该机器与其它卡配对
      update_ca_message(IDS_CAS_E_PAIRED_OTHERCARD_ERROR);
      break;
    case CAS_E_PAIRED_OTHERSTB_ERROR://该卡与此机器不配对
      update_ca_message(IDS_CAS_E_PAIRED_OTHERSTB_ERROR);
      break;
    case CAS_C_MASTERSLAVE_PAIRED_OK://子母卡配对成功
      update_ca_message(IDS_CAS_C_MASTERSLAVE_PAIRED_OK);
      break;
    case CAS_C_MASTERSLAVE_PAIRED_TIMEUPDATE://子母卡配对时间更新
      update_ca_message(IDS_CAS_C_MASTERSLAVE_PAIRED_UPDATE);
      break;
    case CAS_C_MASTERSLAVE_NEEDPAIRED://子母卡需要配对
      update_ca_message(IDS_CAS_C_MASTERSLAVE_NEEDPAIRED);
      break;
    case CAS_C_CANCEL_PAIRED_OK://机卡配对取消成功
      update_ca_message(IDS_CAS_C_CANCEL_PAIRED_OK);
      break;
    case CAS_C_CARD_ZONE_INVALID://您的智能卡不再服务区
      update_ca_message(IDS_CAS_C_CARD_ZONE_INVALID);
      break;
    case CAS_C_CARD_ZONE_CHECK_ENTER://正在寻找网络，请等待
      update_ca_message(IDS_CAS_C_CARD_ZONE_CHECK_ENTER);
      break;
    case CAS_C_CARD_ZONE_CHECK_OK://漫游功能卡检测正常
      update_ca_message(IDS_CAS_C_CARD_ZONE_CHECK_OK);
      break;
    case CAS_E_UNKNOWN_ERROR://未知错误
      update_ca_message(IDS_CAS_E_UNKNOWN_ERROR);
      break;
    case CAS_S_CLEAR_DISPLAY:
      ui_set_playpg_scrambled(FALSE);
      update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_S_ADPT_CARD_INSERT:
	  ui_set_smart_card_insert(TRUE);
      update_ca_message(RSC_INVALID_ID);
      break;
	case CAS_S_ADPT_CARD_REMOVE:
	  ui_set_smart_card_insert(FALSE);
	  break;
    case CAS_E_CARD_AUTH_FAILED: //该机顶盒未经过认证
    	update_ca_message(IDS_CA_NOT_CERTIFIED);
    	break;
    case CAS_E_NO_AUTH_STB: //智能卡系统ID错误
    	update_ca_message(IDS_CA_SYS_ID_ERR);
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
}
RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2)
{
  return SUCCESS;
}
  
