/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_signal_api.h"
#include "ui_pro_info.h"
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif
enum plist_cont_ctrl_id
{
  IDC_PRO_CHAN,
  IDC_CHECK_VIDE,
  IDC_CHECK_VOIC,
  IDC_CHECK_PICT,
  IDC_CHECK_SERV,
  IDC_CHECK_SOUN,
  IDC_CHECK_CARD,
  IDC_NUM_VIDE,
  IDC_NUM_VOIC,
  IDC_NUM_PICT,
  IDC_NUM_SERV,
  IDC_NUM_SOUN,
  IDC_NUM_CARD,
  
  IDC_PRO_FREQ,
  IDC_PRO_SIGN,
  IDC_PRO_MODU,
  IDC_PRO_INTE,
  IDC_PRO_RATE,
  IDC_PRO_SNR,
  IDC_PRO_INUM,
  IDC_PRO_ONUM,
  
  IDC_NUM_FREQ,
  IDC_NUM_SIGN,
  IDC_NUM_MODU,
  IDC_NUM_INTE,
  IDC_NUM_RATE,
  IDC_NUM_SNR,
  IDC_STRE_BAR,
  IDC_BER_BAR,
  IDC_SNR_BAR,
  IDC_NUM_INUM,
  IDC_NUM_ONUM,
  
};

static u8 intensity = 0;
static u8 quality = 0;
static double tuner_ber = 0;
static u8 snr_max = 50;
static u8 snr_min = 0;
u16 prog_info_cont_keymap(u16 key);

RET_CODE prog_info_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_pro_info(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl,*p_bar, *p_text;
  u16 str[]={IDS_VIDEO_PID, IDS_AUDIO_PID, IDS_IMG_PID, IDS_PRO_INFO_SEVER_ID, IDS_AUDIO_TRACK, IDS_SMART_CARD_NUMBER};
  u16 stxt[]={IDS_FREQUENCE, IDS_SYMBOL, IDS_MODULATE, IDS_PRO_INFO_SINGAL_STREN, IDS_BER, IDS_PRO_INFO_SNR, 
              IDS_INSIDE_SERIAL, IDS_OUTSIDE_SERAIL};
  u16 x, y, i, pg_id;
  u8 view_id;
  u16 pos = 0;
  u16 uni_str[32],uni_num[32],uni_pro[64];
  u8 ansstr[16];
  dvbs_prog_node_t pg;
  audio_set_t pg_global_track;//use to set global track status
  dvbs_tp_node_t tp = {0};
  customer_cfg_t cfg = {0};
  intensity = 1;
  quality = 1;
  tuner_ber = 1.0;
  DEBUG_ENABLE_MODE(PRO_INFO,INFO);DEBUG(PRO_INFO,INFO,"\n");
  DEBUG(PRO_INFO,INFO,"Open program info now\n");
  get_customer_config(&cfg);
  if ((pg_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
  {
    return ERR_FAILURE;
  }

  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }

  sys_status_get_audio_set(&pg_global_track);

  view_id = ui_dbase_get_pg_view_id();
  pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  #ifdef LCN_SWITCH
  sprintf((char *)ansstr, "%4d ",pg.logic_ch_num);
  #else
  sprintf((char *)ansstr, "%03d ",pos + 1);
  #endif
  str_asc2uni(ansstr, uni_pro);
  
  memcpy(uni_str, pg.name, 32);
  uni_strcat(uni_pro, uni_str, uni_strlen(uni_str) + uni_strlen(uni_pro));

  if(db_dvbs_get_tp_by_id((u16)(pg.tp_id), &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }
  
  p_cont = ui_background_create(SN_ROOT_ID_PRO_INFO,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_CHANNEL_INFORMATION,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }


  ctrl_set_rstyle(p_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_cont, prog_info_cont_keymap);
  ctrl_set_proc(p_cont, prog_info_cont_proc);
  
  p_text = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PRO_CHAN),
                            PRO_INFO_CHAN_X, PRO_INFO_CHAN_Y, 10,10,
                            p_cont, 0);
  ctrl_set_rstyle(p_text, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  x = PRO_INFO_CHECK_X;
  y = PRO_INFO_CHECK_Y;
  for(i=0; i<5; i++)
  {
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CHECK_VIDE+i),
                              x,
                              y + i*(PRO_INFO_CHECK_H),
                              PRO_INFO_CHECK_W+30, PRO_INFO_CHECK_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, str[i]);

    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CHECK_VIDE+i+6),
                              PRO_INFO_NUMB_X,
                              y + i*(PRO_INFO_NUMB_H) ,
                              PRO_INFO_NUMB_W, PRO_INFO_NUMB_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    if(i == 0)
    {
      sprintf((char *)ansstr, "%d ",pg.video_pid);  
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);  
    }
    else if(i == 1)
    { 
      sprintf((char *)ansstr, "%d ",pg.audio[0].p_id);
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
    else if(i == 2)
    { 
      sprintf((char *)ansstr, "%d ",pg.pcr_pid);
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
    else if(i == 3)
    { 
      sprintf((char *)ansstr, "%d ",pg.s_id);
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
    else if(i == 4)
    {
      if(pg_global_track.is_global_track != 0)
      {
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        if(pg_global_track.global_track == 0)
        {
          text_set_content_by_strid(p_ctrl, IDS_STERO);
        }
        else if(pg_global_track.global_track == 1)
        {
          text_set_content_by_strid(p_ctrl, IDS_LEFT_TRACK);
        }
        else if(pg_global_track.global_track == 2)
        {
          text_set_content_by_strid(p_ctrl, IDS_RIGHT_TRACK);
        }
      }
	  else
      {
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        if(pg.audio_track == 0)
        {
          text_set_content_by_strid(p_ctrl, IDS_STERO);
        }
        else if(pg.audio_track == 1)
        {
          text_set_content_by_strid(p_ctrl, IDS_LEFT_TRACK);
        }
        else if(pg.audio_track == 2)
        {
          text_set_content_by_strid(p_ctrl, IDS_RIGHT_TRACK);
        }
      }
    }
    else if(i == 5)
    {
      
      str_asc2uni((u8 *)"", uni_str);
      text_set_content_by_unistr(p_ctrl, uni_str);
    }

    

  }
  x = PRO_INFO_TEXT_X;
  y = PRO_INFO_TEXT_Y+180;
  for(i=0; i<3; i++)
  {
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PRO_FREQ+i),
                              x,
                              y + i*(PRO_INFO_TEXT_H),
                              PRO_INFO_TEXT_W, PRO_INFO_TEXT_H,
                              p_cont, 0);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, stxt[i]);

    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_NUM_FREQ+i),
                              (x+PRO_INFO_TEXT_W+73),
                              y + i*(PRO_INFO_TEXT_H),
                              PRO_INFO_TEXT_W, PRO_INFO_TEXT_H,
                              p_cont, 0);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    if(i == 0)
    {
      sprintf((char *)ansstr, "%4ld KHz",tp.freq);
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
    else if(i == 1)
    { 
      sprintf((char *)ansstr, "%ld KBau",tp.sym);
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
    else if(i == 2)
    {  
      switch(tp.nim_modulate)
      {
        case NIM_MODULA_AUTO:
          sprintf((char *)ansstr, "AUTO");
          break;
        case NIM_MODULA_QAM16:
          sprintf((char *)ansstr, "QAM16");
          break;
        case NIM_MODULA_QAM32:
          sprintf((char *)ansstr, "QAM32");
          break;
        case NIM_MODULA_QAM64:
          sprintf((char *)ansstr, "QAM64");
          break;
        case NIM_MODULA_QAM128:
          sprintf((char *)ansstr, "QAM128");
          break;
        case NIM_MODULA_QAM256:
          sprintf((char *)ansstr, "QAM256");  
          break;
        default:
           sprintf((char *)ansstr, "QAM64");
          break;
      }
      str_asc2uni(ansstr, uni_num);
      text_set_content_by_unistr(p_ctrl, uni_num);
    }
  }

  switch(tp.nim_modulate)
  {
    case NIM_MODULA_QAM16:
      snr_min = (34080 - 19031) / 1000;
      snr_max = 35;
      break;

    case NIM_MODULA_QAM32:
      snr_min = (37600 - 19031) / 1000;
      snr_max = 38;
      break;

    case NIM_MODULA_QAM64:
      snr_min = (40310 - 19031) / 1000;
      snr_max = 41;
      break;

    case NIM_MODULA_QAM128:
      snr_min = (43720 - 19031) / 1000;
      snr_max = 44;
      break;
 
    case NIM_MODULA_QAM256:
      snr_min = (46390 - 19031) / 1000;
      snr_max = 47;
      break;
    default:
      snr_min = (40310 - 19031) / 1000;
      snr_max = 41;
      break;
  }
  
  // signal strength pbar1  
  p_bar = ui_comm_bar_create(p_cont, IDC_STRE_BAR,
                               SIG_STRENGTH_PBAR_X+102,
                               SIG_STRENGTH_PBAR_Y,
                               SIG_STRENGTH_PBAR_W,
                               SIG_STRENGTH_PBAR_H,
                               SIG_STRENGTH_NAME_X, 
                               SIG_STRENGTH_NAME_Y, 
                               SIG_STRENGTH_NAME_W+50, 
                               SIG_STRENGTH_NAME_H,
                               SIG_STRENGTH_PERCENT_X+102,
                               SIG_STRENGTH_PERCENT_Y,
                               SIG_STRENGTH_PERCENT_W,
                               SIG_STRENGTH_PERCENT_H);
  ui_comm_bar_set_param(p_bar, stxt[3], 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_COMM_PBAR_MID,
                          RSI_IGNORE, FSI_INDEX3,
                          RSI_IGNORE, FSI_INDEX3);
  ui_comm_intensity_bar_update(p_bar, 0,0, TRUE);

  // signal strength pbar2  
  p_bar = ui_comm_bar_create(p_cont, IDC_SNR_BAR,
                               SIG_SET_PBAR_X+102,
                               SIG_SET_PBAR_Y,
                               SIG_SET_PBAR_W,
                               SIG_SET_PBAR_H,
                               SIG_SET_NAME_X, 
                               SIG_SET_NAME_Y, 
                               SIG_SET_NAME_W, 
                               SIG_SET_NAME_H,
                               SIG_SET_PERCENT_X+107,
                               SIG_SET_PERCENT_Y, 
                               SIG_SET_PERCENT_W,
                               SIG_SET_PERCENT_H);
          
  ui_comm_bar_set_param(p_bar, stxt[5], 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_COMM_PBAR_MID,
                          RSI_IGNORE, FSI_INDEX3,
                          RSI_IGNORE, FSI_INDEX3);
  ui_comm_snr_bar_update(p_bar, 0, 0,TRUE);

  // signal strength pbar3  
  p_bar = ui_comm_bar_create(p_cont, IDC_BER_BAR,
                               SIG_SNR_PBAR_X+52,
                               SIG_SNR_PBAR_Y,
                               0,
                               SIG_SNR_PBAR_H,
                               SIG_SNR_NAME_X, 
                               SIG_SNR_NAME_Y, 
                               SIG_SNR_NAME_W+50, 
                               SIG_SNR_NAME_H,
                               SIG_SNR_PERCENT_X-SIG_SNR_PBAR_W+67,
                               SIG_SNR_NAME_Y,
                               SIG_SNR_PERCENT_W,
                               SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, stxt[4], 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_COMM_PBAR_MID,
                          RSI_IGNORE, FSI_INDEX3,
                          RSI_IGNORE, FSI_INDEX3);
  ui_comm_ber_string_update(p_bar, 0, TRUE);
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PRO_INUM),
                              x,
                              y + 3*(PRO_INFO_TEXT_H),
                              PRO_INFO_TEXT1_W, PRO_INFO_TEXT_H,
                              p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
   if(CUSTOMER_YINGJI == cfg.customer) 
    text_set_content_by_strid(p_ctrl, IDS_CUR_STBID);
  else
    text_set_content_by_strid(p_ctrl, stxt[6]);

  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_NUM_INUM),
                              x + PRO_INFO_TEXT1_W+20,
                              y + 3*(PRO_INFO_TEXT_H),
                              PRO_INFO_TEXT1_W, PRO_INFO_TEXT_H,
                              p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  memset(ansstr,0,sizeof(ansstr));
  get_sys_stbid(CUSTOMER_STDID_SIZE,ansstr,sizeof(ansstr));
  str_asc2uni((u8 *)ansstr, uni_str);
  text_set_content_by_unistr(p_ctrl, uni_str);

  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PRO_ONUM),
                                x,
                                y + 4*(PRO_INFO_TEXT_H),
                                PRO_INFO_TEXT1_W + 10, PRO_INFO_TEXT_H,
                                p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  if(CUSTOMER_YINGJI == cfg.customer) 
    text_set_content_by_strid(p_ctrl, IDS_CA_AUTH_TIME_EDN);
  else
    text_set_content_by_strid(p_ctrl, stxt[7]);

  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_NUM_ONUM),
                                x + PRO_INFO_TEXT1_W + 20,
                                y + 4*(PRO_INFO_TEXT_H),
                                PRO_INFO_TEXT1_W - 10, PRO_INFO_TEXT_H,
                                p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //text_set_content_by_unistr(p_ctrl, (u16*)"");
  if(CUSTOMER_YINGJI != cfg.customer) 
  {
    get_sys_stbid(CUSTOMER_STDID_SIZE,ansstr,sizeof(ansstr));
    str_asc2uni((u8 *)ansstr, uni_str);
    text_set_content_by_unistr(p_ctrl, uni_str);
  }
  ui_enable_signal_monitor(TRUE);
  //ui_comm_timedate_create(p_cont,uni_pro,RSI_TITLE_BG);
  
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  if(TRUE == get_full_scr_ad_status())
    ui_pic_stop();
#endif
  
 // ui_comm_help_create(&pro_info_help_data, p_cont);
  ctrl_default_proc(p_text, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif


  DEBUG(PRO_INFO,INFO,"\n");

  return SUCCESS;
}
static RET_CODE on_exit_pro_info(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  manage_close_menu(SN_ROOT_ID_PRO_INFO, 0, 0);
  intensity = 0;
  quality = 0;
  tuner_ber = 0;
  return SUCCESS;
}

static RET_CODE on_signal_info_update(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *p_bar1=NULL;
  control_t *p_bar2, *p_bar3;
  customer_cfg_t cfg = {0};
  struct signal_data *data = (struct signal_data *)(para1);
  u8 percent_strength = 0;
  u8 percent_snr= 0;
  percent_strength = (data->intensity * 100) / DVB_C_TC2800_STRENGTH_MAX;
  percent_snr = (data->quality  * 100) / snr_max ;
  get_customer_config(&cfg);
  if(intensity != data->intensity)
  {
    p_bar1 = ctrl_get_child_by_id(p_cont, IDC_STRE_BAR);
    ui_comm_intensity_bar_update(p_bar1, data->intensity,percent_strength ,TRUE);
    ctrl_paint_ctrl(ctrl_get_root(p_bar1), TRUE);

    intensity = data->intensity;
  }
  if(quality != data->quality)
  {
    p_bar2 = ctrl_get_child_by_id(p_cont, IDC_SNR_BAR);
    ui_comm_snr_bar_update(p_bar2, data->quality,percent_snr, TRUE);
    ctrl_paint_ctrl(ctrl_get_root(p_bar2), TRUE);
    quality = data->quality;
  }
  if(tuner_ber != data->ber_c)
  {
  p_bar3 = ctrl_get_child_by_id(p_cont, IDC_BER_BAR);
    ui_comm_ber_string_update(p_bar3, data->ber_c, TRUE);
    ctrl_paint_ctrl(ctrl_get_root(p_bar3), TRUE);
    tuner_ber = data->ber_c;
  }
#ifndef WIN32
if(CUSTOMER_YINGJI == cfg.customer) 
{
    void *p_dev = NULL;
    u8 str[5]={0};
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev);
    sprintf((char *)str, "%03d", quality);
    uio_display(p_dev, str, 4);
}
#endif

  return SUCCESS;
}

BEGIN_KEYMAP(prog_info_cont_keymap, NULL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)  
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_INFO, MSG_EXIT)
END_KEYMAP(prog_info_cont_keymap, NULL)

BEGIN_MSGPROC(prog_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_info_update)
  ON_COMMAND(MSG_EXIT, on_exit_pro_info)
END_MSGPROC(prog_info_cont_proc, ui_comm_root_proc)

