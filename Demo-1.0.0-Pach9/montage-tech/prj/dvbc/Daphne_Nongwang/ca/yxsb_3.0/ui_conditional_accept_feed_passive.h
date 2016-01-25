/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_FEED_PASSIVE_H__
#define __UI_CONDITIONAL_ACCEPT_FEED_PASSIVE_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W          (SCREEN_WIDTH * 2/3)
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_H          (SCREEN_HEIGHT * 1/3)

//CA frame
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_X  0
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_Y  0
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_W  CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W
#define CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_H  35

//CA feed tip
#define CONDITIONAL_ACCEPT_FEED_TIP_X      ((CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W - CONDITIONAL_ACCEPT_FEED_TIP_W) / 2)
#define CONDITIONAL_ACCEPT_FEED_TIP_Y       ((CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_H - HELP_ITEM_H - CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_H - CONDITIONAL_ACCEPT_FEED_TIP_H) / 2 + CONDITIONAL_ACCEPT_FEED_PASSIVE_CA_FRM_H)
#define CONDITIONAL_ACCEPT_FEED_TIP_W      (CONDITIONAL_ACCEPT_FEED_PASSIVE_CONT_W - 80)
#define CONDITIONAL_ACCEPT_FEED_TIP_H       80


/* rect style */

/* font style */

/* others */

RET_CODE open_conditional_accept_feed_passive(u32 para1, u32 para2);

#endif


