/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_EMAIL_CONTENT_MESS__
#define __UI_EMAIL_CONTENT_MESS__

/* coordinate */
#define EMAIL_CONTENT_MENU_X            0 
#define EMAIL_CONTENT_MENU_Y            0
#define EMAIL_CONTENT_MENU_W           SCREEN_WIDTH 
#define EMAIL_CONTENT_MENU_H            SCREEN_HEIGHT 

//head
#define EMAIL_CONTENT_HEAD_TITLE_X        40
#define EMAIL_CONTENT_HEAD_TITLE_Y        85
#define EMAIL_CONTENT_HEAD_TITLE_W        300
#define EMAIL_CONTENT_HEAD_TITLE_H        35

//send time
#define EMAIL_CONTENT_HEAD_TIME_X        (EMAIL_CONTENT_HEAD_TITLE_X + EMAIL_CONTENT_HEAD_TITLE_W)
#define EMAIL_CONTENT_HEAD_TIME_Y        85
#define EMAIL_CONTENT_HEAD_TIME_W        300
#define EMAIL_CONTENT_HEAD_TIME_H        35

//MAIL CONTENT BG
#define EMAIL_CONTENT_BG_X        EMAIL_CONTENT_HEAD_TITLE_X
#define EMAIL_CONTENT_BG_Y        (EMAIL_CONTENT_HEAD_TITLE_Y+EMAIL_CONTENT_HEAD_TITLE_H+5)
#define EMAIL_CONTENT_BG_W        (EMAIL_CONTENT_HEAD_TITLE_W + EMAIL_CONTENT_HEAD_TIME_W - 18)
#define EMAIL_CONTENT_BG_H        300

//MAIL CONTENT
#define EMAIL_CONTENT_X        20
#define EMAIL_CONTENT_Y        40
#define EMAIL_CONTENT_W        (EMAIL_CONTENT_BG_W - 40)
#define EMAIL_CONTENT_H        (EMAIL_CONTENT_BG_H - 90)

//MAIL SBAR
#define EMAIL_CONTENT_SBAR_X        (EMAIL_CONTENT_BG_X + EMAIL_CONTENT_BG_W + 2)
#define EMAIL_CONTENT_SBAR_Y        EMAIL_CONTENT_BG_Y
#define EMAIL_CONTENT_SBAR_W        16
#define EMAIL_CONTENT_SBAR_H        EMAIL_CONTENT_BG_H

#define EMAIL_CONTENT_TEXT_NAME          FSI_WHITE

#define EMAIL_CONTENT_LIST_PAGE         8

//others
#define EMAIL_CONTENT_LIST_FIELD        5

//rstyle
#define RSI_EMAIL_CONTENT_CONT         RSI_IGNORE
#define RSI_EMAIL_CONTENT_RECT        RSI_BLUE_GREEN
//fstyle

#define EMAIL_CONTENT_MAX_NUMB          20

RET_CODE open_email_mess_content(u32 para1, u32 para2);

#endif




