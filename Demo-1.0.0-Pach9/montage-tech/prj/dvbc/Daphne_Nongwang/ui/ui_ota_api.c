#include "ui_common.h"
#include "ap_ota.h"
//#include <sys_types.h>
//#include <mdl/mmi/ctrl_base.h>
//#include "abs_nit.h"
//#include "eit.h"
#ifdef OTA_SUPPORT   

u16 ui_ota_evtmap(u32 event);
#ifdef OTA_CHECK_DEFAULT
u16 ui_ota_mon_evtmap(u32 event);
#endif
void ui_ota_init(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_OTA;
  OS_PRINTF("\nACTIVATE OTA\n");
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_register_ap_evtmap(APP_OTA, ui_ota_evtmap);
  fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
}

void ui_ota_release(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_OTA;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(APP_OTA);
  fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
}

void ui_ota_start(u32 para1)
{
  cmd_t cmd;

  cmd.id = OTA_CMD_START_SEARCHING;
  cmd.data1 = para1;
  cmd.data2 = 0;
  
  ap_frm_do_command(APP_OTA, &cmd);   

}

void ui_ota_stop(BOOL is_save)
{
  cmd_t cmd;
  
  cmd.id = OTA_CMD_STOP_SEARCHING;
  cmd.data1 = is_save;
  cmd.data2 = 0;
  
  ap_frm_do_command(APP_OTA, &cmd);   
}
#ifdef OTA_CHECK_DEFAULT
void ui_ota_monitor_init(void)
{
  cmd_t cmd;
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_RESERVED1;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  fw_register_ap_evtmap(APP_RESERVED1, ui_ota_mon_evtmap);
  fw_register_ap_msghost(APP_RESERVED1, ROOT_ID_BACKGROUND);
}
void ui_ota_monitor_start(u32 para1)
{
  cmd_t cmd;
  cmd.id = OTA_MON_CMD_MONITOR_START;
  cmd.data1 = 0;
  cmd.data2 = 0;
  ap_frm_do_command(APP_RESERVED1, &cmd);
}
#endif

BEGIN_AP_EVTMAP(ui_ota_evtmap)
  CONVERT_EVENT(OTA_EVT_UPG_INFO_FOUND,   MSG_OTA_FOUND)
  CONVERT_EVENT(OTA_EVT_TRIGGER_RESET, MSG_OTA_TRIGGER_RESET)  
END_AP_EVTMAP(ui_ota_evtmap)
#ifdef OTA_CHECK_DEFAULT
BEGIN_AP_EVTMAP(ui_ota_mon_evtmap)
  CONVERT_EVENT(OTA_MON_EVT_FORCE_OTA, MSG_OTA_MON_FORCE_OTA)
  CONVERT_EVENT(OTA_MON_EVT_MAIN_FREQ_FORCE_OTA, MSG_OTA_MON_MAIN_FREQ_FORCE_OTA)
  CONVERT_EVENT(OTA_MON_EVT_MAIN_FREQ_TMOUT, MSG_OTA_MON_MAIN_FREQ_TMOUT)
END_AP_EVTMAP(ui_ota_mon_evtmap)
#endif

#endif
