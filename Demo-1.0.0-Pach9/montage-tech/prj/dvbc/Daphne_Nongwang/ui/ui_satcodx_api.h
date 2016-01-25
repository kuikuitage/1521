/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Steven Wang <steven.wang@montage-tech.com>
****************************************************************************/
#ifndef __UI_SATCODX_API_H__
#define __UI_SATCODX_API_H__

enum satcodx_msg
{
	MSG_SATCODX_UPDATE_STATUS = MSG_EXTERN_BEGIN + 350,	
};

void ui_init_satcodx(void);

void ui_release_satcodx(void);

void ui_start_satcodx(void);

void ui_stop_satcodx(void);

#endif


