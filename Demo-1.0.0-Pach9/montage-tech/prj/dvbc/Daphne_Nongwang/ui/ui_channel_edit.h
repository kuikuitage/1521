/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CHANNEL_EDIT_H__
#define __UI_CHANNEL_EDIT_H__

/* coordinate */
#define CHAN_MENU_X            0 
#define CHAN_MENU_Y            0
#define CHAN_MENU_W           SCREEN_WIDTH 
#define CHAN_MENU_H            SCREEN_HEIGHT 

//head cont
#define CHAN_HEAD_X        (WHOLE_SCR_TITLE_X - SCREEN_OFFSET_X)
#define CHAN_HEAD_Y        WHOLE_SCR_TITLE_Y
#define CHAN_HEAD_W       WHOLE_SCR_TITLE_W
#define CHAN_HEAD_H        WHOLE_SCR_TITLE_H

//head cont: number
#define CHAN_NUMB_X        5
#define CHAN_NUMB_Y        0
#define CHAN_NUMB_W       60+20
#define CHAN_NUMB_H        CHAN_HEAD_H

//head cont: name
#define CHAN_NAME_X        (CHAN_NUMB_X+CHAN_NUMB_W)
#define CHAN_NAME_Y        CHAN_NUMB_Y
#define CHAN_NAME_W       100
#define CHAN_NAME_H       CHAN_HEAD_H

//head cont: order
#define CHAN_ORDER_X        (CHAN_HEAD_W-60)
#define CHAN_ORDER_Y        CHAN_NUMB_Y
#define CHAN_ORDER_W       60
#define CHAN_ORDER_H        CHAN_HEAD_H

//head cont: f4
#define CHAN_FORDER_X        (CHAN_ORDER_X-25)
#define CHAN_FORDER_Y        CHAN_NUMB_Y
#define CHAN_FORDER_W       25
#define CHAN_FORDER_H        CHAN_HEAD_H

//head cont: hide
#define CHAN_HIDE_X        (CHAN_FORDER_X-45)
#define CHAN_HIDE_Y        CHAN_NUMB_Y
#define CHAN_HIDE_W       45
#define CHAN_HIDE_H        CHAN_HEAD_H

//head cont: f3
#define CHAN_FHIDE_X        (CHAN_HIDE_X-25)
#define CHAN_FHIDE_Y        CHAN_NUMB_Y
#define CHAN_FHIDE_W       25
#define CHAN_FHIDE_H        CHAN_HEAD_H

//head cont: frequ
#define CHAN_FREQ_X       (CHAN_FHIDE_X-50)
#define CHAN_FREQ_Y        CHAN_NUMB_Y
#define CHAN_FREQ_W       50
#define CHAN_FREQ_H        CHAN_HEAD_H

//head cont: f2
#define CHAN_FFREQ_X        (CHAN_FREQ_X-25)
#define CHAN_FFREQ_Y        CHAN_NUMB_Y
#define CHAN_FFREQ_W       25
#define CHAN_FFREQ_H        CHAN_HEAD_H

//head cont: fav
#define CHAN_FAV_X        (CHAN_FFREQ_X-50)
#define CHAN_FAV_Y        CHAN_NUMB_Y
#define CHAN_FAV_W       50
#define CHAN_FAV_H       CHAN_HEAD_H

//head cont: f1
#define CHAN_FFAV_X        (CHAN_FAV_X-25)
#define CHAN_FFAV_Y        CHAN_NUMB_Y
#define CHAN_FFAV_W       25
#define CHAN_FFAV_H         CHAN_HEAD_H


//prog list
#define CHAN_LIST_X       CHAN_HEAD_X
#define CHAN_LIST_Y       RIGHT_BG_Y
#define CHAN_LIST_W      CHAN_HEAD_W
#define CHAN_LIST_H       RIGHT_BG_H

//plist mid rect
#define CHAN_LIST_MIDL   4
#define CHAN_LIST_MIDT   5
#define CHAN_LIST_MIDW  (CHAN_LIST_W-CHAN_LIST_MIDL*2-CHAN_BAR_W)
#define CHAN_LIST_MIDH   (CHAN_LIST_H-CHAN_LIST_MIDT*2)
#define CHAN_LIST_VGAP   2

//bar
#define CHAN_BAR_X       (CHAN_LIST_X + CHAN_LIST_W - CHAN_BAR_W - 2)
#define CHAN_BAR_Y       (CHAN_LIST_Y + CHAN_LIST_MIDT)
#define CHAN_BAR_W      COMM_LIST_BAR_W
#define CHAN_BAR_H       CHAN_LIST_MIDH

#define NUMB_EXCHAN_X              30
#define NUMB_EXCHAN_Y              380

//lvcm
#define SN_NUMB_EXCHAN_X              200
#define SN_NUMB_EXCHAN_Y              180
//lvcm end

#define CHAN_LIST_PAGE        8//11
#define CHAN_LIST_FIELD       5

//rstyle
#define RSI_CHAN_CONT         RSI_IGNORE
//#define RSI_CHAN_RECT        RSI_BLUE_GREEN
//fstyle
#define FSI_CHAN_NUMB          FSI_SECOND_TITLE

RET_CODE open_channel_edit(u32 para1, u32 para2);

#endif


