/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/ 
#ifndef __UI_DESKTOP_H__
#define __UI_DESKTOP_H__

struct hotkey_root{
	u8 menu_id;
	u16 vkey;
};

enum desktop_msg
{
  MSG_DST_RESTORE_FINISH = MSG_EXTERN_BEGIN + 10,
};

void ui_desktop_init (void);

//void ui_desktop_notify(u16 event, u32 param, u32 context);

//void ui_desktop_restore_to_factory(void);
void ui_desktop_main(void);
#endif

