#include "ui_common.h"
#include "ui_browser_api.h"
#include "ui_config.h"
#include "ap_playback.h"
#include "ap_browser.h"

static BOOL b_browser_started = FALSE;

u16 ui_browser_evtmap(u32 event);

static s32 get_ui_region_handle(void)
{
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);

  gdi_set_enable(TRUE);

  return gdi_get_screen_handle();
}
static void recover_ui_region(void)
{
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(TRUE);

  gdi_clear_screen();

  ui_config_normal_osd();
  gdi_set_enable(TRUE);

}
void ui_browser_init(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_BROWSER;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_BROWSER, ui_browser_evtmap);
  fw_register_ap_msghost(APP_BROWSER, ROOT_ID_BACKGROUND); 
}

void ui_browser_release(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_BROWSER;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_evtmap(APP_BROWSER);
  fw_unregister_ap_msghost(APP_BROWSER, ROOT_ID_BACKGROUND); 
}

void ui_browser_start(void)
{
  cmd_t cmd = {0};
  s32 ui_region_handle = 0;

  ui_region_handle = get_ui_region_handle();
  
  cmd.id = CMD_START_BROWSER;
  cmd.data1 = (u32)ui_region_handle;
  cmd.data2 = 0;
  ap_frm_do_command(APP_BROWSER, &cmd);

}

void ui_browser_stop(void)
{
  cmd_t cmd = {0};
  
  cmd.id = CMD_STOP_BROWSER;
  cmd.data1 = APP_BROWSER;
  ap_frm_do_command(APP_BROWSER, &cmd);    
}

void ui_browser_enter(void)
{

  OS_PRINTF("\r\n----browser enter ----\n");

  ui_stop_play(STOP_PLAY_FREEZE, TRUE);
  ui_epg_stop();
  ui_epg_release();
  
  ui_browser_init();
  ui_browser_start();
  b_browser_started = TRUE;

}

void ui_browser_exit(void)
{

  OS_PRINTF("\r\n----browser exit  started[%d]----\n",b_browser_started);

  if(b_browser_started)
  {
    ui_browser_stop();
    ui_browser_release();

    recover_ui_region();
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
    ui_epg_init();
    ui_epg_start();
    b_browser_started = FALSE;
  }
}


BEGIN_AP_EVTMAP(ui_browser_evtmap)
  CONVERT_EVENT(BROWSER_EVT_STOP,   MSG_BROWSER_EXIT) 
  CONVERT_EVENT(BROWSER_EVT_START,   MSG_BROWSER_ENTER) 
END_AP_EVTMAP(ui_browser_evtmap)


