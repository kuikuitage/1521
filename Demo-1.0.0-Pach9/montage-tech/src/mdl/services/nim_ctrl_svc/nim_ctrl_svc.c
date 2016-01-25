/******************************************************************************/
/******************************************************************************/
#include <assert.h>
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

#include "lib_usals.h"
#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "uio.h"
#define NC_SVC_Q_DEPTH (10)

/*!
  nim ctrl status
  */
typedef enum
{
  NC_IDLE = 0,
  NC_DO_BS,
  NC_CHECK_LOCK,
  NC_MONITOR,
  NC_WAIT_IQ_CHANGE
} nc_state_t;

typedef struct
{
  u32 start_ticks;
  u32 duration_ticks;
  u32 monitor_ticks;
  u32 wait_ticks;
  BOOL is_sync_lock;
  BOOL auto_IQ;
  BOOL for_bs;
  BOOL tuner_locking;
  BOOL monitor_enable;
  nim_device_t *p_nim_dev;
  nc_channel_info_t cur_channel_info;
  u8 nim_port;
  u32 msgq_id;
  u8 onoff12v;
  svc_container_t *p_container;
  nc_state_t status;
  service_t *p_last_svc;
  nc_diseqc_info_t dis_info;
  BOOL dis_need_reset;
  BOOL lnb_check_enble;
  u32 last_ticks;
  sys_signal_t lock_mode;
} nc_priv_data_t;

typedef struct
{
  nim_ctrl_t *p_this;
} nim_ctrl_svc_data_t;

/*!
  Declaration
  */
static void _set_22k(class_handle_t handle, u8 onoff22k);

static void _switch_ctrl(class_handle_t handle,
        nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port);

static void _positioner_ctrl(class_handle_t handle,
      rscmd_positer_t cmd, u32 param);
      
static void _usals_ctrl(class_handle_t handle, double sat_longitude, 
        double local_longitude, double local_latitude);

static BOOL _set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc);

static void _diseqc_ctrl(class_handle_t handle, nim_diseqc_cmd_t *p_diseqc_cmd);

static void send_evt(nc_priv_data_t *p_priv, nc_evt_t evt)
{
  os_msg_t msg = {0};
  msg.content = evt;
  mdl_broadcast_msg(&msg);
}

static void send_lock_result(nc_priv_data_t *p_priv, nc_evt_t evt)
{
  os_msg_t msg = {0};
  msg.content = evt;
  msg.para1 = (u32)&p_priv->cur_channel_info;
  msg.para2 = sizeof(nc_channel_info_t);
  OS_PRINTF("nc: send evt lock[%d]\n", (evt == NC_EVT_LOCKED));
  //send by last svc, fix me
  p_priv->p_last_svc->notify(p_priv->p_last_svc, &msg);
}


static BOOL cmp_channel(nc_priv_data_t *p_priv,
  nc_channel_info_t *p_old, nc_channel_info_t *p_new)
{
  BOOL res = FALSE;
  //OS_PRINTF("cmp_channel:  old            new\n");
  //OS_PRINTF("        22K:   %d             %d\n", p_old->onoff22k,
  //  p_new->onoff22k);
  //OS_PRINTF("        pol:   %d             %d\n", p_old->polarization,
  //  p_new->polarization);
  //OS_PRINTF("       freq:   %d             
  //%d\n", p_old->channel_info.frequency,
  //  p_new->channel_info.frequency);

  switch(p_priv->lock_mode)
  {
    case SYS_DVBC:
      res = ((p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.param.dvbc.symbol_rate == 
            p_new->channel_info.param.dvbc.symbol_rate)
        && (p_old->channel_info.param.dvbc.modulation == 
            p_new->channel_info.param.dvbc.modulation));      
      break;

    case SYS_DVBS:
      res = ((p_old->onoff22k == p_new->onoff22k)
        && (p_old->polarization == p_new->polarization)
        && (p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.spectral_polar == 
            p_new->channel_info.spectral_polar)
        && (p_old->channel_info.param.dvbs.symbol_rate == 
            p_new->channel_info.param.dvbs.symbol_rate)
        && (p_old->channel_info.param.dvbs.uc_param.bank == 
            p_new->channel_info.param.dvbs.uc_param.bank) 
        && (p_old->channel_info.param.dvbs.uc_param.user_band == 
            p_new->channel_info.param.dvbs.uc_param.user_band)
        && (p_old->channel_info.param.dvbs.uc_param.ub_freq_mhz == 
            p_new->channel_info.param.dvbs.uc_param.ub_freq_mhz));      
      break;

    case SYS_DVBT:
      res = ((p_old->onoff22k == p_new->onoff22k)
        && (p_old->polarization == p_new->polarization)
        && (p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.spectral_polar == 
            p_new->channel_info.spectral_polar)
        && (p_old->channel_info.param.dvbs.symbol_rate == 
            p_new->channel_info.param.dvbs.symbol_rate));      
      break;
  }

  return res;

}

static BOOL tc_need_relock(nc_priv_data_t *p_priv,
  nc_channel_info_t *p_info, BOOL for_bs)
{
  BOOL b_relock = FALSE;
  nim_channel_info_t channel_info;
  nc_channel_info_t  cur_nc_nim_info;

  memset(&channel_info,0,sizeof(nim_channel_info_t));
  dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_GET_CHANNEL_INFO, (u32)&channel_info);
  cur_nc_nim_info.channel_info.frequency = channel_info.frequency;
  cur_nc_nim_info.channel_info.param.dvbc.symbol_rate = channel_info.param.dvbc.symbol_rate;
  cur_nc_nim_info.channel_info.param.dvbc.modulation = channel_info.param.dvbc.modulation;   
  if(for_bs)  //build scan, set tuner force
  {
    b_relock = TRUE;
  }
  else if(!cmp_channel(p_priv, &p_priv->cur_channel_info, p_info))
  {
    b_relock = TRUE;
  }
  else if (!cmp_channel(p_priv, &cur_nc_nim_info, p_info))
  {
    b_relock = TRUE;
  }
  else if((NC_MONITOR == p_priv->status) || (NC_IDLE == p_priv->status))
  {
    if(p_priv->tuner_locking) //tuner locked
    {
      send_lock_result(p_priv, NC_EVT_LOCKED);

    }
    else
    {
      b_relock = TRUE;
    }
  }
  else
  {
    //tuner locking and no any parameter exchanged. do nothing
  }
  return b_relock;
}

static void do_check_lnb(nc_priv_data_t *p_priv)
{
  //try to remove protect
  dev_io_ctrl(p_priv->p_nim_dev, NIM_REMOVE_PROTECT, 0);
  //delay 10ms.
  mtos_task_delay_ms(10);
  //reset
  dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_LNB_SC_PROT_RESTORE, 0);
}

static void do_set_tp(class_handle_t handle, nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  

  nc_channel_info_t tp_info = {0};
  nim_channel_info_t channel_info = {0};
  nim_channel_set_info_t set_info = {0};
  BOOL is_need_relock = FALSE;  
  mtos_task_lock();
  memcpy(&tp_info, p_tp_info, sizeof(nc_channel_info_t));
  mtos_task_unlock();

  is_need_relock = tc_need_relock(p_priv, &tp_info, p_priv->for_bs);
  if(is_need_relock)
  {
     if (p_priv->lnb_check_enble == TRUE)
    {
      do_check_lnb(p_priv);
    }
   }
  OS_PRINTF("tc lock tuner freq %d, sym %d, polar %d, is_bs %d, need relock %d, pol %d\n", 
    tp_info.channel_info.frequency, 
    tp_info.channel_info.param.dvbs.symbol_rate,
    tp_info.polarization, p_priv->for_bs,
    is_need_relock,tp_info.channel_info.spectral_polar);

  if(is_need_relock)
  {
     if(p_priv->cur_channel_info.polarization != tp_info.polarization)
    {
      p_priv->cur_channel_info.polarization = tp_info.polarization;
      dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_PORLAR,
        tp_info.polarization);
    }
    if(p_priv->cur_channel_info.onoff22k != tp_info.onoff22k)
    {
      p_priv->cur_channel_info.onoff22k = tp_info.onoff22k;
      dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF,
        tp_info.onoff22k);
    }

    if(p_priv->for_bs)
    {
      p_priv->status = NC_DO_BS;
    }
    else
    {
      memcpy(&channel_info, &tp_info.channel_info,
        sizeof(nim_channel_info_t));
      nim_channel_set(p_priv->p_nim_dev, &channel_info, &set_info);
      p_priv->duration_ticks = set_info.lock_time / 10;  //200 ticks = 2s
      p_priv->start_ticks = mtos_ticks_get();
      p_priv->status = NC_CHECK_LOCK;
    }
    memcpy(&p_priv->cur_channel_info, &tp_info, sizeof(nc_channel_info_t));

    // reset diseqc according to tp info
    if (p_priv->dis_info.is_fixed)
    {
      _set_diseqc(p_this, &p_priv->dis_info);
    }
  }
  p_priv->monitor_enable = TRUE;
}


static void enter_monitor_mode(nc_priv_data_t *p_priv)
{
  p_priv->monitor_ticks = mtos_ticks_get();
  p_priv->status = NC_MONITOR;
}

static void msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_svc = (service_t *)handle;
  nim_ctrl_svc_data_t *p_svc_data = p_svc->get_data_buffer(p_svc);
  nim_ctrl_t *p_this = p_svc_data->p_this;
  nc_priv_data_t *p_priv = p_this->p_data;
  
  switch(p_msg->content)
  {
    case NC_LOCK_TUNER:
      p_priv->p_last_svc = p_svc;
      do_set_tp(p_this, (nc_channel_info_t *)p_msg->para1);
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}

static void sm_proc(handle_t handle)
{
  service_t *p_svc = (service_t *)handle;
  nim_ctrl_svc_data_t *p_svc_data = p_svc->get_data_buffer(p_svc);
  nim_ctrl_t *p_this = p_svc_data->p_this;
  nc_priv_data_t *p_priv = p_this->p_data;
  u32 cur_ticks = 0;
  u8 is_lock = 0;
  u32 maxticks_cnk = 0;
  nc_evt_t evt = NC_EVT_BEGIN;
  u8 param = 0;

  cur_ticks = mtos_ticks_get();
  
  switch(p_priv->status)
  {
    case NC_IDLE:
      if(p_priv->lnb_check_enble == TRUE)
      {
        dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);  
        // check lnb short
        if(param == NIM_LNB_SC_PROTING && ((cur_ticks - p_priv->last_ticks) >= 300))
        {
          OS_PRINTF("lnb short \n");
          do_check_lnb(p_priv);
          p_priv->last_ticks = cur_ticks;
       }
      }
      break;
    case NC_MONITOR:
      if(p_priv->monitor_enable)
      {
        dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LOCK,
          (u32)&is_lock);
        if (is_lock != p_priv->tuner_locking)
        {
          p_priv->tuner_locking = is_lock;
          p_priv->monitor_ticks = mtos_ticks_get();
        }

        if(!p_priv->tuner_locking) //tuner unlocked
        {
          cur_ticks = mtos_ticks_get();
          if(SYS_DVBC !=  p_priv->lock_mode)
            {
               maxticks_cnk = 150;//pre 2s
            }
          else
            {
               maxticks_cnk = 200;//pre 2s
            }
          if((cur_ticks - p_priv->monitor_ticks) > maxticks_cnk)  //pre 2s  /150
          {
            nim_channel_info_t channel_info = {0};
            nim_channel_set_info_t set_info = {0};

            memcpy(&channel_info, &p_priv->cur_channel_info.channel_info,
              sizeof(nim_channel_info_t));
            OS_PRINTF("###nim ctrl monitor reset tp[f:%d, s:%d]\n",
              channel_info.frequency, channel_info.param.dvbs.symbol_rate);
            if((SYS_DVBC ==  p_priv->lock_mode) && p_priv->auto_IQ)
            {
                dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHANGE_IQ, (u32)&channel_info);
             }
            nim_channel_set(p_priv->p_nim_dev, &channel_info, &set_info);
            //nim_channel_connect(p_priv->p_nim_dev, &channel_info, FALSE);
            if(SYS_DVBC !=  p_priv->lock_mode)
            {
              //Reset 22k 
              _set_22k(p_this, p_priv->cur_channel_info.onoff22k);
              //Reset dis
              _set_diseqc(p_this, &p_priv->dis_info);
            }
            p_priv->dis_need_reset = TRUE;
            p_priv->monitor_ticks = cur_ticks;
          }
        }
        else
        {
          if(p_priv->dis_need_reset == TRUE)
          {
            //##6171 reset diseqc to avoid using diseqc default port 
            //Reset 22k 
            _set_22k(p_this, p_priv->cur_channel_info.onoff22k);
            //Reset dis
            _set_diseqc(p_this, &p_priv->dis_info);
            
            p_priv->dis_need_reset = FALSE;
          }
        }
      }

      if(p_priv->lnb_check_enble == TRUE)
      {
        dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);  
        // check lnb short
        if(param == NIM_LNB_SC_PROTING && ((cur_ticks - p_priv->last_ticks) >= 300))
        {
          OS_PRINTF("lnb short \n");
          do_check_lnb(p_priv);
          p_priv->last_ticks = cur_ticks;
       }
      }
      break;
    case NC_DO_BS:
      //call the sync api
      nim_channel_connect(p_priv->p_nim_dev,
        &(p_priv->cur_channel_info.channel_info), TRUE);
      p_priv->tuner_locking = p_priv->cur_channel_info.channel_info.lock;
      evt = (p_priv->tuner_locking) ? NC_EVT_LOCKED : NC_EVT_UNLOCKED;
      send_lock_result(p_priv, evt);
      p_priv->status = NC_IDLE;
      break;
    case NC_CHECK_LOCK:
      {
        BOOL locked = FALSE;
        dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LOCK, (u32)&locked);

        if(locked) //tuner locked
        {
          p_priv->tuner_locking = TRUE;
          send_lock_result(p_priv, NC_EVT_LOCKED);
          enter_monitor_mode(p_priv);
        }
        else
        {
          cur_ticks = mtos_ticks_get();
          if(p_priv->duration_ticks < (cur_ticks - p_priv->start_ticks))
          {
            if(p_priv->auto_IQ)
            {
              nim_channel_info_t channel_info = {0};
              memcpy(&channel_info, &p_priv->cur_channel_info.channel_info,
                sizeof(nim_channel_info_t));

              dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHANGE_IQ, (u32)&channel_info);
              //p_priv->duration_ticks = 10;  //wait 100ms
              p_priv->start_ticks = mtos_ticks_get();
              p_priv->status = NC_WAIT_IQ_CHANGE;
            }
            else
            {
              p_priv->tuner_locking = FALSE;
              send_lock_result(p_priv, NC_EVT_UNLOCKED);
              enter_monitor_mode(p_priv);
            }
          }
        }
      }
      break;
    case NC_WAIT_IQ_CHANGE:
      {
        BOOL locked = FALSE;
        dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LOCK, (u32)&locked);

        if(locked) //tuner locked
        {
          p_priv->tuner_locking = TRUE;
          send_lock_result(p_priv, NC_EVT_LOCKED);
          OS_PRINTF("NC_EVT_LOCKED 33\n");
          enter_monitor_mode(p_priv);
        }
        else
        {
          cur_ticks = mtos_ticks_get();
          if(p_priv->wait_ticks < (cur_ticks - p_priv->start_ticks))
          {
            p_priv->tuner_locking = FALSE;
            send_lock_result(p_priv, NC_EVT_UNLOCKED);
            enter_monitor_mode(p_priv);
          }
        }
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}

static void on_decorate_cmd(handle_t handle, os_msg_t *p_msg)
{
  p_msg->is_ext = (NC_LOCK_TUNER == p_msg->content);
}

service_t * nc_get_svc_instance(class_handle_t handle, u32 context)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  service_t *p_svc = NULL;
  
  p_svc = p_priv->p_container->get_svc(p_priv->p_container, context);
  if(NULL == p_svc)
  {
    nim_ctrl_svc_data_t *p_svc_data = NULL;
    
    p_svc = construct_svc(context);
    p_svc->sm_proc = sm_proc;
    p_svc->msg_proc = msg_proc;
    p_svc->on_decorate_cmd = on_decorate_cmd;
    p_svc->allocate_data_buffer(p_svc, sizeof(nim_ctrl_svc_data_t));
    p_svc_data = (nim_ctrl_svc_data_t *)p_svc->get_data_buffer(p_svc);

    p_svc_data->p_this = p_this;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

void nc_remove_svc_instance(class_handle_t handle, service_t *p_svc)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  p_priv->p_container->remove_svc(p_priv->p_container, p_svc);
}


static void _set_tp(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  service_t *p_svc = p_extend;
  
  if(p_priv->is_sync_lock)
  {
    p_priv->p_last_svc = p_svc;
    do_set_tp(p_this, p_tp_info);
  }
  else
  {
    //fix me. just fot nim_ctrl 2010-09-8
    mtos_messageq_clr(p_priv->msgq_id);
    p_svc->do_cmd(p_svc, NC_LOCK_TUNER, (u32)p_tp_info,
      sizeof(nc_channel_info_t));
  }
}

static void _get_tp(class_handle_t handle, nc_channel_info_t *p_current_info)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  MT_ASSERT(p_current_info != NULL);

  memcpy(p_current_info, &(p_priv->cur_channel_info), sizeof(nc_channel_info_t));
}

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  */
static BOOL _get_lock_status(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  return p_priv->tuner_locking;
}

static BOOL _is_finish_locking(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  return (BOOL)(p_priv->status == NC_MONITOR);
}



/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[out] p_perf: channel performance
  */
static void _perf_get(class_handle_t handle, nim_channel_perf_t *p_perf)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_channel_perf(p_priv->p_nim_dev, p_perf);
}

static void _set_blind_scan_mode(class_handle_t handle, BOOL bs)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->for_bs = bs;
  p_priv->monitor_enable = !bs;
}

static void _enable_monitor(class_handle_t handle, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->monitor_enable = enable;
}

static void _enable_lnb_check(class_handle_t handle, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->lnb_check_enble = enable;
}

static BOOL _is_lnb_check_enable(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  
  return p_priv->lnb_check_enble;
}

static void _set_polarization(class_handle_t handle, nim_lnb_porlar_t polar)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  
  if(p_priv->cur_channel_info.polarization != polar)
  {
    p_priv->cur_channel_info.polarization = polar;
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_PORLAR, polar);
  }
}

static void _set_22k(class_handle_t handle, u8 onoff22k)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none 22k \n");
    return ;
  }

  if(p_priv->cur_channel_info.onoff22k != onoff22k)
  {
    p_priv->cur_channel_info.onoff22k = onoff22k;
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, onoff22k);
  }
}

/*!
  Set 12v onoff
  \param[in] handle for nc 
  \param[in] onoff12v 1 is on and 0 is off
  */
static void _set_12v(class_handle_t handle, u8 onoff12v)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none 12v \n");
    return;
  }

  
  if(p_priv->onoff12v != onoff12v)
  {
    p_priv->onoff12v = onoff12v;
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_12V_ONOFF, onoff12v);
    //OS_PRINTF("---> nc set 12v is %s\n", onoff12v ? "ON" : "OFF");
  }
}

static BOOL _tone_burst(class_handle_t handle, nim_diseqc_mode_t mode)
{
  //nim_ctrl_t *p_this = handle;
  //nc_priv_data_t *p_priv = p_this->p_data;
  ///???how do burst
  //return (SUCCESS == rs2k_tone_burst(mode))
  return TRUE;
}

static void _diseqc_ctrl(class_handle_t handle, 
  nim_diseqc_cmd_t *p_diseqc_cmd)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  u8 param = 0;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }


  if(p_priv->lnb_check_enble == FALSE)
  {
    nim_diseqc_ctrl(p_priv->p_nim_dev, p_diseqc_cmd);
  }
  else
  {
    //to detect
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);   

    if(param == NIM_LNB_SC_NO_PROTING)
    {
      //remove protect.
      dev_io_ctrl(p_priv->p_nim_dev, NIM_REMOVE_PROTECT, 0);    

      //nim diseqc control
      nim_diseqc_ctrl(p_priv->p_nim_dev, p_diseqc_cmd);
      mtos_task_delay_ms(100);

      //reset
      dev_io_ctrl(p_priv->p_nim_dev, NIM_ENABLE_CHECK_PROTECT, 0);
      p_priv->last_ticks = mtos_ticks_get();
    }
  }
}

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
static void _switch_ctrl(class_handle_t handle,
        nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_diseqc_cmd_t diseqc_cmd = {0};
  u8 buf[4] = {0};
  u8 i = 0;
  u8 repeat_times = 1;
  u8 is_22k_on = 0;
  u8 pol_status = 0;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }
  diseqc_cmd.mode = NIM_DISEQC_BYTES;
  diseqc_cmd.tx_len = 4;
  diseqc_cmd.p_tx_buf = buf;

  buf[0] = 0xE0;
  buf[1] = 0x10;

  is_22k_on = p_priv->cur_channel_info.onoff22k;
  if(p_priv->cur_channel_info.polarization == NIM_PORLAR_HORIZONTAL)
  {
    pol_status = 1;
  }
  else
  {
    pol_status = 0;
  }
  
  switch(diseqc_level)
  {
    case NIM_DISEQC_LEVEL_X0:
      MT_ASSERT(port < 4);
      /* X.0, only support committed switch */
      //OS_PRINTF("port [%d], pol_status [%d], is_22k_on[%d]\n", 
       // port, pol_status, is_22k_on);
      buf[2] = 0x38;
      buf[3] = 0xF0 + port * 4 + pol_status * 2 + is_22k_on;
      //OS_PRINTF("Command content %x\n", buf[3]);
      break;

    default:
      /* level above X.0, support uncommitted switch */
      MT_ASSERT(port < 16);
      /* uncommitted switch */
      buf[2] = 0x39;
      buf[3] = 0xF0 + port;
      if(mode != RSCMD_SWITCH_TWO_REPEATS)
      {
        repeat_times = 2;
      }
      else
      {
        repeat_times = 3;
      }
      break;
  }

  for(i = 0; i < repeat_times; i++)
  {
    _diseqc_ctrl(handle, &diseqc_cmd);
  }
}

/*!
  DiSEqC control for positioner
  \param[in] handle: nim ctrl handle
  \param[in] cmd: the positioner cmd to DiSEqC
  \param[in] param: cmd with paramter
  */
static void _positioner_ctrl(class_handle_t handle,
      rscmd_positer_t cmd, u32 param)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_diseqc_cmd_t msg = {0};
  u8 tx_buf[8] = {0};
  u16 mode = param >> 16;
  u8 data = (u8)param;
//  u16 degree_decimal = 0;
//  u8 degree = 0;
//  u8 degree_dec_tbl[5] = {0x00, 0x02, 0x03, 0x05, 0x06};

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }

  if(p_priv->cur_channel_info.onoff22k)
  {
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 0);
    mtos_task_delay_ms(20);
  }

  dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_DISEQC1X, 0);
  
  msg.mode = NIM_DISEQC_BYTES;
  msg.p_tx_buf = tx_buf;
  msg.tx_len = 4;
  
  msg.p_tx_buf[0] = 0xE0;
  msg.p_tx_buf[1] = 0x31;
  msg.p_tx_buf[2] = cmd;

  switch(cmd)
  {
    case RSCMD_PSTER_STOP:
    case RSCMD_PSTER_LIMIT_OFF:
    case RSCMD_PSTER_LIMIT_E:
    case RSCMD_PSTER_LIMIT_W:
      msg.tx_len = 3;
      break;

    case RSCMD_PSTER_DRV_E:
    case RSCMD_PSTER_DRV_W:
      if(mode == RSMOD_PSTER_DRV_CONTINUE)
      {
        msg.p_tx_buf[3] = 0;
      }
      else if(mode == RSMOD_PSTER_DRV_TIMEOUT)
      {
        /* time out is 1 ~ 127 s */
        msg.p_tx_buf[3] = data & 0x7F;
      }
      else if(mode == RSMOD_PSTER_DRV_STEPS)
      {
        /* step is 1 ~ 128, data_send is 0x80 ~ 0xFF */
        msg.p_tx_buf[3] = (~data) + 1;
      }
      else
      {
        /* default use continuesly mode */
        msg.p_tx_buf[3] = 0;
      }
      break;

    case RSCMD_PSTER_RECALCULATE:
      //msg.p_tx_buf[3] = data;
      msg.p_tx_buf[3] = p_priv->nim_port;
      break;

    case RSCMD_PSTER_GOTO_NN:
      msg.p_tx_buf[3] = data;

//      if(data != p_priv->nim_port)
      {
        p_priv->nim_port = data;
        // notify driving positioner
        send_evt(p_priv, NC_EVT_POS_TURNING);   
      }
      break;
case RSCMD_PSTER_STORE_NN:
      msg.p_tx_buf[3] = data;
      p_priv->nim_port = data;
      break;
    case RSCMD_PSTER_GOTO_ANG:
      msg.tx_len = 5;
#if 0      
      degree = param >> 16;
      degree_decimal = (u8)param;
      if(degree_decimal > 9)
      {
        degree_decimal = 9;
      }
      msg.p_tx_buf[4] |= (degree / 256) << 4;
      degree %= 256;
      msg.p_tx_buf[4] |= (degree / 16);
      degree %= 16;
      msg.p_tx_buf[5] |= degree << 4;
      msg.p_tx_buf[5] |= (degree_decimal < 5) ? 
        degree_dec_tbl[degree_decimal] : 
        (0x08 + degree_dec_tbl[degree_decimal - 5]);
#else
      if(param == 0)
      {
        msg.p_tx_buf[3] = 0xE0;
        msg.p_tx_buf[4] = 0x00;
      }
      else
      {
        msg.p_tx_buf[3] = (param >> 8) & 0xFF;
        msg.p_tx_buf[4] = (param & 0xFF);
      }
      // notify driving positioner
      send_evt(p_priv, NC_EVT_POS_TURNING);
#endif
      break;
        
    default:
      msg.p_tx_buf[3] = data;
  }

  _diseqc_ctrl(handle, &msg);

  if(p_priv->cur_channel_info.onoff22k)
  {
    mtos_task_delay_ms(50);
    dev_io_ctrl(p_priv->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 1);
  }
  mtos_task_delay_ms(20);
}

static void _usals_ctrl(class_handle_t handle, double sat_longitude, 
        double local_longitude, double local_latitude)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  u16 param = 0;
  double degree = 0;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }

  degree = usals_calc_degree(sat_longitude, 
                             local_longitude, local_latitude);
  if(degree > 90)
  {
    // notify driving positioner
    send_evt(p_priv, NC_EVT_POS_OUT_RANGE);   
  }
  else
  {
    param = usals_convert_degree(degree);
    nc_positioner_ctrl(handle, RSCMD_PSTER_GOTO_ANG, param);
  }
}

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  \param[in] polar nim_lnb_porlar_t mode
  */
static BOOL _set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  rscmd_switch_mode_t mode = 0;
  u8 port = 0;

  if(p_priv->lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return TRUE;
  }

  //Record last disq information
  p_priv->dis_info.is_fixed = p_diseqc->is_fixed;
  
  if(p_diseqc->is_fixed)
  {
    if(p_diseqc->diseqc_type_1_1 != 0)
    {
      //Record disq 1.1 info
      p_priv->dis_info.diseqc_type_1_1 = p_diseqc->diseqc_type_1_1;
      
      port = p_diseqc->diseqc_port_1_1;
      switch (p_diseqc->diseqc_type_1_1)
      {
        case 1: // no repeat
          mode = RSCMD_SWITCH_NO_REPEAT;
          break;
        case 2: // one repeat
          mode = RSCMD_SWITCH_ONE_REPEAT;
          break;
        default: // two repeat
          mode = RSCMD_SWITCH_TWO_REPEATS;
      }
      if(p_diseqc->diseqc_1_1_mode != 0)
      {
        p_priv->dis_info.diseqc_1_1_mode = p_diseqc->diseqc_1_1_mode;
        
        port &= 0x03;
        port = port << (p_diseqc->diseqc_1_1_mode - 1);
      }
      
      p_priv->dis_info.diseqc_port_1_1 = port;
      _switch_ctrl(handle, NIM_DISEQC_LEVEL_X1, mode, port);
    }  
    else
    {
      p_priv->dis_info.diseqc_type_1_1 = 0;
      p_priv->dis_info.diseqc_1_1_mode = 0;
      p_priv->dis_info.diseqc_port_1_1 = 0;
    }
    
    if(p_diseqc->diseqc_type_1_0 != 0)
    {
      _switch_ctrl(handle, NIM_DISEQC_LEVEL_X0, 0, 
        (u8)p_diseqc->diseqc_port_1_0);

      //Record Disq info
      p_priv->dis_info.diseqc_type_1_0 = p_diseqc->diseqc_type_1_0;
      p_priv->dis_info.diseqc_port_1_0 = p_diseqc->diseqc_port_1_0;
    }
    else
    {
      p_priv->dis_info.diseqc_type_1_0 = 0;
      p_priv->dis_info.diseqc_port_1_0 = 0;
    }
  }
  else
  {
    p_priv->dis_info.used_DiSEqC12 = p_diseqc->used_DiSEqC12;
    if(p_diseqc->used_DiSEqC12) // DiSEqC 1.2
    {
      if(p_diseqc->position_type != 0)
      {
        if(p_priv->dis_info.position != p_diseqc->position)
        {
          //_positioner_ctrl(handle, RSCMD_PSTER_GOTO_NN, p_diseqc->position);
          if(p_this->positioner_ctrl != NULL)
          {
            p_this->positioner_ctrl(handle, RSCMD_PSTER_GOTO_NN, p_diseqc->position);
          }
          p_priv->dis_info.position_type = p_diseqc->position_type;
          p_priv->dis_info.position = p_diseqc->position;
          return TRUE;
        }
      }
      else
      {
        // clean info
        p_priv->dis_info.position_type = 0;
        p_priv->dis_info.position = 0;
      }
    }
    else // USALS
    {
      if((p_priv->dis_info.d_sat_longitude != p_diseqc->d_sat_longitude)
        || (p_priv->dis_info.d_local_longitude != p_diseqc->d_local_longitude)
        || (p_priv->dis_info.d_local_latitude != p_diseqc->d_local_latitude))
      {
        //_usals_ctrl(handle,p_diseqc->d_sat_longitude, 
        //  p_diseqc->d_local_longitude, p_diseqc->d_local_latitude);
        if(p_this->usals_ctrl != NULL)
        {
          p_this->usals_ctrl(handle,p_diseqc->d_sat_longitude, 
            p_diseqc->d_local_longitude, p_diseqc->d_local_latitude);
        }
        p_priv->dis_info.d_sat_longitude = p_diseqc->d_sat_longitude;
        p_priv->dis_info.d_local_latitude = p_diseqc->d_local_latitude;
        p_priv->dis_info.d_local_longitude = p_diseqc->d_local_longitude;
      }
    }
  }

  return FALSE;
}

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  */
static void _clr_diseqc_info(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  memset(&p_priv->dis_info, 0, sizeof(nc_diseqc_info_t));
}

handle_t nc_svc_init(nc_svc_cfg_t *p_cfg)
{
  nim_ctrl_t *p_nim_ctrl = mtos_malloc(sizeof(nim_ctrl_t));
  nc_priv_data_t *p_priv = mtos_malloc(sizeof(nc_priv_data_t));
  u32 attach_size = sizeof(nc_channel_info_t) * NC_SVC_Q_DEPTH;
  void *p_attach_buffer = mtos_malloc(attach_size);
  svc_container_t *p_container = construct_svc_container("nim_ctrl",
    p_cfg->priority, p_cfg->stack_size, NC_SVC_Q_DEPTH);

  MT_ASSERT(p_nim_ctrl != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_attach_buffer != NULL);
  
  memset(p_priv, 0, sizeof(nc_priv_data_t));
  memset(p_nim_ctrl, 0, sizeof(nim_ctrl_t));
  p_nim_ctrl->p_data = p_priv;
  p_priv->p_container = p_container;

  //init paramter
  p_priv->cur_channel_info.onoff22k = 0xFF; //to invalid
  p_priv->cur_channel_info.polarization = 0xFF; //to invalid
  p_priv->nim_port = 0xFF;
  p_priv->is_sync_lock = p_cfg->b_sync_lock;
  p_priv->lock_mode = p_cfg->lock_mode;
  p_priv->auto_IQ = p_cfg->auto_iq; 
  if (p_cfg->wait_ticks != 0)
  {
    p_priv->wait_ticks =  p_cfg->wait_ticks;
  }
  else
  {
    p_priv->wait_ticks = 10;
  }

  p_priv->msgq_id = p_container->get_msgq_id(p_container);
  p_priv->p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  p_priv->tuner_locking = TRUE;

  //init function
  p_nim_ctrl->set_tp = _set_tp;
  p_nim_ctrl->get_tp = _get_tp;
  p_nim_ctrl->get_lock_status = _get_lock_status;
  p_nim_ctrl->perf_get = _perf_get;
  p_nim_ctrl->set_blind_scan_mode = _set_blind_scan_mode;
  p_nim_ctrl->enable_monitor = _enable_monitor;
  p_nim_ctrl->is_finish_locking = _is_finish_locking;
  if(SYS_DVBC !=  p_priv->lock_mode)
  {
    p_nim_ctrl->lnb_check_enable = _enable_lnb_check;
    p_nim_ctrl->is_lnb_check_enable = _is_lnb_check_enable;
    p_nim_ctrl->set_polarization = _set_polarization;
    p_nim_ctrl->set_22k = _set_22k;
    p_nim_ctrl->set_12v = _set_12v;
    p_nim_ctrl->tone_burst = _tone_burst;
    p_nim_ctrl->switch_ctrl = _switch_ctrl;
    p_nim_ctrl->positioner_ctrl = _positioner_ctrl;
    p_nim_ctrl->usals_ctrl = _usals_ctrl;
    p_nim_ctrl->set_diseqc = _set_diseqc;
    p_nim_ctrl->clr_diseqc_info = _clr_diseqc_info;
  }
  //attach extern size
  memset(p_attach_buffer, 0, attach_size);
  mtos_messageq_attach(p_priv->msgq_id, p_attach_buffer,
    sizeof(nc_channel_info_t), NC_SVC_Q_DEPTH);

  class_register(NC_CLASS_ID, p_nim_ctrl);

  // reset diseqc
  _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0);
  
  return p_nim_ctrl;
}

handle_t nc_svc_init_without_diseqc12(nc_svc_cfg_t *p_cfg)
{
  nim_ctrl_t *p_nim_ctrl = mtos_malloc(sizeof(nim_ctrl_t));
  nc_priv_data_t *p_priv = mtos_malloc(sizeof(nc_priv_data_t));
  u32 attach_size = sizeof(nc_channel_info_t) * NC_SVC_Q_DEPTH;
  void *p_attach_buffer = mtos_malloc(attach_size);
  svc_container_t *p_container = construct_svc_container("nim_ctrl",
    p_cfg->priority, p_cfg->stack_size, NC_SVC_Q_DEPTH);

  MT_ASSERT(p_nim_ctrl != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_attach_buffer != NULL);
  
  memset(p_priv, 0, sizeof(nc_priv_data_t));
  p_nim_ctrl->p_data = p_priv;
  p_priv->p_container = p_container;

  //init paramter
  p_priv->cur_channel_info.onoff22k = 0xFF; //to invalid
  p_priv->cur_channel_info.polarization = 0xFF; //to invalid
  p_priv->nim_port = 0xFF;
  p_priv->is_sync_lock = p_cfg->b_sync_lock;
  p_priv->lock_mode = p_cfg->lock_mode;
  if (p_cfg->wait_ticks != 0)
  {
    p_priv->wait_ticks =  p_cfg->wait_ticks;
  }
  else
  {
    p_priv->wait_ticks = 10;
  }
  p_priv->msgq_id = p_container->get_msgq_id(p_container);
  p_priv->p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);

  //init function
  p_nim_ctrl->set_tp = _set_tp;
  p_nim_ctrl->get_tp = _get_tp;
  p_nim_ctrl->get_lock_status = _get_lock_status;
  p_nim_ctrl->perf_get = _perf_get;
  p_nim_ctrl->set_blind_scan_mode = _set_blind_scan_mode;
  p_nim_ctrl->enable_monitor = _enable_monitor;
  p_nim_ctrl->lnb_check_enable = _enable_lnb_check;
  p_nim_ctrl->is_lnb_check_enable = _is_lnb_check_enable;
  p_nim_ctrl->set_polarization = _set_polarization;
  p_nim_ctrl->set_22k = _set_22k;
  p_nim_ctrl->set_12v = _set_12v;
  p_nim_ctrl->tone_burst = NULL;//_tone_burst;
  p_nim_ctrl->switch_ctrl = _switch_ctrl;
  p_nim_ctrl->positioner_ctrl = NULL;//_positioner_ctrl;
  p_nim_ctrl->usals_ctrl = NULL;//_usals_ctrl;
  p_nim_ctrl->set_diseqc = _set_diseqc;
  p_nim_ctrl->clr_diseqc_info = _clr_diseqc_info;
  p_nim_ctrl->is_finish_locking = _is_finish_locking;

  //attach extern size
  memset(p_attach_buffer, 0, attach_size);
  mtos_messageq_attach(p_priv->msgq_id, p_attach_buffer,
    sizeof(nc_channel_info_t), NC_SVC_Q_DEPTH);

  class_register(NC_CLASS_ID, p_nim_ctrl);

  // reset diseqc
  _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0);
  
  return p_nim_ctrl;
}

