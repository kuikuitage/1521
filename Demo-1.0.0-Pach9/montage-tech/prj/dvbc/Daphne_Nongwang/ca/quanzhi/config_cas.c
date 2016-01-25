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

static u32 nvram_read(cas_module_id_t module_id, u16 node_id, u16 offset, u32 length, u8 *p_buffer)
{
  u32 read_len = 0;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
            node_id, offset, length, p_buffer);
  return read_len;
}

static RET_CODE nvram_write(cas_module_id_t module_id, u16 *p_node_id, u8 *p_buffer, u16 len)
{
  dm_ret_t ret = DM_SUC;
  ret = dm_write_node(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
                  p_node_id, p_buffer, len);
  if (DM_SUC == ret)
  {
    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}

static RET_CODE nvram_del(cas_module_id_t module_id, u16 node_id)
{
  dm_ret_t ret = DM_SUC;
  ret = dm_del_node(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, node_id, TRUE);
  if (DM_SUC == ret)
  {
    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}

static RET_CODE nvram_node_list(cas_module_id_t module_id, u16 *p_buffer, u16 *p_max)
{
  u32 total_num;
  total_num = dm_get_node_id_list(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
                    p_buffer, *p_max);
  *p_max = (u16)total_num;
  return SUCCESS;
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

static RET_CODE get_stbid(u8 *p_buf, u32 size)
{
    get_sys_stbid(CUSTOMER_STDID_SIZE,p_buf,size);
    return SUCCESS;
}
extern RET_CODE cas_gs_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);


void on_cas_init(void)
{
  u32 cas_id = 0;
  cas_adapter_cfg_t cas_cfg = {0};
  cas_module_cfg_t cas_module_cfg = {0};
  p_ca_buffer = mtos_malloc(CA_DATA_SIZE);
  MT_ASSERT(NULL != p_ca_buffer);
  memset(p_ca_buffer, 0, CA_DATA_SIZE);
  DEBUG_ENABLE_MODE(QZCA,INFO);
  //config cas adapter
  cas_cfg.p_smc_drv[0] = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);

  cas_cfg.slot_num = 1;

  cas_cfg.p_task_stack = mtos_malloc(4096);
  cas_cfg.stack_size = 4096;
  cas_cfg.task_prio = MDL_SUBT_TASK_PRIORITY;

  cas_cfg.p_data_task_stack = mtos_malloc(4096);
  MT_ASSERT(NULL != cas_cfg.p_data_task_stack);
  cas_cfg.data_stack_size = 4096;
  cas_cfg.data_task_prio = CUSTOMER_TASK_PRIORITY;
  cas_cfg.nvram_read = nvram_read;
  cas_cfg.nvram_write = nvram_write;
  cas_cfg.nvram_node_list = nvram_node_list;
  cas_cfg.nvram_del = nvram_del;
  
#ifndef WIN32
  cas_init(&cas_cfg);
#endif
  cas_module_cfg.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  cas_module_cfg.nvram_read = NULL;
  cas_module_cfg.nvram_write = NULL;
  cas_module_cfg.max_mail_num = 30;
  cas_module_cfg.mail_policy = POLICY_BY_ID;
  cas_module_cfg.flash_start_adr = 0;
  OS_PRINTF("set ca addr is 0x%x\n", cas_module_cfg.flash_start_adr);
  cas_module_cfg.flash_size = 0;
  cas_module_cfg.machine_serial_get = get_stbid;

  cas_module_cfg.area_limit_free = AREA_LIMIT_FREE;  /**area_limit_free : TRUE:free FALSE:limit***/

#ifndef WIN32
  cas_gs_attach(&cas_module_cfg, &cas_id);
  cas_module_init(CAS_ID_GS);
#endif
  register_monitor_table();
}

