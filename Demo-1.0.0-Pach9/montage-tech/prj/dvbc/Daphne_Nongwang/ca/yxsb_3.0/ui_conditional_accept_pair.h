/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_PAIR_H__
#define __UI_CONDITIONAL_ACCEPT_PAIR_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_PAIR_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_PAIR_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_PAIR_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_PAIR_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_PAIR_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_PAIR_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_PAIR_CA_FRM_X  SECOND_TITLE_X
#define CONDITIONAL_ACCEPT_PAIR_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define CONDITIONAL_ACCEPT_PAIR_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_PAIR_CA_FRM_H  RIGHT_BG_H

//pair list
#define PAIR_LIST_H               200
#define PAIR_LIST_PAGE         5
#define PAIR_LIST_FIELD        1

//pair list rect
#define PAIR_LIST_MIDL         0
#define PAIR_LIST_MIDT         5
#define PAIR_LIST_MIDW        (CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW - 2 * PAIR_LIST_MIDL)
#define PAIR_LIST_MIDH         (PAIR_LIST_H - 2 * PAIR_LIST_MIDT)
#define PAIR_LIST_VGAP         2

//CA info items
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_CNT        2
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X            5
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_Y            0
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW         120//160
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW         200//330
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H            35 * 2
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_V_GAP    20
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H_GAP    10

/* rect style */

/* font style */

/* others */

//RET_CODE open_conditional_accept_pair(u32 para1, u32 para2);

#endif


