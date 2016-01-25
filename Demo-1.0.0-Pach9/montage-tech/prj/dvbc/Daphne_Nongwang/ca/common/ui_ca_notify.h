/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_NOTIFY_H__
#define __UI_CA_NOTIFY_H__

/* coordinate */
#define NOTIFY_CA_CONT_X     5
#define NOTIFY_CA_CONT_Y     5
#define NOTIFY_CA_CONT_W     500
#define NOTIFY_CA_CONT_H     40

/* rect style */
//#define RSI_CA_NOTIFY_TXT    RSI_SUB_BUTTON_SH

/* font style */
#define FSI_CA_NOTIFY_TXT    FSI_COMM_TXT_N

#define NOTIFY_CA_AUTOCLOSE_MS 60000

enum notify_ca_type
{
  NOTIFY_CA_TYPE_STRID = 0,
  NOTIFY_CA_TYPE_ASC,
  NOTIFY_CA_TYPE_UNI,
};

RET_CODE open_ca_notify(u32 para1, u32 para2);

void close_ca_notify(void);

BOOL ui_is_roll_notify(void);


#endif


