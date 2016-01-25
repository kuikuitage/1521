/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_SENIOR_SUBMENU_H__
#define __UI_SENIOR_SUBMENU_H__

/* coordinate */
#define SUBMENU_CONT_X          ((SCREEN_WIDTH - SUBMENU_CONT_W) / 2)
#define SUBMENU_CONT_Y          ((SCREEN_HEIGHT - SUBMENU_CONT_H) / 2)
#define SUBMENU_CONT_W          SCREEN_WIDTH
#define SUBMENU_CONT_H          SCREEN_HEIGHT

//advertisement window
#define SUBMENU_AD_X     SUB_AD_X
#define SUBMENU_AD_Y     SUB_AD_Y
#define SUBMENU_AD_W    SUB_AD_W
#define SUBMENU_AD_H     SUB_AD_H

//preview window
#define SUBMENU_PREV_X     SUB_PREV_X
#define SUBMENU_PREV_Y     SUB_PREV_Y
#define SUBMENU_PREV_W    SUB_PREV_W
#define SUBMENU_PREV_H     SUB_PREV_H

//second title
#define SUBMENU_SECOND_TITLE_X  SECOND_TITLE_X
#define SUBMENU_SECOND_TITLE_Y  SECOND_TITLE_Y
#define SUBMENU_SECOND_TITLE_W SECOND_TITLE_W
#define SUBMENU_SECOND_TITLE_H  SECOND_TITLE_H

//COL 1 CONT
#define SUBMENU_COL1_CONT_X   SUBMENU_SECOND_TITLE_X
#define SUBMENU_COL1_CONT_Y   RIGHT_BG_Y
#define SUBMENU_COL1_CONT_W  166
#define SUBMENU_COL1_CONT_H   RIGHT_BG_H

//COL 2 CONT
#define SUBMENU_COL2_CONT_X   (SUBMENU_SECOND_TITLE_X + SUBMENU_SECOND_TITLE_W - SUBMENU_COL2_CONT_W)
#define SUBMENU_COL2_CONT_Y   SUBMENU_COL1_CONT_Y
#define SUBMENU_COL2_CONT_W  SUBMENU_COL1_CONT_W
#define SUBMENU_COL2_CONT_H   SUBMENU_COL1_CONT_H

#define SUBMENU_COL1_BTN_X   (SUBMENU_COL1_CONT_X + (SUBMENU_COL1_CONT_W - SUBMENU_BTN_W)/2)
#define SUBMENU_COL2_BTN_X   (SUBMENU_COL2_CONT_X + (SUBMENU_COL1_CONT_W - SUBMENU_BTN_W)/2)
#define SUBMENU_BTN_Y   (SUBMENU_COL1_CONT_Y + 10)
#define SUBMENU_BTN_W  126
#define SUBMENU_BTN_H   35

#define SUBMENU_BTN_V_GAP       18

RET_CODE open_senior_sub_menu(u32 para1, u32 para2);

#endif
