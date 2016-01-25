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
#include "mtos_printk.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"
#include "uio.h"
#include "smc_op.h"
#include "lib_util.h"
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
// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "db_dvbs.h"
#include"config_prj.h"
#include "config_cas.h"

//#define WRITE_DATA_TO_MEM
extern u16 sys_status_get_curn_group_curn_prog_id(void);
extern u8 ui_dbase_get_pg_view_id(void);
void init_ca_module(cas_module_id_t cam_id)
{
  if (cam_id == CAS_UNKNOWN)
  {
    cas_module_init(AP_CAS_ID);
  }
  else
  {
    cas_module_init(cam_id);
  }
}

static void cas_performance(cas_step_t step, u32 ticks)
{
  OS_PRINTF("ap cas cur step %d, tick%d\n", step, ticks);
}

static void pmt_parse_descriptor(u8 *buf, s16 length, u16 es_pid, cas_desc_t *p_ecm_info,
                                      u16 *p_max, u16 *p_index)
{
  u8 *p_data = buf;
  u8 descriptor_tag = 0;
  u8 descriptor_length = 0;

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];

    if(0 == descriptor_length)
    {
      break;
    }
    if (*p_index >= *p_max)
    {
      break;
    }
    length -= (descriptor_length + 2);
    p_data += 2;
    switch(descriptor_tag)
    {
      case CA_DESC_ID:
      {
        p_ecm_info[*p_index].es_pid = es_pid;
        p_ecm_info[*p_index].ca_sys_id = p_data[0]<<8 |p_data[1];
        p_ecm_info[*p_index].ecm_pid = (p_data[2]&0x1F)<<8|p_data[3];

        OS_PRINTF("[cas] es pid %x, ca sys %x, ecm %x\n", p_ecm_info[*p_index].es_pid,
                    p_ecm_info[*p_index].ca_sys_id, p_ecm_info[*p_index].ecm_pid);
        (*p_index)++;
      }
      break;
      default:
        break;
    }
    p_data = p_data + descriptor_length;
  }
}

static BOOL cas_update_ecm_info(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max)
{
  s16 section_length = 0;
  u16 program_number = 0;
  u16 program_info_length = 0;

  u8  stream_type = 0;
  u16 elementary_pid = 0;
  u8 es_info_length = 0;
  u8 *p_data = NULL;
  u16 index = 0;
  p_data = p_pmt_data;

  if(MASK_FIRST_BIT_OF_BYTE(p_data[5]) == 0 || p_data[0] != DVB_TABLE_ID_PMT)
  {
    return FALSE;
  }

  if(NULL != p_data)
  {
    section_length = (((u16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
    if(section_length > 0)
    {
      program_number = (u16)p_data[3] << 8 | p_data[4];

      program_info_length = (u16)(p_data[10] << 8 | p_data[11]) & 0x0FFF;
      p_data += 12;
      pmt_parse_descriptor(p_data, program_info_length,0, p_ecm_info, p_max, (u16 *)&index);
      p_data += program_info_length;
      section_length -= (13 + program_info_length);//head and crc

      while(section_length > 0)
      {
          stream_type = p_data[0];
          elementary_pid = ((u16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
          OS_PRINTF("[cas] stream_type=0x%x, elementary_PID=0x%x\n", stream_type, elementary_pid);
          es_info_length = ((u16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
          p_data += 5;
          pmt_parse_descriptor(p_data, es_info_length,elementary_pid, p_ecm_info, p_max, (u16 *)&index);
          p_data += es_info_length;
          section_length -= (es_info_length + 5);
      }
    }
  }
  else
  {
    return FALSE;
  }

  *p_max = index;
  //update pg info
  {
    u16 view_count = 0, pg_id = 0;
    u16 cur_pg_id = 0;
    u8 view_id;
    u8 i = 0;
    dvbs_prog_node_t pg = {0};
    db_dvbs_ret_t db_ret = DB_DVBS_OK;
    cur_pg_id = sys_status_get_curn_group_curn_prog_id();

    if((db_dvbs_get_pg_by_id(cur_pg_id, &pg) == DB_DVBS_OK) && (pg.s_id == program_number))
    {
      if ((pg.ecm_num != index) || (memcmp(pg.cas_ecm, p_ecm_info,sizeof(cas_desc_t) * index)))
      {
        pg.ecm_num = index;
        memcpy(&pg.cas_ecm, p_ecm_info, sizeof(cas_desc_t) * index);

        db_ret = db_dvbs_edit_program(&pg);
        MT_ASSERT(db_ret == DB_DVBS_OK);
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      view_id = ui_dbase_get_pg_view_id();
      view_count = db_dvbs_get_count(view_id);
      for(i = 0; i < view_count; i++)
      {
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
        if((db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK) && (pg.s_id == program_number))
        {
          if ((pg.ecm_num != index) || (memcmp(pg.cas_ecm, p_ecm_info,sizeof(cas_desc_t) * index)))
          {
            pg.ecm_num = index;
            memcpy(&pg.cas_ecm, p_ecm_info, sizeof(cas_desc_t) * index);

            db_ret = db_dvbs_edit_program(&pg);
            MT_ASSERT(db_ret == DB_DVBS_OK);
            return TRUE;
          }
          else
          {
            return FALSE;
          }
        }
      }
    }
  }
  return FALSE;
}

cas_policy_t* construct_cas_policy(void)
{
  cas_policy_t *p_policy = mtos_malloc(sizeof(cas_policy_t));
  MT_ASSERT(p_policy != NULL);
  memset(p_policy, 0, sizeof(cas_policy_t));

  p_policy->on_init = on_cas_init;
  p_policy->test_ca_performance = cas_performance;
  #if ((CONFIG_CAS_ID == CONFIG_CAS_ID_SHIDA) || (CONFIG_CAS_ID == CONFIG_CAS_ID_TF)||(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  p_policy->init_ca_module = init_ca_module;
  #endif
  p_policy->update_ecm_info = cas_update_ecm_info;
  return p_policy;
}


