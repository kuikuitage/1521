/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_MANUAL_UPGRADE_H__
#define __UI_MANUAL_UPGRADE_H__

/* coordinate */
#define MANUAL_UPG_MENU_X          ((SCREEN_WIDTH - MANUAL_UPG_MENU_W) / 2)
#define MANUAL_UPG_MENU_Y          ((SCREEN_HEIGHT - MANUAL_UPG_MENU_H) / 2)
#define MANUAL_UPG_MENU_W          SCREEN_WIDTH
#define MANUAL_UPG_MENU_H          SCREEN_HEIGHT

//small cont 1
#define MANUAL_UPG_CONT1_X  RIGHT_BG_X
#define MANUAL_UPG_CONT1_Y  RIGHT_BG_Y
#define MANUAL_UPG_CONT1_W  RIGHT_BG_W
#define MANUAL_UPG_CONT1_H  110

//brief
#define MANUAL_UPG_BRIEF_X  4
#define MANUAL_UPG_BRIEF_Y  2
#define MANUAL_UPG_BRIEF_W  (MANUAL_UPG_CONT1_W - MANUAL_UPG_BRIEF_X*2)
#define MANUAL_UPG_BRIEF_H  (MANUAL_UPG_CONT1_H - MANUAL_UPG_BRIEF_Y*2)

//small cont 2
#define MANUAL_UPG_CONT2_X  RIGHT_BG_X
#define MANUAL_UPG_CONT2_Y  (MANUAL_UPG_CONT1_Y + MANUAL_UPG_CONT1_H + 10)
#define MANUAL_UPG_CONT2_W  RIGHT_BG_W
#define MANUAL_UPG_CONT2_H  (RIGHT_BG_H - MANUAL_UPG_CONT1_H - 10)

#define MANUAL_UPG_ITEM_CNT      5
#define MANUAL_UPG_ITEM_X        (MANUAL_UPG_CONT1_X + 10)
#define MANUAL_UPG_ITEM_Y        (MANUAL_UPG_CONT2_Y + MANUAL_UPG_ITEM_V_GAP)
#define MANUAL_UPG_ITEM_LW       120
#define MANUAL_UPG_ITEM_MW       100
#define MANUAL_UPG_ITEM_RW       60
#define MANUAL_UPG_ITEM_H        30
#define MANUAL_UPG_ITEM_V_GAP    10
#define PARAM_OTA_SAVE_X        100
#define PARAM_OTA_SAVE_Y        260

/* rect style */
#define RSI_MANUAL_UPG_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE preopen_manual_upgrade(u32 para1, u32 para2);

RET_CODE open_manual_upgrade(u32 para1, u32 para2);

#endif


