/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
/**
  * \file ui_init.c
  *
  * This file implemented the entry function of shadow project which is the
  * default project of dvbs products
  */
#include "ui_common.h"
#include "unzip.h"
#include "uio.h"
#include "rsc.h"
#include "gb2312.h"
#include "lib_char.h"
#include "sys_data_staic.h"


static RET_CODE mmi_init(void)
{
  rsc_palette_t hdr_pal;
  palette_t pal;
  u8 *bits;
  u32 p_addr;
  BOOL ret = FALSE;
  handle_t rsc_handle = 0;
  paint_param_t paint_param = {0};
  
  gdi_config_t info =
  {
    /* color format */
#ifdef WIN32
     COLORFORMAT_RGB4BIT,
#else
     COLORFORMAT_RGB8BIT,
#endif
    /* screen rectangle */
    {SCREEN_POS_PAL_L, SCREEN_POS_PAL_T,
     SCREEN_POS_PAL_L + SCREEN_WIDTH,
     SCREEN_POS_PAL_T + SCREEN_HEIGHT},
    /* palette */
    &pal,
    /* default color */
    //C_TRANS,
    C_BLACK,
    /* color key */
    C_KEY,
    /* osd odd vmem */
    NULL,
    /* odd vmem size + ctrlword + palette */
    150 * KBYTES + 2 * KBYTES,
    /* osd even vmem */
    NULL,
    /* even vmem size*/
    150 * KBYTES,
    /* virtual buffer addr */
    0,
    /* virtual surface size */
    0 * KBYTES,
    /* surface cnt */
    MAX_SURFACE_CNT,
    /* cliprect cnt */
    MAX_CLIPRECT_CNT,
    /* dc cnt */
    MAX_DC_CNT,
  };

  // init osd frame buffer
  p_addr = mem_mgr_require_block(
    BLOCK_OSD0_8BIT_BUFFER,
    SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD0_8BIT_BUFFER);

  info.p_odd_addr = (u32 *)p_addr;
  info.p_even_addr = (u32 *)(p_addr + info.odd_size);
  
  mmi_init_heap(GUI_PARTITION_SIZE);

  g_rsc_config.rsc_data_addr = (u32)ota_rsc_data;

  rsc_handle = rsc_init(&g_rsc_config);
  
 if(sys_static_read_ota_language() == 0)
  {
    rsc_set_curn_language(rsc_handle,LANGUAGE_CHINESE);
  }
 else
  {
    rsc_set_curn_language(rsc_handle,LANGUAGE_ENGLISH);
  }

  ret = rsc_get_palette(rsc_handle,1, &hdr_pal, (u8**)&bits);
  pal.cnt = hdr_pal.color_num;
  pal.p_entry = (color_t *)bits;
  pal.p_entry[C_TRANS].a = 0;
  rsc_set_curn_palette(rsc_handle,1);

#ifndef WIN32
  gdi_init_v2(&info);
#else
  gdi_init(&info);
#endif

  gdi_set_global_alpha_montage(255);

  ctrl_init_lib(MAX_CLASS_CNT, MAX_OBJ_CNT);
  ctrl_str_init(MAX_CTRL_STRING_BUF_SIZE);

  cont_register_class(MAX_OBJECT_NUMBER_CNT);
  cbox_register_class(MAX_OBJECT_NUMBER_CBB);
  text_register_class(MAX_OBJECT_NUMBER_TXT);
  pbar_register_class(MAX_OBJECT_NUMBER_PGB);
  nbox_register_class(MAX_OBJECT_NUMBER_NUM);

  paint_param.max_str_len = MAX_PAINT_STRING_LENGTH;
  paint_param.max_str_lines = MAX_PAINT_STRING_LINES;
  paint_param.rsc_handle = rsc_handle;
  gui_paint_init(&paint_param);

  return SUCCESS;
}

static void ui_main_proc(void *p_param)
{
  mmi_init();
  
  ui_desktop_init(); 

  ui_ota_start();

  ui_desktop_main();
}



void ui_init(void)
{
  u32 *pstack_pnt = (u32 *)mtos_malloc(UI_FRM_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);

  /* start app task */
  mtos_task_create((u8 *)"ui_frm",
                   ui_main_proc,
                   NULL,
                   UI_FRM_TASK_PRIORITY,
                   pstack_pnt,
                   UI_FRM_TASK_STKSIZE);
}
