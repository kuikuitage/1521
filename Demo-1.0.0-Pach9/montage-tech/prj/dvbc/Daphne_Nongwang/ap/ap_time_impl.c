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
#include "nit.h"
#include "pmt.h"
#include "cat.h"

#include "data_base.h"
#include "data_manager.h"
#include "db_dvbs.h"

#include "ap_framework.h"
#include "ap_time.h"
#include "mt_time.h"
#include "mtos_misc.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "ap_ota.h"
#include "sys_status.h"

#ifdef WIN32_DEBUG
#define time_dbg(x)   (OS_PRINTF x)
#else
#define time_dbg(x)    do{}while(0);
#endif

/*!
  Ap Time impl private data
  */
typedef struct impl_data_tag
{
  /*!
    old time, for time check.
    */
  utc_time_t old_time;
  /*!
    last ticks.
    */
  u32 last_tick;
  /*!
    LNB check enable.
    */
  BOOL lnb_check;
}time_impl_data_t;

static void ap_time_tdt_found(u32 para1, u32 para2)
{
  time_set_t t_set;
  utc_time_t time;

  sys_status_get_time(&t_set);

  if(t_set.gmt_usage)//gmt useage is on
  {
    //time.value = ((utc_time_t *)&para1)->value;
    memcpy(&time, ((utc_time_t *)para1), sizeof(utc_time_t));

    
    //printf_time(&time, "##@@tdt found");
    time_set(&time);
  }
}

static BOOL ap_check_time_update(u32 para1, u32 para2)
{
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;

  utc_time_t new_time = {0};
  // use local time
  time_get(&new_time, FALSE);

  if(time_cmp(&(p_time_impl_data->old_time), &new_time, FALSE) != 0)
  {
    p_time_impl_data->old_time = new_time;
    return TRUE;
  }

  return FALSE;
}

static void ap_time_lnb_check_enable(u32 para1, u32 para2)
{
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;

  OS_PRINTF("enable check lnb[%d]\n", para2);
  p_time_impl_data->lnb_check = (BOOL)para2;
}

static void ap_time_lnb_check(u32 para1, u32 para2)
{
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;
  nim_device_t *dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);  
  event_t evt = {TIME_EVT_LNB_SHORT};
  u32 param = NIM_LNB_SC_NO_PROTING;
  u32 cur_tick = mtos_ticks_get();

  if((p_time_impl_data->lnb_check == TRUE) && 
    ((cur_tick - p_time_impl_data->last_tick) >= 50))
  {
//    OS_PRINTF("check lnb\n");
    dev_io_ctrl(dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);

    p_time_impl_data->last_tick = cur_tick;

    if(param == NIM_LNB_SC_PROTING)
    {
      ap_frm_send_evt_to_ui(APP_TIME, &evt);      
    }
  }
}

#ifndef ROLL_TITLE
static void ap_time_heart_beat(u32 para1, u32 para2)
{

}
#endif

time_policy_t *construct_time_policy(void)
{
  time_policy_t *p_time_impl = mtos_malloc(sizeof(time_policy_t));
  MT_ASSERT(p_time_impl != NULL);
  memset(p_time_impl, 0, sizeof(time_policy_t));

  p_time_impl->on_time_tdt_found  = ap_time_tdt_found;
  p_time_impl->on_time_update_check = ap_check_time_update;
  p_time_impl->on_time_lnb_check = ap_time_lnb_check;
  p_time_impl->on_time_lnb_check_enable = ap_time_lnb_check_enable;

#ifndef ROLL_TITLE
  p_time_impl->on_time_heart_beat = ap_time_heart_beat;
#endif

  p_time_impl->p_data = mtos_malloc(sizeof(time_impl_data_t));
  MT_ASSERT(p_time_impl->p_data != NULL);
  memset(p_time_impl->p_data, 0, sizeof(time_impl_data_t));
  
  return p_time_impl;
}