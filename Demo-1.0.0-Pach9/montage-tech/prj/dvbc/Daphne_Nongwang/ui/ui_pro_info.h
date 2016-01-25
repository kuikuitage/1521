/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_PRO_INFO_H__
#define __UI_PRO_INFO_H__

#define PRO_INFO_X             0
#define PRO_INFO_Y             160
#define PRO_INFO_W             SCREEN_WIDTH
#define PRO_INFO_H             (SCREEN_HEIGHT-PRO_INFO_Y)

#define PRO_INFO_CHAN_X              300//220
#define PRO_INFO_CHAN_Y              58
#define PRO_INFO_CHAN_W              (PRO_INFO_W-PRO_INFO_CHAN_X)
#define PRO_INFO_CHAN_H              (PRO_INFO_H-116)

#define PRO_INFO_CHECK_X              100
#define PRO_INFO_CHECK_Y              10
#define PRO_INFO_CHECK_W             100
#define PRO_INFO_CHECK_H              30

#define PRO_INFO_NUMB_X              (PRO_INFO_CHECK_X+PRO_INFO_CHECK_W +100)
#define PRO_INFO_NUMB_Y              PRO_INFO_CHAN_Y
#define PRO_INFO_NUMB_W              200
#define PRO_INFO_NUMB_H              30

#define PRO_INFO_TEXT_X              100
#define PRO_INFO_TEXT_Y              70
#define PRO_INFO_TEXT_W              130
#define PRO_INFO_TEXT_H              30
#define PRO_INFO_TEXT1_W             180


//signal strength0
#define SIG_STRENGTH_NAME_X       PRO_INFO_TEXT_X
#define SIG_STRENGTH_NAME_Y       (3*PRO_INFO_TEXT_H+70)
#define SIG_STRENGTH_NAME_W       100
#define SIG_STRENGTH_NAME_H       30

#define SIG_STRENGTH_PBAR_X       (SIG_STRENGTH_NAME_X + SIG_STRENGTH_NAME_W)
#define SIG_STRENGTH_PBAR_Y       (SIG_STRENGTH_NAME_Y + (SIG_STRENGTH_NAME_H - SIG_STRENGTH_PBAR_H)/2)
#define SIG_STRENGTH_PBAR_W       150//230
#define SIG_STRENGTH_PBAR_H       COMM_PBAR_H

#define SIG_STRENGTH_PERCENT_X    (SIG_STRENGTH_PBAR_X + SIG_STRENGTH_PBAR_W)
#define SIG_STRENGTH_PERCENT_Y    SIG_STRENGTH_NAME_Y
#define SIG_STRENGTH_PERCENT_W    80//50
#define SIG_STRENGTH_PERCENT_H    30


//signal strength1
#define SIG_SET_NAME_X       PRO_INFO_TEXT_X
#define SIG_SET_NAME_Y       (SIG_STRENGTH_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SET_NAME_W       100
#define SIG_SET_NAME_H       30

#define SIG_SET_PBAR_X       (SIG_SET_NAME_X + SIG_STRENGTH_NAME_W)
#define SIG_SET_PBAR_Y       (SIG_SET_NAME_Y + (SIG_SET_NAME_H - SIG_SET_PBAR_H)/2)
#define SIG_SET_PBAR_W       150
#define SIG_SET_PBAR_H       COMM_PBAR_H

#define SIG_SET_PERCENT_X    (SIG_SET_PBAR_X + SIG_SET_PBAR_W)
#define SIG_SET_PERCENT_Y    SIG_SET_NAME_Y
#define SIG_SET_PERCENT_W    50
#define SIG_SET_PERCENT_H    30

//signal strength2
#define SIG_SNR_NAME_X       PRO_INFO_TEXT_X
#define SIG_SNR_NAME_Y       (SIG_SET_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SNR_NAME_W       100
#define SIG_SNR_NAME_H       30

#define SIG_SNR_PBAR_X       (SIG_SNR_NAME_X + SIG_SNR_NAME_W)
#define SIG_SNR_PBAR_Y       (SIG_SNR_NAME_Y + (SIG_SNR_NAME_H - SIG_SNR_PBAR_H)/2)
#define SIG_SNR_PBAR_W       150
#define SIG_SNR_PBAR_H       COMM_PBAR_H

#define SIG_SNR_PERCENT_X    (SIG_SNR_PBAR_X + SIG_SNR_PBAR_W)
#define SIG_SNR_PERCENT_Y    SIG_SNR_PBAR_Y
#define SIG_SNR_PERCENT_W    100
#define SIG_SNR_PERCENT_H    30

RET_CODE open_pro_info(u32 para1, u32 para2);

#endif

