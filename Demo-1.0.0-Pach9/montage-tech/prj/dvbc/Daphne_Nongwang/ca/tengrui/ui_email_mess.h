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
#define EMAIL_MENU_W           640 
#define EMAIL_MENU_H            480 

//head
#define EMAIL_HEAD_X        20
#define EMAIL_HEAD_Y        0
#define EMAIL_HEAD_W        600
#define EMAIL_HEAD_H        31

//number
#define EMAIL_NUMB_X        EMAIL_HEAD_X+10
#define EMAIL_NUMB_Y        EMAIL_HEAD_Y
#define EMAIL_NUMB_W        60
#define EMAIL_NUMB_H        EMAIL_HEAD_H

//new
#define EMAIL_NEW_X         EMAIL_NUMB_X+80
#define EMAIL_NEW_Y         EMAIL_HEAD_Y
#define EMAIL_NEW_W         60
#define EMAIL_NEW_H         EMAIL_HEAD_H

//name
#define EMAIL_NAME_X        EMAIL_NEW_X+120
#define EMAIL_NAME_Y        EMAIL_HEAD_Y
#define EMAIL_NAME_W        100
#define EMAIL_NAME_H        EMAIL_HEAD_H

//importance
#define EMAIL_IMPORTANCE_X        EMAIL_NAME_X+180
#define EMAIL_IMPORTANCE_Y        EMAIL_HEAD_Y
#define EMAIL_IMPORTANCE_W        120
#define EMAIL_IMPORTANCE_H        EMAIL_HEAD_H

//delete
#define EMAIL_DELETE_X       EMAIL_IMPORTANCE_X+120
#define EMAIL_DELETE_Y       EMAIL_HEAD_Y
#define EMAIL_DELETE_W       80
#define EMAIL_DELETE_H       EMAIL_HEAD_H


//mail list
#define EMAIL_LIST_X        EMAIL_HEAD_X
#define EMAIL_LIST_Y       (EMAIL_HEAD_Y+35+5)
#define EMAIL_LIST_W        EMAIL_HEAD_W
#define EMAIL_LIST_H        270//300

//mail list rect
#define EMAIL_LIST_MIDL         5
#define EMAIL_LIST_MIDT         5
#define EMAIL_LIST_MIDW        (EMAIL_LIST_W-2*EMAIL_LIST_MIDL)
#define EMAIL_LIST_MIDH         (EMAIL_LIST_H-EMAIL_LIST_MIDT)
#define EMAIL_LIST_VGAP         2

//bar
#define EMAIL_BAR_X				EMAIL_HEAD_X
#define EMAIL_BAR_Y				320//350
#define EMAIL_BAR_W				EMAIL_HEAD_W
#define EMAIL_BAR_H				31

//received head
#define EMAIL_RECEIVED_HEAD_X   40
#define EMAIL_RECEIVED_HEAD_Y   EMAIL_BAR_Y
#define EMAIL_RECEIVED_HEAD_W   100
#define EMAIL_RECEIVED_HEAD_H   EMAIL_BAR_H

//received
#define EMAIL_RECEIVED_X        (EMAIL_RECEIVED_HEAD_X + EMAIL_RECEIVED_HEAD_W)
#define EMAIL_RECEIVED_Y        EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RECEIVED_W        60
#define EMAIL_RECEIVED_H        EMAIL_RECEIVED_HEAD_H

//reset head
#define EMAIL_RESET_HEAD_X      (EMAIL_RECEIVED_X + EMAIL_RECEIVED_W +100)
#define EMAIL_RESET_HEAD_Y      EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_HEAD_W      130
#define EMAIL_RESET_HEAD_H      EMAIL_RECEIVED_HEAD_H

//reset
#define EMAIL_RESET_X          (EMAIL_RESET_HEAD_X+EMAIL_RESET_HEAD_W)
#define EMAIL_RESET_Y          EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_W          60
#define EMAIL_RESET_H          EMAIL_RECEIVED_HEAD_H

#define EMAIL_TEXT_NAME          FSI_WHITE_YELLOW

#define EMAIL_LIST_PAGE         8

//others
#define EMAIL_LIST_FIELD        9

//rstyle
#define RSI_EMAIL_CONT         RSI_IGNORE
#define RSI_EMAIL_RECT        RSI_BLUE_GREEN
//fstyle

#define EMAIL_MAX_NUMB          20

RET_CODE open_email_mess(u32 para1, u32 para2);

#endif




