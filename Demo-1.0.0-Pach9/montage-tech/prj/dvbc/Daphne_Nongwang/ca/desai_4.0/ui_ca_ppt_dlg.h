/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_IPPV_PPT_DLG_H__
#define __UI_CA_IPPV_PPT_DLG_H__


#define CA_IPPV_COMM_DLG_X         ((SCREEN_WIDTH - CA_IPPV_COMM_DLG_W) / 2)
#define CA_IPPV_COMM_DLG_Y         ((SCREEN_HEIGHT - CA_IPPV_COMM_DLG_H) / 2 - 110)//((SCREEN_HEIGHT - CA_IPPV_COMM_DLG_H) / 2 - 100)
#define CA_IPPV_COMM_DLG_W         350
#define CA_IPPV_COMM_DLG_H         240+60

#define CA_IPPV_TEXT_TITLE_X       0
#define CA_IPPV_TEXT_TITLE_Y       0
#define CA_IPPV_TEXT_TITLE_W       CA_IPPV_COMM_DLG_W
#define CA_IPPV_TEXT_TITLE_H       40

#define CA_IPPV_INFO_ITEM_X        5
#define CA_IPPV_INFO_ITEM_Y        5
#define CA_IPPV_INFO_ITEM_LW0      100
#define CA_IPPV_INFO_ITEM_RW0      200


#define CA_IPPV_INFO_CONT_X        80
#define CA_IPPV_INFO_CONT_W        200
#define CA_IPPV_INFO_CONT_H        35

#define CA_IPPV_INFO_ITEM_CNT       9

#define CA_IPPV_BURSE_ITEM_GAP   10
#define CA_IPPV_BURSE_ITEM_LW0   60
#define CA_IPPV_BURSE_ITEM_RW0   100
#define CA_IPPV_BURSE_ITEM_X     (CA_IPPV_INFO_ITEM_X+CA_IPPV_BURSE_ITEM_LW0+CA_IPPV_BURSE_ITEM_RW0+CA_IPPV_BURSE_ITEM_GAP)
#define CA_IPPV_PWD_X         CA_IPPV_COMM_DLG_X + 50
#define CA_IPPV_PWD_Y         CA_IPPV_COMM_DLG_Y + 20
RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2);
#endif


