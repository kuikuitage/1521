/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_UTIL_API_H__
#define __UI_UTIL_API_H__

void ui_enable_uio(BOOL is_enable);

void ui_set_front_panel_by_str(const char * str);

void ui_set_front_panel_by_num(u16 num);
void ui_show_logo(u8 block_id);

void ui_show_logo_by_data(u32 size, u8 *p_logo_addr);

void ui_enable_video_display(BOOL is_enable);

void ui_config_normal_osd(void);

void ui_set_channel_mode(BOOL b);

#endif

