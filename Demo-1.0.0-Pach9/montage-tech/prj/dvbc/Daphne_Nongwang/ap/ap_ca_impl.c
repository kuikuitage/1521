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
#include "ca_svc.h"

// ap headers
#include "ap_framework.h"
#include "ap_ca.h"
#include "ca_svc.h"



static u32* p_multi_ecm_buf = NULL;

static service_t *g_p_ca_svc = NULL;

static u8 table_repeat = 0;

u32 mtos_ticks_get(void);

void reqeust_one_ecm(u16 table_id, u16 pid, u32 time_out, 
  u16 pgid, u16 index, u8 *p_filter_code, u8 *p_filter_mask)
{
  dvb_request_t dvb_req = {0};
  //int i;

  if(!p_multi_ecm_buf)
  {
  	p_multi_ecm_buf = mtos_malloc(1024*40);
  }
  
  dvb_req.table_id = DVB_TABLE_ID_ECM;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = pid;
  dvb_req.para2 = time_out;
  dvb_req.context = MAKE_REQUEST_CONTEXT(pgid, index, table_repeat);

  memcpy(dvb_req.filter_code, p_filter_code, MAX_FILTER_MASK_BYTES-4);
  memcpy(dvb_req.filter_mask, p_filter_mask, MAX_FILTER_MASK_BYTES-4);

  #if 0
  UI_PRINTF("\n CA:  ECM MASK  pid [0x%x]\n ", pid);
  for(i=0;i < 8; i++)
  {
  	UI_PRINTF("%02x ",p_filter_code[i]);
  }
  UI_PRINTF("\n ");
  for(i=0;i < 8; i++)
  {
  	UI_PRINTF("%02x ",p_filter_mask[i]);
  }
  UI_PRINTF("\n ");
  #endif
  
  g_p_ca_svc->do_cmd(g_p_ca_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  UI_PRINTF("CA: request ecm pgid %d, index %d, t %d\n", pgid, index, mtos_ticks_get());  
}

void request_one_emm(u16 table_id, u16 pid, u32 time_out, 
  u16 pgid, u16 index, u8 *p_filter_code, u8 *p_filter_mask)
{
  dvb_request_t dvb_req = {0};
  //int i;
  
  #if 0
  UI_PRINTF("\n CA: request_one_emm  table[%d] pid[%d] \n ", table_id, pid);
  for(i=0;i < 8; i++)
  {
  	UI_PRINTF("%02x ",p_filter_code[i]);
  }
  UI_PRINTF("\n ");
  for(i=0;i < 8; i++)
  {
  	UI_PRINTF("%02x ",p_filter_mask[i]);
  }
  UI_PRINTF("\n ");
  #endif
  
  dvb_req.table_id = DVB_TABLE_ID_EMM;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = pid;
  dvb_req.para2 = time_out;
  dvb_req.context = MAKE_REQUEST_CONTEXT(pgid, index, table_repeat);
  memcpy(dvb_req.filter_code, p_filter_code, MAX_FILTER_MASK_BYTES-4);
  memcpy(dvb_req.filter_mask, p_filter_mask, MAX_FILTER_MASK_BYTES-4);
  g_p_ca_svc->do_cmd(g_p_ca_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  UI_PRINTF("CA: request emm pgid %d, index %d, t %d\n", pgid, index, mtos_ticks_get());
}

static s16 on_pmt(pmt_t *p_pmt, u32 context)
{
  UI_PRINTF("CA: on_pmt: pmt[0] = 0x%x\n", *((u8 *)(p_pmt)));

  ca_svc_notify_event(CA_SVC_EVT_PMT_FOUND, context, (u8 *)p_pmt->p_origion_data, MAX_SECTION_LENGTH);
  return 0;
}

static s16 on_cat(cat_t *p_cat, u16 sid, u32 context)
{
  UI_PRINTF("CA: on_cat: cat[0] = 0x%x\n", *((u8 *)(p_cat)));
  context = ((context & 0xFFFF0000) | sid);
  ca_svc_notify_event(CA_SVC_EVT_CAT_FOUND, context, (u8 *)p_cat, MAX_SECTION_LENGTH);
  return 0;
}

static s16 on_emm(u8 *p_buf, u32 len, u32 context)
{
  ca_svc_notify_event(CA_SVC_EVT_EMM_FOUND, context, p_buf, len);
  return 0;
}

static s16 on_ecm(u8 *p_buf, u32 len, u32 context)
{
  ca_svc_notify_event(CA_SVC_EVT_ECM_FOUND, context, p_buf, len);
  return 0;
}

static s16 on_timeout(u16 table_id, u32 context)
{
  switch(table_id)
  {
  case DVB_TABLE_ID_ECM:
    break;
    
  case DVB_TABLE_ID_EMM:
    break;

  default:
	  UI_PRINTF("CA: default tableid =0x%x  context = %d timedout \n", table_id, context);
	  MT_ASSERT(0);
    break;
  }
  
  return 0;
}


static s16 on_reset_sid(u8 repeat_times)
{
  //todo....
  ca_svc_empty_msgq();

  table_repeat = repeat_times;
  return 0;
}

static void on_init(service_t *p_svc)
{
  g_p_ca_svc = p_svc;
}

ca_policy_t* construct_ca_policy(void)
{
  ca_policy_t *p_policy = mtos_malloc(sizeof(ca_policy_t));
  MT_ASSERT(p_policy != NULL);
  p_policy->on_pmt = on_pmt;
  p_policy->on_cat = on_cat;
  p_policy->on_emm = on_emm;
  p_policy->on_ecm = on_ecm;
  p_policy->on_timeout = on_timeout;
  p_policy->on_init = on_init;
  p_policy->on_reset_sid = on_reset_sid;
  
  return p_policy;
}

