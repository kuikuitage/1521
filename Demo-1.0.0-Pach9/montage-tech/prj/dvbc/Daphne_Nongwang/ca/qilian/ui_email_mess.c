
#include "ui_common.h"
#include "ui_email_mess.h"

static cas_mail_headers_t *p_mail_data_buf = NULL;
static u16 cur_focus = 0;
static u8 g_del_mail = 0; 

enum control_id
{
  IDC_INVALID = 0,
  IDC_EMAIL_CONT,
  IDC_EMAIL_HEAD,
  IDC_EMAIL_NUM,
  IDC_EMAIL_FHIDE,
  IDC_EMAIL_FREQ,
  IDC_EMAIL_FFREQ,
  IDC_EMAIL_FAV,
  IDC_EMAIL_FFAV,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_NOTIC,
  IDC_EMAIL_BODY_CONT,
  IDC_EMAIL_TEXT,
};

enum nvod_email_mess_local_msg
{
  MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
  MSG_DELETE_ALL,
};

u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len)
{
  u32 i = 0;
  u32 ascStrLen = 0;
  u32 tmp_value = 0;
  
  if((NULL == p_ascstr) || (NULL == p_dec_number) || (max_len > 8))
  {
      return ERR_FAILURE;
  }
  
  ascStrLen = strlen((char *)p_ascstr);
  *p_dec_number = 0;

  if((0 == ascStrLen) || (ascStrLen > max_len))
  {
      return ERR_FAILURE;
  }
  
  for(i = 0; i < ascStrLen; i++)
  {
    if((p_ascstr[i] >= '0') && (p_ascstr[i] <= '9'))
    {
        tmp_value = p_ascstr[i] - '0';
        *p_dec_number = (*p_dec_number) * 10 + tmp_value;
    }
    else
    {
        break;
    }
  }

  return SUCCESS;
}

extern void ui_ca_get_info(u32 cmd_id, u32 para1, u32 para2);

static RET_CODE email_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(ctrl);
  u8 asc_str[32];
  u16 uni_str[64];
  utc_time_t  s_ca_time = {0};
  utc_time_t  s_local_time = {0};
  u32 tmp_value = 0;
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%d ", i + start + 1);   //p_mail_data_buf->p_mail_head[i + start].m_id);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].subject, 80, 
                      uni_str, 40);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, (u16 *)uni_str);

      /* Is new */
      if(p_mail_data_buf->p_mail_head[i + start].new_email)
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 2, IM_SN_ICON_MAIL_NEW);
      }
      else
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 2, IM_SN_ICON_MAIL_READED);
      }
      
      //date
      s_ca_time.year = p_mail_data_buf->p_mail_head[i + start].send_date[0] * 100 
                                  + p_mail_data_buf->p_mail_head[i + start].send_date[1];
      s_ca_time.month =  p_mail_data_buf->p_mail_head[i + start].send_date[2];
      s_ca_time.day = p_mail_data_buf->p_mail_head[i + start].send_date[3];
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].send_date[4]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.hour = (u8)tmp_value;
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].send_date[5]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.minute = (u8)tmp_value;

      time_to_local(&s_ca_time, &s_local_time);
      sprintf((char *)asc_str,"%2d/%2d/%2d %2d:%2d", 
                s_local_time.year, s_local_time.month, 
                s_local_time.day, s_local_time.hour, s_local_time.minute);
      str_asc2uni(asc_str, uni_str);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 3, uni_str);
    }
  }
  return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u32 email_No = 0;
  u16 size = EMAIL_LIST_PAGE;
  p_mail_data_buf = (cas_mail_headers_t*)para2;
  //p_mail_data_buf = &test_cas_mail_head;

  list_set_count(p_ctrl, p_mail_data_buf->total_email_num, EMAIL_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, cur_focus);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);
  email_No = list_get_focus_pos(p_ctrl);
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_No ,0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
 return SUCCESS;

}

static RET_CODE plist_update_body(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 new_focus;
  control_t *pCtrl_text = NULL;
  cas_mail_body_t *p_mail_body_data = NULL;
  u16 uni_str[256] = {0};

  p_mail_body_data = (cas_mail_body_t *)para2;
  if(NULL == p_mail_body_data)
  {
      return ERR_FAILURE;
  }
  new_focus = list_get_focus_pos(p_ctrl);
  p_mail_data_buf->p_mail_head[new_focus].new_email = 0;

  pCtrl_text = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_TEXT);
  //uni_strlen(const u16 * p_str);
  OS_PRINTF("\r\n ***plist_update_body %s ****\r\n" ,p_mail_body_data->data);
  if((0 == p_mail_body_data->data[0]) && (0 == p_mail_body_data->data[1]))
  {
      OS_PRINTF("\r\n ***the email body data is NULL ****\r\n");
      text_set_content_by_unistr(pCtrl_text,uni_str);
      ctrl_paint_ctrl(pCtrl_text, TRUE);
      return ERR_FAILURE;
  }
  gb2312_to_unicode(p_mail_body_data->data, 256,
                uni_str, 256);
  text_set_content_by_unistr(pCtrl_text,uni_str);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), EMAIL_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(pCtrl_text, TRUE);
  return SUCCESS;
}

RET_CODE open_email_mess(u32 para1, u32 para2)
{
#define SN_EMAIL_HELP_CNT           		 16
	  
	static sn_comm_help_data_t sn_email_help_data;
	help_rsc help_item[SN_EMAIL_HELP_CNT]=
	{ 
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
		 {SN_IDC_HELP_BMAP, 30 , IM_SN_F1},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
		 {SN_IDC_HELP_TEXT, 105 , IDS_DEL_CURRENT},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
		 {SN_IDC_HELP_BMAP, 30 , IM_SN_F2},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
		 {SN_IDC_HELP_TEXT, 95 , IDS_DEL_ALL},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
		 {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
		 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
		 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
		 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
		 {SN_IDC_HELP_TEXT, 65 , IDS_EXIT}
	};

    list_xstyle_t email_item_rstyle =
    {
      RSI_SN_BG,
   	  RSI_SN_BG,
      RSI_SN_BAR_YELLOW_HL,
   	  RSI_SN_BG,
   	  RSI_SN_BAR_YELLOW_HL,
    };
    
    list_xstyle_t emial_plist_field_fstyle =
    {
      FSI_LIST_TXT_G,
      FSI_LIST_TXT_N,
      FSI_LIST_TXT_HL,
      FSI_LIST_TXT_SEL,
      FSI_LIST_TXT_HL,
    };
    list_xstyle_t email_plist_field_rstyle =
    {
      RSI_IGNORE,
      RSI_IGNORE,
      RSI_IGNORE,
      RSI_IGNORE,
      RSI_IGNORE,
    };

    list_field_attr_t email_plist_attr[EMAIL_LIST_FIELD] =
    {
      { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
          40, 10, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

      { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
          230, 60, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

      { LISTFIELD_TYPE_ICON ,
          30, 300, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

      { LISTFIELD_TYPE_UNISTR ,
          200, 380, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},
    };
    control_t *p_cont = NULL;
    control_t *p_ctrl = NULL;
    control_t *p_list = NULL;
    u16 i = 0;
	cur_focus = 0;
  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS,
							   0, 0,
							   640, 480,
							   IDS_EMAIL,TRUE);  

 
  //email list title bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_HEAD,
                           EMAIL_HEAD_X, EMAIL_HEAD_Y, EMAIL_HEAD_W,EMAIL_HEAD_H, p_cont, 0);
  
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);
  //title number
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NUM,
                           EMAIL_NUMB_X+5, EMAIL_NUMB_Y, EMAIL_NUMB_W,EMAIL_NUMB_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUMBER);

  //title date
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FHIDE,
                            EMAIL_DATE_X, EMAIL_DATE_Y,
                            EMAIL_DATE_W, EMAIL_DATE_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DATE);

  //title new/old
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FFREQ,
                            EMAIL_NEW_X, EMAIL_NEW_Y,
                            EMAIL_NEW_W, EMAIL_NEW_H,
                            p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NEW);
  //title topic
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FAV,
                           EMAIL_TITLE_X, EMAIL_TITLE_Y, EMAIL_TITLE_W,EMAIL_TITLE_H, p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TITLE);
  //email list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EMAIL_PRO,
                           EMAIL_LIST_X, EMAIL_LIST_Y, EMAIL_LIST_W,EMAIL_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  list_set_mid_rect(p_list, EMAIL_LIST_MIDL, EMAIL_LIST_MIDT,
           EMAIL_LIST_MIDW, EMAIL_LIST_MIDH,EMAIL_LIST_VGAP);
  list_set_item_rstyle(p_list, &email_item_rstyle);
  list_set_count(p_list, 0, EMAIL_LIST_PAGE);

  list_set_field_count(p_list, EMAIL_LIST_FIELD, EMAIL_LIST_PAGE);
  list_set_focus_pos(p_list, cur_focus);
  list_set_update(p_list, email_plist_update, 0);
 
  for (i = 0; i < EMAIL_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(email_plist_attr[i].attr), (u16)(email_plist_attr[i].width),
                        (u16)(email_plist_attr[i].left), (u8)(email_plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, email_plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, email_plist_attr[i].fstyle);
  }
  //email content
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_TEXT,
                           EMAIL_TEXT_X, EMAIL_TEXT_Y, EMAIL_TEXT_W,EMAIL_TEXT_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG, RSI_SN_EMAIL_CONTENT_BG, RSI_DIG_BG);
  text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
  text_set_offset(p_ctrl,5,5);
  text_set_font_style(p_ctrl, FSI_INDEX3,FSI_INDEX3,FSI_INDEX3);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=SN_EMAIL_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  return SUCCESS;
}

static RET_CODE on_email_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;

  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  cur_focus = new_focus;
  //email_number =  p_mail_data_buf->p_mail_head[new_focus].m_id;
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, new_focus ,0);  
  return SUCCESS;
}

static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 new_focus;
   if(p_mail_data_buf->total_email_num == 0)
    {
      return ERR_FAILURE;
    }
  new_focus = list_get_focus_pos(p_list);
  switch(msg)
  {
      case MSG_DELETE_ONE:
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, new_focus ,0);  
        break;
      case MSG_DELETE_ALL:
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ALL, 0 ,0);  
        break;
      default:
        return ERR_FAILURE;
  }
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  return SUCCESS;
}

void on_select_del(void)
{
  g_del_mail = 1;
}

void on_cancel_select_del(void)
{
  g_del_mail = 0;
}

static RET_CODE on_dlg_email_del_one_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(p_mail_data_buf->total_email_num)
  {
    OS_PRINTF("QI LIAN TEST %s, %d\n", __FUNCTION__, __LINE__);
    ui_comm_ask_for_dodlg_open(NULL, IDS_DEL_CURRENT_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    
    if(g_del_mail == 1)
    {
      on_email_del_mail(p_list,msg,para1,para2);
	  if(cur_focus > 0)
		cur_focus--;
    }
  }
  return SUCCESS;
}

static RET_CODE on_dlg_email_del_all_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(p_mail_data_buf->total_email_num)
  {
	  OS_PRINTF("QI LIAN TEST %s, %d\n", __FUNCTION__, __LINE__);
    ui_comm_ask_for_dodlg_open(NULL, IDS_DEL_ALL_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    if(g_del_mail == 1)
      on_email_del_mail(p_list,msg,para1,para2);
  }
  return SUCCESS;
}

static RET_CODE on_email_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{

  manage_close_menu(ROOT_ID_EMAIL_MESS,0,0);
  if((sys_status_get_curn_group_curn_prog_id() == INVALIDID) && (fw_find_root_by_id(SN_ROOT_ID_MAINMENU) == NULL))
  {
      manage_open_menu(SN_ROOT_ID_MAINMENU, 0, 0);
      return SUCCESS;
  }
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET,0,0);
  return SUCCESS;
  
}

static RET_CODE on_email_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{

  ui_close_all_mennus();
  if((sys_status_get_curn_group_curn_prog_id() == INVALIDID) && (fw_find_root_by_id(SN_ROOT_ID_MAINMENU) == NULL))
  {
      manage_open_menu(SN_ROOT_ID_MAINMENU, 0, 0);
      return SUCCESS;
  }
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET,0,0);
  return SUCCESS;
  
}

BEGIN_KEYMAP(email_plist_list_keymap, NULL)
  ON_EVENT(V_KEY_F1, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_RED, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_F2, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_YELLOW, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_MAIL,MSG_EXIT)
  ON_EVENT(V_KEY_MENU,MSG_EXIT)
  ON_EVENT(V_KEY_BACK,MSG_EXIT)
  ON_EVENT(V_KEY_EXIT,MSG_EXIT_ALL)
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, plist_update_body)
  ON_COMMAND(MSG_DELETE_ONE, on_dlg_email_del_one_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_dlg_email_del_all_mail)
  ON_COMMAND(MSG_EXIT, on_email_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_email_exit_all)
END_MSGPROC(email_plist_list_proc, list_class_proc)

