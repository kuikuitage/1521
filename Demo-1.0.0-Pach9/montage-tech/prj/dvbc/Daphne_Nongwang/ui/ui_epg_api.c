#include "ui_common.h"
#include "ui_epg_api.h"
#include "ui_config.h"

u16 ui_epg_evtmap(u32 event);

void ui_epg_init(void)
{
#if 1
  cmd_t cmd = {0};


  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_EPG;
  OS_PRINTF("\nACTIVATE EPG\n");
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_EPG, ui_epg_evtmap);
  fw_register_ap_msghost(APP_EPG, ROOT_ID_EPG); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PRO_CLASSIFY); 
#endif  
}

void ui_epg_release(void)
{
#if 1
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_EPG;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_evtmap(APP_EPG);
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_EPG); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST);
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PRO_CLASSIFY); 
#endif  
}

void ui_epg_start(void)
{
#if 1
  cmd_t cmd = {0};
 
  cmd.id = EPG_CMD_START_SCAN;
  cmd.data1 = APP_EPG;
  cmd.data2 = BLOCK_COMMON_BUFFER;
  ap_frm_do_command(APP_EPG, &cmd);  
#endif  
}

void ui_epg_stop(void)
{
#if 1
  cmd_t cmd = {0};
 
  cmd.id = EPG_CMD_STOP_SCAN;
  cmd.data1 = APP_EPG;
  cmd.data2 = BLOCK_COMMON_BUFFER;
  ap_frm_do_command(APP_EPG, &cmd);    
#endif  
}

void ui_set_epg_dy_by_offset(u16 offset)
{
  dvbs_prog_node_t pg;
  u16 curn_group;
  u16 curn_mode;
  s16 pg_pos;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 total=0;
  epg_dy_policy_t *p_dy_policy = NULL;
  u16 i;

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, 
                                    (u16 *)&pg_id, (u16 *)&pg_pos, (u32 *)&group_context);
  view_id = ui_dbase_get_pg_view_id();  
  total = db_dvbs_get_count(view_id);
  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("\n##debug: epg_data_get_dy_policy NULL!\n");
    return ;
  }
  
  for (i = 0; i < DY_SVC_NUM ; i++)  //left side num 
  {
    if(offset + i >= total)
    {
      break;
    }
    pg_id = db_dvbs_get_id_by_view_pos(view_id, (offset + i));
    
    //pg_id = db_dvbs_get_id_by_view_pos(view_id,i+pg_pos-offset);
    db_dvbs_get_pg_by_id(pg_id, &pg);  
    p_dy_policy[i].cur_net_id = (u16)pg.orig_net_id;
    p_dy_policy[i].cur_ts_id = (u16)pg.ts_id;
    p_dy_policy[i].cur_svc_id = (u16)pg.s_id;
#if 0
    OS_PRINTF("ui_set_epg_dy_offset: net:%d ts:%d svc:%d\n",  
    p_dy_policy[i].cur_net_id, p_dy_policy[i].cur_ts_id, p_dy_policy[i].cur_svc_id);
#endif
  }
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);
}

void ui_set_epg_dy(void)
{
  dvbs_prog_node_t pg;
  u16 curn_group;
  u16 curn_mode;
  s16 pg_pos;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 i;
  u16 page;
  u16 total;
  u16 offset;
  u16 mubiao;
  epg_dy_policy_t *p_dy_policy = NULL;
  utc_time_t g_time = {0};
  utc_time_t add_time = {0};
 
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, 
                                    (u16 *)&pg_id, (u16 *)&pg_pos, (u32 *)&group_context);
  view_id = ui_dbase_get_pg_view_id();  
  total = db_dvbs_get_count(view_id);

  page = DY_SVC_NUM;

  if(pg_pos < page)
  {
    offset = pg_pos;
  }
  else if(total-pg_pos <= page)
  {
    offset = pg_pos-(total-page);
  }
  else
  {
    offset = 0;
  }

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("\n##debug: epg_data_get_dy_policy NULL!\n");
    return ;
  }
  
#if 1
  time_get(&g_time, TRUE);
  memcpy(&p_dy_policy->start, &g_time, sizeof(utc_time_t));
  p_dy_policy->start.hour = 0;
  p_dy_policy->start.minute = 0;
  p_dy_policy->start.second = 0;
  add_time.day = 6;
  
  //add_time.hour = 12;
  memcpy(&p_dy_policy->end, &p_dy_policy->start, sizeof(utc_time_t));  
  time_add(&p_dy_policy->end, &add_time);
#endif

  for (i = 0; i < DY_SVC_NUM ; i++)
  {        
    mubiao = i + pg_pos - offset;
    if(mubiao >= total)
    {
      break;
    }
    pg_id = db_dvbs_get_id_by_view_pos(view_id, mubiao);
    db_dvbs_get_pg_by_id(pg_id, &pg);  
    p_dy_policy[i].cur_net_id = (u16)pg.orig_net_id;
    p_dy_policy[i].cur_ts_id = (u16)pg.ts_id;
    p_dy_policy[i].cur_svc_id = (u16)pg.s_id;
#if 0
    OS_PRINTF("!!!epg prog bar set dy time: %d:%d:%d %d:%d:%d%-----",
  p_dy_policy->start.year, p_dy_policy->start.month, p_dy_policy->start.day,
  p_dy_policy->start.hour, p_dy_policy->start.minute, p_dy_policy->start.second);
  OS_PRINTF("%d:%d:%d %d:%d:%d%\n",
  p_dy_policy->end.year, p_dy_policy->end.month, p_dy_policy->end.day,
  p_dy_policy->end.hour, p_dy_policy->end.minute, p_dy_policy->end.second);
    OS_PRINTF("ui_set_epg_dy: net:%d ts:%d svc:%d\n",  
      p_dy_policy[i].cur_net_id, p_dy_policy[i].cur_ts_id, p_dy_policy[i].cur_svc_id);
#endif
  }
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);
}

void ui_set_epg_day_offset(u8 offset)
{
  u16 curn_group;
  u16 curn_mode;
  s16 pg_pos;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 total;
  epg_dy_policy_t *p_dy_policy = NULL;
  utc_time_t g_time = {0};
  utc_time_t add_time = {0};
  //cmd_t cmd = {0};
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, 
                                    (u16 *)&pg_id, (u16 *)&pg_pos, (u32 *)&group_context);
  view_id = ui_dbase_get_pg_view_id();  
  total = db_dvbs_get_count(view_id);

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("!!!!!!!!!!p_dy_policy = NULL\n");
    return ;
  }

  time_get(&g_time, TRUE);
  add_time.day = offset;
  time_add(&g_time, &add_time);
  memcpy(&p_dy_policy->start, &g_time, sizeof(utc_time_t));    
  add_time.day = 6;
  time_add(&g_time, &add_time);
  memcpy(&p_dy_policy->end, &g_time, sizeof(utc_time_t));  
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);  
}

BEGIN_AP_EVTMAP(ui_epg_evtmap)
  CONVERT_EVENT(EPG_EVT_EVT_DELETED,    MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_TAB_READY,      MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_EVT_READY,      MSG_EPG_READY)  
  CONVERT_EVENT(EPG_EVT_PF_READY,       MSG_PF_READY) 
  CONVERT_EVENT(EPG_EVT_NEW_SEC_FOUND,    MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_STOP,           MSG_STOP_EPG)
END_AP_EVTMAP(ui_epg_evtmap)


