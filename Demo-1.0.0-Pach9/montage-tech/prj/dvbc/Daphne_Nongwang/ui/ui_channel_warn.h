/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CHANNEL_WARN_H__
#define __UI_CHANNEL_WARN_H__

#define NUMB_WARN_W              280
#define NUMB_WARN_H              110

#define NUMB_WARN_TXT1_L         0
#define NUMB_WARN_TXT1_T         0
#define NUMB_WARN_TXT1_W         NUMB_WARN_W
#define NUMB_WARN_TXT1_H         DLG_TITLE_H

#define NUMB_WARN_STR_L         30
#define NUMB_WARN_STR_T         (NUMB_WARN_TXT1_H+8)
#define NUMB_WARN_STR_W         200
#define NUMB_WARN_STR_H         60

//bmp
#define NUMB_WARN_BMP_L         3
#define NUMB_WARN_BMP_T         0
#define NUMB_WARN_BMP_W        50
#define NUMB_WARN_BMP_H        DLG_TITLE_H



typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
}channel_warn_data_t;

BOOL ui_channel_warn_open(channel_warn_data_t* p_data);

#endif


