/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_feed.h"
#include "sys_dbg.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_CARD_FEED_FRM1,
  IDC_CA_CARD_PROMPT_1,
};

RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{

  control_t *p_cont, *p_ctrl, *p_frm1;

  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED,
							  0,0,
							  640,480,
							  IDS_CARD_MANAGE,TRUE);	

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
 
  
  //CA FEED FRAME1
  p_frm1 = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM1),
                            70, 80, 500, 300, p_cont, 0);
  ctrl_set_rstyle(p_frm1, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  
  //PROMPT
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_PROMPT_1),
                            100, 60, 300, 100, p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_DIG_BG,RSI_DIG_BG,RSI_DIG_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_FEED_CHILD_CARD_PROMPT);

  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  

  return SUCCESS;
}

