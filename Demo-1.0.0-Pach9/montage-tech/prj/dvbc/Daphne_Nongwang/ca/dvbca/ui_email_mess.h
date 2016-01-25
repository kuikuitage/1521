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

//head
#define EMAIL_HEAD_X        28
#define EMAIL_HEAD_Y        85
#define EMAIL_HEAD_W        600
#define EMAIL_HEAD_H        35

//number
#define EMAIL_NUMB_X        (EMAIL_HEAD_X + 8)
#define EMAIL_NUMB_Y        EMAIL_HEAD_Y
#define EMAIL_NUMB_W       	48
#define EMAIL_NUMB_H        EMAIL_HEAD_H

//new
#define EMAIL_NEW_X         (EMAIL_NUMB_X + EMAIL_NUMB_W)
#define EMAIL_NEW_Y         EMAIL_HEAD_Y
#define EMAIL_NEW_W        34
#define EMAIL_NEW_H         EMAIL_HEAD_H

//name
#define EMAIL_NAME_X        (EMAIL_NEW_X + EMAIL_NEW_W + 30)
#define EMAIL_NAME_Y        EMAIL_HEAD_Y
#define EMAIL_NAME_W        80
#define EMAIL_NAME_H        EMAIL_HEAD_H

//sender
#define EMAIL_SENDER_X       (EMAIL_NAME_X + EMAIL_NAME_W)
#define EMAIL_SENDER_Y       EMAIL_HEAD_Y
#define EMAIL_SENDER_W       220
#define EMAIL_SENDER_H       EMAIL_HEAD_H

//time
#define EMAIL_TIME_X		(EMAIL_SENDER_X+EMAIL_SENDER_W)
#define EMAIL_TIME_Y		EMAIL_HEAD_Y
#define EMAIL_TIME_W		100
#define	EMAIL_TIME_H		EMAIL_HEAD_H

//mail list
#define EMAIL_LIST_X        EMAIL_HEAD_X
#define EMAIL_LIST_Y       (EMAIL_HEAD_Y+EMAIL_HEAD_H+5)
#define EMAIL_LIST_W      (EMAIL_HEAD_W - EMAIL_SBAR_W)
#define EMAIL_LIST_H        300

//mail list rect
#define EMAIL_LIST_MIDL         5
#define EMAIL_LIST_MIDT         5
#define EMAIL_LIST_MIDW        (EMAIL_LIST_W-2*EMAIL_LIST_MIDL)
#define EMAIL_LIST_MIDH         (EMAIL_LIST_H-EMAIL_LIST_MIDT-10)
#define EMAIL_LIST_VGAP         2

//bar
#define EMAIL_SBAR_X       (EMAIL_LIST_X + EMAIL_LIST_W)
#define EMAIL_SBAR_Y       EMAIL_LIST_Y
#define EMAIL_SBAR_W       16
#define EMAIL_SBAR_H       EMAIL_LIST_H

//received head
#define EMAIL_RECEIVED_HEAD_X   40
#define EMAIL_RECEIVED_HEAD_Y   450
#define EMAIL_RECEIVED_HEAD_W   100
#define EMAIL_RECEIVED_HEAD_H   35

//received
#define EMAIL_RECEIVED_X        (EMAIL_RECEIVED_HEAD_X + EMAIL_RECEIVED_HEAD_W)
#define EMAIL_RECEIVED_Y        EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RECEIVED_W        40
#define EMAIL_RECEIVED_H        EMAIL_RECEIVED_HEAD_H

//reset head
#define EMAIL_RESET_HEAD_X      (EMAIL_RECEIVED_X + EMAIL_RECEIVED_W +50)
#define EMAIL_RESET_HEAD_Y      EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_HEAD_W      EMAIL_RECEIVED_HEAD_W
#define EMAIL_RESET_HEAD_H      EMAIL_RECEIVED_HEAD_H

//reset
#define EMAIL_RESET_X          (EMAIL_RESET_HEAD_X+EMAIL_RESET_HEAD_W)
#define EMAIL_RESET_Y          EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_W          40
#define EMAIL_RESET_H          EMAIL_RECEIVED_HEAD_H

//max num head
#define EMAIL_MAX_NUM_HEAD_X      (EMAIL_RESET_X + EMAIL_RESET_W +50)
#define EMAIL_MAX_NUM_HEAD_Y      EMAIL_RECEIVED_HEAD_Y
#define EMAIL_MAX_NUM_HEAD_W      (EMAIL_RECEIVED_HEAD_W + 78)
#define EMAIL_MAX_NUM_HEAD_H      EMAIL_RECEIVED_HEAD_H

//max num 
#define EMAIL_MAX_NUM_X          (EMAIL_MAX_NUM_HEAD_X + EMAIL_MAX_NUM_HEAD_W)
#define EMAIL_MAX_NUM_Y          EMAIL_RECEIVED_HEAD_Y
#define EMAIL_MAX_NUM_W          40
#define EMAIL_MAX_NUM_H          EMAIL_RECEIVED_HEAD_H

#define EMAIL_TEXT_NAME          FSI_WHITE

#define EMAIL_LIST_PAGE         8

//others
#define EMAIL_LIST_FIELD        4//5

//rstyle
#define RSI_EMAIL_CONT         RSI_IGNORE
#define RSI_EMAIL_RECT        RSI_BLUE_GREEN
//fstyle

#define EMAIL_MAX_NUMB          50

RET_CODE open_email_mess(u32 para1, u32 para2);

#endif




