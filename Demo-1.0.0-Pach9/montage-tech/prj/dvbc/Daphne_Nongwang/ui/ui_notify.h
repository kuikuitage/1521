/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_NOTIFY_H__
#define __UI_NOTIFY_H__

/* coordinate */
#define NOTIFY_CONT_X     425
#define NOTIFY_CONT_Y     44
#define NOTIFY_CONT_W     180
#define NOTIFY_CONT_H     30

/* rect style */
#define RSI_NOTIFY_TXT    RSI_GREEN

/* font style */
#define FSI_NOTIFY_TXT    FSI_WHITE

#define NOTIFY_AUTOCLOSE_MS 3000

enum notify_type
{
  NOTIFY_TYPE_STRID = 0,
  NOTIFY_TYPE_ASC,
  NOTIFY_TYPE_UNI,
};

void ui_set_notify(rect_t *p_rect, u32 type, u32 content);

BOOL ui_is_notify(void);

RET_CODE open_notify(u32 para1, u32 para2);

void close_notify(void);


#endif


