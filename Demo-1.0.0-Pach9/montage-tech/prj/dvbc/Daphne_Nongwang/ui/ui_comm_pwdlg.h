/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_COMM_PWDLG_H__
#define __UI_COMM_PWDLG_H__

#define PWDLG_T_COMMON       0  // super password or common password can pass , 4 bits
#define PWDLG_T_CA           1  // ca password can pass , 6 bits
#define PWDLG_T_SUPER        2  // super password can pass , 4 bits
#define PWDLG_T_IPPV        3

#define PWD_LENGTH_COMMON    6
#define PWD_LENGTH_SUPER     4
#define PWD_LENGTH_CA        6
#define PWD_LENGTH_IPPV     6

#define PWDLG_W             SUB_PREV_W
#define PWDLG_H              100

#define PWDLG_TXT1_L         0
#define PWDLG_TXT1_T         0
#define PWDLG_TXT1_W         PWDLG_W
#define PWDLG_TXT1_H         40

#define PWDLG_EDIT_L         50
#define PWDLG_EDIT_T         53
#define PWDLG_EDIT_W         (PWDLG_W - 2 * PWDLG_EDIT_L)
#define PWDLG_EDIT_H         35

#define PWDLG_HELP_L         0
#define PWDLG_HELP_T         (PWDLG_H - PWDLG_HELP_H - 10)
#define PWDLG_HELP_W         PWDLG_W
#define PWDLG_HELP_H         30

#define FSI_PWDLG_TXT        FSI_COMM_TXT_N
#define FSI_PWDLG_HELP       FSI_HELP_TEXT

#define RSI_PWDLG_CONT       RSI_DLG_CONT
#define RSI_PWDLG_EDIT       RSI_OLD_PSW

typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
  u8                  pwd_class;
  keymap_t keymap;
  msgproc_t proc;
}comm_pwdlg_data_t;

BOOL ui_comm_pwdlg_open(comm_pwdlg_data_t* p_data);
void ui_comm_pwdlg_close(void);

#endif
