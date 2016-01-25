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
#include "sys_dbg.h"

#include "ui_mainmenu.h"
#include "ui_restore_factory.h"

#include "ui_ca_info.h"

// test whm1210

enum osd_set_control_id
{

  IDC_INVALID = 0,

  //wanghm
  IDC_PROGRAM_SEARCH,
  IDC_DISPLAY_SET,
  IDC_SMARTCARD_INFO,
  IDC_SMARTCARD_SET,
  IDC_CHANGE_PIN_CODE,
  IDC_CA_VIEW_LEVEL,
  IDC_SP_INFO,
  //IDC_LANGUAGE_SET,
  //IDC_ADVANCED_SET,
  //IDC_CHANNEL_INFORMATION,

/*
  IDC_ICON_SEARCH,
  IDC_ICON_DISPLAY,
  IDC_ICON_SMARTCARD,
  IDC_ICON_SYSINFO,
  IDC_ICON_TRACK_SET,
  IDC_ICON_LANGUAGE_set,
  IDC_ICON_ADVANCED_SET,
  IDC_ICON_CHANNEL_INFORMATION,
  */

  //wanghm end
  //xuxin
//  IDC_SN_PROGRAM_SEARCH,
//  IDC_SN_DISPLAY_SET,
//  IDC_SN_SMARTCARD_INFO,
//  IDC_SN_SYSTEM_INFO,
//  IDC_SN_TRACK_SET,
//  IDC_SN_LANGUAGE_SET,
//  IDC_SN_ADVANCED_SET,
//  IDC_SN_CHANNEL_INFORMATION,
  //xuxin
  
  IDC_SUB_PREV,
  IDC_AD_WINDOW,
  IDC_SECOND_TITLE,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,

  IDC_BOOK_MANAGE,
  IDC_PASSWORD_CHANGE,
  IDC_AUTO_SEARCH,
  IDC_MANUAL_SEARCH, 
  IDC_FULL_SEARCH,
  IDC_FACTORY_SET,
  IDC_PARAME_SET,
  IDC_SIGNAL_CHECK,
  IDC_CONDITIONAL_ACCEPT,
  IDC_SOFTWARE_UPGRADE,
  IDC_FREQUENCE_SET,
};
enum submenu_local_msg
{
  MSG_OSD_VER_CHANGE = MSG_LOCAL_BEGIN + 600,
};

static u8 g_sub_curn_index = 0;
/*
static comm_help_data_t submenu_help_data = //help bar data
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
    IM_ICON_BACK,
    IM_ICON_SELECT,
    //IM_ICON_ARROW_LEFT_RIGHT,
    IM_ICON_ARROW_UP_DOWN,
  },
};
*/

u16 card_info_item_keymap(u16 key);

RET_CODE card_info_item_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 card_info_cont_keymap(u16 key);

RET_CODE card_info_cont_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 card_info_factory_pwdlg_keymap(u16 key);

RET_CODE card_info_factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 card_info_freq_set_pwdlg_keymap(u16 key);

RET_CODE card_info_freq_set_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 card_info_tp_frm_keymap(u16 key);


extern void restore_to_factory(void);

RET_CODE open_sn_ca_card_info(u32 para1, u32 para2)
{
#define SN_SUBMENU_W          640
#define SN_SUBMENU_H          480
#define SN_SUBMENU_TITLE_H	  57
#define SN_HELP_H			  43
#define SN_SUBMENU_BTNICON_X(x) 	(x+(SN_SUBMENU_BTN_W/2)/2)
#define SN_SUBMENU_BTNICON_Y(y)	(y+(SN_SUBMENU_BTN_H/2)/2)
#define SN_SUBMENU_BTNICON_W		30
#define SN_SUBMENU_BTNICON_H	  30
#define SN_SUBMENU_TITILHELP	102
#define SN_SUBMENU_BTN_W 	((SN_SUBMENU_W-20)/2)
#define SN_SUBMENU_BTN_H 	61
#define SN_SUBMENU_BTN_X1 	0
#define SN_SUBMENU_BTN_Y 	0
#define SN_SUB_MENU_HELP_RSC_CNT	15


  control_t *p_cont;//, *p_ctrl_item[5]
  control_t *p_item = NULL;
  //control_t *p_icon = NULL;
 // control_t *p_mw = NULL;
  u16 i,x,y;
  u16 item_str[SN_CA_INFO_CNT] =
  {
  	  IDS_SMARTCARD_INFO,
	  IDS_CA_AUTHOR_PROG_INFO,
	  IDS_SLOT_INFO,
      IDS_CA_WORK_TIME_SET,
      IDS_CA_CHANGE_PIN_CODE,
      IDS_CA_VIEW_LEVEL,
      IDS_SP_INFO,
      //IDS_LANGUAGE_SET,
      //IDS_ADVANCED_SET,
      //IDS_CHANNEL_INFORMATION  
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

  DEBUG_ENABLE_MODE(CA_CARD_INFO,INFO);
  DEBUG(CA_CARD_INFO,INFO,"open_sn_ca_card_info.....\n");  

  // create container
  p_cont = ui_background_create(SN_ROOT_ID_CARD_INFO,
                             0, 0,
                             SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_CA_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create 
  for(i = 0; i < SN_CA_INFO_CNT; i++)
  {
      if((i <= SN_CA_INFO_CNT/2))
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2;
        y = SN_SUBMENU_BTN_Y + i*SN_SUBMENU_BTN_H ;
      }
      else
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2+SN_SUBMENU_BTN_W;
        y = SN_SUBMENU_BTN_Y + (((i - 1) - SN_CA_INFO_CNT/2)*SN_SUBMENU_BTN_H);
      }

      //button
      p_item = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PROGRAM_SEARCH + i),
                                x, y, SN_SUBMENU_BTN_W,SN_SUBMENU_BTN_H,
                                p_cont, 0);
      ctrl_set_rstyle(p_item, RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECTH,RSI_SN_SET_BUTTON_SELECT);
      ctrl_set_keymap(p_item, card_info_item_keymap);
      ctrl_set_proc(p_item, card_info_item_proc);
      text_set_font_style(p_item, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
      text_set_align_type(p_item, STL_CENTER | STL_VCENTER);
       text_set_content_type(p_item, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_item, item_str[i]);
  	
   }

  p_item = ctrl_get_child_by_id(p_cont, g_sub_curn_index + IDC_PROGRAM_SEARCH);
  ctrl_default_proc(p_item, MSG_GETFOCUS, 0, 0);

  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  return SUCCESS;
}


static RET_CODE on_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  customer_cfg_t cfg = {0};
    get_customer_config(&cfg);
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl->p_parent));

  switch(ctrlID)
  {
    case IDC_PROGRAM_SEARCH:
      manage_open_menu(ROOT_ID_CA_CARD_INFO, 0, 0);
      break;
      
    case IDC_DISPLAY_SET:
      manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, 0, 0);
      break;

    case IDC_SMARTCARD_INFO:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, 0, 0);
      break;

    case IDC_SMARTCARD_SET:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
      break;

    case IDC_CHANGE_PIN_CODE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
      break;

    case IDC_CA_VIEW_LEVEL: 
     	manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
      break;


    case IDC_SP_INFO:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO,0,0);
      break;

     default:
      break;
  }

  return SUCCESS;
}


static RET_CODE on_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl;
  u8 pre_index = 0;

  pre_index = g_sub_curn_index;
  p_cont = p_ctrl->p_parent;

  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
	  g_sub_curn_index = (g_sub_curn_index + SN_CA_INFO_CNT/2 + 1) % SN_CA_INFO_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }   
	  break;  
    case MSG_FOCUS_LEFT:
	  g_sub_curn_index = (g_sub_curn_index + SN_CA_INFO_CNT/2) % SN_CA_INFO_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }	 
	  break;
    case MSG_FOCUS_UP:
      g_sub_curn_index = (g_sub_curn_index + SN_CA_INFO_CNT - 1) % SN_CA_INFO_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
        g_sub_curn_index = (g_sub_curn_index + SN_CA_INFO_CNT - 1) % SN_CA_INFO_CNT;
      }      
      break;
    case MSG_FOCUS_DOWN:
      g_sub_curn_index = (g_sub_curn_index + 1) % SN_CA_INFO_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = (g_sub_curn_index + 1) % SN_CA_INFO_CNT;
      }      
      break;
  }
  p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_PROGRAM_SEARCH + g_sub_curn_index);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  
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
  ui_comm_pwdlg_close();
 // delete_all_prog();
   restore_to_factory();
  db_previosly_program_for_ae();
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
  manage_open_menu(ROOT_ID_PROGRAM_SEARCH, 0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(card_info_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(card_info_item_keymap, NULL)

BEGIN_MSGPROC(card_info_item_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_change_focus)
  ON_COMMAND(MSG_PAINT, on_item_paint)
END_MSGPROC(card_info_item_proc, text_class_proc)

BEGIN_KEYMAP(card_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
END_KEYMAP(card_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
END_MSGPROC(card_info_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(card_info_factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_factory_pwdlg_keymap, NULL)

BEGIN_MSGPROC(card_info_factory_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_factory_pwdlg_correct)
  ON_COMMAND(MSG_PRESET_PROG, on_preset_prog)
  ON_COMMAND(MSG_EXIT, on_factory_pwdlg_exit)
END_MSGPROC(card_info_factory_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(card_info_freq_set_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_freq_set_pwdlg_keymap, NULL)

BEGIN_MSGPROC(card_info_freq_set_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_freq_set_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_freq_set_pwdlg_exit)
END_MSGPROC(card_info_freq_set_pwdlg_proc, cont_class_proc)
