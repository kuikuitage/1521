/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_PROG_BAR_H__
#define __UI_PROG_BAR_H__

/* coordinate */
#define PROG_BAR_MENU_X             0
#define PROG_BAR_MENU_Y             380// (SCREEN_HEIGHT - PROG_BAR_MENU_H)
#define PROG_BAR_MENU_W             SCREEN_WIDTH//672
#define PROG_BAR_MENU_H             122//6

//introdurce cont
#define PROG_BAR_INTRO_CONT_X              (MENU_CONT_OX + 5)
#define PROG_BAR_INTRO_CONT_Y              50//100
#define PROG_BAR_INTRO_CONT_W              380
#define PROG_BAR_INTRO_CONT_H              140

//introduce title
#define PROG_BAR_INTRO_TITLE_X              0
#define PROG_BAR_INTRO_TITLE_Y              0
#define PROG_BAR_INTRO_TITLE_W              PROG_BAR_INTRO_CONT_W
#define PROG_BAR_INTRO_TITLE_H              30

//introduce detail
#define PROG_BAR_INTRO_DETAIL_X              0
#define PROG_BAR_INTRO_DETAIL_Y              (PROG_BAR_INTRO_TITLE_Y+PROG_BAR_INTRO_TITLE_H)
#define PROG_BAR_INTRO_DETAIL_W              PROG_BAR_INTRO_CONT_W
#define PROG_BAR_INTRO_DETAIL_H              100


//prog info cont
#define PROG_BAR_FRM_X              PROG_BAR_INTRO_CONT_X
#define PROG_BAR_FRM_Y              173
#define PROG_BAR_FRM_W              390
#define PROG_BAR_FRM_H              SUB_AD_H

//prog info cont:pg num
#define PROG_BAR_PG_NUN_X    0
#define PROG_BAR_PG_NUN_Y    0
#ifdef LCN_SWITCH
#define PROG_BAR_PG_NUN_W   60
#else
#define PROG_BAR_PG_NUN_W   55
#endif
#define PROG_BAR_PG_NUN_H    INFOBOX_TITLE_H

//prog info cont:pg name
#define PROG_BAR_PG_NAME_X        (PROG_BAR_PG_NUN_X + PROG_BAR_PG_NUN_W)
#define PROG_BAR_PG_NAME_Y        0
#define PROG_BAR_PG_NAME_W       155
#define PROG_BAR_PG_NAME_H        PROG_BAR_PG_NUN_H

//prog info cont:time
#define PROG_BAR_TIME_X         (PROG_BAR_FRM_W-PROG_BAR_TIME_W)
#define PROG_BAR_TIME_Y         0
#define PROG_BAR_TIME_W        175
#define PROG_BAR_TIME_H         PROG_BAR_PG_NUN_H


//epg
#define PROG_BAR_PF_INFO_CNT        2
#define PROG_BAR_PF_INFO_X          10
#define PROG_BAR_PF_INFO_Y          42
#define PROG_BAR_PF_INFO_W         (PROG_BAR_INTRO_CONT_W - PROG_BAR_PF_INFO_X * 2)// 292
#define PROG_BAR_PF_INFO_H          26
#define PROG_BAR_PF_INFO_V_GAP      2

//advertisement
#define PROG_BAR_AD_X              (PROG_BAR_FRM_X+PROG_BAR_FRM_W+5)
#define PROG_BAR_AD_Y               PROG_BAR_FRM_Y
#define PROG_BAR_AD_W             SUB_AD_W
#define PROG_BAR_AD_H              SUB_AD_H

/* rect style */

/* font style */
#define FSI_PROG_BAR_NAME           FSI_COMM_TXT_N


void fill_prog_info(control_t *cont, BOOL is_redraw, u16 prog_id);

RET_CODE open_prog_bar(u32 para1, u32 para2);

#endif

