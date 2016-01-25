#include "ui_prog_list.h"
#include "ui_fav_set.h"
#include "ui_play_api.h"
#include "ui_comm_root.h"

#include "sys_dbg.h"
#include "ui_prog_bar.h"
#include "ui_signal.h"

static list_xstyle_t plist_item_rstyle =
{
    RSI_SN_BG,
    RSI_SN_BG,
    RSI_SN_BAR_YELLOW_HL,
    RSI_SN_BAR_BLUE_NORMAL,
    RSI_SN_BAR_YELLOW_HL,
};

static list_xstyle_t plist_field_fstyle =
{
  FSI_INDEX3,
  FSI_INDEX3,
  FSI_INDEX4,
  FSI_INDEX3,
  FSI_INDEX3,
};

static list_xstyle_t plist_field_rstyle =
{
  RSI_IGNORE,//RSI_COMM_LIST_N,
  RSI_IGNORE,//RSI_COMM_LIST_N,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
#ifdef LCN_SWITCH
static list_field_attr_t plist_attr[PLIST_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    28, 2, 0, &plist_field_rstyle,  &plist_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    55, 30, 0, &plist_field_rstyle,  &plist_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    PROG_LIST_MIDW - 85, 85, 0, &plist_field_rstyle,  &plist_field_fstyle },
};
#else
static list_field_attr_t plist_attr[PLIST_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    28, 2, 0, &plist_field_rstyle,  &plist_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    50, 30, 0, &plist_field_rstyle,  &plist_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    SN_PROG_LIST_MIDW - 80/*PROG_LIST_MIDW - 80*/, 80, 0, &plist_field_rstyle,  &plist_field_fstyle },
};
#endif

enum plist_ctrl_id
{
  IDC_INVALID = 0,

  IDC_PROG_PREV,
  IDC_PLIST_CURRENT_PROGRAM,
  IDC_PLIST_FOLLOW_PROGRAM,
  IDC_PLIST_CURRENT_PROGRAM_CONTENT,
  IDC_PLIST_FOLLOW_PROGRAM_CONTENT,

  
  IDC_PLIST_CONT_ID,
  IDC_PLIST_PGLIST_CONT,
  IDC_PLIST_PGINFO_CONT,
  IDC_PLIST_AD,
  IDC_PLIST_RECT,
  IDC_PLIST_NAME,
  IDC_PLIST_NUMBER,
  IDC_PLIST_TIME,
  IDC_PLIST_EPG_P,
  IDC_PLIST_EPG_F,
  IDC_PLIST_GROUP_BG,
  IDC_PLIST_GROUP,
  IDC_PLIST_ARROWL,
  IDC_PLIST_ARROWR,
  IDC_PLIST_LIST,
  IDC_PLIST_BAR,
  
  IDC_PLIST_BAR_ARROWL,
  IDC_PLIST_BAR_ARROWR,
  IDC_BAR_VERTICAL,
  IDC_PRO_ADWIN,
};



enum plist_cont_ctrl_id
{
  IDC_PLCONT_GROUP,
  IDC_PLCONT_MBOX,
  IDC_PLCONT_LIST,
};

enum plist_brief_item_id
{
  IDC_PLBRIEF_SAT = 1,
  IDC_PLBRIEF_PG,
  IDC_PLBRIEF_FREQ,
  IDC_PLBRIEF_PID,
  IDC_PLBRIEF_CA,
};

enum menu_state_t
{
  MS_NOR = 1,
  MS_FAV,
  MS_LOCK,
  MS_SKIP,
  MS_MOVE,
  MS_EDIT_NOR,
  MS_EDIT_SORT,
  MS_EDIT_EDIT,
  MSG_SET_FAV,
};

static BOOL is_modified = FALSE;
static BOOL is_move = FALSE;

comm_dlg_data_t plist_exit_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  DLG_STR_MODE_NULL,
  COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_SAVE_MODIFY,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  0,
};

void plist_set_brief_content(control_t *p_cont, BOOL is_paint);
static RET_CODE plist_update(control_t* ctrl, u16 start, u16 size, u32 context);
static void plist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id);
void plist_set_modify_state(BOOL state);
BOOL plist_get_modify_state(void);

u16 plist_cont_keymap(u16 key);
RET_CODE plist_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 plist_list_keymap(u16 key);
RET_CODE plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void fill_time_date(control_t *p_cont, BOOL is_redraw);
static void fill_prog_date(control_t *p_cont, BOOL is_redraw);


void plist_set_modify_state(BOOL state)
{
  is_modified = state;
}


BOOL plist_get_modify_state(void)
{
  return is_modified;
}


void plist_set_move_state(BOOL state)
{
  is_move = state;
}


BOOL plist_get_move_state(void)
{
  return is_move;
}

static void plist_do_save_all(void)
{
  u16 view_type;
  u32 group_context;
  BOOL view_del_flag = FALSE;
#if 1
  u16 i, count;
  u8 view_id;

  //to check if current view is favorit view
  sys_status_get_curn_view_info(&view_type, &group_context);
  
  DEBUG(PROG_LIST,INFO,"plist_do_save_all.....\n");
  
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

  //sys_status_check_group();

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

static void plist_undo_save_all(void)
{
  u16 view_type;
  u32 group_context;
  
  //undo modification about view
  db_dvbs_undo(ui_dbase_get_pg_view_id());
  
  DEBUG(PROG_LIST,INFO,"plist_undo_save_all.....\n");
   
  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);     
  ui_dbase_set_pg_view_id(
    ui_dbase_create_view(view_type, group_context, NULL));
}

static BOOL plist_save_data(void)
{
  u8 org_mode, curn_mode;
      dlg_ret_t ret=FALSE;
	  //control_t *p_ctrl;
	  comm_dialog_data_t dialog;
	  dialog.grade = DIALOG_INFO;
	  dialog.x		= 170;
	  dialog.y		= 160;
	  dialog.w	   = 300;
	  dialog.h	   = 150;
	  dialog.parent_root = 0;
	  dialog.icon_id = 0;
	  dialog.string_mode = STR_MODE_STATIC;
	  dialog.text_strID   = IDS_SAVING_OR_NOT;
	  dialog.text_content = 0;
	  dialog.type	= DLG_FOR_ASK;
	  dialog.timeout = 100000;
	  

  org_mode = sys_status_get_curn_prog_mode();
  
  
  DEBUG(PROG_LIST,INFO,"plist_save_data.....\n");
  
   if (plist_get_modify_state())
  {
    ret=ui_comm_dialog_open(&dialog);
    plist_exit_data.parent_root = ROOT_ID_PROG_LIST;
       
    if(ret == DLG_RET_YES)
    {
      plist_do_save_all();
      manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
    }
    else
    {
      plist_undo_save_all();
      manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
    }

    plist_set_modify_state(FALSE);
    plist_set_move_state(FALSE);
  }

  curn_mode = sys_status_get_curn_prog_mode();

  return (org_mode == curn_mode)?TRUE:FALSE;
}

void plist_set_brief_content(control_t *p_cont, BOOL is_paint)
{
  control_t *p_list;
  control_t *p_list_cont;
  u16 pg_id, focus;
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PLIST_CONT_ID);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PLCONT_LIST);

  focus = list_get_focus_pos(p_list);
  pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                     focus);
  
  DEBUG(PROG_LIST,INFO,"plist_set_brief_content.....\n");
 

  MT_ASSERT(pg_id != INVALIDID);
  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  if (db_dvbs_get_tp_by_id((u16)(pg.tp_id), &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }
}

static RET_CODE on_plist_update_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_pg_info_cont = ctrl_get_child_by_id(p_cont, IDC_PLIST_PGINFO_CONT);
  
  
  DEBUG(PROG_LIST,INFO,"on_plist_update_time.....\n");
  
   fill_time_date(p_pg_info_cont, TRUE);
  
  ui_set_epg_dy();

  return SUCCESS;
}

static RET_CODE on_plist_update_epg_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_pg_info_cont = ctrl_get_child_by_id(p_cont, IDC_PLIST_PGINFO_CONT);
  
  
  DEBUG(PROG_LIST,INFO,"on_plist_update_epg_event.....\n");
  
   fill_prog_date(p_pg_info_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_plist_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  control_t *p_cont = NULL;
  plist_save_data();
  
  DEBUG(PROG_LIST,INFO,"on_plist_exit.....\n");
  
   switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  if((p_cont = fw_find_root_by_id(ROOT_ID_SIGNAL)) != NULL)
  {
    manage_close_menu(ROOT_ID_SIGNAL, 0, 0);
  }
  if((p_cont = fw_find_root_by_id(ROOT_ID_PASSWORD)) != NULL)
  {
    manage_close_menu(ROOT_ID_PASSWORD, 0, 0);
  }
    ui_pic_stop();

  manage_close_menu(ROOT_ID_PROG_LIST, 0, 0);

  return SUCCESS;
}


static RET_CODE on_plist_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  
  plist_save_data();
  
  DEBUG(PROG_LIST,INFO,"on_plist_exit_all.....\n");
  
   switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  ui_pic_stop();
  ui_close_all_mennus();
 if(fw_find_root_by_id(ROOT_ID_PROG_BAR) == NULL && fw_find_root_by_id(ROOT_ID_EPG) == NULL) 
 {
  	open_prog_bar(0,0);
 }
  return SUCCESS;
}


static void fill_time_date(control_t *p_cont, BOOL is_redraw)
{
  utc_time_t time;
  control_t *p_ctrl = NULL;
  u8 time_str[64];
  
  DEBUG(PROG_LIST,INFO,"fill_time_date.....\n");

  if(ui_get_disable_display_time_status() == TRUE)
  {
    return;
  }

  time_get(&time, FALSE);
  sprintf((char *)time_str, "%.2d/%.2d/%.2d %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
  
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_PLIST_TIME);
}
static void fill_prog_date(control_t *p_cont, BOOL is_redraw)
{
  u16 prog_pos;
  u8 asc_buf[8];
  control_t *p_subctrl;
  evt_node_t *p_evt_node = NULL;
  dvbs_prog_node_t pg;
  u8 asc_str[30] = {0};
  epg_filter_t filter;
  u32 evt_cnt = 0;
  u16 uni_char[64];
  u16 uni_buf[40];
  BOOL is_epg_get = FALSE;
  u16 curn_group, curn_mode, pg_id;
  u32 group_context= 0;
  u8 view_id;
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &prog_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
    
  DEBUG(PROG_LIST,INFO,"fill_prog_date.....\n");
 
  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return;
  }

  // set num
  p_subctrl = ctrl_get_child_by_id(p_cont, IDC_PLIST_NUMBER);
  #ifdef LCN_SWITCH
  if(pg.is_scrambled)
    sprintf((char *)asc_buf, "%.4d$", pg.logic_ch_num);
  else
    sprintf((char *)asc_buf, "%.4d ", pg.logic_ch_num);
  #else
  if(pg.is_scrambled)
    sprintf((char *)asc_buf, "%.3d$", prog_pos + 1);
  else
    sprintf((char *)asc_buf, "%.3d ", prog_pos + 1);
  #endif

  // set name
  p_subctrl = ctrl_get_child_by_id(p_cont, IDC_PLIST_NAME); 

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

  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
  &filter, &evt_cnt);

  if (p_evt_node != NULL)
  {
    time_to_local(&(p_evt_node->start_time), &start_time);
    memcpy(&end_time, &(start_time), sizeof(utc_time_t));
    time_add(&end_time, &(p_evt_node->drt_time));

    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour, start_time.minute,
              end_time.hour, end_time.minute);
    str_asc2uni(asc_str,uni_char);
    uni_strncpy(uni_buf, p_evt_node->event_name, 64);
    uni_strcat(uni_char, uni_buf,(uni_strlen(uni_char)+uni_strlen(uni_buf)));
    is_epg_get = TRUE;
  }
  else
  {
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 64);
    #endif
    is_epg_get = FALSE;
  }

  p_subctrl = ctrl_get_child_by_id(p_cont, IDC_PLIST_EPG_P); 
  text_set_content_by_unistr(p_subctrl, uni_char);

  if (is_epg_get == TRUE && p_evt_node->p_next_evt_node != NULL)
  {
    time_to_local(&(p_evt_node->p_next_evt_node->start_time), &start_time);
    memcpy(&end_time, &(start_time), sizeof(utc_time_t));
    time_add(&end_time, &(p_evt_node->p_next_evt_node->drt_time));
    memset(uni_buf, 0, sizeof(u16)*32);
    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour, 
                start_time.minute,end_time.hour, end_time.minute);
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
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 64);
    #endif
    is_epg_get = FALSE;
  }
  p_subctrl = ctrl_get_child_by_id(p_cont, IDC_PLIST_EPG_F); 
  text_set_content_by_unistr(p_subctrl, uni_char);

  if(is_redraw)
  {
    ctrl_paint_ctrl(p_cont, TRUE);
  }
}


static RET_CODE on_plist_set_fav(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  u8 view_id;
  u16 focus;
  u16 param;
  u16 FAV0 = 0;

  plist_set_modify_state(TRUE);
  
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
     
  if(msg == MSG_SET_FAV)
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
      list_set_field_content_by_icon(p_list, focus, 0, IM_SN_ICON_LOVE);//IM_FAV);
    }
    list_draw_item_ext(p_list, focus, TRUE);
  }
  
  return SUCCESS;
}

static RET_CODE on_prog_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;
  u16 pg_id;
  control_t *p_pg_info_cont;

  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
    
  ui_pic_stop();
  if((pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), new_focus)) != INVALIDID)
  {
    ui_play_prog(pg_id, FALSE);
  }

  p_pg_info_cont = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_PLIST_PGINFO_CONT);
  fill_prog_date(p_pg_info_cont, TRUE);
  fw_notify_root(p_list, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  update_signal();
  return SUCCESS;
}

static RET_CODE plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 pg_id, cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[8];
  u16 uni_str[32]; 

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &pg);

      /* NO. */
      #ifdef LCN_SWITCH
      sprintf((char *)asc_str, "%.4d ", pg.logic_ch_num);
      #else
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str); 

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, uni_str); //pg name

      /* MARKS */
      plist_set_list_item_mark(ctrl, (u16)(start + i), pg_id);

      if(pg_id == sys_status_get_curn_group_curn_prog_id())
      {
        list_select_item(ctrl, start + i);
        #ifdef LCN_SWITCH
        ui_set_front_panel_by_num(pg.logic_ch_num);
        #else
        ui_set_front_panel_by_num(start + i + 1);
        #endif
      }      
    }
  }

  return SUCCESS;
}


static void plist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 im_value[4];
  u8 view_id = ui_dbase_get_pg_view_id();
  im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_SN_ICON_LOVE : 0;

  list_set_field_content_by_icon(p_list, pos, 0, im_value[0]);
}

static RET_CODE group_type_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str, u16 max_length)
{
  u8 type;
  u16 pos, group = 0;
  u32 context;

  group = sys_status_get_curn_group();
  sys_status_get_group_info(group, &type, &pos, &context);
  
  switch(type)
  {
    case GROUP_T_ALL:
      switch(focus)
      {
        case 0://tv
          gui_get_string(IDS_TV, p_str, 31);
          break;
          
        case 1://radiao
          gui_get_string(IDS_RADIO, p_str, 31);
          break;
          
        case 2://all
          gui_get_string(IDS_ALL, p_str, 31);
          break;
      }
      break;
    default:
      MT_ASSERT(0);  
  }
  return SUCCESS;
}   


static RET_CODE on_prog_list_change_group(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cbox, *p_prog_info_cont;
  u8 curn_mode = 0;
  u16 curn_group = 0;
  u16 pg_id, pg_pos = 0xFFFF;
  u32 group_context = 0;
  u8 view_id = 0xFF;
  u16 cbox_focus = 0xFFFF;
  u16 view_count = 0;
  u16 offset = 0;

  plist_save_data();
  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLIST_GROUP);
  p_prog_info_cont = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_PLIST_PGINFO_CONT);

  cbox_class_proc(p_cbox, msg, para1, para2);
  cbox_focus = cbox_static_get_focus(p_cbox);

  switch(cbox_focus)
  {
    case 0: //tv
      ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
      break;

    case 1: //radio
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, TRUE);
      break;

    case 2: //all pg
      ui_dvbc_change_view(DB_DVBS_TV_RADIO, TRUE);
      break;
  }

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  list_set_count(p_list, view_count, PLIST_LIST_PAGE - 3);
  list_set_focus_pos(p_list, pg_pos);
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE - 3, 1);
  #else
  list_set_count(p_list, view_count, PLIST_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 1);
  #endif
  ctrl_paint_ctrl(p_list, TRUE);
  offset = list_get_valid_pos(p_list);
  ui_set_epg_dy_by_offset(offset);
  fill_prog_date(p_prog_info_cont, TRUE);
  return SUCCESS;
}


RET_CODE open_prog_list(u32 para1, u32 para2)
{
  #define SN_SUB_MENU_HELP_RSC_CNT	19
  #define SN_PRO_ADWIN_X			344
  #define SN_PRO_ADWIN_Y			244
  #define SN_PRO_ADWIN_W			250
  #define SN_PRO_ADWIN_H			118
  control_t *p_cont, *p_group, *p_tv_win, *p_bar_vertical, *p_bar_cross;// *p_mask,
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  control_t *p_ad_win;
  control_t *p_list, *p_subctrl;
  #else
  control_t *p_ctrl, *p_list, *p_subctrl, *p_barctrl;
  #endif
  control_t *p_current_program, *p_follow_program;
  control_t *p_pglist_cont = NULL;
  control_t *p_pginfo_cont = NULL;
  u8 view_id, i;
  u16 y, pg_pos = 0;//
  u16 curn_group, curn_mode, pg_id, view_count;
  u32 group_context;
  u16 cbox_focus= 0;
    static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_DOWN},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_LEFT},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_RIGHT},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_EXIT},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_LOVE},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_LOVE}
  };
  customer_cfg_t cus_cfg = {0};
  
  DEBUG(PROG_LIST,INFO,"open_prog_list.....\n");
 

  get_customer_config(&cus_cfg);
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  if(0 == view_count)
  {
      return ERR_FAILURE;
  }
  switch(db_dvbs_get_view_type(view_id))
  {
   case DB_DVBS_ALL_TV:
    cbox_focus=0;
    break;
   case DB_DVBS_ALL_RADIO:
    cbox_focus=1;
    break;
   case DB_DVBS_TV_RADIO:
    cbox_focus=2;
    break;
   default:
    cbox_focus=0;
    break;
  }


  p_cont = ui_background_create(ROOT_ID_PROG_LIST,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_PRO_LIST,TRUE); //IDS_TV_PROGRAM
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, plist_cont_keymap);
  ctrl_set_proc(p_cont, plist_cont_proc);

  //TV preview window
  p_tv_win = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_PROG_PREV,
							SN_PROG_PREV_X, SN_PROG_PREV_Y,
							SN_PROG_PREV_W, SN_PROG_PREV_H,
						p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_SN_TVWIN, RSI_SN_TVWIN, RSI_SN_TVWIN);
  //advertisement window
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  p_ad_win = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_PRO_ADWIN,
							SN_PRO_ADWIN_X, SN_PRO_ADWIN_Y,
							SN_PRO_ADWIN_W, SN_PRO_ADWIN_H,
							p_cont, 0);
  
  ctrl_set_rstyle(p_ad_win, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);
  //bmp_pic_draw(p_ad_win);

  //PLIST prog list cont
  p_pglist_cont= ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_PLIST_PGLIST_CONT,
                             SN_PLIST_PGLIST_CONT_X, SN_PLIST_PGLIST_CONT_Y,
                             SN_PLIST_PGLIST_CONT_W, SN_PLIST_PGLIST_CONT_H - 80,
                             p_cont, 0);
  ctrl_set_rstyle(p_pglist_cont, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG);

  //group name bg
  p_group = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_PLIST_GROUP_BG, 0, 0,
                           SN_PLIST_NAME_BACKGROUND_W, SN_PLIST_NAME_BACKGROUND_H, p_pglist_cont, 0);
  ctrl_set_rstyle(p_group, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  
  //group name text
  p_group = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_PLIST_GROUP, 
                            SN_PLIST_GROUP_NAME_X, SN_PLIST_GROUP_NAME_Y,
                            SN_PLIST_GROUP_NAME_W, SN_PLIST_GROUP_NAME_H, 
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_group, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_PLIST_NAME
  cbox_set_font_style(p_group, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);
  cbox_dync_set_count(p_group, GROUP_COUNT);
  cbox_dync_set_update(p_group, group_type_cbox_update);
  cbox_dync_set_focus(p_group, cbox_focus);
  cbox_set_align_style(p_group, STL_CENTER|STL_VCENTER);
  
  //group name arrow
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_ARROWL,
                            SN_PLIST_ARROW_LEFT_X, SN_PLIST_ARROW_Y,
                            SN_PLIST_ARROW_W, SN_PLIST_ARROW_H,
                            p_pglist_cont, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_LEFT);

  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_ARROWR,
                            SN_PLIST_ARROW_RIGHT_X, SN_PLIST_ARROW_Y,
                            SN_PLIST_ARROW_W, SN_PLIST_ARROW_H,
                            p_pglist_cont, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_RIGHT);

  //bar cross
  p_bar_cross = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BAR_VERTICAL,
                               SN_BAR_CROSS_X, SN_BAR_CROSS_Y,
                               SN_BAR_CROSS_W, SN_BAR_CROSS_H,
                               p_pglist_cont, 0);
  ctrl_set_rstyle(p_bar_cross, RSI_SN_BAR_CROSS, RSI_SN_BAR_CROSS, RSI_SN_BAR_CROSS);


  //prog plist
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_PLIST_LIST,
                            SN_PLIST_PGLIST_X, SN_PLIST_PGLIST_Y,
                            SN_PLIST_PGLIST_W, SN_PLIST_PGLIST_H - 80,
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_list, plist_list_keymap);
  ctrl_set_proc(p_list, plist_list_proc);
  list_set_mid_rect(p_list, SN_PROG_LIST_MIDL, SN_PROG_LIST_MIDT, SN_PROG_LIST_MIDW, SN_PROG_LIST_MIDH - 90, SN_PROG_LIST_VGAP+1);//PROG_LIST_VGAP
  list_set_item_rstyle(p_list, &plist_item_rstyle);
  list_set_count(p_list, view_count, SN_PLIST_LIST_PAGE - 3);//PLIST_LIST_PAGE
  list_set_field_count(p_list, PLIST_LIST_FIELD, SN_PLIST_LIST_PAGE - 3);//PLIST_LIST_PAGE
  list_set_focus_pos(p_list, pg_pos);
  list_set_update(p_list, plist_update, 0);
 
  for (i = 0; i < PLIST_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(plist_attr[i].attr), (u16)(plist_attr[i].width),
                        (u16)(plist_attr[i].left), (u8)(plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, plist_attr[i].fstyle);
  }
  
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE - 3, 0);
  
  //prog info cont
  p_pginfo_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_PLIST_PGINFO_CONT,
                               SN_PLIST_PGINFO_CONT_X, SN_PLIST_PGINFO_CONT_Y,
                               SN_PLIST_PGINFO_CONT_W,SN_PLIST_PGINFO_CONT_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_pginfo_cont, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG);


  //prog info: current program info
  p_current_program = ctrl_create_ctrl((u8 *) CTRL_TEXT, IDC_PLIST_CURRENT_PROGRAM,
  								SN_PLIST_PGINFO_CURRENT_PROGRAM_X, SN_PLIST_PGINFO_CURRENT_PROGRAM_Y,
  								SN_PLIST_PGINFO_CURRENT_PROGRAM_W, SN_PLIST_PGINFO_CURRENT_PROGRAM_H,
  								p_pginfo_cont, 0);
  text_set_font_style(p_current_program, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_current_program, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_current_program, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_current_program, IDS_CURRENT_PROGRAM2);
  
  //prog info: follow program info
  p_follow_program = ctrl_create_ctrl((u8 *) CTRL_TEXT, IDC_PLIST_FOLLOW_PROGRAM,
  								SN_PLIST_PGINFO_FOLLOW_PROGRAM_X, SN_PLIST_PGINFO_FOLLOW_PROGRAM_Y,
  								SN_PLIST_PGINFO_FOLLOW_PROGRAM_W, SN_PLIST_PGINFO_FOLLOW_PROGRAM_H,
  								p_pginfo_cont, 0);
  text_set_font_style(p_follow_program, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_follow_program, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_follow_program, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_follow_program, IDS_NEXT_PROGRAM2);

  //bar vertical
  p_bar_vertical = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BAR_VERTICAL,
                               SN_BAR_VERTICAL_X, SN_BAR_VERTICAL_Y,
                               SN_BAR_VERTICAL_W, SN_BAR_VERTICAL_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_bar_vertical, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL);

  //epg info
  y = SN_PLIST_EPG_INFO_Y;
  for (i = 0; i < PLIST_EPG_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PLIST_EPG_P+ i),
                       SN_PLIST_EPG_INFO_X, y,
                       SN_PLIST_EPG_INFO_W, SN_PLIST_EPG_INFO_H,
                       p_pginfo_cont, 0);
    text_set_align_type(p_subctrl, STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_INDEX3,FSI_INDEX3,FSI_INDEX3);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

    y+= SN_PLIST_EPG_INFO_H + SN_PLIST_EPG_INFO_Y;
  }
  #else
  //PLIST prog list cont
  p_pglist_cont= ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_PLIST_PGLIST_CONT,
                             SN_PLIST_PGLIST_CONT_X, SN_PLIST_PGLIST_CONT_Y,
                             SN_PLIST_PGLIST_CONT_W, SN_PLIST_PGLIST_CONT_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_pglist_cont, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG);

  //group name bg
  p_group = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_PLIST_GROUP_BG, 0, 0,
                           SN_PLIST_NAME_BACKGROUND_W, SN_PLIST_NAME_BACKGROUND_H, p_pglist_cont, 0);
  ctrl_set_rstyle(p_group, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  
  //group name text
  p_group = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_PLIST_GROUP, 
                            SN_PLIST_GROUP_NAME_X, SN_PLIST_GROUP_NAME_Y,
                            SN_PLIST_GROUP_NAME_W, SN_PLIST_GROUP_NAME_H, 
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_group, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_PLIST_NAME
  cbox_set_font_style(p_group, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);
  cbox_dync_set_count(p_group, GROUP_COUNT);
  cbox_dync_set_update(p_group, group_type_cbox_update);
  cbox_dync_set_focus(p_group, cbox_focus);
  cbox_set_align_style(p_group, STL_CENTER|STL_VCENTER);
  //cbox_get_focus_update(p_group);
  
  //group name arrow
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_ARROWL,
                            SN_PLIST_ARROW_LEFT_X, SN_PLIST_ARROW_Y,
                            SN_PLIST_ARROW_W, SN_PLIST_ARROW_H,
                            p_pglist_cont, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_LEFT);

  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_ARROWR,
                            SN_PLIST_ARROW_RIGHT_X, SN_PLIST_ARROW_Y,
                            SN_PLIST_ARROW_W, SN_PLIST_ARROW_H,
                            p_pglist_cont, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_RIGHT);

  //bar cross
  p_bar_cross = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BAR_VERTICAL,
                               SN_BAR_CROSS_X, SN_BAR_CROSS_Y,
                               SN_BAR_CROSS_W, SN_BAR_CROSS_H,
                               p_pglist_cont, 0);
  ctrl_set_rstyle(p_bar_cross, RSI_SN_BAR_CROSS, RSI_SN_BAR_CROSS, RSI_SN_BAR_CROSS);


  //prog plist
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_PLIST_LIST,
                            SN_PLIST_PGLIST_X, SN_PLIST_PGLIST_Y,
                            SN_PLIST_PGLIST_W, SN_PLIST_PGLIST_H,
                            p_pglist_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_list, plist_list_keymap);
  ctrl_set_proc(p_list, plist_list_proc);
  list_set_mid_rect(p_list, SN_PROG_LIST_MIDL, SN_PROG_LIST_MIDT, SN_PROG_LIST_MIDW, SN_PROG_LIST_MIDH, SN_PROG_LIST_VGAP);//PROG_LIST_VGAP
  list_set_item_rstyle(p_list, &plist_item_rstyle);
  list_set_count(p_list, view_count, SN_PLIST_LIST_PAGE);//PLIST_LIST_PAGE
  list_set_field_count(p_list, PLIST_LIST_FIELD, SN_PLIST_LIST_PAGE);//PLIST_LIST_PAGE
  list_set_focus_pos(p_list, pg_pos);
  list_set_update(p_list, plist_update, 0);
 
  for (i = 0; i < PLIST_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(plist_attr[i].attr), (u16)(plist_attr[i].width),
                        (u16)(plist_attr[i].left), (u8)(plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, plist_attr[i].fstyle);
  }
  
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
  

  //bar little
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_AD,
                               SN_PLIST_BAR_LITTLE_X, SN_PLIST_BAR_LITTLE_Y,
                               SN_PLIST_BAR_LITTLE_W, SN_PLIST_BAR_LITTLE_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);  
  //bmp_pic_draw(p_ctrl);

  //bar little arrow
  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_BAR_ARROWL,
                            SN_PLIST_BAR_ARROW_LEFT_X, SN_PLIST_BAR_ARROW_Y,
                            SN_PLIST_BAR_ARROW_W, SN_PLIST_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_UP);

  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_PLIST_BAR_ARROWR,
                            SN_PLIST_BAR_ARROW_RIGHT_X, SN_PLIST_BAR_ARROW_Y,
                            SN_PLIST_BAR_ARROW_W, SN_PLIST_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_DOWN);

  //prog info cont
  p_pginfo_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_PLIST_PGINFO_CONT,
                               SN_PLIST_PGINFO_CONT_X, SN_PLIST_PGINFO_CONT_Y,
                               SN_PLIST_PGINFO_CONT_W,SN_PLIST_PGINFO_CONT_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_pginfo_cont, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG);


  //prog info: current program info
  p_current_program = ctrl_create_ctrl((u8 *) CTRL_TEXT, IDC_PLIST_CURRENT_PROGRAM,
  								SN_PLIST_PGINFO_CURRENT_PROGRAM_X, SN_PLIST_PGINFO_CURRENT_PROGRAM_Y,
  								SN_PLIST_PGINFO_CURRENT_PROGRAM_W, SN_PLIST_PGINFO_CURRENT_PROGRAM_H,
  								p_pginfo_cont, 0);
  text_set_font_style(p_current_program, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_current_program, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_current_program, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_current_program, IDS_CURRENT_PROGRAM2);
  
  //prog info: follow program info
  p_follow_program = ctrl_create_ctrl((u8 *) CTRL_TEXT, IDC_PLIST_FOLLOW_PROGRAM,
  								SN_PLIST_PGINFO_FOLLOW_PROGRAM_X, SN_PLIST_PGINFO_FOLLOW_PROGRAM_Y,
  								SN_PLIST_PGINFO_FOLLOW_PROGRAM_W, SN_PLIST_PGINFO_FOLLOW_PROGRAM_H,
  								p_pginfo_cont, 0);
  text_set_font_style(p_follow_program, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_follow_program, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_follow_program, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_follow_program, IDS_NEXT_PROGRAM2);

  //bar vertical
  p_bar_vertical = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BAR_VERTICAL,
                               SN_BAR_VERTICAL_X, SN_BAR_VERTICAL_Y,
                               SN_BAR_VERTICAL_W, SN_BAR_VERTICAL_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_bar_vertical, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL);

  //epg info
  y = SN_PLIST_EPG_INFO_Y;
  for (i = 0; i < PLIST_EPG_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PLIST_EPG_P+ i),
                       SN_PLIST_EPG_INFO_X, y,
                       SN_PLIST_EPG_INFO_W, SN_PLIST_EPG_INFO_H,
                       p_pginfo_cont, 0);
    text_set_align_type(p_subctrl, STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_INDEX3,FSI_INDEX3,FSI_INDEX3);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

    y+= SN_PLIST_EPG_INFO_H + SN_PLIST_EPG_INFO_Y;
  }
  #endif
  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=48;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  fill_time_date(p_pginfo_cont, FALSE);
  fill_prog_date(p_pginfo_cont, FALSE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  plist_set_modify_state(FALSE);
  ui_pic_stop();
  //ui_play_prog(pg_id, FALSE);
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    ui_stop_play(STOP_PLAY_BLACK,TRUE);
    ui_comm_cfmdlg_open(NULL, IDS_NO_PROG, NULL, 4000);
  }
  
  ui_set_epg_dy();

  //ui_set_pic_show(FALSE);
  ui_pic_play(ADS_AD_TYPE_CHLIST, ROOT_ID_PROG_LIST);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif


  return SUCCESS;
}


static RET_CODE on_plist_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  control_t *p_prog_info_cont = NULL;
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

  p_prog_info_cont = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_PLIST_PGINFO_CONT);
  if(NULL != p_prog_info_cont)
  {
    fill_prog_date(p_prog_info_cont, FALSE);
  }
  p_ctrl = ctrl_get_root(p_ctrl);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static RET_CODE on_prog_list_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{  
  return ERR_NOFEATURE;
}

static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  
  ui_pic_play(ADS_AD_TYPE_CHLIST, ROOT_ID_PROG_LIST);
  return SUCCESS;
}

BEGIN_KEYMAP(plist_cont_keymap, NULL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)   
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(plist_cont_keymap, NULL)

BEGIN_MSGPROC(plist_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_plist_exit)
  ON_COMMAND(MSG_TIME_UPDATE, on_plist_update_time)
  ON_COMMAND(MSG_EPG_READY, on_plist_update_epg_event)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_prog_list_draw_pic_end)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
END_MSGPROC(plist_cont_proc, cont_class_proc)

BEGIN_KEYMAP(plist_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_F1, MSG_SET_FAV)
  ON_EVENT(V_KEY_RED, MSG_SET_FAV)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
  ON_EVENT(V_KEY_PROGLIST, MSG_EXIT_ALL)  
END_KEYMAP(plist_list_keymap, NULL)

BEGIN_MSGPROC(plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_plist_exit_all)  
  ON_COMMAND(MSG_SET_FAV, on_plist_set_fav)
  ON_COMMAND(MSG_FOCUS_UP, on_prog_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_prog_list_change_focus)
  ON_COMMAND(MSG_INCREASE, on_prog_list_change_group)
  ON_COMMAND(MSG_DECREASE, on_prog_list_change_group)
  ON_COMMAND(MSG_SET_LANG, on_plist_switch_lang)
END_MSGPROC(plist_list_proc, list_class_proc)


