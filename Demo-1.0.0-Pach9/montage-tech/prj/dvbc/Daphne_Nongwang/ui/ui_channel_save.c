/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_channel_save.h"
#include "ui_channel_numb.h"
#include "ui_channel_warn.h"


enum channel_numb_id
{
  IDC_SAVE_TXT = 1,
  IDC_SAVE_EDIT,
};

struct num_int_data
{
  u8  type;               // PWDLG_T_
  u8  total_bit;          // total bit
  u8  input_bit;          // input bit
  u32 total_value;        
  u32 input_value;        // input value
};

u16 save_edit_keymap(u16 key);
RET_CODE save_edit_proc(control_t *p_edit, u16 msg,u32 para1, u32 para2);

static struct num_int_data g_save_idata;

BOOL ui_channel_save_open(channel_save_data_t* p_data)
{
  control_t *p_cont, *p_title, *p_edit;
  pwd_set_t pwd_set;
  sys_status_get_pwd_set(&pwd_set);
  // init idata
  g_save_idata.type = 0;
  g_save_idata.total_bit = 5;
  g_save_idata.input_bit = 0;
  g_save_idata.total_value = pwd_set.psw_poweron;
  g_save_idata.input_value = 0;
  p_cont = fw_create_mainwin(ROOT_ID_CHANNEL_SAVE,
                                 p_data->left, p_data->top, 
                                  NUMB_EXCHAN_W, NUMB_EXCHAN_H,
                                  p_data->parent_root, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return FALSE;
  }
  ctrl_set_rstyle(p_cont,
                  RSI_DIG_BG,
                  RSI_DIG_BG,
                  RSI_DIG_BG);

  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_SAVE_TXT,
                             NUMB_TXT1_L, NUMB_TXT1_T,
                             NUMB_TXT1_W, NUMB_TXT1_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_WHITE,
                      FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, p_data->strid);

  p_edit = ctrl_create_ctrl((u8 *)CTRL_EBOX, IDC_SAVE_EDIT,
                            NUMB_EDIT_L, NUMB_EDIT_T,
                            NUMB_EDIT_W, NUMB_EDIT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_edit, RSI_OLD_PSW,RSI_OLD_PSW,RSI_OLD_PSW);

  ctrl_set_keymap(p_edit, save_edit_keymap);
  ctrl_set_proc(p_edit, save_edit_proc);

  ebox_set_worktype(p_edit, EBOX_WORKTYPE_HIDE);
  ebox_set_align_type(p_edit, STL_CENTER | STL_VCENTER);
  ebox_set_font_style(p_edit, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  ebox_set_maxtext(p_edit, 5);  
  ebox_set_hide_mask(p_edit, '-', '*');
  
  ctrl_default_proc(p_edit, MSG_GETFOCUS, 0, 0);
  ebox_enter_edit(p_edit);

  ctrl_paint_ctrl(p_cont, FALSE);
  return TRUE;
}

static RET_CODE on_exit_edit(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  ebox_empty_content(p_edit);
  g_save_idata.input_bit = 0;
  g_save_idata.input_value = 0;
  manage_close_menu(ROOT_ID_CHANNEL_SAVE, 0, 0);
  return SUCCESS;
}

static RET_CODE on_exit_edit_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
    
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  manage_close_menu(ROOT_ID_CHANNEL_SAVE, 0, 0);
  return manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
}

static void on_save_exit_all(void)
{
  u8 view_id = 0;
  ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
  view_id = ui_dbase_get_pg_view_id();
  db_dvbs_save(view_id);
  mtos_task_delay_ms(1000);
  manage_close_menu(ROOT_ID_CHANNEL_WARN,0,0);
  manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
}

static RET_CODE on_exchan_char(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  u32 numb;

  // process MSG_CHAR
  ebox_class_proc(p_edit, msg, para1, para2);

  g_save_idata.input_bit++;
  g_save_idata.input_value = g_save_idata.input_value * 10 +
                              (msg & MSG_DATA_MASK) - '0';  
   
  numb = g_save_idata.input_value;
  if(g_save_idata.input_bit == g_save_idata.total_bit)
  {
    if(numb == g_save_idata.total_value)
    {
      channel_warn_data_t warn_data_t =
      {
        ROOT_ID_INVALID,
        NUMB_WARN_X,
        NUMB_WARN_Y,
        IDS_SAVING_WAITE,
      };
      ebox_empty_content(p_edit);
      g_save_idata.input_bit = 0;
      g_save_idata.input_value = 0;
      manage_close_menu(ROOT_ID_CHANNEL_SAVE, 0, 0);
      ui_channel_warn_open(&warn_data_t);
      on_save_exit_all();
    }
    else
    {

      ebox_empty_content(p_edit);
      g_save_idata.input_bit = 0;
      g_save_idata.input_value = 0;
      ctrl_paint_ctrl(p_edit->p_parent,FALSE);
    }
  }
  return SUCCESS;
}

BEGIN_KEYMAP(save_edit_keymap, NULL)
  ON_EVENT(V_KEY_0, MSG_CHAR | '0')
  ON_EVENT(V_KEY_1, MSG_CHAR | '1')
  ON_EVENT(V_KEY_2, MSG_CHAR | '2')
  ON_EVENT(V_KEY_3, MSG_CHAR | '3')
  ON_EVENT(V_KEY_4, MSG_CHAR | '4')
  ON_EVENT(V_KEY_5, MSG_CHAR | '5')
  ON_EVENT(V_KEY_6, MSG_CHAR | '6')
  ON_EVENT(V_KEY_7, MSG_CHAR | '7')
  ON_EVENT(V_KEY_8, MSG_CHAR | '8')
  ON_EVENT(V_KEY_9, MSG_CHAR | '9')
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)
END_KEYMAP(save_edit_keymap, NULL)

BEGIN_MSGPROC(save_edit_proc, ebox_class_proc)
  ON_COMMAND(MSG_CHAR, on_exchan_char)
  ON_COMMAND(MSG_EXIT, on_exit_edit)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_edit_all)
END_MSGPROC(save_edit_proc, ebox_class_proc)




