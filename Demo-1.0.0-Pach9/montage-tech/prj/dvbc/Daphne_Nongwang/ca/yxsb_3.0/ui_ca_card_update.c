/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 ****************************************************************************/
#include "ui_common.h"

#include "ui_ca_card_update.h"
#include "sys_dbg.h"

RET_CODE ca_card_update_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_UPDATE_BAR,
  IDC_CONTENT,
};
//u16 ca_card_update_keymap(u16 key);

extern void set_uio_key_status(BOOL is_disable);

RET_CODE open_ca_card_update_info(u32 para1, u32 para2)
{
  control_t *p_content = NULL, *p_bar = NULL, *p_title = NULL, *p_cont = NULL;
  u8 title_type = ((para2 >> 8) & (0XFF));
  
  if(fw_find_root_by_id(ROOT_ID_CA_CARD_UPDATE) != NULL)
  {
    UI_PRINTF("\n ROOT_ID_CA_CARD_UPDATE already opened............!!! \n");
    return ERR_FAILURE;
  }
  p_cont = fw_create_mainwin(ROOT_ID_CA_CARD_UPDATE,
                           CA_UP_CONT_FULL_X, CA_UP_CONT_FULL_Y,
                           CA_UP_CONT_FULL_W, CA_UP_CONT_FULL_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

  //ctrl_set_keymap(p_cont, ca_card_update_keymap);
  ctrl_set_proc(p_cont, ca_card_update_proc);
  //title
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE,
                         CA_UP_TITLE_FULL_X, CA_UP_TITLE_FULL_Y,
                         CA_UP_TITLE_FULL_W,CA_UP_TITLE_FULL_H,
                         p_cont, 0);

  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);

  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  if(title_type == 1)
  {
    text_set_content_by_strid(p_title, IDS_CA_PACKET_UPDATE);
  }
  else
  {
    text_set_content_by_strid(p_title, IDS_CA_CARD_UPDATE);
  }

//bar
  p_bar = ui_comm_bar_create(p_cont, IDC_UPDATE_BAR,
                       CA_STATUS_BAR_MX, CA_STATUS_BAR_MY, CA_STATUS_BAR_MW, CA_STATUS_BAR_MH,
                       CA_STATUS_BAR_LX, CA_STATUS_BAR_LY, CA_STATUS_BAR_LW, CA_STATUS_BAR_LH,
                       CA_STATUS_BAR_RX, CA_STATUS_BAR_RY,CA_STATUS_BAR_RW, CA_STATUS_BAR_RH);
  ui_comm_bar_set_param(p_bar, IDS_PROGRESS, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_COMM_PBAR_MID,
                          RSI_IGNORE, FSI_INDEX3,
                          RSI_SN_BG, FSI_INDEX3); //QQ   RSI_SUB_BUTTON_SH
  ui_comm_bar_update(p_bar, 0, TRUE);

  //content
  p_content = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                       CA_UP_CONTENT_FULL_X, CA_UP_CONTENT_FULL_Y,
                       CA_UP_CONTENT_FULL_W, CA_UP_CONTENT_FULL_H,
                       p_cont, 0);
  ctrl_set_rstyle(p_content, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  text_set_font_style(p_content, FSI_INDEX3,FSI_INDEX3,FSI_INDEX3);
  text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_content, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_content, IDS_CA_CARD_UPGRADING);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
// ui_ca_order_ipp_rtime.c
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(p_cont != NULL)
  {
    ctrl_paint_ctrl(p_cont, FALSE);
  }
  return SUCCESS;
}

static void card_update_progress(control_t *p_cont, u16 msg, u8 progress)
{
  control_t *p_bar = NULL;
  
  p_bar = ctrl_get_child_by_id(p_cont, IDC_UPDATE_BAR);

  if(p_bar == NULL )
  {
    return;
  }
  if(ctrl_get_attr(p_bar) != OBJ_ATTR_HIDDEN)
  {
    UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
    ui_comm_bar_update(p_bar, progress, TRUE);
    ui_comm_bar_paint(p_bar, TRUE);
  }
  
  ctrl_paint_ctrl(p_cont, TRUE);
  return;
}

static RET_CODE on_ca_card_up_info_update(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  u8 percent = (para2 & 0XFF);
  
  UI_PRINTF("on_ca_card_up_info_update PROCESS -> update progress: %d\n", percent);

  card_update_progress(cont, msg,(u8)percent);
  
  return SUCCESS;
}

BEGIN_MSGPROC(ca_card_update_proc, cont_class_proc)
  //ON_COMMAND(MSG_CA_CARD_UPDATE_BEGIN, on_ca_card_up_info_update)
  ON_COMMAND(MSG_CA_CARD_PROGRESS, on_ca_card_up_info_update)
END_MSGPROC(ca_card_update_proc, cont_class_proc)

