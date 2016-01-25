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
#include "ui_prebook_manage.h"
#include "ui_comm_root.h"
#include "ui_epg.h"


enum osd_set_control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
  IDC_TITLE5,
  IDC_BOOK_LIST,
  IDC_BOOK_LIST_SBAR,

  IDC_BOOK_LITTLE,
  IDC_BOOK_BAR_LITTLE_ARROWL,
  IDC_BOOK_BAR_LITTLE_ARROWR,
};



 static list_xstyle_t book_listfield_rstyle =
{
  RSI_SN_BAR_YELLOW_MIDDLE,//RSI_IGNORE,
  RSI_IGNORE,
  RSI_SN_BAR_YELLOW_MIDDLE,//RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
  
static list_xstyle_t book_listfield_fstyle =
{
  FSI_LIST_TXT_G,
  FSI_LIST_TXT_N,
  FSI_INDEX4,
  FSI_LIST_TXT_SEL,
  FSI_LIST_TXT_HL,
};


static list_xstyle_t book_listitem_rstyle =
{
  RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_N,
  RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_N,
  RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
  RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_SEL,
  RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
};
static list_field_attr_t book_listattr[PREBOOK_PROG_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON | STL_LEFT | STL_VCENTER,
    40, 10, 0, &book_listfield_rstyle,  &book_listfield_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    170/*140*/, 50/*60*/, 0, &book_listfield_rstyle,  &book_listfield_fstyle},
  { LISTFIELD_TYPE_STRID | STL_CENTER | STL_VCENTER,
    0/*80*/, 230, 0, &book_listfield_rstyle,  &book_listfield_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120, 260, 0, &book_listfield_rstyle,  &book_listfield_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    200, 390, 0, &book_listfield_rstyle,  &book_listfield_fstyle},
};

static book_pg_t g_booknode[MAX_BOOK_PG];// = {0};

static u8 g_bookCount = 0;

comm_dlg_data_t book_exit_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,  
  DLG_STR_MODE_NULL,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_SAVE_MODIFY,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  0,
};

u16 prebook_cont_keymap(u16 key);
RET_CODE prebook_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


u16 prebook_list_keymap(u16 key);
RET_CODE prebook_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static u8 book_get_all_booked_node(void)
{
  u8 index = 0;
  u8 i = 0;
  book_pg_t temp_node;

  memset(&g_booknode, 0, sizeof(book_pg_t)*MAX_BOOK_PG);
  book_sort_by_start_time();
  
  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    book_get_book_node(i, &temp_node);
    if ((temp_node.pgid != 0) && (temp_node.timer_mode != BOOK_TMR_OFF))
    {
      memcpy(&(g_booknode[index]), &temp_node, sizeof(book_pg_t));
      index++;
    }
  }

  g_bookCount = index;

  return g_bookCount;
}

static void book_do_save_all()
{
  book_pg_t temp_node = {0};
  u16 i = 0, index = 0;
  
  for (i = 0; i < g_bookCount; i++)
  {
    if ((g_booknode[i].pgid != 0) && (g_booknode[i].timer_mode == BOOK_TMR_OFF))
    {
      memcpy(&temp_node, &(g_booknode[i]), sizeof(book_pg_t));
      temp_node.timer_mode = BOOK_TMR_ONCE;
      index = book_get_match_node(&temp_node);
      book_delete_node((u8)index);
    }
  }
}

static RET_CODE on_book_reset_node_status(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 index = list_get_focus_pos(p_list);
  
  if(index >= g_bookCount)
  {
    return ERR_FAILURE;
  }
  
  if(g_booknode[index].timer_mode != BOOK_TMR_OFF)
  {
    g_booknode[index].timer_mode = BOOK_TMR_OFF;
    list_set_field_content_by_icon(p_list, index, 0, 0);
  }
  else
  {
    g_booknode[index].timer_mode = BOOK_TMR_ONCE;
    list_set_field_content_by_icon(p_list, index, 0, IM_SN_ICON_TIME);
  }

  list_draw_field_ext(p_list, index, 0, TRUE);
  return SUCCESS;
}

static RET_CODE book_list_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, pos;
  u16 cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[64];
  u16 uni_str[128];
  utc_time_t endtime = {0};
  u8 weekday = 0;
  u16 str[] = 
    {
      IDS_MON,
      IDS_TUS,
      IDS_WED,
      IDS_THS,
      IDS_FRI,
      IDS_SAT,
      IDS_SUN,
    };
  
  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      //icon
      if((g_booknode[pos].timer_mode != BOOK_TMR_OFF)
        &&(g_booknode[pos].pgid != 0))
      {
        list_set_field_content_by_icon(ctrl, pos, 0, IM_SN_ICON_TIME);
      }
      else
      {
        list_set_field_content_by_icon(ctrl, pos, 0, 0);
      }

      memcpy(&endtime, &(g_booknode[pos].start_time), sizeof(utc_time_t));
      time_add(&endtime, &(g_booknode[pos].drt_time));
      //timer type
      sprintf((char *)asc_str,"%.4d/%.2d/%.2d %.2d:%.2d-%.2d:%.2d",
                    g_booknode[pos].start_time.year,
                    g_booknode[pos].start_time.month,
                    g_booknode[pos].start_time.day,
                    g_booknode[pos].start_time.hour, 
                    g_booknode[pos].start_time.minute, 
                    endtime.hour, 
                    endtime.minute
                    );
      list_set_field_content_by_ascstr(ctrl, pos, 1, asc_str);

      //weekday
      weekday = date_to_weekday(&(g_booknode[pos].start_time));
      list_set_field_content_by_strid(ctrl, pos, 2, str[weekday]);
      
      //service name
      if(g_booknode[pos].timer_mode)
      {
        db_dvbs_get_pg_by_id(g_booknode[pos].pgid, &pg);
        ui_dbase_get_full_prog_name(&pg, uni_str, 31);
        list_set_field_content_by_unistr(ctrl, pos, 3, uni_str);
      }    
      else
      {
        list_set_field_content_by_ascstr(ctrl, pos, 3, (u8 *)" "); 
      }
      
      //event name
      list_set_field_content_by_unistr(ctrl, pos, 4, g_booknode[pos].event_name);   
    }
  }
  
  return SUCCESS;
}


RET_CODE open_prebook_manage(u32 para1, u32 para2)
{
  //title cont
  #define SN_BOOK_TITLE_CONT_X             10
  #define SN_BOOK_TITLE_CONT_Y             10
  #define SN_BOOK_TITLE_CONT_W             620
  #define SN_BOOK_TITLE_CONT_H             35

  //plist
  #define SN_BOOK_LIST_LIST_X              SN_BOOK_TITLE_CONT_X
  #define SN_BOOK_LIST_LIST_Y              SN_BOOK_TITLE_CONT_Y + SN_BOOK_TITLE_CONT_H + 10
  #define SN_BOOK_LIST_LIST_W              SN_BOOK_TITLE_CONT_W
  #define SN_BOOK_LIST_LIST_H              270

  #define SN_BOOK_LIST_LIST_MIDL           4
  #define SN_BOOK_LIST_LIST_MIDT           4
  #define SN_BOOK_LIST_LIST_MIDW           SN_BOOK_TITLE_CONT_W - 2*SN_BOOK_LIST_LIST_MIDL
  #define SN_BOOK_LIST_LIST_MIDH           SN_BOOK_LIST_LIST_H - 2*SN_BOOK_LIST_LIST_MIDT

  //bar little
  #define SN_BOOK_LITTLE_X				   SN_BOOK_TITLE_CONT_X
  #define SN_BOOK_LITTLE_Y				   SN_BOOK_TITLE_CONT_Y + SN_BOOK_TITLE_CONT_H + SN_BOOK_LIST_LIST_H + 20
  #define SN_BOOK_LITTLE_W				   SN_BOOK_TITLE_CONT_W
  #define SN_BOOK_LITTLE_H				   31

  
  //bar little arrow
  #define SN_BOOK_BAR_ARROW_LEFT_X	      SN_BOOK_TITLE_CONT_W/2 - 15
  #define SN_BOOK_BAR_ARROW_RIGHT_X	      SN_BOOK_TITLE_CONT_W/2 + 15
  #define SN_BOOK_BAR_ARROW_Y		      (SN_BOOK_LITTLE_H - SN_BOOK_BAR_ARROW_H)/2
  #define SN_BOOK_BAR_ARROW_W		      15
  #define SN_BOOK_BAR_ARROW_H		      18

  //help bar
  #define SN_SUB_MENU_PREBOOK_HELP_RSC_CNT        17
      


  control_t *p_cont, *p_title_con,*p_title[TITLE_ITEM_CNT];
  control_t *p_list, *p_bar_little, *p_bar_little_arrow;//*p_sbar,
  u16 i;
  u8 book_count=0;

  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_PREBOOK_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_ARROW_DOWN},
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
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F1},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BOOK}
  };
 
  book_count = book_get_all_booked_node();

  // create container
  p_cont = ui_background_create(ROOT_ID_PREBOOK_MANAGE,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_PROGRAM_BOOK,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, prebook_cont_proc);


  //title cont
  p_title_con = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_TITLE,
                              SN_BOOK_TITLE_CONT_X, SN_BOOK_TITLE_CONT_Y,
                              SN_BOOK_TITLE_CONT_W, SN_BOOK_TITLE_CONT_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_title_con, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SECOND_TITLE
 
  //title cont: item1
  p_title[0] = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE1,
                           10, 0, 70,BOOK_TITLE_CONT_H, p_title_con, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type(p_title[0] , STL_LEFT|STL_VCENTER);
  text_set_font_style(p_title[0] , FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_title[0], TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title[0] , IDS_BOOK);

  //title cont: item2
  p_title[1] = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE2,
                           90, 0, 130,BOOK_TITLE_CONT_H, p_title_con, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type( p_title[1] , STL_LEFT|STL_VCENTER);
  text_set_font_style( p_title[1] , FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type( p_title[1], TEXT_STRTYPE_STRID);
  text_set_content_by_strid( p_title[1] , IDS_DATE_TIME);

  //title cont: item5
  p_title[4] = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE5,
                           320, 0, 100,BOOK_TITLE_CONT_H, p_title_con, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type( p_title[4] , STL_LEFT|STL_VCENTER);
  text_set_font_style( p_title[4] , FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type( p_title[4], TEXT_STRTYPE_STRID);
  text_set_content_by_strid( p_title[4] , IDS_PROGRAM_NAME);
 
  //plist
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_BOOK_LIST, 
                        SN_BOOK_LIST_LIST_X, SN_BOOK_LIST_LIST_Y, 
                        SN_BOOK_LIST_LIST_W, SN_BOOK_LIST_LIST_H, 
                        p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_TIP_BOX
  ctrl_set_keymap(p_list, prebook_list_keymap);
  ctrl_set_proc(p_list, prebook_list_proc);
  list_set_mid_rect(p_list, SN_BOOK_LIST_LIST_MIDL, SN_BOOK_LIST_LIST_MIDT,
                    SN_BOOK_LIST_LIST_MIDW, SN_BOOK_LIST_LIST_MIDH,
                    7);
  list_set_item_rstyle(p_list, &book_listitem_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, book_count, PREBOOK_PROG_LIST_PAGE);
  list_set_field_count(p_list, PREBOOK_PROG_LIST_FIELD, PREBOOK_PROG_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  //list_select_item(p_list, 0);
  list_set_update(p_list, book_list_update, 0);
 
  for (i = 0; i < PREBOOK_PROG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(book_listattr[i].attr), (u16)(book_listattr[i].width),
                        (u16)(book_listattr[i].left), (u8)(book_listattr[i].top));
    list_set_field_rect_style(p_list, (u8)i, book_listattr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, book_listattr[i].fstyle);
  }    
  book_list_update(p_list, list_get_valid_pos(p_list), PREBOOK_PROG_LIST_PAGE, 0);

  //bar little
  p_bar_little = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BOOK_LITTLE,
                               SN_BOOK_LITTLE_X, SN_BOOK_LITTLE_Y,
                               SN_BOOK_LITTLE_W, SN_BOOK_LITTLE_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_bar_little, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);  

  //bar little arrow
  p_bar_little_arrow = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BOOK_BAR_LITTLE_ARROWL,
                            SN_BOOK_BAR_ARROW_LEFT_X, SN_BOOK_BAR_ARROW_Y,
                            SN_BOOK_BAR_ARROW_W, SN_BOOK_BAR_ARROW_H,
                            p_bar_little, 0);
  bmap_set_content_by_id(p_bar_little_arrow, IM_SN_ICON_ARROW_UP);

  p_bar_little_arrow = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BOOK_BAR_LITTLE_ARROWR,
                            SN_BOOK_BAR_ARROW_RIGHT_X, SN_BOOK_BAR_ARROW_Y,
                            SN_BOOK_BAR_ARROW_W, SN_BOOK_BAR_ARROW_H,
                            p_bar_little, 0);
  bmap_set_content_by_id(p_bar_little_arrow, IM_SN_ICON_ARROW_DOWN);

  book_list_update(p_list, list_get_valid_pos(p_list), PREBOOK_PROG_LIST_FIELD, 0);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif


  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_PREBOOK_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=57;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_pic_stop();

  return SUCCESS;
}

static RET_CODE on_pro_book_exit(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	u8 i = 0;
  BOOL bModify = FALSE;
  for(i=0; i<g_bookCount; i++)
  {
    if((g_booknode[i].pgid != 0) && (g_booknode[i].timer_mode == BOOK_TMR_OFF))
    {
      bModify = TRUE;
    }
  }
  if(bModify)
  {
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
	ret=ui_comm_dialog_open(&dialog);
	
  if(DLG_RET_YES==ret)
  {
  	 book_do_save_all();
      fw_notify_parent(ROOT_ID_EPG, NOTIFY_T_MSG, FALSE, MSG_BOOK_UPDATE, 0, 0);
   //mtos_task_delay_ms(500);
   manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
   manage_close_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);
    ret = FALSE;
  }
  else
  {
	manage_close_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);
  }
  	}
  else
  {
	manage_close_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);
  }
  if(fw_get_focus_id() == ROOT_ID_EPG)
  {
	p_ctrl = fw_find_root_by_id(ROOT_ID_EPG);
    ui_pic_play(ADS_AD_TYPE_EPG, ROOT_ID_EPG);
    prebook_set_epg_event_list_update(p_ctrl);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  return SUCCESS;
}

BEGIN_MSGPROC(prebook_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_TO_MAINMENU, on_pro_book_exit)
  ON_COMMAND(MSG_EXIT, on_pro_book_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_pro_book_exit)
END_MSGPROC(prebook_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(prebook_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_F1, MSG_SELECT)
  ON_EVENT(V_KEY_RED, MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(prebook_list_keymap, NULL)

BEGIN_MSGPROC(prebook_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_book_reset_node_status)
  ON_COMMAND(MSG_YES, on_pro_book_exit)
END_MSGPROC(prebook_list_proc, list_class_proc)


