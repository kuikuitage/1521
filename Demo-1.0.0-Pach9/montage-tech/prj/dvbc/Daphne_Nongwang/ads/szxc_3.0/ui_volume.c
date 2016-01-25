/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_volume.h"
#include "ui_mute.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CONT,
  IDC_BAR,
  IDC_NUMBER,
  IDC_AD_WIN
};

static dvbs_prog_node_t curn_prog;

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
  curn_prog.volume = actual_volume;
}


RET_CODE open_volume(u32 para1, u32 para2)
{
  control_t *p_cont, *p_bar, *p_volume,*p_adv;
#ifndef SPT_SUPPORT
  control_t *p_frm;
#endif
  u8 asc_buf[16];
  u16 prog_id;
  u8 volume;

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

  /* get volume */
  volume = (u8)curn_prog.volume;
  
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
                                  0, VOLUME_T,
                                  SCREEN_WIDTH, 110,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_VOLUME_CONT,
                  RSI_VOLUME_CONT, RSI_VOLUME_CONT);
  ctrl_set_proc(p_cont, volume_proc);
  ctrl_set_keymap(p_cont, volume_keymap);

  p_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_CONT,
                           10, 10,
                           400, VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_SN_VOLUME,
                  RSI_SN_VOLUME,
                  RSI_SN_VOLUME);
  
  p_bar = ctrl_create_ctrl((u8 *)CTRL_PBAR, IDC_BAR,
                           VOLUME_BAR_X, VOLUME_BAR_Y,
                           270, VOLUME_BAR_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_SN_COMM_PBAR_BG, RSI_SN_COMM_PBAR_BG, RSI_SN_COMM_PBAR_BG);
  pbar_set_mid_rect(p_bar, 1, 0, 270 -2, VOLUME_BAR_H);
  pbar_set_rstyle(p_bar, RSI_COMM_PBAR_MID, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_count(p_bar, 0, VOLUME_MAX, VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 1, 0);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_bar_proc);
  ctrl_set_keymap(p_bar, volume_bar_keymap);

  //volume number
  p_volume = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMBER,
                            350, VOLUME_TEXT_Y,
                            VOLUME_TEXT_W, VOLUME_TEXT_H,
                            p_frm, 0);
  ctrl_set_rstyle(p_volume, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_align_type(p_volume, STL_CENTER |STL_VCENTER);
  text_set_font_style(p_volume, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
  text_set_content_type(p_volume, TEXT_STRTYPE_UNICODE);
  sprintf((char *)asc_buf, "%.2d ", volume);
  text_set_content_by_ascstr(p_volume, asc_buf);


  p_adv = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_AD_WIN,
                             VOLUME_BAR_AD_X, 0,
                             VOLUME_BAR_AD_W,VOLUME_BAR_AD_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_adv, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);
  bmp_pic_draw(p_adv);


#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_pic_stop();
  ui_pic_play(ADS_AD_TYPE_VOLBAR, ROOT_ID_VOLUME);
  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

RET_CODE ui_check_fullscr_ad_play(void);
RET_CODE close_volume(void)
{
  ui_pic_stop();
  manage_close_menu(ROOT_ID_VOLUME, 0, 0);
  /* sav volume into dbase */
  db_dvbs_edit_program(&curn_prog);
  db_dvbs_save_pg_edit(&curn_prog);
  ui_check_fullscr_ad_play();
  return SUCCESS;
}


static RET_CODE on_exit_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_volume();


  return SUCCESS;
}

static RET_CODE on_volume_draw_pic_end(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("on_volume_draw_pic_end!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  return ERR_NOFEATURE;
}
static RET_CODE on_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;
  u8 asc_buf[16];
  control_t *p_volume;
  pbar_class_proc(p_ctrl, msg, para1, para2);
  /* set volume */
  volume =(u8)pbar_get_current(p_ctrl);
  p_volume = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_NUMBER);
  sprintf((char *)asc_buf, "%.2d ", volume);
  text_set_content_by_ascstr(p_volume, asc_buf);
  ctrl_paint_ctrl(p_volume->p_parent,TRUE);
  set_volume((u8)pbar_get_current(p_ctrl));
  return SUCCESS;
}

BEGIN_KEYMAP(volume_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(volume_keymap, NULL)

BEGIN_MSGPROC(volume_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
END_MSGPROC(volume_proc, cont_class_proc)

BEGIN_KEYMAP(volume_bar_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
END_KEYMAP(volume_bar_keymap, NULL)

BEGIN_MSGPROC(volume_bar_proc, pbar_class_proc)
  ON_COMMAND(MSG_INCREASE, on_change_volume)
  ON_COMMAND(MSG_DECREASE, on_change_volume)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_volume_draw_pic_end)
END_MSGPROC(volume_bar_proc, pbar_class_proc)


