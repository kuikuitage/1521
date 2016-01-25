/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pin.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_PIN,
};

enum ca_pin_modify_id
{
  IDC_CA_PIN_MODIFY_PSW_OLD = 1,
  IDC_CA_PIN_MODIFY_PSW_NEW,
  IDC_CA_PIN_MODIFY_PSW_CONFIRM,
  IDC_CA_PIN_MODIFY_PSW_OK,
  IDC_CA_PIN_CHANGE_RESULT,
  IDC_CA_PIN_CHANGE_RESULT2,
};

comm_prompt_data_t pin_data_t =
   {
	 ROOT_ID_CONDITIONAL_ACCEPT_PIN,
	 STR_MODE_STATIC,
	 200,165,250,150,20,
	 RSC_INVALID_ID,RSC_INVALID_ID,
	 RSC_INVALID_ID,DIALOG_DEADLY,
	 3000
   };

static cas_pin_modify_t pin_modify;
u16 conditional_accept_pin_cont_keymap(u16 key);
RET_CODE conditional_accept_pin_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_pin_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_pin_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



RET_CODE open_conditional_accept_pin(u32 para1, u32 para2)
{
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT      3
  #define SN_CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT	  15
  //CA frm
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_X          106
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_Y          24
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_W          505
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_H          360

  //pin modify
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X         10
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y         20
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW        260
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW        150
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H         35
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP     10

  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_X          170
  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_Y          160
  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_W          70
  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_H          25

  //change pin result
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_X        50
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_Y        200
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_W        260
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_H        90



  control_t *p_cont, *p_ctrl,*p_ctrl2;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_pin[SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT] =
  { 
    IDS_INPUT_OLD_PIN2, IDS_INPUT_NEW_PIN2, IDS_CONFIRM_NEW_PIN,
  };
  static sn_comm_help_data_t PIN_help_data; //help bar data
  help_rsc help_item[SN_CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT]=
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

  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_PIN,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_CA_CHANGE_PIN_CODE,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_pin_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_pin_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_PIN,
                              SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_X-20, SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_W, SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_ca_frm, conditional_accept_pin_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_pin_ca_frm_proc);

  //pin modify
  y = SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y;
  for (i = 0; i<SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X, y+(SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP+20)*i,
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW-50,
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW+100);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_pin[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
        ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_pin_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_pin_pwdedit_proc);

        ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);
        break;
      default:  
        break;
      }
      ctrl_set_related_id(p_ctrl,
                          0,                                     /* left */
                          (u8)((i - 1 +
                                SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT) %
                               SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
                          0,                                     /* right */
                          (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */

  }

  //change result
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_X, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_Y,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_W, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

  p_ctrl2 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT2,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_X, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_Y,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_W, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl2, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl2, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl2, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl2, TEXT_STRTYPE_UNICODE);
  ctrl_set_attr(p_ctrl2,OBJ_ATTR_HIDDEN);
  
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  memset((void*)&PIN_help_data, 0, sizeof(sn_comm_help_data_t));
  PIN_help_data.x=0;
  PIN_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT;
  PIN_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  PIN_help_data.offset=37;
  PIN_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&PIN_help_data, p_cont);

  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_PIN_MODIFY_PSW_OLD), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
  ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_cont),TRUE);
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 enable_psw_pinold, enable_psw_pinnew ,enable_psw_pinnew_confirm;
  //u32 psw_len_confirm_all;
  control_t *p_result, *p_ctrl_temp;
  u32 pin_new_psw,pin_old_psw;
  u32 pin_new_psw_confirm;
  u8 i = 0;
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
  //change result
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_OLD);
  pin_old_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinold = ui_comm_is_pwdedit_enable(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_NEW);
  pin_new_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinnew = ui_comm_is_pwdedit_enable(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_CONFIRM);
  pin_new_psw_confirm = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinnew_confirm = ui_comm_is_pwdedit_enable(p_ctrl_temp);
  if(DLG_RET_YES==ret)
  {

  if(PWD_LENTH_OK != enable_psw_pinold || PWD_LENTH_OK != enable_psw_pinnew || PWD_LENTH_OK != enable_psw_pinnew_confirm)
  {
    pin_data_t.text_strID = IDS_PASSWORD_SIX_DIGIT;
	ui_comm_prompt_open(&pin_data_t);
  }
  else if(pin_new_psw == pin_new_psw_confirm)
  {
    pin_modify.new_pin[0]=(u8)((pin_new_psw & 0x00ff0000)>>16);
    pin_modify.new_pin[1]=(u8)((pin_new_psw & 0x0000ff00)>>8);
    pin_modify.new_pin[2]=(u8)(pin_new_psw & 0x000000ff);

    pin_modify.old_pin[0]=(u8)((pin_old_psw & 0x00ff0000)>>16);
    pin_modify.old_pin[1]=(u8)((pin_old_psw & 0x0000ff00)>>8);
    pin_modify.old_pin[2]=(u8)(pin_old_psw & 0x000000ff);
	#ifndef WIN32
    ui_ca_get_info((u32)CAS_CMD_PIN_SET, (u32)&pin_modify ,0);
	#else
	pin_data_t.text_strID = IDS_SET_SUCCESS;
	#endif
  }
  else
  {
    pin_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
	ui_comm_prompt_open(&pin_data_t);
  }
  for(i=0; i<3; i++)
  {
    p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD+i));
    ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_ctrl_temp), MSG_EMPTY, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_ctrl_temp), TRUE);
  }
  
 }
  else
  {
    manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result,*p_result2;
  RET_CODE ret;
  u16 pin_spare_num = 0;
  cas_pin_modify_t *ca_pin_info = NULL;
  ca_pin_info = (cas_pin_modify_t *)para2;
  ret = (RET_CODE)para1;
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  p_result2 = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT2);

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
  
  if(para1 == 2)
  {
	pin_data_t.text_strID = IDS_SET_SUCCESS;
	
  }
  else if(para1 == 18)
  {
	pin_data_t.text_strID = IDS_CA_PARAME_SET_ERROR;
  }
  else if(para1 == 1)
  {
	pin_data_t.text_strID = IDS_CAS_E_IC_CMD_FAIL;
  }
  else if(para1 == 9)
  {
	pin_data_t.text_strID = IDS_CA_PASSWORD_PIN_NUMB;
  }
  else if(para1 == 7) 
  {
    pin_spare_num = (u16)ca_pin_info->pin_spare_num;
    switch(pin_spare_num){
		case 1:
			pin_data_t.text_strID = IDS_CA_PIN_ERROR_ONE_CHANCES_LEFT;
			break;
		case 2:
			pin_data_t.text_strID = IDS_CA_PIN_ERROR_TWO_CHANCES_LEFT;
			break;
    }
  }
  else
  {
	pin_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
  }
  ui_comm_prompt_open(&pin_data_t);
  ctrl_paint_ctrl(p_result, TRUE);
  ctrl_paint_ctrl(p_result2,TRUE);
  
  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

END_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_pin_ca_frm_ok)  
  ON_COMMAND(MSG_CA_PIN_SET_INFO, on_conditional_accept_pin_ca_modify)
END_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_pin_pwdedit_maxtext)
END_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)



