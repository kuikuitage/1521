/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

#define CA_CARD_INFO_CNT 5

/*cont*/
#define CA_CARD_CONT_X          ((SCREEN_WIDTH - CA_CARD_CONT_W) / 2)
#define CA_CARD_CONT_Y          ((SCREEN_HEIGHT - CA_CARD_CONT_H) / 2)
#define CA_CARD_CONT_W          (SCREEN_WIDTH - 32)
#define CA_CARD_CONT_H          (SCREEN_HEIGHT - 76)

//CA frame
#define ACCEPT_INFO_CA_FRM_X  SECOND_TITLE_X
#define ACCEPT_INFO_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define ACCEPT_INFO_CA_FRM_W  SECOND_TITLE_W
#define ACCEPT_INFO_CA_FRM_H  360

//advertisement window
#define CA_CARD_AD_WINDOW_X     40
#define CA_CARD_AD_WINDOW_Y     100
#define CA_CARD_AD_WINDOW_W     250
#define CA_CARD_AD_WINDOW_H     100

//preview window
#define CA_CARD_TV_WINDOW_X     40
#define CA_CARD_TV_WINDOW_Y     250
#define CA_CARD_TV_WINDOW_W     250
#define CA_CARD_TV_WINDOW_H     220

//small bg
#define CA_CARD_SMALL_BG_X  RIGHT_BG_X
#define CA_CARD_SMALL_BG_Y  RIGHT_BG_Y
#define CA_CARD_SMALL_BG_W  RIGHT_BG_W
#define CA_CARD_SMALL_BG_H  RIGHT_BG_H

//item
#define CA_CARD_ITEM_NAME_W    (CA_CARD_SMALL_BG_W/2)
#define CA_CARD_ITEM_DETAIL_W  (CA_CARD_SMALL_BG_W/2 - 10)
#define CA_CARD_ITEM_X         RIGHT_BG_ITEM_X
#define CA_CARD_ITEM_Y         RIGHT_BG_ITEM_Y
#define CA_CARD_ITEM_H         30
#define CA_CARD_ITEM_VGAP      20
#define CA_CARD_ITEM_HGAP      0

#define ACCEPT_INFO_CA_INFO_ITEM_X        10
#define ACCEPT_INFO_CARDFO_ITEM_Y        5

#define ACCEPT_INFO_CA_INFO_ITEM_LW0       130
#define ACCEPT_INFO_CA_INFO_ITEM_RW0       200
#define ACCEPT_INFO_CA_INFO_ITEM_LW1       130
#define ACCEPT_INFO_CA_INFO_ITEM_RW1       200
#define ACCEPT_INFO_CA_INFO_ITEM_LW2       140
#define ACCEPT_INFO_CA_INFO_ITEM_RW2       180
#define ACCEPT_INFO_CA_INFO_ITEM_LW3       160
#define ACCEPT_INFO_CA_INFO_ITEM_RW3       160

#define ACCEPT_TEXT_TITLE_W0    130
#define ACCEPT_TEXT_TITLE_W1    200
#define ACCEPT_TEXT_TITLE_H    70

#define ACCEPT_CA_INFO_ITEM_V_GAP    5

#define CARD_SN_MAX_LEN 16
#define CARD_CARD_ID_MAX_LEN 20
#define CARD_CARD_VER_MAX_LEN 18

/* rect style */
RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif
