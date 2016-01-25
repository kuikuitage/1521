/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_SELECT_H__
#define __UI_SELECT_H__

typedef struct help_item_fc_t
{
  u16 help_item_fc1;
  u16 help_item_fc2;
  u16 help_item_fc3;
  u16 help_item_fc4;
}help_item_fc;

/*
  set item bmp on help meue from F1~F3 key or color key;default color;
  */
void ui_set_help_item_img_f(void);
void ui_get_help_item_img_fc1(help_item_fc * p_item_fc);
BOOL ui_get_disable_display_time_status(void);
void ui_set_disable_display_time_status(void);
void ui_set_input_number_cont(u8 cont);
u8 ui_get_input_number_cont(void);
void ui_set_input_number_on_right(void);
BOOL ui_get_input_number_on_right(void);
void ui_set_num_big_font(void);
BOOL ui_get_num_big_font(void);
#endif


