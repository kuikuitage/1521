/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_channel_warn.h"

enum channel_numb_id
{
  IDC_NUMB_WARN = 1,
  IDC_NUMB_TEXT,
  IDC_NUMB_BMP
 };
u16 send_message_keymap(u16 key);
RET_CODE send_message_proc(control_t *p_edit, u16 msg,u32 para1, u32 para2);

BOOL ui_channel_warn_open(channel_warn_data_t* p_data)
{
#if 0
rsc

  control_t *p_cont, *p_title;


  p_cont = fw_create_mainwin(ROOT_ID_CHANNEL_WARN,
    p_data->left, p_data->top, NUMB_WARN_W, NUMB_WARN_H, 0, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return FALSE;
  }
  ctrl_set_rstyle(p_cont,  RSI_DLG_CONT, RSI_DLG_CONT, RSI_DLG_CONT);
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMB_WARN,
                             NUMB_WARN_TXT1_L, NUMB_WARN_TXT1_T,
                             NUMB_WARN_TXT1_W, NUMB_WARN_TXT1_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_DLG_TITLE,RSI_DLG_TITLE, RSI_DLG_TITLE);
  ctrl_set_keymap(p_title, send_message_keymap);
  ctrl_set_proc(p_title, send_message_proc);
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMB_TEXT,
                             NUMB_WARN_STR_L, NUMB_WARN_STR_T,
                             NUMB_WARN_STR_W, NUMB_WARN_STR_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);
  text_set_align_type(p_title, STL_LEFT| STL_TOP);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, p_data->strid);

  p_title = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_NUMB_BMP,
                             NUMB_WARN_BMP_L, NUMB_WARN_BMP_T,
                             NUMB_WARN_BMP_W, NUMB_WARN_BMP_H,
                             p_cont, 0);
  bmap_set_content_by_id(p_title, IM_ICON_WARNING);
  ctrl_default_proc(p_title, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);
#endif
  return TRUE;
}

static RET_CODE on_message_exit_edit(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0, 0);
  return SUCCESS;
}
BEGIN_KEYMAP(send_message_keymap, NULL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(send_message_keymap, NULL)

BEGIN_MSGPROC(send_message_proc, text_class_proc)
  ON_COMMAND(MSG_EXIT, on_message_exit_edit)
  ON_COMMAND(MSG_EXIT_ALL, on_message_exit_edit)
END_MSGPROC(send_message_proc, text_class_proc)


