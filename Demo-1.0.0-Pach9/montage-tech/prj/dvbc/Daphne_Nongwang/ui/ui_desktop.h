/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/ 
#ifndef __UI_DESKTOP_H__
#define __UI_DESKTOP_H__

#define MOV_DLG_TMOUT (20*SECOND)

#define OUTRANGE_DLG_TMOUT (5*SECOND)

#define OUTRANGE_TMOUT (2*SECOND)

typedef enum OSD_SHOW_POS
{
	OSD_SHOW_TOP = 0,					//在顶部显示
	OSD_SHOW_BOTTOM,					//在底部显示
	OSD_SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
}OSD_SHOW_POS;

BOOL ui_get_disable_display_time_status(void);
void ui_set_disable_display_time_status(void);
void ui_desktop_init (void);
void ui_desktop_start(void);
void ui_desktop_release(void);

void ui_desktop_main(void);

void ui_sleep_timer_create(void);

void ui_sleep_timer_destory(void);

void ui_sleep_timer_reset(void);

void uio_reset_sleep_timer(void);

BOOL ui_desktop_key_disable(u16 vkey);

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2);

void set_uiokey_status(BOOL is_disable);

BOOL get_uiokey_stayus(void);

void ui_set_finger_show(BOOL is_show);
  
BOOL ui_is_finger_show(void);

RET_CODE open_finger(u32 para1, u32 para2);

BOOL osd_roll_stop_msg_unsend(void);

BOOL get_ca_msg_rolling_status(void);

void set_ca_msg_rolling_status(BOOL is_busy);

BOOL get_force_channel_status(void);

BOOL bmp_pic_draw(control_t *p_ctrl);

#endif

