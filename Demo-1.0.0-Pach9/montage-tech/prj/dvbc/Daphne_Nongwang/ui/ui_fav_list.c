/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_fav_list.h"
#include "ui_comm_root.h"
#include "ui_signal.h"

#include "sys_dbg.h"
#include "ui_prog_bar.h"

enum fav_list_control_id
{
  IDC_INVALID = 0,
  IDC_FLIST_CONT_ID,
  IDC_FLIST_PGLIST_CONT,
  IDC_FLIST_PGINFO_CONT,
  IDC_FLIST_AD,
  IDC_FLIST_RECT,
  IDC_FLIST_NAME,
  IDC_FLIST_NUMBER,
  IDC_FLIST_TIME,
  IDC_FLIST_EPG_P,
  IDC_FLIST_EPG_F,
  IDC_FLIST_TITLE,
  IDC_FLIST_LIST,
  IDC_FLIST_BAR,

  IDC_SN_FLIST_LIST_NUM,
  IDC_SN_FLIST_LIST_NAME,
  IDC_SN_FLIST_LITTLE,
  IDC_SN_FLIST_BAR_LITTLE_ARROWL,
  IDC_SN_FLIST_BAR_LITTLE_ARROWR,
};

static BOOL is_modified = FALSE;

static list_xstyle_t flist_field_fstyle =
{
  FSI_LIST_TXT_G,
  FSI_LIST_TXT_N,
  FSI_LIST_BTN_HL,
  FSI_LIST_TXT_SEL,
  FSI_LIST_BTN_HL,
};
static list_xstyle_t flist_field_rstyle =
{
  RSI_IGNORE,//RSI_WHITE_YELLOW,
  RSI_IGNORE,//RSI_SUB_BUTTON_SH,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_field_attr_t flist_attr[FLIST_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    40/*28*/, 10/*2*/, 0, &flist_field_rstyle,  &flist_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    60/*50*/, 60/*30*/, 0, &flist_field_rstyle,  &flist_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    200/*FLIST_PGLIST_MIDW - 80*/, 300/*80*/, 0, &flist_field_rstyle,  &flist_field_fstyle },
};
enum fav_msg
{
  MSG_SET_LOVE = MSG_LOCAL_BEGIN + 250,
  MSG_SET_ALL,
};
#if 0
rsc

static comm_help_data_t fav_list_help_data = //help bar data
{
  4,
  4,
  {
    IDS_SELECTED,
    IDS_EXIT,
    IDS_FAV,
    IDS_ALL,
  },
  { 
    IM_ICON_SELECT,
    IM_ICON_EXIT,
    IM_F1,
    IM_F3,
  },
};
#endif
u16 fav_list_cont_keymap(u16 key);
RET_CODE fav_list_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 fav_list_keymap(u16 key);
RET_CODE fav_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


void fav_list_set_modify_state(BOOL state)
{
  is_modified = state;
}


BOOL fav_list_get_modify_state(void)
{
  return is_modified;
}

static void fav_list_do_save_all(void)
{
  u16 view_type;
  u32 group_context;
  BOOL view_del_flag = FALSE;
#if 1
  u16 i, count;
  u8 view_id;

  //to check if current view is favorit view
  sys_status_get_curn_view_info(&view_type, &group_context);
  
  if((view_type == DB_DVBS_FAV_TV) || (view_type == DB_DVBS_FAV_RADIO))
  {
    view_id = ui_dbase_get_pg_view_id();
    count = db_dvbs_get_count(view_id);

    for(i = 0; i < count; i++)
    {
      if( db_dvbs_get_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0) == TRUE)
      {
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0);
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_FAV_GRP, (u16)group_context);
      }
    }    
  }
#endif

  //save your modifications.
  db_dvbs_save(ui_dbase_get_pg_view_id());

  view_del_flag = db_dvbs_get_view_del_flag(ui_dbase_get_pg_view_id());

  sys_status_check_group_view(view_del_flag);

  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);

  //if curn view is invalid, that means no pg saved now, so we should 
  //do nothing about it.
  if(view_type != DB_DVBS_INVALID_VIEW)
  {
    ui_dbase_set_pg_view_id(
      ui_dbase_create_view(view_type, group_context, NULL));
  }

  book_check_node_on_delete();
}

static void fav_list_undo_save_all(void)
{
  u16 view_type;
  u32 group_context;

  //undo modification about view
  db_dvbs_undo(ui_dbase_get_pg_view_id());
  
  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);     
  ui_dbase_set_pg_view_id(
  ui_dbase_create_view(view_type, group_context, NULL));
}

 BOOL fav_list_save_data(void)
{
    u8 org_mode, curn_mode;
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	dialog.grade = DIALOG_INFO;
	dialog.x      = 170;
	dialog.y      = 160;
	dialog.w     = 300;
	dialog.h     = 150;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID   = IDS_SAVING_OR_NOT;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;
	
  org_mode = sys_status_get_curn_prog_mode();

  if (fav_list_get_modify_state())
  {   
      ret=ui_comm_dialog_open(&dialog);

    if(ret == DLG_RET_YES)
    {
      fav_list_do_save_all();
 //  mtos_task_delay_ms(500);
   manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
   manage_close_menu(ROOT_ID_FAV_LIST, 0, 0);
    }
    else
    {
      fav_list_undo_save_all();
	  manage_close_menu(ROOT_ID_FAV_LIST, 0, 0);
    }

    fav_list_set_modify_state(FALSE);
  }


  curn_mode = sys_status_get_curn_prog_mode();
  switch (curn_mode)
    {
      case CURN_MODE_TV:
        ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
        break;
      case CURN_MODE_RADIO:
        ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
        break;
      default:
        ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
    }

  return (org_mode == curn_mode)?TRUE:FALSE;
}

static RET_CODE on_fav_list_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{

  fav_list_save_data();
  update_signal();
  manage_close_menu(ROOT_ID_FAV_LIST, 0, 0);
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) == NULL) 
 {
  	open_prog_bar(0,0);
 }
  return SUCCESS;
}

static RET_CODE on_fav_list_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  fav_list_save_data();
  update_signal();
  manage_close_menu(ROOT_ID_FAV_LIST, 0, 0);
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) == NULL) 
 {
  	open_prog_bar(0,0);
 }
  return SUCCESS;
}

static RET_CODE fav_plist_set_fav(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  u8 view_id;
  u16 focus;
  u16 param;
  u16 FAV0 = 0;
  u16 total = 0, i = 0;
  u16 bmp_id = 0;

  if(0 == list_get_count(p_list))
  {
    return SUCCESS;
  }
  
  fav_list_set_modify_state(TRUE);
  
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
  
  if(msg == MSG_SET_LOVE)
  {
    if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, FAV0) == TRUE)
    {
      param = (FAV0 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 0, 0);
    }
    else
    {
      param = (FAV0 | DB_DVBS_PARAM_ACTIVE_FLAG); 
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 0, IM_SN_ICON_LOVE);//IM_ICON_FAVOURITE
    } 
    
    list_draw_item_ext(p_list, focus, TRUE);
  }
  else if(msg == MSG_SET_ALL)
  {
    total = db_dvbs_get_count(view_id);
    if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, FAV0) == TRUE)
    {
      param = (FAV0 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
      bmp_id = 0;
    }
    else
    {
      param = (FAV0 | DB_DVBS_PARAM_ACTIVE_FLAG);
      bmp_id = IM_SN_ICON_LOVE;//IM_ICON_FAVOURITE
    } 
    
    for(i=0; i<total; i++)
    {
      db_dvbs_change_mark_status(view_id, i, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, i, 0, bmp_id);
    }

    ctrl_paint_ctrl(p_list, TRUE);
  }
  
  return SUCCESS;
}
static void fav_fill_prog_date(control_t *cont, BOOL is_redraw)
{
  u16 prog_pos;
  u8 asc_buf[8];
  evt_node_t *p_evt_node = NULL;
  dvbs_prog_node_t pg;
  u8 asc_str[30] = {0};
  epg_filter_t filter;
  u32 evt_cnt = 0;
  u16 uni_char[40];
  utc_time_t start_time = {0};
  utc_time_t utc_time = {0};
  u16 uni_buf[40];
  BOOL is_epg_get = FALSE;
  u16 curn_group, curn_mode, pg_id, view_pos;
  u32 group_context= 0;
  u8 view_id;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &prog_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  view_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  
  if(db_dvbs_get_count(view_id) <= 0 || db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return;
  }
  
  // set icon
#ifdef LCN_SWITCH
  if(pg.is_scrambled)
    sprintf((char *)asc_buf, "%4d$", pg.logic_ch_num);
  else
    sprintf((char *)asc_buf, "%4d ", pg.logic_ch_num);
#else
  if(pg.is_scrambled)
    sprintf((char *)asc_buf, "%.3d$", view_pos + 1);
  else
    sprintf((char *)asc_buf, "%.3d ", view_pos + 1);
  #endif
  
  // set name
  
  memset(&filter, 0, sizeof(epg_filter_t));
  filter.service_id = (u16)pg.s_id;
  if(pg.orig_net_id == 0)                                        //for asia 3
  {
    filter.stream_id = 0xffff;
    filter.orig_network_id = 0xffff;
  }
  else
  {
    filter.stream_id = (u16)pg.ts_id;
    filter.orig_network_id = (u16)pg.orig_net_id;
  }
  filter.cont_level = 0;
  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &evt_cnt);
  
  if (p_evt_node != NULL)
  {
    time_to_local(&(p_evt_node->start_time), &start_time);
    memcpy(&utc_time, &start_time, sizeof(utc_time_t));
    time_add(&utc_time, &(p_evt_node->drt_time));

    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour, start_time.minute,
                  utc_time.hour, utc_time.minute);
    str_asc2uni(asc_str,uni_char);
    uni_strncpy(uni_buf, p_evt_node->event_name, 31);
    uni_strcat(uni_char, uni_buf,(uni_strlen(uni_char)+uni_strlen(uni_buf)));
    is_epg_get = TRUE;
  }
  else
  {
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 40);
    #endif
    is_epg_get = FALSE;
  }
  
  if (is_epg_get == TRUE && p_evt_node->p_next_evt_node != NULL)
  {
    time_to_local(&(p_evt_node->p_next_evt_node->start_time), &start_time);
    memcpy(&utc_time, &start_time, sizeof(utc_time_t));
    time_add(&utc_time, &(p_evt_node->p_next_evt_node->drt_time));
    memset(uni_buf, 0, sizeof(u16)*32);
    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour,start_time.minute, 
              utc_time.hour, utc_time.minute);
    str_asc2uni(asc_str,uni_char);
    uni_strncpy(uni_buf, p_evt_node->p_next_evt_node->event_name, 31);	
    uni_strcat(uni_char, uni_buf,(uni_strlen(uni_char)+uni_strlen(uni_buf)));
    is_epg_get = TRUE;
  }
  else
  {
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 40);
    #endif
    is_epg_get = FALSE;
  }
}

static void fav_fill_time_date(control_t *p_cont, BOOL is_redraw)
{
  utc_time_t time;
  u8 time_str[50];

  if(ui_get_disable_display_time_status() == TRUE)
  {
    return;
  }
  time_get(&time, FALSE);
  sprintf((char *)time_str, "%.2d/%.2d/%.2d %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
}

static void plist_set_fav_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 im_value[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_SN_ICON_LOVE : 0;//IM_ICON_FAVOURITE 

  list_set_field_content_by_icon(p_list, pos, 0, im_value[0]);
}

//load data from database, will be modified later
static RET_CODE load_fav_data(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, uni_str[32];
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[8];
  dvbs_prog_node_t curn_prog;
  u16 pg_id;

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
      ui_dbase_get_pg_view_id(),(u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &curn_prog);
      
      // NO. 
      #ifdef LCN_SWITCH
      sprintf((char *)asc_str, "%.d ", curn_prog.logic_ch_num);
      #else
      sprintf((char *)asc_str, "%.d ", (u16)(start + i + 1));
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str);
      // NAME 
      ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, uni_str);
      
      /* MARKS */
      plist_set_fav_item_mark(ctrl, (u16)(start + i), pg_id);

      if(pg_id == sys_status_get_curn_group_curn_prog_id())
      {
		list_select_item(ctrl, start + i);

		#ifdef LCN_SWITCH
		ui_set_front_panel_by_num(curn_prog.logic_ch_num);
		#else
		ui_set_front_panel_by_num(start + i + 1);
		#endif
      }      
    }
  }
  return SUCCESS;
}

static RET_CODE on_fav_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;
  u16 pg_id;
  control_t *p_pg_info_cont;

  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);

  if((pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), new_focus)) != INVALIDID)
  {
    ui_play_prog(pg_id, FALSE);
  }

  p_pg_info_cont = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_FLIST_PGINFO_CONT);
  fav_fill_prog_date(p_pg_info_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_fav_list_not_change(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  return SUCCESS;
}

RET_CODE open_fav_list(u32 para1, u32 para2)
{

  //FLIST prog list cont
  #define SN_FLIST_PGLIST_CONT_X                     10
  #define SN_FLIST_PGLIST_CONT_Y                     10
  #define SN_FLIST_PGLIST_CONT_W                     620
  #define SN_FLIST_PGLIST_CONT_H                     300

  //FLIST title
  #define SN_FLIST_TITLE_X                           0
  #define SN_FLIST_TITLE_Y                           0
  #define SN_FLIST_TITLE_W                           SN_FLIST_PGLIST_CONT_W
  #define SN_FLIST_TITLE_H                           40

  //head title:number
  #define SN_FAV_LIST_NUMB_X						 70
  #define SN_FAV_LIST_NUMB_Y						 0
  #define SN_FAV_LIST_NUMB_W						 80
  #define SN_FAV_LIST_NUMB_H						 SN_FLIST_TITLE_H

  //head title:name
  #define SN_FAV_LIST_NAME_X                         SN_FAV_LIST_NUMB_X + SN_FAV_LIST_NUMB_W + 150
  #define SN_FAV_LIST_NAME_Y						 SN_FAV_LIST_NUMB_Y
  #define SN_FAV_LIST_NAME_W                         SN_FAV_LIST_NUMB_W*2
  #define SN_FAV_LIST_NAME_H                         SN_FLIST_TITLE_H


  //fav prog plist
  #define SN_FLIST_PGLIST_X                          0
  #define SN_FLIST_PGLIST_Y                          SN_FLIST_TITLE_H
  #define SN_FLIST_PGLIST_W                          SN_FLIST_PGLIST_CONT_W
  #define SN_FLIST_PGLIST_H                          SN_FLIST_PGLIST_CONT_H - SN_FLIST_TITLE_H

  #define SN_FLIST_PGLIST_MIDL 						 0
  #define SN_FLIST_PGLIST_MIDT						 0
  #define SN_FLIST_PGLIST_MIDW						 SN_FLIST_PGLIST_W - 2*FLIST_PGLIST_MIDL
  #define SN_FLIST_PGLIST_MIDH						 SN_FLIST_PGLIST_H - 2*FLIST_PGLIST_MIDT

  #define SN_FLIST_PGLIST_VGAP                       0

  //bar little
  #define SN_FLIST_LITTLE_X				             SN_FLIST_PGLIST_CONT_X
  #define SN_FLIST_LITTLE_Y				             SN_FLIST_PGLIST_CONT_Y+ SN_FLIST_PGLIST_CONT_H + 20
  #define SN_FLIST_LITTLE_W				             SN_FLIST_PGLIST_CONT_W
  #define SN_FLIST_LITTLE_H				             31

  //bar little arrow
  #define SN_FLIST_BAR_ARROW_LEFT_X		             SN_FLIST_PGLIST_CONT_W/2 - 15
  #define SN_FLIST_BAR_ARROW_RIGHT_X		         SN_FLIST_PGLIST_CONT_W/2 + 15
  #define SN_FLIST_BAR_ARROW_Y 			             (SN_FLIST_LITTLE_H - SN_FLIST_BAR_ARROW_H)/2
  #define SN_FLIST_BAR_ARROW_W 			             15
  #define SN_FLIST_BAR_ARROW_H 			             18
        
  //help bar
  #define SN_SUB_MENU_HELP_RSC_CNT                   16

  

  control_t *p_cont, *p_subctrl_number, *p_subctrl_name;//, *p_mask
  control_t *p_bar_little, *p_bar_little_arrow;
  control_t *p_list, *p_subctrl;//*p_ctrl, 
  control_t *p_pglist_cont = NULL;
  control_t *p_pginfo_cont = NULL;
  u8 view_id, i;
  u16 pg_pos = 0;//y, 
  u16 curn_group, curn_mode, pg_id, view_count;
  u32 group_context;
  
  list_xstyle_t flist_item_rstyle =
  {
    RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_N,
    RSI_SN_BG,//RSI_COMM_LIST_N,
    RSI_SN_BAR_YELLOW_HL,//RSI_COMMON_BTN_HL,
    RSI_SN_BG,
    RSI_SN_BG,//RSI_COMM_LIST_N,
  };
  
  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_EXIT},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F1},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_LOVE},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F3},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_ALL},
  };

#if 0
rsc

  get_customer_config(&cfg);
  switch(cfg.customer)
  {
    case CUSTOMER_XINSHIDA:
    case CUSTOMER_XINSIDA_LQ:
    case CUSTOMER_XINSIDA_SPAISH:
    case CUSTOMER_CHANGJIANG:
    case CUSTOMER_CHANGJIANG_LQ:
    case CUSTOMER_CHANGJIANG_JS:
    case CUSTOMER_CHANGJIANG_QY:
    case CUSTOMER_HEBI:
      fav_list_help_data.bmp_id[3] = IM_F3_2;
      break;
    case CUSTOMER_ZHONGDA:
    case CUSTOMER_FANTONG:
    case CUSTOMER_FANTONG_KF:
    case CUSTOMER_FANTONG_KFAJX:
    case CUSTOMER_FANTONG_KF_SZXC312:
    case CUSTOMER_FANTONG_BYAJX:
    case CUSTOMER_FANTONG_XSMAJX:
      fav_list_help_data.bmp_id[3] = IM_F3_ZD;
      break;
    default:
      break;
  }
#endif
  ui_dvbc_change_view(DB_DVBS_FAV_ALL, FALSE);
  
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  if(0 == view_count)
  {
    switch (curn_mode)
      {
        case CURN_MODE_TV:
          ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
          break;
        case CURN_MODE_RADIO:
          ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
          break;
        default:
          ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
          break;
      }
  }
  
  
  p_cont = ui_background_create(ROOT_ID_FAV_LIST,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_FAV_LIST,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, fav_list_cont_keymap);
  ctrl_set_proc(p_cont, fav_list_cont_proc);
 
  //FLIST prog list cont
  p_pglist_cont= ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_FLIST_PGLIST_CONT,
                             SN_FLIST_PGLIST_CONT_X, SN_FLIST_PGLIST_CONT_Y,
                             SN_FLIST_PGLIST_CONT_W, SN_FLIST_PGLIST_CONT_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_pglist_cont, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG);

  //FLIST title
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_FLIST_TITLE,
                            SN_FLIST_TITLE_X, SN_FLIST_TITLE_Y,
                            SN_FLIST_TITLE_W, SN_FLIST_TITLE_H,
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

  //head title:number
  p_subctrl_number = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_SN_FLIST_LIST_NUM,
                           SN_FAV_LIST_NUMB_X, SN_FAV_LIST_NUMB_Y, 
                           SN_FAV_LIST_NUMB_W,SN_FAV_LIST_NUMB_H, p_subctrl, 0);
  text_set_align_type(p_subctrl_number, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl_number, FSI_INFOBOX_TITLE,FSI_INFOBOX_TITLE,FSI_INFOBOX_TITLE);
  text_set_content_type(p_subctrl_number, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_subctrl_number, IDS_ID);//IDS_CHANNEL
  
  //head title:name
  p_subctrl_name = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_SN_FLIST_LIST_NAME,
                         SN_FAV_LIST_NAME_X, SN_FAV_LIST_NAME_Y, 
                         SN_FAV_LIST_NAME_W,SN_FAV_LIST_NAME_H, p_subctrl, 0);
  text_set_align_type(p_subctrl_name, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl_name, FSI_INFOBOX_TITLE,FSI_INFOBOX_TITLE,FSI_INFOBOX_TITLE);
  text_set_content_type(p_subctrl_name, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_subctrl_name, IDS_CHANNEL_NAME);//IDS_CA_PRO_WF

  //fav prog plist
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_FLIST_LIST,
                            SN_FLIST_PGLIST_X, SN_FLIST_PGLIST_Y,
                            SN_FLIST_PGLIST_W, SN_FLIST_PGLIST_H,
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_list, fav_list_keymap);
  ctrl_set_proc(p_list, fav_list_proc);
  list_set_mid_rect(p_list, SN_FLIST_PGLIST_MIDL, SN_FLIST_PGLIST_MIDT, 
  							SN_FLIST_PGLIST_MIDW, SN_FLIST_PGLIST_MIDH, SN_FLIST_PGLIST_VGAP);//FLIST_PGLIST_VGAP
  list_set_item_rstyle(p_list, &flist_item_rstyle);
  list_set_count(p_list, view_count, FLIST_LIST_PAGE);
  list_set_field_count(p_list, FLIST_LIST_FIELD, FLIST_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  list_set_update(p_list, load_fav_data, 0);
 
  for (i = 0; i < FLIST_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(flist_attr[i].attr), (u16)(flist_attr[i].width),
                        (u16)(flist_attr[i].left), (u8)(flist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, flist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, flist_attr[i].fstyle);
  }
  
 load_fav_data(p_list, list_get_valid_pos(p_list), FLIST_LIST_PAGE, 0);

  //bar little
  p_bar_little = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_SN_FLIST_LITTLE,
                               SN_FLIST_LITTLE_X, SN_FLIST_LITTLE_Y,
                               SN_FLIST_LITTLE_W, SN_FLIST_LITTLE_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_bar_little, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);  

  //bar little arrow
  p_bar_little_arrow = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_SN_FLIST_BAR_LITTLE_ARROWL,
  						  SN_FLIST_BAR_ARROW_LEFT_X, SN_FLIST_BAR_ARROW_Y,
  						  SN_FLIST_BAR_ARROW_W, SN_FLIST_BAR_ARROW_H,
  						  p_bar_little, 0);
  bmap_set_content_by_id(p_bar_little_arrow, IM_SN_ICON_ARROW_UP);
  
  p_bar_little_arrow = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_SN_FLIST_BAR_LITTLE_ARROWR,
  						  SN_FLIST_BAR_ARROW_RIGHT_X, SN_FLIST_BAR_ARROW_Y,
  						  SN_FLIST_BAR_ARROW_W, SN_FLIST_BAR_ARROW_H,
  						  p_bar_little, 0);
  bmap_set_content_by_id(p_bar_little_arrow, IM_SN_ICON_ARROW_DOWN);

  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=55;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  fav_fill_time_date(p_pginfo_cont, FALSE);
  fav_fill_prog_date(p_pginfo_cont, FALSE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif


  fav_list_set_modify_state(FALSE);
  ui_pic_stop();
  ui_play_prog(pg_id, FALSE);
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    ui_stop_play(STOP_PLAY_BLACK,TRUE);
  }

  ui_set_epg_dy();
  return SUCCESS;
}

static RET_CODE on_fav_list_update_epg_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_pginfo_cont = ctrl_get_child_by_id(p_cont, IDC_FLIST_PGINFO_CONT);

  fav_fill_prog_date(p_pginfo_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_fav_list_update_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_pginfo_cont = ctrl_get_child_by_id(p_cont, IDC_FLIST_PGINFO_CONT);

  fav_fill_time_date(p_pginfo_cont, TRUE);
  
  ui_set_epg_dy();
  
  return SUCCESS;
}

static RET_CODE on_fav_list_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(fav_list_cont_keymap, NULL)
END_KEYMAP(fav_list_cont_keymap, NULL)

BEGIN_MSGPROC(fav_list_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_TIME_UPDATE, on_fav_list_update_time)
  ON_COMMAND(MSG_EPG_READY, on_fav_list_update_epg_event)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_fav_list_draw_pic_end)
END_MSGPROC(fav_list_cont_proc, cont_class_proc)

BEGIN_KEYMAP(fav_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_INCREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_DECREASE)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_F1, MSG_SET_LOVE)
  ON_EVENT(V_KEY_RED, MSG_SET_LOVE)
#ifdef CUS_TONGGUANG_HEBI    
  ON_EVENT(V_KEY_F3, MSG_SET_ALL)
  ON_EVENT(V_KEY_YELLOW, MSG_SET_ALL)
#else
  ON_EVENT(V_KEY_F3, MSG_SET_ALL)
  ON_EVENT(V_KEY_GREEN, MSG_SET_ALL)
#endif
  ON_EVENT(V_KEY_FAV,MSG_EXIT_ALL)
#ifdef CUS_TONGGUANG_HEBI  
  ON_EVENT(V_KEY_FAVUP, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_FAVDOWN, MSG_FOCUS_UP)
#endif
END_KEYMAP(fav_list_keymap, NULL)

BEGIN_MSGPROC(fav_list_proc, list_class_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_fav_list_exit_all)
  ON_COMMAND(MSG_SET_LOVE, fav_plist_set_fav)
  ON_COMMAND(MSG_SET_ALL, fav_plist_set_fav)
  ON_COMMAND(MSG_EXIT, on_fav_list_exit)
  ON_COMMAND(MSG_FOCUS_UP, on_fav_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_fav_list_change_focus)
  ON_COMMAND(MSG_INCREASE, on_fav_list_not_change)
  ON_COMMAND(MSG_DECREASE, on_fav_list_not_change)
END_MSGPROC(fav_list_proc, list_class_proc)

