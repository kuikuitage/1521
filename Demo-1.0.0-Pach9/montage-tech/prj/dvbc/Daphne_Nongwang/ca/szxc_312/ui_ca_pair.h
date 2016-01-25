/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PAIR_H__
#define __UI_CA_PAIR_H__

/* coordinate */
#define PAIR_INFO_CONT_X        ((SCREEN_WIDTH - PAIR_INFO_CONT_W) / 2)
#define PAIR_INFO_CONT_Y        200
#define PAIR_INFO_CONT_W        COMM_ROOT_W
#define PAIR_INFO_CONT_H        100

#define PAIR_CONT_TITLE_X      50
#define PAIR_CONT_TITLE_Y      0
#define PAIR_CONT_TITLE_W      (PAIR_INFO_CONT_W - 2 * PAIR_CONT_TITLE_X)
#define PAIR_CONT_TITLE_H      100

#define PAIR_CONT_TITLE2_X      50
#define PAIR_CONT_TITLE2_Y      60
#define PAIR_CONT_TITLE2_W      (PAIR_INFO_CONT_W - 2 * PAIR_CONT_TITLE2_X)
#define PAIR_CONT_TITLE2_H      50


#define PAIR_CONT_FULL_X  ((SCREEN_WIDTH-PAIR_CONT_FULL_W)/2)
#define PAIR_CONT_FULL_Y  ((SCREEN_HEIGHT-PAIR_CONT_FULL_H)/2 -20)
#define PAIR_CONT_FULL_W  350
#define PAIR_CONT_FULL_H  100

#define PAIR_TITLE_FULL_X  0
#define PAIR_TITLE_FULL_Y  0
#define PAIR_TITLE_FULL_W  PAIR_CONT_FULL_W
#define PAIR_TITLE_FULL_H  40

#define PAIR_CONTENT_FULL_X  10
#define PAIR_CONTENT_FULL_Y  (PAIR_TITLE_FULL_Y+PAIR_TITLE_FULL_H+5)
#define PAIR_CONTENT_FULL_W  (PAIR_CONT_FULL_W - 2*PAIR_CONTENT_FULL_X)
#define PAIR_CONTENT_FULL_H  50

#define PAIR_ERROR_AUTOCLOSE_MS 20000
RET_CODE open_ca_pair_dlg(u32 para1, u32 para2);
void close_ca_pair_dlg(void);

#endif


