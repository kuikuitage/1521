#include "sys_types.h"
#include "hal_misc.h"
#include "mtos_msg.h"
#include "ui_common.h"
#include "nim.h"
#include "ap_kingvon_ota.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_api.h"
#include "customer_def.h"

enum ota_upgrade_ctrl_id
{
  IDC_OTA_UPGRADE_TDI_INFO = 1,
  IDC_OTA_UPGRADE_STATE,
  IDC_OTA_UPGRADE_NOTES,
  IDC_OTA_UPGRADE_PBAR,
  IDC_OTA_UPGRADE_PER,
  IDC_OTA_UPGRADE_TTL,
};


u16 ota_upgrade_keymap(u16 key);
RET_CODE ota_upgrade_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

s32 open_ota_upgrade(u32 para1, u32 para2)
{
  struct control *p_cont, *p_state, *p_notes, *p_pbar;
  struct control *p_per, *p_title;

  p_cont = fw_create_mainwin(ROOT_ID_OTA_UPGRADE,
    OTA_UPGRADE_CONTX, OTA_UPGRADE_CONTY,
    OTA_UPGRADE_CONTW, OTA_UPGRADE_CONTH,
    0, 0, OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_OTA_UPGRADE_MENU,
    RSI_OTA_UPGRADE_MENU, RSI_OTA_UPGRADE_MENU);
  ctrl_set_proc(p_cont, ota_upgrade_proc);
  ctrl_set_keymap(p_cont, ota_upgrade_keymap);

  p_state = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_OTA_UPGRADE_STATE,
    OTA_UPGRADE_STATEX, OTA_UPGRADE_STATEY,
    OTA_UPGRADE_STATEW, OTA_UPGRADE_STATEH,
    p_cont, 0);
  ctrl_set_rstyle(p_state, RSI_OTA_UPGRADE_MENU,
    RSI_OTA_UPGRADE_MENU, RSI_OTA_UPGRADE_MENU);
  text_set_font_style(p_state, FSI_OTA_UPGRADE_TXT,
    FSI_OTA_UPGRADE_TXT, FSI_OTA_UPGRADE_TXT);
  text_set_align_type(p_state, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_state, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_state, IDS_OTA_SEARCHING);

  p_notes = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_OTA_UPGRADE_NOTES,
    OTA_UPGRADE_NOTESX, OTA_UPGRADE_NOTESY,
    OTA_UPGRADE_NOTESW, OTA_UPGRADE_NOTESH,
    p_cont, 0);
  ctrl_set_rstyle(p_notes, RSI_OTA_UPGRADE_MENU,
    RSI_OTA_UPGRADE_MENU, RSI_OTA_UPGRADE_MENU);
  text_set_font_style(p_notes, FSI_OTA_UPGRADE_TXT,
    FSI_OTA_UPGRADE_TXT, FSI_OTA_UPGRADE_TXT);
  text_set_align_type(p_notes, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_notes, TEXT_STRTYPE_UNICODE);

  p_pbar = ctrl_create_ctrl((u8*)CTRL_PBAR, IDC_OTA_UPGRADE_PBAR,
    OTA_UPGRADE_PBARX, OTA_UPGRADE_PBARY,
    OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH,
    p_cont, 0);
  ctrl_set_rstyle(p_pbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  pbar_set_mid_rect(p_pbar, 0, 0, OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH);
  pbar_set_rstyle(p_pbar, RSI_OTA_UPGRADE_PBAR_MIN,
    RSI_OTA_UPGRADE_PBAR_MAX, RSI_OTA_UPGRADE_PBAR_MIN);
  pbar_set_count(p_pbar, OTA_UPGRADE_PBAR_MIN,
    OTA_UPGRADE_PBAR_MAX, OTA_UPGRADE_PBAR_MAX);
  pbar_set_direction(p_pbar, 1);
  pbar_set_workmode(p_pbar, 1, 0);
  pbar_set_current(p_pbar, 0);

  p_per = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_OTA_UPGRADE_PER,
    OTA_UPGRADE_PERX, OTA_UPGRADE_PERY,
    OTA_UPGRADE_PERW, OTA_UPGRADE_PERH,
    p_cont, 0);
  ctrl_set_rstyle(p_per, RSI_OTA_UPGRADE_MENU,
    RSI_OTA_UPGRADE_MENU, RSI_OTA_UPGRADE_MENU);
  text_set_font_style(p_per, FSI_OTA_UPGRADE_TXT,
    FSI_OTA_UPGRADE_TXT, FSI_OTA_UPGRADE_TXT);
  text_set_align_type(p_per, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_per, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_per, (u8*)"0%");

  p_title = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_OTA_UPGRADE_TTL,
    OTA_UPGRADE_TTLX, OTA_UPGRADE_TTLY,
    OTA_UPGRADE_TTLW, OTA_UPGRADE_TTLH,
    p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_OTA_UPGRADE_MENU,
    RSI_OTA_UPGRADE_MENU, RSI_OTA_UPGRADE_MENU);
  text_set_font_style(p_title, FSI_OTA_UPGRADE_TXT,
    FSI_OTA_UPGRADE_TXT, FSI_OTA_UPGRADE_TXT);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_OTA_UPGRADE);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

static RET_CODE on_ota_upgarde_exit(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cmd_t cmd;

  cmd.id = OTA_CMD_STOP;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_OTA, &cmd);
  return SUCCESS;
}

static RET_CODE on_ota_upgrade_step(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_pbar, *p_per;
  char ascstr[8];
  u8 string_pro[8];
  //u8 string_temp[4];
  void *p_dev = NULL;
  static u32 temp_flag = 0;
  
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  
  p_pbar = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PBAR);
  p_per = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PER);

  pbar_set_current(p_pbar, (u16)para1);
  ctrl_paint_ctrl(p_pbar, TRUE);

  //OS_PRINTF("percent [%d]", para1);
  //OS_PRINTF("\n para2 is %d",para2);
  memset(string_pro, 0, sizeof(string_pro));
  if(para2 == 1)
    temp_flag =1;
  
  if( 0 == temp_flag)
  { 
    if(para1 == 100)
      sprintf((char*)string_pro, "%03d ", (int)para1);
    else
      sprintf((char*)string_pro, "D%02d ", (int)para1);
  }
  else if( 1 == temp_flag )
  {
    if(para1 == 100)
      sprintf((char*)string_pro, "%03d ", (int)para1);
    else
      sprintf((char*)string_pro, "B%02d ", (int)para1);
  }
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  //mtos_printk("%s\n", string_pro);
  uio_display(p_dev, string_pro, 4);
  
  sprintf(ascstr, "%d%%", (int)para1);
  text_set_content_by_ascstr(p_per, (u8*)ascstr);
  ctrl_paint_ctrl(p_per, TRUE);

  return SUCCESS;
}

static RET_CODE on_ota_unzip_ok(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
/*
    comm_dlg_data_t dlg_data =
    {
      ROOT_ID_OTA_UPGRADE,
      DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
      0,
      0,
    };

  u16 uni_str[32];

  str_asc2uni("Uzip Ok, Burn Flash Now?", uni_str);

  dlg_data.content = (u32)uni_str;

  if(DLG_RET_YES == ui_comm_dlg_open(&dlg_data))
  {
    ui_ota_burn_flash(TRUE);
  }
  else
  {
    ui_ota_burn_flash(FALSE);
  }
*/
    ui_ota_burn_flash(TRUE);

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_start_download(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_OTA_DOWNLOADING);
  ctrl_paint_ctrl(p_states, TRUE);

  text_set_content_by_strid(p_notes, IDS_PRESS_EXIT_TO_EXIT_UPGRADE_AND_RESTART);
  ctrl_paint_ctrl(p_notes, TRUE);

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_checking(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_CHECKING_UPGRADE_FILE);
  ctrl_paint_ctrl(p_states, TRUE);

  //mtos_task_sleep(3000);

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_burning(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control  *p_states;

  //p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_BURNING_FLASH);
  ctrl_paint_ctrl(p_states, TRUE);

  //text_set_content_by_strid(p_notes, IDS_DONT_POWER_OFF);
  //ctrl_paint_ctrl(p_notes, TRUE);

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_finish(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_OTA_UPGRADE_COMPLETE);
  ctrl_paint_ctrl(p_states, TRUE);

  text_set_content_by_strid(p_notes, IDS_STB_WILL_AUTO_RESET_THEN);
  ctrl_paint_ctrl(p_notes, TRUE);

  ui_ota_stop();

  return SUCCESS;
}

BEGIN_KEYMAP(ota_upgrade_keymap, NULL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(ota_upgrade_keymap, NULL)

BEGIN_MSGPROC(ota_upgrade_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ota_upgarde_exit)
  ON_COMMAND(MSG_OTA_PROGRESS, on_ota_upgrade_step)
  ON_COMMAND(MSG_OTA_UNZIP_OK, on_ota_unzip_ok)
  ON_COMMAND(MSG_OTA_START_DL, on_ota_upgrade_start_download)
  ON_COMMAND(MSG_OTA_CHECKING, on_ota_upgrade_checking)
  ON_COMMAND(MSG_OTA_BURNING, on_ota_upgrade_burning)
  ON_COMMAND(MSG_OTA_FINISH, on_ota_upgrade_finish)
END_MSGPROC(ota_upgrade_proc, cont_class_proc);

