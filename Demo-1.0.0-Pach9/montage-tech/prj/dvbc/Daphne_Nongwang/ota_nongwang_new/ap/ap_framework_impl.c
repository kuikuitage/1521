/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
//#include "sys_cfg.h"
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
#include "mt_time.h"

#include "ap_framework.h"
#include "ap_uio.h"
#include "cat.h"
#include "ap_signal_monitor.h"
#include "dvbc_util.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "sys_status.h"
#include "hal_base.h"
#include "lpower.h"

#include "ap_kingvon_ota.h"
#include "sys_cfg.h"

static void ap_proc(void)
{
#ifndef WIN32
  hal_watchdog_feed();
#endif
}

BOOL ap_get_standby(void)
{
  return TRUE;
}

void ap_set_standby(u32 flag)
{
}


#define REG32(addr)                 (*((volatile u32 *)(addr)))
#define ASM_2NOP \
__asm__ __volatile__ (  \
      "nop \n"    \
      "nop \n"    \
      : : );

void ap_enter_standby(u32 tm_out)
{
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

  p_policy->resotre_to_factory = NULL;
  p_policy->set_standby   =ap_set_standby;
  p_policy->test_uart     = ap_test_uart;
  p_policy->consume_ui_event = ap_consume_ui_event;

  return p_policy;
}

