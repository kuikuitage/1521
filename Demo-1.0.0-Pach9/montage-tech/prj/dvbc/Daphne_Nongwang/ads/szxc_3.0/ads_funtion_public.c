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
#include "sub.h"
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
//#include "ca_svc.h"
#include "ads_ware.h"

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
#ifdef OTA_DM_ON_STATIC_FLASH
#include "sys_data_staic.h"
#endif
#include "mem_cfg.h"

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

#include "ap_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#include "ap_pic_play.h"

#include "customer_def.h"
#include "config_customer.h"
#include "config_prj.h"
#include "ui_util_api.h"

#include "ads_ware.h"
#include "ads_funtion_public.h"
#include "ui_ads_szxc_display.h"
#include "ui_ad_logo_api.h"

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u16 nim_modulate;

} ad_lock_t;

extern void ui_show_logo_by_data(u32 size, u8 *p_logo_addr);

u32 adm_id = ADS_ID_ADT_SZXC;

static ad_lock_t ad_maintp = {407000,6875,2};
static ads_module_cfg_t module_config = {0};

static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  OS_PRINTF("ad write addr[0x%x], size[0x%x]\n", offset, size);
  //offset -= module_config.flash_start_adr;
  if(size!=0)
  {
    ret = dm_direct_write(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,size,p_buf);
    MT_ASSERT(ret != DM_FAIL);
  }
  OS_PRINTF("AD nvram_write end\n");

  return SUCCESS;
}

//read the ad data from flash
static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *p_size)
{
  void *p_dm_handle = NULL;
  u32 nvm_read =0;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  OS_PRINTF("ad read dm_direct_read addr[0x%x], size[0x%x]\n", offset,*p_size);
  //offset -= module_config.flash_start_adr;
  //nvm_read=dm_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,node_id,offset,*p_size,p_buf);
  nvm_read=dm_direct_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,*p_size,p_buf);
  OS_PRINTF("ad nvm_read[0x%x]\n", nvm_read);
  //for(i=0;i<nvm_read;i++)
  //OS_PRINTF("ad nvm_read[0x%x]\n", p_buf[i]);

  OS_PRINTF("AD nvram_read success!\n");

  return SUCCESS;
}


static RET_CODE nvram_erase(u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;
  u32 offset = 0;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  OS_PRINTF("ADS:Erase size_pre===%d\n", size);
  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, module_config.flash_size);
  MT_ASSERT(ret != DM_FAIL);

  return SUCCESS;
}


static void ads_module_open(u32 m_id)
{
  RET_CODE ret = ERR_FAILURE;

  ret = ads_open(m_id);
  OS_PRINTF("ADS: ads_module_open===%d\n", ret);
 // MT_ASSERT(SUCCESS == ret);
}


static void ui_adv_szxc_logo(u32 length, u8 *p_data)
{
    OS_PRINTF("ADV: SHOW LOGO\n");
    ui_show_logo_by_data(length, p_data);
}

extern RET_CODE ads_adt_szxc_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);

static RET_CODE ads_szxc_module_init()
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;

  module_config.channel_frequency = ad_maintp.tp_freq;
  module_config.channel_symbolrate = ad_maintp.tp_sym;
  module_config.channel_qam = ad_maintp.nim_modulate;

  module_config.flash_start_adr= 0;
  module_config.flash_size= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);

  module_config.nvram_read= nvram_read;
  module_config.nvram_write= nvram_write;
  module_config.nvram_erase= nvram_erase;

  module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  module_config.pic_in = ADS_PIC_IN_SDRAM;
  module_config.task_prio_start= AD_TASK_PRIO_START;
  module_config.task_prio_end= AD_TASK_PRIO_END;
  module_config.display_logo = ui_adv_szxc_logo;
  module_config.display_ad = ui_adv_szxc_display;
  module_config.get_screen_statu = ui_adv_szxc_statu;
  module_config.hide_ad = ui_adv_szxc_hide;
  module_config.platform_type = ADS_PLATFORM_SD;
  OS_PRINTF("ADS:ads_module_config.flash_start_adr===0x%x, flash_size = 0x%x \n",   module_config.flash_start_adr, module_config.flash_size);

  ret = ads_adt_szxc_attach(&module_config, &adm_id);

  MT_ASSERT(SUCCESS == ret);

  ret=ads_init(ADS_ID_ADT_SZXC,&module_config);
  OS_PRINTF("-------------line : %d , ret : %d",__LINE__,ret);
  if(ret != SUCCESS)
  {
    return ret;
  }

  ads_module_open(ADS_ID_ADT_SZXC);
  mtos_task_delay_ms(100);
  return SUCCESS;
#endif
}

static void ui_show_szxc_adver_logo()
{
    u8 *param = NULL;
    RET_CODE ret = ERR_FAILURE;
    ret = ads_io_ctrl(adm_id, ADS_IOCMD_AD_OPEN_LOGO, param);
    if( ret != SUCCESS )
    {
         OS_PRINTF("ADS: get logo wrong\n");
         return;
    }
}

static void sz_ads_show_logo(void)
{
  //ui_ads_set_show_logo(TRUE);
#ifndef WIN32
  //OS_PRINTF("NOR_LOGO:Normal logo first show get_logho_flag = 0x%x \n",get_boot_logo_flag());
#ifdef OTA_DM_ON_STATIC_FLASH
    if(get_sys_static_boot_flag() != 1)
#else
    if(get_boot_logo_flag() != 1) /**upg first boot***/
#endif
  {
    OS_PRINTF("NOR_LOGO:Normal logo first show \n");
    ui_show_logo(LOGO_BLOCK_ID_M0);
  }
  else
  {
    ui_show_szxc_adver_logo();
    OS_PRINTF("ADS_LOGO: szcx3.0 ads boot logo show \n");
  }
#endif
}

static void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate)
{
  ad_maintp.tp_freq = tp_freq;
  ad_maintp.tp_sym = tp_sym;
  switch(nim_modulate )
  {
    case 4:
       ad_maintp.nim_modulate = 16;
      break;

    case 5:
       ad_maintp.nim_modulate = 32;
      break;

    case 6:
       ad_maintp.nim_modulate = 64;
      break;

    case 7:
       ad_maintp.nim_modulate = 128;
      break;

    case 8:
      ad_maintp.nim_modulate = 256;
      break;

    default:
       ad_maintp.nim_modulate = 64;
      break;
  }
}

void ads_mem_init(u32 mem_start)
{
}

void ads_ap_init(void)
{
  dvbc_lock_t tmp_maintp = {0};
  RET_CODE ret = ERR_FAILURE;

  sys_status_get_main_tp1(&tmp_maintp);
  set_adver_maintp(tmp_maintp.tp_freq,tmp_maintp.tp_sym,tmp_maintp.nim_modulate);
  OS_PRINTF("[AD_INIT] tmp_maintp.nim_modulate= %d \n",tmp_maintp.nim_modulate);
      
  ret = ads_szxc_module_init();
  OS_PRINTF("-----ads_ap_init--------line : %d , ret : %d",__LINE__,ret);
  if(ret == SUCCESS)
  {
    sz_ads_show_logo();
    pic_set_init_ret(TRUE);
  }
  else
  {
#ifdef OTA_DM_ON_STATIC_FLASH
      if(get_sys_static_boot_flag() != 1)
#else
      if(get_boot_logo_flag() != 1)
#endif
      ui_show_logo(LOGO_BLOCK_ID_M0);
  }
}  

void subt_pic_ap_init(void)
{
#ifdef ROLL_TITLE
  subt_pic_module_cfg_t cfg;
  cfg.task_prio_start = subt_pic_TASK_PRIO_START;
  cfg.task_prio_end = subt_pic_TASK_PRIO_END;
  cfg.subt_is_scroll = subt_is_scroll;
  cfg.osd_display_is_ok = osd_display_is_ok;
  cfg.channel_info_set = channel_info_set;
  cfg.scroll_step_pixel_set = scroll_step_pixel_set;
  cfg.draw_rectangle = draw_rectangle;
  cfg.draw_picture = draw_picture;

  subt_pic_module_init(cfg);

  p_subt_pic_mem = mtos_malloc(10);
  MT_ASSERT(p_subt_pic_mem != NULL);
  memset(p_subt_pic_mem, 0x8a, 10);
#endif
}
/*
BOOL ui_set_pic_show (BOOL show)
{
   return TRUE;
}
void ui_pic_stop(void)
{
}
BOOL ui_pic_play(ads_ad_type_t type, u8 root_id)
{
  return TRUE;
}

BOOL ui_is_pic_showing (void)
{
    return TRUE;
}

BOOL ui_check_pic_by_type(ads_ad_type_t type)
{
    return FALSE;
}

void pic_player_init(void)
{
}

void ui_create_adv(void)
{
}
*/
void set_full_scr_ad_status(BOOL is_show)
{
  is_show = is_show;
 }



