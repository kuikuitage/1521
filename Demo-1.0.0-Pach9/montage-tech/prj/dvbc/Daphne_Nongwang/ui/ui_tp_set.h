/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_TP_SET_H__
#define __UI_TP_SET_H__

/* coordinate */
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

#define TP_SET_MENU_X          ((SN_SCREEN_WIDTH - TP_SET_MENU_W) / 2)
#define TP_SET_MENU_Y          ((SN_SCREEN_HEIGHT - TP_SET_MENU_H) / 2)
#define TP_SET_MENU_W          SN_SCREEN_WIDTH
#define TP_SET_MENU_H          SN_SCREEN_HEIGHT

//tv main tp set:cont
#define TP_SET_CONT1_X  RIGHT_BG_X
#define TP_SET_CONT1_Y  RIGHT_BG_Y
#define TP_SET_CONT1_W  RIGHT_BG_W
#define TP_SET_CONT1_H  (RIGHT_BG_H/2 - 5)

//tv name
#define TP_SET_TV_X  (TP_SET_CONT1_X + 10)
#define TP_SET_TV_Y  (TP_SET_CONT1_Y + 6)
#define TP_SET_TV_W  (TP_SET_CONT1_W - 20)
#define TP_SET_TV_H  TP_SET_ITEM_H

//NVOD main tp set:cont
#define TP_SET_CONT2_X  RIGHT_BG_X
#define TP_SET_CONT2_Y  (TP_SET_CONT1_Y + TP_SET_CONT1_H + 10)
#define TP_SET_CONT2_W  SECOND_TITLE_W
#define TP_SET_CONT2_H  TP_SET_CONT1_H

//nvod name
#define TP_SET_NVOD_X  TP_SET_TV_X
#define TP_SET_NVOD_Y  (TP_SET_CONT2_Y + 6)
#define TP_SET_NVOD_W  TP_SET_TV_W
#define TP_SET_NVOD_H  TP_SET_ITEM_H

#define TP_SET_ITEM_CNT      6
#define TP_SET_ITEM_X        TP_SET_TV_X
#define TP_SET_ITEM_Y        (TP_SET_TV_Y + TP_SET_TV_H + TP_SET_ITEM_V_GAP)
#define TP_SET_ITEM_LW       120
#define TP_SET_ITEM_MW       100
#define TP_SET_ITEM_RW       60


#define TP_SET_ITEM_H        30
#define TP_SET_ITEM_V_GAP    2

#define PARAM_NUMB_SAVE_X        100
#define PARAM_NUMB_SAVE_Y        260

//preview window
#define TP_PREV_X     36
#define TP_PREV_Y     265
#define TP_PREV_W     245
#define TP_PREV_H     200

/* rect style */

/* font style */

/* others */

RET_CODE preopen_tp_set(u32 para1, u32 para2);

RET_CODE open_tp_set(u32 para1, u32 para2);

#endif


