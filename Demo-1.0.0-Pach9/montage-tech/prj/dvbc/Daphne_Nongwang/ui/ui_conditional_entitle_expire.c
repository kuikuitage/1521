/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 ****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_entitle_expire.h"

RET_CODE comm_ca_entitle_expire_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CONTENT,
  IDC_ICON,
};

#if 0
rsc

static void get_entitle_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
  u16 str[10] = {0}; 
  u16 len = 0;
  
  gui_get_string(str_id1, uni_str, 64);
  convert_i_to_dec_str(str, content); 
  uni_strcat(uni_str, str,64);
  
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id2, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}
#endif

RET_CODE open_ca_entitle_expire_dlg(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_content, *p_title, *p_icon, *p_cont = NULL;
  u16 uni_str[64] = {0};
  u32 reminds = *(u32*)para2;
  
  if((!ui_is_fullscreen_menu(fw_get_focus_id()) )|| (fw_find_root_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG) != NULL))
  {
    return ERR_FAILURE;
  }
  p_cont = fw_create_mainwin(ROOT_ID_CA_ENTITLE_EXP_DLG,
                           ENTITLE_CONT_FULL_X, ENTITLE_CONT_FULL_Y,
                           ENTITLE_CONT_FULL_W, ENTITLE_CONT_FULL_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
  return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_proc(p_cont, comm_ca_entitle_expire_proc);
  //title
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE,
                         ENTITLE_TITLE_FULL_X, ENTITLE_TITLE_FULL_Y,
                         ENTITLE_TITLE_FULL_W,ENTITLE_TITLE_FULL_H,
                         p_cont, 0);

  ctrl_set_rstyle(p_title, RSI_DLG_TITLE, RSI_DLG_TITLE, RSI_DLG_TITLE);
  text_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);

  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_ACCREDIT_INFO);

  //content
  p_content = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                       ENTITLE_CONTENT_FULL_X, ENTITLE_CONTENT_FULL_Y,
                       ENTITLE_CONTENT_FULL_W, ENTITLE_CONTENT_FULL_H,
                       p_cont, 0);
  ctrl_set_rstyle(p_content, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
  text_set_font_style(p_content, FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);
  text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
  get_entitle_str(uni_str,IDS_CA_AUTH_EXPIRE_TIME,IDS_DAY,reminds);
  text_set_content_by_unistr(p_content, uni_str);

  //icon
  p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_ICON,
                   10, 0,
                   40, 40,
                   p_cont, 0);
  bmap_set_content_by_id(p_icon, IM_ICON_WARNING);

  fw_tmr_create(ROOT_ID_CA_ENTITLE_EXP_DLG, MSG_TIMER_EXPIRED, ENTITLE_TIME_OUT, FALSE);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(p_cont != NULL)
  {
    ctrl_paint_ctrl(p_cont, FALSE);
  }
#endif
  return SUCCESS;
}


void close_ca_entitle_expire_dlg(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG);
}

static RET_CODE on_ca_entitle_expire_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_entitle_expire_dlg();
  return SUCCESS;
}

BEGIN_MSGPROC(comm_ca_entitle_expire_proc, cont_class_proc)
  ON_COMMAND(MSG_TIMER_EXPIRED, on_ca_entitle_expire_timeout)
END_MSGPROC(comm_ca_entitle_expire_proc, cont_class_proc)

