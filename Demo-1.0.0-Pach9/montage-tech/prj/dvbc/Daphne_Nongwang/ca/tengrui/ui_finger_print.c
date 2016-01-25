/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#include "ui_common.h"
#include "ui_finger_print.h"

#ifdef PATULOUS_FUNCTION_CA
enum control_id
{
  IDC_FINGER_PRINT = 1,
};

u16 finger_print_keymap(u16 key);
RET_CODE finger_print_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_finger_print(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl;
  finger_msg_t *p_finger_info = (finger_msg_t *)para2;
  dlg_ret_t ret;

  // check for close
  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
  {
    close_finger_print();
  }

  p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
                           (rand() % 3 + 1)*FINGER_PRINT_L,FINGER_PRINT_T,
                           FINGER_PRINT_W,FINGER_PRINT_H,
                           ROOT_ID_INVALID,
                           0,
                           OBJ_ATTR_INACTIVE,
                           0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_FINGER_PRINT_CONT, RSI_FINGER_PRINT_CONT, RSI_FINGER_PRINT_CONT);
  //ctrl_set_proc(p_cont, finger_print_proc);
  //ctrl_set_keymap(p_cont, finger_print_keymap);
  //finger
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_FINGER_PRINT,
                            FINGER_PRINT_TEXT_X, FINGER_PRINT_TEXT_Y,
                            FINGER_PRINT_TEXT_W, FINGER_PRINT_TEXT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME);
  text_set_font_style(p_ctrl, FSI_FINGER_PRINT_FONT, FSI_FINGER_PRINT_FONT, FSI_FINGER_PRINT_FONT);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  if(p_finger_info)
    text_set_content_by_ascstr(p_ctrl, (u8*)p_finger_info->data);
  else
    text_set_content_by_ascstr(p_ctrl, (u8*)" ");

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_finger_info)
    ret = fw_tmr_create(ROOT_ID_FINGER_PRINT, MSG_CANCEL, (u32)(p_finger_info->during * 1000), FALSE);
  else
    ret = fw_tmr_create(ROOT_ID_FINGER_PRINT, MSG_CANCEL, FINGER_PRINT_TIMEOUT, FALSE);

  MT_ASSERT(ret == SUCCESS);
  
  return SUCCESS;
}

RET_CODE close_finger_print(void)
{
  //manage_close_menu(ROOT_ID_FINGER_PRINT, 0, 0);
  fw_tmr_destroy(ROOT_ID_FINGER_PRINT, MSG_CANCEL);
  fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
  return SUCCESS;
}

static RET_CODE on_exit_finger_print(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_finger_print();
  return SUCCESS;
}

BEGIN_KEYMAP(finger_print_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(finger_print_keymap, NULL)

BEGIN_MSGPROC(finger_print_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_finger_print)
  ON_COMMAND(MSG_CANCEL, on_exit_finger_print)
END_MSGPROC(finger_print_proc, cont_class_proc)
#else
RET_CODE open_finger_print(u32 para1, u32 para2)
{
  return SUCCESS;
}
#endif

