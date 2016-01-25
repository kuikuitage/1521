/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_ANTIAUTH_H__
#define __UI_CONDITIONAL_ACCEPT_ANTIAUTH_H__


/* coordinate */
#define CONDITIONAL_ACCEPT_BURSE_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_BURSE_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_BURSE_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_BURSE_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_BURSE_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_BURSE_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_BURSE_CA_FRM_X  SECOND_TITLE_X
#define CONDITIONAL_ACCEPT_BURSE_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define CONDITIONAL_ACCEPT_BURSE_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_BURSE_CA_FRM_H  360

//piar info items
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT      4
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_X        20
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_Y        40
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_LW       200
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_RW       80
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_H        35
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_V_GAP    15//5


/* rect style */

/* font style */

/* others */

//RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2);

#endif


