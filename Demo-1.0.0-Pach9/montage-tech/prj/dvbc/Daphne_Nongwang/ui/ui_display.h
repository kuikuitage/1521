/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_DISPLAY_H__
#define __UI_DISPLAY_H__

/* coordinate */
//xuxin
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

#define SN_PARAM_SET_CONT_X          ((SN_SCREEN_WIDTH - PARAM_SET_CONT_W) / 2)
#define SN_PARAM_SET_CONT_Y          ((SN_SCREEN_HEIGHT - PARAM_SET_CONT_H) / 2)
#define SN_PARAM_SET_CONT_W          SN_SCREEN_WIDTH
#define SN_PARAM_SET_CONT_H          SN_SCREEN_HEIGHT
//xuxin end
//small bg
#define PARAM_SET_SMALL_BG_X  RIGHT_BG_X
#define PARAM_SET_SMALL_BG_Y  RIGHT_BG_Y
#define PARAM_SET_SMALL_BG_W  RIGHT_BG_W
#define PARAM_SET_SMALL_BG_H  RIGHT_BG_H

#define PARAM_SET_ITEM_CNT      9
#define PARAM_SET_ITEM_X        (PARAM_SET_SMALL_BG_X + 10)
#define PARAM_SET_ITEM_START_Y        (PARAM_SET_SMALL_BG_Y + RIGHT_BG_ITEM_START_VGAP)
#define PARAM_SET_ITEM_LW       120
#define PARAM_SET_ITEM_RW       200
#define PARAM_SET_ITEM_H        35
#define PARAM_SET_ITEM_V_GAP    0

#define PARAM_NUMB_WARN_X        100
#define PARAM_NUMB_WARN_Y        260

/* rect style */
/* font style */

/* others */
#define PARAM_SET_GMT_OFFSET_NUM 48

RET_CODE open_display_set(u32 para1, u32 para2);

#endif



