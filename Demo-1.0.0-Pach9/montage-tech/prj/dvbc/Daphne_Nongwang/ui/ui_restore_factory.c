/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_restore_factory.h"
#include "ui_notify.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_ca_public.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,
  IDC_RST_FAC_BRIEF1,
  IDC_RST_FAC_BRIEF2,
  IDC_RST_FAC_DETAIL,
};

u16 restore_factory_cont_keymap(u16 key);
RET_CODE restore_factory_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE restore_factory_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

 void restore_to_factory(void)
{
  u16 view_id;
  dvbs_tp_node_t tp = {0};

  // set front panel
  ui_set_front_panel_by_str("----");
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  // kill tmr

  //set default
  ui_cas_factory_set(CAS_CMD_FACTORY_SET, 0, 0);
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
  sys_status_load();
  sys_status_check_group();
  sys_status_save();

  // set environment according ss_data
  sys_status_reload_environment_setting();
    //set boot status
  #ifdef OTA_DM_ON_STATIC_FLASH
  {
      sys_static_data_restore_factory();
  }
  #else
  sys_status_set_boot_status();
  #endif
  sys_reset_adver_logo();
  
  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }
  if (ui_is_notify())
  {
    ui_set_notify(NULL, NOTIFY_TYPE_STRID, RSC_INVALID_ID);
  }

  ui_play_api_factory();
  
  ui_set_transpond(&tp);

  // send cmd to ap frm and wait for finish

  //create a new view after load default, and save the new view id.
  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id((u8)view_id);
}


static RET_CODE on_restore_factory_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_detail = ctrl_get_child_by_id(p_ctrl, IDC_RST_FAC_DETAIL);
  text_set_content_by_strid(p_detail, IDS_RESETING_FACTORY);
  ctrl_paint_ctrl(p_detail, TRUE);
  
  restore_to_factory();
  ui_stop_play(STOP_PLAY_BLACK,TRUE);
  mtos_task_delay_ms(1000);
  text_set_content_by_strid(p_detail, IDS_RESETING_FINISHED);
  ctrl_paint_ctrl(p_ctrl, TRUE); //repaint the whole menu

  return SUCCESS;
}

RET_CODE open_restore_factory(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_brief;
  control_t *p_small_cont1, *p_small_cont2;
  comm_help_data_t factorymenu_help_data = //help bar data
  {
    2,
    2,
    {
      IDS_BACK,
      IDS_SELECTED,
    },
    { 
      IM_ICON_BACK,
      IM_ICON_SELECT,
    },
  };
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SYSTEM_SET,RSI_TITLE_BG, IDS_FACTORY_SET};

  p_cont = ui_comm_prev_root_create(ROOT_ID_RESTORE_FACTORY, RSI_FULL_SCREEN_BG,
                          RST_FAC_MENU_X, RST_FAC_MENU_Y,
                          RST_FAC_MENU_W, RST_FAC_MENU_H,
                          &title_data);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, restore_factory_cont_keymap);
  ctrl_set_proc(p_cont, restore_factory_cont_proc);
  
  //small cont 1
  p_small_cont1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG1,
                              RST_FAC_CONT1_X, RST_FAC_CONT1_Y,
                              RST_FAC_CONT1_W, RST_FAC_CONT1_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_small_cont1, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);

  //brief 1
  p_brief = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_RST_FAC_BRIEF1,
                              RST_FAC_BRIEF1_X, RST_FAC_BRIEF1_Y,
                              RST_FAC_BRIEF1_W, RST_FAC_BRIEF1_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_brief, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_brief, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_brief, STL_LEFT | STL_TOP);
  text_set_offset(p_brief, 10, 10);
  text_set_content_type(p_brief, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_brief, IDS_RESTORE_FACTORY_INTRODUCE);

  //brief 2
  p_brief = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_RST_FAC_BRIEF2,
                              RST_FAC_BRIEF2_X, RST_FAC_BRIEF2_Y,
                              RST_FAC_BRIEF2_W, RST_FAC_BRIEF2_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_brief, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_brief, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_brief, STL_LEFT | STL_VCENTER);
  text_set_offset(p_brief, 10, 0);
  text_set_content_type(p_brief, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_brief, IDS_CONFIRM_TO_RESTORE);

  //small cont 2
  p_small_cont2= ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG2,
                              RST_FAC_CONT2_X, RST_FAC_CONT2_Y,
                              RST_FAC_CONT2_W, RST_FAC_CONT2_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_small_cont2, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);

  //detail
  p_brief = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_RST_FAC_DETAIL,
                              RST_FAC_DETAIL_X, RST_FAC_DETAIL_Y,
                              RST_FAC_DETAIL_W, RST_FAC_DETAIL_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_brief, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
  text_set_font_style(p_brief, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_brief, STL_LEFT | STL_VCENTER);
  text_set_offset(p_brief, 10, 20);
  text_set_content_type(p_brief, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_brief, IDS_FREQ_SET_TV);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_comm_help_create(&factorymenu_help_data, p_cont);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif

  return SUCCESS;
}

static RET_CODE on_factory_to_mainmenu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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
  manage_close_menu(ROOT_ID_RESTORE_FACTORY,0,0);
  manage_close_menu(SN_ROOT_ID_SUBMENU,0,0);
  return SUCCESS;
}

BEGIN_KEYMAP(restore_factory_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
END_KEYMAP(restore_factory_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(restore_factory_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_TO_MAINMENU,on_factory_to_mainmenu)
  ON_COMMAND(MSG_SELECT, on_restore_factory_select)
END_MSGPROC(restore_factory_cont_proc, ui_comm_root_proc)


