/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "sys_regs_jazz.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "nit.h"
#include "cat.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "db_dvbs.h"
#include "mt_time.h"

#include "ss_ctrl.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_epg.h"
//#include "ap_satcodx.h"
#include "ap_ota.h"
#include "sys_status.h"
#include "hal_base.h"

#include "smc_op.h"
#include "lpower.h"
#include "customer_def.h"
#include "config_prj.h"
#include "config_cas.h"
#include "config_customer.h"



//static u8 sram_code_data[] = {0};

static void ap_proc(void)
{
 //static BOOL flag = 0;

#ifndef WIN32
  hal_watchdog_feed();

#endif
}

static void ap_restore_to_factory(void)
{
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
}

BOOL ap_get_standby(void)
{
  BOOL ret = FALSE;
#ifndef WIN32
  if ((RESET_STATUS_POWER == hal_pm_get_state())
    &&(0)) // && get statndby flag
  {
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }
  hal_pm_state_clr();
#endif

  return ret;
}

void ap_set_standby(u32 flag)
{
  s32 ret;
  misc_options_t misc;

  if (TRUE == flag) //set standby flag
  {
    void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);

    ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0,
      sizeof(misc_options_t), (u8 *)&misc);
    if (ret <= 0) //if customer configuration is not found, ignore
    {
      OS_PRINTF("custom config is not found!\n");
      return;
    }
    if (0 == misc.standby_mode) // if standby mode is not set, ignore
    {
      OS_PRINTF("Entering standby mode after cold reset is not support!\n");
      return;
    }

    //set flag here
    //?????
  }
  else //clear standby flag
  {
    //?????
  }
}

#define M88RS2000_TUNER_ADDRESS    0xC0
#define M88RS2000_DEMOD_ADDRESS    0xD0

#define STANDBY_DELAY_TM           50 // actual delay 100ms
#define STANDBY_CHK_TMOUT          (STANDBY_DELAY_TM * 2)
#define STANDBY_TM_UPDATE           ((6 * 100 )/STANDBY_CHK_TMOUT) // 1 minute

#define REG32(addr)                 (*((volatile u32 *)(addr)))
#define ASM_2NOP \
__asm__ __volatile__ (  \
      "nop \n"    \
      "nop \n"    \
      : : );

extern u8 get_powerkey_value_to_standy(u8 mode);
void ap_enter_standby(u32 tm_out)
{
#ifndef WIN32
  BOOL ret = FALSE;
  void *p_dev = NULL;
  lpower_cfg_t lpower_cfg = {0};
  // user config this
  u32 nothing;
  struct nim_device_t* p_nim = NULL ;
  aud_device_vsb_t * audio_dev = NULL;
  void * p_video_dev = NULL;
  void * p_disp_dev = NULL;
  u8 tmp[4] = {' ', ' ',' ',' '};
  uio_device_t *uio_dev = NULL;
  class_handle_t avc_handle = NULL;
  //u32 test = 0;
  //test = sizeof(sram_code_data);


  OS_PRINTF("@@@Enter real standby mode!ddddd\n");
  mtos_task_lock();
  power_off_led_display();
  OS_PRINTF("@@@Enter real standby mode!\n");

    audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    MT_ASSERT(NULL != audio_dev);
    aud_mute_onoff_vsb(audio_dev,TRUE);
    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                                            SYS_DEV_TYPE_VDEC_VSB);
    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                                                  SYS_DEV_TYPE_DISPLAY);
    vdec_stop(p_video_dev);
    disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

    /**display off**/
    avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
    avc_cfg_video_out_format_jazz(avc_handle,VIDEO_OUT_VID_NULL);
        
    uio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    uio_display(uio_dev, tmp, 4);

    //for lpower ir
    dev_io_ctrl(uio_dev, UIO_IR_SET_WAVEFILT, 0);
    
    /**sleep tuner on standby**/
    p_nim = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_NIM);
    dev_io_ctrl(p_nim, NIM_IOCTRL_SET_TUNER_SLEEP, (u32)&nothing);

    /**mute by hardware**/
    set_volume_mute_by_hardware(1);

  //Attach driver
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_POW);
  MT_ASSERT(NULL != p_dev);
   standby_config(&lpower_cfg);

  OS_PRINTF("@@@@@Enter standby new mode!!! \n");


  ret = dev_open(p_dev, &lpower_cfg);

  if(CUSTOMER_DEFAULT != lpower_cfg.customer)
  {
    ap_lpower_enter(p_dev);
  }
  
   #if (FRONT_BOARD == FRONT_BOARD_DEMO)
   ret = ap_lpower_ioctl(p_dev,  FP_DISPLAY_LOCAL_TIME, 0);
   #else
    ret = ap_lpower_ioctl(p_dev,  KEY_WAKE_UP, 0);
   #endif

  hal_pm_reset();
  mtos_task_unlock();
#endif
}


void ap_test_uart(void)
{

}

static BOOL ap_consume_ui_event(u32 ui_state, os_msg_t *p_msg)
{
  //temp solution, fix me andy.chen
  if((ui_state != 0)  //ui is waiting sync ack
    && (APP_UIO == ((p_msg->content >> 16) & 0xFF))) //key evt
  {
    //OS_PRINTF("UI is waiting for ack, miss key...\n");
    return TRUE;
  }

  return FALSE;
}

/*!
  Construct APP. framework policy
  */
ap_frm_policy_t* construct_ap_frm_policy(void)
{
	ap_frm_policy_t *p_policy = mtos_malloc(sizeof(ap_frm_policy_t));
  MT_ASSERT(p_policy != NULL);

  p_policy->enter_standby = ap_enter_standby;
  p_policy->extand_proc   = ap_proc;
  p_policy->is_standby    = ap_get_standby;

  p_policy->resotre_to_factory = ap_restore_to_factory;
  p_policy->set_standby   = ap_set_standby;
  p_policy->test_uart     = ap_test_uart;
  p_policy->consume_ui_event = ap_consume_ui_event;

  return p_policy;
}


