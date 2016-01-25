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
  IDC_CA_CAS_VERSION = 1,
  IDC_CA_CALIB_CRC_CODE,
  IDC_CA_CARD_VERSION,
};


RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_cas_version = ctrl_get_child_by_id(p_frm, IDC_CA_CAS_VERSION);
  control_t *p_crc_time = ctrl_get_child_by_id(p_frm, IDC_CA_CALIB_CRC_CODE);
  //control_t *p_crc_code = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_VERSION);
  u8 asc_str[16];
  u16 uni_num[32];
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_cas_version, (u8 *)" ");
    //ui_comm_static_set_content_by_ascstr(p_crc_code,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_crc_time, (u8 *)" ");
  }
  else
  {
    sprintf((char *)asc_str, "%s", p_card_info->sn);
    str_asc2uni(asc_str, uni_num);
    ui_comm_static_set_content_by_unistr(p_cas_version, uni_num);

    //tbd
    sprintf((char *)asc_str, "%04d-%02d-%02d", (p_card_info->expire_date[0]<<8) |(p_card_info->expire_date[1]),p_card_info->expire_date[2],
        p_card_info->expire_date[3]);
    ui_comm_static_set_content_by_ascstr(p_crc_time, asc_str);

    //sprintf((char *)asc_str, "%d", (p_card_info->area_code[0] + p_card_info->area_code[1]));
    //ui_comm_static_set_content_by_ascstr(p_crc_code, asc_str);

  }
}

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl = NULL;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_ca_info[CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ] =
  {
    IDS_SMART_CARD_NUMBER, IDS_CA_AUTH_TIME_EDN,
  };
  

  #if 0
  quanzhi
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE, IDS_SYSTEM_SET, RSI_TITLE_BG, IDS_CA_CARD_INFO};

  static comm_help_data_t ca_info_help_data = //help bar data
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

  p_cont = ui_comm_prev_root_create(ROOT_ID_CA_CARD_INFO, RSI_FULL_SCREEN_BG,
                          0, CONDITIONAL_ACCEPT_INFO_CONT_Y,
                          CONDITIONAL_ACCEPT_INFO_CONT_W, CONDITIONAL_ACCEPT_INFO_CONT_H,
                          &title_data);
#endif
  p_cont = ui_background_create(ROOT_ID_CA_CARD_INFO,
							   0, 0,
							   //SCREEN_WIDTH, SCREEN_HEIGHT,
							   640, 480,
							   IDS_CA_CARD_INFO, TRUE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_cont, ca_card_info_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              CONDITIONAL_ACCEPT_INFO_CA_FRM_X, CONDITIONAL_ACCEPT_INFO_CA_FRM_Y,
                              CONDITIONAL_ACCEPT_INFO_CA_FRM_W, CONDITIONAL_ACCEPT_INFO_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  //ctrl_set_keymap(p_ca_frm, conditional_accept_info_ca_frm_btn_keymap);
  //ctrl_set_proc(p_ca_frm, conditional_accept_info_ca_frm_btn_proc);

  //CA info
  y = CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_CAS_VERSION + i),
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW0,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW0);
        ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

        break;

      case 1:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_CAS_VERSION + i),
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW1,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW1);
        ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        break;
#if 0
      case 2:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_CA_CAS_VERSION + i),
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW2,
                                           CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW2);
        ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        break;
#endif
      default:
        break;
    }

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ) %
                             CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ + IDC_CA_CAS_VERSION),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ + IDC_CA_CAS_VERSION));/* down */

    y += CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H + CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_V_GAP;
  }


  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  //quanzhi ui_comm_help_create(&ca_info_help_data, p_cont);

  conditional_accept_info_set_content(p_cont, (cas_card_info_t *) para1);

  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_get_info((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif

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


