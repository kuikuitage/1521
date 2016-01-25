/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

/*cont*/
#define CA_CARD_CONT_X          ((SCREEN_WIDTH - CA_CARD_CONT_W) / 2)
#define CA_CARD_CONT_Y          ((SCREEN_HEIGHT - CA_CARD_CONT_H) / 2)
#define CA_CARD_CONT_W          SCREEN_WIDTH
#define CA_CARD_CONT_H          SCREEN_HEIGHT


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

//item
#define CA_CARD_ITEM_NAME_W    (RIGHT_BG_W/2)
#define CA_CARD_ITEM_DETAIL_W  (RIGHT_BG_W/2 - 10)
#define CA_CARD_ITEM_X         200
#define CA_CARD_ITEM_Y         80
#define CA_CARD_ITEM_H         30
#define CA_CARD_ITEM_VGAP      20
#define CA_CARD_ITEM_HGAP      0

/* rect style */
RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif
