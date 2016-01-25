/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_DO_SEARCH_H__
#define __UI_DO_SEARCH_H__

/* coordinate */
#define DSEARCH_CONT_X            ((SCREEN_WIDTH - DSEARCH_CONT_W) / 2)
#define DSEARCH_CONT_Y            ((SCREEN_HEIGHT - DSEARCH_CONT_H) / 2)
#define DSEARCH_CONT_W            SCREEN_WIDTH
#define DSEARCH_CONT_H            SCREEN_HEIGHT

//advertisement window
#define DSEARCH_AD_X       SUB_AD_X
#define DSEARCH_AD_Y       SECOND_TITLE_Y
#define DSEARCH_AD_W       SUB_AD_W
#define DSEARCH_AD_H       SUB_AD_H

//search result:channel title
#define DSEARCH_CH_TITLE_X   DSEARCH_AD_X
#define DSEARCH_CH_TITLE_Y   (DSEARCH_AD_Y + 10)
#define DSEARCH_CH_TITLE_W   DSEARCH_AD_W
#define DSEARCH_CH_TITLE_H   35

//prog list
#define DSEARCH_LIST_X            DSEARCH_AD_X
#define DSEARCH_LIST_Y            (DSEARCH_CH_TITLE_Y + DSEARCH_CH_TITLE_H + 15)
#define DSEARCH_LIST_W            DSEARCH_AD_W
#define DSEARCH_LIST_H            215
#define DSEARCH_LIST_ITEM_V_GAP   0

//second title
#define DSEARCH_SECOND_TITLE_X      (DSEARCH_AD_X + DSEARCH_AD_W + 15)
#define DSEARCH_SECOND_TITLE_Y      SECOND_TITLE_Y
#define DSEARCH_SECOND_TITLE_W     SECOND_TITLE_W
#define DSEARCH_SECOND_TITLE_H      SECOND_TITLE_H

//search main tp: cont
#define DSEARCH_MAIN_TP_CONT_X     DSEARCH_SECOND_TITLE_X
#define DSEARCH_MAIN_TP_CONT_Y     RIGHT_BG_Y
#define DSEARCH_MAIN_TP_CONT_W     RIGHT_BG_W
#define DSEARCH_MAIN_TP_CONT_H     110

#define DSEARCH_MAIN_TP_NAME_X       8
#define DSEARCH_MAIN_TP_NAME_W      100
#define DSEARCH_MAIN_TP_DATA_X       (DSEARCH_MAIN_TP_NAME_X + DSEARCH_MAIN_TP_NAME_W + 10)
#define DSEARCH_MAIN_TP_DATA_W      150
#define DSEARCH_ITEM_H          30
#define DSEARCH_ITEM_VGAP1   20
#define DSEARCH_ITEM_VGAP2  20

//searching info:prompt string
#define DSEARCH_STATUS_STR_X       DSEARCH_MAIN_TP_NAME_X
#define DSEARCH_STATUS_STR_Y      (2 * DSEARCH_ITEM_VGAP1 + DSEARCH_ITEM_H)
#define DSEARCH_STATUS_STR_W       250

//searching info: cont
#define DSEARCH_INFO_CONT_X        DSEARCH_SECOND_TITLE_X
#define DSEARCH_INFO_CONT_Y        DSEARCH_LIST_Y
#define DSEARCH_INFO_CONT_W        RIGHT_BG_W
#define DSEARCH_INFO_CONT_H        DSEARCH_LIST_H


#define DSEARCH_INFO_ITEM_LX         8
#define DSEARCH_INFO_ITEM_LW       150
#define DSEARCH_INFO_ITEM_RX        (DSEARCH_INFO_ITEM_LX + DSEARCH_INFO_ITEM_LW)
#define DSEARCH_INFO_ITEM_RW       150
#define DSEARCH_INFO_ITEM_Y         10

//searching info:comm bar
#define D_STATUS_BAR1_Y       (DSEARCH_INFO_ITEM_Y + DSEARCH_ITEM_H + DSEARCH_ITEM_VGAP2)
#define D_STATUS_BAR_H       30
#define D_STATUS_BAR_LX       8
#define D_STATUS_BAR_LW       60
#define D_STATUS_BAR_MX       (D_STATUS_BAR_LX + D_STATUS_BAR_LW)
#define D_STATUS_BAR_MW       170
#define D_STATUS_BAR_MH       COMM_PBAR_H
#define D_STATUS_BAR_RX       (D_STATUS_BAR_MX + D_STATUS_BAR_MW)
#define D_STATUS_BAR_RW      60

/* rect style */
#define RSI_DSEARCH_PBAR_BG           RSI_MIN_PROGRESS_BAR_BG
#define RSI_DSEARCH_PBAR_MID          RSI_MIN_COMM_PBAR_MID

/* font style */
#define FSI_DSEARCH_LIST_ITEM         FSI_COMM_TXT_N

/* others */
#define DSEARCH_LIST_CNT          3
#define DSEARCH_PROG_TYPE_CNT          2
#define DSEARCH_LIST_PAGE         5
#define DSEARCH_LIST_FIELD_CNT    2

#define DSEARCH_LIST_PAGE_CNT		7
RET_CODE open_do_search(u32 para1, u32 para2);

BOOL do_search_is_finish(void);
#endif


