/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_tvsys_set.h"

enum control_id
{
  IDC_DISPLAY_MODE = 1,
  IDC_ASPECT_MODE,
  IDC_VIDEO_OUTPUT,
  IDC_RF_SYSTEM,
  IDC_RF_CHANNEL
};

static u16 tvsys_set_cont_keymap(u16 key);

static RET_CODE tvsys_set_select_proc(control_t *p_ctrl,
                                      u16 msg,
                                      u32 para1,
                                      u32 para2);

static RET_CODE tvsys_set_comm_fill_content(control_t *p_ctrl,
                                            u16 focus,
                                            u16 *p_str,
                                            u16 max_length)
{
  static u8 *display_mode_str[] = {"Auto", "NTSC", "PAL", "PAL-M", "PAL-N"};
  static u8 *aspect_mode_str[] = {"4:3PanScan", "4:3LetterBox", "16:9PanScan", "16:9LetterBox"};
  static u8 *video_output_str[] = {"RGB", "CVBS"};
  static u8 *rf_system_str[] = {"NTSC", "PAL-BG", "PAL-I", "PAL-DK"};
  static u8 rf_channel[3] = {0};

  u8 *content;

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_DISPLAY_MODE:
      content = display_mode_str[focus];
      break;
    case IDC_ASPECT_MODE:
      content = aspect_mode_str[focus];
      break;
    case IDC_VIDEO_OUTPUT:
      content = video_output_str[focus];
      break;
    case IDC_RF_SYSTEM:
      content = rf_system_str[focus];
      break;
    case IDC_RF_CHANNEL:
      if(cbox_dync_get_count(p_ctrl) == RF_NTSC_CNT)
      {
        focus += RF_NTSC_MIN;
      }
      else
      {
        focus += RF_PAL_MIN;
      }
      sprintf((char *)rf_channel, "%d", focus);
      content = rf_channel;
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  str_nasc2uni(content, p_str, max_length);

  return SUCCESS;
}


RET_CODE open_tvsys_set(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_ctrl[TVSYS_SET_ITEM_CNT];
  u8 i;
  av_set_t av_set;

#ifndef SPT_SUPPORT
  u16 stxt[TVSYS_SET_ITEM_CNT] =
  {
    IDS_DISPLAY_MODE, IDS_ASPECT_MODE, IDS_VIDEO_OUTPUT,
    IDS_RF_SYSTEM, IDS_RF_CHANNEL
  };
  u8 opt_cnt[TVSYS_SET_ITEM_CNT] = {5, 4, 2, 4, 0};
  u16 y;
#endif
  u16 droplist_page[TVSYS_SET_ITEM_CNT] = {5, 3, 0, 5, 5};

#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_TVSYS_SET, RSI_TVSYS_SET_FRM,
                        TVSYS_SET_CONT_X, TVSYS_SET_CONT_Y,
                        TVSYS_SET_CONT_W, TVSYS_SET_CONT_H,
                        IDS_TV_SYSTEM, RSI_TITLE_BG);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, tvsys_set_cont_keymap);

  y = TVSYS_SET_ITEM_Y;
  for(i = 0; i < TVSYS_SET_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_DISPLAY_MODE + i),
                                      TVSYS_SET_ITEM_X, y,
                                      TVSYS_SET_ITEM_LW,
                                      TVSYS_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], tvsys_set_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_DYNAMIC, opt_cnt[i],
                             0, tvsys_set_comm_fill_content);
    ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
    ctrl_set_related_id(p_ctrl[i],
                        0,                                       /* left */
                        (u8)((i - 1 +
                              TVSYS_SET_ITEM_CNT) %
                             TVSYS_SET_ITEM_CNT + 1),            /* up */
                        0,                                       /* right */
                        (u8)((i + 1) % TVSYS_SET_ITEM_CNT + 1)); /* down */

    y += TVSYS_SET_ITEM_H + TVSYS_SET_ITEM_V_GAP;
  }
#else
#endif

  /* set focus according to current info */
  sys_status_get_av_set(&av_set);

  ui_comm_select_set_focus(p_ctrl[0], av_set.tv_mode);
  ui_comm_select_set_focus(p_ctrl[1], av_set.tv_ratio);
  ui_comm_select_set_focus(p_ctrl[2], av_set.video_output);
  ui_comm_select_set_focus(p_ctrl[3], av_set.rf_system);
  ui_comm_select_set_focus(p_ctrl[4], av_set.rf_channel);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}


static RET_CODE on_tvsys_set_select_change(control_t *p_ctrl,
                                           u16 msg,
                                           u32 para1,
                                           u32 para2)
{
  u8 to, from;
  u16 curn;
  av_set_t av_set;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  control_t *p_other = NULL;
  RET_CODE ret = SUCCESS;
  rf_device_t *p_rf = (rf_device_t *)dev_find_identifier(NULL,
                                                         DEV_IDT_TYPE,
                                                         SYS_DEV_TYPE_RF);

  MT_ASSERT(NULL != p_rf);

  sys_status_get_av_set(&av_set);

  from = (u8)(para1), to = (u8)(para2);

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_DISPLAY_MODE:
      // check for initialize
      if (!ctrl_is_onfocus(p_ctrl) 
        || av_set.tv_mode == to)
      {
        return SUCCESS;
      }
      av_set.tv_mode = to;

      avc_switch_video_mode_1(avc_handle, av_set.tv_mode);
      break;
    case IDC_ASPECT_MODE:
      // check for initialize
      if (!ctrl_is_onfocus(p_ctrl) 
        || av_set.tv_ratio == to)
      {
        return SUCCESS;
      }
      av_set.tv_ratio = to;
      avc_set_video_aspect_mode_1(avc_handle, 
                               sys_status_get_video_aspect(av_set.tv_ratio));
      ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
      
      break;
    case IDC_VIDEO_OUTPUT:
      // check for initialize
      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.video_output == to)
      {
        return SUCCESS;
      }
      av_set.video_output = to;
      avc_cfg_scart_format_jazz(avc_handle,
                           sys_status_get_scart_out_cfg(av_set.video_output));
      break;
    case IDC_RF_SYSTEM:
      p_other = ui_comm_root_get_ctrl(ROOT_ID_TVSYS_SET, IDC_RF_CHANNEL);
      ui_comm_select_set_param(p_other, TRUE,
                               CBOX_WORKMODE_DYNAMIC,
                               to ? RF_PAL_CNT : RF_NTSC_CNT,
                               0, tvsys_set_comm_fill_content);
      curn = ui_comm_select_get_focus(p_other);
      if(from == 0 && to > 0) /* N to P */
      {
        if(curn + RF_NTSC_MIN < RF_PAL_MIN || curn + RF_NTSC_MIN > RF_PAL_MAX)
        {
          curn = 0;
        }
        else
        {
          curn -= (RF_PAL_MIN - RF_NTSC_MIN);
        }
      }
      else if(from > 0 && to == 0)  /* P to N*/
      {
        curn += (RF_PAL_MIN - RF_NTSC_MIN);
      }

      ui_comm_select_set_focus(p_other, curn);
      ui_comm_ctrl_update_attr(p_other, TRUE);

      // check for initialize
      if(!ctrl_is_onfocus(p_ctrl)
        || av_set.rf_system == to)
      {
        return SUCCESS;
      }
      else
      {
        av_set.rf_system = to;
        avc_set_rf_system_1(avc_handle,
                          sys_status_get_rf_system(av_set.rf_system));
        ctrl_paint_ctrl(p_other, TRUE);
      }

      break;
    case IDC_RF_CHANNEL:
      // check for initialize
      if(!ctrl_is_onfocus(p_ctrl)
        || av_set.rf_channel == to)
      {
        return SUCCESS;
      }
      av_set.rf_channel = to;
      p_other = ui_comm_root_get_ctrl(ROOT_ID_TVSYS_SET, IDC_RF_SYSTEM);
      avc_set_rf_channel_1(avc_handle,
                         sys_status_get_rf_channel(
                           ui_comm_select_get_focus(p_other) > 0, 
                           av_set.rf_channel));
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  sys_status_set_av_set(&av_set);
  sys_status_save();

  return ret;
}


BEGIN_KEYMAP(tvsys_set_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(tvsys_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tvsys_set_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_tvsys_set_select_change)
END_MSGPROC(tvsys_set_select_proc, cbox_class_proc)
