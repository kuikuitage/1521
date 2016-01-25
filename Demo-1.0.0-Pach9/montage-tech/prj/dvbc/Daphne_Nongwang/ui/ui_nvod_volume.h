/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_NVOD_VOLUME_H__
#define __UI_NVOD_VOLUME_H__

#define VOLUME_L   ((SN_SCREEN_WIDTH-VOLUME_W)/2)
#define VOLUME_T   340
#define VOLUME_W   480
#define VOLUME_H   92

#define VOLUME_ICON_X 0
#define VOLUME_ICON_Y 0
#define VOLUME_ICON_W 60
#define VOLUME_ICON_H 60

#define VOLUME_BAR_CONT_X (VOLUME_ICON_W/2)
#define VOLUME_BAR_CONT_Y (VOLUME_H-VOLUME_BAR_CONT_H)
#define VOLUME_BAR_CONT_W (VOLUME_W - VOLUME_ICON_H/2)
#define VOLUME_BAR_CONT_H 92

#define VOLUME_BAR_X  70
#define VOLUME_BAR_Y  ((VOLUME_BAR_CONT_H-VOLUME_BAR_H)/2)
#define VOLUME_BAR_W  340
#define VOLUME_BAR_H  COMM_PBAR_H

//introduce detail
#define VOLUME_TEXT_X              (VOLUME_BAR_X+VOLUME_BAR_W+10)
#define VOLUME_TEXT_Y              30
#define VOLUME_TEXT_W              30
#define VOLUME_TEXT_H              30

#define VOLUME_MAX    30

/* rect style */
#define RSI_VOLUME_CONT     RSI_IGNORE

RET_CODE open_nvod_mosic_volume (u32 para1, u32 para2);

RET_CODE close_nvod_mosic_volume(void);

#endif


