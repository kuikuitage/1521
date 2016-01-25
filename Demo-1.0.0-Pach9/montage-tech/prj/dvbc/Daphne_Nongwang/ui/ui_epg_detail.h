/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_EPG_DETAIL_H__
#define __UI_EPG_DETAIL_H__

/* coordinate */
#define DETAIL_CONT_X        ((SCREEN_WIDTH - DETAIL_CONT_W) / 2)
#define DETAIL_CONT_Y        ((SCREEN_HEIGHT- DETAIL_CONT_H) / 2)
#define DETAIL_CONT_W        (DETAIL_TTL_W + DETAIL_BAR_W)
#define DETAIL_CONT_H        (DETAIL_TTL_H + DETAIL_DETAIL_H + DETAIL_HELP_H + 2 * DETAIL_V_GAP)

#define DETAIL_V_GAP  4
#define DETAIL_H_GAP  4

#define DETAIL_TTL_X  0
#define DETAIL_TTL_Y  0
#define DETAIL_TTL_W  (DETAIL_ITEM_W + 2 * DETAIL_H_GAP)
#define DETAIL_TTL_H (4 * DETAIL_ITEM_H + 5 * DETAIL_V_GAP)

#define DETAIL_ITEM_X  DETAIL_H_GAP
#define DETAIL_ITEM_Y  DETAIL_V_GAP
#define DETAIL_ITEM_LW  168
#define DETAIL_ITEM_RW 168
#define DETAIL_ITEM_W (DETAIL_ITEM_LW + DETAIL_ITEM_RW)
#define DETAIL_ITEM_H  24

#define DETAIL_DETAIL_X 0
#define DETAIL_DETAIL_Y (DETAIL_TTL_Y + DETAIL_TTL_H + DETAIL_V_GAP)
#define DETAIL_DETAIL_W DETAIL_TTL_W
#define DETAIL_DETAIL_H 240

#define DETAIL_DETAIL_MIDX DETAIL_H_GAP
#define DETAIL_DETAIL_MIDY DETAIL_V_GAP
#define DETAIL_DETAIL_MIDW (DETAIL_DETAIL_W - 2 * DETAIL_DETAIL_MIDX)
#define DETAIL_DETAIL_MIDH (DETAIL_DETAIL_H - 2 * DETAIL_DETAIL_MIDY)

#define DETAIL_BAR_X (DETAIL_DETAIL_X + DETAIL_DETAIL_W)
#define DETAIL_BAR_Y DETAIL_DETAIL_Y
#define DETAIL_BAR_W 12
#define DETAIL_BAR_H DETAIL_DETAIL_H

#define DETAIL_HELP_X DETAIL_DETAIL_X
#define DETAIL_HELP_Y (DETAIL_DETAIL_Y + DETAIL_DETAIL_H + DETAIL_V_GAP)
#define DETAIL_HELP_W DETAIL_DETAIL_W
#define DETAIL_HELP_H 40

/* rect style */
#define RSI_DETAIL_FRM       RSI_IGNORE
#define RSI_DETAIL_TTL  RSI_ITEM_7_SH
#define RSI_DETAIL_DTL       RSI_ITEM_7_SH 
#define RSI_DETAIL_HELP RSI_ITEM_7_SH

/* font style */

/* others */
#define DETAIL_ITEM_NUM 5

#define DETAIL_MAX_LENTH 1024

RET_CODE open_epg_detail(u32 para1, u32 para2);

#endif

