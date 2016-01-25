
#include "ui_common.h"
#include "ui_email_mess.h"
#include "ui_new_mail.h"
#include "ui_select.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_EMAIL_CONT,
  IDC_EMAIL_HEAD,
  IDC_EMAIL_NUM,
  IDC_EMAIL_HIDE,
  IDC_EMAIL_NAME,
  IDC_EMAIL_IMPORTANCE,
  IDC_EMAIL_DELETE,
  IDC_EMAIL_FDELETE,
  IDC_EMAIL_FAV,
  IDC_EMAIL_FFAV,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_RECEIVED_HEAD,
  IDC_EMAIL_RECEIVED,
  IDC_EMAIL_RESET_HEAD,
  IDC_EMAIL_RESET,
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

static comm_dlg_data_t del_dlg =
{
  ROOT_ID_EMAIL_MESS,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  DLG_STR_MODE_NULL,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_DEL_MAIL,
  RSC_INVALID_ID,
  0,
};

static list_xstyle_t email_item_rstyle =
{
   RSI_SN_BG,//RSI_COMM_LIST_N,
   RSI_SN_BG,//RSI_COMM_LIST_N,
   RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
   RSI_SN_BG,//RSI_COMM_LIST_SEL,
   RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_N,
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

static comm_help_data_t email_help_data = //help bar data
{
  4,
  4,
  {
    IDS_BACK,
    IDS_EXIT,
    IDS_DEL_CURRENT,
    IDS_DEL_ALL,
  },
  { 
    IM_SN_F1,//IM_ICON_EXIT,
    IM_SN_F1,//IM_ICON_BACK,
    IM_SN_F1,//IM_F1,
    IM_SN_F1,//IM_F3,
  },
};

static comm_help_data_t2 email_help_data2 = 
{
  7, 0, {118, 118, 34, 84, 34, 84},
  {
    HELP_RSC_STRID | IDS_HELP_BACK,
    HELP_RSC_STRID | IDS_HELP_EXIT,
    HELP_RSC_BMP   | IM_SN_F1,//IM_F1,
    HELP_RSC_STRID | IDS_DEL_CURRENT,
    HELP_RSC_BMP   | IM_SN_F1,//IM_F3,
    HELP_RSC_STRID | IDS_DEL_ALL,
  },
};

static list_field_attr_t email_plist_attr[EMAIL_LIST_FIELD] =
{
  { LISTFIELD_TYPE_DEC | STL_LEFT | STL_VCENTER,
  50, 10, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

  { LISTFIELD_TYPE_STRID | STL_VCENTER | STL_VCENTER,
  60, 60, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
  250, 130, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

  { LISTFIELD_TYPE_STRID,
  60, 380, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

  { LISTFIELD_TYPE_ICON ,
  60, 510, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },
};

u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static cas_mail_headers_t mail_header;
static u32 del_flag[EMAIL_MAX_NUMB];
static BOOL first_into_list = FALSE;

static RET_CODE email_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[8];
  control_t *p_ctrl= NULL;

  //if(mail_header == NULL)
  {
    //return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      //NO.
      //sprintf((char*)asc_str, "%d ", mail_header.p_mail_head[start + i].m_id);
     // list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, mail_header.p_mail_head[start + i].m_id);
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, start + i + 1); 
      
      //new
      list_set_field_content_by_strid(ctrl, (u16)(start + i), 1, 
              mail_header.p_mail_head[i + start].new_email ? IDS_YES : IDS_NO);

      //NAME
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, mail_header.p_mail_head[i + start].subject);

      //importance
      list_set_field_content_by_strid(ctrl, (u16)(start + i), 3, 
        mail_header.p_mail_head[i + start].priority ? IDS_YES : IDS_NO);

      if(del_flag[start + i] == STATUS_DEL_TRUE)
      {
        list_set_field_content_by_icon(ctrl, start + i, 4, IM_SN_F1);//IM_ICON_HIDE);
      }
      else if(del_flag[start + i] == STATUS_DEL_FALSE)
      {
         list_set_field_content_by_icon(ctrl, start + i, 4, 0);
      }
    }
  }
  if((0 != mail_header.max_num) && (0xFFFF != mail_header.max_num))
  {
    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RECEIVED);
    sprintf((char*)asc_str,"%d",mail_header.max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RESET);
    sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB - mail_header.max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str); 
  }
  return SUCCESS;
}

#if 0
static RET_CODE on_email_content(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 item_idx;
  item_idx = list_get_focus_pos(p_list);

  //list_set_field_content_by_icon(p_list, item_idx, 2, IM_ICON_MAIL_READ);
  list_draw_field_ext(p_list, item_idx, 2, TRUE);
  return SUCCESS;
}
#endif

RET_CODE open_email_mess(u32 para1, u32 para2)
{
#define SN_EMAIL_W          640
#define SN_EMAIL_H          480
#define SN_EMAIL_HELP_CNT				   16

  control_t *p_cont, *p_ctrl, *p_list, *p_bar;
  u16 i;
  u8 asc_str[8];
  help_item_fc help_fc = {0};
  //TRCA full_screen_title_t title_data = {IM_COMMON_BANNER_MESSAGE,IDS_EMAIL, RSI_TITLE_BG};
  customer_cfg_t cus_cfg = {0};
  static sn_comm_help_data_t sn_email_help_data;
  
  help_rsc help_item[SN_EMAIL_HELP_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_F1},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 105 , IDS_SEL_CURRENT},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_F2},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 95 , IDS_SEL_ALL},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F4},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT_SURE},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 65 , IDS_EXIT}
  };
  
  get_customer_config(&cus_cfg);
  memset(&mail_header, 0, sizeof(mail_header));
  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS,
                             0, 0,
                             SN_EMAIL_W, SN_EMAIL_H,
                             IDS_EMAIL,TRUE);  
  //head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_HEAD,
                           EMAIL_HEAD_X, EMAIL_HEAD_Y, EMAIL_HEAD_W,EMAIL_HEAD_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);
  
  //number
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_NUM,
                           EMAIL_NUMB_X, EMAIL_NUMB_Y, EMAIL_NUMB_W,EMAIL_NUMB_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUMBER);
   
  //new
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_FDELETE,
                            EMAIL_NEW_X, EMAIL_NEW_Y,
                            EMAIL_NEW_W, EMAIL_NEW_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NEW);
 
  //name
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_NAME,
                            EMAIL_NAME_X, EMAIL_NAME_Y,
                            EMAIL_NAME_W, EMAIL_NAME_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NAME2);

  //importance
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_IMPORTANCE,
                            EMAIL_IMPORTANCE_X, EMAIL_IMPORTANCE_Y,
                            EMAIL_IMPORTANCE_W, EMAIL_IMPORTANCE_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IMPORTANT);
  
  //delete
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_DELETE,
                           EMAIL_DELETE_X, EMAIL_DELETE_Y, EMAIL_DELETE_W,EMAIL_DELETE_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DELETE2);
  
  //LIST
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_EMAIL_PRO,
                           EMAIL_LIST_X, EMAIL_LIST_Y, EMAIL_LIST_W,EMAIL_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
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
  
#if 0
  //bar
  p_sbar = ctrl_create_ctrl((u8*)CTRL_SBAR, IDC_EMAIL_BAR, EMAIL_BAR_X,
                          EMAIL_BAR_Y, EMAIL_BAR_W, EMAIL_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  sbar_set_mid_rect(p_sbar, 0, (EMAIL_BAR_H-80)/2, EMAIL_BAR_W,  (EMAIL_BAR_H-80)/2+40);
  list_set_scrollbar(p_list, p_sbar);
#endif

  //email_plist_update(p_list, list_get_valid_pos(p_list), EMAIL_LIST_PAGE, 0);

  //bar
  p_bar = ctrl_create_ctrl((u8*)CTRL_CONT, IDC_EMAIL_BAR,
                           EMAIL_BAR_X, EMAIL_BAR_Y,
                           EMAIL_BAR_W,EMAIL_BAR_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_bar, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);

  //received head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RECEIVED_HEAD,
                           EMAIL_RECEIVED_HEAD_X, EMAIL_RECEIVED_HEAD_Y,
                           EMAIL_RECEIVED_HEAD_W,EMAIL_RECEIVED_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVED);

  //received
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RECEIVED,
                           EMAIL_RECEIVED_X, EMAIL_RECEIVED_Y,
                           EMAIL_RECEIVED_W,EMAIL_RECEIVED_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_ctrl, (u8*)"0");
  
  //rest space head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RESET_HEAD,
                           EMAIL_RESET_HEAD_X, EMAIL_RESET_HEAD_Y,
                           EMAIL_RESET_HEAD_W, EMAIL_RESET_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_REST_SPACE);

  //rest space
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_RESET,
                           EMAIL_RESET_X, EMAIL_RESET_Y,
                           EMAIL_RESET_W, EMAIL_RESET_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB);
  text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  
  ui_get_help_item_img_fc1(&help_fc);
  if(cus_cfg.customer == CUSTOMER_TONGGUANG
    || cus_cfg.customer == CUSTOMER_YINHE
    || cus_cfg.customer == CUSTOMER_TONGGUANG_CG
    || cus_cfg.customer == CUSTOMER_JINYA
    || cus_cfg.customer == CUSTOMER_TONGGUANG_QY
    || cus_cfg.customer == CUSTOMER_PANLONGHU_TR)
  {
    email_help_data2.rsc_id[2] = HELP_RSC_BMP | help_fc.help_item_fc1;
    email_help_data2.rsc_id[4] = HELP_RSC_BMP | help_fc.help_item_fc3;
    //TRCA ui_comm_help_create2(&email_help_data2, p_cont, FALSE);
  }
  else
  {
    email_help_data.bmp_id[2] = help_fc.help_item_fc1;
    email_help_data.bmp_id[3] = help_fc.help_item_fc3;
    //TRCA ui_comm_help_create(&email_help_data, p_cont);
  }
  
  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=SN_EMAIL_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_cont);

  
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  for(i = 0; i < EMAIL_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  first_into_list = TRUE;
#ifndef WIN32
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
#endif
  return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 size = EMAIL_LIST_PAGE;
  u16 i;
  //mail_header = (cas_mail_headers_t*)para2;
  UI_PRINTF("plist_update_email para2[0x%x]\n",para2);
  memcpy(&mail_header, (void*)para2, sizeof(cas_mail_headers_t));
  list_set_count(p_ctrl, mail_header.max_num, EMAIL_LIST_PAGE);
  for(i = 0; i < mail_header.max_num; i ++)
    del_flag[i] = STATUS_DEL_FALSE;
  if(first_into_list == TRUE)
  {
    list_set_focus_pos(p_ctrl, 0);
    first_into_list = FALSE;
  }
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);

  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);

 return SUCCESS;

}

static RET_CODE on_email_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 index = 0;
  prompt_type_t prompt_type;

#ifdef WIN32
  manage_open_menu(ROOT_ID_CA_PROMPT, 0, 0);
  return SUCCESS;
#endif
  index = list_get_focus_pos(p_list); 
  prompt_type.index = index;
  DEBUG(MAIN,INFO,"prompt_type.index  = %d\n",prompt_type.index );
  prompt_type.message_type = 0;

   if((0 != mail_header.max_num) && (0xFFFF != mail_header.max_num))
   {
	    manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)&mail_header);
	    DEBUG(MAIN,INFO,"mail_header.max_num = %d \n",mail_header.max_num);
   }
  
  return SUCCESS;
}

static RET_CODE on_email_del_one(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 index = 0;
  if((0 != mail_header.max_num) && (0xFFFF != mail_header.max_num))
  {
    index = list_get_focus_pos(p_list);
    if(del_flag[index] == STATUS_DEL_FALSE)
    {
      list_set_field_content_by_icon(p_list, index, 4, IM_SN_F1);//IM_ICON_HIDE);
      del_flag[index] = STATUS_DEL_TRUE;
    }
    else if(del_flag[index] == STATUS_DEL_TRUE)
    {
       list_set_field_content_by_icon(p_list, index, 4, 0);
      del_flag[index] = STATUS_DEL_FALSE;
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}


static RET_CODE on_email_del_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  if((0 != mail_header.max_num) && (0xFFFF != mail_header.max_num))
  {
    for(i=0; i<EMAIL_MAX_NUMB; i++)
    {
      if(del_flag[i] == STATUS_DEL_TRUE)
        del_cnt ++;
    }
    if(del_cnt == mail_header.max_num)
    {
      for (i = 0; i < mail_header.max_num; i++)
      {
        list_set_field_content_by_icon(p_list, i, 4, 0);
        del_flag[i] = STATUS_DEL_FALSE;
      }
    }
    else
    {
      for (i = 0; i < mail_header.max_num; i++)
      {
        list_set_field_content_by_icon(p_list, i, 4, IM_SN_F1);//IM_ICON_HIDE);
        del_flag[i] = STATUS_DEL_TRUE;
      }
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}

static RET_CODE on_email_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  u16 new_mail_cnt = 0;
  DEBUG(MAIN,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>on_email_exit\n");
  for(i=0; i<EMAIL_MAX_NUMB; i++)
  {
    if(del_flag[i] == STATUS_DEL_TRUE)
      del_cnt ++;
  }
  if(del_cnt)
  {
    if(ui_comm_dlg_open(&del_dlg) == DLG_RET_YES)
    {
       if(del_cnt == mail_header.max_num)
      {
        //TRCA ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ALL, 0, 0);
        ui_ca_get_info(CAS_CMD_MAIL_DEL_ALL, 0 ,0);
        if(ui_is_new_mail())
        {
          close_new_mail();
          ui_set_new_mail(FALSE);
        }
      }
      else
      {
        for(i=0; i<EMAIL_MAX_NUMB; i++)
        {
          if(mail_header.p_mail_head[i].new_email && del_flag[i] != STATUS_DEL_TRUE)
          {
            new_mail_cnt ++;
          }
          if(del_flag[i] == STATUS_DEL_TRUE)
          {
            //TRCA ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ONE, mail_header.p_mail_head[i].m_id, 0);
            ui_ca_get_info(CAS_CMD_MAIL_DEL_ONE, mail_header.p_mail_head[i].m_id ,0);
          }
        }
        if(new_mail_cnt == 0)
        {
          if(ui_is_new_mail())
          {
            close_new_mail();
            ui_set_new_mail(FALSE);
          }
        }
      }
    }
  }
  else
  {
    for(i=0; i < mail_header.max_num; i++)
    {
      if(mail_header.p_mail_head[i].new_email)
        new_mail_cnt ++;
    }
    if(new_mail_cnt == 0)
    {
      if(ui_is_new_mail())
      {
        close_new_mail();
        ui_set_new_mail(FALSE);
      }
    }
  }
  for(i = 0; i < EMAIL_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
  // ctrl_paint_ctrl(ctrl_get_root(p_list),FALSE);  
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
 // manage_close_menu(ROOT_ID_EMAIL_MESS, para1, para2);
  return ERR_NOFEATURE;
}


static RET_CODE on_email_exit_key(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  u16 new_mail_cnt = 0;
  DEBUG(MAIN,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>on_email_exit\n");
  for(i=0; i<EMAIL_MAX_NUMB; i++)
  {
    if(del_flag[i] == STATUS_DEL_TRUE)
      del_cnt ++;
  }
  if(del_cnt == 0)
  {
    for(i=0; i < mail_header.max_num; i++)
    {
      if(mail_header.p_mail_head[i].new_email)
        new_mail_cnt ++;
    }
    if(new_mail_cnt == 0)
    {
      if(ui_is_new_mail())
      {
        close_new_mail();
        ui_set_new_mail(FALSE);
      }
    }
  }
  for(i = 0; i < EMAIL_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  manage_close_menu(ROOT_ID_EMAIL_MESS, para1, para2);
  return ERR_NOFEATURE;
}


static RET_CODE on_email_return(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  DEBUG(MAIN,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  if((0 != mail_header.max_num) && (0xFFFF != mail_header.max_num))
  {
      for (i = 0; i < mail_header.max_num; i++)
      {
        list_set_field_content_by_icon(p_list, i, 4, 0);
      }
  }
  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}


/*
static RET_CODE on_email_return(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  customer_cfg_t customer_cfg = {0};
  get_customer_config(&customer_cfg);
  if(customer_cfg.customer == CUSTOMER_TONGGUANG
    || customer_cfg.customer == CUSTOMER_YINHE
    || customer_cfg.customer == CUSTOMER_TONGGUANG_CG
    || customer_cfg.customer == CUSTOMER_JINYA
    || customer_cfg.customer == CUSTOMER_TONGGUANG_QY
    || customer_cfg.customer == CUSTOMER_PANLONGHU_TR)
  {
     on_email_exit(p_list, msg, 0, 0);
     ui_close_all_mennus();
  }
  return SUCCESS;
}
*/



BEGIN_KEYMAP(email_plist_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  //ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_F1, MSG_DEL_SET)
  ON_EVENT(V_KEY_RED, MSG_DEL_SET)
  ON_EVENT(V_KEY_F3, MSG_DEL_ALL)
  ON_EVENT(V_KEY_YELLOW, MSG_DEL_ALL)

  ON_EVENT(V_KEY_BLUE, MSG_EXIT)  
  ON_EVENT(V_KEY_F4, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  
  ON_EVENT(V_KEY_MAIL, MSG_RETURN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)  
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_email_list_select)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  ON_COMMAND(MSG_DEL_SET, on_email_del_one)
  ON_COMMAND(MSG_DEL_ALL, on_email_del_all)

  ON_COMMAND(MSG_EXIT, on_email_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_email_exit_key)
  
  ON_COMMAND(MSG_RETURN, on_email_return)
 // ON_COMMAND(MSG_TO_MAINMENU, on_email_exit)
END_MSGPROC(email_plist_list_proc, list_class_proc)



