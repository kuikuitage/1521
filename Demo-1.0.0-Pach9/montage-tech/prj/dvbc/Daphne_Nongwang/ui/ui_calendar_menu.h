/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef _UI_CALENDAR_MENU_H_
#define _UI_CALENDAR_MENU_H_
/*!
 \file ui_calendar_menu.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in calendar menu.
   And interfaces for open a calendar menu.
   
   Development policy:
   If you want to change the menu style, you will just need to modify the macro 
   definitions only.
 */

/* coordinate */
//root container coordinate
#define CALENDAR_MENU_CONT_X      ((SCREEN_WIDTH-CALENDAR_MENU_CONT_W)/2)
#define CALENDAR_MENU_CONT_Y      0
#define CALENDAR_MENU_CONT_W      SCREEN_WIDTH
#define CALENDAR_MENU_CONT_H      SCREEN_HEIGHT//400//304

#define CALENDAR_TITLE_H      35

#define SOLAR_TXT_T           30//top interval

//日期生肖背景
#define SOLAR_TITLE_BG_X           (WHOLE_SCR_TITLE_X - SCREEN_OFFSET_X)
#define SOLAR_TITLE_BG_Y           65
#define SOLAR_TITLE_BG_W           610
#define SOLAR_TITLE_BG_H           CALENDAR_TITLE_H

//公历日期坐标
#define SOLAR_TXT_B           5//botton interval
#define SOLAR_TXT_X           (SOLAR_TITLE_BG_X + 30)
#define SOLAR_TXT_Y           SOLAR_TITLE_BG_Y
#define SOLAR_TXT_H           CALENDAR_TITLE_H

#define SOLAR_1_CBOX_W        50
#define SOLAR_2_TEXT_W        25
#define SOLAR_3_CBOX_W        25
#define SOLAR_4_TEXT_W        25

#define LUNAR_TXT_W           300
#define LUNAR_TXT_H           SOLAR_TXT_H

//生肖坐标
#define ANIMAL_TXT_W          100
#define ANIMAL_TXT_H          SOLAR_TXT_H

//日历坐标
#define WEEK_MBOX_X           SOLAR_TITLE_BG_X
#define WEEK_MBOX_Y           (SOLAR_TXT_Y + SOLAR_TXT_B + SOLAR_TXT_H)
#define WEEK_MBOX_W           (CALENDAR_MENU_CONT_W - 2 * SOLAR_TXT_T)
#define WEEK_MBOX_H           CALENDAR_TITLE_H

#define DAY_MBOX_X            WEEK_MBOX_X
#define DAY_MBOX_Y            (WEEK_MBOX_Y + WEEK_MBOX_H + 6)
#define DAY_MBOX_W            WEEK_MBOX_W
#define DAY_MBOX_H            200//225

//宜忌坐标，农历日期坐标
#define INFO_CONT_X           SOLAR_TITLE_BG_X
#define INFO_CONT_Y           (DAY_MBOX_Y + DAY_MBOX_H)
#define INFO_CONT_W           (CALENDAR_MENU_CONT_W - 2*SOLAR_TITLE_BG_X)
#define INFO_CONT_H           (INFO_YI_H + INFO_JI_H + INFO_YI_JI_I+ 2 * INFO_YI_Y)

#define INFO_LUNAR_X          2
#define INFO_LUNAR_Y          2
#define INFO_LUNAR_W          48
#define INFO_LUNAR_H          75//(3 * CALENDAR_TITLE_H)
#define INFO_LHOL_X           INFO_LUNAR_X
#define INFO_LHOL_Y           (INFO_LUNAR_Y + INFO_LUNAR_H + INFO_YI_JI_I)
#define INFO_LHOL_W           INFO_LUNAR_W
#define INFO_LHOL_H           (1 * CALENDAR_TITLE_H)
#define INFO_YI_X             (INFO_LUNAR_X + INFO_LUNAR_W)
#define INFO_YI_Y             INFO_LUNAR_Y
#define INFO_YI_W             (CALENDAR_MENU_CONT_W - 2*SOLAR_TITLE_BG_X - 2 * INFO_LUNAR_X - INFO_LUNAR_W)
#define INFO_YI_H             INFO_LUNAR_H

#define INFO_YI_JI_I          0

#define INFO_JI_X             INFO_YI_X
#define INFO_JI_Y             INFO_LHOL_Y
#define INFO_JI_W             INFO_YI_W
#define INFO_JI_H             (1 * CALENDAR_TITLE_H)
/* rect style */
//#define RSI_TXT               RSI_SECOND_TITLE
#define RSI_MBOX_WEEK_ITEM_SH RSI_IGNORE
#define RSI_MBOX_WEEK_ITEM_HL RSI_WHITE_YELLOW

#define RSI_MBOX_DAY_ITEM_SH  RSI_WHITE_YELLOW
#define RSI_MBOX_DAY_ITEM_HL  RSI_ORANGE//RSI_WHITE_GREEN
#define RSI_INFO_CONT_SH      RSI_IGNORE
#define RSI_INFO_CONT_HL      RSI_IGNORE

/* font style */
#define FSI_TXT               FSI_COMM_TXT_N//公历，阴历，生肖等文字的颜色
#define FSI_MBOX_WEEK_TEXT    FSI_COMM_TXT_N//星期栏的颜色
#define FSI_MBOX_DAY_TEXT_F   FSI_COMM_TXT_HL//日历主体的颜色
#define FSI_MBOX_DAY_TEXT_N   FSI_COMM_TXT_N

/* others */
#define CALENDAR_MBOX_DAY_TOL (CALENDAR_MBOX_DAY_COL * CALENDAR_MBOX_DAY_ROW - 5)
#define CALENDAR_MBOX_DAY_COL 7
#define CALENDAR_MBOX_DAY_ROW 6

#define CALENDAR_MBOX_WEEK_TOL  (CALENDAR_MBOX_WEEK_COL * CALENDAR_MBOX_WEEK_ROW)
#define CALENDAR_MBOX_WEEK_COL  7
#define CALENDAR_MBOX_WEEK_ROW  1

#define YEAR_MIN              1901
#define YEAR_MAX              2050
#define YEAR_STEP             1
#define YEAR_BLEN             4
#define MONTH_MIN             1
#define MONTH_MAX             12
#define MONTH_STEP            1
#define MONTH_BLEN            2

#define YEAR_CHANGED          0x01
#define MONTH_CHANGED         0x02
#define DAY_CHANGED           0x04
#define FIRST_DRAW            0X08
/*!
Interface for open a calendar menu
*/
s32 open_calendar_menu(u32 para1, u32 para2);
#endif

