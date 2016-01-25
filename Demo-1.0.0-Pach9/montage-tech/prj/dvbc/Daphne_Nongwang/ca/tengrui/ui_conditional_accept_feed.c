/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_feed.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_CARD_FEED_FRM1,
  IDC_CA_CARD_FEED_FRM2,
};

enum ca_feed_frm1_info_id
{
  IDC_CA_CARD_PROMPT_1 = 1,
  IDC_CA_CARD_PROMPT_2,
};

enum ca_feed_frm2_info_id
{
  IDC_CA_CARD_FEED_TIP = 1,
  IDC_CA_CARD_FEED_OK,
  IDC_CA_CARD_FEED_CANCEL,
  IDC_CA_CARD_FEED_STATUS,
};

enum ca_feed_status
{
  FEED_STATUS_NULL = 0,
  FEED_STATUS_PROMPT,
  FEED_STATUS_INSERT,
  FEED_STATUS_READY,
  FEED_STATUS_MOTHER_SUCCESS,
  FEED_STATUS_SON_SUCCESS,
  FEED_STATUS_PARAM_ERR,
  FEED_STATUS_FEED_DATA_ERR,
  FEED_STATUS_CARD_NOTSUPPORT,
  FEED_STATUS_PARTNER_FAILED,
  FEED_STATUS_UNKNOW_ERR,
}ca_feed_status_t;

static u8 g_feed_status = FEED_STATUS_NULL;
static chlid_card_status_info *p_child_info = NULL;
static chlid_card_status_info g_feed_info;

//static u32 g_info_addr = 0;

u16 conditional_accept_feed_cont_keymap(u16 key);
RET_CODE conditional_accept_feed_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_feed_btn_keymap(u16 key);
RET_CODE conditional_accept_feed_btn_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static RET_CODE on_conditional_accept_feed_state_update(control_t *p_cont, u16 msg, u32 para1, u32 para2, BOOL is_force)
{
  control_t *p_ctrl_tip = NULL;
  control_t *p_ctrl_frm2 = NULL;
  
  p_ctrl_frm2 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM2);
  p_ctrl_tip = ctrl_get_child_by_id(p_ctrl_frm2, IDC_CA_CARD_FEED_TIP);
  
  switch(g_feed_status)
  {
    case FEED_STATUS_MOTHER_SUCCESS:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_INSERT_CHILD_CARD);
      break;

    case FEED_STATUS_SON_SUCCESS:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_SUCCESS);
      break;

    case FEED_STATUS_PARAM_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_PARAM_ERROR);
      break;

    case FEED_STATUS_FEED_DATA_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_DATA_ERROR);
      break;

    case FEED_STATUS_CARD_NOTSUPPORT:
      text_set_content_by_strid(p_ctrl_tip, IDS_NOT_SUPPORT_FEED);
      break;

    case FEED_STATUS_PARTNER_FAILED:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_PARTNER_FAILED);
      break;

    case FEED_STATUS_UNKNOW_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_UNKNOW_ERROR);
      break;

    default:
      break;
  }

  ctrl_paint_ctrl(p_ctrl_tip, is_force);

  return ERR_NOFEATURE;
}

RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_FEED_HELP_RSC_CNT		13

  control_t *p_cont, *p_ctrl, *p_frm1, *p_frm2;
  customer_cfg_t cus_cfg = {0};
  
  static sn_comm_help_data_t feed_help_data; //help bar data
  help_rsc help_item[SN_CONDITIONAL_ACCEPT_FEED_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 80 , IDS_MOVE},

	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
     {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
     {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
     {SN_IDC_HELP_TEXT, 110 , IDS_SON_CARD_FEED},

	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
     {SN_IDC_HELP_TEXT, 40 , IDS_BACK},
     {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
     {SN_IDC_HELP_TEXT, 80 , IDS_EXIT}
  };

  get_customer_config(&cus_cfg);
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED,
							  0,0,
							  640,480,
							  IDS_SMARTCARD_FEED,TRUE);	

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_feed_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_feed_cont_proc);
  
  //CA FEED FRAME1
  p_frm1 = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM1),
                            70, 80, 500, 300, p_cont, 0);
  ctrl_set_rstyle(p_frm1, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  //ctrl_set_attr(p_frm1, OBJ_ATTR_HIDDEN);
  
  //PROMPT
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_PROMPT_1),
                            60, 60, 100, 40, p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG,RSI_DIG_BG,RSI_DIG_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_PROMPT);

  //SUPPORT
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_PROMPT_2),
                            250, 60, 200, 40, p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG,RSI_DIG_BG,RSI_DIG_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_ctrl, IDS_NOT_SUPPORT_FEED);

  //CA FEED FRAME2
  p_frm2 = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM2),
                            70, 80, 500, 300, p_cont, 0);
  ctrl_set_rstyle(p_frm2, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  ctrl_set_attr(p_frm2, OBJ_ATTR_HIDDEN);
  
  //feed tip
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_TIP),
                            100, 60, 300, 40, p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG,RSI_DIG_BG,RSI_DIG_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_INSERT_PARENT_CARD);

  //OK
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_OK),
                            200, 110, 100, 31, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, conditional_accept_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, conditional_accept_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BAR_YELLOW_HL,RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_OK);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);

  //Cancel
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_CANCEL),
                            200, 160, 100, 31, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, conditional_accept_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, conditional_accept_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BAR_YELLOW_HL,RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CANCEL);
  //ctrl_set_related_id(p_ctrl, 0, IDC_CA_CARD_FEED_OK, 0, IDC_CA_CARD_FEED_OK);

  //status
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_STATUS),
                            100, 230, 300, 40, p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_ctrl, IDS_CANCEL);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
  
  ctrl_default_proc(ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK), MSG_GETFOCUS, 0, 0);
  //ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
	memset((void*)&feed_help_data, 0, sizeof(sn_comm_help_data_t));
	feed_help_data.x=0;
	feed_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_FEED_HELP_RSC_CNT;
	feed_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
	feed_help_data.offset=37;
	feed_help_data.rsc =help_item;
	sn_ui_comm_help_create(&feed_help_data, p_cont);

#ifndef WIN32
  ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
#endif
  g_feed_status = FEED_STATUS_NULL;

  return SUCCESS;
}

static RET_CODE on_conditional_accept_feed_btn_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 next_id = 0;
  control_t *p_next_ctrl = NULL;
  if(g_feed_status !=FEED_STATUS_NULL && g_feed_status != FEED_STATUS_PROMPT)
  {
    switch(p_ctrl->id)
    {
      case IDC_CA_CARD_FEED_OK:
        if(msg == MSG_FOCUS_DOWN)
        {
          next_id = IDC_CA_CARD_FEED_CANCEL;
        }
        else if(msg == MSG_FOCUS_UP)
        {
          next_id = IDC_CA_CARD_FEED_CANCEL;
        }
        break;

      case IDC_CA_CARD_FEED_CANCEL:
        if(msg == MSG_FOCUS_DOWN)
        {
          next_id = IDC_CA_CARD_FEED_OK;
        }
        else if(msg == MSG_FOCUS_UP)
        {
          next_id = IDC_CA_CARD_FEED_OK;
        }
        break;
    }

    p_next_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, next_id);

    ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    
    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }
  return ERR_NOFEATURE;
}

static RET_CODE on_conditional_accept_feed_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm1, *p_frm2, *p_button;
  p_frm2 = p_ctrl->p_parent;
  p_frm1 = ctrl_get_child_by_id(p_frm2->p_parent, IDC_CA_CARD_FEED_FRM1);
  p_button = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK);
    
  if(g_feed_status == FEED_STATUS_PROMPT)
  {
    ctrl_set_attr(p_frm1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_frm2, OBJ_ATTR_ACTIVE);
    ctrl_default_proc(p_button, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_frm1->p_parent, TRUE);
    g_feed_status = FEED_STATUS_INSERT;
  }
  else
  {
    switch(p_ctrl->id)
    {
      case IDC_CA_CARD_FEED_OK:
        if(g_feed_status == FEED_STATUS_INSERT)
        {
         g_feed_status = FEED_STATUS_READY;
         ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
        }
        else if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
        {
          //ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, g_info_addr, 0);
          ui_ca_get_info(CAS_CMD_MON_CHILD_FEED, (u32)&g_feed_info, 0);
        }
        else if(g_feed_status == FEED_STATUS_SON_SUCCESS)
        {
          manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
        }
        break;

      case IDC_CA_CARD_FEED_CANCEL:
        g_feed_status = FEED_STATUS_NULL;
        ctrl_set_attr(p_frm2, OBJ_ATTR_HIDDEN);
        ctrl_set_attr(p_frm1, OBJ_ATTR_ACTIVE);
        ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
        ctrl_default_proc(p_button, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_frm2->p_parent, TRUE);
        ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
        break;
        
      default:
        break;
    }
  }
  return ERR_NOFEATURE;
}

extern BOOL ui_is_smart_card_insert(void);

static RET_CODE on_conditional_accept_cont_ca_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm1, *p_frm2, *p_support, *p_button, *p_tip, *p_status;
  RET_CODE ret = (u32)para1;
  p_child_info = (chlid_card_status_info *)para2;
  
  p_frm1 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM1);
  p_frm2 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM2);
  p_support = ctrl_get_child_by_id(p_frm1, IDC_CA_CARD_PROMPT_2);
  p_button = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK);
  p_tip = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_TIP);
  p_status  = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_STATUS);
  if(g_feed_status == FEED_STATUS_NULL)
  {
    p_child_info = (chlid_card_status_info *)para2;
    g_feed_status = FEED_STATUS_PROMPT;
    if(ret == SUCCESS)
    {
      text_set_content_by_strid(p_support, IDS_CURRENT_IS_MOTHER_CARD);
    }
    else if(ret == CAS_ERROR_CARD_NOTSUPPORT)
    {
       text_set_content_by_strid(p_support, IDS_CURRENT_IS_SON_CARD);
    }
    else
    {
      if(ui_is_smart_card_insert() == FALSE)
        text_set_content_by_strid(p_support, IDS_CA_FEED_NULL);
      else
        text_set_content_by_strid(p_support, IDS_NOT_SUPPORT_FEED);
    }
    ctrl_paint_ctrl(p_cont, TRUE);
    return ERR_NOFEATURE;
  }

  switch(ret)
  {
    case SUCCESS:
      if(g_feed_status == FEED_STATUS_READY)
      {
        //g_info_addr = para2;
        memcpy(&g_feed_info, (void*)para2, sizeof(chlid_card_status_info));
        g_feed_status = FEED_STATUS_MOTHER_SUCCESS;  
      }
      else if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
      {
        g_feed_status = FEED_STATUS_SON_SUCCESS;          
      }
      break;

    case CAS_E_PARAM_ERR:
      g_feed_status = FEED_STATUS_PARAM_ERR; 
      break;
      
    case CAS_E_FEED_DATA_ERROR:
      g_feed_status = FEED_STATUS_FEED_DATA_ERR; 
      break;
      
    case CAS_ERROR_CARD_NOTSUPPORT:
      g_feed_status = FEED_STATUS_CARD_NOTSUPPORT; 
      break;
      
    case CAS_E_CARD_PARTNER_FAILED:
      g_feed_status = FEED_STATUS_PARTNER_FAILED; 
      break;
      
    case CAS_E_UNKNOW_ERR:
      g_feed_status = FEED_STATUS_UNKNOW_ERR; 
      break;
      
    default:
      break;
  }

  on_conditional_accept_feed_state_update(p_cont, 0, 0, 0, TRUE);

  return ERR_NOFEATURE;
}


BEGIN_KEYMAP(conditional_accept_feed_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_feed_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_feed_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)  
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_conditional_accept_cont_ca_msg)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_conditional_accept_cont_ca_msg)
END_MSGPROC(conditional_accept_feed_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_feed_btn_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_feed_btn_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_feed_btn_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_feed_btn_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_feed_btn_change)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_feed_btn_select) 
END_MSGPROC(conditional_accept_feed_btn_proc, text_class_proc)

