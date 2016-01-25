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
#include "ui_nvod.h"
#include "ui_nvod_api.h"


enum nvod_control_id
{
  IDC_INVALID = 0,
  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_PROG_INFO,
  IDC_REF_SVC_LIST,
  IDC_REF_EVENT_LIST,
  IDC_SHIFT_EVENT_LIST,
  IDC_REMIND_STR,
};

static list_xstyle_t nvod_list_item_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_COMM_LIST_HL,
    RSI_IGNORE,
    RSI_IGNORE,
};

static list_xstyle_t nvod_list_field_fstyle =
{
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_TXT_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
};

static list_xstyle_t nvod_list_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_COMM_LIST_HL,
    RSI_IGNORE,
    RSI_IGNORE,
};
//global
static  BOOL is_shift_evt_found = FALSE;

static list_field_attr_t ref_svc_list_attr[REF_SVC_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    100, 0, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
};

static list_field_attr_t ref_event_list_attr[REF_EVENT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    50, 0, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120, 50, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
};

static list_field_attr_t shift_event_list_attr[SHIFT_EVENT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120, 0, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    90, 120, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    30, 230, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
};

u16 nvod_cont_keymap(u16 key);
RET_CODE nvod_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ref_svc_list_keymap(u16 key);
RET_CODE ref_svc_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ref_event_keymap(u16 key);
RET_CODE ref_event_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 shift_event_list_keymap(u16 key);
RET_CODE shift_event_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



static RET_CODE refresh_refrence_event_info(control_t* p_cont, BOOL is_paint)
{
  u32 i = 0;
  control_t* p_info = ctrl_get_child_by_id(p_cont, IDC_PROG_INFO);
  u32 ref_event_count = 0;
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_cont, IDC_REF_EVENT_LIST); 
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;

  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  if(p_ref_svc != NULL)
  {
    p_ref_event = nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    text_set_content_by_unistr(p_info, p_ref_event->p_sht_txt);
  }
  else
  {
    text_set_content_by_unistr(p_info, (u16*)"");
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_info, TRUE);
  }
  return SUCCESS;
}

static RET_CODE ref_svc_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_ctrl);
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      p_ref_svc = nvod_data_get_ref_svc(h_nvod, i+start);

      /* NAME */
      if(p_ref_svc != NULL)
      {
        list_set_field_content_by_unistr(p_ctrl, (u16)(start + i), 0, (u16*)p_ref_svc->name); //pg name
      }

    }
  }

  return SUCCESS;
}

static RET_CODE ref_event_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  u16 cnt = list_get_count(p_ctrl);
  u8 asc_str[8];

  u32 ref_event_count = 0;
  u32 i = 0;
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_REF_SVC_LIST);
  u16 ref_svc_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event_list = NULL;

  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }

  if(p_ref_svc != NULL)
  {
    p_ref_event_list = nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }

  if(p_ref_event_list != NULL)
  {
    for (i = 0; i<ref_event_count; i++)
    {
      if((p_ref_event_list == NULL) || (i == start))
      {
        break;
      }
      
      p_ref_event_list = nvod_data_get_ref_evt_next(h_nvod, p_ref_event_list);
    }
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      if(p_ref_event_list != NULL)
      {
        list_set_field_content_by_unistr(p_ctrl, (u16)(start + i), 1, p_ref_event_list->event_name);
        
        p_ref_event_list = nvod_data_get_ref_evt_next(h_nvod, p_ref_event_list);
      }
      else
      {
        break;
      }
    }
  }

  return SUCCESS;
}

static RET_CODE shift_event_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  u16 cnt = list_get_count(p_ctrl);
  u8 asc_str[64];
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  u32 i = 0;
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_REF_EVENT_LIST); 
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_REF_SVC_LIST);
  u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;
  book_pg_t book_node = {0};

  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  if(p_ref_svc != NULL)
  {
    p_ref_event = nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  if(p_shift_event != NULL)
  {
    for (i = 0; i<start; i++)
    {
      if(p_shift_event == NULL)
      {
        break;
      }
      
      p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
    }
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      if(p_shift_event != NULL)
      {
        memset(asc_str, 0, sizeof(asc_str));
        time_to_local(&(p_shift_event->start_time), &start_time);
        memcpy(&end_time, &start_time, sizeof(utc_time_t));
        time_add(&end_time, &p_shift_event->drt_time);
        sprintf((char *)asc_str, "%02d:%02d-%02d:%02d",start_time.hour,start_time.minute,end_time.hour,end_time.minute);
               
        list_set_field_content_by_ascstr(p_ctrl, (u16)(start + i), 0, asc_str);

        if(p_shift_event->evt_status)
        {
          list_set_field_content_by_strid(p_ctrl, (u16)(start + i), 1, IDS_NVOD_IS_PLAYING);
        }
        else
        {
          list_set_field_content_by_strid(p_ctrl, (u16)(start + i), 1, IDS_NVOD_NOT_PLAYING);
        }

        p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);
        book_node.pgid = ui_get_timeshift_svc_pgid(p_ts_svc);
        book_node.svc_type = SVC_TYPE_NVOD_REFRENCE;
        book_node.timer_mode = BOOK_TMR_ONCE;  
        time_to_local(&(p_shift_event->start_time), &(book_node.start_time));
        memcpy(&(book_node.drt_time), &(p_shift_event->drt_time), sizeof(utc_time_t)); 
        memcpy(&(book_node.event_name), &(p_ref_event->event_name), sizeof(p_ref_event->event_name));
        
        if(book_get_match_node(&book_node) < MAX_BOOK_PG)
        {
          list_set_field_content_by_icon(p_ctrl, (u16)(start + i), 2, IM_ICON_TIME);
        }
        else
        {
          list_set_field_content_by_icon(p_ctrl, (u16)(start + i), 2, 0);
        }
        
        p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
      }
      else
      {
        break;
      }
    }
  }
 
  return SUCCESS;
}


static RET_CODE refresh_shift_event_list(control_t* p_cont, BOOL is_paint)
{
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  u16 cur_focus = 0;
  u32 i = 0;
  control_t* p_shift_event_list = ctrl_get_child_by_id(p_cont, IDC_SHIFT_EVENT_LIST);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_cont, IDC_REF_EVENT_LIST);
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  u16 ref_svc_index = 0;
  u16 ref_event_index = 0;

  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  if(p_ref_svc != NULL)
  {
    p_ref_event = nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }

  if(p_ref_event != NULL)
  {
    for(i=0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }
  
  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  if(p_shift_event == NULL)
  {
    shift_event_count = 0;
  }

  cur_focus = list_get_focus_pos(p_shift_event_list);
  cur_focus = (cur_focus == LIST_INVALID_FOCUS)?0:cur_focus;
  cur_focus = (cur_focus<shift_event_count)?cur_focus:0;
  
  list_set_count(p_shift_event_list, (u16)shift_event_count, NVOD_LIST_PAGE);
  list_set_focus_pos(p_shift_event_list, cur_focus);
  list_select_item(p_shift_event_list, cur_focus);
  shift_event_list_update(p_shift_event_list, list_get_valid_pos(p_shift_event_list), NVOD_LIST_PAGE, 0);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_shift_event_list, TRUE);
  }
  
  return SUCCESS;
}

static RET_CODE refresh_refrence_event_list(control_t* p_cont, BOOL is_paint)
{
  u32 ref_event_count = 0;
  u16 cur_focus = 0; 
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_cont, IDC_REF_EVENT_LIST);
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  u16 ref_svc_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;

  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }

  if(p_ref_svc != NULL)
  {
    nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }
  
  cur_focus = list_get_focus_pos(p_ref_event_list);
  cur_focus = (cur_focus == LIST_INVALID_FOCUS)?0:cur_focus;
  cur_focus = (cur_focus<ref_event_count)?cur_focus:0;
  
  list_set_count(p_ref_event_list, (u16)ref_event_count, NVOD_LIST_PAGE);
  list_set_focus_pos(p_ref_event_list, cur_focus);
  list_select_item(p_ref_event_list, cur_focus);
  ref_event_list_update(p_ref_event_list, list_get_valid_pos(p_ref_event_list), NVOD_LIST_PAGE, 0);

  refresh_refrence_event_info(p_cont, is_paint);

  refresh_shift_event_list(p_cont, is_paint);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_ref_event_list, TRUE);
  }
  
  return SUCCESS;
}


static RET_CODE refresh_refrence_service_list(control_t* p_cont, BOOL is_paint)
{
  u32 ref_svc_count = 0;
  u16 cur_focus = 0;
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  
  ref_svc_count = nvod_data_get_ref_svc_cnt(class_get_handle_by_id(NVOD_CLASS_ID));

  cur_focus = list_get_focus_pos(p_ref_svc_list);
  cur_focus = (cur_focus == LIST_INVALID_FOCUS)?0:cur_focus;
  cur_focus = (cur_focus<ref_svc_count)?cur_focus:((u16)ref_svc_count-1);
  
  list_set_count(p_ref_svc_list, (u16)ref_svc_count, NVOD_LIST_PAGE);
  list_set_focus_pos(p_ref_svc_list, cur_focus);
  list_select_item(p_ref_svc_list, cur_focus);

  ref_svc_list_update(p_ref_svc_list, list_get_valid_pos(p_ref_svc_list), NVOD_LIST_PAGE, 0);

  refresh_refrence_event_list(p_cont, is_paint);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_ref_svc_list, TRUE);
  }
  
  return SUCCESS;
}

RET_CODE open_nvod(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_info, *p_title;
  //control_t *p_ref_svc_bg, *p_ref_event_bg, *p_shift_event_bg;
  control_t *p_ref_svc_list, *p_ref_event_list, *p_shift_event_list;
  u16 i;
  full_screen_title_t title_data = {IM_COMMON_BANNER_CYCLE,IDS_NVOD, RSI_TITLE_BG};
  
  ui_nvod_enter();
  is_shift_evt_found = FALSE;
  // create container
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_NVOD,
                             RSI_FULL_SCREEN_BG,
                            NVOD_CONT_X, NVOD_CONT_Y,
                             NVOD_CONT_W, NVOD_CONT_H,
                             &title_data);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, nvod_cont_keymap);
  ctrl_set_proc(p_cont, nvod_cont_proc);
  
  //title 1 prog classify
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_TITLE1,
                              NVOD_TITLE_ITEM_X, NVOD_TITLE_ITEM_Y,
                              NVOD_TITLE_ITEM_1_W, NVOD_TITLE_ITEM_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_align_type(p_title, STL_CENTER|STL_VCENTER);
  text_set_font_style(p_title, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_PROGRAM_CLASSIFY);

  //title 2 prog name
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_TITLE2,
                            NVOD_TITLE_ITEM_X+NVOD_TITLE_ITEM_1_W+3, NVOD_TITLE_ITEM_Y,
                            NVOD_TITLE_ITEM_2_W, NVOD_TITLE_ITEM_H, 
                            p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_align_type(p_title, STL_CENTER|STL_VCENTER);
  text_set_font_style(p_title, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_PROGRAM_NAME);
  
  //title 3 play time
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_TITLE3,
                            NVOD_TITLE_ITEM_X+NVOD_TITLE_ITEM_1_W+3+NVOD_TITLE_ITEM_2_W+4, 
                            NVOD_TITLE_ITEM_Y,
                            WHOLE_SCR_TITLE_W -( NVOD_TITLE_ITEM_1_W+3+NVOD_TITLE_ITEM_2_W+4), 
                            NVOD_TITLE_ITEM_H, 
                            p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_align_type(p_title, STL_CENTER|STL_VCENTER);
  text_set_font_style(p_title, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_BROADCAST_TIME);
  
  //reference service list
  p_ref_svc_list =ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_REF_SVC_LIST, 
                  REF_SVC_LIST_X, REF_SVC_LIST_Y,
                  REF_SVC_LIST_W, REF_SVC_LIST_H, p_cont,
                  0);
  ctrl_set_rstyle(p_ref_svc_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_ref_svc_list, ref_svc_list_keymap);
  ctrl_set_proc(p_ref_svc_list, ref_svc_list_proc);
  list_set_mid_rect(p_ref_svc_list, REF_SVC_LIST_MIDL, REF_SVC_LIST_MIDT,
                    REF_SVC_LIST_MIDW, REF_SVC_LIST_MIDH,
                    REF_SVC_LIST_VGAP);
  list_set_item_rstyle(p_ref_svc_list, &nvod_list_item_rstyle);
  list_enable_select_mode(p_ref_svc_list, TRUE);
  list_set_select_mode(p_ref_svc_list, LIST_SINGLE_SELECT);
  list_set_count(p_ref_svc_list, 0, NVOD_LIST_PAGE);
  list_set_field_count(p_ref_svc_list, REF_SVC_LIST_FIELD, NVOD_LIST_PAGE);
  list_set_focus_pos(p_ref_svc_list, 0);
  list_select_item(p_ref_svc_list, 0);
  list_set_update(p_ref_svc_list, ref_svc_list_update, 0);
 
  for (i = 0; i < REF_SVC_LIST_FIELD; i++)
  {
    list_set_field_attr(p_ref_svc_list, (u8)i, (u32)(ref_svc_list_attr[i].attr),
                       (u16)(ref_svc_list_attr[i].width),
                       (u16)(ref_svc_list_attr[i].left), (u8)(ref_svc_list_attr[i].top));
    list_set_field_rect_style(p_ref_svc_list, (u8)i, ref_svc_list_attr[i].rstyle);
    list_set_field_font_style(p_ref_svc_list, (u8)i, ref_svc_list_attr[i].fstyle);
  }
  //ref_svc_list_update(p_ref_svc_list, list_get_valid_pos(p_ref_svc_list), NVOD_LIST_PAGE, 0);

  //reference event list
  p_ref_event_list =ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_REF_EVENT_LIST, REF_EVENT_LIST_X, REF_EVENT_LIST_Y,
              REF_EVENT_LIST_W, REF_EVENT_LIST_H, p_cont,
              0);
  ctrl_set_rstyle(p_ref_event_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_ref_event_list, ref_event_keymap);
  ctrl_set_proc(p_ref_event_list, ref_event_proc);
  list_set_mid_rect(p_ref_event_list, REF_EVENT_LIST_MIDL, REF_EVENT_LIST_MIDT,
                REF_EVENT_LIST_MIDW, REF_EVENT_LIST_MIDH,
                REF_SVC_LIST_VGAP);
  list_set_item_rstyle(p_ref_event_list, &nvod_list_item_rstyle);
  list_enable_select_mode(p_ref_event_list, TRUE);
  list_set_select_mode(p_ref_event_list, LIST_SINGLE_SELECT);
  list_set_count(p_ref_event_list, 0, NVOD_LIST_PAGE);
  list_set_field_count(p_ref_event_list, REF_EVENT_LIST_FIELD, NVOD_LIST_PAGE);
  list_select_item(p_ref_event_list,0);
  list_set_focus_pos(p_ref_event_list,0);
  list_set_update(p_ref_event_list, ref_event_list_update, 0);
 
  for (i = 0; i < REF_EVENT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_ref_event_list, (u8)i, (u32)(ref_event_list_attr[i].attr),
                       (u16)(ref_event_list_attr[i].width),
                       (u16)(ref_event_list_attr[i].left), (u8)(ref_event_list_attr[i].top));
    list_set_field_rect_style(p_ref_event_list, (u8)i, ref_event_list_attr[i].rstyle);
    list_set_field_font_style(p_ref_event_list, (u8)i, ref_event_list_attr[i].fstyle);
  }
  //ref_event_list_update(p_ref_event_list, list_get_valid_pos(p_ref_event_list), NVOD_LIST_PAGE, 0);
 
  //shift event list
  p_shift_event_list =ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_SHIFT_EVENT_LIST, SHIFT_EVENT_LIST_X, SHIFT_EVENT_LIST_Y,
                 SHIFT_EVENT_LIST_W, SHIFT_EVENT_LIST_H, p_cont,
                 0);
  ctrl_set_rstyle(p_shift_event_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_shift_event_list, shift_event_list_keymap);
  ctrl_set_proc(p_shift_event_list, shift_event_list_proc);
  list_set_mid_rect(p_shift_event_list, SHIFT_EVENT_LIST_MIDL, SHIFT_EVENT_LIST_MIDT,
                   SHIFT_EVENT_LIST_MIDW, SHIFT_EVENT_LIST_MIDH,
                   REF_SVC_LIST_VGAP);
  list_set_item_rstyle(p_shift_event_list, &nvod_list_item_rstyle);
  list_enable_select_mode(p_shift_event_list, TRUE);
  list_set_select_mode(p_shift_event_list, LIST_SINGLE_SELECT);
  list_set_count(p_shift_event_list, 0, NVOD_LIST_PAGE);
  list_set_field_count(p_shift_event_list, SHIFT_EVENT_LIST_FIELD, NVOD_LIST_PAGE);
  list_set_focus_pos(p_shift_event_list, 0);
  list_select_item(p_shift_event_list, 0);
  list_set_update(p_shift_event_list, shift_event_list_update, 0);
  
  for (i = 0; i < SHIFT_EVENT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_shift_event_list, (u8)i, (u32)(shift_event_list_attr[i].attr),
                      (u16)(shift_event_list_attr[i].width),
                      (u16)(shift_event_list_attr[i].left), (u8)(shift_event_list_attr[i].top));
    list_set_field_rect_style(p_shift_event_list, (u8)i, shift_event_list_attr[i].rstyle);
    list_set_field_font_style(p_shift_event_list, (u8)i, shift_event_list_attr[i].fstyle);
  }

  //detail info
  p_info = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_PROG_INFO,
                              NVOD_INFO_X, NVOD_INFO_Y,
                              NVOD_INFO_W, NVOD_INFO_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_info, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_mid_rect(p_info, 10, 10, NVOD_INFO_W-20, NVOD_INFO_H-20);
  text_set_align_type(p_info, STL_LEFT | STL_TOP);
  text_set_font_style(p_info, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_info, TEXT_STRTYPE_UNICODE);
 
  //shift_event_list_update(p_shift_event_list, list_get_valid_pos(p_shift_event_list), NVOD_LIST_PAGE, 0);
  // receive data remind string
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_REMIND_STR,
                              NVOD_REMIND_STR_X, NVOD_REMIND_STR_Y,
                              NVOD_REMIND_STR_W, NVOD_TITLE_ITEM_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_title, STL_CENTER|STL_VCENTER);
  text_set_font_style(p_title, FSI_COMM_TXT_N,FSI_COMM_TXT_N,FSI_COMM_TXT_N);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_RECIEVING_DATA);
  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  refresh_refrence_service_list(p_cont, FALSE);

  ui_comm_help_create(&nvod_help_data, p_cont);
  
  ctrl_default_proc(p_ref_svc_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}
static void nvod_menu_exit(void)
{
  ui_nvod_exit();
  manage_close_menu(ROOT_ID_NVOD, 0, 0);
}
static void nvod_menu_exit_all(void)
{
  ui_nvod_exit();
  ui_close_all_mennus();
}

static RET_CODE on_ref_svc_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  
  if(!is_shift_evt_found)
  {
      return ERR_FAILURE;
  }
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST);
      break;
      
    case MSG_FOCUS_LEFT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_SHIFT_EVENT_LIST);
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, 0, 0);
      refresh_refrence_service_list(p_list->p_parent, TRUE);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_ref_event_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_SHIFT_EVENT_LIST);
      break;
      
    case MSG_FOCUS_LEFT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_REF_SVC_LIST);
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, 0, 0);
      refresh_refrence_event_list(p_list->p_parent, TRUE);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}


static RET_CODE on_shift_event_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_REF_SVC_LIST);
      break;
      
    case MSG_FOCUS_LEFT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST);
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, 0, 0);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_shift_event_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = p_list->p_parent;
  u32 i = 0;
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  u32 shift_event_index = 0;
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_cont, IDC_REF_EVENT_LIST); 
  control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;
  book_pg_t book_node;
  
  ref_svc_index = list_get_focus_pos(p_ref_svc_list);

  if(nvod_data_get_ref_svc_cnt(h_nvod) > 0)
  {
    p_ref_svc = nvod_data_get_ref_svc(h_nvod, ref_svc_index);
  }
  else
  {
      OS_PRINTF("**** nvod_data_get_ref_svc_cnt error*******\r\n");
      return ERR_FAILURE;
  }
  ref_event_index = list_get_focus_pos(p_ref_event_list);

  if(p_ref_svc != NULL)
  {
    p_ref_event = nvod_data_get_ref_evt(h_nvod, p_ref_svc, &ref_event_count);
  }

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  shift_event_index = list_get_focus_pos(p_list);
  
  if(p_shift_event != NULL)
  {
    for (i = 0; (i<shift_event_count)&&(i<shift_event_index); i++)
    {
      if(p_shift_event == NULL)
      {
        break;
      }
      
      p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
    }
  }

  if(p_shift_event != NULL)
  {
    if(p_shift_event->evt_status == 1)
    {
      if(ui_is_pause())
      {
          ui_set_pause(FALSE);
      }
      p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);

      ui_set_playing_nvod_reference_svc_evt(p_ref_event);
      ui_set_playing_nvod_time_shifted_svc_evt(p_shift_event);
      
      avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
      ui_play_nvod(p_ts_svc, &p_ref_svc->tp);
      
      return manage_open_menu(ROOT_ID_NVOD_VIDEO, 0, 0);
    }
    else
    {
      p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);
      book_node.pgid = ui_get_timeshift_svc_pgid(p_ts_svc);
      book_node.svc_type = SVC_TYPE_NVOD_REFRENCE;
      book_node.timer_mode = BOOK_TMR_ONCE;  
      time_to_local(&(p_shift_event->start_time), &(book_node.start_time));
      memcpy(&(book_node.drt_time), &(p_shift_event->drt_time), sizeof(utc_time_t)); 
      memcpy(&(book_node.event_name), &(p_ref_event->event_name), sizeof(p_ref_event->event_name));

      switch(check_book_pg(&book_node))
      {
        case BOOK_ERR_DUR_TOO_SHORT:
          ui_comm_cfmdlg_open(NULL, IDS_LESS_ONE_MIN, NULL, 3000); 
          break;

        case BOOK_ERR_PLAYING:
          ui_comm_cfmdlg_open(NULL, IDS_CUR_EVENT_IS_PLAYING, NULL, 3000); 
          break;

        case BOOK_ERR_CONFILICT:
          ui_comm_cfmdlg_open(NULL, IDS_BOOK_CONFLICT, NULL, 3000); 
          break;
        case BOOK_ERR_SAME:
          book_delete_node(book_get_match_node(&book_node));
          list_set_field_content_by_icon(p_list, (u16)shift_event_index, 2, 0);
          list_draw_field_ext(p_list, (u16)shift_event_index, 2, TRUE);
          break;

        case BOOK_ERR_FULL:
          ui_comm_cfmdlg_open(NULL, IDS_BOOK_IS_FULL, NULL, 3000);
          break;

        case BOOK_ERR_NO:  //book success
          list_set_field_content_by_icon(p_list, (u16)shift_event_index, 2, IM_ICON_TIME);
          list_draw_field_ext(p_list, (u16)shift_event_index, 2, TRUE);
          book_add_node(&book_node);
          break;
        default:
          break;
      }
    }
  }

  return SUCCESS;
}


static RET_CODE on_nvod_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{

  switch(msg)
  {    
    case MSG_TO_MAINMENU:
    case MSG_EXIT:
      ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_NVOD_MENU, 
                                   nvod_menu_exit, NULL, 0);
      break;
    case MSG_EXIT_ALL:
      ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_NVOD_MENU, 
                                   nvod_menu_exit_all, NULL, 0);
      break;
    default:
      break;
  }
  
  return SUCCESS;
}

static RET_CODE on_nvod_refresh(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  control_t *p_CtrlRemind = NULL;
  OS_PRINTF("\r\n----received refresh msg[0x%x],ticks[0x%x]---", msg, mtos_ticks_get());

  switch(msg)
  {
    case MSG_NVOD_REF_SVC_FOUND:  //0x6064
      refresh_refrence_service_list(p_cont, (is_shift_evt_found ? TRUE : FALSE));
      break;      
    case MSG_NVOD_REF_EVT_FOUND:  //0x6065
      refresh_refrence_event_list(p_cont, (is_shift_evt_found ? TRUE : FALSE));
      break;      
    case MSG_NVOD_SHIFT_EVT_FOUND: //0x6066
      refresh_shift_event_list(p_cont, (is_shift_evt_found ? TRUE : FALSE));
      if(!is_shift_evt_found)
      {
          is_shift_evt_found = TRUE;
          p_CtrlRemind = ctrl_get_child_by_id(p_cont, IDC_REMIND_STR);
          if(NULL != p_CtrlRemind)
          {
            ctrl_set_attr(p_CtrlRemind, OBJ_ATTR_HIDDEN);
          }
          ctrl_paint_ctrl(p_cont, TRUE);
      }
      break;      
    case MSG_NVOD_SVC_SCAN_END: //0x6067
      if(nvod_data_get_ref_svc_cnt(h_nvod))
      {
        p_ref_svc = nvod_data_get_ref_svc(h_nvod, 0);
        ui_nvod_start_ref_svc(&p_ref_svc->tp);
      }
      else
      {
          p_CtrlRemind = ctrl_get_child_by_id(p_cont, IDC_REMIND_STR);
          if(NULL != p_CtrlRemind)
          {
            text_set_content_by_strid(p_CtrlRemind, IDS_NO_NVOD);
            ctrl_paint_ctrl(p_CtrlRemind, TRUE);
          }
      }
      break;    
    default:
      break;
  }
  
  return SUCCESS;
}

BEGIN_KEYMAP(nvod_cont_keymap, ui_comm_root_keymap) 
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(nvod_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(nvod_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_nvod_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_nvod_exit)
  ON_COMMAND(MSG_TO_MAINMENU, on_nvod_exit)
  ON_COMMAND(MSG_NVOD_SVC_SCAN_END, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_REF_SVC_FOUND, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_REF_EVT_FOUND, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_SHIFT_EVT_FOUND, on_nvod_refresh)
END_MSGPROC(nvod_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(ref_svc_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ref_svc_list_keymap, NULL)

BEGIN_MSGPROC(ref_svc_list_proc, list_class_proc)
   ON_COMMAND(MSG_FOCUS_RIGHT, on_ref_svc_list_msg)
   ON_COMMAND(MSG_FOCUS_LEFT, on_ref_svc_list_msg)
   ON_COMMAND(MSG_FOCUS_UP, on_ref_svc_list_msg)
   ON_COMMAND(MSG_FOCUS_DOWN, on_ref_svc_list_msg)
   ON_COMMAND(MSG_PAGE_UP, on_ref_svc_list_msg)
   ON_COMMAND(MSG_PAGE_DOWN, on_ref_svc_list_msg)
END_MSGPROC(ref_svc_list_proc, list_class_proc)

BEGIN_KEYMAP(ref_event_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ref_event_keymap, NULL)

BEGIN_MSGPROC(ref_event_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_LEFT, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_UP, on_ref_event_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, on_ref_event_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_ref_event_list_msg)
END_MSGPROC(ref_event_proc, list_class_proc)

BEGIN_KEYMAP(shift_event_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(shift_event_list_keymap, NULL)

BEGIN_MSGPROC(shift_event_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_LEFT, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_UP, on_shift_event_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, on_shift_event_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_shift_event_list_msg)
  ON_COMMAND(MSG_SELECT, on_shift_event_list_ok)
END_MSGPROC(shift_event_list_proc, list_class_proc)


