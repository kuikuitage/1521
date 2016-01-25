/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_NEW_MAIL_H__
#define __UI_NEW_MAIL_H__

/* coordinate */
#define NEW_MAIL_CONT_FULL_X			15//370
#define NEW_MAIL_CONT_FULL_Y			50
#define NEW_MAIL_CONT_FULL_W			70//40
#define NEW_MAIL_CONT_FULL_H			50//35

#define NEW_MAIL_ICON_FULL_X			0
#define NEW_MAIL_ICON_FULL_Y			0
#define NEW_MAIL_ICON_FULL_W			NEW_MAIL_CONT_FULL_W
#define NEW_MAIL_ICON_FULL_H			NEW_MAIL_CONT_FULL_H

BOOL ui_is_new_mail(void);

void ui_set_new_mail(BOOL is_new_mail);

RET_CODE open_ui_new_mail(u32 para1, u32 para2);

void close_new_mail(void);

#endif


