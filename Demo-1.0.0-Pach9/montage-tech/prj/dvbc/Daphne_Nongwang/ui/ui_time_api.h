/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef _UI_TIME_API_H
#define _UI_TIME_API_H

void ui_time_init(void);

void ui_time_release(void);

void ui_time_req_tdt(void);

void ui_time_lnb_check_enable(BOOL is_enable);
#endif
