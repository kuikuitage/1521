/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
   
#include "ui_mosaic_play.h"

enum mosaic_play_cont_id
{
  IDC_INVALID = 0,
  IDC_MOSAIC_PLAY_INFO_FRM,
};

u16 mosaic_play_cont_keymap(u16 key);
RET_CODE mosaic_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 mosaic_play_video_keymap(u16 key);
RET_CODE mosaic_play_video_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE mosaic_play_video_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 mosaic_play_text_keymap(u16 key);
RET_CODE mosaic_play_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


RET_CODE open_mosaic_play(u32 para1, u32 para2)
{
  control_t *p_mask,*p_cont;

#if 0
  p_cont = ui_comm_root_create(ROOT_ID_MOSAIC_PLAY,
                             RSI_IGNORE,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             0, RSI_IGNORE);
#else
  p_mask = fw_create_mainwin(ROOT_ID_MOSAIC_PLAY,
                             0, 0, 720, 576,
                             ROOT_ID_MOSAIC, 0, OBJ_ATTR_ACTIVE, 0);
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_ROOT_CONT,
                            0, 0, 720, 576, p_mask, 0);
#endif

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, mosaic_play_cont_keymap);
  ctrl_set_proc(p_cont, mosaic_play_cont_proc);

  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);
  return SUCCESS;
  
}

static void mosaic_play_do_ok(void)
{
  if(ui_is_pause())
  {
      ui_set_pause(FALSE);
  }
  fw_notify_parent(ROOT_ID_MOSAIC_PLAY, NOTIFY_T_MSG, FALSE,
                      MSG_EXIT_MOSAIC_PROG_PLAY, 0, 0);
  manage_close_menu(ROOT_ID_MOSAIC_PLAY, 0, 0);
}

static void mosaic_play_do_cancel(void)
{
  
}

static RET_CODE on_mosaic_set_sound(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_MOSNVOD_VOLUME,0,0);
  return SUCCESS;
}

static RET_CODE on_close_mosaic_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_MOSAIC, 
                               mosaic_play_do_ok, mosaic_play_do_cancel, 0);
  return SUCCESS;
}


BEGIN_KEYMAP(mosaic_play_cont_keymap, ui_comm_root_keymap) 
  ON_EVENT(V_KEY_MENU, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_VDOWN,  MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT,  MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
END_KEYMAP(mosaic_play_cont_keymap, ui_comm_root_keymap)
  
BEGIN_MSGPROC(mosaic_play_cont_proc, ui_comm_root_proc)
   ON_COMMAND(MSG_CLOSE_MENU, on_close_mosaic_play)
   ON_COMMAND(MSG_EXIT, on_close_mosaic_play)
   ON_COMMAND(MSG_DECREASE, on_mosaic_set_sound)
  ON_COMMAND(MSG_INCREASE, on_mosaic_set_sound)
END_MSGPROC(mosaic_play_cont_proc, ui_comm_root_proc)


