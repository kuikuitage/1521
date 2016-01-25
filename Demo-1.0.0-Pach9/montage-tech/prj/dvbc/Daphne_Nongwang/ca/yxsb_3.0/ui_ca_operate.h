/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_OPERATE_H__
#define __UI_CA_OPERATE_H__

/*cont*/
#define CA_OPE_CONT_X          ((SCREEN_WIDTH - CA_OPE_CONT_W) / 2)
#define CA_OPE_CONT_Y          ((SCREEN_HEIGHT - CA_OPE_CONT_H) / 2)
#define CA_OPE_CONT_W          SCREEN_WIDTH
#define CA_OPE_CONT_H           SCREEN_HEIGHT

//plist
#define CA_OPE_LIST_X       RIGHT_BG_X
#define CA_OPE_LIST_Y       RIGHT_BG_Y
#define CA_OPE_LIST_W      RIGHT_BG_W
#define CA_OPE_LIST_H       RIGHT_BG_H

//plist rect
#define CA_LIST_MIDL         10
#define CA_LIST_MIDT         RIGHT_BG_ITEM_START_VGAP
#define CA_LIST_MIDW        (CA_OPE_LIST_W-CA_LIST_MIDL*2)
#define CA_LIST_MIDH        (CA_LIST_PAGE*(COMM_BTN_ITEM_H+CA_LIST_VGAP) - CA_LIST_VGAP)
#define CA_LIST_VGAP         8



#define CA_LIST_PAGE         6
#define CA_LIST_FIELD        2


/* rect style */
RET_CODE open_ca_operate(u32 para1, u32 para2);

#endif
