/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PROMPT_H__
#define __UI_CA_PROMPT_H__
#define CA_PROMPT_X             (SCREEN_WIDTH-640)/2
#define CA_PROMPT_Y             (SCREEN_HEIGHT-480)/2
#define CA_PROMPT_W             640
#define CA_PROMPT_H             480

#define CA_PROMPT_CHANNEL_TYPE_X              100
#define CA_PROMPT_CHANNEL_TYPE_Y              90
#define CA_PROMPT_CHANNEL_TYPE_W              120
#define CA_PROMPT_CHANNEL_TYPE_H              30

#define CA_PROMPT_COLUMN0_W                     130
#define CA_PROMPT_COLUMN1_W                     120
#define CA_PROMPT_COLUMN2_W                     130
#define CA_PROMPT_COLUMN3_W                     120

#define CA_PROMPT_COLUMN_HGAP                   6
#define CA_PROMPT_ROW_VGAP                      10


RET_CODE open_ca_prompt(u32 para1, u32 para2);

#endif

