/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_OTA_USER_INPUT_H__
#define __UI_OTA_USER_INPUT_H__


/*! coordinates*/
#define OTA_USER_INPUT_CONTX 0
#define OTA_USER_INPUT_CONTY 0
#define OTA_USER_INPUT_CONTW  SCREEN_WIDTH
#define OTA_USER_INPUT_CONTH SCREEN_HEIGHT

#define OTA_USER_INPUT_TTLX 144
#define OTA_USER_INPUT_TTLY 120
#define OTA_USER_INPUT_TTLW (OTA_USER_INPUT_CONTW - 2 * OTA_USER_INPUT_TTLX)
#define OTA_USER_INPUT_TTLH 92

#define OTA_USER_INPUT_TDI_X 0
#define OTA_USER_INPUT_TDI_Y 20
#define OTA_USER_INPUT_TDI_W (SCREEN_WIDTH - 20)
#define OTA_USER_INPUT_TDI_H 32

#define OTA_USER_INPUT_ITEMLX OTA_USER_INPUT_TTLX
#define OTA_USER_INPUT_ITEMRX (OTA_USER_INPUT_ITEMLX + OTA_USER_INPUT_ITEMLW)
#define OTA_USER_INPUT_ITEMY  (OTA_USER_INPUT_TTLY + OTA_USER_INPUT_TTLH + OTA_USER_INPUT_ITEM_VGAP)
#define OTA_USER_INPUT_ITEMW (OTA_USER_INPUT_CONTW - 2 * OTA_USER_INPUT_ITEMLX)
#define OTA_USER_INPUT_ITEMLW 200
#define OTA_USER_INPUT_ITEMRW (OTA_USER_INPUT_ITEMW - OTA_USER_INPUT_ITEMLW)
#define OTA_USER_INPUT_ITEMH  32

#define OTA_USER_INPUT_ITEM_VGAP 3
/*! r-style*/
#define RSI_OTA_USER_INPUT_MENU RSI_BLUE
#define RSI_OTA_USER_INPUT_STC_SH RSI_BLUE
#define RSI_OTA_USER_INPUT_STC_HL RSI_YELLOW
#define RSI_OTA_USER_INPUT_SEL_SH RSI_BLUE
#define RSI_OTA_USER_INPUT_SEL_HL RSI_YELLOW

/*! f-style*/
#define FSI_OTA_USER_INPUT_SH FSI_BLACK
#define FSI_OTA_USER_INPUT_HL FSI_WHITE

/*! others*/
#define OTA_USER_INPUT_ITEM_CNT 8

#define OTA_TPFREQ_MIN 0
#define OTA_TPFREQ_MAX 999
#define OTA_TPFREQ_BIT 3

#define OTA_LNBFREQ_MIN 0
#define OTA_LNBFREQ_MAX 999
#define OTA_LNBFREQ_BIT  4

#define OTA_SYM_MIN 0
#define OTA_SYM_MAX 9999
#define OTA_SYM_BIT 4

#define OTA_DPID_MIN 0
#define OTA_DPID_MAX 8191//0x1FFF
#define OTA_DPID_BIT 4

#define LNB_FREQ_TYPE_CNT 15

s32 open_ota_user_input(u32 para1, u32 para2);
#endif
