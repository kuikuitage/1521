/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_BROWSER_H__
#define __UI_BROWSER_H__

#define BROWSER_L   ((SCREEN_WIDTH-BROWSER_W)/2)
#define BROWSER_T   ((SCREEN_HEIGHT-BROWSER_H)/2)
#define BROWSER_W   SCREEN_WIDTH
#define BROWSER_H   SCREEN_HEIGHT



/* rect style */
#define RSI_BROWSER_CONT     RSI_TRANSPARENT

RET_CODE open_browser(u32 para1, u32 para2);

RET_CODE close_browser(void);

#endif


