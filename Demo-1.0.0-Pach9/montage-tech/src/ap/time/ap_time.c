/******************************************************************************/
/******************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "nim.h"
#include "lib_util.h"

#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "tdt.h"
#include "mt_time.h"

#include "ap_framework.h"
#include "ap_time.h"
#include "ap_time_default.h"

#ifdef PRINT_ON
//  #define TIME_DEBUG
#endif

#ifdef TIME_DEBUG
#define TIME_DBG OS_PRINTF
#else
#define TIME_DBG(x) DUMMY_PRINTF
#endif


/*!
  Time private information
  */
typedef struct
{
  /*!
    Time instance
    */
  app_t time_instance;

  /*!
    DVB service handle
    */
  service_t *p_svc;
  /*!
    policy
    */
  time_policy_t *p_policy;
  /*!
    Tdt found or not
    */
  BOOL is_tdt_found;
  /*!
    is send heart beat
    */
  BOOL is_send_heart;
  /*!
    old ticks
    */
  u32 old_tick;    
} time_priv_t;

static void time_single_step(void *p_handle, os_msg_t *p_msg)
{
  time_priv_t *p_priv_data = (time_priv_t *)p_handle;
  utc_time_t time = {0};
  u32 curn_tick = 0;
  event_t evt = {0};
  
  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
      case TIME_CMD_START:
        break;
        
      case TIME_CMD_STOP:
        break;

      case TIME_CMD_OPEN_HEART:
        p_priv_data->is_send_heart = TRUE;
        break;

      case TIME_CMD_CLOSE_HEART:
        p_priv_data->is_send_heart = FALSE;
        break;
        
      case DVB_TDT_FOUND:
        time.year = (p_msg->para1 >> 16);
        time.month = (p_msg->para1 & 0x0000FF00) >> 8;
        time.day = (p_msg->para1 & 0x000000FF);
        time.hour = (p_msg->para2 & 0x00FF0000) >> 16;
        time.minute = (p_msg->para2 & 0x0000FF00) >> 8;
        time.second = (p_msg->para2 & 0x000000FF);
        p_priv_data->p_policy->on_time_tdt_found((u32)&time, 0);
        break;

      case TIME_CMD_LNB_CHECK_ENABLE:
        p_priv_data->p_policy->on_time_lnb_check_enable(
          (u32)(p_priv_data->p_policy->p_data), p_msg->para1);
        break;
        
      default:
        break;
    }
  }
  else
  {
    if(p_priv_data->p_policy->on_time_update_check(
      (u32)(p_priv_data->p_policy->p_data), 0))
    {
      evt.id = TIME_EVT_TIME_UPDATE;
      ap_frm_send_evt_to_ui(APP_TIME, &evt);
    }

    p_priv_data->p_policy->on_time_lnb_check(
      (u32)(p_priv_data->p_policy->p_data), 0);
  }

  if(p_priv_data->p_policy->on_time_heart_beat != NULL
  && p_priv_data->is_send_heart)
  {
    curn_tick = mtos_ticks_get();
    
    if((curn_tick - p_priv_data->old_tick) >= 10)
    {
      p_priv_data->old_tick = curn_tick;
      
      evt.id = TIME_EVT_HEART_BEAT;
      
      ap_frm_send_evt_to_ui(APP_TIME, &evt);
    }
  }
}


/*!
   The init operation will be invoked once when system power up
  */
static void init(void *p_handle)
{
  time_priv_t *p_time_priv = (time_priv_t *)p_handle;
  dvb_request_t tdt_req = {0};
  dvb_t *p_dvb_handle = NULL;
  dvb_register_t eit_reg_param = {0};
  
  p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  MT_ASSERT(p_dvb_handle != NULL);

  p_time_priv->p_svc = p_dvb_handle->get_svc_instance(p_dvb_handle, APP_TIME);

  //register tdt table by periodic mode
  eit_reg_param.free_proc = NULL;
  eit_reg_param.parse_proc = parse_tdt;
  eit_reg_param.request_proc = request_tdt_section;
  eit_reg_param.req_mode = DATA_PERIODIC;
  eit_reg_param.table_id = DVB_TABLE_ID_TDT;
  p_dvb_handle->register_table(p_dvb_handle, &eit_reg_param);  

  //Register TDT by single data mode
  eit_reg_param.req_mode = DATA_SINGLE;
  p_dvb_handle->register_table(p_dvb_handle, &eit_reg_param);  
  
  //request tdt table  
  tdt_req.para1 = DVB_TABLE_ID_TDT;
  tdt_req.para2 = 0;
  tdt_req.req_mode = DATA_PERIODIC;
  tdt_req.table_id = DVB_TABLE_ID_TDT;
  tdt_req.period = 5000;
  p_time_priv->p_svc->do_cmd(p_time_priv->p_svc, DVB_REQUEST, (u32)&tdt_req,
    sizeof(dvb_request_t));
}

app_t *construct_ap_time(time_policy_t *p_policy)
{
  time_priv_t *p_time_priv = mtos_malloc(sizeof(time_priv_t));

  MT_ASSERT(p_time_priv != NULL);

  memset(p_time_priv, 0, sizeof(time_priv_t));

  //Attach Time instance
  p_time_priv->time_instance.init = init;
  p_time_priv->time_instance.task_single_step = time_single_step;
  p_time_priv->time_instance.p_data = (void *)p_time_priv;

  p_time_priv->is_send_heart = TRUE;
  p_time_priv->is_tdt_found = FALSE;
  if(p_policy)
  {
    p_time_priv->p_policy = p_policy;
  }
  else
  {
    p_time_priv->p_policy = construct_time_policy_defautl();
  }
  return &(p_time_priv->time_instance);
}

