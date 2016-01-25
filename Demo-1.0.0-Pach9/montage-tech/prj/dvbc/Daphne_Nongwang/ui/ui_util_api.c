/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"


void ui_enable_uio(BOOL is_enable)
{
  cmd_t cmd;

  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}


static void _set_fp_display(char *content)
{
  u32 data;
  cmd_t cmd;

  data = MT_MAKE_DWORD(MAKE_WORD(content[3], content[2]), 
    MAKE_WORD(content[1], content[0]));


  cmd.id = AP_UIO_CMD_FP_DISPLAY;
  cmd.data1 = data;
  cmd.data2 = 4;

  ap_frm_do_command(APP_UIO, &cmd);
}

void ui_set_front_panel_by_str(const char * str)
{
  char content[5];

  if (strlen(str) > 4)
  {
    memcpy(content, str, 4);
    content[4] = '\0';
  }
  else
  {
    sprintf(content, "%4s", str);
  }

  _set_fp_display(content);
}


void ui_set_front_panel_by_num(u16 num)
{
  u16 pg_id;
  dvbs_prog_node_t prog;
  char content[6];
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);

  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &prog);

  if(cfg.customer != CUSTOMER_CHANGJIANG_NY)
  {
  #ifdef LCN_SWITCH
    if(num > 1000)
    num = num % 1000;
  #endif
  }
  

 if(get_uio_led_number_type() == (u8)UIO_LED_TYPE_3D)
  {  
    if((BOOL)prog.tv_flag)
    {
      sprintf(content, "%.3d", num);
    }
    else
    {
      sprintf(content, "%.3d", num);
    }
  }
  else
  {
    if((BOOL)prog.tv_flag)
    {
	if(cfg.customer == CUSTOMER_CHANGJIANG_NY)
	{
		sprintf(content, "%.4d", num);
	}
	else
  	{
		sprintf(content, "P%.3d", num);
  	}
    }
    else
    {
      sprintf(content, "R%.3d", num);
    }
  }
  _set_fp_display(content);
}

void ui_show_logo(u8 block_id)
{
  RET_CODE ret = SUCCESS;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);

  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), block_id);
  u8 *p_logo = mtos_malloc(size);

  MT_ASSERT(p_logo != NULL);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, size, p_logo);

#ifndef WIN32  
#endif
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);  
  MT_ASSERT(SUCCESS == ret);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)p_logo, size);
  MT_ASSERT(SUCCESS == ret);
  ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  MT_ASSERT(SUCCESS == ret); 
  mtos_free(p_logo);
}
  
void ui_show_logo_by_data(u32 size, u8 *p_logo_addr)
{
  RET_CODE ret = SUCCESS;
  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);

  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
  u8 *p_logo = mtos_malloc(size);

  MT_ASSERT(p_logo != NULL);
  memcpy(p_logo, p_logo_addr, size);
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);  
  
  MT_ASSERT(SUCCESS == ret);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)p_logo, size);

  MT_ASSERT(SUCCESS == ret);
  ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);

  MT_ASSERT(SUCCESS == ret); 

  mtos_free(p_logo);
}

void ui_enable_video_display(BOOL is_enable)
{
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, is_enable);
}


static void _config_osd(rect_t *p_scr)
{
  palette_t pal;
  u8 format;
  u32 cdef, ckey;
  u16 ss_value;
  rsc_palette_t hdr_pal;
  osd_set_t osd_set;
  u8*bits = NULL;

  format = COLORFORMAT_ARGB1555;

  sys_status_get_osd_set(&osd_set);
  ss_value = osd_set.palette;
  rsc_get_palette(gui_get_rsc_handle(),ss_value, &hdr_pal, (u8**)&bits);
  pal.cnt = hdr_pal.color_num;
  pal.p_entry = (color_t *)bits;
  pal.p_entry[C_TRANS].a = 0;
  rsc_set_curn_palette(gui_get_rsc_handle(),ss_value);

  cdef = C_TRANS, ckey = C_KEY;

//  gdi_clear_screen();
  gdi_reset_screen(format, &pal, p_scr, cdef, ckey);

  OS_PRINTF("\n\n-----config OSD-----: %d, %d, %d, %d\n\n", 
    p_scr->left, p_scr->top, 
    RECTWP(p_scr), RECTHP(p_scr));
}

void ui_config_normal_osd(void)
{
  rect_t rc_scr = 
    { 0, 0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT };

  s16 x, y;
  disp_sys_t video_std = 0;
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      x = SCREEN_POS_NTSC_L;
      y = SCREEN_POS_NTSC_T;
      break;
    default:
      x = SCREEN_POS_PAL_L;
      y = SCREEN_POS_PAL_T;
  }

  offset_rect(&rc_scr, x, y);
  _config_osd(&rc_scr);
}

void ui_set_channel_mode(BOOL b)
{  
  static BOOL is_single_mode = TRUE;
  void *p_dev = NULL;
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_dev);

  if(is_single_mode == b)
  {
    return;
  }

  is_single_mode = b;

  if(b)
  {
    vdec_chchg_mode(p_dev, VDEC_CHCHG_SINGLE);
  }
  else
  {
    vdec_chchg_mode(p_dev, VDEC_CHCHG_DOUBLE);
  }
}
