/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_operate.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_AD_WINDOW,
  IDC_TV_WINDOW,
  IDC_CA_TIP,
  IDC_CA_PRO,
  IDC_TITILE_1,
  IDC_TITILE_2

};
#ifdef WIN32
static cas_operator_info_t gst_operator[CAS_MAX_OPERTOR_NUM] = 
{
    {1,1,"operator1",9,0},
    {0},
    {0},
    {0},
    {0},
};
static cas_operators_info_t gst_operators_info = 
{
    2,
    {{2,1,"operator1",9,0},
      {2,2,"operator2",9,0},},
};
#endif
/*QQ
static comm_help_data_t ca_opero_help_data = //help bar data
{
  3,
  3,
  {
    IDS_BACK,
    IDS_SELECTED,
    //IDS_FN_SELECT,
    IDS_FN_SELECT,
  },
  { 
    IM_SN_DLG_BG_LT,      //QQ  IM_ICON_BACK,
    IM_SN_DLG_BG_LT,      //QQ  IM_ICON_SELECT,
    //IM_ICON_ARROW_LEFT_RIGHT,
    IM_SN_DLG_BG_LT,      //QQ  IM_ICON_ARROW_UP_DOWN,
  },
};
*/

static list_xstyle_t ca_item_rstyle =
{
/*	RSI_SN_BG,     //QQ  RSI_COMM_LIST_N,
	RSI_SN_BG,		//QQ   RSI_COMM_LIST_N,
	RSI_SN_BG,		//QQ   RSI_COMMON_BTN_HL,
	RSI_SN_BG,		//QQ   RSI_COMM_LIST_SEL,
	RSI_SN_BG,		//QQ   RSI_COMM_LIST_N,
*/
	
	RSI_SN_BAR_YELLOW_HL,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL,
	RSI_SN_BG,
	RSI_SN_BAR_YELLOW_HL
};
static list_xstyle_t ca_field_fstyle =
{
	FSI_LIST_TXT_G,
	FSI_LIST_TXT_N,
	FSI_LIST_TXT_HL,
	FSI_LIST_TXT_SEL,
	FSI_LIST_TXT_HL,

};


static list_xstyle_t ca_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,//QQ  RSI_SUB_BUTTON_SH,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t ca_list_attr[CA_LIST_FIELD] =
{
  {LISTFIELD_TYPE_DEC,
  45, 5, 0, &ca_field_rstyle,  &ca_field_fstyle },
  {LISTFIELD_TYPE_DEC,
  200,50, 0, &ca_field_rstyle,  &ca_field_fstyle},

};

static cas_operators_info_t *p_operator = NULL;
u16 cur_pos = 0;

RET_CODE ca_operate_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ca_list_keymap(u16 key);
RET_CODE ca_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

extern BOOL ui_is_smart_card_insert();
static RET_CODE on_ca_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 opr_idx;
  if(NULL == p_operator)
  {
      return ERR_FAILURE;
  }
  #ifdef WIN32
    manage_open_menu(ROOT_ID_CA_INFO, 0, 0);
  #endif
  if(!ui_is_smart_card_insert())
  {
    ui_comm_cfmdlg_open2(NULL,IDS_CA_CARD_NOT_INSERT,NULL,0);
  }
  else
  {
    opr_idx = list_get_focus_pos(p_ctrl);
    OS_PRINTF("@@@@@@@on_ca_item_select!!!!\n");
    OS_PRINTF("operator_id == %d \n",p_operator->p_operator_info[opr_idx].operator_id);
    
    //manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, 0, (u32)(&p_operator->p_operator_info[opr_idx]));
    manage_open_menu(ROOT_ID_CA_INFO, 0, (u32)(&p_operator->p_operator_info[opr_idx]));
    
    #if 0 
    #ifndef WIN32
    OS_PRINTF("@@@@@@@on_ca_item_select!!!!\n");
    OS_PRINTF("operator_id == %d \n",p_operator->p_operator_info[opr_idx].operator_id);
    ui_ca_get_info(CAS_CMD_ENTITLE_INFO_GET, p_operator->p_operator_info[opr_idx].operator_id, 0);
    #else
    on_ca_info_entitle(p_ctrl, 0,0, (u32)(&product_entitle_t));
    #endif
    #endif
  }
  return SUCCESS;
}

static RET_CODE ca_list_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u8 cnt;
  u16 i;
 
  cnt = p_operator->max_num;
  list_set_count(ctrl, cnt, CA_LIST_PAGE);
  list_set_focus_pos(ctrl, cur_pos);
  
  if(p_operator == NULL)
  {
    return ERR_FAILURE;
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      //NO
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, (start + i + 1)); 

      //operator name
      OS_PRINTF("operator_id == %d \n",p_operator->p_operator_info[i + start].operator_id);
     list_set_field_content_by_dec(ctrl, (u16)(start + i), 1, p_operator->p_operator_info[i + start].operator_id);
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_oper_list_change_focus(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;

  old_focus = list_get_focus_pos(p_cont);
  ret = list_class_proc(p_cont, msg, para1, para2);
  new_focus = list_get_focus_pos(p_cont);
  cur_pos = new_focus;

  return SUCCESS;
}

extern cas_operators_info_t *g_get_operator_info();
RET_CODE open_ca_operate(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT 15
  control_t *p_cont,*p_list,*p_ctrl;
  u8 i;
  u16 x, title_width[] = {100, 110};
  u16 stxt_title[2] ={IDS_NUMBER,IDS_SP_ID};
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
 //QQ   full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SYSTEM_SET, RSI_TITLE_BG, IDS_PROG_SP};
  //full_screen_title_t title_data = {IM_SN_BG,IDS_SYSTEM_SET, RSI_ORANGE, IDS_PROG_SP};
 p_cont = ui_background_create(ROOT_ID_CA_OPE,
                            0,0, //CA_CONT_X, CA_CONT_Y,
                            640,480,//CA_CONT_W, CA_CONT_H,
                             IDS_SP,TRUE);
 /*
  p_cont = ui_comm_prev_root_create(ROOT_ID_CA_OPE, RSI_ORANGE,  //QQ  RSI_FULL_SCREEN_BG
                          CA_OPE_CONT_X, CA_OPE_CONT_Y,
                          CA_OPE_CONT_W, CA_OPE_CONT_H,
                          &title_data);
  */
  
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_operate_proc);

  x = 80;
  for(i=0; i<2; i++)
  {
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_TITILE_1 + i),
                              x, 20,
                              title_width[i],
                              35,
                              p_cont, 0);
    
   //QQ   ctrl_set_rstyle(p_ctrl, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
    ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
    text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, stxt_title[i]);
    
    x += title_width[i];
  }


  //LIST
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_PRO,
                           80, CA_OPE_LIST_Y-70, CA_OPE_LIST_W,CA_OPE_LIST_H, p_cont, 0);
  
 //QQ   ctrl_set_rstyle(p_list, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_list, ca_list_keymap);
  ctrl_set_proc(p_list, ca_list_proc);
  list_set_mid_rect(p_list, CA_LIST_MIDL, CA_LIST_MIDT,
                    CA_LIST_MIDW, CA_LIST_MIDH-13,CA_LIST_VGAP);
  list_set_item_rstyle(p_list, &ca_item_rstyle);
  list_set_count(p_list, 0, CA_LIST_PAGE);
  list_set_field_count(p_list, CA_LIST_FIELD, CA_LIST_PAGE); 
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, ca_list_update, 0);
  
  for (i = 0; i < CA_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(ca_list_attr[i].attr), (u16)(ca_list_attr[i].width),
                         (u16)(ca_list_attr[i].left), (u8)(ca_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, ca_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, ca_list_attr[i].fstyle);
  }

  OS_PRINTF("open_ca_operate\n");
  p_operator = g_get_operator_info();
  list_set_count(p_list, p_operator->max_num, CA_LIST_PAGE);
  list_set_focus_pos(p_list, cur_pos);
  ca_list_update(p_list, list_get_valid_pos(p_list), (u16)CA_LIST_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
//QQ   ui_comm_help_create(&ca_opero_help_data, p_cont);
    memset((void*)&operate_help_data, 0, sizeof(sn_comm_help_data_t));
  operate_help_data.x=0;
  operate_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT;
  operate_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  operate_help_data.offset=37;
  operate_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&operate_help_data, p_cont);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_exit_ca_list(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_CA_OPE, 0, 0);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_operate_proc, ui_comm_root_proc)
END_MSGPROC(ca_operate_proc, ui_comm_root_proc)


BEGIN_KEYMAP(ca_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ca_list_keymap, NULL)

BEGIN_MSGPROC(ca_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_ca_item_select)
  ON_COMMAND(MSG_EXIT, on_exit_ca_list)
  ON_COMMAND(MSG_FOCUS_UP, on_oper_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_oper_list_change_focus)
END_MSGPROC(ca_list_proc, list_class_proc)

 
