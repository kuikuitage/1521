/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_game_inn.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum ca_btn_id
{
  IDC_BTN_TETRIS = 1,
  IDC_BTN_CHESS,
  IDC_BTN_SNAKE,
};
enum game_local_msg
{
  MSG_GAME_NULL = MSG_LOCAL_BEGIN + 300,
  MSG_GAME_HOTKEY_BLUE,
};

#if 0
rsc

static comm_help_data_t game_inn_help_data = //help bar data
{
  3,
  3,
  {
    IDS_BACK,
    IDS_SELECTED,
    //IDS_FN_SELECT,
    IDS_FN_SELECT,
  },
  { 
    IM_ICON_BACK,
    IM_ICON_SELECT,
    //IM_ICON_ARROW_LEFT_RIGHT,
    IM_ICON_ARROW_UP_DOWN,
  },
};
#endif
static u16 num = 0;

u16 game_inn_cont_keymap(u16 key);
RET_CODE game_inn_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 game_inn_btn_keymap(u16 key);

RET_CODE game_inn_btn_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 game_inn_frm_btn_keymap(u16 key);

RET_CODE game_inn_frm_btn_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


RET_CODE open_game_inn(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_btn[GAME_INN_BTN_CNT];
  control_t *p_frm;
  u8 i;
  u16 y;
  u16 stxt_btn[GAME_INN_BTN_CNT] =
  { 
    IDS_TETRIS, IDS_BW_CHESS, IDS_SNAKE,
  };
  full_screen_title_t title_data = {IM_COMMON_BANNER_GAME,IDS_GAME_INN, RSI_TITLE_BG,IDS_GAME_INN};
  
  p_cont = ui_comm_prev_root_create(ROOT_ID_GAME_INN, RSI_FULL_SCREEN_BG,
                          GAME_INN_MENU_X, GAME_INN_MENU_Y,
                          GAME_INN_MENU_W, GAME_INN_MENU_H,
                          &title_data);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, game_inn_cont_keymap);
  ctrl_set_proc(p_cont, game_inn_cont_proc);
  
  // frm
  p_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              GAME_INN_FRM_X, GAME_INN_FRM_Y,
                              GAME_INN_FRM_W, GAME_INN_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_frm, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_frm, game_inn_frm_btn_keymap);
  ctrl_set_proc(p_frm, game_inn_frm_btn_proc);

  //button
  y = GAME_INN_BTN_Y;
  for(i=0; i<GAME_INN_BTN_CNT; i++)
  {
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_BTN_TETRIS+i),
                              GAME_INN_BTN_X, y,
                              GAME_INN_BTN_W, GAME_INN_BTN_H,
                              p_frm, 0);
    ctrl_set_rstyle(p_btn[i], RSI_SUB_BUTTON_SH, RSI_COMMON_BTN_HL, RSI_SUB_BUTTON_SH);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_font_style(p_btn[i], FSI_COMM_TXT_N, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], stxt_btn[i]);
    y += (GAME_INN_BTN_H+GAME_INN_BTN_V_GAP);

    ctrl_set_related_id(p_btn[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              GAME_INN_BTN_CNT) %
                             GAME_INN_BTN_CNT + IDC_BTN_TETRIS),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % GAME_INN_BTN_CNT + IDC_BTN_TETRIS));/* down */
  }

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_pic_play(ADS_AD_TYPE_SUB_MENU, SN_ROOT_ID_SUBMENU);

  ui_comm_help_create(&game_inn_help_data, p_cont);

  ctrl_default_proc(p_btn[num], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}

static RET_CODE on_ca_frm_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  customer_cfg_t cus_cfg = {0};
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl));

  get_customer_config(&cus_cfg);
  switch(ctrlID)
  {
    case IDC_BTN_TETRIS:
      manage_open_menu(ROOT_ID_TETRIS, 0, 0);
      num = 0;
      break;

    case IDC_BTN_CHESS:
      manage_open_menu(ROOT_ID_GAME_BWC, 0, 0);
      num = 1;
      break;

    case IDC_BTN_SNAKE:
      num = 2;
      manage_open_menu(ROOT_ID_GAME_SNAKE, 0, 0);
      break;

    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE on_game_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  ui_pic_stop();
  manage_close_menu(ROOT_ID_GAME_INN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_game_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  ui_pic_stop();
  ui_close_all_mennus();
  
  return SUCCESS;
}

static RET_CODE on_game_process_blue_hotkey(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
  	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
  	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
    {
      return on_game_exit_all(p_list,msg, para1,para2);
    }
  return SUCCESS;
}

static RET_CODE on_game_in_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(game_inn_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_GAME, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_BLUE, MSG_GAME_HOTKEY_BLUE)
END_KEYMAP(game_inn_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(game_inn_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_game_exit_all)
  ON_COMMAND(MSG_GAME_HOTKEY_BLUE, on_game_process_blue_hotkey)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_game_in_draw_pic_end)
END_MSGPROC(game_inn_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(game_inn_frm_btn_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(game_inn_frm_btn_keymap, NULL)

BEGIN_MSGPROC(game_inn_frm_btn_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_ca_frm_btn_select)
  ON_COMMAND(MSG_BACKSPACE, on_game_exit)
END_MSGPROC(game_inn_frm_btn_proc, cont_class_proc)



