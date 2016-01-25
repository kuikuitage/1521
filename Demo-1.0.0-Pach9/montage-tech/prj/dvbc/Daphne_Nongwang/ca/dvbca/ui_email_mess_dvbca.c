
#include "ui_common.h"
#include "ui_email_mess.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_EMAIL_CONT,
  IDC_EMAIL_HEAD,
  IDC_EMAIL_NUM,
  IDC_EMAIL_HIDE,
  IDC_EMAIL_NAME,
  IDC_EMAIL_SENDER,
  IDC_EMAIL_TIME,
  IDC_EMAIL_FDELETE,
  IDC_EMAIL_FAV,
  IDC_EMAIL_FFAV,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_LEFT_ARROW,
  IDC_EMAIL_RIGHT_ARROW,
  #if 0
  IDC_EMAIL_RECEIVED_HEAD,
  IDC_EMAIL_RECEIVED,
  IDC_EMAIL_RESET_HEAD,
  IDC_EMAIL_RESET,
  IDC_EMAIL_MAX_NUM_HEAD,
  IDC_EMAIL_MAX_NUM,
  #endif
};

enum nvod_email_mess_local_msg
{
  MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
  MSG_DELETE_ALL,
};

enum prv_msg_t
{
  MSG_DEL_SET = MSG_USER_BEGIN + 200,
  MSG_DEL_ALL,
  MSG_RETURN
};

enum del_status_t
{
  STATUS_DEL_NULL = 0,
  STATUS_DEL_FALSE,
  STATUS_DEL_TRUE,
};
#ifdef WIN32

static cas_mail_headers_t test_cas_mail_head = 
{
      {
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xA3, 0xD7, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4, 0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4, 0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      },
      50,
      9,
      18,
      8,
};
static cas_mail_body_t test_cas_mail_body =
{
    {0x30,0x31,0x32,0x33,0x34}, 
    5,
    0, 
    1, 
    {0x56,0x59},
    0,
};
#endif

static list_xstyle_t email_item_rstyle =
  {
      RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,
      RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,
      RSI_SN_BAR_YELLOW_HL,
      RSI_SN_BG,
      RSI_SN_BAR_YELLOW_HL,
  };


  static list_xstyle_t emial_plist_field_fstyle =
  {
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_TXT_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
  };

  static list_xstyle_t email_plist_field_rstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };

  static list_field_attr_t email_plist_attr[EMAIL_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_DEC | STL_LEFT| STL_VCENTER,
    (EMAIL_NUMB_W - 16), 16, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

    { LISTFIELD_TYPE_ICON | STL_LEFT | STL_VCENTER,
    EMAIL_NEW_W, (EMAIL_NUMB_W +75), 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

    { LISTFIELD_TYPE_UNISTR | STL_LEFT| STL_VCENTER,
    EMAIL_NAME_W, (EMAIL_NUMB_W + EMAIL_NEW_W+160), 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

    { LISTFIELD_TYPE_UNISTR | STL_LEFT| STL_VCENTER,
    EMAIL_SENDER_W-40, EMAIL_NUMB_W + EMAIL_NEW_W+EMAIL_NAME_W+217, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },
    
//	{ LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
//	EMAIL_TIME_W+20, (EMAIL_NUMB_W + EMAIL_NEW_W+EMAIL_SENDER_W+160), 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },
  };


static u32 del_flag[EMAIL_MAX_NUMB];
static BOOL first_into_list = FALSE;
static u16 cur_focus = 0;
static u8 g_del_mail = 0; 

static cas_mail_headers_t *p_mail_data_buf = NULL;

u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#if 0
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
#endif

extern void ui_ca_get_info(u32 cmd_id, u32 para1, u32 para2);

static RET_CODE email_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(ctrl);
  u16 uni_str[64];
//  u8 asc_str[64];
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      list_set_field_content_by_dec(ctrl, i + start, 0, start + i + 1); 

      /* Is new */
      if(p_mail_data_buf->p_mail_head[i + start].new_email)
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 1, IM_SN_ICON_MAIL_NEW);
      }
      else
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 1, IM_SN_ICON_MAIL_READED);
      }

      /* NAME */
      gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].subject, 80, 
                      uni_str, 40);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, uni_str);
      
      //sender
      gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].sender, 80, 
                      uni_str, 40);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 3, (u16 *)uni_str);

/*	  //time
	  sprintf((char *)asc_str,"%.2x%.2x-%.2x-%.2x",
		      p_mail_data_buf->p_mail_head[i + start].creat_date[0],
		      p_mail_data_buf->p_mail_head[i + start].creat_date[1],
		      p_mail_data_buf->p_mail_head[i + start].creat_date[2],
		      p_mail_data_buf->p_mail_head[i + start].creat_date[3]);
  	  list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 4, asc_str); 
*/   }
  }
  return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  //u32 email_No = 0;
  #if 0
  control_t *p_cnt = NULL,*p_unread = NULL, *p_max = NULL;
  u8 asc_str[8] = {0};
  #endif
  u16 focus = 0;
  p_mail_data_buf = (cas_mail_headers_t*)para2;
  #ifdef WIN32
  p_mail_data_buf = &test_cas_mail_head;
  #endif
  #if 0
  p_cnt = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_RECEIVED);
  sprintf((char*)asc_str,"%d",p_mail_data_buf->max_num);
  text_set_content_by_ascstr(p_cnt, (u8*)asc_str);

  p_unread = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_RESET);
  sprintf((char*)asc_str,"%d",p_mail_data_buf->new_mail_num);
  text_set_content_by_ascstr(p_unread, (u8*)asc_str);

  p_max = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_MAX_NUM);
  sprintf((char*)asc_str,"%d",p_mail_data_buf->max_display_email_num);
  text_set_content_by_ascstr(p_max, (u8*)asc_str);
  #endif
  list_set_count(p_ctrl, p_mail_data_buf->max_num, EMAIL_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, cur_focus);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), EMAIL_LIST_PAGE, 0);
  focus = list_get_valid_pos(p_ctrl);
  

  OS_PRINTF("-------email  plist_update_email!!!!!!\n");
  OS_PRINTF("-------email  max_num %d\n",p_mail_data_buf->max_num);
  OS_PRINTF("-------email  new_mail_num %d\n",p_mail_data_buf->new_mail_num);
  OS_PRINTF("-------email  m_id %d\n",p_mail_data_buf->p_mail_head[focus].m_id);
  OS_PRINTF("-------email  sender %s\n",p_mail_data_buf->p_mail_head[focus].sender);
  OS_PRINTF("-------email  subject %s\n",p_mail_data_buf->p_mail_head[focus].subject);
  OS_PRINTF("-------email  new_email %d\n",p_mail_data_buf->p_mail_head[focus].new_email);
  OS_PRINTF("-------email  max_display_email_num %d\n",p_mail_data_buf->max_display_email_num);
  OS_PRINTF("-------email  max_num %d\n",p_mail_data_buf->max_num);
  OS_PRINTF("-------email  max_display_email_num %d\n",p_mail_data_buf->max_display_email_num);
  
  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
 return SUCCESS;

}

#if 0
static RET_CODE plist_update_body(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 new_focus;
  control_t *pCtrl_text = NULL;
  control_t *pCtrl_body = NULL;
  cas_mail_body_t *p_mail_body_data = NULL;
  u16 uni_str[256] = {0};

  p_mail_body_data = (cas_mail_body_t *)para2;
  if(NULL == p_mail_body_data)
  {
      return ERR_FAILURE;
  }
  new_focus = list_get_focus_pos(p_ctrl);
  p_mail_data_buf->p_mail_head[new_focus].new_email = 0;

  pCtrl_text = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_BODY_CONT);
  pCtrl_body = ctrl_get_child_by_id(pCtrl_text, IDC_EMAIL_TEXT);
  //uni_strlen(const u16 * p_str);
  OS_PRINTF("\r\n ***plist_update_body %s ****\r\n" ,p_mail_body_data->data);
  if((0 == p_mail_body_data->data[0]) && (0 == p_mail_body_data->data[1]))
  {
      OS_PRINTF("\r\n ***the email body data is NULL ****\r\n");
      text_set_content_by_unistr(pCtrl_body,uni_str);
      ctrl_paint_ctrl(pCtrl_body, TRUE);
      return ERR_FAILURE;
  }
  gb2312_to_unicode(p_mail_body_data->data, 256,
                uni_str, 256);
  text_set_content_by_unistr(pCtrl_body,uni_str);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), EMAIL_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(pCtrl_text,TRUE);
  ctrl_paint_ctrl(pCtrl_body, TRUE);
  return SUCCESS;
}
#endif
RET_CODE open_email_mess(u32 para1, u32 para2)
{
#define SN_SCREEN_WIDTH		   640
#define SN_SCREEN_HEIGHT       480
#define SN_EMAIL_HELP_CNT      16
//head
#define SN_EMAIL_HEAD_X        20
#define SN_EMAIL_HEAD_Y        10
#define SN_EMAIL_HEAD_W        600
#define SN_EMAIL_HEAD_H        31

  //number
#define SN_EMAIL_NUMB_X        (SN_EMAIL_HEAD_X + 10)
#define SN_EMAIL_NUMB_Y        SN_EMAIL_HEAD_Y
#define SN_EMAIL_NUMB_W        60
#define SN_EMAIL_NUMB_H        SN_EMAIL_HEAD_H

  //mail list rect
#define SN_EMAIL_LIST_MIDL         0
#define SN_EMAIL_LIST_MIDT         0
#define SN_EMAIL_LIST_MIDW        600
#define SN_EMAIL_LIST_MIDH        EMAIL_LIST_H
#define SN_EMAIL_LIST_VGAP         8

#define SN_SUB_MENU_HELP_RSC_CNT		   15

  //email bar
#define SN_EMAIL_BAR_X				  20
#define SN_EMAIL_BAR_Y				  360
#define SN_EMAIL_BAR_W				  SN_EMAIL_HEAD_W
#define SN_EMAIL_BAR_H				  31

  //email arrow
#define SN_EMAIL_LEFT_ARROW_X			  295
#define SN_EMAIL_RIGHT_ARROW_X			  SN_EMAIL_LEFT_ARROW_X+SN_EMAIL_ARROW_W+6
#define SN_EMAIL_ARROW_Y			  10
#define SN_EMAIL_ARROW_W			  12
#define SN_EMAIL_ARROW_H			  11



  control_t *p_ctrl, *p_list, *p_menu, *p_sbar,*p_cont;
//  control_t *p_item = NULL;
  u16 i;
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
  cur_focus = 0;
  p_menu = ui_background_create(ROOT_ID_EMAIL_MESS,
                           0, 0,
                           SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                           IDS_MEMAIL,TRUE);
  ctrl_set_rstyle(p_menu, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  
  //head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_HEAD,
                           SN_EMAIL_HEAD_X, SN_EMAIL_HEAD_Y, SN_EMAIL_HEAD_W,SN_EMAIL_HEAD_H, p_menu, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);
  
  //number
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_NUM,
                           SN_EMAIL_NUMB_X, SN_EMAIL_NUMB_Y, SN_EMAIL_NUMB_W, SN_EMAIL_NUMB_H, p_menu, 0);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUMBER);
   
  //state
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_FDELETE,
                            EMAIL_NEW_X+40, SN_EMAIL_HEAD_Y,
                            EMAIL_NEW_W+30, EMAIL_NEW_H,
                            p_menu, 0);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_STATE);
 
  //title
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_NAME,
                            EMAIL_NAME_X+90, SN_EMAIL_HEAD_Y,
                            EMAIL_NAME_W, EMAIL_NAME_H,
                            p_menu, 0);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TITLE);
  
  //sender
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_SENDER,
                           EMAIL_SENDER_X+90, SN_EMAIL_HEAD_Y, EMAIL_SENDER_W,EMAIL_SENDER_H, p_menu, 0);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_SENDER);

 /* //time
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_TIME,
                           EMAIL_SENDER_X+180, SN_EMAIL_HEAD_Y, 100,EMAIL_SENDER_H, p_menu, 0);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DATE_TIME);
  */
  
  //LIST
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_EMAIL_PRO,
                           SN_EMAIL_HEAD_X, SN_EMAIL_HEAD_Y+SN_EMAIL_HEAD_H, SN_EMAIL_HEAD_W,EMAIL_LIST_H, p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  list_set_mid_rect(p_list, SN_EMAIL_LIST_MIDL, SN_EMAIL_LIST_MIDT,
           SN_EMAIL_LIST_MIDW, SN_EMAIL_LIST_MIDH,SN_EMAIL_LIST_VGAP);
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

    //email bar
  p_sbar = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EMAIL_BAR,
                               SN_EMAIL_BAR_X, SN_EMAIL_BAR_Y-20,
                               SN_EMAIL_BAR_W, SN_EMAIL_BAR_H,
                               p_menu, 0);
  ctrl_set_rstyle(p_sbar, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);

  //email arrow
  p_cont = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EMAIL_LEFT_ARROW,
                               SN_EMAIL_LEFT_ARROW_X, SN_EMAIL_ARROW_Y,
                               SN_EMAIL_ARROW_W, SN_EMAIL_ARROW_H,
                               p_sbar, 0);
  bmap_set_content_by_id(p_cont, IM_SN_ICON_ARROW_UP);
  p_cont = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_EMAIL_RIGHT_ARROW,
                               SN_EMAIL_RIGHT_ARROW_X, SN_EMAIL_ARROW_Y,
                               SN_EMAIL_ARROW_W, SN_EMAIL_ARROW_H,
                               p_sbar, 0);
  bmap_set_content_by_id(p_cont, IM_SN_ICON_ARROW_DOWN);
  
#if 0
  //received head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RECEIVED_HEAD,
                           EMAIL_RECEIVED_HEAD_X, EMAIL_RECEIVED_HEAD_Y,
                           EMAIL_RECEIVED_HEAD_W,EMAIL_RECEIVED_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_MAIL_COUNT);

  //received space
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RECEIVED,
                           EMAIL_RECEIVED_X, EMAIL_RECEIVED_Y,
                           EMAIL_RECEIVED_W,EMAIL_RECEIVED_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //un read head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RESET_HEAD,
                           EMAIL_RESET_HEAD_X, EMAIL_RESET_HEAD_Y,
                           EMAIL_RESET_HEAD_W, EMAIL_RESET_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_UNREAD_MAIL_CNT);

  //rest space
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RESET,
                           EMAIL_RESET_X, EMAIL_RESET_Y,
                           EMAIL_RESET_W, EMAIL_RESET_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);


  //max show num string
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_MAX_NUM_HEAD,
                           EMAIL_MAX_NUM_HEAD_X, EMAIL_MAX_NUM_HEAD_Y,
                           EMAIL_MAX_NUM_HEAD_W,EMAIL_MAX_NUM_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_SHOW_MAX_MAIL_CNT);

  //max num show space
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_MAX_NUM,
                           EMAIL_MAX_NUM_X, EMAIL_MAX_NUM_Y,
                           EMAIL_MAX_NUM_W, EMAIL_MAX_NUM_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
#endif
  
  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=SN_EMAIL_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_menu)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_menu);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
  for(i = 0; i < EMAIL_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  first_into_list = TRUE;
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0, 0);
  #else
  plist_update_email(p_list, 0, 0, 0);
  #endif
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
  OS_PRINTF("m_id %d\n",p_mail_data_buf->p_mail_head[new_focus].m_id);
  OS_PRINTF("sender %d\n",p_mail_data_buf->p_mail_head[new_focus].sender);
  OS_PRINTF("subject %d\n",p_mail_data_buf->p_mail_head[new_focus].subject);
  
  OS_PRINTF("on_email_list_change_focus : old focus : %d,  new focus : %d\n",old_focus, new_focus);
  ctrl_paint_ctrl(fw_find_root_by_id(ROOT_ID_EMAIL_MESS),TRUE);
  return SUCCESS;
}

static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u16 new_focus;
   if(p_mail_data_buf->max_num == 0)
    {
      return ERR_FAILURE;
    }
  new_focus = list_get_focus_pos(p_list);
  switch(msg)
  {
      case MSG_DELETE_ONE:
	  	  #ifndef WIN32
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[new_focus].m_id ,0);
		
		    #endif
        break;
      case MSG_DELETE_ALL:
	  	  #ifndef WIN32
		do{
          ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[p_mail_data_buf->max_num-1].m_id ,0);
		  p_mail_data_buf->max_num--;
		}while(p_mail_data_buf->max_num);
		    #endif
        break;
      default:
        return ERR_FAILURE;
  }
    #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
	  #endif
  return SUCCESS;
}
extern BOOL ui_is_smart_card_insert(void);

u16 mail_index = 0;
static RET_CODE on_email_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  
  DEBUG(DVBCAS_PORTING,INFO,"1---###################printf the OK in mess_dvbca.c#################\n");
  if(p_mail_data_buf->max_num == 0)
  {
    return SUCCESS;
  }
  
  OS_PRINTF("on_email_list_select : m_id == %d\n", p_mail_data_buf->p_mail_head[mail_index].m_id);
  mail_index = list_get_focus_pos(p_list);
  #ifndef WIN32
  if(ui_is_smart_card_insert() == TRUE)
  	ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, p_mail_data_buf->p_mail_head[mail_index].m_id, 0);
  else
  	ui_comm_cfmdlg_open2(NULL,IDS_CA_CARD_NOT_INSERT, NULL,0);
  #else
  on_ca_mail_preread(p_list, 0, para1, para2);
  #endif
  return SUCCESS;
}

static RET_CODE on_ca_mail_preread(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u32 ret = para1;
  cas_mail_body_t *mail_body = NULL;
  u16 index = 0;
  u16 i;
  index = list_get_focus_pos(p_list);
  
  #ifndef WIN32
  mail_body = (cas_mail_body_t *)para2;
  #else
  mail_body = &test_cas_mail_body;
  manage_open_menu(ROOT_ID_EMAIL_MESS_CONTENT, (u32)&p_mail_data_buf->p_mail_head[index],(u32)mail_body);
  return SUCCESS;
  #endif
  OS_PRINTF("MMMMMMMMMMMp_mail_data_buf->max_num = %d, index = %d\n",p_mail_data_buf->max_num,index);
  
  OS_PRINTF("max_num %d\n",mail_body->data_len);
  OS_PRINTF("mail boy data start!!!!!!\n");
  for(i = 0;i < mail_body->data_len;i ++)
  {
    OS_PRINTF("%s",mail_body->data[i]);
    if(i % 16 == 0)
      OS_PRINTF("\n");
  }
  switch(ret)
  {
  	case CAS_E_MAIL_CAN_NOT_READ://CAS_E_MAIL_CAN_NOT_READ
  	  ui_comm_cfmdlg_open2(NULL,IDS_CA_CARD_EMAIL_ERR,NULL,0);
     OS_PRINTF("ret  = CAS_E_MAIL_CAN_NOT_READ\n");
  	  break;

  	default:
     if((0 != p_mail_data_buf->max_num) && (0xFFFF != p_mail_data_buf->max_num))
     {
       OS_PRINTF("open ROOT_ID_EMAIL_MESS_CONTENT\n");
       if(ui_is_smart_card_insert() == FALSE)
        {
          ui_comm_cfmdlg_open2(NULL,IDS_CA_CARD_NOT_INSERT, NULL,0);
        }
        else
          manage_open_menu(ROOT_ID_EMAIL_MESS_CONTENT, (u32)&p_mail_data_buf->p_mail_head[index],(u32)mail_body);
     }
  	  break;
  }
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
  if(p_mail_data_buf->max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_DEL_CURRENT_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    
  if(g_del_mail == 1){
    on_email_del_mail(p_list,msg,para1,para2);
	if(cur_focus > 0)
		cur_focus--;
  }
  } 
  
  return SUCCESS;
}

static RET_CODE on_dlg_email_del_all_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(p_mail_data_buf->max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_DEL_ALL_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    if(g_del_mail == 1)
      on_email_del_mail(p_list,msg,para1,para2);
  } 
  return SUCCESS;
}

static RET_CODE on_email_change_status(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("change the email status in ca_mail!\n");
    #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0, 0);
	  #endif
  return SUCCESS;
}

BEGIN_KEYMAP(email_plist_list_keymap, NULL)
  #ifdef WIN32
  ON_EVENT(V_KEY_F1, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_F2, MSG_DELETE_ALL)
  #endif
  ON_EVENT(V_KEY_RED, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_YELLOW, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, on_ca_mail_preread)
  ON_COMMAND(MSG_DELETE_ONE, on_dlg_email_del_one_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_dlg_email_del_all_mail)
  ON_COMMAND(MSG_SELECT, on_email_list_select)
  ON_COMMAND(MSG_CA_MAIL_CHANGE_STATUS, on_email_change_status)
END_MSGPROC(email_plist_list_proc, list_class_proc)

