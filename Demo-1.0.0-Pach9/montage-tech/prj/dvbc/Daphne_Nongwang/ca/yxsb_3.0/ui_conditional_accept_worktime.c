/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_worktime.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_WORKTIME,
};

enum ca_work_duration_id
{
  IDC_CA_WORK_DURATION_PIN = 1,
  IDC_CA_WORK_DURATION_START_TIME,
  IDC_CA_WORK_DURATION_END_TIME,
  IDC_CA_WORK_DURATION_OK,
  IDC_CA_WORK_DURATION_CANCEL,
  IDC_CA_WORK_DURATION_CHANGE_RESULT,
};
comm_prompt_data_t work_data_t =
	 {
	   ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,
	   STR_MODE_STATIC,
	   200,165,250,150,20,
	   RSC_INVALID_ID,RSC_INVALID_ID,
	   RSC_INVALID_ID,DIALOG_DEADLY,
	   3000
	 };

static cas_card_work_time_t card_word_time;

u16 conditional_accept_worktime_cont_keymap(u16 key);
RET_CODE conditional_accept_worktime_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_worktime_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_worktime_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_worktime_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_worktime_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE conditional_accept_worktime_timedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void conditional_accept_worktime_set_content(control_t *p_cont, cas_card_work_time_t*p_card_worktime_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_WORKTIME);
  control_t *p_start = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_START_TIME);
  control_t *p_end = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_END_TIME);
  utc_time_t time = {0};
   UI_PRINTF("@@@@@ start hour:%d min:%d sec:%d\n", 
                  p_card_worktime_info->start_hour,
                  p_card_worktime_info->start_minute,
                  p_card_worktime_info->start_second);
  UI_PRINTF("@@@@@ end hour:%d min:%d sec:%d\n", 
                  p_card_worktime_info->end_hour,
                  p_card_worktime_info->end_minute,
                  p_card_worktime_info->end_second);   
  if(p_card_worktime_info == NULL)
  {
    ui_comm_timedit_set_time(p_start, &time);
    ui_comm_timedit_set_time(p_end, &time);
  }
  else
  {
    time.hour = p_card_worktime_info->start_hour;
    time.minute = p_card_worktime_info->start_minute;
    time.second = p_card_worktime_info->start_second;
    ui_comm_timedit_set_time(p_start, &time);

    time.hour = p_card_worktime_info->end_hour;
    time.minute = p_card_worktime_info->end_minute;
    time.second = p_card_worktime_info->end_second;
    ui_comm_timedit_set_time(p_end, &time);
  }
  //ctrl_paint_ctrl(p_end ,TRUE);
  //ctrl_paint_ctrl(p_start ,TRUE);
}

extern void g_set_worktime_info(cas_card_work_time_t *p);
extern cas_card_work_time_t *g_get_worktime_info();

RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT  5
#define SN_CONDITIONAL_ACCEPT_WORKTIME_HELP_RSC_CNT   15
	//CA frame
#define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_X 	  106
#define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_Y 	  24
#define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_W 	  505
#define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_H 	  360
	
	//work duration items
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X		  10
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y		  20
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW 	  260
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW 	  140
#define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H		  35
	
#define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X		140
#define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y		160
#define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W		70
#define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H		25
  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  control_t *p_ca_frm;
  cas_card_info_t *p_test = 0;
  u8 i;
  u16 y;
  u16 stxt_work[CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT] =
  { 
    IDS_PLS_INPUT_PIN, IDS_START_TIME3, IDS_END_TIME,
  };
#if 0
QQ
  comm_help_data_t ca_worktime_help_data = //help bar data
  {
    3,
    3,
    {
      IDS_BACK,
      IDS_SELECTED,
      //IDS_FN_SELECT,
      IDS_FN_SELECT,
    },
    { 
      IM_SN_ICON_BRACKET_LEFT,//IM_ICON_BACK,
      IM_SN_ICON_BRACKET_LEFT,//IM_ICON_SELECT,
      //IM_ICON_ARROW_LEFT_RIGHT,
      IM_SN_ICON_BRACKET_LEFT,//IM_ICON_ARROW_UP_DOWN,
    },
  };
#endif
 static sn_comm_help_data_t worktime_help_data; //help bar data
 help_rsc help_item[SN_CONDITIONAL_ACCEPT_WORKTIME_HELP_RSC_CNT]=
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

 // full_screen_title_t title_data = {IM_SN_ICON_BRACKET_LEFT/*IM_COMMON_BANNER_SETTLE*/,IDS_CONDITIONAL_ACCEPT, RSI_IGNORE/*RSI_TITLE_BG*/, IDS_WOR_DURATION_SET};

  p_test = (cas_card_info_t *)para1;
  
  UI_PRINTF("@111@ start hour:%d min:%d sec:%d\n", 
                p_test->work_time.start_hour,
                p_test->work_time.start_minute,
                p_test->work_time.start_second);
  UI_PRINTF("@111@ end hour:%d min:%d sec:%d\n", 
                p_test->work_time.end_hour,
                p_test->work_time.end_minute,
                p_test->work_time.end_second);   

  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,
							 0, 0,
							 640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
							 IDS_CA_WORK_TIME_SET,TRUE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_worktime_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_worktime_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_WORKTIME,
                              SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_X-20, SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_W, SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_H,
                              p_cont, 0);

  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_ca_frm, conditional_accept_worktime_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_worktime_ca_frm_proc);

  //work duration
  y = SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT; i++)
  {
    switch(i)
    {
		case 0:
		  p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_WORK_DURATION_PIN + i),
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X, y,
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW-50,
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW+100);
		  ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_work[i]);
		  ui_comm_pwdedit_set_param(p_ctrl, 6); 
		  //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
		  ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_worktime_pwdedit_keymap);
		  ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_worktime_pwdedit_proc);
		  ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SUB_BUTTON_SH
		  break;
      case 1:
      case 2: 
		  p_ctrl = ui_comm_timedit_create(p_ca_frm, (u8)(IDC_CA_WORK_DURATION_PIN + i),
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X, y,
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW-50,
											 SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW+100);
		  ui_comm_timedit_set_static_txt(p_ctrl, stxt_work[i]);
		  ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_worktime_timedit_proc);
		  ui_comm_timedit_set_param(p_ctrl, 0, TBOX_ITEM_HOUR, TBOX_HOUR| TBOX_MIN|TBOX_SECOND, 
										  TBOX_SEPARATOR_TYPE_UNICODE, 12);
		  ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_HOUR, ':');
		  ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_MIN, ':');
		  ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SUB_BUTTON_SH
        break;
		case 3:
		   p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CA_WORK_DURATION_PIN + i),
							   SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X+85, 
							   SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y+120,
							   SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W, 
							   SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H+6,
							   p_ca_frm, 0);
		   ctrl_set_rstyle(p_ctrl,RSI_DIG_BG, RSI_SN_BAR_YELLOW_HL, RSI_DIG_BG);
		   text_set_font_style(p_ctrl,FSI_DLG_BTN_N, FSI_DLG_BTN_HL, FSI_DLG_BTN_N);
		   text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		   text_set_content_by_strid(p_ctrl, IDS_OK);  
		   break;

      default:
        p_ctrl = NULL;
        break;
    }

    if(NULL == p_ctrl)
    {
        break;
    }
    //ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN));/* down */

    y += CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H + CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_V_GAP;
  }

  //change result
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CA_WORK_DURATION_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_X, 
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_W, 
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH
  text_set_font_style(p_ctrl, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE);//FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

  //text_set_content_by_strid(p_ctrl, IDS_SET_SUCCESS);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  // QQ ui_comm_help_create(&ca_worktime_help_data, p_cont);

  memset((void*)&worktime_help_data, 0, sizeof(sn_comm_help_data_t));
  worktime_help_data.x=0;
  worktime_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_WORKTIME_HELP_RSC_CNT;
  worktime_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  worktime_help_data.offset=37;
  worktime_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&worktime_help_data, p_cont);

  OS_PRINTF("open_conditional_accept_worktime\n");
  
  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_WORK_DURATION_PIN), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_ca_get_info(CAS_CMD_WORK_TIME_GET, 0 ,0);
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_worktime_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}


static RET_CODE on_conditional_accept_worktime_timedit_unselect(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#if 0
  control_t *p_cont, *p_date, *p_time;
  utc_time_t loc_time = {0};
  utc_time_t gmt_time = {0};

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  p_date = ctrl_get_child_by_id(p_cont, IDC_DATE);
  p_time = ctrl_get_child_by_id(p_cont, IDC_TIME);

  ui_comm_timedit_get_time(p_date, &loc_time);
  ui_comm_timedit_get_time(p_time, &loc_time);

  time_to_gmt(&loc_time, &gmt_time);

  time_set(&gmt_time);
#endif
  
  return ERR_NOFEATURE;
}
#if 0
static BOOL is_conditional_accept_worktime_valid(utc_time_t p_star_time,utc_time_t p_end_time)
{
  u32 start_time = 0;
  u32 end_time = 0;

  start_time = (u32)((p_star_time.hour<<16) | (p_star_time.minute<<8) |(p_star_time.second));
  end_time = (u32)((p_end_time.hour<<16) | (p_end_time.minute<<8) | (p_end_time.second));
  
  UI_PRINTF("****** print ca worktime: start time= [%d : %d : %d], \
                        end time = [%d : %d : %d], \
                        start_time value = 0x%x, end_time value = 0x%x \n",
                      p_star_time.hour,p_star_time.minute,p_star_time.second,
                      p_end_time.hour,p_end_time.minute,p_end_time.second,
                      start_time,end_time);

  if(start_time >= end_time)
    return FALSE;
  else
    return TRUE;
}
#endif
static BOOL is_conditional_accept_worktime_valid(utc_time_t p_star_time,utc_time_t p_end_time)
{
  u32 start_time = 0;
  u32 end_time = 0;

  start_time = (u32)((p_star_time.hour<<16) | (p_star_time.minute<<8)); //|(p_star_time.second)
  end_time = (u32)((p_end_time.hour<<16) | (p_end_time.minute<<8) );//| (p_end_time.second)
  
  UI_PRINTF("****** print ca worktime: start time= [%d : %d ], \
                        end time = [%d : %d ], \
                        start_time value = 0x%x, end_time value = 0x%x \n",
                      p_star_time.hour,p_star_time.minute,//p_star_time.second,
                      p_end_time.hour,p_end_time.minute,//p_end_time.second,
                      start_time,end_time);

  if(start_time >= end_time)
    return FALSE;
  else
    return TRUE;
}


static RET_CODE on_conditional_accept_worktime_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_pin, *p_start, *p_end,*p_result,*p_child;
	  utc_time_t p_star_time, p_end_time ;
	  u32 card_pin = 0;
	  BOOL card_pin_is_full = FALSE;
	  dlg_ret_t ret=FALSE;
	  comm_dialog_data_t dialog;
	  u8 ctrl_id;
	  
	  p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_PIN);
	  p_start = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_START_TIME);
	  p_end = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_END_TIME);
	  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_CHANGE_RESULT);
	  card_pin = ui_comm_pwdedit_get_value(p_pin);
	  card_pin_is_full = ui_comm_pwdedit_is_full(p_pin);
	
		dialog.grade = DIALOG_INFO;
		dialog.x	  = 170;
		dialog.y	  = 160;
		dialog.w	 = 300;
		dialog.h	 = 150;
		dialog.parent_root = 0;
		dialog.icon_id = 0;
		dialog.string_mode = STR_MODE_STATIC;
		dialog.text_strID	= IDS_SAVING_OR_NOT;
		dialog.text_content = 0;
		dialog.type   = DLG_FOR_ASK;
		dialog.timeout = 100000;
	
		p_child = ctrl_get_active_ctrl(p_ctrl);
		ctrl_id = ctrl_get_ctrl_id(p_child);
	 if(ctrl_id == IDC_CA_WORK_DURATION_CANCEL){
	   manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
	 }
	 else{
		ret=ui_comm_dialog_open(&dialog);
		
	  if(DLG_RET_YES==ret)
	  {
		card_word_time.pin[0]=(u8)(card_pin/100000);
		card_word_time.pin[1]=(u8)((card_pin/10000)%10);
		card_word_time.pin[2]=(u8)(((card_pin/1000)%100)%10);
		card_word_time.pin[3]=(u8)((((card_pin/100)%1000)%100)%10);
		card_word_time.pin[4]=(u8)(((((card_pin/10)%10000)%1000)%100)%10);
		card_word_time.pin[5]=(u8)(((((card_pin%100000)%10000)%1000)%100)%10);
	
		ui_comm_timedit_get_time(p_start,&p_star_time);
		ui_comm_timedit_get_time(p_end,&p_end_time);
	
		if(FALSE == is_conditional_accept_worktime_valid(p_star_time,p_end_time))
		{
			ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
			work_data_t.text_strID = IDS_CA_WORKTIME_INVALID;
			ctrl_paint_ctrl(p_pin, TRUE);
			ui_comm_prompt_open(&work_data_t);
			return ERR_FAILURE;
		}
	
		card_word_time.start_hour = p_star_time.hour;
		card_word_time.start_minute= p_star_time.minute;
		//card_word_time.start_second= p_star_time.second;
		card_word_time.end_hour= p_end_time.hour;
		card_word_time.end_minute= p_end_time.minute;
		//card_word_time.end_second= p_end_time.second;
#ifndef WIN32
		if(card_pin_is_full)
		{
			ui_ca_get_info((u32)CAS_CMD_WORK_TIME_SET, (u32)&card_word_time ,0);
		}
		else
		{
		//PIN invalid
			work_data_t.text_strID = IDS_CA_PIN_INVALID;
			ui_comm_prompt_open(&work_data_t);
		}
#else
		work_data_t.text_strID = IDS_CA_WORKTIME_CHANGED;
#endif
		ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
		ctrl_paint_ctrl(p_pin, TRUE);
	
	  }
	  else
	  {
		manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
	  }
	}
	  return SUCCESS;

}

static RET_CODE on_conditional_set_worktime_right(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm;
  RET_CODE ret;
  p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_WORKTIME);
  p_frm = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_CHANGE_RESULT);
  ret = (RET_CODE)para1;
  if(ret == SUCCESS)
  {
  //  text_set_content_by_strid(p_frm, IDS_MAINMENU);//IDS_CA_WORKTIME_CHANGED);
	  work_data_t.text_strID = IDS_CA_WORKTIME_CHANGED;
	  ui_comm_prompt_open(&work_data_t);  
	  g_set_worktime_info(&card_word_time);
	
  }
  else
  {
    //text_set_content_by_strid(p_frm, IDS_MAINMENU);//IDS_CA_PIN_INVALID);
    
	work_data_t.text_strID = IDS_CA_PIN_INVALID;
	ui_comm_prompt_open(&work_data_t);	
  }
  ctrl_paint_ctrl(p_frm, TRUE);
  return SUCCESS;
}
static RET_CODE on_conditional_accept_worktime_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_worktime_set_content(p_cont, (cas_card_work_time_t*)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_worktime_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_worktime_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_worktime_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_worktime_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_worktime_cont_focus_change)  
    ON_COMMAND(MSG_CA_GET_WORK_TIME_INFO, on_conditional_accept_worktime_update)
  ON_COMMAND(MSG_CA_WORK_TIME_INFO, on_conditional_set_worktime_right)
END_MSGPROC(conditional_accept_worktime_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_worktime_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)  
END_KEYMAP(conditional_accept_worktime_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_worktime_ca_frm_proc, ui_comm_root_proc)  
  ON_COMMAND(MSG_SELECT, on_conditional_accept_worktime_ca_frm_ok)  
END_MSGPROC(conditional_accept_worktime_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_worktime_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_worktime_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_worktime_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_worktime_pwdedit_maxtext)
END_MSGPROC(conditional_accept_worktime_pwdedit_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(conditional_accept_worktime_timedit_proc, ui_comm_time_proc)  
  ON_COMMAND(MSG_UNSELECT, on_conditional_accept_worktime_timedit_unselect) 
END_MSGPROC(conditional_accept_worktime_timedit_proc, ui_comm_time_proc)



