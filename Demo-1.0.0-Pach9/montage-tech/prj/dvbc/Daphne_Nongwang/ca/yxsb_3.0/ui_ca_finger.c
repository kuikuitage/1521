/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#include "ui_common.h"
#include "ui_ca_finger.h"

enum control_id
{
  IDC_FINGER_PRINT = 1,
};

RET_CODE close_ca_finger_menu(void);

u16 finger_print_keymap(u16 key);
RET_CODE finger_print_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

void open_ca_finger_menu(finger_msg_t *p_finger_info)
{
  control_t *p_cont, *p_ctrl;
  dlg_ret_t ret;

  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    return;
  }
  
  
  // check for close
  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
  {
    close_ca_finger_menu();
  }

  p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
                           (rand() % 3 + 1)*FINGER_PRINT_L,FINGER_PRINT_T,
                           FINGER_PRINT_W,FINGER_PRINT_H,
                           ROOT_ID_INVALID,
                           0,
                           OBJ_ATTR_INACTIVE,
                           0);
  OS_PRINTF("p_cont->frame.left == %d\n",p_cont->frame.left);
  if (p_cont == NULL)
  {
    OS_PRINTF("p_cont is NULL, return !!!!\n");
    return ;
  }
  ctrl_set_rstyle(p_cont, RSI_FINGER_PRINT_CONT, RSI_FINGER_PRINT_CONT, RSI_FINGER_PRINT_CONT);
  //ctrl_set_proc(p_cont, finger_print_proc);
  //ctrl_set_keymap(p_cont, finger_print_keymap);
  //finger
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_FINGER_PRINT,
                            FINGER_PRINT_TEXT_X, FINGER_PRINT_TEXT_Y,
                            FINGER_PRINT_TEXT_W, FINGER_PRINT_TEXT_H,
                            p_cont, 0);
  //QQ ctrl_set_rstyle(p_ctrl, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
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
    ret = fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER, (u32)(p_finger_info->during * 1000), FALSE);
  else
    ret = fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER, FINGER_PRINT_TIMEOUT, FALSE);
}

RET_CODE close_ca_finger_menu(void)
{
  //manage_close_menu(ROOT_ID_FINGER_PRINT, 0, 0);
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER);
  fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
  return SUCCESS;
}

static RET_CODE on_exit_finger_print(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_finger_menu();
  return SUCCESS;
}

BEGIN_KEYMAP(finger_print_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(finger_print_keymap, NULL)

BEGIN_MSGPROC(finger_print_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_finger_print)
END_MSGPROC(finger_print_proc, cont_class_proc)

