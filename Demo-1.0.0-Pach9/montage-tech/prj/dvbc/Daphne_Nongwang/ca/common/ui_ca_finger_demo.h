/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_FINGER_H__
#define __UI_CA_FINGER_H__

/* coordinate */

/* rect style */
//#define RSI_CA_FINGER_TXT     RSI_SUB_BUTTON_SH

/* font style */
#define FSI_CA_FINGER_TXT     FSI_COMM_TXT_N
#define FINGER_AUTOCLOSE_MS 180000
void open_ca_finger_menu(finger_msg_t *p_ca_finger);
void close_ca_finger_menu(void);

#endif


