
#include "ui_common.h"
#include "ui_email_mess.h"
#include "sys_dbg.h"
static cas_mail_headers_t *p_mail_data_buf = NULL;
static u16 g_uni_str[1024] = {0};
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
  IDC_EMAIL_IMPORTANT,
  IDC_EMAIL_FAV,
  IDC_EMAIL_FFAV,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_NOTIC,
  IDC_EMAIL_BODY_CONT,
  IDC_EMAIL_TEXT_BAR,
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
u16 email_body_text_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE email_body_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
/*QQ
static comm_help_data_t email_help_data = //help bar data
{
  4,                                            
  4,
  {
    IDS_SELECT,
    IDS_SELECTED,
    IDS_DEL_CURRENT,
    IDS_DEL_ALL
  },
  { 
    IM_ICON_ARROW_UP_DOWN,
    IM_ICON_SELECT,
    IM_F1,
    IM_F2
  },
};
*/

#ifdef WIN32
static cas_mail_headers_t test_cas_mail_head = 
{
    {
      {0, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4,0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4,0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4,0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
      {1, 0, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
      {2, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
      {3, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
      {4, 1, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
      {5, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      {0, 1, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
      {1, 0, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
      {2, 1, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
      {3, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
      {4, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
      {5, 0, 0, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      {0, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
      {1, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
      {2, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
      {3, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
      {4, 1, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
      {5, 0, 1, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
    },
    100,
    9,
    18,
    8,
};
#endif

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
  //utc_time_t  s_ca_time = {0};
  //utc_time_t  s_local_time = {0};
  //u32 tmp_value = 0;
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%d ", i + start + 1);   //p_mail_data_buf->p_mail_head[i + start].m_id);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 
      /* Is new */
      if(p_mail_data_buf->p_mail_head[i + start].new_email)
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 1, IM_SN_ICON_MAIL_NEW);//QQ  IM_ICON_MAIL_UNREAD
      }
      else
      {
          list_set_field_content_by_icon(ctrl, (u16)(start + i), 1, IM_SN_ICON_MAIL_READED);//QQ IM_ICON_MAIL_READ
      }

      /* NAME */
      gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].subject, 80, 
                      uni_str, 40);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, (u16 *)uni_str);

      
      //date
      sprintf((char *)asc_str, "%04d/%02d/%02d",
              p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 100 
                + p_mail_data_buf->p_mail_head[i + start].creat_date[1],
              p_mail_data_buf->p_mail_head[i + start].creat_date[2],
              p_mail_data_buf->p_mail_head[i + start].creat_date[3]
             // p_mail_data_buf->p_mail_head[i + start].creat_date[4],
            //  p_mail_data_buf->p_mail_head[i + start].creat_date[5],
             // p_mail_data_buf->p_mail_head[i + start].creat_date[6]
             );
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);

      //important
      if(p_mail_data_buf->p_mail_head[i + start].priority)
      {
        list_set_field_content_by_strid(ctrl, (u16)(start + i), 4, IDS_YES);        
      }
      else
      {
        list_set_field_content_by_strid(ctrl, (u16)(start + i), 4, IDS_NO);        
      }
    }
  }
  return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u32 email_No = 0;
  u16 size = EMAIL_LIST_PAGE;
  #ifndef WIN32
  p_mail_data_buf = (cas_mail_headers_t*)para2;
  #else
  p_mail_data_buf = &test_cas_mail_head;
  #endif

  list_set_count(p_ctrl, p_mail_data_buf->max_num, EMAIL_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, 0);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);
  email_No =  p_mail_data_buf->p_mail_head[list_get_focus_pos(p_ctrl)].m_id;
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_No ,0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
 return SUCCESS;

}

static RET_CODE plist_update_body(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 new_focus;
  control_t *pCtrl_body = NULL;
  cas_mail_body_t *p_mail_body_data = NULL;
  p_mail_body_data = (cas_mail_body_t *)para2;
 
  if(NULL == p_mail_body_data)
  {
    return ERR_FAILURE;
  }
     
  pCtrl_body = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_EMAIL_TEXT);
  new_focus = list_get_focus_pos(p_ctrl);
  p_mail_data_buf->p_mail_head[new_focus].new_email = 0;
 
  OS_PRINTF("\r\n ***plist_update_body %s ****\r\n" ,p_mail_body_data->data);
  
  if((0 == p_mail_body_data->data[0]) && (0 == p_mail_body_data->data[1]))
  {
    OS_PRINTF("\r\n ***the email body data is NULL ****\r\n");
    memset(g_uni_str, 0, 1024);
    text_set_content_by_unistr(pCtrl_body,g_uni_str);
    ctrl_paint_ctrl(pCtrl_body, TRUE);
    return ERR_FAILURE;
  }
  
  gb2312_to_unicode(p_mail_body_data->data, 1023, g_uni_str, 1023);
  text_set_content_by_extstr(pCtrl_body, g_uni_str);
  text_reset_param(pCtrl_body);
  
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), EMAIL_LIST_PAGE, 0);
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(pCtrl_body, TRUE);
  
  return SUCCESS;
}

RET_CODE open_email_mess(u32 para1, u32 para2)
{

 #define SN_EMAIL_HELP_CNT      16
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


    customer_cfg_t cfg = {0};
    list_xstyle_t email_item_rstyle =
    {
	/*QQ      RSI_COMM_LIST_N,
		      RSI_COMM_LIST_N,
		      RSI_COMM_LIST_HL,
		      RSI_COMM_LIST_SEL,
		      RSI_COMM_LIST_N,
	*/
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
          40, 0, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

      { LISTFIELD_TYPE_ICON | STL_CENTER| STL_VCENTER,
          30, 40, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },
          
      { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
          285, 70, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

 
      { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER ,
          205, 355, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

      { LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
          30, 530, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},
    }; 
    control_t *p_cont = NULL;
    control_t *p_ctrl = NULL;
    control_t *p_sbar = NULL;
    control_t *p_list = NULL;
    //control_t *p_ctrl_cont = NULL;
    u16 i = 0;
	DEBUG(MAIN,INFO, "**********************\n");

  //QQ  full_screen_title_t title_data = {IM_SN_BGIM_COMMON_BANNER_MESSAGE,IDS_EMAIL, RSI_TITLE_BG};
  //full_screen_title_t title_data = {IM_SN_BG,IDS_EMAIL, RSI_ORANGE};
  get_customer_config(&cfg);
/*QQ
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_EMAIL_MESS, 
                             RSI_FULL_SCREEN_BG,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             &title_data);
*/
  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS, 
						 0, 0,
						 SN_SCREEN_WIDTH, SN_SCREEN_EMAIL_HEIGHT,
						 IDS_MEMAIL,TRUE);

	DEBUG(MAIN,INFO, "**********************\n");
  ctrl_set_rstyle(p_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
 // ctrl_paint_ctrl(p_cont, FALSE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  //email list title bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_HEAD,
                           EMAIL_HEAD_X, EMAIL_HEAD_Y, EMAIL_HEAD_W,EMAIL_HEAD_H, p_cont, 0);
  //QQ  ctrl_set_rstyle(p_ctrl, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  //title number
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NUM,
                           EMAIL_NUMB_X, EMAIL_NUMB_Y, EMAIL_NUMB_Y,EMAIL_NUMB_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUMBER);

  //title date
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FHIDE,
                            EMAIL_DATE_X, EMAIL_DATE_Y,
                            EMAIL_DATE_W, EMAIL_DATE_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DATE);

  //title new/old
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FFREQ,
                            SN_EMAIL_NEW_X, EMAIL_NEW_Y,
                            EMAIL_NEW_W, EMAIL_NEW_H,
                            p_cont, 0);
  text_set_align_type(p_ctrl, STL_RIGHT|STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NEW);
  //title topic
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FAV,
                           SN_EMAIL_TITLE_X, EMAIL_TITLE_Y, EMAIL_TITLE_W,EMAIL_TITLE_H, p_cont, 0);
  text_set_align_type(p_ctrl, STL_CENTER |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TITLE);
  
  //title important/old
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_IMPORTANT,
                            EMAIL_IMPORT_X, EMAIL_IMPORT_Y,
                            EMAIL_IMPORT_W, EMAIL_IMPORT_H,
                            p_cont, 0);
  text_set_align_type(p_ctrl, STL_RIGHT|STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IMPORTANT);
  
  //email list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EMAIL_PRO,
                           EMAIL_LIST_X, EMAIL_LIST_Y, EMAIL_LIST_W,EMAIL_LIST_H+40, p_cont, 0);
  //QQ  ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  list_set_mid_rect(p_list, EMAIL_LIST_MIDL, EMAIL_LIST_MIDT,
           EMAIL_LIST_MIDW, EMAIL_LIST_MIDH+15,EMAIL_LIST_VGAP);
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
 //QQ  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
 ctrl_set_rstyle(p_sbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  //QQ  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  sbar_set_mid_rstyle(p_sbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  sbar_set_mid_rect(p_sbar, 0, 16, EMAIL_SBAR_W,  EMAIL_SBAR_H - 16);
  list_set_scrollbar(p_list, p_sbar);
  //email_plist_update(p_list, list_get_valid_pos(p_list), EMAIL_LIST_PAGE, 0);
 /* 
  //email content head bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NOTIC,
                           EMAIL_CONTENT_BAR_X, EMAIL_CONTENT_BAR_Y, 
                           EMAIL_CONTENT_BAR_W, EMAIL_CONTENT_BAR_H, 
                           p_cont, 0);
  //QQ  ctrl_set_rstyle(p_ctrl, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);*/
  //email content
  #if 0
  p_ctrl_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_EMAIL_BODY_CONT,
                           EMAIL_TEXT_X, EMAIL_TEXT_Y, EMAIL_TEXT_W,EMAIL_TEXT_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl_cont, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
#endif
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_TEXT,
                           EMAIL_TEXT_X, EMAIL_TEXT_Y+60, 
                           EMAIL_TEXT_W ,EMAIL_TEXT_H, 
                           p_cont, 0);
  ctrl_set_keymap(p_ctrl, email_body_text_keymap); 
  ctrl_set_proc(p_ctrl, email_body_text_proc);
  //QQ  ctrl_set_rstyle(p_ctrl, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_ORANGE, RSI_SN_BG);
  text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
  text_enable_page(p_ctrl, TRUE);
  
  //list scroll bar
  p_sbar = ctrl_create_ctrl((u8 *)CTRL_SBAR, IDC_EMAIL_TEXT_BAR, 
                              EMAIL_TEXT_BAR_X, EMAIL_TEXT_BAR_Y,
                              EMAIL_TEXT_BAR_W, EMAIL_TEXT_BAR_H, 
                              p_cont, 0);
  //QQ  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  ctrl_set_rstyle(p_sbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  //QQ  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  sbar_set_mid_rstyle(p_sbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  sbar_set_mid_rect(p_sbar, 0, 16, EMAIL_TEXT_BAR_W,  EMAIL_TEXT_BAR_H - 16);
  text_set_scrollbar(p_ctrl, p_sbar);
  
  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=SN_EMAIL_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_cont);
  
 //QQ  ui_comm_help_create(&email_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);
  #ifdef WIN32
    plist_update_email(p_list,0,0,0);
  #endif
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  return SUCCESS;
}

static RET_CODE on_email_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;
  u32 email_number = 0;
  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  email_number =  p_mail_data_buf->p_mail_head[new_focus].m_id;
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_number,0);  
  return SUCCESS;
}

static RET_CODE on_email_ctrl_change_focus(control_t *p_trl, u16 msg, u32 para1, u32 para2)  
{  
  u8 act_id;
  control_t *p_cont = NULL;
  control_t *p_new_act_ctrl = NULL;
  u32 email_No = 0;

  p_cont = ctrl_get_parent(p_trl);
  act_id = ctrl_get_ctrl_id(p_trl);

  switch(act_id)
  {
  case IDC_EMAIL_PRO:
    p_new_act_ctrl = ctrl_get_child_by_id(p_cont, IDC_EMAIL_TEXT);
  //QQ   ui_comm_help_set_data(&email_help_data2, p_cont);
    break;

  case IDC_EMAIL_TEXT:
    p_new_act_ctrl = ctrl_get_child_by_id(p_cont, IDC_EMAIL_PRO);
    email_plist_update(p_new_act_ctrl, list_get_valid_pos(p_new_act_ctrl), EMAIL_LIST_PAGE, 0);
    email_No = p_mail_data_buf->p_mail_head[list_get_focus_pos(p_new_act_ctrl)].m_id;
    ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_No ,0);
//QQ     ui_comm_help_set_data(&email_help_data, p_cont);
    break;
  default:
    break;
  }
  
  if(NULL == p_new_act_ctrl)
  {
      return ERR_FAILURE;
  }
  ctrl_process_msg(p_trl, MSG_LOSTFOCUS, 0, 0);
  ctrl_set_attr(p_new_act_ctrl, OBJ_ATTR_ACTIVE);
  ctrl_process_msg(p_new_act_ctrl, MSG_GETFOCUS, 0, 0);

 //QQ  ui_comm_help_update(p_cont);
  ctrl_paint_ctrl(p_trl, TRUE);
  ctrl_paint_ctrl(p_new_act_ctrl, TRUE);
  return SUCCESS;
  
}
extern u16 get_message_strid();
static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 new_focus;
  control_t *p_ctrl = NULL;
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);

  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("--------on_email_del_mail   :focus :%d\n",new_focus);
  switch(msg)
  {
      case MSG_DELETE_ONE:
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[new_focus].m_id,0);  
        break;
      case MSG_DELETE_ALL:
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ALL, 0 ,0);  
        break;
      default:
        return ERR_FAILURE;
  }
  if((get_message_strid() == IDS_CA_EMAIL_NO_SPACE) && (NULL != p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
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
  if(p_mail_data_buf->max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_DEL_CURRENT_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    
  if(g_del_mail == 1)
    on_email_del_mail(p_list,msg,para1,para2);
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

static RET_CODE on_ca_mail_del_result_accept(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  OS_PRINTF("--------on_ca_mail_del_result_accept   :para1 :%d\n",para1);
  if(para1 == 1)
   //QQ  ui_comm_cfmdlg_open2(NULL,IDS_CA_MAIL_DEL_SUCCESS,NULL,0);
   ui_comm_cfmdlg_open2(NULL,IDS_CA_ADDRES_INFO,NULL,0);
  else if(para1 == 2)
   //QQ  ui_comm_cfmdlg_open2(NULL,IDS_CARD_EMAIL_DEL_ERR,NULL,0);
   ui_comm_cfmdlg_open2(NULL,IDS_CA_ADDRES_INFO,NULL,0);

 return SUCCESS;

}

static RET_CODE on_email_exit_to_mainmenu(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
   
   ui_return_to_mainmenu();
   ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
   return SUCCESS;
}

static RET_CODE on_email_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

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
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MAIL, MSG_EXIT_ALL)
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, plist_update_body)
  ON_COMMAND(MSG_CA_MAIL_DEL_RESULT, on_ca_mail_del_result_accept)
  ON_COMMAND(MSG_SELECT, on_email_ctrl_change_focus)
  ON_COMMAND(MSG_DELETE_ONE, on_dlg_email_del_one_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_dlg_email_del_all_mail)
  ON_COMMAND(MSG_EXIT, on_email_exit_to_mainmenu)
  ON_COMMAND(MSG_EXIT_ALL, on_email_exit_all)
END_MSGPROC(email_plist_list_proc, list_class_proc)

BEGIN_KEYMAP(email_body_text_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)    
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(email_body_text_keymap, NULL)

BEGIN_MSGPROC(email_body_text_proc, text_class_proc)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, plist_update_body)
  ON_COMMAND(MSG_CANCEL, on_email_ctrl_change_focus)
  ON_COMMAND(MSG_EXIT, on_email_exit_to_mainmenu)
END_MSGPROC(email_body_text_proc, text_class_proc)



