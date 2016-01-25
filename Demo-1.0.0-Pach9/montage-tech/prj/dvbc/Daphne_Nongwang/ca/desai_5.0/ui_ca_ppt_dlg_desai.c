/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_ca_ppt_dlg.h"
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif

enum ippv_ctrl_id
{
  IDC_IPPV_TITLE = 1,
  IDC_IPPV_NAME,
  IDC_IPPV_PROVIDER_ID,
  IDC_IPPV_PRICE,
  IDC_IPPV_START_TIME,
  IDC_IPPV_END_TIME,
  IDC_IPPV_BURSE1,
  IDC_IPPV_BURSE2,
  IDC_IPPV_BURSE3,
  IDC_IPPV_BURSE4,
  IDC_IPPV_BIT_YES,
  IDC_IPPV_BIT_NO,
};
static ipps_info_t *p_ippv_info = NULL;

static BOOL is_prog_bar = FALSE;


u16 ca_ippv_info_keymap(u16 key);
RET_CODE ca_ippv_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 Ippv_pwdlg_keymap(u16 key);
RET_CODE Ippv_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);
extern void update_ca_message(u16 strid);


static RET_CODE on_ippv_burse_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_burse1 = ctrl_get_child_by_id(p_cont, IDC_IPPV_BURSE1);
  control_t *p_burse2 = ctrl_get_child_by_id(p_cont, IDC_IPPV_BURSE2);
  control_t *p_burse3 = ctrl_get_child_by_id(p_cont, IDC_IPPV_BURSE3);
  control_t *p_burse4 = ctrl_get_child_by_id(p_cont, IDC_IPPV_BURSE4);
  u8 asc_str[32];
  burses_info_t * burses = (burses_info_t *)para2;
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[1].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse1, asc_str);

  sprintf((char *)asc_str, "%s", burses->p_burse_info[2].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse2, asc_str);
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[3].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse3, asc_str);
  
  sprintf((char *)asc_str, "%s", burses->p_burse_info[4].burse_value);
  ui_comm_static_set_content_by_ascstr(p_burse4, asc_str);
   ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

static void ca_ippv_info_set_content(control_t *p_cont, ipps_info_t *p_ippv)
{
  
  control_t *p_ca_provider = ctrl_get_child_by_id(p_cont, IDC_IPPV_PROVIDER_ID);
  control_t *p_ca_price = ctrl_get_child_by_id(p_cont, IDC_IPPV_PRICE);
  control_t *p_start_time = ctrl_get_child_by_id(p_cont, IDC_IPPV_START_TIME);
  control_t *p_end_time = ctrl_get_child_by_id(p_cont, IDC_IPPV_END_TIME);
  control_t *p_ca_name = ctrl_get_child_by_id(p_cont, IDC_IPPV_NAME);
  u8 asc_str[32];
  u16 uni_num[64];
  p_ippv_info = p_ippv;
  if(p_ippv_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_ca_provider,(u8 *) " ");
  }
  else
  {
    sprintf((char *)asc_str, "%d ", p_ippv_info->p_ipp_info[0].operator_id);
    ui_comm_static_set_content_by_ascstr(p_ca_provider, asc_str);
    
    sprintf((char *)asc_str, "%d ", p_ippv_info->p_ipp_info[0].price);
    gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,63);
    ui_comm_static_set_content_by_unistr(p_ca_price, uni_num);
    
    sprintf((char *)asc_str,"%04d/%02d/%02d %02d:%02d:%02d",
                p_ippv_info->p_ipp_info[0].start_time[0]*256 + 
                p_ippv_info->p_ipp_info[0].start_time[1], 
                p_ippv_info->p_ipp_info[0].start_time[2],
                p_ippv_info->p_ipp_info[0].start_time[3],
                p_ippv_info->p_ipp_info[0].start_time[4],
                p_ippv_info->p_ipp_info[0].start_time[5],
                p_ippv_info->p_ipp_info[0].start_time[6]);
    ui_comm_static_set_content_by_ascstr(p_start_time, asc_str);

    sprintf((char *)asc_str,"%04d/%02d/%02d %02d:%02d:%02d",
                p_ippv_info->p_ipp_info[0].expired_time[0]*256 + 
                p_ippv_info->p_ipp_info[0].expired_time[1],
                p_ippv_info->p_ipp_info[0].expired_time[2],
                p_ippv_info->p_ipp_info[0].expired_time[3],
                p_ippv_info->p_ipp_info[0].expired_time[4],
                p_ippv_info->p_ipp_info[0].expired_time[5],
                p_ippv_info->p_ipp_info[0].expired_time[6]);
    ui_comm_static_set_content_by_ascstr(p_end_time, asc_str);
    
    sprintf((char *)asc_str, "%s", p_ippv_info->p_ipp_info[0].product_name);
    gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,63);
    ui_comm_static_set_content_by_unistr(p_ca_name, uni_num);
    on_ippv_burse_update(p_cont,0,0,0);
  }

}

RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2)
{
  control_t *p_cont, *p_title, *p_ctrl, *p_btn;
  u16 i, y, x;
  u16 stxt_ippv_info[CA_IPPV_INFO_ITEM_CNT] = {
  							IDS_PROGRAM_NAME,IDS_CA_SP_ID_XSD,
                         	IDS_PRICE, IDS_START_TIME2,IDS_END_TIME,
                            IDS_CA_PURSE_BALANCE1,IDS_CA_PURSE_BALANCE2,
                            IDS_CA_PURSE_BALANCE3,IDS_CA_PURSE_BALANCE4};

  if(fw_find_root_by_id(ROOT_ID_CA_IPPV_PPT_DLG) != NULL) // already opened
  {
    return ERR_FAILURE;
  }
  
  p_cont = fw_create_mainwin(ROOT_ID_CA_IPPV_PPT_DLG,
                             CA_IPPV_COMM_DLG_X, CA_IPPV_COMM_DLG_Y,
                             CA_IPPV_COMM_DLG_W, CA_IPPV_COMM_DLG_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE ;
  }
  ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);//RSI_TIP_BOX
  ctrl_set_keymap(p_cont, ca_ippv_info_keymap);
  ctrl_set_proc(p_cont, ca_ippv_info_proc);
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_IPPV_TITLE),
                        CA_IPPV_TEXT_TITLE_X,
                        CA_IPPV_TEXT_TITLE_Y,
                        CA_IPPV_TEXT_TITLE_W, CA_IPPV_TEXT_TITLE_H,
                        p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_DLG_TITLE, FSI_DLG_TITLE, FSI_DLG_TITLE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_IPPV_BUY);
  y = CA_IPPV_INFO_ITEM_Y + CA_IPPV_TEXT_TITLE_H;
  for (i = 0; i<CA_IPPV_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
	  DEBUG_ENABLE_MODE(CA_IPPV_PPT_DLG,INFO);
	  DEBUG(CA_IPPV_PPT_DLG,INFO,"open_ca_ippv_ppt_dlg.....\n");
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_INFO_ITEM_LW0,
                                         CA_IPPV_INFO_ITEM_RW0);
	  DEBUG_ENABLE_MODE(CA_IPPV_PPT_DLG,INFO);
	  DEBUG(CA_IPPV_PPT_DLG,INFO,"open_ca_ippv_ppt_dlg.....\n");
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 1:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_INFO_ITEM_LW0,
                                         CA_IPPV_INFO_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 2:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_INFO_ITEM_LW0,
                                         CA_IPPV_INFO_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 3:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_INFO_ITEM_LW0,
                                         CA_IPPV_INFO_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 4:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_INFO_ITEM_LW0,
                                         CA_IPPV_INFO_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 5:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_BURSE_ITEM_LW0,
                                         CA_IPPV_BURSE_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 6:
      y-=30;
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_BURSE_ITEM_X, y,
                                         CA_IPPV_BURSE_ITEM_LW0,
                                         CA_IPPV_BURSE_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 7:
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_INFO_ITEM_X, y,
                                         CA_IPPV_BURSE_ITEM_LW0,
                                         CA_IPPV_BURSE_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;
      case 8:
      y-=30;
      p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_IPPV_NAME + i),
                                         CA_IPPV_BURSE_ITEM_X, y,
                                         CA_IPPV_BURSE_ITEM_LW0,
                                         CA_IPPV_BURSE_ITEM_RW0);
      ui_comm_static_set_static_txt(p_ctrl, stxt_ippv_info[i]);
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
      ui_comm_static_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      break;

    }
    y+= 30;
  }

  x = 80;
  for (i = 0; i < 2; i++)
  {
    p_btn = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_IPPV_BIT_YES + i),
                           x, (y + 5), 60, 25, p_cont, 0);
    ctrl_set_rstyle(p_btn,SN_RSI_BNT_TEXT_SH, SN_RSI_BNT_TEXT_HL, SN_RSI_BNT_TEXT_SH);//RSI_DLG_BTN_SH, RSI_DLG_BTN_HL, RSI_DLG_BTN_SH
    text_set_font_style(p_btn,FSI_DLG_BTN_N, FSI_DLG_BTN_HL, FSI_DLG_BTN_N);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn,(u16)(i == 1 ? IDS_NO: IDS_YES));
    ctrl_set_related_id(p_btn,
                      (u8)((i - 1 +2) % 2 + IDC_IPPV_BIT_YES),    /* left */
                      0,                                          /* up */
                      (u8)((i + 1) % 2 + IDC_IPPV_BIT_YES),       /* right */
                      0);                                         /* down */
    x += 100;
  }
 p_btn = ctrl_get_child_by_id(p_cont,IDC_IPPV_BIT_YES);
 ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
 p_ippv_info = (ipps_info_t *) para2;
 ca_ippv_info_set_content(p_cont, p_ippv_info);

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  if(TRUE == get_full_scr_ad_status())
    ui_pic_stop();
#endif
 
 ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);  
 #ifndef WIN32
 ui_ca_get_info(CAS_CMD_BURSE_INFO_GET, 0, 0);
 #endif

 p_cont = fw_find_root_by_id(ROOT_ID_PROG_BAR);
 if(p_cont != NULL)
 {
   manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
   is_prog_bar = TRUE;
 }

 return SUCCESS;

}

static RET_CODE on_ippv_channel_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 prog_id;

  manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);  
  
  if(is_prog_bar == TRUE)
  {
	ui_shift_prog(1, TRUE, &prog_id);
    manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
	is_prog_bar = FALSE;
  }  
  manage_notify_root(ROOT_ID_EPG, msg, 0, 0);
  return SUCCESS;
}

static RET_CODE on_ippv_channel_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 prog_id;
  
  manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);  
  
  if(is_prog_bar == TRUE)
  {
	ui_shift_prog(-1, TRUE, &prog_id);
    manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
	is_prog_bar = FALSE;
  }
    
  manage_notify_root(ROOT_ID_EPG, msg, 0, 0);
  return SUCCESS;
}

static RET_CODE on_ippv_ppt_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;  

  comm_pwdlg_data_t ippv_pwdlg_data =
  {
    ROOT_ID_INVALID,
    CA_IPPV_PWD_X,
    CA_IPPV_PWD_Y,
    IDS_INPUT_PASSWORD,
    PWDLG_T_IPPV,
    Ippv_pwdlg_keymap,
    Ippv_pwdlg_proc,
  };
  
  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if ((p_btn != NULL) && (ctrl_get_ctrl_id(p_btn) == IDC_IPPV_BIT_YES))
  {
    manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
    ui_comm_pwdlg_open(&ippv_pwdlg_data);
    
    manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
   }
  else if((p_btn != NULL) && (ctrl_get_ctrl_id(p_btn) == IDC_IPPV_BIT_NO))
  {
    manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
    on_ca_message_update(p_ctrl, 0, 0, CAS_E_PROG_UNAUTH);
  }
  return SUCCESS;
}


static RET_CODE on_ippv_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ui_comm_pwdlg_close();
  on_ca_message_update(p_ctrl, 0, 0, CAS_E_PROG_UNAUTH);
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  return SUCCESS;
}

static RET_CODE on_ippv_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, 
u32 para2)
{
  ui_comm_pwdlg_close();
  on_ca_message_update(p_ctrl, 0, 0, CAS_E_PROG_UNAUTH);
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  return SUCCESS;
}

static RET_CODE on_ippv_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
   #ifndef WIN32
   ui_ca_get_info((u32)CAS_CMD_IPP_BUY_SET, (u32)p_ippv_info,0);
   #endif
   ui_comm_pwdlg_close();
   if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
   return SUCCESS;
}


BEGIN_KEYMAP(ca_ippv_info_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
END_KEYMAP(ca_ippv_info_keymap, NULL)


BEGIN_MSGPROC(ca_ippv_info_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_ippv_ppt_select)
  ON_COMMAND(MSG_FOCUS_UP, on_ippv_channel_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ippv_channel_down)
  ON_COMMAND(MSG_CA_BUR_INFO, on_ippv_burse_update)
END_MSGPROC(ca_ippv_info_proc, cont_class_proc)

BEGIN_KEYMAP(Ippv_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(Ippv_pwdlg_keymap, NULL)

BEGIN_MSGPROC(Ippv_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_ippv_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ippv_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_ippv_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_ippv_pwdlg_exit)
END_MSGPROC(Ippv_pwdlg_proc, cont_class_proc)

