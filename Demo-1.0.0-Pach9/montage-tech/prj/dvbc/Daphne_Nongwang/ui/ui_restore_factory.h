/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_RESTORE_FACTORY_H__
#define __UI_RESTORE_FACTORY_H__

/* coordinate */
#define RST_FAC_MENU_X          ((SCREEN_WIDTH - RST_FAC_MENU_W) / 2)
#define RST_FAC_MENU_Y          ((SCREEN_HEIGHT - RST_FAC_MENU_H) / 2)
#define RST_FAC_MENU_W          SCREEN_WIDTH
#define RST_FAC_MENU_H          SCREEN_HEIGHT

//small cont1
#define RST_FAC_CONT1_X  SECOND_TITLE_X
#define RST_FAC_CONT1_Y  RIGHT_BG_Y
#define RST_FAC_CONT1_W  SECOND_TITLE_W
#define RST_FAC_CONT1_H  180

//small cont1:brief1
#define RST_FAC_BRIEF1_X  (SECOND_TITLE_X + 4)
#define RST_FAC_BRIEF1_Y  (RST_FAC_CONT1_Y + 6)
#define RST_FAC_BRIEF1_W  (SECOND_TITLE_W - 4)
#define RST_FAC_BRIEF1_H  (RST_FAC_CONT1_H - 50)

//small cont1:brief2
#define RST_FAC_BRIEF2_X  (SECOND_TITLE_X + 4)
#define RST_FAC_BRIEF2_Y  (RST_FAC_BRIEF1_Y + RST_FAC_BRIEF1_H + 6)
#define RST_FAC_BRIEF2_W  (SECOND_TITLE_W - 4)
#define RST_FAC_BRIEF2_H  30


//small cont2
#define RST_FAC_CONT2_X  SECOND_TITLE_X
#define RST_FAC_CONT2_Y  (RST_FAC_CONT1_Y + RST_FAC_CONT1_H + 5)
#define RST_FAC_CONT2_W  SECOND_TITLE_W
#define RST_FAC_CONT2_H  (RIGHT_BG_H - RST_FAC_CONT1_H - 5)

//small cont2:brief1
#define DETAIL_OFFSET_X   4
#define DETAIL_OFFSET_Y   6
#define RST_FAC_DETAIL_X  (SECOND_TITLE_X + DETAIL_OFFSET_X)
#define RST_FAC_DETAIL_Y  (RST_FAC_CONT2_Y + DETAIL_OFFSET_Y)
#define RST_FAC_DETAIL_W  (SECOND_TITLE_W - DETAIL_OFFSET_X*2)
#define RST_FAC_DETAIL_H  (RST_FAC_CONT2_H - DETAIL_OFFSET_Y*2)


/* rect style */
#define RSI_RST_FAC_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_restore_factory(u32 para1, u32 para2);

#endif


