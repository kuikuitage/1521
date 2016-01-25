/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
   Learner:   lucifer wang <yangwen.wang@montage-tech.com>
****************************************************************************/
#ifndef __UI_NVOD_H__
#define __UI_NVOD_H__

/* coordinate */
#define NVOD_CONT_X          0
#define NVOD_CONT_Y         ((SCREEN_HEIGHT - NVOD_CONT_H) / 2)
#define NVOD_CONT_W          SCREEN_WIDTH
#define NVOD_CONT_H          SCREEN_HEIGHT

//title_item
#define NVOD_TITLE_ITEM_X      WHOLE_SCR_TITLE_X
#define NVOD_TITLE_ITEM_Y      WHOLE_SCR_TITLE_Y
#define NVOD_TITLE_ITEM_H      WHOLE_SCR_TITLE_H
#define NVOD_TITLE_ITEM_1_W     126
#define NVOD_TITLE_ITEM_2_W     189

//plist1
#define REF_SVC_LIST_X    WHOLE_SCR_TITLE_X
#define REF_SVC_LIST_Y    RIGHT_BG_Y
#define REF_SVC_LIST_W   126
#define REF_SVC_LIST_H    190

//plist_list1
#define REF_SVC_LIST_MIDL  5
#define REF_SVC_LIST_MIDT  5
#define REF_SVC_LIST_MIDW  (REF_SVC_LIST_W - 2 * REF_SVC_LIST_MIDL)
#define REF_SVC_LIST_MIDH   (REF_SVC_LIST_H - 2 * REF_SVC_LIST_MIDT)
#define REF_SVC_LIST_VGAP         0
#define REF_SVC_LIST_FIELD        1

//plist2
#define REF_EVENT_LIST_X            (REF_SVC_LIST_X+REF_SVC_LIST_W+3)
#define REF_EVENT_LIST_Y            RIGHT_BG_Y
#define REF_EVENT_LIST_W            NVOD_TITLE_ITEM_2_W
#define REF_EVENT_LIST_H            190

#define REF_EVENT_LIST_MIDL  5
#define REF_EVENT_LIST_MIDT  5
#define REF_EVENT_LIST_MIDW  (REF_EVENT_LIST_W - 2 * REF_EVENT_LIST_MIDL)
#define REF_EVENT_LIST_MIDH (REF_EVENT_LIST_H - 2 * REF_EVENT_LIST_MIDT)
#define REF_EVENT_LIST_FIELD        2

//plist3
#define SHIFT_EVENT_LIST_X            (REF_EVENT_LIST_X+REF_EVENT_LIST_W+3)
#define SHIFT_EVENT_LIST_Y            RIGHT_BG_Y
#define SHIFT_EVENT_LIST_W            (WHOLE_SCR_TITLE_W -( NVOD_TITLE_ITEM_1_W+3+NVOD_TITLE_ITEM_2_W+4))
#define SHIFT_EVENT_LIST_H            190

#define SHIFT_EVENT_LIST_MIDL  5
#define SHIFT_EVENT_LIST_MIDT  5
#define SHIFT_EVENT_LIST_MIDW  (SHIFT_EVENT_LIST_W - 2 * SHIFT_EVENT_LIST_MIDL)
#define SHIFT_EVENT_LIST_MIDH  (SHIFT_EVENT_LIST_H - 2 * SHIFT_EVENT_LIST_MIDT)
#define SHIFT_EVENT_LIST_FIELD        3

//info
#define NVOD_INFO_X  REF_SVC_LIST_X
#define NVOD_INFO_Y  (REF_SVC_LIST_Y + REF_SVC_LIST_H + SUB_CONT_VGAP)
#define NVOD_INFO_W  WHOLE_SCR_TITLE_W
#define NVOD_INFO_H  (RIGHT_BG_H - REF_SVC_LIST_H - SUB_CONT_VGAP)

//remind string 
#define NVOD_REMIND_STR_X     ((NVOD_CONT_W - NVOD_REMIND_STR_W)/2)
#define NVOD_REMIND_STR_Y     (NVOD_TITLE_ITEM_Y + NVOD_TITLE_ITEM_H + 100)
#define NVOD_REMIND_STR_W     300

/* others */
#define NVOD_LIST_PAGE         6

RET_CODE open_nvod(u32 para1, u32 para2);

#endif

