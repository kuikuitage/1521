/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Steven Wang <steven.wang@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

u16 ui_satcodx_evtmap(u32 event);

void ui_init_satcodx(void)
{
	cmd_t cmd = {0};

  mem_cfg(MEMCFG_T_NO_AV);

	cmd.id = AP_FRM_CMD_ACTIVATE_APP;
	cmd.data1 = APP_SATCODX;	
	ap_frm_do_command(APP_FRAMEWORK, &cmd); 

  fw_register_ap_evtmap(APP_SATCODX, ui_satcodx_evtmap);
  fw_register_ap_msghost(APP_SATCODX, ROOT_ID_SATCODX);

}

void ui_release_satcodx(void)
{
	cmd_t cmd = {0};

	cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
	cmd.data1 = APP_SATCODX;
	ap_frm_do_command(APP_FRAMEWORK, &cmd);

	fw_unregister_ap_evtmap(APP_SATCODX);	
	fw_unregister_ap_msghost(APP_SATCODX, ROOT_ID_SATCODX);

  mem_cfg(MEMCFG_T_NORMAL);
}

void ui_start_satcodx(void)
{
	cmd_t cmd = { 0 };

  cmd.id = SATCODX_CMD_START;  
  cmd.data1 = BLOCK_UNION_BUFFER;
  ap_frm_do_command(APP_SATCODX, &cmd);
}

void ui_stop_satcodx(void)
{
	cmd_t cmd = { 0 };

	cmd.id = SATCODX_CMD_STOP;	

  ap_frm_do_command(APP_SATCODX, &cmd);
}

BEGIN_AP_EVTMAP(ui_satcodx_evtmap)
  CONVERT_EVENT(SATCODX_EVT_UPDATE_STATUS, MSG_SATCODX_UPDATE_STATUS)
END_AP_EVTMAP(ui_satcodx_evtmap)


