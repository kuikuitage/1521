/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_language_set.h"
#include "ui_channel_warn.h"
#include "ui_comm_dlg.h"
#include "ui_comm_root.h"
#include <string.h>

enum control_id
{
  IDC_INVALID = 0,
  IDC_LANG_SET,
  //IDC_OUTPUT_SET,
  IDC_MAX_CNT,
};
static language_set_t lang_set;
static language_set_t lang_set_1;
u16 language_set_cont_keymap(u16 key);
RET_CODE language_set_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE language_set_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static RET_CODE on_language_set_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

  return ERR_NOFEATURE;
}

RET_CODE open_language_set(u32 para1, u32 para2)
{
#define SN_LANGUAGE_X	160
#define SN_LANGUAGE_Y 	160
#define SN_LANGUAGE_W   360
#define SN_LANGUAGE_H   180
#define SN_LANGUAGE_SELECT_X	60
#define SN_LANGUAGE_SELECT_Y	80
  control_t *p_cont, *p_ctrl_item,*p_bg;
  u8 j;
  //rect_t frm = {0};
  u16 para_set_item[3] = {IDC_LANG_SET,2,  IDS_LANGUAGE_SET};

  u16 content [2] = { IDS_CHINESE, IDS_ENGLISH };
  p_cont = fw_create_mainwin(SN_ROOT_ID_LANGUAGE_SET,
                             SN_LANGUAGE_X, SN_LANGUAGE_Y, SN_LANGUAGE_W, SN_LANGUAGE_H,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  p_bg = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_BG_CONT,
							  0, 0, SN_LANGUAGE_W, SN_LANGUAGE_H, p_cont, 0);
ctrl_set_rstyle(p_bg, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, language_set_cont_keymap);
  ctrl_set_proc(p_cont, language_set_cont_proc);

  //create menu item
   p_ctrl_item = ui_comm_select_create(p_cont, IDC_LANG_SET,SN_LANGUAGE_SELECT_X,
   							SN_LANGUAGE_SELECT_Y,
                                         (SN_LANGUAGE_W-(2*SN_LANGUAGE_SELECT_X)-20)/2+30, 
                                         (SN_LANGUAGE_W-(2*SN_LANGUAGE_SELECT_X)-20)/2-10);
    
    ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    ui_comm_ctrl_set_txt_font_style(p_ctrl_item, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
    ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item, STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item, STL_LEFT|STL_VCENTER);
    ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item, RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
	ui_comm_ctrl_set_txt_rstyle(p_ctrl_item, RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
    
    ui_comm_ctrl_set_proc(p_ctrl_item, language_set_select_proc);
    ui_comm_select_set_static_txt(p_ctrl_item,IDS_LANGUAGE_SET);
    ui_comm_select_set_param(p_ctrl_item, TRUE, CBOX_WORKMODE_STATIC,
                                   para_set_item[1], CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < 2; j++)
          ui_comm_select_set_content(p_ctrl_item, j, content[j]);

    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item, RSI_SN_ALL_BG, RSI_SN_ALL_BG, RSI_SN_ALL_BG);

  /* set focus according to current info */
  sys_status_get_lang_set(&lang_set);
  memset(&lang_set_1, 0, sizeof(language_set_t));
  memcpy(&lang_set_1,&lang_set,(sizeof(language_set_t)));
  ui_comm_select_set_focus(p_ctrl_item, lang_set.osd_text);

  ctrl_default_proc(p_ctrl_item, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_language_set_changed(control_t *p_ctrl, u16 msg,
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
    case IDC_LANG_SET:
      lang_set.osd_text = focus;
      break;
      
    default:
	MT_ASSERT(0);
      break;
  } 
  
  return ret;
}

 static RET_CODE on_language_set_save(control_t *p_ctrl)
 {
   book_pg_t temp_node[MAX_BOOK_PG];
   control_t *pctrl;
   memset(&temp_node, 0, sizeof(book_pg_t)*MAX_BOOK_PG);
 
   rsc_set_curn_language(gui_get_rsc_handle(),lang_set.osd_text + 1);
   #ifndef WIN32
   ui_ca_set_language(lang_set);
   #endif
   // set time
 
   
   gdi_set_enable(TRUE);
 
   sys_status_set_lang_set(&lang_set);
 
   sys_status_save();
#ifdef OTA_DM_ON_STATIC_FLASH
   sys_static_write_ota_language(lang_set.osd_text);
   sys_static_write_set_boot_status();
#else
   sys_status_set_boot_status();
#endif
   //ui_comm_prompt_open(&warn_data_t);
   pctrl =	fw_find_root_by_id(SN_ROOT_ID_LANGUAGE_SET);
   ctrl_paint_ctrl(pctrl, TRUE);
   
   mtos_task_delay_ms(500);
   manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
   manage_close_menu(SN_ROOT_ID_LANGUAGE_SET, 0, 0);
   pctrl =	fw_find_root_by_id(SN_ROOT_ID_SUBMENU);
   ctrl_paint_ctrl(pctrl, TRUE);
 
   return SUCCESS;
 }


static RET_CODE on_language_set(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	dialog.grade = DIALOG_INFO;
	dialog.x      = 185;
	dialog.y      = 175;
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
    
    on_language_set_save(p_ctrl);
    ret = FALSE;
  }
  else
  {
    manage_close_menu(ROOT_ID_DIALOG, 0, 0);
  }
  return SUCCESS;
}

   static RET_CODE on_language_set2 (control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	if(lang_set_1.osd_text==lang_set.osd_text){
		manage_close_menu(SN_ROOT_ID_LANGUAGE_SET, 0, 0);
	}
	else{
		dialog.grade = DIALOG_INFO;
		dialog.x      = 185;
		dialog.y      = 175;
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
    
   	 on_language_set_save(p_ctrl);
   	 ret = FALSE;
 	 }
 	 else
  	{
   	 manage_close_menu(SN_ROOT_ID_LANGUAGE_SET, 0, 0);
  	}
 }
  return SUCCESS;
}

BEGIN_KEYMAP(language_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK,MSG_BACKSPACE)
END_KEYMAP(language_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(language_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_language_set_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_language_set_cont_focus_change) 
  ON_COMMAND(MSG_SELECT, on_language_set)
  ON_COMMAND(MSG_BACKSPACE,on_language_set2)
END_MSGPROC(language_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(language_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_language_set_changed)
  ON_COMMAND(MSG_SELECT, on_language_set)
  ON_COMMAND(MSG_BACKSPACE,on_language_set2)
END_MSGPROC(language_set_select_proc, cbox_class_proc)



