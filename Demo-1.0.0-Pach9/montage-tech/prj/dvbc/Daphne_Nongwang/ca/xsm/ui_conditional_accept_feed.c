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

//static u32 g_info_addr = 0;
/*
static comm_help_data_t feed_help_data = //help bar data
{
  2,
  2,
  {
    IDS_BACK,
    IDS_EXIT,
  },
  {
    IM_SN_ICON_BRACKET_LEFT,
    IM_SN_ICON_BRACKET_LEFT,
  },
};
*/
u16 conditional_accept_feed_cont_keymap(u16 key);
RET_CODE conditional_accept_feed_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_feed_btn_keymap(u16 key);
RET_CODE conditional_accept_feed_btn_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

extern void g_ca_set_feed_status(u8 *status);
  
#if 0
static RET_CODE on_conditional_accept_feed_state_update(control_t *p_cont, u16 msg, u32 para1, u32 para2, BOOL is_force)
{
  control_t *p_ctrl_tip = NULL;
  control_t *p_ctrl_frm2 = NULL;

  p_ctrl_frm2 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM2);
  p_ctrl_tip = ctrl_get_child_by_id(p_ctrl_frm2, IDC_CA_CARD_FEED_TIP);
  if(g_feed_status == FEED_STATUS_READY)
  {
    if(p_child_info->is_child == 0)
    {
      text_set_content_by_strid(p_ctrl_tip, IDS_E04);
      g_feed_status = FEED_STATUS_MOTHER_SUCCESS;
    }
    if(p_child_info->is_child == 1)

    {
      text_set_content_by_strid(p_ctrl_tip, IDS_CAS_E_IC_CMD_FAIL);
    }
  }

  if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
  {
    if(p_child_info->is_can_feed == TRUE)
    {

      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_SUCCESS);
      g_feed_status = FEED_STATUS_SON_SUCCESS;
    }
    else
    {

     text_set_content_by_strid(p_ctrl_tip, IDS_SAVE_DATA_FAILED);
    }
  }


  ctrl_paint_ctrl(p_ctrl_tip, TRUE);

  return ERR_NOFEATURE;
}
#endif
RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{
#define RSI_SUBMENU_BG	(RSI_SN_BG)
#define RSI_TITLE_BG	(RSI_SN_BG)
#define RSI_TIP_BOX	(RSI_SN_BG)
#define RSI_DLG_BTN_SH	(RSI_SN_BG)
#define RSI_DLG_BTN_HL	(RSI_SN_BG)

  control_t *p_cont, *p_ctrl, *p_frm2;

  g_ca_set_feed_status(&g_feed_status);
  p_cont = ui_comm_root_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED,
                               RSI_SUBMENU_BG,
                               CONDITIONAL_ACCEPT_FEED_CONT_X, CONDITIONAL_ACCEPT_FEED_CONT_Y,
                               CONDITIONAL_ACCEPT_FEED_CONT_W, CONDITIONAL_ACCEPT_FEED_CONT_H,
                               IDS_FEED, RSI_TITLE_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_feed_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_feed_cont_proc);

  //CA FEED FRAME2
  p_frm2 = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM2),
                            100, 130, 500, 300, p_cont, 0);
  ctrl_set_rstyle(p_frm2, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  //ctrl_set_attr(p_frm2, OBJ_ATTR_HIDDEN);

  //feed tip
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_TIP),
                            100, 60, 300, 40, p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  //text_set_font_style(p_ctrl, FSI_GREEN, FSI_GREEN, FSI_GREEN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_INSERT_PARENT_CARD);

  //OK
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_OK),
                            200, 110, 100, 35, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, conditional_accept_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, conditional_accept_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_DLG_BTN_SH, RSI_DLG_BTN_HL, RSI_DLG_BTN_SH);
  //text_set_font_style(p_ctrl, FSI_GREEN, FSI_GREEN, FSI_GREEN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_OK);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);

  //Cancel
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_CANCEL),
                            200, 160, 100, 35, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, conditional_accept_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, conditional_accept_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_DLG_BTN_SH, RSI_DLG_BTN_HL, RSI_DLG_BTN_SH);
  //text_set_font_style(p_ctrl, FSI_GREEN, FSI_GREEN, FSI_GREEN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CANCEL);
  ctrl_set_related_id(p_ctrl, 0, IDC_CA_CARD_FEED_OK, 0, IDC_CA_CARD_FEED_CANCEL);

  //status
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_STATUS),
                            100, 230, 300, 40, p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  //text_set_font_style(p_ctrl, FSI_GREEN, FSI_GREEN, FSI_GREEN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);

//  ui_comm_help_create(&feed_help_data, p_cont);

  ctrl_default_proc(ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  g_feed_status = FEED_STATUS_INSERT;
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_feed_btn_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 next_id = 0;
  control_t *p_next_ctrl = NULL;
  if(1)
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
  control_t  *p_frm2, *p_button;
  p_frm2 = p_ctrl->p_parent;
  p_button = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK);
  
  switch(p_ctrl->id)
  {
    case IDC_CA_CARD_FEED_OK:
      if(g_feed_status == FEED_STATUS_INSERT)
      {
       g_feed_status = FEED_STATUS_READY;
       ui_ca_get_info(CAS_CMD_READ_FEED_DATA, 0 ,0);
       //ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
      }
      else if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
      {
        ui_ca_get_info(CAS_CMD_MON_CHILD_FEED, (u32)p_child_info, 0);
        //ui_ca_get_info(CAS_CMD_MON_CHILD_FEED, (u32)p_child_info, 0);
        g_feed_status = FEED_STATUS_SON_SUCCESS;
      }
      else if(g_feed_status == FEED_STATUS_PROMPT)
      {
        manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
      }
      break;

    case IDC_CA_CARD_FEED_CANCEL:
      g_feed_status = FEED_STATUS_INSERT;
      manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
      break;

    default:
      break;
  }
return ERR_NOFEATURE;
}



static RET_CODE on_conditional_accept_cont_ca_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl_tip = NULL;
  control_t *p_ctrl_frm2 = NULL;
  RET_CODE ret = para1;
  OS_PRINTF("is or not child card: %d  (0:monther card  1:son card 2:normal card)\n",p_child_info->is_child);
  OS_PRINTF("operator_id: %d  \n",p_child_info->operator_id);
  OS_PRINTF("parent_card_sn: %s  \n",p_child_info->parent_card_sn);
  OS_PRINTF("parent_card_stb_sn: %s  \n",p_child_info->parent_card_stb_sn);
  OS_PRINTF("is_can_feed: %d  \n",p_child_info->is_can_feed);

  p_ctrl_frm2 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM2);
  p_ctrl_tip = ctrl_get_child_by_id(p_ctrl_frm2, IDC_CA_CARD_FEED_TIP);

  if(ret != CAS_EVT_NONE && g_feed_status == FEED_STATUS_READY)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_MAINMENU);
    p_child_info = NULL;
  }
  else if(ret == CAS_EVT_NONE && g_feed_status == FEED_STATUS_READY)
  {
    p_child_info = (chlid_card_status_info *)para2;
    text_set_content_by_strid(p_ctrl_tip,IDS_MAINMENU);
    g_feed_status = FEED_STATUS_MOTHER_SUCCESS;
  }
  else if(ret != SUCCESS && g_feed_status == FEED_STATUS_SON_SUCCESS)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_MAINMENU);
  }
  else if(ret == SUCCESS && g_feed_status == FEED_STATUS_SON_SUCCESS)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_MAINMENU);
    g_feed_status = FEED_STATUS_PROMPT;
  }


  ctrl_paint_ctrl(p_ctrl_tip, TRUE);

  return ERR_NOFEATURE;

  //on_conditional_accept_feed_state_update(p_cont, 0, 0, 0, TRUE);

  return ERR_NOFEATURE;
}

static RET_CODE on_conditional_accept_cont_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{

  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
  return SUCCESS;

}

BEGIN_KEYMAP(conditional_accept_feed_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(conditional_accept_feed_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_feed_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_READ_FEED_DATA, on_conditional_accept_cont_ca_msg)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_conditional_accept_cont_ca_msg)
  ON_COMMAND(MSG_EXIT, on_conditional_accept_cont_exit)
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


