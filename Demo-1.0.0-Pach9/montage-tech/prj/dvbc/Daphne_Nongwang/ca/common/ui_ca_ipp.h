/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_IPP_H__
#define __UI_CA_IPP_H__

#define CA_IPP_X             (CA_IPP_W-CA_IPP_W)/2
#define CA_IPP_Y             (CA_IPP_H-CA_IPP_H)/2
#define CA_IPP_W             SCREEN_WIDTH
#define CA_IPP_H             SCREEN_HEIGHT

#define CA_IPP_CHANNEL_TYPE_X              100
#define CA_IPP_CHANNEL_TYPE_Y              90
#define CA_IPP_CHANNEL_TYPE_W              120
#define CA_IPP_CHANNEL_TYPE_H              30

#define CA_IPP_COLUMN0_W                     130
#define CA_IPP_COLUMN1_W                     120
#define CA_IPP_COLUMN2_W                     130
#define CA_IPP_COLUMN3_W                     120

#define CA_IPP_COLUMN_HGAP                   6
#define CA_IPP_ROW_VGAP                      10

RET_CODE open_ca_ipp(u32 para1, u32 para2);

#endif

