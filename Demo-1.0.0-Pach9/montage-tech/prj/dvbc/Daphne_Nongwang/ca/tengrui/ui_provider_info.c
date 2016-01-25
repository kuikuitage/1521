/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_provider_info.h"
#include "sys_dbg.h"

enum control_id
{
  IDC_INVALID = 0,
    
  IDC_BG,
  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
  
  IDC_PROVIDER_INFO_HEAD1,
  IDC_PROVIDER_INFO_HEAD2,
  IDC_PROVIDER_INFO_HEAD3,
  IDC_PROVIDER_INFO_HEAD4,
  
  IDC_PROVIDER_INFO_LIST1,
  IDC_PROVIDER_INFO_LIST2,
  IDC_PROVIDER_INFO_LIST3,
  IDC_PROVIDER_INFO_LIST4,
};

static list_xstyle_t provider_info_item_rstyle =
{
	RSI_SN_BAR_YELLOW_HL,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL

};


static list_xstyle_t provider_info_field_fstyle =
{
	FSI_LIST_TXT_G,
	FSI_LIST_TXT_N,
	FSI_LIST_TXT_HL,
	FSI_LIST_TXT_SEL,
	FSI_LIST_TXT_HL,
};

static list_xstyle_t provider_info_field_rstyle =
{
    RSI_IGNORE,//RSI_COMM_LIST_N,
    RSI_IGNORE,//RSI_COMM_LIST_N,
    RSI_IGNORE,//RSI_COMM_LIST_HL,
    RSI_IGNORE,//RSI_COMM_LIST_SEL,
    RSI_IGNORE,//RSI_COMM_LIST_N,
};
#if 0
TRCA
static comm_help_data_t provider_help_data = //help bar data
{
  2,
  2,
  {
    IDS_BACK,
    IDS_EXIT,
  },
  { 
    IM_SN_F1,//IM_ICON_EXIT,
    IM_SN_F1,//IM_ICON_BACK,
  },
};

static comm_help_data_t2 provider_help_data2 = 
{
  3, 0, {236, 236},
  {
    HELP_RSC_STRID | IDS_HELP_BACK,
    HELP_RSC_STRID | IDS_HELP_EXIT,
  },
};
#endif
static list_field_attr_t provider_info_list1_attr[PROVIDER_LIST_FIELD3] =
{
  { LISTFIELD_TYPE_UNISTR,
    100, 30, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_STRID,
    180, 160, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    230, 340, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static list_field_attr_t provider_info_list2_attr[PROVIDER_LIST_FIELD4] =
{
  { LISTFIELD_TYPE_DEC,
    100, 30, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 160, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    120, 310, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    120, 460, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static list_field_attr_t provider_info_list3_attr[PROVIDER_LIST_FIELD3] =
{
  { LISTFIELD_TYPE_UNISTR,
    100, 30, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 255, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 430, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};


static list_field_attr_t provider_info_list4_attr[PROVIDER_LIST_FIELD5] =
{
  { LISTFIELD_TYPE_DEC | STL_VCENTER| STL_VCENTER,
    50, 10, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    200, 60, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_STRID | STL_VCENTER | STL_VCENTER,
    100, 260, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    60, 380, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    80, 500, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static u8 curn_index = 0;
static u8 g_sub_curn_index = 0;
static product_entitles_info_t *p_entitle = NULL;
static product_entitles_info_t entitle_info = {0};
static ipps_info_t *p_ippv = NULL;
static burses_info_t *p_burse = NULL;
static ipp_total_record_info_t *p_consume = NULL;
static u16 menu_btn_str[PROVIDER_ITEMMENU_CNT] =
{
  IDS_GENERAL_PRO, IDS_IPPV_PRO_INFO, IDS_SLOT_INFO, IDS_IPP_CONSUME,
};

static u16 list1_title_str[PROVIDER_LIST_FIELD3] =
{
  IDS_PRODUCT_ID2, IDS_COMMISION_STATE, IDS_COMMISION_DATE
};
static u16 list2_title_str[PROVIDER_LIST_FIELD4] =
{
  IDS_IPP_ID, IDS_TYPE2, IDS_START_TIME4, IDS_END_TIME4
};
static u16 list3_title_str[PROVIDER_LIST_FIELD3] =
{
  IDS_SLOT_ID, IDS_CREDIT, IDS_REMAIN_NUMBER
};
static u16 list4_title_str[PROVIDER_LIST_FIELD5] =
{
  IDS_INDEX,IDS_TIME/*TRCA IDS_INDEX, IDS_TIME*/, IDS_STATE, IDS_GLIDE_NUM, IDS_MONEY
};

RET_CODE provider_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 provider_info_button_keymap(u16 key);
RET_CODE provider_info_button_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 provider_info_list_keymap(u16 key);
RET_CODE provider_info_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE provider_info_list1_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];
  u16 strid = RSC_INVALID_ID;
  
  if(p_entitle == NULL)
  {
    return ERR_FAILURE;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%d",(int)entitle_info.p_entitle_info[i].product_id);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      //list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, 
      //p_entitle->p_entitle_info[i + start].states?"No authorization":"Authorization OK"); 
      UI_PRINTF("provider_info_list1_update item[%d] states[%d]\n",i + start, p_entitle->p_entitle_info[i + start].states);
      switch(entitle_info.p_entitle_info[i].states)
      {
        case 0:
          strid = IDS_AUTHORIZATION_OK;
          break;

        case 1:
          strid = IDS_PARAMETER_ERROR;
          break;

        case 2:
          strid = IDS_NO_ENTITLE_INFO;
          break;

        case 3:
          strid = IDS_LIMITED;
          break;

        case 4:
          strid = IDS_ENTITLE_CLOSED;
          break;

        case 5:
          strid = IDS_ENTITLE_PAUSED;
          break;

        default:
          break;
      }
      list_set_field_content_by_strid(ctrl, (u16)(start + i), 1, strid);

      memset(asc_str, 0, sizeof(asc_str));
      if(entitle_info.p_entitle_info[i].states == 0)
      {
        sprintf((char*)asc_str,"%04d.%02d.%02d--%04d.%02d.%02d",
                  entitle_info.p_entitle_info[i].start_time[0] * 100 + 
                  entitle_info.p_entitle_info[i].start_time[1], 
                  entitle_info.p_entitle_info[i].start_time[2],
                  entitle_info.p_entitle_info[i].start_time[3],
                  entitle_info.p_entitle_info[i].expired_time[0] * 100 + 
                  entitle_info.p_entitle_info[i].expired_time[1], 
                  entitle_info.p_entitle_info[i].expired_time[2],
                  entitle_info.p_entitle_info[i].expired_time[3]);
      }
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, asc_str);    
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list2_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_ippv == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, p_ippv->p_ipp_info[i + start].index); 
      
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, p_ippv->p_ipp_info[i + start].type?(u8*)"ippt":(u8*)"ippv"); 

      sprintf((char*)asc_str,"%04d.%02d.%02d",p_ippv->p_ipp_info[i + start].start_time[0] * 100 + 
              p_ippv->p_ipp_info[i + start].start_time[1], 
              p_ippv->p_ipp_info[i + start].start_time[2],
              p_ippv->p_ipp_info[i + start].start_time[3]);     
       list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, asc_str); 

      sprintf((char*)asc_str,"%04d.%02d.%02d",p_ippv->p_ipp_info[i + start].expired_time[0] * 100 + 
              p_ippv->p_ipp_info[i + start].expired_time[1], 
              p_ippv->p_ipp_info[i + start].expired_time[2],
              p_ippv->p_ipp_info[i + start].expired_time[3]);     
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list3_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_burse == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].record_index);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 
      
      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].credit_limit);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1,  asc_str); 

      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].cash_value);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2,  asc_str); 
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list4_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_consume == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, p_consume->record_info[i + start].index); 
      sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",p_consume->record_info[i + start].start_time[0] * 100 + 
              p_consume->record_info[i + start].start_time[1], 
              p_consume->record_info[i + start].start_time[2],
              p_consume->record_info[i + start].start_time[3],
              p_consume->record_info[i + start].start_time[4],
              p_consume->record_info[i + start].start_time[5],
              p_consume->record_info[i + start].start_time[6]);   
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str); 

      list_set_field_content_by_strid(ctrl, (u16)(start + i), 2, p_consume->record_info[i + start].state_flag?
       IDS_CHARGE:IDS_CONSUME);

      sprintf((char*)asc_str,"%d",(int)p_consume->record_info[i + start].runningNum);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
      
      sprintf((char*)asc_str,"%d",(int)p_consume->record_info[i + start].value);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 4, asc_str);
    }
  }
  return SUCCESS;
}

RET_CODE open_provider_info(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT 15 
  control_t *p_cont,*p_btn[PROVIDER_ITEMMENU_CNT],*p_list,*p_mbox1,*p_mbox2,*p_mbox3,*p_mbox4;
  u16 i, x, y;
  static sn_comm_help_data_t operate_help_data; //help bar data  
  help_rsc help_item[SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT]=
  { 
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_LEFT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_RIGHT},
	   {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_MOVE},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
 };
  //TRCA full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_CA_CARD_INFO, RSI_TITLE_BG};
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  
  // create container
   p_cont = ui_background_create(ROOT_ID_PROVIDER_INFO,
                            0,0, 
                            640,480,
                            IDS_ACCREDIT_INFO,TRUE);  
  #if 0
  TRCA
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_PROVIDER_INFO, 0, 
                             RSI_FULL_SCREEN_BG,
                             PROVIDER_MENU_X, PROVIDER_MENU_Y,
                             PROVIDER_MENU_W, PROVIDER_MENU_H,
                             &title_data);
  #endif
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  
  p_entitle = NULL;
  
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, provider_info_proc);
 // ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);
  //first line title 
  for(i=0; i<PROVIDER_ITEMMENU_CNT; i++)
  {
    x = PROVIDER_TITLE_BTN_X + PROVIDER_TITLE_BTN_W*i +5;
    y = PROVIDER_TITLE_BTN_Y - 65;
    p_btn[i] = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_TITLE1+ i),
                              x, y, PROVIDER_TITLE_BTN_W,PROVIDER_TITLE_BTN_H,
                              p_cont, 0);
    ctrl_set_keymap(p_btn[i], provider_info_button_keymap);
    ctrl_set_proc(p_btn[i], provider_info_button_proc);
    ctrl_set_rstyle(p_btn[i],RSI_SN_SET_BUTTON_SELECT,RSI_SN_SET_BUTTON_SELECTH,RSI_SN_SET_BUTTON_SELECTH);//RSI_SECOND_TITLE, RSI_SECOND_TITLE_HL, RSI_SECOND_TITLE);
    text_set_font_style(p_btn[i], FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], menu_btn_str[i]);
  }
      
  //LIST1
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_PROVIDER_INFO_LIST1,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y-60, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H-60, p_cont, 0);
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_ACTIVE);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, PROVIDER_LIST_BG_MIDL, PROVIDER_LIST_BG_MIDT,
           PROVIDER_LIST_BG_MIDW, PROVIDER_LIST_BG_MIDH,PROVIDER_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, CAS_MAX_PRODUCT_ENTITLE_NUM, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD3, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list1_update, 0);
 
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list1_attr[i].attr), (u16)(provider_info_list1_attr[i].width),
                        (u16)(provider_info_list1_attr[i].left), (u8)(provider_info_list1_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list1_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list1_attr[i].fstyle);
  }
  //provider_info_list1_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //LIST2
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_PROVIDER_INFO_LIST2,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y-60, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H-60, p_cont, 0);
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, PROVIDER_LIST_BG_MIDL, PROVIDER_LIST_BG_MIDT,
           PROVIDER_LIST_BG_MIDW, PROVIDER_LIST_BG_MIDH,PROVIDER_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 6, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD4, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list2_update, 0);
  for (i = 0; i < PROVIDER_LIST_FIELD4; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list2_attr[i].attr), (u16)(provider_info_list2_attr[i].width),
                        (u16)(provider_info_list2_attr[i].left), (u8)(provider_info_list2_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list2_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list2_attr[i].fstyle);
  }
  //provider_info_list2_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //LIST3
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_PROVIDER_INFO_LIST3,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y-60, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H-60, p_cont, 0);
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, PROVIDER_LIST_BG_MIDL, PROVIDER_LIST_BG_MIDT,
           PROVIDER_LIST_BG_MIDW, PROVIDER_LIST_BG_MIDH,PROVIDER_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 8, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD3, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list3_update, 0);
 
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list3_attr[i].attr), (u16)(provider_info_list3_attr[i].width),
                        (u16)(provider_info_list3_attr[i].left), (u8)(provider_info_list3_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list3_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list3_attr[i].fstyle);
  }
  //provider_info_list3_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //LIST4
  p_list = ctrl_create_ctrl((u8*)CTRL_LIST, IDC_PROVIDER_INFO_LIST4,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y-60, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H-60, p_cont, 0);
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_list, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, PROVIDER_LIST_BG_MIDL, PROVIDER_LIST_BG_MIDT,
           PROVIDER_LIST_BG_MIDW, PROVIDER_LIST_BG_MIDH,PROVIDER_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 8, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD5, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list4_update, 0);
 
  for (i = 0; i < PROVIDER_LIST_FIELD5; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list4_attr[i].attr), (u16)(provider_info_list4_attr[i].width),
                        (u16)(provider_info_list4_attr[i].left), (u8)(provider_info_list4_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list4_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list4_attr[i].fstyle);
  }
  //provider_info_list4_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //mbox1
  p_mbox1 = ctrl_create_ctrl((u8*)CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD1,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y-60,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox1, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_SECOND_TITLE,RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_attr(p_mbox1, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox1, 1);
  mbox_enable_string_mode(p_mbox1, TRUE);
  mbox_set_content_strtype(p_mbox1, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox1, PROVIDER_LIST_FIELD3, PROVIDER_LIST_FIELD3, 1);
  mbox_set_item_rect(p_mbox1, 0, 0, PROVIDER_MBOX_BTN_W-80, PROVIDER_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox1, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  mbox_set_string_offset(p_mbox1, 0, 0);
  mbox_set_string_align_type(p_mbox1, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    mbox_set_content_by_strid(p_mbox1, i, list1_title_str[i]);
  }
  
  //mbox2
  p_mbox2 = ctrl_create_ctrl((u8*)CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD2,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y-60,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox2, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_SECOND_TITLE,RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox2, 0);
  mbox_enable_string_mode(p_mbox2, TRUE);
  mbox_set_content_strtype(p_mbox2, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox2, PROVIDER_LIST_FIELD4, PROVIDER_LIST_FIELD4, 1);
  mbox_set_item_rect(p_mbox2, 0, 0, PROVIDER_MBOX_BTN_W, PROVIDER_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox2, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  mbox_set_string_offset(p_mbox2, 0, 0);
  mbox_set_string_align_type(p_mbox2, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD4; i++)
  {
    mbox_set_content_by_strid(p_mbox2, i, list2_title_str[i]);
  }
  
  //mbox3
  p_mbox3 = ctrl_create_ctrl((u8*)CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD3,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y-60,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox3, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_SECOND_TITLE,RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox3, 0);
  mbox_enable_string_mode(p_mbox3, TRUE);
  mbox_set_content_strtype(p_mbox3, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox3, PROVIDER_LIST_FIELD3, PROVIDER_LIST_FIELD3, 1);
  mbox_set_item_rect(p_mbox3, 0, 0, PROVIDER_MBOX_BTN_W, PROVIDER_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox3, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  mbox_set_string_offset(p_mbox3, 0, 0);
  mbox_set_string_align_type(p_mbox3, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    mbox_set_content_by_strid(p_mbox3, i, list3_title_str[i]);
  }

  //mbox4
  p_mbox4 = ctrl_create_ctrl((u8*)CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD4,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y-60,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox4, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);//RSI_SECOND_TITLE,RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox4, 0);
  mbox_enable_string_mode(p_mbox4, TRUE);
  mbox_set_content_strtype(p_mbox4, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox4, PROVIDER_LIST_FIELD5, PROVIDER_LIST_FIELD5, 1);
  mbox_set_item_rect(p_mbox4, 0, 0, PROVIDER_MBOX_BTN_W, PROVIDER_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox4, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  mbox_set_string_offset(p_mbox4, 0, 0);
  mbox_set_string_align_type(p_mbox4, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD5; i++)
  {
    mbox_set_content_by_strid(p_mbox4, i, list4_title_str[i]);
  }
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(cus_cfg.customer == CUSTOMER_TONGGUANG
    || cus_cfg.customer == CUSTOMER_YINHE
    || cus_cfg.customer == CUSTOMER_TONGGUANG_CG
    || cus_cfg.customer == CUSTOMER_JINYA
    || cus_cfg.customer == CUSTOMER_TONGGUANG_QY
    || cus_cfg.customer == CUSTOMER_PANLONGHU_TR)
  {
    //TRCA ui_comm_help_create2(&provider_help_data2, p_cont, FALSE);
  }
  else
  {  
    //TRCA ui_comm_help_create(&provider_help_data, p_cont);
  }
  memset((void*)&operate_help_data, 0, sizeof(sn_comm_help_data_t));
  operate_help_data.x=0;
  operate_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT;
  operate_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  operate_help_data.offset=37;
  operate_help_data.rsc =help_item;
  sn_ui_comm_help_create(&operate_help_data, p_cont);
  

  ctrl_default_proc(p_btn[g_sub_curn_index], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);
  ui_ca_get_info(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE), 0);
  ui_ca_get_info(CAS_CMD_IPPV_INFO_GET, 0, 0);
  ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0, 0);
  ui_ca_get_info(CAS_CMD_IPP_REC_INFO_GET, 0, 0);
  return SUCCESS;
}

#if 0
static RET_CODE on_change_focus_to_list(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = p_ctrl->p_parent;
  control_t *p_next_ctrl = NULL;
  u8 id;
  u16 num = 0;
  id = ctrl_get_ctrl_id(p_ctrl);

  switch(id)
  {
    case IDC_TITLE1:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
      break;
      
    case IDC_TITLE2:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
      break;
      
    case IDC_TITLE3:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
      break;

    case IDC_TITLE4:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
      break;
  }
  
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  
  ctrl_set_attr(p_ctrl, OBJ_ATTR_INACTIVE);
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  
  return SUCCESS;
}
#endif

/*
static RET_CODE on_provider_info_list_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 id;
  u16 valid_pos = 0;
  control_t *p_cont = p_ctrl->p_parent;
  control_t *p_list = NULL;
  id = ctrl_get_ctrl_id(p_ctrl);

  switch(id)
  {
    case IDC_TITLE1:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
      break;
      
    case IDC_TITLE2:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
      break;
      
    case IDC_TITLE3:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
      break;

    case IDC_TITLE4:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
      break;
  }
  
  list_class_proc(p_list, msg, para1, para2);

  if(id == IDC_TITLE1)
  {
    p_entitle = NULL;
    valid_pos = list_get_valid_pos(p_list);
    UI_PRINTF("on_provider_info_list_msg valid_pos[%d]\n",valid_pos);
    //TRCA ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (valid_pos<<16)|(valid_pos+PROVIDER_LIST_PAGE), 0);
  }
  else
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }
  
  return SUCCESS;
}
*/

static RET_CODE on_ca_info_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      #if 0
      curn_index++;
      if(curn_index >2)
      {
        curn_index = 0;
      }
      #endif
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));      
      break;
      
    case MSG_FOCUS_LEFT:
      #if 0
      curn_index--;
      if(curn_index >2)
      {
        curn_index = 2;
      }
      #endif
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));
      break;
      
    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, 0, 0);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_set_attr(p_next_ctrl, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}
	
#if 0
static RET_CODE on_provider_info_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  u16 num = 0;
  
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      #if 0
      curn_index++;
      if(curn_index >2)
      {
        curn_index = 0;
      }
      #endif
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));      
      break;
      
    case MSG_FOCUS_LEFT:
      #if 0
      curn_index--;
      if(curn_index >2)
      {
        curn_index = 2;
      }
      #endif
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, 0, 0);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_set_attr(p_next_ctrl, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}
#endif 

static RET_CODE on_provider_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl, *p_mbox1, *p_mbox2, *p_mbox3,*p_mbox4,*p_list1,*p_list2,*p_list3,*p_list4;
  u16 num = 0;
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      curn_index++;
      if(curn_index >3)
      {
        curn_index = 0;
      }
      break;
    case MSG_FOCUS_LEFT:
      curn_index--;
      if(curn_index >3)
      {
      curn_index = 3;
      }
      break;
    default:
      break;
  }
  p_cont = p_ctrl->p_parent;
  p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));
  num = (IDC_TITLE1 + curn_index);
  
  p_list1 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
  p_list2 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
  p_list3 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
  p_list4 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
  p_mbox1 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD1);
  p_mbox2 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD2);
  p_mbox3 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD3);
  p_mbox4 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD4);
  
  if(num == IDC_TITLE1)
  {
    ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox1, OBJ_ATTR_ACTIVE);
    
    ctrl_set_attr(p_list2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list1, OBJ_ATTR_ACTIVE);
    
    ctrl_paint_ctrl(p_list1, TRUE);
    ctrl_paint_ctrl(p_mbox1, TRUE);
  }
  else if(num == IDC_TITLE2)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_ACTIVE);
    
    ctrl_set_attr(p_list1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list2, OBJ_ATTR_ACTIVE);
    
    ctrl_paint_ctrl(p_list2, TRUE);
    ctrl_paint_ctrl(p_mbox2, TRUE);
  }
  else if(num == IDC_TITLE3)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_ACTIVE);
    
    ctrl_set_attr(p_list1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_ACTIVE);
    
    ctrl_paint_ctrl(p_list3, TRUE);
    ctrl_paint_ctrl(p_mbox3, TRUE);
  }
  else if(num == IDC_TITLE4)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_ACTIVE);
    
    ctrl_set_attr(p_list1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_ACTIVE);
    
    ctrl_paint_ctrl(p_list4, TRUE);
    ctrl_paint_ctrl(p_mbox4, TRUE);
  }
  
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_provider_info_entitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST1);

  p_entitle = (product_entitles_info_t *)para2;

  if(p_entitle != NULL)
  {
    memcpy(&entitle_info, p_entitle, sizeof(product_entitles_info_t));
  }
  
  //list_set_count(p_list, p_entitle->max_num, PROVIDER_LIST_PAGE);
  
  provider_info_list1_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_provider_info_ippv(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST2);

  p_ippv = (ipps_info_t *)para2;
  
  list_set_count(p_list, p_ippv->max_num, PROVIDER_LIST_PAGE);

  provider_info_list2_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_provider_info_burse(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST3);

  p_burse = (burses_info_t *)para2;

  list_set_count(p_list, p_burse->max_num, PROVIDER_LIST_PAGE);
  
  provider_info_list3_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_provider_info_consume(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST4);

  p_consume = (ipp_total_record_info_t *)para2;

  list_set_count(p_list, (u16)p_consume->max_num, PROVIDER_LIST_PAGE);

  provider_info_list4_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

#if 0
static RET_CODE on_provider_info_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  p_entitle = NULL;
  p_ippv = NULL;
  p_burse = NULL;
  p_consume = NULL;
  return SUCCESS;
}
#endif

static RET_CODE on_provider_info_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  p_entitle = NULL;
  p_ippv = NULL;
  p_burse = NULL;
  p_consume = NULL;
  switch(msg)
  {
    case MSG_EXIT:
      manage_close_menu(ROOT_ID_PROVIDER_INFO, para1, para2);
      break;
      
    case MSG_EXIT_ALL:
      ui_close_all_mennus();
      break;
  }
  
  return SUCCESS;
}

static RET_CODE on_provider_info_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;
  
  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_get_info(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE), 0);
      ui_ca_get_info(CAS_CMD_IPPV_INFO_GET, 0, 0);
      ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0, 0);
      ui_ca_get_info(CAS_CMD_IPP_REC_INFO_GET, 0, 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE)
      {
        //on_conditional_accept_info_update(p_cont, 0, 0, (u32)NULL);
        ui_ca_get_info(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE), 0);
        ui_ca_get_info(CAS_CMD_IPPV_INFO_GET, 0, 0);
        ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0, 0);
        ui_ca_get_info(CAS_CMD_IPP_REC_INFO_GET, 0, 0);
      }
      break;
  }

  return SUCCESS;
}

static RET_CODE on_change_focus_to_list(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = p_ctrl->p_parent;
  control_t *p_next_ctrl = NULL;
  u8 id;
  id = ctrl_get_ctrl_id(p_ctrl);

  switch(id)
  {
    case IDC_TITLE1:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
      break;
      
    case IDC_TITLE2:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
      break;
      
    case IDC_TITLE3:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
      break;
      
    case IDC_TITLE4:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
      break;
    default:
      p_next_ctrl = NULL;
      break;
  }
  if(NULL == p_next_ctrl)
  {
      return ERR_FAILURE;
  }
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  
  ctrl_set_attr(p_ctrl, OBJ_ATTR_INACTIVE);
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(ctrl_get_root(p_next_ctrl),FALSE);
  return SUCCESS;
}



BEGIN_MSGPROC(provider_info_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_ENT_INFO, on_provider_info_entitle)
	ON_COMMAND(MSG_CA_IPV_INFO, on_provider_info_ippv)
	ON_COMMAND(MSG_CA_BUR_INFO, on_provider_info_burse)
	ON_COMMAND(MSG_CA_IPP_REC_INFO, on_provider_info_consume)
	//ON_COMMAND(MSG_SAVE, on_provider_info_save)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_provider_info_accept_notify)
	ON_COMMAND(MSG_CA_INIT_OK, on_provider_info_accept_notify)
END_MSGPROC(provider_info_proc, ui_comm_root_proc) 

BEGIN_KEYMAP(provider_info_button_keymap, NULL)
	/*
	ON_EVENT(V_KEY_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
	*/
 	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)	
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)

	ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(provider_info_button_keymap, NULL)

BEGIN_MSGPROC(provider_info_button_proc, text_class_proc)
	/*  ON_COMMAND(MSG_FOCUS_LEFT, on_provider_item_change_focus)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_provider_item_change_focus)

	//ON_COMMAND(MSG_FOCUS_DOWN, on_change_focus_to_list)
	//ON_COMMAND(MSG_FOCUS_UP, on_change_focus_to_list)

	ON_COMMAND(MSG_PAGE_DOWN, on_provider_info_list_msg)
	ON_COMMAND(MSG_PAGE_UP, on_provider_info_list_msg)
	ON_COMMAND(MSG_EXIT, on_provider_info_exit)
	ON_COMMAND(MSG_EXIT_ALL, on_provider_info_exit)  
	*/
	ON_COMMAND(MSG_FOCUS_LEFT, on_provider_item_change_focus)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_provider_item_change_focus)
	ON_COMMAND(MSG_FOCUS_DOWN, on_change_focus_to_list)
    ON_COMMAND(MSG_FOCUS_UP, on_change_focus_to_list)
    ON_COMMAND(MSG_EXIT, on_provider_info_exit)
    ON_COMMAND(MSG_EXIT_ALL, on_provider_info_exit)   
END_MSGPROC(provider_info_button_proc, text_class_proc)
 
BEGIN_KEYMAP(provider_info_list_keymap, NULL)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)    
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    //
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)//
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	//ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(provider_info_list_keymap, NULL)

BEGIN_MSGPROC(provider_info_list_proc, list_class_proc)
	ON_COMMAND(MSG_FOCUS_LEFT, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_UP, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ca_info_list_msg)
	ON_COMMAND(MSG_PAGE_DOWN, on_ca_info_list_msg)
	ON_COMMAND(MSG_PAGE_UP, on_ca_info_list_msg)
    //N_COMMAND(MSG_SELECT, on_provider_item_select)
END_MSGPROC(provider_info_list_proc, list_class_proc)

