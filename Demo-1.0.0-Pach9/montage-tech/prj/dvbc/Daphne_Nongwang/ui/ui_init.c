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
#ifdef WIN32
  #ifdef XINSIDA_SPANISH_RSC
    #include "rsc_rgb565_xinsida_spanish.h"
  #else
   #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
    #include "rsc_rgb565_desai.h"
   #elif(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
   #include "rsc_rgb565_szxc.h"
   #elif(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
   #include "rsc_rgb565_tr.h"
   #elif(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
   #include "rsc_rgb565_qz.h"
   #else
   #include "rsc_rgb565.h"
   #endif
  #endif
#endif
#include "gb2312.h"
#include "lib_char.h"
#include "ui_char_map.h"
   
#define GB2312_MAP_LENGTH    (sizeof(g_charmap) / sizeof(struct char_map))

extern u32 get_flash_addr(void);
extern BOOL ui_is_mute(void);
extern void ui_set_mute(BOOL is_mute);
extern BOOL ui_is_pause(void);
extern void ui_set_pause(BOOL is_pause);
extern void ui_set_input_number_cont(u8 cont);
extern void ui_set_input_number_on_right(void);
extern void ui_set_num_big_font(void);

void dvb_set_convert(void)
{
  link_gb2312_maptab(g_charmap, GB2312_MAP_LENGTH);
  dvb_set_def_convertor((dvb_txt_convertor_t) gb2312_to_unicode,1);
}

static RET_CODE ui_color_map_get(u8 block_id)
{
  u8 i = 0;
  u16 color_map[MAX_FSTYLE_CNT];
  u32 read_len = 0;

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                              block_id, 0, 0,
                              MAX_FSTYLE_CNT*sizeof(u16),
                              (u8 *)color_map);
  if(read_len > 0)
  {
      for(i=0; i<3; i++)
      {
          f_tab[i].color = color_map[i];
      }
  }
  return SUCCESS;
}

static RET_CODE mmi_init(void)
{
  palette_t pal;
  u32 p_addr;
  u8 *p_res_mem_addr;
  sys_status_t *p_systatus ;
  u8 ss_value = 0;
  gdi_config_t info =
  {
    /* color format */
    COLORFORMAT_ARGB1555,
    /* screen rectangle */
    {SCREEN_POS_PAL_L, SCREEN_POS_PAL_T,
     SCREEN_POS_PAL_L + SCREEN_WIDTH,
     SCREEN_POS_PAL_T + SCREEN_HEIGHT},
    /* palette */
    &pal,
    /* default color */
    C_TRANS,
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

    #ifdef ChangJiang_WanFa
    1024 * KBYTES,		//512 * KBYTES,
    #else
    512 * KBYTES,
    #endif
    
  };
  u8 *p_zip_sys = NULL;
  u32 file_size = 0;
  u32 out_size = 0;  
  u32 rsc_bin_size = 0;  
  
  BOOL ret = TRUE;
  u32 addr = 0;
  handle_t rsc_handle = 0;
  paint_param_t paint_param = {0};
  // get sys status
  p_systatus = sys_status_get();
  ui_color_map_get(0xD0);

  // init osd frame buffer
  p_addr = mem_mgr_require_block(
    BLOCK_OSD0_16BIT_BUFFER,
    SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD0_16BIT_BUFFER);

  info.p_odd_addr = (u32 *)p_addr;
  info.p_even_addr = (u32 *)(p_addr + info.odd_size);

  mmi_init_heap(GUI_PARTITION_SIZE);

  // get rsc block addr on flash
#if 1
  //unzip resource
  p_addr = (u32)mtos_malloc(g_rsc_config.rsc_buf_size);//allocate memory for resource.
  MT_ASSERT(p_addr != 0);

  addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
    RS_BLOCK_ID);  
  file_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),
    RS_BLOCK_ID);
  
  p_zip_sys = mtos_malloc(200*KBYTES);  //allocate memory for unzip
  MT_ASSERT(p_zip_sys != NULL);

#if(SYS_MEMORY_TOTALT_SIZE == SYS_MEMORY_TOTALT_SIZE_32M)
  rsc_bin_size = 1024*KBYTES;
#else
  rsc_bin_size = 1024*KBYTES;
#endif
  p_res_mem_addr = mtos_malloc(rsc_bin_size);
  MT_ASSERT(p_res_mem_addr != NULL);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), RS_BLOCK_ID, 0, 0, rsc_bin_size, p_res_mem_addr);

#if 1//#ifdef USE_GZIP
  init_fake_mem(p_zip_sys);
  ret = gunzip((void *)p_res_mem_addr, (void *)p_addr, file_size, &out_size, memcpy);
#else
  init_fake_mem_lzma(p_zip_sys,200*1024);
  out_size = g_rsc_config.rsc_buf_size;
  ret = LZMA_Decompress((void *)p_addr, &out_size, (void *)addr, file_size);
#endif
  MT_ASSERT(ret == 0);

  OS_PRINTF("\n unzip resource. in_addr = 0x%x, out_addr = 0x%x, in_len = %d, out_len = %d\n",
          addr, p_addr, file_size, out_size);

  mtos_free(p_zip_sys);
  mtos_free(p_res_mem_addr);
#else
  p_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                             RS_BLOCK_ID);
  MT_ASSERT(p_addr != 0);
#endif

  UI_PRINTF("GUI partation is ok\n");

#ifndef WIN32
  g_rsc_config.rsc_data_addr = p_addr;
#else
  g_rsc_config.rsc_data_addr = (u32)rsc_data;
#endif
  g_rsc_config.flash_base = get_flash_addr();

  rsc_handle = rsc_init(&g_rsc_config);
  UI_PRINTF("rsc_init is ok\n");

  ss_value = p_systatus->osd_set.palette;
  UI_PRINTF("FROM SS: palette id = 0x%x\n", ss_value);
  rsc_set_curn_palette(rsc_handle,ss_value);


  ss_value = p_systatus->av_set.tv_mode;
  #if 0
  if((ss_value == AVC_VIDEO_MODE_NTSC_1)
    || (ss_value == AVC_VIDEO_MODE_PAL_M_1))
  {
 //   offset_rect(&info.osd_rect,
  //              SCREEN_POS_NTSC_L - SCREEN_POS_PAL_L,
   //             SCREEN_POS_NTSC_T - SCREEN_POS_PAL_T);
  }
  #endif

  info.vsurf_buf_addr = (u32)mtos_malloc(820 * KBYTES);
  info.vsurf_buf_size = 820 * KBYTES;

  MT_ASSERT(info.vsurf_buf_addr != 0);


  gdi_init_v2(&info);


  ss_value = p_systatus->osd_set.transparent;
   gdi_set_global_alpha_montage(10*(10 - p_systatus->osd_set.transparent) * 255 / 100);

  ctrl_init_lib(MAX_CLASS_CNT, MAX_OBJ_CNT);
  ctrl_str_init(MAX_CTRL_STRING_BUF_SIZE);

  cont_register_class(MAX_OBJECT_NUMBER_CNT);
  text_register_class(MAX_OBJECT_NUMBER_TXT);
  bmap_register_class(MAX_OBJECT_NUMBER_BMP);
  cbox_register_class(MAX_OBJECT_NUMBER_CBB);
  list_register_class(MAX_OBJECT_NUMBER_LIST);
  mbox_register_class(MAX_OBJECT_NUMBER_MTB);
  pbar_register_class(MAX_OBJECT_NUMBER_PGB);
  sbar_register_class(MAX_OBJECT_NUMBER_SCB);
  ebox_register_class(MAX_OBJECT_NUMBER_EDT);
  nbox_register_class(MAX_OBJECT_NUMBER_NUM);
  tbox_register_class(MAX_OBJECT_NUMBER_TIM);
  sbox_register_class(MAX_OBJECT_NUMBER_SBX);
  paint_param.max_str_len = MAX_PAINT_STRING_LENGTH;
  paint_param.max_str_lines = MAX_PAINT_STRING_LINES;
  paint_param.rsc_handle = rsc_handle;
  gui_paint_init(&paint_param);
  gui_roll_init(MAX_ROLL_CNT,3);
  // trigger system set
  UI_PRINTF("all classes is ok\n");

  return SUCCESS;
}
extern void ui_create_adv();

static void ui_main_proc(void *p_param)
{
  RET_CODE ret = ERR_FAILURE;
  customer_cfg_t cfg = {0};
  #ifndef OTA_CHECK_DEFAULT
  ss_public_t *ss_public;
  #endif
  //ui init ad module
  #ifndef WIN32
  ui_create_adv();
  #endif
  get_customer_config(&cfg);
  DEBUG(MAIN, INFO, "\n");
  ui_set_input_number_on_right();
  ui_set_num_big_font();

  if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
  	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
  	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
    ui_set_disable_display_time_status();
    ui_set_input_number_cont(3);
  }
  if((cfg.customer == CUSTOMER_XINSIDA_LQ) ||(cfg.customer == CUSTOMER_CHANGJIANG_LQ) ||(cfg.customer == CUSTOMER_CHANGJIANG_JS)
    || (cfg.customer == CUSTOMER_XINSIDA_SPAISH) ||(cfg.customer == CUSTOMER_CHANGJIANG_QY)
    || (cfg.customer == CUSTOMER_TEFA_DS5)  || (cfg.customer == CUSTOMER_ZHILING)
    || (cfg.customer == CUSTOMER_ZHILING_KF)  || (cfg.customer == CUSTOMER_ZHILING_LQ) )
  {
    ui_set_disable_display_time_status();
  }
  //init ui_dbase
  ui_dbase_init();

  //init book.
  ret = ui_book_init();
  MT_ASSERT(ret == SUCCESS);

  // init ui frm
  mmi_init();
  ui_desktop_init();
 #ifndef OTA_CHECK_DEFAULT
  // restore the setting
  sys_status_reload_environment_setting();
  //set sw version.
  ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID)); 
  OS_PRINTF("get ver %d\n", sys_status_get_sw_version());
  ss_public->otai.orig_software_version = (u16)sys_status_get_sw_version();
  ss_ctrl_update_public(ss_public);
  // try to play
  epg_data_init_dy_policy(class_get_handle_by_id(EPG_CLASS_ID),
                          DY_SVC_NUM);
  sys_status_check_group_view(TRUE);

  //create sleep timer.
  ui_sleep_timer_create();

  #ifndef WIN32
  	DEBUG(MAIN, "on_ca_zone_check_start before\n");
    on_ca_zone_check_start(CAS_CMD_ZONE_CHECK,0,0);
  #endif
  
  on_ca_zone_check_stop();
 #endif

#ifdef CUS_TONGGUANG_HEBI 
  if(1)
#else
  if(1)
#endif
  {
      mtos_open_printk();
  }
  else
  {
     mtos_close_printk();
  }
  // goto loop
  ui_desktop_main();
}


void ui_init(void)
{
  u32 *pstack_pnt;

  pstack_pnt = (u32 *)mtos_malloc(UI_FRM_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);

  /* start app task */
  mtos_task_create((u8 *)"ui_frm",
                   ui_main_proc,
                   NULL,
                   UI_FRM_TASK_PRIORITY,
                   pstack_pnt,
                   UI_FRM_TASK_STKSIZE);
}
