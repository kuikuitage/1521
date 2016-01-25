#include "ui_common.h"

#include "ui_ota_search.h"

enum ota_search_collect_id
{
  IDC_COLLECT_INFO = 1,
  IDC_COLLECT_EXIT,
};

enum ota_search_check_id
{
  IDC_CHECK_LOCAL = 1,
  IDC_CHECK_SERVICE,
  IDC_CHECK_TXT,
  IDC_CHECK_YES,
  IDC_CHECK_NO,
};

enum ota_search_ctrl_id
{
  IDC_OTA_SEARCH_CONT = 1,
  IDC_OTA_SEARCH_COLLECT,
  IDC_OTA_SEARCH_CHECK,
};

ota_info_t upgrade;

u16 ota_search_keymap(u16 key);
RET_CODE ota_search_proc(control_t * p_cont, u16 msg, u32 para1, u32 para2);

u16 ota_search_btn_keymap(u16 key);
RET_CODE ota_search_btn_proc(control_t * p_btn, u16 msg, u32 para1, u32 para2);

u16 ota_search_check_keymap(u16 key);


static void ota_search_set_menu_state(control_t *p_cont,
  BOOL is_collect, BOOL is_paint, u32 version)
{
  control_t *p_collect, *p_check;
  control_t *p_service, *p_local;
  control_t *p_btn;
  ss_public_t *p_public;

  p_collect = ctrl_get_child_by_id(p_cont, IDC_OTA_SEARCH_COLLECT);
  p_btn = ctrl_get_active_ctrl(p_collect);

  ctrl_set_attr(p_collect, is_collect?OBJ_ATTR_ACTIVE:OBJ_ATTR_HIDDEN);
  ctrl_default_proc(p_btn, is_collect?MSG_GETFOCUS:MSG_LOSTFOCUS, 0, 0);

  p_check = ctrl_get_child_by_id(p_cont, IDC_OTA_SEARCH_CHECK);
  p_service = ctrl_get_child_by_id(p_check, IDC_CHECK_SERVICE);
  p_local = ctrl_get_child_by_id(p_check, IDC_CHECK_LOCAL);
  p_btn = ctrl_get_active_ctrl(p_check);

  ctrl_set_attr(p_check, is_collect?OBJ_ATTR_HIDDEN:OBJ_ATTR_ACTIVE);
  ctrl_default_proc(p_btn, is_collect?MSG_LOSTFOCUS:MSG_GETFOCUS, 0, 0);


  if(!is_collect)
  {
    ui_comm_static_set_content_by_dec(p_service, version>>16);

    p_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));
    ui_comm_static_set_content_by_dec(p_local,
      version<<16>>16);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_cont, TRUE);
  }
}

static RET_CODE on_ota_search_found(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  OS_PRINTF("MSG_OTA_FOUND\n");
  ota_search_set_menu_state(p_cont, FALSE, TRUE, para1);

  return SUCCESS;
}

static RET_CODE on_ota_search_save(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  #ifdef OTA_SUPPORT
  ui_ota_stop(TRUE);
  #endif
  return SUCCESS;
}

static RET_CODE on_ota_search_reset(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  #ifndef WIN32
  hal_pm_reset();
  #endif

  return SUCCESS;
}

static RET_CODE on_ota_search_select(control_t *p_btn,
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_parent;
  u8 parent_id, btn_id;
  u16 pgid = sys_status_get_curn_group_curn_prog_id();

  p_parent = ctrl_get_parent(p_btn);
  parent_id = ctrl_get_ctrl_id(p_parent);
  btn_id = ctrl_get_ctrl_id(p_btn);
  if((parent_id == IDC_OTA_SEARCH_COLLECT) ||
    ((parent_id == IDC_OTA_SEARCH_CHECK) && (btn_id == IDC_CHECK_NO)))
  {
    //exit
    ret = manage_close_menu(ROOT_ID_OTA_SEARCH, 0, 0);
    #ifdef OTA_SUPPORT
    ui_ota_stop(FALSE);
    ui_ota_release();
    #endif
    if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
    {
      ui_play_prog(pgid, TRUE);
    }
  }
  else if((parent_id == IDC_OTA_SEARCH_CHECK) && (btn_id == IDC_CHECK_YES))
  {
     #ifdef OTA_DM_ON_STATIC_FLASH
    ota_info_t ota_info = {0};
    memset(&ota_info, 0x0, sizeof(ota_info));
    ota_info.download_data_pid = upgrade.download_data_pid;
    ota_info.ota_tri = OTA_TRI_AUTO;//tmp, it should be OTA_TRI_MONITOR
    ota_info.sys_mode = SYS_DVBC;
    ota_info.lockc.tp_freq = upgrade.lockc.tp_freq;
    ota_info.lockc.tp_sym = upgrade.lockc.tp_sym;
    ota_info.lockc.nim_modulate = upgrade.lockc.nim_modulate;
    ota_info.orig_software_version = (para1 & 0x00ff);
    ota_info.new_software_version = ((para1 >> 16) & 0x00ff);
    sys_static_write_ota_info(&ota_info);
     sys_static_write_into_ota_times(0);
     #endif
    #ifdef OTA_SUPPORT
    ui_ota_stop(TRUE);
    #endif

  }
  else
  {
    MT_ASSERT(0);
  }

  return ret;
}

RET_CODE open_ota_search(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_mask;
  control_t *p_collect, *p_info, *p_exit;
  control_t *p_check, *p_local, *p_service, *p_txt, *p_yes, *p_no;
  u16 y;
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SOFTWARE_UPGRADE, RSI_TITLE_BG, 0};

  memcpy((void *)&upgrade, (void *)para2, sizeof(ota_info_t));
  OS_PRINTF("pid = %d\n", upgrade.download_data_pid);
  OS_PRINTF("mod = %d\n", upgrade.lockc.nim_modulate);
  OS_PRINTF("freq = %d\n", upgrade.lockc.tp_freq);
  OS_PRINTF("sym = %d\n", upgrade.lockc.tp_sym);

  p_mask = ui_comm_root_create_full_screen(ROOT_ID_OTA_SEARCH,
                    RSI_FULL_SCREEN_BG,
                    0, 0,
                    SCREEN_WIDTH, SCREEN_HEIGHT,
                    &title_data);

  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_OTA_SEARCH_CONT,
          OTA_SEARCH_CONT_X, OTA_SEARCH_CONT_Y,
          OTA_SEARCH_CONT_W, OTA_SEARCH_CONT_H,
          p_mask, 0);
  ctrl_set_keymap(p_cont, ota_search_keymap);
  ctrl_set_proc(p_cont, ota_search_proc);

  //collect information container.
  p_collect = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_OTA_SEARCH_COLLECT,
                                  0, 0,
                                  OTA_SEARCH_CONT_W, 140,
                                  p_cont, 0);
  ctrl_set_rstyle(p_collect, RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID);
  ctrl_set_attr(p_collect, OBJ_ATTR_HIDDEN);

  y = OTA_SEARCH_ITEM_Y;
  p_info = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COLLECT_INFO,
                        OTA_SEARCH_ITEM_X, y,
                        OTA_SEARCH_ITEM_W, OTA_SEARCH_ITEM_H,
                        p_collect, 0);
  text_set_align_type(p_info, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_info, FSI_OTA_SEARCH_INFO,
    FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO);
  text_set_content_type(p_info, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_info, IDS_RECIEVING_DATA);

  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);
  p_exit = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COLLECT_EXIT,
                            OTA_COLLECT_EXIT_X, y,
                            OTA_COLLECT_EXIT_W, OTA_COLLECT_EXIT_H,
                            p_collect, 0);
  ctrl_set_keymap(p_exit, ota_search_btn_keymap);
  ctrl_set_proc(p_exit, ota_search_btn_proc);
  ctrl_set_rstyle(p_exit, RSI_COMMON_BTN_HL, RSI_COMMON_BTN_ORANGE, RSI_COMMON_BTN_HL);
  text_set_font_style(p_exit, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_content_type(p_exit, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_exit, IDS_EXIT);

  ctrl_set_active_ctrl(p_collect, p_exit);

  //check container.
  p_check = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_OTA_SEARCH_CHECK,
    0, 0,
    OTA_SEARCH_CONT_W, OTA_SEARCH_CONT_H,
    p_cont, 0);
  ctrl_set_keymap(p_check, ota_search_check_keymap);
  ctrl_set_rstyle(p_check, RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID);

  y = OTA_SEARCH_ITEM_Y;
  p_local = ui_comm_static_create(p_check, IDC_CHECK_LOCAL,
    OTA_SEARCH_ITEM_X, y,
    OTA_SEARCH_ITEM_LW, OTA_SEARCH_ITEM_RW);
  ui_comm_static_set_param(p_local, TEXT_STRTYPE_DEC);
  ui_comm_static_set_static_txt(p_local, IDS_LOCAL_SOFT_VER);

  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);

  p_service = ui_comm_static_create(p_check, IDC_CHECK_SERVICE,
    OTA_SEARCH_ITEM_X, y,
    OTA_SEARCH_ITEM_LW, OTA_SEARCH_ITEM_RW);
  ui_comm_static_set_param(p_service, TEXT_STRTYPE_DEC);
  ui_comm_static_set_static_txt(p_service, IDS_UPGRADE_SOFT_VER);

  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);

  p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHECK_TXT,
    OTA_SEARCH_ITEM_X, y,
    OTA_SEARCH_ITEM_W, OTA_SEARCH_ITEM_H,
    p_check, 0);
  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_OTA_SEARCH_INFO,
    FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, IDS_UPGRADE_TO_NEW_VER);

  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);

  p_yes = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHECK_YES,
    OTA_CHECK_YES_X, y,
    OTA_CHECK_YES_W, OTA_CHECK_YES_H,
    p_check, 0);
  ctrl_set_keymap(p_yes, ota_search_btn_keymap);
  ctrl_set_proc(p_yes, ota_search_btn_proc);
  ctrl_set_rstyle(p_yes, RSI_OTA_SEARCH_BTN_SH, RSI_OTA_SEARCH_BTN_HL, RSI_OTA_SEARCH_BTN_SH);
  text_set_font_style(p_yes, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN);
  text_set_content_type(p_yes, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_yes, IDS_YES);

  p_no = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHECK_NO,
    OTA_CHECK_NO_X, y,
    OTA_CHECK_NO_W, OTA_CHECK_NO_H,
    p_check, 0);
  ctrl_set_keymap(p_no, ota_search_btn_keymap);
  ctrl_set_proc(p_no, ota_search_btn_proc);
  ctrl_set_rstyle(p_no, RSI_OTA_SEARCH_BTN_SH, RSI_OTA_SEARCH_BTN_HL, RSI_OTA_SEARCH_BTN_SH);
  text_set_font_style(p_no, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN);
  text_set_content_type(p_no, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_no, IDS_NO);

  ctrl_set_related_id(p_yes, IDC_CHECK_NO, 0, IDC_CHECK_NO, 0);
  ctrl_set_related_id(p_no, IDC_CHECK_YES, 0, IDC_CHECK_YES, 0);

  ctrl_set_active_ctrl(p_check, p_yes);

  ota_search_set_menu_state(p_cont, TRUE, FALSE, 0);
  #ifdef OTA_SUPPORT
  ui_ota_start((u32)&upgrade);
  #endif
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}


static RET_CODE on_ota_search_exit_all(control_t *cont, u16 msg, u32 para1,
                             u32 para2)
{
  u16 pgid = sys_status_get_curn_group_curn_prog_id();
#ifdef OTA_SUPPORT
  ui_ota_release();
#endif
  if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
  {
    ui_play_prog(pgid, TRUE);
  }
  return ERR_NOFEATURE;
}

static RET_CODE on_ota_search_exit(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
  u16 pgid = sys_status_get_curn_group_curn_prog_id();
#ifdef OTA_SUPPORT
  ui_ota_release();
#endif
  if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
  {
    ui_play_prog(pgid, TRUE);
  }
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(ota_search_keymap, NULL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(ota_search_keymap, NULL)
BEGIN_MSGPROC(ota_search_proc, cont_class_proc)
  ON_COMMAND(MSG_OTA_FOUND, on_ota_search_found)
  ON_COMMAND(MSG_SAVE, on_ota_search_save)
  ON_COMMAND(MSG_OTA_TRIGGER_RESET, on_ota_search_reset)
  ON_COMMAND(MSG_EXIT_ALL, on_ota_search_exit_all)
  ON_COMMAND(MSG_EXIT, on_ota_search_exit)
END_MSGPROC(ota_search_proc, cont_class_proc)

BEGIN_KEYMAP(ota_search_btn_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ota_search_btn_keymap, NULL)

BEGIN_MSGPROC(ota_search_btn_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_ota_search_select)
END_MSGPROC(ota_search_btn_proc, text_class_proc)

BEGIN_KEYMAP(ota_search_check_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ota_search_check_keymap, NULL)

