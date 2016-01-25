/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_feed_passive.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_CARD_FEED_FRM2,
  IDC_CA_CARD_FEED_CONTENT,
};

enum ca_feed_frm1_info_id
{
  IDC_CA_CARD_PROMPT_1 = 1,
  IDC_CA_CARD_PROMPT_2,
};

enum ca_feed_frm2_info_id
{
  IDC_CA_CARD_FEED_TIP = 2,
  IDC_CA_CARD_FEED_WARN,
};

enum ca_feed_status
{
  FEED_STATUS_NULL = 0,
  FEED_STATUS_INSERT_CHILD,
  FEED_STATUS_SUCCESS,
  FEED_STATUS_INVALID,

}ca_feed_status_t;

enum local_msg
{
  MSG_CA_CLOSE_TIMER = MSG_LOCAL_BEGIN + 1160,
  MSG_CA_PARWAIT_TIMER,
};

static u8 g_feed_status = FEED_STATUS_NULL;
static chlid_card_status_info *p_child_info = NULL;

//static u32 g_info_addr = 0;

RET_CODE conditional_accept_feed_passive_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);


extern void g_ca_set_feed_status(u8 *status);
#if 0
static RET_CODE on_conditional_accept_feed_passive_state_update(control_t *p_cont, u16 msg, u32 para1, u32 para2, BOOL is_force)
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
RET_CODE open_conditional_accept_feed_passive(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl, *p_frm2;

  //g_ca_set_feed_status(&g_feed_status);
  
  p_cont = fw_create_mainwin(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE,
                               CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_X, CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_Y,
                               CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W, CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_H,
                               0, 0, OBJ_ATTR_ACTIVE, 0);
  //QQ  ctrl_set_rstyle(p_cont, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_cont, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, conditional_accept_feed_passive_cont_proc);

  //CA FEED FRAME2
  p_frm2 = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM2),
                            CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_X, CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_Y, 
                            CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_W, CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_H, 
                            p_cont, 0);
  //QQ  ctrl_set_rstyle(p_frm2, RSI_DLG_TITLE, RSI_DLG_TITLE, RSI_DLG_TITLE);
   ctrl_set_rstyle(p_frm2, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  //ctrl_set_attr(p_frm2, OBJ_ATTR_HIDDEN);

  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_CONTENT),
                            0, 0, 
                            CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_W, CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_H, 
                            p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_by_strid(p_ctrl, IDS_CARD_PAIR);
  //feed tip
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_TIP),
                            CONDITIONAL_ACCEPT_FEED_TIP_X, CONDITIONAL_ACCEPT_FEED_TIP_Y,
                            CONDITIONAL_ACCEPT_FEED_TIP_W, CONDITIONAL_ACCEPT_FEED_TIP_H,
                            p_cont, 0);
  //QQ   ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_ctrl, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  //text_set_font_style(p_ctrl, FSI_GREEN, FSI_GREEN, FSI_GREEN);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_ADDRES_INFO);//QQ  IDS_CA_MATCH_CARD_BEGIN

  
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_WARN),
                            0, (CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_H - HELP_ITEM_H),
                            CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W, HELP_ITEM_H,
                            p_cont, 0);
 //QQ  ctrl_set_rstyle(p_ctrl, RSI_HELP, RSI_HELP, RSI_HELP);
  ctrl_set_rstyle(p_ctrl, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CARD_PAIR);//QQ  IDS_MATCH_WARN

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  g_feed_status = FEED_STATUS_NULL;

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  return SUCCESS;
}


static RET_CODE on_conditional_accept_cont_ca_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl_tip = NULL;
  RET_CODE ret = para1;

  OS_PRINTF("~~~~~~~~~~~~~~~ACCEPT ca msg and data !!!!!!!!!!!!!!!!!!!statue : %d\n",g_feed_status);
  p_ctrl_tip = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_TIP);
  if(ret != CAS_EVT_NONE && g_feed_status == FEED_STATUS_NULL)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_CARD_PAIR);//QQ  IDS_READ_MOTHER_CARD_FAILURE
    ctrl_paint_ctrl(p_ctrl_tip, TRUE);
    p_child_info = NULL;
  }
  else if(ret == CAS_EVT_NONE && g_feed_status == FEED_STATUS_NULL)
  {
    p_child_info = (chlid_card_status_info *)para2;
    text_set_content_by_strid(p_ctrl_tip,IDS_CARD_PAIR);//QQ IDS_CA_INSERT_CHILD_CARD
    ctrl_paint_ctrl(p_ctrl_tip, TRUE);
    g_feed_status = FEED_STATUS_INSERT_CHILD;
  }
  else if(ret != SUCCESS && g_feed_status == FEED_STATUS_INSERT_CHILD)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_CARD_PAIR);//QQ  IDS_MATCH_FAIL_INCHILD
    ctrl_paint_ctrl(p_ctrl_tip, TRUE);
  }
  else if(ret == SUCCESS && g_feed_status == FEED_STATUS_INSERT_CHILD)
  {
    text_set_content_by_strid(p_ctrl_tip,IDS_CARD_PAIR);//IDS_MATCH_SUC_OTHER_CHID_CARD
    ctrl_paint_ctrl(p_ctrl_tip, TRUE);
    g_feed_status = FEED_STATUS_INSERT_CHILD;
    g_ca_set_feed_status(&g_feed_status);
  }
  

  return SUCCESS;
}

static RET_CODE on_plug_in_card(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  ret = fw_tmr_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE, MSG_CA_PARWAIT_TIMER, 500, FALSE);
  if(ret != SUCCESS)
  {
    OS_PRINTF("card paring has issue:timer creat failed\n");
  }
  else
    OS_PRINTF("~~~~~~~~~~~~~~~~~~~card paring has issue:timer creat successed\n");
  return SUCCESS;
}

static RET_CODE on_card_insert_handle(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{ 
  fw_tmr_destroy(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE, MSG_CA_PARWAIT_TIMER);
  OS_PRINTF("~~~~~~~~~~~~~~~~~~~~~on_card_insert_handle! g_feed_status : %d\n",g_feed_status);
  if(g_feed_status == FEED_STATUS_NULL)
  {
    OS_PRINTF("insert card for    juge mother card     card paired! \n");
    ui_ca_get_info(CAS_CMD_READ_FEED_DATA, 0, 0);
  }
  else
  {
    ui_ca_get_info(CAS_CMD_MON_CHILD_FEED,(u32)p_child_info,0);
  }
  return SUCCESS;
}

static RET_CODE on_judge_card_is_mon_child(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  s32 ret = 0;
  s32 timer_ret = SUCCESS;
  control_t *p_child = NULL;
  ret = (s32)para1;
  p_child = ctrl_get_child_by_id(p_ctrl , IDC_CA_CARD_FEED_TIP);
  if(ret != SUCCESS)
  { 
    OS_PRINTF("~~~~~~~~~~~~~~~~~~this is not monther card   ui\n ");
    text_set_content_by_strid(p_child,IDS_CARD_PAIR);//QQ IDS_READ_MOTHER_CARD_FAILURE
    ctrl_paint_ctrl(p_child, TRUE);
    return SUCCESS;
  }
  else
  {
    OS_PRINTF("~~~~~~~~~~~~~~~~~~this is monther card   ui\n ");
    text_set_content_by_strid(p_child,IDS_CARD_PAIR);//QQ  IDS_CA_MOTHER_GET_DATA
    ctrl_paint_ctrl(p_child, TRUE);
    timer_ret = fw_tmr_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE, MSG_CA_CLOSE_TIMER, 2000, FALSE);//wait 2s
    MT_ASSERT(timer_ret == SUCCESS);
    return SUCCESS;
  }
}


static RET_CODE begin_mon_child_match(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  fw_tmr_destroy(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE, MSG_CA_CLOSE_TIMER);
  if(g_feed_status == FEED_STATUS_NULL)
  {
    ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE close_menu_match_card(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
   fw_tmr_destroy(ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE, MSG_CA_CLOSE_TIMER);
   g_feed_status = FEED_STATUS_INVALID;
   return ERR_NOFEATURE;
}

BEGIN_MSGPROC(conditional_accept_feed_passive_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_READ_FEED_DATA, on_judge_card_is_mon_child)
  ON_COMMAND(MSG_CA_INIT_OK, on_plug_in_card)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_conditional_accept_cont_ca_msg)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_conditional_accept_cont_ca_msg)
  ON_COMMAND(MSG_EXIT, close_menu_match_card)
  ON_COMMAND(MSG_CA_PARWAIT_TIMER, on_card_insert_handle)
  ON_COMMAND(MSG_CA_CLOSE_TIMER, begin_mon_child_match)
END_MSGPROC(conditional_accept_feed_passive_cont_proc, ui_comm_root_proc)


