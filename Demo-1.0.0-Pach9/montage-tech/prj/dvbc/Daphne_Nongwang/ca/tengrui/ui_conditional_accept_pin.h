/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_PIN_H__
#define __UI_CONDITIONAL_ACCEPT_PIN_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_PIN_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_PIN_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_PIN_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_PIN_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_PIN_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_PIN_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_X  RIGHT_BG_X
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_Y  RIGHT_BG_Y
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_W  RIGHT_BG_W
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_H  RIGHT_BG_H

//pin modify items
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT      3
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X        10
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y        RIGHT_BG_ITEM_START_VGAP
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW       140
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW       140
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H        35
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP    4

//change pin result
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_X        10
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_Y        140
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_W        260
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_H        35

/* rect style */

/* font style */

/* others */

//RET_CODE open_conditional_accept_pin(u32 para1, u32 para2);

#endif


