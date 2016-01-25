/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CHANNEL_NUMB_H__
#define __UI_CHANNEL_NUMB_H__

#define NUMB_EXCHAN_W              250
#define NUMB_EXCHAN_H              100

#define NUMB_TXT1_L         0
#define NUMB_TXT1_T         0
#define NUMB_TXT1_W         NUMB_EXCHAN_W
#define NUMB_TXT1_H         DLG_TITLE_H

#define NUMB_EDIT_L         50
#define NUMB_EDIT_T         53
#define NUMB_EDIT_W         (NUMB_EXCHAN_W - 2 * PWDLG_EDIT_L)
#define NUMB_EDIT_H         31

#define CHAN_MENU_W           SCREEN_WIDTH 
#define CHAN_MENU_H            SCREEN_HEIGHT

#define NUMB_WARN_X        100
#define NUMB_WARN_Y        260

typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
}channel_numb_data_t;

BOOL ui_channel_numb_open(channel_numb_data_t* p_data, u16 numb);

#endif


