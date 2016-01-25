/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_dvb_conditional_accept_level.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_CA,
};

enum ca_level_control_id
{
  IDC_CA_LEVEL_CONTROL_PIN = 1,
  IDC_CA_LEVEL_CONTROL_LEVEL,
  IDC_CA_LEVEL_CHANGE_OK,
  IDC_CA_LEVEL_CHANGE_CANCEL,
  IDC_CA_LEVEL_CHANGE_RESULT,
  IDC_CA_LEVEL_CHANGE_RESULT2,
};

comm_prompt_data_t level_data_t =
	 {
	   ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,
	   STR_MODE_STATIC,
	   200,165,250,150,20,
	   RSC_INVALID_ID,RSC_INVALID_ID,
	   RSC_INVALID_ID,DIALOG_DEADLY,
	   3000
	 };

static cas_rating_set_t rate_info;

u16 conditional_accept_level_cont_keymap(u16 key);
RET_CODE conditional_accept_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);


u16 conditional_accept_level_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_level_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE conditional_accept_level_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_level_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void conditional_accept_level_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_CA);
  control_t *p_level = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CONTROL_LEVEL);
  u16 focus = 0;
  if(p_card_info == NULL)
  {
    ui_comm_select_set_focus(p_level, 0);
  }
  else
  {
  
      switch(p_card_info->card_work_level)
    {
    
		  case 4: focus = 0;break;
		  case 5: focus = 1;break;		  	
		  case 6: focus = 2;break;		  
		  case 7: focus = 3;break;		  	
		  case 8: focus = 4;break;		  	
		  case 9: focus = 5;break;		  	
		  case 10: focus = 6;break;		  	
		  case 11: focus = 7;break;		  	
		  case 12: focus = 8;break;		  	
		  case 13: focus = 9;break;		  	
		  case 14: focus = 10;break;		  	
		  case 15: focus = 11;break;		  
		  case 16: focus = 12;break;		  	
		  case 17: focus = 13;break;		  	
		  case 18: focus = 14;break;		  	
		  default: focus = 0;break;
		}
    }
    

    ui_comm_select_set_focus(p_level, focus);
  }



RET_CODE open_dvbcas_accept_level(u32 para1, u32 para2)
{
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT 4
  #define SN_CONDITIONAL_ACCEPT_LEVEL_HELP_RSC_CNT	  15
  //CA frm
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_X             106
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_Y             24
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_W             505
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_H             360
  
  //level control items
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X        10
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y        20
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW       260
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW       150
  #define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H        35
  
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X        140
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_Y        130
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W        70
  #define SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H        25


  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  control_t *p_ctrl2 = NULL;
  control_t *p_ca_frm;
  cas_card_info_t *P_card_info_t = NULL;
  u8 i;
  u16 y;
  u16 stxt_level_control[SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT] =
  {
    IDS_PLS_INPUT_PIN, IDS_PLS_SELECT_LEVEL,
  };
  static sn_comm_help_data_t level_help_data; //help bar data
  help_rsc help_item[SN_CONDITIONAL_ACCEPT_LEVEL_HELP_RSC_CNT]=
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
  
  P_card_info_t = (cas_card_info_t *)para1;

  // create container
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_WATCH_LEVE,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_level_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_level_cont_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_CA,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_X-20, SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_W, SN_CONDITIONAL_ACCEPT_LEVEL_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_ca_frm, conditional_accept_level_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_level_ca_frm_proc);

  //level control
  y = SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y;
  for (i = 0; i<SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW-50,
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW+90);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
        ui_comm_ctrl_set_keymap(p_ctrl, conditional_accept_level_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_level_pwdedit_proc);
        ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BG);//RSI_SUB_BUTTON_SH
        break;
      case 1:
        p_ctrl = ui_comm_select_create(p_ca_frm, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y+SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP,
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW+85,
                                           SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW-50);
        ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl, STL_LEFT|STL_VCENTER);
        ui_comm_ctrl_set_txt_align_tyle(p_ctrl, STL_LEFT|STL_VCENTER);
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl, RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
	    ui_comm_ctrl_set_txt_rstyle(p_ctrl, RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
        ui_comm_select_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_select_set_param(p_ctrl, TRUE,
                                   CBOX_WORKMODE_STATIC, CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);

        ui_comm_select_set_content(p_ctrl, 0, IDS_CA_LEVEL4);
        ui_comm_select_set_content(p_ctrl, 1, IDS_CA_LEVEL5);
        ui_comm_select_set_content(p_ctrl, 2, IDS_CA_LEVEL6);
        ui_comm_select_set_content(p_ctrl, 3, IDS_CA_LEVEL7);
        ui_comm_select_set_content(p_ctrl, 4, IDS_CA_LEVEL8);
        ui_comm_select_set_content(p_ctrl, 5, IDS_CA_LEVEL9);
        ui_comm_select_set_content(p_ctrl, 6, IDS_CA_LEVEL10);		
        ui_comm_select_set_content(p_ctrl, 7, IDS_CA_LEVEL11);
        ui_comm_select_set_content(p_ctrl, 8, IDS_CA_LEVEL12);
        ui_comm_select_set_content(p_ctrl, 9, IDS_CA_LEVEL13);
        ui_comm_select_set_content(p_ctrl, 10, IDS_CA_LEVEL14);
        ui_comm_select_set_content(p_ctrl, 11, IDS_CA_LEVEL15);
        ui_comm_select_set_content(p_ctrl, 12, IDS_CA_LEVEL16);
        ui_comm_select_set_content(p_ctrl, 13, IDS_CA_LEVEL17);
        ui_comm_select_set_content(p_ctrl, 14, IDS_CA_LEVEL18);		
		#ifndef WIN32
		
        ui_comm_select_set_focus(p_ctrl, P_card_info_t->card_work_level);
		#endif
        ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SUB_BUTTON_SH
        break;
      case 2:
        p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X+100,
                              y+(SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP)*i+102,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H+6,
                              p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_DIG_BG, RSI_SN_BAR_YELLOW_HL, RSI_DIG_BG);//RSI_DLG_BTN_SH, RSI_DLG_BTN_HL, RSI_DLG_BTN_SH
        text_set_font_style(p_ctrl,FSI_DLG_BTN_N, FSI_DLG_BTN_HL, FSI_DLG_BTN_N);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_OK);
        break;
      case 3: 
	  	p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X+SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W+170,
                              y+(SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP)*2-35+100,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W,
                              SN_CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H+6,
                              p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_DIG_BG, RSI_SN_BAR_YELLOW_HL, RSI_DIG_BG);//RSI_DLG_BTN_SH, RSI_DLG_BTN_HL, RSI_DLG_BTN_SH
        text_set_font_style(p_ctrl,FSI_DLG_BTN_N, FSI_DLG_BTN_HL, FSI_DLG_BTN_N);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_CANCEL);
		break;
      default:
        p_ctrl = NULL;
        break;
    }
    if(NULL  == p_ctrl)
    {
        break;
    }
    if(i >= 0 && i < 2){
    	ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */
    }
	else{
		ctrl_set_related_id(p_ctrl,
                        (u8) ((i - 1) % 2 + IDC_CA_LEVEL_CHANGE_OK),                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
                        (u8) ((i + 1) % 2 + IDC_CA_LEVEL_CHANGE_OK),                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */

	}
    y += SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP;
  }

  //change result
  
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X-50,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W-60,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SUB_BUTTON_SH
  text_set_font_style(p_ctrl, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE);//FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
  ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

  p_ctrl2 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT2,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X-50,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W-60,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl2, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SUB_BUTTON_SH
  text_set_font_style(p_ctrl2, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl2, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl2, TEXT_STRTYPE_UNICODE);   
  ctrl_set_attr(p_ctrl2,OBJ_ATTR_HIDDEN);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  memset((void*)&level_help_data, 0, sizeof(sn_comm_help_data_t));
  level_help_data.x=0;
  level_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_LEVEL_HELP_RSC_CNT;
  level_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  level_help_data.offset=37;
  level_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&level_help_data, p_cont);
  conditional_accept_level_set_content(p_cont, (cas_card_info_t *)para1);

  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_LEVEL_CONTROL_PIN), MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_level_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result = NULL,*p_result2 = NULL, *p_frm = NULL;
 // u16 pin_spare_num = 0;
  cas_rating_set_t *ca_pin_info = NULL;
  ca_pin_info = (cas_rating_set_t *)para2;
  p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_CA);
  p_result = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CHANGE_RESULT);
  p_result2 = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CHANGE_RESULT2);

  if(7 == para1)
  {
    ctrl_set_attr(p_result2,OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_result,OBJ_ATTR_HIDDEN);
  }
  else
  {
    ctrl_set_attr(p_result,OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_result2,OBJ_ATTR_HIDDEN);
  }
  if(para1 == CAS_SUCESS)
  {
	level_data_t.text_strID = IDS_SET_SUCCESS;
	ui_comm_prompt_open(&level_data_t);
  }
  else if(para1 == CAS_ERR_INVALID_CARD)
  {
	level_data_t.text_strID = IDS_CA_INVALID_CARD;
  }
  else if(para1 == CAS_ERR_NOCARD)
  {
	level_data_t.text_strID = IDS_CA_NO_CARD;
  }
  else if(para1 == CAS_ERR_INVALID_ECMPID)
  {
	level_data_t.text_strID = IDS_CAS_E_ECM_ERROR;
  }
  else if(para1 == CAS_ERR_INVALID_EMMPID) 
  {
    level_data_t.text_strID = IDS_CAS_E_EMM_ERROR;
  }
  else if(para1 == CAS_ERR_NOENTITLE)
  {
	level_data_t.text_strID = IDS_NO_ENTITLEMENT;
  }
  else if(para1 == CAS_ERR_STB_PAIR_FAILED)
  {
	level_data_t.text_strID = IDS_CA_PAIR_FAILED;
  }
  else if(para1 == CAS_FAILED)
  {
	level_data_t.text_strID = IDS_CA_CAS_FAILED;
  }
  else if(para1 == CAS_ERR_INIT_ERROR)
  {
	level_data_t.text_strID = IDS_CA_INIT_ERR;
  }
  else if(para1 == CAS_ERR_CARD_VER_ERROR)
  {
	level_data_t.text_strID = IDS_CA_CARD_VER_ERR;
  }
  else
  {
	level_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
	ui_comm_prompt_open(&level_data_t);
  }
  
  ctrl_paint_ctrl(p_result,TRUE);
  ctrl_paint_ctrl(p_result2,TRUE);

  return SUCCESS;
}


static RET_CODE on_conditional_accept_level_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
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

static RET_CODE on_conditional_accept_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_pin,*p_result,*p_child, *p_level = NULL;
	u32 card_pin = 0;
	BOOL card_pin_is_full = FALSE;
	dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	u8 ctrl_id;
	p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_PIN);
	p_level = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_LEVEL);
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CHANGE_RESULT);
	card_pin = ui_comm_pwdedit_get_value(p_pin);
	card_pin_is_full = ui_comm_pwdedit_is_full(p_pin);
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
	p_child = ctrl_get_active_ctrl(p_ctrl);
	ctrl_id = ctrl_get_ctrl_id(p_child);
	if(ctrl_id == IDC_CA_LEVEL_CHANGE_CANCEL){
		manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
	}
	else
	{
		ret=ui_comm_dialog_open(&dialog);

		if(DLG_RET_YES==ret)
		{
			rate_info.pin[0]=(u8)(card_pin/100000);
	   		rate_info.pin[1]=(u8)((card_pin/10000)%10);
	    	rate_info.pin[2]=(u8)(((card_pin/1000)%100)%10);
			rate_info.pin[3]=(u8)((((card_pin/100)%1000)%100)%10);
	    	rate_info.pin[4]=(u8)(((((card_pin/10)%10000)%1000)%100)%10);
	    	rate_info.pin[5]=(u8)(((((card_pin%100000)%10000)%1000)%100)%10);

			rate_info.rate = (u8)ui_comm_select_get_focus(p_level) + 4;
			UI_PRINTF("[ca_accept_level]rate_info.rate= %d \n",rate_info.rate);
			if(card_pin_is_full)
			{
#ifndef WIN32
				ui_ca_get_info((u32)CAS_CMD_RATING_SET, (u32)&rate_info, 0);
#else
				level_data_t.text_strID = IDS_SET_SUCCESS;
#endif
			}
			else
			{
				//PIN invalid
				level_data_t.text_strID = IDS_CAS_E_PIN_INVALID;
				ui_comm_prompt_open(&level_data_t);
			}
			ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);

			ctrl_paint_ctrl(p_pin, TRUE);

		}
		else
		{
			manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
		}
	}
	return SUCCESS;
}



BEGIN_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_level_update)
  ON_COMMAND(MSG_CA_RATING_SET, on_conditional_accept_level_set)
END_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_level_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_level_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_level_ca_frm_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_level_ca_frm_ok)
END_MSGPROC(conditional_accept_level_ca_frm_proc, cont_class_proc)

BEGIN_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_level_pwdedit_maxtext)
END_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)



