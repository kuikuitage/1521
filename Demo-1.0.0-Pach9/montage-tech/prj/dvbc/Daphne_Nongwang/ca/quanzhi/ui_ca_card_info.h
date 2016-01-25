/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_INFO_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_INFO_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_INFO_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_INFO_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_INFO_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_INFO_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_INFO_CA_FRM_X  SECOND_TITLE_X-140
#define CONDITIONAL_ACCEPT_INFO_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H)-85
#define CONDITIONAL_ACCEPT_INFO_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_INFO_CA_FRM_H  300

//CA info items
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_CNT_QZ      2
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_X        10
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_Y        40

#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW0       100
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW0       180
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW1       140
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW1       140
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_LW2       80
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_RW2       200
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_H        35
#define CONDITIONAL_ACCEPT_INFO_CA_INFO_ITEM_V_GAP    20


/* rect style */

/* font style */

/* others */

RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


