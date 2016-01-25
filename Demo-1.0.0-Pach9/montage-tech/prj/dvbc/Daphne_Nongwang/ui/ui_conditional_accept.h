/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_H__
#define __UI_CONDITIONAL_ACCEPT_H__
typedef struct
{
  cas_card_info_t *p_card_info ;
  burses_info_t *p_ipp_buy_info;
}card_and_ipp_info;

/* coordinate */
#define CONDITIONAL_ACCEPT_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_CA_FRM_X  RIGHT_BG_X
#define CONDITIONAL_ACCEPT_CA_FRM_Y  RIGHT_BG_Y
#define CONDITIONAL_ACCEPT_CA_FRM_W  RIGHT_BG_W
#define CONDITIONAL_ACCEPT_CA_FRM_H  RIGHT_BG_H

#define CONDITIONAL_ACCEPT_BTN_CNT      7
#define CONDITIONAL_ACCEPT_BTN_X        TEXT_START_OX
#define CONDITIONAL_ACCEPT_BTN_Y        RIGHT_BG_ITEM_START_VGAP
#define CONDITIONAL_ACCEPT_BTN_W        (CONDITIONAL_ACCEPT_CA_FRM_W - TEXT_START_OX*2)
#define CONDITIONAL_ACCEPT_BTN_H        COMM_BTN_ITEM_H
#define CONDITIONAL_ACCEPT_BTN_V_GAP    4

/* rect style */

/* font style */

/* others */

RET_CODE open_conditional_accept(u32 para1, u32 para2);

#endif


