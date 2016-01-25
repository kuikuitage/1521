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
  IDC_CA_NUM,
  IDC_CAS_VER,
  IDC_STB_ID_CONTENT,
  IDC_CA_PATCH_TIME,
  IDC_CA_PATCH_STATUS,
};

static u16 ca_card_info_key_map(u16 key);
static RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT		15

  control_t *p_cont;
  control_t *p_ctrl;
  control_t *p_small_bg;
  u8 i,y_cas;
  u16 y;
  u16 str[CA_CARD_INFO_CNT] = 
  {
    IDS_SMART_CARD_NUMBER,
    IDS_CAS_VERSION,
    IDS_CUR_STBID,
    IDS_PATCH_TIME,
    IDS_PATCH_STATUS,
  };


  
  p_cont = ui_background_create(ROOT_ID_CA_CARD_INFO,
                            CA_CARD_CONT_X,CA_CARD_CONT_Y, //CA_CONT_X, CA_CONT_Y,
                            640,480,//CA_CONT_W, CA_CONT_H,
                            IDS_CA_CARD_INFO,TRUE);  
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ca_card_info_key_map);
  ctrl_set_proc(p_cont, ca_card_info_proc);
  //small bg
  p_small_bg = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG,
                              CA_CARD_SMALL_BG_X-140, CA_CARD_SMALL_BG_Y-60,
                              CA_CARD_SMALL_BG_W, CA_CARD_SMALL_BG_H-80,
                              p_cont, 0);
  //QQ ctrl_set_rstyle(p_small_bg, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);

  ctrl_set_rstyle(p_small_bg, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

  y = 5;
  y_cas = 0;
  for(i=0; i < CA_CARD_INFO_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_static_create(p_small_bg,(u8)(IDC_CA_NUM+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW0,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW0);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        //QQ ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
        break;
      case 1:
        p_ctrl = ui_comm_static_create(p_small_bg,(u8)(IDC_CA_NUM+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW0,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW0);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        //QQ  ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
        break;
      case 2:
        p_ctrl = ui_comm_static_create(p_small_bg,(u8)(IDC_CA_NUM+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW1,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW1);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        //QQ  ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
        break;
      case 3: 
         p_ctrl = ui_comm_static_create(p_small_bg,(u8)(IDC_CA_NUM+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW2,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW2);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        //QQ ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
        break;
      case 4:
        p_ctrl = ui_comm_static_create(p_small_bg,(u8)(IDC_CA_NUM+ i),
                                                            ACCEPT_INFO_CA_INFO_ITEM_X,
                                                            y,
                                                            ACCEPT_INFO_CA_INFO_ITEM_LW3,
                                                            ACCEPT_INFO_CA_INFO_ITEM_RW3);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        //QQ  ui_comm_static_set_rstyle(p_ctrl, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);
        ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
        break;
      default:
        break;
    }
    
    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CA_CARD_INFO_CNT) %
                             CA_CARD_INFO_CNT + IDC_CA_NUM),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CA_CARD_INFO_CNT + IDC_CA_NUM));/* down */

    y += (30+ACCEPT_CA_INFO_ITEM_V_GAP+y_cas);
    y_cas = 0;
  }

  

 
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  //QQ  ui_comm_help_create(&ca_card_help_data, p_cont);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  //send command to get operator info.
  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_ca_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl_ca_info = ctrl_get_child_by_id(p_ctrl, IDC_SMALL_BG);
  control_t *p_ctrl_ca_num = ctrl_get_child_by_id(p_ctrl_ca_info, IDC_CA_NUM);
  control_t *p_ctrl_cas_ver = ctrl_get_child_by_id(p_ctrl_ca_info, IDC_CAS_VER);
  control_t *p_ctrl_stb_id = ctrl_get_child_by_id(p_ctrl_ca_info, IDC_STB_ID_CONTENT);
  control_t *p_ctrl_cas_patch_time = ctrl_get_child_by_id(p_ctrl_ca_info, IDC_CA_PATCH_TIME);
  control_t *p_ctrl_cas_patch_status = ctrl_get_child_by_id(p_ctrl_ca_info, IDC_CA_PATCH_STATUS);
  
  cas_card_info_t *p_card_info = NULL;
  u8 asc_str[32];
  //u16 uni_num[32];
  utc_time_t card_upg_time = {0};
  u8 ascstr[14];
  
  p_card_info = (cas_card_info_t *)para2;
  
  OS_PRINTF("\r\n****on_ca_card_info para2[0x%x]***\n",para2);

  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl_cas_ver, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_ctrl_stb_id,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_ctrl_cas_patch_time, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_ctrl_cas_patch_status,(u8 *) " ");
  }
  else
  {
     //cas version
    OS_PRINTF("CAS VERSION:0X%02X%02X%02X%02X\n",p_card_info->cas_ver[0], 
              p_card_info->cas_ver[1], p_card_info->cas_ver[2], p_card_info->cas_ver[3]);
    sprintf((char *)asc_str, "0X%02X%02X%02X%02X",p_card_info->cas_ver[0], 
              p_card_info->cas_ver[1], p_card_info->cas_ver[2], p_card_info->cas_ver[3]);
    ui_comm_static_set_content_by_ascstr(p_ctrl_cas_ver, asc_str);

     //ca number
    memset(asc_str, 0, sizeof(asc_str));
    OS_PRINTF("ca num:%s\n",p_card_info->sn);
    sprintf((char *)asc_str, "%s",p_card_info->sn);
    ui_comm_static_set_content_by_ascstr(p_ctrl_ca_num, asc_str);
	
  //current STB ID
    OS_PRINTF("buffer Value:0X%X%X%X%X%X%X\n", 
      p_card_info->stb_sn[0], p_card_info->stb_sn[1], p_card_info->stb_sn[2], p_card_info->stb_sn[3], p_card_info->stb_sn[4], p_card_info->stb_sn[5]);
    sprintf((char*)ascstr, "0X%X%X%X%X%X%X", p_card_info->stb_sn[0], p_card_info->stb_sn[1], p_card_info->stb_sn[2], p_card_info->stb_sn[3], p_card_info->stb_sn[4], p_card_info->stb_sn[5]);

    ui_comm_static_set_content_by_ascstr(p_ctrl_stb_id, ascstr);
    
    //CAS Patch Time
    #if 1
    card_upg_time = sys_status_get_card_upg_time();
    UI_PRINTF("@@@@@ %.4d.%.2d.%.2d %.2d:%.2d\n", card_upg_time.year, card_upg_time.month, card_upg_time.day,
              card_upg_time.hour, card_upg_time.minute);
    
    if((card_upg_time.month > 12) || (card_upg_time.day > 31))
    {
      sprintf((char*)asc_str, " ");
    }
    else
    {
      sprintf((char*)asc_str, "%.4d.%.2d.%.2d %.2d:%.2d", card_upg_time.year, card_upg_time.month, card_upg_time.day,
                  card_upg_time.hour, card_upg_time.minute);
    }
    ui_comm_static_set_content_by_ascstr(p_ctrl_cas_patch_time, asc_str);

    //CAS Patch Status
    
	OS_PRINTF("CAS Patch Status sys_status_get_card_upg_state() ==== %d\n", sys_status_get_card_upg_state());
    if(sys_status_get_card_upg_state() == 1)
    {
      OS_PRINTF("CAS Patch Status Successful\n");
      ui_comm_static_set_content_by_strid(p_ctrl_cas_patch_status, IDS_CA_SUCCESS);
    }
    else if(sys_status_get_card_upg_state() == 0)
    {
      OS_PRINTF("CAS Patch Status Failure\n");
      ui_comm_static_set_content_by_strid(p_ctrl_cas_patch_status, IDS_CA_FAIL);
    }
   #endif
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);

  return SUCCESS;
}


static RET_CODE on_exit_ca_card_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_CA_CARD_INFO, 0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(ca_card_info_key_map, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ca_card_info_key_map, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_ca_card_info)
  ON_COMMAND(MSG_EXIT, on_exit_ca_card_info)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)

 
