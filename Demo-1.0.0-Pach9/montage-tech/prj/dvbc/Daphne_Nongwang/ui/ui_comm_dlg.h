/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_COMM_DLG_H__
#define __UI_COMM_DLG_H__

/* coordinate */
#define COMM_DLG_X         ((SCREEN_WIDTH - COMM_DLG_W) / 2)
#define COMM_DLG_Y         ((SCREEN_HEIGHT - COMM_DLG_H) / 2)
#define COMM_DLG_W         310
#define COMM_DLG_H         100
#define COMM_DLG_WITHTEXT_H         200

#define DLG_CONTENT_GAP    16

#define DLG_BTN_W          64
#define DLG_BTN_H          35

#define DLG_TITLE_H    40
#define DLG_TEXT_X    4
#define DLG_TEXT_H    80
#define INFOBOX_TITLE_H       DLG_TITLE_H

/* rect style */
//#define RSI_DLG_CONT       RSI_TIP_BOX

/* font style */
#define FSI_DLG_BTN_N     FSI_COMM_BTN
#define FSI_DLG_BTN_HL     FSI_COMM_TXT_HL
#define FSI_DLG_TITLE    FSI_SECOND_TITLE

typedef enum 
{
  DLG_FOR_ASK = 0x00, //popup dialog type:	with  yes/no button
  DLG_FOR_CONFIRM,    //popup dialog type:	with ok button
  DLG_FOR_SHOW,       //popup dialog type:	without button
}dlg_type;

typedef enum 
{
  DLG_RET_YES,
  DLG_RET_NO,
  DLG_RET_NULL,
}dlg_ret_t;

typedef enum {
	DIALOG_INFO = 1,
	DIALOG_WARNING,
	DIALOG_ERR,
	DIALOG_DEADLY,
}dialog_grade_t;

enum dlg_str_mode
{
  DLG_STR_MODE_NULL = 0x00,
  DLG_STR_MODE_STATIC = 0x10,
  DLG_STR_MODE_EXTSTR = 0x20,
};

typedef struct
{
  u8        parent_root;
  u8        title_style;
  u8        text_style;
  u16       x;
  u16       y;
  u16       w;
  u16       h;
  u32       title_strID;  //标题
  u32       title_content;//标题
  u32       text_strID;   //正文
  u32       text_content; //正文
  u32       dlg_tmout;
}comm_dlg_data_t;

typedef struct
{
	u8        			parent_root;
#define  STR_MODE_STATIC       (1) 	//use string id to view string
	u8 				string_mode;	
	dlg_type 		type;
	u16 				x,y,w,h;
	u32       			text_strID;   		//正文
	char       			* text_content; 	//正文
	u32 	 			icon_id; // if icon_id is not 0, we will use icon_id to view prompt bmp
	dialog_grade_t 	grade;  // if icon_id is 0, use default bmp
	u32       			timeout;
}comm_dialog_data_t;

typedef enum {
	PROMPT_INFO = 1,
	PROMPT_WARNING,
	PROMPT_ERR,
	PROMPT_DEADLY,
}prompt_grade_t;
typedef struct
{
	u8        			parent_root;
#define  STR_MODE_STATIC       (1) //use string id to use string
	u8 				string_mode;
	u16       			x,y,w,h;
	u16					offset;
	u32       			text_strID;   //正文
	char       			* text_content; //正文
	u32 	 			icon_id; // if icon_id is not 0, we will use icon_id to view prompt bmp
	prompt_grade_t 	grade;  // if icon_id is 0, use default bmp
	u32       			timeout;
}comm_prompt_data_t;

typedef struct
{
	u8        			parent_root;
	u16       			x,y,w,h;
	u16					offset;
	s32					radio_num;
	s32					tv_num;
	u32       			tv_text_strID;   //正文tv id
	u32 				radio_text_strID;		//radio id
	char       			* text_content; //正文
	u32 	 			icon_id; // if icon_id is not 0, we will use icon_id to view prompt bmp
	prompt_grade_t 	grade;  // if icon_id is 0, use default bmp
	u32       			timeout;
}comm_prompt_data;

int ui_comm_prompt_open(const comm_prompt_data_t *p_data);
void ui_comm_prompt_close(void);
int ui_comm_prompt_search_open(const comm_prompt_data *p_data);

typedef void (*do_func_t)(void);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data);

void ui_comm_dlg_close(void);

void ui_comm_cfmdlg_open2(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out);
dlg_ret_t ui_comm_dialog_open(const comm_dialog_data_t *p_data);

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);
#endif


