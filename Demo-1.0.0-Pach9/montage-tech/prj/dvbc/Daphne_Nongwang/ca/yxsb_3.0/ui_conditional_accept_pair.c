/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pair.h"

enum control_id
{
  IDC_INVALID = 0,
};

enum ca_ca_info_id
{
  IDC_FRM_BUTTONS= 1 ,
    
  IDC_STB_CARD_PAIR_STATE,
  IDC_STB_CARD_PAIR_STATE_CONTENT,

  IDC_STB_CARD_PAIR_ID,
  IDC_STB_CARD_PAIR_ID_CONTENT,
};

static list_xstyle_t pair_item_rstyle =
{
/*QQ
  RSI_COMM_LIST_N,
  RSI_COMM_LIST_N,
  RSI_COMM_LIST_HL,
  RSI_COMM_LIST_SEL,
  RSI_COMM_LIST_N,
 */
 RSI_SN_BG,
 RSI_SN_BG,
 RSI_SN_BG,
 RSI_SN_BG,
 RSI_SN_BG,
};

static list_xstyle_t pair_list_field_fstyle =
{
  FSI_SECOND_TITLE,
  FSI_SECOND_TITLE,
  FSI_SECOND_TITLE,
  FSI_SECOND_TITLE,
  FSI_SECOND_TITLE,
};

static list_xstyle_t pair_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t pair_list_attr =
{
    LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    160, 30, 0, 
    &pair_list_field_rstyle,  &pair_list_field_fstyle
};

card_stb_paired_list_t g_card_stb_paired = {0,};

u16 conditional_accept_pair_cont_keymap(u16 key);
RET_CODE conditional_accept_pair_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_conditional_accept_pair(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl,*p_ca_frm;     //
  u8 i;
  u16 x=0, y=0, h=0;
  u16 str[CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_CNT] =
  { 
    IDS_PAIR_STATE,
    IDS_PLAT_ID,
  };
 /*QQ    
  comm_help_data_t ca_pair_help_data = //help bar data
  {
    1,
    1,
    {
      IDS_BACK,
    },
    { 
      IM_ICON_BACK,
    },
  };
  */

 
  /*QQ
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_CONDITIONAL_ACCEPT, RSI_TITLE_BG, IDS_STB_PAIR};
  p_cont = ui_comm_prev_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, RSI_FULL_SCREEN_BG,
                          CONDITIONAL_ACCEPT_PAIR_CONT_X, CONDITIONAL_ACCEPT_PAIR_CONT_Y,
                          CONDITIONAL_ACCEPT_PAIR_CONT_W, CONDITIONAL_ACCEPT_PAIR_CONT_H,
                          &title_data);
*/
//  full_screen_title_t title_data = {IM_SN_BG,IDS_CONDITIONAL_ACCEPT, RSI_ORANGE, IDS_STB_PAIR};
/*
  p_cont = ui_comm_prev_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, RSI_ORANGE,
						  CONDITIONAL_ACCEPT_PAIR_CONT_X, CONDITIONAL_ACCEPT_PAIR_CONT_Y,
						  CONDITIONAL_ACCEPT_PAIR_CONT_W, CONDITIONAL_ACCEPT_PAIR_CONT_H,
						  &title_data);
*/
	p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_PAIR,
	                        CONDITIONAL_ACCEPT_PAIR_CONT_X,CONDITIONAL_ACCEPT_PAIR_CONT_Y, //CA_CONT_X, CA_CONT_Y,
	                        640,480,//CA_CONT_W, CA_CONT_H,
	                        IDS_STB_PAIR,TRUE);
                    
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  } 

   //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              CONDITIONAL_ACCEPT_PAIR_CA_FRM_X-140, CONDITIONAL_ACCEPT_PAIR_CA_FRM_Y-100,
                              CONDITIONAL_ACCEPT_PAIR_CA_FRM_W+60, CONDITIONAL_ACCEPT_PAIR_CA_FRM_H-30,
                              p_cont, 0);
  //QQ  ctrl_set_rstyle(p_ca_frm, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  ctrl_set_rstyle(p_ca_frm, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_ca_frm, conditional_accept_pair_cont_keymap); 
  ctrl_set_proc(p_ca_frm, conditional_accept_pair_cont_proc);
  
   //CA pair
  x = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X;
  y = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_Y;
  h = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H;
  for(i=0; i<4; i++)
  {
    switch(i)
    {      
      case 0:
        p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                                x, 
                                y + (i / 2) * h,
                                CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW,
                                h,
                                p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
        text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, str[i / 2]);
        break;

      case 1:
        p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                                x + CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW,
                                y + (i / 2) *h,
                                CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW+25,
                                h,
                                p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
        text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
        break;
      case 2:
        p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                                x,
                                y + (i / 2) *h,
                                CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW,
                                h / 2,
                                p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
        text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
       // text_set_content_by_strid(p_ctrl, str[i / 2]);
        break;

      case 3:
        p_ctrl = ctrl_create_ctrl((u8*)CTRL_LIST, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                                x + CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW,
                                y + (i / 2) *h,
                                CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW+25,
                                PAIR_LIST_H, p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        list_set_mid_rect(p_ctrl, PAIR_LIST_MIDL, PAIR_LIST_MIDT,
             PAIR_LIST_MIDW, PAIR_LIST_MIDH,PAIR_LIST_VGAP);
        list_set_item_rstyle(p_ctrl, &pair_item_rstyle);
        list_set_count(p_ctrl, 0, PAIR_LIST_PAGE);

        list_set_field_count(p_ctrl, PAIR_LIST_PAGE, PAIR_LIST_PAGE);
        list_set_focus_pos(p_ctrl, 0);
        
        list_set_field_attr(p_ctrl, 0, (u32)(pair_list_attr.attr), (u16)(pair_list_attr.width),
                            (u16)(pair_list_attr.left), (u8)(pair_list_attr.top));
        list_set_field_rect_style(p_ctrl, 0, pair_list_attr.rstyle);
        list_set_field_font_style(p_ctrl, 0, pair_list_attr.fstyle);   
        break;
      default :
        break;
    }
  }

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

//QQ  ui_comm_help_create(&ca_pair_help_data, p_cont);

  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifdef WIN32
  on_conditional_accept_pair_update(p_ca_frm, 0, para1, para2);
  #else
  ui_ca_get_info(CAS_CMD_PAIRE_STATUS_GET, (u32)&g_card_stb_paired, 0);
  #endif
  return SUCCESS;
}

#ifdef WIN32
card_stb_paired_list_t p_data_test;
card_stb_paired_list_t *ui_get_p_card_stb_paired_list()
{
  u8 i = 0, k = 0;
  p_data_test.paired_status = CAS_E_CARD_DIS_PARTNER;
  p_data_test.paired_num = 5;
  for (i = 0; i < p_data_test.paired_num; i++)
  {
    for(k = 0;k < 6;k ++)
    {
      p_data_test.STB_list[6 * k + i] = i;
    }
  }
  return &p_data_test;
}
#endif

static void conditional_accept_pair_set_content(control_t *p_frm, u32 result, u32 stb_id_arr)
{
  u32 i = 0, k = 0;
  card_stb_paired_list_t *p_card_stb_paired_list = NULL;
  
  control_t *p_state = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_PAIR_STATE_CONTENT);
  control_t *p_stb_id_content = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_PAIR_ID_CONTENT);
  
  u16 start = 0;
  u8 asc_str[64];
  
  #ifndef WIN32
  p_card_stb_paired_list = (card_stb_paired_list_t *)stb_id_arr;
  #else
  p_card_stb_paired_list = ui_get_p_card_stb_paired_list();
  #endif
  
  
  UI_PRINTF("@@@@@ state: %d, %x \n", p_card_stb_paired_list->paired_status, p_card_stb_paired_list->STB_list);
  
  switch(p_card_stb_paired_list->paired_status)
  {
    case CAS_EVT_NONE:
      OS_PRINTF("The card is paired with the current STB\n");
      text_set_content_by_strid(p_state, IDS_CA_PAIRED_THIS_STB);//QQ IDS_CA_PAIRED_THIS_STB
      break;
    case CAS_E_ILLEGAL_CARD:
      OS_PRINTF("Card invalid\n");
      text_set_content_by_strid(p_state, IDS_CA_NOT_PAIRED);//QQ IDS_CA_CARD_INVALID
      break;
    case CAS_E_CARD_PAIROTHER:
      OS_PRINTF("The card is paired with another STB\n");
      text_set_content_by_strid(p_state, IDS_CA_PAIRED_OTHER_STB);//QQ IDS_CA_PAIRED_OTHER_STB
      break;
    case CAS_E_CARD_DIS_PARTNER:
      OS_PRINTF("The card is not paired with any STB\n");
      text_set_content_by_strid(p_state, IDS_CA_NOT_PAIRED_ANY_STB);//QQ  IDS_CA_NOT_PAIRED_ANY_STB
      break;
    default:
      OS_PRINTF("Unknown\n"); 
      text_set_content_by_strid(p_state, IDS_CA_NOT_PAIRED);
      break;
  }
  ctrl_paint_ctrl(p_state,TRUE);
  UI_PRINTF("@@@@@ conditional_accept_pair_set_content paired_num:%d\n", p_card_stb_paired_list->paired_num);
  if(p_card_stb_paired_list->paired_num > 0)
  {
    list_set_count(p_stb_id_content, p_card_stb_paired_list->paired_num , PAIR_LIST_PAGE);
    for (i = 0; i < PAIR_LIST_PAGE; i++)
    {
      if (i + start < p_card_stb_paired_list->paired_num)
      {
        memset(asc_str, 0, sizeof(asc_str));
        sprintf((char*)asc_str, "%.2X%.2X%.2X%.2X%.2X%.2X", p_card_stb_paired_list->STB_list[6 * k], 
                  p_card_stb_paired_list->STB_list[6 * k + 1], p_card_stb_paired_list->STB_list[6 * k + 2],
                  p_card_stb_paired_list->STB_list[6 * k + 3], p_card_stb_paired_list->STB_list[6 * k + 4],
                  p_card_stb_paired_list->STB_list[6 * k + 5]);
        list_set_field_content_by_ascstr(p_stb_id_content, (u16)(start + i), 0, asc_str); 
        k++;
      }
    }
    ctrl_paint_ctrl(p_stb_id_content,TRUE);
  }
  
}

static RET_CODE on_conditional_accept_pair_update(control_t *p_frm, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@@@ on_conditional_accept_pair_update\n");
  conditional_accept_pair_set_content(p_frm, para1, para2);
  
  return SUCCESS;
}

static RET_CODE on_exit_conditional_accept_pair(control_t *p_frm, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@@@ on_exit_conditional_accept_pair\n");
  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_PAIRE_INFO, on_conditional_accept_pair_update)
  ON_COMMAND(MSG_EXIT, on_exit_conditional_accept_pair)
END_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)


