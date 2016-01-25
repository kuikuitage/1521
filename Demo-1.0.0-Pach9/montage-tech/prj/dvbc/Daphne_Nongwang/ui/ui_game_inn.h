/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_GAME_INN_H__
#define __UI_GAME_INN_H__

/* coordinate */
#define GAME_INN_MENU_X          ((SCREEN_WIDTH - GAME_INN_MENU_W) / 2)
#define GAME_INN_MENU_Y          ((SCREEN_HEIGHT - GAME_INN_MENU_H) / 2)
#define GAME_INN_MENU_W          SCREEN_WIDTH
#define GAME_INN_MENU_H          SCREEN_HEIGHT

//item cont
#define GAME_INN_FRM_X  RIGHT_BG_X
#define GAME_INN_FRM_Y  RIGHT_BG_Y
#define GAME_INN_FRM_W  RIGHT_BG_W
#define GAME_INN_FRM_H  RIGHT_BG_H

#define GAME_INN_BTN_CNT      4
#define GAME_INN_BTN_X        ((SECOND_TITLE_W - GAME_INN_BTN_W) / 2)
#define GAME_INN_BTN_Y        RIGHT_BG_ITEM_START_VGAP
#define GAME_INN_BTN_W       (GAME_INN_FRM_W - 60)
#define GAME_INN_BTN_H        35
#define GAME_INN_BTN_V_GAP    4

//preview window
#define GAME_INN_PREV_X     SUB_PREV_X
#define GAME_INN_PREV_Y     SUB_PREV_Y
#define GAME_INN_PREV_W    SUB_PREV_W
#define GAME_INN_PREV_H     SUB_PREV_H

/* rect style */

/* font style */

/* others */

RET_CODE open_game_inn(u32 para1, u32 para2);

#endif


