/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_PROVIDER_INFO_H__
#define __UI_PROVIDER_INFO_H__

/*provider menu root cont*/
#define PROVIDER_MENU_X          0
#define PROVIDER_MENU_Y          ((SCREEN_HEIGHT - PROVIDER_MENU_H) / 2)
#define PROVIDER_MENU_W          SCREEN_WIDTH
#define PROVIDER_MENU_H          SCREEN_HEIGHT

//the first line title
#define PROVIDER_TITLE_BTN_X       (SUB_AD_X-20) 
#define PROVIDER_TITLE_BTN_Y       SECOND_TITLE_Y
#define PROVIDER_TITLE_BTN_W       150
#define PROVIDER_TITLE_BTN_H       SECOND_TITLE_H

//mbox
#define PROVIDER_MBOX_BTN_X           (SUB_AD_X-20) 
#define PROVIDER_MBOX_BTN_Y           RIGHT_BG_Y 
#define PROVIDER_MBOX_BTN_W           605
#define PROVIDER_MBOX_BTN_H           COMM_BTN_ITEM_H

//plist
#define PROVIDER_PLIST_BG_X   (SUB_AD_X -20)
#define PROVIDER_PLIST_BG_Y  (PROVIDER_MBOX_BTN_Y + PROVIDER_MBOX_BTN_H + 10)
#define PROVIDER_PLIST_BG_W   PROVIDER_MBOX_BTN_W 
#define PROVIDER_PLIST_BG_H  (RIGHT_BG_H - PROVIDER_MBOX_BTN_H - 10)


//plist rect
#define PROVIDER_LIST_BG_MIDL         10
#define PROVIDER_LIST_BG_MIDT         15
#define PROVIDER_LIST_BG_MIDW        (PROVIDER_PLIST_BG_W-PROVIDER_LIST_BG_MIDL*2)
#define PROVIDER_LIST_BG_MIDH        (PROVIDER_PLIST_BG_H-PROVIDER_LIST_BG_MIDT*2  - 60) 
#define PROVIDER_LIST_BG_VGAP         5
//other
#define PROVIDER_LIST_PAGE         6//8
#define PROVIDER_LIST_FIELD3       3
#define PROVIDER_LIST_FIELD4       4
#define PROVIDER_LIST_FIELD5       5


#define PROVIDER_BTN_V_GAP       4
#define PROVIDER_BTN_H_GAP       0

//title_item
#define TITLE_ITEM_Y      65
#define TITLE_ITEM_H      35
#define PROVIDER_ITEMMENU_CNT      4

/* rect style */
RET_CODE open_provider_info(u32 para1, u32 para2);

#endif
