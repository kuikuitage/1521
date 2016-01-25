#include "ui_common.h"
#include "ui_nvod_api.h"
#include "ui_config.h"
#include "nvod_data.h"
#include "ap_playback.h"

#define MAX_NVOD_TP  (10)

static nvod_tp_info_t tp_info[MAX_NVOD_TP];
static u8 g_nvod_tp_num = 0;
static nvod_reference_svc_evt_t *p_playing_ref_svc_evt = NULL;
static time_shifted_svc_evt_t *p_playing_time_shift_svc_evt = NULL;

static play_param_t g_nvod_pb_info;// = {SYS_DVBC, 0, 0};

static BOOL is_nvod_started = FALSE;


static BOOL is_nvod_playing = FALSE;

static BOOL ui_create_nvod_tp(void);

u16 ui_nvod_evtmap(u32 event);

void ui_nvod_init(void)
{

  cmd_t cmd = {0};
  
  memset(&g_nvod_pb_info, 0, sizeof(play_param_t));
  g_nvod_pb_info.lock_mode = SYS_DVBC;

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_NVOD;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_NVOD, ui_nvod_evtmap);
  fw_register_ap_msghost(APP_NVOD, ROOT_ID_NVOD); 
  fw_register_ap_msghost(APP_NVOD, ROOT_ID_NVOD_VIDEO); 

}

void ui_nvod_release(void)
{
#if 1
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_NVOD;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_evtmap(APP_NVOD);
  fw_unregister_ap_msghost(APP_NVOD, ROOT_ID_NVOD); 
  fw_unregister_ap_msghost(APP_NVOD, ROOT_ID_NVOD_VIDEO); 
#endif  
}

void ui_nvod_start(void)
{
  cmd_t cmd = {0};

  ui_create_nvod_tp();

  OS_PRINTF("\r\n----nvod start cmd:tp num[%d],first freq[%d],is main_freq[%s]\n",
                      g_nvod_tp_num, tp_info[0].frequency, tp_info[0].main_freq ? "yes":"no");
  
  cmd.id = CMD_START_NVOD;
  cmd.data1 = (u32)(&tp_info);
  cmd.data2 = g_nvod_tp_num;
  ap_frm_do_command(APP_NVOD, &cmd);

  is_nvod_started = TRUE;
}

BOOL ui_nvod_start_ref_svc(dvbc_tp_info_t *p_tp)
{
cmd_t cmd = {0};
  
  cmd.id = CMD_NVOD_START_REF_SVC;
  cmd.data1 = (u32)p_tp;
  
  ap_frm_do_command(APP_NVOD, &cmd);

  return TRUE;
}

void ui_nvod_stop_ref_svc(void)
{
#if 1
  cmd_t cmd = {0};
  
  if(is_nvod_started)
  {
    cmd.id = CMD_NVOD_STOP_REF_SVC;
    cmd.data1 = APP_NVOD;
    ap_frm_do_command(APP_NVOD, &cmd);    
  }
#endif  
}

void ui_nvod_stop(void)
{
#if 1
  cmd_t cmd = {0};
  
  if(is_nvod_started)
  {
    cmd.id = CMD_STOP_NVOD;
    cmd.data1 = APP_NVOD;
    ap_frm_do_command(APP_NVOD, &cmd);    
  }
#endif  
}

void ui_set_nvod_play_state(BOOL b)
{
  is_nvod_playing = b;
}

BOOL ui_is_nvod_playing()
{
  return is_nvod_playing;
}

void ui_set_playing_nvod_reference_svc_evt(nvod_reference_svc_evt_t *p_ref_svc_event)
{
  p_playing_ref_svc_evt = p_ref_svc_event;
}

nvod_reference_svc_evt_t * ui_get_playing_nvod_reference_svc_evt()
{
  return p_playing_ref_svc_evt;
}

void ui_set_playing_nvod_time_shifted_svc_evt(time_shifted_svc_evt_t *p_shift_svc_event)
{
  p_playing_time_shift_svc_evt = p_shift_svc_event;
}

time_shifted_svc_evt_t * ui_get_playing_nvod_time_shifted_svc_evt()
{
  return p_playing_time_shift_svc_evt;
}

BOOL nvod_play_offset(s8 offset)
{
  u32 i;
  u32 shift_event_count = 0;
  u16 shift_event_index = 0;
  s32 dividend = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = ui_get_playing_nvod_reference_svc_evt();
  time_shifted_svc_evt_t *p_shift_event = ui_get_playing_nvod_time_shifted_svc_evt();
  time_shifted_svc_evt_t *p_shift_event_temp = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;

  if(nvod_data_get_ref_svc_cnt(h_nvod) <= 0)
  {
    return FALSE;
  }

  if(p_ref_event != NULL)
  {
    p_shift_event_temp = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }
  else
  {
    return FALSE;
  }
 
  while(p_shift_event_temp != NULL)
  {
    if(p_shift_event == p_shift_event_temp)
    {
      break;
    }
    else
    {  
      p_shift_event_temp = nvod_data_get_shift_evt_next(h_nvod, p_shift_event_temp);
      shift_event_index++;
    }
  }

  if(p_shift_event_temp != NULL)
  {
    dividend = shift_event_index + offset;
    while (dividend < 0)
    {
      dividend += shift_event_count;
    }
    shift_event_index = (u16)((u32)(dividend) % shift_event_count);

    p_shift_event_temp = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
    for(i=0; i<shift_event_index; i++)
    {
      if(p_shift_event_temp == NULL)
      {
        return FALSE;
      }
      p_shift_event_temp = nvod_data_get_shift_evt_next(h_nvod, p_shift_event_temp);
    }

    if(p_shift_event_temp == NULL)
    {
      return FALSE;
    }

    if(p_shift_event_temp->evt_status != 1)
    {
      p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
      if(offset>0)
      {
      }
      else
      {
        p_shift_event_temp = p_shift_event;
        while(p_shift_event_temp->evt_status == 1 && p_shift_event_temp != NULL)
        {
          p_shift_event = p_shift_event_temp;
          p_shift_event_temp = nvod_data_get_shift_evt_next(h_nvod, p_shift_event_temp);
        }
      }
    }
    else
    {
      p_shift_event = p_shift_event_temp;
    }
  }

  if(ui_get_playing_nvod_time_shifted_svc_evt() != p_shift_event)
  {
    if(p_shift_event != NULL && p_shift_event->evt_status == 1)
    {
      p_ref_svc = nvod_data_get_ref_svc(h_nvod, p_ref_event->ref_svc_idx);
      p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);

      ui_set_playing_nvod_reference_svc_evt(p_ref_event);
      ui_set_playing_nvod_time_shifted_svc_evt(p_shift_event);
      
      ui_play_nvod(p_ts_svc, &p_ref_svc->tp);
    }
  }

  return TRUE;
}


static void nvod_play_info_print()
{
  OS_PRINTF("=========nvod_play_info_print========\n");
  OS_PRINTF("g_nvod_pb_info.lock_mode:%d\n",g_nvod_pb_info.lock_mode);
  OS_PRINTF("g_nvod_pb_info.inner.dvbc_lock_info.tp_sym:%d\n",g_nvod_pb_info.inner.dvbc_lock_info.tp_sym);
  OS_PRINTF("g_nvod_pb_info.inner.dvbc_lock_info.tp_freq:%d\n",g_nvod_pb_info.inner.dvbc_lock_info.tp_freq);
  OS_PRINTF("g_nvod_pb_info.inner.dvbc_lock_info.nim_modulate:%d\n",g_nvod_pb_info.inner.dvbc_lock_info.nim_modulate);
  OS_PRINTF("g_nvod_pb_info.pg_info.s_id:%d\n",g_nvod_pb_info.pg_info.s_id);
  OS_PRINTF("g_nvod_pb_info.pg_info.is_scrambled:%d\n",g_nvod_pb_info.pg_info.is_scrambled);
  OS_PRINTF("g_nvod_pb_info.pg_info.tv_mode:%d\n",g_nvod_pb_info.pg_info.tv_mode);
  OS_PRINTF("g_nvod_pb_info.pg_info.v_pid:%d\n",g_nvod_pb_info.pg_info.v_pid);
  OS_PRINTF("g_nvod_pb_info.pg_info.pcr_pid:%d\n",g_nvod_pb_info.pg_info.pcr_pid);
  OS_PRINTF("g_nvod_pb_info.pg_info.a_pid:%d\n",g_nvod_pb_info.pg_info.a_pid);
  OS_PRINTF("g_nvod_pb_info.pg_info.audio_type:%d\n",g_nvod_pb_info.pg_info.audio_type);
  OS_PRINTF("g_nvod_pb_info.pg_info.audio_track:%d\n",g_nvod_pb_info.pg_info.audio_track);
  OS_PRINTF("g_nvod_pb_info.pg_info.audio_volume:%d\n",g_nvod_pb_info.pg_info.audio_volume);
  OS_PRINTF("g_nvod_pb_info.pg_info.pmt_pid:%d\n",g_nvod_pb_info.pg_info.pmt_pid);
  OS_PRINTF("g_nvod_pb_info.pg_info.video_type:%d\n",g_nvod_pb_info.pg_info.video_type);
  OS_PRINTF("g_nvod_pb_info.pg_info.aspect_ratio:%d\n",g_nvod_pb_info.pg_info.aspect_ratio);
  OS_PRINTF("g_nvod_pb_info.pg_info.reserved:%d\n",g_nvod_pb_info.pg_info.reserved);
  OS_PRINTF("g_nvod_pb_info.pg_info.context1:%d\n",g_nvod_pb_info.pg_info.context1);
  OS_PRINTF("g_nvod_pb_info.pg_info.context2:%d\n",g_nvod_pb_info.pg_info.context2);
  OS_PRINTF("==========nvod_play_info_print end=======\n");
}

static void nvod_do_play_pg(time_shifted_svc_t *p_ts_svc, dvbc_tp_info_t *p_tp)
{
  cmd_t cmd;
  av_set_t av_set = {0};
  audio_set_t sAudioSet = {0};

  sys_status_get_audio_set(&sAudioSet);
  
  g_nvod_pb_info.lock_mode = SYS_DVBC;
  g_nvod_pb_info.inner.dvbs_lock_info.tp_rcv.sym = p_tp->symbol_rate;
  g_nvod_pb_info.inner.dvbs_lock_info.tp_rcv.freq = p_tp->frequency;
  g_nvod_pb_info.inner.dvbs_lock_info.tp_rcv.nim_modulate = p_tp->modulation;

  sys_status_get_av_set(&av_set);
  
  {
  }

  g_nvod_pb_info.pg_info.s_id = (u16)p_ts_svc->svc_id;
  g_nvod_pb_info.pg_info.is_scrambled = p_ts_svc->is_scrambled;
  g_nvod_pb_info.pg_info.tv_mode = sys_status_get()->av_set.tv_mode;

  g_nvod_pb_info.pg_info.v_pid = (u16)p_ts_svc->video_pid;
  g_nvod_pb_info.pg_info.pcr_pid = (u16)p_ts_svc->pcr_pid;
  g_nvod_pb_info.pg_info.a_pid = (u16)p_ts_svc->audio_id;
  g_nvod_pb_info.pg_info.audio_type = (u16)p_ts_svc->a_type;
  g_nvod_pb_info.pg_info.audio_track = 0;//(u16)p_pg->audio_track;
  g_nvod_pb_info.pg_info.audio_volume = sAudioSet.global_volume;//16;
  g_nvod_pb_info.pg_info.pmt_pid = (u16)p_ts_svc->pmt_pid;

  g_nvod_pb_info.pg_info.aspect_ratio = sys_status_get_video_aspect(av_set.tv_ratio);

  nvod_play_info_print();

  ui_set_nvod_play_state(TRUE);

  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_nvod_pb_info;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

u16 ui_get_timeshift_svc_pgid(time_shifted_svc_t *p_ts_svc)
{
  u8 nvod_prog_view = 0;
  u16 i=0;
  u16 pg_id = INVALIDID;
  dvbs_prog_node_t nvod_prog = {0};
        
  nvod_prog_view = ui_dbase_create_view(DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG, 0, NULL);
  if(db_dvbs_get_count(nvod_prog_view)>0)
  {
    ui_dbase_set_pg_view_id(nvod_prog_view);
    for(i=0; i<db_dvbs_get_count(nvod_prog_view); i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(nvod_prog_view, i);
      db_dvbs_get_pg_by_id(pg_id, &nvod_prog);

      if(nvod_prog.s_id == p_ts_svc->svc_id
        && nvod_prog.ts_id == p_ts_svc->stream_id)
      {
        break;
      }
      else
      {
        pg_id = INVALIDID;
      }
    }
  }

  return pg_id;
}

void ui_play_nvod(time_shifted_svc_t *p_ts_svc, dvbc_tp_info_t *p_tp)
{
  if(p_ts_svc != NULL)
  {
    if(ui_is_nvod_playing())
    {
      ui_stop_play_nvod(STOP_PLAY_BLACK, TRUE);
    }
    nvod_do_play_pg(p_ts_svc, p_tp);
  }
}

BOOL ui_play_nvod_book(u16 pg_id)
{
  u8 nvod_prog_view = 0;
  BOOL bRet = FALSE; 
  nvod_prog_view = ui_dbase_create_view(DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG, 0, NULL);
  if(db_dvbs_get_count(nvod_prog_view)>0)
  {
    if(pg_id != INVALIDID)
    {
      bRet = TRUE;
    }
  }

  return bRet;
}

void ui_stop_play_nvod(stop_mode_t type, BOOL is_sync)
{
  cmd_t cmd;

  if(!ui_is_nvod_playing())
  {
    OS_PRINTF("ui_stop_play_nvod: alreay stoped\n");
    return;
  }
  
  ui_set_nvod_play_state(FALSE);
  
  cmd.id = is_sync ? PB_CMD_STOP_SYNC : PB_CMD_STOP;
  cmd.data1 = type;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

static BOOL ui_create_nvod_tp(void)
{
  u8 nvod_prog_view = 0;
  u16 i=0, j=0;
  u16 pg_id = INVALIDID;
  u16 timeshift_pg_count = 0;
  BOOL bRet = FALSE;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t nvod_prog = {0};
  dvbs_tp_node_t tp = {0};
        
  nvod_prog_view = ui_dbase_create_view(DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG, 0, NULL);
  timeshift_pg_count = db_dvbs_get_count(nvod_prog_view);
  OS_PRINTF("timeshift pg count %d\n", timeshift_pg_count);

  g_nvod_tp_num = 0;

  if(timeshift_pg_count>0)
  {
    memset(tp_info, 0, sizeof(tp_info));

    for(i=0; i<timeshift_pg_count; i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(nvod_prog_view, i);
      db_ret = db_dvbs_get_pg_by_id(pg_id, &nvod_prog);
      MT_ASSERT(DB_DVBS_OK == db_ret);
      db_ret = db_dvbs_get_tp_by_id((u16)nvod_prog.tp_id, &tp);
      MT_ASSERT(DB_DVBS_OK == db_ret);

      for(j=0; j<=g_nvod_tp_num; j++) //find the empty tp_info
      {
        if((tp_info[j].frequency == tp.freq) 
          && (tp_info[j].symbol_rate == tp.sym) 
          && (tp_info[j].modulation == tp.nim_modulate))
        {
          break;
        }
      }

      if((j>g_nvod_tp_num) && (g_nvod_tp_num<MAX_NVOD_TP))
      {
        tp_info[g_nvod_tp_num].frequency = tp.freq;
        tp_info[g_nvod_tp_num].symbol_rate = tp.sym;
        tp_info[g_nvod_tp_num].modulation = tp.nim_modulate;

        g_nvod_tp_num++;
      }
    }
  
    bRet = TRUE;
  }
  else
  {
    tp_info[g_nvod_tp_num].frequency = 337000;
    tp_info[g_nvod_tp_num].symbol_rate = 6900;
    tp_info[g_nvod_tp_num].modulation = 6;
    tp_info[g_nvod_tp_num].main_freq = FALSE;

    g_nvod_tp_num=1;
  }

  return bRet;
}

BOOL ui_nvod_enter(void)
{
  m_svc_t *p_this = class_get_handle_by_id(M_SVC_CLASS_ID);
  /**stop monitor svc***/
   if(NULL  != p_this)
    {
        p_this->stop(p_this);
    }
  ui_stop_play(STOP_PLAY_FREEZE, TRUE);
  ui_epg_stop();
  ui_epg_release();
  nvod_data_init();
  ui_nvod_init();
  ui_nvod_start();
  return TRUE;
}

BOOL ui_nvod_exit(void)
{
  ui_stop_play_nvod(STOP_PLAY_BLACK, TRUE);
  ui_nvod_stop();
  ui_nvod_release();
  //ui_set_transpond(&tp);
  nvod_data_deinit(class_get_handle_by_id(NVOD_CLASS_ID));
  ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  ui_epg_init();
  ui_epg_start();


  g_nvod_tp_num = 0;
  
  return TRUE;
}

BEGIN_AP_EVTMAP(ui_nvod_evtmap)
  CONVERT_EVENT(NVOD_SVC_SCAN_END,   MSG_NVOD_SVC_SCAN_END) 
  CONVERT_EVENT(NVOD_REF_SVC_FOUND,   MSG_NVOD_REF_SVC_FOUND) 
  CONVERT_EVENT(NVOD_REF_EVT_FOUND,   MSG_NVOD_REF_EVT_FOUND)
  CONVERT_EVENT(NVOD_SHIFT_EVT_FOUND, MSG_NVOD_SHIFT_EVT_FOUND)
END_AP_EVTMAP(ui_nvod_evtmap)


