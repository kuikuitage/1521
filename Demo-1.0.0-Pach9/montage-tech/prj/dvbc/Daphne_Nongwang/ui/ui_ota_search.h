/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_OTA_SEARCH_H__
#define __UI_OTA_SEARCH_H__

/* coordinate */
#define OTA_SEARCH_CONT_X        ((SCREEN_WIDTH - OTA_SEARCH_CONT_W)/2)
#define OTA_SEARCH_CONT_Y        RIGHT_BG_Y
#define OTA_SEARCH_CONT_W        320
#define OTA_SEARCH_CONT_H        250

#define OTA_SEARCH_ITEM_X TEXT_START_OX
#define OTA_SEARCH_ITEM_Y RIGHT_BG_ITEM_START_VGAP
#define OTA_SEARCH_ITEM_W 300
#define OTA_SEARCH_ITEM_LW 140
#define OTA_SEARCH_ITEM_RW 140
#define OTA_SEARCH_ITEM_H COMM_BTN_ITEM_H

#define OTA_SEARCH_ITEM_VGAP 5

#define OTA_COLLECT_EXIT_X ((OTA_SEARCH_CONT_W - OTA_COLLECT_EXIT_W)/2)
#define OTA_COLLECT_EXIT_W 80
#define OTA_COLLECT_EXIT_H COMM_BTN_ITEM_H

#define OTA_CHECK_YES_X ((OTA_SEARCH_CONT_W - 2 * OTA_CHECK_YES_W)/3)
#define OTA_CHECK_YES_W 60
#define OTA_CHECK_YES_H COMM_BTN_ITEM_H

#define OTA_CHECK_NO_X  (OTA_SEARCH_CONT_W - OTA_CHECK_YES_X - OTA_CHECK_NO_W)
#define OTA_CHECK_NO_W OTA_CHECK_YES_W
#define OTA_CHECK_NO_H COMM_BTN_ITEM_H

/* rect style */
#define RSI_OTA_SEARCH_FRM       RSI_WINDOW_2

/* font style */
#define FSI_OTA_SEARCH_INFO  FSI_COMM_TXT_N
#define FSI_OTA_SEARCH_BTN FSI_SECOND_TITLE

/* others */
#define OTA_CHECK_ITEM_CNT 3
#define OTA_COLLECT_ITEM_CNT 2

RET_CODE open_ota_search(u32 para1, u32 para2);

#endif
