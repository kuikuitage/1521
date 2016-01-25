/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_PASSWORD_MODIFY_H__
#define __UI_PASSWORD_MODIFY_H__

/* coordinate */
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

#define PSW_MODIFY_MENU_X          ((SN_SCREEN_WIDTH - PSW_MODIFY_MENU_W) / 2)
#define PSW_MODIFY_MENU_Y          ((SN_SCREEN_HEIGHT - PSW_MODIFY_MENU_H) / 2)
#define PSW_MODIFY_MENU_W          SN_SCREEN_WIDTH
#define PSW_MODIFY_MENU_H          SN_SCREEN_HEIGHT

//small cont1: power on password
#define PSW_MODIFY_CONT1_X  RIGHT_BG_X
#define PSW_MODIFY_CONT1_Y  RIGHT_BG_Y
#define PSW_MODIFY_CONT1_W  RIGHT_BG_W
#define PSW_MODIFY_CONT1_H  135

//power on password item
#define PSW_POWER_ON_ITEM_X  (PSW_MODIFY_CONT1_X + TEXT_START_OX)
#define PSW_POWER_ON_ITEM_Y  (PSW_MODIFY_CONT1_Y + 2)
#define PSW_POWER_ON_ITEM_W  (SECOND_TITLE_W - 4)
#define PSW_POWER_ON_ITEM_H  PSW_MODIFY_ITEM_H

//small cont2: lock channel password
#define PSW_MODIFY_CONT2_X  RIGHT_BG_X
#define PSW_MODIFY_CONT2_Y  (PSW_MODIFY_CONT1_Y + PSW_MODIFY_CONT1_H + PSW_MODIFY_CONT_V_GAP)
#define PSW_MODIFY_CONT2_W  SECOND_TITLE_W
#define PSW_MODIFY_CONT2_H  135

//lock channel password item
#define PSW_LOCK_CHL_ITEM_X  (SECOND_TITLE_X + TEXT_START_OX)
#define PSW_LOCK_CHL_ITEM_Y  (PSW_MODIFY_CONT2_Y + 2)
#define PSW_LOCK_CHL_ITEM_W  (SECOND_TITLE_W - 4)
#define PSW_LOCK_CHL_ITEM_H  PSW_MODIFY_ITEM_H

//brief string
#define PSW_MODIFY_BRIEF_X  SECOND_TITLE_X
#define PSW_MODIFY_BRIEF_Y  (PSW_MODIFY_CONT2_Y + PSW_MODIFY_CONT2_H + PSW_MODIFY_CONT_V_GAP)
#define PSW_MODIFY_BRIEF_W  SECOND_TITLE_W
#define PSW_MODIFY_BRIEF_H  (RIGHT_BG_H-PSW_MODIFY_CONT1_H-PSW_MODIFY_CONT2_H-PSW_MODIFY_CONT_V_GAP*2)

#define PSW_MODIFY_ITEM_CNT      6
#define PSW_MODIFY_ITEM_X        (PSW_MODIFY_CONT1_X + PSW_MODIFY_ITEM_H_GAP)
#define PSW_MODIFY_ITEM_Y1        (PSW_POWER_ON_ITEM_Y + PSW_MODIFY_ITEM_H + PSW_MODIFY_ITEM_V_GAP)
#define PSW_MODIFY_ITEM_Y2        (PSW_LOCK_CHL_ITEM_Y + PSW_MODIFY_ITEM_H + PSW_MODIFY_ITEM_V_GAP)
#define PSW_MODIFY_ITEM_LW       140
#define PSW_MODIFY_ITEM_RW       140

#define PSW_MODIFY_ITEM_H        30
#define PSW_MODIFY_ITEM_V_GAP    0
#define PSW_MODIFY_ITEM_H_GAP    14
#define PSW_MODIFY_CONT_V_GAP    5

/* rect style */
#define RSI_TEXT_ITEM       RSI_IGNORE
/* font style */

/* others */
#define MAX_PROMPT_STR_SIZE   64
RET_CODE preopen_password_modify(u32 para1, u32 para2);

RET_CODE open_password_modify(u32 para1, u32 para2);

#endif


