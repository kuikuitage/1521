/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"
#include "ui_pause.h"
#include "ui_mute.h"
#include "ui_prog_bar.h"
#include "ap_framework.h"
#include "ui_conditional_entitle_expire.h"
#if((CONFIG_CAS_ID != CONFIG_CAS_ID_XSM)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_QL))
#include "ui_ca_finger.h"
#else
#include "ui_finger_print.h"
#endif

#include "ui_email_picture.h"
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif
typedef struct
{
  u16  group;
  u8  mode;
  u16 prog_id;
  u8  is_scramble;
} play_info_t;

static play_info_t g_play_info = { 0, CURN_MODE_NONE, INVALIDID, FALSE };
static play_info_t g_last_info = { 0, CURN_MODE_NONE, INVALIDID, FALSE };

static u8 g_play_mode = CURN_MODE_NONE;

static BOOL g_is_playing = FALSE;
static BOOL g_is_blkscr = TRUE;

struct
{
  BOOL is_chkpwd;
  BOOL is_passchk;
  u16  passchk_pg;
} g_chkpwd_info = { TRUE, FALSE, INVALIDID };

u16 ui_playback_evtmap(u32 event);

u16 pwdlg_keymap_in_scrn(u16 key);

u16 pwdlg_keymap_in_list(u16 key);

extern void set_full_scr_ad_status(BOOL is_show);

play_param_t g_pb_info = {0};


RET_CODE pwdlg_proc_in_scrn(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

RET_CODE pwdlg_proc_in_list(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);


void ui_enable_playback(BOOL is_enable)
{
  cmd_t cmd;

  cmd.data1 = APP_PLAYBACK;
  cmd.data2 = 0;
 // static int count = 0;
  
  if (is_enable)
  {
    cmd.id = AP_FRM_CMD_ACTIVATE_APP;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);
	
    fw_register_ap_evtmap(APP_PLAYBACK, ui_playback_evtmap);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);

    // enable ca
#ifndef WIN32
#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_DS)
    ui_init_ca();
#endif
#endif
  }
  else
  {
    
    cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_unregister_ap_evtmap(APP_PLAYBACK);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);

    // STOP_CAS
#ifndef WIN32
#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_DS)
     ui_release_ca();
#else
    cmd.id = CAS_CMD_STOP_CAS;
    ap_frm_do_command(APP_CA, &cmd); 
#endif
#endif
  }

  OS_PRINTF("UI: %s playback.........\n", is_enable ? "active" : "deactive");
}


void ui_pause_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_PAUSE;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_resume_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESUME;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_stop_play(stop_mode_t type, BOOL is_sync)
{
  cmd_t cmd;
#ifndef WIN32
  cmd.id = CAS_CMD_STOP_SYNC;
  ap_frm_do_command(APP_CA, &cmd);
#endif
  cmd.id = is_sync ? PB_CMD_STOP_SYNC : PB_CMD_STOP;
  cmd.data1 = type;
  cmd.data2 = 0;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
  // set flag
  g_is_playing = FALSE;
  g_is_blkscr = (BOOL)(type == STOP_PLAY_BLACK);

}

void ui_set_audio_channel(u8 channel)
{
  cmd_t cmd;
  u16 pg_id;
  dvbs_prog_node_t pg;

  pg_id = sys_status_get_curn_group_curn_prog_id();
  MT_ASSERT(pg_id != INVALIDID);
  
  db_dvbs_get_pg_by_id(pg_id, &pg);

  cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;
  cmd.data1 = pg.audio[channel].p_id;
  cmd.data2 = pg.audio[channel].type;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

BOOL ui_is_playing(void)
{
  return g_is_playing;
}


BOOL ui_is_blkscr(void)
{
  return g_is_blkscr;
}

static void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  p_info->ecm_num = (pg.ecm_num < 8 ? (u8)pg.ecm_num : 8);
  p_info->emm_num = (tp.emm_num < 8 ? (u8)tp.emm_num : 8);
  p_info->org_network_id = pg.orig_net_id;
  p_info->ts_id= pg.ts_id;
  memcpy(&p_info->ecm_info, &pg.cas_ecm, p_info->ecm_num * sizeof(cas_desc_t));
  
#if (AP_CAS_ID == CAS_ID_DS)
  if(p_info->ecm_num == 0)
  {
    p_info->ecm_num = 1;
    p_info->ecm_info[0].ecm_pid= 0x1FFF;
  }
#endif

  memcpy(&p_info->emm_info, &tp.emm_info, p_info->emm_num * sizeof(ca_desc_t));
}

static void play_pg_with_pwd(u32 prog_id)
{	
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_LOCKED_INPUT,
    1,
    NULL,
    NULL
  };

  u8 root_id = fw_get_focus_id();
  update_ca_message(RSC_INVALID_ID);
  ui_reset_ca_last_msg();
  
  if(ui_is_email())
  {
  	update_email_picture(EMAIL_PIC_SHOW);
  }
  
  if(ui_is_finger_show() && ui_is_fullscreen_menu(fw_get_focus_id()))
  {
	open_finger(0, 0);
  }

  if(root_id == ROOT_ID_NUM_PLAY)
  {
    manage_close_menu(ROOT_ID_NUM_PLAY, 0, 0);
  }
  else if(root_id == SN_ROOT_ID_SUBMENU || root_id == ROOT_ID_CHANNEL_EDIT ||
    root_id == SN_ROOT_ID_MAINMENU)
  {
    return;
  }
  
  if (sys_status_get_curn_group_curn_prog_id() == prog_id)
  {
    if (root_id == ROOT_ID_PROG_LIST
        || root_id == ROOT_ID_EPG)
    {
      pwdlg_data.keymap = pwdlg_keymap_in_list;
      pwdlg_data.proc = pwdlg_proc_in_list;
    }
    else
    {
      pwdlg_data.keymap = pwdlg_keymap_in_scrn;
      pwdlg_data.proc = pwdlg_proc_in_scrn;
    }
    ui_comm_pwdlg_open(&pwdlg_data);
	if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL)
    {
		ctrl_paint_ctrl(fw_find_root_by_id(root_id),TRUE);
	}
  }
}

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
static void do_play_pg_just_set_ca_info(u16 pg_id)
{
  cas_sid_t ca_sid;


  UI_PRINTF("DO_PLAY_PG:  id =%d \n", pg_id);
  load_play_paramter_by_pgid(&g_pb_info, pg_id);
    
  if(1)//g_pb_info.pg_info.is_scrambled)
  {
    memset(&ca_sid, 0, sizeof(cas_sid_t));   
    OS_PRINTF("send cmd to ap cas \n");

    ca_sid.num = 1;
    ca_sid.pgid = pg_id;
    ca_sid.pg_sid_list[0] = g_pb_info.pg_info.s_id;
    ca_sid.tp_freq = g_pb_info.inner.dvbs_lock_info.tp_rcv.freq;
    ca_sid.a_pid = g_pb_info.pg_info.a_pid;
    ca_sid.v_pid = g_pb_info.pg_info.v_pid;
    ca_sid.pmt_pid = g_pb_info.pg_info.pmt_pid;
    ca_sid.ca_sys_id = 0x1FFF;
	ca_sid.ecm_num = 1;
    ca_sid.ecm_info[0].ecm_pid= 0x1FFF;
    #ifndef WIN32
    ui_ca_set_sid(&ca_sid);
    #endif
  }  
}
#endif

static void do_play_pg(u16 pg_id)
{
  cmd_t cmd;
  language_set_t lang_set;
  cas_sid_t ca_sid;

  if (ui_is_pause())
  {
    UI_PRINTF("DO_PLAY_PG: ui_set_pause(FALSE)\n");
    ui_set_pause(FALSE);
  }


  UI_PRINTF("DO_PLAY_PG:  id =%d \n", pg_id);
  load_play_paramter_by_pgid(&g_pb_info, pg_id);
    
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
  if(TRUE == ui_get_ads_osd_status())
  {
    ui_ads_osd_roll_stop();
  }
  if(FALSE == get_force_channel_status())
  {
    ui_adv_get_osd_msg(pg_id);
  }
  else
  {
    UI_PRINTF("[play] enter force channel \n");
  }
#endif
  set_full_scr_ad_status(TRUE);
  
 // to play
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_pb_info;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  // set ca sid
  if(1)//g_pb_info.pg_info.is_scrambled)
  {
    memset(&ca_sid, 0, sizeof(cas_sid_t));   
    OS_PRINTF("send cmd to ap cas \n");

    ca_sid.num = 1;
    ca_sid.pgid = pg_id;
    ca_sid.pg_sid_list[0] = g_pb_info.pg_info.s_id;
    ca_sid.tp_freq = g_pb_info.inner.dvbs_lock_info.tp_rcv.freq;
    ca_sid.a_pid = g_pb_info.pg_info.a_pid;
    ca_sid.v_pid = g_pb_info.pg_info.v_pid;
    ca_sid.pmt_pid = g_pb_info.pg_info.pmt_pid;
    ca_sid.ca_sys_id = 0x1FFF;
    load_desc_paramter_by_pgid(&ca_sid, pg_id);
    #ifndef WIN32
    ui_ca_set_sid(&ca_sid);
    #endif
  }

  sys_status_get_lang_set(&lang_set);
  
  //start monitor service
  {
    m_svc_cmd_p_t param = {0};
    param.s_id = g_pb_info.pg_info.s_id;
    param.pmt_pid = g_pb_info.pg_info.pmt_pid;

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_RESET_CMD, &param);
  }
}


void ui_reset_tvmode(u16 av_mode)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESET_TVMODE;
  cmd.data1 = av_mode;
  cmd.data2 = 0;

  g_pb_info.pg_info.tv_mode = av_mode;

  ap_frm_do_command(APP_PLAYBACK, &cmd);  
}

void ui_reset_video_aspect_mode(u32 aspect)
{
  cmd_t cmd;
  cmd.id = PB_CMD_RESET_VIDEO_ASPECT_MODE;
  cmd.data1 = aspect;
  cmd.data2 = 0;
  g_pb_info.pg_info.aspect_ratio = aspect;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

void do_lock_pg(u16 pg_id)
{
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;

  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  if (db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  ui_set_transpond(&tp);
}


static BOOL do_chk_pg(u16 prog_id)
{
  u8 view_id;
  u16 pos;
  BOOL is_ss_chk;

  view_id = ui_dbase_get_pg_view_id();
  pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  if (ui_is_chk_pwd())
  {
    sys_status_get_status(BS_PROG_LOCK, &is_ss_chk);
    if (is_ss_chk)
    {
      if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0))
      {
        if (!ui_is_pass_chkpwd(prog_id))
        {
          if ((prog_id != g_chkpwd_info.passchk_pg) || (FALSE == ui_is_playing()))
          {
            ui_set_chk_pwd(prog_id, FALSE);
            play_pg_with_pwd(prog_id);
            // TODO: fw_post_msg(MSG_CHK_PWD, prog_id, 0);
          }
          return FALSE;
        }
      }
    }
  }

  // reset flag
  ui_set_chk_pwd(prog_id, TRUE);

  return TRUE;
}

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp)
{
  p_tp_rcv->sym = p_tp->sym;
  p_tp_rcv->freq = p_tp->freq;
  p_tp_rcv->nim_modulate = p_tp->nim_modulate;
}

void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg)
{
  av_set_t av_set = {0};
  u32 new_audio_ch = 0;
  u16 audio_pid = 0;

  u16 audio_type = AUDIO_MP2;
  customer_cfg_t customer_cfg = {0};

  sys_status_get_av_set(&av_set);
  #if 0 /*******/
  new_audio_ch = get_audio_channel(p_pg);
  
  if(p_pg->audio_channel != new_audio_ch)
  {
    p_pg->audio_channel = new_audio_ch;
    db_dvbs_edit_program(p_pg);
  }
  #endif
  audio_pid = (u16)p_pg->audio[new_audio_ch].p_id;
  audio_type = (u16)p_pg->audio[new_audio_ch].type;

  p_pg_info->s_id = (u16)p_pg->s_id;
  p_pg_info->is_scrambled = p_pg->is_scrambled;
  p_pg_info->tv_mode = sys_status_get()->av_set.tv_mode;

  p_pg_info->v_pid = (u16)p_pg->video_pid;
  p_pg_info->pcr_pid = (u16)p_pg->pcr_pid;
  p_pg_info->a_pid = audio_pid;
  p_pg_info->audio_type = audio_type;
  p_pg_info->audio_track = (u16)p_pg->audio_track;  
  
  get_customer_config(&customer_cfg);
  if((customer_cfg.customer== CUSTOMER_TAIHUI) || (customer_cfg.customer== CUSTOMER_FANTONG_XSMAJX))
  {
    p_pg_info->audio_volume = (u16)get_audio_global_volume();
    p_pg->volume = (u32)p_pg_info->audio_volume;
  }
  else
  {
    p_pg_info->audio_volume = (u16)p_pg->volume;
  }
  p_pg_info->pmt_pid = (u16)p_pg->pmt_pid;

  p_pg_info->aspect_ratio = sys_status_get_video_aspect(av_set.tv_ratio);
  p_pg_info->context1 = p_pg->id;
  OS_PRINTF("--------------------------------------\n");
  OS_PRINTF("TEST for force channel information: ts_id : %d\n",p_pg->ts_id);
  OS_PRINTF("TEST for force channel information: orig_net_id : %d\n",p_pg->orig_net_id);
  OS_PRINTF("TEST for force channel information: s_id : %d\n",p_pg->s_id);
  OS_PRINTF("TEST for force channel information: id : %d\n",p_pg->id);
  if(audio_type != AUDIO_MP2)//Jazz only support AUDIO_MP2
  {
    p_pg_info->a_pid = 0x1FFF;//Invalide PID
    OS_PRINTF("audio_type = %d, not support!\n", audio_type);
  }
  OS_PRINTF("trans_pg_info      --------------p_pg_info->audio_volume: %d\n",p_pg_info->audio_volume);
}

void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  memset(p_info, 0, sizeof(play_param_t));
  
  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  
  p_info->lock_mode = SYS_DVBC;
  trans_tp_info(&p_info->inner.dvbs_lock_info.tp_rcv, &tp);
  trans_pg_info(&p_info->pg_info, &pg);
}

//recall
BOOL ui_recall(BOOL is_play, u16 *p_curn)
{
  u16 view_type;
  u32 context;
  dvbs_prog_node_t last_pg;

  // init
  *p_curn = g_play_info.prog_id;
  if (db_dvbs_get_pg_by_id(g_last_info.prog_id, &last_pg) == DB_DVBS_FAILED
    || last_pg.skp_flag == 1)
  {
    return FALSE;
  }

  // recreate view for recall
  if ((g_last_info.group != g_play_info.group)
    || (g_last_info.mode != g_play_info.mode))
  {
    sys_status_set_curn_prog_mode(g_last_info.mode);
    sys_status_set_curn_group(g_last_info.group);
    sys_status_save();
    
    sys_status_get_curn_view_info(&view_type, &context);
    ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, context, NULL));
  }

  *p_curn = g_last_info.prog_id;
  if (is_play)
  {
    return ui_play_prog(*p_curn, FALSE);
  }
  return TRUE;
}


//switch between tv and radio
u16 ui_tvradio_switch(BOOL is_play, u16 *p_curn)
{
  u16 org_group, pos_in_set;
  u8 curn_mode, org_mode, group_type;
  u8 view_id;
  u16 pg_id, pg_pos;
  u32 context;
  u16 view_type,view_count;

  // init
  *p_curn = g_play_info.prog_id;
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  switch (org_mode)
  {
    case CURN_MODE_TV:
      curn_mode = CURN_MODE_RADIO;
      break;
    case CURN_MODE_RADIO:
      curn_mode = CURN_MODE_TV;
      break;
    default:
      return IDS_NO_PROG;
  }

  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);
  switch (group_type)
  {
    case GROUP_T_ALL:
      if (!sys_status_get_group_all_info(curn_mode, &pg_id, &pg_pos))
      {
        pg_id = INVALIDID, pg_pos = INVALIDPOS;
      }
      break;
    case GROUP_T_FAV:
      if (!sys_status_get_fav_group_info(pos_in_set, curn_mode, &pg_id, &pg_pos))
      {
        pg_id = INVALIDID, pg_pos = INVALIDPOS;
      }
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }    

  if (pg_id == INVALIDID || pg_pos == INVALIDPOS)
  {
    if(group_type == GROUP_T_ALL)
    {
      if(curn_mode == CURN_MODE_TV)
      {
        return IDS_NO_TV_PROG;
      }
      else
      {
        return IDS_NO_RADIO_PROG;
      }
    }
    else
    {
      if(curn_mode == CURN_MODE_TV)
      {
        return IDS_NO_GROUP_TV;
      }
      else
      {
        return IDS_NO_GROUP_RADIO;
      }
    }
  }

  sys_status_set_curn_prog_mode(curn_mode);
  sys_status_save();
  sys_status_get_curn_view_info(&view_type, &context);
  view_id = ui_dbase_create_view(view_type, context, NULL);
  view_count = db_dvbs_get_count(view_id);
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE || (view_count == 0))
  {
    if(curn_mode == CURN_MODE_TV)
    {
      curn_mode = CURN_MODE_RADIO;
      sys_status_set_curn_prog_mode(curn_mode);
      sys_status_save();
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, TRUE);
    }
    if(curn_mode == CURN_MODE_RADIO)
    {
      curn_mode = CURN_MODE_TV;
      sys_status_set_curn_prog_mode(curn_mode);
      sys_status_save();
      ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
    }
    return IDS_NO_PROG;
  }

  ui_dbase_set_pg_view_id(view_id);
  //close pause menu.
  ui_set_pause(FALSE);
  
  *p_curn = pg_id;
  if (is_play)
  {
    ui_play_prog(*p_curn, FALSE);
  }
  
  return RSC_INVALID_ID;
}

u16 ui_find_prog_pos_in_cur_group(dvbs_prog_node_t *p_prog)
{
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 count = db_dvbs_get_count(view_id);
  u16 i, pg_id, pg_new_pos = INVALIDPOS;
  dvbs_prog_node_t prog = {0};

  if(count > 0)
  {
    for(i=0; i<db_dvbs_get_count(view_id); i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);

      if(p_prog->s_id == prog.s_id)
      {
        pg_new_pos = i;
        break;
      }
    }
#if 0
    if(i == db_dvbs_get_count(view_id))
    {
      pg_new_pos = 0;
    }
    else
    {
      pg_new_pos = i;
    }
#endif	
  }

  return pg_new_pos;
}

BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play)
{
  u16 org_group, pos_in_set;
  u8 curn_mode, org_mode, group_type;
  u8 view_id;
  u16 pg_id = INVALIDID;
  u16 pg_pos = INVALIDPOS;
  u32 context;
  BOOL ret = FALSE;
  dvbs_prog_node_t prog = {0};

  // init
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  if(org_group != GROUP_T_ALL)
  {
    MT_ASSERT(0);
  }

  switch(view_type)
  {
    case DB_DVBS_ALL_TV:
      curn_mode = CURN_MODE_TV;
      if(org_mode == CURN_MODE_TV)
      {
        //return TRUE;
      }
      break;

    case DB_DVBS_ALL_RADIO:
      curn_mode = CURN_MODE_RADIO;
      if(org_mode == CURN_MODE_RADIO)
      {
        //return TRUE;
      }
      break;

    case DB_DVBS_ALL_PG:
    case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
      curn_mode = org_mode;
      break;

    case DB_DVBS_FAV_ALL:
      curn_mode = org_mode;
      break;

    default:
      curn_mode = org_mode;
      break;
  }
  
  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);
  
  switch (group_type)
  {
    case GROUP_T_ALL:
      if (!sys_status_get_group_all_info(curn_mode, &pg_id, &pg_pos))
      {
        pg_id = INVALIDID, pg_pos = INVALIDPOS;
      }
      break;

    default:
      MT_ASSERT(0);
      return FALSE;
  }

  if (pg_id == INVALIDID || pg_pos == INVALIDPOS)
  {
    view_id = ui_dbase_create_view(view_type, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);

    if(db_dvbs_get_count(view_id)>0)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
      pg_pos = 0;
      
      db_dvbs_get_pg_by_id(pg_id, &prog);
      
      if((BOOL)prog.tv_flag)
      {
        curn_mode = CURN_MODE_TV;
      }
      else
      {
        curn_mode = CURN_MODE_RADIO;
      }

      sys_status_set_curn_prog_mode(curn_mode);
      sys_status_set_curn_group_info(pg_id, pg_pos);
      //sys_status_save();
    
      if(is_play)
      {
        ui_play_prog(pg_id, FALSE);
      }

      ret = TRUE;
    }
    else
    {
      return ret;
    }
  }
  else
  {
    view_id = ui_dbase_create_view(view_type, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);

    //set sys info
    pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
    
    if(pg_id == INVALIDID || pg_pos == INVALIDPOS) 
    {
      if(db_dvbs_get_count(view_id)>0)
      {
        pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
        pg_pos = 0;
        
        db_dvbs_get_pg_by_id(pg_id, &prog);
        
        if((BOOL)prog.tv_flag)
        {
          curn_mode = CURN_MODE_TV;
        }
        else
        {
          curn_mode = CURN_MODE_RADIO;
        }
      }
      else
      {
        return ret;
      }
    }
    
    sys_status_set_curn_prog_mode(curn_mode);
    sys_status_set_curn_group_info(pg_id, pg_pos);
    //sys_status_save();
    view_id = ui_dbase_create_view(view_type, context, NULL);
    ui_dbase_set_pg_view_id(view_id);
  
    if(is_play)
    {
      ui_play_prog(pg_id, is_play);
    }

    ret = TRUE;
  }  
  
  return ret;
}

BOOL ui_reset_curn_mode()
{
  u8 curn_mode;
  u8 view_id;
  u16 pg_id;
  BOOL ret = FALSE;
  u16 view_count = 0;
  dvbs_prog_node_t prog = {0};

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  
  if(view_count>0)
  {
    pg_id = sys_status_get_curn_group_curn_prog_id();
    db_dvbs_get_pg_by_id(pg_id, &prog);

    if((BOOL)prog.tv_flag)
    {
      curn_mode = CURN_MODE_TV;
    }
    else
    {
      curn_mode = CURN_MODE_RADIO;
    }

    sys_status_set_curn_prog_mode(curn_mode);
    sys_status_save();
  }
    
  return ret;
}


void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn)
{
  play_set_t play_set;
  dvbs_prog_node_t pg;
  u16 prog_id, prev_prog, total_prog;
  u16 curn_prog;
  s32 dividend = 0;
  u8 curn_view = ui_dbase_get_pg_view_id();
  BOOL is_force_skip = FALSE;

  // init
  *p_curn = g_play_info.prog_id;
	

  sys_status_get_play_set(&play_set);

  total_prog = db_dvbs_get_count(curn_view);
  prog_id = sys_status_get_curn_group_curn_prog_id();
  curn_prog = prev_prog = db_dvbs_get_view_pos_by_id(
                curn_view, prog_id);

  if(total_prog <= 0)
  {
    return;
  }
  do
  {
    dividend = curn_prog + offset;
    while (dividend < 0)
    {
      dividend += total_prog;
    }
    curn_prog = (u16)(dividend) % total_prog;
    /* pos 2 id */
    prog_id = db_dvbs_get_id_by_view_pos(curn_view, curn_prog);
    MT_ASSERT(prog_id != INVALIDID);
    if (prev_prog == curn_prog) /* all is skip*/
    {
      return;
    }

    // check play type
    db_dvbs_get_pg_by_id(prog_id, &pg);
    OS_PRINTF("pg is scramble = %d, play type = %d\n", 
                pg.is_scrambled, play_set.type);
    
    switch(play_set.type)
    {
      case 1: // only play free pg
      is_force_skip = (BOOL)(pg.is_scrambled == 1);
      break;

      case 2: // only play scramble pg
      is_force_skip = (BOOL)(pg.is_scrambled == 0);
      break;

      default:
        ;
    }
  } while (is_force_skip || db_dvbs_get_mark_status(curn_view, curn_prog,
                                   DB_DVBS_MARK_SKP, FALSE));

  *p_curn = prog_id;
  if (is_play)
  {
    ui_play_prog(*p_curn, FALSE);
  }
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
  if(ui_is_finger_show())
  {
    close_ca_finger_menu();
  }
  #endif
}


void ui_set_frontpanel_by_curn_pg(void)
{
  u16 pos;
  #ifdef LCN_SWITCH
  dvbs_prog_node_t pg ={0};
  #endif
  if (g_play_info.prog_id != INVALIDID)
  {
    pos = db_dvbs_get_view_pos_by_id(
      ui_dbase_get_pg_view_id(), g_play_info.prog_id);
    #ifdef LCN_SWITCH
     db_dvbs_get_pg_by_id( g_play_info.prog_id, &pg);
     ui_set_front_panel_by_num(pg.logic_ch_num);
    #else
    ui_set_front_panel_by_num(pos + 1);
    #endif
  }
  else
  {
    ui_set_front_panel_by_str("----");
  }
}

static RET_CODE on_pwdlg_correct_in_list(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  /* if get correct pwd then don't check again in list
     and enable it when leave list */
  ui_enable_chk_pwd(FALSE);
  g_chkpwd_info.is_passchk = TRUE;
  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);

  ui_comm_pwdlg_close();

  update_signal();
  
  return SUCCESS;
}

static RET_CODE on_pwdlg_correct_in_scrn(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  g_chkpwd_info.is_passchk = TRUE;
  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);

  ui_comm_pwdlg_close();

  update_signal();
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  return SUCCESS;
}

static RET_CODE on_pwdlg_save(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
  do_lock_pg(g_chkpwd_info.passchk_pg);

  update_signal();

  return SUCCESS;
}

static void pass_key_to_parent(u16 msg)
{
  u16 key;
    
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    case MSG_FOCUS_LEFT:
      key = V_KEY_LEFT;
      break;
    case MSG_FOCUS_RIGHT:
      key = V_KEY_RIGHT;
      break;
    case MSG_PAGE_UP:
      key = V_KEY_PAGE_UP;
      break;
    case MSG_PAGE_DOWN:
      key = V_KEY_PAGE_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);
}

static RET_CODE on_pwdlg_cancel_in_list(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
  pass_key_to_parent(msg);
  ui_comm_pwdlg_close();

  return SUCCESS;
}

static RET_CODE on_pwdlg_cancel_in_scrn(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
  if(fw_find_root_by_id(ROOT_ID_FAV_LIST) == NULL) 
  {
    switch(msg)
    {
      case MSG_FOCUS_UP:
        ui_shift_prog(1, TRUE, &prog_id);
        break;
      case MSG_FOCUS_DOWN:
        ui_shift_prog(-1, TRUE, &prog_id);
        break;
      case MSG_PAGE_UP:
        ui_shift_prog(2, TRUE, &prog_id);
        break;
      case MSG_PAGE_DOWN:
        ui_shift_prog(2, TRUE, &prog_id);
        break;
     case MSG_BLACK_SCREEN: 
        ui_recall(FALSE, &prog_id);
        ui_play_prog(prog_id, FALSE);
        break;
      default:
        break;
    }
    fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, prog_id);
  }
  else
  {
	  pass_key_to_parent(msg);
	  ui_comm_pwdlg_close();
  }
  return SUCCESS;
}

//play current channel in current mode
void ui_play_reset_pmt_pid(void)
{
	{
		m_svc_cmd_p_t param = {0};
		param.s_id = g_pb_info.pg_info.s_id;
		param.pmt_pid = g_pb_info.pg_info.pmt_pid;
	
		dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_RESET_CMD, &param);
	  }

}

//play current channel in current mode
BOOL ui_play_curn_pg(void)
{
  u16 curn_group;
  u16 prog_id;
  u16 prog_pos;
  u8 curn_mode;
  u32 context;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if (curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if (curn_mode == CURN_MODE_NONE)
    {
      ui_set_front_panel_by_str("----");
      return FALSE;
    }
  }
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &prog_pos,
                                    &context);

  ui_play_prog(prog_id, FALSE);
  return TRUE;
}


BOOL ui_play_prog(u16 prog_id, BOOL is_force)
{
  dvbs_prog_node_t pg;
  BOOL is_switch = FALSE;
  u8 curn_mode;
  u16 prog_pos;
  play_set_t play_set;
  customer_cfg_t g_cus_cfg;
  BOOL is_playing = ui_is_playing();

  if (prog_id == INVALIDID)
  {
    UI_PRINTF("AP PLAY: INVALID pg = 0x%.4x\n", prog_id);
    return FALSE;
  }
  get_customer_config(&g_cus_cfg);
  // check for play
  if (g_play_info.prog_id == prog_id
      && !is_force
      && ui_is_playing()
      && ui_is_pass_chkpwd(prog_id))
  {
    UI_PRINTF("AP PLAY: same pg = 0x%.4x\n", prog_id);
    prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), 
                                          prog_id);
    // set frontpanel 
    #ifdef LCN_SWITCH
    db_dvbs_get_pg_by_id(prog_id, &pg);
    ui_set_front_panel_by_num(pg.logic_ch_num);
    #else
    ui_set_front_panel_by_num(prog_pos + 1);
    #endif
    return FALSE;
  }

  if(g_play_info.prog_id != prog_id)
  {
    clean_ca_prompt();
  }
  
  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("AP PLAY: can not find pg = 0x%.4x\n", prog_id);
    return FALSE;
  }

  // set sys info
  if((BOOL)pg.tv_flag)
  {
    curn_mode = CURN_MODE_TV;
    ui_set_channel_mode(TRUE);
  }
  else
  {
    curn_mode = CURN_MODE_RADIO;
    ui_set_channel_mode(FALSE);
  }
  
  if (curn_mode != g_play_mode)
  {
    g_play_mode = curn_mode;
    is_switch = TRUE;
  }

  // just stop before play
  sys_status_get_play_set(&play_set);
  if (is_playing)
  {
    if(is_switch)
    {
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
    else
    {
      if(curn_mode == CURN_MODE_RADIO)
      {
		ui_stop_play(play_set.mode ? STOP_PLAY_FREEZE : STOP_PLAY_BLACK, FALSE);
      }
      else
      {
        ui_stop_play(play_set.mode ? STOP_PLAY_FREEZE : STOP_PLAY_BLACK, FALSE);
      }
    }
  }
  else
  {
	#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
    static BOOL is_first_poweron = TRUE;
	#else
	static BOOL is_first_poweron = FALSE;
	#endif
    if(TRUE == is_first_poweron)
  	{
  		is_first_poweron = FALSE;//if first poweron noting to do		
  	}
  	else
  	{
  		ui_stop_play(STOP_PLAY_BLACK, TRUE);
  	}
  }
  
  if (curn_mode == CURN_MODE_RADIO)
  {
    OS_PRINTF("is_switch = %d, is_playing = %d, is_blkscr = %d\n",
      is_switch, g_is_playing, g_is_blkscr);
    if (is_switch || !is_playing)
    {
      // show logo again
      ui_show_logo(LOGO_BLOCK_ID_M1);
      mtos_task_delay_ms(50);
    }

  }
      
  // set play info
  if (g_play_info.prog_id != prog_id)
  {
    memcpy(&g_last_info, &g_play_info, sizeof(play_info_t));
    // update scrambe info
    g_play_info.is_scramble = (u8)pg.is_scrambled;
  }

  // set play info
  g_play_info.group = sys_status_get_curn_group();
  g_play_info.mode = curn_mode;
  g_play_info.prog_id = prog_id;

  //set sys info
  prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), 
                                          g_play_info.prog_id);
  sys_status_set_curn_prog_mode(curn_mode);
  sys_status_set_curn_group_info(g_play_info.prog_id, prog_pos);
  sys_status_save();

  // set frontpanel
  ui_set_frontpanel_by_curn_pg();
 
  // try to close pwd dlg
  ui_comm_pwdlg_close();
  
  //close eneitle info 
  if(fw_find_root_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG) != NULL)
  {
    UI_PRINTF("UI:  force close ROOT_ID_CA_ENTITLE_EXP_DLG ! \n");
    close_ca_entitle_expire_dlg();
  }

  //CONFIG_CAS_ID_DS set ca sid
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
  do_play_pg_just_set_ca_info(g_play_info.prog_id); 
#endif
  
  // check pwd for lock pg
  if (do_chk_pg(g_play_info.prog_id))
  {
    //to play
    UI_PRINTF("AP PLAY: 0x%.4x\n", g_play_info.prog_id);
    do_play_pg(g_play_info.prog_id);
    if(!g_play_info.is_scramble)
    {
      if(CONFIG_CAS_ID != CONFIG_CAS_ID_DS)
      {
        update_ca_message(RSC_INVALID_ID);
	  	ui_reset_ca_last_msg();
      }
    }
	else
	{
		#if CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG
		ui_set_ca_last_msg_valid(1);
		#endif
	}
  }
  else
  {
    if(!g_play_info.is_scramble)
    {
      if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
        update_ca_message(RSC_INVALID_ID);
	    ui_reset_ca_last_msg();
        OS_PRINTF("CLEAR MSG FOR SV!!!\n");
    }
	else
	{
	    #if CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG
		ui_set_ca_last_msg_valid(1);
		#endif
	}
  }  
  // set flag
  g_is_playing = TRUE;
 
  return TRUE;
}

void ui_clear_play_history(void)
{
  g_last_info.group = 0;
  g_last_info.mode = CURN_MODE_NONE;
  g_last_info.prog_id = INVALIDID;

  g_chkpwd_info.is_chkpwd = TRUE;
  g_chkpwd_info.is_passchk = FALSE;
  g_chkpwd_info.passchk_pg = INVALIDID;
}


BOOL ui_is_pass_chkpwd(u16 prog_id)
{
  if (prog_id == g_chkpwd_info.passchk_pg)
  {
    return g_chkpwd_info.is_passchk;
  }

  return FALSE;
}


void ui_enable_chk_pwd(BOOL is_enable)
{
  //g_chkpwd_info.is_chkpwd = is_enable;
}


BOOL ui_is_chk_pwd(void)
{
  return g_chkpwd_info.is_chkpwd;
}


void ui_set_chk_pwd(u16 prog_id, BOOL is_pass)
{
  g_chkpwd_info.is_passchk = is_pass;
  g_chkpwd_info.passchk_pg = prog_id;
}

BOOL ui_is_tvradio_switch(u16 pgid)
{
  dvbs_prog_node_t pg;
  u8 curn_mode = sys_status_get_curn_prog_mode();

  db_dvbs_get_pg_by_id(pgid, &pg);

  if(((curn_mode == CURN_MODE_TV) && (pg.video_pid == 0))
    ||((curn_mode == CURN_MODE_RADIO)  && (pg.video_pid != 0)))
  {
    return TRUE;
  }

  return FALSE;
}

BOOL ui_is_playpg_scrambled(void)
{
  return g_play_info.is_scramble;
}

BOOL ui_get_curpg_scramble_flag(void)
{
  dvbs_prog_node_t pg;
  if(g_play_info.prog_id != INVALIDID)
  {
    db_dvbs_get_pg_by_id(g_play_info.prog_id, &pg);
    return pg.is_scrambled;
  }
  return FALSE;
}

void ui_set_playpg_scrambled(BOOL is_scramble)
{
  if (g_play_info.prog_id != INVALIDID)
  {
    g_play_info.is_scramble = is_scramble;
  }
}

void ui_book_play(u16 pgid)
{
  u16 prog_id;
  u8 mode, view_id;
  u16 ret;

  if(sys_status_get_curn_group_curn_prog_id() != pgid)
  {
    //need tv/radio switch?
    if(ui_is_tvradio_switch(pgid))
    {
      ret = ui_tvradio_switch(FALSE, &prog_id);
      MT_ASSERT(ret == RSC_INVALID_ID);
    }

    //need change group? if current group isn't GROUP_ALL, change group.
    if(sys_status_get_curn_group() != 0)
    {
      //set the group as group all.
      sys_status_set_curn_group(0);

      //create new view.
      mode = sys_status_get_curn_prog_mode();
      view_id = ui_dbase_create_view(
        (mode == CURN_MODE_TV)?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO, 0, NULL);

      ui_dbase_set_pg_view_id(view_id);      
    }
  }

  ui_play_prog(pgid, FALSE);
}

void ui_play_api_factory()
{
  OS_PRINTF("curn pg is NULL!!!\n");
  g_play_info.group = 0;
  g_play_info.is_scramble = FALSE;
  g_play_info.mode = CURN_MODE_NONE;
  g_play_info.prog_id = INVALIDID;
}

static RET_CODE exit_to_mainmenu(control_t *p_ctrl, u16 msg, u32 para1, u32 paa2)
{
	ui_pic_stop();
	manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	manage_close_menu(ROOT_ID_PASSWORD, 0, 0);
	ui_return_to_mainmenu();

	return SUCCESS;
}


BEGIN_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_BLACK_SCREEN, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_scrn)
  ON_COMMAND(MSG_PRESET_PROG, on_pwdlg_correct_in_scrn)
  ON_COMMAND(MSG_SAVE, on_pwdlg_save)
  ON_COMMAND(MSG_TO_MAINMENU, exit_to_mainmenu)//YYF
END_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)

BEGIN_MSGPROC(pwdlg_proc_in_list, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_FOCUS_LEFT, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_pwdlg_cancel_in_list)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_list)
  ON_COMMAND(MSG_PRESET_PROG, on_pwdlg_correct_in_scrn)
  ON_COMMAND(MSG_SAVE, on_pwdlg_save)
END_MSGPROC(pwdlg_proc_in_list, cont_class_proc)


BEGIN_KEYMAP(pwdlg_keymap_in_scrn, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_FAVUP, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_FAVDOWN, MSG_FOCUS_UP)
#endif
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_BACK, MSG_BLACK_SCREEN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)//YYF
END_KEYMAP(pwdlg_keymap_in_scrn, NULL)

BEGIN_KEYMAP(pwdlg_keymap_in_list, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_FAVUP, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_FAVDOWN, MSG_FOCUS_UP)
#endif
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)  
END_KEYMAP(pwdlg_keymap_in_list, NULL)

BEGIN_AP_EVTMAP(ui_playback_evtmap)
  CONVERT_EVENT(PB_EVT_TTX_READY, MSG_TTX_READY)
  CONVERT_EVENT(PB_EVT_SUB_READY, MSG_SUBT_READY)
  CONVERT_EVENT(PB_EVT_DYNAMIC_PID, MSG_DYNC_PID_UPDATE)
  CONVERT_EVENT(PB_DESCRAMBL_SUCCESS, MSG_DESCRAMBLE_SUCCESS)
  CONVERT_EVENT(PB_DESCRAMBL_FAILED, MSG_DESCRAMBLE_FAILED)
  CONVERT_EVENT(PB_EVT_SCART_VCR_DETECTED, MSG_SCART_VCR_DETECTED)
  CONVERT_EVENT(PB_EVT_NIT_VERSION_SWITCH, MSG_PB_NIT_FOUND)
  CONVERT_EVENT(PB_EVT_NIT_OTA_UPGRADE, MSG_NIT_OTA_UPGRADE)
END_AP_EVTMAP(ui_playback_evtmap)

