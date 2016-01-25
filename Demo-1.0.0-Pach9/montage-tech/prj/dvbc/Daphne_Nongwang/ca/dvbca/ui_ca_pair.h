/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PAIR_H__
#define __UI_CA_PAIR_H__

#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT	3
#define SN_CONDITIONAL_ACCEPT_PAIR_HELP_RSC_CNT	15
//CA frame
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_X       106
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_Y       24
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_W       505
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_H       360

//work duration items
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_X        10
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_Y        20
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_LW       260
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_RW       140
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_H        35

#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_X        140
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_Y        160
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_W        70
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_H        25

//change work time result
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_X        50
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_Y        220
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_W        260
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_H        90

RET_CODE open_stb_card_pair_info(u32 para1, u32 para2);

#endif


