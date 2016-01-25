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
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"


// driver
#include "lib_util.h"
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

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "monitor_service.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "cas_ware.h"
#include "Data_manager.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "db_dvbs.h"
#include "ss_ctrl.h"
#include "sys_status.h"
#include "config_customer.h"
#include"config_prj.h"
#include "config_cas.h"
#include "sys_dbg.h"


//#define WRITE_DATA_TO_MEM
static u8 *p_ca_buffer = NULL;
extern u32 get_flash_addr(void);

//cadata_block_id: CADATA1_BLOCK_ID or CADATA2_BLOCK_ID
static RET_CODE nvram_read(u32 cadata_block_id, u8 *p_buf, u32 *size)
{
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(dm_handle != NULL);

  if(cadata_block_id == 1)
  {
    ret = dm_direct_read(dm_handle, CADATA1_BLOCK_ID, 0, *size, p_buf);
  }
  else if(cadata_block_id == 2)
  {
    ret = dm_direct_read(dm_handle, CADATA2_BLOCK_ID, 0, *size, p_buf);
  }
  OS_PRINTF("nvram_read success!\n");
  return SUCCESS;
}

#if 1
static RET_CODE nvram_write(u32 cadata_block_id, u8 *p_buf, u32 size)
{
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  //u32 base_addr = 0;
 // u32 padding = 0;
  //u32 wrlen = 0;
  u8 block_id = CADATA1_BLOCK_ID;
  DEBUG(MAIN,INFO,"cadata_block_id=%d\n", cadata_block_id);
  if(cadata_block_id == 1)
  {
    //base_addr =  dm_get_block_addr(dm_handle, CADATA1_BLOCK_ID) - get_flash_addr();
    block_id = CADATA1_BLOCK_ID;
  }
  else if(cadata_block_id == 2)
  {
    //base_addr =  dm_get_block_addr(dm_handle, CADATA2_BLOCK_ID) - get_flash_addr();
    block_id = CADATA2_BLOCK_ID;
  }
  MT_ASSERT(dm_handle != NULL);

  //padding = (base_addr) % CHARSTO_SECTOR_SIZE;
  /*
  while(size)
  {
    if (size > (CHARSTO_SECTOR_SIZE - padding))
    {
      wrlen = CHARSTO_SECTOR_SIZE - padding;
    }
    else
    {
      wrlen = size;
    }
    //read sector data
    //ret = charsto_read(p_charsto_dev, base_addr, p_ca_buffer, CHARSTO_SECTOR_SIZE);
    ret = dm_direct_read(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
    
    if (ret != SUCCESS)
    {
      OS_PRINTF("write/read error1\n");
      return ERR_FAILURE;
    }
    memcpy(p_ca_buffer + padding, p_buf, wrlen);

    //ret = charsto_erase(p_charsto_dev, base_addr, 1);
    ret = dm_direct_erase(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE);
    if (ret != SUCCESS)
    {
      OS_PRINTF("write/erase error2\n");
      return ERR_FAILURE;
    }
    //charsto_writeonly(p_charsto_dev, base_addr, p_ca_buffer, CHARSTO_SECTOR_SIZE);
    ret = dm_direct_write(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
    if (ret != SUCCESS)
    {
      OS_PRINTF("write error3\n");
      return ERR_FAILURE;
    }
    base_addr += CHARSTO_SECTOR_SIZE;
    padding = 0;
    p_buf += wrlen;
    size -= wrlen;
  }
*/

    ret = dm_direct_read(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
    
    memset(p_ca_buffer, 0, CHARSTO_SECTOR_SIZE);

    memcpy(p_ca_buffer, p_buf, CHARSTO_SECTOR_SIZE);

    //ret = charsto_erase(p_charsto_dev, base_addr, 1);
    ret = dm_direct_erase(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE);
    MT_ASSERT(ret != DM_FAIL);

    //charsto_writeonly(p_charsto_dev, base_addr, p_ca_buffer, CHARSTO_SECTOR_SIZE);
    ret = dm_direct_write(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
    MT_ASSERT(ret != DM_FAIL);

  OS_PRINTF("nvram_write success!\n");
  return SUCCESS;

}
#endif

static RET_CODE machine_serial_get(u8 *p_buf, u32 size)
{
  u32 read_len = 0;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID, 0, 12,
                     size, p_buf);
  
  if(read_len > 0)
  {
    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }

}

static void register_monitor_table(void)
{
  m_register_t reg;
  m_svc_t *p_svc = NULL;

  p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  reg.app_id = APP_CA;
  reg.num = 0;
  reg.table[reg.num].t_id = M_PMT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  reg.table[reg.num].t_id = M_CAT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  reg.table[reg.num].t_id = M_NIT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  dvb_monitor_register_table(p_svc, &reg);

}

#if 0
static RET_CODE get_stbid(u8 *p_buf, u32 size)
{
    get_sys_stbid(CUSTOMER_STDID_SIZE,p_buf,size);
    return SUCCESS;
}
#endif

extern RET_CODE cas_yxsb_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

void on_cas_init(void)
{
  u32 cas_id = 0;
  cas_adapter_cfg_t cas_cfg = {0};
  cas_module_cfg_t cas_module_cfg = {0};
  p_ca_buffer = mtos_malloc(CA_DATA_SIZE);
  MT_ASSERT(NULL != p_ca_buffer);
  memset(p_ca_buffer, 0, CA_DATA_SIZE);
  
  //config cas adapter
  cas_cfg.p_smc_drv[0] = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);

  cas_cfg.slot_num = 1;

  cas_cfg.p_task_stack = mtos_malloc(4096);
  cas_cfg.stack_size = 4096;
  cas_cfg.task_prio =  DRV_CAS_ADAPTER_TASK_PRIORITY;
#ifndef WIN32
  cas_init(&cas_cfg);
#endif
  cas_module_cfg.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  cas_module_cfg.nvram_read = nvram_read;
  cas_module_cfg.nvram_write = nvram_write;
  
  cas_module_cfg.flash_start_adr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID)
     - get_flash_addr();
  OS_PRINTF("set ca addr is 0x%x\n", cas_module_cfg.flash_start_adr);
  cas_module_cfg.flash_size = CA_DATA_SIZE;
  cas_module_cfg.machine_serial_get = machine_serial_get;
  cas_module_cfg.task_prio = DRV_CAS_MODUL_TASK_PRIORITY_START;
  cas_module_cfg.end_task_prio = DRV_CAS_MODUL_TASK_PRIORITY_END;
#ifndef WIN32
  cas_yxsb_attach(&cas_module_cfg, &cas_id);
  cas_module_init(CAS_ID_TF);
#endif
  register_monitor_table();
}

