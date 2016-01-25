/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "lib_util.h"
#include "mem_manager.h"

#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "db_dvbs.h"
#include "ap_satcodx_common.h"
#include "ap_framework.h"
#include "ap_satcodx.h"

#define SATCODX_RECV_BUF_MAX_SIZE  (2 * 1024 * 1024)
#define SATCODX_RECV_BUF_OFFSET (0x1000)
#define SATCODX_WAIT_TIMEOUT (20)
#define SATCODX_RECV_TIMEOUT (2)
#define SATCODX_COM_DURATION_1_SECOND (1000)
#define SATCODX_NOTIFY_PER_SIZE (1024)
#define SATCODX_NOTIFY_PER_CNT (5)

void ap_satcodx_impl_init(void *p_data)
{
  satcodx_impl_data_t *p_satcodx_impl_data = (satcodx_impl_data_t*)p_data;

  p_satcodx_impl_data->recv_buf_offset = SATCODX_RECV_BUF_OFFSET;
  p_satcodx_impl_data->recv_buf_max_size = SATCODX_RECV_BUF_MAX_SIZE;
  p_satcodx_impl_data->max_buf_size = p_satcodx_impl_data->recv_buf_offset + 
                                         p_satcodx_impl_data->recv_buf_max_size;
  p_satcodx_impl_data->mem_partition_id = MEM_SYS_PARTITION;

  p_satcodx_impl_data->wait_host_start_timeout = SATCODX_WAIT_TIMEOUT;
  p_satcodx_impl_data->recv_host_stop_timeout = SATCODX_RECV_TIMEOUT;
  p_satcodx_impl_data->duration_per_second = SATCODX_COM_DURATION_1_SECOND;
  p_satcodx_impl_data->notify_per_size = SATCODX_NOTIFY_PER_SIZE;
}


void ap_satcodx_impl_deinit(void *p_data)
{
  //Dummy
  MT_ASSERT(p_data != NULL);
}

satcodx_policy_t *construct_satcodx_policy(void)
{
  satcodx_policy_t *p_policy = mtos_malloc(sizeof(satcodx_policy_t));
  MT_ASSERT(p_policy != NULL);

  p_policy->on_satcodx_init   = ap_satcodx_impl_init;
  p_policy->on_satcodx_deinit = ap_satcodx_impl_deinit;  
  p_policy->on_satcodx_start   = ap_satcodx_start;
  p_policy->on_satcodx_stop  = ap_satcodx_stop;

  p_policy->p_data = mtos_malloc(sizeof(satcodx_impl_data_t));
  MT_ASSERT(p_policy->p_data != NULL);
  
  memset(p_policy->p_data, 0, sizeof(satcodx_impl_data_t));
  return p_policy;
}

void destruct_satcodx_policy(satcodx_policy_t *p_policy)
{
  //Free private data
  mtos_free(p_policy->p_data);
  
  //Free implement policy
  mtos_free(p_policy);
}