/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_COMM_ROOT_H__
#define __UI_COMM_ROOT_H__

/*********************************************************************
 * COMMON DEFINATION
 *********************************************************************/
#define IDC_COMM_TITLE_CONT     0xFE   /* special for title */
#define IDC_COMM_ROOT_CONT      0xFD   /* special for signbar */
#define IDC_COMM_SIGN_CONT      0xFC   /* special for signbar */
#define IDC_COMM_HELP_CONT      0xFB   /* special for signbar */
#define IDC_COMM_TIMEDATE_CONT  0xFA   /* special for title */
#define IDC_COMM_ROOT_BG      0xF9   /* the frm except title and help */
#define BACKGROUND_IDC_TITLE_CONT     			0xF8   /* special for title */
#define BACKGROUND_IDC_CLIENT_CONT      		0xF7   /* special for signbar */
#define BACKGROUND_IDC_SIGN_CONT      			0xF6   /* special for signbar */
#define BACKGROUND_IDC_HELP_CONT      			0xF5   /* special for signbar */
#define BACKGROUND_IDC_TIMEDATE_CONT  		0xF4   /* special for title */
#define BACKGROUND_IDC_BG_CONT				0xF3   /* the frm except title and help */
#define BACKGROUND_IDC_BG_TITLE             0xF2   /*the frm title*/

#define IDC_COMM_AD             0XEF
#define IDC_COMM_TV             0XEE
#define IDC_COMM_SECOND_TITLE   0XED
#define IDC_COMM_RIGHT_BG       0XEC
#define SN_IDC_COMM_HELP_CONT	0xEB


/*********************************************************************
 * COMMON COORDINATE
 *********************************************************************/
#define COMM_ROOT_Y             12
#define COMM_ROOT_W             580
#define COMM_ITEM_OX_IN_ROOT    4
#define COMM_ITEM_OY_IN_ROOT    (SECOND_TITLE_H + 16)
#define COMM_ITEM_LW            280
#define COMM_ITEM_H             COMM_CTRL_H
#define COMM_ITEM_MAX_WIDTH     (COMM_ROOT_W - 2 * COMM_ITEM_OX_IN_ROOT)

/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
/* coordinate */
#define TITLE_CONT_X      0
#define TITLE_CONT_Y      0
#define TITLE_CONT_W     180
#define TITLE_CONT_H      53

#define TITLE_ICON_X       230
#define TITLE_ICON_Y       0
#define TITLE_ICON_W      30
#define TITLE_ICON_H       TITLE_CONT_H

#define TITLE_TXT_X         (TITLE_ICON_X + TITLE_ICON_W + 20)
#define TITLE_TXT_Y         0
#define TITLE_TXT_W        230
#define TITLE_TXT_H         TITLE_CONT_H


#define TIMEDATE_X                 300
#define TIMEDATE_Y                 0
#define TIMEDATE_W                 360
#define TIMEDATE_H                 57

#define TIMEDATE_TXT_X             34
#define TIMEDATE_TXT_Y             ((TIMEDATE_H - TIMEDATE_TXT_H) / 2)
#define TIMEDATE_TXT_W             (TIMEDATE_W - 72)
#define TIMEDATE_TXT_H             26

#define FSI_TIMEDATE_TXT           FSI_TOP_TITLE_TXT

/*********************************************************************
 * COMMON  advertisement window
 *********************************************************************/
//advertisement window
#define SUB_AD_X     36
#define SUB_AD_Y     125//130
#define SUB_AD_W     245
#define SUB_AD_H     110

//preview window
#define SUB_PREV_X     SUB_AD_X
#define SUB_PREV_Y     (SUB_AD_Y + SUB_AD_H + 30)
#define SUB_PREV_W     SUB_AD_W
#define SUB_PREV_H     200

//rignt bg:second title,the title in left side
#define SECOND_TITLE_X  296
#define SECOND_TITLE_Y  80//85
#define SECOND_TITLE_W  345
#define SECOND_TITLE_H  35

//right bg
#define RIGHT_BG_X  SECOND_TITLE_X
#define RIGHT_BG_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 10)
#define RIGHT_BG_W  SECOND_TITLE_W
#define RIGHT_BG_H  340
#define RIGHT_BG_ITEM_Y (RIGHT_BG_Y + RIGHT_BG_ITEM_START_VGAP)
#define RIGHT_BG_ITEM_X (SECOND_TITLE_X + TEXT_START_OX)
#define RIGHT_BG_ITEM_H 30
#define RIGHT_BG_ITEM_START_VGAP 10
#define SUB_CONT_VGAP   10

//whole screen title
#define WHOLE_SCR_TITLE_X  SUB_AD_X
#define WHOLE_SCR_TITLE_Y  SECOND_TITLE_Y
#define WHOLE_SCR_TITLE_W  605
#define WHOLE_SCR_TITLE_H  SECOND_TITLE_H

#define COMM_BTN_ITEM_H   35  /*the btn pic hight is 35*/
#define COMM_LIST_BAR_W   16  
#define COMM_PBAR_H       12//15    //LXD   
#define LIST_BAR_MID_OY  14

#define SCREEN_OFFSET_X        10 /*move to left*/
#define MENU_CONT_OX        5
#define TEXT_START_OX       COMM_CTRL_OX
/*********************************************************************
 * COMMON MENU STYLE
 *********************************************************************/
/* rect style */
//#define RSI_FULL_SCREEN_BG    RSI_COMM_ROOT_BG
//#define RSI_FULL_SCR_BG_MID    RSI_MAINWND_BG
//#define RSI_HELP         RSI_HELP_BG2

//#define RSI_COMM_LIST_N      RSI_TIP_BOX_BG
//#define RSI_COMM_LIST_HL      RSI_ITEM_6_SH
//#define RSI_COMM_LIST_SEL      RSI_WHITE_GREEN

//#define RSI_INFOBOX                 RSI_TIP_BOX
//#define RSI_INFOBOX_TITLE       RSI_BLUE_GREEN1
//#define RSI_INFOBOX_TITLE2       RSI_BLUE_GREEN

//#define RSI_SECOND_TITLE          RSI_COMMON_BTN_HL
//#define RSI_SECOND_TITLE_HL     RSI_COMMON_BTN_ORANGE
//#define RSI_DLG_TITLE                 RSI_BLUE_GREEN1

void ui_comm_timedate_create(control_t *parent, u16* p_unistr, u8 rstyle);

void ui_comm_timedate_set_content_unistr(control_t *parent, u16* p_unistr);

void ui_comm_timedate_update(control_t *parent, BOOL is_draw);

void ui_comm_title_create(control_t *parent, u16 icon_id, u16 strid, u8 rstyle);

void ui_comm_title_set_content(control_t *parent, u16 strid);

void ui_comm_title_update(control_t *parent);

/****************************************************
 * COMMON HELP BAR
 ****************************************************/

/* coordinate */
#define SN_HELP_H		43

#define HELP_ITEM_H_GAP      100
#define HELP_ITEM_MAX_CNT    10
#define HELP_ITEM_H          52
#define SN_HELP_ITEM_MAX_CNT		 15

#define HELP_ITEM_ICON_W     34
#define HELP_ITEM_ICON_OX    0
#define HELP_ITEM_ICON_OY    0

#define HELP_ITEM_TEXT_OX    HELP_ITEM_ICON_W
#define HELP_ITEM_TEXT_OY    0

#define HELP_RSC_MASK         (0xF0000000)
#define HELP_RSC_BMP          (0x10000000)
#define HELP_RSC_STRID        (0x20000000)

typedef struct
{
  u8 item_cnt;                        //item number
  u8 col_cnt;                         //item number per row
  u16 str_id[HELP_ITEM_MAX_CNT];      //string id
  u16 bmp_id[HELP_ITEM_MAX_CNT];      //bmp id
  u8 bg_rsi;                        //background style
}comm_help_data_t;

typedef struct
{
  int item_cnt;                        //item number
  int offset;                         //offset
  u16 rsc_width[HELP_ITEM_MAX_CNT];      //string id
  u32 rsc_id[HELP_ITEM_MAX_CNT];      //string id or bitmap id
}comm_help_data_t2;

typedef enum{
 SN_IDC_HELP_BMAP=1,
 SN_IDC_HELP_TEXT,
}help_type;

typedef struct{
	help_type type;
	u16 rsc_width;
	u16 rsc_id;
}help_rsc;
typedef struct
{
  u16 x,y;						
  int offset;    //初始偏移量                     
  int item_cnt;  //需要的图片及字符串个数                      
  help_rsc *rsc;//类型、宽度及id
}sn_comm_help_data_t;


typedef struct
{
  u16 bmp_id;
  u16 str_id;
  u8 bg_rsi;          //background style
  u16 second_title_strid;
}full_screen_title_t;

void sn_ui_comm_help_create(sn_comm_help_data_t *p_data, control_t *p_parent);

/*********************************************************************
 * COMMON ROOT
 *********************************************************************/

/* coordinate */
#define ROOT_SIGN_CONT_H             80
#define ROOT_SIGN_CONT_GAP           4

#define ROOT_SIGN_BAR_STXT_X         20
#define ROOT_SIGN_BAR_STXT_Y         10
#define ROOT_SIGN_BAR_STXT_W         160
#define ROOT_SIGN_BAR_STXT_H         30

#define ROOT_SIGN_BAR_X \
  (ROOT_SIGN_BAR_STXT_X + \
   ROOT_SIGN_BAR_STXT_W)
#define ROOT_SIGN_BAR_Y \
  (ROOT_SIGN_BAR_STXT_Y +  \
   (ROOT_SIGN_BAR_STXT_H - \
    ROOT_SIGN_BAR_H) / 2)
#define ROOT_SIGN_BAR_W \
  (COMM_ROOT_W - 2 *      \
   ROOT_SIGN_BAR_STXT_X - \
   ROOT_SIGN_BAR_STXT_W - \
   ROOT_SIGN_BAR_PERCENT_W)
#define ROOT_SIGN_BAR_H              16

#define ROOT_SIGN_BAR_PERCENT_X \
  (ROOT_SIGN_BAR_X + \
   ROOT_SIGN_BAR_W)
#define ROOT_SIGN_BAR_PERCENT_Y      ROOT_SIGN_BAR_STXT_Y
#define ROOT_SIGN_BAR_PERCENT_W      60
#define ROOT_SIGN_BAR_PERCENT_H      ROOT_SIGN_BAR_STXT_H

#define ROOT_SIGN_CTRL_V_GAP         0

/* rect style */
#define RSI_ROOT_SIGN_CONT           RSI_TRANSPARENT
//#define RSI_ROOT_SIGN_BAR_MID_1      RSI_COMM_PBAR_MID
//#define RSI_ROOT_SIGN_BAR_MID_2      RSI_COMM_PBAR_MID

#define RSI_ROOT_SIGN_BAR_STXT       RSI_ITEM_3_SH
#define RSI_ROOT_SIGN_BAR_PERCENT    RSI_ITEM_3_SH

/* font style */
#define FSI_ROOT_SIGN_BAR_STXT       FSI_WHITE
#define FSI_ROOT_SIGN_BAR_PERCENT    FSI_WHITE

/* others */
#define BEEPER_UNLKD_TMOUT           2000
#define BEEPER_LOCKD_TMOUT           1000
#define BACKGROUND_FLAGS_HAVE_TIME 		(1<<1)

control_t *ui_background_create(u8 root_id,
                               u16 x, u16 y, u16 w, u16 h,
                               u16 title_strid,u32 flags);

control_t *ui_comm_root_create(u8 root_id,
                               u8 rsi_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_strid,
                               u8 rsi_title);


control_t *ui_comm_prev_root_create(u8 root_id,  u8 rsi_root,
                              u16 x, u16 y, u16 w, u16 h,
                              full_screen_title_t *p_title_data);

control_t *ui_comm_root_create_with_signbar(u8 root_id,
                                            u8 rsi_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_strid,
                                            u8 rsi_title,
                                            BOOL is_beeper);

void ui_comm_root_update_signbar(control_t *root,
                                 u8 intensity,
                                 u8 quality,
                                 BOOL is_lock);

control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id);

u16 ui_comm_root_keymap(u16 key);

RET_CODE ui_comm_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#endif

