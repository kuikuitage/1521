
/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept.h"
#include "ui_ca_public.h"



#include "ui_pause.h"
#include "ui_notify.h"
#include "sys_dbg.h"

#include "ui_mainmenu.h"
#include "ui_restore_factory.h"



//#ifdef PATULOUS_FUNCTION_CA
#define YXSB_CONDITIONAL_ACCEPT_BTN_CNT      6

static u8 g_sub_curn_index = 0;

cas_card_info_t *p_card_info = NULL;

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
static card_and_ipp_info p_card_ipp_info ;
#endif


enum ca_btn_id
{
  IDC_INVALID = 0,

  IDC_BTN_CA_CARD_INFO ,
  IDC_BTN_CA_INFO2,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_ALERT_MESS,
  IDC_BTN_STB_PAIR,
  IDC_BTN_CARD_MANAGE,
};

u16 card_info_item_keymap(u16 key);

RET_CODE card_info_item_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);


u16 conditional_accept_cont_keymap(u16 key);

RET_CODE conditional_accept_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_conditional_accept(u32 para1, u32 para2)
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
	
	
	  control_t *p_cont = NULL;//, *p_ctrl_item[5]
	  control_t *p_item = NULL;
	  //control_t *p_icon = NULL;
	 // control_t *p_mw = NULL;
	  u16 i,x,y;
	  
	  u16 item_str[YXSB_CONDITIONAL_ACCEPT_BTN_CNT] =
	  {
		    IDS_CA_CARD_INFO,IDS_ACCREDIT_INFO, IDS_LEVEL_CONTROL_INFO,
    		IDS_PIN_MODIFY, IDS_SMARTCARD_FEED,IDS_ANNOUNCE_INFO
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
	  // create container    
	  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT,
								 0, 0,
								 SN_SUBMENU_W, SN_SUBMENU_H,
								 IDS_CA_INFO,TRUE);
	  if(p_cont == NULL)
	  {
		return ERR_FAILURE;
	  } 
	  ctrl_set_keymap(p_cont, conditional_accept_cont_keymap);
	  ctrl_set_proc(p_cont, conditional_accept_cont_proc);
	
	  // create 
	  for(i = 0; i < YXSB_CONDITIONAL_ACCEPT_BTN_CNT; i++)
	  {
		  if(i < YXSB_CONDITIONAL_ACCEPT_BTN_CNT/2)
		  {
			x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2;
			y = SN_SUBMENU_BTN_Y + i*SN_SUBMENU_BTN_H ;
		  }
		  else
		  {
			x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2+SN_SUBMENU_BTN_W;
			y = SN_SUBMENU_BTN_Y + ((i - YXSB_CONDITIONAL_ACCEPT_BTN_CNT/2)*SN_SUBMENU_BTN_H);
		  }
		  //butto
		  p_item = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_BTN_CA_CARD_INFO + i),
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

	  p_item = ctrl_get_child_by_id(p_cont, g_sub_curn_index + IDC_BTN_CA_CARD_INFO);//           
	  ctrl_default_proc(p_item, MSG_GETFOCUS, 0, 0);                          //
	  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
	  sn_submenu_help_data.x=0;
	  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
	  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
	  sn_submenu_help_data.offset=37;
	  sn_submenu_help_data.rsc =help_item;
	  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
	  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#ifndef WIN32
	  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);

	  ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0,0);
	  ui_ca_get_info((u32)CAS_CMD_OPERATOR_INFO_GET, 0 ,0);
#endif
	  return SUCCESS;

}

static RET_CODE on_conditional_accept_card_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  //RET_CODE ret = para1;
  //u8 i = 0;
  p_card_info = (cas_card_info_t *)para2;
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
  p_card_ipp_info.p_card_info =p_card_info;
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_card_burses_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  //RET_CODE ret = para1;
  //u8 i = 0;
   #if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
   p_card_ipp_info.p_ipp_buy_info= (burses_info_t *)para2;
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_ope_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF) || (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  extern void g_set_operator_info(cas_operators_info_t *p);
  cas_operators_info_t *oper_info = NULL;
  oper_info = (cas_operators_info_t *)para2;
  OS_PRINTF("!!!!!!!!!!!!!!!!!!!operator max num : %d\n",oper_info->max_num);
  g_set_operator_info(oper_info);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  p_card_info = NULL;

  return SUCCESS;
}

static RET_CODE on_conditional_accept_cont_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{

  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT, 0, 0);
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
	  g_sub_curn_index = (g_sub_curn_index + YXSB_CONDITIONAL_ACCEPT_BTN_CNT/2) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_BTN_CA_CARD_INFO);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }   
	  break;  
    case MSG_FOCUS_LEFT:
	  g_sub_curn_index = (g_sub_curn_index + YXSB_CONDITIONAL_ACCEPT_BTN_CNT/2) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_BTN_CA_CARD_INFO);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }	 
	  break;
    case MSG_FOCUS_UP:
      g_sub_curn_index = (g_sub_curn_index + YXSB_CONDITIONAL_ACCEPT_BTN_CNT - 1) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_BTN_CA_CARD_INFO);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
        g_sub_curn_index = (g_sub_curn_index + YXSB_CONDITIONAL_ACCEPT_BTN_CNT - 1) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
      }      
      break;
    case MSG_FOCUS_DOWN:
      g_sub_curn_index = (g_sub_curn_index + 1) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_BTN_CA_CARD_INFO);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = (g_sub_curn_index + 1) % YXSB_CONDITIONAL_ACCEPT_BTN_CNT;
      }      
      break;
  }
  p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_BTN_CA_CARD_INFO + g_sub_curn_index);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  
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
    case IDC_BTN_CA_CARD_INFO:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO, 0, 0);
      break;
      
    case IDC_BTN_CA_INFO2:
      manage_open_menu(ROOT_ID_PROVIDER_INFO, 0, 0);
      break;

    case IDC_BTN_CA_LEVEL_CONTROL:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
      break;

    case IDC_BTN_CA_PIN_MODIFY:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
      break;

    case IDC_BTN_CA_WORK_DURATION:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
      break;

	case IDC_BTN_ALERT_MESS:
      manage_open_menu(ROOT_ID_ALERT_MESS, 0, 0);
	  break;
/*
    case IDC_BTN_STB_PAIR: 
     	manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 0, 0);
      break;


    case IDC_BTN_CARD_MANAGE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER,0,0);
      break;
*/
     default:
      break;
  }

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




BEGIN_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)
//  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
//  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)
	//ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
	//ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)  
	ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_card_info_update) 
	ON_COMMAND(MSG_CA_BUR_INFO, on_conditional_accept_card_burses_info_update)  
	ON_COMMAND(MSG_CA_OPE_INFO, on_conditional_accept_ope_update)
	ON_COMMAND(MSG_SAVE, on_conditional_accept_save)
	ON_COMMAND(MSG_EXIT, on_conditional_accept_cont_exit)
	ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
END_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)



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

