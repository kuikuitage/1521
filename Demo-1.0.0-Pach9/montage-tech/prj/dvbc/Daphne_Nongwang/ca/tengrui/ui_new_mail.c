/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_new_mail.h"

static BOOL g_is_new_mail = FALSE;

BOOL ui_is_new_mail(void)
{
  return g_is_new_mail;
}

void ui_set_new_mail(BOOL is_new_mail)
{
  g_is_new_mail = is_new_mail;
}

RET_CODE open_ui_new_mail(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  
  // check for close
  if(fw_find_root_by_id(ROOT_ID_NEW_MAIL) != NULL)
  {
    close_new_mail();
  }

  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    return 0;
  }
  
  p_cont = fw_create_mainwin(ROOT_ID_NEW_MAIL,
                             NEW_MAIL_CONT_FULL_X, 
                             NEW_MAIL_CONT_FULL_Y, 
                             NEW_MAIL_CONT_FULL_W, 
                             NEW_MAIL_CONT_FULL_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  p_icon = ctrl_create_ctrl((u8*)CTRL_BMAP, 0,
                            NEW_MAIL_ICON_FULL_X, 
                            NEW_MAIL_ICON_FULL_Y,
                            NEW_MAIL_ICON_FULL_W, 
                            NEW_MAIL_ICON_FULL_H,
                            p_cont, 0);
  
  bmap_set_content_by_id(p_icon, IM_SN_SCREEN_EMAIL);//IM_ICON_MAIL);IM_SN_F1  wwww

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


void close_new_mail(void)
{
  //manage_close_menu(ROOT_ID_NEW_MAIL, 0, 0);
  fw_destroy_mainwin_by_id(ROOT_ID_NEW_MAIL);
  fw_destroy_mainwin_by_id(ROOT_ID_EMAIL_PIC);
}
