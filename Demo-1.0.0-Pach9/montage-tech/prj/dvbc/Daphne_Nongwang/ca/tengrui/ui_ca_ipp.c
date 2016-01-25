/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_ipp.h"
#include "sys_dbg.h"

enum ca_ipp_cont_id
{
  IDC_IPP_FRM = 1,
  IDC_IPP_HEAD = 2,
};

enum ca_ipp_ctrl_id
{
  IDC_IPP_TYPE = 1,
  IDC_IPP_PRICE,
  IDC_IPP_DISTANCE,  
  IDC_IPP_INFO,
  IDC_IPP_INPUT_PIN,
  
  IDC_IPP_HELP,

  IDC_IPP_TYPE_CONTENT,
  IDC_IPP_PRICE_CONTENT,
  IDC_IPP_DISTANCE_CONTENT,  
  IDC_IPP_INFO_CONTENT,
  IDC_IPP_INFO_PIN,
  IDC_IPP_PSW,
  IDC_IPP_HEAD_CONTENT,
  IDC_IPP_HEAD_ICON,
};

 ipp_buy_info_t g_ipp_buy_info = {0};
static customer_cfg_t cus_cfg;
struct ipp_pwdlg_int_data
{
    u8  type;               // PWDLG_T_
    u8  total_bit;          // total bit
    u8  input_bit;          // input bit
    u32 total_value;        // total value
    u32 input_value;        // input value
};
static struct ipp_pwdlg_int_data g_ipp_pwdlg_idata;
//static pwdlg_ret_t g_ipp_pwdlg_ret;

u16 ca_ipp_root_keymap(u16 key);
RET_CODE ca_ipp_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ipp_pwdlg_edit_keymap(u16 key);
RET_CODE ipp_pwdlg_edit_proc(control_t *p_mbox, u16 msg, u32 para1, u32 para2);

#if 0
static void ipp_show_info(control_t *p_cont, BOOL is_force)
{
  control_t *p_ctrl, *p_frm;

  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_IPP_TYPE_CONTENT);
  text_set_content_by_ascstr(p_ctrl, (u8*)"IPPV");
  ctrl_paint_ctrl(p_ctrl, is_force);

  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_IPP_PRICE_CONTENT);
  text_set_content_by_ascstr(p_ctrl, (u8*)"1Dot/1Minute");
  ctrl_paint_ctrl(p_ctrl, is_force);

  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_IPP_DISTANCE_CONTENT);
  text_set_content_by_ascstr(p_ctrl, (u8*)"2009.01.02-11:04:06--2012.01.02-11:04:06");
  ctrl_paint_ctrl(p_ctrl, is_force);

  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_IPP_INFO_CONTENT);
  text_set_content_by_ascstr(p_ctrl, (u8*)"-A");
  ctrl_paint_ctrl(p_ctrl, is_force);
}
#endif

static RET_CODE ipp_info_set_content(control_t *p_cont, ipp_buy_info_t *p_ipp_buy_info)
{
  control_t *p_frm, *p_type, *p_price, *p_distance, *p_info, *p_head, *p_headtxt;
  u8 asc_str[64];
  u16 uni_str[32];
  u16 uni_str2[64];
  u16 uni_str3[64];
  
  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_type = ctrl_get_child_by_id(p_frm, IDC_IPP_TYPE_CONTENT);
  p_price = ctrl_get_child_by_id(p_frm, IDC_IPP_PRICE_CONTENT);
  p_distance = ctrl_get_child_by_id(p_frm, IDC_IPP_DISTANCE_CONTENT);
  p_info = ctrl_get_child_by_id(p_frm, IDC_IPP_INFO_CONTENT);
  p_head = ctrl_get_child_by_id(p_cont, IDC_IPP_HEAD);
  p_headtxt = ctrl_get_child_by_id(p_head, IDC_IPP_HEAD_CONTENT);
  
  if(p_ipp_buy_info == NULL)
  {
    text_set_content_by_ascstr(p_type, (u8*)" ");
    text_set_content_by_ascstr(p_price, (u8*)" ");
    text_set_content_by_ascstr(p_distance, (u8*)" ");
    text_set_content_by_ascstr(p_info, (u8*)" ");
  }
  else
  {
    sprintf((char*)asc_str, "%s", p_ipp_buy_info->message_type?"IPPT":"IPPV");
    text_set_content_by_ascstr(p_type, (u8*)asc_str);
    
    sprintf((char*)asc_str, "%s", p_ipp_buy_info->message_type?"IPPT":"IPPV");
    text_set_content_by_ascstr(p_headtxt, (u8*)asc_str);

    if(p_ipp_buy_info->message_type)
    {
      gui_get_string(IDS_DOT, uni_str, 32);
      sprintf((char*)asc_str, "%d", (int)p_ipp_buy_info->ipp_charge);
      str_asc2uni(asc_str, uni_str2);
      uni_strcat(uni_str2, uni_str, 32);
      sprintf((char*)asc_str, "/");
      str_asc2uni(asc_str, uni_str);
      uni_strcat(uni_str2, uni_str, 32);
      sprintf((char*)asc_str, "%d", (int)(p_ipp_buy_info->ipp_unit_time/60));
      str_asc2uni(asc_str, uni_str);
      uni_strcat(uni_str2, uni_str, 32);
      gui_get_string(IDS_MINUTE, uni_str, 32);
      uni_strcat(uni_str2, uni_str, 32);
    }
    else  
    {
      gui_get_string(IDS_DOT, uni_str, 32);
      sprintf((char*)asc_str, "%d", (int)p_ipp_buy_info->ipp_charge);
      str_asc2uni(asc_str, uni_str2);
      uni_strcat(uni_str2, uni_str, 32);
    }
    text_set_content_by_unistr(p_price, uni_str2);
    
    sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d--%04d.%02d.%02d-%02d:%02d:%02d",
              p_ipp_buy_info->start_time[0] * 100 + 
              p_ipp_buy_info->start_time[1],
              p_ipp_buy_info->start_time[2],
              p_ipp_buy_info->start_time[3],
              p_ipp_buy_info->start_time[4],
              p_ipp_buy_info->start_time[5],
              p_ipp_buy_info->start_time[6],
              p_ipp_buy_info->expired_time[0] * 100 + 
              p_ipp_buy_info->expired_time[1],
              p_ipp_buy_info->expired_time[2],
              p_ipp_buy_info->expired_time[3],
              p_ipp_buy_info->expired_time[4],
              p_ipp_buy_info->expired_time[5],
              p_ipp_buy_info->expired_time[6]);
    text_set_content_by_ascstr(p_distance, (u8*)asc_str);

    sprintf((char*)asc_str, "%s", p_ipp_buy_info->note);
    str_asc2uni(asc_str, uni_str3);
    text_set_content_by_unistr(p_info, (u16*)uni_str3);
    
    p_ipp_buy_info->purchase_num = 1;
  }

  if(p_ipp_buy_info == NULL)
  {
    memset(&g_ipp_buy_info, 0, sizeof(ipp_buy_info_t));
  }
  else
  {
    memcpy(&g_ipp_buy_info, p_ipp_buy_info, sizeof(ipp_buy_info_t));
  }

  UI_PRINTF("[ipp_info_set_content]ipp_buy_info pin %s\n",g_ipp_buy_info.pin);
  UI_PRINTF("[ipp_info_set_content]ipp_buy_info channel id %d\n",g_ipp_buy_info.channel_id);
  UI_PRINTF("[ipp_info_set_content]ipp_buy_info ipp_charge %d\n",g_ipp_buy_info.ipp_charge);
  UI_PRINTF("[ipp_info_set_content]ipp_buy_info price %d\n",g_ipp_buy_info.ipp_price[0].price);
  UI_PRINTF("[ipp_info_set_content]ipp_buy_info price type %d\n",g_ipp_buy_info.ipp_price[0].price_type);

  return SUCCESS;
}

RET_CODE open_ca_ipp(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl, *p_frm, *p_mbox, *p_head;//, *p_bmp;
  u16 i;

#ifndef WIN32
  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
    return ERR_FAILURE;
#endif
  if(fw_find_root_by_id(ROOT_ID_CA_IPP) != NULL)
    return ERR_FAILURE;
  
  if (ui_get_ca_last_msg() == INVALID_U32)
  {
  	return SUCCESS;
  }
  
  //p_ipp_buy_info = (ipp_buy_info_t *)para2;
  //memcpy(&g_ipp_buy_info, para2, sizeof(ipp_buy_info_t));
  get_customer_config(&cus_cfg);
  g_ipp_pwdlg_idata.input_bit = 0;
  g_ipp_pwdlg_idata.input_value = 0;
  if(cus_cfg.customer == CUSTOMER_TONGGUANG
    || cus_cfg.customer == CUSTOMER_YINHE
    || cus_cfg.customer == CUSTOMER_TONGGUANG_CG
    || cus_cfg.customer == CUSTOMER_JINYA
    || cus_cfg.customer == CUSTOMER_TONGGUANG_QY
    || cus_cfg.customer == CUSTOMER_PANLONGHU_TR)
  {
    ui_pic_stop();
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_PROG_BAR))
  {
	  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }
  // create window
  p_cont = fw_create_mainwin(ROOT_ID_CA_IPP,
                             CA_IPP_X, CA_IPP_Y,
                             CA_IPP_W, CA_IPP_H,
                             0, 0,
                             OBJ_ATTR_ACTIVE, 0);  
  if(p_cont == NULL)
  {
    return ERR_FAILURE ;
  }
  ctrl_set_rstyle(p_cont, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  #if 0
  TRCA
  p_cont = ui_comm_root_create(ROOT_ID_CA_IPP,
                             RSI_TRANSPARENT,
                             CA_IPP_X, CA_IPP_Y,
                             CA_IPP_W, CA_IPP_H,
                             IDS_IPPV_INFO/*RSC_INVALID_ID,RSC_INVALID_ID*/, RSI_TRANSPARENT);

  #endif
  ctrl_set_keymap(p_cont, ca_ipp_root_keymap);
  ctrl_set_proc(p_cont, ca_ipp_root_proc);

  //head
  p_head = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_IPP_HEAD),
                            CA_IPP_HEAD_X, CA_IPP_HEAD_Y, CA_IPP_HEAD_W, CA_IPP_HEAD_H, p_cont, 0);  
  ctrl_set_rstyle(p_head, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  
  //head text 
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_HEAD_CONTENT),
                            CA_IPP_HEAD_X+260,CA_IPP_HEAD_Y, 60, 40, p_head, 0);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //icon
  //p_bmp = ctrl_create_ctrl((u8*)CTRL_BMAP, (u8)IDC_IPP_HEAD_ICON,
  //65, 0, 30, 30, p_head, 0);
  //bmap_set_content_by_id(p_bmp, IM_SNAKE_BODY);
  
  //frm
  p_frm = ctrl_create_ctrl((u8*)CTRL_CONT, (u8)(IDC_IPP_FRM),
                            CA_IPP_frm_X, CA_IPP_frm_Y, CA_IPP_frm_W, CA_IPP_frm_H, p_cont, 0);  
  ctrl_set_rstyle(p_frm, RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  

  //ipp type
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_TYPE),
                            20, 20, 160, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IPP_TYPE);

  //ipp price
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_PRICE),
                            20, 60, 160, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IPP_PRICE);

  //ipp distance
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_DISTANCE),
                            20, 100, 160, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IPP_DISTANCE);

  //ipp info
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_INFO),
                            20, 140, 160, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IPP_INFOR2);

  //ipp input pin
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_INPUT_PIN),
                            20, 200, 160, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BG,RSI_SN_BG, RSI_SN_BG);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_INPUT_PIN2);


  //ipp type content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_TYPE_CONTENT),
                            150, 20, 360, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //ipp price content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_PRICE_CONTENT),
                            150, 60, 360, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //ipp distance content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_DISTANCE_CONTENT),
                            150, 100, 420, 30, p_frm, 0);
 // ctrl_set_rstyle(p_ctrl, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //ipp info content
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_INFO_CONTENT),
                            150, 140, 360, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
  //ipp pin info
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_INFO_PIN),
                            150, 170, 320, 30, p_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_VCENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_PIN_ERROR);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_HIDDEN);

  //ipp help
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_IPP_HELP),
                            20, 250, 400, 30, p_frm, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL, RSI_LIST_ITEM_HL);
  text_set_font_style(p_ctrl, FSI_INDEX3, FSI_INDEX3, FSI_INDEX3);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_IPP_HELP);

  //password
  p_mbox = ctrl_create_ctrl((u8*)CTRL_MBOX, IDC_IPP_PSW,
                            150, 200,
                            300, 40,
                            p_frm, 0);
  ctrl_set_keymap(p_mbox, ipp_pwdlg_edit_keymap);
  ctrl_set_proc(p_mbox, ipp_pwdlg_edit_proc);
  mbox_enable_string_mode(p_mbox, FALSE);
  mbox_set_count(p_mbox, 6, 6, 1);
  mbox_set_item_rect(p_mbox, 0,
                     0,
                     300,
                     40,
                     0,
                     10);
  mbox_set_item_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_icon_offset(p_mbox, 0, 0);
  mbox_set_icon_align_type(p_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_focus(p_mbox, 0);
  for(i=0; i <6; i++)
  {
    mbox_set_content_by_icon(p_mbox, i,  IM_SN_F1, IM_SN_F3);//IM_GAME_TETRIS_1, IM_GAME_TETRIS_5);
  }
  
 // #ifdef WIN32
  //ipp_show_info(p_cont, FALSE);
//  #else
  ipp_info_set_content(p_cont, (ipp_buy_info_t *)para2);
//  #endif
  
  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

static RET_CODE on_ca_ipp_exit(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  g_ipp_pwdlg_idata.input_bit = 0;
  g_ipp_pwdlg_idata.input_value = 0;
  if(g_ipp_buy_info.state_flag == 0)
  {
    //ui_set_playpg_noaccess(TRUE);
  }
  return ERR_NOFEATURE;
}

#if 1
static RET_CODE on_ca_ipp_exit_updown(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  on_ca_ipp_exit(p_cont, msg, para1, para2);
  //fw_notify_parent(ROOT_ID_CA_IPP, NOTIFY_T_KEY, FALSE, (u16)para1, para1, 0);
  //fw_destroy_mainwin_by_id(ROOT_ID_CA_IPP);
  get_customer_config(&cus_cfg);
  //manage_close_menu(ROOT_ID_CA_IPP, 0, 0);
  manage_notify_root(ROOT_ID_CA_IPP, MSG_EXIT_ALL, 0, 0);
  //manage_open_menu(ROOT_ID_PROG_BAR, para1, para2);
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
  {
    UI_PRINTF("on_ca_ipp_exit_updown\n");
    if(cus_cfg.customer == CUSTOMER_TONGGUANG
      || cus_cfg.customer == CUSTOMER_YINHE
      || cus_cfg.customer == CUSTOMER_TONGGUANG_CG
      || cus_cfg.customer == CUSTOMER_JINYA
      || cus_cfg.customer == CUSTOMER_TONGGUANG_QY
      || cus_cfg.customer == CUSTOMER_PANLONGHU_TR)
    {
      #ifdef PATULOUS_FUNCTION_ADS
      ui_pic_play(ADS_AD_TYPE_BANNER, ROOT_ID_PROG_BAR);
      #endif
    }
    manage_notify_root(ROOT_ID_PROG_BAR, msg, para1, para2);
  }
  
  return SUCCESS;
}
#endif

static RET_CODE on_ipp_pwdlg_char(control_t *p_mbox, u16 msg,
                              u32 para1, u32 para2)
{
  control_t *p_parent, *p_txt, *p_cont;
  
  p_parent = p_mbox->p_parent;
  p_txt = ctrl_get_child_by_id(p_parent, IDC_IPP_INFO_PIN);
  OS_PRINTF("%s msd %d\n",__FUNCTION__,msg);
  // process MSG_CHAR
  mbox_class_proc(p_mbox, MSG_FOCUS_RIGHT, para1, para2);

  g_ipp_pwdlg_idata.input_bit++;
  g_ipp_pwdlg_idata.input_value = g_ipp_pwdlg_idata.input_value * 10 +
                              (msg & MSG_DATA_MASK) - '0';
  OS_PRINTF("%s input_value %d\n",__FUNCTION__,g_ipp_pwdlg_idata.input_value);

  if (g_ipp_pwdlg_idata.input_bit == 6) // finish input ?
  {
    p_cont = p_mbox->p_parent->p_parent;
    //if (g_ipp_pwdlg_idata.input_value == g_ipp_pwdlg_idata.total_value)
    //{
      //g_ipp_pwdlg_ret = PWDLG_RET_SUCCESS;
      
	  OS_PRINTF("%s send msg MSG_CORRECT_PWD\n",__FUNCTION__);
      ctrl_process_msg(p_cont, MSG_CORRECT_PWD, g_ipp_pwdlg_idata.input_value, 0);
      // reset & reinput
      //ebox_empty_content(p_mbox);
      g_ipp_pwdlg_idata.input_bit = 0;
      g_ipp_pwdlg_idata.input_value = 0;
    //}
    //else
    //{
     // g_ipp_pwdlg_ret = PWDLG_RET_FAILURE;
     // ctrl_process_msg(p_cont, MSG_WRONG_PWD, g_ipp_pwdlg_idata.input_value, 0);
      // reset & reinput
      //ebox_empty_content(p_mbox);
     // g_ipp_pwdlg_idata.input_bit = 0;
     // g_ipp_pwdlg_idata.input_value = 0;
     // text_set_content_by_strid(p_txt, IDS_PIN_ERROR);
     // ctrl_set_attr(p_txt, OBJ_ATTR_ACTIVE);
     // ctrl_paint_ctrl(p_txt, TRUE);
      // update view
      //p_txt = ctrl_get_child_by_id(p_cont, IDC_TXT);
      //if (p_txt != NULL)
      //{
      //  text_set_content_by_strid(p_txt, IDS_INPUT_PASSWORD_ERROR);
      //  ctrl_paint_ctrl(p_txt, TRUE);
      //}
      
    //}
  }

  return SUCCESS;
}

static RET_CODE on_ca_ipp_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ipp_info_set_content(p_cont, (ipp_buy_info_t *)para2);
  
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_ca_ipp_buy_ok(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 pin_old_psw;
  s8 i =0, pin = 0;

  pin_old_psw = para1;
  
  for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
  {
    pin = pin_old_psw%10;
    g_ipp_buy_info.pin[i] = pin;
    pin_old_psw = pin_old_psw/10;
  }
  
  UI_PRINTF("[on_ca_ipp_buy_ok]ipp_buy_info pin %s\n",g_ipp_buy_info.pin);
  UI_PRINTF("[on_ca_ipp_buy_ok]ipp_buy_info channel id %d\n",g_ipp_buy_info.channel_id);
  UI_PRINTF("[on_ca_ipp_buy_ok]ipp_buy_info ipp_charge %d\n",g_ipp_buy_info.ipp_charge);
  UI_PRINTF("[on_ca_ipp_buy_ok]ipp_buy_info price %d\n",g_ipp_buy_info.ipp_price[0].price);
  UI_PRINTF("[on_ca_ipp_buy_ok]ipp_buy_info price type %d\n",g_ipp_buy_info.ipp_price[0].price_type);

  ui_ca_get_info(CAS_CMD_IPP_BUY_SET, (u32)&g_ipp_buy_info, 0);
  //ui_set_playpg_noaccess(FALSE);
  return SUCCESS;
}

static RET_CODE on_ca_ipp_buy_result(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm, *p_result;

  p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
  p_result = ctrl_get_child_by_id(p_frm, IDC_IPP_INFO_PIN);

  UI_PRINTF("on_ca_ipp_buy_result %d\n",para1);


  switch(para1)
  {
      case SUCCESS:
        manage_close_menu( ROOT_ID_CA_IPP,  0, 0);
        break;
        
      case CAS_E_PARAM_ERR: /* parameter error */
        text_set_content_by_strid(p_result, IDS_PARAM_ERROR);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;

      case CAS_E_PIN_INVALID:  /* invalid pin */
        text_set_content_by_strid(p_result, IDS_PIN_ERROR);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;
          
      case CAS_E_NVRAM_ERROR: /* Nvram error from smart card */
        text_set_content_by_strid(p_result, IDS_NVRAM_ERROR);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;
          
      case CAS_E_MEM_ALLOC_FAIL: /* No enough money */
        text_set_content_by_strid(p_result, IDS_CAS_E_CARD_IPPV_NO_MONEY);//IDS_NO_ENOUGH_MEM);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;
          
      case CAS_E_CARD_NO_ROOM: /* No space save product */
        text_set_content_by_strid(p_result, IDS_SPACE_FULL);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;
          
      default: /* CAS_E_IOCMD_NOT_OK */
        text_set_content_by_strid(p_result, IDS_UNKNOWN_ERROR);
        ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
        ctrl_paint_ctrl(p_result,  TRUE);
        break;
      
  }
  return SUCCESS;
}
  
BEGIN_KEYMAP(ca_ipp_root_keymap, ui_comm_root_keymap)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  //ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_ipp_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_ipp_root_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_EXIT, on_ca_ipp_exit)
  //ON_COMMAND(MSG_EXIT_ALL, on_ca_ipp_exit)
  ON_COMMAND(MSG_CA_IPPV_BUY_INFO, on_ca_ipp_info_update)
  ON_COMMAND(MSG_CA_IPP_BUY_OK, on_ca_ipp_buy_result)
  //ON_COMMAND(MSG_EXIT, on_exit_ca_ipp)
  ON_COMMAND(MSG_CORRECT_PWD, on_ca_ipp_buy_ok)
  ON_COMMAND(MSG_FOCUS_UP, on_ca_ipp_exit_updown)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ca_ipp_exit_updown)
END_MSGPROC(ca_ipp_root_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ipp_pwdlg_edit_keymap, NULL)
  ON_EVENT(V_KEY_0, MSG_CHAR | '0')
  ON_EVENT(V_KEY_1, MSG_CHAR | '1')
  ON_EVENT(V_KEY_2, MSG_CHAR | '2')
  ON_EVENT(V_KEY_3, MSG_CHAR | '3')
  ON_EVENT(V_KEY_4, MSG_CHAR | '4')
  ON_EVENT(V_KEY_5, MSG_CHAR | '5')
  ON_EVENT(V_KEY_6, MSG_CHAR | '6')
  ON_EVENT(V_KEY_7, MSG_CHAR | '7')
  ON_EVENT(V_KEY_8, MSG_CHAR | '8')
  ON_EVENT(V_KEY_9, MSG_CHAR | '9')
END_KEYMAP(ipp_pwdlg_edit_keymap, NULL)

BEGIN_MSGPROC(ipp_pwdlg_edit_proc, mbox_class_proc)
  ON_COMMAND(MSG_CHAR, on_ipp_pwdlg_char)
END_MSGPROC(ipp_pwdlg_edit_proc, mbox_class_proc)

