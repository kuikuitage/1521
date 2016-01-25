/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_FINGER_PRINT_H__
#define __UI_FINGER_PRINT_H__

#define FINGER_PRINT_L   100
#define FINGER_PRINT_T   100
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
//#define RSI_FINGER_PRINT_FRAME RSI_ITEM_1_SH
#define FSI_FINGER_PRINT_FONT FSI_COMM_TXT_N

RET_CODE open_finger_print (u32 para1, u32 para2);

RET_CODE close_finger_print(void);

#endif

