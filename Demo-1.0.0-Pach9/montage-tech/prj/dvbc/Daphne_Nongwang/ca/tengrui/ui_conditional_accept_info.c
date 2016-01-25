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
  IDC_CA_CARD_NUM = 1,
  IDC_CA_CARD_NUM_CONTENT,

  IDC_CA_CARD_VERSION,
  IDC_CA_CARD_VERSION_CONTENT,

  IDC_CA_CENSORSHIP,
  IDC_CA_CENSORSHIP_CONTENT,

  IDC_CA_RATING,
  IDC_CA_RATING_CONTENT,
};

u16 conditional_accept_info_cont_keymap(u16 key);
RET_CODE conditional_accept_info_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_conditional_accept_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl;
  u8 i=0;
  u16 x=0, y=0, w=0, h=0;
  u32 text_type = 0, align_type = 0;
  u16 str[CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT] =
  {
    IDS_SMART_CARD_NUMBER,
    IDS_CARD_VERSION,
    IDS_CA_VERSION,
    IDS_RATING,
  };
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_INFO,
							  0,0,
							  640,480,
							  IDS_CA_CARD_INFO,TRUE);	

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_info_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_info_cont_proc);

  //CA info

  x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X;
  y = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y;
  h = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H;
  for(i=0; i<8; i++)
  {
    switch(i%2)
    {
      case 0:
        x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X;
        y = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y+i/2*(CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_V_GAP)-80;
        w = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      case 1:
        x = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW+CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H_GAP;
        w = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW+60;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT| STL_VCENTER;
        break;
    }

    p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_CA_CARD_NUM+i),
                              x, y, w, h, p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
    text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i%2==0)
    {
      text_set_content_by_strid(p_ctrl, str[i/2]);
    }
//#ifdef WIN32
//    else
//    {
//      switch(i)
//      {
//        case 1://serial num
//          sprintf((char*)ansstr, "%s ","8 0101 37 0000004 5");
//          str_asc2uni(ansstr, uni_num);
//          text_set_content_by_unistr(p_ctrl, uni_num);
//          break;
//
//        case 3://version
//          sprintf((char*)ansstr, "%s ","V2.0");
//          str_asc2uni(ansstr, uni_num);
//          text_set_content_by_unistr(p_ctrl, uni_num);
//          break;
//
//        case 5://pcvisition
//          sprintf((char*)ansstr, "%s ","$Revision:2.20$");
//          str_asc2uni(ansstr, uni_num);
//          text_set_content_by_unistr(p_ctrl, uni_num);
//          break;
//
//        case 7://rating
//          sprintf((char*)ansstr, "%s ","0");
//          str_asc2uni(ansstr, uni_num);
//          text_set_content_by_unistr(p_ctrl, uni_num);
//          break;
//
//        default:
//          break;
//      }
//    }
//#endif
  }



  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
 #ifndef WIN32
	ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
#endif
  
 
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
  return SUCCESS;
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_card_num = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_NUM_CONTENT);
  //control_t *p_car_labe = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_LABEL_CONTENT);
  control_t *p_card_version = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_VERSION_CONTENT);
  control_t *p_core_version = ctrl_get_child_by_id(p_cont, IDC_CA_CENSORSHIP_CONTENT);
  control_t *p_rating = ctrl_get_child_by_id(p_cont, IDC_CA_RATING_CONTENT);
  u8 asc_str[32];
  u16 uni_num[64];
  if(p_card_info == NULL)
  {
    text_set_content_by_unistr(p_card_num, (u16*)" ");
    text_set_content_by_unistr(p_card_version, (u16*)" ");
    text_set_content_by_unistr(p_core_version, (u16*)" ");
    text_set_content_by_unistr(p_rating, (u16*)" ");
  }
  else
  {
    //cars sn
    sprintf((char*)asc_str, "%s", p_card_info->sn);

    str_asc2uni(asc_str, uni_num);
    text_set_content_by_unistr(p_card_num, uni_num);

    //tbd
    sprintf((char*)asc_str, "%s", p_card_info->card_version);
    //str_asc2uni(asc_str, uni_num);
    text_set_content_by_ascstr(p_card_version, (u8*)asc_str);

    sprintf((char*)asc_str, "%s", p_card_info->cas_ver);
    //str_asc2uni(asc_str, uni_num);
    text_set_content_by_ascstr(p_core_version, (u8*)asc_str);

    //sprintf((char*)asc_str, "%d", p_card_info->cas_ver);
    //str_asc2uni(asc_str, uni_num);
    sprintf((char*)asc_str, "%d", p_card_info->cas_rating);
    str_asc2uni(asc_str, uni_num);
    text_set_content_by_unistr(p_rating, uni_num);

  }
}


static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;

  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE)
      {
        //on_conditional_accept_info_update(p_cont, 0, 0, (u32)NULL);
        ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
      }
      break;
  }

  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_conditional_accept_info_accept_notify)
  ON_COMMAND(MSG_CA_INIT_OK, on_conditional_accept_info_accept_notify)
END_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)


