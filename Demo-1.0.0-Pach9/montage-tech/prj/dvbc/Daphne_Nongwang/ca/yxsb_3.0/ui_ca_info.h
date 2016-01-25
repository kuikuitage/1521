/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_INFO_H__
#define __UI_CA_INFO_H__

/*cont*/
#define CA_CONT_X          0
#define CA_CONT_Y          ((SCREEN_HEIGHT - CA_CONT_H) / 2)     //SCREEN_HEIGHT = 556
#define CA_CONT_W          SCREEN_WIDTH
#define CA_CONT_H          SCREEN_HEIGHT                                    //SCREEN_HEIGHT = 556

//plist
#define CA_PLIST_BG_X  WHOLE_SCR_TITLE_X
#define CA_PLIST_BG_Y  (RIGHT_BG_Y + COMM_BTN_ITEM_H)
#define CA_PLIST_BG_W WHOLE_SCR_TITLE_W
#define CA_PLIST_BG_H  (RIGHT_BG_H - COMM_BTN_ITEM_H)

//mbox
#define CA_MBOX_BTN_X           CA_PLIST_BG_X
#define CA_MBOX_BTN_Y           (CA_PLIST_BG_Y - COMM_BTN_ITEM_H)
#define CA_MBOX_BTN_W           CA_PLIST_BG_W
#define CA_MBOX_BTN_H           (COMM_BTN_ITEM_H)

//bg
#define CA_LIST_HEAD_BG_X      0
#define CA_LIST_HEAD_BG_Y      53
#define CA_LIST_HEAD_BG_W     CA_CONT_W
#define CA_LIST_HEAD_BG_H      (CA_CONT_H - 106)

//plist rect
#define CA_LIST_BG_MIDL         10
#define CA_LIST_BG_MIDT         15
#define CA_LIST_BG_MIDW        (CA_PLIST_BG_W-20)
#define CA_LIST_BG_MIDH        (CA_PLIST_BG_H-100)
#define CA_LIST_BG_VGAP         5
//other
#define CA_INFO_LIST_PAGE                      6
#define CA_INFO_ENTITLE_FIELD               3
#define CA_INFO_BURSE_FIELD                  3
#define CA_INFO_IPPV_FIELD                     6
#define CA_INFO_CHARACTERS_FIELD       3

#define CA_MBOX1_ITME_COUNT     3
#define CA_MBOX2_ITME_COUNT     3
#define CA_MBOX3_ITME_COUNT     6
#define CA_MBOX4_ITME_COUNT     3




//TILTLE
#define ITEMMENU_BTN_X       (WHOLE_SCR_TITLE_X-10)
#define ITEMMENU_BTN_Y       WHOLE_SCR_TITLE_Y
#define ITEMMENU_BTN_W      ((WHOLE_SCR_TITLE_W)/4 - 6)
#define ITEMMENU_BTN_H       WHOLE_SCR_TITLE_H


#define ITEMMENU_BTN_V_GAP       4
#define ITEMMENU_BTN_H_GAP       0

//title_item
#define ITEMMENU_CNT      4
#define ITEMMENU_CNT1      5

/* rect style */
RET_CODE open_ca_info(u32 para1, u32 para2);

#endif
