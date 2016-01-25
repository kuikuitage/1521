/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_notify.h"

enum ca_notify_cont_id
{
  IDC_NOTIFY_FRM = 1,
};

enum ca_notify_ctrl_id
{
  IDC_NOTIFY_TYPE = 1,
  IDC_NOTIFY_TITLE,
  IDC_NOTIFY_CONTENT,  

  IDC_NOTIFY_DETAIL_INFO_BG,

  IDC_NOTIFY_RECEIVE_TIME_TITLE,
  IDC_NOTIFY_RECEIVE_TIME_CONTENT,

  IDC_NOTIFY_NEW_TITLE,
  IDC_NOTIFY_NEW_CONTENT,

  IDC_NOTIFY_IMPORT_TITLE,
  IDC_NOTIFY_IMPORT_CONTENT,
};

u16 ca_notify_root_keymap(u16 key);
RET_CODE ca_notify_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
  
RET_CODE open_ca_notify(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl,*p_frm;
  msg_info_t *p_cas_msg = NULL;
  u8 asc_str[64];
  u16 uni_str[1024] = {0};
  // check for close
  DEBUG(TRCA, INFO, "*****start\n");
  if(fw_find_root_by_id(ROOT_ID_CA_NOTIFY) != NULL)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_CA_NOTIFY);
  }
  
  p_cont = fw_create_mainwin(ROOT_ID_CA_NOTIFY,
                             CA_NOTIFY_X, CA_NOTIFY_Y,
                             CA_NOTIFY_W, CA_NOTIFY_H,
                             0, 0,
                             OBJ_ATTR_ACTIVE, 0);  
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, ca_notify_root_keymap);
  ctrl_set_proc(p_cont, ca_notify_root_proc);

  //frm
  p_frm = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_NOTIFY_FRM),
                            0, 0, 580, 400, p_cont, 0);

  //message type
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_TYPE),
                            0, 0, 200, 31, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //message title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_TITLE),
                            0, 40, 580, 35, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_offset(p_ctrl,0,5);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //message content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_CONTENT),
                            0, 75, 580, 200, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //message content
  /*
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_DETAIL_INFO_BG),
                            0, 285, 580, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_SH, RSI_LIST_ITEM_SH, RSI_LIST_ITEM_SH);
  DEBUG(TRCA, INFO, "end\n");
  */

  //message receive time title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_RECEIVE_TIME_TITLE),
                            0, 285, 120, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVE_TIME);

  //message receive time content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_RECEIVE_TIME_CONTENT),
                            120, 285, 200, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //message new title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_NEW_TITLE),
                            320, 285, 70, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NEW);
  
  //message new content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_NEW_CONTENT),
                            390, 285, 50, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_ctrl, IDS_YES);

  //message import title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_IMPORT_TITLE),
                            440, 285, 90, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IMPORTANT);
  
  //message import content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_NOTIFY_IMPORT_CONTENT),
                            530, 285, 50, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_ctrl, IDS_YES);
  
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  
  p_cas_msg = (msg_info_t *)para2;

  if(p_cas_msg->type == 1/*CAS_MSG_ANNOUNCE*/)
  {
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_TYPE);
    sprintf((char*)asc_str, "Announce"); 
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  }
  else if(p_cas_msg->type == 2/*CAS_MSG_NOTIFICATION*/)
  {
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_TYPE);
    sprintf((char*)asc_str, "Notify"); 
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  }

  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_TITLE);
  text_set_content_by_ascstr(p_ctrl, (u8*)p_cas_msg->title);

  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_CONTENT);
  gb2312_to_unicode(p_cas_msg->data,1023,uni_str,1023);
  text_set_content_by_unistr(p_ctrl, uni_str);

  p_ctrl  = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_RECEIVE_TIME_CONTENT);
  sprintf((char*)asc_str, "%04d.%02d.%02d-%02d:%02d:%02d", 
  p_cas_msg->create_time[0] * 100 + 
  p_cas_msg->create_time[1],
  p_cas_msg->create_time[2],
  p_cas_msg->create_time[3],
  p_cas_msg->create_time[4],
  p_cas_msg->create_time[5],
  p_cas_msg->create_time[6]);
  text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

  p_ctrl= ctrl_get_child_by_id(p_frm, IDC_NOTIFY_NEW_CONTENT);
  text_set_content_by_strid(p_ctrl, IDS_YES);

  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_NOTIFY_IMPORT_CONTENT);
  text_set_content_by_strid(p_ctrl, p_cas_msg->priority?IDS_YES:IDS_NO);

  ctrl_paint_ctrl(p_cont, FALSE);
  DEBUG(TRCA, INFO, "end\n");
  return SUCCESS;
}

BEGIN_KEYMAP(ca_notify_root_keymap, ui_comm_root_keymap)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ca_notify_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_notify_root_proc, ui_comm_root_proc)
END_MSGPROC(ca_notify_root_proc, ui_comm_root_proc)

