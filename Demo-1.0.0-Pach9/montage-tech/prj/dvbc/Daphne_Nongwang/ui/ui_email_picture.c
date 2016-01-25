/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_email_picture.h"

static BOOL is_pic_flash = FALSE;
static BOOL g_is_email = FALSE;
BOOL email_pic_exit = FALSE;
enum nvod_email_pic_local_msg
{
  MSG_PIC_FLASH = MSG_LOCAL_BEGIN + 175,
};

enum control_id
{
  IDC_BMP_ICON = 1,
};

BOOL ui_is_email(void)
{
  return g_is_email;
}
static RET_CODE email_pic_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);


static void close_email_picture(void)
{
  g_is_email = FALSE;
  fw_destroy_mainwin_by_id(ROOT_ID_EMAIL_PIC);
}

RET_CODE open_email_picture(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  u16 cont_x, cont_y, cont_w, cont_h;
  u16 icon_x, icon_y, icon_w, icon_h;
  u16 icon_id;
  u8 focus = fw_get_focus_id();
  g_is_email = TRUE;
  // check for close
  if(fw_find_root_by_id(ROOT_ID_EMAIL_PIC) != NULL)
  {
    close_email_picture();
     g_is_email = TRUE;
  }
 
  if(ui_is_fullscreen_menu(focus) ||(fw_find_root_by_id(ROOT_ID_CA_IPPV_PPT_DLG) != NULL))
  {
    cont_x = EMAIL_CONT_FULL_X, cont_y = EMAIL_CONT_FULL_Y;
    cont_w = EMAIL_CONT_FULL_W, cont_h = EMAIL_CONT_FULL_H;
    icon_x = EMAIL_ICON_FULL_X, icon_y = EMAIL_ICON_FULL_Y;
    icon_w = EMAIL_ICON_FULL_W, icon_h = EMAIL_ICON_FULL_H;
    icon_id = IM_SN_SCREEN_EMAIL;
  }
  else
  {
     return SUCCESS;
   }
 

  p_cont = fw_create_mainwin(ROOT_ID_EMAIL_PIC,
                             cont_x, cont_y, cont_w, cont_h,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, email_pic_cont_proc);
  
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_BMP_ICON,
                            icon_x, icon_y, icon_w, icon_h,
                            p_cont, 0);
  bmap_set_content_by_id(p_icon, icon_id);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif


  return SUCCESS;
}


void update_email_picture(email_pic_show_mode_t e_show_mode)
{
  control_t *p_cont = NULL;
  RET_CODE ret= SUCCESS;

  if((is_pic_flash)&&(EMAIL_PIC_FLASH  != e_show_mode))
  {
    is_pic_flash = FALSE;
    fw_tmr_destroy(ROOT_ID_EMAIL_PIC, MSG_PIC_FLASH);
    close_email_picture();
  }

  switch(e_show_mode)
  {
      case EMAIL_PIC_SHOW:
        open_email_picture(0, 0);
        break;
      case EMAIL_PIC_HIDE:
        close_email_picture();
        break;
      case EMAIL_PIC_FLASH:
        if(!is_pic_flash)
        {
          if((p_cont = fw_find_root_by_id(ROOT_ID_EMAIL_PIC)) == NULL)
          {
              open_email_picture(0, 0);
          }
          ret = fw_tmr_create(ROOT_ID_EMAIL_PIC, MSG_PIC_FLASH, 4000, TRUE);
          is_pic_flash = TRUE;
        }
        break;
      default:
        return;
  }
  
  return ;
}

static RET_CODE on_email_pic_flash(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bmp_ctrl = NULL;
  static u16 us_flash_count = 0;

  if(!is_pic_flash)
  {
      return ERR_FAILURE;
  }

  p_bmp_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BMP_ICON);
  if(NULL == p_bmp_ctrl)
  {
      return ERR_FAILURE;
  }

  us_flash_count ++;
  us_flash_count = us_flash_count%2;
  
  if(0 == us_flash_count)
  {
     bmap_set_content_by_id(p_bmp_ctrl, 0);
  }
  else if(1 == us_flash_count)
  {

   //  bmap_set_content_by_id(p_bmp_ctrl, IM_SN_SCREEN_EMAIL);
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}


BEGIN_MSGPROC(email_pic_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PIC_FLASH, on_email_pic_flash)
END_MSGPROC(email_pic_cont_proc, ui_comm_root_proc)

