/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_display.h"
#include "ui_channel_warn.h"
#include "ui_comm_dlg.h"
#include "ui_comm_root.h"
#include <string.h>


enum control_id
{
  IDC_INVALID = 0,
  SN_IDC_TRANS_SET,
  SN_IDC_RATION_SET,
  SN_IDC_TIMEZONE,
  SN_IDC_BMP_LEFT,
  SN_IDC_BMP_RIGHT,
  //IDC_OUTPUT_SET,
  IDC_MAX_CNT,
};
static osd_set_t osd_set;
static osd_set_t osd_set1;
static time_set_t timer_set;
static time_set_t timer_set1;
static av_set_t av_set;
static av_set_t av_set1;

u16 display_set_cont_keymap(u16 key);
RET_CODE display_set_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE display_set_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static RET_CODE param_fill_gmt_offset(control_t *ctrl, u16 focus, u16 *p_str,
                        u16 max_length)
{
  u8 asc_buf[32];
  
  if(focus >= 23)//+
  {
    sprintf((char *)asc_buf, "UTC + %.2d:%.2d", (focus - 23)/2, (focus - 23)%2*30);
  }
  else//-
  {
    sprintf((char *)asc_buf, "UTC - %.2d:%.2d", (23 - focus)/2, (23 - focus)%2*30);

  }

  str_nasc2uni(asc_buf, p_str, max_length);

  return SUCCESS;
}

static RET_CODE on_dislpay_set_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

  return ERR_NOFEATURE;
}

RET_CODE open_display_set(u32 para1, u32 para2)
{
#define SN_DISPLAY_SET_X	130
#define SN_DISPLAY_SET_Y	150
#define SN_DISPLAY_SET_CNT	3
#define SN_DISPLAY_W   400
#define SN_DISPLAY_H   200
#define SN_DISPLAY_SELECT_X	30
#define SN_DISPLAY_SELECT_Y	38
#define SN_DISPLAY_BMAP_W		12

  control_t *p_cont, *p_ctrl_item[IDC_MAX_CNT];
  control_t *p_bg;//*p_select_bmp,
  u8 i, j;
  u8 index = 0;
   u16 y;
  u16 para_set_item [SN_DISPLAY_SET_CNT][3] =
  { 
    {SN_IDC_TRANS_SET,     100,  IDS_TRANSPARENCE_SET}, 
    {SN_IDC_RATION_SET,    3,  IDS_RATION}, 
    {SN_IDC_TIMEZONE,    0,  IDS_TIMEZONE}, 
   };
  u16 content [SN_DISPLAY_SET_CNT][3] = 
  {
    {0},
    { IDS_43, IDS_169, IDS_AUTO },
   };

  DEBUG(DISPLAY,INFO,"open_display_set open\n");
 	p_cont = fw_create_mainwin(SN_ROOT_ID_DISPLAY_SET,
									 SN_DISPLAY_SET_X, SN_DISPLAY_SET_Y, SN_DISPLAY_W, SN_DISPLAY_H,
									 0, 0, OBJ_ATTR_ACTIVE, 0);
		  p_bg = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_BG_CONT,
									  0, 0, SN_DISPLAY_W, SN_DISPLAY_H, p_cont, 0);
		ctrl_set_rstyle(p_bg, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, display_set_cont_keymap);
  ctrl_set_proc(p_cont, display_set_cont_proc);
  
   //create menu item
  y = SN_DISPLAY_SELECT_Y;
  for (i = SN_IDC_TRANS_SET,index=0; i <=SN_IDC_TIMEZONE ; i++,index++)
  {
    switch (i)
    {
      case SN_IDC_TRANS_SET:
    p_ctrl_item[i] = ui_comm_select_create(p_cont, i,
                                        SN_DISPLAY_SELECT_X, y, 
                                        (SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2+100,
                                        (SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2-82);
    
    ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
    ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
    ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
    
    ui_comm_ctrl_set_proc(p_ctrl_item[i], display_set_select_proc);
    ui_comm_select_set_static_txt(p_ctrl_item[i], para_set_item[index][2]);
    
    
        ui_comm_select_set_param(p_ctrl_item[i], TRUE, CBOX_WORKMODE_NUMBER,
                                   para_set_item[index][1], CBOX_ITEM_STRTYPE_DEC, NULL);
        ui_comm_select_set_num_range(p_ctrl_item[i], 0, 5, 1, 0, NULL);
		ctrl_set_related_id(p_ctrl_item[i],0,SN_IDC_TIMEZONE,0,SN_IDC_RATION_SET);
        break;
      case SN_IDC_RATION_SET:
	  	p_ctrl_item[i] = ui_comm_select_create(p_cont, i,
                                        SN_DISPLAY_SELECT_X, y, 
                                        (SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2+100,
                                        (SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2-60);
    
    ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
    ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
    ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
    
    ui_comm_ctrl_set_proc(p_ctrl_item[i], display_set_select_proc);
    ui_comm_select_set_static_txt(p_ctrl_item[i], para_set_item[index][2]);
        ui_comm_select_set_param(p_ctrl_item[i], TRUE, CBOX_WORKMODE_STATIC,
                                   para_set_item[index][1], CBOX_ITEM_STRTYPE_STRID, NULL);
        for (j = 0; j < para_set_item[index][1]; j++)
        {
          ui_comm_select_set_content(p_ctrl_item[i], j, content[index][j]);
        }
		ctrl_set_related_id(p_ctrl_item[i],0,SN_IDC_TRANS_SET,0,SN_IDC_TIMEZONE);
        break;
		
	case SN_IDC_TIMEZONE:
		p_ctrl_item[i] = ui_comm_select_create(p_cont, i,
											SN_DISPLAY_SELECT_X, y, 
											(SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2+20,
											(SN_DISPLAY_W-(2*SN_DISPLAY_SELECT_X)-20)/2+17);
		
		ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
		ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
		ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
		ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_proc(p_ctrl_item[i], display_set_select_proc);
		ui_comm_select_set_static_txt(p_ctrl_item[i], para_set_item[index][2]);
		ui_comm_select_set_param(p_ctrl_item[i], TRUE, 
			  CBOX_WORKMODE_DYNAMIC, PARAM_SET_GMT_OFFSET_NUM, 0, param_fill_gmt_offset);
		ctrl_set_related_id(p_ctrl_item[i],0,SN_IDC_RATION_SET,0,SN_IDC_TRANS_SET);
		break;
	 default:
	 	MT_ASSERT(0);
		break;
	 	
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);
 
    y += 10+SN_DISPLAY_SELECT_Y;

  }

  /* set focus according to current info */

  sys_status_get_osd_set(&osd_set);
  sys_status_get_av_set(&av_set);
  sys_status_get_time(&timer_set);
  memset(&osd_set1, 0, sizeof(osd_set_t));
  memcpy(&osd_set1,&osd_set,(sizeof(osd_set_t)));
  memset(&av_set1, 0, sizeof(av_set_t));
  memcpy(&av_set1,&av_set,(sizeof(av_set_t)));
  memset(&timer_set1, 0, sizeof(time_set_t));
  memcpy(&timer_set1,&timer_set,(sizeof(time_set_t)));
  ui_comm_select_set_focus(p_ctrl_item[SN_IDC_TIMEZONE], (u16)timer_set.gmt_offset);
  ui_comm_select_set_num(p_ctrl_item[SN_IDC_TRANS_SET], osd_set.transparent);
  ui_comm_select_set_focus(p_ctrl_item[SN_IDC_RATION_SET], av_set.tv_ratio);
   ctrl_default_proc(p_ctrl_item[SN_IDC_TRANS_SET], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_display_set_changed(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u8 focus = (u8)(para2);
  control_t *p_cont;
  u8 id = ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl));

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);
  p_cont = p_ctrl->p_parent->p_parent;

  switch(id)
  {      
    case SN_IDC_RATION_SET:
      av_set.tv_ratio = focus;
      break;
    case SN_IDC_TRANS_SET:
      osd_set.transparent = focus;
      break;
	  
	case SN_IDC_TIMEZONE:
	  timer_set.gmt_offset = focus;
	  break;
    default:
      break;
  } 
  
  return ret;
}

static RET_CODE on_display_set_save(control_t *p_ctrl)
{
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
   book_pg_t temp_node[MAX_BOOK_PG];
   memset(&temp_node, 0, sizeof(book_pg_t)*MAX_BOOK_PG);

  gdi_set_enable(TRUE);

  sys_status_set_time(&timer_set);
  sys_status_set_time_zone();
  avc_switch_video_mode_1(avc_handle, av_set.tv_mode);
  ui_reset_tvmode(av_set.tv_mode);
  
  avc_set_video_aspect_mode_1(avc_handle, 
                               sys_status_get_video_aspect(av_set.tv_ratio));
   ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
   
   gdi_set_global_alpha_montage(10*(10 - osd_set.transparent) * 255 / 100);
  avc_set_video_bright(avc_handle, osd_set.bright);
  avc_set_video_contrast(avc_handle,  osd_set.contrast);
  avc_set_video_sature(avc_handle,  osd_set.chroma);

  sys_status_set_osd_set(&osd_set);
   sys_status_set_av_set(&av_set);
  sys_status_save();
  #ifdef OTA_DM_ON_STATIC_FLASH
  sys_static_write_set_boot_status();
  #else
  sys_status_set_boot_status();
  #endif
   mtos_task_delay_ms(1000);
  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
  manage_close_menu(SN_ROOT_ID_DISPLAY_SET, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_display_set(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	dialog.grade = DIALOG_INFO;
	dialog.x      = 180;
	dialog.y      = 170;
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
    
    on_display_set_save(p_ctrl);
    ret = FALSE;
  }
  else
  {
    manage_close_menu(ROOT_ID_DIALOG, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_display_set2 (control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	if((osd_set1.transparent==osd_set.transparent) 
			&& (av_set1.tv_ratio==av_set.tv_ratio)
			&& (timer_set1.gmt_offset==timer_set.gmt_offset)){
		manage_close_menu(SN_ROOT_ID_DISPLAY_SET, 0, 0);
	}
	else
	{
		dialog.grade = DIALOG_INFO;
		dialog.x      = 180;
		dialog.y      = 170;
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
    
    		on_display_set_save(p_ctrl);
    		ret = FALSE;
  		}
  		else
  		{
    		manage_close_menu(SN_ROOT_ID_DISPLAY_SET, 0, 0);
  		}
	}
  return SUCCESS;
}

BEGIN_KEYMAP(display_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(display_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(display_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_dislpay_set_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_dislpay_set_cont_focus_change) 
  ON_COMMAND(MSG_SELECT, on_display_set)
  ON_COMMAND(MSG_BACKSPACE, on_display_set2)
END_MSGPROC(display_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(display_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_display_set_changed)
  ON_COMMAND(MSG_SELECT, on_display_set)
  ON_COMMAND(MSG_BACKSPACE, on_display_set2)
END_MSGPROC(display_set_select_proc, cbox_class_proc)



