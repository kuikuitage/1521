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

#include "config_customer.h"
#include "config_prj.h"
#include "ads_funtion_public.h"

#include "ui_util_api.h"
#include "ui_ad_logo_api.h"

#include "ui_ads_ds_display.h"
#include "ads_ware.h"

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

u8 *p_ads_mem = NULL;

u32 adm_id = ADS_ID_ADT_DESAI;

static ad_lock_t ad_maintp = {307000,6875,2};
static ads_module_cfg_t module_config = {0};
static u8 version[25];
extern u32 get_flash_addr(void);

//store the AD data to flash
static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
  RET_CODE ret = FALSE;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  OS_PRINTF("ad write addr[0x%x], size[0x%x]\n", offset, size);
  //offset -= module_config.flash_start_adr;
  if(size!=0)
  {
    ret=dm_direct_write(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,size,p_buf);
  }
  OS_PRINTF("AD nvram_write end\n");

  return TRUE;
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

  return TRUE;
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

static RET_CODE nvram_erase2(u32 offset, u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  OS_PRINTF("ADS:Erase size_erase = 0x%x \n", size);
  //offset -= module_config.flash_start_adr;
  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, size);
  MT_ASSERT(ret != DM_FAIL);

  return TRUE;
}
u8* ads_get_ad_version(void)
{
  return version;
}
static void ads_module_open(u32 m_id)
{
  RET_CODE ret = ERR_FAILURE;

  ret = ads_open(m_id);
  OS_PRINTF("ADS: ads_module_open===%d\n", ret);
 // MT_ASSERT(SUCCESS == ret);
}

extern RET_CODE ads_adt_desai_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);

static RET_CODE ads_desai_module_init()
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;

  module_config.channel_frequency = ad_maintp.tp_freq;
 // module_config.channel_frequency = ads_maintp->tp_freq;
  module_config.channel_symbolrate = ad_maintp.tp_sym;
  module_config.channel_qam = ad_maintp.nim_modulate;

  //module_config.flash_start_adr= dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID)
                                     //             - get_flash_addr();
  module_config.flash_start_adr= 0;
  //ads_module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  module_config.nvram_read= nvram_read;
  module_config.nvram_write= nvram_write;
  module_config.nvram_erase= nvram_erase;
  module_config.nvram_erase2 = nvram_erase2;
  module_config.flash_size= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);
  module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  module_config.pic_in = ADS_PIC_IN_SDRAM;
  module_config.task_prio_start= AD_TASK_PRIO_START;
  module_config.task_prio_end= AD_TASK_PRIO_END;
  module_config.display_ad = ui_adv_desai_display;
  module_config.hide_ad = ui_adv_desai_hide;
  module_config.platform_type = ADS_PLATFORM_SD;
  OS_PRINTF("ADS:ads_module_config.flash_start_adr===0x%x, flash_size = 0x%x \n",   module_config.flash_start_adr, module_config.flash_size);

  ret = ads_adt_desai_attach(&module_config, &adm_id);

  MT_ASSERT(SUCCESS == ret);

  ret=ads_init(ADS_ID_ADT_DESAI,&module_config);
  if(ret != SUCCESS)
  {
    return ret;
  }

  ads_module_open(ADS_ID_ADT_DESAI);
  mtos_task_delay_ms(100);
  memset(version,0,25);
  ads_io_ctrl(ADS_ID_ADT_DESAI, ADS_IOCMD_AD_VERSION_GET, version);
  mtos_printk("Desai_ADS Version:%s\n",version);
  return SUCCESS;
#endif
}
RET_CODE nvram_ad_logo_read(u32 offset, u8 *p_buf, u32 p_size);
static void ui_show_desai_adver_logo(void)
{
    u32 flash_addr = 0;
    u32 block_size = 0;
    RET_CODE  ret = 0; 
    u8 *p_logo_buffer = NULL;
    u8 *p_logo_data = NULL;
    u32 offset = 0;
    //u32 logo_info_size =0;
    ST_AD_DS_LOG_SAVE_INFO *logo_info;

    flash_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),DEC_RW_ADS_LOGO_BLOCK_ID)
                 - FLASH_BASE_ADDR;
    block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_LOGO_BLOCK_ID);

    if(flash_addr > CHARSTO_SIZE)
    {
       ui_show_logo(LOGO_BLOCK_ID_M0);
       return; 
    } 
   
    p_logo_buffer = (u8*)mtos_malloc(block_size);
    if(p_logo_buffer == NULL)
    {
      OS_PRINTF("[DESAI_ADS] buffer malloc  error \n");
      ui_show_logo(LOGO_BLOCK_ID_M0);
      return; 
    }
    memset(p_logo_buffer, 0, block_size);
    
    ret = nvram_ad_logo_read(offset, (u8*)p_logo_buffer, block_size);
    if(ret != SUCCESS)
    {
      OS_PRINTF("[DESAI_ADS] read logo error,show defaule logo \n");
      ui_show_logo(LOGO_BLOCK_ID_M0);
      mtos_free(p_logo_buffer);
      return;
    }
    //show logo
    OS_PRINTF("[DESAI_ADS] show logo by ad nvm_data \n");
    
    logo_info = (ST_AD_DS_LOG_SAVE_INFO *)p_logo_buffer;
    
    p_logo_data = (u8*)(p_logo_buffer+ sizeof(ST_AD_DS_LOG_SAVE_INFO));
    
    if(DSAD_AV_VIDEO_M2V == logo_info->ucLogType)
    {
      ui_show_logo_by_data(logo_info->uiLogoSize, p_logo_data);
    }
    else
    {    
      ui_show_logo(LOGO_BLOCK_ID_M0);
    }
    mtos_free(p_logo_buffer);
    return;
  }

static void ads_show_logo(void)
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
    ui_show_desai_adver_logo();
    OS_PRINTF("ADS_LOGO: Desai ads boot logo show \n");
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
  //set_adver_maintp(858000,6875,NIM_MODULA_QAM64);
  
  OS_PRINTF("[AD_INIT] tmp_maintp.nim_modulate= %d \n",tmp_maintp.nim_modulate);
      
  ret = ads_desai_module_init();
  if(ret == SUCCESS)
  {
    p_ads_mem = mtos_malloc(10);
    MT_ASSERT(p_ads_mem != NULL);
    memset(p_ads_mem, 0x8a, 10);
    //OS_PRINTF("[AD_INIT] show ads logo  %s %d \n",__func__,__LINE__);
    ads_show_logo();
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

