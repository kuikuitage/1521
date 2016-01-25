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
  IDC_FRM_BUTTONS,
};

enum ca_ca_info_id
{
  IDC_CA_SP_ID = 1,
  IDC_CA_CAS_VER,
  IDC_CA_SMART_CARD_NUMBER,
  IDC_CA_CA_STATUS,
  IDC_CA_CARD_AREA_CODE,
//  IDC_CA_CARD_LIMIT_CODE,
  IDC_CA_CARD_OVERDRAFT,
  IDC_CA_CARD_BALANCE,
//  IDC_CA_ENTITLE_EXPIRED,
SN_CA_CARD_INFO_1,
SN_CA_CARD_INFO_2,
SN_CA_CARD_INFO_3,
SN_CA_CARD_INFO_4,
SN_CA_CARD_INFO_5,
SN_CA_CARD_INFO_6,
SN_CA_CARD_INFO_7,
SN_CA_CARD_INFO_8,
SN_CA_CARD_INFO_9,
};
#define CARD_SN_MAX_LEN             16

RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void get_ppv_str2(u16 *uni_str, u16 str_id, u8 *ansstr)
{
  u16 str[20] = {0}; 
  u16 len = 0;

  str_asc2uni((u8 *)ansstr, str);
  uni_strcat(uni_str, str,64);
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}


static void accept_ca_burse_info(control_t *p_cont, burses_info_t *p_burse)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_slot_total = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_BALANCE);
  control_t *p_overdraft = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_OVERDRAFT);

  u8 asc_str[128];
  u16 uni_str[64];
//  u8 limt_hight[4];
  u32 vale;
  u16 vale_h,vale_l;

  DEBUG(CA_CARD_INFO,INFO,"cash_value %ld;\n  credit_limit %ld\n",p_burse->p_burse_info[p_burse->index].cash_value,p_burse->p_burse_info[p_burse->index].credit_limit);
  if(p_burse == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_slot_total,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_overdraft,(u8 *) " ");
  }
  else
  {
    if(p_burse->p_burse_info[p_burse->index].cash_value >= 0)
    {
      vale = p_burse->p_burse_info[p_burse->index].cash_value-p_burse->p_burse_info[p_burse->index].balance;
      vale_h = vale/10000;
	  vale_l = (vale-vale_h*10000)/100;

      sprintf((char *)asc_str, "%d.%02d", vale_h,vale_l);
      memset(uni_str, 0, 32 * sizeof(u16));
      get_ppv_str2(uni_str, IDS_CA_CA_RMB, asc_str);
      ui_comm_static_set_content_by_unistr(p_slot_total, uni_str);
    }
    else
    {
      
      p_burse->p_burse_info[p_burse->index].cash_value *=(-1);
      vale = p_burse->p_burse_info[p_burse->index].cash_value-p_burse->p_burse_info[p_burse->index].balance;
      vale_h = vale/10000;
	  vale_l = (vale-vale_h*10000)/100;
      sprintf((char *)asc_str, "-%d.%02d", vale_h,vale_l);
      memset(uni_str, 0, 32 * sizeof(u16));
      get_ppv_str2(uni_str, IDS_CA_CA_RMB, asc_str);
      ui_comm_static_set_content_by_unistr(p_slot_total, uni_str);
    }
    if(p_burse->p_burse_info[p_burse->index].credit_limit >= 0)
    {
      #if 0
      hight_vale = p_burse->p_burse_info[p_burse->index].balance/10000;
      low_vale = (p_burse->p_burse_info[p_burse->index].balance - hight_vale*10000)/100;
      limt_hight[0] = (u8)(hight_vale/1000);
      limt_hight[1] = (u8)((hight_vale%1000)/100);
      limt_hight[2] = (u8)((hight_vale%1000)%100/10);
      limt_hight[3] = (u8)((hight_vale%1000)%100%10);
      
      sprintf((char *)asc_str, "%d.%02d", (limt_hight[0]*1000 + limt_hight[1]*100 + limt_hight[2]*10 + limt_hight[3]),(u8)low_vale);
      #endif
      sprintf((char *)asc_str, "%ld", p_burse->p_burse_info[p_burse->index].credit_limit);
	  memset(uni_str, 0, 32 * sizeof(u16));
      get_ppv_str2(uni_str, IDS_CA_CA_RMB, asc_str);
      ui_comm_static_set_content_by_unistr(p_overdraft, uni_str);
    }
    else
    {
      #if 0
      p_burse->p_burse_info[p_burse->index].balance *=(-1);
      hight_vale = p_burse->p_burse_info[p_burse->index].balance/10000;
      low_vale = (p_burse->p_burse_info[p_burse->index].balance - hight_vale*10000)/100;
      sprintf((char *)asc_str, "-%d.%02d", (u8)hight_vale,(u8)low_vale);
      #endif
      sprintf((char *)asc_str, "-%ld", p_burse->p_burse_info[p_burse->index].credit_limit);
	  memset(uni_str, 0, 32 * sizeof(u16));
      get_ppv_str2(uni_str, IDS_CA_CA_RMB, asc_str);
      ui_comm_static_set_content_by_unistr(p_overdraft, uni_str);
    }
  }
  ctrl_paint_ctrl(p_slot_total, TRUE);
  ctrl_paint_ctrl(p_overdraft, TRUE);
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_cas_sp_id = ctrl_get_child_by_id(p_frm, IDC_CA_SP_ID);
  control_t *p_cas_ver = ctrl_get_child_by_id(p_frm, IDC_CA_CAS_VER);
  control_t *p_card_id = ctrl_get_child_by_id(p_frm, IDC_CA_SMART_CARD_NUMBER);
  control_t *p_cas_satue = ctrl_get_child_by_id(p_frm, IDC_CA_CA_STATUS);
  control_t *p_arear_code = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_AREA_CODE);
  
  u8 asc_str[64];
  #ifdef WIN32
  u8 card_state = 13;
  #else
  u8 card_state = p_card_info->paterner_stat;
  #endif
  u8 i =0, j = 0;
  u16 uni_num[64];

  #ifndef WIN32
  OS_PRINTF(" conditional_accept_info_set_content ########V%d.%d.%d\n", p_card_info->cas_ver[0], 
                      p_card_info->cas_ver[1] >>4, p_card_info->cas_ver[1] & 0xf);
  
  OS_PRINTF("p_card_info->card_state : %d\n", p_card_info->paterner_stat);
  OS_PRINTF("p_card_info->sn : ");
  for(i = 0, j = 0; i < CARD_SN_MAX_LEN; i += 2, j++)
  {
    OS_PRINTF("%02x", p_card_info->sn[j]);
  }
  OS_PRINTF("\n");
  OS_PRINTF("p_card_info->card_zone_code : %d\n", p_card_info->card_zone_code);
  OS_PRINTF("p_card_info->card_ctrl_code : %d\n", p_card_info->card_ctrl_code);
  OS_PRINTF("p_card_info->expire_date : %d\n", ((p_card_info->expire_date[0] << 8) | (p_card_info->expire_date[1])));
  #endif
  
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_cas_sp_id, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_arear_code, (u8 *)" ");
  }
  else
  {
    //card sp 
    sprintf((char *)asc_str, "ÉÏº£ÈýÖÞÑ¸³Û");
    gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,55);
    ui_comm_static_set_content_by_unistr(p_cas_sp_id, uni_num);
    
    //card version
    sprintf((char *)asc_str, "V%d.%d.%d", p_card_info->cas_ver[0], 
                      p_card_info->cas_ver[1] >>4, p_card_info->cas_ver[1] & 0xf);
    ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);
    
    //card id
    for(i = 0, j = 0; i < CARD_SN_MAX_LEN; i += 2, j++)
    {
      sprintf((char *)(&asc_str[i]), "%02x", p_card_info->sn[j]);
    }
    ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);
	
    //card status
    if(card_state == CAS_PATERNER)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_STB_PAIR);
    }
    else if(card_state == CAS_NOT_PATERNER)
    {
      ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_STB_NO_PAIR);
    }
	
    //area code
    sprintf((char *)asc_str, "%d", p_card_info->card_zone_code);
    ui_comm_static_set_content_by_ascstr(p_arear_code, asc_str);
  }
  //for test
  #ifdef WIN32
  sprintf((char *)asc_str, "8047499004002961");
  ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);
  #endif
}

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
//CA frame
#define SN_ACCEPT_INFO_CA_FRM_X                   80//106
#define SN_ACCEPT_INFO_CA_FRM_Y                   24
#define SN_ACCEPT_INFO_CA_FRM_W                   456
#define SN_ACCEPT_INFO_CA_FRM_H                   360

  control_t *p_cont, *p_ctrl = NULL;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_card_info[ACCEPT_INFO_CARD_INFO_ITEM_CNT] =
  {
    IDS_CA_PROVIDER, IDS_CA_CONDITION_VER,IDS_CA_CARD_SERIAL,IDS_CA_STB_PAIR_STATUS,
    IDS_CA_CARD_AREA_CODE,IDS_CA_CARD_OVERDRAFT,IDS_CA_CARD_BALANCE,
  };

//LXD DEFINE
#define 	SN_CA_CARD_X    (ACCEPT_INFO_CA_INFO_ITEM_X + 250)
#define		SN_CA_CARD_Y    (y+3)
#define 	SN_CA_CARD_W    100
#define	    SN_CA_CARD_H    30
												
#define     SN_CA_CARD_INFO_CNT     9

#ifdef WIN32	
	//LXD DEFINE
	 u16 card_info[SN_CA_CARD_INFO_CNT] =
	{
		IDS_QPSK_HP,
		IDS_QAM128,
		IDS_UNIT_KHZ,
		IDS_UNIT_KBAUD,
		IDS_QPSK_VP,
		IDS_QPSK_HP,
		IDS_QAM64,
		IDS_UNIT_KHZ,
		IDS_QAM256
	};
#endif

  DEBUG_ENABLE_MODE(CA_CARD_INFO,INFO);
  DEBUG(CA_CARD_INFO,INFO,"open_ca_card_info now\n");

  // create container
  p_cont = ui_background_create(ROOT_ID_CA_CARD_INFO,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_SMARTCARD_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  //ctrl_set_keymap(p_cont, ca_mother_cont_keymap);
  ctrl_set_proc(p_cont, ca_card_info_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              SN_ACCEPT_INFO_CA_FRM_X, SN_ACCEPT_INFO_CA_FRM_Y,
                              SN_ACCEPT_INFO_CA_FRM_W, SN_ACCEPT_INFO_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

  //CA info
  y = 5;
  for (i = 0; i<ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
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
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
		
		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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

      case 2:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW2,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW2);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
		
		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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

      case 3:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW3,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW3);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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
                                           ACCEPT_INFO_CA_INFO_ITEM_LW6,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW6);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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
                                           ACCEPT_INFO_CA_INFO_ITEM_LW7,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW7);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

		#ifdef WIN32
		//LXD ADD
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_CA_CARD_INFO_1 + i),
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
		
      default:
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

    y += (30+ACCEPT_CA_INFO_ITEM_V_GAP);
  }
  
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
   
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *) para1);
  
  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
  ui_ca_get_info((u32)CAS_CMD_BURSE_INFO_GET,0,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}
static RET_CODE on_ca_accept_burse_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  accept_ca_burse_info(p_cont, (burses_info_t *)para2);

  return SUCCESS;
}

static RET_CODE on_is_card_move(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;
  switch(event_id)
  {
    case CAS_S_ADPT_CARD_REMOVE:
      manage_close_menu(ROOT_ID_CA_CARD_INFO,0,0);
      break;
    default:
      break;
  }
    
  return SUCCESS;
}

BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
  ON_COMMAND(MSG_CA_BUR_INFO, on_ca_accept_burse_update)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_is_card_move)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)




