/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_nvod_volume.h"
#include "ui_mute.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CONT,
  IDC_BAR,
  IDC_NUMBER
};

static audio_set_t audio = {0};
static u8 volume = 0;
u16 volume_nvod_keymap(u16 key);
RET_CODE volume_nvod_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 volume_nvod_bar_keymap(u16 key);
RET_CODE volume_nvod_bar_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


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
  sys_status_set_audio_set(&audio);
  sys_status_save();
}


RET_CODE open_nvod_mosic_volume(u32 para1, u32 para2)
{
  control_t *p_cont, *p_bar, *p_volume;
#ifndef SPT_SUPPORT
  control_t *p_frm;
#endif
  u8 asc_buf[16];

  sys_status_get_audio_set(&audio);

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  /* get volume */
  volume = (u8)audio.global_volume;

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
    case V_KEY_VUP:
      if (volume < VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_volume(volume);
      }  
    default:
      /* do nothing */
      break;
  }

  p_cont = fw_create_mainwin(ROOT_ID_MOSNVOD_VOLUME,
                                  VOLUME_L, VOLUME_T,
                                  VOLUME_W, VOLUME_H,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_VOLUME_CONT,
                  RSI_VOLUME_CONT, RSI_VOLUME_CONT);
  ctrl_set_proc(p_cont, volume_nvod_proc);
  ctrl_set_keymap(p_cont, volume_nvod_keymap);

  p_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_CONT,
                           VOLUME_BAR_CONT_X, VOLUME_BAR_CONT_Y,
                           VOLUME_BAR_CONT_W, VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_VOLUME_BAR_FRAME,
                  RSI_VOLUME_BAR_FRAME,
                  RSI_VOLUME_BAR_FRAME);

  p_volume = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMBER,
                            VOLUME_TEXT_X, VOLUME_TEXT_Y,
                            VOLUME_TEXT_W, VOLUME_TEXT_H,
                            p_frm, 0);
  ctrl_set_rstyle(p_volume, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_align_type(p_volume, STL_CENTER |STL_VCENTER);
  text_set_font_style(p_volume, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
  text_set_content_type(p_volume, TEXT_STRTYPE_UNICODE);
  sprintf((char *)asc_buf, "%.2d ", volume);
  text_set_content_by_ascstr(p_volume, asc_buf);
  
  p_bar = ctrl_create_ctrl((u8 *)CTRL_PBAR, IDC_BAR,
                           VOLUME_BAR_X, VOLUME_BAR_Y,
                           VOLUME_BAR_W, VOLUME_BAR_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_COMM_PBAR_BG, RSI_COMM_PBAR_BG, RSI_COMM_PBAR_BG);
  pbar_set_mid_rect(p_bar, 1, 0, VOLUME_BAR_W-2, VOLUME_BAR_H);
  pbar_set_rstyle(p_bar, M_SOUND_STATE_BAR_MI, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_count(p_bar, 0, VOLUME_MAX, VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 1, 0);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_nvod_bar_proc);
  ctrl_set_keymap(p_bar, volume_nvod_bar_keymap);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif


  return SUCCESS;
}


RET_CODE close_nvod_mosic_volume(void)
{
  manage_close_menu(ROOT_ID_MOSNVOD_VOLUME, 0, 0);
  sys_status_save();

  return SUCCESS;
}


static RET_CODE on_exit_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  close_nvod_mosic_volume();
  return SUCCESS;
}

static RET_CODE on_nvod_mosic_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
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

static RET_CODE on_nvod_mosic_input_num(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  return SUCCESS;
}


BEGIN_KEYMAP(volume_nvod_keymap, NULL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(volume_nvod_keymap, NULL)

BEGIN_MSGPROC(volume_nvod_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
END_MSGPROC(volume_nvod_proc, cont_class_proc)

BEGIN_KEYMAP(volume_nvod_bar_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
  ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
  ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
  ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
  ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
  ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
  ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
  ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
  ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
  ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(volume_nvod_bar_keymap, NULL)

BEGIN_MSGPROC(volume_nvod_bar_proc, pbar_class_proc)
  ON_COMMAND(MSG_INCREASE, on_nvod_mosic_change_volume)
  ON_COMMAND(MSG_DECREASE, on_nvod_mosic_change_volume)
  ON_COMMAND(MSG_NUMBER, on_nvod_mosic_input_num)
  ON_COMMAND(MSG_FOCUS_DOWN, on_nvod_mosic_input_num)
  ON_COMMAND(MSG_FOCUS_UP, on_nvod_mosic_input_num)
  ON_COMMAND(MSG_EXIT, on_exit_volume)
END_MSGPROC(volume_nvod_bar_proc, pbar_class_proc)


