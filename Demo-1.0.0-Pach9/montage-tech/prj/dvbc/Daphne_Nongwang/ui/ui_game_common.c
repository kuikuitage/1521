/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_game_common.h"

/*!
   Function:
   Get the pointer of a specified child control
   Parameter:
    u8 root_id		[in]:	root container id
    u8 child_id		[in]:	the specified child control id
   Return:
   the pointer of a specified child control
 */
control_t *ui_game_get_ctrl(u8 root_id, u8 child_id)
{
  control_t *p_child;
  control_t *p_comm = NULL;

  p_child = fw_find_root_by_id(root_id);
  p_comm = ctrl_get_child_by_id(p_child, IDC_COMM_ROOT_CONT);
  if(NULL != p_comm)
  {
      p_child= p_comm;
  }
  p_child = ctrl_get_child_by_id(p_child, GAME_CONT_ID);
  p_child = ctrl_get_child_by_id(p_child, child_id);

  return p_child;
}

static void empty_board(control_t *p_board)
{
  u16 i, total;
  u8 col, row;

  mbox_get_count(p_board, &total, &col, &row);

  for (i = 0; i < total; i++)
  {
    mbox_set_content_by_icon(p_board, i, 0, 0);
  }
}

void game_over_common(u8 root_id)
{
  control_t *p_mbox, *p_info, *p_ctrl;

  p_mbox = ui_game_get_ctrl(root_id, GAME_BOARD_ID);
  p_info = ui_game_get_ctrl(root_id, GAME_INFO_ID);

  empty_board(p_mbox);
  ctrl_default_proc(p_mbox, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mbox, FALSE);

  p_ctrl = ctrl_get_child_by_id(p_info, 8);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_ctrl, FALSE);
}

BEGIN_KEYMAP(game_level_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(game_level_keymap, NULL)

BEGIN_KEYMAP(game_se_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(game_se_keymap, NULL)

BEGIN_KEYMAP(game_board_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(game_board_keymap, NULL)

BEGIN_KEYMAP(game_info_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(game_info_keymap, NULL)

