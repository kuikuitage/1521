/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_TVSYS_SET_H__
#define __UI_TVSYS_SET_H__

/* coordinate */
#define TVSYS_SET_CONT_X        ((SCREEN_WIDTH - TVSYS_SET_CONT_W) / 2)
#define TVSYS_SET_CONT_Y        COMM_ROOT_Y
#define TVSYS_SET_CONT_W        COMM_ROOT_W
#define TVSYS_SET_CONT_H        420

#define TVSYS_SET_ITEM_CNT      5
#define TVSYS_SET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define TVSYS_SET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define TVSYS_SET_ITEM_LW       COMM_ITEM_LW
#define TVSYS_SET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - TVSYS_SET_ITEM_LW)
#define TVSYS_SET_ITEM_H        COMM_ITEM_H
#define TVSYS_SET_ITEM_V_GAP    2

/* rect style */
#define RSI_TVSYS_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_tvsys_set(u32 para1, u32 para2);

#endif


