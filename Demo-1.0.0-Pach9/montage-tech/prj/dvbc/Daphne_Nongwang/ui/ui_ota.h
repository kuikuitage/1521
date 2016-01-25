/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_OTA_H__
#define __UI_OTA_H__

/* coordinate */
#define OTA_CONT_X        ((SCREEN_WIDTH - OTA_CONT_W) / 2)
#define OTA_CONT_Y        COMM_ROOT_Y
#define OTA_CONT_W        COMM_ROOT_W
#define OTA_CONT_H        360

#define OTA_ITEM_CNT      7
#define OTA_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define OTA_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define OTA_ITEM_LW       COMM_ITEM_LW
#define OTA_ITEM_RW       (COMM_ITEM_MAX_WIDTH - OTA_ITEM_LW)
#define OTA_ITEM_H        COMM_ITEM_H
#define OTA_ITEM_V_GAP    2

/* rect style */
#define RSI_OTA_FRM       RSI_WINDOW_1

/* font style */

/* others */
#define OTA_FREQ_MIN 0
#define OTA_FREQ_MAX 99999
#define OTA_FREQ_BIT  5

#define OTA_SYM_MIN 0
#define OTA_SYM_MAX 99999
#define OTA_SYM_BIT 5

#define OTA_PID_MIN 0
#define OTA_PID_MAX 17777//0x1FFF
#define OTA_PID_BIT 5

RET_CODE open_ota(u32 para1, u32 para2);

#endif
