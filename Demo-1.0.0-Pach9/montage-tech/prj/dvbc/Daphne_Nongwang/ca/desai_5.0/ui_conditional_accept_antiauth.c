/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_antiauth.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
  IDC_STB_CARD_BURSE1,
  IDC_STB_CARD_BURSE2,
  IDC_STB_CARD_BURSE3,
  IDC_STB_CARD_BURSE4,
  SN_IDC_CONDITIONAL_ACCEPT_MONEY_LEFT1,
  SN_IDC_CONDITIONAL_ACCEPT_MONEY_LEFT2,
  SN_IDC_CONDITIONAL_ACCEPT_MONEY_LEFT3,
  SN_IDC_CONDITIONAL_ACCEPT_MONEY_LEFT4,
};

u16 conditional_accept_burse_cont_keymap(u16 key);
RET_CODE conditional_accept_burse_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static void conditional_accept_burse_set_content(control_t *p_cont, burses_info_t * burses)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_burse1 = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_BURSE1);
  
  control_t *p_burse2 = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_BURSE2);

  control_t *p_burse3 = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_BURSE3);

  control_t *p_burse4 = ctrl_get_child_by_id(p_frm, IDC_STB_CARD_BURSE4);
  u8 asc_str[32];
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[1].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse1, asc_str);

  sprintf((char *)asc_str, "%s", burses->p_burse_info[2].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse2, asc_str);
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[3].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse3, asc_str);
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[4].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse4, asc_str);
  
}

RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2)
{
  //CA frm
  #define SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_X             106
  #define SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_Y             24
  #define SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_W             505
  #define SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_H             360

  //piar info items
  #define SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_X        20
  #define SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_Y        40
  #define SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_LW       200
  #define SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_RW       80
  #define SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_H        35

#ifdef WIN32
//LXD DEFINE
#define 	SN_CONDITIONAL_ACCEPT_MONEY_LEFT_X    (SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_X + 200)
#define		SN_CONDITIONAL_ACCEPT_MONEY_LEFT_Y    (y+3)
#define 	SN_CONDITIONAL_ACCEPT_MONEY_LEFT_W    130
#define	    SN_CONDITIONAL_ACCEPT_MONEY_LEFT_H    30

#define     SN_CA_CARD_INFO_CNT     4

//LXD DEFINE
 u16 card_info[SN_CA_CARD_INFO_CNT] =
{
	IDS_QPSK_HP,
	IDS_QAM128,
	IDS_UNIT_KBAUD,
	IDS_QPSK_VP
};
#endif
  control_t *p_cont, *p_ctrl;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_ca_info[CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT] =
  { 
   IDS_CA_PURSE_BALANCE1,IDS_CA_PURSE_BALANCE2,IDS_CA_PURSE_BALANCE3,IDS_CA_PURSE_BALANCE4
  };
  // create container
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_SLOT_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_burse_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_burse_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_X, SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_W, SN_CONDITIONAL_ACCEPT_BURSE_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  //pair info
  y = SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
       p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_STB_CARD_BURSE1 + i),
                                           SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_X, y,
                                           SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_LW,
                                           SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_SUB_BUTTON_SH
        ui_comm_static_set_content_by_ascstr(p_ctrl, (u8 *)" ");
#ifdef WIN32
        //LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CONDITIONAL_ACCEPT_MONEY_LEFT1 + i),
						  SN_CONDITIONAL_ACCEPT_MONEY_LEFT_X, SN_CONDITIONAL_ACCEPT_MONEY_LEFT_Y,
						  SN_CONDITIONAL_ACCEPT_MONEY_LEFT_W, SN_CONDITIONAL_ACCEPT_MONEY_LEFT_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif		
        break;

      default:
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BAR_BLUE_NORMAL);//RSI_SUB_BUTTON_SH

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT + IDC_STB_CARD_BURSE1),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT + IDC_STB_CARD_BURSE1));/* down */

    y += SN_CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_H + CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_V_GAP;
  }

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0, 0);
  #endif

  return SUCCESS;
}

static RET_CODE on_conditional_accept_burse_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_burse_set_content(p_cont, (burses_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_burse_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_burse_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_burse_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_BUR_INFO, on_conditional_accept_burse_update)
END_MSGPROC(conditional_accept_burse_cont_proc, ui_comm_root_proc)


