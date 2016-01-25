
#include "ui_common.h"
#include "ui_email_mess.h"
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif
#include "ui_comm_root.h"
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
  IDC_EMAIL_FAV,
  IDC_EMAIL_FFAV,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_NOTIC,
  IDC_EMAIL_BODY_CONT,
  IDC_EMAIL_TEXT_BAR,
  IDC_EMAIL_TEXT,

  IDC_EMAIL_LITTLE,
  IDC_EMAIL_BAR_LITTLE_ARROWL,
  IDC_EMAIL_BAR_LITTLE_ARROWR,
};

enum nvod_email_mess_local_msg
{
  MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
  MSG_DELETE_ALL,
};


u16 email_plist_list_keymap(u16 key);
u16 email_body_text_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE email_body_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#if 0
rsc

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

static comm_help_data_t email_help_data2 = //help bar data: email page up/down
{
  2,                                            
  2,
  {
    IDS_MOVE,
    IDS_BACK
  },
  { 
    IM_ICON_ARROW_UP_DOWN,
    IM_ICON_BACK
  },
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
      #if 0
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
      sprintf((char *)asc_str,"%2d/%2d/%2d %2d:%2d", 
                s_local_time.year, s_local_time.month, 
                s_local_time.day, s_local_time.hour, s_local_time.minute);
      str_asc2uni(asc_str, uni_str);
      #endif
      #if 1
      sprintf((char *)asc_str, "%04d/%02d/%02d %02d:%02d:%02d",
              p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 256 
                + p_mail_data_buf->p_mail_head[i + start].creat_date[1],
              p_mail_data_buf->p_mail_head[i + start].creat_date[2],
              p_mail_data_buf->p_mail_head[i + start].creat_date[3],
              p_mail_data_buf->p_mail_head[i + start].creat_date[4],
              p_mail_data_buf->p_mail_head[i + start].creat_date[5],
              p_mail_data_buf->p_mail_head[i + start].creat_date[6]);
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
  //email list title bar
  #define SN_EMAIL_HEAD_X                    10
  #define SN_EMAIL_HEAD_Y                    10
  #define SN_EMAIL_HEAD_W                    620
  #define SN_EMAIL_HEAD_H                    31

  //title number
  #define SN_EMAIL_NUMB_X                    10
  #define SN_EMAIL_NUMB_Y                    0
  #define SN_EMAIL_NUMB_W                    SN_EMAIL_HEAD_W/4
  #define SN_EMAIL_NUMB_H                    SN_EMAIL_HEAD_H
  
  //title topic
  #define SN_EMAIL_TITLE_X				   SN_EMAIL_NUMB_X + SN_EMAIL_NUMB_W
  #define SN_EMAIL_TITLE_Y				   SN_EMAIL_NUMB_Y
  #define SN_EMAIL_TITLE_W				   SN_EMAIL_HEAD_W/4
  #define SN_EMAIL_TITLE_H				   SN_EMAIL_HEAD_H

  //title new/old
  #define SN_EMAIL_NEW_X                   SN_EMAIL_TITLE_X + SN_EMAIL_TITLE_W - 23
  #define SN_EMAIL_NEW_Y                   SN_EMAIL_NUMB_Y
  #define SN_EMAIL_NEW_W                   SN_EMAIL_HEAD_W/4
  #define SN_EMAIL_NEW_H                   SN_EMAIL_HEAD_H

  //title date
  #define SN_EMAIL_DATE_X                    SN_EMAIL_NEW_X + SN_EMAIL_NEW_W + 23
  #define SN_EMAIL_DATE_Y                    SN_EMAIL_NUMB_Y
  #define SN_EMAIL_DATE_W                    SN_EMAIL_HEAD_W/4 - 10
  #define SN_EMAIL_DATE_H                    SN_EMAIL_HEAD_H

  //email list
  #define SN_EMAIL_LIST_X                    SN_EMAIL_HEAD_X
  #define SN_EMAIL_LIST_Y                    SN_EMAIL_HEAD_Y + SN_EMAIL_HEAD_H
  #define SN_EMAIL_LIST_W                    SN_EMAIL_HEAD_W
  #define SN_EMAIL_LIST_H                    180

  #define SN_EMAIL_LIST_MIDL                 0
  #define SN_EMAIL_LIST_MIDT                 0
  #define SN_EMAIL_LIST_MIDW                 SN_EMAIL_LIST_W - 2*SN_EMAIL_LIST_MIDL
  #define SN_EMAIL_LIST_MIDH                 SN_EMAIL_LIST_H - 2*SN_EMAIL_LIST_MIDT

  //email content
  
#define SN_EMAIL_TEXT_X					SN_EMAIL_HEAD_X
#define SN_EMAIL_TEXT_Y					SN_EMAIL_HEAD_Y + SN_EMAIL_HEAD_H + SN_EMAIL_LIST_H
#define SN_EMAIL_TEXT_W					SN_EMAIL_HEAD_W
#define SN_EMAIL_TEXT_H					120
  
  #define SN_EMAIL_LIST_VGAP                 8
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
      RSI_SN_BG,//RSI_COMM_LIST_N,
      RSI_SN_BG,//RSI_COMM_LIST_N,
      RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
      RSI_SN_BG,//RSI_COMM_LIST_SEL,
      RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_N,
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
	control_t *p_ctrl_number, *p_ctrl_topic, *p_ctrl_date, *p_ctrl_new_old;//, *p_ctrl_little, *p_ctrl_little_arrow
    control_t *p_cont = NULL;
    control_t *p_ctrl = NULL;
    control_t *p_list = NULL;
    u16 i = 0;
#if 0
rsc

  get_customer_config(&cfg);
  if((CUSTOMER_XINSHIDA== cfg.customer) 
    || (CUSTOMER_XINSIDA_LQ == cfg.customer) 
    || (CUSTOMER_CHANGJIANG == cfg.customer) 
    || (CUSTOMER_CHANGJIANG_LQ == cfg.customer)
    || (CUSTOMER_CHANGJIANG_NY== cfg.customer)
    || (CUSTOMER_CHANGJIANG_QY== cfg.customer)
    || (CUSTOMER_CHANGJIANG_JS == cfg.customer)
    || (CUSTOMER_XINSIDA_SPAISH == cfg.customer)
    || (CUSTOMER_HEBI == cfg.customer))
  {
    email_help_data.bmp_id[3] = IM_F2_2;
  }
#endif

  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_MAIL_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  
  //email list title bar
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_HEAD,
                           SN_EMAIL_HEAD_X, SN_EMAIL_HEAD_Y, 
                           SN_EMAIL_HEAD_W,SN_EMAIL_HEAD_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SECOND_TITLE
  
  //title number
  p_ctrl_number = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_NUM,
                           SN_EMAIL_NUMB_X, SN_EMAIL_NUMB_Y, 
                           SN_EMAIL_NUMB_W,SN_EMAIL_NUMB_H, p_ctrl, 0);
  text_set_font_style(p_ctrl_number, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_align_type(p_ctrl_number, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl_number, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl_number, IDS_NUMBER);

  //title date
  p_ctrl_date = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FHIDE,
                            SN_EMAIL_DATE_X, SN_EMAIL_DATE_Y,
                            SN_EMAIL_DATE_W, SN_EMAIL_DATE_H,
                            p_ctrl, 0);
  text_set_font_style(p_ctrl_date, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_align_type(p_ctrl_date, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl_date, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl_date, IDS_DATE);

  //title new/old
  p_ctrl_new_old = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FFREQ,
                            SN_EMAIL_NEW_X, SN_EMAIL_NEW_Y,
                            SN_EMAIL_NEW_W, SN_EMAIL_NEW_H,
                            p_ctrl, 0);
  text_set_align_type(p_ctrl_new_old, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl_new_old, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl_new_old, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl_new_old, IDS_STATE);
  
  //title topic
  p_ctrl_topic = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_FAV,
                           SN_EMAIL_TITLE_X-10, SN_EMAIL_TITLE_Y, 
                           SN_EMAIL_TITLE_W,SN_EMAIL_TITLE_H, p_ctrl, 0);
  text_set_align_type(p_ctrl_topic, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl_topic, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
  text_set_content_type(p_ctrl_topic, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl_topic, IDS_TITLE);
  
  //email list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_EMAIL_PRO,
                           SN_EMAIL_LIST_X, SN_EMAIL_LIST_Y, 
                           SN_EMAIL_LIST_W,SN_EMAIL_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_TIP_BOX
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  list_set_mid_rect(p_list, SN_EMAIL_LIST_MIDL, SN_EMAIL_LIST_MIDT,
           					SN_EMAIL_LIST_MIDW, SN_EMAIL_LIST_MIDH,SN_EMAIL_LIST_VGAP);//EMAIL_LIST_VGAP
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

  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EMAIL_TEXT,
                           SN_EMAIL_TEXT_X, SN_EMAIL_TEXT_Y, 
                           SN_EMAIL_TEXT_W ,SN_EMAIL_TEXT_H, 
                           p_cont, 0);
  ctrl_set_keymap(p_ctrl, email_body_text_keymap); 
  ctrl_set_proc(p_ctrl, email_body_text_proc);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG, RSI_SN_EMAIL_CONTENT_BG, RSI_DIG_BG);//RSI_TIP_BOX
  text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
  text_set_offset(p_ctrl,5,5);
  text_set_font_style(p_ctrl, FSI_COMM_BTN,FSI_COMM_TXT_HL,FSI_COMM_BTN);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
  text_enable_page(p_ctrl, TRUE);

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  if(TRUE == get_full_scr_ad_status())
    ui_pic_stop();
#endif


  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=SN_EMAIL_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  #endif
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
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_number,0);  
  #endif
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
//    ui_comm_help_set_data(&email_help_data2, p_cont);
    break;

  case IDC_EMAIL_TEXT:
    p_new_act_ctrl = ctrl_get_child_by_id(p_cont, IDC_EMAIL_PRO);
    email_plist_update(p_new_act_ctrl, list_get_valid_pos(p_new_act_ctrl), EMAIL_LIST_PAGE, 0);
    email_No = p_mail_data_buf->p_mail_head[list_get_focus_pos(p_new_act_ctrl)].m_id;
	#ifndef WIN32
	ui_ca_get_info(CAS_CMD_MAIL_BODY_GET, email_No ,0);
	#endif
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

  ctrl_paint_ctrl(p_trl, TRUE);
  ctrl_paint_ctrl(p_new_act_ctrl, TRUE);
  return SUCCESS;
  
}
extern u16 get_message_strid();
static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u16 new_focus;
  control_t *p_ctrl = NULL;
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);

  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("--------on_email_del_mail   :focus :%d\n",new_focus);
  switch(msg)
  {
      case MSG_DELETE_ONE:
		#ifndef WIN32
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[new_focus].m_id,0);  
		#endif
		break;
      case MSG_DELETE_ALL:
		#ifndef WIN32
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ALL, 0 ,0);  
		#endif
        break;
      default:
        return ERR_FAILURE;
  }
  if((get_message_strid() == IDS_CA_EMAIL_NO_SPACE) && (NULL != p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
  }
  
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  #endif
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

static RET_CODE on_email_exit_to_mainmenu(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
   
   ui_return_to_mainmenu();
   
   #ifndef WIN32
   ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
   #endif
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
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_GREEN, MSG_DELETE_ALL)
#else
  ON_EVENT(V_KEY_YELLOW, MSG_DELETE_ALL)
#endif
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
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(email_body_text_keymap, NULL)

BEGIN_MSGPROC(email_body_text_proc, text_class_proc)
  ON_COMMAND(MSG_CA_MAIL_BODY_INFO, plist_update_body)
  ON_COMMAND(MSG_CANCEL, on_email_ctrl_change_focus)
  ON_COMMAND(MSG_EXIT, on_email_exit_to_mainmenu)
END_MSGPROC(email_body_text_proc, text_class_proc)

