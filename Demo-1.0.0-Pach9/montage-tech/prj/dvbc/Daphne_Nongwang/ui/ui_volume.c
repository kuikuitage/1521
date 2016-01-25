/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_volume.h"
#include "ui_mute.h"


#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "desai_ca_api_v5.0.h"
#include "ds_cas_include_v5.0.h"
#endif



enum control_id
{
  IDC_INVALID = 0,
  IDC_CONT,
  IDC_BAR,
  IDC_NUMBER,
  IDC_AD_WIN
};

static dvbs_prog_node_t curn_prog;
static audio_set_t audio = {0};

u16 volume_keymap(u16 key);
RET_CODE volume_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 volume_bar_keymap(u16 key);
RET_CODE volume_bar_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


static void set_volume(u8 volume)
{
  u8 actual_volume = 0;

  if(volume <= AP_VOLUME_MAX)
  {
      actual_volume = volume;
  }
  else
  {
      actual_volume = AP_VOLUME_MAX;
  }
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), 
  actual_volume);
  audio.global_volume = actual_volume;
  curn_prog.volume = actual_volume;
  sys_status_set_audio_set(&audio);
  sys_status_save();
}


RET_CODE open_volume(u32 para1, u32 para2)
{
  #define SN_VOLUME_X	0
  #define SN_VOLUME_Y	400
  #define SN_VOLUME_W	640
  #define SN_VOLUME_H	90
  #define SN_VOLUME_BAR_CONT_X	0
  #define SN_VOLUME_BAR_CONT_Y	0
  #define SN_VOLUME_BAR_CONT_W	394
  #define SN_VOLUME_BAR_CONT_H	90
  #define SN_VOLUME_LABA_X	10
  #define SN_VOLUME_LABA_Y	35
  #define SN_VOLUME_LABA_W	38
  #define SN_VOLUME_LABA_H	35
  #define SN_VOLUME_AD_X	404
  #define SN_VOLUME_AD_Y    0
  #define SN_VOLUME_AD_W	236//252//
  #define SN_VOLUME_AD_H	90//116//
  #define SN_VOLUME_BAR_W	314
  
  control_t *p_cont, *p_bar, *p_volume;
#ifndef SPT_SUPPORT
  control_t *p_frm,*p_lab,*p_icon;
  control_t *p_ctrl;
#endif
  u8 asc_buf[16];
  u16 prog_id;
  u8 volume;

  customer_cfg_t cus_cfg = {0};
  //prog_id = ui_sys_get_curn_prog_in_curn_view();
  prog_id = sys_status_get_curn_group_curn_prog_id();
  if (db_dvbs_get_pg_by_id(prog_id, &curn_prog) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  get_customer_config(&cus_cfg);
    /* get volume */
  if((cus_cfg.customer== CUSTOMER_TAIHUI) || (cus_cfg.customer== CUSTOMER_FANTONG_XSMAJX))
  {
    volume = get_audio_global_volume();
  }
  else
  {
    volume = (u8)curn_prog.volume;
  }
  
  switch (para1)
  {
    case V_KEY_LEFT:
      if ((volume > 0) && (volume != AP_VOLUME_MAX))
      {
        volume--;
        /* set volume */
        set_volume(volume);
      }
      break;
    case V_KEY_VDOWN:
      if ((volume > 0) && (volume != AP_VOLUME_MAX))
      {
        volume--;
        /* set volume */
        set_volume(volume);
      }
      break;  
    case V_KEY_RIGHT:
      if (volume < VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_volume(volume);
      }
      break;
    case V_KEY_VUP:
      if (volume < VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_volume(volume);
      }
      break;
    default:
      /* do nothing */
      break;
  }

  p_cont = fw_create_mainwin(ROOT_ID_VOLUME,
                                  SN_VOLUME_X, SN_VOLUME_Y-20,//VOLUME_T,
                                  SN_VOLUME_W+32, SN_VOLUME_H+26,//VOLUME_H,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_VOLUME_CONT,
                  RSI_VOLUME_CONT, RSI_VOLUME_CONT);//RSI_VOLUME_CONT
  ctrl_set_proc(p_cont, volume_proc);
  ctrl_set_keymap(p_cont, volume_keymap);

  p_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_CONT,
                           SN_VOLUME_BAR_CONT_X+2, SN_VOLUME_BAR_CONT_Y,//VOLUME_BAR_CONT_Y,
                           SN_VOLUME_BAR_CONT_W+6, SN_VOLUME_BAR_CONT_H+26,//VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_SN_VOLUME,
                  RSI_SN_VOLUME,
                  RSI_SN_VOLUME);              //  
//create laba
  p_icon = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_CONT,
                           SN_VOLUME_LABA_X, 0,//VOLUME_BAR_CONT_Y,
                           SN_VOLUME_LABA_W, SN_VOLUME_BAR_CONT_H+26,//VOLUME_BAR_CONT_H,
                           p_frm, 0);

  ctrl_set_rstyle(p_icon,
                  RSI_SN_ICON_LABA,
                  RSI_SN_ICON_LABA,
                  RSI_SN_ICON_LABA);

  p_lab = ctrl_create_ctrl((u8 *)CTRL_CONT, 0,
                           0, SN_VOLUME_LABA_Y+12,//VOLUME_BAR_CONT_Y,
                           SN_VOLUME_LABA_W, SN_VOLUME_LABA_H,//VOLUME_BAR_CONT_H,
                           p_icon, 0);
  ctrl_set_rstyle(p_lab,
                  RSI_SN_VOLUME_LABA,
                  RSI_SN_VOLUME_LABA,
                  RSI_SN_VOLUME_LABA);
  
  p_bar = ctrl_create_ctrl((u8 *)CTRL_PBAR, IDC_BAR,
                           (VOLUME_BAR_X - 20), 39+13,//VOLUME_BAR_Y,
                           SN_VOLUME_BAR_W, VOLUME_BAR_H+6,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_SN_COMM_PBAR_BG, RSI_SN_COMM_PBAR_BG, RSI_SN_COMM_PBAR_BG);
  pbar_set_mid_rect(p_bar, 0, 0, SN_VOLUME_BAR_W , VOLUME_BAR_H+6);
  pbar_set_rstyle(p_bar, RSI_SN_COMM_PBAR_MID, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_count(p_bar, 0, VOLUME_MAX, VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 1, 0);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_bar_proc);
  ctrl_set_keymap(p_bar, volume_bar_keymap);

  //volume number
  p_volume = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMBER,
                            (VOLUME_TEXT_X-51), 30+16,//VOLUME_TEXT_Y,
                            30, VOLUME_TEXT_H,
                            p_frm, 0);
  ctrl_set_rstyle(p_volume, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_align_type(p_volume, STL_CENTER |STL_VCENTER);
  text_set_font_style(p_volume, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
  text_set_content_type(p_volume, TEXT_STRTYPE_UNICODE);
  sprintf((char *)asc_buf, "%.2d ", volume);
  text_set_content_by_ascstr(p_volume, asc_buf);

  // advertisement
	p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_AD_WIN,
							   SN_VOLUME_AD_X+15, SN_VOLUME_AD_Y,
							   SN_VOLUME_AD_W+16,SN_VOLUME_AD_H+26,
							   p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);
	//bmp_pic_draw(p_ctrl);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);

 #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
 #endif
 
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
	  //ui_set_pic_show(FALSE);
  ui_pic_stop();
  ui_pic_play(ADS_AD_TYPE_VOLBAR, ROOT_ID_VOLUME);
#endif

#ifndef WIN32
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  DSCA_SC_UnlockPin();
#endif
#endif

  return SUCCESS;
}


RET_CODE close_volume(void)
{
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  manage_close_menu(ROOT_ID_VOLUME, 0, 0);
  /* sav volume into dbase */
  if((cus_cfg.customer== CUSTOMER_TAIHUI) || (cus_cfg.customer== CUSTOMER_FANTONG_XSMAJX))
  {
    set_audio_global_volume(curn_prog.volume);
    OS_PRINTF("!!!!!!!!!curn_prog.volume = %d\n",curn_prog.volume);
  }
  
  db_dvbs_edit_program(&curn_prog);
  db_dvbs_save_pg_edit(&curn_prog);

  return SUCCESS;
}

extern RET_CODE ui_check_fullscr_ad_play(void);
static RET_CODE on_exit_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_pic_stop();
  close_volume();
  ui_check_fullscr_ad_play();
  return SUCCESS;
}

static RET_CODE on_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;
  u8 asc_buf[16];
  control_t *p_volume;
  customer_cfg_t cus_cfg = {0};
  pbar_class_proc(p_ctrl, msg, para1, para2);
  /* set volume */
  volume =(u8)pbar_get_current(p_ctrl);
  p_volume = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_NUMBER);
  sprintf((char *)asc_buf, "%.2d ", volume);
  text_set_content_by_ascstr(p_volume, asc_buf);
  ctrl_paint_ctrl(p_volume->p_parent,TRUE);
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  
  set_volume((u8)pbar_get_current(p_ctrl));
  
  get_customer_config(&cus_cfg);
  if((cus_cfg.customer== CUSTOMER_TAIHUI) || (cus_cfg.customer== CUSTOMER_FANTONG_XSMAJX))
  {
    set_audio_global_volume(curn_prog.volume);
  }
  db_dvbs_edit_program(&curn_prog);
  db_dvbs_save_pg_edit(&curn_prog);
  //fw_notify_root(p_ctrl, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_pic_play(ADS_AD_TYPE_VOLBAR, ROOT_ID_VOLUME);
  return SUCCESS;
}

BEGIN_KEYMAP(volume_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(volume_keymap, NULL)

BEGIN_MSGPROC(volume_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
END_MSGPROC(volume_proc, cont_class_proc)

BEGIN_KEYMAP(volume_bar_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
END_KEYMAP(volume_bar_keymap, NULL)

BEGIN_MSGPROC(volume_bar_proc, pbar_class_proc)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
  ON_COMMAND(MSG_INCREASE, on_change_volume)
  ON_COMMAND(MSG_DECREASE, on_change_volume)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
END_MSGPROC(volume_bar_proc, pbar_class_proc)


