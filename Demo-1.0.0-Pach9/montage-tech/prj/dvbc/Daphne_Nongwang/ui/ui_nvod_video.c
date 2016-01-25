/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
   
#include "ui_nvod_video.h"
#include "ui_nvod_api.h"
#include "ui_mute.h"
#include "ui_do_search.h"

enum nvod_video_local_msg
{
  MSG_ONE_SECOND = MSG_LOCAL_BEGIN + 400,
};

enum nvod_video_ctrl_id
{
  IDC_INVALID = 0,
  IDC_NVOD_CONT,
  IDC_NVOD_TIME,
  IDC_NVOD_PLAY_INFO_CONT,
  IDC_NVOD_TITLE,
  IDC_NVOD_STR,
  
  IDC_NVOD_NAME,
  IDC_NVOD_START,
  IDC_NVOD_END,
  IDC_NVOD_PLAY_PROGRESS,

  IDC_NVOD_NAME_CONTENT,
  IDC_NVOD_STIME_CONTENT,
  IDC_NVOD_ETIME_CONTENT,
  IDC_NVOD_PLAY_PROGRESS_CONTENT,
};
static BOOL g_is_nvod_play_book = FALSE;
static utc_time_t left_time = {0};
static u32 g_past_second = 0;
static book_pg_t nvod_book_node;
u16 nvod_full_play_cont_keymap(u16 key);
RET_CODE nvod_full_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 nvod_full_play_text_keymap(u16 key);
RET_CODE nvod_full_play_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static RET_CODE nvod_video_refresh_info(control_t *p_cont, BOOL is_paint)
{
  control_t *p_play_info_cont, *p_ctrl;
  nvod_reference_svc_evt_t *p_ref_svc_evt = NULL;
  time_shifted_svc_evt_t *p_ts_svc_evt = NULL;
  u8 asc_str[32];
  u32 temp_data = 0;
  utc_time_t temp_time;

  if(!g_is_nvod_play_book)
  {
    p_ref_svc_evt = ui_get_playing_nvod_reference_svc_evt();
    p_ts_svc_evt = ui_get_playing_nvod_time_shifted_svc_evt();

    p_play_info_cont = ctrl_get_child_by_id(p_cont, IDC_NVOD_PLAY_INFO_CONT);

    if(p_ref_svc_evt != NULL && p_ts_svc_evt != NULL)
    {
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_NAME_CONTENT);
      text_set_content_by_ascstr(p_ctrl, (u8*)p_ref_svc_evt->event_name);
      
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_STIME_CONTENT);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      time_to_local(&(p_ts_svc_evt->start_time), &temp_time);
      sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
      text_set_content_by_ascstr(p_ctrl, asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_ETIME_CONTENT);
      time_to_local(&(p_ts_svc_evt->start_time), &temp_time);
      time_add(&temp_time, &p_ts_svc_evt->drt_time);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
      text_set_content_by_ascstr(p_ctrl, asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_PLAY_PROGRESS_CONTENT);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      if(time_conver(&p_ts_svc_evt->drt_time)>0)
      {
        temp_data = g_past_second*100/time_conver(&p_ts_svc_evt->drt_time);
        sprintf((char *)asc_str, "%ld%%", temp_data);
      }
      text_set_content_by_ascstr(p_ctrl, asc_str);
    }
  }
  else
  {
    p_play_info_cont = ctrl_get_child_by_id(p_cont, IDC_NVOD_PLAY_INFO_CONT);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_NAME_CONTENT);
    text_set_content_by_ascstr(p_ctrl, (u8*)nvod_book_node.event_name);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_STIME_CONTENT);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    time_to_local(&(nvod_book_node.start_time), &temp_time);
    sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
    text_set_content_by_ascstr(p_ctrl, asc_str);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_ETIME_CONTENT);
    time_to_local(&(nvod_book_node.start_time), &temp_time);
    time_add(&temp_time, &nvod_book_node.drt_time);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
    text_set_content_by_ascstr(p_ctrl, asc_str);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_PLAY_PROGRESS_CONTENT);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    if(time_conver(&nvod_book_node.drt_time)>0)
    {
      temp_data = g_past_second*100/time_conver(&nvod_book_node.drt_time);
      sprintf((char *)asc_str, "%ld%%", temp_data);
    }
    text_set_content_by_ascstr(p_ctrl, asc_str);
  }
  
  if(is_paint)
  {
    ctrl_paint_ctrl(p_play_info_cont, TRUE);
  }
  
  return SUCCESS;
}

RET_CODE open_nvod_video(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont,*p_info_cont,*p_mask,*p_time, *p_title,*p_title1;
  u16 i;
  u32 total_left_second = 0;
  utc_time_t end_time = {0};
  u16 strid[] = 
    {
      IDS_PROGRAM_NAME2, IDS_START_TIME2, IDS_END_TIME, IDS_NVOD_PLAY_PROGRESS
    };
  utc_time_t cur_time = {0};
  time_shifted_svc_evt_t *p_ts_svc_evt = NULL;
  
  g_is_nvod_play_book = (BOOL)para1;

  if(!g_is_nvod_play_book)
  {
    p_ts_svc_evt = ui_get_playing_nvod_time_shifted_svc_evt();

    time_get(&cur_time,TRUE);
     
    //g_past_second = time_conver(&cur_time) - time_conver(&p_ts_svc_evt->start_time);
    g_past_second = time_dec(&cur_time, &p_ts_svc_evt->start_time);

    if(g_past_second<0)
    {
      g_past_second = 0;
    }

    memset(&left_time, 0, sizeof(utc_time_t));
    memcpy(&end_time, &p_ts_svc_evt->start_time, sizeof(utc_time_t));
    time_add(&end_time, &p_ts_svc_evt->drt_time);
    total_left_second = time_dec(&end_time, &cur_time);
    left_time.hour = (u8)(total_left_second / 3600);
    left_time.minute = (u8)((total_left_second % 3600)/60);
    left_time.second = (u8)((total_left_second % 60));
  }
  else
  {
    memcpy((void *)&nvod_book_node, (void *)para2, sizeof(book_pg_t));
    g_past_second = 0;
    memset(&left_time, 0, sizeof(utc_time_t));
    memcpy(&left_time, &nvod_book_node.drt_time, sizeof(utc_time_t));
  }
  
  p_mask = ui_comm_root_create(ROOT_ID_NVOD_VIDEO,
                             RSI_IGNORE,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             0, RSI_IGNORE);
  

  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_NVOD_CONT, NVOD_VIDEO_CONT_X,
                            NVOD_VIDEO_CONT_Y, NVOD_VIDEO_CONT_W, NVOD_VIDEO_CONT_H, p_mask, 0);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, nvod_full_play_cont_keymap);
  ctrl_set_proc(p_cont, nvod_full_play_cont_proc);
  
  
  //time
  p_time = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_NVOD_TIME,
                              NVOD_VIDEO_TIME_X, NVOD_VIDEO_TIME_Y,
                              NVOD_VIDEO_TIME_W, NVOD_VIDEO_TIME_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_time, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_align_type(p_time , STL_VCENTER|STL_VCENTER);
  text_set_font_style(p_time , FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_time , TEXT_STRTYPE_UNICODE);
  //text_set_content_by_strid(p_time, IDS_TIME_SET);

  //
  p_info_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_NVOD_PLAY_INFO_CONT,
                             NVOD_VIDEO_X, NVOD_VIDEO_Y,
                             NVOD_VIDEO_W, NVOD_VIDEO_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_info_cont, RSI_INFOBOX, RSI_INFOBOX, RSI_INFOBOX);
  ctrl_set_keymap(p_info_cont, nvod_full_play_text_keymap);
  ctrl_set_proc(p_info_cont, nvod_full_play_text_proc);
  ctrl_set_attr(p_info_cont,OBJ_ATTR_HIDDEN);
 
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NVOD_TITLE,
                             NVOD_TITLE_X, NVOD_TITLE_Y,
                             NVOD_TITLE_W, NVOD_TITLE_H,
                             p_info_cont, 0);
  ctrl_set_rstyle(p_title, RSI_DLG_TITLE,RSI_DLG_TITLE, RSI_DLG_TITLE);

  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NVOD_STR,
                             NVOD_TITLE_STR_X, NVOD_TITLE_STR_Y,
                             NVOD_TITLE_STR_W, NVOD_TITLE_STR_H,
                             p_info_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_title, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_NVOD2);
  
  for(i=0; i<4; i++)
  {
    p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (IDC_NVOD_NAME+i),
                               NVOD_TITLE_NAME_X, (NVOD_TITLE_NAME_Y+i*NVOD_TITLE_NAME_H+2),
                               NVOD_TITLE_NAME_W, NVOD_TITLE_NAME_H,
                               p_info_cont, 0);
    ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_title, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_title, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_title, strid[i]);

    p_title1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (IDC_NVOD_NAME_CONTENT+i),
                               NVOD_TITLE_SCREEN_X, (NVOD_TITLE_SCREEN_Y+i*NVOD_TITLE_SCREEN_H+2),
                               NVOD_TITLE_SCREEN_W, NVOD_TITLE_SCREEN_H,
                               p_info_cont, 0);
    ctrl_set_rstyle(p_title1, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_title1, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_title1, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_title1, TEXT_STRTYPE_UNICODE);
  }

  nvod_video_refresh_info(p_cont, FALSE);
  fw_tmr_create(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND, 1000, TRUE);  
  
  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_mask),FALSE);
#endif 
  return SUCCESS;
  
}

static void do_cancel_video(void)
{
  ui_stop_play_nvod(STOP_PLAY_BLACK, TRUE);
  fw_tmr_destroy(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND);
  manage_close_menu(ROOT_ID_NVOD_VIDEO, 0, 0);
}

static void undo_cancel_video(void)
{
 
}


static RET_CODE on_close_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t p_dlg_rc = {0};
  p_dlg_rc.left= ((SCREEN_WIDTH - COMM_DLG_W) / 2);
  p_dlg_rc.top= ((SCREEN_HEIGHT - COMM_DLG_H) / 2+20);
  p_dlg_rc.right= ((SCREEN_WIDTH - COMM_DLG_W) / 2+300);
  p_dlg_rc.bottom= ((SCREEN_HEIGHT - COMM_DLG_H) / 2+150);
  ui_comm_ask_for_dodlg_open(&p_dlg_rc, IDS_EXIT_NVOD, 
                               do_cancel_video, undo_cancel_video, 0);
  return SUCCESS;
  
}

static RET_CODE on_display_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;

  nvod_video_refresh_info(p_ctrl, TRUE);
  p_child = ctrl_get_child_by_id(p_ctrl, IDC_NVOD_PLAY_INFO_CONT);
  ctrl_set_attr(p_child,OBJ_ATTR_ACTIVE);
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_child, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static void nvod_time_down(utc_time_t *p_time, u32 sec)
{
  u32 all_sec = time_conver(p_time);
  if(sec > 0)
  {
    if(all_sec>sec)
    {
      all_sec -= sec;
      p_time->hour = (u8)(all_sec / 3600);
      p_time->minute = (u8)((all_sec % 3600)/60);
      p_time->second = (u8)((all_sec % 60));
    }
    else
    {
      memset(p_time, 0, sizeof(utc_time_t));
      fw_tmr_destroy(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND);
    }
  }
}

static RET_CODE on_display_nvod_time(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ascstr[16];
  memset(ascstr, 0, sizeof(ascstr)/sizeof(u8));
  
  p_child = ctrl_get_child_by_id(p_ctrl, IDC_NVOD_TIME);

  g_past_second++;
  nvod_time_down(&left_time, 1);
  sprintf((char *)ascstr, "%02d:%02d:%02d", left_time.hour, left_time.minute, left_time.second);
  text_set_content_by_ascstr(p_child, (u8*)ascstr);
  
  ctrl_paint_ctrl(p_child,TRUE);
  return SUCCESS;
}

static RET_CODE on_nvod_set_sound(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_MOSNVOD_VOLUME,0,0);
  return SUCCESS;
}

static RET_CODE on_hid_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_parent;
  
  p_parent = ctrl_get_parent(p_ctrl);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_parent, MSG_GETFOCUS, 0, 0);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  ctrl_paint_ctrl(p_parent,TRUE);
  return SUCCESS;
}

static RET_CODE on_nvod_mute(control_t *p_ctrl, u16 msg,
                        u32 para1, u32 para2)
{
  u8 index;
  index = fw_get_focus_id();
  if (index != ROOT_ID_DO_SEARCH
       && do_search_is_finish()
       /*&& !ui_is_upgrading()*/)
  {
    ui_set_mute(!ui_is_mute());
  }

  return SUCCESS;
}

BEGIN_KEYMAP(nvod_full_play_cont_keymap, NULL) 
  ON_EVENT(V_KEY_MENU, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_EXIT, MSG_BLACK_SCREEN)
  ON_EVENT(V_KEY_BACK, MSG_BLACK_SCREEN)
  ON_EVENT(V_KEY_CANCEL, MSG_BLACK_SCREEN)
  ON_EVENT(V_KEY_OK, MSG_SELECT) 
  ON_EVENT(V_KEY_VDOWN,  MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT,  MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)
END_KEYMAP(nvod_full_play_cont_keymap, NULL)
  
BEGIN_MSGPROC(nvod_full_play_cont_proc, cont_class_proc)
   ON_COMMAND(MSG_CLOSE_MENU, on_close_video_menu)
   ON_COMMAND(MSG_BLACK_SCREEN, on_close_video_menu)
   ON_COMMAND(MSG_SELECT, on_display_video_menu)
   ON_COMMAND(MSG_ONE_SECOND, on_display_nvod_time)
   ON_COMMAND(MSG_DECREASE, on_nvod_set_sound)
   ON_COMMAND(MSG_INCREASE, on_nvod_set_sound)
   ON_COMMAND(MSG_MUTE, on_nvod_mute)
END_MSGPROC(nvod_full_play_cont_proc, cont_class_proc)

BEGIN_KEYMAP(nvod_full_play_text_keymap, NULL) 
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)  
END_KEYMAP(nvod_full_play_text_keymap, NULL)

BEGIN_MSGPROC(nvod_full_play_text_proc, cont_class_proc)
   ON_COMMAND(MSG_CANCEL, on_hid_video_menu)
END_MSGPROC(nvod_full_play_text_proc, cont_class_proc)



