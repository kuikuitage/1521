
#include "ui_common.h"
#include "ui_epg.h"
#include "ui_prebook_manage.h"
#include "ui_book_api.h"
#include "ui_mute.h"

#include "sys_dbg.h"
#include "ui_play_api.h"
#include "ui_signal.h"

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h" 
#endif
#define IDC_EPG_CONT_ID IDC_COMM_ROOT_CONT
#define UI_EPG_UPDATE_INTERVAL 5000

#define GROUP_COUNT 3 //TV RADIO ALL

enum epg_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 200,    //0x40c8
  MSG_BOOK,
  MSG_AREA_CHANGE,
  MSG_SHOW_DETAIL,
  MSG_GROUP_UP,
  MSG_GROUP_DOWN,
  MSG_WEEKDAY_LEFT,
  MSG_WEEKDAY_RIGHT,
  MSG_OPEN_BOOK,
};


enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum epg_menu_ctrl_id
{
  IDC_EPG_ICON = 1,
  IDC_TV_AND_DETAIL_CONT,
  IDC_EPG_GROUP,
  IDC_EPG_PROG_LIST,
  IDC_EPG_PROG_PROG_LIST_SBAR,
  IDC_EPG_WEEKDAY,
  IDC_EPG_EVENT_LIST,
  IDC_EPG_EVENT_PROG_LIST_SBAR,
  IDC_EPG_TIME,

  //lvcm
  IDC_EPG_ADWIN,
  IDC_EPG_BAR_VERTICAL,
  IDC_EPG_ARROWL,
  IDC_EPG_ARROWR,
  //lvcm end
};

enum epg_detail_id
{
  IDC_EPG_DETAIL=1,  
};

static list_xstyle_t epg_prog_list_field_fstyle =
{
  FSI_LIST_TXT_N,
  FSI_LIST_TXT_N,
  FSI_LIST_TXT_HL,
  FSI_LIST_TXT_SEL,
  FSI_INDEX4,
};

static list_xstyle_t epg_prog_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_XX_BAR,//RSI_IGNORE,
};

static list_field_attr_t epg_prog_list_attr[EPG_PROG_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    48, 12/*5*/, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    170/*EPG_PG_LIST_MIDW - 60*/, 60, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},    
};

static list_field_attr_t epg_event_list_attr[EPG_EVENT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    25/*35*/, 10/*5*/, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120/*200*/, 35/*45*/, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    157/*300*/, 155/*250*/, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},    
  { LISTFIELD_TYPE_ICON,
    0/*EPG_EVENT_LIST_MIDW - 550*/, 318/*550*/, 0, &epg_prog_list_field_rstyle,  &epg_prog_list_field_fstyle},    
};

book_pg_t book_node;
epg_filter_t filter = {0};
static BOOL paint_now = FALSE;
static BOOL b_EpgItemReceiveOK = FALSE;

static RET_CODE epg_prog_list_update(control_t* p_list, u16 start, u16 size, u32 context);
static void epg_data_update(control_t *p_menu, u8 focus, BOOL is_paint);
static void epg_event_list_get_time_area(utc_time_t *start, utc_time_t *end);

u16 epg_cont_keymap(u16 key);
RET_CODE epg_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 epg_sbox_week_keymap(u16 key);
RET_CODE epg_sbox_week_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 epg_prog_list_keymap(u16 key);
RET_CODE epg_prog_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

u16 epg_event_list_keymap(u16 key);
RET_CODE epg_event_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE epg_event_list_update(control_t* p_list, u16 start, u16 size, u32 context);

static void epg_event_list_get_time_area(utc_time_t *p_start, utc_time_t *p_end)
{
  utc_time_t g_time = {0};
  
  time_get(&g_time, FALSE);
  
  memcpy(p_start, &g_time, sizeof(utc_time_t));
  memcpy(p_end, &g_time, sizeof(utc_time_t));
  p_end->hour = 23;
  p_end->minute = 59;
}

void epg_prog_list_update_time(control_t *p_info, BOOL is_paint)
{
  utc_time_t time;
  u8 asc_str[10];

  time_get(&time, FALSE);

  //printf_time(&time, "epg_prog_list_update_time");

  sprintf((char *)asc_str, "%.2d/%.2d", time.month, time.day);
  mbox_set_content_by_ascstr(p_info, 0, asc_str);

  sprintf((char *)asc_str, "%.2d:%.2d", time.hour, time.minute);
  mbox_set_content_by_ascstr(p_info, 1, asc_str);  

  if(is_paint)
  {
    ctrl_paint_ctrl(p_info, TRUE);
  }
}

static void epg_play_pg(u16 focus)
{
  u16 rid;

  if ((rid =
         db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                    focus)) != INVALIDID)
  {
    ui_play_prog(rid, FALSE);  
  }
}

static void epg_item_update(control_t *p_cont, u8 focus_state, BOOL is_paint )
{
  dvbs_prog_node_t pg;
  evt_node_t *p_evt_node = NULL;//, *p_next_node;
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0};  
  control_t *p_list, *p_event_list, *p_sbox_week;
  u16 list_curn, pg_id;
  u16 unistr[64];
  u16 old_focus = 0;
  u16 new_focus = 0;
  u32 item_num = 0;
  u8 day_pos;
  memset(&pg, 0, sizeof(dvbs_prog_node_t));

  epg_event_list_get_time_area(&s_time, &e_time);
    
  p_list = ctrl_get_child_by_id(p_cont, IDC_EPG_PROG_LIST);
  p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);
  p_sbox_week = ctrl_get_child_by_id(p_cont, IDC_EPG_WEEKDAY);

  if(p_sbox_week!=NULL)
  {
    day_pos = sbox_get_focus_pos(p_sbox_week);

    if(day_pos != 0)
    {
      time_up(&s_time, day_pos*24*60*60);
      s_time.hour = 0;
      s_time.minute = 0;
      s_time.second = 0;

      memcpy(&e_time, &s_time, sizeof(utc_time_t));
      e_time.hour = 23;
      e_time.minute = 59;
      e_time.second = 59;
    }
    ui_set_epg_day_offset(day_pos);
  }
  
  list_curn = list_get_focus_pos(p_list);
  
  pg_id =db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), list_curn);
  db_dvbs_get_pg_by_id(pg_id, &pg);

  filter.service_id      = (u16)pg.s_id;
  filter.cont_level = 0;
  
  if(pg.orig_net_id == 0
    //&& pg.tp_id == 1
   // && pg.ts_id == 0
    )                                        //for asia 3
  {
    filter.stream_id = 0xffff;
    filter.orig_network_id = 0xffff;
  }
  else
  {
    filter.stream_id = (u16)pg.ts_id;
    filter.orig_network_id = (u16)pg.orig_net_id;
  }
  time_to_gmt(&s_time, &(filter.start_time));
  time_to_gmt(&e_time, &(filter.end_time));

  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &item_num);

  if(item_num>0)
  {
    b_EpgItemReceiveOK = TRUE;
    old_focus = list_get_focus_pos(p_event_list);
    list_set_count(p_event_list, (u16)item_num, EPG_EVENT_LIST_PAGE);
    switch(focus_state)
    {
        case FOCUS_FIRST:
          new_focus = 0;
          break;
        case FOCUS_PREV:
          new_focus = (old_focus < item_num-1) ? old_focus : 0;
          break;
        default:  
          new_focus = 0;
          break;
    }
    list_set_focus_pos(p_event_list, new_focus);
    list_select_item(p_event_list, new_focus);
    epg_event_list_update(p_event_list, list_get_valid_pos(p_event_list), EPG_EVENT_LIST_PAGE, 0);
  }
  else
  {
    b_EpgItemReceiveOK = FALSE;
    list_set_count(p_event_list, 1, EPG_EVENT_LIST_PAGE);
    list_set_focus_pos(p_event_list, 0);
    list_select_item(p_event_list, 0);

    list_set_field_content_by_icon(p_event_list, 0, 0, 0);
    
    #ifdef changjiang_js
      unistr[0] = '\0';
    #else
      gui_get_string(IDS_RECIEVING_EPG, unistr, 64);
    #endif
    
    list_set_field_content_by_unistr(p_event_list, 0, 1, unistr);
    list_set_field_content_by_icon(p_event_list, 0, 3, 0);
  }
  if(is_paint)
  {
    ctrl_paint_ctrl(p_event_list, TRUE);
  }
}

static void epg_weekday_upgrade(control_t *p_sbox_weekday, BOOL is_force)
{
  u16 x, w;
  s8 focus = 0;
  u8 i, hgap = 4;
  //u16 unistr[64];

  u8  weekday;
  u16 dst_unistr[128];
  u16 tmp_unistr[128];
  u8  ansstr[256];
  u16 weekstr[] = 
    {
      IDS_MON,
      IDS_TUS,
      IDS_WED,
      IDS_THS,
      IDS_FRI,
      IDS_SAT,
      IDS_SUN,
    };
  utc_time_t cur_time = {0};
  utc_time_t tmp_time = {0};
  
  time_get(&cur_time, TRUE);
  
  focus = sbox_get_focus_pos(p_sbox_weekday);

  x = 0;
  w = 0;
  for(i = 0; i<SEVEN; i++)
  {
    memcpy(&tmp_time, &cur_time, sizeof(utc_time_t));
    time_up(&tmp_time, ONE_DAY_SECONDS*i);
    weekday = date_to_weekday(&tmp_time);
    
    if(focus == i)
    {
      w = (7+(EPG_WEEKDAY_W - 6*hgap)/7);

      if(i == 0)
      {
        gui_get_string(IDS_TODAY, dst_unistr, 64);
      }
      else
      {
        gui_get_string(weekstr[weekday], dst_unistr, 64);
      }

      sprintf((char *)ansstr, "(%.2d/%.2d)",tmp_time.month,tmp_time.day);
      str_asc2uni(ansstr, tmp_unistr);  
//      uni_strcat(dst_unistr, tmp_unistr, uni_strlen(dst_unistr) + uni_strlen(tmp_unistr));
    }
    else
    {
      w = ((EPG_WEEKDAY_W - 6*hgap)/7);
      gui_get_string(weekstr[weekday], dst_unistr, 64);
    }
    sbox_set_item_status(p_sbox_weekday, i, x, w);
    x += (w+hgap);
    sbox_set_content_by_unistr(p_sbox_weekday, i, dst_unistr);
  }

}


static void epg_data_update(control_t *p_cont, u8 focus, BOOL is_paint)
{
#if 0
  epg_dy_policy_t *p_dy_policy = NULL;

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));

  epg_event_list_get_time_area(&s_time, &e_time);

  memcpy(&p_dy_policy->start, &s_time, sizeof(utc_time_t));
  p_dy_policy->start.hour = 0;
  p_dy_policy->start.minute = 0;
  p_dy_policy->start.second = 0;
  memcpy(&p_dy_policy->end, &s_time, sizeof(utc_time_t));
  time_up(&p_dy_policy->end, 3600 * 24);
#endif
  epg_item_update(p_cont, focus, is_paint);

}

static RET_CODE on_epg_ready(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_event_list;
  dvbs_prog_node_t pg = {0};
  u16 pg_id = 0;
  pg_id = sys_status_get_curn_group_curn_prog_id();
  
  db_dvbs_get_pg_by_id(pg_id, &pg);
   p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);
  //update epg data.  
  if((para1 == pg.s_id)
   && (para2 == pg.ts_id))
    {
      if(paint_now)
      {
        epg_data_update(p_cont, FOCUS_PREV, TRUE);
        ctrl_paint_ctrl(p_event_list, TRUE);
      }
    }
  return SUCCESS;
}

static RET_CODE on_epg_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  ui_enable_chk_pwd(TRUE);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_epg_book_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);
  ctrl_paint_ctrl(p_event_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE epg_prog_list_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  u16 i;
  u8 asc_str[8];
  u16 uni_str[32];
  u16 pg_id;
  u16 cnt = list_get_count(p_list);
  dvbs_prog_node_t pg;
  
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
      sprintf((char *)asc_str, "%4d ", pg.logic_ch_num);
      #else
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      #endif
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str); 

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str);
    }
  }
  
  epg_data_update(ctrl_get_parent(p_list), FOCUS_FIRST, (context == 0)?FALSE:TRUE);  
  return SUCCESS;
}

void prebook_set_epg_event_list_update(control_t* p_ctrl)
{
	control_t *p_event_list,*cont;
	//find container
	cont = ctrl_get_child_by_id(p_ctrl, BACKGROUND_IDC_CLIENT_CONT);
	p_event_list = ctrl_get_child_by_id(cont, IDC_EPG_EVENT_LIST);
    epg_event_list_update(p_event_list, list_get_valid_pos(p_event_list), EPG_EVENT_LIST_PAGE, 0);
}

static RET_CODE epg_event_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  evt_node_t *p_evt_node = NULL;
  control_t* p_prog_list = ctrl_get_child_by_id(p_list->p_parent, IDC_EPG_PROG_LIST);
  u8 i, pos;
  u32 item_num;
  u8 ascstr[32];
  u16 cnt = list_get_count(p_list);
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  book_pg_t temp_node = {0};
  
  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &item_num);
  for (i = 0; i < start; i++)
  {
    if(i < item_num)
    {
      p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &filter);
    }
    else
    {
      p_evt_node = NULL;
    }
  }

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if((pos < cnt) && (pos < item_num))
    {
      if(p_evt_node != NULL)
      { 
        time_to_local(&(p_evt_node->start_time), &start_time);
        memcpy(&end_time, &start_time, sizeof(utc_time_t));
        time_add(&end_time, &(p_evt_node->drt_time));
        sprintf((char *)ascstr,"%.2d:%.2d-%.2d:%.2d", \
          start_time.hour, start_time.minute,\
          end_time.hour, end_time.minute);

        memset(&temp_node, 0, sizeof(book_pg_t));
        temp_node.pgid = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), list_get_focus_pos(p_prog_list));
        memcpy(&(temp_node.start_time), &start_time, sizeof(utc_time_t));
        memcpy(&(temp_node.drt_time), &(p_evt_node->drt_time), sizeof(utc_time_t));
        temp_node.timer_mode = BOOK_TMR_ONCE;
        
        if(book_get_match_node(&temp_node) < MAX_BOOK_PG)
        {
          list_set_field_content_by_icon(p_list, pos, 0, IM_SN_ICON_TIME);
        }
        else
        {
          list_set_field_content_by_icon(p_list, pos, 0, 0);
        }
        
        list_set_field_content_by_ascstr(p_list, pos, 1, ascstr);
        list_set_field_content_by_unistr(p_list, pos, 2, p_evt_node->event_name);

        if(p_evt_node->p_sht_txt != NULL)
        {
          list_set_field_content_by_icon(p_list, pos, 3, 0);//IM_ICON_EDIT
        }
        else
        {
          list_set_field_content_by_icon(p_list, pos, 3, 0);
        }
        
        p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &filter);
      }
    }
  }

  return SUCCESS;
}


static RET_CODE on_epg_area_change(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u8 act_id;
  control_t *p_cont = NULL;
  control_t *p_new_act_list = NULL;
  control_t *p_tv_and_detail_cont = NULL;
  control_t *p_detail = NULL;

  p_cont = ctrl_get_parent(p_list);
  act_id = ctrl_get_ctrl_id(p_list);
  
  switch(act_id)
  {
  case IDC_EPG_PROG_LIST:
    p_new_act_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);
    break;

  case IDC_EPG_EVENT_LIST:
    p_new_act_list = ctrl_get_child_by_id(p_cont, IDC_EPG_PROG_LIST);

    p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont, IDC_TV_AND_DETAIL_CONT);
    p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);
    text_set_content_by_unistr(p_detail, (u16*)"");
    ctrl_set_rstyle(p_detail, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_attr(p_detail, OBJ_ATTR_HIDDEN);
    ctrl_paint_ctrl(p_tv_and_detail_cont, TRUE);
    break;
  default:
    break;
  }
  
  if(NULL == p_new_act_list)
  {
      return ERR_FAILURE;
  }
  ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);
  ctrl_set_attr(p_new_act_list, OBJ_ATTR_ACTIVE);
  ctrl_process_msg(p_new_act_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_paint_ctrl(p_new_act_list, TRUE);
  return SUCCESS;
}

static RET_CODE on_epg_show_detail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  evt_node_t *p_evt_node = NULL;
  control_t *p_cont, *p_tv_and_detail_cont, *p_detail;
  u32 item_num;
  u16 pos = 0;
  u16 i = 0;
  u16 count = list_get_count(p_list);

  p_cont = ctrl_get_parent(p_list);

  p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont, IDC_TV_AND_DETAIL_CONT);
  p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);
  
  if(ctrl_get_attr(p_detail) != OBJ_ATTR_HIDDEN && (msg == MSG_SHOW_DETAIL))
  {
  ctrl_set_attr(p_detail, OBJ_ATTR_HIDDEN);
    if(ui_is_mute())
      open_mute(0,0);
  }
  else
  {
    close_mute();
    if(count > 0)
    {
      pos = list_get_focus_pos(p_list);

      p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &item_num);

      if(p_evt_node != NULL)
      {
        while((p_evt_node != NULL) && (i<item_num) && (i!=pos))
        {
          p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &filter);
          i++;
        }
      }

      if((p_evt_node != NULL)&&(NULL != p_evt_node->p_sht_txt))
      {
        
        text_set_content_by_unistr(p_detail, p_evt_node->p_sht_txt);
        ctrl_set_rstyle(p_detail, RSI_WHITE_YELLOW, RSI_WHITE_YELLOW, RSI_WHITE_YELLOW);
        ctrl_set_attr(p_detail, OBJ_ATTR_ACTIVE);
      }
      else
      {
        text_set_content_by_unistr(p_detail, (u16*)"");
        ctrl_set_rstyle(p_detail, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_attr(p_detail, OBJ_ATTR_ACTIVE);
      }
    }
    else
    {
      ctrl_set_rstyle(p_detail, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
      text_set_content_by_unistr(p_detail, (u16*)"");
    }
  }

  ctrl_paint_ctrl(p_detail->p_parent, TRUE);

  return SUCCESS;
}


static RET_CODE on_epg_prog_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{

  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus, offset;
  control_t *p_cont, *p_event_list;
  
  p_cont = ctrl_get_parent(p_list);
  p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);
  
  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  list_select_item(p_list, new_focus);
  
  list_draw_item_ext(p_list, old_focus, TRUE);
  list_draw_item_ext(p_list, new_focus, TRUE); 
  
  epg_play_pg(list_get_focus_pos(p_list));   
  offset = list_get_valid_pos(p_list);
  ui_set_epg_dy_by_offset(offset);
  epg_item_update(p_list->p_parent, FOCUS_FIRST, TRUE);
  fw_notify_root(p_list, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  update_signal();

  return SUCCESS;
}

static RET_CODE on_epg_prog_list_change_group(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cbox;
  u8 curn_mode = 0;
  u16 curn_group = 0;
  u16 pg_id, pg_pos = 0xFFFF;
  u32 group_context = 0;
  u8 view_id = 0xFF;
  u16 cbox_focus = 0xFFFF;
  u16 view_count = 0;
  u16 offset = 0;
  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_EPG_GROUP);
  
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

    case 2: //tv and radio
      ui_dvbc_change_view(DB_DVBS_TV_RADIO, TRUE);     
      break;
  }

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  update_signal(); 
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  
  list_set_count(p_list, view_count, EPG_PROG_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  epg_prog_list_update(p_list, list_get_valid_pos(p_list), EPG_PROG_LIST_PAGE, 1);
  list_select_item(p_list, pg_pos);
  
  ctrl_paint_ctrl(p_list, TRUE);
  offset = list_get_valid_pos(p_list);
  ui_set_epg_dy_by_offset(offset);

  return SUCCESS;
}

static RET_CODE on_epg_event_list_up_down(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  control_t *p_cont, *p_tv_and_detail_cont, *p_detail;
  u16 old_focus, new_focus;
  
  old_focus = list_get_focus_pos(p_list);
  list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  list_select_item(p_list, new_focus);
  //mtos_task_delay_ms(1000);
  list_draw_item_ext(p_list, old_focus, TRUE);
  list_draw_item_ext(p_list, new_focus, TRUE);
  
  p_cont = ctrl_get_parent(p_list);

  p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont, IDC_TV_AND_DETAIL_CONT);
  p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);

  if(ctrl_get_attr(p_detail) != OBJ_ATTR_HIDDEN)
  {
      on_epg_show_detail(p_list, msg, para1, para2);
  }
  
  return SUCCESS;
}

static RET_CODE on_epg_open_book(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //when open prebook menu stop play
  ui_stop_play(STOP_PLAY_BLACK,TRUE);
  return manage_open_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);
}

static RET_CODE on_epg_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  ui_close_all_mennus();
  return SUCCESS;
}

static RET_CODE on_epg_event_list_book(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  control_t *p_cont, *p_prog_list;
  evt_node_t *p_evt_node;
  u32 item_num = 0, i = 0;
  u16 count;
  u16 focus;
  book_pg_t book_node;
  dvbs_prog_node_t pg = {0};
  comm_prompt_data_t epg_data_t =
	 {
	   ROOT_ID_EPG,
	   STR_MODE_STATIC,
	   200,165,250,150,0,
	   RSC_INVALID_ID,RSC_INVALID_ID,
	   RSC_INVALID_ID,DIALOG_DEADLY,
	   2000
	 };

  count = list_get_count(p_list);
  
  if(count > 0)
  {
    focus = list_get_focus_pos(p_list);

    p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &item_num);

    if(p_evt_node != NULL)
    {
      while((p_evt_node != NULL) && (i < item_num) && (i != focus))
      {
        p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &filter);
        i++;
      }
    }

    if(p_evt_node != NULL)
    {
      p_cont = ctrl_get_parent(p_list);
      p_prog_list = ctrl_get_child_by_id(p_cont, IDC_EPG_PROG_LIST);
      book_node.pgid = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), list_get_focus_pos(p_prog_list));
      db_dvbs_get_pg_by_id(book_node.pgid, &pg);
      book_node.svc_type = (u8)pg.service_type;
      book_node.timer_mode = BOOK_TMR_ONCE;  
      time_to_local(&(p_evt_node->start_time), &(book_node.start_time));
      memcpy(&(book_node.drt_time), &(p_evt_node->drt_time), sizeof(utc_time_t)); 

      memcpy(&(book_node.event_name), &(p_evt_node->event_name), sizeof(p_evt_node->event_name));

      switch(check_book_pg(&book_node))
      {
        case BOOK_ERR_DUR_TOO_SHORT:
          epg_data_t.text_strID = IDS_LESS_ONE_MIN;
		  ui_comm_prompt_open(&epg_data_t);
          break;

        case BOOK_ERR_PLAYING:
		  epg_data_t.text_strID = IDS_CUR_EVENT_IS_PLAYING;
		  ui_comm_prompt_open(&epg_data_t);
          break;

        case BOOK_ERR_CONFILICT:
		  epg_data_t.text_strID = IDS_BOOK_CONFLICT;
		  ui_comm_prompt_open(&epg_data_t);
          break;
        case BOOK_ERR_SAME:
          book_delete_node(book_get_match_node(&book_node));
          list_set_field_content_by_icon(p_list, focus, 0, 0);
          list_draw_field_ext(p_list, focus, 0, TRUE);
          break;

        case BOOK_ERR_FULL:
		  epg_data_t.text_strID = IDS_BOOK_IS_FULL;
		  ui_comm_prompt_open(&epg_data_t);
          break;

        case BOOK_ERR_NO:
          list_set_field_content_by_icon(p_list, focus, 0, IM_SN_ICON_TIME);
          list_draw_field_ext(p_list, focus, 0, TRUE);
          book_add_node(&book_node);
          break;
        default:
          break;
      }  
    }
  }

  return SUCCESS;
}

static RET_CODE on_epg_change_weekday(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  control_t *p_weekday_cbox, *p_cont, *p_event_list, *p_tv_and_detail_cont, *p_detail;
  u8 old_focus = 0;
  u8 new_focus = 0;
  
  p_cont = ctrl_get_parent(p_list);
  p_weekday_cbox = ctrl_get_child_by_id(p_cont, IDC_EPG_WEEKDAY);
  p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_EVENT_LIST);

  old_focus = sbox_get_focus_pos(p_weekday_cbox);

  switch(msg)
  {
    case MSG_WEEKDAY_LEFT:
      new_focus = (old_focus - 1 + SEVEN)%SEVEN;
      break;

    case MSG_WEEKDAY_RIGHT:
      new_focus = (old_focus + 1)%SEVEN;
      break;
    default:
      new_focus = old_focus;
      break;
  }
  sbox_set_focus_pos(p_weekday_cbox, new_focus);

  epg_weekday_upgrade(p_weekday_cbox, TRUE);
  ctrl_paint_ctrl(p_weekday_cbox, TRUE);

  epg_item_update(p_cont, FOCUS_FIRST, TRUE);
  ctrl_paint_ctrl(p_event_list, TRUE);

  p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont, IDC_TV_AND_DETAIL_CONT);
  p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);

  if(ctrl_get_attr(p_detail) != OBJ_ATTR_HIDDEN)
  {
    on_epg_show_detail(p_list, msg, para1, para2);
  }

  return SUCCESS;
}

/****************************************************
NAME:   on_epg_switch_lang
FUNCTION: update the ctrl which fill str is  unicode 
****************************************************/
static RET_CODE on_epg_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  control_t *p_sbox_week = NULL;
  
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

  if(!b_EpgItemReceiveOK)
  {
    epg_item_update(p_ctrl, FOCUS_PREV, FALSE);  //update the epg item unicode
  }
  p_sbox_week = ctrl_get_child_by_id(p_ctrl, IDC_EPG_WEEKDAY);
  if(NULL != p_sbox_week)
  {
    epg_weekday_upgrade(p_sbox_week, FALSE);  //update the weekday unicode
  }

  p_ctrl = ctrl_get_root(p_ctrl);
  ctrl_paint_ctrl(p_ctrl,TRUE);

  return SUCCESS;
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

RET_CODE open_epg(u32 para1, u32 para2)
{
  control_t *p_group_name = NULL;
  control_t *p_cont;
  control_t *p_ctrl_detail = NULL;
  control_t *p_ctrl_prev = NULL;
  control_t *p_ctrl_adwin, *p_ctrl_vertical, *p_subctrl;
  control_t *p_prog_list, *p_event_list, *p_sbox_week;
  u16 i, curn_group, curn_mode, pg_id, pg_pos;
  u16 view_count = 0, cbox_focus = 0;
  u32 group_context;
  u8 view_id;
  
  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F1},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 70 , IDS_SWITCH_AREA},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F2},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_INTRODUCE},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F3},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 80 , IDS_BOOK_SET},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F4},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 100 , IDS_BOOK_MANAGE}
  };	
  list_xstyle_t epg_list_item_rstyle =
  {
    RSI_SN_BG,//RSI_COMM_LIST_N,
    RSI_SN_BG,//RSI_COMM_LIST_N,
    RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
    RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_SEL,
    RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
  };

  paint_now = FALSE;
  update_signal();  
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);
 
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
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
    cbox_focus=2;
    break;
  }

  p_cont = ui_background_create(ROOT_ID_EPG,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_CHANNEL_GUIDE,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, epg_cont_keymap);
  ctrl_set_proc(p_cont, epg_cont_proc);

  //tv and detail container
  p_ctrl_prev = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_TV_AND_DETAIL_CONT, 
                                        SN_EPG_PREV_X, SN_EPG_PREV_Y, 
                                        SN_EPG_PREV_W, SN_EPG_PREV_H, 
                                        p_cont, 0);
  ctrl_set_rstyle(p_ctrl_prev, RSI_SN_TVWIN, RSI_SN_TVWIN, RSI_SN_TVWIN); 

  //advertisement window
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  p_ctrl_adwin = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_EPG_ADWIN,
							SN_EPG_ADWIN_X+28, SN_EPG_ADWIN_Y-7,
							SN_EPG_ADWIN_W-56, SN_EPG_ADWIN_H+18,
							p_cont, 0);
  #else
  p_ctrl_adwin = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_EPG_ADWIN,
							SN_EPG_ADWIN_X, SN_EPG_ADWIN_Y,
							SN_EPG_ADWIN_W, SN_EPG_ADWIN_H,
							p_cont, 0);
  #endif
  ctrl_set_rstyle(p_ctrl_adwin, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);

  //detail information
  p_ctrl_detail = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EPG_DETAIL, 
                                        SN_EPG_DETAIL_X, SN_EPG_DETAIL_Y, 
                                        SN_EPG_DETAIL_W, SN_EPG_DETAIL_H, 
                                        p_ctrl_prev, 0);
  ctrl_set_rstyle(p_ctrl_detail, RSI_WHITE_YELLOW, RSI_WHITE_YELLOW, RSI_WHITE_YELLOW);
  ctrl_set_attr(p_ctrl_detail, OBJ_ATTR_HIDDEN);
  text_set_font_style(p_ctrl_detail, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl_detail, STL_CENTER|STL_TOP);
  text_set_content_type(p_ctrl_detail, TEXT_STRTYPE_UNICODE);

  //group title
  p_group_name = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_EPG_GROUP, 
                                        SN_EPG_GROUP_NAME_X, SN_EPG_GROUP_NAME_Y,
                                        SN_EPG_GROUP_NAME_W, SN_EPG_GROUP_NAME_H, 
                                        p_cont, 0);
  ctrl_set_rstyle(p_group_name, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  cbox_set_font_style(p_group_name, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  cbox_set_work_mode(p_group_name, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group_name, TRUE);
  cbox_dync_set_count(p_group_name, GROUP_COUNT);
  cbox_dync_set_update(p_group_name, group_type_cbox_update);
  cbox_dync_set_focus(p_group_name, cbox_focus);

  //group title arrow
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EPG_ARROWL,
  						  SN_EPG_ARROW_LEFT_X, SN_EPG_ARROW_Y,
  						  SN_EPG_ARROW_W, SN_EPG_ARROW_H,
  						  p_group_name, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_LEFT);
  
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EPG_ARROWR,
  						  SN_EPG_ARROW_RIGHT_X, SN_EPG_ARROW_Y,
  						  SN_EPG_ARROW_W, SN_EPG_ARROW_H,
  						  p_group_name, 0);
  bmap_set_content_by_id(p_subctrl, IM_SN_ICON_ARROW_RIGHT);
  
  //prog list
  p_prog_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EPG_PROG_LIST, 
                                        SN_EPG_PG_LISTX, SN_EPG_PG_LISTY, 
                                        SN_EPG_PG_LISTW, SN_EPG_PG_LISTH, 
                                        p_cont, 0);
  ctrl_set_rstyle(p_prog_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_prog_list, epg_prog_list_keymap);
  ctrl_set_proc(p_prog_list, epg_prog_list_proc);
  list_set_mid_rect(p_prog_list,SN_EPG_PG_LIST_MIDL, SN_EPG_PG_LIST_MIDT,
                                            SN_EPG_PG_LIST_MIDW, SN_EPG_PG_LIST_MIDH,
                                            SN_EPG_LIST_H_GAP);
  list_set_item_rstyle(p_prog_list, &epg_list_item_rstyle);
  list_enable_select_mode(p_prog_list, TRUE);
  list_set_select_mode(p_prog_list, LIST_SINGLE_SELECT);
  list_set_count(p_prog_list, view_count, EPG_PROG_LIST_PAGE);
  list_set_field_count(p_prog_list, EPG_PROG_LIST_FIELD, EPG_PROG_LIST_PAGE);
  list_set_focus_pos(p_prog_list, pg_pos);
  list_select_item(p_prog_list, pg_pos);
  list_set_update(p_prog_list, epg_prog_list_update, 0);

  for (i = 0; i < EPG_PROG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_prog_list, (u8)i, (u32)(epg_prog_list_attr[i].attr), (u16)(epg_prog_list_attr[i].width),
                        (u16)(epg_prog_list_attr[i].left), (u8)(epg_prog_list_attr[i].top));
    list_set_field_rect_style(p_prog_list, (u8)i, epg_prog_list_attr[i].rstyle);
    list_set_field_font_style(p_prog_list, (u8)i, epg_prog_list_attr[i].fstyle);
  }    
  
  //week day
  p_sbox_week = ctrl_create_ctrl((u8 *)CTRL_SBOX, (u8)IDC_EPG_WEEKDAY, 
                                        SN_EPG_EVENT_LISTX-15, EPG_WEEKDAY_Y, 
                                        SN_EPG_EVENT_LISTW+20, EPG_WEEKDAY_H, 
                                        p_cont, 0);
  ctrl_set_rstyle(p_sbox_week, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);  
  ctrl_set_keymap(p_sbox_week, epg_sbox_week_keymap);
  ctrl_set_proc(p_sbox_week, epg_sbox_week_proc);
  ctrl_set_style(p_sbox_week, STL_EX_ALWAYS_HL);
  sbox_set_count(p_sbox_week, SEVEN);
  sbox_set_mid_rstyle(p_sbox_week, RSI_SN_BG, RSI_SN_BAR_YELLOW_HL, RSI_SN_BG);
  sbox_set_fstyle(p_sbox_week, FSI_INDEX4, FSI_COMM_BTN, FSI_COMM_BTN);
  sbox_set_content_type(p_sbox_week, SBOX_STRTYPE_UNICODE);
  sbox_set_focus_pos(p_sbox_week, 0);
  epg_weekday_upgrade(p_sbox_week, FALSE);

  //event list
  p_event_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EPG_EVENT_LIST, 
                                        SN_EPG_EVENT_LISTX - 8, SN_EPG_EVENT_LISTY, 
                                        SN_EPG_EVENT_LISTW + 20, SN_EPG_EVENT_LISTH, 
                                        p_cont, 0);
  ctrl_set_rstyle(p_event_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_event_list, epg_event_list_keymap);
  ctrl_set_proc(p_event_list, epg_event_list_proc);
  list_set_mid_rect(p_event_list, EPG_EVENT_LIST_MIDL, EPG_EVENT_LIST_MIDT,
                                          SN_EPG_EVENT_LIST_MIDW + 20, SN_EPG_EVENT_LIST_MIDH,
                                          SN_EPG_LIST_V_GAP);
  list_set_item_rstyle(p_event_list, &epg_list_item_rstyle);
  list_enable_select_mode(p_event_list, TRUE);
  list_set_select_mode(p_event_list, LIST_SINGLE_SELECT);
  list_set_field_count(p_event_list, EPG_EVENT_LIST_FIELD, EPG_EVENT_LIST_PAGE);
  list_set_update(p_event_list, epg_event_list_update, 0);

  for (i = 0; i < EPG_EVENT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_event_list, (u8)i, (u32)(epg_event_list_attr[i].attr), (u16)(epg_event_list_attr[i].width),
                        (u16)(epg_event_list_attr[i].left), (u8)(epg_event_list_attr[i].top));
    list_set_field_rect_style(p_event_list, (u8)i, epg_event_list_attr[i].rstyle);
    list_set_field_font_style(p_event_list, (u8)i, epg_event_list_attr[i].fstyle);
  }    

  //bar vertical
  p_ctrl_vertical = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EPG_BAR_VERTICAL,
  							 SN_EPG_BAR_VERTICAL_X, SN_EPG_BAR_VERTICAL_Y,
  							 SN_EPG_BAR_VERTICAL_W, SN_EPG_BAR_VERTICAL_H,
  							 p_cont, 0);
  ctrl_set_rstyle(p_ctrl_vertical, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL, RSI_SN_BAR_VERTICAL);

  epg_prog_list_update(p_prog_list, list_get_valid_pos(p_prog_list), EPG_PROG_LIST_PAGE, 0);

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  if(TRUE == get_full_scr_ad_status())
    ui_pic_stop();
#endif

   ctrl_default_proc(p_prog_list, MSG_GETFOCUS, 0, 0);
	memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
   sn_submenu_help_data.x=0;
   sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
   sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
   sn_submenu_help_data.offset=28;
   sn_submenu_help_data.rsc =help_item;
   
   sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

   ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
   paint_now = TRUE;
   
   ui_set_epg_dy();
   
   ui_pic_stop();
   ui_pic_play(ADS_AD_TYPE_EPG, ROOT_ID_EPG);
   
   return SUCCESS;
}

static RET_CODE on_epg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
  
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  return ERR_NOFEATURE;
}
static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  
  ui_pic_play(ADS_AD_TYPE_EPG, ROOT_ID_EPG);
  return SUCCESS;
}

BEGIN_KEYMAP(epg_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EPG, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(epg_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(epg_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EPG_READY, on_epg_ready)
  ON_COMMAND(MSG_DESTROY, on_epg_destory)
  ON_COMMAND(MSG_BOOK_UPDATE, on_epg_book_update)
  ON_COMMAND(MSG_SET_LANG, on_epg_switch_lang)
  ON_COMMAND(MSG_EXIT_ALL, on_epg_exit)
  ON_COMMAND(MSG_EXIT, on_epg_exit)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
END_MSGPROC(epg_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(epg_prog_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)   
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE) 
  ON_EVENT(V_KEY_F1, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_RED, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_F4, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
END_KEYMAP(epg_prog_list_keymap, NULL)

BEGIN_MSGPROC(epg_prog_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_INCREASE, on_epg_prog_list_change_group)
  ON_COMMAND(MSG_DECREASE, on_epg_prog_list_change_group)
  ON_COMMAND(MSG_AREA_CHANGE, on_epg_area_change)
  ON_COMMAND(MSG_OPEN_BOOK, on_epg_open_book)
  ON_COMMAND(MSG_EXIT_ALL, on_epg_list_ok)
END_MSGPROC(epg_prog_list_proc, list_class_proc)

BEGIN_KEYMAP(epg_event_list_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_WEEKDAY_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_WEEKDAY_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_F1, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_RED, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_F3, MSG_BOOK)
  ON_EVENT(V_KEY_YELLOW, MSG_BOOK)
  ON_EVENT(V_KEY_F4, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_F2, MSG_SHOW_DETAIL)
  ON_EVENT(V_KEY_GREEN, MSG_SHOW_DETAIL)
#else
  ON_EVENT(V_KEY_F1, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_RED, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_F3, MSG_BOOK)
  ON_EVENT(V_KEY_GREEN, MSG_BOOK)
  ON_EVENT(V_KEY_F4, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_F2, MSG_SHOW_DETAIL)
  ON_EVENT(V_KEY_YELLOW, MSG_SHOW_DETAIL)
#endif
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
END_KEYMAP(epg_event_list_keymap, NULL)

BEGIN_MSGPROC(epg_event_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_event_list_up_down)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_event_list_up_down)
  ON_COMMAND(MSG_PAGE_UP, on_epg_event_list_up_down)
  ON_COMMAND(MSG_PAGE_DOWN, on_epg_event_list_up_down)
  ON_COMMAND(MSG_OPEN_BOOK, on_epg_open_book)
  ON_COMMAND(MSG_BOOK, on_epg_event_list_book)
  ON_COMMAND(MSG_AREA_CHANGE, on_epg_area_change)
  ON_COMMAND(MSG_SHOW_DETAIL, on_epg_show_detail)
  ON_COMMAND(MSG_EXIT_ALL, on_epg_list_ok)
  ON_COMMAND(MSG_WEEKDAY_LEFT, on_epg_change_weekday)
  ON_COMMAND(MSG_WEEKDAY_RIGHT, on_epg_change_weekday)
END_MSGPROC(epg_event_list_proc, list_class_proc)

BEGIN_KEYMAP(epg_sbox_week_keymap, NULL)
END_KEYMAP(epg_sbox_week_keymap, NULL)

BEGIN_MSGPROC(epg_sbox_week_proc, sbox_class_proc)
END_MSGPROC(epg_sbox_week_proc, sbox_class_proc)

