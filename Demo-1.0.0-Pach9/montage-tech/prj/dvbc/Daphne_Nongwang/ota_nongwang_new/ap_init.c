// lib
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
//#include "sys_devs.h"
//#include "sys_cfg.h"
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

#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"

#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "sys_types.h"
#include "spi.h"
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
#include "dsmcc.h"

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
//#include "ca_svc.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "ss_ctrl.h"

#include "surface.h"
//#include "db_dvbs.h"
#include "mem_cfg.h"
// ap
#include "ap_framework.h"
#include "ap_kingvon_ota.h"
#include "sys_cfg.h"


#include "ap_uio.h"
#include "sys_data_staic.h"
#include "customer_def.h"

#ifdef WIN32
#undef TEST_FRAMEWORK
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

u8 *p_ap1_mem[13] = {NULL};

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


#ifdef WIN32
extern RET_CODE disp_win32_attach(char *p_name);
#endif
extern u32 get_flash_addr(void);
 void init_fp_cfg(fp_cfg_t *p_cfg)
{
  // use default in drv
  p_cfg->map_size = FP_TABLE_SIZE;
  p_cfg->p_map = fp_bitmap;
  p_cfg->p_info = (pan_hw_info_t *)&pan_info;
}

 BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  // init osd0 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD0_8BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD0_8BIT_BUFFER);

  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr +  OSD0_ODD_MEMSIZE;
  p_cfg->p_osd0_cfg->even_mem_start = p_cfg->p_osd0_cfg->odd_mem_end;
  p_cfg->p_osd0_cfg->even_mem_end = p_cfg->p_osd0_cfg->even_mem_start + OSD0_EVEN_MEMSIZE;

  return TRUE;
}

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


#ifdef WIN32

void uio_init()
{
  RET_CODE ret;
  void *p_dev = NULL;
  uio_cfg_t  uiocfg  = {0};
  fp_cfg_t   fpcfg   = {0};
  irda_cfg_t irdacfg = {0};
  u8 content[5] = {' ', 'B','L',' ', 0};

  /* uio including irda and frontpanel dev */
  ret = ATTACH_DRIVER(UIO, magic, default, gpio);

  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  irdacfg.protocol = IRDA_NEC;
  init_fp_cfg(&fpcfg);

  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);

  uio_display(p_dev, content, 4);
}

static void drv_init(void)
{
  RET_CODE ret;
  void  *p_dev = NULL;
  i2c_cfg_t i2c_cfg = {0};
  nim_config_t nim_cfg;


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


  /* uio */
  uio_init();

  /* nim */
  ret = ATTACH_DRIVER(NIM, m88cs2200, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_dev);

  if(init_nim_cfg(&nim_cfg))
  {
    ret = dev_open(p_dev, &nim_cfg);
  }
  else
  {
    ret = dev_open(p_dev, NULL);
  }

  dev_io_ctrl(p_dev, NIM_IOCTRL_SET_LNB_ONOFF, TRUE);

  MT_ASSERT(SUCCESS == ret);

  /* osd dev */
  ret = ATTACH_DRIVER(OSD, magic, default, default);
  MT_ASSERT(ret == SUCCESS);

  /* gpe dev */
  ret = ATTACH_DRIVER(GPE_4BIT, magic, default, default);
  MT_ASSERT(ret == SUCCESS);

  /* pti */
  ret = ATTACH_DRIVER(PTI, magic, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev);

  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);


}



#else

extern void drv_globalinit(void);
extern void glb_info_init(void);
extern void ota_uio_init(void);
extern void nim_init_config(nim_config_t *nim_cfg);
static void drv_init(void)
{
  RET_CODE ret;
  void *p_dev = NULL;
#ifdef CHIP
#else
  outl(0x06462b22, 0x12000000);/**FPGA need ;but IC don't need*****/
#endif

  OS_PRINTF("drv --=5\n");
  {
    /* i2c master dev */
    i2c_cfg_t i2c_cfg = {0};


    ret = ATTACH_DRIVER(I2C, jazz, default, default);
    MT_ASSERT(SUCCESS == ret);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
    MT_ASSERT(NULL != p_dev);
#ifdef CHIP
  i2c_cfg.i2c_id = 1;
#else
#endif
    i2c_cfg.bus_clk_khz = 300;
    i2c_cfg.lock_mode = OS_TASK_LOCK;
    ret = dev_open(p_dev, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
  }
  OS_PRINTF("drv --6\n");
   //ota_uio_init(); //move to ota_check()

  OS_PRINTF("drv --7\n");

  {
    /* nim */
    nim_config_t nim_cfg = {0};


    ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
    MT_ASSERT(ret == SUCCESS);
#ifdef CHIP
  extern void tuner_attach_tc2800();
  tuner_attach_tc2800();
 // tuner_attach_XG_XDCT6A();
#else
    extern void tuner_attach_TDCC_G051F();
    tuner_attach_TDCC_G051F();
#endif


    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);


    /* get customer config data from flash */
    //if(init_nim_cfg(&nim_cfg))
    nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
#ifdef CHIP
  nim_cfg.ts_mode = NIM_TS_INTF_PARALLEL;
  nim_cfg.dem_ver = DEM_VER_1;
  nim_init_config(&nim_cfg);
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

  drv_globalinit();
  glb_info_init();

  OS_PRINTF("drv --9\n");
  {
    /* display */
    disp_cfg_t disp_cfg = {0};
    layer_cfg_t osd0_cfg = {0};
    layer_cfg_t sub_cfg = {0};
    layer_cfg_t osd1_cfg = {0};
    still_cfg_t still_cfg = {0};

    //display dev....
    disp_cfg.p_sub_cfg = &sub_cfg;
    disp_cfg.p_osd0_cfg = &osd0_cfg;
    disp_cfg.p_osd1_cfg = &osd1_cfg;
    disp_cfg.p_still_sd_cfg = &still_cfg;
    //disp_cfg.p_still_sd_cfg = NULL;

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

    //gpe_vsb dev....
    ret = ATTACH_DRIVER(GPE_VSB, jazz, default, default);
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
  #ifdef CUS_XIONGMAO
  *((volatile unsigned int *)( 0x7000008c )) = 0x1063; // power up analog
  #endif
  
  OS_PRINTF("drv --11\n");

}
#endif


void register_dvb_tables(void)
{
  dvb_register_t reg_param = {0};
  dvb_t *p_dvb_handle = NULL;

  p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_dsmcc_dsi_dii_section;
  reg_param.request_proc = request_dsmcc_dsi_dii_section;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_dvb_handle->register_table(p_dvb_handle, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_dsmcc_db_section;
  reg_param.request_proc = request_dsmcc_db_multi_section;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_DSMCC_DDM;
  p_dvb_handle->register_table(p_dvb_handle, &reg_param);
}


void middleware_init(void)
{
  handle_t dm_handle = NULL;
  dvb_t *p_dvb = NULL;
  u32 size = 0;
  u8 *p_buf = NULL;
//  avc_cfg_t p_avc_conf;
//  utc_time_t time;
//  m_svc_init_para_t m_init_param = {0};

  mdl_init();
  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(dm_handle != NULL);

  // Whether cache is enabled, it is up to the configuration from UI
  //OS_PRINTF("dm set cache\n");
  //dm_set_cache(dm_handle, IW_TABLE_BLOCK_ID, MAX_TABLE_CACHE_SIZE);
  //dm_set_cache(dm_handle, IW_VIEW_BLOCK_ID, MAX_VIEW_CACHE_SIZE);
  //OS_PRINTF("dm set cache is done\n");

  p_nc_mem = mtos_malloc(10);
  MT_ASSERT(p_nc_mem != NULL);
  memset(p_nc_mem, 0x1b, 10);
  {
    nc_svc_cfg_t nc_cfg = {0};
    nc_cfg.b_sync_lock = TRUE;
    nc_cfg.lock_mode = SYS_DVBC;
    nc_cfg.priority = MDL_NIM_CTRL_TASK_PRIORITY;
    nc_cfg.stack_size = MDL_NIM_CTRL_TASK_STKSIZE;
    nc_cfg.auto_iq = TRUE;
    nc_cfg.wait_ticks = 100;  //500ms
    nc_svc_init(&nc_cfg);
  }
   
  p_dvb_mem = mtos_malloc(10);
  MT_ASSERT(p_dvb_mem != NULL);
  memset(p_dvb_mem, 0x2b, 10);
#ifndef WIN32
  dvb_init_1(MDL_DVB_TASK_PRIORITY, MDL_DVB_TASK_STKSIZE);
#else
  dvb_init(MDL_DVB_TASK_PRIORITY, MDL_DVB_TASK_STKSIZE);
#endif
  //nvod_mosaic_buf_init(30 * KBYTES);
  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  register_dvb_tables();
  OS_PRINTF("register table ok \n");

  size = p_dvb->get_mem_size(p_dvb, TRUE, 4, 4);
  OS_PRINTF("get mem ok \n");

  p_buf = mtos_malloc(size);
  p_dvb->start(p_dvb, p_buf, size, TRUE, 4, 4);
  OS_PRINTF("start ok \n");
}


// TODO: ... add declare to_i.h files.
extern ap_frm_policy_t *construct_ap_frm_policy(void);
extern ap_uio_policy_t *construct_ap_uio_policy(void);
extern ota_policy_t *construct_mingxin_ota_policy(void);
extern app_t *construct_ap_mingxin_ota(ota_policy_t *p_policy);

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

   OS_PRINTF("app_init 1 !\n");

  INIT_APP_INFO(param.app_array[idx],
                APP_OTA,
                (u8*)"app ota bl",
                construct_ap_mingxin_ota(construct_mingxin_ota_policy()),
                AP_OTA_TASK_PRIORITY,
                AP_OTA_TASK_STKSIZE);
  idx++;
   OS_PRINTF("app_init 2\n");

  INIT_APP_INFO(param.app_array[idx],
                APP_UIO,
                (u8*)"app uio",
                construct_ap_uio(construct_ap_uio_policy()),
                AP_UIO_TASK_PRIORITY,
                AP_UIO_TASK_STKSIZE);
  idx++;
   OS_PRINTF("app_init 3 !\n");
  param.p_implement_policy = construct_ap_frm_policy();
  param.num_of_apps = idx;
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;

  ap_frm_handle = ap_frm_init(&param, &ap_frm_msgq_id);
  OS_PRINTF("app_init 5 !\n");
  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
    OS_PRINTF("app_init 6 !\n");


}

extern void ui_init(void);

static void board_config(void)
{
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

}

  
void ota_init_info(ota_info_t *otai)
{
  customer_cfg_t cfg = {0};
  set_customer_config();
  get_customer_config(&cfg);
  OS_PRINTK("\n******************************************\n");
  OS_PRINTK("jazz nongwang ota v2.1 customer:[%d] build:TIME %s %s \n",
                                                        cfg.customer,__DATE__,__TIME__);
  OS_PRINTK("otai.orig_software_version=%d\n",otai->orig_software_version);
  OS_PRINTK("otai.new_software_version=%d\n",otai->new_software_version);
  OS_PRINTK("otai.download_data_pid=%d\n",otai->download_data_pid);
  OS_PRINTK("otai.lockc.tp_freq=%d\n",otai->lockc.tp_freq);
  OS_PRINTK("otai.lockc.tp_sym=%d\n",otai->lockc.tp_sym);
  OS_PRINTK("otai.lockc.nim_modulate=%d\n",otai->lockc.nim_modulate);
  OS_PRINTK("otai.sys_mode=%d\n",otai->sys_mode);
  OS_PRINTK("otai.ota_tri=%d\n",otai->ota_tri);
  OS_PRINTK("\n******************************************\n");
}
void ap_init(void)
{
  mem_cfg(MEMCFG_T_NORMAL);
  set_customer_config();
  p_board_mem = mtos_malloc(10);
  MT_ASSERT(p_board_mem != NULL);
  memset(p_board_mem, 0x1a, 10);
 OS_PRINTF("ap_init start \n");
  p_cfg_mem = mtos_malloc(10);
  MT_ASSERT(p_cfg_mem != NULL);
  memset(p_cfg_mem, 0x2a, 10);
  OS_PRINTF("util_init start \n");
  util_init();
  p_util_mem = mtos_malloc(10);
  MT_ASSERT(p_util_mem != NULL);
  memset(p_util_mem, 0x3a, 10);

  drv_init();
  p_drv_mem = mtos_malloc(10);
  MT_ASSERT(p_drv_mem != NULL);
  memset(p_drv_mem, 0x4a, 10);
  OS_PRINTF("drv_init ok \n");
#ifndef WIN32
  board_config();
  OS_PRINTF("board_config ok \n");
#endif
  middleware_init();
  OS_PRINTF("middleware_init ok \n");

  p_mdl_mem = mtos_malloc(10);
  MT_ASSERT(p_mdl_mem != NULL);
  memset(p_mdl_mem, 0x5a, 10);

  app_init();
  OS_PRINTF("app_init end !\n");
  p_ap_mem = mtos_malloc(10);
  MT_ASSERT(p_ap_mem != NULL);
  memset(p_ap_mem, 0x6a, 10);
  OS_PRINTF("app_init ok \n");

  ui_init();
  p_ui_mem = mtos_malloc(10);
  MT_ASSERT(p_ui_mem != NULL);
  memset(p_ui_mem, 0x7a, 10);
  OS_PRINTF("ui_init ok \n");

#if 0
  extern void patch_init(void);
  patch_init();
#endif
}


void flush_dcache_all() // for compile OK
{

}
