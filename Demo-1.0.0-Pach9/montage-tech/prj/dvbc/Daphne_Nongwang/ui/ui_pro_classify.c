/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
   Learner:   lucifer wang <yangwen.wang@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_notify.h"

#include "ui_mainmenu.h"
#include "ui_pro_classify.h"


enum osd_set_control_id
{
  IDC_INVALID = 0,
  IDC_CLASSIFY_TITLE,
  IDC_AD_WINDOW,
  IDC_LIST_BG,
  IDC_PROG_DETAIL_BG,
  IDC_PROG_DETAIL,
  IDC_LIST_SBAR,
  IDC_EVENT_LIST,

  //lvcm
  IDC_CLASSIFY_BAR_ARROWL,
  IDC_CLASSIFY_BAR_ARROWR,
  //lvcm end
};

static u16 prog_id = 0;

epg_filter_t classify_filter = {0};
#if 1
epg_query_by_nibble_t nibble_query = {0};
#endif
u16 classify_list_cont_keymap(u16 key);
RET_CODE classify_list_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 classify_group_keymap(u16 key);
RET_CODE classify_group_proc(control_t *p_cbox, u16 msg, u32 para1, u32 para2);

u16 classify_list_keymap(u16 key);
RET_CODE classify_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE on_classify_show_introduce(control_t *p_list, u16 msg, u32 para1, u32 para2);

enum classify_local_msg
{
  MSG_INTRODUCE_P = MSG_USER_BEGIN + 800,
  MSG_INTRODUCE_F,
};

static RET_CODE classify_title_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  u16 str_id[CLASSIFY_ITEM_CNT] = 
  {
    IDS_TV_FILM, IDS_NEWS, IDS_AMUSE, IDS_SPORT, IDS_CHILD,
    IDS_MUSIC, IDS_ART, IDS_SOCIAL, IDS_SCIENCE, IDS_HOBBIES, IDS_OTHER
  };
  gui_get_string(str_id[focus], p_str, 31);

  return SUCCESS;
}

static void classify_get_time_area(utc_time_t *p_start, utc_time_t *p_end)
{
  utc_time_t g_time = {0};
  
  time_get(&g_time, FALSE);
  
  memcpy(p_start, &g_time, sizeof(utc_time_t));
  memcpy(p_end, &g_time, sizeof(utc_time_t));

  time_up(p_end, 60);

}

static RET_CODE classify_get_class_level(control_t *p_cont, u8 *p_level)
{
  control_t *p_cassify_title = NULL;

  *p_level = 0;  
  p_cassify_title = ctrl_get_child_by_id(p_cont, IDC_CLASSIFY_TITLE);
  if(NULL == p_cassify_title)
  {
      return ERR_FAILURE;
  }

  switch(cbox_dync_get_focus(p_cassify_title))
  {
    case 0:
      *p_level = 0x1F;
      break;

    case 1:
      *p_level = 0x2F;
      break;

    case 2:
      *p_level = 0x3F;
      break;

    case 3:
      *p_level = 0x4F;
      break;

    case 4:
      *p_level = 0x5F;
      break;

    case 5:
      *p_level = 0x6F;
      break;

    case 6:
      *p_level = 0x7F;
      break;

    case 7:
      *p_level = 0x8F;
      break;

    case 8:
      *p_level = 0x9F;
      break;

    case 9:
      *p_level = 0xAF;
      break;

    default:
      *p_level = 0x00;
      break;
  }
  return SUCCESS;
}

static RET_CODE classify_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  evt_node_t *p_evt_node = NULL;
  u32 item_num = 0;
  u16 cnt = list_get_count(p_list);
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  control_t *p_cont = p_list->p_parent;
  u16 view_count, i, j, pg_id, pos;
  u8 level = 0;
  u8 view_id, temp, hasPaintCount = 0;
  BOOL bFirst = TRUE;
  dvbs_prog_node_t pg = {0};
  u8 ascstr[32];
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0};
  
  

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  #if 1
  classify_get_time_area(&s_time, &e_time);

  classify_get_class_level(p_cont, &level);
  
  for(j=0; j<view_count; j++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, j);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    
    nibble_query.service_id      = (u16)pg.s_id;;
    nibble_query.stream_id       = (u16)pg.ts_id;
    nibble_query.orig_network_id = (u16)pg.orig_net_id;  
    nibble_query.cont_level      = level;
    nibble_query.cnt             = 0;
    time_to_gmt(&s_time, &(nibble_query.start_time));
    time_to_gmt(&e_time, &(nibble_query.end_time));
    epg_query_by_nibble(class_get_handle_by_id(EPG_CLASS_ID), &nibble_query);
    item_num += nibble_query.cnt;

    if(nibble_query.cnt>0)
    {
      MT_ASSERT(nibble_query.p_result != NULL);
    }

    if(item_num>start && nibble_query.cnt>0)
    {
      if(bFirst)
      {
        for (i = 0; i < size; i++)
        {
          pos = i + start;
          if((pos<cnt) && (nibble_query.cnt-(item_num-start)+i<nibble_query.cnt))
          {
            p_evt_node = (evt_node_t *)(nibble_query.p_result[nibble_query.cnt-(item_num-start)+i]);

            if(p_evt_node != NULL)
            { 
              time_to_local(&(p_evt_node->start_time), &start_time);
              memcpy(&end_time, &start_time, sizeof(utc_time_t));
              time_add(&end_time, &(p_evt_node->drt_time));
              sprintf((char *)ascstr,"%.2d:%.2d-%.2d:%.2d", \
                start_time.hour, start_time.minute,\
                end_time.hour, end_time.minute);
              list_set_field_content_by_unistr(p_list, pos, 0, p_evt_node->event_name);
              list_set_field_content_by_ascstr(p_list, pos, 1, ascstr);
              list_set_field_content_by_unistr(p_list, pos, 2, pg.name);
              
              p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &classify_filter);
            }

            hasPaintCount++;
          }
        }
      }
      else
      {
        temp = hasPaintCount;
        for (i = temp; i < size; i++)
        {
          pos = i + start;
          if((pos<cnt) && (nibble_query.cnt-(item_num-start)+i<nibble_query.cnt))
          {
            p_evt_node = (evt_node_t *)(nibble_query.p_result[i-temp]);

            if(p_evt_node != NULL)
            { 
              time_to_local(&(p_evt_node->start_time), &start_time);
              memcpy(&end_time, &start_time, sizeof(utc_time_t));
              time_add(&end_time, &(p_evt_node->drt_time));
              sprintf((char *)ascstr,"%.2d:%.2d-%.2d:%.2d", \
                start_time.hour, start_time.minute,\
                end_time.hour, end_time.minute);
              list_set_field_content_by_unistr(p_list, pos, 0, p_evt_node->event_name);
              list_set_field_content_by_ascstr(p_list, pos, 1, ascstr);
              list_set_field_content_by_unistr(p_list, pos, 2, pg.name);
              
              p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &classify_filter);
            }

            hasPaintCount++;
          }
        }
      }
            
      bFirst = FALSE;
    }
  }
  #endif
  return SUCCESS;
}

static void classify_refresh_introduce(control_t *p_cont, BOOL is_paint)
{
  control_t *p_cassify_list = NULL;

  p_cassify_list = ctrl_get_child_by_id(p_cont, IDC_EVENT_LIST);
  
  on_classify_show_introduce(p_cassify_list, 0, is_paint, 0);
}
#if 0
{
  control_t *p_detail, *p_list;
  evt_node_t *p_evt_node = NULL;
  u32 item_num;
  u16 pos = 0;
  u16 i = 0;
  u16 count;
  p_list = ctrl_get_child_by_id(p_cont, IDC_EVENT_LIST);
  p_detail = ctrl_get_child_by_id(p_cont, IDC_PROG_DETAIL);

  count = list_get_count(p_list);

  if(count > 0)
  {
    pos = list_get_focus_pos(p_list);

    p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &classify_filter, &item_num);

    if(p_evt_node != NULL)
    {
      while((p_evt_node != NULL) && (i<item_num) && (i!=pos))
      {
        p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(EPG_CLASS_ID), p_evt_node, &classify_filter);
        i++;
      }
    }

    if(p_evt_node != NULL)
    {
      text_set_content_by_unistr(p_detail, p_evt_node->p_sht_txt);
    }
    else
    {
      text_set_content_by_unistr(p_detail, (u16*)"");
    }
  }
  else
  {
    text_set_content_by_unistr(p_detail, (u16*)"");
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_detail, TRUE);
  }
}
#endif

static RET_CODE on_classify_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;
  
  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list); 

  list_select_item(p_list, new_focus);
  list_draw_item_ext(p_list, old_focus, TRUE);
  list_draw_item_ext(p_list, new_focus, TRUE);

  classify_refresh_introduce(p_list->p_parent, TRUE);

  return SUCCESS;
}

static void classify_refresh_list(control_t *p_cont, BOOL is_paint )
{
  dvbs_prog_node_t pg = {0};
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0};  
  control_t *p_event_list;
  u32 item_num = 0;
  u8 level = 0;
  u8 view_id = 0;
  u16 view_count = 0;
  u16 pg_id = 0;
  u16 i = 0;

  memset(&pg, 0, sizeof(dvbs_prog_node_t));
  classify_get_class_level(p_cont, &level);
  
  p_event_list = ctrl_get_child_by_id(p_cont, IDC_EVENT_LIST);
#if 1
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  classify_get_time_area(&s_time, &e_time);
  
  memset(&nibble_query ,0,sizeof(epg_query_by_nibble_t));
  for(i=0; i<view_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    nibble_query.service_id      = (u16)pg.s_id;;
    nibble_query.stream_id       = (u16)pg.ts_id;
    nibble_query.orig_network_id = (u16)pg.orig_net_id;  
    nibble_query.cont_level      = level;
    nibble_query.cnt             = 0;
    time_to_gmt(&s_time, &(nibble_query.start_time));
    time_to_gmt(&e_time, &(nibble_query.end_time));
    epg_query_by_nibble(class_get_handle_by_id(EPG_CLASS_ID), &nibble_query);
    item_num += nibble_query.cnt;
  }
  
  if(item_num>0)
  {
    list_set_count(p_event_list, (u16)item_num, CLASSIFY_PROG_LIST_PAGE);
    list_set_focus_pos(p_event_list, 0);
    list_select_item(p_event_list, 0);
    classify_list_update(p_event_list, list_get_valid_pos(p_event_list), CLASSIFY_PROG_LIST_PAGE, 0);
  }
  else
  {
    list_set_count(p_event_list, 0, CLASSIFY_PROG_LIST_PAGE);
    list_set_focus_pos(p_event_list, 0);
  }
#endif
  if(is_paint)
  {
    ctrl_paint_ctrl(p_event_list, TRUE);
  }
}

static RET_CODE on_classify_list_change_classify(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = p_list->p_parent;
  control_t *p_classify_title;

  //update title
  p_classify_title = ctrl_get_child_by_id(p_cont, IDC_CLASSIFY_TITLE);
  cbox_class_proc(p_classify_title, msg, para1, para2);

  //update prog list
  classify_refresh_list(p_cont, TRUE);

  //update prog detail info
  classify_refresh_introduce(p_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_classify_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u32 item_num = 0;
  control_t *p_cont = p_list->p_parent, *p_cassify_cbox;
  u16 view_count, j;
  u8 level, view_id;
  dvbs_prog_node_t pg = {0};
  u16 list_index=0;
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0}; 

  list_index = list_get_focus_pos(p_list);
  p_cassify_cbox = ctrl_get_child_by_id(p_cont, IDC_CLASSIFY_TITLE);
  switch(cbox_dync_get_focus(p_cassify_cbox))
  {
    case 0:
      level = 0x1F;
      break;

    case 1:
      level = 0x2F;
      break;

    case 2:
      level = 0x3F;
      break;

    case 3:
      level = 0x4F;
      break;

    case 4:
      level = 0x5F;
      break;

    case 5:
      level = 0x6F;
      break;

    case 6:
      level = 0x7F;
      break;

    case 7:
      level = 0x8F;
      break;

    case 8:
      level = 0x9F;
      break;

    case 9:
      level = 0xAF;
      break;

    default:
      level = 0x00;
      break;
  }

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);

  classify_get_time_area(&s_time, &e_time);
  
  for(j=0; j<view_count; j++)
  {
    prog_id = db_dvbs_get_id_by_view_pos(view_id, j);
    db_dvbs_get_pg_by_id(prog_id, &pg);
    
    nibble_query.service_id      = (u16)pg.s_id;;
    nibble_query.stream_id       = (u16)pg.ts_id;
    nibble_query.orig_network_id = (u16)pg.orig_net_id;  
    nibble_query.cont_level      = level;
    nibble_query.cnt             = 0;
    time_to_gmt(&s_time, &(nibble_query.start_time));
    time_to_gmt(&e_time, &(nibble_query.end_time));
    epg_query_by_nibble(class_get_handle_by_id(EPG_CLASS_ID), &nibble_query);
    item_num += nibble_query.cnt;

    if(nibble_query.cnt>0)
    {
      MT_ASSERT(nibble_query.p_result != NULL);
      if(item_num >= (u32)(list_index+1))
      {
        ui_play_prog(prog_id, FALSE);
        ui_close_all_mennus();
        break;
      }
    }
  }

  return SUCCESS;
}


RET_CODE open_pro_classify(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ad_win, *p_title, *p_bartitle;//,*p_sbar
  control_t *classify_list_bg1, *p_small_bg2,*p_small_bg3 ,*p_list;
  u16 i;

  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_PRO_CLASSIFY_HELP_RSC_CNT]=
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

  list_xstyle_t classify_list_item_rstyle =
  {
      RSI_SN_BG,//RSI_COMM_LIST_N,
      RSI_SN_BG,//RSI_COMM_LIST_N,
      RSI_SN_SEARCH_BAR_FOCUS,//RSI_COMM_LIST_HL,
      RSI_SN_BG,//RSI_COMM_LIST_SEL,
      RSI_SN_SEARCH_BAR_FOCUS,//RSI_COMM_LIST_N,
  };
  list_xstyle_t classify_list_field_rstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  list_xstyle_t classify_list_field_fstyle =
  {
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_TXT_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
  };
  list_field_attr_t classify_list_attr[CLASSIFY_PROG_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      200, 10/*0*/, 0, &classify_list_field_rstyle,  &classify_list_field_fstyle},
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      140, 220/*210*/, 0, &classify_list_field_rstyle,  &classify_list_field_fstyle},
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      180, 370/*360*/, 0, &classify_list_field_rstyle,  &classify_list_field_fstyle},    
  };

  // create container
  p_cont = ui_background_create(ROOT_ID_PRO_CLASSIFY,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_PROGRAM_CLASSIFY, TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  ctrl_set_style(p_cont, STL_EX_WHOLE_HL);

  //advertisement window

  p_ad_win = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_AD_WINDOW,
                              SN_PROCLASSIFY_AD_X, SN_PROCLASSIFY_AD_Y,
                              SN_PROCLASSIFY_AD_W, SN_PROCLASSIFY_AD_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ad_win, RSI_SN_ADWIN, RSI_SN_ADWIN, RSI_SN_ADWIN);
  bmp_pic_draw(p_ad_win);

  //classify title
  p_title = ctrl_create_ctrl((u8 *)CTRL_CBOX, IDC_CLASSIFY_TITLE, 
  							SN_CLASSIFY_ITEM_X, SN_CLASSIFY_ITEM_Y,
  							SN_CLASSIFY_ITEM_W, SN_CLASSIFY_ITEM_H, p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SELECT_TITLE
  cbox_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  cbox_set_work_mode(p_title, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_title, TRUE);
  cbox_dync_set_count(p_title, CLASSIFY_ITEM_CNT);
  cbox_dync_set_update(p_title, classify_title_update);
  cbox_dync_set_focus(p_title, 0);

  //bar classify title arrow
  p_bartitle = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CLASSIFY_BAR_ARROWL,
                            SN_CLASSIFY_BAR_ARROW_LEFT_X, SN_CLASSIFY_BAR_ARROW_Y,
                            SN_CLASSIFY_BAR_ARROW_W, SN_CLASSIFY_BAR_ARROW_H,
                            p_title, 0);
  bmap_set_content_by_id(p_bartitle, IM_SN_ICON_ARROW_LEFT);

  p_bartitle = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CLASSIFY_BAR_ARROWR,
                            SN_CLASSIFY_BAR_ARROW_RIGHT_X, SN_CLASSIFY_BAR_ARROW_Y,
                            SN_CLASSIFY_BAR_ARROW_W, SN_CLASSIFY_BAR_ARROW_H,
                            p_title, 0);
  bmap_set_content_by_id(p_bartitle, IM_SN_ICON_ARROW_RIGHT);
  
  //classify prog list background
  classify_list_bg1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_LIST_BG,
                              SN_CLASSIFY_LIST_BG1_X, SN_CLASSIFY_LIST_BG1_Y,
                              SN_CLASSIFY_LIST_BG1_W, SN_CLASSIFY_LIST_BG1_H,
                              p_cont, 0);
  ctrl_set_rstyle(classify_list_bg1, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_TIP_BOX

  //classify_list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EVENT_LIST, 
  						SN_CLASSIFY_LIST_BG1_X, SN_CLASSIFY_LIST_BG1_Y, 
  						SN_CLASSIFY_LIST_LIST_W, SN_CLASSIFY_LIST_LIST_H, p_cont, 0);
  ctrl_set_keymap(p_list, classify_list_keymap);
  ctrl_set_proc(p_list, classify_list_proc);
  list_set_mid_rect(p_list, 
  					SN_CLASSIFY_LIST_MIDL, SN_CLASSIFY_LIST_MIDT,
                    SN_CLASSIFY_LIST_MIDW, SN_CLASSIFY_LIST_MIDH,
                    SN_CLASSIFY_LIST_VGAP);
  list_set_item_rstyle(p_list, &classify_list_item_rstyle);
  list_set_count(p_list, 0, CLASSIFY_PROG_LIST_PAGE);
  list_set_field_count(p_list, CLASSIFY_PROG_LIST_FIELD, CLASSIFY_PROG_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, classify_list_update, 0);
 
  for (i = 0; i < CLASSIFY_PROG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(classify_list_attr[i].attr), (u16)(classify_list_attr[i].width),
                        (u16)(classify_list_attr[i].left), (u8)(classify_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, classify_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, classify_list_attr[i].fstyle);
  }    

  //proinfo bg 2
  p_small_bg2 = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_PROG_DETAIL_BG,
                              SN_PRO_INFO_BG2_X, SN_PRO_INFO_BG2_Y,
                              SN_PRO_INFO_BG2_W, SN_PRO_INFO_BG2_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_small_bg2, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_TIP_BOX

  p_small_bg3 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_PROG_DETAIL,
                              5, 0,
                              SN_PRO_INFO_BG2_W - 10, SN_PRO_INFO_BG2_H,
                              p_small_bg2, 0);
  text_set_font_style(p_small_bg3, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_small_bg3, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_small_bg3, TEXT_STRTYPE_UNICODE);

  classify_refresh_introduce(p_cont, FALSE);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
#if 0
  ui_set_pic_show(FALSE);
  ui_pic_play(ADS_AD_TYPE_OTHER_GIF, ROOT_ID_PRO_CLASSIFY);
#endif

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); /* focus on prog_name */

  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt = SN_PRO_CLASSIFY_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_classify_show_introduce(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  evt_node_t *p_evt_node = NULL;
  u32 item_num = 0;
  u16 view_count, j, pg_id;
  u8 level = 0;
  u8 view_id = 0;
  dvbs_prog_node_t pg = {0};
  u16 list_index=0;
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0};
  epg_filter_t filter;
  u32 evt_cnt = 0;
  u16 uni_buf[256] = {0};
  control_t *p_detail_bg = NULL;
  control_t *p_detail= NULL;
  BOOL is_paint = (BOOL)para1;
  
  list_index = list_get_focus_pos(p_list);
  classify_get_class_level(p_list->p_parent, &level);

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);

  classify_get_time_area(&s_time, &e_time);
  
  for(j=0; j<view_count; j++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, j);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    
    nibble_query.service_id      = (u16)pg.s_id;
    nibble_query.stream_id       = (u16)pg.ts_id;
    nibble_query.orig_network_id = (u16)pg.orig_net_id;  
    nibble_query.cont_level      = level;
    nibble_query.cnt             = 0;
    time_to_gmt(&s_time, &(nibble_query.start_time));
    time_to_gmt(&e_time, &(nibble_query.end_time));
    epg_query_by_nibble(class_get_handle_by_id(EPG_CLASS_ID), &nibble_query);
    item_num += nibble_query.cnt;

    if(nibble_query.cnt>0)
    {
      MT_ASSERT(nibble_query.p_result != NULL);
      if(item_num >= (u32)(list_index+1))
      {
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
          uni_strncpy(uni_buf, p_evt_node->p_sht_txt, p_evt_node->sht_txt_len);
        } 
        break;
      }
    }
  }

  p_detail_bg = ctrl_get_child_by_id(p_list->p_parent,IDC_PROG_DETAIL_BG);
  p_detail = ctrl_get_child_by_id(p_detail_bg,IDC_PROG_DETAIL);
  if(NULL == p_detail)
  {
      return ERR_FAILURE;
  }
  
  text_set_content_by_unistr(p_detail, uni_buf);
  if(is_paint)
  {
    ctrl_paint_ctrl(p_detail_bg, TRUE);
    ctrl_paint_ctrl(p_detail, TRUE);
  }

  return SUCCESS;
}

#if 0
static RET_CODE on_prog_classify_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  ui_set_pic_show(TRUE);
  return ERR_NOFEATURE;
}
#endif

BEGIN_KEYMAP(classify_group_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)  
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  
END_KEYMAP(classify_group_keymap, NULL)

BEGIN_MSGPROC(classify_group_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_classify_list_change_classify)
END_MSGPROC(classify_group_proc, cbox_class_proc)


BEGIN_KEYMAP(classify_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)   
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_OK, MSG_SELECT) 
  ON_EVENT(V_KEY_F4, MSG_INTRODUCE_P)
  ON_EVENT(V_KEY_BLUE, MSG_INTRODUCE_P)
END_KEYMAP(classify_list_keymap, NULL)

BEGIN_MSGPROC(classify_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_classify_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_classify_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_classify_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_classify_list_change_focus)
  ON_COMMAND(MSG_INCREASE, on_classify_list_change_classify)
  ON_COMMAND(MSG_DECREASE, on_classify_list_change_classify)
  ON_COMMAND(MSG_SELECT, on_classify_list_ok)
  ON_COMMAND(MSG_INTRODUCE_P, on_classify_show_introduce)
END_MSGPROC(classify_list_proc, list_class_proc)

