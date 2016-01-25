/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_track_set.h"
#include "ui_channel_warn.h"
#include "ui_comm_dlg.h"
#include "ui_comm_root.h"
#include <string.h>


enum control_id
{
  IDC_INVALID = 0,
  SN_IDC_ALL_TRACK_SET,
  SN_IDC_TRACK_SET,
  //IDC_OUTPUT_SET,
  IDC_MAX_CNT,
};

static audio_set_t audio_set;
static audio_set_t audio_set1;
static dvbs_prog_node_t curn_prog;
static dvbs_prog_node_t curn_prog1;

u16 track_set_cont_keymap(u16 key);
RET_CODE track_set_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE track_set_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static RET_CODE on_track_set_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

  return ERR_NOFEATURE;
}
static control_t *p_ctrl_item[IDC_MAX_CNT]={NULL};
RET_CODE open_track_set(u32 para1, u32 para2)
{
#define SN_TRACK_SET_X		120
#define SN_TRACK_SET_Y		140
#define SN_TRACK_SET_CNT	2
#define SN_TRACK_W   400
#define SN_TRACK_H   200
#define SN_TRACK_SELECT_X	30
#define SN_TRACK_SELECT_Y	45//85
#define SN_TRACK_BMAP_W		12

  control_t *p_cont,*p_bg;
  u8 i, j;
  u8 index = 0;
  u16 prog_id = 0;
  u16 para_set_item [SN_TRACK_SET_CNT][3] =
  { 
    {SN_IDC_ALL_TRACK_SET,2,  IDS_TRACK_GLOBAL}, 
    {SN_IDC_TRACK_SET,      3,  IDS_TRACK_SET}, 
  };
  u16 content [SN_TRACK_SET_CNT][3] = 
  {
    { IDS_NO, IDS_YES},
    { IDS_STERO, IDS_LEFT_TRACK, IDS_RIGHT_TRACK},
  };

	p_cont = fw_create_mainwin(SN_ROOT_ID_TRACK_SET,
								 SN_TRACK_SET_X, SN_TRACK_SET_Y, SN_TRACK_W, SN_TRACK_H,
								 0, 0, OBJ_ATTR_ACTIVE, 0);
	  p_bg = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_BG_CONT,
								  0, 0, SN_TRACK_W, SN_TRACK_H, p_cont, 0);
	ctrl_set_rstyle(p_bg, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, track_set_cont_keymap);
  ctrl_set_proc(p_cont, track_set_cont_proc);

  //create menu item
  for (i = SN_IDC_ALL_TRACK_SET,index=0; i <= SN_IDC_TRACK_SET; i++,index++)
  {
  	//the COMM_SELECT wide of ALL_TRACK_SET is different from TRACK_SET
  	if(i == SN_IDC_ALL_TRACK_SET)
  	{
	    p_ctrl_item[i] = ui_comm_select_create(p_cont, i,SN_TRACK_SELECT_X,
										SN_TRACK_SELECT_Y+(index*60), 
										(SN_TRACK_W-(2*SN_TRACK_SELECT_X)-20)/2+60,
										(SN_TRACK_W-(2*SN_TRACK_SELECT_X)-20)/2-65);
  	}
	else
	{
		p_ctrl_item[i] = ui_comm_select_create(p_cont, i,SN_TRACK_SELECT_X,
											SN_TRACK_SELECT_Y+(index*60), 
											(SN_TRACK_W-(2*SN_TRACK_SELECT_X)-20)/2+40,
											(SN_TRACK_W-(2*SN_TRACK_SELECT_X)-20)/2-35);
	}
    
    ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
	ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
    ui_comm_ctrl_set_proc(p_ctrl_item[i], track_set_select_proc);
    ui_comm_select_set_static_txt(p_ctrl_item[i], para_set_item[index][2]);

    ui_comm_select_set_param(p_ctrl_item[i], TRUE, CBOX_WORKMODE_STATIC,
                               para_set_item[index][1], CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < para_set_item[index][1]; j++)
    {
      ui_comm_select_set_content(p_ctrl_item[i], j, content[index][j]);
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SN_SET_BUTTON_SELECT, RSI_IGNORE, RSI_SN_SET_BUTTON_SELECT);

  }
	
	ctrl_set_related_id(p_ctrl_item[SN_IDC_ALL_TRACK_SET],0,SN_IDC_TRACK_SET,0,SN_IDC_TRACK_SET);
	ctrl_set_related_id(p_ctrl_item[SN_IDC_TRACK_SET],0,SN_IDC_ALL_TRACK_SET,0,SN_IDC_ALL_TRACK_SET);
  /* set focus according to current info */
  sys_status_get_audio_set(&audio_set);
  memset(&audio_set1, 0, sizeof(audio_set_t));
  memcpy(&audio_set1,&audio_set,(sizeof(audio_set_t)));
  memset(&curn_prog1, 0, sizeof(dvbs_prog_node_t));
  memcpy(&curn_prog1,&curn_prog,(sizeof(dvbs_prog_node_t)));

    if ((prog_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
    {
      memset(&curn_prog, 0, sizeof(dvbs_prog_node_t));
      curn_prog.audio_track = 1;
      ctrl_set_attr(p_ctrl_item[SN_IDC_TRACK_SET], OBJ_ATTR_INACTIVE);
    }
    else if (db_dvbs_get_pg_by_id(prog_id, &curn_prog) != DB_DVBS_OK)
    {
      memset(&curn_prog, 0, sizeof(dvbs_prog_node_t));
      curn_prog.audio_track = 1;
      ctrl_set_attr(p_ctrl_item[SN_IDC_TRACK_SET], OBJ_ATTR_INACTIVE);
    }
    
    ui_comm_select_set_focus(p_ctrl_item[SN_IDC_ALL_TRACK_SET], (u16)audio_set.is_global_track);
	if(audio_set.is_global_track == 0)
	{
    	ui_comm_select_set_focus(p_ctrl_item[SN_IDC_TRACK_SET], (u16)curn_prog.audio_track);
	}
	else
	{
    	ui_comm_select_set_focus(p_ctrl_item[SN_IDC_TRACK_SET], (u16)audio_set.global_track);
	}

  	ctrl_default_proc(p_ctrl_item[SN_IDC_ALL_TRACK_SET], MSG_GETFOCUS, 0, 0);
  	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_track_set_changed(control_t *p_ctrl, u16 msg,
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
    case SN_IDC_TRACK_SET:
		
      if(audio_set.is_global_track != 0)
      {
        audio_set.global_track= focus;
      }
      else
      {
        curn_prog.audio_track = focus;
      }
      break;
	case SN_IDC_ALL_TRACK_SET:
        audio_set.is_global_track= focus;
		if (audio_set.is_global_track == 0)
		{
			ui_comm_select_set_focus(p_ctrl_item[SN_IDC_TRACK_SET], (u16)curn_prog.audio_track);
		}
		else
		{
			ui_comm_select_set_focus(p_ctrl_item[SN_IDC_TRACK_SET], (u16)audio_set.global_track);
		}
		ctrl_paint_ctrl(ctrl_get_root(p_ctrl), FALSE);
		break;
    default:
      break;
  } 
  
  return ret;
}

static RET_CODE on_track_set_save(control_t *p_ctrl)
{
	if(audio_set.is_global_track != 0)
	{
		avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), audio_set.global_track);
		sys_status_set_audio_set(&audio_set);
	}
	else
	{
		sys_status_set_audio_set(&audio_set);
		avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), curn_prog.audio_track);
		db_dvbs_edit_program(&curn_prog);
		db_dvbs_save_pg_edit(&curn_prog);
	}

	sys_status_save();
  
	manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
	manage_close_menu(SN_ROOT_ID_TRACK_SET, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_track_set(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
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
	ret=ui_comm_dialog_open(&dialog);

  if(DLG_RET_YES==ret)
  {
    
    on_track_set_save(p_ctrl);
    ret = FALSE;
  }
  else
  {
    manage_close_menu(ROOT_ID_DIALOG, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_track_set2 (control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	if((audio_set1.global_track==audio_set.global_track)
		&&(audio_set1.is_global_track==audio_set.is_global_track)
		&&(curn_prog1.audio_track==curn_prog.audio_track)){
		manage_close_menu(SN_ROOT_ID_TRACK_SET, 0, 0);
	}
	else
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
	    
	    on_track_set_save(p_ctrl);
	    ret = FALSE;
	  }
	  else
  	  {
  	    manage_close_menu(SN_ROOT_ID_TRACK_SET, 0, 0);
  	  }
    }
  	return SUCCESS;
}

BEGIN_KEYMAP(track_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK,MSG_BACKSPACE)
END_KEYMAP(track_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(track_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_track_set_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_track_set_cont_focus_change) 
  ON_COMMAND(MSG_SELECT, on_track_set)
  ON_COMMAND(MSG_BACKSPACE,on_track_set2)
END_MSGPROC(track_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(track_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_track_set_changed)
  ON_COMMAND(MSG_SELECT, on_track_set)
  ON_COMMAND(MSG_BACKSPACE,on_track_set2)
END_MSGPROC(track_set_select_proc, cbox_class_proc)



