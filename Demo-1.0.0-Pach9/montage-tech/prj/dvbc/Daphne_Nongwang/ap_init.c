// lib
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
//#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_irq_jazz.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "pdec.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "spi.h"
#include "cas_ware.h"
#include "driver.h"
#include "lpower.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager32.h"
#include "data_manager_v3.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"

#include "monitor_service.h"

#include "dsmcc.h"
#include "epg_data4.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"
#include "vbi_vsb.h"
#include "surface.h"

#include "db_dvbs.h"
#include "sys_status.h"
#include "mem_cfg.h"
#include "ads_ware.h"

//eva
#include "interface.h"
#include "eva.h"

// ap

#include "ap_framework.h"

#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_epg4.h"
#include "ap_signal_monitor.h"
//#include "ap_dvbs_ota.h"
#include "ap_ota.h"
#ifdef OTA_CHECK_DEFAULT
#include "ap_ota_monitor.h"
#endif
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#include "ap_pic_play.h"
//#include "ap_browser.h"
#include "nvod_data.h"
#include "ads_funtion_public.h"

#include "customer_def.h"
#include "config_prj.h"
#include "config_cas.h"
#include "config_customer.h"
#include "sys_dbg.h"

#ifdef LCN_SWITCH
#include "ui_logic_num_proc.h"
#endif
#include "ui_ad_logo_api.h"
#ifdef WIN32
#undef TEST_FRAMEWORK
#endif
#ifdef TEST_FRAMEWORK
#include "testfm_define.h"
#include "testfm_inc.h"
/*!
  console_mode
  */
typedef enum
{
/*!
  console_mode test
  */
  TEST_MODE,
  /*!
  console_mode download
  */
  DOWNLOAD_MODE
}console_mode_t;


#endif


#ifdef LCN_SWITCH
static register_nit_fuc register_lcn = {0};
#endif
u8 *p_board_mem = NULL;
u8 *p_cfg_mem = NULL;
u8 *p_util_mem = NULL;
u8 *p_drv_mem = NULL;
u8 *p_mdl_mem = NULL;
u8 *p_ap_mem = NULL;
u8 *p_ui_mem = NULL;

u8 *p_nc_mem = NULL;
u8 *p_dvb_mem = NULL;
u8 *p_dvb2_mem = NULL;
u8 *p_db_mem = NULL;
u8 *p_epg_mem = NULL;
u8 *p_gb_mem = NULL;

#ifdef OTA_CHECK_DEFAULT
u8 *p_ap1_mem[14] = {NULL};
#else
u8 *p_ap1_mem[13] = {NULL};
#endif

//gpio config for shadow, replace the FRONTPANEL module in chip
static const pan_hw_info_t pan_info =
{
  PAN_SCAN_SLOT,  /*type_scan, */
  0,              /*type_lbd,  0: Stand-alone LBD, 1: LBD in shifter */
  1,              /*type_com, 0: Stand-alone COM, 1: COM in shifter*/
  1,              /*num_scan, Number of scan PIN, 0: no scan; <= 2 */
  4,              /*num_com,  Number of COM PIN, 0: no com; <= 8 */
  1,              /*pos_colon, Position of colon flag, 0 to 7, 8 no colon */
  0,              /*special control byte: bit 0: revert the data sequence or not
                     bit 1: HT1628 special cmd, bit 2: GPIO control lbd or not*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*flatch*/
  {GPIO_LEVEL_HIGH, GPIO_DIR_OUTPUT, 24},  /*fclock*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 25},   /*fdata*/
  {{GPIO_LEVEL_HIGH, GPIO_DIR_INPUT, 18},    /*scan[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}},  /*scan[1]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[3]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[4]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[5]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[6]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*com[7]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 32},  /*lbd[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*lbd[3]*/
  5,       /* com_scan_intv, scan com digital interval in ms, */
  300,     /* repeat_intv, Repeat interval in ms, for rpt key only */
};

static const led_bitmap_t fp_bitmap[] =
{
  {'.', 0x04},
  {'0', 0xEB},  {'1', 0x28},  {'2', 0xB3},  {'3', 0xBA},
  {'4', 0x78},  {'5', 0xDA},  {'6', 0xDB},  {'7', 0xA8},
  {'8', 0xFB},  {'9', 0xFA},  {'a', 0xF9},  {'A', 0xF9},
  {'b', 0x5B},  {'B', 0x5B},  {'c', 0xC3},  {'C', 0xC3},
  {'d', 0x3B},  {'D', 0x3B},  {'e', 0xD3},  {'E', 0xD3},
  {'f', 0xD1},  {'F', 0xD1},  {'g', 0xFA},  {'G', 0xCB},
  {'h', 0x79},  {'H', 0x79},  {'i', 0x08},  {'I', 0x41},
  {'j', 0x2A},  {'J', 0x2A},  {'l', 0x43},  {'L', 0x43},
  {'n', 0x19},  {'N', 0xE9},  {'o', 0x1B},  {'O', 0xEB},
  {'p', 0xF1},  {'P', 0xF1},  {'q', 0xF8},  {'Q', 0xF8},
  {'r', 0x11},  {'R', 0xF9},  {'s', 0xDA},  {'S', 0xDA},
  {'t', 0x53},  {'T', 0xC1},  {'u', 0x6B},  {'U', 0x6B},
  {'y', 0x7A},  {'Y', 0x7A},  {'z', 0xB3},  {'Z', 0xB3},
  {':', 0x04},  {'-', 0x10},  {'_', 0x02},  {' ', 0x00},

};
#define FP_TABLE_SIZE sizeof(fp_bitmap) / sizeof(led_bitmap_t)

//gpio map for scart
static const scart_hw_info_t gpio_scart_info =
{
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 9, SCART_VID_CVBS, SCART_VID_RGB},   /*outmode*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 5, SCART_ASPECT_4_3, SCART_ASPECT_16_9},  /*aspect*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63, SCART_TERM_STB, SCART_TERM_TV},   /*vcr input*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 32, SCART_TERM_STB, SCART_TERM_VCR},   /*tv master*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63, VCR_DETECTED, VCR_NOT_DETECTED},   /*vcr dectect*/
};

#ifdef WIN32
extern RET_CODE disp_win32_attach(char *p_name);
#endif
extern u32 get_flash_addr(void);
#ifndef WIN32
typedef struct  bin_board_cfg_st
{
  BOOL bin_flag;
  hal_board_config_t  board_cfg;
}bin_board_cfg;
static bin_board_cfg bin_board_cfg_info;
void set_board_config(void)
{
   u8 cfg_buff[2 * KBYTES] = {0};
   u32 read_len = 0;
   u32 size = 0;

   memset(&bin_board_cfg_info,0,sizeof(bin_board_cfg));
   bin_board_cfg_info.bin_flag = 0;
   size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), BOARD_CONFIG_BLOCK_ID);
   if(size >0)
    {

        if(size > 2 * KBYTES)
          {
            size = 2 * KBYTES;
          }
         read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            BOARD_CONFIG_BLOCK_ID, 0, 0,
                            size,cfg_buff);
          #if 0
            {
              u32 i =0;
              OS_PRINTK("debug board config bin date\n");
              for(i=0; i<read_len; i++)
              {
                if(i%16==0)
                {
                    OS_PRINTK("\n%06xh:",i);
                }
                OS_PRINTK("%02x ", cfg_buff[i]);
              }
             OS_PRINTK("\ndebug board config bin date end \n");
            }
         #endif
         if(read_len >0)
          {
            bin_board_cfg_info.bin_flag = 1;
            memcpy(&bin_board_cfg_info.board_cfg,cfg_buff,sizeof(hal_board_config_t));
            #if 0
            OS_PRINTK("board confg av_perf.changed_param_cnt :%d\n",bin_board_cfg_info.board_cfg.av_perf.changed_param_cnt);
            OS_PRINTK("board confg av_perf.pal_changed_param_cn :%d\n",bin_board_cfg_info.board_cfg.av_perf.pal_changed_param_cnt);
            OS_PRINTK("board confg av_perf.ntsc_changed_param_cnt :%d\n",bin_board_cfg_info.board_cfg.av_perf.ntsc_changed_param_cnt);

            OS_PRINTK("board confg smc_dete :%d\n",bin_board_cfg_info.board_cfg.misc_config.smc_detect_validity);
            OS_PRINTK("board confg smc_power :%d\n",bin_board_cfg_info.board_cfg.misc_config.smc_power_validity);
            OS_PRINTK("board confg tn_loopout :%d\n",bin_board_cfg_info.board_cfg.misc_config.is_tn_loopthrough);
            #endif
            #ifdef USE_BOARD_CONFIG  //兼容没有boardconfig 项目
            hal_board_config(cfg_buff);
            #else
            bin_board_cfg_info.bin_flag = 0;
            #endif
          }
    }
}
#endif
#ifdef WIN32
static void init_fp_cfg(fp_cfg_t *p_cfg)
{
  // use default in drv
  p_cfg->map_size = FP_TABLE_SIZE;
  p_cfg->p_map = fp_bitmap;
  p_cfg->p_info = (pan_hw_info_t *)&pan_info;
}
#endif
static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = mem_mgr_require_block(BLOCK_SUB_16BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  memset((void*)p_addr, 0, mem_mgr_get_block_size(BLOCK_SUB_16BIT_BUFFER));

  mem_mgr_release_block(BLOCK_SUB_16BIT_BUFFER);

  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  #ifndef WIN32
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_16BIT_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;//p_cfg->p_sub_cfg->odd_mem_end;
  p_cfg->p_sub_cfg->even_mem_end = 0;//p_cfg->p_sub_cfg->even_mem_start + SUB_EVEN_MEMSIZE;
  #else
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_ODD_MEMSIZE;
  p_cfg->p_sub_cfg->even_mem_start = p_cfg->p_sub_cfg->odd_mem_end;
  p_cfg->p_sub_cfg->even_mem_end = p_cfg->p_sub_cfg->even_mem_start + SUB_EVEN_MEMSIZE;
  #endif

  // init osd0 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD0_16BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD0_16BIT_BUFFER);

  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  #ifndef WIN32
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr +  OSD0_16BIT_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;
  #else
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr +  OSD0_ODD_MEMSIZE;
  p_cfg->p_osd0_cfg->even_mem_start = p_cfg->p_osd0_cfg->odd_mem_end;
  p_cfg->p_osd0_cfg->even_mem_end = p_cfg->p_osd0_cfg->even_mem_start + OSD0_EVEN_MEMSIZE;
  #endif


  // init osdd frame buffer
//  p_addr = mem_mgr_require_block(BLOCK_OSD1_16BIT_BUFFER, SYS_MODULE_GDI);
//  MT_ASSERT(p_addr != 0);

//  mem_mgr_release_block(BLOCK_OSD1_16BIT_BUFFER);

 // p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
 // p_cfg->p_osd1_cfg->odd_mem_end = p_addr +  OSD1_ODD_MEMSIZE;
 // p_cfg->p_osd1_cfg->even_mem_start = p_cfg->p_osd1_cfg->odd_mem_end;
 // p_cfg->p_osd1_cfg->even_mem_end = p_cfg->p_osd1_cfg->even_mem_start + OSD1_EVEN_MEMSIZE;
  return TRUE;
}

#ifdef WIN32
static BOOL init_nim_cfg(nim_config_t *p_cfg)
{
  //Ts2022 config
  p_cfg->tun_crystal_khz = 27000;
  p_cfg->tun_rf_bypass_en = 0;
  p_cfg->tun_clk_out_by_tn = 1;
  p_cfg->tun_clk_out_div = 1;
  p_cfg->bs_times = 1;

  p_cfg->pin_config.lnb_enable = 1;
  p_cfg->pin_config.vsel_when_lnb_off = 0;
  p_cfg->pin_config.diseqc_out_when_lnb_off = 0;


  p_cfg->pin_config.vsel_when_13v = 0;
  p_cfg->pin_config.lnb_enable_by_mcu = 0;
  p_cfg->pin_config.lnb_prot_by_mcu = 0;

  return TRUE;
  /*
     RET_CODE ret;
     misc_options_t misc;
     handle_t dm_handle = NULL;

     dm_handle = class_get_handle_by_id(DM_CLASS_ID);
     MT_ASSERT(dm_handle != NULL);

     ret = dm_read_bl_block(TOOL_BL_CONFIG_ID,
                         MISC_PARA_OFFSET,
                         sizeof(misc_options_t),
                         (u8 *)&misc);
     if(ret != ERR_FAILURE)
     {
     p_cfg->sig_stren_ratio = misc.sig_str_ratio;
     p_cfg->sig_stren_max = misc.sig_str_max;
     p_cfg->sig_quality_max = misc.sig_qua_max;
     p_cfg->stren_l_quality = misc.str_l_qua;
     p_cfg->stren_0_gain = misc.str_0_gain;
     p_cfg->bs_times = misc.bs_times;

     return TRUE;
     }
     return FALSE;
    */
}

#endif
static void util_init(void)
{
#ifdef CORE_DUMP_DEBUG
  static debug_mem_t d_mem = {0};

  d_mem.start_addr = (u8 *)mtos_malloc(10 * KBYTES);
  MT_ASSERT(d_mem.start_addr != NULL);

  d_mem.common_print_size = 2 * KBYTES;
  d_mem.msgq_print_size = 2 * KBYTES;
  d_mem.os_info_size = 4 * KBYTES;
  d_mem.stack_info_size = 1 * KBYTES;
  d_mem.mem_map_size = 1 * KBYTES;

  os_debug_init(&d_mem);
#endif
  simple_queue_init();
}

static void board_config(void)
{
#ifndef WIN32
#ifdef CHIP
  u32 value;

  /* set dmx ts input mode as PARALEL */
  value = hal_get_u32((u32 *)0x61000740);
  value &= ~(1 << 0);
  hal_put_u32((u32 *)0x61000740, value);

  /* set ts input from soc-internal demod */
  value = hal_get_u32((u32 *)0x6f8e0000);
  value |= 1 << 0;
  hal_put_u32((u32 *)0x6f8e0000, value);

#else
  /* for FPGA, need set for vdec init, for chip not to set */
  hal_put_u32((u32 *)0x12000000, 0x6462b22);
#endif
#endif
}
/*******************************
 * for checking if has manual ota key pressed when boot up
 ********************************/
extern void ui_set_channel_mode(BOOL b);
extern void ui_show_logo(u8 block_id);
extern void drv_globalinit(void);
extern void glb_info_init(void);
extern RET_CODE spi_attach_jazz(char *p_name);
extern RET_CODE uio_jazz_attach(char *p_name);
extern void tuner_attach_tc2800();
extern void sys_status_set_video_out(void);
extern RET_CODE smc_attach_jazz(char *name);
extern RET_CODE lpower_attach_jazz(char *p_name);

#ifdef TEST_FRAMEWORK
extern void testfm_console_run(void);
extern void shell_init_cmd(void);
extern void testfm_set_platform(u32 platform, const char *platform_name);
extern void dmx_init_cmd(void);
extern void set_console_mode(console_mode_t mode);
extern testfm_error_code_t testfm_init(void);
#endif

extern void channel_init(u32 freq, u32 sym,u16  modulation, u16 pid, u16 table_id);
extern void dvb_set_convert(void);
extern void request_channel_sec(dvb_section_t *p_sec, u32 para1, u32 para2);
extern void parse_channel_sec(handle_t handle, dvb_section_t *p_sec);

 u32 ap_show_black_screen(u8 Enable)
{
  u32 dtmp, ptmp;
  ptmp = R_DISC_VID_VID_CHCHG_DISPLAY_REG;
  if (Enable)
    {
      dtmp = hal_get_u32((volatile unsigned long *) 0x6f810004);
      dtmp |= 0x22;
    }
  else
  {
      dtmp = hal_get_u32((volatile unsigned long *) 0x6f810004);
	  dtmp = dtmp & 0xffffffdd;
	  dtmp = dtmp | 0x20;
  }
 hal_put_u32((volatile unsigned long *) 0x6f810004, dtmp);
  return 0;
}

#ifndef WIN32
#if 0
extern u32 inl(u32);
extern void outl(u32, u32);
u32 get_chipVersion(void)
{
  u32 data = 0;
  u32 version = 0;
  outl(((0x0f << 16) | inl(0x6f200008)), 0x6f200008);//enable
  data = (inl(0x6f200014) & 0xffff); //[15:0]
  outl((0x0d << 16) | (inl(0x6f200008) & 0xfff0ffff), 0x6f200008);//disbale

  if(0x2800 == data) /**A1**/
  {
    version = 0X01;
  }
  else if(0x2810 == data)    /**A2**/
  {
    version = 0X02;
  }
  else
  {
     version = 0X02;
  }
  return version;
}


static void venc_init(void)
{
  int dtmp;
  dtmp = inl(0x50000000);
  dtmp |= 0x110000;
  outl(dtmp, 0x50000000);
  if(0x02 == get_chipVersion())
    {
      OS_PRINTF("jazz ic is A2\n");
      //outl(0xc222626, 0x6f840008);
      outl(0xc362626, 0x6f840008);
      outl(0x1650118, 0x6f840020);
      outl(0x78011b, 0x6f840024);
    }
  else
    {
      OS_PRINTF("jazz ic is A1\n");
      //outl(0xc222619, 0x6f840008);
      outl(0xc362619, 0x6f840008);
      outl(0x1800112, 0x6f840020);
      outl(0x840127, 0x6f840024);
    }
  //outl(0x114aa55, 0x6f840040);
  outl(0x55555555, 0x6f8400b4);
  outl(0x55555555, 0x6f8400c4);
  outl(0x55555555, 0x6f8400c8);
  outl(0x1650120, 0x6f840028);
  outl(0x73011f, 0x6f84002c);
  outl(0x1601, 0x6f840074);
  outl(0x11a011a, 0x6f840068);
}
#else
extern void venc_init(void);
#endif
#endif

#ifndef WIN32
static void audout_spdif_onoff(BOOL is_on)
{
   u32 dtmp = 0;
   customer_cfg_t cus_cfg = {0};
   if ((CUSTOMER_HAIHUIDA== cus_cfg.customer) || (CUSTOMER_PANLONGHU== cus_cfg.customer))
  {
    if(is_on)
    {
      dtmp = inl(0x6f880000);
      dtmp &= (~0x8000);
      outl(dtmp, 0x6f880000);

      dtmp = inl(0x6f820040);
      dtmp &= (~0x40000000);
      outl(dtmp, 0x6f820040);

    }
    else
    {
      dtmp = inl(0x6f880000);
      dtmp |= 0x8000;
      outl(dtmp, 0x6f880000);
    }

  }
}
#endif
/*
static void audout_npcm_mute_onoff(BOOL is_on)
{
   u32 dtmp = 0;

   if(is_on)
   {
           dtmp = inl(0x6f820040);
        dtmp |= 0x40000000;
           outl(dtmp, 0x6f820040);

     }
else
   {
           dtmp = inl(0x6f820040);
        dtmp &= (~0xc0000000);
           outl(dtmp, 0x6f820040);
     }

 }
*/

static void drv_init(void)
{
  RET_CODE ret;
  void *p_dev = NULL;
  dm_v3_init_para_t dm_para = { 0 };
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  // init gpio & dma
  OS_PRINTF("drv --1\n");
  gpio_init();

  OS_PRINTF("drv --2\n");
  hal_dma_init();
  OS_PRINTF("drv --3\n");


  //ddr set
#ifndef CHIP
#ifndef WIN32
  outl(0x06462b22, 0x12000000);/**FPGA need ;but IC don't need*****/
#endif
#endif
#ifndef WIN32
  {
      spi_cfg_t spi_cfg = {0};
      void *p_spi_0 = NULL;
      charsto_cfg_t charsto_cfg = {0};
      /* init spi bus driver */
      ret = spi_attach_jazz("spi_jazz_0");
      MT_ASSERT(SUCCESS == ret);
      p_spi_0 = dev_find_identifier(NULL,
                                                      DEV_IDT_NAME,
                                                      (u32)"spi_jazz_0");
#ifdef CHIP
       {
          u32 dtmp = 0;
          dtmp = hal_get_u32((u32*)0x70000000);
          //dtmp &= ~(3 << 8);
          dtmp &= 0xfffffcff;
          hal_put_u32((u32*)0x70000000, dtmp);
       }
      //spi_cfg.bus_clk_mhz = 80;
      spi_cfg.bus_clk_mhz = 50;
#else
      spi_cfg.bus_clk_mhz = 50;
#endif
      /* config spi bus for flash */
      spi_cfg.bus_clk_delay = 1;
      spi_cfg.io_num = 1;
      spi_cfg.lock_mode = OS_MUTEX_LOCK;
      spi_cfg.op_mode = 0;
      spi_cfg.pins_cfg[0].miso1_src = 0;
      spi_cfg.pins_cfg[0].miso0_src = 1;
      spi_cfg.pins_cfg[0].spiio1_src = 0;
      spi_cfg.pins_cfg[0].spiio0_src = 0;
      spi_cfg.pins_dir[0].spiio1_dir = 0;
      spi_cfg.pins_dir[0].spiio0_dir = 1;
      spi_cfg.spi_id = 0;
      ret = dev_open(p_spi_0, &spi_cfg);
      MT_ASSERT(SUCCESS == ret);


      OS_PRINTF("drv --spi end\n");
    /* char storage */

      ret = ATTACH_DRIVER(CHARSTO, jazz, default, default);
      MT_ASSERT(ret == SUCCESS);

      p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
      MT_ASSERT(NULL != p_dev);

      charsto_cfg.rd_mode = SPI_FR_MODE;
#ifndef WIN32
      charsto_cfg.p_bus_handle = p_spi_0;
#endif
      charsto_cfg.size = CHARSTO_SIZE;
      ret = dev_open(p_dev, &charsto_cfg);
      MT_ASSERT(ret == SUCCESS);
    }
#else
    {
        /* char storage */
        charsto_cfg_t charsto_cfg = {0};


        ret = ATTACH_DRIVER(CHARSTO, magic, default, default);
        MT_ASSERT(ret == SUCCESS);

        p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
        MT_ASSERT(NULL != p_dev);

        charsto_cfg.size = CHARSTO_SIZE;
        charsto_cfg.spi_clk = FLASH_C_CLK_P2;
        charsto_cfg.rd_mode = SPI_FR_MODE;
        ret = dev_open(p_dev, &charsto_cfg);
        MT_ASSERT(ret == SUCCESS);
      }
#endif
  OS_PRINTF("drv --4\n");

  {
  /* init data manager */
  dm_para.ver = DM_VER_3;
  dm_para.flash_base_addr = get_flash_addr();
  OS_PRINTF("flash base addr --0x%x\n",dm_para.flash_base_addr);
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.use_mutex = TRUE;
  dm_para.mutex_prio = OS_MUTEX_PRIORITY;
  dm_para.piece_size = 0;
  dm_para.ghost_addr = 0;
  dm_para.using_max_node_len = DM_MAX_DATA_LEN;
  OS_PRINTF("init v3 --4\n");
  dm_init_v3(&dm_para);
  OS_PRINTF("init v3 end --4\n");

  OS_PRINTF("set header \n");
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), FLASH_BOOT_ADRR_DM);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), FLASH_LOGIC_ADRR_DM);
  }
  OS_PRINTF("drv --=5\n");
  #ifndef WIN32
  set_board_config();
  #endif

#ifndef WIN32
  {
    /* i2c master dev */
    i2c_cfg_t i2c_cfg = {0};


    ret = ATTACH_DRIVER(I2C, jazz, default, default);
    MT_ASSERT(SUCCESS == ret);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
    MT_ASSERT(NULL != p_dev);
#ifdef CHIP
  i2c_cfg.i2c_id = 1;
#endif
    i2c_cfg.bus_clk_khz = 300;
    i2c_cfg.lock_mode = OS_TASK_LOCK;
    ret = dev_open(p_dev, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
  }
OS_PRINTF("drv --6\n");
#endif

#ifdef WIN32
  {
    /* uio including irda and frontpanel dev */
    uio_cfg_t uiocfg = {0};
  //  fp_cfg_t fpcfg = {0};
    irda_cfg_t irdacfg = {0};
    u8 content[5] = {' ', 'O', 'N', ' ', 0};

    ret = ATTACH_DRIVER(UIO, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev);


    irdacfg.protocol = IRDA_NEC;
 //   init_fp_cfg(&fpcfg);
    uiocfg.p_ircfg = &irdacfg;
    uiocfg.p_fpcfg = NULL;//&fpcfg;
    ret = dev_open(p_dev, &uiocfg);
    MT_ASSERT(SUCCESS == ret);
    uio_display(p_dev, content, 4);
  }
#else
  {
    uio_cfg_t uiocfg = {0};
    irda_cfg_t irdacfg = {0};
    u8 content[5] = {' ', 'O', 'N', ' ', 0};

  #if (FRONT_BOARD == FRONT_BOARD_DEMO)
   ret = ATTACH_DRIVER(UIO, jazz, default, mcu);
  #elif (FRONT_BOARD == FRONT_BOARD_BOYUAN_ON_BROAD)
   ret = uio_jazz_attach(NULL);
  #elif (FRONT_BOARD == FRONT_BOARD_PANLONGHU)
   ret = uio_jazz_attach(NULL);
  #elif (FRONT_BOARD == FRONT_BOARD_YINGJI)
   ret = uio_jazz_attach(NULL);
  #elif (FRONT_BOARD == FRONT_BOARD_XINSHIDA)
   ret = ATTACH_DRIVER(UIO, jazz, default, mcu);
  #elif (FRONT_BOARD == FRONT_BOARD_CHANGJIANG)
   ret = ATTACH_DRIVER(UIO, jazz, default, mcu);
  #elif (FRONT_BOARD == FRONT_BOARD_TONGGUANG)
   ret = ATTACH_DRIVER(UIO, jazz, default, mcu);
  #elif (FRONT_BOARD == FRONT_BOARD_JIUZHOU)
   ret = uio_jazz_attach(NULL);
  #else
   ret = uio_attach("mcu");
  #endif
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev);


    irdacfg.protocol = IRDA_NEC;
    irdacfg.irda_repeat_time = 300;
    uiocfg.p_ircfg = &irdacfg;
    uiocfg.p_fpcfg = NULL;
    uio_init_config(&uiocfg);
    ret = dev_open(p_dev, &uiocfg);
    MT_ASSERT(SUCCESS == ret);

    if(get_uio_led_number_type() == (u8)UIO_LED_TYPE_3D)
    {
       if(CUSTOMER_ZHONGDA == cfg.customer)
       uio_display(p_dev, content, 4);
       else
       uio_display(p_dev, content, 3);
    }
    else
      {
       uio_display(p_dev, content, 4);
      }

    power_on_led_display();
    set_volume_mute_by_hardware(0);

  }
#endif

  OS_PRINTF("drv --7\n");
  {
    /* nim */
    nim_config_t nim_cfg = {0};
    ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
    MT_ASSERT(ret == SUCCESS);
#ifndef WIN32
#ifdef CHIP
  tuner_attach_tc2800();
 // tuner_attach_XG_XDCT6A();
#else
    extern void tuner_attach_TDCC_G051F();
    tuner_attach_TDCC_G051F();
#endif
#endif
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);
    /* get customer config data from flash */
    //if(init_nim_cfg(&nim_cfg))
  nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
#ifdef CHIP
 nim_init_config(&nim_cfg);
  if(bin_board_cfg_info.bin_flag)
  {
      nim_cfg.tuner_loopthrough =
        bin_board_cfg_info.board_cfg.misc_config.is_tn_loopthrough;
  }
#else
  nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
  nim_cfg.dem_ver = DEM_VER_0;
#endif
    if(1)
    {
      ret = dev_open(p_dev, &nim_cfg);
    }
    else /* use default in drv */
    {
      ret = dev_open(p_dev, NULL);
    }

    MT_ASSERT(SUCCESS == ret);
  }
  OS_PRINTF("drv --8\n");

/* Install global info ISR */
#ifndef WIN32
  drv_globalinit();
  glb_info_init();
#endif
  OS_PRINTF("drv --9\n");
  {
    /* display */
    disp_cfg_t disp_cfg = {0};
    layer_cfg_t osd0_cfg = {0};
    layer_cfg_t sub_cfg = {0};
    layer_cfg_t osd1_cfg = {0};
    still_cfg_t still_cfg = {0};
    layer_cfg_t dummy_cfg = {0};

    //display dev....
    disp_cfg.p_sub_cfg = &sub_cfg;
    disp_cfg.p_osd0_cfg = &osd0_cfg;
    disp_cfg.p_osd1_cfg = &osd1_cfg;
    disp_cfg.p_still_sd_cfg = &still_cfg;
    disp_cfg.p_dummy_osd_cfg = &dummy_cfg;

    ret = ATTACH_DRIVER(DISP, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_dev);

    if(init_display_cfg(&disp_cfg))
    {
      ret = dev_open(p_dev, &disp_cfg);
    }
    else
    {
      ret = dev_open(p_dev, NULL);
    }
    MT_ASSERT(SUCCESS == ret);

      if((CUSTOMER_JINGGONG == cfg.customer) ||
             (CUSTOMER_BOYUAN == cfg.customer) ||
             (CUSTOMER_ZHILING_KF== cfg.customer) ||
             (CUSTOMER_ZHILING_LQ== cfg.customer) ||
             (CUSTOMER_ZHILING == cfg.customer) ||
             (CUSTOMER_SHENZHOU_QN== cfg.customer) )
        {
           /*******only cvbs out**********/
          disp_component_set_type (p_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
          disp_component_onoff (p_dev, COMPONENT_GRP0, FALSE);
          disp_cvbs_onoff (p_dev, CVBS_GRP0,TRUE);
        }
      else if((CUSTOMER_YINGJI == cfg.customer) ||(CUSTOMER_ZHONGDA == cfg.customer)
	  	||(CUSTOMER_TDX_PAKISTAN == cfg.customer)||(CUSTOMER_FANTONG_KFAJX == cfg.customer)
	  	||(CUSTOMER_FANTONG_BYAJX == cfg.customer)||(CUSTOMER_FANTONG_XSMAJX == cfg.customer)||
	  	(CUSTOMER_AOJIEXUN == cfg.customer))
        {
          //doing nothing, TBD...
        }
     else
      {
        /***make sure YPBPR color out*****/
        disp_cvbs_onoff (p_dev, CVBS_GRP0,TRUE);
        disp_component_set_type (p_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
        disp_component_onoff (p_dev, COMPONENT_GRP0, TRUE);
      }

    //gpe_vsb dev....
    #ifndef WIN32
    ret = ATTACH_DRIVER(GPE_VSB, jazz, default, default);
    #else
    ret = ATTACH_DRIVER(GPE_VSB, magic, default, default);
    #endif
    MT_ASSERT(ret == SUCCESS);
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_dev);
    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
  }
  OS_PRINTF("drv --10\n");

  {
    /* dmx */
    ret = ATTACH_DRIVER(DMX, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev);

    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
  }
  OS_PRINTF("drv --11\n");

  {
    /* video */
    ret = ATTACH_DRIVER(VDEC, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    MT_ASSERT(NULL != p_dev);

    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
   set_normal_buf();  /**move to midware_init by chenguangfu**/
   ui_set_channel_mode(FALSE);
  }

  OS_PRINTF("drv --12\n");

{
    /* audio */
    ret = ATTACH_DRIVER(AUDIO_VSB, jazz, default, default);

    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    MT_ASSERT(NULL != p_dev);
    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
    #ifndef WIN32
    if(cfg.customer == CUSTOMER_DEMO
      || cfg.customer == CUSTOMER_BOYUAN
      || cfg.customer == CUSTOMER_WANGFA
      || cfg.customer == CUSTOMER_JINGHUICHENG_QZ
      || cfg.customer == CUSTOMER_JINGHUICHENG_WF
      || cfg.customer == CUSTOMER_PANLONGHU
      || cfg.customer == CUSTOMER_YINHE
      || cfg.customer == CUSTOMER_TAIHUI
      || cfg.customer == CUSTOMER_YINGJI
      || cfg.customer == CUSTOMER_XINSHIDA
      || cfg.customer == CUSTOMER_JINGGONG
      || cfg.customer == CUSTOMER_ZHONGDA
      || cfg.customer == CUSTOMER_HAIHUIDA
      || cfg.customer == CUSTOMER_TONGGUANG
      || cfg.customer == CUSTOMER_CHANGHONG
      || cfg.customer == CUSTOMER_JINGHUICHENG
      || cfg.customer == CUSTOMER_JINYA
      || cfg.customer == CUSTOMER_SHIDA
      || cfg.customer == CUSTOMER_XIONGMAO
      || cfg.customer == CUSTOMER_TONGGUANG_CG
      || cfg.customer == CUSTOMER_TONGGUANG_QY
      || cfg.customer == CUSTOMER_PANLONGHU_TR
      || cfg.customer == CUSTOMER_CHANGJIANG
      || cfg.customer == CUSTOMER_CHANGJIANG_LQ
      || cfg.customer == CUSTOMER_CHANGJIANG_NY
      || cfg.customer == CUSTOMER_CHANGJIANG_QY
      || cfg.customer == CUSTOMER_CHANGJIANG_JS
      || cfg.customer == CUSTOMER_FANTONG
      || cfg.customer == CUSTOMER_FANTONG_KF
      || cfg.customer == CUSTOMER_FANTONG_KFAJX
      || cfg.customer == CUSTOMER_FANTONG_KF_SZXC312
      || cfg.customer == CUSTOMER_FANTONG_BYAJX
      || cfg.customer == CUSTOMER_FANTONG_XSMAJX
      || cfg.customer == CUSTOMER_DESAI
      || cfg.customer == CUSTOMER_XINSIDA_LQ
      || cfg.customer == CUSTOMER_ZHILING
      || cfg.customer == CUSTOMER_ZHILING_KF
      || cfg.customer == CUSTOMER_ZHILING_LQ
      || cfg.customer == CUSTOMER_XINSIDA_SPAISH
      || cfg.customer == CUSTOMER_SHENZHOU_QN
      || cfg.customer == CUSTOMER_HEBI)
    {
      //add set max volume here for customer
    }
    else
    {
      ret = aud_set_max_volume(p_dev, 0x8000);
      MT_ASSERT(SUCCESS == ret);
    }

    if ((CUSTOMER_PANLONGHU == cfg.customer) || (CUSTOMER_HAIHUIDA == cfg.customer))
    {
      audout_spdif_onoff(TRUE);	//spdif PCM
	//    aud_start_vsb(p_audio_dev, AUDIO_SPDIF_AC3, AUDIO_NO_FILE);	/////spdif npcm
    }

    if(CUSTOMER_JIUZHOU == cfg.customer)
    {
        hal_pinmux_gpio_enable(0, 1);
        gpio_set_dir(0, GPIO_DIR_OUTPUT);
        gpio_set_value(0, 1);
    }
    #endif
  }

#ifndef WIN32
  //open volume
  hal_pinmux_gpio_enable(7, 1);
  gpio_set_dir(7, GPIO_DIR_OUTPUT);
  gpio_set_value(7, 1);
#endif

  OS_PRINTF("drv --13\n");
    {
    //pdec....
    ret = ATTACH_DRIVER(PDEC,jazz,default,default);
    //ret = PDEC_SIM(PDEC,jazz,default,default);
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
    MT_ASSERT(NULL != p_dev);

    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
  }
  OS_PRINTF("drv --14\n");
#ifndef WIN32
  {
    /* avsync */
    ret = ATTACH_DRIVER(AVSYNC_VSB, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AVSYNC);
    MT_ASSERT(NULL != p_dev);

    ret = dev_open(p_dev, NULL);
    MT_ASSERT(SUCCESS == ret);
  }

  OS_PRINTF("drv --avsync end\n");

  {
		scard_open_t smc_op_p;

		ATTACH_DRIVER(SMC, jazz, default, default);
		MT_ASSERT(SUCCESS == ret);
		p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
		MT_ASSERT(NULL != p_dev);
		smc_op_p.smc_op_pri = DRV_SMC_TASK_PRIORITY;
		smc_op_p.smc_op_stksize = DRV_SMC_TASK_STKSIZE;
       smc_op_p.detect_pin_pol = 1;
       smc_op_p.convention = 1;
       smc_init_config(&smc_op_p);
        if(bin_board_cfg_info.bin_flag)
        {
           smc_op_p.detect_pin_pol =
            bin_board_cfg_info.board_cfg.misc_config.smc_detect_validity;

           if(bin_board_cfg_info.board_cfg.misc_config.smc_power_validity)
            {
                smc_op_p.vcc_enable_pol = 0;  /**reverse from config***/
            }
           else
            {
                smc_op_p.vcc_enable_pol = 1;
            }
        }
		ret = dev_open(p_dev, (void*)&smc_op_p);
		MT_ASSERT(SUCCESS == ret);
  }

  OS_PRINTF("drv --15\n");
  {

    //Low power module init
    ret = ATTACH_DRIVER(LPOWER, jazz, default, default);
    MT_ASSERT(ret == SUCCESS);

  }
  OS_PRINTF("drv --16\n");

  {
    /*board config */
    set_board_config();
    /* venc */
    venc_init();
    if((CUSTOMER_TAIHUI== cfg.customer) ||
             (CUSTOMER_JINGHUICHENG_QZ== cfg.customer))
      {
        /********add if use jinhuicheng board**********/
        outl(0x55104080, 0x6f820068); /**add for jinghuicheng board***/
      }
    else if((CUSTOMER_ZHILING== cfg.customer) || (CUSTOMER_ZHILING_KF== cfg.customer) || (CUSTOMER_ZHILING_LQ== cfg.customer))
    {
      hal_put_u32((u32*)0x6f82006c, 0x0);
    }

    outl(0x00908d8d,0x6f84006c); /**add for lihui test board***/
  }
  OS_PRINTF("drv --venc end\n");

#ifdef TEST_FRAMEWORK
{
  u32* pstack_csl = NULL;
  OS_PRINTF("\nInitialize tests registry\n");
  MT_ASSERT(testfm_init() == TESTFM_SUCCESS);

  testfm_set_platform(PLATFORM_JAZZ, "jazz");
  shell_init_cmd();
  dmx_init_cmd();
  set_console_mode(0);
 //create the first task to conitinue

  pstack_csl = (u32*)mtos_malloc(SYS_CONSOLE_TASK_STKSIZE);
  MT_ASSERT(pstack_csl != NULL);
  mtos_task_create((u8 *)"console",
                   (void *)testfm_console_run,
                 NULL,
                 SYS_CONSOLE_PRIORITY,
                 (u32*)pstack_csl,
                 SYS_CONSOLE_TASK_STKSIZE);
}
#endif
#else
ui_show_logo(LOGO_BLOCK_ID_M0);
#endif

}

void register_dvb_tables(void)
{
  dvb_register_t reg_param = {0};
  dvb_t *p_dvb = NULL;
  customer_cfg_t cfg = {0};

  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  get_customer_config(&cfg);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_cas_cat;
  reg_param.request_proc = request_cat;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_CAT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_emm;
  reg_param.request_proc = request_emm;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_EMM;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ecm;
  reg_param.request_proc = request_ecm;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_ECM;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pat;
  reg_param.request_proc = request_pat;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PAT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_sdt_dvbc;
  reg_param.request_proc = request_sdt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_SDT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);


  reg_param.free_proc = NULL;
  #ifdef LCN_SWITCH_NIT
  attach_nit_logic_num(&register_lcn);
  reg_param.parse_proc = register_lcn.parse;
  reg_param.request_proc = register_lcn.request;
  #else
  reg_param.parse_proc = parse_nit;
  reg_param.request_proc = request_nit;
  #endif
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pmt_dvbc;
  reg_param.request_proc = request_pmt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PMT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ts_packet;
  reg_param.request_proc = request_ts_packet;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = MPEG_TS_PACKET;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT multi-sec mode request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT multi-sec mode request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT multi-sec mode request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL_51;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT single section  periodic request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_single_mode;
  reg_param.req_mode = DATA_PERIODIC;
  reg_param.table_id = DVB_TABLE_ID_EIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT single section  periodic request

  reg_param.free_proc = NULL;
  #ifdef LCN_SWITCH_BAT
  attach_bat_logic_num(&register_lcn);
  reg_param.parse_proc = register_lcn.parse;
  reg_param.request_proc = register_lcn.request;
  #else
  reg_param.parse_proc = parse_bat;
  reg_param.request_proc = request_bat_multi_mode;
  #endif
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_BAT;
  p_dvb->register_table(p_dvb, &reg_param);

  //dsmcc msg section:single
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_dsmcc_dsi_dii_section;
  reg_param.request_proc = request_dsmcc_dsi_dii_section;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_dvb->register_table(p_dvb, &reg_param);

  {
    //channel table section:single
    reg_param.free_proc = NULL;
    reg_param.parse_proc = parse_channel_sec;
    reg_param.request_proc = request_channel_sec;
    reg_param.req_mode = DATA_SINGLE;
    reg_param.table_id = DVB_TABLE_ID_RUN_STATUS;
    p_dvb->register_table(p_dvb, &reg_param);
  }
}


void middleware_init(void)
{
  handle_t dm_handle = NULL;
  dvb_t *p_dvb = NULL;
  u32 size = 0;
  u8 *p_buf = NULL;
  avc_cfg_t p_avc_conf;
  utc_time_t time;
  eva_init_para_t eva_para;
  m_svc_init_para_t m_init_param = {0};
  u8 version_buf[64] = {0};

  mdl_init();
  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(dm_handle != NULL);

  eva_para.debug_level = EVA_DEBUG_L;
  eva_para.eva_sys_task_prio = EVA_SYS_TASK_PRIORITY;
  eva_init(&eva_para);

  // Whether cache is enabled, it is up to the configuration from UI
  OS_PRINTF("dm set cache\n");
  dm_set_cache(dm_handle, IW_TABLE_BLOCK_ID, MAX_TABLE_CACHE_SIZE);
  dm_set_cache(dm_handle, IW_VIEW_BLOCK_ID, MAX_VIEW_CACHE_SIZE);
  OS_PRINTF("dm set cache is done\n");

  p_nc_mem = mtos_malloc(10);
  MT_ASSERT(p_nc_mem != NULL);
  memset(p_nc_mem, 0x1b, 10);
  {
    nc_svc_cfg_t nc_cfg = {0};
    //nc_cfg.b_sync_lock = TRUE;
    nc_cfg.b_sync_lock = FALSE;
    nc_cfg.lock_mode = SYS_DVBC;
    nc_cfg.priority = MDL_NIM_CTRL_TASK_PRIORITY;
    nc_cfg.stack_size = MDL_NIM_CTRL_TASK_STKSIZE;
    nc_cfg.auto_iq = FALSE;  // JAZZ driver auto change iq, middleware no need do it again
    nc_cfg.wait_ticks = 100;  //500ms
    nc_svc_init(&nc_cfg);
  }

  p_dvb_mem = mtos_malloc(10);
  MT_ASSERT(p_dvb_mem != NULL);
  memset(p_dvb_mem, 0x2b, 10);
  dvb_init_1(MDL_DVB_TASK_PRIORITY, MDL_DVB_TASK_STKSIZE);

  //nvod_mosaic_buf_init(30 * KBYTES);
   #ifdef SYS_FUNCTION_MOSAIC_GUIDE
   nvod_mosaic_buf_init(120 * KBYTES);
  #else
   nvod_mosaic_buf_init(0);
  #endif

  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  register_dvb_tables();

  size = p_dvb->get_mem_size(p_dvb, TRUE, MAX_FILTER_NUM, 128);

  p_dvb2_mem = mtos_malloc(10);
  MT_ASSERT(p_dvb2_mem != NULL);
  memset(p_dvb2_mem, 0x3b, 10);
  mem_show();

  p_buf = mtos_malloc(size);

  p_dvb->start(p_dvb, p_buf, size, TRUE, MAX_FILTER_NUM, 128);

 {
  //m_svc_t * p_svc = NULL;
  m_init_param.nstksize = MDL_MONITOR_TASK_STKSIZE;
  m_init_param.service_prio = MDL_MONITOR_TASK_PRIORITY;
  dvb_monitor_service_init(&m_init_param);
  //p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);

  //p_svc->get_svc_instance(class_get_handle_by_id(M_SVC_CLASS_ID), APP_FRAMEWORK);
}

  p_db_mem = mtos_malloc(10);
  MT_ASSERT(p_db_mem != NULL);
  memset(p_db_mem, 0x4b, 10);

  db_dvbs_init();
  smcctrl_init();
  //sys status initial.
  OS_PRINTK("ap sys status init start!\n");
  sys_status_init();
  OS_PRINTK("ap sys status init over!\n");
  if(sys_status_get()->av_set.tv_mode == 1)
    {
        p_avc_conf.disp_fmt = VID_SYS_NTSC_M;
    }
  else
    {
        p_avc_conf.disp_fmt = VID_SYS_PAL;
    }

  p_avc_conf.disp_chan = DISP_CHANNEL_SD;
  avc_init_1(&p_avc_conf);
#ifndef WIN32
  //sys_status_set_video_out();
  OS_PRINTK("\n----------------------------------------\n");
  OS_PRINTK(" changeset:%s\n", sys_status_get()->changeset);
  memset(version_buf,0,sizeof(version_buf));
  sys_status_get_all_hw_version(version_buf,sizeof(version_buf));
  OS_PRINTK("hardware version:%s \n",version_buf);
  memset(version_buf,0,sizeof(version_buf));
  sys_status_get_all_sw_version(version_buf,sizeof(version_buf));
  OS_PRINTK("software version:%s \n",version_buf);
  memset(version_buf,0,sizeof(version_buf));
  sys_status_get_all_boot_version_info(version_buf,sizeof(version_buf));
  OS_PRINTK("boot info:%s \n",version_buf);
  OS_PRINTK("\n----------------------------------------\n");
#endif
   //ui_show_logo(LOGO_BLOCK_ID_M0);
  //time initial
	OS_PRINTK("\n--------------------546546--------------------\n");
  sys_status_get_utc_time(&time);
  time_init(&time);

  //set time zone(should be called after time initial.)
  sys_status_set_time_zone();

  p_epg_mem = mtos_malloc(10);
  MT_ASSERT(p_epg_mem != NULL);
  memset(p_epg_mem, 0x5b, 10);
  epg_data_init4();
  //nvod_data_init();

  p_gb_mem = mtos_malloc(10);
  MT_ASSERT(p_gb_mem != NULL);
  memset(p_gb_mem, 0xFF, 10);
  link_gb2312_maptab(NULL, 0);

#if 0
{
  extern int sc_initialize(void);
  sc_initialize();
}
#endif

  //ca_svc_init();
  //cas init.
#if 0
#endif
}


// TODO: ... add declare to_i.h files.
extern ap_frm_policy_t *construct_ap_frm_policy(void);
extern ap_uio_policy_t *construct_ap_uio_policy(void);
extern epg_policy_t *construct_epg_policy4(void);
extern sig_mon_policy_t *construct_sig_mon_policy(void);
extern pb_policy_t *construct_pb_policy(void);
extern ota_policy_t *construct_ota_policy(void);
extern upg_policy_t *construct_upg_policy(void);
extern time_policy_t *construct_time_policy(void);
extern cas_policy_t *construct_cas_policy(void);
extern app_t *construct_ap_ota(ota_policy_t *p_policy);
extern app_t *construct_ap_nvod(void);
//extern app_t *construct_ap_browser(void);

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_BOYUAN)
extern app_t *construct_ap_ad(void);
#endif
#define INIT_APP_INFO(info, id, name, instance, task_prio, task_stack) \
  { \
    info.app_id = id; \
    info.p_name = name; \
    info.p_instance = instance; \
    info.priority = task_prio; \
    info.stack_size = task_stack; \
  }


void app_init(void)
{
  ap_frm_init_param_t param = {0};
  handle_t ap_frm_handle = NULL;
  u32 ap_frm_msgq_id = 0;
  u8 idx = 0;
  #ifndef WIN32
  #if(CONFIG_ADS_ID != CONFIG_ADS_ID_BOYUAN)
  ap_pic_policy_t picture_policy;
  #endif
  #endif
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

  INIT_APP_INFO(param.app_array[idx],
                APP_PLAYBACK,
                (u8 *)"app playback",
                construct_ap_playback_1(construct_pb_policy()),
                AP_PLAYBACK_TASK_PRIORITY,
                AP_PLAYBACK_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
#ifndef WIN32
   INIT_APP_INFO(param.app_array[idx],
                APP_CA,
                (u8 *)"app ca",
                construct_ap_cas(construct_cas_policy()),
                AP_CAS_TASK_PRIORITY,
                AP_CA_TASK_STKSIZE);
   idx++;

  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
#endif

  INIT_APP_INFO(param.app_array[idx],
                APP_UIO,
                (u8 *)"app uio",
                construct_ap_uio(construct_ap_uio_policy()),
                AP_UIO_TASK_PRIORITY,
                AP_UIO_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

  INIT_APP_INFO(param.app_array[idx],
                APP_EPG,
                (u8 *)"app epg",
                construct_ap_epg4(construct_epg_policy4()),
                AP_EPG_TASK_PRIORITY,
                AP_EPG_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

  INIT_APP_INFO(param.app_array[idx],
                APP_SIGNAL_MONITOR,
                (u8 *)"app singal monitor",
                construct_ap_signal_monintor(construct_sig_mon_policy()),
                AP_SIGN_MON_TASK_PRIORITY,
                AP_SIGN_MON_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

  INIT_APP_INFO(param.app_array[idx],
                APP_SCAN,
                (u8 *)"app scan",
                construct_ap_scan(),
                AP_SCAN_TASK_PRIORITY,
                AP_SCAN_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

#ifdef OTA_SUPPORT
   INIT_APP_INFO(param.app_array[idx],
                APP_OTA,
                (u8 *)"app ota",
                construct_ap_ota(construct_ota_policy()),
                AP_OTA_TASK_PRIORITY,
                AP_OTA_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
  #ifdef OTA_CHECK_DEFAULT
  INIT_APP_INFO(param.app_array[idx],
                APP_RESERVED1,
                (u8*)"app ota monitor",
                construct_ap_ota_monitor(),
                AP_OTA_MON_TASK_PRIORITY,
                AP_SIGN_MON_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

#endif

#endif
/*
  INIT_APP_INFO(param.app_array[idx],
                APP_UPG,
                "app upgrade",
                construct_ap_upg(construct_upg_policy()),
                AP_UPGRADE_TASK_PRIORITY,
                AP_UPGRADE_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
*/
  INIT_APP_INFO(param.app_array[idx],
                APP_TIME,
               (u8 *)"app time",
                construct_ap_time(construct_time_policy()),
                AP_TIME_TASK_PRIORITY,
                AP_TIME_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
#ifndef WIN32

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_BOYUAN)
  INIT_APP_INFO(param.app_array[idx],
                APP_AD,
                (u8 *)"app ad",
                construct_ap_ad(),
                AP_PICTURE_TASK_PRIORITY,
                AP_PICTURE_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
 #endif

#if(CONFIG_ADS_ID != CONFIG_ADS_ID_BOYUAN)
  picture_policy.max_pic_cnt_one_time = 2;
  picture_policy.pic_chain_priority[0] = PICTURE_CHAIN_1_PRIORITY;
  picture_policy.pic_chain_priority[1] = PICTURE_CHAIN_2_PRIORITY;
  INIT_APP_INFO(param.app_array[idx],
                APP_PICTURE,
                (u8*)"app pic",
                construct_ap_pic_play(&picture_policy),
                AP_PICTURE_TASK_PRIORITY,
                AP_PICTURE_TASK_STKSIZE);
  idx++;
#endif
#endif
#if 0
  INIT_APP_INFO(param.app_array[idx],
                APP_NVOD,
                (u8 *)"app nvod",
                construct_ap_nvod(),
                AP_NVOD_TASK_PRIORITY,
                AP_SCAN_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
 #endif
#if 0

  INIT_APP_INFO(param.app_array[idx],
                APP_BROWSER,
                "app browser",
                construct_ap_browser(),
                AP_BROWSE_TASK_PRIORITY,
                AP_BROWSE_TASK_STKSIZE);
  idx++;
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);
#endif

  param.p_implement_policy = construct_ap_frm_policy();
  param.num_of_apps = idx;
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;

  ap_frm_handle = ap_frm_init(&param, &ap_frm_msgq_id);
  p_ap1_mem[idx] = mtos_malloc(10);
  MT_ASSERT(p_ap1_mem[idx] != NULL);
  memset(p_ap1_mem[idx], 0x2a, 10);

  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
}


extern void ui_init(void);
void ap_init(void)
{
  //dvbc_lock_t main_tp = {0};
  customer_cfg_t cfg = {0};
  set_customer_config();
  mem_cfg(MEMCFG_T_NORMAL);
  set_customer_config();
  get_customer_config(&cfg);
  OS_PRINTK("\n******************************************\n");
  OS_PRINTK("jazz nongwang ap customer:[%d] build:TIME %s %s \n",
                                                        cfg.customer,__DATE__,__TIME__);
  OS_PRINTK("\n******************************************\n");

#ifdef OTA_DM_ON_STATIC_FLASH
  OS_PRINTK("\n----user new ota v2.1 ota data on static flash-----\n");
#endif

  p_board_mem = mtos_malloc(10);
  MT_ASSERT(p_board_mem != NULL);
  memset(p_board_mem, 0x1a, 10);

  p_cfg_mem = mtos_malloc(10);
  MT_ASSERT(p_cfg_mem != NULL);
  memset(p_cfg_mem, 0x2a, 10);

  util_init();
  p_util_mem = mtos_malloc(10);
  MT_ASSERT(p_util_mem != NULL);
  memset(p_util_mem, 0x3a, 10);

  drv_init();
  p_drv_mem = mtos_malloc(10);
  MT_ASSERT(p_drv_mem != NULL);
  memset(p_drv_mem, 0x4a, 10);
#ifndef WIN32
  board_config();
#endif

  middleware_init();
  p_mdl_mem = mtos_malloc(10);
  MT_ASSERT(p_mdl_mem != NULL);
  memset(p_mdl_mem, 0x5a, 10);

  DEBUG_ENABLE_MODE(TRCA, INFO);
  DEBUG_ENABLE_MODE(TRAD, INFO);
  ads_ap_init();

  #ifdef PowerOnLogo_3
  ui_show_logo(LOGO_BLOCK_ID_M4);
  #endif
  

  app_init();
  p_ap_mem = mtos_malloc(10);
  MT_ASSERT(p_ap_mem != NULL);
  memset(p_ap_mem, 0x6a, 10);

  {
    dvb_set_convert();
    //sys_status_get_main_tp1(&main_tp);
    //channel_init(main_tp.tp_freq, main_tp.tp_sym,main_tp.nim_modulate, 0x1DA1, 0xDA);
  }

  ui_init();
  p_ui_mem = mtos_malloc(10);
  MT_ASSERT(p_ui_mem != NULL);
  memset(p_ui_mem, 0x7a, 10);

  #ifdef PowerOnLogo_3
  	ui_show_logo(LOGO_BLOCK_ID_M5);
  #endif

#if 0
  extern void patch_init(void);
  patch_init();
#endif

}


void mem_dbg(u8 *p_mem, u32 size)
{
  u32 cnt = 0;

  if(p_mem == NULL)
  {
    OS_PRINTF("No valid memory allocation");
    return;
  }

  OS_PRINTF("!!!!mem dbg\n");
  for(cnt = 0; cnt < size; cnt++)
  {
    OS_PRINTF("mem_start_addr[%x], data[%x]\n", (u32)p_mem, *(p_mem + cnt));
  }

  OS_PRINTF("!!!!mem dbg end\n");
}


void mem_dump(void)
{
  u16 i = 0;

  //Board memory
  mem_dbg(p_board_mem, 10);

  //cfg memory
  mem_dbg(p_cfg_mem, 10);

  //Board memory
  mem_dbg(p_util_mem, 10);
  mem_dbg(p_drv_mem, 10);
  mem_dbg(p_nc_mem, 10);
  mem_dbg(p_dvb_mem, 10);
  mem_dbg(p_dvb2_mem, 10);
  mem_dbg(p_db_mem, 10);
  mem_dbg(p_epg_mem, 10);
  mem_dbg(p_gb_mem, 10);
  //cfg memory
  mem_dbg(p_mdl_mem, 10);
  for(i = 0; i < 11; i++)
  {
    mem_dbg(p_ap1_mem[i], 10);
  }

  //Board memory
  mem_dbg(p_ap_mem, 10);
  mem_dbg(p_ui_mem, 10);
}


void flush_dcache_all() // for compile OK
{

}
