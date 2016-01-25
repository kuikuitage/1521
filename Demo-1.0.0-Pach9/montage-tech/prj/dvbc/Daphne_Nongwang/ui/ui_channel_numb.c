/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_channel_numb.h"
#include "ui_channel_warn.h"
#include "ui_num_play.h"


enum channel_numb_id
{
  IDC_NUMB_TXT = 1,
  IDC_NUMB_EDIT,
};

struct num_int_data
{
  u8  type;               // PWDLG_T_
  u8  total_bit;          // total bit
  u8  input_bit;          // input bit
  u16 input_value;        // input value
};
static u16 total;

u16 numb_edit_keymap(u16 key);
RET_CODE numb_edit_proc(control_t *p_edit, u16 msg,u32 para1, u32 para2);
static struct num_int_data g_num_idata;

BOOL ui_channel_numb_open(channel_numb_data_t* p_data, u16 numb)
{
  control_t *p_cont, *p_title, *p_edit;

  // init idata
  g_num_idata.type = 0;
  g_num_idata.total_bit = 4;
  g_num_idata.input_bit = 0;
  g_num_idata.input_value = 0;
  total = numb;

  p_cont = fw_create_mainwin(ROOT_ID_CHANNEL_NUM,
                                 p_data->left, p_data->top, 
                                  NUMB_EXCHAN_W, NUMB_EXCHAN_H,
                                  p_data->parent_root, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return FALSE;
  }
  ctrl_set_rstyle(p_cont,
                  RSI_DIG_BG,//RSI_INFOBOX,
                  RSI_DIG_BG,//RSI_INFOBOX,
                  RSI_DIG_BG);//RSI_INFOBOX,

  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NUMB_TXT,
                             NUMB_TXT1_L, NUMB_TXT1_T,
                             NUMB_TXT1_W, NUMB_TXT1_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);//RSI_DLG_TITLE
  text_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, p_data->strid);

  p_edit = ctrl_create_ctrl((u8 *)CTRL_EBOX, IDC_NUMB_EDIT,
                            NUMB_EDIT_L, NUMB_EDIT_T,
                            NUMB_EDIT_W, NUMB_EDIT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_edit, RSI_OLD_PSW, RSI_OLD_PSW, RSI_OLD_PSW);

  ctrl_set_keymap(p_edit, numb_edit_keymap);
  ctrl_set_proc(p_edit, numb_edit_proc);

  ebox_set_worktype(p_edit, EBOX_WORKTYPE_EDIT);
  ebox_set_align_type(p_edit, STL_CENTER | STL_VCENTER);
  ebox_set_font_style(p_edit, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  ebox_set_maxtext(p_edit, 4);  
  
  ctrl_default_proc(p_edit, MSG_GETFOCUS, 0, 0);
  ebox_enter_edit(p_edit);

  ctrl_paint_ctrl(p_cont, FALSE);
  return TRUE;
}

static RET_CODE on_exit_exchan_num(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  ebox_empty_content(p_edit);
  g_num_idata.input_bit = 0;
  g_num_idata.input_value = 0;
  manage_close_menu(ROOT_ID_CHANNEL_NUM, 0, 0);
  return SUCCESS;
}

static RET_CODE on_exit_exchan_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
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
  manage_close_menu(ROOT_ID_CHANNEL_NUM, 0, 0);
  return manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
}


static RET_CODE on_exchan_num(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  u32 numb,lcn_numb;
  control_t *p_ctrl = NULL;

  lcn_numb = 0;
  
#ifdef LCN_SWITCH
{
  u16 pos_id =0;
  lcn_numb = g_num_idata.input_value;
  pos_id = get_pos_by_logic_num(lcn_numb);
  if(pos_id == INVALIDPOS)
    numb = 0;
  else
    numb = pos_id + 1;
}
#else
  numb = g_num_idata.input_value;
#endif

  if(numb > total || (numb == 0) )
  {
    ebox_empty_content(p_edit);
    g_num_idata.input_bit = 0;
    g_num_idata.input_value = 0;
    p_ctrl = ctrl_get_child_by_id(p_edit->p_parent,IDC_NUMB_TXT);
    text_set_content_by_strid(p_ctrl, IDS_INDEX_NOT_EXIST);
    ctrl_paint_ctrl(p_edit->p_parent,TRUE);
  }
  else
  {
    fw_notify_parent(ROOT_ID_CHANNEL_NUM, NOTIFY_T_MSG, FALSE,
                      MSG_NUM_SELECT, (u32)numb, 0);
    manage_close_menu(ROOT_ID_CHANNEL_NUM, 0, 0);
  }
                              
  return SUCCESS;
}

static RET_CODE on_exchan_char(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  // process MSG_CHAR
  ebox_class_proc(p_edit, msg, para1, para2);

  g_num_idata.input_bit++;
  g_num_idata.input_value = g_num_idata.input_value * 10 +
                              (msg & MSG_DATA_MASK) - '0';  
  return SUCCESS;
}

static RET_CODE on_ebox_back_space(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
     // process MSG_CHAR
    ebox_class_proc(p_ctrl, msg, para1, para2);
  
    g_num_idata.input_bit--;
    g_num_idata.input_value = g_num_idata.input_value / 10;

    return SUCCESS;
}

BEGIN_KEYMAP(numb_edit_keymap, NULL)
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
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)  
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(numb_edit_keymap, NULL)

BEGIN_MSGPROC(numb_edit_proc, ebox_class_proc)
  ON_COMMAND(MSG_CHAR, on_exchan_char)
  ON_COMMAND(MSG_SELECT, on_exchan_num)
  ON_COMMAND(MSG_EXIT, on_exit_exchan_num)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_exchan_all)
  ON_COMMAND(MSG_BACKSPACE, on_ebox_back_space)
END_MSGPROC(numb_edit_proc, ebox_class_proc)




