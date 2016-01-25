/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 ****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_update.h"

static BOOL g_is_error_timer_exist = FALSE;
RET_CODE ca_card_update_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_UPDATE_BAR,
  IDC_CONTENT,
  IDC_ICON,
};
RET_CODE open_ca_card_update_info(u32 para1, u32 para2)
{
  control_t *p_content = NULL, *p_bar = NULL, *p_title = NULL, *p_icon = NULL, *p_cont = NULL;
  u16 uni_str[MAX_UPDATE_STR_LEN] = {0};
  if(fw_find_root_by_id(ROOT_ID_CA_CARD_UPDATE) != NULL)
  {
    UI_PRINTF("\n ROOT_ID_CA_CARD_UPDATE already opened............!!! \n");
    return ERR_FAILURE;
  }
  p_cont = fw_create_mainwin(ROOT_ID_CA_CARD_UPDATE,
                           CA_UP_CONT_FULL_X, CA_UP_CONT_FULL_Y,
                           CA_UP_CONT_FULL_W, CA_UP_CONT_FULL_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);
  ctrl_set_proc(p_cont, ca_card_update_proc);
  //title
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE,
                         CA_UP_TITLE_FULL_X, 5,//CA_UP_TITLE_FULL_Y,
                         CA_UP_TITLE_FULL_W,CA_UP_TITLE_FULL_H,
                         p_cont, 0);

  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);

  text_set_content_type(p_title, TEXT_STRTYPE_STRID);  
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  	text_set_content_by_strid(p_title, IDS_CAS_I_CARD_UPDATE);
  #elif CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG
  	text_set_content_by_strid(p_title, IDS_CAS_E_CARD_UPDATE);
  #else
    text_set_content_by_strid(p_title, IDS_CAS_E_CARD_UPDATE);
  #endif
//bar
  p_bar = ui_comm_bar_create(p_cont, IDC_UPDATE_BAR,
                       CA_STATUS_BAR_MX, CA_STATUS_BAR_MY, CA_STATUS_BAR_MW, CA_STATUS_BAR_MH,
                       CA_STATUS_BAR_LX, CA_STATUS_BAR_LY, CA_STATUS_BAR_LW, CA_STATUS_BAR_LH,
                       CA_STATUS_BAR_RX, CA_STATUS_BAR_RY,CA_STATUS_BAR_RW, CA_STATUS_BAR_RH);
  ui_comm_bar_set_param(p_bar, IDS_PROGRESS, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_COMM_PBAR_MID,
                          RSI_IGNORE, FSI_INDEX3,
                          RSI_SN_BG, FSI_INDEX3);//FSI_COMM_TXT_N
  ui_comm_bar_update(p_bar, 0, TRUE);

  //content
  p_content = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                       CA_UP_CONTENT_FULL_X, CA_UP_CONTENT_FULL_Y,
                       CA_UP_CONTENT_FULL_W, CA_UP_CONTENT_FULL_H,
                       p_cont, 0);
  ctrl_set_rstyle(p_content, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  text_set_font_style(p_content, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);//FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY
  text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
  gui_get_string(IDS_CA_CARD_NEED_UPDATE, uni_str, MAX_UPDATE_STR_LEN-1);
  text_set_content_by_unistr(p_content, uni_str);
  //icon
  p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_ICON,
                   10, 10,
                   46, 42,
                   p_cont, 0);
  bmap_set_content_by_id(p_icon, IM_SN_ICON_WARNING_2);

  ctrl_set_attr(p_content, OBJ_ATTR_HIDDEN);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  
  if(g_is_error_timer_exist)
  {  
    fw_tmr_destroy(ROOT_ID_CA_CARD_UPDATE, MSG_TIMER_EXPIRED);
    g_is_error_timer_exist = FALSE;
  }
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(p_cont != NULL)
  {
    ctrl_paint_ctrl(p_cont, FALSE);
  }
  return SUCCESS;
}

static void get_medley_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
  u16 str[10] = {0}; 
  u16 len = 0;
  
  gui_get_string(str_id1, uni_str, MAX_UPDATE_STR_LEN-1);
  convert_i_to_dec_str(str, content); 
  uni_strcat(uni_str, str,MAX_UPDATE_STR_LEN-1);

  if(0 != str_id2)
  {
    len = (u16)uni_strlen(uni_str);
    gui_get_string(str_id2, str, MAX_UPDATE_STR_LEN - len -1); 
    uni_strcat(uni_str, str, MAX_UPDATE_STR_LEN-1); 
  }
  return;
}

static void card_update_progress(control_t *p_cont, u16 msg, u16 progress)
{
  control_t *p_bar = NULL;
  
  p_bar = ctrl_get_child_by_id(p_cont, IDC_UPDATE_BAR);

  if(p_bar == NULL )
  {
    return;
  }
  if(ctrl_get_attr(p_bar) != OBJ_ATTR_HIDDEN)
  {
    UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
    ui_comm_bar_update(p_bar, progress, TRUE);
    ui_comm_bar_paint(p_bar, TRUE);
  }
  
  ctrl_paint_ctrl(p_cont, TRUE);
  return;
}

static RET_CODE on_ca_card_up_info_update(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  u8 percent = *(u8 *)para2;

  if(percent <= 0)
    percent = 0;
  
  if(percent >= 100)
    percent = 100;
  
  UI_PRINTF("PROCESS -> update progress\n");

  card_update_progress(cont, msg,(u16)percent);
  
  return SUCCESS;
}

static RET_CODE on_ca_card_up_show_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 error_code = *(u8*)para2;
  control_t *p_bar = NULL;
  control_t *p_content = NULL;
  u16 uni_str[MAX_UPDATE_STR_LEN]={0};
  
  p_bar = ctrl_get_child_by_id(p_cont, IDC_UPDATE_BAR);
  p_content = ctrl_get_child_by_id(p_cont, IDC_CONTENT);
  
  if(ctrl_get_attr(p_bar) != OBJ_ATTR_HIDDEN)
  {
    ctrl_set_attr(p_bar, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_content, OBJ_ATTR_ACTIVE);
    text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
    get_medley_str(uni_str,IDS_CA_CARD_UPDATE_ERR,0,error_code);
    text_set_content_by_unistr(p_content, uni_str);

  }
  if(!g_is_error_timer_exist)
  {  
    fw_tmr_create(ROOT_ID_CA_CARD_UPDATE, MSG_TIMER_EXPIRED, 10000,FALSE);
    g_is_error_timer_exist = TRUE;
  }
  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

RET_CODE close_ca_card_update_info(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  set_uiokey_status(FALSE);
  
  fw_tmr_destroy(ROOT_ID_CA_CARD_UPDATE, MSG_TIMER_EXPIRED);
  manage_close_menu(ROOT_ID_CA_CARD_UPDATE, 0, 0);
  g_is_error_timer_exist = FALSE;
  
  return SUCCESS;
}

BEGIN_MSGPROC(ca_card_update_proc, cont_class_proc)
  //ON_COMMAND(MSG_CA_CARD_UPDATE_BEGIN, on_ca_card_up_info_update)
  ON_COMMAND(MSG_CA_CARD_PROGRESS, on_ca_card_up_info_update)
  ON_COMMAND(MSG_CA_CARD_UPDATE_ERR, on_ca_card_up_show_error)
  ON_COMMAND(MSG_CA_CARD_UPDATE_END, close_ca_card_update_info)
  ON_COMMAND(MSG_TIMER_EXPIRED, close_ca_card_update_info)
END_MSGPROC(ca_card_update_proc, cont_class_proc)

