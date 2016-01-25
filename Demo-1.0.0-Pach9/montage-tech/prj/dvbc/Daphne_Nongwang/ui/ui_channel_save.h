/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CHANNEL_SAVE_H__
#define __UI_CHANNEL_SAVE_H__


typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
}channel_save_data_t;

BOOL ui_channel_save_open(channel_save_data_t* p_data);

#endif


