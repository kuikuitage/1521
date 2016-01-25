/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*                  Lei Chen <lei.chen@montage-tech.com>
****************************************************************************/
#ifndef __UI_AUTO_FEED_H__
#define __UI_AUTO_FEED_H__

/* coordinate */
#define AUTO_FEED_CONT_X          ((SCREEN_WIDTH - AUTO_FEED_CONT_W) / 2)

#define AUTO_FEED_CONT_Y        (350-45-100)
#define AUTO_FEED_CONT_W        (150+200)
#define AUTO_FEED_CONT_H        (50+50)

/* rect style */
#define RSI_AUTO_FEED_TXT    RSI_GREEN

/* font style */
#define FSI_AUTO_FEED_TXT    FSI_WHITE

/* others */
#ifdef CUS_TONGGUANG_DF
  #define AUTO_FEED_AUTOCLOSE_MS   4000
#else
#define AUTO_FEED_AUTOCLOSE_MS   3000
#endif

RET_CODE open_auto_feed(u32 para1, u32 para2);

#endif



