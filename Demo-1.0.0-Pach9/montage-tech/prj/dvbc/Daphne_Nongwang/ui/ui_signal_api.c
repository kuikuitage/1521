/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"

static BOOL g_is_lock = TRUE;
static struct signal_data g_sign_data = {0, 0, TRUE};
static dvbs_lock_info_t g_tuner_param = {{0}};
static BOOL is_show_signal_logo = FALSE;

u16 ui_signal_evtmap(u32 event);

void set_signal_logo_show_flag(BOOL  is_show)
{
  is_show_signal_logo = is_show;
}

BOOL get_signal_logo_show_flag()
{
  return is_show_signal_logo;
}


void ui_init_signal(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  g_tuner_param.tp_rcv.freq = 4125;
  g_tuner_param.tp_rcv.sym = 1000;
  g_tuner_param.tp_rcv.polarity = 0;

  fw_register_ap_evtmap(APP_SIGNAL_MONITOR, ui_signal_evtmap);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_SIGNAL_CHECK);
}

void ui_enable_signal_monitor(BOOL is_enable)
{
  cmd_t cmd = {0};
  
  cmd.id = SIG_MON_MONITOR_PERF;
  cmd.data1 = is_enable;
  
  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);
}

void ui_release_signal(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_SIGNAL_CHECK);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_unregister_ap_evtmap(APP_SIGNAL_MONITOR);
}


void ui_set_transpond(dvbs_tp_node_t *p_tp)
{
  cmd_t cmd = {0};
  tp_rcv_para_t *p_tp_rcv = &g_tuner_param.tp_rcv;

  trans_tp_info(p_tp_rcv, p_tp);
    
  cmd.id = SIG_MON_LOCK_TP;
  cmd.data1 = (u32)(&g_tuner_param);
  cmd.data2 = SYS_DVBC;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);

  UI_PRINTF("----------------------------------\n");
  UI_PRINTF("- set transpond id = %d\n", p_tp->id);
  UI_PRINTF("---freq = %d\n", p_tp->freq);
  UI_PRINTF("---symbol = %d\n", p_tp->sym);
  UI_PRINTF("---modulation = %d\n", p_tp->nim_modulate);
  UI_PRINTF("----------------------------------\n");

}

BOOL ui_signal_is_lock(void)
{
  return g_is_lock;
}

void ui_signal_set_lock(BOOL is_lock)
{
  g_is_lock = is_lock;
}

static u8 is_show_radio_logo = TRUE;
extern void set_signal_flag(BOOL flag);
extern u16 get_message_strid();
BOOL ui_signal_show_logo(u32 para1, u32 para2)
{
  static u32 unlock_count = 0;
  static u32 lock_count = 0;
  BOOL is_lock = (BOOL)para2;
  u8 curn_mode= 0;
  u32 limit = 3;
  void *p_video_dev = NULL;
  curn_mode = sys_status_get_curn_prog_mode();
  set_signal_flag(FALSE);

  if( (fw_find_root_by_id(SN_ROOT_ID_SEARCH_RESULT)!=NULL)
    || (fw_find_root_by_id(SN_ROOT_ID_SEARCH)!=NULL)
    || (fw_find_root_by_id(ROOT_ID_SIGNAL_CHECK)!=NULL)
    || (fw_find_root_by_id(ROOT_ID_OTA_SEARCH)!=NULL))
  {
    UI_PRINTF("ui_signal_show_logo ignore show logo\n");
    return TRUE;
  }

  if(is_lock == FALSE)
  {
    if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
    {
      if(get_signal_logo_show_flag() == FALSE)
      {
        unlock_count++;
      }

      update_signal();
      if((unlock_count == limit) && (get_signal_logo_show_flag() == FALSE) && get_message_strid() != IDS_LOCK)
      {
        UI_PRINTF("ui_signal_show_logo\n");
        ui_set_channel_mode(FALSE);
        ui_show_logo(LOGO_BLOCK_ID_M2); 
        lock_count = 0;
        unlock_count = 0;
        set_signal_logo_show_flag(TRUE);
        is_show_radio_logo = FALSE;
      }
    }
  }
  else
  {
    if(is_show_radio_logo == FALSE)
    {
      lock_count++;
    }

    if((lock_count == limit) && (curn_mode != CURN_MODE_RADIO))
    {
      ui_set_channel_mode(TRUE);
    }

    if((lock_count == limit) && (is_show_radio_logo == FALSE))
    {
      lock_count = 0;
      is_show_radio_logo = TRUE;
      p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                                              SYS_DEV_TYPE_VDEC_VSB);

      if(get_message_strid() != RSC_INVALID_ID)
      {
        UI_PRINTF("get_message_strid()=%d\n",get_message_strid());
        vdec_clear_videobuf(p_video_dev);
      }
      if(curn_mode == CURN_MODE_RADIO)
      {
        ui_set_channel_mode(FALSE);
        ui_show_logo(LOGO_BLOCK_ID_M1);
      }
    }
    unlock_count = 0;
    set_signal_logo_show_flag(FALSE);

  }

  return TRUE;
}


void ui_signal_check(u32 para1, u32 para2)
{
  menu_attr_t *p_attr;
  control_t *p_root;
  BOOL bret_sig_callback=FALSE;
  BOOL is_showLogo = FALSE;
  sig_mon_info_t *p_signal;

#ifdef CUS_TONGGUANG_HEBI
  is_showLogo = TRUE;
#endif

  if(is_showLogo == TRUE)
  {
     bret_sig_callback = ui_signal_show_logo(para1, para2);
  }
  
  if(para1 != 0)
  {
    p_signal = (sig_mon_info_t *)para1;

    g_sign_data.intensity = (u8)p_signal->strength;
    g_sign_data.quality = (u8)p_signal->snr;
    g_sign_data.ber_c = p_signal->ber_c;
  }
  g_sign_data.lock = (BOOL)para2;
  
  p_attr = manage_get_curn_menu_attr();
  p_root = fw_find_root_by_id(p_attr->root_id);

  if (g_sign_data.lock != g_is_lock)
  {
    g_is_lock = g_sign_data.lock;
   
   if(!bret_sig_callback)
   {
     update_signal();
   }

    if (p_attr->signal_msg == SM_LOCK)
    {
      if (p_root != NULL)
      {
        ctrl_process_msg(p_root, 
          (u16)(g_is_lock ? MSG_SIGNAL_LOCK : MSG_SIGNAL_UNLOCK),
          0, 0);
      }
    }
  }

  if (p_attr->signal_msg == SM_BAR)
  {
    if (p_root != NULL)
    { 
      ctrl_process_msg(p_root, MSG_SIGNAL_UPDATE,
        (u32) & g_sign_data, 0);
    }
  }
}

BEGIN_AP_EVTMAP(ui_signal_evtmap)
  CONVERT_EVENT(SIG_MON_SIGNAL_INFO, MSG_SIGNAL_CHECK)
  CONVERT_EVENT(SIG_MON_POS_TURNING, MSG_MOTOR_DRVING)
  CONVERT_EVENT(SIG_MON_POS_OUT_RANGE, MSG_MOTOR_OUT_RANGE)
END_AP_EVTMAP(ui_signal_evtmap)



