/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

/* coordinate */
#define ACCEPT_INFO_CONT_X          ((SCREEN_WIDTH - ACCEPT_INFO_CONT_W) / 2)
#define ACCEPT_INFO_CONT_Y          ((SCREEN_HEIGHT - ACCEPT_INFO_CONT_H) / 2)
#define ACCEPT_INFO_CONT_W          SCREEN_WIDTH
#define ACCEPT_INFO_CONT_H          SCREEN_HEIGHT

//CA frame
#define ACCEPT_INFO_CA_FRM_X  SECOND_TITLE_X
#define ACCEPT_INFO_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define ACCEPT_INFO_CA_FRM_W  SECOND_TITLE_W
#define ACCEPT_INFO_CA_FRM_H  360

//CA info items
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      11
#define ACCEPT_INFO_CARD_INFO_PAGE_ITEMS 7
#define ACCEPT_INFO_CA_INFO_ITEM_X        10
#define ACCEPT_INFO_CARDFO_ITEM_Y        5

#define ACCEPT_INFO_CA_INFO_ITEM_LW0       160//140
#define ACCEPT_INFO_CA_INFO_ITEM_RW0       200//180
#define ACCEPT_INFO_CA_INFO_ITEM_LW1       160//140
#define ACCEPT_INFO_CA_INFO_ITEM_RW1       200//140
#define ACCEPT_INFO_CA_INFO_ITEM_LW2       160//130
#define ACCEPT_INFO_CA_INFO_ITEM_RW2       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_LW3       160//140
#define ACCEPT_INFO_CA_INFO_ITEM_RW3       200//180
#define ACCEPT_INFO_CA_INFO_ITEM_LW4       160//110
#define ACCEPT_INFO_CA_INFO_ITEM_RW4       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_LW5       160//150
#define ACCEPT_INFO_CA_INFO_ITEM_RW5       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW6       160//130
#define ACCEPT_INFO_CA_INFO_ITEM_RW6       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_LW7       160//160
#define ACCEPT_INFO_CA_INFO_ITEM_RW7       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW8       160//120
#define ACCEPT_INFO_CA_INFO_ITEM_RW8       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_LW9       160//100
#define ACCEPT_INFO_CA_INFO_ITEM_RW9       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_LW10       160//130
#define ACCEPT_INFO_CA_INFO_ITEM_RW10       200//200
#define ACCEPT_INFO_CA_INFO_ITEM_W       360

#define ACCEPT_INFO_CA_INFO_ITEM_H        35
#define ACCEPT_CA_INFO_ITEM_V_GAP    10


/* rect style */

/* font style */

/* others */

RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


