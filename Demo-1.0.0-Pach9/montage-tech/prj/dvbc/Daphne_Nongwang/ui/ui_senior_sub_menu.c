/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_notify.h"

#include "ui_mainmenu.h"
#include "ui_senior_sub_menu.h"
#include "ui_restore_factory.h"
#include "ui_comm_root.h"
#include "ui_comm_dlg.h"


enum osd_set_control_id
{
  IDC_INVALID = 0,

  IDC_FACTORY_SET,
  IDC_FREQUENCE_SET,
  IDC_PASSWORD_CHANGE,
  
  IDC_SUB_PREV,
  IDC_AD_WINDOW,
  IDC_SECOND_TITLE,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,
  
  IDC_BOOK_MANAGE,
  IDC_AUTO_SEARCH,
  IDC_MANUAL_SEARCH, 
  IDC_FULL_SEARCH,
  IDC_PARAME_SET,
  IDC_SIGNAL_CHECK,
  IDC_CONDITIONAL_ACCEPT,
  IDC_SOFTWARE_UPGRADE,

};
enum submenu_local_msg
{
  MSG_OSD_VER_CHANGE = MSG_LOCAL_BEGIN + 600,
};

static u8 g_sub_curn_index = 0;
u16 senior_sub_menu_item_keymap(u16 key);

RET_CODE senior_sub_menu_item_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 senior_sub_menu_cont_keymap(u16 key);

RET_CODE senior_sub_menu_cont_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 senior_factory_pwdlg_keymap(u16 key);

RET_CODE senior_factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 senior_freq_set_pwdlg_keymap(u16 key);

RET_CODE senior_freq_set_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

extern void restore_to_factory(void);

RET_CODE open_senior_sub_menu(u32 para1, u32 para2)
{
  #define SN_SUB_MENU_HELP_RSC_CNT				15
  #define SN_SCREEN_WIDTH						640
  #define SN_SCREEN_HEIGHT						480
  #define SENIOR_SUBMENU_CNT                    3
  #define SN_SENIOR_SUBMENU_X					(SN_SCREEN_WIDTH-SN_SENIOR_SUBMENU_W)/2
  #define SN_SENIOR_SUBMENU_Y					(SN_SCREEN_HEIGHT-SN_SENIOR_SUBMENU_H)/2
  #define SN_SENIOR_SUBMENU_W					SN_SCREEN_WIDTH
  #define SN_SENIOR_SUBMENU_H					SN_SCREEN_HEIGHT

  //button
  #define SENIOR_SUBMENU_BTN_X                  (SN_SENIOR_SUBMENU_W - SENIOR_SUBMENU_BTN_W)/2//180
  #define SENIOR_SUBMENU_BTN_Y                  25
  #define SENIOR_SUBMENU_BTN_W                  300
  #define SENIOR_SUBMENU_BTN_H                  61

  #define SENIOR_SUBMENU_BTN_V_GAP              50
  

  control_t *p_cont;//, *p_tv_win, *p_ad_win, *p_second_title
  control_t *p_item = NULL;
  u16 i,x,y;
  u16 item_str[SENIOR_SUBMENU_CNT] =
  {	  
      IDS_RESET_FACTORY,           
      IDS_CA_MAIN_FREQ_SET,//IDS_MAIN_FREQ_SET,                    
      IDS_PASSWORD_SET, 
  };
  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
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
  
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  
  p_cont = ui_background_create(ROOT_ID_SENIOR_SUBMENU,
                             SN_SENIOR_SUBMENU_X, SN_SENIOR_SUBMENU_Y,
                             SN_SENIOR_SUBMENU_W, SN_SENIOR_SUBMENU_H,
                             IDS_ADVANCED_SET,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create 
  for(i = 0; i < SENIOR_SUBMENU_CNT; i++)//SUBMENU_CNT
  {
        x = SENIOR_SUBMENU_BTN_X;
        y = SENIOR_SUBMENU_BTN_Y + i*(SENIOR_SUBMENU_BTN_H + SENIOR_SUBMENU_BTN_V_GAP);//SUBMENU_BTN_V_GAP

      //button
      p_item = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_FACTORY_SET + i),

		  x, y, SENIOR_SUBMENU_BTN_W,SENIOR_SUBMENU_BTN_H,
                                p_cont, 0);
      ctrl_set_rstyle(p_item, RSI_SN_BUTTON_SET_BLUE_SH, RSI_SN_BUTTON_SET_YELLOW_HL,RSI_SN_SET_BUTTON_SELECT);
      ctrl_set_keymap(p_item, senior_sub_menu_item_keymap);
      ctrl_set_proc(p_item, senior_sub_menu_item_proc);
      text_set_font_style(p_item, FSI_INDEX3, FSI_INDEX4, FSI_INDEX3);
      text_set_align_type(p_item, STL_CENTER | STL_VCENTER);
      text_set_content_type(p_item, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_item, item_str[i]);

  }

  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  p_item = ctrl_get_child_by_id(p_cont, g_sub_curn_index + IDC_FACTORY_SET);
  ctrl_default_proc(p_item, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  customer_cfg_t cfg = {0};
  comm_pwdlg_data_t factory_pwdlg_data =
  {
    ROOT_ID_SENIOR_SUBMENU,
    SUBMENU_PREV_X,
    SUBMENU_PREV_Y+SUBMENU_PREV_H-PWDLG_H,
    IDS_INPUT_PASSWORD,
    0,
    senior_factory_pwdlg_keymap,
    senior_factory_pwdlg_proc,
  };

    comm_pwdlg_data_t freq_set_pwdlg_data =
  {
    ROOT_ID_SENIOR_SUBMENU,
    SUBMENU_PREV_X,
    SUBMENU_PREV_Y+SUBMENU_PREV_H-PWDLG_H,
    IDS_INPUT_PASSWORD,
    0,
    senior_freq_set_pwdlg_keymap,
    senior_freq_set_pwdlg_proc,
  };

   get_customer_config(&cfg);
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl->p_parent));

  switch(ctrlID)
  {
    case IDC_PASSWORD_CHANGE:
      manage_open_menu(ROOT_ID_PASSWORD_MODIFY, 0, 0);
      break;

    case IDC_FACTORY_SET:
      ui_comm_pwdlg_open(&factory_pwdlg_data);
      break;

    case IDC_FREQUENCE_SET:
      if(CUSTOMER_YINGJI == cfg.customer)
      {
        ui_comm_pwdlg_open(&freq_set_pwdlg_data);
      }else
      {
        manage_open_menu(ROOT_ID_TP_SET, 0, 0);
      }
      break;

    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE on_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #define SENIOR_SUBMENU_CNT                3

  control_t *p_cont, *p_next_ctrl;
  u8 pre_index = 0;

  pre_index = g_sub_curn_index;
  p_cont = p_ctrl->p_parent;

  switch(msg)
  {
    case MSG_FOCUS_UP:
      g_sub_curn_index = (g_sub_curn_index + SENIOR_SUBMENU_CNT - 1) % SENIOR_SUBMENU_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_FACTORY_SET);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
        g_sub_curn_index = (g_sub_curn_index + SENIOR_SUBMENU_CNT - 1) % SENIOR_SUBMENU_CNT;
      }      
      break;
    case MSG_FOCUS_DOWN:
      g_sub_curn_index = (g_sub_curn_index + 1) % SENIOR_SUBMENU_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_FACTORY_SET);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = (g_sub_curn_index + 1) % SENIOR_SUBMENU_CNT;
      }      
      break;
  }
  p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_FACTORY_SET + g_sub_curn_index);
  
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, FALSE);
  ctrl_paint_ctrl(p_next_ctrl, FALSE);
  
  return SUCCESS;
}


static RET_CODE on_item_paint(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{  
  return ERR_NOFEATURE;
}

static RET_CODE on_paint_sub_menu(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  cont_class_proc(ctrl, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();
  manage_open_menu(ROOT_ID_RESTORE_FACTORY, 0, 0);
      
  return SUCCESS;
}

static RET_CODE on_preset_prog(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
	control_t *pctrl;
	dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	dialog.grade = DIALOG_INFO;
	dialog.x	  = 216;
	dialog.y	  = 183;
	dialog.w	 = 250;
	dialog.h	 = 135;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID	= IDS_CONFIRM_TO_RESTORE;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;
	if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL) // already opened
	{
		ui_comm_pwdlg_close();
	}
	mtos_task_delay_ms(500);
	ret=ui_comm_dialog_open(&dialog);

	if(DLG_RET_YES==ret)
	{
		comm_prompt_data_t warn_data_t =
    {
      ROOT_ID_SENIOR_SUBMENU,
	  STR_MODE_STATIC,
      216,183,250,135,15,
      IDS_SAVE_DATA,0,
      0,DIALOG_DEADLY,
      3000
    };
		ui_comm_prompt_open(&warn_data_t);
  		mtos_task_delay_ms(10);
  		manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
		ui_comm_pwdlg_close();
		pctrl =	fw_find_root_by_id(ROOT_ID_SENIOR_SUBMENU);
   		ctrl_paint_ctrl(pctrl, TRUE);
 	  restore_to_factory();
		pctrl =	fw_find_root_by_id(ROOT_ID_SENIOR_SUBMENU);
   		ctrl_paint_ctrl(pctrl, TRUE);
		ret = FALSE;
	 }
	else
	{
		ui_comm_pwdlg_close();
	 	pctrl =	fw_find_root_by_id(ROOT_ID_SENIOR_SUBMENU);
   		ctrl_paint_ctrl(pctrl, TRUE);
	}
	  return SUCCESS;
}

static RET_CODE on_freq_set_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_freq_set_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_freq_set_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();
  manage_open_menu(ROOT_ID_TP_SET, 0, 0);
  return SUCCESS;
}

static RET_CODE on_sub_menu_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(senior_sub_menu_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(senior_sub_menu_item_keymap, NULL)

BEGIN_MSGPROC(senior_sub_menu_item_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_change_focus)
  ON_COMMAND(MSG_PAINT, on_item_paint)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_sub_menu_draw_pic_end)
END_MSGPROC(senior_sub_menu_item_proc, text_class_proc)

BEGIN_KEYMAP(senior_sub_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(senior_sub_menu_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(senior_sub_menu_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
END_MSGPROC(senior_sub_menu_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(senior_factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(senior_factory_pwdlg_keymap, NULL)

BEGIN_MSGPROC(senior_factory_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_factory_pwdlg_correct)
  ON_COMMAND(MSG_PRESET_PROG, on_preset_prog)
  ON_COMMAND(MSG_EXIT, on_factory_pwdlg_exit)
  ON_COMMAND(MSG_BACKSPACE, on_factory_pwdlg_exit)
END_MSGPROC(senior_factory_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(senior_freq_set_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(senior_freq_set_pwdlg_keymap, NULL)

BEGIN_MSGPROC(senior_freq_set_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_freq_set_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_freq_set_pwdlg_exit)
  ON_COMMAND(MSG_BACKSPACE, on_freq_set_pwdlg_exit)
END_MSGPROC(senior_freq_set_pwdlg_proc, cont_class_proc)


