/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_SYS_INFO_H__
#define __UI_SYS_INFO_H__

/* coordinate */
#define SYS_INFO_CONT_X          ((SCREEN_WIDTH - SYS_INFO_CONT_W) / 2)
#define SYS_INFO_CONT_Y          ((SCREEN_HEIGHT - SYS_INFO_CONT_H) / 2)
#define SYS_INFO_CONT_W          SCREEN_WIDTH
#define SYS_INFO_CONT_H          SCREEN_HEIGHT

#define SYS_INFO_CNT             14

//advertisement window
#define SYS_INFO_AD_X     SUB_AD_X
#define SYS_INFO_AD_Y     SUB_AD_Y
#define SYS_INFO_AD_W    SUB_AD_W
#define SYS_INFO_AD_H     SUB_AD_H

//preview window
#define SYS_INFO_PREV_X     SUB_PREV_X
#define SYS_INFO_PREV_Y     SUB_PREV_Y
#define SYS_INFO_PREV_W    SUB_PREV_W
#define SYS_INFO_PREV_H     SUB_PREV_H

//second title
#define SYS_INFO_SECOND_TITLE_X  SECOND_TITLE_X
#define SYS_INFO_SECOND_TITLE_Y  SECOND_TITLE_Y
#define SYS_INFO_SECOND_TITLE_W  SECOND_TITLE_W
#define SYS_INFO_SECOND_TITLE_H  SECOND_TITLE_H

//small bg
#define SYS_INFO_SMALL_BG_X  RIGHT_BG_X
#define SYS_INFO_SMALL_BG_Y  RIGHT_BG_Y
#define SYS_INFO_SMALL_BG_W  RIGHT_BG_W
#define SYS_INFO_SMALL_BG_H  RIGHT_BG_H

//item
#define SYS_INFO_ITEM_NAME_W    140 
#define SYS_INFO_ITEM_DETAIL_W  190
#define SYS_INFO_ITEM_X         RIGHT_BG_ITEM_X
#define SYS_INFO_ITEM_Y         RIGHT_BG_ITEM_Y
#define SYS_INFO_ITEM_H         RIGHT_BG_ITEM_H
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
#define SYS_INFO_ITEM_VGAP      15
#else
#define SYS_INFO_ITEM_VGAP      20
#endif
#define SYS_INFO_ITEM_HGAP      0

/* font style */

/* others */
#define revert_endian_16(x)  (((x&0xff)<<8)|(x>>8))

RET_CODE open_sys_info(u32 para1, u32 para2);

#endif

