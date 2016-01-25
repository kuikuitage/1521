/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "ads_ware.h"

// ap headers
#include "ap_framework.h"
//#include "ap_cas.h"
#include "sys_cfg.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "../../../../inc/os/mtos_sem.h"

#include  "../../../../src/kware/ads/ads_adapter.h"
#include  "../../../../src/kware/ads/ads_quanzhi/Qzdef.h"
#include "data_manager.h"


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

//static ad_lock_t ad_maintp = {407000,6875,2};
static ad_lock_t ad_maintp = {858000,6875,2};
static ads_module_cfg_t module_config = {0};

void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate)
{
  ad_maintp.tp_freq = tp_freq;
  ad_maintp.tp_sym = tp_sym;
  switch(nim_modulate)
  {
    case 0:
       ad_maintp.nim_modulate = 16;
      break;

    case 1:
       ad_maintp.nim_modulate = 32;
      break;

    case 2:
       ad_maintp.nim_modulate = 64;
      break;

    case 3:
       ad_maintp.nim_modulate = 128;
      break;

    case 4:
      ad_maintp.nim_modulate = 256;
      break;

    default:
       ad_maintp.nim_modulate = 64;
      break;
  }

}

//store the AD data to flash
static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
  RET_CODE ret = SUCCESS;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  //OS_PRINTF("ad write addr[0x%x], size[0x%x]\n", offset, size);

  if(size!=0)
  {
    ret=dm_direct_write(p_dm_handle,LOGO_BLOCK_ID_M3,offset,size,p_buf);
  }
  //OS_PRINTF("AD nvram_write end\n");

  return SUCCESS;
}

//read the ad data from flash
static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *p_size)
{
  void *p_dm_handle = NULL;
  u32 nvm_read =0;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  //OS_PRINTF("ad read dm_direct_read addr[0x%x], size[0x%x]\n", offset,*p_size);

  //nvm_read=dm_read(p_dm_handle,LOGO_BLOCK_ID_M3,node_id,offset,*p_size,p_buf);
  nvm_read=dm_direct_read(p_dm_handle,LOGO_BLOCK_ID_M3,offset,*p_size,p_buf);
  //OS_PRINTF("ad nvm_read[0x%x]\n", nvm_read);
  //for(i=0;i<nvm_read;i++)
  //OS_PRINTF("ad nvm_read[0x%x]\n", p_buf[i]);

  //OS_PRINTF("AD nvram_read success!\n");

  return SUCCESS;
}


static RET_CODE nvram_erase(u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;
  u32 offset = 0;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  OS_PRINTF("ADS:Erase size-pre===%d\n", size);
  ret=dm_direct_erase(p_dm_handle,LOGO_BLOCK_ID_M3,offset, module_config.flash_size);
  MT_ASSERT(ret != DM_FAIL);

  return SUCCESS;
}

static void ads_module_open(u32 m_id)
{
  RET_CODE ret = ERR_FAILURE;

  ret = ads_open(m_id);
  OS_PRINTF("ADS: ads_module_open===%d\n", ret);
  MT_ASSERT(SUCCESS == ret);
}

extern RET_CODE ui_adv_set_pic(u32 length, u8 *p_data);
extern RET_CODE ui_adv_set_float(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data);
//extern void ads_show_logo(void);
RET_CODE ads_module_init(void)
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;

  OS_PRINTF("ADS: ise==init enter\n" );

  //customize ads
  ads_event_info_t event_info;
  ads_event_id_t  ads_event = ADS_EVT_NONE;
  u32 bticks=0;
  u32 adm_id;

  module_config.channel_frequency = ad_maintp.tp_freq;
  module_config.channel_symbolrate = ad_maintp.tp_sym;
  module_config.channel_qam = ad_maintp.nim_modulate;

  module_config.flash_start_adr= dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), LOGO_BLOCK_ID_M3);
  //module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  module_config.nvram_read= nvram_read;
  module_config.nvram_write= nvram_write;
  module_config.nvram_erase= nvram_erase;
  module_config.flash_size= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), LOGO_BLOCK_ID_M3);
  module_config.pic_in = ADS_PIC_IN_SDRAM;
  module_config.task_prio_start= AD_TASK_PRIO_START;
  module_config.task_prio_end= AD_TASK_PRIO_END;
  module_config.read_gif = ui_adv_set_pic;
  module_config.read_float= ui_adv_set_float;

  OS_PRINTF("ADS: task_prio_start\n" );
  OS_PRINTF("ADS:module_config.flash_start_adr===0x%x\n",   module_config.flash_start_adr);

  ret = ads_adt_qz_attach(&module_config, &adm_id);
  MT_ASSERT(SUCCESS == ret);

  ret=ads_init(adm_id,&module_config);
  if(ret != SUCCESS && ret != ERR_TIMEOUT)
  {
    return ret;
  }
  //ads_show_logo();
  //MT_ASSERT(SUCCESS == ret);

  bticks = mtos_ticks_get();
  ret = ads_io_ctrl(adm_id, ADS_IOCMD_AD_SEARCH_SET, &module_config);
  OS_PRINTF("ADS: ioctrl ret===%d\n", ret);

  while(1)
  {
    ads_event_polling(&event_info);
    ads_event = ads_event_parse(event_info.events[adm_id]);

    //OS_PRINTF("ads_module_init while 1\n");

    if(ads_event == ADS_S_ADSERACH_FINISHED)
    {
      break;
    }
    if ((mtos_ticks_get() - bticks) > 2000)
    {
      ret = ads_io_ctrl(ADS_ID_ADT_QZ, ADS_IOCMD_AD_EXIT_TIMEOUT, NULL);
      OS_PRINTF("ADS: search time out");
    }

    mtos_task_sleep(500);
  }
  OS_PRINTF("ads_module_init while 4\n");
  ads_module_open(adm_id);
  mtos_task_delay_ms(100);
  ret = SUCCESS;
  return ret; 
#endif
}


