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

//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = INVALID_U32;//CAS_S_CLEAR_DISPLAY;
static cas_sid_t g_ca_sid;
static  cas_mail_headers_t *g_mail_header_info = NULL;
static u8 *g_ca_feed_status = NULL;

#ifdef WIN32
static BOOL g_is_smart_card_insert = 0;
#else
static BOOL g_is_smart_card_insert = 1;
#endif


void g_ca_set_feed_status(u8 *status)
{
  g_ca_feed_status = status;
}

u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
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
    cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_GS);
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
#if 0
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
  #if 0
  cmd_t cmd = {0};
  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_GS);
  #endif
}

control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  u8 i =0;
  u8 accept_bit_num = 1;
  control_t *p_btn[1];
  u16 stxt_btn[1] =
  {
   IDS_SMARTCARD_INFO,
  };


  for(i=0; i<accept_bit_num; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(1+i),
                              10, y,
                              325, 35,
                           p_ca_frm, 0);
    ctrl_set_rstyle(p_btn[i], RSI_WHITE_YELLOW, RSI_WHITE_YELLOW, RSI_WHITE_YELLOW);
    text_set_font_style(p_btn[i], FSI_COMM_TXT_N, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], stxt_btn[i]);
    y += (35+4);
    //ctrl_set_attr(p_btn[i], OBJ_ATTR_INACTIVE);
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
  manage_open_menu(ROOT_ID_CA_CARD_INFO, (u32)para1, 0);
}


RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;
  u16 prog_id;
  dvbs_prog_node_t curn_prog;

  //OS_PRINTF("\r\n*** ca_message_update  g_ca_last_msg[0x%x],event_id[0x%x]\r\n",
  //               g_ca_last_msg,event_id);

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

    case CAS_S_ADPT_CARD_REMOVE:
      prog_id = sys_status_get_curn_group_curn_prog_id();
      db_dvbs_get_pg_by_id(prog_id, &curn_prog);
      if(curn_prog.is_scrambled)
      {
        update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);//quanzhi IDS_E04);
      }
      break;
    case CAS_E_CARD_INIT_FAIL:
      update_ca_message(IDS_CA_SW1SW2_CARD_NOT_INIT);
      break;
    case CAS_E_SW1SW2_INCORRECT_SMARTCARD_ID:
      update_ca_message(IDS_CA_SW1SW2_INCORRECT_SMARTCARD_ID);
      break;
    case CAS_E_IPPV_NO_BOOK:
      update_ca_message(IDS_CA_SW1SW2_PPV_EXCEED);
      break;
    case CAS_E_CARD_DIS_PARTNER:
      update_ca_message(IDS_CA_SW1SW2_NOT_PAIRED);
      break;
    case CAS_E_PROG_UNAUTH:
      update_ca_message(IDS_CAS_I_UNLICENSED);//quanzhi IDS_CAS_UNLICENSED);
      break;
    case CAS_E_ZONE_CODE_ERR:
      update_ca_message(IDS_CA_SW1SW2_LIMIT_AREA);
      break;
    case CAS_C_CARD_ZONE_INVALID:
      update_ca_message(IDS_CA_SW1SW2_LIMIT_CARD);
      break;
    case CAS_E_SW1SW2_NOT_CHILD_MODE:
      update_ca_message(IDS_CA_SW1SW2_NOT_CHILD_MODE);
      break;
    case CAS_C_MASTERSLAVE_NEEDPAIRED:
      update_ca_message(IDS_CA_SW1SW2_CHILD_NOT_AUTHORIZED);
      break;
    case CAS_E_SW1SW2_CHILD_AUTH_TIMEOUT:
      update_ca_message(IDS_CA_SW1SW2_CHILD_AUTH_TIMEOUT);
      break;

    case CAS_S_CLEAR_DISPLAY:
      ui_set_playpg_scrambled(FALSE);
      update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_S_ADPT_CARD_INSERT:
      update_ca_message(RSC_INVALID_ID);
      break;
    default:
      //g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
      break;
  }

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

void g_set_mail_header_info(cas_mail_headers_t *p)
{
  g_mail_header_info = p;
}

cas_mail_headers_t *g_get_mail_header_info()
{
  return g_mail_header_info;
}

