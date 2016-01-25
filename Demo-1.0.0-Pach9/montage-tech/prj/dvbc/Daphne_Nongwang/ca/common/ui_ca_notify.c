/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_ca_notify.h"

enum comm_dlg_idc
{
  IDC_CA_CONTENT = 1,
};

static u32 g_ca_notify_type = NOTIFY_CA_TYPE_STRID;
static control_t *p_roll_txt = NULL;
static BOOL b_roll_notify = FALSE;

RET_CODE notify_ca_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

BOOL ui_is_roll_notify(void)
{
  return b_roll_notify;
}

void close_ca_notify(void)
{
  b_roll_notify = FALSE;
  (void)fw_tmr_destroy(ROOT_ID_CA_NOTIFY, MSG_EXIT);
  gui_stop_roll(p_roll_txt);
  fw_destroy_mainwin_by_id(ROOT_ID_CA_NOTIFY);
}


RET_CODE open_ca_notify(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_txt = NULL;
  roll_param_t p_param = {0};
  msg_info_t *p_roll_msg = NULL;
  u32 g_ca_notify_content = 0;
  u8 focus = fw_get_focus_id();

  if(focus == SN_ROOT_ID_SUBMENU)
  {
    return SUCCESS;
  }
  g_ca_notify_type = NOTIFY_CA_TYPE_UNI;
  p_roll_msg = (msg_info_t *)para1;
  g_ca_notify_content = para2;
  if ((g_ca_notify_content == 0) || (NULL == p_roll_msg))
    return ERR_FAILURE;

  if (fw_find_root_by_id(ROOT_ID_CA_NOTIFY) != NULL)
  {
    close_ca_notify();
  }
  
  p_cont = fw_create_mainwin(ROOT_ID_CA_NOTIFY,
                                  0, NOTIFY_CA_CONT_Y, 
                                  SCREEN_WIDTH, 32,
                                  0, 0, 
                                  OBJ_ATTR_INACTIVE, 0);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, notify_ca_proc);
  MT_ASSERT(NULL != p_cont);
  ctrl_set_rstyle(p_cont,
                  RSI_TRANSPARENT,
                  RSI_TRANSPARENT,
                  RSI_TRANSPARENT);

  p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CA_CONTENT,
                           0, 0,
                           SCREEN_WIDTH,32,
                           p_cont, 0);
  p_roll_txt = p_txt;
  ctrl_set_rstyle(p_txt,
                  RSI_CA_NOTIFY_TXT,
                  RSI_CA_NOTIFY_TXT,
                  RSI_CA_NOTIFY_TXT);
  text_set_font_style(p_txt, FSI_CA_NOTIFY_TXT, FSI_CA_NOTIFY_TXT, FSI_CA_NOTIFY_TXT);
  
  switch(g_ca_notify_type)
  {
    case NOTIFY_CA_TYPE_STRID:
      text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_txt, (u16)g_ca_notify_content);
      break;
    case NOTIFY_CA_TYPE_ASC:
      text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);  
      text_set_content_by_ascstr(p_txt, (u8 *)g_ca_notify_content);
      break;
    case NOTIFY_CA_TYPE_UNI:
      text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE); 
      text_set_content_by_unistr(p_txt, (u16 *)g_ca_notify_content);
      break;
    default:
      MT_ASSERT(0);
  }    
  p_param.pace = ROLL_SINGLE;
  p_param.style = ROLL_LR;
  if(1 == p_roll_msg->osd_display.roll_mode)  // times
  {
    p_param.repeats= p_roll_msg->osd_display.roll_value;
  }
  else //dure time
  {
    p_param.repeats= 0;
    OS_PRINTF("\r\n *** dura time[0x%x]",p_roll_msg->osd_display.roll_value);
    fw_tmr_create(ROOT_ID_CA_NOTIFY, MSG_EXIT, (p_roll_msg->osd_display.roll_value) * 1000, FALSE);
  }
  gui_start_roll(p_txt, &p_param);

  ctrl_paint_ctrl(p_cont, FALSE);
  b_roll_notify = TRUE;
  
  return SUCCESS;
}

static RET_CODE on_ca_notify_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_notify();
  return SUCCESS;
}


BEGIN_MSGPROC(notify_ca_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_ca_notify_exit)
END_MSGPROC(notify_ca_proc, ui_comm_root_proc)

