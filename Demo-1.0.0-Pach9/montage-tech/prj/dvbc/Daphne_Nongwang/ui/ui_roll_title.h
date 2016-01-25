/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2012).
*              All rights reserved.
* Owner:       Lei Chen <lei.chen@montage-tech.com>
****************************************************************************/


#ifndef __UI_ROLL_TITLE_H__
#define __UI_ROLL_TITLE_H__

//roll title
#define ROLL_TITLE_X              (0)
#define ROLL_TITLE_Y               20
#define ROLL_TITLE_W              SCREEN_WIDTH
#define ROLL_TITLE_H             40

#define ROLL_TITLE_RGN_W              720
#define ROLL_TITLE_RGN_H              576

#define ROLL_TITLE_ERASE_H            100

#define LENGTH_OF_ROLL_BMP          (ROLL_TITLE_RGN_W*ROLL_TITLE_RGN_H)


enum background_menu_ctrl_id
{
  IDC_ROLL_TITLE = 1,
};

RET_CODE draw_picture(u32 x,u32 y,u32 width,u32 height,
                                   u8 *source,u8 *source1,u32 Bmpwidth,u8 Scoll_time);
RET_CODE subt_is_scroll(BOOL *flag);
RET_CODE osd_display_is_ok(BOOL *flag);
RET_CODE scroll_step_pixel_set(u8 *p_pixel_num);
RET_CODE channel_info_set(u16 *network_id, u16 *ts_id, u16 *service_id);
RET_CODE draw_rectangle(u32 x, u32 y, u32 width, u32 height, u32 color);
RET_CODE draw_roll_title_to_trans();


#endif
