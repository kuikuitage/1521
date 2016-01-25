/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_PROG_LIST_H__
#define __UI_PROG_LIST_H__

#include "ui_common.h"

//lvcm
//TV preview window
#define SN_PROG_PREV_X         30
#define SN_PROG_PREV_Y         10
#define SN_PROG_PREV_W         255
#define SN_PROG_PREV_H         189
  
//PLIST prog list cont
#define SN_PLIST_PGLIST_CONT_X     SN_PROG_PREV_X + SN_PROG_PREV_W + 25//330
#define SN_PLIST_PGLIST_CONT_Y	 SN_PROG_PREV_Y
#define SN_PLIST_PGLIST_CONT_W	 300
#define SN_PLIST_PGLIST_CONT_H	 310

//group name bg
#define SN_PLIST_NAME_BACKGROUND_X	     50
#define SN_PLIST_NAME_BACKGROUND_Y	     0
#define SN_PLIST_NAME_BACKGROUND_W	     200
#define SN_PLIST_NAME_BACKGROUND_H         40

//group name text
#define SN_PLIST_GROUP_NAME_X	       SN_PLIST_NAME_BACKGROUND_X
#define SN_PLIST_GROUP_NAME_Y	       SN_PLIST_NAME_BACKGROUND_Y
#define SN_PLIST_GROUP_NAME_W	       SN_PLIST_NAME_BACKGROUND_W
#define SN_PLIST_GROUP_NAME_H        SN_PLIST_NAME_BACKGROUND_H  

//group name arrow
#define SN_PLIST_ARROW_LEFT_X        0
#define SN_PLIST_ARROW_RIGHT_X       SN_PLIST_GROUP_NAME_X + SN_PLIST_NAME_BACKGROUND_W
#define SN_PLIST_ARROW_Y             SN_PLIST_NAME_BACKGROUND_Y
#define SN_PLIST_ARROW_W             (SN_PLIST_PGLIST_CONT_W - SN_PLIST_NAME_BACKGROUND_W)/2
#define SN_PLIST_ARROW_H             SN_PLIST_NAME_BACKGROUND_H

//prog plist
#define SN_PLIST_PGLIST_X            0
#define SN_PLIST_PGLIST_Y            SN_PLIST_NAME_BACKGROUND_H + 20
#define SN_PLIST_PGLIST_W            SN_PLIST_PGLIST_CONT_W
#define SN_PLIST_PGLIST_H            SN_PLIST_PGLIST_CONT_H - SN_PLIST_NAME_BACKGROUND_H - 20

#define SN_PROG_LIST_MIDL                 0
#define SN_PROG_LIST_MIDT                 0
#define SN_PROG_LIST_MIDW                 SN_PLIST_PGLIST_W - 2*SN_PROG_LIST_MIDL
#define SN_PROG_LIST_MIDH                 SN_PLIST_PGLIST_H - 2*SN_PROG_LIST_MIDT

#define SN_PROG_LIST_VGAP                 0
#define SN_PLIST_LIST_PAGE                8


//prog info cont
#define SN_PLIST_PGINFO_CONT_X            SN_PROG_PREV_X
#define SN_PLIST_PGINFO_CONT_Y            SN_PROG_PREV_Y + SN_PROG_PREV_H + 10
#define SN_PLIST_PGINFO_CONT_W            SN_PROG_PREV_W
#define SN_PLIST_PGINFO_CONT_H            140//150

//prog info: current program info
#define SN_PLIST_PGINFO_CURRENT_PROGRAM_X     0
#define SN_PLIST_PGINFO_CURRENT_PROGRAM_Y     0
#define SN_PLIST_PGINFO_CURRENT_PROGRAM_W     100
#define SN_PLIST_PGINFO_CURRENT_PROGRAM_H     35

//prog info: follow program info
#define SN_PLIST_PGINFO_FOLLOW_PROGRAM_X      SN_PLIST_PGINFO_CURRENT_PROGRAM_X
#define SN_PLIST_PGINFO_FOLLOW_PROGRAM_Y      SN_PLIST_PGINFO_CURRENT_PROGRAM_H + 30//5
#define SN_PLIST_PGINFO_FOLLOW_PROGRAM_W      SN_PLIST_PGINFO_CURRENT_PROGRAM_W
#define SN_PLIST_PGINFO_FOLLOW_PROGRAM_H      35//70

//bar little
#define SN_PLIST_BAR_LITTLE_X                     SN_PLIST_PGLIST_CONT_X
#define SN_PLIST_BAR_LITTLE_Y                     SN_PLIST_PGLIST_CONT_H + SN_PROG_PREV_Y
#define SN_PLIST_BAR_LITTLE_W                     SN_PLIST_PGLIST_CONT_W
#define SN_PLIST_BAR_LITTLE_H                     31

//group name arrow
#define SN_PLIST_BAR_ARROW_LEFT_X        SN_PLIST_PGLIST_CONT_W/2 - 10
#define SN_PLIST_BAR_ARROW_RIGHT_X       SN_PLIST_PGLIST_CONT_W/2 + 10
#define SN_PLIST_BAR_ARROW_Y             (SN_PLIST_BAR_LITTLE_H - SN_PLIST_BAR_ARROW_H)/2
#define SN_PLIST_BAR_ARROW_W             10
#define SN_PLIST_BAR_ARROW_H             18

//bar vertical
#define SN_BAR_VERTICAL_X                SN_PROG_PREV_X + SN_PROG_PREV_W + 10
#define SN_BAR_VERTICAL_Y                SN_PROG_PREV_Y + 30
#define SN_BAR_VERTICAL_W                4
#define SN_BAR_VERTICAL_H                SN_PROG_PREV_Y + SN_PLIST_PGLIST_CONT_H - 30

//bar cross
#define SN_BAR_CROSS_X                25
#define SN_BAR_CROSS_Y                SN_PLIST_NAME_BACKGROUND_H + 10
#define SN_BAR_CROSS_W                SN_PLIST_PGLIST_CONT_W -58
#define SN_BAR_CROSS_H                5

//epg info
#define SN_PLIST_EPG_INFO_X           0//SN_PLIST_PGINFO_CURRENT_PROGRAM_W
#define SN_PLIST_EPG_INFO_Y           35//0
#define SN_PLIST_EPG_INFO_W           SN_PLIST_PGINFO_CONT_W //- SN_PLIST_PGINFO_CURRENT_PROGRAM_W
#define SN_PLIST_EPG_INFO_H           SN_PLIST_PGINFO_CURRENT_PROGRAM_H
//lvcm end

//PLIST prog list cont
#define PLIST_PGLIST_CONT_X         (PLIST_PGINFO_CONT_X + PLIST_PGINFO_CONT_W + 10)
#define PLIST_PGLIST_CONT_Y         50
#define PLIST_PGLIST_CONT_W        240
#define PLIST_PGLIST_CONT_H         330

//list group arrow
#define PLIST_ARROW_LEFT_X         (PLIST_GROUP_NAME_X - 20)
#define PLIST_ARROW_RIGHT_X      ( PLIST_GROUP_NAME_X + PLIST_GROUP_NAME_W + 20)
#define PLIST_ARROW_Y         PLIST_GROUP_NAME_Y
#define PLIST_ARROW_W        15
#define PLIST_ARROW_H         PLIST_GROUP_NAME_H

//PLIST prog list
#define PLIST_PGLIST_X         0
#define PLIST_PGLIST_Y         PLIST_GROUP_NAME_H
#define PLIST_PGLIST_W        (PLIST_PGLIST_CONT_W -PROG_LIST_BAR_W-4)
#define PLIST_PGLIST_H         (PLIST_PGLIST_CONT_H - PLIST_PGLIST_Y)

//plist rect
#define PROG_LIST_MIDL         5
#define PROG_LIST_MIDT         5
#define PROG_LIST_MIDW        (PLIST_PGLIST_W -PROG_LIST_MIDL*2)
#define PROG_LIST_MIDH         (PLIST_PGLIST_H -PROG_LIST_MIDT*2)
#define PROG_LIST_VGAP         1

//pglist bar
#define PROG_LIST_BAR_X        (PLIST_PGLIST_X + PLIST_PGLIST_W)
#define PROG_LIST_BAR_Y         (PLIST_PGLIST_Y + PROG_LIST_MIDT)
#define PROG_LIST_BAR_W         COMM_LIST_BAR_W
#define PROG_LIST_BAR_H        PROG_LIST_MIDH

//PLIST advertise window
#define PLIST_AD_X        PLIST_PGLIST_CONT_X
#define PLIST_AD_Y        (PLIST_PGLIST_CONT_Y + PLIST_PGLIST_CONT_H + 10)
#define PLIST_AD_W        PLIST_PGLIST_CONT_W
#define PLIST_AD_H        110

 //prog info cont: epg info
#define PLIST_EPG_INFO_CNT     2
#define PLIST_EPG_INFO_X         10
#define PLIST_EPG_INFO_Y         42
#define PLIST_EPG_INFO_W        (PLIST_PGINFO_CONT_W - PLIST_EPG_INFO_X * 2)
#define PLIST_EPG_INFO_H         30

/*others*/
#define PLIST_LIST_PAGE         8
#define PLIST_LIST_FIELD        3


#define GROUP_COUNT 3 //TV RADIO ALL

RET_CODE open_prog_list(u32 para1, u32 para2);

void plist_set_modify_state(BOOL state);

#endif

