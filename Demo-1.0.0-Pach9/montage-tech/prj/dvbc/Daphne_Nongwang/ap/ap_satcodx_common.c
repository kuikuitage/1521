/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"

#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "mem_manager.h"

#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "pmt.h"
#include "db_dvbs.h"
#include "ap_satcodx_common.h"
#include "ap_framework.h"
#include "ap_satcodx.h"
#include "ap_satcodx_recv.h"
/*!
  \file ap_satcodx_common.c
  This is the implementation of basic common part used in all other satcodx module.  
*/

#define SATCODX_ALIGN_BYTE_CNT 4
#define SATCODX_ALIGN_MASK 0xFFFFFFFC

void* satcodx_sys_malloc(satcodx_impl_data_t *p_satcodx_impl_data, u32 size)
{
  mem_mgr_alloc_param_t alloc_t = {0};
  alloc_t.id = p_satcodx_impl_data->mem_partition_id;
  alloc_t.size = size;
  alloc_t.user_id = SYS_MODULE_SATCODX;
  OS_PRINTF("M: satcodx alloc size = %d\n", size);
  return MEM_ALLOC(&alloc_t);
}

void  satcodx_sys_free(satcodx_impl_data_t *p_satcodx_impl_data, void *ptr)
{
  mem_mgr_free_param_t free = {0};
  free.id =  p_satcodx_impl_data->mem_partition_id;
  free.p_addr = ptr;
  free.user_id = SYS_MODULE_SATCODX;
  OS_PRINTF("M: satcodx free addr = 0x%x\n", ptr);
  FREE_MEM(&free);
}

void* satcodx_block_malloc(satcodx_impl_data_t *p_satcodx_impl_data, u32 *p_size)
{
  u32 p_addr = mem_mgr_require_block(
                p_satcodx_impl_data->mem_block_id,
                SYS_MODULE_SATCODX);  
  MT_ASSERT(p_addr != 0);  
  mem_mgr_release_block(p_satcodx_impl_data->mem_block_id);
  
  *p_size = mem_mgr_get_block_size(p_satcodx_impl_data->mem_block_id);  
  return (void*)p_addr;
}

void  satcodx_block_free(satcodx_impl_data_t *p_satcodx_impl_data, void *ptr)
{
}

void *satcodx_static_malloc(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       u32 size)
{
  void *ptr;
  if(size%SATCODX_ALIGN_BYTE_CNT)
  {
    size += SATCODX_ALIGN_BYTE_CNT;
    size &= SATCODX_ALIGN_MASK;
  }
  if(p_satcodx_impl_data->cur_static_buf_left_size >= size)
  {
    p_satcodx_impl_data->cur_static_buf_left_size -= size;
    ptr = p_satcodx_impl_data->p_cur_pos_of_static_buf;
    p_satcodx_impl_data->p_cur_pos_of_static_buf += size;
  }
  else
  {
    ptr = NULL;
  }
  return ptr;
}

void satcodx_static_free(satcodx_impl_data_t *p_satcodx_impl_data, void *ptr)
{
  return ;
}

void ap_satcodx_notify_ui(u32 evt_id, u32 para1, u32 para2)
{
  event_t evt = { 0 };
  evt.id = evt_id ;
  evt.data1 = para1;
  evt.data2 = para2;

  ap_frm_send_evt_to_ui(APP_SATCODX, &evt);
}

/*!
  Start satcodx app
  */
BOOL ap_satcodx_start(void *p_data, u32 mem_block_id)
{
  satcodx_impl_data_t *p_impl_data = (satcodx_impl_data_t*)p_data;  
  if(p_impl_data->sm != SATCODX_SM_IDLE)
  {
    return FALSE;
  }
  
  p_impl_data->mem_block_id = mem_block_id;  
  ap_satcodx_recv_start(p_impl_data);
  return TRUE;
}

/*!
  Stop satcodx app
  */
BOOL ap_satcodx_stop(void *p_data)
{
  satcodx_impl_data_t *p_impl_data = (satcodx_impl_data_t*)p_data;
  p_impl_data->sm = SATCODX_SM_IDLE;
  ap_satcodx_recv_stop(p_impl_data);
  ap_satcodx_notify_ui(SATCODX_EVT_END,0, 0);
  return TRUE;
}

