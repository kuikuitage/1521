/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_num_play.h"
#include "ui_signal.h"
#include "ui_mute.h"
enum control_id
{
  IDC_NUM_ICON = 1,
  IDC_NUM_FRM,
};

enum sub_control_id
{
  IDC_NUM_TXT = 1,
};
static comm_prompt_data_t dlg_data = 
{
   ROOT_ID_INVALID,
   STR_MODE_STATIC,
   COMM_DLG_X,COMM_DLG_Y - 29,COMM_DLG_W,COMM_DLG_H,30,
   IDS_CHANNEL_NOT_EXIST,0,
   0,DIALOG_DEADLY,
   3000 	
};
static u8 g_num_play_bit;
static u16 g_prog_cnt;
BOOL is_fullsrc = FALSE;

static u8 g_input_cont = NUM_PLAY_CNT;
static u8 g_pos_on_right = FALSE;
static u8 g_num_big_font = FALSE;

void ui_set_input_number_cont(u8 cont)
{
    g_input_cont = cont;
}
void ui_set_input_number_on_right(void)
{
    g_pos_on_right = TRUE;
}
void ui_set_num_big_font(void)
{
  g_num_big_font = TRUE;
}
#ifdef LCN_SWITCH
u16 get_pos_by_logic_num(u16 logic_num)
{
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 count = db_dvbs_get_count(view_id);
  u16 i, pg_id, pg_pos = INVALIDPOS;
  dvbs_prog_node_t prog = {0};

  for(i=0; i<count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);

    if(logic_num == prog.logic_ch_num)
    {
      pg_pos = i;
      break;
    }
  }
  return pg_pos;
}

static void jump_to_prog_with_logic_num(control_t *p_cont)
{
  control_t *p_ctrl, *p_subctrl;
  u16 curn;
  u16 prog_id;
  u16 prog_pos;
  control_t *p_mute;
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NUM_FRM);
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUM_ICON);

  curn = (u16)nbox_get_num(p_subctrl);
 
  /* jump to prog */                    
  prog_pos = get_pos_by_logic_num(curn);
  if(prog_pos == INVALIDPOS)
  {
    if(fw_find_root_by_id(ROOT_ID_DIALOG) != NULL)
    { 
      return;
    }
	p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
	update_signal();
    ui_comm_prompt_open(&dlg_data);
	if(fw_find_root_by_id(ROOT_ID_FAV_LIST) != NULL){
		ui_comm_prompt_close();
	}
	if(p_mute != NULL)
  	{
    	open_mute(0,0);
 	}
	update_signal();
    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
    return;
  }
  
  if(is_fullsrc)
  {
    prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), prog_pos);
    ui_play_prog(prog_id, FALSE);
  }
  else
  {
    fw_notify_parent(ROOT_ID_NUM_PLAY, NOTIFY_T_MSG, 
      FALSE, MSG_NUM_SELECT, (curn - 1), 0);
  }
  
  /* close */
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  return;
}
#else
static void jump_to_prog(control_t *p_cont)
{
  control_t *p_ctrl, *p_subctrl;
  control_t *p_root = NULL;
  u16 curn,curn_prog_id;
  u16 prog_id;
  control_t *p_mute;
  customer_cfg_t cus_cfg = {0};
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NUM_FRM);
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUM_TXT);

  curn = (u16)nbox_get_num(p_subctrl);
  curn_prog_id = sys_status_get_curn_group_curn_prog_id();
  get_customer_config(&cus_cfg);
  
  /* jump to prog */
  if (curn > g_prog_cnt || curn == 0)
  {
    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
    manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_9, (u32)curn_prog_id);
	p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
	update_signal();  
	
 	if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL) // already opened
	{
		ui_comm_pwdlg_close();
	}	
	ui_comm_prompt_open(&dlg_data);
	if(fw_find_root_by_id(ROOT_ID_FAV_LIST) != NULL){
		ui_comm_prompt_close();
	}
	if(p_mute != NULL)
  	{
    	open_mute(0,0);
 	}
	update_signal();
    manage_close_menu(ROOT_ID_PROG_BAR, V_KEY_9, (u32)curn_prog_id);
    return;
  }
  
  if(is_fullsrc)
  {
    prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), curn - 1 /* base on 1*/);
    manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_9, (u32)prog_id);
  }
  else
  {
    fw_notify_parent(ROOT_ID_NUM_PLAY, NOTIFY_T_MSG, 
      FALSE, MSG_NUM_SELECT, (curn - 1), 0);
  }
  
  /* close */
  p_root = fw_find_root_by_id(ROOT_ID_NUM_PLAY);
  if(p_root != NULL)
  {
    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  }
  return;
}
#endif

static BOOL input_num(control_t *parent, u8 num, BOOL is_update)
{
  control_t *p_ctrl, *p_subctrl;
  u32 curn;

  if (g_num_play_bit < g_input_cont)
  {
    p_ctrl = ctrl_get_child_by_id(parent, IDC_NUM_FRM);
    p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUM_TXT);

    curn = nbox_get_num(p_subctrl);
    curn = curn * 10 + num;
    nbox_set_num_by_dec(p_subctrl, curn);

    g_num_play_bit++;
    if (is_update)
    {
      ctrl_paint_ctrl(p_subctrl, TRUE);
    }

    return TRUE;
  }

  return FALSE;
}


u16 num_play_cont_keymap(u16 key);

RET_CODE num_play_cont_proc(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2);

RET_CODE open_num_play(u32 para1, u32 para2)
{
  control_t *p_cont;
#ifndef SPT_SUPPORT
  control_t *p_ctrl, *p_subctrl;
#endif
 u32 max_number = 0;
u16 num_pos_x = 0;

 switch(g_input_cont)
  {
    case 2:
    max_number = 99;
    case 3:
      max_number = 999;
     break;
    case 4:
      max_number = 9999;
     break;
     default:
      max_number = 999;
      break;
    
  }

  // init
  g_num_play_bit = 0;
  if ((g_prog_cnt = db_dvbs_get_count(ui_dbase_get_pg_view_id())) == 0)
  {
    return ERR_FAILURE;
  }

  is_fullsrc = ui_is_fullscreen_menu(fw_get_focus_id());
  OS_PRINTF("is fullsrc[%d]\n", is_fullsrc);
   if(!is_fullsrc)
  {
       return ERR_FAILURE;
    }


#ifndef SPT_SUPPORT

    p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                    NUM_PLAY_CONT_X, NUM_PLAY_CONT_Y,
                                    NUM_PLAY_CONT_W, NUM_PLAY_CONT_H,
                                    ROOT_ID_INVALID, 0, 
                                    OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_cont, num_play_cont_keymap);
    ctrl_set_proc(p_cont, num_play_cont_proc);

    // frm
    
    if( g_pos_on_right == FALSE)
    {
        num_pos_x =  NUM_PLAY_FRM_X;
        
    }
    else
    {
        num_pos_x = NUM_PLAY_FRM_X+NUM_PLAY_CONT_W-NUM_PLAY_FRM_W;
    }
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_NUM_FRM,
                              num_pos_x, NUM_PLAY_FRM_Y,
                              NUM_PLAY_FRM_W, NUM_PLAY_FRM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_NUM_PLAY_CONT,
                    RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT);

    p_subctrl = ctrl_create_ctrl((u8 *)CTRL_NBOX, IDC_NUM_TXT,
                                 NUM_PLAY_TXT_X, NUM_PLAY_TXT_Y,
                                 NUM_PLAY_TXT_W, NUM_PLAY_TXT_H,
                                 p_ctrl, 0);
    ctrl_set_rstyle(p_subctrl, RSI_NUM_PLAY_TXT,
                    RSI_NUM_PLAY_TXT, RSI_NUM_PLAY_TXT);
    nbox_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);

    if(g_num_big_font == FALSE)
    {
      nbox_set_font_style(p_subctrl, FSI_NUM_PLAY_TXT,
                          FSI_NUM_PLAY_TXT, FSI_NUM_PLAY_TXT);
    }
    else
    {
        nbox_set_font_style(p_subctrl, FSI_NUM_PLAY_TXT_BIG,
                          FSI_NUM_PLAY_TXT_BIG, FSI_NUM_PLAY_TXT_BIG);
    }
    
    nbox_set_num_type(p_subctrl, NBOX_NUMTYPE_DEC);
    nbox_set_range(p_subctrl, 0, max_number, g_input_cont);
    input_num(p_cont, (u8)(para1 - V_KEY_0), FALSE);
#else
  p_cont = spt_load_menu(ROOT_ID_NUM_PLAY);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, num_play_cont_keymap);
  ctrl_set_proc(p_cont, num_play_cont_proc);

  input_num(p_cont, (u8)(para1 - V_KEY_0), FALSE);
#endif


#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_paint_ctrl(p_cont, FALSE);

  /* create tmr for jump */
  fw_tmr_create(ROOT_ID_NUM_PLAY, MSG_SELECT, 5000, FALSE);

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  return SUCCESS;
}


static RET_CODE on_input_num(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  u8 num = (u8)(MSG_DATA_MASK & msg);
  input_num(p_ctrl, num, TRUE);
  return SUCCESS;
}

static RET_CODE on_select(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
 #ifdef LCN_SWITCH
  jump_to_prog_with_logic_num(p_ctrl);
 #else
  jump_to_prog(p_ctrl);
 #endif
  return SUCCESS;
}


static RET_CODE on_exit_num_play(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  fw_tmr_destroy(ROOT_ID_NUM_PLAY, MSG_SELECT);
  return ERR_NOFEATURE;
}
static RET_CODE exit_to_mainmenu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_pic_stop();

  return SUCCESS;
}

BEGIN_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
  ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
  ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
  ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
  ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
  ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
  ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
  ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
  ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
  ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
END_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(num_play_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_TO_MAINMENU, exit_to_mainmenu)
  ON_COMMAND(MSG_NUMBER, on_input_num)
  ON_COMMAND(MSG_SELECT, on_select)
  ON_COMMAND(MSG_EXIT, on_exit_num_play)
END_MSGPROC(num_play_cont_proc, ui_comm_root_proc)


