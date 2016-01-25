/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_signal_api.h"
#include "ui_manual_search.h"
#include "ui_menu_manager.h"
#include "ui_comm_root.h"

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
  IDC_MSEARCH_AD = 1,
  IDC_SECOND_TITLE,
  IDC_CHANNEL_TITLE,
  IDC_LIST,
  IDC_MSEARCH_SET_CONT,
  IDC_SIGNAL_INFO_FRM,
};

enum msearch_set_item_id
{
  SN_IDC_PATTERN=1,
  SN_IDC_FREQ,
  SN_IDC_SYMBOL,
  SN_IDC_DEMODUL,
  SN_IDC_BMP_LEFT,
  SN_IDC_BMP_RIGHT,
};

enum signal_info_frm_id
{
  IDC_MSEARCH_TP_RESULT = 1,
  IDC_MSEARCH_STRENGTH_BAR,
  IDC_MSEARCH_STREN_NAME,
  IDC_MSEARCH_STREN_BAR,
  IDC_MSEARCH_STREN_PER,
  IDC_MSEARCH_BER_NAME,
  IDC_MSEARCH_BER_BAR,
  IDC_MSEARCH_BER_PER,
  IDC_MSEARCH_SNR_NAME,
  IDC_MSEARCH_SNR_BAR,
  IDC_MSEARCH_SNR_PER,
};

enum search_prog_type_id
{
    PROG_TYPE_TV = 0,
    PROG_TYPE_RADIO,
};


#define PROG_NAME_STRLEN    DB_DVBS_MAX_NAME_LENGTH

static u16 prog_name_str[MSEARCH_LIST_CNT][MSEARCH_LIST_PAGE][
  PROG_NAME_STRLEN + 1];
static u16 *prog_name_str_addr[MSEARCH_LIST_CNT][MSEARCH_LIST_PAGE];
static u16 prog_curn[MSEARCH_LIST_CNT];
static u16 last_id[MSEARCH_PROG_TYPE_CNT] = {0};
static u8 seach_intensity = 0;
static u8 seach_quality = 0;
static double search_ber = 0;
static u8 snr_max = 50;
static u8 snr_min = 0;
static u16 exit_msg = MSG_INVALID;
#define TP_NO_STRLEN      3 /* 999 */
#define TP_INFO_STRLEN    64

static u16 tp_curn;

static BOOL is_stop = FALSE;
static BOOL is_ask_for_cancel = FALSE;
static BOOL is_finished = TRUE;
static BOOL is_init_scan = FALSE;
static BOOL is_first_flag = TRUE;
extern u32 ui_get_dvbc_min_freq(void);
extern u32 ui_get_dvbc_max_freq(void);
extern void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2);
extern BOOL osd_roll_stop_msg_unsend(void);
u16 manual_search_cont_keymap(u16 key);

RET_CODE manual_search_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 manual_search_select_keymap(u16 key);

RET_CODE manual_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 manual_search_tp_frm_keymap(u16 key);
RET_CODE manual_search_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
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
  for (i = 0; i < MSEARCH_LIST_CNT; i++)
  {
    for (j = 0; j < MSEARCH_LIST_PAGE; j++)
    {
      p_str = prog_name_str[i][j];
      prog_name_str_addr[i][j] = p_str;
      p_str[0] = '\0';
    }
  }
  memset(prog_curn, 0, sizeof(prog_curn));

  for (i = 0; i < MSEARCH_PROG_TYPE_CNT; i++)
  {
    last_id[i] = INVALIDID;
  }
  tp_curn = 0;
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
  //control_t *searching_frm = ctrl_get_child_by_id(cont, IDC_MSEARCHING_INFO_FRM);
  //control_t *bar = ctrl_get_child_by_id(searching_frm, IDC_MSEARCH_BAR);

  //if(ctrl_get_attr(bar) != OBJ_ATTR_HIDDEN)
  //{
  //  UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
  //  ui_comm_bar_update(bar, progress, TRUE);
  //  ui_comm_bar_paint(bar, TRUE);
  //}
}

static void process_finish(void)
{  
  is_stop = TRUE;

  // already popup a dlg for ask, force close it
  if (is_ask_for_cancel)
  {
    // resume scan
    ui_resume_scan();
  }
}
static void manual_search_post_close(void)
{
  /* set callback func */
  UI_PRINTF("manual_search_post_close: reset the desktop notify func\n");

  //to enable book remind.
  ui_set_book_flag(TRUE);

//  ui_signal_force_lock(TRUE);

  /* delete the local view */

  /* chk, if need sort on autoscan mode */

  /* recheck group info*/
  reset_last_prog();

  ui_set_front_panel_by_str("----");

  ui_epg_init();
  ui_epg_start();
  
  /* restart epg in ui_play_channel */
  ui_release_scan();

	// disable pb
  ui_enable_playback(TRUE);
  //resend osd roll over msg when search finished
  if(osd_roll_stop_msg_unsend())
  {
    ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
  }
  is_stop = is_finished = TRUE;
}

BOOL manual_search_is_finish(void)
{
  return is_finished;
}

static RET_CODE on_start_scan(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl;
  u8 pattern;
  u32 freq;
  u32 sym;
  u8 demod;
  dvbs_tp_node_t tp = {0};

  DEBUG(SEARCH,INFO,"on_start_scan start\n");
  p_ctrl=ctrl_get_child_by_id(cont, SN_IDC_PATTERN);
  pattern=ui_comm_select_get_focus(p_ctrl);
  
  if(!is_finished)
  {
    return SUCCESS;
  }
  p_ctrl = ctrl_get_child_by_id(cont, SN_IDC_FREQ);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, SN_IDC_SYMBOL);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, SN_IDC_DEMODUL);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);

  tp.freq = freq;
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
  // start scan
  switch(pattern){
	case 0:
	manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, (u32)&tp);
	return SUCCESS;
	break;
	case 1:
	manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_MANUAL, (u32)&tp);
	break;
	case 2:
	manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_FULL, 0);
	return SUCCESS;
	break;
	default:
	MT_ASSERT(0);
	break;
  }
  return SUCCESS;
}

static RET_CODE search_start_check_sig(control_t *p_cont)
{
  control_t *p_ctrl_info_frm, *p_ctrl;
  dvbs_tp_node_t tp = {0};
  u8 demod;

  p_ctrl_info_frm = ctrl_get_child_by_id(p_cont, IDC_MSEARCH_SET_CONT);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, SN_IDC_FREQ);
  tp.freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, SN_IDC_SYMBOL);
  tp.sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_ctrl_info_frm, SN_IDC_DEMODUL);
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

RET_CODE open_manual_search(u32 para1, u32 para2)
{
#define SN_SEARCH_W          	640
#define SN_SEARCH_H          	480
#define SN_SEARCH_ITEM_X		40
#define SN_SEARCH_ITEM_Y		20
#define SN_SEARCH_POSIX			2
#define SN_SEARCH_POSIX_MW		((SN_SEARCH_W-20)/2)
#define SN_SEARCH_SELECT_CNT	3
#define SN_SEARCH_BMAP_W		12

  control_t *p_cont  ;
  control_t *p_ctrl_item[4], *p_specify_tp_frm;
  u16 i,y;
  u16 stxt[SN_SEARCH_SELECT_CNT] =
  {IDS_SPECIFY_FREQ, IDS_MANUL_UPGRADE_SYMBOL, IDS_DEMODUL};
  dvbc_lock_t tmp_maintp = {0};

  help_rsc help_item[15]=
  { 
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_RIGHT},
	 {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_MOVE},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
  };
  static sn_comm_help_data_t sn_submenu_help_data;

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
  DEBUG_ENABLE_MODE(SEARCH,INFO);

  sys_status_get_main_tp1(&tmp_maintp);

  p_cont = ui_background_create(SN_ROOT_ID_SEARCH,0, 0,
								  SN_SEARCH_W, SN_SEARCH_H,
								  IDS_SEARCH,TRUE);
  if (p_cont == NULL)
  {
	  return ERR_FAILURE;    
  }
  ctrl_set_keymap(p_cont, manual_search_cont_keymap);
  ctrl_set_proc(p_cont, manual_search_cont_proc);



  //search param set: cont
  p_specify_tp_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_MSEARCH_SET_CONT,
                            0, 0,
                            SN_SEARCH_W-20, SN_SEARCH_H-102,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_specify_tp_frm, manual_search_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, manual_search_tp_frm_proc);
  
  //search param set: item
  y = 51+SN_SEARCH_ITEM_Y;
  for (i = 0; i < 4; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
        p_ctrl_item[i] = ui_comm_numedit_create_withposix(p_specify_tp_frm, (u8)(SN_IDC_FREQ + i),
                                             SN_SEARCH_ITEM_X, y, 
                                             ((SN_SEARCH_W-100)/3)*2+40,
											((SN_SEARCH_W-100)/3)-20, 
											0);
        ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_LEFT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i],  RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
        break;
      case 2:
        p_ctrl_item[i] = ui_comm_select_create(p_specify_tp_frm, (u8)(SN_IDC_FREQ + i),
					                                            SN_SEARCH_ITEM_X, y,
					                                            ((SN_SEARCH_W-100)/3)*2+40,
					                                            ((SN_SEARCH_W-100)/3)-20);
		ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
        ui_comm_ctrl_set_proc(p_ctrl_item[i], manual_search_select_proc);
        ui_comm_select_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl_item[i], TRUE,CBOX_WORKMODE_STATIC, 5,
					                                   CBOX_ITEM_STRTYPE_STRID,
					                                   NULL);
		ui_comm_select_set_content(p_ctrl_item[i], 0, IDS_QAM16);
        ui_comm_select_set_content(p_ctrl_item[i], 1, IDS_QAM32);
        ui_comm_select_set_content(p_ctrl_item[i], 2, IDS_QAM64);
        ui_comm_select_set_content(p_ctrl_item[i], 3, IDS_QAM128);
        ui_comm_select_set_content(p_ctrl_item[i], 4, IDS_QAM256);
		ui_comm_select_set_focus(p_ctrl_item[i], tmp_maintp.nim_modulate - 4);
        break;
	 case 3:
		p_ctrl_item[3] = ui_comm_select_create(p_specify_tp_frm, (u8)SN_IDC_PATTERN,
																SN_SEARCH_ITEM_X, SN_SEARCH_ITEM_Y,
																(((SN_SEARCH_W-100)/3)*2)+40,
																((SN_SEARCH_W-100)/3-20));
		ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[3], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[3], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[3], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_proc(p_ctrl_item[3], manual_search_select_proc);
		ui_comm_select_set_static_txt(p_ctrl_item[3], IDS_SEARCH_PATTERN);
		ui_comm_select_set_param(p_ctrl_item[3], TRUE,CBOX_WORKMODE_STATIC, 3,
													   CBOX_ITEM_STRTYPE_STRID,
													   NULL);
		ui_comm_select_set_content(p_ctrl_item[3], 0, IDS_AUTO_SEARCH);
		ui_comm_select_set_content(p_ctrl_item[3], 1, IDS_MANUAL_SEARCH);
		ui_comm_select_set_content(p_ctrl_item[3], 2, IDS_FULL_SEARCH);
		ui_comm_select_set_focus(p_ctrl_item[3], 0);
	 break;
      default:
        MT_ASSERT(0);
        break;
    }
	ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT | STL_VCENTER);
	ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT,p_ctrl_item[i], STL_LEFT | STL_VCENTER);
	ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
    y += 51;
  }
  ctrl_set_related_id(p_ctrl_item[3],0, SN_IDC_DEMODUL, 0, SN_IDC_FREQ);	
  ctrl_set_related_id(p_ctrl_item[0],0, SN_IDC_PATTERN, 0, SN_IDC_SYMBOL);  
  ctrl_set_related_id(p_ctrl_item[1],0, SN_IDC_FREQ, 0, SN_IDC_DEMODUL);   
  ctrl_set_related_id(p_ctrl_item[2],0, SN_IDC_SYMBOL, 0, SN_IDC_PATTERN);  

  ui_comm_numedit_set_param(p_ctrl_item[0], NBOX_NUMTYPE_DEC, 
  								ui_get_dvbc_min_freq(), 
  								ui_get_dvbc_max_freq(),
  								6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[0], tmp_maintp.tp_freq);

  ui_comm_numedit_set_param(p_ctrl_item[1], NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[1], tmp_maintp.tp_sym);

  ui_enable_signal_monitor(TRUE);

  search_start_check_sig(p_cont);
  
  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
    sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=15;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_ctrl_item[3], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  return SUCCESS;
}
static RET_CODE on_nit_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> nit found [0x%x]\n", para2);
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
static RET_CODE on_signal_search_update(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  struct signal_data *data = (struct signal_data *)(para1);
  customer_cfg_t cfg = {0};
  u8 percent_strength = 0;
  u8 percent_snr= 0;
  percent_strength = (data->intensity * 100) / DVB_C_TC2800_STRENGTH_MAX;
  percent_snr = (data->quality  * 100) / snr_max ;

  get_customer_config(&cfg);

  return SUCCESS;
}

static RET_CODE on_save_manual_search(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{  
  UI_PRINTF("MANUAL_SEARCH: before MSG_SAVE!\n");
  if (!is_stop) /* cancel check, dvbs_cancel_scan manualn't clear it */
  {
    UI_PRINTF("MANUAL_SEARCH: cancel scan!\n");
    ui_stop_scan();
  }
  OS_PRINTF("*************************manual_search_post_close 1 : %d\n", mtos_ticks_get());
  #ifdef LCN_SWITCH
  manual_search_logic_num_process();
  #endif
  ui_dbase_pg_sort(DB_DVBS_ALL_RADIO);
  ui_dbase_pg_sort(DB_DVBS_ALL_TV);
  if(is_init_scan)
  {
      manual_search_post_close();
  }
  
  if(ui_dbase_check_tvradio(1))
  {
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  }
  else if(ui_dbase_check_tvradio(0))
  {
    ui_dvbc_change_view(DB_DVBS_ALL_RADIO, TRUE);
  }
  else
  {
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  }
  
  OS_PRINTF("*************************manual_search_post_close 2 : %d\n", mtos_ticks_get());
  return SUCCESS;
}

static RET_CODE on_finished(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  DEBUG(SEARCH,INFO,"on_finshed start\n");

  ui_enable_signal_monitor(FALSE);
  UI_PRINTF("PROCESS -> finish\n");
  OS_PRINTF("*************************on finished : %d\n", mtos_ticks_get());
  process_finish();
  OS_PRINTF("*************************process finished : %d\n", mtos_ticks_get());
  is_finished = TRUE;
  //back key to submenu
  if((MSG_EXIT == exit_msg) &&(0 == prog_curn[0]))
  {
    on_save_manual_search(cont, msg, para1, para2);
    manage_close_menu(ROOT_ID_MANUAL_SEARCH, 0, 0);
  }
  else
  {
    ui_close_all_mennus();
  }
  OS_PRINTF("*************************close all menus : %d\n", mtos_ticks_get());
   DEBUG(SEARCH,INFO,"on_finshed end\n");
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
      manual_search_post_close();
    }
    ui_close_all_mennus();
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_stop_scan_menum(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  sys_status_get_curn_prog_mode();
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
    if(is_init_scan)
    {
      manual_search_post_close();
    }
    manage_close_menu(SN_ROOT_ID_SEARCH, 0, 0);
    manage_close_menu(SN_ROOT_ID_SUBMENU, 0, 0);
    ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_stop_scan_exit(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  sys_status_get_curn_prog_mode();
  DEBUG(SEARCH,INFO,"exit search now is_stop %d\n", is_stop);
  if (!is_stop)
  {
    // pause scanning, at first
    UI_PRINTF("MANUAL_SEARCH: pause scan!\n");
    ui_pause_scan();
    is_ask_for_cancel = TRUE;
    exit_msg = msg;
    // ask for cancel
    ui_comm_ask_for_dodlg_open(NULL, IDS_ASK_FOR_EXIT_SCAN, 
                               manual_cancel, unmanual_cancel, 0);
    is_ask_for_cancel = FALSE;
  }
  else
  {
    if(is_init_scan)
    {
      manual_search_post_close();
    }
    manage_close_menu(SN_ROOT_ID_SEARCH, 0, 0);
    ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
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

BEGIN_KEYMAP(manual_search_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_EXIT, MSG_STOP_SCAN)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(manual_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_SCAN_PROGRESS, on_update_progress)
  ON_COMMAND(MSG_SCAN_FINISHED, on_finished)
  ON_COMMAND(MSG_STOP_SCAN, on_stop_scan)
  ON_COMMAND(MSG_EXIT_ALL, on_stop_scan) 
  ON_COMMAND(MSG_TO_MAINMENU, on_stop_scan_menum)
  ON_COMMAND(MSG_EXIT, on_stop_scan_exit)
  //ON_COMMAND(MSG_SAVE, on_save_manual_search)
  ON_COMMAND(MSG_SCAN_DB_FULL, on_db_is_full)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_search_update)
  ON_COMMAND(MSG_SET_LANG, on_search_switch_lang)
  ON_COMMAND(MSG_SCAN_NIT_FOUND, on_nit_found)
END_MSGPROC(manual_search_cont_proc, cont_class_proc)

BEGIN_MSGPROC(manual_search_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(manual_search_select_proc, cbox_class_proc)

BEGIN_KEYMAP(manual_search_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_START_SCAN)
END_KEYMAP(manual_search_tp_frm_keymap, NULL)

BEGIN_MSGPROC(manual_search_tp_frm_proc, cont_class_proc)
  ON_COMMAND(MSG_START_SCAN, on_start_scan)
END_MSGPROC(manual_search_tp_frm_proc, cont_class_proc)



