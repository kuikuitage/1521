/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_info.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum ca_ca_info_id
{
  IDC_CA_CARD_SP_ID = 1,
  IDC_CA_CARD_SP_ID_CONTENT,
  IDC_CA_CARD_SP_NAME,
  IDC_CA_CARD_SP_NAME_CONTENT,
  IDC_CA_CARD_SP_TEL,
  IDC_CA_CARD_SP_TEL_CONTENT,
  IDC_CA_CARD_SP_ADDR,
  IDC_CA_CARD_SP_ADDR_CONTENT
};

u16 conditional_accept_info_cont_keymap(u16 key);
RET_CODE conditional_accept_info_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static void conditional_accept_info_get_unistr(u16 * uni_str, u8 * str_addr)
{
  u8 asc_str[CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH];
  
  sprintf((char *)asc_str, "%s", str_addr);
  gb2312_to_unicode(asc_str,sizeof(asc_str),uni_str,CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH);
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_operators_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_card_sp_id = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_SP_ID_CONTENT);
  control_t *p_card_name = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_SP_NAME_CONTENT);
  control_t *p_card_sp_tel = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_SP_TEL_CONTENT);
  control_t *p_card_sp_addr = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_SP_ADDR_CONTENT);
  u16 uni_str[CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH];

  if(p_card_info == NULL)
  {
    text_set_content_by_unistr(p_card_sp_id, (u16 *)" ");
    text_set_content_by_unistr(p_card_name,(u16 *) " ");
    text_set_content_by_unistr(p_card_sp_tel, (u16 *)" ");
    text_set_content_by_unistr(p_card_sp_addr, (u16 *)" ");
  }
  else
  {
    //sp_id
    conditional_accept_info_get_unistr(uni_str, p_card_info->p_operator_info[0].operator_id_ascii);
    text_set_content_by_unistr(p_card_sp_id, uni_str);

    //SP-name
    conditional_accept_info_get_unistr(uni_str, p_card_info->p_operator_info[0].operator_name);
    text_set_content_by_unistr(p_card_name, uni_str);
    
    //sp_tel
    conditional_accept_info_get_unistr(uni_str, p_card_info->p_operator_info[0].operator_phone);
    text_set_content_by_unistr(p_card_sp_tel, uni_str);
    
    //sp_addr
    conditional_accept_info_get_unistr(uni_str, p_card_info->p_operator_info[0].operator_address);
    text_set_content_by_unistr(p_card_sp_addr, uni_str);
  }
}

RET_CODE open_conditional_accept_info(u32 para1, u32 para2)
{
  //CA frm
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_X			0//106
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_Y			24
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_W			620//505
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_H			360

  //CA info items
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X        80
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y        20
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_W       170
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H        30

  
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_X    SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X + SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_W + 20
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_W    340
  #define SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_H    SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H

  control_t *p_cont, *p_ctrl = NULL;
  control_t *p_ca_frm;//,*pctrl;
  u8 i;
  u16 y;
  u16 stxt_ca_info[CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT] =
  { 
    IDS_CA_SP_ID, IDS_CA_SP_NAME, IDS_CA_OPERATOR_TEL,IDS_CA_OPERATOR_ADDRES
  };

  #ifdef WIN32
  u16 stxt_ca_info2[CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT] = 
  {
	IDS_CA_LEVEL1, IDS_CA_LEVEL2
  };
  #endif

  // create container
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_INFO,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_SP_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_info_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_info_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_X, SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_W, SN_CONDITIONAL_ACCEPT_INFO_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  //CA info
  y = 0;//SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y
  for (i = 0; i<CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:      
      case 2:
      case 4:
      case 6:
        p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT,(u8)(IDC_CA_CARD_SP_ID + i ), 
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_W, SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H, 
                                              p_ca_frm, 0);
        text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);//FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, stxt_ca_info[i /2]);
        break;
		
      case 1:
      case 5:
        p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT,(u8)(IDC_CA_CARD_SP_ID + i ), 
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_X, y - 30,
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_W, SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_H, 
                                              p_ca_frm, 0);
        text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
		#ifdef WIN32	
		text_set_content_by_strid(p_ctrl, stxt_ca_info2[i /4]);
		#endif
        break;

      case 3:
        p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT,(u8)(IDC_CA_CARD_SP_ID + i ), 
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_X, y - 30,
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_W, SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_H * 2, 
                                              p_ca_frm, 0);
        text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
		#ifdef WIN32
		text_set_content_by_strid(p_ctrl, IDS_NEED_SMC_STB_PAIR);
		#endif
        y += SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H;
        break;

      case 7:
        p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT,(u8)(IDC_CA_CARD_SP_ID +i ), 
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_X, y - 30,
                                              SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_W, SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_NEXT_H * 3 , 
                                              p_ca_frm, 0);
        text_set_align_type(p_ctrl, STL_LEFT |STL_TOP);
        text_set_font_style(p_ctrl, FSI_SECOND_TITLE,FSI_SECOND_TITLE,FSI_SECOND_TITLE);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
		#ifdef WIN32
		text_set_content_by_strid(p_ctrl, IDS_INFORMATION_PLAZA);
		#endif
        break;        
      default:
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SUB_BUTTON_SH

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID));/* down */

    y += SN_CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H ;
  }


  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info((u32)CAS_CMD_OPERATOR_INFO_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_operators_info_t *)para2);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_OPE_INFO, on_conditional_accept_info_update)
END_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)


