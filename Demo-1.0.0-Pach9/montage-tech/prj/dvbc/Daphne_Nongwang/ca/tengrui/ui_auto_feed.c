/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2013).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*                  Lei Chen <lei.chen@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_auto_feed.h"
#include "sys_dbg.h"


enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
};

enum auto_feed_local_msg
{
  MSG_INSERT_MON_CARD = MSG_LOCAL_BEGIN + 625,
  MSG_INSERT_CHILD_CARD
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

static u8 g_auto_feed_status = FEED_STATUS_READY;
static chlid_card_status_info g_feed_info;
static u8 monther_timer_flag = 0;
static u8 child_timer_flag = 0;
static u8 exit_timer_flag = 0;
static u8 child_card_init_ok_flag = 0;
static u8 monther_card_init_ok_flag = 0;

extern BOOL g_son_card_need_feed_msg; 

u16 auto_feed_cont_keymap(u16 key);

RET_CODE auto_feed_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

extern BOOL ui_is_smart_card_insert(void);

RET_CODE open_auto_feed(u32 para1, u32 para2)
{
  control_t *p_cont, *p_txt;
  if(fw_find_root_by_id(ROOT_ID_AUTO_FEED) == NULL)
  {
    p_cont = fw_create_mainwin(ROOT_ID_AUTO_FEED,
                               AUTO_FEED_CONT_X, AUTO_FEED_CONT_Y,
                               AUTO_FEED_CONT_W, AUTO_FEED_CONT_H,
                               ROOT_ID_INVALID, 0,
                               OBJ_ATTR_INACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    
    ctrl_set_keymap(p_cont, auto_feed_cont_keymap);
    ctrl_set_proc(p_cont, auto_feed_cont_proc);
    
    ctrl_set_rstyle(p_cont,
                    RSI_TRANSPARENT,
                    RSI_TRANSPARENT,
                    RSI_TRANSPARENT);
    
    p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                             0, 0,
                             AUTO_FEED_CONT_W,AUTO_FEED_CONT_H,
                             p_cont, 0);
    ctrl_set_rstyle(p_txt,
                    RSI_AUTO_FEED_TXT,
                    RSI_AUTO_FEED_TXT,
                    RSI_AUTO_FEED_TXT);
    text_set_font_style(p_txt, FSI_AUTO_FEED_TXT, FSI_AUTO_FEED_TXT, FSI_AUTO_FEED_TXT);
    
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, IDS_E12_DF);
    ctrl_paint_ctrl(p_cont, TRUE);
    
    fw_tmr_create(ROOT_ID_AUTO_FEED, MSG_INSERT_MON_CARD, AUTO_FEED_AUTOCLOSE_MS, TRUE);
    monther_timer_flag = 1;

  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}

static RET_CODE on_auto_feed_state_update(control_t *p_cont, u16 msg, u32 para1, u32 para2, BOOL is_force)
{
  control_t *p_ctrl = NULL;
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CONTENT);

  switch(g_auto_feed_status)
  {
    case FEED_STATUS_MOTHER_SUCCESS:
      text_set_content_by_strid(p_ctrl, IDS_INSERT_CHILD_CARD);
      break;

    case FEED_STATUS_SON_SUCCESS:
      text_set_content_by_strid(p_ctrl, IDS_FEED_SUCCESS);
      fw_tmr_create(ROOT_ID_AUTO_FEED, MSG_EXIT, AUTO_FEED_AUTOCLOSE_MS, FALSE);
      exit_timer_flag = 1;
      g_son_card_need_feed_msg = FALSE;
      break;
    case FEED_STATUS_FEED_DATA_ERR:
      text_set_content_by_strid(p_ctrl, IDS_CAS_E_FEED_DATA_ERROR_DF);
      fw_tmr_create(ROOT_ID_AUTO_FEED, MSG_EXIT, AUTO_FEED_AUTOCLOSE_MS, FALSE);
      exit_timer_flag = 1;
      break;
    default:
      break;
  }

  ctrl_paint_ctrl(p_cont, is_force);

  return SUCCESS;
}

static RET_CODE on_insert_mon_card(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("on_insert_mon_card\n");
  if(monther_card_init_ok_flag)
  {
    ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
  }
  return SUCCESS;
}

static RET_CODE on_insert_child_card(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  UI_PRINTF("on_insert_child_card\n");
  if(child_card_init_ok_flag)
  {
    ui_ca_get_info(CAS_CMD_MON_CHILD_FEED, (u32)&g_feed_info, 0);
    fw_tmr_destroy(ROOT_ID_AUTO_FEED, MSG_INSERT_CHILD_CARD);
    child_timer_flag = 0;
  }

  return SUCCESS;
}

static RET_CODE on_ca_init_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(g_auto_feed_status == FEED_STATUS_MOTHER_SUCCESS)
  {
    child_card_init_ok_flag = 1;
  }
  if(g_auto_feed_status == FEED_STATUS_READY)
  {
    monther_card_init_ok_flag = 1;
  }

  return SUCCESS;
}

static RET_CODE on_auto_feed_cont_ca_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = (u32)para1;

  UI_PRINTF("ui ret = %d, feed_status = %d\n", ret, g_auto_feed_status);
  if(msg == MSG_CA_MON_CHILD_INFO)
  {
    switch(ret)
    {
      case SUCCESS:
        if(g_auto_feed_status == FEED_STATUS_READY)
        {
          g_auto_feed_status = FEED_STATUS_MOTHER_SUCCESS;
          memcpy(&g_feed_info, (void*)para2, sizeof(chlid_card_status_info));
          fw_tmr_destroy(ROOT_ID_AUTO_FEED, MSG_INSERT_MON_CARD);
          child_timer_flag = 0;
          fw_tmr_create(ROOT_ID_AUTO_FEED, MSG_INSERT_CHILD_CARD, AUTO_FEED_AUTOCLOSE_MS, TRUE);
          child_timer_flag = 1;
        }
        break;
      default:
        break;
    }

  }
  else if(msg == MSG_CA_MON_CHILD_FEED)
  {
    switch(ret)
    {
      case SUCCESS:
        if(g_auto_feed_status == FEED_STATUS_MOTHER_SUCCESS)
        {
          g_auto_feed_status = FEED_STATUS_SON_SUCCESS;
          child_card_init_ok_flag = 0;
          monther_card_init_ok_flag = 0;
        }
        break;
      default:
        g_son_card_need_feed_msg = FALSE;
        g_auto_feed_status = FEED_STATUS_FEED_DATA_ERR;
        break;
    }
  }
  on_auto_feed_state_update(p_cont, 0, 0, 0, TRUE);
  return SUCCESS;
}

static RET_CODE on_auto_feed_cont_destroy(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(monther_timer_flag)
  {
    fw_tmr_destroy(ROOT_ID_AUTO_FEED, MSG_INSERT_MON_CARD);
  }

  if(child_timer_flag)
  {
    fw_tmr_destroy(ROOT_ID_AUTO_FEED, MSG_INSERT_CHILD_CARD);
  }

  if(exit_timer_flag)
  {
    fw_tmr_destroy(ROOT_ID_AUTO_FEED, MSG_EXIT);
  }

  g_auto_feed_status = FEED_STATUS_READY;
  child_card_init_ok_flag = 0;
  monther_card_init_ok_flag = 0;
  monther_timer_flag = 0;
  child_timer_flag = 0;
  exit_timer_flag = 0;
  g_son_card_need_feed_msg = FALSE;
  return SUCCESS;
}

static RET_CODE on_auto_feed_cont_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("on_auto_feed_cont_exit\n");
  fw_destroy_mainwin_by_id(ROOT_ID_AUTO_FEED);
  exit_timer_flag = 0;
  g_son_card_need_feed_msg = FALSE;
  return SUCCESS;
}

BEGIN_KEYMAP(auto_feed_cont_keymap, NULL)
  //ON_EVENT(V_KEY_MENU, MSG_EXIT)
 // ON_EVENT(V_KEY_EXIT, MSG_EXIT)
END_KEYMAP(auto_feed_cont_keymap, NULL)

BEGIN_MSGPROC(auto_feed_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_INSERT_MON_CARD, on_insert_mon_card)
  ON_COMMAND(MSG_INSERT_CHILD_CARD, on_insert_child_card)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_auto_feed_cont_ca_msg)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_auto_feed_cont_ca_msg)
  ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_msg)
  ON_COMMAND(MSG_DESTROY, on_auto_feed_cont_destroy)
  ON_COMMAND(MSG_EXIT, on_auto_feed_cont_exit)
END_MSGPROC(auto_feed_cont_proc, cont_class_proc)


