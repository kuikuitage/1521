/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_LEVEL_H__
#define __UI_CONDITIONAL_ACCEPT_LEVEL_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_LEVEL_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_LEVEL_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_LEVEL_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_LEVEL_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_LEVEL_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_LEVEL_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_X  SECOND_TITLE_X
#define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_H  360

//level control items
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT      2
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X        70
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y        90
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW       272
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW       272
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H        36
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP    10

//change level result
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X        100
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y        240
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W       260
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H        35


/* rect style */

/* font style */

/* others */

//RET_CODE open_conditional_accept_level(u32 para1, u32 para2);

#endif


