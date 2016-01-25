/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

#define UI_DEBUG

#ifdef UI_DEBUG
#define UI_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define UI_PRINTF(param ...)    do { } while (0)
#else
#define UI_PRINTF
#endif
#endif

enum user_msg
{
  MSG_POWER_ON = MSG_USER_BEGIN,
  MSG_POWER_OFF,
  MSG_TIMER_EXPIRED,
  MSG_CLOSE_MENU,       /* only for the system timer with the attribute of autoclose */
  MSG_EXIT_ALL,
  MSG_MUTE,
  MSG_PAUSE,
  MSG_AUTO_SWITCH,
  MSG_TVSYS_SWITCH,
  MSG_OPEN_MENU,
  MSG_BLACK_SCREEN,
  MSG_1ST_PIC,
  MSG_CHK_PWD,
  MSG_CORRECT_PWD,
  MSG_WRONG_PWD,

  MSG_SUBT,
  MSG_AUTO_TEST,

  MSG_BEEPER,
  MSG_T9_CHANGED,

  MSG_RENAME_CHECK,
  MSG_RENAME_UPDATE,  
  MSG_FAV_UPDATE,
  MSG_SORT_UPDATE,
  MSG_NUM_SELECT,
  MSG_TIME_UPDATE,
  MSG_TVRADIO,
  MSG_KEY_UPDATE,
  MSG_LNB_SHORT,
  MSG_SLEEP_TMROUT,
  MSG_EXIT_MOSAIC_PROG_PLAY,
  MSG_TO_MAINMENU,
  MSG_UPDATE_SUBTIEM,
  MSG_SET_LANG,
  MSG_PRESET_PROG,
  MSG_RECOVER_CA_PROMPT,
  MSG_CLOSE_ALL_MENUS,
  MSG_PIC_EVT_DRAW_END,
  MSG_PIC_EVT_DRAW_FAIL,
  MSG_SHOW_CHANGE_CHANNEL_PIC,
  MSG_REFRESH_ADS_PIC,
  MSG_PROG_FAST_SWITCH,
  MSG_REVERT_SW_ON_OTA,
  MSG_PRINTF_SWITCH,
  MSG_EXIT_AD_ROLLING,
  MSG_PRO_CLASSIFY,
  MSG_CA_ZONE_CHECK_END,
  MSG_CA_DS_CLEAR_UNAUTH,
};

//user shift key define
enum user_shfit_msg
{
  MSG_OPEN_MENU_IN_TAB = MSG_SHIFT_USER_BEGIN + 0x0100, // increase 0x100 once.
};

// desktop config
#define SCREEN_WIDTH              672//620    //608
#define SCREEN_HEIGHT             556//(452 + 2)    //430 // tmp for osd patch

#define SCREEN_POS_PAL_L          ((720 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_PAL_T          ((576 - SCREEN_HEIGHT) / 2)
#define SCREEN_POS_NTSC_L         ((720 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_NTSC_T         ((480 - SCREEN_HEIGHT) / 2)

#define MAX_ROOT_CONT_CNT         8
#define MAX_MESSAGE_CNT           64
#define MAX_HOST_CNT              64
#define MAX_TMR_CNT               32

#define DST_IDLE_TMOUT            -1
#define POST_MSG_TMOUT            0     //100 ms
#define RECEIVE_MSG_TMOUT         0     //100 ms

// gdi config
#define MAX_CLIPRECT_CNT          512
#define MAX_SURFACE_CNT           3
#define MAX_DC_CNT                6

#define MAX_CLASS_CNT             12
#define MAX_OBJ_CNT               (MAX_OBJECT_NUMBER_CNT +  \
                                   MAX_OBJECT_NUMBER_TXT +  \
                                   MAX_OBJECT_NUMBER_BMP +  \
                                   MAX_OBJECT_NUMBER_CBB +  \
                                   MAX_OBJECT_NUMBER_LIST + \
                                   MAX_OBJECT_NUMBER_MTB +  \
                                   MAX_OBJECT_NUMBER_PGB +  \
                                   MAX_OBJECT_NUMBER_SCB +  \
                                   MAX_OBJECT_NUMBER_EDT +  \
                                   MAX_OBJECT_NUMBER_NUM + \
                                   MAX_OBJECT_NUMBER_TIM + \
                                   MAX_OBJECT_NUMBER_SBX)

//how many control will be created at the same time
#define MAX_OBJECT_NUMBER_CNT     64
#define MAX_OBJECT_NUMBER_TXT    128// 64
#define MAX_OBJECT_NUMBER_BMP     80
#define MAX_OBJECT_NUMBER_CBB     16
#define MAX_OBJECT_NUMBER_LIST    16
#define MAX_OBJECT_NUMBER_MTB     16
#define MAX_OBJECT_NUMBER_PGB     8
#define MAX_OBJECT_NUMBER_SCB     16
#define MAX_OBJECT_NUMBER_EDT     8
#define MAX_OBJECT_NUMBER_NUM     32
#define MAX_OBJECT_NUMBER_TIM     8
#define MAX_OBJECT_NUMBER_SBX     8
#define MAX_ROLL_CNT 2

// db

//string length
#define MAX_CTRL_STRING_BUF_SIZE    8192
#define MAX_PAINT_STRING_LENGTH     512
#define MAX_PAINT_STRING_LINES        10

// others
#define AP_VOLUME_MAX            31
#define SS_PWD_LENGTH            6

#define DEFAULT_YEAR             2009
#define DEFAULT_MONTH            1
#define DEFAULT_DAY              1

#define SECOND                   1000

#define DEFAULT_TMOUT_MS             (10*SECOND)

#define INVALID_U32    0xFFFFFFFF
#endif

