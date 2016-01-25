/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_level.h"

enum control_id
{
  IDC_INVALID = 0, 
  IDC_FRM_CA,
};

enum ca_level_control_id
{
  IDC_CA_LEVEL_CONTROL_LEVEL = 1,
  IDC_CA_LEVEL_CONTROL_PIN,
  IDC_CA_LEVEL_CHANGE_RESULT,
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

u16 conditional_accept_level_cont_keymap(u16 key);
RET_CODE conditional_accept_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);


u16 conditional_accept_level_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_level_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE conditional_accept_level_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_level_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

extern u32 ui_get_smart_card_rate(void);

RET_CODE open_conditional_accept_level(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT 4
#define SN_CONDITIONAL_ACCEPT_LEVEL_HELP_RSC_CNT	15


  control_t *p_cont=NULL, *p_ctrl=NULL;
  u8 i;
  u16 y;
  u16 stxt_level_control[CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT] =
  { 
    IDS_INPUT_RATE, IDS_INPUT_PASSWORD,
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
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,
							 0,0, 
							 640,480,
							 IDS_RATE_CONTROL,TRUE);  
  
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  } 
  ctrl_set_keymap(p_cont, conditional_accept_level_ca_frm_keymap);
  ctrl_set_proc(p_cont, conditional_accept_level_ca_frm_proc);

  //level control
  y = CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 1:
        p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_LEVEL + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_level_pwdedit_proc);
        ui_comm_ctrl_set_keymap(p_ctrl, conditional_accept_level_pwdedit_keymap);
		
		ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);
        break;

      case 0:
        p_ctrl = ui_comm_numedit_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_LEVEL + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_numedit_set_param(p_ctrl, NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 99, 2, 0);
        ui_comm_numedit_set_num(p_ctrl, ui_get_smart_card_rate());
		ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);

		break;

      default:
        break;
    }

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_LEVEL),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_LEVEL));/* down */

    y += CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP;
  }

  //change result
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X, 
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W, 
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_ctrl, IDS_SET_SUCCESS);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);

  /* set focus according to current info */
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
#ifndef WIN32
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
#endif

 
  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_get_info(CAS_CMD_RATING_GET, 0, 0);

  return SUCCESS;
}

#if 0
static RET_CODE on_ca_frm_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_ca_frm_btn_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cont_class_proc(p_ctrl, msg, para1, para2);
  
  //ca_show_frm_by_active_btn(p_ctrl->p_parent, FALSE);
  
  return SUCCESS;
}
#endif

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

#if 0
static RET_CODE on_conditional_accept_level_timedit_unselect(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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
#endif

#if 0
static RET_CODE on_conditional_accept_level_ca_frm_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  switch(msg)
  {
    case MSG_EXIT:
      manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, para1, para2);
      break;
      
    case MSG_EXIT_ALL:
      ui_close_all_mennus();
      break;
  }
  
  return SUCCESS;
}
#endif

static RET_CODE on_conditional_accept_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result, *p_ctrl_temp;
  u32 level = 0;
  u32 pin_code = 0;
  BOOL pin_code_is_full;
  static cas_rating_set_t cas_rating_t;
  s8 i =0, pin = 0;
  //change result
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CHANGE_RESULT);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_LEVEL);
  level = ui_comm_numedit_get_num(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_PIN);
  pin_code = ui_comm_pwdedit_get_value(p_ctrl_temp);
  pin_code_is_full = ui_comm_pwdedit_is_full(p_ctrl_temp);
  for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
  {
    pin = pin_code%10;
    cas_rating_t.pin[i] = pin;
    pin_code = pin_code/10;
  }
  if(pin_code_is_full == FALSE)
  {
	
	level_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
	ui_comm_prompt_open(&level_data_t);
    ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
    ui_comm_pwdedit_empty_value(p_ctrl_temp);
    ctrl_paint_ctrl(p_result, TRUE);
  }
  else if(level >= 11)
  {
	
	level_data_t.text_strID = IDS_PARAMETER_ERROR;
	ui_comm_prompt_open(&level_data_t);
    ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
    ui_comm_pwdedit_empty_value(p_ctrl_temp);
    ctrl_paint_ctrl(p_result, TRUE);
  }
  else
  {
    cas_rating_t.rate = (u8)level;	
   #ifndef WIN32
	ui_ca_get_info((u32)CAS_CMD_RATING_SET, (u32)&cas_rating_t, 0);
   #else
	level_data_t.text_strID = IDS_SET_SUCCESS;
   #endif
    ui_ca_get_info(CAS_CMD_RATING_SET, (u32)&cas_rating_t ,0);
  }
  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_ca_frm_updown(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_active;

  p_active = ctrl_get_active_ctrl(p_ctrl);

  switch(p_active->id)
  {
    case IDC_CA_LEVEL_CONTROL_LEVEL:
      //ui_comm_help_create2((comm_help_data_t2*)&ca_level_help_data[1], p_ctrl, TRUE);
      break;

    case IDC_CA_LEVEL_CONTROL_PIN:
      // ui_comm_help_create2((comm_help_data_t2*)&ca_level_help_data[0], p_ctrl, TRUE);
      break;
  }

  return ERR_NOFEATURE;
}

extern void ui_set_smart_card_rate(u32 level);

static RET_CODE on_conditional_accept_level_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result, *p_level, *p_pin;
  RET_CODE ret;
  u32 level;

  ret = (RET_CODE)para1;
  p_level = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_LEVEL);
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CHANGE_RESULT);
  p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_PIN);
  if(ret == SUCCESS)
  {
    level = ui_comm_numedit_get_num(p_level);
    ui_set_smart_card_rate(level);
	level_data_t.text_strID = IDS_SET_SUCCESS;
	ui_comm_prompt_open(&level_data_t);
  }
  else
  {
	
	level_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
	ui_comm_prompt_open(&level_data_t);
    ui_comm_pwdedit_empty_value(p_pin);
  }
  ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
  ctrl_paint_ctrl(p_result, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_level;

  p_level = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL);

  if (para1 != SUCCESS)
  {
    return SUCCESS;
  }
  
  ui_set_smart_card_rate(para2);
  
  ui_comm_numedit_set_num(p_level, para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;
  
  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_get_info(CAS_CMD_RATING_GET, 0, 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE)
      {
        on_conditional_accept_level_update(p_cont, 0, (u32)SUCCESS, 0);
      }
      break;
  }

  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_level_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_level_cont_focus_change)  
END_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_level_ca_frm_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_MENU, MSG_EXIT)
  //ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_level_ca_frm_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_level_ca_frm_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_EXIT, on_conditional_accept_level_ca_frm_exit)
  //ON_COMMAND(MSG_EXIT_ALL, on_conditional_accept_level_ca_frm_exit)  
  ON_COMMAND(MSG_SELECT, on_conditional_accept_level_ca_frm_ok)
  ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_level_ca_frm_updown)  
  ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_level_ca_frm_updown)
  ON_COMMAND(MSG_CA_RATING_SET, on_conditional_accept_level_ca_modify)
  ON_COMMAND(MSG_CA_EVT_LEVEL, on_conditional_accept_level_ca_modify)
  ON_COMMAND(MSG_CA_RATING_INFO, on_conditional_accept_level_update)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_conditional_accept_level_accept_notify)
  ON_COMMAND(MSG_CA_INIT_OK, on_conditional_accept_level_accept_notify)
END_MSGPROC(conditional_accept_level_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_level_pwdedit_maxtext)
END_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)

