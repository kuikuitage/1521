/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef _UI_EPG_H_
#define _UI_EPG_H_

#include "ui_common.h"

#if 0
//lvcm
//tv and detail container
#define SN_EPG_PREV_X              15//36
#define SN_EPG_PREV_Y              10//85
#define SN_EPG_PREV_W              255//245
#define SN_EPG_PREV_H              189//185

/*
//group title
#define SN_EPG_GROUP_NAME_X        36//296
#define SN_EPG_GROUP_NAME_Y        285//85
#define SN_EPG_GROUP_NAME_W        270//345
#define SN_EPG_GROUP_NAME_H        35
*/

//prog list
#define SN_EPG_PG_LISTX            SN_EPG_PREV_X
#define SN_EPG_PG_LISTY            SN_EPG_PREV_Y + SN_EPG_PREV_H + 10
#define SN_EPG_PG_LISTW            SN_EPG_PREV_W
#define SN_EPG_PG_LISTH            140

#define SN_EPG_PG_LIST_MIDL        4
#define SN_EPG_PG_LIST_MIDT        4
#define SN_EPG_PG_LIST_MIDW        SN_EPG_PREV_W - 8
#define SN_EPG_PG_LIST_MIDH        SN_EPG_PG_LISTH - 2*SN_EPG_PG_LIST_MIDT

/*
//week day
#define SN_EPG_WEEKDAY_X           366//296//36
#define SN_EPG_WEEKDAY_Y           85//85//285
#define SN_EPG_WEEKDAY_W           290//345//605
#define SN_EPG_WEEKDAY_H           0
*/

//event list
#define SN_EPG_EVENT_LISTX         SN_EPG_BAR_VERTICAL_X + 20//366//SN_EPG_WEEKDAY_X
#define SN_EPG_EVENT_LISTY         SN_EPG_PREV_Y//SN_EPG_WEEKDAY_Y + SN_EPG_WEEKDAY_H + 10
#define SN_EPG_EVENT_LISTW         310//SN_EPG_WEEKDAY_W
#define SN_EPG_EVENT_LISTH         220

#define SN_EPG_EVENT_LIST_MIDL     4
#define SN_EPG_EVENT_LIST_MIDT     4
#define SN_EPG_EVENT_LIST_MIDW     SN_EPG_EVENT_LISTW - 2*SN_EPG_EVENT_LIST_MIDL
#define SN_EPG_EVENT_LIST_MIDH     SN_EPG_EVENT_LISTH - 2*SN_EPG_EVENT_LIST_MIDT

#define SN_EPG_LIST_V_GAP          12

//advertisement window
#define SN_EPG_ADWIN_X             SN_EPG_EVENT_LISTX
#define SN_EPG_ADWIN_Y             SN_EPG_PREV_Y + SN_EPG_EVENT_LISTH + 20//350
#define SN_EPG_ADWIN_W             SN_EPG_EVENT_LISTW
#define SN_EPG_ADWIN_H             100

//bar vertical
#define SN_EPG_BAR_VERTICAL_X				 SN_EPG_PREV_X + SN_EPG_PREV_W + 20//350
#define SN_EPG_BAR_VERTICAL_Y				 SN_EPG_PREV_Y +20
#define SN_EPG_BAR_VERTICAL_W				 4
#define SN_EPG_BAR_VERTICAL_H				 290
//lvcm end
#endif


//lvcm
	//preview
#define SN_EPG_PREV_X  15//36
#define SN_EPG_PREV_Y  10//85
#define SN_EPG_PREV_W  255//245
#define SN_EPG_PREV_H  189//185
	
	//short event
#define SN_EPG_DETAIL_X  5
#define SN_EPG_DETAIL_Y  5
#define SN_EPG_DETAIL_W  245//(EPG_PREV_W - 2*EPG_DETAIL_X)
#define SN_EPG_DETAIL_H  179//(EPG_PREV_H - 2*EPG_DETAIL_Y)
	
	//group name
#define SN_EPG_GROUP_NAME_X    SN_EPG_PREV_X//275//(EPG_PREV_X + EPG_PREV_W + 15)
#define SN_EPG_GROUP_NAME_Y    SN_EPG_PREV_Y + SN_EPG_PREV_H + 15//10//EPG_PREV_Y
#define SN_EPG_GROUP_NAME_W    SN_EPG_PREV_W//345
#define SN_EPG_GROUP_NAME_H    31//35


#define SN_EPG_ARROW_LEFT_X    20
#define SN_EPG_ARROW_RIGHT_X   SN_EPG_PREV_W - SN_EPG_ARROW_W - SN_EPG_ARROW_LEFT_X
#define SN_EPG_ARROW_Y         (SN_EPG_GROUP_NAME_H - SN_EPG_ARROW_H)/2
#define SN_EPG_ARROW_W         15
#define SN_EPG_ARROW_H         18
						
#define SN_EPG_LIST_H_GAP    4//5
#define SN_EPG_LIST_V_GAP     5//0
	
	//prog list
#define SN_EPG_PG_LISTX        SN_EPG_PREV_X//275//EPG_GROUP_NAME_X
#define SN_EPG_PG_LISTY        SN_EPG_GROUP_NAME_Y + SN_EPG_GROUP_NAME_H + 5//55//(EPG_GROUP_NAME_Y +EPG_GROUP_NAME_H +10)
#define SN_EPG_PG_LISTW        SN_EPG_PREV_W//345//EPG_GROUP_NAME_W
#define SN_EPG_PG_LISTH        110//140
	
#define SN_EPG_PG_LIST_MIDL  4
#define SN_EPG_PG_LIST_MIDT  4
#define SN_EPG_PG_LIST_MIDW  SN_EPG_PREV_W - 2*SN_EPG_PG_LIST_MIDL//321+16//(EPG_PG_LISTW - 2 * EPG_PG_LIST_MIDL - EPG_PG_LIST_SBARW)
#define SN_EPG_PG_LIST_MIDH  SN_EPG_PG_LISTH - 2*SN_EPG_PG_LIST_MIDT//92//132//(EPG_PG_LISTH - 2 * EPG_PG_LIST_MIDT)
	
	//prog list scroll bar
#define SN_EPG_PG_LIST_SBARX  325//(EPG_PG_LISTW - EPG_PG_LIST_SBARW -4)
#define SN_EPG_PG_LIST_SBARY  4//EPG_PG_LIST_MIDT
#define SN_EPG_PG_LIST_SBARW  16//COMM_LIST_BAR_W
#define SN_EPG_PG_LIST_SBARH  132//EPG_PG_LIST_MIDH
	
	//weekday
#define SN_EPG_WEEKDAY_X     15//EPG_PREV_X
#define SN_EPG_WEEKDAY_Y     210//(EPG_PREV_Y + EPG_PREV_H + 15)
#define SN_EPG_WEEKDAY_W    605
#define SN_EPG_WEEKDAY_H     35
	
	//event list
#define SN_EPG_EVENT_LISTX   SN_EPG_BAR_VERTICAL_X + 24//15//EPG_PREV_X
#define SN_EPG_EVENT_LISTY   (31+SN_EPG_PREV_Y)//210//255//(EPG_WEEKDAY_Y + EPG_WEEKDAY_H + 10)
#define SN_EPG_EVENT_LISTW   310//605//EPG_WEEKDAY_W
#define SN_EPG_EVENT_LISTH   189//140
	
#define SN_EPG_EVENT_LIST_MIDL  4
#define SN_EPG_EVENT_LIST_MIDT  4
#define SN_EPG_EVENT_LIST_MIDW  SN_EPG_EVENT_LISTW - 2*SN_EPG_EVENT_LIST_MIDL//581//(EPG_EVENT_LISTW - 2 * EPG_EVENT_LIST_MIDL - EPG_EVENT_LIST_SBARW)
#define SN_EPG_EVENT_LIST_MIDH  SN_EPG_EVENT_LISTH - 2*SN_EPG_EVENT_LIST_MIDT//132//(EPG_EVENT_LISTH - 2 * EPG_EVENT_LIST_MIDT)
	
	//event list scroll bar
#define SN_EPG_EVENT_LIST_SBARX  585//(EPG_EVENT_LISTW - EPG_EVENT_LIST_SBARW - 4)
#define SN_EPG_EVENT_LIST_SBARY  4//EPG_EVENT_LIST_MIDT
#define SN_EPG_EVENT_LIST_SBARW  16//COMM_LIST_BAR_W
#define SN_EPG_EVENT_LIST_SBARH  132//EPG_EVENT_LIST_MIDH

//advertisement window
#define SN_EPG_ADWIN_X             SN_EPG_EVENT_LISTX
#define SN_EPG_ADWIN_Y             SN_EPG_PREV_Y + SN_EPG_EVENT_LISTH + 51//350
#define SN_EPG_ADWIN_W             SN_EPG_EVENT_LISTW
#define SN_EPG_ADWIN_H             100

//bar vertical
#define SN_EPG_BAR_VERTICAL_X				 SN_EPG_PREV_X + SN_EPG_PREV_W + 20//350
#define SN_EPG_BAR_VERTICAL_Y				 SN_EPG_PREV_Y +20
#define SN_EPG_BAR_VERTICAL_W				 4
#define SN_EPG_BAR_VERTICAL_H				 290

#define SN_SUB_MENU_HELP_RSC_CNT             16

//lvcm end


/*coordinates*/
#define EPG_MENU_X  (SCREEN_WIDTH - EPG_MENU_W)/2
#define EPG_MENU_Y  (SCREEN_HEIGHT - EPG_MENU_H)/2
#define EPG_MENU_W  SCREEN_WIDTH
#define EPG_MENU_H  SCREEN_HEIGHT

//preview
#define EPG_PREV_X  36
#define EPG_PREV_Y  85
#define EPG_PREV_W  245
#define EPG_PREV_H  185

//short event
#define EPG_DETAIL_X  8
#define EPG_DETAIL_Y  8
#define EPG_DETAIL_W  (EPG_PREV_W - 2*EPG_DETAIL_X)
#define EPG_DETAIL_H  (EPG_PREV_H - 2*EPG_DETAIL_Y)

//group name
#define EPG_GROUP_NAME_X    (EPG_PREV_X + EPG_PREV_W + 15)
#define EPG_GROUP_NAME_Y    EPG_PREV_Y
#define EPG_GROUP_NAME_W    345
#define EPG_GROUP_NAME_H     35

#define EPG_LIST_H_GAP    5
#define EPG_LIST_V_GAP     0

//prog list
#define EPG_PG_LISTX        EPG_GROUP_NAME_X
#define EPG_PG_LISTY        (EPG_GROUP_NAME_Y +EPG_GROUP_NAME_H +10)
#define EPG_PG_LISTW        EPG_GROUP_NAME_W
#define EPG_PG_LISTH        140

#define EPG_PG_LIST_MIDL  4
#define EPG_PG_LIST_MIDT  4
#define EPG_PG_LIST_MIDW  (EPG_PG_LISTW - 2 * EPG_PG_LIST_MIDL - EPG_PG_LIST_SBARW)
#define EPG_PG_LIST_MIDH  (EPG_PG_LISTH - 2 * EPG_PG_LIST_MIDT)

//prog list scroll bar
#define EPG_PG_LIST_SBARX  (EPG_PG_LISTW - EPG_PG_LIST_SBARW -4)
#define EPG_PG_LIST_SBARY  EPG_PG_LIST_MIDT
#define EPG_PG_LIST_SBARW  COMM_LIST_BAR_W
#define EPG_PG_LIST_SBARH  EPG_PG_LIST_MIDH

//weekday
#define EPG_WEEKDAY_X     EPG_PREV_X-5
#define EPG_WEEKDAY_Y     10
#define EPG_WEEKDAY_W    330
#define EPG_WEEKDAY_H     31

//event list
#define EPG_EVENT_LISTX   EPG_PREV_X
#define EPG_EVENT_LISTY   (EPG_WEEKDAY_Y + EPG_WEEKDAY_H + 10)
#define EPG_EVENT_LISTW   EPG_WEEKDAY_W
#define EPG_EVENT_LISTH   140

#define EPG_EVENT_LIST_MIDL  0
#define EPG_EVENT_LIST_MIDT  4
#define EPG_EVENT_LIST_MIDW  (EPG_EVENT_LISTW - 2 * EPG_EVENT_LIST_MIDL - EPG_EVENT_LIST_SBARW)
#define EPG_EVENT_LIST_MIDH  (EPG_EVENT_LISTH - 2 * EPG_EVENT_LIST_MIDT)

//event list scroll bar
#define EPG_EVENT_LIST_SBARX  (EPG_EVENT_LISTW - EPG_EVENT_LIST_SBARW - 4)
#define EPG_EVENT_LIST_SBARY  EPG_EVENT_LIST_MIDT
#define EPG_EVENT_LIST_SBARW  COMM_LIST_BAR_W
#define EPG_EVENT_LIST_SBARH  EPG_EVENT_LIST_MIDH

/*rstyle*/
#define RSI_EPG_MENU           RSI_IGNORE
//#define RSI_EPG_PREV           RSI_PREVIEW_TV_WIN
#define RSI_EPG_DETAIL         RSI_WHITE_YELLOW
#define RSI_EPG_TIME           RSI_ITEM_3_SH
//#define RSI_EPG_SBAR           RSI_SCROLL_BAR_BG
//#define RSI_EPG_SBAR_MID       RSI_SCROLL_BAR_MID
#define RSI_EPG_PROG_LIST     RSI_WINDOW_2


/*others*/
#define EPG_PROG_LIST_PAGE  3//5
#define EPG_PROG_LIST_FIELD 2

#define EPG_EVENT_LIST_PAGE 5//5
#define EPG_EVENT_LIST_FIELD 4


#define EPG_LCONT_MBOX_TOL  4
#define EPG_LCONT_MBOX_COL  4
#define EPG_LCONT_MBOX_ROW  1
#define EPG_LCONT_MBOX_HGAP  0
#define EPG_LCONT_MBOX_VGAP  0

#define EPG_INFO_MBOX_TOL  2
#define EPG_INFO_MBOX_COL  2
#define EPG_INFO_MBOX_ROW  1
#define EPG_INFO_MBOX_HGAP  4
#define EPG_INFO_MBOX_VGAP  0

#define EPG_TIME_PBAR_MIN 0
#define EPG_TIME_PBAR_MAX 36

#define EPG_TIME_TOT_HOURS 2

#define EPG_TIME_STEP_MAX (17*24*2)//17 days

/*fstyle*/
#define FSI_EPG_LCONT_MBOX FSI_WHITE
#define FSI_EPG_INFO_MBOX FSI_WHITE

/*others*/
#define SEVEN 7
#define ONE_DAY_SECONDS 86400 //24*60*60

RET_CODE open_epg(u32 para1, u32 para2);

void prebook_set_epg_event_list_update(control_t* p_ctrl);

#endif
