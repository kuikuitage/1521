
#include "ui_common.h"
#include "ui_alert_mess.h"
#include "ui_select.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_ALERT_CONT,
  IDC_ALERT_HEAD,
  IDC_ALERT_NUM,
  IDC_ALERT_HIDE,
  IDC_ALERT_NAME,
  IDC_ALERT_DELETE,
  IDC_ALERT_FDELETE,
  IDC_ALERT_FAV,
  IDC_ALERT_FFAV,
  IDC_ALERT_PRO,
  IDC_ALERT_BAR,
  IDC_ALERT_RECEIVED_HEAD,
  IDC_ALERT_RECEIVED,
  IDC_ALERT_RESET_HEAD,
  IDC_ALERT_RESET,
  IDC_ALERT_RECEIVED_BAR,
};

enum prv_msg_t
{
  MSG_DEL_SET = MSG_USER_BEGIN + 200,
  MSG_DEL_ALL
};

enum del_status_t
{
  STATUS_DEL_NULL = 0,
  STATUS_DEL_FALSE,
  STATUS_DEL_TRUE,
};



static comm_help_data_t alert_help_data = //help bar data
{
  4,
  4,
  {
    IDS_BACK,
    IDS_EXIT,
    IDS_DEL_CURRENT,
    IDS_DEL_ALL ,
  },
  { 
    IM_SN_F1,//IM_ICON_EXIT,
    IM_SN_F1,//IM_ICON_BACK,
    IM_SN_F1,//IM_F1,
    IM_SN_F1,//IM_F3,
  },
};

static comm_help_data_t2 alert_help_data2 = 
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

static comm_dlg_data_t del_dlg =
{
  ROOT_ID_ALERT_MESS,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  DLG_STR_MODE_NULL,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_DEL_ANNOUNCE,
  RSC_INVALID_ID,
  0,
};

static list_xstyle_t alert_item_rstyle =
{
	RSI_SN_BG,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL,
};


static list_xstyle_t alert_list_field_fstyle =
{
  FSI_LIST_TXT_G,
  FSI_LIST_TXT_N,
  FSI_LIST_TXT_HL,
  FSI_LIST_TXT_SEL,
  FSI_LIST_TXT_HL,
};

static list_xstyle_t alert_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t alert_plist_attr[ALERT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
 	60, ALERT_HEAD_X+20, 0, &alert_list_field_rstyle,  &alert_list_field_fstyle },

  { LISTFIELD_TYPE_UNISTR |STL_VCENTER|STL_VCENTER,
 300, ALERT_HEAD_X+ALERT_NUMB_W+50, 0, &alert_list_field_rstyle,  &alert_list_field_fstyle },

  { LISTFIELD_TYPE_ICON|STL_VCENTER|STL_VCENTER ,
   60,  ALERT_HEAD_X+ALERT_NUMB_W+ALERT_NAME_W+265, 0, &alert_list_field_rstyle,  &alert_list_field_fstyle },
};

static cas_announce_headers_t alert_header;
static u32 del_flag[ALERT_MAX_NUMB];
static BOOL first_into_list = FALSE;

u16 alert_list_keymap(u16 key);
RET_CODE alert_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE alert_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[8];
  control_t *p_ctrl= NULL;

  //if (alert_header == NULL)
  {
    //return SUCCESS;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char*)asc_str, "%d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */

      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, alert_header.p_mail_head[start + i].subject);

      if(del_flag[start + i] == STATUS_DEL_TRUE)
      {
        list_set_field_content_by_icon(ctrl, start + i, 2, IM_SN_F1);//IM_ICON_HIDE);
      }
      else if(del_flag[start + i] == STATUS_DEL_FALSE)
      {
         list_set_field_content_by_icon(ctrl, start + i, 2, 0);
      }
    }
  }
  if((0 != alert_header.max_num) && (0xFFFF != alert_header.max_num))
  {
    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_ALERT_RECEIVED);
    sprintf((char*)asc_str,"%d",alert_header.max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_ALERT_RESET);
    sprintf((char*)asc_str,"%d",20 - alert_header.max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  }
  return SUCCESS;
}

#if 0
static RET_CODE on_alert_content(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 view_id;
  u16 item_idx;
  view_id = ui_dbase_get_pg_view_id();
  item_idx = list_get_focus_pos(p_list);

  //list_set_field_content_by_icon(p_list, item_idx, 2, IM_ICON_MAIL_READ);
  list_draw_field_ext(p_list, item_idx, 2, TRUE);
  return SUCCESS;

}
#endif

static RET_CODE on_alert_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 index = 0;
  prompt_type_t prompt_type;
  
  index = list_get_focus_pos(p_list);
  
  prompt_type.index = index;
  prompt_type.message_type = 1;
  
  if((0 != alert_header.max_num) && (0xFFFF != alert_header.max_num))
    manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)&alert_header);
  
  return SUCCESS;
}

RET_CODE open_alert_mess(u32 para1, u32 para2)
{
#define ALERT_RECEIVED_HELP_CNT				   16
  control_t *p_cont, *p_ctrl, *p_list, *p_bar;
  u16 i;
  // TRCA full_screen_title_t title_data = {IM_SN_F1/*IM_COMMON_BANNER_STOCK*/,IDS_ANNOUNCE_INFO, RSI_TITLE_BG};
  customer_cfg_t cus_cfg = {0};
  help_item_fc help_fc = {0};
  static sn_comm_help_data_t sn_email_help_data;
  
  help_rsc help_item[ALERT_RECEIVED_HELP_CNT]=
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
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F4},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 15 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 65 , IDS_EXIT}
  };
  DEBUG(TRCA, INFO, "start\n");
  get_customer_config(&cus_cfg);
  memset(&alert_header, 0, sizeof(cas_announce_headers_t));
  p_cont = ui_background_create(ROOT_ID_ALERT_MESS,
                            0,0,
                            640,480,
                            IDS_ANNOUNCE_INFO,TRUE);  
#if 0
TRCA
  p_menu = ui_comm_root_create_full_screen(ROOT_ID_ALERT_MESS,
                             0,
                             RSI_FULL_SCREEN_BG,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             &title_data);
#endif
 /*
  p_cont = ctrl_create_ctrl((u8*)CTRL_CONT, IDC_ALERT_CONT, ALERT_MENU_X,
                            ALERT_MENU_Y, ALERT_MENU_W, ALERT_MENU_H, p_menu, 0);

  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  */
  //head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_HEAD,
                           ALERT_HEAD_X, ALERT_HEAD_Y, ALERT_HEAD_W,ALERT_HEAD_H, p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);
  
  //number
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_NUM,
                           		ALERT_HEAD_X+40, ALERT_NUMB_Y, 
                           		ALERT_NUMB_W,ALERT_NUMB_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUMBER);
 
  //name
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_NAME,
                            ALERT_HEAD_X+ALERT_NUMB_W+200, ALERT_NAME_Y,
                            ALERT_NAME_W, ALERT_NAME_H,
                            p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NAME2);
  
  //delete
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_DELETE,
                           ALERT_HEAD_X+ALERT_NUMB_W+ALERT_NAME_W+300, ALERT_DELETE_Y, ALERT_DELETE_W,ALERT_DELETE_H, p_cont, 0);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DELETE2);
  
  //LIST
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_ALERT_PRO,
                           ALERT_LIST_X, ALERT_LIST_Y, ALERT_LIST_W,ALERT_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_keymap(p_list, alert_list_keymap);
  ctrl_set_proc(p_list, alert_list_proc);

  list_set_mid_rect(p_list, ALERT_LIST_MIDL, ALERT_LIST_MIDT,
           ALERT_LIST_MIDW, ALERT_LIST_MIDH,ALERT_LIST_VGAP);
  list_set_item_rstyle(p_list, &alert_item_rstyle);
  list_set_count(p_list, 0, ALERT_LIST_PAGE);

  list_set_field_count(p_list, ALERT_LIST_FIELD, ALERT_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, alert_plist_update, 0);
 
  for (i = 0; i < ALERT_LIST_FIELD; i++)
  {
	  DEBUG(TRCA, INFO, "\n");
    list_set_field_attr(p_list, (u8)i, (u32)(alert_plist_attr[i].attr), (u16)(alert_plist_attr[i].width),
                        (u16)(alert_plist_attr[i].left), (u8)(alert_plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, alert_plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, alert_plist_attr[i].fstyle);
  }
  
#if 0
  //bar
  p_sbar = ctrl_create_ctrl((u8*)CTRL_SBAR, IDC_ALERT_BAR, ALERT_BAR_X,
                          ALERT_BAR_Y, ALERT_BAR_W, ALERT_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  sbar_set_mid_rect(p_sbar, 0, (ALERT_BAR_H-80)/2, ALERT_BAR_W,  (ALERT_BAR_H-80)/2+40);
  list_set_scrollbar(p_list, p_sbar);
#endif

  //alert_plist_update(p_list, list_get_valid_pos(p_list), ALERT_LIST_PAGE, 0);

  p_bar = ctrl_create_ctrl((u8*)CTRL_CONT, IDC_ALERT_RECEIVED_BAR,
                          ALERT_RECEIVED_BAR_X, ALERT_RECEIVED_BAR_Y,
                           ALERT_RECEIVED_BAR_W,ALERT_RECEIVED_BAR_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_bar, RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL,RSI_SN_BAR_BLUE_NORMAL);
  //received head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_RECEIVED_HEAD,
                           ALERT_RECEIVED_HEAD_X, ALERT_RECEIVED_HEAD_Y,
                           ALERT_RECEIVED_HEAD_W,ALERT_RECEIVED_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVED);

  //received
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_RECEIVED,
                           ALERT_RECEIVED_X, ALERT_RECEIVED_Y,
                           ALERT_RECEIVED_W,ALERT_RECEIVED_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_ctrl, (u8*)"0");
  
  //rest space head
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_RESET_HEAD,
                           ALERT_RESET_HEAD_X, ALERT_RESET_HEAD_Y,
                           ALERT_RESET_HEAD_W, ALERT_RESET_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_REST_SPACE);

  //rest space
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_ALERT_RESET,
                           ALERT_RESET_X, ALERT_RESET_Y,
                           ALERT_RESET_W, ALERT_RESET_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_ctrl, (u8*)"20");
  ui_get_help_item_img_fc1(&help_fc);
  
  if(cus_cfg.customer == CUSTOMER_TONGGUANG
    || cus_cfg.customer == CUSTOMER_YINHE
    || cus_cfg.customer == CUSTOMER_TONGGUANG_CG
    || cus_cfg.customer == CUSTOMER_JINYA
    || cus_cfg.customer == CUSTOMER_TONGGUANG_QY
    || cus_cfg.customer == CUSTOMER_PANLONGHU_TR)
  {
    alert_help_data2.rsc_id[2] = HELP_RSC_BMP |help_fc.help_item_fc1;
    alert_help_data2.rsc_id[4] = HELP_RSC_BMP |help_fc.help_item_fc3;
    //TRCA ui_comm_help_create2(&alert_help_data2, p_cont, FALSE);
  }
  else
  {
    alert_help_data.bmp_id[2] = help_fc.help_item_fc1;
    alert_help_data.bmp_id[3] = help_fc.help_item_fc3;
    //TRCA ui_comm_help_create(&alert_help_data, p_cont);
  }
  memset((void*)&sn_email_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_email_help_data.x=0;
  sn_email_help_data.item_cnt=ALERT_RECEIVED_HELP_CNT;
  sn_email_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_email_help_data.offset=40;
  sn_email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_email_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  for(i = 0; i < ALERT_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  first_into_list = TRUE;
#ifndef WIN32
  ui_ca_get_info(CAS_CMD_ANNOUNCE_HEADER_GET, 0, 0);
 #endif
  DEBUG(TRCA, INFO, "end\n");
  return SUCCESS;
}

static RET_CODE plist_update_alert(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 size = ALERT_LIST_PAGE;
  u16 i;
  //alert_header = (cas_announce_headers_t *)para2;
  UI_PRINTF("plist_update_alert para2[0x%x]\n",para2);
  memcpy(&alert_header, (void*)para2, sizeof(cas_announce_headers_t));
  list_set_count(p_ctrl, alert_header.max_num, ALERT_LIST_PAGE);
  for(i = 0; i < alert_header.max_num; i ++)
    del_flag[i] = STATUS_DEL_FALSE;
  if(first_into_list == TRUE)
  {
    list_set_focus_pos(p_ctrl, 0);
    first_into_list = FALSE;
  }
  alert_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);

  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
  return SUCCESS;
}

static RET_CODE on_alert_del_one(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 index = 0;
  if((0 != alert_header.max_num) && (0xFFFF != alert_header.max_num))
  {
    index = list_get_focus_pos(p_list);
    if(del_flag[index] == STATUS_DEL_FALSE)
    {
      list_set_field_content_by_icon(p_list, index, 2, IM_SN_F1);//IM_ICON_HIDE);
      del_flag[index] = STATUS_DEL_TRUE;
    }
    else if(del_flag[index] == STATUS_DEL_TRUE)
    {
       list_set_field_content_by_icon(p_list, index, 2, 0);
      del_flag[index] = STATUS_DEL_FALSE;
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}

static RET_CODE on_alert_del_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  if((0 != alert_header.max_num) && (0xFFFF != alert_header.max_num))
  {
    for (i = 0; i < alert_header.max_num; i++)
    {
      for(i=0; i<ALERT_MAX_NUMB; i++)
      {
        if(del_flag[i] == STATUS_DEL_TRUE)
          del_cnt ++;
      }
      if(del_cnt == alert_header.max_num)
      {
        for (i = 0; i < alert_header.max_num; i++)
        {
          list_set_field_content_by_icon(p_list, i, 2, 0);
          del_flag[i] = STATUS_DEL_FALSE;
        }
      }
      else
      {
        for (i = 0; i < alert_header.max_num; i++)
        {
          list_set_field_content_by_icon(p_list, i, 2, IM_SN_F1);//IM_ICON_HIDE);
          del_flag[i] = STATUS_DEL_TRUE;
        }
      }
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
   return SUCCESS;
}

static RET_CODE on_alert_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  for(i=0; i<ALERT_MAX_NUMB; i++)
  {
    if(del_flag[i] == STATUS_DEL_TRUE)
      del_cnt ++;
  }
  if(del_cnt)
  {
    if(ui_comm_dlg_open(&del_dlg) == DLG_RET_YES)
    {
       if(del_cnt == alert_header.max_num)
      {
        ui_ca_get_info(CAS_CMD_ANNOUNCE_DEL_ALL, 0, 0);
      }
      else
      {
        for(i=0; i<ALERT_MAX_NUMB; i++)
        {
          if(del_flag[i] == STATUS_DEL_TRUE)
          {
            ui_ca_get_info(CAS_CMD_ANNOUNCE_DEL_ONE, alert_header.p_mail_head[i].m_id, 0);
          }
        }
      }
    }
  }

  for(i = 0; i < ALERT_MAX_NUMB; i ++)
    del_flag[i] = STATUS_DEL_NULL;
  //manage_close_menu(ROOT_ID_ALERT_MESS, para1, para2);
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(alert_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_F1, MSG_DEL_SET)
  ON_EVENT(V_KEY_RED, MSG_DEL_SET)
  ON_EVENT(V_KEY_F3, MSG_DEL_ALL)
  ON_EVENT(V_KEY_GREEN, MSG_DEL_ALL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)  
END_KEYMAP(alert_list_keymap, NULL)

BEGIN_MSGPROC(alert_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_alert_list_select)
  ON_COMMAND(MSG_CA_ANNOUNCE_HEADER_INFO, plist_update_alert)
  ON_COMMAND(MSG_DEL_SET, on_alert_del_one)
  ON_COMMAND(MSG_DEL_ALL, on_alert_del_all)
  ON_COMMAND(MSG_EXIT, on_alert_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_alert_exit)
  ON_COMMAND(MSG_TO_MAINMENU, on_alert_exit)
END_MSGPROC(alert_list_proc, list_class_proc)

