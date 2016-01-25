/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef _UI_FAV_LIST_H_
#define _UI_FAV_LIST_H_
/*!
 \file ui_fav_list.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in SMALLor list menu.
   And interfaces for open the SMALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */
#include "ui_common.h"

#define FLIST_MENU_X            0 
#define FLIST_MENU_Y            0
#define FLIST_MENU_W            SCREEN_WIDTH 
#define FLIST_MENU_H            SCREEN_HEIGHT 

//FLIST prog list cont
#define FLIST_PGLIST_CONT_X         (FLIST_PGINFO_CONT_X + FLIST_PGINFO_CONT_W + 10)
#define FLIST_PGLIST_CONT_Y         50
#define FLIST_PGLIST_CONT_W        240
#define FLIST_PGLIST_CONT_H        330

//list title
#define FLIST_TITLE_X          0
#define FLIST_TITLE_Y          0
#define FLIST_TITLE_W         FLIST_PGLIST_CONT_W
#define FLIST_TITLE_H          40

//FLIST prog list
#define FLIST_PGLIST_X         0
#define FLIST_PGLIST_Y         FLIST_TITLE_H
#define FLIST_PGLIST_W        (FLIST_PGLIST_CONT_W -FLIST_PGLIST_BAR_W-4)
#define FLIST_PGLIST_H         (FLIST_PGLIST_CONT_H - FLIST_PGLIST_Y)

//FLIST prog list rect
#define FLIST_PGLIST_MIDL         5
#define FLIST_PGLIST_MIDT         5
#define FLIST_PGLIST_MIDW        (FLIST_PGLIST_W -FLIST_PGLIST_MIDL*2)
#define FLIST_PGLIST_MIDH         (FLIST_PGLIST_H -FLIST_PGLIST_MIDT*2 - 10)
#define FLIST_PGLIST_VGAP         2

//pglist bar
#define FLIST_PGLIST_BAR_X        (FLIST_PGLIST_X + FLIST_PGLIST_W)
#define FLIST_PGLIST_BAR_Y         (FLIST_PGLIST_Y + FLIST_PGLIST_MIDT)
#define FLIST_PGLIST_BAR_W       COMM_LIST_BAR_W
#define FLIST_PGLIST_BAR_H        FLIST_PGLIST_MIDH

//FLIST advertise window
#define FLIST_AD_X        FLIST_PGLIST_CONT_X
#define FLIST_AD_Y        (FLIST_PGLIST_CONT_Y + FLIST_PGLIST_CONT_H + 10)
#define FLIST_AD_W        FLIST_PGLIST_CONT_W
#define FLIST_AD_H        110

//FLIST prog info cont
#define FLIST_PGINFO_CONT_X       MENU_CONT_OX
#define FLIST_PGINFO_CONT_Y       FLIST_AD_Y
#define FLIST_PGINFO_CONT_W      390
#define FLIST_PGINFO_CONT_H       110

//FLIST prog info:prog number
#define FLIST_PGINFO_NUM_X       0
#define FLIST_PGINFO_NUM_Y       0

#define FLIST_PGINFO_NUM_W      60

#define FLIST_PGINFO_NUM_H       40

 //FLIST prog info: prog name
#define FLIST_PGINFO_NAME_X       (FLIST_PGINFO_NUM_X + FLIST_PGINFO_NUM_W +3)
#define FLIST_PGINFO_NAME_Y       FLIST_PGINFO_NUM_X
#define FLIST_PGINFO_NAME_W      145
#define FLIST_PGINFO_NAME_H       FLIST_PGINFO_NUM_H

//FLIST date & time
#define FLIST_PGINFO_TIME_X           ( FLIST_PGINFO_CONT_W - FLIST_PGINFO_TIME_W)
#define FLIST_PGINFO_TIME_Y            FLIST_PGINFO_NUM_X
#define FLIST_PGINFO_TIME_W           180
#define FLIST_PGINFO_TIME_H            FLIST_PGINFO_NUM_H

//FLIST prog info:epg info
#define FLIST_EPG_INFO_CNT     2
#define FLIST_EPG_INFO_X         10
#define FLIST_EPG_INFO_Y         42
#define FLIST_EPG_INFO_W        (FLIST_PGINFO_CONT_W - FLIST_EPG_INFO_X * 2)
#define FLIST_EPG_INFO_H         30

/*fstyle*/
#define FSI_FLIST_NAME          FSI_SECOND_TITLE
/*others*/
#define FLIST_LIST_PAGE             8
#define FLIST_LIST_FIELD            3

RET_CODE open_fav_list(u32 para1, u32 para2);

#endif
