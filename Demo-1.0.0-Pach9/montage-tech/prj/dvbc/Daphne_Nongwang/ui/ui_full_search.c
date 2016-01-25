/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_signal_api.h"
#include "ui_full_search.h"
#ifdef LCN_SWITCH
#include "ui_logic_num_proc.h"
#endif
enum local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 350,
  MSG_CLOSE_ALL,
  MSG_START_SCAN,
};

enum control_id
{
  IDC_FSEARCH_AD = 1,
  IDC_SECOND_TITLE,
  IDC_CHANNEL_TITLE,
  IDC_LIST,
  IDC_FSEARCH_SET_CONT,
  IDC_SIGNAL_INFO_FRM,
};

enum fsearch_set_item_id
{
  IDC_FSEARCH_SET_FREQ = 1,
  IDC_FSEARCH_SET_FREQ_END,
  IDC_FSEARCH_SET_SYM,
  IDC_FSEARCH_SET_DEMOD,
};

enum signal_info_frm_id
{
  IDC_FSEARCH_TP_RESULT = 1,
  IDC_FSEARCH_STRENGTH_BAR,
  IDC_FSEARCH_STREN_NAME,
  IDC_FSEARCH_STREN_BAR,
  IDC_FSEARCH_STREN_PER,
  IDC_FSEARCH_BER_NAME,
  IDC_FSEARCH_BER_BAR,
  IDC_FSEARCH_BER_PER,
  IDC_FSEARCH_SNR_NAME,
  IDC_FSEARCH_SNR_BAR,
  IDC_FSEARCH_SNR_PER,
};

enum search_prog_type_id
{
    PROG_TYPE_TV = 0,
    PROG_TYPE_RADIO,
};

s32 g_DVBCFreqTableYj[] =
{
    261000, 269000, 278000, 285000, 293000, 301000, 309000, 317000,
    474000, 482000, 490000, 498000, 506000, 509000, 514000, 517000,
    522000, 525000, 530000, 533000, 538000, 541000, 546000, 549000, 
    554000, 557000, 562000, 565000, 573000, 581000, 589000, 597000,
    605000, 613000, 621000, 629000, 637000, 645000, 653000, 661000,
    701000, 709000, 717000, 725000, 733000, 741000, 749000, 757000,
};

#define DVBC_TP_ALL_COUNT_YJ (sizeof(g_DVBCFreqTableYj) / (sizeof(s32)))

#define PROG_NAME_STRLEN    DB_DVBS_MAX_NAME_LENGTH

static u16 prog_name_str[FSEARCH_LIST_CNT][FSEARCH_LIST_PAGE][
  PROG_NAME_STRLEN + 1];
static u16 *prog_name_str_addr[FSEARCH_LIST_CNT][FSEARCH_LIST_PAGE];
static u16 prog_curn[FSEARCH_LIST_CNT];
static u16 last_id[FSEARCH_PROG_TYPE_CNT] = {0};
static u8 seach_intensity = 0;
static u8 seach_quality = 0;
static double search_ber = 0;
static u8 snr_max = 50;
static u8 snr_min = 0;
static u16 exit_msg = MSG_INVALID;
#define TP_NO_STRLEN      3 /* 999 */
#define TP_INFO_STRLEN    64

static u16 tp_curn;

//static BOOL is_db_empty = FALSE;
static BOOL is_stop = FALSE;
static BOOL is_ask_for_cancel = FALSE;
static BOOL is_finished = TRUE;
static BOOL is_init_scan = FALSE;
static BOOL is_first_flag = TRUE;
extern u32 ui_get_dvbc_min_freq(void);
extern u32 ui_get_dvbc_max_freq(void);
extern void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2);
extern BOOL osd_roll_stop_msg_unsend(void);
u16 full_search_cont_keymap(u16 key);

RET_CODE full_search_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 full_search_select_keymap(u16 key);

RET_CODE full_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 full_search_tp_frm_keymap(u16 key);
RET_CODE full_search_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern void set_full_scr_ad_status(BOOL is_show);

static void manual_cancel(void)
{
  UI_PRINTF("MANUAL_SEARCH: cancel scan!\n");

  OS_PRINTF("*************************press ok : %d\n", mtos_ticks_get());
  is_stop = TRUE;
  ui_stop_scan();
}

static void unmanual_cancel(void)
{
  UI_PRINTF("MANUAL_SEARCH: resume scan!\n");
  ui_resume_scan();
  exit_msg = MSG_INVALID;
}

static void init_static_data(void)
{
  u16 i, j, *p_str;
  for (i = 0; i < FSEARCH_LIST_CNT; i++)
  {
    for (j = 0; j < FSEARCH_LIST_PAGE; j++)
    {
      p_str = prog_name_str[i][j];
      prog_name_str_addr[i][j] = p_str;
      p_str[0] = '\0';
    }
  }
  memset(prog_curn, 0, sizeof(prog_curn));

  for (i = 0; i < FSEARCH_PROG_TYPE_CNT; i++)
  {
    last_id[i] = INVALIDID;
  }

  //for (i = 0; i < FSEARCH_TP_LIST_PAGE; i++)
  //{
  //  p_str = tp_no_str[i];
  //  tp_no_str_addr[i] = p_str;
  //  p_str[0] = '\0';

  //  p_str = tp_info_str[i];
  //  tp_info_str_addr[i] = p_str;
  //  p_str[0] = '\0';
  //}
  tp_curn = 0;
}


static void init_prog_list_content(control_t *list)
{
  u8 i = 0;
  u8 list_idx = 0;// = ctrl_get_ctrl_id(list) == IDC_LIST ? 0 : 1;
  u8 asc_str[5] = {0};

  for (i = 0; i < FSEARCH_LIST_PAGE; i++)
  {
    if(prog_curn[list_idx] <= FSEARCH_LIST_PAGE)
    {
      if(i < prog_curn[list_idx])
      {
        sprintf((char *)asc_str, "%.4d", i + 1);
        list_set_field_content_by_ascstr(list, i, 0, asc_str);
      }
      else
      {
        list_set_field_content_by_ascstr(list, i, 0,(u8 *)" ");
      }
    }
    else
    {
      sprintf((char *)asc_str, "%.4d", prog_curn[list_idx] - 4 + i);
      list_set_field_content_by_ascstr(list, i, 0, asc_str);
    }

    
    list_set_field_content_by_extstr(list, i, 1,
                                     prog_name_str_addr[list_idx][i]);
  }
}

static void add_name_to_data(u8 list, u16 *p_str)
{
  u16 i;
  u16 curn = 0;

  if(list >= FSEARCH_LIST_CNT)   //memory overflow protect
  {
      list = FSEARCH_LIST_CNT - 1;
  }
  curn = prog_curn[list] % FSEARCH_LIST_PAGE;

  prog_name_str[list][curn][0] = '\0';
  uni_strcat(prog_name_str[list][curn], p_str, PROG_NAME_STRLEN);
  prog_name_str_addr[list][curn] = prog_name_str[list][curn];

  if (prog_curn[list] >= FSEARCH_LIST_PAGE)
  {
    for (i = 0; i < FSEARCH_LIST_PAGE; i++)
    {
      prog_name_str_addr[list][i] =
        prog_name_str[list][(prog_curn[list] + i + 1) % FSEARCH_LIST_PAGE];
    }
  }

  prog_curn[list]++;
}

static void add_prog_to_list(control_t *cont, dvbs_prog_node_t *p_pg_info)
{
  control_t *list;
  dvbs_prog_node_t pg;
  u16 content[PROG_NAME_STRLEN + 1];
  //u8 idx =0;
  static u8 total_prog_num = 0;

  total_prog_num++;

  memcpy(&pg, p_pg_info, sizeof(dvbs_prog_node_t));

  ui_dbase_get_full_prog_name(&pg, content, PROG_NAME_STRLEN);
  add_name_to_data(0, content);

  if(pg.service_type == SVC_TYPE_TV)
  {
    last_id[PROG_TYPE_TV] = (u16)pg.id;
  }
  else if(pg.service_type == SVC_TYPE_RADIO)
  {
    last_id[PROG_TYPE_RADIO] = (u16)pg.id;
  }
  
  list = ctrl_get_child_by_id(cont, IDC_LIST);
  init_prog_list_content(list);

  ctrl_paint_ctrl(list, TRUE);
}


static void reset_last_prog(void)
{
  u16 prog_pos = 0;
  u16 prog_id = 0;
  u8 mode, view_id;
  
  sys_status_check_group();

  mode = sys_status_get_curn_prog_mode();

  if (mode != CURN_MODE_NONE)
  {
    //to set last prog as curn prog
    if(last_id[PROG_TYPE_TV] != INVALIDID)
    {
      view_id = db_dvbs_create_view(DB_DVBS_ALL_TV, 0, NULL);
      prog_id = db_dvbs_get_id_by_view_pos(view_id, prog_pos);
      sys_status_set_group_all_info(CURN_MODE_TV, prog_id, prog_pos);
    }

    if (last_id[PROG_TYPE_RADIO] != INVALIDID)
    {
      view_id = db_dvbs_create_view(DB_DVBS_ALL_RADIO, 0, NULL);
      prog_id = db_dvbs_get_id_by_view_pos(view_id, prog_pos);
      sys_status_set_group_all_info(CURN_MODE_RADIO, prog_id, prog_pos);
    }
    
    // set curn group to group_all
    sys_status_set_curn_group(0);
    view_id = db_dvbs_create_view(
      (mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);
  }

  // save
  sys_status_save();  
}

static void update_progress(control_t *cont, u16 progress)
{
  //control_t *searching_frm = ctrl_get_child_by_id(cont, IDC_FSEARCHING_INFO_FRM);
  //control_t *bar = ctrl_get_child_by_id(searching_frm, IDC_FSEARCH_BAR);

  //if(ctrl_get_attr(bar) != OBJ_ATTR_HIDDEN)
  //{
  //  UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
  //  ui_comm_bar_update(bar, progress, TRUE);
  //  ui_comm_bar_paint(bar, TRUE);
  //}
}

/*static void get_finish_str(u16 *str, u16 max_len)
{
  u16 uni_str[10], len;

  if (prog_curn[0] > 0 || prog_curn[1] > 0)
  {
    len = 0, str[0] = '\0';
    gui_get_string(IDS_SEARCH_IS_END, str, max_len);

    convert_i_to_dec_str(uni_str, prog_curn[0]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_N_TV, &str[len], (u16)(max_len - len));

    convert_i_to_dec_str(uni_str, prog_curn[1]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_N_RADIO, &str[len], (u16)(max_len - len));
  }
  else
  {
    gui_get_string(IDS_NO_PROG_FOUND, str, max_len);
  }
}*/


static void process_finish(void)
{
/*
  comm_dlg_data_t dlg_data = //popup dialog data
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW| DLG_STR_MODE_STATIC,
    DLG_STR_MODE_NULL,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W,COMM_DLG_H,
    IDS_SEARCH_FINISHED,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    1000,
  };
 */
  //u16 content[64 + 1];

  //get_finish_str(content, 64);
  //dlg_data.content = (u32)content;
  
  is_stop = TRUE;

  // already popup a dlg for ask, force close it
  if (is_ask_for_cancel)
  {
    // resume scan
    ui_resume_scan();
  }

 // ui_comm_dlg_open(&dlg_data);

}

/*static void full_search_pre_open(void)
{
  m_svc_t *p_this = class_get_handle_by_id(M_SVC_CLASS_ID);
  OS_PRINTF("manual scan pre open\n");
 
  //stop monitor svc
   if(NULL  != p_this)
    {
        p_this->stop(p_this);
    }
   
  //to disable book remind.
  ui_set_book_flag(FALSE);
  OS_PRINTF("set book flag\n");

  // disable pb
  ui_enable_playback(FALSE);

  ui_set_front_panel_by_str("----");
  OS_PRINTF("set front panel\n");

  // stop epg 
  init_static_data();
  OS_PRINTF("init static data\n");

  // set flag
  is_stop = is_finished = TRUE;

  if(sys_status_get_curn_group_curn_prog_id() == INVALIDID)
  {
    is_db_empty = TRUE;
  }
  else
  {
    is_db_empty = FALSE;
  }
  
  ui_init_scan();
  OS_PRINTF("init scan\n");
}*/


static void full_search_post_close(void)
{
  /* set callback func */
  UI_PRINTF("full_search_post_close: reset the desktop notify func\n");

  //to enable book remind.
  ui_set_book_flag(TRUE);

//  ui_signal_force_lock(TRUE);

  /* delete the local view */

  /* chk, if need sort on autoscan mode */

  /* recheck group info*/

  //ui_dbase_reset_last_prog(is_db_empty);
  reset_last_prog();

  ui_set_front_panel_by_str("----");

  ui_epg_init();
  ui_epg_start();
  
  /* restart epg in ui_play_channel */
  ui_release_scan();
  
  /* restart monitor */
//  ui_simon_init();
//  ui_simon_start();

  // disable pb
  ui_enable_playback(TRUE);
  //resend osd roll over msg when search finished
  if(osd_roll_stop_msg_unsend())
  {
    ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
  }
  is_stop = is_finished = TRUE;
}

BOOL full_search_is_finish(void)
{
  return is_finished;
}
extern void add_dvbc_all_tp_yj(void);

static RET_CODE on_start_scan(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl, *p_result;
  u32 freq_start, freq_end;
  u32 sym;
  u8 demod;
  dvbs_tp_node_t tp = {0};
  u8 i;
  if(!is_finished)
  {
    return SUCCESS;
  }

  //do_search_pre_open();
  //ctrl_set_attr(cont, OBJ_ATTR_INACTIVE);
  
  p_ctrl = ctrl_get_child_by_id(cont->p_parent, IDC_SIGNAL_INFO_FRM);
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_FSEARCH_TP_RESULT);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FSEARCH_SET_FREQ);
  freq_start = ui_comm_numedit_get_num(p_ctrl);
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_FSEARCH_SET_FREQ_END);
  freq_end = ui_comm_numedit_get_num(p_ctrl);
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_FSEARCH_SET_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FSEARCH_SET_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);

  //tp.freq = freq;
  tp.sym = sym;
  
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
  
  if(freq_start < freq_end)
  {
    ui_scan_param_init();
    for(i =0; i< DVBC_TP_ALL_COUNT_YJ; i++)
    {
      if( g_DVBCFreqTableYj[i] >= freq_start && g_DVBCFreqTableYj[i] <= freq_end)
      {
        tp.freq = g_DVBCFreqTableYj[i];
        ui_scan_param_add_tp(&tp);
      }
    }
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, 
      FALSE, NIT_SCAN_WITHOUT, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
    // start scan
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_RANGE, 0);
  }
  else if(freq_start == freq_end)
  {
    ui_scan_param_init();
    for(i =0; i< DVBC_TP_ALL_COUNT_YJ; i++)
    {
      if(g_DVBCFreqTableYj[i] == freq_start)
      {
        tp.freq = freq_start;
        ui_scan_param_add_tp(&tp);
      }
    }
    ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, 
      FALSE, NIT_SCAN_WITHOUT, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
    // start scan
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_RANGE, 0);
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_CA_PARAME_SET_ERROR, NULL, 1000); 
  }
  return SUCCESS;
}

static RET_CODE search_start_check_sig(control_t *p_cont)
{
  control_t *p_ctrl_info_frm, *p_ctrl;
  dvbs_tp_node_t tp = {0};
  u8 demod;

  p_ctrl_info_frm = ctrl_get_child_by_id(p_cont, IDC_FSEARCH_SET_CONT);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_FSEARCH_SET_FREQ);
  tp.freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_FSEARCH_SET_SYM);
  tp.sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, IDC_FSEARCH_SET_DEMOD);
  demod = ui_comm_select_get_focus(p_ctrl);
  
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

RET_CODE open_full_search(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_list, *p_bar, *p_channel_title, *p_search_type, *p_ad_win;
  control_t *p_ctrl, *p_ctrl_item[FSEARCH_SET_ITEM_CNT], *p_searching_frm, *p_specify_tp_frm;
  list_xstyle_t rstyle =
  { RSI_FSEARCH_ITEM, RSI_FSEARCH_ITEM,
    RSI_FSEARCH_ITEM, RSI_FSEARCH_ITEM,
    RSI_FSEARCH_ITEM };
  list_xstyle_t fstyle =
  { FSI_FSEARCH_LIST_ITEM, FSI_FSEARCH_LIST_ITEM,
    FSI_FSEARCH_LIST_ITEM, FSI_FSEARCH_LIST_ITEM, 
    FSI_FSEARCH_LIST_ITEM };
  u16 i,y;
  u16 stxt[FSEARCH_SET_ITEM_CNT] =
  { IDS_START_FREQ, IDS_END_FREQ, IDS_SYMBOL, IDS_DEMODUL};
  dvbc_lock_t tmp_maintp = {0};
  u16 item_bar_y = 0;
  u16 pbar_y = 0;
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SYSTEM_SET, RSI_TITLE_BG};
  comm_help_data_t search_help_data = //help bar data
  {
    2,
    2,
    {
      IDS_BACK,
      IDS_EXCUTE,
    },
    { 
      IM_ICON_BACK,
      IM_ICON_SELECT,
    },
  };

  // before open
  is_stop = TRUE;
  is_init_scan = FALSE;
  exit_msg = MSG_INVALID;
  seach_intensity = 1;
  seach_quality = 1;
  search_ber = 1.0;
  is_first_flag = TRUE;
  sys_status_reset_channel_and_nit_version(); /***make sure next start up can auto scan*****/
  init_static_data();
  DEBUG(SEARCH,INFO,"open_full_search now.\n");

  sys_status_get_main_tp1(&tmp_maintp);

  p_cont = ui_comm_root_create_full_screen(ROOT_ID_FULL_SEARCH,
                                      RSI_SN_BG,
                                      FSEARCH_MENU_X, FSEARCH_MENU_Y,
                                      FSEARCH_MENU_W, FSEARCH_MENU_H,
                                      &title_data);
  
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, full_search_cont_keymap);
  ctrl_set_proc(p_cont, full_search_cont_proc);

  
  //advertisement window
  p_ad_win = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_FSEARCH_AD,
                              FSEARCH_AD_X, FSEARCH_AD_Y,
                              FSEARCH_AD_W, FSEARCH_AD_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ad_win, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN);
  bmp_pic_draw(p_ad_win);

  //search result:channel title
  p_channel_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CHANNEL_TITLE,
                              FSEARCH_CH_TITLE_X, FSEARCH_CH_TITLE_Y,
                              FSEARCH_CH_TITLE_W, FSEARCH_CH_TITLE_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_channel_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_font_style(p_channel_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_channel_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_channel_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_channel_title, IDS_CHANNEL);

  //search result:channel list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, (u8)IDC_LIST,
                            FSEARCH_LIST_X, FSEARCH_LIST_Y,
                            FSEARCH_LIST_W, FSEARCH_LIST_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST);
  list_set_count(p_list, FSEARCH_LIST_PAGE, FSEARCH_LIST_PAGE);
  list_set_mid_rect(p_list, 4, 4,
                    FSEARCH_LIST_W - 8,
                    FSEARCH_LIST_H - 8,
                    FSEARCH_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &rstyle);
  list_set_field_count(p_list, FSEARCH_LIST_FIELD_CNT, FSEARCH_LIST_PAGE);
  list_set_field_attr(p_list, 0,
                    STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_UNISTR,
                    60, 0, 0);
  list_set_field_attr(p_list, 1,
                    STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      FSEARCH_LIST_W - 68, 60, 0);
  list_set_field_font_style(p_list, 0, &fstyle);
  list_set_field_font_style(p_list, 1, &fstyle);

  init_prog_list_content(p_list);

  // sccond title
  p_search_type = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SECOND_TITLE,
                              FSEARCH_SECOND_TITLE_X, FSEARCH_SECOND_TITLE_Y,
                              FSEARCH_SECOND_TITLE_W, FSEARCH_SECOND_TITLE_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_search_type, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_font_style(p_search_type, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_search_type, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_search_type, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_search_type, IDS_FULL_SEARCH);


  //search param set: cont
  p_specify_tp_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FSEARCH_SET_CONT,
                            FSEARCH_SET_CONT_X, FSEARCH_SET_CONT_Y,
                            FSEARCH_SET_CONT_W, FSEARCH_SET_CONT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST);
  ctrl_set_keymap(p_specify_tp_frm, full_search_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, full_search_tp_frm_proc);

  //search param set: item
  y = FSEARCH_SET_ITEM_VGAP;

  for (i = 0; i < FSEARCH_SET_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl_item[i] = ui_comm_numedit_create_withposix(p_specify_tp_frm, (u8)(IDC_FSEARCH_SET_FREQ + i),
                                             FSEARCH_SET_ITEM_X, y, FSEARCH_SET_ITEM_LW,
                                             FSEARCH_SET_ITEM_MW, FSEARCH_SET_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_numedit_set_posix_txt(p_ctrl_item[i],
                                      (i == 2) ? IDS_UNIT_KBAUD : IDS_UNIT_KHZ);
        ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
        break;
      case 3:
        p_ctrl_item[i] = ui_comm_select_create(p_specify_tp_frm, (u8)(IDC_FSEARCH_SET_FREQ + i),
                                            FSEARCH_SET_ITEM_X, y,
                                            FSEARCH_SET_ITEM_LW,
                                            FSEARCH_SET_ITEM_MW);
        ui_comm_ctrl_set_proc(p_ctrl_item[i], full_search_select_proc);
        ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_NEW_PSW, RSI_OLD_PSW, RSI_NEW_PSW);
        ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
        ui_comm_select_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl_item[i], TRUE,
                                   CBOX_WORKMODE_STATIC, 5,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);
        ui_comm_select_set_content(p_ctrl_item[i], 0, IDS_QAM16);
        ui_comm_select_set_content(p_ctrl_item[i], 1, IDS_QAM32);
        ui_comm_select_set_content(p_ctrl_item[i], 2, IDS_QAM64);
        ui_comm_select_set_content(p_ctrl_item[i], 3, IDS_QAM128);
        ui_comm_select_set_content(p_ctrl_item[i], 4, IDS_QAM256);
        ui_comm_select_set_focus(p_ctrl_item[i], tmp_maintp.nim_modulate - 4);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);

    y += (FSEARCH_SET_ITEM_H + FSEARCH_SET_ITEM_VGAP);
  }

  ctrl_set_related_id(p_ctrl_item[0],
                      0, IDC_FSEARCH_SET_DEMOD, 0, IDC_FSEARCH_SET_FREQ_END);   //FREQ
  ctrl_set_related_id(p_ctrl_item[1],
                      0, IDC_FSEARCH_SET_FREQ, 0, IDC_FSEARCH_SET_SYM);  //SYMB
  ctrl_set_related_id(p_ctrl_item[2],
                      0, IDC_FSEARCH_SET_FREQ_END, 0, IDC_FSEARCH_SET_DEMOD);   //DEMOD
  ctrl_set_related_id(p_ctrl_item[3],
                      0, IDC_FSEARCH_SET_SYM, 0, IDC_FSEARCH_SET_FREQ);   //DEMOD

  ui_comm_numedit_set_param(p_ctrl_item[0], NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[0], tmp_maintp.tp_freq);
  
  ui_comm_numedit_set_param(p_ctrl_item[1], NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[1], tmp_maintp.tp_freq);

  ui_comm_numedit_set_param(p_ctrl_item[2], NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[2], tmp_maintp.tp_sym);

  //searching info: cont
  p_searching_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            FSEARCH_INFO_CONT_X, FSEARCH_INFO_CONT_Y,
                            FSEARCH_INFO_CONT_W, FSEARCH_INFO_CONT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_searching_frm, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST, RSI_FSEARCH_LIST);

  //searching info:prompt string
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_FSEARCH_TP_RESULT,
                            FSEARCH_INFO_ITEM_X, FSEARCH_INFO_ITEM_Y,
                            FSEARCH_INFO_ITEM_W, FSEARCH_INFO_ITEM_H,
                            p_searching_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_FSEARCH_ITEM, RSI_FSEARCH_ITEM, RSI_FSEARCH_ITEM);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  
  
  // signal strength pbar  
  item_bar_y = F_STATUS_BAR1_Y;
  pbar_y = item_bar_y + (F_STATUS_BAR_H - F_STATUS_BAR_MH)/2;  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_FSEARCH_STRENGTH_BAR,
                       F_STATUS_BAR_MX, pbar_y, F_STATUS_BAR_MW, F_STATUS_BAR_MH,
                       F_STATUS_BAR_LX, item_bar_y, F_STATUS_BAR_LW, F_STATUS_BAR_H,
                       F_STATUS_BAR_RX, item_bar_y,F_STATUS_BAR_RW, F_STATUS_BAR_H);
  ui_comm_bar_set_param(p_bar, IDS_STRENGTH, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_FSEARCH_BAR_BG, RSI_FSEARCH_BAR_MID,
                          RSI_IGNORE, FSI_COMM_TXT_N,
                          RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  ui_comm_intensity_bar_update(p_bar, seach_intensity,0, TRUE);

  // signal SNR pbar  
  item_bar_y += F_STATUS_BAR_H + FSEARCH_V_GAP;
  pbar_y = item_bar_y + (F_STATUS_BAR_H - F_STATUS_BAR_MH)/2;  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_FSEARCH_SNR_BAR,
                       F_STATUS_BAR_MX, pbar_y, F_STATUS_BAR_MW, F_STATUS_BAR_MH,
                       F_STATUS_BAR_LX, item_bar_y, F_STATUS_BAR_LW, F_STATUS_BAR_H,
                       F_STATUS_BAR_RX, item_bar_y,F_STATUS_BAR_RW, F_STATUS_BAR_H);
  ui_comm_bar_set_param(p_bar, IDS_SNR, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_FSEARCH_BAR_BG, RSI_FSEARCH_BAR_MID,
                          RSI_IGNORE, FSI_COMM_TXT_N,
                          RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  ui_comm_snr_bar_update(p_bar, seach_quality,0,TRUE);

    // signal BER pbar  
  item_bar_y += F_STATUS_BAR_H + FSEARCH_V_GAP;
  pbar_y = item_bar_y + (F_STATUS_BAR_H - F_STATUS_BAR_MH)/2;
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_FSEARCH_BER_BAR,
                       F_STATUS_BAR_MX, pbar_y, 0, F_STATUS_BAR_MH,
                       F_STATUS_BAR_LX, item_bar_y, F_STATUS_BAR_LW, F_STATUS_BAR_H,
                       F_STATUS_BAR_LX+F_STATUS_BAR_LW, item_bar_y,F_STATUS_BAR_RW+40, F_STATUS_BAR_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_FSEARCH_BAR_BG, RSI_FSEARCH_BAR_MID,
                          RSI_IGNORE, FSI_COMM_TXT_N,
                          RSI_SUB_BUTTON_SH, FSI_COMM_TXT_N);
  ui_comm_ber_string_update(p_bar, 0, TRUE);
  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ui_enable_signal_monitor(TRUE);

  search_start_check_sig(p_cont);
  ui_comm_help_create(&search_help_data, p_cont);
  
  ctrl_default_proc(p_ctrl_item[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}

static RET_CODE on_pg_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> add prog\n");
  add_prog_to_list(cont, (dvbs_prog_node_t *)para1);
  return SUCCESS;
}

static RET_CODE on_tp_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> add/change tp\n");
  //show_searching_tp(cont, (dvbs_tp_node_t *)para1);
  return SUCCESS;
}

static RET_CODE on_nit_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> nit found [0x%x]\n", para2);
  //sys_status_set_nit_version(para2);
  return SUCCESS;
}

static RET_CODE on_update_progress(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> update progress\n");
  update_progress(cont, (u16)para1);
  return SUCCESS;
}



static RET_CODE on_db_is_full(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> dbase full\n");
  // pause scanning, at first
  UI_PRINTF("MANUAL_SEARCH: pause scan!\n");
  ui_pause_scan();
  
  ui_comm_cfmdlg_open2(NULL, IDS_SPACE_IS_FULL, manual_cancel, 0);
  return SUCCESS;
}
static void on_lock_new_tp(control_t *p_cont)
{
  control_t * temp_cont = ctrl_get_child_by_id(p_cont,IDC_FSEARCH_SET_CONT);
  control_t * fre_cont = NULL;
  control_t * sym_cont = NULL;
  control_t * mod_cont = NULL;
  static u32 pre_tp_fre;
  static u32 pre_tp_sym;
  static u8   pre_tp_mod;
  u32 tp_fre;
  u32 tp_sym;
  u32 tp_mod;
  dvbs_tp_node_t tp = {0};
  
  fre_cont   = ctrl_get_child_by_id(temp_cont,IDC_FSEARCH_SET_FREQ);
  sym_cont = ctrl_get_child_by_id(temp_cont,IDC_FSEARCH_SET_SYM);
  mod_cont = ctrl_get_child_by_id(temp_cont,IDC_FSEARCH_SET_DEMOD);
  tp_fre  =  ui_comm_numedit_get_num(fre_cont);
  tp_sym =  ui_comm_numedit_get_num(sym_cont );
  tp_mod =  (u8)ui_comm_select_get_focus(mod_cont);
  if(is_first_flag == TRUE)
  {
    is_first_flag =FALSE;
    pre_tp_fre = tp_fre;
    pre_tp_sym = tp_sym;
    pre_tp_mod  = (u8)tp_mod;
  }
  else if((pre_tp_fre != tp_fre)||(pre_tp_sym  != tp_sym)||(pre_tp_mod != tp_mod)) 
  {
      tp.freq = tp_fre;
      tp.sym  = tp_sym;
      //tp.nim_modulate = tp_mod;
      switch(tp_mod)
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
      pre_tp_fre = tp_fre;
      pre_tp_sym = tp_sym;
      pre_tp_mod  = (u8)tp_mod;
   }

}
static RET_CODE on_signal_search_update(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  
  control_t *p_ctrl;
  control_t *p_strength, *p_ber, *p_snr;
  struct signal_data *data = (struct signal_data *)(para1);
  customer_cfg_t cfg = {0};
  u8 percent_strength = 0;
  u8 percent_snr= 0;
  percent_strength = (data->intensity * 100) / DVB_C_TC2800_STRENGTH_MAX;
  //percent_snr = ((data->quality -snr_min) * 100) / (snr_max -snr_min);
  percent_snr = (data->quality  * 100) / snr_max ;

  get_customer_config(&cfg);
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SIGNAL_INFO_FRM);
  p_strength = ctrl_get_child_by_id(p_ctrl,IDC_FSEARCH_STRENGTH_BAR);
  p_ber = ctrl_get_child_by_id(p_ctrl,IDC_FSEARCH_BER_BAR); 
  p_snr = ctrl_get_child_by_id(p_ctrl,IDC_FSEARCH_SNR_BAR);
  ui_comm_intensity_bar_update(p_strength, (u16)data->intensity,percent_strength, TRUE);
  ui_comm_ber_string_update(p_ber, data->ber_c, TRUE);
  ui_comm_snr_bar_update(p_snr, (u16)data->quality,percent_snr ,TRUE);
  if(seach_intensity != data->intensity)
  {
    ctrl_paint_ctrl(p_strength,TRUE);
    seach_intensity = data->intensity;
  }
  if(seach_quality!= data->quality)
  {
    ctrl_paint_ctrl(p_snr,TRUE);
    seach_quality = data->quality;
  }
   if(search_ber!= data->ber_c)
  {
    ctrl_paint_ctrl(p_ber,TRUE);
    search_ber = data->ber_c;
  }
  on_lock_new_tp(cont);
  
#ifndef WIN32
if(CUSTOMER_YINGJI == cfg.customer) 
{
    void *p_dev = NULL;
    u8 str[5]={0};
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev);
    sprintf((char *)str, "%03d ", seach_quality);
    uio_display(p_dev, str, 4);
}
#endif
  return SUCCESS;
}

static RET_CODE on_finished(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  control_t *p_ctrl;
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SIGNAL_INFO_FRM);
  p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_FSEARCH_TP_RESULT);
  text_set_content_by_strid(p_ctrl, IDS_SAVING);
  ctrl_paint_ctrl(p_ctrl , TRUE);
  ui_enable_signal_monitor(FALSE);
  UI_PRINTF("PROCESS -> finish\n");
  OS_PRINTF("*************************on finished : %d\n", mtos_ticks_get());
  process_finish();
  OS_PRINTF("*************************process finished : %d\n", mtos_ticks_get());
  is_finished = TRUE;
  //back key to submenu
  if((MSG_EXIT == exit_msg) &&(0 == prog_curn[0]))
  {
    //on_save_full_search(cont, msg, para1, para2);
    manage_close_menu(ROOT_ID_FULL_SEARCH, 0, 0);
  }
  else
  {
    ui_close_all_mennus();
  }
  //ui_enable_signal_monitor(FALSE);
  OS_PRINTF("*************************close all menus : %d\n", mtos_ticks_get());
  return SUCCESS;
}

static RET_CODE on_stop_scan(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  if (!is_stop)
  {
    // pause scanning, at first
    UI_PRINTF("MANUAL_SEARCH: pause scan!\n");
    ui_pause_scan();
    is_ask_for_cancel = TRUE;
    // ask for cancel
    ui_comm_ask_for_dodlg_open(NULL, IDS_ASK_FOR_EXIT_SCAN, 
                               manual_cancel, unmanual_cancel, 0);
    is_ask_for_cancel = FALSE;
  }
  else
  {
    // restart epg after finish scan
    if(is_init_scan)
    {
      full_search_post_close();
    }
    manage_close_menu(ROOT_ID_FULL_SEARCH, 0, 0);
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_search_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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


static RET_CODE on_select_msg_start_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  on_start_scan(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_full_search_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_FULL_SEARCH, 0, 0);
  ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  return SUCCESS;
}

BEGIN_KEYMAP(full_search_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(full_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(full_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_SCAN_PG_FOUND, on_pg_found)
  ON_COMMAND(MSG_SCAN_TP_FOUND, on_tp_found)
  ON_COMMAND(MSG_SCAN_PROGRESS, on_update_progress)
  ON_COMMAND(MSG_SCAN_FINISHED, on_finished)
  ON_COMMAND(MSG_STOP_SCAN, on_stop_scan)
  //ON_COMMAND(MSG_EXIT_ALL, on_stop_scan) 
  //ON_COMMAND(MSG_TO_MAINMENU, on_stop_scan_menum)
  ON_COMMAND(MSG_EXIT, on_full_search_exit)
  //ON_COMMAND(MSG_SAVE, on_save_full_search)
  ON_COMMAND(MSG_SCAN_DB_FULL, on_db_is_full)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_search_update)
  ON_COMMAND(MSG_SET_LANG, on_search_switch_lang)
  ON_COMMAND(MSG_SCAN_NIT_FOUND, on_nit_found)
END_MSGPROC(full_search_cont_proc, cont_class_proc)

BEGIN_MSGPROC(full_search_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(full_search_select_proc, cbox_class_proc)

BEGIN_KEYMAP(full_search_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  //ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  //ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_START_SCAN)
END_KEYMAP(full_search_tp_frm_keymap, NULL)

BEGIN_MSGPROC(full_search_tp_frm_proc, cont_class_proc)
  //ON_COMMAND(MSG_EXIT, on_exit_menu)
  ON_COMMAND(MSG_START_SCAN, on_start_scan)
END_MSGPROC(full_search_tp_frm_proc, cont_class_proc)


