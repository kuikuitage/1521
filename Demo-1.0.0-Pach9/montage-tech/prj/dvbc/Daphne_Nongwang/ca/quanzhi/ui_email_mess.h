/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_EMAIL_MESS__
#define __UI_EMAIL_MESS__

/* coordinate */
#define EMAIL_MENU_X            0 
#define EMAIL_MENU_Y            0
#define EMAIL_MENU_W           SCREEN_WIDTH 
#define EMAIL_MENU_H            SCREEN_HEIGHT 

//rect
#define EMAIL_RECT_X        0
#define EMAIL_RECT_Y        50
#define EMAIL_RECT_W       SCREEN_WIDTH
#define EMAIL_RECT_H        (SCREEN_HEIGHT-100)//456

//head
#define EMAIL_HEAD_X        10
#define EMAIL_HEAD_Y        10
#define EMAIL_HEAD_W       620
#define EMAIL_HEAD_H        31

//number
#define EMAIL_NUMB_X        (EMAIL_HEAD_X+10)
#define EMAIL_NUMB_Y        EMAIL_HEAD_Y
#define EMAIL_NUMB_W       50
#define EMAIL_NUMB_H        EMAIL_HEAD_H

//date
#define EMAIL_DATE_X        (EMAIL_HEAD_X+EMAIL_HEAD_W-160)
#define EMAIL_DATE_Y       EMAIL_HEAD_Y
#define EMAIL_DATE_W       160
#define EMAIL_DATE_H       EMAIL_HEAD_H

//import
#define EMAIL_IMPORT_X       (EMAIL_DATE_X-100)
#define EMAIL_IMPORT_Y        EMAIL_HEAD_Y
#define EMAIL_IMPORT_W       100
#define EMAIL_IMPORT_H        EMAIL_HEAD_H

//new
#define EMAIL_NEW_X        (EMAIL_IMPORT_X-60)
#define EMAIL_NEW_Y        EMAIL_HEAD_Y
#define EMAIL_NEW_W       60
#define EMAIL_NEW_H        EMAIL_HEAD_H

//title
#define EMAIL_TITLE_X        (EMAIL_NEW_X - 145)
#define EMAIL_TITLE_Y        EMAIL_HEAD_Y
#define EMAIL_TITLE_W       60
#define EMAIL_TITLE_H       EMAIL_HEAD_H

//plist
#define EMAIL_LIST_X		EMAIL_HEAD_X
#define EMAIL_LIST_Y       (EMAIL_HEAD_Y+EMAIL_HEAD_H)//41
#define EMAIL_LIST_W		EMAIL_HEAD_W
#define EMAIL_LIST_H        180

//plist rect
#define EMAIL_LIST_MIDL         5
#define EMAIL_LIST_MIDT         5
#define EMAIL_LIST_MIDW        (EMAIL_LIST_W - EMAIL_LIST_MIDL * 2)
#define EMAIL_LIST_MIDH         (EMAIL_LIST_H - EMAIL_LIST_MIDT * 2)
#define EMAIL_LIST_VGAP         2

//list scroll bar
#define EMAIL_SBAR_X       (EMAIL_LIST_X + EMAIL_LIST_W + EMAIL_LIST_VGAP)
#define EMAIL_SBAR_Y       EMAIL_LIST_Y
#define EMAIL_SBAR_W       16
#define EMAIL_SBAR_H       EMAIL_LIST_H

//content bar
#define EMAIL_CONTENT_BAR_X      EMAIL_HEAD_X
#define EMAIL_CONTENT_BAR_Y      (EMAIL_LIST_Y + EMAIL_LIST_H)
#define EMAIL_CONTENT_BAR_W     EMAIL_HEAD_W
#define EMAIL_CONTENT_BAR_H      EMAIL_HEAD_H

//content
#define EMAIL_TEXT_X        EMAIL_HEAD_X//10
#define EMAIL_TEXT_Y        (EMAIL_CONTENT_BAR_Y + EMAIL_CONTENT_BAR_H-10)
#define EMAIL_TEXT_W       EMAIL_HEAD_W
#define EMAIL_TEXT_H        EMAIL_LIST_H-60


#define EMAIL_LIST_PAGE         5

//others

#define EMAIL_LIST_FIELD        4






//rstyle
#define RSI_EMAIL_CONT         RSI_IGNORE
#define RSI_EMAIL_RECT        RSI_BLUE_GREEN
//fstyle



RET_CODE open_email_mess(u32 para1, u32 para2);

#endif




