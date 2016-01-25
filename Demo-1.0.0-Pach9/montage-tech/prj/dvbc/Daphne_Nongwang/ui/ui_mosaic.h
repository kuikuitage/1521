/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_MOSAIC_H__
#define __UI_MOSAIC_H__

//info frame
#define MOSAIC_INFO_FRM_X            0 
#define MOSAIC_INFO_FRM_Y            0
#define MOSAIC_INFO_FRM_W            SCREEN_WIDTH 
#define MOSAIC_INFO_FRM_H            40 

//name
#define MOSAIC_NAME_X   30
#define MOSAIC_NAME_Y   0
#define MOSAIC_NAME_W   250
#define MOSAIC_NAME_H   40

//help
#define MOSAIC_HELP_X         400
#define MOSAIC_HELP_Y         0
#define MOSAIC_HELP_W         200
#define MOSAIC_HELP_H         40

//no_mosaic_tip
#define MOSAIC_EMPTY_TIP_X         200
#define MOSAIC_EMPTY_TIP_Y         200
#define MOSAIC_EMPTY_TIP_W         (SCREEN_WIDTH-2*MOSAIC_EMPTY_TIP_X)
#define MOSAIC_EMPTY_TIP_H         40

//video frame
#define MOSAIC_VIDEO_FRM_X            0 
#define MOSAIC_VIDEO_FRM_Y            50
#define MOSAIC_VIDEO_FRM_W            SCREEN_WIDTH 
#define MOSAIC_VIDEO_FRM_H            460

//video mbox
#define MOSAIC_VIDEO_MBOX_X            0 
#define MOSAIC_VIDEO_MBOX_Y            0
#define MOSAIC_VIDEO_MBOX_W            MOSAIC_VIDEO_FRM_W 
#define MOSAIC_VIDEO_MBOX_H            MOSAIC_VIDEO_FRM_H


RET_CODE open_mosaic(u32 para1, u32 para2);

#endif

