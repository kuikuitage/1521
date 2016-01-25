/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_SIGNAL_CHECK_H__
#define __UI_SIGNAL_CHECK_H__

/* coordinate */
#define SIG_CHECK_MENU_X            ((SCREEN_WIDTH - SIG_CHECK_MENU_W) / 2)
#define SIG_CHECK_MENU_Y            ((SCREEN_HEIGHT - SIG_CHECK_MENU_H) / 2)
#define SIG_CHECK_MENU_W            SCREEN_WIDTH
#define SIG_CHECK_MENU_H            SCREEN_HEIGHT

//title in left side
#define SIG_CHECK_LTITLE_X    SUB_AD_X
#define SIG_CHECK_LTITLE_Y    SECOND_TITLE_Y
#define SIG_CHECK_LTITLE_W    SUB_AD_W
#define SIG_CHECK_LTITLE_H    SECOND_TITLE_H

//advertisement window
#define SIG_CHECK_AD_X       SUB_AD_X
#define SIG_CHECK_AD_Y       150
#define SIG_CHECK_AD_W      SUB_AD_W
#define SIG_CHECK_AD_H       SUB_AD_H

//title in right side
#define SIG_CHECK_RTITLE_X   SECOND_TITLE_X
#define SIG_CHECK_RTITLE_Y   SECOND_TITLE_Y
#define SIG_CHECK_RTITLE_W  SECOND_TITLE_W
#define SIG_CHECK_RTITLE_H   SECOND_TITLE_H

//freq list
#define SIG_CHECK_LIST_X      RIGHT_BG_X
#define SIG_CHECK_LIST_Y      RIGHT_BG_Y
#define SIG_CHECK_LIST_W     RIGHT_BG_W
#define SIG_CHECK_LIST_H      135

#define SIG_CHECK_LIST_MIDL         4
#define SIG_CHECK_LIST_MIDT         4
#define SIG_CHECK_LIST_MIDW        (SIG_CHECK_LIST_W -SIG_CHECK_LIST_MIDL*2 - SIG_CHECK_SCROLL_W)
#define SIG_CHECK_LIST_MIDH         (SIG_CHECK_LIST_H -SIG_CHECK_LIST_MIDT*2)
#define SIG_CHECK_LIST_VGAP         1
#define SIG_CHECK_LIST_PAGE         4
#define SIG_CHECK_LIST_FIELD_CNT    3

//scroll
#define SIG_CHECK_SCROLL_X     (SIG_CHECK_LIST_W - SIG_CHECK_SCROLL_W-4)
#define SIG_CHECK_SCROLL_Y     SIG_CHECK_LIST_MIDT
#define SIG_CHECK_SCROLL_W    COMM_LIST_BAR_W
#define SIG_CHECK_SCROLL_H     SIG_CHECK_LIST_MIDH

#define SIG_CHECK_ITEM_H        30
#define SIG_CHECK_V_GAP          1

//signal status container
#define SIG_CHECK_STATUS_CONT_X     SUB_AD_X
#define SIG_CHECK_STATUS_CONT_Y     (SIG_CHECK_LIST_Y + SIG_CHECK_LIST_H + 10)
#define SIG_CHECK_STATUS_CONT_W     605
#define SIG_CHECK_STATUS_CONT_H     (RIGHT_BG_H - SIG_CHECK_LIST_H - 10)

//input tp
#define SIG_CHECK_INPUT_TP_X         4
#define SIG_CHECK_INPUT_TP_Y         3
#define SIG_CHECK_INPUT_TP_LW      (SIG_STATUS_BAR_LW+10)
#define SIG_CHECK_INPUT_TP_MW      150
#define SIG_CHECK_INPUT_TP_RW      60
#define SIG_CHECK_INPUT_TP_H         SIG_CHECK_ITEM_H

//input sym
#define SIG_CHECK_INPUT_SYM_X         4
#define SIG_CHECK_INPUT_SYM_Y         (SIG_CHECK_INPUT_TP_Y + SIG_CHECK_ITEM_H + SIG_CHECK_V_GAP)
#define SIG_CHECK_INPUT_SYM_LW      (SIG_STATUS_BAR_LW+10)
#define SIG_CHECK_INPUT_SYM_MW      150
#define SIG_CHECK_INPUT_SYM_RW      60
#define SIG_CHECK_INPUT_SYM_H         SIG_CHECK_ITEM_H

//select nim modulate
#define SIG_CHECK_INPUT_DEMODUL_X         4
#define SIG_CHECK_INPUT_DEMODUL_Y         (SIG_CHECK_INPUT_TP_Y + 2 * (SIG_CHECK_ITEM_H + SIG_CHECK_V_GAP))
#define SIG_CHECK_INPUT_DEMODUL_LW      (SIG_STATUS_BAR_LW+10)
#define SIG_CHECK_INPUT_DEMODUL_RW      150
#define SIG_CHECK_INPUT_DEMODUL_H         SIG_CHECK_ITEM_H

//signal status:comm bar
#define SIG_STATUS_BAR1_Y     (SIG_CHECK_INPUT_TP_Y + 3 * (SIG_CHECK_ITEM_H + SIG_CHECK_V_GAP))
#define SIG_STATUS_BAR_H       SIG_CHECK_ITEM_H

#define SIG_STATUS_BAR_LX       (4 +TEXT_START_OX)
#define SIG_STATUS_BAR_LW      120

#define SIG_STATUS_BAR_MX       (SIG_STATUS_BAR_LX + SIG_STATUS_BAR_LW)
#define SIG_STATUS_BAR_MW       350
#define SIG_STATUS_BAR_MH       COMM_PBAR_H

#define SIG_STATUS_BAR_RX       (SIG_STATUS_BAR_MX + SIG_STATUS_BAR_MW)
#define SIG_STATUS_BAR_RW      80

/* rect style */
#define RSI_SIG_CHECK_FRM               RSI_WINDOW_1

#define RSI_SIG_CHECK_PBAR_BG           RSI_COMM_PBAR_BG

/* font style */

/* others */

RET_CODE open_signal_check(u32 para1, u32 para2);

#endif


