/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_ca_card_info.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum ca_ca_info_id
{
  IDC_CA_CARD_NO = 1, 
  IDC_CA_CARD_NO_CONTENT,  
  /*
  IDC_CA_CALIB_CRC_CODE,
  IDC_CA_CARD_VERSION,
  */
  IDC_CA_CARD_TYPE,
  IDC_CA_CARD_TYPE_CONTENT,
  
  IDC_CA_CARD_ENTITLE,
  IDC_CA_CARD_ENTITLE_CONTENT,
  
  IDC_CA_WORK_ID,
  IDC_CA_WORK_ID_CONTENT,
};
/*
static comm_help_data_t info_help_data = //help bar data
{
  2,
  2,
  {
    IDS_BACK,
    IDS_EXIT,
  },
  {
    IM_SN_ICON_MOVE,
    IM_SN_ICON_MOVE,
  },
};
*/

RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  //control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  
  control_t *p_cas_card_no = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_NO_CONTENT);
  control_t *p_cas_card_type = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_TYPE_CONTENT);
  control_t *p_cas_card_entile = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_ENTITLE_CONTENT); 
  control_t *p_cas_workid = ctrl_get_child_by_id(p_cont, IDC_CA_WORK_ID_CONTENT);

  
  u8 asc_str[16];
  u16 uni_num[32];
  if(p_card_info == NULL)
  {
    text_set_content_by_unistr(p_cas_card_no, (u16*)" ");
    text_set_content_by_unistr(p_cas_card_type, (u16*)" ");
    text_set_content_by_unistr(p_cas_card_entile, (u16*)" ");
    text_set_content_by_unistr(p_cas_workid, (u16*)" ");
  }
  else
  {
    OS_PRINTF("card sn = %s\n",p_card_info->sn);
   
    sprintf((char *)asc_str, "%s", p_card_info->sn);
    str_asc2uni(asc_str, uni_num);
    text_set_content_by_unistr(p_cas_card_no, uni_num);


     OS_PRINTF("card type = %d\n",p_card_info->card_type);
    if(0 == p_card_info->card_type)
    {
      text_set_content_type(p_cas_card_type, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_cas_card_type, IDS_MAINMENU);      
    }
    else
    {
      text_set_content_type(p_cas_card_type, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_cas_card_type, IDS_MAINMENU);      
    }

    OS_PRINTF("card status = %d\n",p_card_info->card_state);

    if(1 == p_card_info->card_state)
    {
      text_set_content_type(p_cas_card_entile, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_cas_card_entile, IDS_MAINMENU);      

    }
    else
    {
      text_set_content_type(p_cas_card_entile, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_cas_card_entile, IDS_MAINMENU);      
 
    }

    OS_PRINTF("card type = %d\n",p_card_info->card_ctrl_code);

    sprintf((char *)asc_str, "%d", p_card_info->card_ctrl_code);
    str_asc2uni(asc_str, uni_num);
    text_set_content_by_unistr(p_cas_workid, uni_num);


  }
}

//    IDS_SMART_CARD_NUMBER,IDS_CA_TYPE,IDS_CA_AUTH_STATUS,IDS_AREA_CODE,

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

  control_t *p_cont, *p_ctrl;
  u8 i=0;
  u16 x=0, y=0, w=0, h=0;
  u32 text_type = 0, align_type = 0;
  u16 str[CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT] =
  {
    IDS_SMART_CARD_NUMBER,
    IDS_MAINMENU,
    IDS_MAINMENU,
    IDS_MAINMENU,
  };


//  full_screen_title_t title_data = {IM_SN_F1,IDS_CA_CARD_INFO, RSI_SN_BG, IDS_CA_CARD_INFO};
  //customer_cfg_t cus_cfg = {0};
  //ui_get_customer_config(&cus_cfg);
/*
  p_cont = ui_comm_root_create_full_screen(ROOT_ID_CONDITIONAL_ACCEPT_INFO, RSI_SN_BG,
                          CONDITIONAL_ACCEPT_INFO_CONT_X, CONDITIONAL_ACCEPT_INFO_CONT_Y,
                          CONDITIONAL_ACCEPT_INFO_CONT_W, CONDITIONAL_ACCEPT_INFO_CONT_H,
                          &title_data);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  */
  p_cont = ui_background_create(ROOT_ID_EMAIL_MESS,
                           0, 0,
                           SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                           IDS_MEMAIL,TRUE);
  ctrl_set_rstyle(p_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  //ctrl_set_keymap(p_cont, conditional_accept_info_cont_keymap);
  //ctrl_set_proc(p_cont, conditional_accept_info_cont_proc);

  //CA info

  x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X;
  y = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y;
  h = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H;
  for(i=0; i<CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT*2; i++)
  {
    switch(i%2)
    {
      case 0:
        x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X;
        y = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y+i/2*(CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_V_GAP);
        w = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_CENTER | STL_VCENTER;
        break;

      case 1:
        x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H_GAP;
        w = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW+60;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_CENTER| STL_VCENTER;
        break;
    }

    p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_NO+i),
                              x, y, w, h, p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
    text_set_font_style(p_ctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i%2==0)
    {
      text_set_content_by_strid(p_ctrl, str[i/2]);
    }

  }

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif


  
  //ui_comm_help_create(&info_help_data, p_cont);

  conditional_accept_info_set_content(p_cont, (cas_card_info_t *) para1);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_ca_get_info((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}


BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)


