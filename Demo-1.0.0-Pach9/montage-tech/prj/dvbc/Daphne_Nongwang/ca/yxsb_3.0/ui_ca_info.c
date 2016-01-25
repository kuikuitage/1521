/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_info.h"
#include "sys_dbg.h"


enum control_id
{
  IDC_INVALID = 0,
    
  IDC_BG,
  
  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
  
  IDC_CA_INFO_HEAD1,
  IDC_CA_INFO_HEAD2,
  IDC_CA_INFO_HEAD3,
  IDC_CA_INFO_HEAD4,
  
  IDC_CA_INFO_LIST_ENTITLE,
  IDC_CA_INFO_LIST_BURSE,
  IDC_CA_INFO_LIST_IPPV,
  IDC_CA_INFO_LIST_CHARACTERS,
};

static list_xstyle_t ca_info_item_rstyle =
{
/*QQ	RSI_COMM_LIST_N,
		RSI_COMM_LIST_N,
		RSI_COMM_LIST_HL,
		RSI_COMM_LIST_SEL,
		RSI_COMM_LIST_HL,

*/
        RSI_SN_BAR_YELLOW_HL,
	    RSI_SN_BG,
		RSI_SN_BAR_YELLOW_HL,
		RSI_SN_BG,
		RSI_SN_BAR_YELLOW_HL



};

static list_xstyle_t ca_info_field_fstyle =
{
/*QQ
  FSI_LIST_TXT_G,
  FSI_LIST_TXT_N,
  FSI_LIST_TXT_HL,
  FSI_LIST_TXT_SEL,
  FSI_LIST_TXT_HL,
 */
	FSI_LIST_TXT_G,
	FSI_LIST_TXT_N,
	FSI_LIST_TXT_HL,
	FSI_LIST_TXT_SEL,
	FSI_LIST_TXT_HL,

};

static list_xstyle_t ca_info_field_rstyle =
{
/*QQ
  RSI_WHITE_YELLOW,
  //QQ   RSI_SUB_BUTTON_SH,
  RSI_WHITE_YELLOW,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  */
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,

};

static list_field_attr_t ca_info_list_entitle_attr[CA_INFO_ENTITLE_FIELD] =
{
  { LISTFIELD_TYPE_DEC,
    100, 30, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 215, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    130, 390, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
};

static list_field_attr_t ca_info_list_burse_attr[CA_INFO_BURSE_FIELD] =
{
  { LISTFIELD_TYPE_DEC,
    120, 30, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    140, 215, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    140, 430, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
};

static list_field_attr_t ca_info_list_ippv_attr[CA_INFO_IPPV_FIELD] =
{
  { LISTFIELD_TYPE_DEC | STL_VCENTER| STL_VCENTER,
    60, 10, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    120, 80, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    70, 200, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    60, 310, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_DEC | STL_VCENTER | STL_VCENTER,
    40, 410, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle }, 
    
  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    100, 460, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },   
};

static list_field_attr_t ca_info_list_characters_attr[CA_INFO_CHARACTERS_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_VCENTER| STL_VCENTER,
    100, 30, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    100, 215, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_DEC | STL_VCENTER | STL_VCENTER,
    100, 430, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
};

static s8 curn_index = 0;
static u8 g_sub_curn_index = 0;
static u16 itemmenu1_btn_str[4] =
{
  IDS_ACCREDIT_INFO, IDS_SLOT_INFO, IDS_IPPV_INFO,IDS_CHARACTERS,
};
static u16 itemmenu2_btn_str[3] =
{
  IDS_PRODUCT_ID, IDS_RECORDABLE, IDS_AUTHORIZE_DEADLINE
};
static u16 itemmenu3_btn_str[3] =
{
  IDS_SLOT_ID, IDS_SLOT_POINT, IDS_CONSUMED_POINT
};
static u16 itemmenu4_btn_str[6] =
{
  IDS_SLOT_ID, IDS_PROGRAM_ID, IDS_BROADCAST_STATE, IDS_RECORDABLE,IDS_PRICE2,IDS_AUTHORIZE_DEADLINE
};
static u16 itemmenu5_btn_str[3] =
{
 IDS_AREA_CODE,IDS_BOUQUET_ID,IDS_CHARACTERS
};

static product_entitles_info_t *p_entitle = NULL;
static ipps_info_t *p_ippv = NULL;
static burses_info_t *p_burse = NULL;
static u32 *p_aclist = NULL;
static cas_operator_info_t *g_oper_entitle_info = NULL;

RET_CODE ca_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_info_button_keymap(u16 key);
RET_CODE ca_info_button_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_info_list_keymap(u16 key);
RET_CODE ca_info_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE ca_info_list_entitle_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u8 asc_str[16];
  
  OS_PRINTF("ca_info_list_entitle_update start = %d, size = %d,cnt = %d\n",start,size,cnt);
  
  if(p_entitle == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_dec(p_list, 
        (u16)(start + i), 0, p_entitle->p_entitle_info[i + start].product_id); 

      //recorded
      list_set_field_content_by_ascstr(p_list, 
        (u16)(start + i), 1, p_entitle->p_entitle_info[i + start].can_tape?(u8 *)"Yes":(u8 *)"NO"); 

      sprintf((char *)asc_str,"%04d.%2d.%2d",p_entitle->p_entitle_info[i + start].expired_time[0] * 100 + 
                p_entitle->p_entitle_info[i + start].expired_time[1], 
                p_entitle->p_entitle_info[i + start].expired_time[2],
                p_entitle->p_entitle_info[i + start].expired_time[3]);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str);         
    }
  }
  return SUCCESS;
}

static RET_CODE ca_info_list_burse_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u8 asc_buf[32];
  u16 temp[20];
  OS_PRINTF("ca_info_list_burse_update start = %d, size = %d,cnt = %d\n",start,size,cnt);

  if(p_burse == NULL)
  {
    return ERR_FAILURE;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_dec(p_list, 
        (u16)(start + i), 0, (s32)(i + start+1)); 

      sprintf((char *)asc_buf,"%lu",p_burse->p_burse_info[i + start].credit_limit);
      str_asc2uni(asc_buf, temp);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, temp);
      
      OS_PRINTF("ca_info_list_burse_update start credit_limit = 0x%x\n",p_burse->p_burse_info[i + start].credit_limit);
      
      sprintf((char *)asc_buf,"%lu",p_burse->p_burse_info[i + start].balance);
      str_asc2uni(asc_buf, temp);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, temp);
      
      OS_PRINTF("ca_info_list_burse_update start balance = 0x%x\n",p_burse->p_burse_info[i + start].balance);
    }
  }
  return SUCCESS;
}

static RET_CODE ca_info_list_ippv_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u8 asc_str[16];
  u16 temp[20];
  u16 i, cnt = list_get_count(p_list);
  OS_PRINTF("ca_info_list_ippv_update start = %d, size = %d,cnt = %d\n",start,size,cnt);

  if(p_ippv == NULL)
  {
    return ERR_FAILURE;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      //slot_id
      list_set_field_content_by_dec(p_list, 
        (u16)(start + i), 0, p_ippv->p_ipp_info[i + start].burse_id); 

      //product_id
      sprintf((char *)asc_str,"%lu",p_ippv->p_ipp_info[i + start].product_id);
      str_asc2uni(asc_str, temp);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, temp);
      //list_set_field_content_by_dec(p_list, 
         //(u16)(start + i), 1, p_ippv->p_ipp_info[i + start].product_id); 

      //product state
      list_set_field_content_by_ascstr(p_list, 
         (u16)(start + i), 2, p_ippv->p_ipp_info[i + start].book_state_flag?(u8 *)"viewed":(u8 *)"book"); 

      //recorded
      list_set_field_content_by_ascstr(p_list, 
         (u16)(start + i), 3, p_ippv->p_ipp_info[i + start].is_can_tape?(u8 *)"Yes":(u8 *)"NO"); 

      //price
      list_set_field_content_by_dec(p_list, 
         (u16)(start + i), 4, p_ippv->p_ipp_info[i + start].price); 

      //over data
      sprintf((char *)asc_str,"%04d.%2d.%2d",p_ippv->p_ipp_info[i + start].expired_time[0] * 100 + 
                p_ippv->p_ipp_info[i + start].expired_time[1], 
                p_ippv->p_ipp_info[i + start].expired_time[2],
                p_ippv->p_ipp_info[i + start].expired_time[3]);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 5, asc_str);   
    }
  }
  return SUCCESS;
}

static RET_CODE ca_info_list_characters_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u8 p_asc[2] = {0,};
  OS_PRINTF("ca_info_list_characters_update start = %d, size = %d,cnt = %d\n",start,size,cnt);
  
  if(p_aclist == NULL)
  {
    return ERR_FAILURE;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      if(i == 0)//区域码和业务标识符只显示一次就可以
      {
        p_asc[0] = p_aclist[0] + '0';
        p_asc[1] = '\0';
        list_set_field_content_by_ascstr(p_list, 
        (u16)(start + i), 0, p_asc); 
        
        p_asc[0] = p_aclist[1] + '0';
        p_asc[1] = '\0';
        list_set_field_content_by_ascstr(p_list, 
         (u16)(start + i), 1, p_asc); 
      }
      else
      {
        p_asc[0] = ' ';
        p_asc[1] = '\0';
        list_set_field_content_by_ascstr(p_list, 
        (u16)(start + i), 0, p_asc); 

        list_set_field_content_by_ascstr(p_list, 
         (u16)(start + i), 1, p_asc); 
      }
      list_set_field_content_by_dec(p_list, 
         (u16)(start + i), 2, p_aclist[start + i + 4]);    //0:区域码，1:业务描述符，2,3预留，4-9:特征值
    }
  }
  
  return SUCCESS;
}

RET_CODE open_ca_info(u32 para1, u32 para2)
{
 #define SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT 15 
  control_t *p_cont,*p_btn[ITEMMENU_CNT],*p_list,*p_mbox1,*p_mbox2,*p_mbox3,*p_mbox4;//*p_bg;
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
 



/*QQ
  // create container
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_CA_INFO, 
                             RSI_SUBMENU_BG,
                             CA_CONT_X, CA_CONT_Y,
                             CA_CONT_W, CA_CONT_H,
                             &title_data);
*/
/*QQ
  p_cont = ui_comm_prev_root_create(ROOT_ID_CA_INFO, 
							 RSI_ORANGE,                //QQ  RSI_SUBMENU_BG,
							 CA_CONT_X, CA_CONT_Y,
							 CA_CONT_W, CA_CONT_H,
							 &title_data);


  p_cont = ui_background_create(ROOT_ID_CA_INFO,
						   0, 0,
						   SCREEN_WIDTH, SCREEN_HEIGHT,
						   IDS_CA_INFO,TRUE);
  */


  p_cont = ui_background_create(ROOT_ID_CA_INFO,
                            0,0, //CA_CONT_X, CA_CONT_Y,
                            640,480,//CA_CONT_W, CA_CONT_H,
                             IDS_CA_INFO,TRUE);

  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_info_proc);
  
   OS_PRINTF("!!!!!!!!!!!!!!!!!!!operator max num\n");
#ifndef WIN32
  g_oper_entitle_info = (cas_operator_info_t *)para2;
  OS_PRINTF("------------------g_oper_entitle_info->operator_id = %d\n",g_oper_entitle_info->operator_id);
#endif

  //title 
  for(i=0; i<ITEMMENU_CNT; i++)
  {
    x = ITEMMENU_BTN_X + ITEMMENU_BTN_W*i;
    y = ITEMMENU_BTN_Y -65;
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_TITLE1+ i),
                              x, y, (u16)ITEMMENU_BTN_W,(u16)ITEMMENU_BTN_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_btn[i],
                RSI_SN_SET_BUTTON_SELECT,
                RSI_SN_SET_BUTTON_SELECTH,
                RSI_SN_SET_BUTTON_SELECTH);
    ctrl_set_keymap(p_btn[i], ca_info_button_keymap);
    ctrl_set_proc(p_btn[i], ca_info_button_proc);
    text_set_font_style(p_btn[i], FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], itemmenu1_btn_str[i]);
  }


  //entitle list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_INFO_LIST_ENTITLE,
                           CA_PLIST_BG_X-15, CA_PLIST_BG_Y-60, CA_PLIST_BG_W,CA_PLIST_BG_H-70, p_cont, 0);
  ctrl_set_keymap(p_list, ca_info_list_keymap);
  ctrl_set_proc(p_list, ca_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_ACTIVE);
 //QQ  ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, CA_LIST_BG_MIDL, CA_LIST_BG_MIDT,
           CA_LIST_BG_MIDW, CA_LIST_BG_MIDH,CA_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_count(p_list, 0, CA_INFO_LIST_PAGE);
  list_set_field_count(p_list, CA_INFO_ENTITLE_FIELD, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, ca_info_list_entitle_update, 0);
  //ctrl_paint_ctrl(p_list,FALSE);
  for (i = 0; i < CA_INFO_ENTITLE_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,
      (u32)(ca_info_list_entitle_attr[i].attr),
      (u16)(ca_info_list_entitle_attr[i].width),
      (u16)(ca_info_list_entitle_attr[i].left),
      (u8)(ca_info_list_entitle_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,
      ca_info_list_entitle_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,
      ca_info_list_entitle_attr[i].fstyle);
  }

  //burset list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_INFO_LIST_BURSE,
                           CA_PLIST_BG_X-15, CA_PLIST_BG_Y-60, CA_PLIST_BG_W,CA_PLIST_BG_H-70, p_cont, 0);
  ctrl_set_keymap(p_list, ca_info_list_keymap);
  ctrl_set_proc(p_list, ca_info_list_proc);
  //QQ  ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
 // ctrl_set_attr(p_list, OBJ_ATTR_ACTIVE);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, CA_LIST_BG_MIDL, CA_LIST_BG_MIDT,
           CA_LIST_BG_MIDW, CA_LIST_BG_MIDH,CA_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);

  list_set_field_count(p_list, CA_INFO_BURSE_FIELD, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, ca_info_list_burse_update, 0);
 // ctrl_paint_ctrl(p_list,FALSE);
  for (i = 0; i < CA_INFO_BURSE_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,
      (u32)(ca_info_list_burse_attr[i].attr),
      (u16)(ca_info_list_burse_attr[i].width),
      (u16)(ca_info_list_burse_attr[i].left),
      (u8)(ca_info_list_burse_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,
      ca_info_list_burse_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,
      ca_info_list_burse_attr[i].fstyle);
  }

  //ippv list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_INFO_LIST_IPPV,
                           CA_PLIST_BG_X-15, CA_PLIST_BG_Y-60, CA_PLIST_BG_W,CA_PLIST_BG_H-70, p_cont, 0);
  ctrl_set_keymap(p_list, ca_info_list_keymap);
  ctrl_set_proc(p_list, ca_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
  //QQ  ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, CA_LIST_BG_MIDL, CA_LIST_BG_MIDT,
           CA_LIST_BG_MIDW, CA_LIST_BG_MIDH,CA_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_field_count(p_list, CA_INFO_IPPV_FIELD, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, ca_info_list_ippv_update, 0);
 // ctrl_paint_ctrl(p_list,FALSE);
 
  for (i = 0; i < CA_INFO_IPPV_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,
      (u32)(ca_info_list_ippv_attr[i].attr),
      (u16)(ca_info_list_ippv_attr[i].width),
      (u16)(ca_info_list_ippv_attr[i].left),
      (u8)(ca_info_list_ippv_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,
      ca_info_list_ippv_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,
      ca_info_list_ippv_attr[i].fstyle);
  } 
  

  //characters list
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_INFO_LIST_CHARACTERS,
                           CA_PLIST_BG_X-15, CA_PLIST_BG_Y-60, CA_PLIST_BG_W,CA_PLIST_BG_H-70, p_cont, 0);
  ctrl_set_keymap(p_list, ca_info_list_keymap);
  ctrl_set_proc(p_list, ca_info_list_proc);
  ctrl_set_attr(p_list, OBJ_ATTR_HIDDEN);
  //QQ  ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_mid_rect(p_list, CA_LIST_BG_MIDL, CA_LIST_BG_MIDT,
           CA_LIST_BG_MIDW, CA_LIST_BG_MIDH,CA_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_field_count(p_list, CA_INFO_CHARACTERS_FIELD, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, ca_info_list_characters_update, 0);
  
 
  for (i = 0; i < CA_INFO_CHARACTERS_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,
      (u32)(ca_info_list_characters_attr[i].attr),
      (u16)(ca_info_list_characters_attr[i].width),
      (u16)(ca_info_list_characters_attr[i].left),
      (u8)(ca_info_list_characters_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,
      ca_info_list_characters_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,
      ca_info_list_characters_attr[i].fstyle);
  }  
  
  //mbox1
  p_mbox1 = ctrl_create_ctrl((u8 *)CTRL_MBOX, (u8)IDC_CA_INFO_HEAD1,
                            CA_MBOX_BTN_X-20,
                            CA_MBOX_BTN_Y-60,
                            CA_MBOX_BTN_W,
                            CA_MBOX_BTN_H,
                            p_cont, 0);
  //QQ  ctrl_set_rstyle(p_mbox1, RSI_TIP_BOX,RSI_COMMON_BTN_HL, RSI_TIP_BOX);
  ctrl_set_rstyle(p_mbox1, RSI_SN_BG,RSI_SN_BG, RSI_SN_BG);
  ctrl_set_attr(p_mbox1, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox1, 1);
  mbox_enable_string_mode(p_mbox1, TRUE);
  mbox_set_content_strtype(p_mbox1, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox1, CA_MBOX1_ITME_COUNT, CA_MBOX1_ITME_COUNT, 1);
  mbox_set_item_rect(p_mbox1, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox1, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  mbox_set_string_offset(p_mbox1, 0, 0);
  mbox_set_string_align_type(p_mbox1, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < CA_MBOX1_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox1, i, itemmenu2_btn_str[i]);
  }

  //mbox2
  p_mbox2 = ctrl_create_ctrl((u8 *)CTRL_MBOX, (u8)IDC_CA_INFO_HEAD2,
                            CA_MBOX_BTN_X-20,
                            CA_MBOX_BTN_Y-60,
                            CA_MBOX_BTN_W,
                            CA_MBOX_BTN_H,
                            p_cont, 0);
  //QQ  ctrl_set_rstyle(p_mbox2, RSI_TIP_BOX,RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_mbox2, RSI_SN_BG,RSI_SN_BG, RSI_SN_BG);
  ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox2, 0);
  mbox_enable_string_mode(p_mbox2, TRUE);
  mbox_set_content_strtype(p_mbox2, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox2, CA_MBOX2_ITME_COUNT, CA_MBOX2_ITME_COUNT, 1);
  mbox_set_item_rect(p_mbox2, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox2, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  mbox_set_string_offset(p_mbox2, 0, 0);
  mbox_set_string_align_type(p_mbox2, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < CA_MBOX2_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox2, i, itemmenu3_btn_str[i]);
  }

  //mbox3
  p_mbox3 = ctrl_create_ctrl((u8 *)CTRL_MBOX, (u8)IDC_CA_INFO_HEAD3,
                            CA_MBOX_BTN_X-20,
                            CA_MBOX_BTN_Y-60,
                            CA_MBOX_BTN_W,
                            CA_MBOX_BTN_H,
                            p_cont, 0);
  //QQ  ctrl_set_rstyle(p_mbox3, RSI_TIP_BOX,RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_mbox3, RSI_SN_BG,RSI_SN_BG, RSI_SN_BG);
  ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox3, 0);
  mbox_enable_string_mode(p_mbox3, TRUE);
  mbox_set_content_strtype(p_mbox3, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox3, CA_MBOX3_ITME_COUNT, CA_MBOX3_ITME_COUNT, 1);
  mbox_set_item_rect(p_mbox3, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox3, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  mbox_set_string_offset(p_mbox3, 0, 0);
  mbox_set_string_align_type(p_mbox3, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < CA_MBOX3_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox3, i, itemmenu4_btn_str[i]);
  }

  //mbox4
  p_mbox4 = ctrl_create_ctrl((u8 *)CTRL_MBOX, (u8)IDC_CA_INFO_HEAD4,
                            CA_MBOX_BTN_X-20,
                            CA_MBOX_BTN_Y-60,
                            CA_MBOX_BTN_W,
                            CA_MBOX_BTN_H,
                            p_cont, 0);
 //QQ  ctrl_set_rstyle(p_mbox4, RSI_TIP_BOX,RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_mbox4, RSI_SN_BG,RSI_SN_BG, RSI_SN_BG);
  ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
  mbox_set_focus(p_mbox4, 0);
  mbox_enable_string_mode(p_mbox4, TRUE);
  mbox_set_content_strtype(p_mbox4, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox4, CA_MBOX4_ITME_COUNT, CA_MBOX4_ITME_COUNT, 1);
  mbox_set_item_rect(p_mbox4, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H, 0, 0);
  mbox_set_string_fstyle(p_mbox4, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  mbox_set_string_offset(p_mbox4, 0, 0);
  mbox_set_string_align_type(p_mbox4, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < CA_MBOX4_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox4, i, itemmenu5_btn_str[i]);
  }
  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
//QQ  ui_comm_help_create(&ca_help_data, p_cont);
  memset((void*)&operate_help_data, 0, sizeof(sn_comm_help_data_t));
  operate_help_data.x=0;
  operate_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT;
  operate_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  operate_help_data.offset=37;
  operate_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&operate_help_data, p_cont);


  ctrl_default_proc(p_btn[g_sub_curn_index], MSG_GETFOCUS, 0, 0);
 // ctrl_paint_ctrl(p_cont, FALSE);
#ifndef WIN32
  OS_PRINTF("END entitle info g_oper_entitle_info->operator_id = %d\n",g_oper_entitle_info->operator_id);
  ui_ca_get_info(CAS_CMD_ENTITLE_INFO_GET, g_oper_entitle_info->operator_id, 0);
  ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, g_oper_entitle_info->operator_id, 0);
  ui_ca_get_info(CAS_CMD_IPPV_INFO_GET, g_oper_entitle_info->operator_id, 0);
  ui_ca_get_info(CAS_CMD_ACLIST_INFO_GET, g_oper_entitle_info->operator_id, 0);
 
#endif
  ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);

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
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_ENTITLE);
      break;
      
    case IDC_TITLE2:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_BURSE);
      break;
      
    case IDC_TITLE3:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_IPPV);
      break;
      
    case IDC_TITLE4:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_CHARACTERS);
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

static RET_CODE on_ca_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl, *p_mbox1, *p_mbox2, *p_mbox3, *p_mbox4, *p_list1,*p_list2,*p_list3,*p_list4;
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
      if(curn_index < 0)
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
  p_list1 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_ENTITLE);
  p_list2 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_BURSE);
  p_list3 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_IPPV);
  p_list4 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST_CHARACTERS);
  p_mbox1 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD1);
  p_mbox2 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD2);
  p_mbox3 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD3);
  p_mbox4 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD4);
  if(num == IDC_TITLE1)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);

    ctrl_set_attr(p_list1, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_list2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    
    ctrl_paint_ctrl((p_list1), TRUE);
    ctrl_paint_ctrl((p_mbox1), TRUE);
  }
  else if(num == IDC_TITLE2)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
    
    ctrl_set_attr(p_list1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list2, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_list3, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    
    ctrl_paint_ctrl((p_list2), TRUE);
    ctrl_paint_ctrl((p_mbox2), TRUE);
  }
  else if(num == IDC_TITLE3)
  {
    ctrl_set_attr(p_mbox1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_mbox3, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_mbox4, OBJ_ATTR_HIDDEN);
    
    ctrl_set_attr(p_list1, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list2, OBJ_ATTR_HIDDEN);
    ctrl_set_attr(p_list3, OBJ_ATTR_ACTIVE);
    ctrl_set_attr(p_list4, OBJ_ATTR_HIDDEN);
    
    ctrl_paint_ctrl((p_list3), TRUE);
    ctrl_paint_ctrl((p_mbox3), TRUE);
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
    
    ctrl_paint_ctrl((p_list4), TRUE);
    ctrl_paint_ctrl((p_mbox4), TRUE);
  }
   
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_ctrl), TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_ca_info_entitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_ENTITLE);

  p_entitle = (product_entitles_info_t *)para2;
  
  UI_PRINTF("@@@@@ p_entitle->max_num = %d\n", p_entitle->max_num);
  list_set_count(p_list, p_entitle->max_num, CA_INFO_LIST_PAGE);

  ca_info_list_entitle_update(p_list, list_get_valid_pos(p_list), CA_INFO_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_ca_info_burse(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_BURSE);

  p_burse = (burses_info_t *)para2;

  list_set_count(p_list, p_burse->max_num, CA_INFO_LIST_PAGE);

  ca_info_list_burse_update(p_list, list_get_valid_pos(p_list), CA_INFO_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_ca_info_ippv(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_IPPV);

  p_ippv = (ipps_info_t *)para2;

  list_set_count(p_list, p_ippv->max_num, CA_INFO_LIST_PAGE);

  ca_info_list_ippv_update(p_list, list_get_valid_pos(p_list), CA_INFO_LIST_PAGE, 0);
  
  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_ca_info_aclist_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_CHARACTERS);
  
  p_aclist = (u32 *)para2;
  
  list_set_count(p_list, 6, CA_INFO_LIST_PAGE);//CDCA_MAXNUM_ACLIST - 2

  ca_info_list_characters_update(p_list, list_get_valid_pos(p_list), CA_INFO_LIST_PAGE, 0);
  
  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_ca_info_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  p_entitle = NULL;
  p_ippv = NULL;
  p_burse = NULL;

  return SUCCESS;
}


BEGIN_MSGPROC(ca_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_ENT_INFO, on_ca_info_entitle)
  ON_COMMAND(MSG_CA_BUR_INFO, on_ca_info_burse)
  ON_COMMAND(MSG_CA_IPV_INFO, on_ca_info_ippv)
  ON_COMMAND(MSG_CA_ACLIST_INFO, on_ca_info_aclist_info)
  
  ON_COMMAND(MSG_SAVE, on_ca_info_save)
END_MSGPROC(ca_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_info_button_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ca_info_button_keymap, NULL)

BEGIN_MSGPROC(ca_info_button_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_ca_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_change_focus_to_list)
  ON_COMMAND(MSG_FOCUS_UP, on_change_focus_to_list)
END_MSGPROC(ca_info_button_proc, text_class_proc)

BEGIN_KEYMAP(ca_info_list_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)    
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  //ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ca_info_list_keymap, NULL)

BEGIN_MSGPROC(ca_info_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_ca_info_list_msg)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_info_list_msg)
  ON_COMMAND(MSG_FOCUS_UP, on_ca_info_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ca_info_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, on_ca_info_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_ca_info_list_msg)
END_MSGPROC(ca_info_list_proc, list_class_proc)

