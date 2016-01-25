/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_FINGER_PRINT_H__
#define __UI_FINGER_PRINT_H__

#define FINGER_PRINT_L   100
#define FINGER_PRINT_T   60
#define FINGER_PRINT_W   200
#define FINGER_PRINT_H   30

//finger
#define FINGER_PRINT_TEXT_X  0
#define FINGER_PRINT_TEXT_Y  0
#define FINGER_PRINT_TEXT_W  200
#define FINGER_PRINT_TEXT_H  30

#define FINGER_PRINT_TIMEOUT 20000

/* rect style */
#define RSI_FINGER_PRINT_CONT     RSI_IGNORE
#define RSI_FINGER_PRINT_FRAME RSI_SUB_BUTTON_SH
#define FSI_FINGER_PRINT_FONT FSI_COMM_TXT_N

void open_ca_finger_menu(finger_msg_t *p_finger_info);

RET_CODE close_ca_finger_menu(void);

#endif

