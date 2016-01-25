/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

rsc_rstyle_t c_tab[MAX_RSTYLE_CNT] =
{
  // RSI_IGNORE
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_TRANSPARENT
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_TRANS},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_BLUE
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLUE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_YELLOW
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_YELLOW},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_WHITE
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_WHITE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_BLACK
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLACK},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_DLG_FRM
  {
    {R_LINE2, C_WHITE},
    {R_LINE2, C_WHITE},
    {R_LINE2, C_WHITE},
    {R_LINE2, C_WHITE},
    {R_FILL_RECT_TO_BORDER, C_BLUE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },
};

font_map_t f_map_tab[] =
{
  { FONT_ID_1, FONT_ENGLISH,  FONT_BUFFER_COMMON, 0, NULL },
  { FONT_ID_1, FONT_CHINESE,  FONT_BUFFER_LOCAL,  0, NULL },  
//  { FONT_ID_2, FONT_NUMBER, FONT_BUFFER_LOCAL, 0, NULL },
};

#define FONTMAP_CNT    sizeof(f_map_tab) / sizeof(font_map_t)

rsc_fstyle_t f_tab[MAX_FSTYLE_CNT] =
{
  //FSI_BLACK
  { FONT_ID_1, 0, C_BLACK     },

  //FSI_WHITE
  { FONT_ID_1, 0, C_WHITE     },
};

rsc_config_t g_rsc_config =
{
  512,         // common data buf size
  0,//80 * 336,    //max bitmap size,every pixel cost 2 Byte
  0,           //max language size,every char cast 2 Bytes
  4 * 256,     //palette size,every color cost 4 Bytes
  0,           //must bigger than latin font size
  0,           //must bigger than GB2312 font size
  0,           // script buffer size
  24 * 24 * 2,     //cache buffer size,every pixel cost 2 Byte
  10,          //cache buffer length
  0,           //offset for block
  0,					 //flash base addr
  RSTYLE_CNT,
  c_tab,
  FSTYLE_CNT,
  f_tab,
  FONTMAP_CNT,
  f_map_tab,
};

