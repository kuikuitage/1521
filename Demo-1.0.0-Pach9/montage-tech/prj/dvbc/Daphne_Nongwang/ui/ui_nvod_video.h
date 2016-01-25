/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_NVOD_VIDEO_H__
#define __UI_NVOD_VIDEO_H__

#define NVOD_VIDEO_CONT_X            0 
#define NVOD_VIDEO_CONT_Y            0
#define NVOD_VIDEO_CONT_W            SCREEN_WIDTH 
#define NVOD_VIDEO_CONT_H            SCREEN_HEIGHT 
//time
#define NVOD_VIDEO_TIME_X   30
#define NVOD_VIDEO_TIME_Y   10
#define NVOD_VIDEO_TIME_W   100
#define NVOD_VIDEO_TIME_H   40

#define NVOD_VIDEO_X         140
#define NVOD_VIDEO_Y         50
#define NVOD_VIDEO_W         410
#define NVOD_VIDEO_H         180

#define NVOD_TITLE_X         0
#define NVOD_TITLE_Y         0
#define NVOD_TITLE_W         NVOD_VIDEO_W
#define NVOD_TITLE_H         40

#define NVOD_TITLE_STR_X         20
#define NVOD_TITLE_STR_Y         0
#define NVOD_TITLE_STR_W         200
#define NVOD_TITLE_STR_H         40

#define NVOD_TITLE_NAME_X         20
#define NVOD_TITLE_NAME_Y         (NVOD_TITLE_H+3)
#define NVOD_TITLE_NAME_W         120
#define NVOD_TITLE_NAME_H         30

#define NVOD_TITLE_SCREEN_X         (NVOD_TITLE_NAME_X+NVOD_TITLE_NAME_W+5)
#define NVOD_TITLE_SCREEN_Y         NVOD_TITLE_NAME_Y
#define NVOD_TITLE_SCREEN_W         100
#define NVOD_TITLE_SCREEN_H         30


RET_CODE open_nvod_video(u32 para1, u32 para2);

#endif

