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
#include "ui_sub_menu.h"
#include "ui_restore_factory.h"
#include "ui_comm_root.h"

#include "sys_dbg.h"

// test whm1210

enum osd_set_control_id
{

  IDC_INVALID = 0,

  //wanghm
  IDC_PROGRAM_SEARCH,
  IDC_DISPLAY_SET,
  IDC_SMARTCARD_INFO,
  IDC_SYSTEM_INFO,
  IDC_TRACK_SET,
  IDC_LANGUAGE_SET,
  IDC_ADVANCED_SET,
  IDC_CHANNEL_INFORMATION,

  IDC_ICON_SEARCH,
  IDC_ICON_DISPLAY,
  IDC_ICON_SMARTCARD,
  IDC_ICON_SYSINFO,
  IDC_ICON_TRACK_SET,
  IDC_ICON_LANGUAGE_set,
  IDC_ICON_ADVANCED_SET,
  IDC_ICON_CHANNEL_INFORMATION,
  //wanghm end
  
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

u16 sub_menu_item_keymap(u16 key);

RET_CODE sub_menu_item_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 sub_menu_cont_keymap(u16 key);

RET_CODE sub_menu_cont_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 factory_pwdlg_keymap(u16 key);

RET_CODE factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 freq_set_pwdlg_keymap(u16 key);

RET_CODE freq_set_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

extern void restore_to_factory(void);

RET_CODE open_sub_menu(u32 para1, u32 para2)
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


  control_t *p_cont;
  control_t *p_item = NULL;
  control_t *p_icon = NULL;
  u16 i,x,y;
  u16 item_str[SN_SUBMENU_CNT] =
  {
  	  IDS_SEARCH,
	  IDS_DISPLAY_SET,
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
	  IDS_CA_INFO,//IDS_CA_CARD_INFO,
#else
	  IDS_CA_INFO,
#endif
      IDS_SYSTEM_INFO,
      IDS_TRACK_SET,
      IDS_LANGUAGE_SET,
      IDS_ADVANCED_SET,
      IDS_CHANNEL_INFORMATION  
  };
    u16 item_icon[SN_SUBMENU_CNT] =
  {
  	  IM_SN_ICON_SEARCH,
	  IM_SN_ICON_VIEW,
	  IM_SN_ICON_CAINFO,
	  IM_SN_ICON_SYSINFO,
      IM_SN_ICON_VIEW_TRACK,
      IM_SN_ICON_LANGUAGE,
      IM_SN_ICON_SETTING,
      IM_SN_ICON_PROGINFO
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
  DEBUG_ENABLE_MODE(SYS_SET,INFO);

  // create container
  p_cont = ui_background_create(SN_ROOT_ID_SUBMENU,
                             0, 0,
                             SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_SYSTEM_SET,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create 
  for(i = 0; i < SN_SUBMENU_CNT; i++)
  {
      if(i < SN_SUBMENU_CNT/2)
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2;
        y = SN_SUBMENU_BTN_Y + i*SN_SUBMENU_BTN_H ;
      }
      else
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2+SN_SUBMENU_BTN_W;
        y = SN_SUBMENU_BTN_Y + ((i-SN_SUBMENU_CNT/2)*SN_SUBMENU_BTN_H);
      }
	  
      //button
      p_item = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PROGRAM_SEARCH + i),
                                x, y, SN_SUBMENU_BTN_W,SN_SUBMENU_BTN_H,
                                p_cont, 0);
      ctrl_set_rstyle(p_item, RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECTH,RSI_SN_SET_BUTTON_SELECT);
      ctrl_set_keymap(p_item, sub_menu_item_keymap);
      ctrl_set_proc(p_item, sub_menu_item_proc);
      text_set_font_style(p_item, FSI_COMM_BTN, FSI_COMM_TXT_HL, FSI_COMM_BTN);
      text_set_align_type(p_item, STL_LEFT | STL_VCENTER);
	  text_set_offset(p_item, SN_SUBMENU_BTN_W/3+12, 0);
      text_set_content_type(p_item, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_item, item_str[i]);

	  p_icon = ctrl_create_ctrl((u8*)CTRL_BMAP,(u8)(IDC_ICON_SEARCH + i),(SN_SUBMENU_BTN_W/4)-(SN_SUBMENU_BTNICON_W/2),
	  								((SN_SUBMENU_BTN_H/2)-(SN_SUBMENU_BTNICON_W/2)),SN_SUBMENU_BTNICON_W,SN_SUBMENU_BTNICON_H,
	  								p_item,0);
	  ctrl_set_rstyle(p_icon, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	  bmap_set_content_by_id(p_icon, item_icon[i]);
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
      manage_open_menu(SN_ROOT_ID_SEARCH, 0, 0);
      break;
      
    case IDC_DISPLAY_SET:
      manage_open_menu(SN_ROOT_ID_DISPLAY_SET, 0, 0);
      break;

    case IDC_SMARTCARD_INFO:
#if((CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB) || (CONFIG_CAS_ID == CONFIG_CAS_ID_TR))
	  manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT, 0, 0);
#elif((CONFIG_CAS_ID == CONFIG_CAS_ID_GS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_QL))
	  manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT, 0, 0);
#elif(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	  manage_open_menu(SN_ROOT_ID_CARD_INFO, 0, 0);
#else
	  manage_open_menu(ROOT_ID_CA_INFO, 0, 0);
#endif
      break;

    case IDC_SYSTEM_INFO:
      manage_open_menu(ROOT_ID_SYS_INFO, 0, 0);
      break;

    case IDC_TRACK_SET:
      manage_open_menu(SN_ROOT_ID_TRACK_SET, 0, 0);
      break;

    case IDC_LANGUAGE_SET: 
     	manage_open_menu(SN_ROOT_ID_LANGUAGE_SET, 0, 0);
      break;

    case IDC_ADVANCED_SET:
      manage_open_menu(ROOT_ID_SENIOR_SUBMENU,0,0);
      break;

    case IDC_CHANNEL_INFORMATION: 
      manage_open_menu(SN_ROOT_ID_PRO_INFO, 0, 0);
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
    case MSG_FOCUS_LEFT:
      g_sub_curn_index = (g_sub_curn_index + SN_SUBMENU_CNT/2) % SN_SUBMENU_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = pre_index;
      }      
      break;
    case MSG_FOCUS_UP:
      g_sub_curn_index = (g_sub_curn_index + SN_SUBMENU_CNT - 1) % SN_SUBMENU_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
        g_sub_curn_index = (g_sub_curn_index + SN_SUBMENU_CNT - 1) % SN_SUBMENU_CNT;
      }      
      break;
    case MSG_FOCUS_DOWN:
      g_sub_curn_index = (g_sub_curn_index + 1) % SN_SUBMENU_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_PROGRAM_SEARCH);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = (g_sub_curn_index + 1) % SN_SUBMENU_CNT;
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

BEGIN_KEYMAP(sub_menu_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(sub_menu_item_keymap, NULL)

BEGIN_MSGPROC(sub_menu_item_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_change_focus)
  ON_COMMAND(MSG_PAINT, on_item_paint)
END_MSGPROC(sub_menu_item_proc, text_class_proc)

BEGIN_KEYMAP(sub_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
END_KEYMAP(sub_menu_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(sub_menu_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
END_MSGPROC(sub_menu_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(factory_pwdlg_keymap, NULL)

BEGIN_MSGPROC(factory_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_factory_pwdlg_correct)
  ON_COMMAND(MSG_PRESET_PROG, on_preset_prog)
  ON_COMMAND(MSG_EXIT, on_factory_pwdlg_exit)
END_MSGPROC(factory_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(freq_set_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(freq_set_pwdlg_keymap, NULL)

BEGIN_MSGPROC(freq_set_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_freq_set_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_freq_set_pwdlg_exit)
END_MSGPROC(freq_set_pwdlg_proc, cont_class_proc)

