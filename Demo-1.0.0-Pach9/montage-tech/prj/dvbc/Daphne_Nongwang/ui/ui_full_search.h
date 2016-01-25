/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_FULL_SEARCH_H__
#define __UI_FULL_SEARCH_H__

/* coordinate */
#define FSEARCH_MENU_X            ((SCREEN_WIDTH - FSEARCH_MENU_W) / 2)
#define FSEARCH_MENU_Y            ((SCREEN_HEIGHT - FSEARCH_MENU_H) / 2)
#define FSEARCH_MENU_W            SCREEN_WIDTH
#define FSEARCH_MENU_H            SCREEN_HEIGHT

//advertisement window
#define FSEARCH_AD_X       SUB_AD_X
#define FSEARCH_AD_Y       SECOND_TITLE_Y
#define FSEARCH_AD_W       SUB_AD_W
#define FSEARCH_AD_H       SUB_AD_H

//search result:channel title
#define FSEARCH_CH_TITLE_X   FSEARCH_AD_X
#define FSEARCH_CH_TITLE_Y   (FSEARCH_AD_Y + FSEARCH_AD_H + 10)
#define FSEARCH_CH_TITLE_W   FSEARCH_AD_W
#define FSEARCH_CH_TITLE_H   35

//list
#define FSEARCH_LIST_X            FSEARCH_AD_X
#define FSEARCH_LIST_Y            (FSEARCH_CH_TITLE_Y + FSEARCH_CH_TITLE_H + 15)
#define FSEARCH_LIST_W            FSEARCH_AD_W
#define FSEARCH_LIST_H            215
#define FSEARCH_LIST_ITEM_V_GAP   0

//second title
#define FSEARCH_SECOND_TITLE_X      (FSEARCH_AD_X + FSEARCH_AD_W + 15)
#define FSEARCH_SECOND_TITLE_Y      SECOND_TITLE_Y
#define FSEARCH_SECOND_TITLE_W     SECOND_TITLE_W
#define FSEARCH_SECOND_TITLE_H      SECOND_TITLE_H

//search param set: cont
#define FSEARCH_SET_CONT_X     FSEARCH_SECOND_TITLE_X
#define FSEARCH_SET_CONT_Y     RIGHT_BG_Y
#define FSEARCH_SET_CONT_W     RIGHT_BG_W
#define FSEARCH_SET_CONT_H     110 + 34

#define FSEARCH_SET_ITEM_X       4
#define FSEARCH_SET_ITEM_LW      130
#define FSEARCH_SET_ITEM_MW      110
#define FSEARCH_SET_ITEM_RW      (FSEARCH_SET_CONT_W - 2 * FSEARCH_SET_ITEM_X - \
                              FSEARCH_SET_ITEM_LW - FSEARCH_SET_ITEM_MW)                              
#define FSEARCH_SET_ITEM_H       30
#define FSEARCH_SET_ITEM_VGAP    4

//searching info: cont
#define FSEARCH_INFO_CONT_X        FSEARCH_SECOND_TITLE_X
#define FSEARCH_INFO_CONT_Y        FSEARCH_LIST_Y + 34
#define FSEARCH_INFO_CONT_W        RIGHT_BG_W
#define FSEARCH_INFO_CONT_H        FSEARCH_LIST_H - 34

#define FSEARCH_V_GAP               10

//searching info:prompt string
#define FSEARCH_INFO_ITEM_X         14
#define FSEARCH_INFO_ITEM_Y         10
#define FSEARCH_INFO_ITEM_W         260
#define FSEARCH_INFO_ITEM_H         30

//searching info:comm bar
#define F_STATUS_BAR1_Y       (FSEARCH_INFO_ITEM_Y + FSEARCH_INFO_ITEM_H + FSEARCH_V_GAP)
#define F_STATUS_BAR_H       30

#define F_STATUS_BAR_LX       14
#define F_STATUS_BAR_LW       110

#define F_STATUS_BAR_MX       (F_STATUS_BAR_LX + F_STATUS_BAR_LW)
#define F_STATUS_BAR_MW       100//120
#define F_STATUS_BAR_MH       COMM_PBAR_H

#define F_STATUS_BAR_RX       (F_STATUS_BAR_MX + F_STATUS_BAR_MW)
#define F_STATUS_BAR_RW      80//60

/* rect style */
//#define RSI_FSEARCH_LIST              RSI_TIP_BOX
//#define RSI_FSEARCH_ITEM              RSI_TIP_BOX_BG
//#define RSI_FSEARCH_BAR_MID          RSI_COMM_PBAR_MID

/* font style */
#define FSI_FSEARCH_LIST_ITEM         FSI_COMM_TXT_N

/* others */
#define FSEARCH_LIST_CNT          1
#define FSEARCH_PROG_TYPE_CNT          2
#define FSEARCH_LIST_PAGE         5
#define FSEARCH_LIST_FIELD_CNT    2
#define FSEARCH_SET_ITEM_CNT     4

RET_CODE open_full_search(u32 para1, u32 para2);

BOOL full_search_is_finish(void);

#endif


