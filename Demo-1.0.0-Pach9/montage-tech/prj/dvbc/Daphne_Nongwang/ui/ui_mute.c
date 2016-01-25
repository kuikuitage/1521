/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_volume.h"
#include "ui_nvod_volume.h"

//#include "hal_gpio.h"
static BOOL g_is_mute = FALSE;

BOOL ui_is_mute(void)
{
  return g_is_mute;
}


void ui_set_mute(BOOL is_mute)
{
  g_is_mute = is_mute;
  if(g_is_mute)
  {
    u8 focus = fw_get_focus_id();

    /* try to close volume bar */
    if(fw_find_root_by_id(ROOT_ID_VOLUME) != NULL)
    {
      UI_PRINTF("set_mute: close volume\n");
      close_volume();
    }
    
    if(ui_is_fullscreen_menu(focus))
    {
      open_mute(0, 0);
    }
    else if((ui_is_preview_menu(focus))
              ||(focus == ROOT_ID_NVOD_VIDEO)
              ||(focus == ROOT_ID_MOSAIC)
              ||(focus == ROOT_ID_MOSAIC_PLAY))
    {
        open_mute(0, 0);
    }
    #ifndef WIN32
    set_volume_mute_by_hardware(1);
    #endif
  }
  else
  {
    close_mute();
    #ifndef WIN32
    set_volume_mute_by_hardware(0);
    #endif 
  }

  /* do something to set mute */
  UI_PRINTF("AP: set_mute = %d\n", g_is_mute);
  avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID),
               g_is_mute, TRUE);
}


RET_CODE open_mute(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  u16 cont_x, cont_y, cont_w, cont_h;
  u16 icon_x, icon_y, icon_w, icon_h;
  u16 icon_id;
  u16 left, top, width, height;
  BOOL is_fullscr = TRUE;

  // check for close
  if(fw_find_root_by_id(ROOT_ID_MUTE) != NULL)
  {
    close_mute();
  }

  if(manage_get_preview_rect(fw_get_focus_id(),
                             &left, &top,
                             &width, &height))
  {
    is_fullscr = FALSE;
  }

  if(is_fullscr)  // list mode // LXD FULL SCREEN MODEL
  {
    cont_x = MUTE_CONT_FULL_X-50, cont_y = MUTE_CONT_FULL_Y+20;  //LXD " X -"MOVE LEFT  "Y+"MOVE DOWN
    cont_w = MUTE_CONT_FULL_W, cont_h = MUTE_CONT_FULL_H;
    icon_x = MUTE_ICON_FULL_X, icon_y = MUTE_ICON_FULL_Y;
    icon_w = MUTE_ICON_FULL_W, icon_h = MUTE_ICON_FULL_H;
    icon_id = IM_SN_ICON_MUTE;//// LXD QUITE MAP
  }
  else
  {
    cont_x = left + width - MUTE_CONT_LIST_W - MUTE_CONT_LIST_GAP;
    cont_y = top + MUTE_CONT_LIST_GAP+15;    //LXD ADD "+15"
    cont_w = MUTE_CONT_LIST_W;
    cont_h = MUTE_CONT_LIST_H;
    
    icon_x = MUTE_ICON_LIST_X, icon_y = MUTE_ICON_LIST_Y;
    icon_w = MUTE_ICON_LIST_W, icon_h = MUTE_ICON_LIST_H;
    icon_id = IM_SN_ICON_MUTE;
  }

  p_cont = fw_create_mainwin(ROOT_ID_MUTE,
                             cont_x, cont_y, cont_w, cont_h,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, 0,
                            icon_x, icon_y, icon_w, icon_h,
                            p_cont, 0);
  bmap_set_content_by_id(p_icon, icon_id);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_paint_ctrl(p_cont, FALSE);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  return SUCCESS;
}


void close_mute(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_MUTE);
}



