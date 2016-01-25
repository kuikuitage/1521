/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <assert.h>

#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "sys_devs.h"
#include "sys_cfg.h"

#include "string.h"

#include "lib_util.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "fcrc.h"
#include "ss_ctrl.h"
#include "data_manager.h"
#include "ap_framework.h"
#include "ap_dvbs_ota.h"

/*!
  App. EPG implementation private data
  */
typedef struct impl_data_tag
{
  //DVBS ota tdi info structure;
  ota_tdi_t ota_tdi;
}ota_impl_data_t;


void ota_crc_init()
{
  RET_CODE ret;
  
   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret); 
}

u32 ota_crc32_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 
                                         0xFFFFFFFF, buf, len);
}

void write_otai(ota_info_t *p_otai)
{
  class_handle_t p_handle = class_get_handle_by_id(SC_CLASS_ID);
  ss_public_t *pub = ss_ctrl_get_public(p_handle);

  /* update the crc */
  /*p_otai->check_flag = OTAI_CHECK_FLAG;
  p_otai->upg_info_crc = ota_crc32_generate((u8 *)p_otai, 
                                          sizeof(ota_info_t) - sizeof(u32));
  */
  memcpy(&pub->otai, p_otai, sizeof(ota_info_t));

  ss_ctrl_update_public(p_handle);
}

void init_tdi(void *p_data)
{
  s32 ret = 0;
  u8 misc_info[100] = {0};
  misc_options_t *misc = NULL;
  class_handle_t p_handle = class_get_handle_by_id(DM_CLASS_ID);
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  
  ret = dm_read(p_handle, MISC_OPTION_BLOCK_ID, 
                              0, 0, sizeof(misc_options_t), misc_info);
  
  MT_ASSERT(ret != ERR_FAILURE);
  
  misc = (misc_options_t *)misc_info;
  memcpy((u8 *)&p_ota_impl_data->ota_tdi, 
              (u8 *)&misc->ota_tdi, sizeof(ota_tdi_t));  
}

u32 get_oui(void *p_data)
{
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  return p_ota_impl_data->ota_tdi.oui;
}

u16 get_manufact_id(void *p_data)
{
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  return p_ota_impl_data->ota_tdi.manufacture_id;
}

u16 get_hw_mod_id(void *p_data)
{
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  return p_ota_impl_data->ota_tdi.hw_mod_id;
}

u16 get_hw_ver(void *p_data)
{
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  return p_ota_impl_data->ota_tdi.hw_version;
}

u32 get_sw_mod_id(void *p_data)
{
  ota_impl_data_t *p_ota_impl_data = (ota_impl_data_t *)p_data;
  return p_ota_impl_data->ota_tdi.sw_mod_id;
}

ota_policy_t *construct_ota_policy(void)
{
  ota_policy_t * p_ota_impl = mtos_malloc(sizeof(ota_policy_t));
  MT_ASSERT(p_ota_impl != NULL);
  memset(p_ota_impl, 0, sizeof(ota_policy_t));

  p_ota_impl->ota_crc_init = ota_crc_init;
  p_ota_impl->ota_crc32_generate = ota_crc32_generate;
  p_ota_impl->write_otai = write_otai;
  p_ota_impl->init_tdi = init_tdi;
  p_ota_impl->get_oui = get_oui;
  p_ota_impl->get_manufact_id = get_manufact_id;
  p_ota_impl->get_hw_mod_id = get_hw_mod_id;
  p_ota_impl->get_hw_ver = get_hw_ver;
  p_ota_impl->get_sw_mod_id  = get_sw_mod_id;
  
  p_ota_impl->p_data = mtos_malloc(sizeof(ota_impl_data_t));
  MT_ASSERT(p_ota_impl->p_data != NULL);
  
  memset(p_ota_impl->p_data, 0, sizeof(ota_impl_data_t));
  return p_ota_impl;
}

void destruct_ota_policy(ota_policy_t *p_ota_impl)
{
  //Free private data
  mtos_free(p_ota_impl->p_data);

  //Free implement policy
  mtos_free(p_ota_impl);
}


