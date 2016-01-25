/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_MAINMENU_H__
#define __UI_MAINMENU_H__

/* coordinate */
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

#define MAINMENU_CONT_X          ((SN_SCREEN_WIDTH - MAINMENU_CONT_W) / 2)
#define MAINMENU_CONT_Y          ((SN_SCREEN_HEIGHT - MAINMENU_CONT_H) / 2)
#define MAINMENU_CONT_W          SN_SCREEN_WIDTH
#define MAINMENU_CONT_H          SN_SCREEN_HEIGHT

#define MAINMENU_CNT             6//8//12


#define MAINMENU_PREV_X     (((SN_SCREEN_WIDTH - MAINMENU_PREV_W) / 2))// - SCREEN_OFFSET_X)
#define MAINMENU_PREV_Y     0//110//105//110
#define MAINMENU_PREV_W     300//320//235//250
#define MAINMENU_PREV_H     230//240//235//206

#define MAINMENU_AD_X     MAINMENU_PREV_X
#define MAINMENU_AD_Y     (MAINMENU_PREV_Y + MAINMENU_PREV_H + 25)//(MAINMENU_PREV_Y + MAINMENU_PREV_H + 18)
#define MAINMENU_AD_W     MAINMENU_PREV_W
#define MAINMENU_AD_H     110//112

#define MAINMENU_BTN_PREV_GAP   33//16

#define MAINMENU_BTN_X_COL1  (MAINMENU_PREV_X - MAINMENU_BTN_W- MAINMENU_BTN_PREV_GAP)
#define MAINMENU_BTN_X_COL2  (MAINMENU_PREV_X + MAINMENU_PREV_W + MAINMENU_BTN_PREV_GAP)
#define MAINMENU_BTN_Y            0//150//126//106
#define MAINMENU_BTN_W           111//170
#define MAINMENU_BTN_H            110//100//58


#define MAINMENU_BTN_V_GAP       18

/* rect style */

/* font style */

/* others */
#define MAINMENU_SUBENTRY_CNT    9

RET_CODE open_main_menu(u32 para1, u32 para2);

#endif

