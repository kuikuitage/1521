/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_GUI_DEFINES_H__
#define __UI_GUI_DEFINES_H__
#include "Z_lvcm.h"
//#include "Z_zhaomeng.h"
#include "Z_whm.h"
#include "Z_XUXIN.h"
#include "z_lxd.h"


#ifndef WIN32
#define C_TRANS             0x0 //255,0,0
#define C_KEY               0xF81F //255,0,255
#define C_WHITE_YELLOW      0xF3DF //E1F6FF
#define C_WHITE_GREEN       0xF3DF //E1F6FF
#define C_BLACK             0x8000 //000000
#define C_WHITE             0xFFFF //FFFFFF
#define C_GREEN             0x8931 //154C8F
#define C_GRAY              0xC210 //838383
#define C_PURPLE            0xC21F //8383FF
#define C_ORANGE            0xFB00 //F6C500
#define C_GAME_GRAY         0xA9CF //52737B
#define C_GAME_ORANGE       0xECCB //D5AC5A
#define C_GAME_WHITE_YELLOW 0xF3BA //E6EED5
#define C_YELLOW            0xFFE0 //FFFF00
//#define C_INDEX_BG       0xF3FB 
//#define C_TIP_BOX_BG       0xF7D9 
#else
#define C_TRANS             0xF81F //255,0,0
#define C_KEY               0xF81F //255,0,255
#define C_WHITE_YELLOW      0xE7DF //E1F6FF
#define C_WHITE_GREEN       0xE7DF //E1F6FF
#define C_BLACK             0x0000 //000000
#define C_WHITE             0xFFFF //FFFFFF
#define C_GREEN             0x1271 //154C8F
#define C_GRAY              0x8410 //838383
#define C_PURPLE            0x841F //8383FF
#define C_ORANGE            0xEE20 //F6C500
#define C_GAME_GRAY         0x538F //52737B
#define C_GAME_ORANGE       0xD56B //D5AC5A
#define C_GAME_WHITE_YELLOW 0xE77A //E6EED5
#define C_YELLOW            0xFFF0 //FFFF00
//#define C_INDEX_BG       0xE7FB 
//#define C_TIP_BOX_BG       0xEF99 
#endif

#define C_BLUE            0x11
#define C_BLUE_LIGHT      0x3D
#define C_BLUE_LIGHTER    0x23
#define C_BLUE_TRANS      0xB1
#define C_RED             0x31

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
#define C_FINGER_BG       0x0
#define C_FINGER_FRONT    0x0
#endif

enum rect_style_id
{
	RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn

	//ZHAOMENG_RSI_DEFINE,
	LVCM_RSI_DEFINE,
	WHM_RSI_DEFINE,
	RSI_XUXIN_DEFINE,
	LXD_RSI_DEFINE,

	SN_RSI_POP_MODAL,
	SN_RSI_BNT_TEXT_HL,
	SN_RSI_BNT_TEXT_SH,
	
	RSI_TRANSPARENT,
//	RSI_MAINWND_BG,
//	RSI_COMM_ROOT_BG,   /*the frm except title and help*/  
//	RSI_SUBMENU_BG,
//	RSI_HELP_BG1,
//	RSI_HELP_BG2,
	RSI_ITEM_3_SH,
	RSI_WINDOW_1,
	RSI_WINDOW_2,
//	RSI_ITEM_1_SH,
	RSI_ITEM_7_SH,
	RSI_ITEM_4_SEL,
//	RSI_PREVIEW_TV_WIN,
//	RSI_ADVERTISEMENT_WIN,
//	RSI_COMMON_BTN_HL,
//	RSI_COMMON_BTN_ORANGE,
	RSI_WHITE_YELLOW,
//	RSI_WHITE_GREEN,
//	RSI_TITLE_BG,
//	RSI_BUTTON_GREY,
	//RSI_BUTTON_MID,
//	RSI_TIP_BOX,
//	RSI_TIP_BOX_BG,
//	RSI_PURPLE,
	RSI_ORANGE,
	RSI_GREEN,
	RSI_PROGRESS_BAR_BG,
	RSI_NEW_PSW,
	RSI_OLD_PSW,
//	RSI_BLUE_GREEN,
//	RSI_BLUE_GREEN1,
//	RSI_SCROLL_BAR_BG,
//	RSI_SCROLL_BAR_MID,
//	RSI_RECT_WHITE_GREEN,
//	RSI_BUTTON_YELLOW,
//	RSI_SELECT_BTN_SH,
//	RSI_SELECT_TITLE,
//	RSI_DLG_BTN_SH,
//	RSI_DLG_BTN_HL,  
//	RSI_VOLUME,
//	RSI_COMM_PBAR_BG,
	RSI_COMM_PBAR_MID,
	RSI_GAME_BG1,
	RSI_GAME_BG2,
	RSI_GAME_INFO1,
//	RSI_GAME_INFO2,
	RSI_CHESS_ITEM1,
	RSI_MOSAIC_YELLOW,
//	RSI_SUB_BUTTON_SH,
//	RSI_MAIN_CHLIST_SH,
//	RSI_MAIN_CHLIST_HL,
//	RSI_MAIN_EPG_SH,
//	RSI_MAIN_EPG_HL,
//	RSI_MAIN_CLISSFY_SH,
//	RSI_MAIN_CLISSFY_HL,
//	RSI_MAIN_FAV_SH,
//	RSI_MAIN_FAV_HL,
//	RSI_MAIN_MOSAIC_SH,
//	RSI_MAIN_MOSAIC_HL,
//	RSI_MAIN_CHEDIT_SH,
//	RSI_MAIN_CHEDIT_HL,
//	RSI_MAIN_NVOD_SH,
//	RSI_MAIN_NVOD_HL,
//	RSI_MAIN_NEWS_SH,
//	RSI_MAIN_NEWS_HL,
//	RSI_MAIN_MSG_SH,
//	RSI_MAIN_MSG_HL,
//	RSI_MAIN_GAME_SH,
//	RSI_MAIN_GAME_HL,
//	RSI_MAIN_STOCK_SH,
//	RSI_MAIN_STOCK_HL,
//	RSI_MAIN_SYSSET_SH,
//	RSI_MAIN_SYSSET_HL,  
	RSI_WHITE,
//	RSI_SECOND_TITLE_MID,
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
	RSI_FINGER_BG,
#endif
	RSTYLE_CNT,
};

enum font_style_id
{
  FSI_INDEX1 = 0,   //the color value read form color.bin
  FSI_INDEX2,
  FSI_INDEX3,
  FSI_INDEX4,        
  FSI_GRAY,             //gray string
  FSI_WHITE,           //white string
  FSI_NUMBER,
  FSI_RESERVE2,
  FSI_RESERVE3,
  FSI_RESERVE4,
  FSI_NUMBER_BIG,
  #if(CONFIG_CAS_ID != CONFIG_CAS_ID_SV)
  FSI_CHINESE_BIG,
  #else
  FSI_FINGER1,
  FSI_FINGER2,
  FSI_FINGER3,
  FSI_FINGER4,
  FSI_FINGER5,
  FSI_FINGER6,
  FSI_FINGER7,
  FSI_FINGER8,
  FSI_FINGER9,
  FSI_FINGER10,
  FSI_FINGER11,
  FSI_FINGER12,
  FSI_CHINESE_BIG,
  #endif
  FSTYLE_CNT,
  
};

/* font style define for menu */
#define FSI_MAIN_MENU_BTN   FSI_INDEX1

#define FSI_HELP_TEXT            FSI_INDEX2
#define FSI_TOP_TITLE_TXT     FSI_CHINESE_BIG

#define FSI_SECOND_TITLE      FSI_INDEX3
#define FSI_INFOBOX_TITLE    FSI_INDEX3
#define FSI_COMM_BTN           FSI_INDEX3

#define FSI_COMM_TXT_N         FSI_INDEX4
#define FSI_COMM_TXT_HL        FSI_INDEX4
#define FSI_COMM_TXT_GRAY   FSI_GRAY

//lvcm
#define FSI_LIST_TXT_G          FSI_INDEX3
#define FSI_LIST_TXT_N          FSI_INDEX3
#define FSI_LIST_TXT_HL         FSI_INDEX4
#define FSI_LIST_TXT_SEL        FSI_INDEX3
#define FSI_LIST_BTN_HL         FSI_INDEX4

//lvcm end


enum font_lib_id
{
  FONT_ID_1 = 1,
  FONT_ID_2,
  FONT_ID_3,
  FONT_ID_4,
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  FONT_ID_FG1,
  FONT_ID_FG2,
  FONT_ID_FG3,
  FONT_ID_FG4,
  FONT_ID_FG5,
  FONT_ID_FG6,
  FONT_ID_FG7,
  FONT_ID_FG8,
  FONT_ID_FG9,
  FONT_ID_FG10,
  FONT_ID_FG11,
  FONT_ID_FG12,
  #endif
};

#define MAX_RSTYLE_CNT        200
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_SV)
#define MAX_FSTYLE_CNT        12
#else
#define MAX_FSTYLE_CNT        24
#endif
extern rsc_config_t g_rsc_config;
extern rsc_fstyle_t f_tab[MAX_FSTYLE_CNT];

#endif

