/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_MANUAL_SEARCH_H__
#define __UI_MANUAL_SEARCH_H__

/* coordinate */
#define MSEARCH_MENU_X            ((SCREEN_WIDTH - MSEARCH_MENU_W) / 2)
#define MSEARCH_MENU_Y            ((SCREEN_HEIGHT - MSEARCH_MENU_H) / 2)
#define MSEARCH_MENU_W            SCREEN_WIDTH
#define MSEARCH_MENU_H            SCREEN_HEIGHT

//advertisement window
#define MSEARCH_AD_X       SUB_AD_X
#define MSEARCH_AD_Y       SECOND_TITLE_Y
#define MSEARCH_AD_W       SUB_AD_W
#define MSEARCH_AD_H       SUB_AD_H

//search result:channel title
#define MSEARCH_CH_TITLE_X   MSEARCH_AD_X
#define MSEARCH_CH_TITLE_Y   (MSEARCH_AD_Y + MSEARCH_AD_H + 10)
#define MSEARCH_CH_TITLE_W   MSEARCH_AD_W
#define MSEARCH_CH_TITLE_H   35

//list
#define MSEARCH_LIST_X            MSEARCH_AD_X
#define MSEARCH_LIST_Y            (MSEARCH_CH_TITLE_Y + MSEARCH_CH_TITLE_H + 15)
#define MSEARCH_LIST_W            MSEARCH_AD_W
#define MSEARCH_LIST_H            215
#define MSEARCH_LIST_ITEM_V_GAP   0

//second title
#define MSEARCH_SECOND_TITLE_X      (MSEARCH_AD_X + MSEARCH_AD_W + 15)
#define MSEARCH_SECOND_TITLE_Y      SECOND_TITLE_Y
#define MSEARCH_SECOND_TITLE_W     SECOND_TITLE_W
#define MSEARCH_SECOND_TITLE_H      SECOND_TITLE_H

//search param set: cont
#define MSEARCH_SET_CONT_X     MSEARCH_SECOND_TITLE_X
#define MSEARCH_SET_CONT_Y     RIGHT_BG_Y
#define MSEARCH_SET_CONT_W     RIGHT_BG_W
#define MSEARCH_SET_CONT_H     110

#define MSEARCH_SET_ITEM_X       4
#define MSEARCH_SET_ITEM_LW      130
#define MSEARCH_SET_ITEM_MW      110
#define MSEARCH_SET_ITEM_RW      (MSEARCH_SET_CONT_W - 2 * MSEARCH_SET_ITEM_X - \
                              MSEARCH_SET_ITEM_LW - MSEARCH_SET_ITEM_MW)                              
#define MSEARCH_SET_ITEM_H       30
#define MSEARCH_SET_ITEM_VGAP    4

//searching info: cont
#define MSEARCH_INFO_CONT_X        MSEARCH_SECOND_TITLE_X
#define MSEARCH_INFO_CONT_Y        MSEARCH_LIST_Y
#define MSEARCH_INFO_CONT_W        RIGHT_BG_W
#define MSEARCH_INFO_CONT_H        MSEARCH_LIST_H

#define MSEARCH_V_GAP               20

//searching info:prompt string
#define MSEARCH_INFO_ITEM_X         14
#define MSEARCH_INFO_ITEM_Y         10
#define MSEARCH_INFO_ITEM_W         260
#define MSEARCH_INFO_ITEM_H         30

//searching info:comm bar
#define M_STATUS_BAR1_Y       (MSEARCH_INFO_ITEM_Y + MSEARCH_INFO_ITEM_H + MSEARCH_V_GAP)
#define M_STATUS_BAR_H       30

#define M_STATUS_BAR_LX       14
#define M_STATUS_BAR_LW       110

#define M_STATUS_BAR_MX       (M_STATUS_BAR_LX + M_STATUS_BAR_LW)
#define M_STATUS_BAR_MW       100//120
#define M_STATUS_BAR_MH       COMM_PBAR_H

#define M_STATUS_BAR_RX       (M_STATUS_BAR_MX + M_STATUS_BAR_MW)
#define M_STATUS_BAR_RW      80//60


/* font style */
#define FSI_MSEARCH_LIST_ITEM         FSI_COMM_TXT_N

/* others */
#define MSEARCH_LIST_CNT          1
#define MSEARCH_PROG_TYPE_CNT          2
#define MSEARCH_LIST_PAGE         5
#define MSEARCH_LIST_FIELD_CNT    2
#define MSEARCH_SET_ITEM_CNT     3

RET_CODE open_manual_search(u32 para1, u32 para2);

BOOL manual_search_is_finish(void);

#endif


