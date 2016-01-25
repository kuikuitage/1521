/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_card_info.h"
#include "ui_comm_root.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

static BOOL page_flag = FALSE;

enum ca_ca_info_id
{
  IDC_CA_SP_ID = 1,
  IDC_CA_SMART_CARD_NUMBER,
  IDC_CA_CARD_NUMBER2,
  IDC_CA_CA_STATUS,
  IDC_CA_CAS_VER,
  IDC_CA_WATCH_LEVE,
  IDC_CA_START_TIME3,
  IDC_CA_MASTER_ID,
  IDC_CA_LCO_ID,
  IDC_CA_GROUP_ID,
  IDC_CA_CARD_VERSION,
  SN_IDC_CA_CARD_1,
  SN_IDC_CA_CARD_2,
  SN_IDC_CA_CARD_3,
  SN_IDC_CA_CARD_4,
  SN_IDC_CA_CARD_5,
  SN_IDC_CA_CARD_6,  
  SN_IDC_CA_CARD_7
};

u16 stxt_card_info[ACCEPT_INFO_CARD_INFO_ITEM_CNT] =
{
  IDS_SP_OPERATOR_INFO, 
  IDS_SMART_CARD_NUMBER,
  IDS_CA_ADDRES_INFO,
  IDS_CA_CARD_STATE,
  IDS_CA_CARD_VER, //IDS_CA_CARD_VER
  IDS_CA_WATCH_LEVEL,
  IDS_CA_WORK_TIME,
  IDS_CA_MASTER_CARD_ID,
  IDS_CA_LOCAL_DIVISION_ID,
  IDS_CA_GROUP_ID,
  IDS_CA_CARD_VERSION
};

u16 ca_card_info_keymap(u16 key);
RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_cas_sp_id = ctrl_get_child_by_id(p_frm, IDC_CA_SP_ID);
  control_t *p_card_id = ctrl_get_child_by_id(p_frm, IDC_CA_SMART_CARD_NUMBER);
  control_t *p_arear_code = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_NUMBER2);
  #ifndef WIN32
  control_t *p_cas_satue = ctrl_get_child_by_id(p_frm, IDC_CA_CA_STATUS);
  #endif
  
  control_t *p_cas_ver = ctrl_get_child_by_id(p_frm, IDC_CA_CAS_VER);
  control_t *p_cas_time = ctrl_get_child_by_id(p_frm, IDC_CA_START_TIME3);
  control_t *p_cas_leve = ctrl_get_child_by_id(p_frm, IDC_CA_WATCH_LEVE);
  
  control_t *p_cas_master_id = ctrl_get_child_by_id(p_frm, IDC_CA_MASTER_ID);
  control_t *p_cas_lco_id = ctrl_get_child_by_id(p_frm, IDC_CA_LCO_ID);
  control_t *p_cas_group_id = ctrl_get_child_by_id(p_frm, IDC_CA_GROUP_ID);
  control_t *p_cas_card_version = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_VERSION);

  u8 asc_str[64];
  //u16 uni_str[64]={0};
  #ifndef WIN32
  u8 card_state = p_card_info->card_state;
  #endif
  
  
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_cas_sp_id, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_arear_code, (u8 *)" ");
  }
  else
  {
  
    sprintf((char *)asc_str, "%s", p_card_info->cas_id);
    ui_comm_static_set_content_by_ascstr(p_cas_sp_id, asc_str);

    //tbd
    sprintf((char *)asc_str, "%s", p_card_info->card_id);
    ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);

    sprintf((char *)asc_str, "%s", p_card_info->area_code);
    ui_comm_static_set_content_by_ascstr(p_arear_code, asc_str);

#ifndef WIN32
    sprintf((char *)asc_str, "%d", p_card_info->card_state);
    if(card_state == 14)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_DELETE_CARD);
    }
    else if(card_state == 16)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NO_ENABLE);
    }
    else if(card_state == 15)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_SMC_OVERDUE);
    }
    else if(card_state == 11)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NO_NEED_PAIR);//IDS_E33
    }
    else if(card_state == 13)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_STOP);
    }
    else if(card_state == 17)
    {
      
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NO_NEED_PAIR);//IDS_CA_NO_NEED_PAIR
    }
    else if(card_state == 0)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_STB_PAIR);//IDS_CA_STB_PAIR
    }
    else if(card_state == 12)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CAS_E_CARD_PAIROTHER);
    }
    ui_comm_static_set_content_by_ascstr(p_cas_satue, asc_str);
#endif

    
    sprintf((char *)asc_str, "%s", p_card_info->cas_ver);
    ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

    sprintf((char *)asc_str, "%02d:%02d:%02d ~ %02d:%02d:%02d ", p_card_info->work_time.start_hour,p_card_info->work_time.start_minute,p_card_info->work_time.start_second,
                                    p_card_info->work_time.end_hour,p_card_info->work_time.end_minute,p_card_info->work_time.end_second);
    ui_comm_static_set_content_by_ascstr(p_cas_time, asc_str);
    
    sprintf((char *)asc_str, "%d", p_card_info->card_work_level);
    ui_comm_static_set_content_by_ascstr(p_cas_leve, asc_str);
    
    //lco id
    sprintf((char *)asc_str, "%s", p_card_info->cas_loc);
    ui_comm_static_set_content_by_ascstr(p_cas_lco_id, asc_str);

	//master id
    sprintf((char *)asc_str, "%s", p_card_info->mother_card_id);
    ui_comm_static_set_content_by_ascstr(p_cas_master_id, asc_str);
	
    //group id
    sprintf((char *)asc_str, "%s", p_card_info->cas_group);
    ui_comm_static_set_content_by_ascstr(p_cas_group_id, asc_str);
	
    //card_version
    sprintf((char *)asc_str, "%s", p_card_info->card_version);
    ui_comm_static_set_content_by_ascstr(p_cas_card_version, asc_str);

  }
}

static RET_CODE set_page1_available(control_t * p_ca_frm)
{
  u8 i = 0;
  control_t * p_ctrl = NULL;

  for (i = 0; i<ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i++)
  {
    p_ctrl = ctrl_get_child_by_id(p_ca_frm, (u8)(IDC_CA_SP_ID + i));
    ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);
  }
  
  for (i = ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i<ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
    p_ctrl = ctrl_get_child_by_id(p_ca_frm, (u8)(IDC_CA_SP_ID + i));
    ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  }
  page_flag = FALSE;  
  return SUCCESS;
}

static RET_CODE set_page2_available(control_t * p_ca_frm)
{
  u8 i = 0;
  control_t * p_ctrl = NULL;
  
  for (i = 0; i<ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i++)
  {
    p_ctrl = ctrl_get_child_by_id(p_ca_frm, (u8)(IDC_CA_SP_ID + i));
    ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  }
  
  for (i = ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i<ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
    p_ctrl = ctrl_get_child_by_id(p_ca_frm, (u8)(IDC_CA_SP_ID + i));
    ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);
  }
  page_flag = TRUE;  
  return SUCCESS;
}

static RET_CODE creat_page1_items(control_t * p_ca_frm )
{

#ifdef WIN32
//LXD DEFINE
#define 	SN_CA_CARD_X    (ACCEPT_INFO_CA_INFO_ITEM_X + 200)
#define		SN_CA_CARD_Y    (y+3)
#define 	SN_CA_CARD_W    130
#define	    SN_CA_CARD_H    30

#define     SN_CA_CARD_INFO_CNT     7

//LXD DEFINE
 u16 card_info[SN_CA_CARD_INFO_CNT] =
{
	IDS_QPSK_HP,
	IDS_QAM128,
	IDS_UNIT_KBAUD,
	IDS_QPSK_VP,
	IDS_QAM64,
	IDS_UNIT_KHZ,
	IDS_QAM256
};
#endif

  u8 i;
  u16 y;
  control_t * p_ctrl = NULL;

  y = 15;
  
  for (i = 0; i<ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW0,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW0);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
        //LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30, SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif		
        break;

      case 1:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW1,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW1);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
        //LXD ADD		
    	p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30,SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif		
	    break;
      case 2:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW2,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW2);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
        //LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30,SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif		
		break;


	  case 3:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW3,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW3);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30, SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif	
        break;
      case 4:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW4,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW4);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30, SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);

		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif			
        break;  
      case 5:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW5,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW5);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
	    //LXD ADD (test)
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30, SN_CA_CARD_Y,// LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
						  p_ca_frm, 0);
		
		ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
		text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, card_info[i]);
#endif	
        break;
      case 6:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW6,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW6);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_CA_CARD_1 + i),
						  SN_CA_CARD_X+30, SN_CA_CARD_Y,  // LXD ADD "30"
						  SN_CA_CARD_W, SN_CA_CARD_H,
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

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              ACCEPT_INFO_CARD_INFO_ITEM_CNT) %
                             ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID));/* down */

    y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
  }
return SUCCESS;
}

static RET_CODE creat_page2_items(control_t * p_ca_frm)
{
  u8 i;
  u16 y;
  control_t * p_ctrl = NULL;

  y = 15;
  
  for (i = ACCEPT_INFO_CARD_INFO_PAGE_ITEMS; i<ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
        case 7:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_MASTER_ID), //IDC_CA_SP_ID + i
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW7,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW7+45);
      	ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH
        break;
      case 8:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW8,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW8);
   		ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]); 
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

        break;

      case 9:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW9,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW9);
    	ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]); 
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH
        break;

      case 10:
	  	
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW10,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW10+45);
    	ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]); 
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH
        break;

      default:
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_SUB_BUTTON_SH

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              ACCEPT_INFO_CARD_INFO_ITEM_CNT) %
                             ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID));/* down */

    y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
  }
return SUCCESS;
}

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  //CA frm
  #define SN_ACCEPT_INFO_CA_FRM_X              80
  #define SN_ACCEPT_INFO_CA_FRM_Y              30//53
  #define SN_ACCEPT_INFO_CA_FRM_W              345
  #define SN_ACCEPT_INFO_CA_FRM_H              300//360
  #define SN_INFO_SCREEN_WIDTH                 640//672
  #define SN_INFO_SCREEN_HEIGHT                480//576
  
  #define SN_SUB_MENU_HELP_RSC_CNT             9
  
  control_t *p_cont = NULL;
  control_t *p_ca_frm = NULL;

static sn_comm_help_data_t sn_submenu_help_data; //help bar data
help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
{ 
   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
   {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
   {SN_IDC_HELP_TEXT, 60 , IDS_PAGEUP},
   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
   {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
   {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
};

#if 0


  static comm_help_data_t ca_info_help_data = //help bar data
  {
    2,
    2,
    {
      IDS_PAGEUP,
      IDS_BACK,
    },
    {
      IM_ICON_ARROW_UP_DOWN,
      IM_ICON_BACK,
    },
  };
#endif

  p_cont = ui_background_create(ROOT_ID_CA_CARD_INFO,
                             0, 0,
                             //SCREEN_WIDTH, SCREEN_HEIGHT,
                             SN_INFO_SCREEN_WIDTH, SN_INFO_SCREEN_HEIGHT,
                             IDS_SMARTCARD_INFO, TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ca_card_info_keymap);
  ctrl_set_proc(p_cont, ca_card_info_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              SN_ACCEPT_INFO_CA_FRM_X, SN_ACCEPT_INFO_CA_FRM_Y,
                              SN_ACCEPT_INFO_CA_FRM_W+100, SN_ACCEPT_INFO_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_TIP_BOX
 
  //CA info
  creat_page1_items(p_ca_frm);
  creat_page2_items(p_ca_frm);
 
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  set_page1_available(p_ca_frm);

  conditional_accept_info_set_content(p_cont, (cas_card_info_t *) para1);
  
  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=168;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  #ifndef WIN32
  ui_ca_get_info((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif

  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t * p_ca_frm = NULL;

  p_ca_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);
  set_page1_available(p_ca_frm);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_card_info_focus_change(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t * p_ca_frm = NULL;

  p_ca_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  
  if(page_flag)
  {
    set_page1_available(p_ca_frm);
  }
  else
  {  
    set_page2_available(p_ca_frm);
  }
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  
  return SUCCESS;
}

BEGIN_KEYMAP(ca_card_info_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(ca_card_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_card_info_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_card_info_focus_change)
  ON_COMMAND(MSG_PAGE_UP, on_card_info_focus_change)
  ON_COMMAND(MSG_PAGE_DOWN, on_card_info_focus_change)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)



