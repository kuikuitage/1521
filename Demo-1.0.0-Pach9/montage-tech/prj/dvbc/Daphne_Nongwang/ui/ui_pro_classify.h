/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
   Learner:   lucifer wang <yangwen.wang@montage-tech.com>
****************************************************************************/
#ifndef __UI_PRO_CLASSIFY_H__
#define __UI_PRO_CLASSIFY_H__

//lvcm
//title_select_item
#define SN_CLASSIFY_ITEM_CNT		      11
#define SN_CLASSIFY_ITEM_X		          (640 - SN_CLASSIFY_ITEM_W)/2//181
#define SN_CLASSIFY_ITEM_Y			      5//65
#define SN_CLASSIFY_ITEM_W		          350//310
#define SN_CLASSIFY_ITEM_H			      31

//bar little arrow
#define SN_CLASSIFY_BAR_ARROW_LEFT_X        20
#define SN_CLASSIFY_BAR_ARROW_RIGHT_X       SN_CLASSIFY_ITEM_W - SN_CLASSIFY_BAR_ARROW_W - SN_CLASSIFY_BAR_ARROW_LEFT_X
#define SN_CLASSIFY_BAR_ARROW_Y             (SN_CLASSIFY_ITEM_H - SN_CLASSIFY_BAR_ARROW_H)/2
#define SN_CLASSIFY_BAR_ARROW_W             15
#define SN_CLASSIFY_BAR_ARROW_H             18


//plist_bg1
#define SN_CLASSIFY_LIST_BG1_X            (640 - SN_CLASSIFY_LIST_BG1_W)/2//36//WHOLE_SCR_TITLE_X
#define SN_CLASSIFY_LIST_BG1_Y            SN_CLASSIFY_ITEM_Y + SN_CLASSIFY_ITEM_H + 2//105//(CLASSIFY_ITEM_Y + CLASSIFY_ITEM_H + 5)
#define SN_CLASSIFY_LIST_BG1_W            605//WHOLE_SCR_TITLE_W
#define SN_CLASSIFY_LIST_BG1_H            195//270

//plist
#define SN_CLASSIFY_LIST_LIST_X           0
#define SN_CLASSIFY_LIST_LIST_Y           0
#define SN_CLASSIFY_LIST_LIST_W           605//(WHOLE_SCR_TITLE_W)
#define SN_CLASSIFY_LIST_LIST_H           SN_CLASSIFY_LIST_BG1_H

#define SN_CLASSIFY_LIST_MIDL             4
#define SN_CLASSIFY_LIST_MIDT             4
#define SN_CLASSIFY_LIST_MIDW             SN_CLASSIFY_LIST_LIST_W - 2*SN_CLASSIFY_LIST_MIDL
#define SN_CLASSIFY_LIST_MIDH             SN_CLASSIFY_LIST_LIST_H - 2*SN_CLASSIFY_LIST_MIDT
#define SN_CLASSIFY_LIST_VGAP             0

//advertisement window
#define SN_PROCLASSIFY_AD_X               SN_CLASSIFY_LIST_BG1_X//36//WHOLE_SCR_TITLE_X
#define SN_PROCLASSIFY_AD_Y               SN_CLASSIFY_LIST_BG1_Y + SN_CLASSIFY_LIST_BG1_H + 10//380//CLASSIFY_LIST_BG1_Y+275
#define SN_PROCLASSIFY_AD_W               245//SUB_AD_W
#define SN_PROCLASSIFY_AD_H               110//SUB_AD_H

// pro_info 2
#define SN_PRO_INFO_BG2_X                 (SN_PROCLASSIFY_AD_X + SN_PROCLASSIFY_AD_W + 10)
#define SN_PRO_INFO_BG2_Y                 SN_PROCLASSIFY_AD_Y
#define SN_PRO_INFO_BG2_W                 (SN_CLASSIFY_LIST_BG1_W - SN_PROCLASSIFY_AD_W - 10)
#define SN_PRO_INFO_BG2_H                 SN_PROCLASSIFY_AD_H

//sbar 
#define SN_CLASSIFY_LIST_SBAR_X           (SN_CLASSIFY_LIST_LIST_W -SN_CLASSIFY_LIST_SBAR_W - 4)
#define SN_CLASSIFY_LIST_SBAR_W           16//COMM_LIST_BAR_W
#define SN_CLASSIFY_LIST_SBAR_H           (266)

#define SN_PRO_CLASSIFY_HELP_RSC_CNT	15

//lvcm end


/* coordinate */
#define PROCLASSIFY_CONT_X          0
#define PROCLASSIFY_CONT_Y          ((SCREEN_HEIGHT - PROCLASSIFY_CONT_H) / 2)
#define PROCLASSIFY_CONT_W          SCREEN_WIDTH
#define PROCLASSIFY_CONT_H          SCREEN_HEIGHT


//advertisement window
#define PROCLASSIFY_AD_X     WHOLE_SCR_TITLE_X
#define PROCLASSIFY_AD_Y     CLASSIFY_LIST_BG1_Y+275
#define PROCLASSIFY_AD_W     40//245//SUB_AD_W
#define PROCLASSIFY_AD_H     30//110//SUB_AD_H


//plist_bg1
#define CLASSIFY_LIST_BG1_X  WHOLE_SCR_TITLE_X
#define CLASSIFY_LIST_BG1_Y  (CLASSIFY_ITEM_Y + CLASSIFY_ITEM_H + 5)
#define CLASSIFY_LIST_BG1_W  WHOLE_SCR_TITLE_W
#define CLASSIFY_LIST_BG1_H  270

// pro_info 2
#define PRO_INFO_BG2_X  (PROCLASSIFY_AD_X + PROCLASSIFY_AD_W + 10)
#define PRO_INFO_BG2_Y  PROCLASSIFY_AD_Y
#define PRO_INFO_BG2_W (CLASSIFY_LIST_BG1_W - PROCLASSIFY_AD_W - 10)
#define PRO_INFO_BG2_H PROCLASSIFY_AD_H

//plist
#define CLASSIFY_LIST_LIST_X            0
#define CLASSIFY_LIST_LIST_Y            0
#define CLASSIFY_LIST_LIST_W           (WHOLE_SCR_TITLE_W)
#define CLASSIFY_LIST_LIST_H            CLASSIFY_LIST_BG1_H


//sbar 
#define CLASSIFY_LIST_SBAR_X     (CLASSIFY_LIST_LIST_W -CLASSIFY_LIST_SBAR_W - 4)
#define CLASSIFY_LIST_SBAR_W            COMM_LIST_BAR_W
#define CLASSIFY_LIST_SBAR_H             (266)

//title_select_item
#define CLASSIFY_ITEM_CNT		11
#define CLASSIFY_ITEM_X			181
#define CLASSIFY_ITEM_Y			65
#define CLASSIFY_ITEM_W		310
#define CLASSIFY_ITEM_H			35

#define CLASSIFY_PROG_LIST_PAGE 6//8
#define CLASSIFY_PROG_LIST_FIELD 3

/* rect style */
#define RSI_CLASSIFY_LIST_SBAR          RSI_SCROLL_BAR_BG
#define RSI_CLASSIFY_LIST_SBAR_MID      RSI_SCROLL_BAR_MID
/* font style */

/* others */

RET_CODE open_pro_classify(u32 para1, u32 para2);

#endif

