/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept.h"
#include "ui_ca_public.h"


cas_card_info_t *p_card_info = NULL;

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
static card_and_ipp_info p_card_ipp_info ;
#endif

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

#if 0
rsc

static comm_help_data_t ca_menu_help_data = //help bar data
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
#endif
u16 conditional_accept_cont_keymap(u16 key);
RET_CODE conditional_accept_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_btn_keymap(u16 key);

RET_CODE conditional_accept_btn_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_ca_frm_btn_keymap(u16 key);

RET_CODE conditional_accept_ca_frm_btn_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

extern void g_set_operator_info(cas_operators_info_t *p);
extern void g_set_worktime_info(cas_card_work_time_t * p);

RET_CODE open_conditional_accept(u32 para1, u32 para2)
{
  control_t *p_cont, *p_btn = NULL;
  u16 y = 0;

  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT,
								 0, 0,
								 640, 480,
								 IDS_CA_INFO,TRUE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_cont_proc);

  //button
  y = CONDITIONAL_ACCEPT_BTN_Y;
  //choose menu  base on CA
  p_btn = conditional_accept_menu_info(p_cont, y);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif


  ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
  ui_ca_get_info(CAS_CMD_OPERATOR_INFO_GET,0,0);
  ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0,0);
  ui_ca_get_info(CAS_CMD_RATING_GET, 0 ,0);
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
  ui_ca_get_info(CAS_CMD_WORK_TIME_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_ope_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB) || (CONFIG_CAS_ID == CAS_ID_LX))
  cas_operators_info_t *oper_info = NULL;
  oper_info = (cas_operators_info_t *)para2;
  OS_PRINTF("!!!!!!!!!!!!!!!!!!!operator max num : %d\n",oper_info->max_num);
  g_set_operator_info(oper_info);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_worktime_initial(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
  cas_card_work_time_t *card_word_time_info = NULL;
  card_word_time_info = (cas_card_work_time_t *)para2;
  
  UI_PRINTF("@@@@@ start hour:%d min:%d sec:%d\n", 
                  card_word_time_info->start_hour,
                  card_word_time_info->start_minute,
                  card_word_time_info->start_second);
  UI_PRINTF("@@@@@ start hour:%d min:%d sec:%d\n", 
                  card_word_time_info->end_hour,
                  card_word_time_info->end_minute,
                  card_word_time_info->end_second);
 
  g_set_worktime_info(card_word_time_info);
  #endif
  return SUCCESS;
}

#if WIN32
cas_rating_set_t p_data_test;
card_stb_paired_list_t *ui_get_p_card_rating_data()
{
  p_data_test.rate= 7;
  return &p_data_test;
}
#endif

static RET_CODE on_conditional_accept_level_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
  cas_rating_set_t *p_rate_info = NULL;

  #ifndef WIN32
    p_rate_info = (cas_rating_set_t *)para2;
  #else
    p_rate_info = ui_get_p_card_rating_data();
  #endif

  OS_PRINTF("!!!!!!!!!!!!!!!!!!!p_rate_info->rate : %d\n",p_rate_info->rate);
  p_card_info->cas_rating = p_rate_info->rate;
  #endif
  return SUCCESS;
}

static RET_CODE on_ca_frm_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl));
  #if(CONFIG_CAS_ID != CONFIG_CAS_ID_WF)
  on_ca_frm_btn_select_number(ctrlID, (u32)p_card_info);
  #else
  on_ca_frm_btn_select_number(ctrlID,(u32)(&p_card_ipp_info));
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_card_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  p_card_info = (cas_card_info_t *)para2;
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
  p_card_ipp_info.p_card_info =p_card_info;
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_card_burses_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
   #if(CONFIG_CAS_ID == CONFIG_CAS_ID_WF)
   p_card_ipp_info.p_ipp_buy_info= (burses_info_t *)para2;
  #endif
  return SUCCESS;
}


static RET_CODE on_conditional_accept_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  p_card_info = NULL;

  return SUCCESS;
}

static RET_CODE on_exit_conditional_accept(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT, 0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_card_info_update) 
  ON_COMMAND(MSG_CA_OPE_INFO, on_conditional_accept_ope_update) 
  ON_COMMAND(MSG_CA_BUR_INFO, on_conditional_accept_card_burses_info_update) 
  ON_COMMAND(MSG_CA_RATING_GET, on_conditional_accept_level_info_update)
  ON_COMMAND(MSG_CA_GET_WORK_TIME_INFO, on_conditional_accept_worktime_initial)
  ON_COMMAND(MSG_SAVE, on_conditional_accept_save)
END_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_ca_frm_btn_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(conditional_accept_ca_frm_btn_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_ca_frm_btn_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_ca_frm_btn_select)
  ON_COMMAND(MSG_EXIT, on_exit_conditional_accept)
END_MSGPROC(conditional_accept_ca_frm_btn_proc, cont_class_proc)



