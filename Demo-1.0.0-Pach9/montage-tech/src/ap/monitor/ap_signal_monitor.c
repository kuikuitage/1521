/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

#include "lib_util.h"

#include "drv_dev.h"
#include "uio.h"
#include "nim.h"
#include "class_factory.h"
#include "service.h"
#include "mdl.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ap_framework.h"
#include "ap_signal_monitor.h"

/*!
  Signal monitor private data
  */
typedef struct
{
  /*!
    Signal monitor start tick
    */
  u32 start_tick;
  /*!
    Signal monitor performed flag
    */
  BOOL monitor_perf;
  /*!
    NC service handle
    */
  service_t *p_nc_svc;
  /*!
    NC monitor info
    */
  sig_mon_info_t info;
} signal_mon_data_t;

/*!
  Private info for signal monitor APP.
  */
typedef struct 
{
  /*!
    signal monitor private data
    */
  signal_mon_data_t *p_data;
  /*!
    Signal monitor implement policy
    */
  sig_mon_policy_t *p_sig_mon_impl;
  /*!
    Signal monitor instance
    */
  app_t instance;
} sig_mon_priv_t;

static void sf_send_evt(sig_mon_evt_t s_evt, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = s_evt;
  evt.data1 = data1;
  evt.data2 = data2;
  ap_frm_send_evt_to_ui(APP_SIGNAL_MONITOR, &evt);
}

static void init(handle_t p_handle)
{
  sig_mon_priv_t *p_priv = (sig_mon_priv_t *)p_handle;
  nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);

  p_priv->p_data->p_nc_svc = nc_get_svc_instance(p_nc, APP_SIGNAL_MONITOR);
}

static BOOL is_valid_tp(dvbs_lock_info_t *p_info, sys_signal_t lock_mode)
{
  u32 lock_freq = 0;
  
  if(lock_mode == SYS_DVBS)
  {
    lock_freq = dvbs_calc_mid_freq(&p_info->tp_rcv, &p_info->sat_rcv);
    if((lock_freq > 2150 * KHZ) || (lock_freq < 950 * KHZ))
    {
      return FALSE;
    }
  }
  else if(lock_mode == SYS_DVBT)
  {

  }
  else if(lock_mode == SYS_DVBC)
  {

  }
  else
  {
    MT_ASSERT(0);
  }
  return TRUE;
}

static void set_freq_lock(sig_mon_priv_t *p_priv, dvbs_lock_info_t *p_info, sys_signal_t lock_mode)
{
  nc_channel_info_t search_info = {0};
  sat_rcv_para_t *p_sat_rcv = &p_info->sat_rcv;
  tp_rcv_para_t  *p_tp_rcv  = &p_info->tp_rcv;
  service_t *p_nc_svc = p_priv->p_data->p_nc_svc;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  if(lock_mode == SYS_DVBS)
  {
    nc_set_diseqc(nc_handle, &p_info->disepc_rcv);
    nc_set_12v(nc_handle, (u8)p_sat_rcv->v12);
    
    if(FALSE == is_valid_tp(p_info, lock_mode))  //pass invalid parameter
    {
      search_info.channel_info.frequency = 900 * KHZ;//p_sat_rcv->lnb_low;
      search_info.channel_info.param.dvbs.symbol_rate = p_tp_rcv->sym;
    }
    else
    {
      dvbs_calc_search_info(&search_info, p_sat_rcv, p_tp_rcv);
    }
  }
  else if(lock_mode == SYS_DVBT)
  {
    //to do...
    search_info.channel_info.frequency = p_tp_rcv->freq;
    search_info.channel_info.param.dvbc.symbol_rate = p_tp_rcv->sym;
    search_info.channel_info.param.dvbc.modulation = p_tp_rcv->nim_modulate;
  }
  else if(lock_mode == SYS_DVBC)
  {
    search_info.channel_info.frequency = p_tp_rcv->freq;
    search_info.channel_info.param.dvbc.symbol_rate = p_tp_rcv->sym;
    search_info.channel_info.param.dvbc.modulation = p_tp_rcv->nim_modulate;
    search_info.polarization = NIM_PORLAR_HORIZONTAL;  
    search_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
    search_info.channel_info.lock = NIM_CHANNEL_UNLOCK;      
  }
  else
  {
    MT_ASSERT(0);
  }

  nc_set_tp(nc_handle, p_nc_svc, &search_info);
}


static u32 get_signal_info(sig_mon_priv_t *p_priv)
{
  nim_channel_perf_t signal = {0};
  
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  
  nc_perf_get(nc_handle, &signal);

  p_priv->p_sig_mon_impl->p_convert_perf(&signal, &p_priv->p_data->info);

  
  //memcpy(&ret, &info, sizeof(sig_mon_info_t));
  return (u32)&p_priv->p_data->info;
}

static u32 get_nim_locked(sig_mon_priv_t *p_priv)
{
  static u8 repeat_time = 0;
  static BOOL status = TRUE;
  //uio_device_t *p_uio_dev = NULL;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  {
    if(nc_get_lock_status(nc_handle))
    {
      repeat_time = 0;
      status = TRUE;
    }
    else if(repeat_time++ > 1)
    {
      repeat_time = 0;
      status = FALSE;
    }
  }
  //OS_PRINTF("status [%d], lock[%d], get_lock[%d], repeat_time[%d]\n",
  //  status,  nc_get_lock_status(nc_handle), repeat_time);
  if(p_priv->p_sig_mon_impl->lock_led_set != NULL)
  {
    p_priv->p_sig_mon_impl->lock_led_set(status);
  }
  return status;
}

static void task_single_step(void *p_handle, os_msg_t *p_cmd)
{
  sig_mon_priv_t *p_priv = (sig_mon_priv_t *)p_handle;
  signal_mon_data_t *p_data = p_priv->p_data;
  
  if(p_cmd != NULL)
  {
    switch(p_cmd->content)
    {
      case SIG_MON_LOCK_TP:
        set_freq_lock(p_priv, (dvbs_lock_info_t *)p_cmd->para1, (sys_signal_t)p_cmd->para2);
        break;
      case SIG_MON_MONITOR_PERF:
        p_data->monitor_perf = p_cmd->para1;
        break;
      case SIG_MON_RESET:
        sf_send_evt(SIG_MON_SIGNAL_INFO, 0, SF_SIGNAL_STABLE);
        OS_PRINTF("!!!SIG_MON_RESET %d\n", SF_SIGNAL_STABLE);
        break;
      case NC_EVT_POS_TURNING:
        sf_send_evt(SIG_MON_POS_TURNING, 0, 0);
        break;
      case NC_EVT_POS_OUT_RANGE:
        sf_send_evt(SIG_MON_POS_OUT_RANGE, 0, 0);
        break;
      case NC_EVT_LOCKED:
        OS_PRINTF("Signal Status : LOCKED\n");
        sf_send_evt(SIG_MON_LOCKED, 0, 0);
        break;
      case NC_EVT_UNLOCKED:
        OS_PRINTF("Signal Status : UNLOCKED\n");
        sf_send_evt(SIG_MON_UNLOCKED, 0, 0);
        break;      
      default:
        break;
    }
  }
  //else
  {
    u32 cur_tick = mtos_ticks_get();
    u32 signal = 0;
    class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    
    if((cur_tick - p_data->start_tick) >= 40
      && nc_is_finish_locking(nc_handle))//pre 40 ticks
    {
      if(p_data->monitor_perf)
      {
        signal = get_signal_info(p_priv);
          //// use real snr to correct lock status in monitor mode
          //if (signal.snr == 0)
          //{
         //is_lock = FALSE;
          //}
      }
    else
    {
       memset(&p_priv->p_data->info,0,sizeof(sig_mon_info_t));
       signal = (u32)&p_priv->p_data->info;
    }
      sf_send_evt(SIG_MON_SIGNAL_INFO, signal, get_nim_locked(p_priv));
//      OS_PRINTF("!!!!Signal monitor signal[%d], lock[%d]\n", signal, 
//        get_nim_locked(p_priv));
      p_data->start_tick = cur_tick;      
    }
  }
}

app_t *construct_ap_signal_monintor(sig_mon_policy_t *p_impl_policy)
{
  sig_mon_priv_t *p_priv = mtos_malloc(sizeof(sig_mon_priv_t));
  MT_ASSERT(p_priv != NULL);

  //Create private data buffer
  p_priv->p_data = mtos_malloc(sizeof(signal_mon_data_t));
  MT_ASSERT(p_priv->p_data != NULL);

  memset(p_priv->p_data, 0, sizeof(signal_mon_data_t));
  memset(&(p_priv->instance), 0, sizeof(app_t));
  
  //Attach SM implement policy
  MT_ASSERT(p_impl_policy != NULL);
  p_priv->p_sig_mon_impl = p_impl_policy;
  
  //Attach APP signal instance
  p_priv->instance.p_data = p_priv;
  p_priv->instance.init = init;
  p_priv->instance.task_single_step = task_single_step;
  
  return &p_priv->instance;
}
