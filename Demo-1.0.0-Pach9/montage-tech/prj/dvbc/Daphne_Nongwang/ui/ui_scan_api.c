/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_mute.h"

#define DVBCFREQTABLE_SHUZHOU        1
#define DVBCFREQTABLE_HANGZHOU       2
#define DVBCFREQTABLE_CHENGDU        3
#define G_DVBCFREQTABLE              DVBCFREQTABLE_CHENGDU

#if 0  
/****shuzhou***/
s32 g_DVBCFreqTable[][2] =
{
    {107000,6875}, {115000,6875}, {123000,6875}, {131000,6875}, {139000,6875},
    {147000,6875}, {155000,6875}, {163000,6875}, {171000,6875}, {179000,6875},
    {187000,6875}, {195000,6875}, {203000,6875}, {211000,6875}, {219000,6875},
    {227000,6875}, {235000,6875}, {243000,6875}, {251000,6875}, {259000,6875}, 
    {267000,6875}, {275000,6875}, {283000,6875}, {291000,6875}, {299000,6875},
    {307000,6875}, {315000,6875}, {323000,6875}, {331000,6875}, {339000,6875},
    {347000,6875}, {355000,6875}, {363000,6875}, {371000,6875}, {379000,6875}, 
    {387000,6875}, {395000,6875}, {403000,6875}, {411000,6875}, {419000,6875},
    {427000,6875}, {435000,6875}, {443000,6875}, {451000,6875}, {459000,6875},
    {467000,6875},
    {474000,6875}, {482000,6875}, {490000,6875}, {498000,6875}, {506000,6875},
    {514000,6875}, {522000,6875}, {530000,6875}, {538000,6875}, {546000,6875}, 
    {554000,6875}, {562000,6875}, 
    /******auto ********  full scan********/
    {570000,6875}, {578000,6875}, {586000,6875},
    {594000,6875}, {602000,6875}, {610000,6875}, {618000,6875}, {626000,6875},
    {634000,6875}, {642000,6875}, {650000,6875}, {658000,6875}, {666000,6875},
    {674000,6875}, {682000,6875}, {690000,6875}, {698000,6875}, {706000,6875}, 
    {714000,6875}, {722000,6875}, {730000,6875}, {738000,6875}, {746000,6875}, 
    {754000,6875}, {762000,6875}, {770000,6875}, {778000,6875}, {786000,6875}, 
    {794000,6875}, {802000,6875}, {810000,6875}, {818000,6875}, {826000,6875},
    {834000,6875}, {842000,6875}, {850000,6875}, {858000,6875}, {866000,6875},
    {874000,6875}, {882000,6875}, {890000,6875},    
};
#elif(G_DVBCFREQTABLE == DVBCFREQTABLE_HANGZHOU)
/***hangzhou***/
s32 g_DVBCFreqTable[][2] =
{
    {107000,6900}, {115000,6900}, {123000,6900}, {131000,6900}, {139000,6900},
    {147000,6900}, {155000,6900}, {163000,6900}, {171000,6875}, {179000,6900},
    {187000,6900}, {195000,6900}, {203000,6900}, {211000,6875}, {219000,6900},
    {227000,6900}, {235000,6900}, {243000,6900}, {251000,6900}, {259000,6900}, 
    {267000,6900}, {275000,6900}, {283000,6900}, {291000,6900}, {299000,6900},
    {307000,6900}, {315000,6875}, {323000,6900}, {331000,6900}, {339000,6900},
    {347000,6900}, {355000,6900}, {363000,6900}, {371000,6900}, {379000,6900}, 
    {387000,6900}, {395000,6900}, {403000,6900}, {411000,6900}, {419000,6900},
    {427000,6900}, {435000,6900}, {443000,6900}, {451000,6900}, {459000,6900},
    {467000,6900},
    {474000,6900}, {482000,6900}, {490000,6900}, {498000,6900}, {506000,6900},
    {514000,6900}, {522000,6900}, {530000,6900}, {538000,6900}, {546000,6875}, 
    {554000,6900}, {562000,6875}, {570000,6900}, {578000,6900}, {586000,6900},
    {594000,6900}, {602000,6900}, {610000,6900}, {618000,6900}, {626000,6900},
    {634000,6875}, {642000,6875}, {650000,6875}, {658000,6900}, {666000,6900},
    {674000,6875}, {682000,6875}, {690000,6900}, {698000,6900}, {706000,6875}, 
    {714000,6900}, {722000,6900}, {730000,6900}, {738000,6900}, {746000,6900}, 
    {754000,6900}, {762000,6900}, {770000,6900}, {778000,6900}, {786000,6900}, 
    {794000,6900}, {802000,6900}, {810000,6900}, {818000,6900}, {826000,6900},
    {834000,6900}, {842000,6900}, {850000,6900}, {858000,6900}, {866000,6900},
    {874000,6900}, {882000,6900}, {890000,6900},    
};
#elif(G_DVBCFREQTABLE == DVBCFREQTABLE_CHENGDU)
/***chengdu***/
s32 g_DVBCFreqTable[][2] =
{
    {107000,6875}, {115000,6875}, {123000,6875}, {131000,6875}, {139000,6875},
    {147000,6875}, {155000,6875}, {163000,6875}, {171000,6875}, {179000,6875},
    {187000,6875}, {195000,6875}, {203000,6875}, {211000,6875}, {219000,6875},
    {227000,6875}, {235000,6875}, {243000,6875}, {251000,6875}, {259000,6875}, 
    {267000,6875}, {275000,6875}, {283000,6875}, {291000,6875}, {299000,6875},
    {307000,6875}, {315000,6875}, {323000,6875}, {331000,6875}, {339000,6875},
    {347000,6875}, {355000,6875}, {363000,6875}, {371000,6875}, {379000,6875}, 
    {387000,6875}, {395000,6875}, {403000,6875}, {411000,6875}, {419000,6875},
    {427000,6875}, {435000,6875}, {443000,6875}, {451000,6875}, {459000,6875},
    {467000,6875},
    {474000,6875}, {482000,6875}, {490000,6875}, {498000,6875}, {506000,6875},
    {514000,6875}, {522000,6875}, {530000,6875}, {538000,6875}, {546000,6875}, 
    {554000,6875}, {562000,6875}, {570000,6875}, {578000,6875}, {586000,6875},
    {594000,6875}, {602000,6875}, {610000,6875}, {618000,6875}, {626000,6875},
    {634000,6875}, {642000,6875}, {650000,6875}, {658000,6875}, {666000,6875},
    {674000,6875}, {682000,6875}, {690000,6875}, {698000,6875}, {706000,6875}, 
    {714000,6875}, {722000,6875}, {730000,6875}, {738000,6875}, {746000,6875}, 
    {754000,6875}, {762000,6875}, {770000,6875}, {778000,6875}, {786000,6875}, 
    {794000,6875}, {802000,6875}, {810000,6875}, {818000,6875}, {826000,6875},
    {834000,6875}, {842000,6875}, {850000,6875}, {858000,6875}, {866000,6875},
    {874000,6875}, {882000,6875}, {890000,6875},    
};
#endif
s32 g_DVBCFreqTableYinji[][2] =
{
    {261000,6875}, {269000,6875}, {278000,6875}, {285000,6875}, 
    {293000,6875}, {301000,6875}, {309000,6875}, {317000,6875}, 
    {474000,6875}, {482000,6875}, {490000,6875}, {498000,6875}, 
    {506000,6875}, {509000,6875}, {514000,6875}, {517000,6875}, 
    {522000,6875}, {525000,6875}, {530000,6875}, {533000,6875}, 
    {538000,6875}, {541000,6875}, {546000,6875}, {549000,6875}, 
    {554000,6875}, {557000,6875}, {562000,6875}, {565000,6875}, 
    {573000,6875}, {581000,6875}, {589000,6875}, {597000,6875}, 
    {605000,6875}, {613000,6875}, {621000,6875}, {629000,6875}, 
    {637000,6875}, {645000,6875}, {653000,6875}, {661000,6875}, 
    {701000,6875}, {709000,6875}, {717000,6875}, {725000,6875}, 
    {733000,6875}, {741000,6875}, {749000,6875}, {757000,6875}, 
};

s32 g_DVBCFreqTableXinsidaSpaish[][2] =
{
    {123000,5057}, {129000,5057}, {135000,5057}, {141000,5057}, 
    {147000,5057}, {153000,5057}, {159000,5057}, {165000,5057}, 
    {171000,5057}, {177000,5057}, {183000,5057}, {189000,5057}, 
    {195000,5057}, {201000,5057}, {207000,5057}, {213000,5057}, 
    {219000,5057}, {225000,5057}, {231000,5057}, {237000,5057}, 
    {243000,5057}, {249000,5057}, {255000,5057}, {261000,5057}, 
    {267000,5057}, {273000,5057}, {279000,5057}, {285000,5057}, 
    {291000,5057}, {297000,5057}, {303000,5057}, {309000,5057}, 
    {315000,5057}, {321000,5057}, {327000,5057}, {333000,5057}, 
    {339000,5057}, {345000,5057}, {351000,5057}, {357000,5057}, 
    {363000,5057}, {369000,5057}, {375000,5057}, {381000,5057}, 
    {387000,5057}, {393000,5057}, {399000,5057}, {405000,5057}, 
    {411000,5057}, {417000,5057}, {423000,5057}, {429000,5057}, 
    {435000,5057}, {441000,5057}, {447000,5057}, {453000,5057}, 
    {459000,5057}, {465000,5057}, {471000,5057}, {477000,5057}, 
    {483000,5057}, {489000,5057}, {495000,5057}, {501000,5057}, 
    {507000,5057}, {513000,5057}, {519000,5057}, {525000,5057}, 
    {531000,5057}, {537000,5057}, {543000,5057}, {549000,5057}, 
    {555000,5057}, {561000,5057}, {567000,5057}, {573000,5057}, 
    {579000,5057}, {585000,5057}, {591000,5057}, {597000,5057}, 
    {603000,5057}, {609000,5057}, {615000,5057}, {621000,5057}, 
    {627000,5057}, {633000,5057}, {639000,5057}, {645000,5057}, 
    {651000,5057}, {657000,5057}, {663000,5057}, {669000,5057}, 
    {675000,5057}, {681000,5057}, {687000,5057}, {693000,5057}, 
    {699000,5057}, {705000,5057}, {711000,5057}, {717000,5057}, 
    {723000,5057}, {729000,5057}, {735000,5057}, {741000,5057}, 
    {747000,5057}, {753000,5057}, {759000,5057}, {765000,5057}, 
    {771000,5057}, {777000,5057}, {783000,5057}, {789000,5057}, 
    {795000,5057}, {801000,5057}, {807000,5057}, {813000,5057}, 
    {819000,5057}, {825000,5057}, {831000,5057}, {837000,5057}, 
    {843000,5057}, {849000,5057}, {855000,5057}, {861000,5057}, 
};

#define DVBC_TP_ALL_COUNT (sizeof(g_DVBCFreqTable) / (sizeof(s32) * 2))

#define DVBC_TP_ALL_COUNT_YINGJI (sizeof(g_DVBCFreqTableYinji) / (sizeof(s32) * 2))

#define DVBC_TP_ALL_COUNT_XINSIDA_SPAISH (sizeof(g_DVBCFreqTableXinsidaSpaish) / (sizeof(s32) * 2))
// param package
struct
{
  u8 type;
  BOOL is_paused;
  scan_input_param_t *p_param;
}g_scan_param = {0, FALSE, NULL};

BOOL ui_get_scan_pause_status(void)
{
  return g_scan_param.is_paused;
}

void ui_set_dvbc_allfreq_symb(u16 symb)
{
  u8 i = 0;
  dvbc_lock_t tp = {0};
  u16 table_sym = 6875;
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);

  if(cfg.customer == CUSTOMER_DEMO)
    {
       return;
    }

  if(symb == 0)
  {
    sys_status_get_main_tp1(&tp);
    if(tp.tp_sym != 0)
    {
      table_sym = tp.tp_sym;
    }
  }
  else
    {
      table_sym = symb;
    }
  
  for (i = 0; i < DVBC_TP_ALL_COUNT; i++)
  {
    g_DVBCFreqTable[i][1] = table_sym;
  }

  return;
}

u8 ui_get_dvbc_allfreq_count(void)
{
   return (u8)DVBC_TP_ALL_COUNT;
}

u32 ui_get_dvbc_min_freq(void)
{
   return g_DVBCFreqTable[0][0];
}

u32 ui_get_index_by_freq(u32 freq)
{
  u8 i;
  u8 allfreq_count;
  s32 num;
  u32 index = 0;
  
  allfreq_count = ui_get_dvbc_allfreq_count();
  for(i=0; i < allfreq_count; i++)
  {
    num = freq-g_DVBCFreqTable[i][0];
    if(abs(num) <= 3000)
    {
      index = i;
	  break;
    }
  }
  if(i >= allfreq_count)
  {
    index = 0;
  }
  return index;
}

u32 ui_get_dvbc_max_freq(void)
{
   return g_DVBCFreqTable[(DVBC_TP_ALL_COUNT - 1)][0];
}

u16 ui_scan_evtmap(u32 event);

void add_dvbc_all_tp(void)
{
  u8 i = 0;
  dvbs_tp_node_t tp = {0};
  for (i = 0; i < DVBC_TP_ALL_COUNT; i++)
  {
    tp.freq = g_DVBCFreqTable[i][0];
    tp.sym = g_DVBCFreqTable[i][1];
    tp.nim_modulate = NIM_MODULA_QAM64;
    ui_scan_param_add_tp(&tp);
  }
}

void add_dvbc_all_tp_yj(void)
{
  u8 i = 0;
  dvbs_tp_node_t tp = {0};
  for (i = 0; i < DVBC_TP_ALL_COUNT_YINGJI; i++)
  {
    tp.freq = g_DVBCFreqTableYinji[i][0];
    tp.sym = g_DVBCFreqTableYinji[i][1];
    tp.nim_modulate = NIM_MODULA_QAM64;
    ui_scan_param_add_tp(&tp);
  }
}

void add_dvbc_all_tp_xinsida_spaish(void)
{
  u8 i = 0;
  dvbs_tp_node_t tp = {0};
  for (i = 0; i < DVBC_TP_ALL_COUNT_XINSIDA_SPAISH; i++)
  {
    tp.freq = g_DVBCFreqTableXinsidaSpaish[i][0];
    tp.sym = g_DVBCFreqTableXinsidaSpaish[i][1];
    tp.nim_modulate = NIM_MODULA_QAM64;
    ui_scan_param_add_tp(&tp);
  }
}

void ui_scan_param_init(void)
{
  // use common block
  ap_scan_set_attach_block(TRUE, BLOCK_COMMON_BUFFER);
  
  g_scan_param.p_param = ap_scan_param_buffer();
  MT_ASSERT(g_scan_param.p_param != NULL);

  memset(g_scan_param.p_param, 0, sizeof(scan_input_param_t));
}

void ui_scan_param_set_type(u8 type, u8 chan_type, u8 free_only, nit_scan_type_t nit_type, 
                            scan_scramble_origin_t scramble, cat_scan_type_t cat_type,bat_scan_type_t bat_type)
{
  scan_input_param_t *p_param = g_scan_param.p_param;
  MT_ASSERT(p_param != NULL);
  
  g_scan_param.type = type;
  
  p_param->scan_data.nit_scan_type = nit_type;
  p_param->scan_data.scramble_scan_origin = scramble;
  p_param->scan_data.cat_scan_type = cat_type;
  p_param->chan_type = chan_type;
  p_param->is_free_only = free_only;
  

  p_param->scan_data.bat_scan_type = bat_type;

}


u8 ui_scan_param_get_type(void)
{
  return g_scan_param.type;
}

nit_scan_type_t ui_scan_param_nit_type(void)
{
  return g_scan_param.p_param->scan_data.nit_scan_type;
}

u16 ui_scan_param_get_sat_num(void)
{
  return g_scan_param.p_param->scan_data.total_sat;
}


BOOL ui_scan_param_add_tp(dvbs_tp_node_t *p_tp_info)
{
  scan_preset_data_t *p_scan_buf = &g_scan_param.p_param->scan_data;
  u16 cur_tp_num = p_scan_buf->total_tp;
  scan_tp_info_i_t *p_buf_tp = &p_scan_buf->tp_list[cur_tp_num];

  MT_ASSERT(p_scan_buf != NULL);
  if(p_scan_buf->total_tp >= MAX_TP_NUM_SUPPORTED)
  {
    return FALSE;
  }

  trans_tp_info(&p_buf_tp->tp_info, p_tp_info);
  //Set tp info
  p_buf_tp->id = p_tp_info->id;

  //Update current satellite information
  p_scan_buf->total_tp++;
  return TRUE;
}

void ui_scan_param_set_pid(u16 v_pid, u16 a_pid, u16 pcr_pid)
{
  scan_input_param_t *p_param = g_scan_param.p_param;
  MT_ASSERT(p_param != NULL);

  p_param->pid_scan_enable = TRUE;
  
  p_param->pid_parm.video_pid = v_pid;
  p_param->pid_parm.audio_pid = a_pid;
  p_param->pid_parm.pcr_pid = pcr_pid;
}


void ui_init_scan(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_SCAN, ui_scan_evtmap);
  fw_register_ap_msghost(APP_SCAN, SN_ROOT_ID_SEARCH_RESULT);
  fw_register_ap_msghost(APP_SCAN, SN_ROOT_ID_SEARCH);
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
  fw_register_ap_msghost(APP_SCAN, ROOT_ID_FULL_SEARCH);
#endif
}


void ui_release_scan(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_msghost(APP_SCAN, SN_ROOT_ID_SEARCH);
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
  fw_unregister_ap_msghost(APP_SCAN, ROOT_ID_FULL_SEARCH);
#endif
  fw_unregister_ap_msghost(APP_SCAN, SN_ROOT_ID_SEARCH_RESULT);
  fw_unregister_ap_evtmap(APP_SCAN);
}

BOOL ui_start_scan(void)
{
  cmd_t cmd = {SCAN_CMD_START_SCAN};
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  switch(g_scan_param.type)
  {
    case USC_BLIND_SCAN:
      g_scan_param.p_param->scan_mode = BLIND_SCAN;
      break;
    case USC_PID_SCAN:
    case USC_TP_SCAN:
    case USC_PRESET_SCAN:
      g_scan_param.p_param->scan_mode = TP_SCAN;
      break;
    case USC_DVBC_FULL_SCAN:
      g_scan_param.p_param->scan_mode = DVBC_SCAN;
      break;
    case USC_DVBC_MANUAL_SCAN:
      g_scan_param.p_param->scan_mode = DVBC_SCAN;
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }
if(get_uio_led_number_type() == (u8)UIO_LED_TYPE_3D)
{
  if(CUSTOMER_ZHONGDA == cfg.customer)
   ui_set_front_panel_by_str("SCAN");
  else
   ui_set_front_panel_by_str("SCH");
}
else
  ui_set_front_panel_by_str("SCAN");

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  cmd.data1 = (u32)ap_scan_param_apply();
  ap_frm_do_command(APP_SCAN, &cmd);

  g_scan_param.is_paused = FALSE;
  
  return TRUE;
}

void ui_stop_scan(void)
{
  cmd_t cmd = {SCAN_CMD_CANCEL_SCAN};

  if(g_scan_param.is_paused)
  {
    ui_resume_scan();
  }
  ui_set_front_panel_by_str("----");
  ap_frm_do_command(APP_SCAN, &cmd);
}


void ui_pause_scan(void)
{
  cmd_t cmd;

  if(g_scan_param.is_paused)
  {
    OS_PRINTF("ui_pause_scan: already paused, ERROR!\n");
    return;
  }

  cmd.id = AP_FRM_CMD_PAUSE_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  g_scan_param.is_paused = TRUE;
}


void ui_resume_scan(void)
{
  cmd_t cmd;

  if(!g_scan_param.is_paused)
  {
    OS_PRINTF("ui_resume_scan: not paused, ERROR!\n");
    return;
  }

  cmd.id = AP_FRM_CMD_RESUME_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  g_scan_param.is_paused = FALSE;
}

BEGIN_AP_EVTMAP(ui_scan_evtmap)
  CONVERT_EVENT(SCAN_EVT_PG_FOUND,  MSG_SCAN_PG_FOUND)
  CONVERT_EVENT(SCAN_EVT_TP_FOUND,  MSG_SCAN_TP_FOUND)
  CONVERT_EVENT(SCAN_EVT_PROGRESS,  MSG_SCAN_PROGRESS)
  CONVERT_EVENT(SCAN_EVT_SAT_SWITCH, MSG_SCAN_SAT_FOUND)
  CONVERT_EVENT(SCAN_EVT_NO_MEMORY,   MSG_SCAN_DB_FULL)
  CONVERT_EVENT(SCAN_EVT_FINISHED,  MSG_SCAN_FINISHED)
  CONVERT_EVENT(SCAN_EVT_NIT_FOUND, MSG_SCAN_NIT_FOUND)
END_AP_EVTMAP(ui_scan_evtmap);


