/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_ca_finger.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
};

void open_ca_finger_menu(finger_msg_t *p_ca_finger)
{
  control_t *p_txt, *p_cont = NULL;
  u16 finger_x = 0;
  u16 finger_y = 0;
  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    return;
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_CA_FINGER))
  {
      close_ca_finger_menu();
  }
  if(p_ca_finger->show_postion == 0)
  {
    finger_x = 20;
    finger_y = 60;
  }
  else if(p_ca_finger->show_postion == 1)
  {
    finger_x = 460;
    finger_y = 60;
  }
  else if(p_ca_finger->show_postion == 2)
  {
    finger_x = 20;
    finger_y = 510;
  }
  else if(p_ca_finger->show_postion == 3)
  {
    finger_x = 460;
    finger_y = 510;
  }
  else
  {
    finger_x = p_ca_finger->show_postion_x;
    finger_y = p_ca_finger->show_postion_y;
  } 
  
  if(finger_x < 0)
    finger_x = 20;
    
  if(finger_x > (SCREEN_WIDTH - 200))
    finger_x = SCREEN_WIDTH - 200;

  //roll_title y pos + roll title width 
  if(finger_y < 60)
    finger_y = 60;
  
  //roll_title + finger_width = 30
  if(finger_y > (SCREEN_HEIGHT - 80))
    finger_y = SCREEN_HEIGHT - 60;
  
  p_cont = fw_create_mainwin(ROOT_ID_CA_FINGER,
                             finger_x, finger_y,
                             200, 30,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return;
  }
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);


  p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                           0, 0, 200, 30,
                           p_cont, 0);
  ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_CA_FINGER_TXT, FSI_CA_FINGER_TXT, FSI_CA_FINGER_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_txt, p_ca_finger->data);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_paint_ctrl(p_cont, FALSE);
}
void close_ca_finger_menu(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_FINGER);
}

