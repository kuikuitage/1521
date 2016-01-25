#include "ui_common.h"
#include "ui_time_api.h"

u16 ui_time_evtmap(u32 event);

void ui_time_init(void)
{
  cmd_t cmd = {0};


  // activate upg
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_TIME;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_TIME, ui_time_evtmap);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_TIME_SET);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_PROG_BAR);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_PROG_LIST);
  fw_register_ap_msghost(APP_TIME, SN_ROOT_ID_SEARCH_RESULT);
  fw_register_ap_msghost(APP_TIME, SN_ROOT_ID_SEARCH);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_INFO);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_LEVEL);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_MOTHER);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_PAIR);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_PIN);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
  fw_register_ap_msghost(APP_TIME, ROOT_ID_SP_LIST);  
  fw_register_ap_msghost(APP_TIME, ROOT_ID_SP_LIST2);
  #endif
  fw_register_ap_msghost(APP_TIME, ROOT_ID_FAV_LIST);
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
  fw_register_ap_msghost(APP_TIME, ROOT_ID_FULL_SEARCH);
#endif
}

void ui_time_release(void)
{
  cmd_t cmd = {0};

  // deactivate upg
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_TIME;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(APP_TIME);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_BACKGROUND); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_TIME_SET);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_PROG_BAR);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_PROG_LIST);
  fw_unregister_ap_msghost(APP_TIME, SN_ROOT_ID_SEARCH_RESULT);
  fw_unregister_ap_msghost(APP_TIME, SN_ROOT_ID_SEARCH);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_INFO);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_LEVEL);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_MOTHER);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_PAIR);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_CONDITIONAL_ACCEPT_PIN);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
  fw_register_ap_msghost(APP_TIME, ROOT_ID_SP_LIST);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_SP_LIST2);
  #endif
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_FAV_LIST);
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_FULL_SEARCH);
#endif
}

void ui_time_req_tdt(void)
{
  cmd_t cmd = {0};

  cmd.id = TIME_CMD_REQ_TDT;

  ap_frm_do_command(APP_TIME, &cmd);
}

void ui_time_lnb_check_enable(BOOL is_enable)
{
  cmd_t cmd = {0};

  cmd.id = TIME_CMD_LNB_CHECK_ENABLE;
  cmd.data1 = (u32)is_enable;

  ap_frm_do_command(APP_TIME, &cmd);
}

BEGIN_AP_EVTMAP(ui_time_evtmap)
  CONVERT_EVENT(TIME_EVT_TIME_UPDATE, MSG_TIME_UPDATE) 
  CONVERT_EVENT(TIME_EVT_LNB_SHORT, MSG_LNB_SHORT)
  CONVERT_EVENT(TIME_EVT_HEART_BEAT, MSG_HEART_BEAT)    
END_AP_EVTMAP(ui_time_evtmap)
