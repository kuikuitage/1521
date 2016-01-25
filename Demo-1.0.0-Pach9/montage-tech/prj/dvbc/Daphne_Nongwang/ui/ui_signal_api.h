/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_SIGNAL_API_H__
#define __UI_SIGNAL_API_H__

#define DVB_C_TC2800_STRENGTH_MAX 120
enum signal_msg
{
  MSG_SIGNAL_CHECK = MSG_EXTERN_BEGIN + 300,
  MSG_SIGNAL_UPDATE,
  MSG_SIGNAL_UNLOCK,
  MSG_SIGNAL_LOCK,
  MSG_SIGNAL_WEAK,
  
  MSG_MOTOR_DRVING,
  MSG_MOTOR_OUT_RANGE,
};

struct signal_data
{
	u8 intensity;
	u8 quality;
    u8 ber;    
    double ber_c;  
	BOOL lock;
};

void ui_init_signal(void);

void ui_enable_signal_monitor(BOOL is_enable);

void ui_release_signal(void);

BOOL ui_signal_is_lock(void);

void ui_signal_check(u32 para1, u32 para2);

void ui_set_transpond(dvbs_tp_node_t *p_tp);

void ui_signal_set_lock(BOOL is_lock);

#endif



