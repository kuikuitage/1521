/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_prompt.h"
#include "ui_new_mail.h"

enum ca_prompt_ctrl_id
{
  IDC_MESSAGE_TYPE = 1,
  IDC_MESSAGE_TITLE,
  IDC_MESSAGE_CONTENT,  

//  IDC_MESSAGE_DETAIL_INFO_BG,

  IDC_MESSAGE_RECEIVE_TIME_TITLE,
  IDC_MESSAGE_RECEIVE_TIME_CONTENT,

  IDC_MESSAGE_BAR,
  IDC_MESSAGE_NEW_TITLE,
  IDC_MESSAGE_NEW_CONTENT,

  IDC_MESSAGE_IMPORT_TITLE,
  IDC_MESSAGE_IMPORT_CONTENT,
  IDC_EMAIL_HEAD,
  IDC_MESSAGE_FRM,
};

#if 0
static comm_help_data_t2 ca_prompt_help_data = 
{
  1, 30, {650,},
  {
    HELP_RSC_STRID | IDS_HELP_OK_EXIT_MENU,
  },
};
#endif

prompt_type_t prompt_type;
cas_mail_headers_t mail_header;
cas_announce_headers_t alert_header;

u16 ca_prompt_root_keymap(u16 key);
RET_CODE ca_prompt_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE ca_prompt_announce_set_content(control_t *p_cont, u16 index, cas_announce_headers_t *p_alert_header, 
  cas_announce_body_t *p_cas_announce_body)
{
	control_t *p_frm =	ctrl_get_child_by_id(p_cont, IDC_MESSAGE_FRM);
	control_t *p_message_content = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_CONTENT);
  u32 new_announce = 0;
  u32 i;
  u16 uni_str[1024] = {0};
  for(i = 0; i < p_alert_header->max_num; i++)
  {
    if(p_alert_header->p_mail_head[i].new_email
    && i != index)
      new_announce++;
  }
  if(p_cas_announce_body)
  {
    gb2312_to_unicode(p_cas_announce_body->data,1023,uni_str,1023);
    text_set_content_by_unistr(p_message_content, uni_str);
  }
  return SUCCESS;
}
  
static RET_CODE ca_prompt_email_set_content(control_t *p_cont, u16 index, cas_mail_headers_t *p_mail_header, 
  cas_mail_body_t *p_cas_mail_body)
{
  control_t *p_frm =  ctrl_get_child_by_id(p_cont, IDC_MESSAGE_FRM);
  control_t *p_message_content = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_CONTENT);
  u32 new_mail = 0;
  u32 i;
  u16 uni_str[256];
  //UI_PRINTF("ca_prompt_email_set_content p_mail_header->max_num=%d\n",p_mail_header->max_num);
  for(i = 0; i < p_mail_header->max_num; i++)
  {
    if(p_mail_header->p_mail_head[i].new_email
      && i != index)
      new_mail++;
  }
  
  //UI_PRINTF("ca_prompt_email_set_content new_mail=%d\n",new_mail);
  if(new_mail == 0)
  {
    if(ui_is_new_mail())
    {
      close_new_mail();
      ui_set_new_mail(FALSE);
    }
  }
  
  if(p_cas_mail_body)
  {
    gb2312_to_unicode(p_cas_mail_body->data,255,uni_str,255);
    text_set_content_by_unistr(p_message_content, uni_str);
  }
  return SUCCESS;
}
  
RET_CODE open_ca_prompt(u32 para1, u32 para2)
{
#define EMAIL_HEAD_PG_X		40
#define EMAIL_HEAD_PG_Y		5
#define EMAIL_HEAD_PG_W		(640-EMAIL_HEAD_PG_X-30)
#define EMAIL_HEAD_PG_H		75
#define EMAIL_TYPE_X		EMAIL_HEAD_PG_X+20
#define EMAIL_TYPE_Y		EMAIL_HEAD_PG_Y
#define EMAIL_TYPE_W		40
#define EMAIL_TYPE_H		30
#define EMAIL_BAR_Y			290
#define EMAIL_BAR_H			31
#define EMAIL_HELP_CNT      4
  control_t *p_cont,*p_ctrl, *p_frm;
  msg_info_t *p_cas_msg = NULL;
  u8 asc_str[64];
  u32 new_mail = 0;
  u32 new_announce = 0;
  u32 index = 0;
  u32 i;
#ifdef WIN32
	u8 str[1024];
#endif
  DEBUG(TRCA, INFO, "start\n");
  if(fw_find_root_by_id(ROOT_ID_CA_PROMPT) != NULL)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_CA_PROMPT);
  }
  
#ifndef WIN32
  memcpy(&prompt_type, (prompt_type_t *)para1, sizeof(prompt_type_t));
#else
  prompt_type.message_type = 0xFF;
#endif

  p_cont = fw_create_mainwin(ROOT_ID_CA_PROMPT,
                             CA_PROMPT_X, CA_PROMPT_Y,
                             CA_PROMPT_W, CA_PROMPT_H,
                             0, 0,
                             OBJ_ATTR_ACTIVE, 0);  
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);  
  ctrl_set_keymap(p_cont, ca_prompt_root_keymap);
  ctrl_set_proc(p_cont, ca_prompt_root_proc);
  
  p_frm = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_MESSAGE_FRM),
                            (CA_PROMPT_W-580)/2, (CA_PROMPT_H-400)/2, 580, 400, p_cont, 0);
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_TYPE),
                            0, 0, 200, 31, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
#ifdef WIN32
	sprintf((char*)str, "mail type");
	text_set_content_by_ascstr(p_ctrl, (u8*)str);
#endif
  //message title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_TITLE),
                            0, 40, 580, 35, p_frm, 0);

  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_offset(p_ctrl,0,5);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

#ifdef WIN32
	sprintf((char*)str, "message 3");
	text_set_content_by_ascstr(p_ctrl, (u8*)str); 
#endif 
  
  //message content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_CONTENT),
                            0, 75, 580, 200, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

#ifdef WIN32
	sprintf((char*)str, "messagem78bhjdwa,jguishyfoiels,iwjfgryug,fbhedf3sad256461dsfgtzdfdwedsadwuasdw,igyh,adeerfhnutihjed89uiorljnkdjfhwuiowosmdwtesttesthjsaihde78bhjdwadskihjigjksdbnfeuoishfdneouihlfneusibdfawiodowpk;pogihr89ygrhugjlrjdipojguishyfoielsuifhesufeusoiWH[doiwjfgryug,fbhedf3sad256461dsfgtzdfdwedsadwuasdw,igyh,adeerfhnutihjed89uiorljnkdjfhwuiowosmdwtest");
	text_set_content_by_ascstr(p_ctrl, (u8*)str); 
#endif

  //message receive time title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_RECEIVE_TIME_TITLE),
                            0, 285, 120, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVE_TIME);

  //message receive time content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_RECEIVE_TIME_CONTENT),
                            120, 285, 200, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

#ifdef WIN32
  sprintf((char*)asc_str, "2014.05.05-20:20:20");
  text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
#endif
  //message new title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_NEW_TITLE),
                            320, 285, 70, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NEW);
  
  //message new content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_NEW_CONTENT),
                            390, 285, 50, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

#ifdef WIN32
  text_set_content_by_strid(p_ctrl, IDS_YES);
#endif
  //message import title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_IMPORT_TITLE),
                            440, 285, 90, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IMPORTANT);
  
  //message import content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MESSAGE_IMPORT_CONTENT),
                            530, 285, 50, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

#ifdef WIN32
  text_set_content_by_strid(p_ctrl, IDS_YES);
#endif
    ctrl_default_proc(p_frm, MSG_GETFOCUS, 0, 0);
  if(prompt_type.message_type == 0)
  {
    memcpy(&mail_header, (cas_mail_headers_t *)para2, sizeof(cas_mail_headers_t));
    //UI_PRINTF("open_ca_prompt max=%d\n",mail_header.max_num);
    index = mail_header.p_mail_head[prompt_type.index].m_id;
    for(i = 0; i < mail_header.max_num; i++)
    {
      if(mail_header.p_mail_head[i].new_email
        && i != index)
        new_mail++;
      
      p_ctrl =ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TYPE);
      sprintf((char*)asc_str, "[%d] Mail", (int)new_mail);
	  DEBUG(TRCA, INFO, "\n");
      text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TITLE);
	  DEBUG(TRCA, INFO, "\n");
      text_set_content_by_ascstr(p_ctrl, (u8*)mail_header.p_mail_head[index].subject);
      
      p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_CONTENT);
      sprintf((char*)asc_str, "%04d.%02d.%02d-%02d:%02d:%02d", 
      mail_header.p_mail_head[index].creat_date[0] * 100 + 
      mail_header.p_mail_head[index].creat_date[1],
      mail_header.p_mail_head[index].creat_date[2],
      mail_header.p_mail_head[index].creat_date[3],
      mail_header.p_mail_head[index].creat_date[4],
      mail_header.p_mail_head[index].creat_date[5],
      mail_header.p_mail_head[index].creat_date[6]);
    
	DEBUG(TRCA, INFO, "\n");
      text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_CONTENT);

      if(mail_header.p_mail_head[index].new_email)
        text_set_content_by_strid(p_ctrl, IDS_YES);
      else
        text_set_content_by_strid(p_ctrl, IDS_NO);
      
      p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_CONTENT);

      if(mail_header.p_mail_head[index].priority)
        text_set_content_by_strid(p_ctrl, IDS_YES);
      else
        text_set_content_by_strid(p_ctrl, IDS_NO);
    }
    ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, 
      mail_header.p_mail_head[prompt_type.index].m_id, 0);
  }
  else if(prompt_type.message_type == 1)
  {
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    
    memcpy(&alert_header, (cas_announce_headers_t *)para2, sizeof(cas_announce_headers_t));
    index = alert_header.p_mail_head[prompt_type.index].m_id;
    for(i = 0; i < alert_header.max_num; i++)
    {
      if(alert_header.p_mail_head[i].new_email
      && i != index)
        new_announce++;
    }
    p_ctrl =ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TYPE);
    sprintf((char*)asc_str, "[%d] Announce", (int)new_announce);
	DEBUG(TRCA, INFO, "\n");
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TITLE);
	DEBUG(TRCA, INFO, "\n");
    text_set_content_by_ascstr(p_ctrl, (u8*)alert_header.p_mail_head[index].subject);
    
    ui_ca_get_info(CAS_CMD_ANNOUNCE_BODY_GET, 
      alert_header.p_mail_head[prompt_type.index].m_id, 0);
  }
  else if(prompt_type.message_type == 2)
  {
     p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TYPE);
     sprintf((char*)asc_str, "Mail"); 
	 DEBUG(TRCA, INFO, "\n");
     text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
     
     p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TITLE);
     sprintf((char*)asc_str, "New Mail"); 
	 DEBUG(TRCA, INFO, "\n");
     text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_TITLE);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_CONTENT);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
    
  }
  else if(prompt_type.message_type == 3)
  {
     p_cas_msg = (msg_info_t *)para2;

     if(p_cas_msg->type == 1/*CAS_MSG_ANNOUNCE*/)
     {
       p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TYPE);
       sprintf((char*)asc_str, "Announce"); 
	   DEBUG(TRCA, INFO, "\n");
       text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
     }
     else if(p_cas_msg->type == 2/*CAS_MSG_NOTIFICATION*/)
     {
       p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TYPE);
       sprintf((char*)asc_str, "Notify"); 
	   DEBUG(TRCA, INFO, "\n");
       text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
     }
     
     p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TITLE);
	 DEBUG(TRCA, INFO, "\n");
     text_set_content_by_ascstr(p_ctrl, (u8*)p_cas_msg->title);

     p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_CONTENT);
	 DEBUG(TRCA, INFO, "\n");
     text_set_content_by_ascstr(p_ctrl, (u8*)p_cas_msg->data);

     p_ctrl  = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_CONTENT);
      sprintf((char*)asc_str, "%04d.%02d.%02d-%02d:%02d:%02d", 
      p_cas_msg->create_time[0] * 100 + 
      p_cas_msg->create_time[1],
      p_cas_msg->create_time[2],
      p_cas_msg->create_time[3],
      p_cas_msg->create_time[4],
      p_cas_msg->create_time[5],
      p_cas_msg->create_time[6]);
	  DEBUG(TRCA, INFO, "\n");
      text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
     
     p_ctrl= ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_CONTENT);
     text_set_content_by_strid(p_ctrl, IDS_YES);

     p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_CONTENT);
     text_set_content_by_strid(p_ctrl, p_cas_msg->priority?IDS_YES:IDS_NO);
     
  }
  ctrl_paint_ctrl(p_cont, FALSE);
  return SUCCESS;
}

static RET_CODE on_ca_mail_update_body(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  
  //UI_PRINTF("on_ca_mail_update_body p_prompt_type->index=%d p_mail_header->max_num=%d\n",prompt_type.index, mail_header.max_num);
  ca_prompt_email_set_content(p_cont, (u16)prompt_type.index, 
    &mail_header, (cas_mail_body_t *)para2);
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_ca_announce_update_body(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ca_prompt_announce_set_content(p_cont, (u16)prompt_type.index, 
    &alert_header, (cas_announce_body_t *)para2);
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_ca_prompt_exit(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  if(prompt_type.message_type == 0)
  {
    ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0, 0);
  }
  else if(prompt_type.message_type == 1)
  {
    ui_ca_get_info(CAS_CMD_ANNOUNCE_HEADER_GET, 0, 0);
  }
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(ca_prompt_root_keymap, ui_comm_root_keymap)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ca_prompt_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_prompt_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_ca_prompt_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_ca_prompt_exit)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, on_ca_mail_update_body)
  ON_COMMAND(MSG_CA_ANNOUNCE_BODY_INFO, on_ca_announce_update_body)  
END_MSGPROC(ca_prompt_root_proc, ui_comm_root_proc)

