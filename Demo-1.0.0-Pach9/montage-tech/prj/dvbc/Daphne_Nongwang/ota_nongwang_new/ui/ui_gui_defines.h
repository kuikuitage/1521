/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_GUI_DEFINES_H__
#define __UI_GUI_DEFINES_H__

#ifdef WIN32
#define C_TRANS           0x2
#define C_KEY             0xF

#define C_BLUE            0x8

#define C_YELLOW          0xD

#define C_BLACK           0x0
#define C_WHITE           0x1
#else

#define C_TRANS           0xFF
#define C_KEY             0x2

#define C_BLUE            0xD3

#define C_YELLOW          0xA6

#define C_BLACK           0x0
#define C_WHITE           0xFC
#endif

#if 0
enum rect_style_id
{
  RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn
  RSI_TRANSPARENT,
  RSI_MAINWND_BG,
  RSI_SUBMENU_BG,
  RSI_HELP_BG,

  
  RSI_ITEM_3_SH,
  RSI_ITEM_5_HL,
  RSI_SELECT_ITEM_3,
  RSI_ITEM_6_SH,
  RSI_WINDOW_1,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
  RSI_ITEM_7_SH,
  RSI_ITEM_4_SEL,
  



  RSI_PREVIEW_TV_WIN,
  RSI_ADVERTISEMENT_WIN,
  RSI_COMMON_BTN_HL,
  RSI_COMMON_BTN_SH,
  RSI_COMMON_BTN_ORANGE,
  RSI_WHITE_YELLOW,
  RSI_WHITE_GREEN,
  RSI_TITLE_BG,
  RSI_BUTTON_GREY,
  RSI_BUTTON_MID,
  RSI_BUTTON_GREY_WITH_ARROW,
  RSI_TIP_BOX,
  RSI_GROUP_CLASSIFY,
  RSI_PURPLE,
  RSI_PROGRESS_BAR_BG,
  RSI_PROGRESS_BAR_MID_BLUE,
  RSI_PROGRESS_BAR_MID_GREEN,
  RSI_PROGRESS_BAR_MID_GRAY,
  RSI_PROGRESS_BAR_MID_YELLOW,
  RSI_NEW_PSW,
  RSI_OLD_PSW,
  RSI_ORANGE,
  RSI_BLUE_GREEN,
  RSI_BLUE_GREEN1,
  RSI_GREEN,
  RSI_SCROLL_BAR_BG,
  RSI_SCROLL_BAR_MID,
  RSI_RECT_WHITE_GREEN,
  RSI_BUTTON_YELLOW,
  RSI_SELECT_CTRL,
  RSI_DLG_TEXT,
  RSI_DLG_CONTENT,
  RSI_DLG_BTN_SH,
  RSI_DLG_BTN_HL,  
  RSI_PSW_EDIT,
  RSI_INFOBOX,
  RSI_VOLUME,
  RSI_VOLUME_PROGRESS_BAR_BG,
  RSI_VOLUME_PROGRESS_BAR_MID,
  
  RSI_GAME_BG1,
  RSI_GAME_BG2,
  RSI_GAME_INFO1,
  RSI_GAME_INFO2,
  RSI_CHESS_ITEM1,

  RSI_MOSAIC_YELLOW,
  RSI_SUB_BUTTON_SH,
  
  RSTYLE_CNT,
};
#else
enum rect_style_id
{
  RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn
  RSI_TRANSPARENT,
  RSI_BLUE,
  RSI_YELLOW,
  RSI_WHITE,
  RSI_BLACK,
  RSI_DLG_FRM,

  RSTYLE_CNT,
};

#endif

enum font_style_id
{
  FSI_BLACK = 0,        //black string
  FSI_WHITE,            //white string
  FSTYLE_CNT,
};

enum font_lib_id
{
  FONT_ID_1 = 1,
  FONT_ID_2
};

#define MAX_RSTYLE_CNT        50
#define MAX_FSTYLE_CNT        10

extern rsc_config_t g_rsc_config;

#endif

