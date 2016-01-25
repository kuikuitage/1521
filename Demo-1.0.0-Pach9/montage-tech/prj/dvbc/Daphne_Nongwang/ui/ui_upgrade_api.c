#include "ui_common.h"
#include "ui_upgrade_api.h"
#ifdef WIN32
#include "uart_win32.h"
#endif
u16 ui_upg_evtmap(u32 event);

static RET_CODE upgrade_null_write_byte(u8 id, u8 chval)
{
  return SUCCESS;
}


static RET_CODE upgrade_null_read_byte(u8 id, u8 *data, u32 timeout)
{
  return SUCCESS;
}


void ui_init_upg(void)
{
  cmd_t cmd = {0};

#ifdef  CORE_DUMP_DEBUG
  mtos_task_suspend(TRANSPORT_SHELL_TASK_PRIORITY);
#endif

  // deactivate others
  ui_epg_stop();
  ui_epg_release();
  ui_enable_playback(FALSE);
  ui_release_signal();
  

  // activate upg
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_UPG;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_UPG, ui_upg_evtmap);
}


void ui_release_upg(void)
{
  cmd_t cmd = {0};

  // deactivate upg
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UPG;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(APP_UPG);
  
#ifndef WIN32
  mtos_register_putchar((s32 (*)(u8, u8))(uart_write_byte));
  mtos_register_getchar((s32 (*)(u8, u8 *, u32))(uart_read_byte));
#endif

  ui_init_signal();
  
  ui_enable_playback(TRUE);
  ui_epg_init();
  ui_epg_start();
  
#ifdef  CORE_DUMP_DEBUG
  mtos_task_resume(TRANSPORT_SHELL_TASK_PRIORITY);
#endif
}


void ui_start_upgrade(upg_mode_t md, u8 block_num, upg_block_t *block)
{
  cmd_t cmd;

  cmd.id = UPG_CMD_START;
  cmd.data1 = (md << 16) | block_num;
  cmd.data2 = (u32)block;
  
  ap_frm_do_command(APP_UPG, &cmd);

#ifndef WIN32
  mtos_register_putchar(upgrade_null_write_byte);
  mtos_register_getchar(upgrade_null_read_byte);
#else
  uartwin_init(3);
#endif
}


void ui_exit_upgrade(BOOL is_exit_all)
{
  cmd_t cmd;

  cmd.id = UPG_CMD_EXIT;
  cmd.data1 = is_exit_all;
  cmd.data2 = 0;

  ap_frm_do_command(APP_UPG, &cmd);
}


BEGIN_AP_EVTMAP(ui_upg_evtmap)
  CONVERT_EVENT(UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(UPG_EVT_SUCCESS, MSG_UPG_OK)
END_AP_EVTMAP(ui_upg_evtmap)
