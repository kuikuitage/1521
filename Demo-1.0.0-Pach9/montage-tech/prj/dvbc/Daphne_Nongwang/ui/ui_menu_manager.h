/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_MENU_MANAGER_H__
#define __UI_MENU_MANAGER_H__

enum ui_root_id
{
	ROOT_ID_BACKGROUND = 1,
//	ROOT_ID_MAINMENU = 2,
//	ROOT_ID_SUBMENU = 3,

	ROOT_ID_PROG_LIST = 4,

//	ROOT_ID_DO_SEARCH = 17,

	ROOT_ID_TIME_SET = 21,

	ROOT_ID_OTA = 30,
	//ROOT_ID_UPGRADE = 31,

	ROOT_ID_TETRIS = 32,
//	ROOT_ID_POPUP = 34,
	ROOT_ID_PASSWORD = 35,
	ROOT_ID_VOLUME = 36,
	ROOT_ID_SIGNAL = 37,
	ROOT_ID_MUTE = 38,
	ROOT_ID_PAUSE = 39,
	ROOT_ID_PROG_BAR = 41,
	ROOT_ID_NUM_PLAY = 42,
	ROOT_ID_NOTIFY = 43,
	ROOT_ID_EPG = 46,
	ROOT_ID_EPG_DETAIL = 47,
	ROOT_ID_FAV_SET = 49,

	ROOT_ID_FAV_LIST = 54,
	ROOT_ID_OTA_SEARCH = 56,

	ROOT_ID_CA_ENTITLE_INFO = 59,
	ROOT_ID_SLEEP_TIMER = 64,
	ROOT_ID_CHANNEL_EDIT = 65,
	ROOT_ID_EMAIL_MESS = 66,
	ROOT_ID_MANUAL_SEARCH = 68,
	ROOT_ID_SYS_INFO = 69,
	ROOT_ID_PASSWORD_MODIFY = 70,
	ROOT_ID_TP_SET = 71,
	ROOT_ID_MANUAL_UPGRADE = 72,
	ROOT_ID_RESTORE_FACTORY = 73,
	ROOT_ID_CONDITIONAL_ACCEPT = 74,
	ROOT_ID_PARAM_SET = 75,
	ROOT_ID_SIGNAL_CHECK = 76,
	ROOT_ID_PRO_CLASSIFY=77,
	ROOT_ID_PREBOOK_MANAGE=78,
	ROOT_ID_NVOD=79,
	ROOT_ID_CONDITIONAL_ACCEPT_INFO = 80,
	ROOT_ID_CONDITIONAL_ACCEPT_PAIR = 81,
	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL = 82,
	ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME = 83,
	ROOT_ID_CONDITIONAL_ACCEPT_PIN = 84,
	ROOT_ID_CONDITIONAL_ACCEPT_MOTHER = 85,
	ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH = 86,
	ROOT_ID_CONDITIONAL_ACCEPT_FEED = 87,
	ROOT_ID_GAME_INN = 90,
	ROOT_ID_GAME_BWC = 91,
	ROOT_ID_GAME_SNAKE = 92,
	ROOT_ID_CALENDAR = 93,
	ROOT_ID_CA_OPE = 94,
	ROOT_ID_CHANNEL_NUM = 95,
	ROOT_ID_CHANNEL_WARN = 96,
	ROOT_ID_CHANNEL_SAVE = 97,
	ROOT_ID_PRO_INFO = 98,
	ROOT_ID_NVOD_VIDEO = 99,

	ROOT_ID_MOSAIC = 100,
	ROOT_ID_MOSAIC_PLAY = 101,
	ROOT_ID_MOSNVOD_VOLUME = 102,
	ROOT_ID_BROWSER = 103,
	ROOT_ID_EMAIL_PIC = 104,
	ROOT_ID_CA_NOTIFY = 105,
	ROOT_ID_CA_FINGER = 106,
	ROOT_ID_CA_CARD_INFO = 107,
	ROOT_ID_CA_IPPV_PPT_DLG = 108,
	ROOT_ID_CA_ENTITLE_EXP_DLG = 109,
	ROOT_ID_CA_CARD_UPDATE = 110,
	ROOT_ID_CA_MOTHER = 111,
	ROOT_ID_CA_INFO = 112,
	ROOT_ID_EMAIL_MESS_CONTENT = 113,
	ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE = 114,
	ROOT_ID_CA_O_S_IPP_PROG = 115,
	ROOT_ID_CA_PIN_IPP= 116,
	ROOT_ID_CA_BOOK_IPP_REAL_TIME = 117,
	ROOT_ID_CA_BOOK_HISTORY = 118,
	ROOT_ID_HIDE_CA_MENU = 119,
	ROOT_ID_CA_BROADCAST = 120,
	ROOT_ID_CA_FREE_PREVIEW = 121,
	ROOT_ID_FULL_SEARCH = 122,
	ROOT_ID_FORCE_EMAIL_CONTENT = 123,
	ROOT_ID_FINGER_PRINT = 124,
	ROOT_ID_CA_PPV_ICON = 125,
	ROOT_ID_CA_PRE_AUTH = 126,
	ROOT_ID_CA_PAIR = 127,
	ROOT_ID_CHARACTER_CODE = 128,
	ROOT_ID_CA_BURSE_CHARGE = 129,

	SN_ROOT_ID_MAINMENU = 130,
	ROOT_ID_SENIOR_SUBMENU = 131,
	ROOT_ID_PROGRAM_SEARCH = 132,
	SN_ROOT_ID_SUBMENU = 133,
	ROOT_ID_PROMPT = 134,
	ROOT_ID_DIALOG = 135,
	ROOT_ID_TEST = 136,
	SN_ROOT_ID_SEARCH = 137,
	SN_ROOT_ID_SEARCH_RESULT = 138,
	SN_ROOT_ID_DISPLAY_SET = 139,
	SN_ROOT_ID_LANGUAGE_SET = 140,
	SN_ROOT_ID_TRACK_SET = 141,
	SN_ROOT_ID_PRO_INFO = 142,
	ROOT_ID_SEARCH_PROMPT = 143,
	SN_ROOT_ID_CARD_INFO = 144,
	SN_ROOT_ID_LANGUAGE_SET_COPY = 145,
	ROOT_ID_PROMPT_BAR = 146,
    ROOT_ID_CA_URGENT = 147,
	ROOT_ID_SP_LIST = 148,
	ROOT_ID_SP_LIST2 = 149,
	ROOT_ID_CA_PROMPT = 150,
	ROOT_ID_ALERT_MESS = 151,
	ROOT_ID_CA_IPP = 152,
	ROOT_ID_AUTO_FEED = 153,
	ROOT_ID_NEW_MAIL = 154,
	ROOT_ID_PROVIDER_INFO = 155,

	
	ROOT_ID_MAX,
};


#define PS_KEEP                 0 // keep status
#define PS_PLAY                 1 // play full screen
#define PS_PREV                 2 // preview play with logo
#define PS_LOGO                 3 // stop and show logo
#define PS_STOP                 4 // stop

#define OFF                     0
#define ON                      1

#define SM_OFF                  0 // needn't signal message
#define SM_LOCK                 1 // need message when lock status changed
#define SM_BAR                  2 // need message always

#define MENU_TYPE_FULLSCREEN    0
#define MENU_TYPE_PREVIEW       1
#define MENU_TYPE_NORMAL        2
#define MENU_TYPE_POPUP         3

typedef  RET_CODE (*open_menu_t)(u32 para1, u32 para2);

typedef struct
{
  u8 root_id;
  u8 play_state;                //PS_
  u8 auto_close;                //OFF,ON
  u8 signal_msg;                //SM_
  open_menu_t open_func;      // open function
}menu_attr_t;

typedef struct
{
  u8 root_id;
  u32 logo_id;
}logo_attr_t;

typedef struct
{
  u8 root_id;
  rect_t position;
}preview_attr_t;

#define INVALID_IDX 0xFF

void manage_init(void);

RET_CODE ui_menu_manage(u32 event, u32 para1, u32 para2);

RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2);

void manage_autoclose(void);

menu_attr_t *manage_get_curn_menu_attr(void);

BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height);

void manage_logo2preview(u8 root_id);

void manage_enable_autoswitch(BOOL is_enable);

u8 manage_find_preview(u8 root_id, rect_t *orc);

void ui_close_all_mennus(void);
void ui_return_to_mainmenu(void);
BOOL ui_is_preview_menu(u8 root_id);

u8 ui_get_preview_menu(void);

BOOL ui_is_fullscreen_menu(u8 root_id);

BOOL ui_is_popup_menu(u8 root_id);

void manage_auto_switch(void);

RET_CODE manage_tmr_reset(void);


//void menu_manage_enter_preview(void);


enum menu_msg
{
  MSG_SWITCH_PREVIEW = MSG_LOCAL_BEGIN + 375,
};
#endif
