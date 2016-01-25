/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#include "ui_common.h"
#include "ui_do_search.h"
#include "ui_comm_root.h"
#include "ui_comm_dlg.h"


#ifdef LCN_SWITCH
#include "ui_logic_num_proc.h"
#endif
enum local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 125,
  MSG_CLOSE_ALL,
  MSG_STOP_ADN_BACK,
};

enum control_id
{
//wanghm
  SN_IDC_TV=1,
  SN_IDC_LIST_TV,
  SN_IDC_LIST_RADIO,
  SN_BMAP_BAR,
//wanghm end

  IDC_AD_WINDOW,
  IDC_SECOND_TITLE,
//  IDC_CHANNEL_TITLE,
  IDC_LIST,
//  IDC_MAIN_TP_FRM,
//  IDC_SEARCHING_INFO_FRM,

//enum main_tp_frm_id

  IDC_MAIN_TP_NAME ,
  IDC_MAIN_TP_DETAIL,
  IDC_MAIN_TP_SEARCH_INFO,
 // IDC_MAIN_HZ,
//enum tp_info_frm_id

  IDC_SEARCH_TP_NAME ,
  IDC_SEARCH_TP_DETAIL,
  IDC_SEARCH_PROGRESS_NAME,
  IDC_SEARCH_PBAR,
  IDC_SEARCH_PBAR_PERCENT,
  IDC_SEARCH_TOTAL_TP_NAME,
  IDC_SEARCH_TOTAL_TP_NUM,
  IDC_SEARCH_TOTAL_PROG_NAME,
//  IDC_SEARCH_TOTAL_PROG_NUM,

//enum search_prog_type_id
    PROG_TYPE_TV ,
    PROG_TYPE_RADIO,
};

#define PROG_NAME_STRLEN    DB_DVBS_MAX_NAME_LENGTH

static u16 prog_name_str[DSEARCH_LIST_CNT][DSEARCH_LIST_PAGE][
  PROG_NAME_STRLEN + 1];
static u16 *prog_name_str_addr[DSEARCH_LIST_CNT][DSEARCH_LIST_PAGE];
static u16 prog_curn[DSEARCH_LIST_CNT];

static u16 last_id[DSEARCH_PROG_TYPE_CNT] = {0};
static u16 exit_msg = MSG_INVALID;


#define TP_NO_STRLEN      3 /* 999 */
#define TP_INFO_STRLEN    64

static u16 tp_curn;

static s32 total_prog_num = 0;
static s32 total_tp = 0;
static s32 total_tv_num = 0;
static s32 total_radio_num = 0;

static BOOL is_db_empty = FALSE;
static BOOL is_stop = FALSE;
static BOOL is_ask_for_cancel = FALSE;
static BOOL is_finished = TRUE;

static u8 g_ui_type = 0;
static u8 g_scan_mode = 0;

extern void set_full_scr_ad_status(BOOL is_show);
extern u8 get_channel_init_nit_ver(void);
extern void set_channel_init_nit_ver(u8 nit_version);
extern void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2);
extern BOOL osd_roll_stop_msg_unsend(void);
u16 do_search_cont_keymap(u16 key);
#ifdef LCN_SWITCH
 logic_channel g_logic_ch_data ={0};
 BOOL auto_search_flag = 0;
#endif
RET_CODE do_search_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

void comm_search_prompt_open(void)
{
    control_t *pctrl;
	comm_prompt_data warn_data_t =
	{
	  SN_ROOT_ID_SEARCH_RESULT,
	  0,0,350,97,0,total_radio_num,total_tv_num,
	  IDS_PRO_SEARCH,IDS_SEARCH_RADIO_PROG,
	  0,0,DIALOG_DEADLY,
	  1000
	};
	ui_comm_prompt_search_open(&warn_data_t);
	mtos_task_delay_ms(1000);
  	manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
	pctrl = fw_find_root_by_id(SN_ROOT_ID_SEARCH_RESULT);
	ctrl_paint_ctrl(pctrl,TRUE);
	return;
}

static void do_cancel(void)
{
  UI_PRINTF("DO_SEARCH: cancel scan!\n");
  
  OS_PRINTF("*************************press ok : %d\n", mtos_ticks_get());
  is_stop = TRUE;
  ui_stop_scan();
}


static void undo_cancel(void)
{
  UI_PRINTF("DO_SEARCH: resume scan!\n");
  ui_resume_scan();
  exit_msg = MSG_INVALID;
}

static void init_static_data(void)
{
  u16 i, j, *p_str;
  for (i = 0; i < DSEARCH_LIST_CNT; i++)
  {
    for (j = 0; j < DSEARCH_LIST_PAGE; j++)
    {
      p_str = prog_name_str[i][j];
      prog_name_str_addr[i][j] = p_str;
      p_str[0] = '\0';
    }
  }
  memset(prog_curn, 0, sizeof(prog_curn));

  for (i = 0; i < DSEARCH_PROG_TYPE_CNT; i++)
  {
    last_id[i] = INVALIDID;
  }

   tp_curn = 0;
}


static void init_prog_list_content(control_t *list)
{
  u8 i;
  u8 list_idx = 0;
  u8 asc_str[5] = {0};

  if(NULL==list)
  	DEBUG(SEARCH,ERR,"list==null\n");
  list_idx=(ctrl_get_ctrl_id(list) == SN_IDC_LIST_TV ? 2 : 1);
  for (i = 0; i < DSEARCH_LIST_PAGE; i++)
  {
    if(prog_curn[list_idx] <= DSEARCH_LIST_PAGE)
    {
      if(i < prog_curn[list_idx])
      {
        sprintf((char *)asc_str, "%.4d", i + 1);
        list_set_field_content_by_ascstr(list, i, 0, asc_str);
      }
      else
      {
        list_set_field_content_by_ascstr(list, i, 0,(u8 *) " ");
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

  if(list >= DSEARCH_LIST_CNT)   //memory overflow protect
  {
      list = DSEARCH_LIST_CNT - 1;
  }
  curn = prog_curn[list] % DSEARCH_LIST_PAGE;

  prog_name_str[list][curn][0] = '\0';
  uni_strcat(prog_name_str[list][curn], p_str, PROG_NAME_STRLEN);
  
  prog_name_str_addr[list][curn] = prog_name_str[list][curn];

  if (prog_curn[list] >= DSEARCH_LIST_PAGE)
  {
    for (i = 0; i < DSEARCH_LIST_PAGE; i++)
    {
      prog_name_str_addr[list][i] =
        prog_name_str[list][(prog_curn[list] + i + 1) % DSEARCH_LIST_PAGE];
    }
  }

  prog_curn[list]++;
}

static void add_prog_to_list(control_t *cont, dvbs_prog_node_t *p_pg_info)
{
  control_t *list=NULL;
  dvbs_prog_node_t pg;
  u16 content[PROG_NAME_STRLEN + 1];
  u8 idx =0;

  total_prog_num++;

  memcpy(&pg, p_pg_info, sizeof(dvbs_prog_node_t));

  ui_dbase_get_full_prog_name(&pg, content, PROG_NAME_STRLEN);

  if(pg.service_type == SVC_TYPE_RADIO)
  {
  	idx=1;
    last_id[PROG_TYPE_RADIO] = (u16)pg.id;
	list = ctrl_get_child_by_id(cont, SN_IDC_LIST_RADIO);
	total_radio_num++;
	add_name_to_data(idx, content);
    init_prog_list_content(list);
    ctrl_paint_ctrl(list, TRUE);
  } 
  else if(pg.service_type == SVC_TYPE_TV) 
  {
  	idx=2;
    last_id[PROG_TYPE_TV] = (u16)pg.id;
	list = ctrl_get_child_by_id(cont, SN_IDC_LIST_TV);
	total_tv_num++;
	add_name_to_data(idx, content);
    init_prog_list_content(list);
    ctrl_paint_ctrl(list, TRUE);
  }
  
}

static void show_searching_tp(control_t *cont, dvbs_tp_node_t * p_tp_info, u32 lock_status)
{
  control_t *p_ctrl_freq;
  dvbs_tp_node_t tp;
  
  u8 asc_buf[TP_INFO_STRLEN + 1];
  u16 uni_buf[TP_INFO_STRLEN + 1];

  memcpy(&tp, p_tp_info, sizeof(dvbs_tp_node_t));

  if(lock_status)
  {
    total_tp++;
  }
  
  sprintf((char *)asc_buf, " %.5d KHz", (int)tp.freq);
  str_asc2uni(asc_buf, uni_buf);

 
  //freq
  p_ctrl_freq = ctrl_get_child_by_id(cont, IDC_SEARCH_TP_DETAIL);
  text_set_content_by_unistr(p_ctrl_freq, uni_buf);
   ctrl_paint_ctrl(p_ctrl_freq, TRUE);
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
  control_t *bar = ctrl_get_child_by_id(cont, IDC_SEARCH_PBAR);

  if(ctrl_get_attr(bar) != OBJ_ATTR_HIDDEN)
  {
    UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
    ui_comm_bar_update(bar, progress, TRUE);
    ui_comm_bar_paint(bar, TRUE);
  }
}

 
static void process_finish(void)
{   
  is_stop = TRUE;

   if (is_ask_for_cancel)
  {
    // resume scan
    ui_resume_scan();
  } 
  
}

//用于初始化和去初始化计数，防止多次去初始化的情况发生
static u16 scan_init_deinit_count = 0;
static void do_search_pre_open(void)
{
  m_svc_t *p_this = class_get_handle_by_id(M_SVC_CLASS_ID);
  OS_PRINTF("do scan pre open\n");

  /**stop monitor svc***/
   if(NULL  != p_this)
    {
        p_this->stop(p_this);
    }
   
  //to disable book remind.
  ui_set_book_flag(FALSE);
  OS_PRINTF("set book flag\n");

  /* stop epg */
  OS_PRINTF("\n\nUI_do_search_pre_open !!!!!!!!!!!!!!\n\n");
  ui_epg_stop();      /*stop epg first and disable playback*/
  ui_epg_release();
  set_full_scr_ad_status(FALSE);
  
  // disable pb
  ui_enable_playback(FALSE);

  ui_set_front_panel_by_str("----");
  OS_PRINTF("set front panel\n");

  total_prog_num = 0;  
  total_tp = 0; 
  total_tv_num = 0;
  total_radio_num = 0;
  
  init_static_data();
  OS_PRINTF("init static data\n");

  /* set flag */
  is_stop = is_finished = FALSE;
  exit_msg = MSG_INVALID;

  if(sys_status_get_curn_group_curn_prog_id() == INVALIDID)
  {
    is_db_empty = TRUE;
  }
  else
  {
    is_db_empty = FALSE;
  }
  
  ui_init_scan();
  //增加初始化计数
  scan_init_deinit_count ++;
  OS_PRINTF("init scan count :%d\n",scan_init_deinit_count);
}


static void do_search_post_close(void)
{
  /* set callback func */
  UI_PRINTF("do_search_post_close: reset the desktop notify func\n");
  
  OS_PRINTF("deinit scan count :%d\n",scan_init_deinit_count);
  //检测初始化次数
  if (scan_init_deinit_count == 0)
  	return;
  
  scan_init_deinit_count --;
  OS_PRINTF("deinit!!!!\n",scan_init_deinit_count);

  ui_set_book_flag(TRUE);

  /* delete the local view */

  /* chk, if need sort on autoscan mode */

  /* recheck group info*/

  //ui_dbase_reset_last_prog(is_db_empty);
  reset_last_prog();

  ui_set_front_panel_by_str("----");

  /* restart epg in ui_play_channel */
  ui_release_scan();
  ui_epg_init();
  ui_epg_start();
  /* restart monitor */

  // disable pb
  ui_enable_playback(TRUE);
 //resend osd roll over msg when search finished
  if(osd_roll_stop_msg_unsend())
  {
    ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
  }
  is_stop = is_finished = TRUE;
}

BOOL do_search_is_finish(void)
{
  return is_finished;
}

extern void add_dvbc_all_tp(void);
extern void add_dvbc_all_tp_yj(void);
extern void add_dvbc_all_tp_xinsida_spaish(void);
static void ui_delete_pg_data(void)
{
       /**clear db date***/
    ui_dbase_delete_all_pg();     
    //set default
    db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
    //sys_status_load();
    sys_group_reset();
    sys_status_check_group();
    sys_status_save();
}

RET_CODE open_do_search(u32 para1, u32 para2)
{
#define SN_SEARCH_RESULT_TITILHELP	102
#define SN_SEARCH_RESULT_LEFT_X		10
#define SN_SEARCH_RESULT_Y			30
#define SN_SEARCH_LIST_Y			65
#define SN_SEARCH_LIST_X			40
#define SN_SEARCH_BAR_W				330
#define SN_SEARCH_RESULT_W			640
#define SN_SEARCH_RESULT_H			480
#define SN_SEARCH_RESULT_SECOND_TITLE_H		35
#define SN_SEARCH_BAR_H				180
control_t *p_bar_m=NULL;


  control_t *p_cont, *p_list, *p_bar, *p_channel_title,*p_search_type;// *p_ad_win;
  control_t *p_ctrl;
  list_xstyle_t rstyle =
  { RSI_SN_ALL_BG, RSI_SN_ALL_BG,
    RSI_SN_ALL_BG, RSI_SN_ALL_BG,
    RSI_SN_ALL_BG };
  list_xstyle_t fstyle =
  { FSI_WHITE, FSI_WHITE,
    FSI_WHITE, FSI_WHITE, 
    FSI_WHITE };
  u16 title_strid = RSC_INVALID_ID;
  dvbc_lock_t main_tp = {0};
  dvbs_tp_node_t tp_node_info = {0};
  dvbs_tp_node_t *tp=(dvbs_tp_node_t*)para2;
  u16 i;
  u16 item_y = 0;
  u16 pbar_y = 0;
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  
  g_ui_type = (u8)para1;
  g_scan_mode = (u8)para1;


  p_cont = ui_background_create(SN_ROOT_ID_SEARCH_RESULT,
	                             0, 0,
	                             SN_SEARCH_RESULT_W, 
	                             SN_SEARCH_RESULT_H,
	                             IDS_SEARCH_RESULTS,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_keymap(p_cont, do_search_cont_keymap);
  ctrl_set_proc(p_cont, do_search_cont_proc);

  // before open
  do_search_pre_open();
  sys_status_get_main_tp1(&main_tp);
  switch(g_ui_type)
  {
    case SCAN_TYPE_AUTO:
      #ifdef LCN_SWITCH
      auto_search_flag = 1;
      #endif
      ui_delete_pg_data(); /****delete all pg****/
      sys_status_reset_channel_and_nit_version();/***make sure next start up can auto scan*****/
      title_strid = IDS_AUTO_SEARCH;
      if(cfg.customer == CUSTOMER_YINGJI)
      {
        ui_scan_param_init();
        add_dvbc_all_tp_yj();      
        ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, 
                  FALSE, NIT_SCAN_WITHOUT, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
      }
      else
      {
        ui_scan_param_init();
        if(0 == para2)
		{
     	  tp_node_info.freq = main_tp.tp_freq;
     	  tp_node_info.sym = main_tp.tp_sym;
     	  tp_node_info.nim_modulate = main_tp.nim_modulate;
		}
		else
		{
		  tp_node_info.freq = tp->freq;
		  tp_node_info.sym = tp->sym;
		  tp_node_info.nim_modulate = tp->nim_modulate;
		}
        ui_scan_param_add_tp(&tp_node_info);
       #ifdef LCN_SWITCH_BAT
	ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, 
             FALSE, NIT_SCAN_ONCE, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_ALL_TP);
       #else
        ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, 
                    FALSE, NIT_SCAN_ONCE, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
       #endif
      }
      break;
    case SCAN_TYPE_MANUAL:
	ui_scan_param_init();
	tp_node_info.freq = tp->freq;
    tp_node_info.sym = tp->sym;
    tp_node_info.nim_modulate = tp->nim_modulate;
	
    ui_scan_param_add_tp(&tp_node_info);
	ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, 
				   FALSE, NIT_SCAN_WITHOUT, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
	break;
    case SCAN_TYPE_FULL:
      sys_status_reset_channel_and_nit_version();/***make sure next start up can auto scan*****/
      title_strid = IDS_FULL_SEARCH;
      ui_scan_param_init();
      if(cfg.customer == CUSTOMER_XINSIDA_SPAISH)
        add_dvbc_all_tp_xinsida_spaish(); 
      else
        add_dvbc_all_tp();      
      ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, 
                FALSE, NIT_SCAN_WITHOUT, SCAN_SCRAMBLE_FROM_PMT, CAT_SCAN_ALL_TP,BAT_SCAN_WITHOUT);
      break;
    case SCAN_TYPE_RANGE:
      title_strid = IDS_FULL_SEARCH;
      break;

    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  //TV title
  p_channel_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)SN_IDC_TV,
				                              SN_SEARCH_LIST_X, SN_SEARCH_RESULT_Y,
				                              150, 
				                              SN_SEARCH_RESULT_SECOND_TITLE_H,
				                              p_cont, 0);
  ctrl_set_rstyle(p_channel_title, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
  text_set_font_style(p_channel_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_channel_title, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_channel_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_channel_title, IDS_SEARCH_TV_PROG);

  //search result:channel list
  for(i=0;i<2;i++)
  {
	p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, (u8)SN_IDC_LIST_TV+i,
								SN_SEARCH_LIST_X+(i*(SN_SEARCH_RESULT_W-80+4)/2), 
								SN_SEARCH_LIST_Y,
								((SN_SEARCH_RESULT_W-80-4)/2), 
								SN_SEARCH_BAR_H,
								p_cont, 0);
	ctrl_set_rstyle(p_list, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
	list_set_count(p_list, DSEARCH_LIST_PAGE, DSEARCH_LIST_PAGE);
	list_set_mid_rect(p_list, 4, 4,
	                ((SN_SEARCH_RESULT_W-SN_SEARCH_LIST_X)/2) - 30,
	                (SN_SEARCH_BAR_H-SN_SEARCH_RESULT_SECOND_TITLE_H) + 18,
	                DSEARCH_LIST_ITEM_V_GAP);
	list_set_item_rstyle(p_list, &rstyle);
	list_set_field_count(p_list, DSEARCH_LIST_FIELD_CNT, DSEARCH_LIST_PAGE);
	list_set_field_attr(p_list, 0,
	                STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_UNISTR,
	                60, 0, 0);
	list_set_field_attr(p_list, 1,
	                STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
	                  160, 68, 0);
	list_set_field_font_style(p_list, 0, &fstyle);
	list_set_field_font_style(p_list, 1, &fstyle);

	init_prog_list_content(p_list);
  }

  //middle bars
  p_bar_m = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)SN_BMAP_BAR,
  								(SN_SEARCH_RESULT_W/2-1),
  								SN_SEARCH_LIST_Y,
  								2,SN_SEARCH_BAR_H ,
  								p_cont,0);
  ctrl_set_rstyle(p_bar_m, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL);

  //Radio title
  p_search_type = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SECOND_TITLE,
                              (SN_SEARCH_RESULT_W/2+5)-25, SN_SEARCH_RESULT_Y,
                              150, SN_SEARCH_RESULT_SECOND_TITLE_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_search_type, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
  text_set_font_style(p_search_type, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_search_type, STL_LEFT | STL_VCENTER);
  text_set_offset(p_search_type, 30,0);
  text_set_content_type(p_search_type, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_search_type, IDS_SEARCH_RADIO_PROG);

  //freq text
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_MAIN_TP_NAME,
                            SN_SEARCH_LIST_X, SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+15,
                            DSEARCH_MAIN_TP_NAME_W, DSEARCH_ITEM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_MAIN_FREQ);

  //search status prompt string
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_MAIN_TP_SEARCH_INFO,
                            SN_SEARCH_LIST_X, SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+15+DSEARCH_ITEM_H,
                            1, 1,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

  //searching tp detail
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SEARCH_TP_DETAIL,
                            SN_SEARCH_LIST_X+120, SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+15,
                            DSEARCH_INFO_ITEM_RW, DSEARCH_ITEM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  // pbar  
  item_y = D_STATUS_BAR1_Y;
  pbar_y = item_y + (D_STATUS_BAR_H - D_STATUS_BAR_MH)/2;
  p_bar = ui_comm_bar_create(p_cont, IDC_SEARCH_PBAR,
                       SN_SEARCH_LIST_X+120 , SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+DSEARCH_ITEM_H+20+D_STATUS_BAR_MH/2+5, 
                       SN_SEARCH_BAR_W, D_STATUS_BAR_MH,
                       SN_SEARCH_LIST_X, SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+DSEARCH_ITEM_H+20, 
                       D_STATUS_BAR_LW, D_STATUS_BAR_H,
                       SN_SEARCH_LIST_X+120+SN_SEARCH_BAR_W, SN_SEARCH_LIST_Y+SN_SEARCH_BAR_H+DSEARCH_ITEM_H+20,
                       D_STATUS_BAR_RW, D_STATUS_BAR_H);
  ui_comm_bar_set_param(p_bar, IDS_PROGRESS, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_DSEARCH_PBAR_BG, RSI_DSEARCH_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_SN_ALL_BG, FSI_WHITE);
  ui_comm_bar_update(p_bar, 0, TRUE);

  ui_enable_signal_monitor(TRUE);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifdef LCN_SWITCH
  g_logic_ch_data.log_ch_num = 0;
  g_logic_ch_data.p_logic_channel =(logic_channel_tag*) mtos_malloc(MAX_LCN_NUM*sizeof(logic_channel_tag));
  #endif
  // start scan
  ui_start_scan();

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
  if(ui_get_scan_pause_status() == TRUE)
 {
     return SUCCESS;
 }
  UI_PRINTF("PROCESS -> add/change tp\n");
  show_searching_tp(cont, (dvbs_tp_node_t *)para1,para2);
  return SUCCESS;
}

static RET_CODE on_nit_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> nit found [0x%x]\n", para2);
  set_channel_init_nit_ver((u8)para2);
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
  UI_PRINTF("DO_SEARCH: pause scan!\n");
  ui_pause_scan();
  
  ui_comm_cfmdlg_open2(NULL, IDS_SPACE_IS_FULL, do_cancel, 0);
  return SUCCESS;
}

static RET_CODE on_save_do_search(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("DO_SEARCH: before MSG_SAVE!\n");
  if (!is_stop) /* cancel check, dvbs_cancel_scan don't clear it */
  {
    UI_PRINTF("DO_SEARCH: cancel scan!\n");
    ui_stop_scan();
  }
  OS_PRINTF("*************************do_search_post_close 1 : %d\n", mtos_ticks_get());

#ifdef LCN_SWITCH
  auto_search_flag = 0;
  do_search_logic_num_process(g_logic_ch_data);
  mtos_free( g_logic_ch_data.p_logic_channel);

#endif
  ui_dbase_pg_sort(DB_DVBS_ALL_RADIO);
  ui_dbase_pg_sort(DB_DVBS_ALL_TV);
  do_search_post_close();
  
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

  OS_PRINTF("*************************do_search_post_close 2 : %d\n", mtos_ticks_get());
  return SUCCESS;
}

static RET_CODE on_finished(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  control_t *p_ctrl;

  UI_PRINTF("PROCESS -> finish\n");
  p_ctrl = ctrl_get_child_by_id(cont, IDC_MAIN_TP_SEARCH_INFO);
  text_set_content_by_strid(p_ctrl, IDS_SAVING);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  if(( is_stop == FALSE) && ((g_scan_mode == SCAN_TYPE_AUTO)||(g_scan_mode==SCAN_TYPE_MANUAL)))
  {
     sys_status_set_nit_version((u32)get_channel_init_nit_ver());
  } 
  process_finish();  /***delete entern dlg*****/

   //提前锁频点
  {
	 u16 prog_id;
	 dvbs_prog_node_t pg;
	 dvbs_tp_node_t tp;
	 db_dvbs_ret_t ret;
	 ui_dbase_pg_sort(DB_DVBS_ALL_TV);
	 prog_id = sys_status_get_curn_group_curn_prog_id();
	 if (prog_id == INVALIDID)
 	 {
		dvbc_lock_t main_tp;
		sys_status_get_main_tp1(&main_tp);
		memset(&tp,0,sizeof(tp));
		tp.freq = main_tp.tp_freq;
		tp.sym  = main_tp.tp_sym;
		tp.nim_modulate = main_tp.nim_modulate;
		ui_set_transpond(&tp);
 	 }
	 if (prog_id != INVALIDID)
	 {
		 ret = db_dvbs_get_pg_by_id(prog_id, &pg);
		 MT_ASSERT(DB_DVBS_OK == ret);
		 ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
		 MT_ASSERT(DB_DVBS_OK == ret);
		 ui_set_transpond(&tp);
 	 }
  }	 
  comm_search_prompt_open();
  is_finished = TRUE;
  if((MSG_STOP_ADN_BACK == exit_msg) && (0 == total_prog_num))
  {
    on_save_do_search(cont, msg, para1, para2);
    manage_close_menu(SN_ROOT_ID_SEARCH_RESULT, 0, 0);
  }
  else
  {
    ui_close_all_mennus();
    if((sys_status_get_curn_group_curn_prog_id() == INVALIDID) && (fw_find_root_by_id(SN_ROOT_ID_MAINMENU) == NULL))
    {
      manage_open_menu(SN_ROOT_ID_MAINMENU, 0, 0);
    }
  }
  return SUCCESS;
}
static RET_CODE search_pause(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{ 
	if(msg == MSG_STOP_SCAN)
	{
		dlg_ret_t ret=FALSE;
		comm_dialog_data_t dialog;
		dialog.grade = DIALOG_INFO;
		dialog.x      = 170;
		dialog.y      = 140;
		dialog.w     = 300;
		dialog.h     = 200;
		dialog.parent_root = 0;
		dialog.icon_id = 0;
		dialog.string_mode = STR_MODE_STATIC;
		dialog.text_strID   = IDS_SEARCHING_TO_EXIST;
		dialog.text_content = 0;
		dialog.type   = DLG_FOR_ASK;
		dialog.timeout = 100000;
		if (!is_stop)
		{
			// pause scanning, at first
			UI_PRINTF("DO_SEARCH: pause scan!\n");
			ui_pause_scan();
			is_ask_for_cancel = TRUE;
			exit_msg = msg;
			is_ask_for_cancel = FALSE;
			ret=ui_comm_dialog_open(&dialog);

			if(DLG_RET_YES==ret)
			{

				do_cancel();
				ret = FALSE;
			}
			else
			{
				undo_cancel();
				manage_close_menu(ROOT_ID_DIALOG, 0, 0);
			}
		}
	}
	else if(msg == MSG_STOP_ADN_BACK)
	{
		do_cancel();
	}
	return SUCCESS;
}


BEGIN_KEYMAP(do_search_cont_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_STOP_SCAN)
  ON_EVENT(V_KEY_MENU, MSG_STOP_SCAN)
  ON_EVENT(V_KEY_BACK, MSG_STOP_ADN_BACK)
END_KEYMAP(do_search_cont_keymap, NULL)

BEGIN_MSGPROC(do_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_SCAN_PG_FOUND, on_pg_found)
  ON_COMMAND(MSG_SCAN_TP_FOUND, on_tp_found)
  ON_COMMAND(MSG_SCAN_PROGRESS, on_update_progress)
  ON_COMMAND(MSG_SCAN_FINISHED, on_finished)
  ON_COMMAND(MSG_STOP_SCAN, search_pause)
  ON_COMMAND(MSG_STOP_ADN_BACK, search_pause)
  ON_COMMAND(MSG_SAVE, on_save_do_search)
  ON_COMMAND(MSG_SCAN_DB_FULL, on_db_is_full)
  ON_COMMAND(MSG_SCAN_NIT_FOUND, on_nit_found)
END_MSGPROC(do_search_cont_proc, cont_class_proc)



