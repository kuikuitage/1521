/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
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

#include "eit.h"
#include "epg_data.h"
#include "epg_util.h"

#include "ap_framework.h"
#include "ap_epg.h"

/*!
  Total partition size of EPG:
  Configurable with different project
  */
#define MAX_NET_NODE_NUM          (10)
#define MAX_TS_NODE_NUM           (50)
#define MAX_SVC_NODE_NUM          (150)
#define MAX_EVT_NODE_NUM          (1200)


//SUPPORT 10 original network
#define NET_NODE_MEM_SIZE         (MAX_NET_NODE_NUM*(sizeof(orig_net_node_t)))          
//Support 50 ts node
#define TS_NODE_MEM_SIZE          (MAX_TS_NODE_NUM*(sizeof(ts_node_t))) 
//Support 256 service node
#define SVC_NODE_MEM_SIZE         (MAX_SVC_NODE_NUM*(sizeof(sev_node_t)))             
#define EVT_NODE_HEAD_MEM_SIZE    (MAX_EVT_NODE_NUM*sizeof(evt_node_t))
#define EVT_NODE_EXT_INFO_SIZE    (170 * 1024)

#define PTI_BUF_UNIT_SIZE        (12 * 5 * KBYTES)


/*!
  EPG implement private data
  */
typedef struct 
{
  /*!
    memf for net node with fixed size
    */
  lib_memf_t net_node_memf;
  /*!
    memf for net node with fixed size
    */
  lib_memf_t ts_node_memf;
  /*!
    memf for net node with fixed size
    */
  lib_memf_t svc_node_memf;
  /*!
    memf for net node with fixed size
    */
  lib_memf_t evt_node_memf;
  /*!
    None fixed memory partition
    */
  lib_memp_t memp;
  /*!
    EPG fifo
    */
  epg_fifo_t epg_fifo; 
  /*!
    Memory share or not
    */
  /*!
    Starting address of buffer unit
    */
  filter_ext_buf_t  *p_ext_buf_list;
  /*!
    PTI buffer number and this value should be more than max PTI number 
    available
    */
  u8 ext_buf_num;
  /*!
    Block id
    */
  u8 block_id;
  /*!
    Block address
    */
  u32 block_addr;
  /*! 
    Block size
    */
  u32 block_size;
}epg_impl_data_t;

static u32 epg_db_mem_cfg(void *p_data, u8 *p_addr)
{
  void *p_epg_handle = class_get_handle_by_id(EPG_CLASS_ID);
  u32 mem_size = 0;  
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  RET_CODE ret_mem = SUCCESS;
  
  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_epg_handle != NULL);
    
  //Create net node memf
  ret_mem = lib_memf_create(&p_impl_data->net_node_memf, (u32)p_addr, 
      NET_NODE_MEM_SIZE, sizeof(orig_net_node_t));
  MT_ASSERT(ret_mem == SUCCESS);
  
  p_addr += NET_NODE_MEM_SIZE;
  mem_size += NET_NODE_MEM_SIZE;
  epg_data_set_net_node_memf(p_epg_handle, &p_impl_data->net_node_memf);

  //Create ts node memf
  ret_mem = lib_memf_create(&p_impl_data->ts_node_memf, (u32)p_addr, 
      TS_NODE_MEM_SIZE, sizeof(ts_node_t));
  MT_ASSERT(ret_mem == SUCCESS);
  
  p_addr += TS_NODE_MEM_SIZE;
  mem_size += TS_NODE_MEM_SIZE;
  epg_data_set_ts_node_memf(p_epg_handle, &p_impl_data->ts_node_memf);

  //Create service node memf
  ret_mem = lib_memf_create(&p_impl_data->svc_node_memf, (u32)p_addr, 
      SVC_NODE_MEM_SIZE, sizeof(sev_node_t));
  MT_ASSERT(ret_mem == SUCCESS);
  
  p_addr += SVC_NODE_MEM_SIZE;
  mem_size += SVC_NODE_MEM_SIZE;
  epg_data_set_svc_node_memf(p_epg_handle, &p_impl_data->svc_node_memf);

  //Create event node memf
  //Create event head memf
  ret_mem = lib_memf_create(&p_impl_data->evt_node_memf, (u32)p_addr, 
      EVT_NODE_HEAD_MEM_SIZE, sizeof(evt_node_t));
  MT_ASSERT(ret_mem == SUCCESS);
  
  p_addr += EVT_NODE_HEAD_MEM_SIZE;
  mem_size += EVT_NODE_HEAD_MEM_SIZE;
  epg_data_set_evt_node_memf(p_epg_handle, &p_impl_data->evt_node_memf);
  
  //Set non-fixed partition for saving txt and desc information
  ret_mem = lib_memp_create(&p_impl_data->memp, (u32)p_addr , 
                             EVT_NODE_EXT_INFO_SIZE);
  MT_ASSERT(ret_mem == SUCCESS);
  epg_data_set_memp(p_epg_handle, &p_impl_data->memp);
  mem_size += EVT_NODE_EXT_INFO_SIZE;

  OS_PRINTF("!!!EPG memory size \n");
  OS_PRINTF("evt memory: %d\n", EVT_NODE_HEAD_MEM_SIZE);
  OS_PRINTF("svc memory: %d\n", SVC_NODE_MEM_SIZE);
  OS_PRINTF("ts memory: %d\n", TS_NODE_MEM_SIZE);
  OS_PRINTF("net memory: %d\n", NET_NODE_MEM_SIZE);
  OS_PRINTF("Non fixed memory: %d\n", EVT_NODE_EXT_INFO_SIZE);
  OS_PRINTF("Total memory size %d\n", (EVT_NODE_HEAD_MEM_SIZE 
                                    + SVC_NODE_MEM_SIZE 
                                    + TS_NODE_MEM_SIZE 
                                    + NET_NODE_MEM_SIZE
                                    + EVT_NODE_EXT_INFO_SIZE));
  return mem_size;
}

static void epg_db_release(void *p_data)
{
  RET_CODE ret = SUCCESS;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  
  MT_ASSERT(p_impl_data != NULL);
  
  //Release net node memf
  ret = lib_memf_destroy(&p_impl_data->net_node_memf);
  MT_ASSERT(ret == SUCCESS);

  //Release ts node memf
  ret = lib_memf_destroy(&p_impl_data->ts_node_memf);
  MT_ASSERT(ret == SUCCESS);

  //Release service node memf
  ret = lib_memf_destroy(&p_impl_data->svc_node_memf);
  MT_ASSERT(ret == SUCCESS);

  //Release event node memf
  ret = lib_memf_destroy(&p_impl_data->evt_node_memf);
  MT_ASSERT(ret == SUCCESS);  
  //Release segment memf

  //Release nonfixed memory
  ret = lib_memp_destroy(&p_impl_data->memp);
  MT_ASSERT(ret == SUCCESS);
}

static void impl_mem_cfg(void *p_data)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  u8 *p_block_addr = NULL;
  u32 block_size = 0;
  u32 mem_size = 0;
  u8 buf_cnt = 0;
  
  p_block_addr = 
          (u8*)mem_mgr_require_block(p_impl_data->block_id,  SYS_MODULE_EPG);
  block_size = mem_mgr_get_block_size(p_impl_data->block_id);
  memset(p_block_addr, 0, block_size);
  
  OS_PRINTF("AP EPG start %x\n", (u32)p_block_addr);
  OS_PRINTF("block size %x\n", block_size);

  
  MT_ASSERT(p_block_addr != NULL);
  mem_mgr_release_block(p_impl_data->block_id);

  //Attach fifo buffer 
  p_impl_data->epg_fifo.p_buffer = (u32*)p_block_addr;
  p_block_addr += p_impl_data->epg_fifo.m_size * sizeof(u32);
  
  //Allocate memory for EPG database
  mem_size = epg_db_mem_cfg(p_data, p_block_addr);
  p_block_addr += mem_size;

  MT_ASSERT(mem_size + PTI_BUF_UNIT_SIZE * p_impl_data->ext_buf_num 
                                                                < block_size);
  //Add PTI buffer into FIFO
  for(buf_cnt = 0; buf_cnt < p_impl_data->ext_buf_num; buf_cnt ++)
  {
    
    p_impl_data->p_ext_buf_list[buf_cnt].p_buf = p_block_addr;
    p_impl_data->p_ext_buf_list[buf_cnt].p_next = NULL;
    p_impl_data->p_ext_buf_list[buf_cnt].size = PTI_BUF_UNIT_SIZE;
    epg_fifo_put(&p_impl_data->epg_fifo,
                          (u32)(&p_impl_data->p_ext_buf_list[buf_cnt]));
    p_block_addr += PTI_BUF_UNIT_SIZE;
  }
}

static void impl_mem_release(void *p_data)
{
  u8 buf_cnt = 0;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  //Clear PTI buffer 
  for(buf_cnt = 0; buf_cnt < p_impl_data->ext_buf_num; buf_cnt ++)
  {
    p_impl_data->p_ext_buf_list[buf_cnt].p_buf = NULL;
    p_impl_data->p_ext_buf_list[buf_cnt].p_next = NULL;
    p_impl_data->p_ext_buf_list[buf_cnt].size = 0;
  }

  //Release EPG data memory
  epg_db_release(p_data);

  //Set fifo buffer address to NULL
  p_impl_data->epg_fifo.p_buffer = NULL;
  //Release block
}

/*!
  Build basic structure for EPG data
  */
static void epg_impl_init(void *p_data, u8 max_pti_num)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_data != NULL);
  
  p_impl_data->ext_buf_num = max_pti_num + 1;
  p_impl_data->p_ext_buf_list = 
              mtos_malloc(sizeof(filter_ext_buf_t) * p_impl_data->ext_buf_num);
  MT_ASSERT(p_impl_data->p_ext_buf_list != NULL);
  
  //Memory alloc for fifo
  p_impl_data->epg_fifo.p_buffer  = NULL;
  p_impl_data->epg_fifo.m_overlay = FALSE;
  p_impl_data->epg_fifo.m_size    = p_impl_data->ext_buf_num;
}

static BOOL epg_impl_start(void *p_data, u8 block_id)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  p_impl_data->block_id = block_id;

  //Flush EPG fifo
  epg_fifo_flush(&p_impl_data->epg_fifo); 
  

    //Memory reconfig
    impl_mem_cfg(p_data);
    //memory has not been configed
    //Memory reconfig
  
  return TRUE;
}

static void epg_impl_stop(void *p_data)
{
  //Release memory partition
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_impl_data != NULL);
  
  epg_data_stop(class_get_handle_by_id(EPG_CLASS_ID));
    impl_mem_release(p_data);
  }


static u32 get_ext_buf(void *p_data)
{
  u32 pti_buf = 0;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_impl_data != NULL);
  
  epg_fifo_get(&p_impl_data->epg_fifo, &pti_buf);
  MT_ASSERT(pti_buf != 0);

  return pti_buf;
}

static void release_ext_buf(void *p_data, u32 p_ext_buf)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_impl_data != NULL);

  MT_ASSERT(p_ext_buf != 0);
  epg_fifo_put(&p_impl_data->epg_fifo, p_ext_buf);
}

epg_policy_t *construct_epg_policy(void)
{
  epg_policy_t    *p_epg_impl = mtos_malloc(sizeof(epg_policy_t));
  epg_impl_data_t *p_imp_data = NULL;
  
  MT_ASSERT(p_epg_impl != NULL);

  p_epg_impl->epg_impl_start = epg_impl_start;
  p_epg_impl->epg_impl_stop  = epg_impl_stop;
  p_epg_impl->epg_impl_init  = epg_impl_init;

  
  p_epg_impl->get_ext_buf = get_ext_buf;
  p_epg_impl->release_ext_buf = release_ext_buf;
  
  p_epg_impl->p_data = mtos_malloc(sizeof(epg_impl_data_t));
  MT_ASSERT(p_epg_impl->p_data != NULL);
  
  memset(p_epg_impl->p_data, 0, sizeof(epg_impl_data_t));
  p_imp_data = (epg_impl_data_t*)p_epg_impl->p_data;
  
  
  return p_epg_impl;
}

