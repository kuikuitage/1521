/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "lib_util.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "common.h"
#include "lib_rect.h"
#include "display.h"
#include "aud_vsb.h"
#include "hal_gpio.h"
#include "uio.h"
#include "smc_op.h"
#include "lpower.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "avctrl1.h"
#include "db_dvbs.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "ap_framework.h"
#include "ap_signal_monitor.h"
#include "ss_ctrl.h"
#include "ap_ota.h"
#include "sys_status.h"
#include "config_prj.h"
// for strength
#define AVG_NUM 1
#define STRENGTH_MAX 95
#define SIGNAL_STRENGTH_RATIO 100
#define STRENGTH_G_THAN_QUALITY 1 //strength is greater than quality

/* TS2000 strength */
#define TS2000_GAIN_NO_SIGNAL 10600
#define TS2000_GAIN_STREN_60 10280
#define TS2000_GAIN_STREN_70 10200
#define TS2000_GAIN_STREN_80 9500
#define TS2000_GAIN_STREN_95 4500
/* TS2020 strength */
#define TS2020_GAIN_NO_SIGNAL 8200
#define TS2020_GAIN_STREN_60 8235
#define TS2020_GAIN_STREN_70 7300
#define TS2020_GAIN_STREN_80 6000
#define TS2020_GAIN_STREN_95 4000
/* TS2022 strength */
#define TS2022_GAIN_NO_SIGNAL 11500
#define TS2022_GAIN_STREN_60 11380
#define TS2022_GAIN_STREN_70 11290
#define TS2022_GAIN_STREN_80 10000
#define TS2022_GAIN_STREN_95 7000

//for snr
#define QUALITY_MAX 85
#define SNR_0 0
#define SNR_30 26
#define SNR_45 46
#define SNR_60 70
#define SNR_75 90
#define SNR_85 115


static void convert_perf_hw(nim_channel_perf_t *p_signal)
{

#if 1
  static u8 status_lock =0;
  MT_ASSERT(p_signal != NULL);
  if(status_lock !=  p_signal->lock)
    {
      status_lock =  p_signal->lock;
      OS_PRINTF("---- signal[%s]: agc[%d] snr[%d], ber[%ld] ----\n",
        p_signal->lock ? "lock" : "unlock",
        p_signal->agc, p_signal->snr,(u32)(p_signal->ber * 10000000000));
    }
#else
  struct nim_device_t* p_nim = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_NIM);
  u8 tuner_type = UNKNOW_TUNER;
  u8 i;
  u32 gain_no_signal, gain_60, gain_70, gain_80, gain_95;
  u16 snr_0, snr_30, snr_45, snr_60, snr_75, snr_85; 
  u8 strength_per, snr_per = 0;
  u16 sum;
  static u8 signal_strength[AVG_NUM];
  static u8 signal_index = 0;  
  static u8 status_lock =0;

  MT_ASSERT(p_signal != NULL);

  if(status_lock !=  p_signal->lock)
    {
      status_lock =  p_signal->lock;
      OS_PRINTF("---- signal[%s]: agc[%d] snr[%d], ber[%ld] ----\n", 
        p_signal->lock ? "lock" : "unlock",
        p_signal->agc, p_signal->snr,p_signal->ber*10000000000);
    }
  


  //OS_PRINTF("original gain[%d], snr[%d]\n", p_signal->agc, perf_info->snr);
  /* calculate signal strength */
  dev_io_ctrl(p_nim, NIM_IOCTRL_GET_TN_TYPE, (u32)&tuner_type);
  
  switch(tuner_type)
  {
     case TC2800:
      gain_no_signal = 30;
      gain_60 = 34;
      gain_70 = 38;
      gain_80 = 42;
      gain_95 = 45;

      break;
    case TC2000:
      gain_no_signal = TS2000_GAIN_NO_SIGNAL;
      gain_60 = TS2000_GAIN_STREN_60;
      gain_70 = TS2000_GAIN_STREN_70;
      gain_80 = TS2000_GAIN_STREN_80;
      gain_95 = TS2000_GAIN_STREN_95;
      break;
    default: //TC2800
    gain_no_signal = 30;
    gain_60 = 34;
    gain_70 = 38;
    gain_80 = 42;
    gain_95 = 45;

      break;
    }

  if(p_signal->lock != 1)
  {
    if(p_signal->agc <= gain_no_signal)		
    {//0%, no signal or weak signal
      strength_per =  0;
    }
    else
    {
      strength_per =  60;
    }
  }
  else
  {
    if(p_signal->agc <= gain_60)
    {
      strength_per =  60;
    }
    else if(p_signal->agc <= gain_70)
    {//60%-70%    normal signal
      strength_per = (u8)(60 + (p_signal->agc - gain_60) * 10 
                             / (gain_70 -gain_60));   
    }
    else if(p_signal->agc <= gain_80)				
    {//70% - 80%   strong signal
      strength_per = (u8)(70 + (p_signal->agc - gain_70) * 10 
                            / (gain_80-gain_70));  
    }
    else
    {//80% - 95%   very strong signal
      strength_per = (u8)(80 + (p_signal->agc - gain_80) * 15 
                            / (gain_95 -gain_80));        
    }
  }
  
  signal_strength[signal_index] = strength_per;   
  signal_index++;
  if(signal_index == AVG_NUM)
  {
  	signal_index = 0;
  }

  sum = 0;
  for (i = 0; i < AVG_NUM; i++)
  {
  	sum = (u16)(sum + signal_strength[i]);
  }
  strength_per = (u8)(sum / AVG_NUM);

  //Scale the display by multiplying the signal strength with a coefficient
  strength_per = (u8)((u16)strength_per * SIGNAL_STRENGTH_RATIO / 100); 

  //Limit the display within 0% to 100% to avoid errors
  if(strength_per > 100)  strength_per = 100;        
  
  p_signal->agc = strength_per;

  /* calculate signal quality */
  snr_0 = SNR_0;
  snr_30 = SNR_30;
  snr_45 = SNR_45;
  snr_60 = SNR_60;
  snr_75 = SNR_75;
  snr_85 = SNR_85;

  if(p_signal->snr <= snr_0)
  {
    snr_per = 0;
  }
  else if(p_signal->snr < snr_30)
  {
    snr_per = (u8)(30*(p_signal->snr-snr_0)/(snr_30-snr_0));
  }
  else if(p_signal->snr < snr_45)
  {
    snr_per = (u8)(30 + 15*(p_signal->snr-snr_30)/(snr_45-snr_30));
  }
  else if(p_signal->snr < snr_60)
  {
    snr_per = (u8)(45 + 15*(p_signal->snr-snr_45)/(snr_60-snr_45));
  }
  else if(p_signal->snr < snr_75)
  {
    snr_per = (u8)(60 + 15*(p_signal->snr-snr_60)/(snr_75-snr_60));
  }
  else if(p_signal->snr <= snr_85)
  {
    snr_per= (u8)(75 + 10*(p_signal->snr-snr_75)/(snr_85-snr_75));
  }
  else
  {
    snr_per = 85;
  }
  
  p_signal->snr = (u32)snr_per;
#endif
  
}

static void convert_perf_to_custom(nim_channel_perf_t *p_signal)
{
#if 0
  static u8 strength_buf = 0;	
  
  if(p_signal->agc == 0)
  {
    p_signal->snr = 0;
    p_signal->ber = 0;
    return;
  }
	
  if(p_signal->lock != 1)
  {
    p_signal->snr = 0;
    p_signal->ber = 0;
  	if(p_signal->agc != 0)
  	{
  	    if(p_signal->agc < 60)
        {
          p_signal->agc = 60;
        } 
  		strength_buf = (u8)(p_signal->agc);
  	}
  	else
  	{
  		if(strength_buf != 0)
  			p_signal->agc = strength_buf;
  	}
  }
  else
  {
    if(STRENGTH_G_THAN_QUALITY)
    {
      while(p_signal->agc < p_signal->snr)
      {
        p_signal->agc += ((p_signal->snr - p_signal->agc) / 10 + 1) * 10;
      }
      if((p_signal->agc - p_signal->snr) < 5)
      {
        p_signal->agc += 5;
      }
    }
  	
  	if(p_signal->agc < 60)
  		p_signal->agc = 60;
  }

  //OS_PRINTF("stren_max[%d], quality_max[%d]\n",
  //    cfg.sig_stren_max, cfg.sig_quality_max);
  if(p_signal->snr > QUALITY_MAX)
  {
    p_signal->snr = QUALITY_MAX;
  }
  else if(p_signal->snr < 5)
  {
    p_signal->snr = 0;
  }
  	
  if(p_signal->agc > STRENGTH_MAX)
  {
    p_signal->agc = STRENGTH_MAX;
  }
  else if(p_signal->agc < 10)
  {
    p_signal->agc = 0;
  }

  p_signal->ber = p_signal->ber;
 #endif
  
}

static void convert_perf(nim_channel_perf_t *p_signal, sig_mon_info_t *p_info)
{
  convert_perf_hw(p_signal);
  convert_perf_to_custom(p_signal);
  p_info->strength = (u8)p_signal->agc;
  p_info->snr = (u8)p_signal->snr;
  p_info->ber_c = p_signal->ber;
}

static void lock_led_set(BOOL is_locked)
{
#ifndef WIN32 
signal_led_display(is_locked);
#endif
}


sig_mon_policy_t * construct_sig_mon_policy(void)
{
  sig_mon_policy_t *p_sig_mon_pol = mtos_malloc(sizeof(sig_mon_policy_t));
  MT_ASSERT(p_sig_mon_pol != NULL);
  memset(p_sig_mon_pol, 0, sizeof(sig_mon_policy_t));

  p_sig_mon_pol->p_convert_perf = convert_perf;
   p_sig_mon_pol->lock_led_set = lock_led_set;
  return p_sig_mon_pol;
}


