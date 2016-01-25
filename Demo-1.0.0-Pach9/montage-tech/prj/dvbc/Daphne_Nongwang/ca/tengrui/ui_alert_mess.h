/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_ALERT_MESS__
#define __UI_ALERT_MESS__

/* coordinate */
#define ALERT_MENU_X            0 
#define ALERT_MENU_Y            0
#define ALERT_MENU_W           SCREEN_WIDTH 
#define ALERT_MENU_H            SCREEN_HEIGHT 

//head
#define ALERT_HEAD_X        20
#define ALERT_HEAD_Y        0
#define ALERT_HEAD_W        600
#define ALERT_HEAD_H        31

//number
#define ALERT_NUMB_X        ALERT_HEAD_X+40
#define ALERT_NUMB_Y        ALERT_HEAD_Y
#define ALERT_NUMB_W        60
#define ALERT_NUMB_H        ALERT_HEAD_H

//new
#define ALERT_NEW_X         120
#define ALERT_NEW_Y         ALERT_HEAD_Y
#define ALERT_NEW_W         60
#define ALERT_NEW_H         ALERT_HEAD_H

//name
#define ALERT_NAME_X        ALERT_HEAD_X+ALERT_NUMB_W+150
#define ALERT_NAME_Y        ALERT_HEAD_Y
#define ALERT_NAME_W        100
#define ALERT_NAME_H        ALERT_HEAD_H

//delete
#define ALERT_DELETE_X       ALERT_HEAD_X+ALERT_NUMB_W+ALERT_NAME_W+300
#define ALERT_DELETE_Y       ALERT_HEAD_Y
#define ALERT_DELETE_W       100
#define ALERT_DELETE_H       ALERT_HEAD_H


//mail list
#define ALERT_LIST_X        ALERT_HEAD_X
#define ALERT_LIST_Y       (ALERT_HEAD_Y+ALERT_HEAD_H+5)
#define ALERT_LIST_W        ALERT_HEAD_W
#define ALERT_LIST_H        300

//mail list rect
#define ALERT_LIST_MIDL         5
#define ALERT_LIST_MIDT         5
#define ALERT_LIST_MIDW        (ALERT_LIST_W-2*ALERT_LIST_MIDL)
#define ALERT_LIST_MIDH         (ALERT_LIST_H-ALERT_LIST_MIDT-10)
#define ALERT_LIST_VGAP         2

//bar
#define ALERT_RECEIVED_BAR_X		ALERT_HEAD_X
#define ALERT_RECEIVED_BAR_Y		320
#define ALERT_RECEIVED_BAR_W		ALERT_HEAD_W
#define ALERT_RECEIVED_BAR_H		ALERT_HEAD_H

//received head
#define ALERT_RECEIVED_HEAD_X   80
#define ALERT_RECEIVED_HEAD_Y   ALERT_RECEIVED_BAR_Y
#define ALERT_RECEIVED_HEAD_W   100
#define ALERT_RECEIVED_HEAD_H   35

//received
#define ALERT_RECEIVED_X        (ALERT_RECEIVED_HEAD_X + ALERT_RECEIVED_HEAD_W)
#define ALERT_RECEIVED_Y        ALERT_RECEIVED_HEAD_Y
#define ALERT_RECEIVED_W        60
#define ALERT_RECEIVED_H        ALERT_RECEIVED_HEAD_H

//reset head
#define ALERT_RESET_HEAD_X      (ALERT_RECEIVED_X + ALERT_RECEIVED_W + 100)
#define ALERT_RESET_HEAD_Y      ALERT_RECEIVED_HEAD_Y
#define ALERT_RESET_HEAD_W      130
#define ALERT_RESET_HEAD_H      ALERT_RECEIVED_HEAD_H

//reset
#define ALERT_RESET_X          (ALERT_RESET_HEAD_X+ALERT_RESET_HEAD_W)
#define ALERT_RESET_Y          ALERT_RECEIVED_HEAD_Y
#define ALERT_RESET_W          60
#define ALERT_RESET_H          ALERT_RECEIVED_HEAD_H

#define ALERT_TEXT_NAME          FSI_WHITE_YELLOW

#define ALERT_LIST_PAGE         8

//others
#define ALERT_LIST_FIELD        9

//rstyle
#define RSI_ALERT_CONT         RSI_IGNORE
#define RSI_ALERT_RECT        RSI_BLUE_GREEN
//fstyle

#define ALERT_MAX_NUMB          20

RET_CODE open_alert_mess(u32 para1, u32 para2);

#endif




