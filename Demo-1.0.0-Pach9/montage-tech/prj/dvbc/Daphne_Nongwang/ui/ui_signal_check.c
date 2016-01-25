/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_scan_api.h"
#include "ui_signal_check.h"


enum local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 500,
  MSG_CLOSE_ALL,
  MSG_CHECK,
  MSG_FOCUS,
};

enum control_id
{
  IDC_AD_WINDOW = 1,
  IDC_SIG_CHECK,
  IDC_CHANNEL_TITLE1,
  IDC_CHANNEL_TITLE2,
  IDC_LIST,
  IDC_SPECIFY_TP_FRM,
  IDC_SIGNAL_INFO_FRM,
  IDC_CONT_TITLE,
  IDC_SBAR,
};

enum specify_tp_frm_id
{
  IDC_SPECIFY_TP_FREQ = 1,
  IDC_SPECIFY_TP_SYM,
  IDC_SPECIFY_TP_DEMOD,
};

enum signal_info_frm_id
{
  IDC_SEARCH_INPUT_TP = 1,
  IDC_SEARCH_INPUT_SYM,
  IDC_SEARCH_INPUT_DEMODUL,
  IDC_SEARCH_SIG_SNR_BAR,
  IDC_SEARCH_SIG_BER_BAR,
  IDC_SEARCH_SIG_STRENGTH_BAR,
  IDC_SEARCH_SIG_STRENGTH_NAME,
  IDC_SEARCH_SIG_STRENGTH_PBAR,
  IDC_SEARCH_SIG_STRENGTH_PERCENT,
  IDC_SEARCH_SIG_BER_NAME,
  IDC_SEARCH_SIG_BER_PBAR,
  IDC_SEARCH_SIG_BER_PERCENT,
  IDC_SEARCH_SIG_SNR_NAME,
  IDC_SEARCH_SIG_SNR_PBAR,
  IDC_SEARCH_SIG_SNR_PERCENT,
};

#if 0
static s32 g_freqTable[] =
{
    123000, 131000, 139000, 147000, 155000, 163000, 171000, 179000,
    187000, 195000, 203000, 211000, 219000, 227000, 235000, 243000,
    251000, 259000, 267000, 275000, 283000, 291000, 299000, 307000,
    315000, 323000, 331000, 339000, 347000, 355000, 363000, 371000,
    
    379000, 387000, 395000, 403000, 411000, 419000, 427000, 435000,
    443000, 451000, 459000, 474000, 482000, 490000, 498000, 506000,
    514000, 522000, 530000, 538000, 546000, 554000, 562000, 570000,
    578000, 586000, 594000, 602000, 610000, 618000, 626000, 634000,
    
    642000, 650000, 658000, 666000, 674000, 682000, 690000, 698000,
    706000, 714000, 722000, 730000, 738000, 746000, 754000, 762000,
    770000, 778000, 786000, 794000, 802000, 810000, 818000, 826000,
    834000, 842000, 850000, 858000, 866000, 874000, 882000, 890000,    
};
#else
extern s32 g_DVBCFreqTable[][2];
extern s32 g_DVBCFreqTableXinsidaSpaish[][2];
#endif

#if 0
rsc

static comm_help_data_t signal_check_help_data = //help bar data
{
  3,
  3,
  {
    IDS_BACK,
    IDS_CHECK,
    //IDS_FN_SELECT,
    IDS_EXIT,
  },
  { 
    IM_ICON_BACK,
    IM_ICON_START_PAUSE,
    //IM_ICON_ARROW_LEFT_RIGHT,
    IM_ICON_EXIT,
  },
};
//#define TP_COUNT (sizeof(g_DVBCFreqTable)/sizeof(s32))

static u8 intensity = 0;
static u8 quality = 0;
static double tuner_ber = 0;
static u8 snr_max = 50;
static u8 snr_min = 0;
#endif
extern u32 ui_get_dvbc_min_freq(void);
extern u32 ui_get_dvbc_max_freq(void);
extern u8 ui_get_dvbc_allfreq_count(void);

u16 signal_check_cont_keymap(u16 key);

u16 fre_list_keymap(u16 key);

#if 0
rsc

static RET_CODE signal_check_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

static RET_CODE fre_list_pro(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE focus_to_list(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#endif

u16 signal_check_signal_frm_keymap(u16 key);

u16 signal_check_input_tp_keymap(u16 key);
#if 0
rsc

static RET_CODE signal_check_input_tp_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE focus_to_scan_tp(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE nim_modulate_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE on_check(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE signal_start_check(control_t *p_cont);

static RET_CODE signal_list_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 cnt = list_get_count(p_list);
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      // NO. 
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      // frequency
      if(cus_cfg.customer == CUSTOMER_XINSIDA_SPAISH)
      {
        list_set_field_content_by_dec(p_list, (u16)(start + i), 1, g_DVBCFreqTableXinsidaSpaish[i + start][0]);
      }
      else
      {
        list_set_field_content_by_dec(p_list, (u16)(start + i), 1, g_DVBCFreqTable[i + start][0]);
      }

    }
  }
  return SUCCESS;
}
#endif


 RET_CODE open_signal_check(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_list, *p_comm_bar,*p_sbar, *p_second_title, *p_ad_win, *p_cont_title;//
  control_t *p_ctrl_tp, *p_signal_frm, *p_ctrl_sym, *p_ctrl_demodul;
  u16 item_bar_y = 0;
  u16 pbar_y = 0;
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SYSTEM_SET, RSI_TITLE_BG};
  list_xstyle_t rstyle =
  {
    RSI_COMM_LIST_N,
    RSI_COMM_LIST_N,
    RSI_COMM_LIST_HL,
    RSI_COMM_LIST_SEL,
    RSI_COMM_LIST_N,
  };
  list_xstyle_t fstyle =
  {
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_TXT_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
  };

  dvbc_lock_t main_tp = {0};
  intensity = 1;
  quality = 1;
  tuner_ber = 1.0;
  sys_status_get_main_tp1(&main_tp);
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_SIGNAL_CHECK,
                               RSI_FULL_SCREEN_BG,
                               SIG_CHECK_MENU_X, SIG_CHECK_MENU_Y,
                               SIG_CHECK_MENU_W, SIG_CHECK_MENU_H,
                               &title_data);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, signal_check_cont_keymap);
  ctrl_set_proc(p_cont, signal_check_cont_proc);

  //the title in left side
  p_second_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SIG_CHECK,
                              SIG_CHECK_LTITLE_X, SIG_CHECK_LTITLE_Y,
                              SIG_CHECK_LTITLE_W, SIG_CHECK_LTITLE_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_second_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_font_style(p_second_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_second_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_second_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_second_title, IDS_SIGNAL_CHECK);

  //advertisement window
  p_ad_win = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_AD_WINDOW,
                              SIG_CHECK_AD_X, SIG_CHECK_AD_Y,
                              SIG_CHECK_AD_W, SIG_CHECK_AD_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ad_win, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN);
  bmp_pic_draw(p_ad_win);

  //title in right side
  p_cont_title = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_CONT_TITLE, 
                              SIG_CHECK_RTITLE_X, SIG_CHECK_RTITLE_Y, 
                              SIG_CHECK_RTITLE_W, SIG_CHECK_RTITLE_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_cont_title,RSI_SECOND_TITLE,RSI_SECOND_TITLE, RSI_SECOND_TITLE);

  
  p_second_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHANNEL_TITLE1,
                              35, 0,
                              80, SIG_CHECK_RTITLE_H,
                              p_cont_title, 0);
  text_set_font_style(p_second_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_second_title, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_second_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_second_title, IDS_FREQUENCE);

   p_second_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHANNEL_TITLE2,
                              160, 0,
                              150, SIG_CHECK_RTITLE_H,
                              p_cont_title, 0);
  text_set_font_style(p_second_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_second_title, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_second_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_second_title, IDS_FREQ_KHZ);


  //freq list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, (u8)IDC_LIST,
                            SIG_CHECK_LIST_X, SIG_CHECK_LIST_Y,
                            SIG_CHECK_LIST_W, SIG_CHECK_LIST_H,
                            p_cont, 0);
  ctrl_set_keymap(p_list, fre_list_keymap);
  ctrl_set_proc(p_list,fre_list_pro);
  ctrl_set_rstyle(p_list, RSI_SIG_CHECK_LIST, RSI_SIG_CHECK_LIST, RSI_SIG_CHECK_LIST);
  list_set_count(p_list, (u16)ui_get_dvbc_allfreq_count(), SIG_CHECK_LIST_PAGE);
  list_set_mid_rect(p_list, SIG_CHECK_LIST_MIDL, SIG_CHECK_LIST_MIDT,
                    SIG_CHECK_LIST_MIDW, SIG_CHECK_LIST_MIDH, SIG_CHECK_LIST_VGAP);
  list_set_select_mode(p_list,LIST_SINGLE_SELECT);
  list_set_item_rstyle(p_list, &rstyle);
  list_set_field_count(p_list, SIG_CHECK_LIST_FIELD_CNT, SIG_CHECK_LIST_PAGE);
  list_set_field_attr(p_list, 0, STL_LEFT | LISTFIELD_TYPE_UNISTR,
                    80, 35, 0);
  list_set_field_attr(p_list, 1, STL_LEFT | LISTFIELD_TYPE_DEC,
                     SIG_CHECK_LIST_MIDW - 160, 160, 0);
  list_set_field_font_style(p_list, 0, &fstyle);
  list_set_field_font_style(p_list, 1, &fstyle);
  list_set_update(p_list, signal_list_update, 0);
  list_set_focus_pos(p_list, ui_get_index_by_freq(para1));
  signal_list_update(p_list, list_get_valid_pos(p_list), SIG_CHECK_LIST_PAGE, 0);

  //sbar
  p_sbar = ctrl_create_ctrl((u8 *)CTRL_SBAR, IDC_SBAR, 
                            SIG_CHECK_SCROLL_X, SIG_CHECK_SCROLL_Y, 
                            SIG_CHECK_SCROLL_W, SIG_CHECK_SCROLL_H,
                            p_list, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG,RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rect(p_sbar, 0, LIST_BAR_MID_OY, SIG_CHECK_SCROLL_W, SIG_CHECK_SCROLL_H-LIST_BAR_MID_OY);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  list_set_scrollbar(p_list,p_sbar);

  //signal status cont
  p_signal_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            SIG_CHECK_STATUS_CONT_X, SIG_CHECK_STATUS_CONT_Y,
                            SIG_CHECK_STATUS_CONT_W, SIG_CHECK_STATUS_CONT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_signal_frm, RSI_SIG_CHECK_LIST, RSI_SIG_CHECK_LIST, RSI_SIG_CHECK_LIST);
  
  //input tp
  p_ctrl_tp = ui_comm_numedit_create_withposix(p_signal_frm, (u8)IDC_SEARCH_INPUT_TP,
                                  SIG_CHECK_INPUT_TP_X,
                                  SIG_CHECK_INPUT_TP_Y,
                                  SIG_CHECK_INPUT_TP_LW,
                                  SIG_CHECK_INPUT_TP_MW,
                                  SIG_CHECK_INPUT_TP_RW);
  ui_comm_ctrl_set_keymap(p_ctrl_tp, signal_check_input_tp_keymap);
  ui_comm_ctrl_set_proc(p_ctrl_tp, signal_check_input_tp_proc);
  ui_comm_numedit_set_static_txt(p_ctrl_tp, IDS_ENTER_FREQ);
  ui_comm_ctrl_set_cont_rstyle(p_ctrl_tp, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
  ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_tp, RSI_NEW_PSW, RSI_OLD_PSW, RSI_NEW_PSW);
  ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_tp, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
  ui_comm_numedit_set_posix_txt(p_ctrl_tp,IDS_UNIT_KHZ);
  ui_comm_numedit_set_param(p_ctrl_tp, NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_tp, list_get_field_content(p_list, list_get_focus_pos(p_list), 1));

  //input freq
  p_ctrl_sym = ui_comm_numedit_create_withposix(p_signal_frm, (u8)IDC_SEARCH_INPUT_SYM,
                                  SIG_CHECK_INPUT_SYM_X,
                                  SIG_CHECK_INPUT_SYM_Y,
                                  SIG_CHECK_INPUT_SYM_LW,
                                  SIG_CHECK_INPUT_SYM_MW,
                                  SIG_CHECK_INPUT_SYM_RW);
  ui_comm_ctrl_set_keymap(p_ctrl_sym, signal_check_input_tp_keymap);
  ui_comm_ctrl_set_proc(p_ctrl_sym, signal_check_input_tp_proc);
  ui_comm_numedit_set_static_txt(p_ctrl_sym, IDS_FREQ_SET_SYMBOL);
  ui_comm_ctrl_set_cont_rstyle(p_ctrl_sym, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
  ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_sym, RSI_NEW_PSW, RSI_OLD_PSW, RSI_NEW_PSW);
  ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_sym, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
    ui_comm_numedit_set_posix_txt(p_ctrl_sym,IDS_UNIT_KBAUD);
  ui_comm_numedit_set_param(p_ctrl_sym, NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_sym, main_tp.tp_sym);

  //select nim modulate
  p_ctrl_demodul = ui_comm_select_create(p_signal_frm, (u8)IDC_SEARCH_INPUT_DEMODUL,
                                    SIG_CHECK_INPUT_DEMODUL_X,
                                    SIG_CHECK_INPUT_DEMODUL_Y,
                                    SIG_CHECK_INPUT_DEMODUL_LW,
                                    SIG_CHECK_INPUT_DEMODUL_RW);
  //ui_comm_ctrl_set_keymap(p_ctrl_demodul, nim_modulate_select_keymap);
  ui_comm_ctrl_set_proc(p_ctrl_demodul, nim_modulate_select_proc);
  ui_comm_ctrl_set_cont_rstyle(p_ctrl_demodul, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
  ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_demodul, RSI_NEW_PSW, RSI_OLD_PSW, RSI_NEW_PSW);
  ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_demodul, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
  ui_comm_select_set_static_txt(p_ctrl_demodul, IDS_FREQ_SET_DEMODUL);
  ui_comm_select_set_param(p_ctrl_demodul, TRUE,
                             CBOX_WORKMODE_STATIC, 5,
                             CBOX_ITEM_STRTYPE_STRID,
                             NULL);
  ui_comm_select_set_content(p_ctrl_demodul, 0, IDS_QAM16);
  ui_comm_select_set_content(p_ctrl_demodul, 1, IDS_QAM32);
  ui_comm_select_set_content(p_ctrl_demodul, 2, IDS_QAM64);
  ui_comm_select_set_content(p_ctrl_demodul, 3, IDS_QAM128);
  ui_comm_select_set_content(p_ctrl_demodul, 4, IDS_QAM256);
  ui_comm_select_set_focus(p_ctrl_demodul, main_tp.nim_modulate-4);

  item_bar_y = SIG_STATUS_BAR1_Y;
  pbar_y = item_bar_y + (SIG_STATUS_BAR_H - SIG_STATUS_BAR_MH)/2;  
  // signal strength pbar  
  p_comm_bar = ui_comm_bar_create(p_signal_frm, IDC_SEARCH_SIG_STRENGTH_BAR,
                       SIG_STATUS_BAR_MX, pbar_y, SIG_STATUS_BAR_MW, SIG_STATUS_BAR_MH,
                       SIG_STATUS_BAR_LX, item_bar_y, SIG_STATUS_BAR_LW, SIG_STATUS_BAR_H,
                       SIG_STATUS_BAR_RX, item_bar_y,SIG_STATUS_BAR_RW, SIG_STATUS_BAR_H);
  ui_comm_bar_set_param(p_comm_bar,IDS_STRENGTH, 0, 100, 100);
  ui_comm_bar_set_style(p_comm_bar,
                          RSI_SIG_CHECK_PBAR_BG, RSI_SIG_CHECK_PBAR_MID,
                          RSI_IGNORE, FSI_COMM_TXT_N,
                          RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  ui_comm_intensity_bar_update(p_comm_bar, 0,0, TRUE);

  // signal SNR pbar  
  item_bar_y += SIG_STATUS_BAR_H + SIG_CHECK_V_GAP;
  pbar_y = item_bar_y + (SIG_STATUS_BAR_H - SIG_STATUS_BAR_MH)/2;    
  p_comm_bar = ui_comm_bar_create(p_signal_frm, IDC_SEARCH_SIG_SNR_BAR,
                       SIG_STATUS_BAR_MX, pbar_y, SIG_STATUS_BAR_MW, SIG_STATUS_BAR_MH,
                       SIG_STATUS_BAR_LX, item_bar_y, SIG_STATUS_BAR_LW, SIG_STATUS_BAR_H,
                       SIG_STATUS_BAR_RX, item_bar_y,SIG_STATUS_BAR_RW, SIG_STATUS_BAR_H);
  ui_comm_bar_set_param(p_comm_bar,IDS_SNR , 0, 100, 100);
  ui_comm_bar_set_style(p_comm_bar,
                          RSI_SIG_CHECK_PBAR_BG, RSI_SIG_CHECK_PBAR_MID,
                          RSI_IGNORE, FSI_COMM_TXT_N,
                          RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  
  ui_comm_snr_bar_update(p_comm_bar, 0,0, TRUE);

    // signal BER pbar  
  item_bar_y += SIG_STATUS_BAR_H + SIG_CHECK_V_GAP;
  pbar_y = item_bar_y + (SIG_STATUS_BAR_H - SIG_STATUS_BAR_MH)/2;  
  p_comm_bar = ui_comm_bar_create(p_signal_frm, IDC_SEARCH_SIG_BER_BAR,
                       SIG_STATUS_BAR_MX, pbar_y, 0, SIG_STATUS_BAR_MH,
                       SIG_STATUS_BAR_LX, item_bar_y, SIG_STATUS_BAR_LW, SIG_STATUS_BAR_H,
                       SIG_STATUS_BAR_LX + SIG_STATUS_BAR_LW, item_bar_y,SIG_STATUS_BAR_RW+40, SIG_STATUS_BAR_H);
  ui_comm_bar_set_param(p_comm_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_comm_bar,
                        RSI_SIG_CHECK_PBAR_BG, RSI_SIG_CHECK_PBAR_MID,
                        RSI_IGNORE, FSI_COMM_TXT_N,
                        RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  ui_comm_ber_string_update(p_comm_bar, 0, TRUE);
  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_enable_signal_monitor(TRUE);

  ui_comm_help_create(&signal_check_help_data, p_cont);
  ctrl_default_proc(p_ctrl_tp, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  signal_start_check(p_cont);
#endif
  return SUCCESS;
}

#if 0
rsc

static RET_CODE focus_to_list(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *ctrl1,*ctrl2,*ctrl3,*p_list, *p_tp, *p_sym, *p_demodul;

  ctrl1 = ctrl_get_parent(p_ctrl);
  ctrl2 = ctrl_get_parent(ctrl1);
  ctrl3 = ctrl_get_parent(ctrl2);
  p_list = ctrl_get_child_by_id(ctrl3, IDC_LIST);
  p_tp = ctrl_get_child_by_id(ctrl2,IDC_SEARCH_INPUT_TP);
  p_sym = ctrl_get_child_by_id(ctrl2,IDC_SEARCH_INPUT_SYM);
  p_demodul = ctrl_get_child_by_id(ctrl2,IDC_SEARCH_INPUT_DEMODUL);
  if(para1 == V_KEY_DOWN)
  {
    switch(ctrl1->id)
    {
      case IDC_SEARCH_INPUT_TP:
        nbox_exit_edit(p_ctrl);
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(ctrl_get_child_by_id(p_sym,IDC_COMM_CTRL), MSG_GETFOCUS, 0, 0);
        break;
      case IDC_SEARCH_INPUT_SYM:
        nbox_exit_edit(p_ctrl);
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(ctrl_get_child_by_id(p_demodul,IDC_COMM_CTRL), MSG_GETFOCUS, 0, 0);
        break;
      case IDC_SEARCH_INPUT_DEMODUL:
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(ctrl_get_child_by_id(p_tp,IDC_COMM_CTRL), MSG_GETFOCUS, 0, 0);
        break;
      default:
        break;
    }
  }
  else if(para1 == V_KEY_UP)
  {
    switch(ctrl1->id)
    {
      case IDC_SEARCH_INPUT_TP:
        nbox_exit_edit(p_ctrl);
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
        ui_comm_numedit_set_num(ctrl1, list_get_field_content(p_list, list_get_focus_pos(p_list), 1));
        break;
      case IDC_SEARCH_INPUT_SYM:
        nbox_exit_edit(p_ctrl);
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(ctrl_get_child_by_id(p_tp,IDC_COMM_CTRL), MSG_GETFOCUS, 0, 0);
        break;
      case IDC_SEARCH_INPUT_DEMODUL:
        ctrl_process_msg(p_ctrl->p_parent, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(ctrl_get_child_by_id(p_sym,IDC_COMM_CTRL), MSG_GETFOCUS, 0, 0);
        break;
      default:
        break;
    }
  }
  ctrl_paint_ctrl(ctrl3, TRUE);
  return SUCCESS;
}

static RET_CODE focus_to_scan_tp(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *ctrl1,*ctrl2,*ctrl3,*p_tp;
  ctrl1 = ctrl_get_parent(p_ctrl);
  ctrl2 = ctrl_get_child_by_id(ctrl1,IDC_SIGNAL_INFO_FRM);
  ctrl3 = ctrl_get_child_by_id(ctrl2,IDC_SEARCH_INPUT_TP);
  p_tp = ctrl_get_child_by_id(ctrl3,IDC_COMM_CTRL);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
 
  ctrl_process_msg(p_tp, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl1, TRUE);

  return SUCCESS;
}
#endif

#if 0
rsc

static RET_CODE signal_start_check(control_t *p_cont)
{
  control_t *p_ctrl_info_frm, *p_ctrl_tp, *p_ctrl_sym, *p_ctrl_demodul;
  dvbs_tp_node_t tp = {0};
  u8 demod;

  p_ctrl_info_frm = ctrl_get_child_by_id(p_cont, IDC_SIGNAL_INFO_FRM);
  p_ctrl_tp = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_SEARCH_INPUT_TP);
  p_ctrl_sym = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_SEARCH_INPUT_SYM);
  p_ctrl_demodul = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_SEARCH_INPUT_DEMODUL);
  
  tp.freq = ui_comm_numedit_get_num(p_ctrl_tp);
  tp.sym = ui_comm_numedit_get_num(p_ctrl_sym);
  demod = (u8)ui_comm_select_get_focus(p_ctrl_demodul);

  switch(demod)
  {
    case 0:
      tp.nim_modulate = NIM_MODULA_QAM16;
      snr_min = (34080 - 19031) / 1000;
      snr_max = 35;
      break;

    case 1:
      tp.nim_modulate = NIM_MODULA_QAM32;
      snr_min = (37600 - 19031) / 1000;
      snr_max = 38;
      break;

    case 2:
      tp.nim_modulate = NIM_MODULA_QAM64;
      snr_min = (40310 - 19031) / 1000;
      snr_max = 41;
      break;

    case 3:
      tp.nim_modulate = NIM_MODULA_QAM128;
      snr_min = (43720 - 19031) / 1000;
      snr_max = 44;
      break;

    case 4:
      tp.nim_modulate = NIM_MODULA_QAM256;
      snr_min = (46390 - 19031) / 1000;
      snr_max = 47;
      break;

    default:
      tp.nim_modulate = NIM_MODULA_QAM64;
      snr_min = (40310 - 19031) / 1000;
      snr_max = 41;
      break;
  }
  
  ui_set_transpond(&tp);

  return SUCCESS;
}

static RET_CODE signal_check_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont,*ctrl2,*ctrl3;

  list_class_proc(p_list,  msg,  para1,  para2);

  list_get_focus_pos(p_list);
  
  p_cont = ctrl_get_parent(p_list);
  ctrl2 = ctrl_get_child_by_id(p_cont,IDC_SIGNAL_INFO_FRM);
  ctrl3 = ctrl_get_child_by_id(ctrl2,IDC_SEARCH_INPUT_TP);

  ui_comm_numedit_set_num(ctrl3, list_get_field_content(p_list, list_get_focus_pos(p_list), 1));
  ctrl_paint_ctrl(ctrl3, TRUE);

  return SUCCESS;
}

static RET_CODE signal_check_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  signal_start_check(p_list->p_parent);

  return SUCCESS;
}

static RET_CODE on_check(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *ctrl1,*ctrl2,*ctrl3;
  u8 i;
  ctrl1 = ctrl_get_parent(p_ctrl);
  ctrl2 = ctrl_get_parent(ctrl1);
  signal_start_check(p_ctrl->p_parent->p_parent->p_parent);
#if 0
  for(i=0;i<3;i++)
  {
    switch(i)
    {
      case 0:
        ctrl3 = ctrl_get_child_by_id(ctrl2, IDC_SEARCH_SIG_SNR_BAR+i);
        //ctrl_get_ctrl_id(control_t * p_ctrl)
        ui_comm_tp_bar_update(ctrl3, 0, 1,"dB");
        ui_comm_bar_paint(ctrl3, TRUE);
        break;
        
      case 1:
        ctrl3 = ctrl_get_child_by_id(ctrl2, IDC_SEARCH_SIG_SNR_BAR+i);
        //ctrl_get_ctrl_id(control_t * p_ctrl)
        ui_comm_tp_bar_update(ctrl3, 0, 1,"E -6");
        ui_comm_bar_paint(ctrl3, TRUE);
        break;
        
      case 2:
        ctrl3 = ctrl_get_child_by_id(ctrl2, IDC_SEARCH_SIG_SNR_BAR+i);
        //ctrl_get_ctrl_id(control_t * p_ctrl)
        ui_comm_tp_bar_update(ctrl3, 0, 1,"dBuV");
        ui_comm_bar_paint(ctrl3, TRUE);
        break;
        
      default:
        break;
    }
  }
#endif  
  return SUCCESS;
 }
#endif

#if 0
rsc

static RET_CODE on_signal_check_update(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *p_sig_info_frm, *p_strength, *p_snr, *p_ber;
  struct signal_data *data = (struct signal_data *)(para1);
  customer_cfg_t cfg = {0};
  u8 percent_strength = 0;
  u8 percent_snr= 0;
  percent_strength = (data->intensity * 100) / DVB_C_TC2800_STRENGTH_MAX;
  //percent_snr = ((data->quality -snr_min) * 100) / (snr_max -snr_min);
  percent_snr = (data->quality  * 100) / snr_max ;
  
  get_customer_config(&cfg);
  p_sig_info_frm = ctrl_get_child_by_id(p_cont, IDC_SIGNAL_INFO_FRM);

  p_strength = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_STRENGTH_BAR);
  //ui_comm_bar_update(p_strength, data->intensity, TRUE);
  ui_comm_intensity_bar_update(p_strength, data->intensity,percent_strength,TRUE);
  p_snr = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_SNR_BAR);
  //ui_comm_bar_update(p_snr, data->quality, TRUE);
  ui_comm_snr_bar_update(p_snr, data->quality,percent_snr,TRUE);
  
  p_ber = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_BER_BAR);
  ui_comm_ber_string_update(p_ber, data->ber_c, TRUE);
  if(data->intensity != intensity)
  {
    ctrl_paint_ctrl(p_strength, TRUE);
    intensity = data->intensity;
  }
  if(data->quality != quality)
  {
    ctrl_paint_ctrl(p_snr, TRUE);
    quality = data->quality;
  }
   if(data->ber_c!= tuner_ber)
  {
    ctrl_paint_ctrl(p_ber, TRUE);
    tuner_ber = data->ber_c;
  }
#ifndef WIN32
if(CUSTOMER_YINGJI == cfg.customer) 
{
    void *p_dev = NULL;
    u8 str[5]={0};
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev);
    sprintf((char *)str, "%03d ", quality);
    uio_display(p_dev, str, 4);
}
#endif

  return SUCCESS;
}

static RET_CODE on_signal_check_exit(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  ui_play_curn_pg();
  intensity = 0;
  quality = 0;
  tuner_ber = 0;
  return ERR_NOFEATURE;
}
static RET_CODE on_singal_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  sys_status_get_lang_set(&lang_set);
  if(lang_set.osd_text == 0)
  {
    rsc_set_curn_language(gui_get_rsc_handle(),2);
    lang_set.osd_text = 1;
  }
  else
  {
    rsc_set_curn_language(gui_get_rsc_handle(),1);
    lang_set.osd_text = 0;
  }
  sys_status_set_lang_set(&lang_set);
  sys_status_save();
  p_ctrl = ctrl_get_root(p_ctrl);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

#endif

BEGIN_KEYMAP(fre_list_keymap, NULL)
  ON_EVENT(V_KEY_UP,MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN,MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP,MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN,MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(fre_list_keymap, NULL)

#if 0
rsc

BEGIN_MSGPROC(fre_list_pro, list_class_proc)
  ON_COMMAND(MSG_FOCUS, focus_to_scan_tp)
  ON_COMMAND(MSG_FOCUS_UP, signal_check_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, signal_check_list_msg)
  ON_COMMAND(MSG_PAGE_UP, signal_check_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, signal_check_list_msg)
  ON_COMMAND(MSG_SELECT, signal_check_list_ok)
END_MSGPROC(fre_list_pro,list_class_proc)
#endif

BEGIN_KEYMAP(signal_check_cont_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(signal_check_cont_keymap, NULL)

#if 0
rsc

BEGIN_MSGPROC(signal_check_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_check_update)
  ON_COMMAND(MSG_EXIT, on_signal_check_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_signal_check_exit)
  ON_COMMAND(MSG_TO_MAINMENU, on_signal_check_exit)  
  ON_COMMAND(MSG_SET_LANG, on_singal_switch_lang)
END_MSGPROC(signal_check_cont_proc, ui_comm_root_proc)
#endif

BEGIN_KEYMAP(signal_check_input_tp_keymap, ui_comm_num_keymap)
  ON_EVENT(V_KEY_OK, MSG_CHECK)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS)
  ON_EVENT(V_KEY_UP, MSG_FOCUS)
END_KEYMAP(signal_check_input_tp_keymap, ui_comm_num_keymap)

#if 0
rsc

BEGIN_MSGPROC(signal_check_input_tp_proc,ui_comm_num_proc)
  ON_COMMAND(MSG_CHECK, on_check)
  ON_COMMAND(MSG_FOCUS, focus_to_list)
END_MSGPROC(signal_check_input_tp_proc,ui_comm_num_proc)

BEGIN_MSGPROC(nim_modulate_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, focus_to_list)
  ON_COMMAND(MSG_FOCUS_DOWN, focus_to_list)
  ON_COMMAND(MSG_SELECT, on_check)
END_MSGPROC(nim_modulate_select_proc, cbox_class_proc)
#endif

