/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
   Learner:   lucifer wang <yangwen.wang@montage-tech.com>
****************************************************************************/
#ifndef __UI_PREBOOK_MANAGE_H__
#define __UI_PREBOOK_MANAGE_H__

enum book_update_msg
{
  MSG_BOOK_UPDATE = MSG_LOCAL_BEGIN + 450,
};

/* coordinate */
#define PREBOOK_MENU_X          0
#define PREBOOK_MENU_Y          ((SCREEN_HEIGHT - PREBOOK_MENU_H) / 2)
#define PREBOOK_MENU_W          SCREEN_WIDTH
#define PREBOOK_MENU_H          SCREEN_HEIGHT

//second title cont
#define BOOK_TITLE_CONT_X  (WHOLE_SCR_TITLE_X)
#define BOOK_TITLE_CONT_Y  WHOLE_SCR_TITLE_Y
#define BOOK_TITLE_CONT_W WHOLE_SCR_TITLE_W
#define BOOK_TITLE_CONT_H  WHOLE_SCR_TITLE_H

//book prog list
#define BOOK_LIST_LIST_X    BOOK_TITLE_CONT_X
#define BOOK_LIST_LIST_Y    RIGHT_BG_Y
#define BOOK_LIST_LIST_W   BOOK_TITLE_CONT_W
#define BOOK_LIST_LIST_H    RIGHT_BG_H

//plist_list
#define BOOK_LIST_LIST_MIDL  4
#define BOOK_LIST_LIST_MIDT  4
#define BOOK_LIST_LIST_MIDW  (BOOK_LIST_LIST_W - 2 * BOOK_LIST_LIST_MIDL - BOOK_LIST_SBAR_W - 2)
#define BOOK_LIST_LIST_MIDH   (BOOK_LIST_LIST_H - 2 * BOOK_LIST_LIST_MIDT)
#define BOOK_LIST_LIST_VGAP   0
#define BOOK_LIST_LIST_PAGE   8
#define BOOK_LIST_LIST_FIELD   6

//sbar 
#define BOOK_LIST_SBAR_X       (BOOK_LIST_LIST_W - BOOK_LIST_SBAR_W - 2)
#define BOOK_LIST_SBAR_Y       BOOK_LIST_LIST_MIDT
#define BOOK_LIST_SBAR_W      COMM_LIST_BAR_W
#define BOOK_LIST_SBAR_H       BOOK_LIST_LIST_MIDH

#define TITLE_ITEM_CNT         5

//title_item
#define TITLE_ITEM_Y			65
#define TITLE_ITEM_H			35

/* font style */
#define PREBOOK_PROG_LIST_PAGE  7//6
#define PREBOOK_PROG_LIST_FIELD 5
/* others */

RET_CODE open_prebook_manage(u32 para1, u32 para2);

#endif

