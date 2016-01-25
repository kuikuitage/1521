/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_card_info.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG,
  IDC_CARD_NUMBER_TITLE,
  IDC_CAS_VERSION_TITLE,
  IDC_CARD_TYPE_TITLE,
  IDC_MOTHER_CARD_NUMBER_TITLE,
  IDC_CARD_NUMBER,
  IDC_CAS_VERSION,
  IDC_CARD_TYPE,
  IDC_MOTHER_CARD_NUMBER,
};

RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_ctrl;
  u16 x,y;
  u8 i;
  u16 str[4] = {IDS_SMART_CARD_NUMBER,IDS_CA_VER2,IDS_CARD_TYPE,IDS_CA_MASTER_CARD_ID};

  p_cont = ui_background_create(ROOT_ID_CA_CARD_INFO,
								 0, 0,
								 640, 480,
								 IDS_CA_CARD_INFO, TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_card_info_proc);
  //create menu item
  x = CA_CARD_ITEM_X;
  y = CA_CARD_ITEM_Y;

  for(i=0; i<4; i++)
  {
    // title
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CARD_NUMBER_TITLE+i),
                              x,
                              y + i*(CA_CARD_ITEM_H + CA_CARD_ITEM_VGAP),
                              CA_CARD_ITEM_NAME_W, CA_CARD_ITEM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, str[i]);
	if(i == 3)
	{
		ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
	}

    //data
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CARD_NUMBER_TITLE+i+4),
                              x + (CA_CARD_ITEM_NAME_W + CA_CARD_ITEM_HGAP),
                              y + i*(CA_CARD_ITEM_H + CA_CARD_ITEM_VGAP),
                              CA_CARD_ITEM_DETAIL_W, CA_CARD_ITEM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	if(i == 2)
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	else
    	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	if(i == 3)
	{
		ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);
	}
  }

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  //send command to get operator info.
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_ca_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl_sn = NULL;
  control_t *p_ctrl_cas_ver = NULL;
  control_t *p_ctrl_card_type = NULL;
  control_t *p_ctrl_mother_card_title = NULL;
  control_t *p_ctrl_mother_card_num = NULL;
  cas_card_info_t *p_card_info = NULL;
  u16 unistr[33] = {0};
  p_card_info = (cas_card_info_t *)para2;

  OS_PRINTF("\r\n****on_ca_card_info para2[0x%x]***",para2);

  p_ctrl_sn = ctrl_get_child_by_id(p_ctrl, IDC_CARD_NUMBER);
  p_ctrl_cas_ver = ctrl_get_child_by_id(p_ctrl, IDC_CAS_VERSION);
  p_ctrl_card_type = ctrl_get_child_by_id(p_ctrl, IDC_CARD_TYPE);
  p_ctrl_mother_card_title = ctrl_get_child_by_id(p_ctrl, IDC_MOTHER_CARD_NUMBER_TITLE);
  p_ctrl_mother_card_num = ctrl_get_child_by_id(p_ctrl, IDC_MOTHER_CARD_NUMBER);
  if(FALSE == ui_is_smart_card_insert())
  {
	  return SUCCESS;
  }
  if(NULL != p_ctrl_sn)
  {
    str_asc2uni((u8 *)p_card_info->sn, unistr);
    text_set_content_by_unistr(p_ctrl_sn, unistr);
  }

  if(NULL != p_ctrl_cas_ver)
  {
    memset(unistr,0,sizeof(unistr));
    str_asc2uni((u8 *)p_card_info->cas_ver, unistr);
    text_set_content_by_unistr(p_ctrl_cas_ver, unistr);
  }

  if(NULL != p_ctrl_card_type)
  {
	 if(p_card_info->card_type == 1)
	 {
		text_set_content_by_strid(p_ctrl_card_type, IDS_CA_CHILD_CARD);
		if((NULL != p_ctrl_mother_card_title) && (NULL != p_ctrl_mother_card_num))
		{
			memset(unistr,0,sizeof(unistr));
    		str_asc2uni((u8 *)p_card_info->mother_card_id, unistr);
    		text_set_content_by_unistr(p_ctrl_mother_card_num, unistr);
			ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
		}
	 }
	 else
	 {
		text_set_content_by_strid(p_ctrl_card_type, IDS_CA_MOTHER_CARD);
	 }
	 	
  }
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}


BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_ca_card_info)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)

 
