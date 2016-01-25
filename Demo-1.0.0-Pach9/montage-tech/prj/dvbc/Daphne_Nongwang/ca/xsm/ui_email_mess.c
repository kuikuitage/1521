
#include "ui_common.h"
#include "ui_email_mess.h"

static cas_mail_headers_t *p_mail_data_buf = NULL;

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

/*static cas_mail_headers_t test_cas_mail_head = 
{
      {
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {1, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      },
      6,
      6,
      6,
      6
};*/

u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
/*
static comm_help_data_t email_help_data = //help bar data
{
  2,                                            
  2,
  {
    IDS_DEL_CURRENT,
    IDS_DEL_ALL
  },
  { 
    IM_SN_F1,
    IM_SN_F1
  },
};
*/

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
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 2, IM_SN_F1);
      }
      else
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 2, IM_SN_F1);
      }
      
      //date
      #if 1
      s_ca_time.year = p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 100 
                                  + p_mail_data_buf->p_mail_head[i + start].creat_date[1];
      s_ca_time.month =  p_mail_data_buf->p_mail_head[i + start].creat_date[2];
      s_ca_time.day = p_mail_data_buf->p_mail_head[i + start].creat_date[3];
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[4]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.hour = (u8)tmp_value;
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[5]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.minute = (u8)tmp_value;

      time_to_local(&s_ca_time, &s_local_time);
      sprintf((char *)asc_str,"%4d/%02d/%02d %02d:%02d", 
                s_local_time.year, s_local_time.month, 
                s_local_time.day, s_local_time.hour, s_local_time.minute);
      str_asc2uni(asc_str, uni_str);
      #endif
      #if 0
      sprintf((char *)asc_str, "%.2x/%.2x/%.2x %.2x:%.2x",
              p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 100 
                + p_mail_data_buf->p_mail_head[i + start].creat_date[1],
              p_mail_data_buf->p_mail_head[i + start].creat_date[2],
              p_mail_data_buf->p_mail_head[i + start].creat_date[3],
              p_mail_data_buf->p_mail_head[i + start].creat_date[4],
              p_mail_data_buf->p_mail_head[i + start].creat_date[5]);
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
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
  
  list_set_count(p_ctrl, p_mail_data_buf->max_num, EMAIL_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, 0);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);
  email_No = list_get_focus_pos(p_ctrl);
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, (email_No+1) ,0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
 return SUCCESS;

}

static RET_CODE plist_update_body(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 new_focus;
  control_t *pCtrl_body = NULL;
  control_t *pCtrl_text = NULL;
  cas_mail_body_t *p_mail_body_data = NULL;
  u16 uni_str[256] = {0};

  p_mail_body_data = (cas_mail_body_t *)para2;
  if(NULL == p_mail_body_data)
  {
    text_set_content_by_unistr(pCtrl_text,uni_str);
    ctrl_paint_ctrl(pCtrl_body, TRUE);
    return ERR_FAILURE;
  }
  new_focus = list_get_focus_pos(p_ctrl);
  p_mail_data_buf->p_mail_head[new_focus].new_email = 0;

  pCtrl_body = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_BODY_CONT);
  pCtrl_text = ctrl_get_child_by_id(pCtrl_body, IDC_EMAIL_TEXT);
  //uni_strlen(const u16 * p_str);
  OS_PRINTF("\r\n ***plist_update_body %s ****\r\n" ,p_mail_body_data->data);
  if((0 == p_mail_body_data->data[0]) && (0 == p_mail_body_data->data[1]))
  {
      OS_PRINTF("\r\n ***the email body data is NULL ****\r\n");
      text_set_content_by_unistr(pCtrl_text,uni_str);
      ctrl_paint_ctrl(pCtrl_body, TRUE);
      return ERR_FAILURE;
  }
  gb2312_to_unicode(p_mail_body_data->data, 256,
                uni_str, 256);
  text_set_content_by_unistr(pCtrl_text,uni_str);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), EMAIL_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(pCtrl_body, TRUE);
  return SUCCESS;
}

RET_CODE open_email_mess(u32 para1, u32 para2)
{
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

    list_xstyle_t email_item_rstyle =
    {
      RSI_SN_BG,
      RSI_SN_BG,
      RSI_SN_BG,
      RSI_SN_BG,
      RSI_SN_BG,
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
    control_t *p_sbar = NULL;
    control_t *p_list = NULL;
    control_t *p_ctrl_cont = NULL;
    u16 i = 0;
//  full_screen_title_t title_data = {IM_SN_F1,IDS_EMAIL, RSI_SN_BG};
/*
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_EMAIL_MESS,
                             RSI_SN_BG,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             &title_data);
  */
  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS,
                           0, 0,
                           SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                           IDS_MEMAIL,TRUE);
  ctrl_set_rstyle(p_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  
  //email list title bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_HEAD,
                           EMAIL_HEAD_X, EMAIL_HEAD_Y, EMAIL_HEAD_W,EMAIL_HEAD_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  //title number
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NUM,
                           EMAIL_NUMB_X, EMAIL_NUMB_Y, EMAIL_NUMB_Y,EMAIL_NUMB_H, p_cont, 0);
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
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  list_set_mid_rect(p_list, EMAIL_LIST_MIDL, EMAIL_LIST_MIDT,
           EMAIL_LIST_MIDW, EMAIL_LIST_MIDH,EMAIL_LIST_VGAP);
  list_set_item_rstyle(p_list, &email_item_rstyle);
  list_set_count(p_list, 0, EMAIL_LIST_PAGE);

  list_set_field_count(p_list, EMAIL_LIST_FIELD, EMAIL_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, email_plist_update, 0);
 
  for (i = 0; i < EMAIL_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(email_plist_attr[i].attr), (u16)(email_plist_attr[i].width),
                        (u16)(email_plist_attr[i].left), (u8)(email_plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, email_plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, email_plist_attr[i].fstyle);
  }
  
  //list scroll bar
  p_sbar = ctrl_create_ctrl((u8 *)CTRL_SBAR, IDC_EMAIL_BAR, 
                              EMAIL_SBAR_X, EMAIL_SBAR_Y, EMAIL_SBAR_W, EMAIL_SBAR_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  sbar_set_mid_rect(p_sbar, 0, 16, EMAIL_SBAR_W,  EMAIL_SBAR_H - 16);
  list_set_scrollbar(p_list, p_sbar);
  //email_plist_update(p_list, list_get_valid_pos(p_list), EMAIL_LIST_PAGE, 0);
  
  //email content head bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NOTIC,
                           EMAIL_CONTENT_BAR_X, EMAIL_CONTENT_BAR_Y, 
                           EMAIL_CONTENT_BAR_W, EMAIL_CONTENT_BAR_H, 
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  //email content
  p_ctrl_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_EMAIL_BODY_CONT,
                           EMAIL_TEXT_X, EMAIL_TEXT_Y, EMAIL_TEXT_W,EMAIL_TEXT_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_TEXT,
                           10, 0, EMAIL_TEXT_W - 20,EMAIL_TEXT_H, p_ctrl_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //ui_comm_help_create(&email_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

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
  //email_number =  p_mail_data_buf->p_mail_head[new_focus].m_id;
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, (new_focus + 1) ,0);  
  return SUCCESS;
}

static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 new_focus;
  if(p_mail_data_buf->max_num == 0)
    {
      return ERR_FAILURE;
    }
  new_focus = list_get_focus_pos(p_list);
  switch(msg)
  {
      case MSG_DELETE_ONE:
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf ->p_mail_head[new_focus].m_id,0);  
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

static RET_CODE on_email_body_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  
  return ERR_NOFEATURE;
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
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, plist_update_body)
  ON_COMMAND(MSG_DELETE_ONE, on_email_del_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_email_del_mail)
  ON_COMMAND(MSG_EXIT, on_email_body_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_email_body_exit)
END_MSGPROC(email_plist_list_proc, list_class_proc)

