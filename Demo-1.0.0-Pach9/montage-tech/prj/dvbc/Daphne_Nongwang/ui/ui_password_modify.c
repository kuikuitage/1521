/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_channel_warn.h"
#include "ui_password_modify.h"
#include "ui_comm_root.h"

enum epg_local_msg
{
  MSG_CONFIRM = MSG_LOCAL_BEGIN + 425,
};

enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,
  IDC_BRIEF,
  IDC_POWER_ON_PASSWORD,
  IDC_LOCK_CHANNEL_PASSWORD,

  IDC_POWER_OLD_PSW,
  IDC_POWER_NEW_PSW,
  IDC_POWER_NEW_PSW_CONFIRM,
  IDC_LOCK_OLD_PSW,
  IDC_LOCK_NEW_PSW,
  IDC_LOCK_NEW_PSW_CONFIRM,

  IDC_MENU_LOCK,
  IDC_CHAN_LOCK,
  IDC_NEW,
  IDC_CONFIRM,
  IDC_PWD_MENU,
};


static BOOL is_save = FALSE;
u16 password_modify_cont_keymap(u16 key);
RET_CODE password_modify_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE password_modify_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 password_modify_pwdedit_keymap(u16 key);
RET_CODE password_modify_pwdedit_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static u16 password_modify_pwdlg_keymap(u16 key);
RET_CODE password_modify_pwdlg_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

static RET_CODE on_password_modify_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

#if 0
  if(ctrl_id == IDC_CONFIRM)
  {
    ui_comm_pwdedit_empty_value(p_child);
    
    p_new = ctrl_get_child_by_id(p_ctrl, IDC_NEW);
    ui_comm_pwdedit_empty_value(p_new);

    ui_comm_ctrl_update_attr(p_child, FALSE);
  }
#endif

  return ERR_NOFEATURE;
}

static RET_CODE on_password_modify_select_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 focus;

  /*before switch*/

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);

  /*after switch*/
  switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_MENU_LOCK:
      focus = cbox_static_get_focus(p_ctrl);
      sys_status_set_status(BS_MENU_LOCK, (BOOL)focus);
      break;
    case IDC_CHAN_LOCK:
      focus = cbox_static_get_focus(p_ctrl);
      sys_status_set_status(BS_PROG_LOCK, (BOOL)focus);    
      break;
    default:
      ;
  }
  sys_status_save();   

  return ret;
}

static RET_CODE on_password_empty_all(control_t *p_cont)
{
  control_t *p_ctrl_temp;
  u8 i = 0;
  for(i=0; i<(IDC_LOCK_NEW_PSW_CONFIRM-IDC_LOCK_OLD_PSW+1); i++)
  {
    p_ctrl_temp = ctrl_get_child_by_id(p_cont, IDC_LOCK_OLD_PSW+i);
    ui_comm_pwdedit_empty_value(p_ctrl_temp);
  }
  return SUCCESS;
}
static RET_CODE on_password_modify_confirm(control_t *p_ctrl)
{
  control_t *p_cont, *p_ctrl_temp;
  control_t  *p_active;
  u8 ctrl_id;
  u32 pow_old_psw = 0;
  u32 pow_new_psw = 0;
  u32 pow_confirm_psw = 0;
  u32 psw_len_powold = 0; 
  u32 psw_len_pownew = 0; 
  u32 psw_len_powconfirm = 0;
  u32 psw_len_powall = 0;
  u32 lock_old_psw, lock_new_psw,lock_confirm_psw;
  u32 psw_len_lockold, psw_len_locknew, psw_len_lockconfirm;
  u32 psw_len_lockall = 0;
  pwd_set_t pwd_set = {0};
  BOOL bPowerPSWModify = TRUE;
  BOOL bLockPSWSuccess = TRUE;
  
  comm_prompt_data_t modify_data_t =
	 {
	   ROOT_ID_PASSWORD_MODIFY,
	   STR_MODE_STATIC,
	   200,165,250,150,0,
	   RSC_INVALID_ID,RSC_INVALID_ID,
	   RSC_INVALID_ID,DIALOG_DEADLY,
	   3000
	 };
  
  sys_status_get_pwd_set(&pwd_set);

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);


  p_ctrl_temp = ctrl_get_child_by_id(p_cont, IDC_LOCK_OLD_PSW);
  lock_old_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  psw_len_lockold = ui_comm_is_pwdedit_enable(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_cont, IDC_LOCK_NEW_PSW);
  lock_new_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  psw_len_locknew = ui_comm_is_pwdedit_enable(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_cont, IDC_LOCK_NEW_PSW_CONFIRM);
  lock_confirm_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  psw_len_lockconfirm = ui_comm_is_pwdedit_enable(p_ctrl_temp);

  psw_len_powall = (psw_len_powold + psw_len_pownew + psw_len_powconfirm);
  psw_len_lockall = (psw_len_lockold + psw_len_locknew + psw_len_lockconfirm);
  //power on password
  if(0 == (psw_len_powall + psw_len_lockall))
  {
    modify_data_t.text_strID = IDS_WRONG_PWD;
    bPowerPSWModify = FALSE;
  }
  else if(0 == psw_len_powall)
  {
    modify_data_t.text_strID = IDS_WRONG_PWD;
    bPowerPSWModify = FALSE;  //show nothing
  }
  else if((pow_old_psw != pwd_set.psw_poweron) || (PWD_LENTH_OK !=  psw_len_powold))
  {
    modify_data_t.text_strID = IDS_WRONG_PWD;
    bPowerPSWModify = FALSE;
  }
  else if((pow_new_psw != pow_confirm_psw)||(PWD_LENTH_OK != psw_len_pownew))
  {
    modify_data_t.text_strID = IDS_CONFLICT_PWD;
    bPowerPSWModify = FALSE;
  }
  else
  {
    modify_data_t.text_strID = IDS_POSWER_PWD_SUCCESS;
    bPowerPSWModify = TRUE;
  }

  //lock password
  if(0 == (psw_len_powall + psw_len_lockall))
  {
    modify_data_t.text_strID = IDS_LOCK_PWD_WRONG;
    bLockPSWSuccess = FALSE;
  }
  else if(0 == psw_len_lockall)
  {
    modify_data_t.text_strID = IDS_LOCK_PWD_WRONG;
    bLockPSWSuccess = FALSE; //show nothing
  }
  else if((lock_old_psw != pwd_set.psw_lockchannel) || (PWD_LENTH_OK != psw_len_lockold) || (PWD_LENTH_OK != psw_len_lockconfirm))
  {
	modify_data_t.text_strID = IDS_LOCK_PWD_WRONG;
    bLockPSWSuccess = FALSE;
  }
  else if((lock_new_psw != lock_confirm_psw) || (PWD_LENTH_OK != psw_len_locknew))
  {
    modify_data_t.text_strID = IDS_LOCK_PWD_CONFLICT;
    bLockPSWSuccess = FALSE;
  }
  else
  {
    modify_data_t.text_strID = IDS_LOCK_PWD_SUCCESS;
    bLockPSWSuccess = TRUE;
  }
  
  on_password_empty_all(p_cont);
  
  if(bPowerPSWModify)
  {
    pwd_set.enable_psw_poweron = 1;
    pwd_set.psw_poweron = pow_new_psw;
  }

  if(bLockPSWSuccess)
  {
    pwd_set.enable_psw_lockchannel = 1;
    pwd_set.psw_lockchannel = lock_new_psw;
    sys_status_set_status(BS_PROG_LOCK, TRUE); 
  }
  
  sys_status_set_pwd_set(&pwd_set);
  sys_status_save();

  ui_comm_prompt_open(&modify_data_t);
  
  p_ctrl =	fw_find_root_by_id(ROOT_ID_PASSWORD_MODIFY);
   ctrl_paint_ctrl(p_ctrl, TRUE);
   
   mtos_task_delay_ms(500);
   manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
   manage_close_menu(ROOT_ID_PASSWORD_MODIFY, 0, 0);
   p_ctrl =	fw_find_root_by_id(SN_ROOT_ID_SUBMENU);
   ctrl_paint_ctrl(p_ctrl, TRUE);

   return SUCCESS;
}

static void password_set_save(void)
{
  is_save = TRUE;
}

static void password_set_no_save(void)
{
  is_save = FALSE;
}
static RET_CODE on_password_modify_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_password_modify(0, 0);
}

static RET_CODE on_password_modify_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

static RET_CODE on_password_set(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
	dialog.grade = DIALOG_INFO;
	dialog.x      = 180;
	dialog.y      = 180;
	dialog.w     = 300;
	dialog.h     = 150;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID   = IDS_SAVING_OR_NOT;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;
	ret=ui_comm_dialog_open(&dialog);
	
  if(DLG_RET_YES==ret)
  {
  	
    	password_set_save();
		is_save = FALSE;
		on_password_modify_confirm(p_ctrl);
      ui_comm_pwdlg_close();

    ret = FALSE;
  }
  else
  {
    password_set_no_save();
	manage_close_menu(ROOT_ID_PASSWORD_MODIFY, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_password_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
	manage_close_menu(ROOT_ID_PASSWORD_MODIFY, 0, 0);
	return SUCCESS;
}

RET_CODE preopen_password_modify(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_INPUT_PASSWORD,
    0,
    password_modify_pwdlg_keymap,
    password_modify_pwdlg_proc,
  };

  ui_comm_pwdlg_open(&pwdlg_data);

  return SUCCESS;
}

RET_CODE open_password_modify(u32 para1, u32 para2)
{	
  #define SN_SUB_MENU_HELP_RSC_CNT				15
  #define SN_SCREEN_WIDTH						640
  #define SN_SCREEN_HEIGHT						480
  #define SN_PASSWORD_MODIFY_X				  (SN_SCREEN_WIDTH - SN_PASSWORD_MODIFY_W)/2
  #define SN_PASSWORD_MODIFY_Y				  (SN_SCREEN_HEIGHT - SN_PASSWORD_MODIFY_H)/2
  #define SN_PASSWORD_MODIFY_W				  640
  #define SN_PASSWORD_MODIFY_H				  480
  
  //lock channel psw cont
  #define SN_PSW_MODIFY_CONT2_X				  46
  #define SN_PSW_MODIFY_CONT2_Y				  0//55
  #define SN_PSW_MODIFY_CONT2_W				  445
  #define SN_PSW_MODIFY_CONT2_H				  135
  
  #define SN_PSW_MODIFY_ITEM_X				  SN_PSW_MODIFY_CONT2_X + 14//310
  #define SN_PSW_MODIFY_ITEM_Y1				  SN_PSW_MODIFY_CONT2_Y + 32//157
  #define SN_PSW_MODIFY_ITEM_LW				  170//320
  #define SN_PSW_MODIFY_ITEM_RW				  SN_PSW_MODIFY_CONT2_W - SN_PSW_MODIFY_ITEM_LW //- 14

  //input number
  #define SN_PSW_MODIFY_ITEM_CNT                3

  #define SN_PSW_MODIFY_ITEM_V_GAP              40
  

  control_t *p_cont, *p_psw[SN_PSW_MODIFY_ITEM_CNT];//, *p_power, *p_lock
  control_t *p_lockch_cont = NULL;
  u8 i;
  u16 stxt [SN_PSW_MODIFY_ITEM_CNT] =
  { 
    IDS_OLD_PASSWORD, IDS_NEW_PASSWORD, IDS_VERIFY_PASSWORD,
  };
  u16 y;

 	static sn_comm_help_data_t sn_submenu_help_data; //help bar data
	help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
	{ 
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_LEFT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_RIGHT},
	   {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_MOVE},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	   {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	   {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	   {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
	};
  p_cont = ui_background_create(ROOT_ID_PASSWORD_MODIFY,
                             SN_PASSWORD_MODIFY_X, SN_PASSWORD_MODIFY_Y,
                             SN_PASSWORD_MODIFY_W, SN_PASSWORD_MODIFY_H,
                             IDS_PASSWORD_SET,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, password_modify_cont_keymap);
  ctrl_set_proc(p_cont, password_modify_cont_proc);

  //lock channel psw cont
  p_lockch_cont = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG2,
                              SN_PSW_MODIFY_CONT2_X, SN_PSW_MODIFY_CONT2_Y,
                              SN_PSW_MODIFY_CONT2_W, SN_PSW_MODIFY_CONT2_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_lockch_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_TIP_BOX

  y = SN_PSW_MODIFY_ITEM_Y1;
  for (i = 0; i < SN_PSW_MODIFY_ITEM_CNT; i++)
  {
    p_psw[i] = ui_comm_pwdedit_create(p_cont, (u8)(IDC_LOCK_OLD_PSW + i),
                                         SN_PSW_MODIFY_ITEM_X+40, y+50,
                                         SN_PSW_MODIFY_ITEM_LW+70,
                                         SN_PSW_MODIFY_ITEM_RW-70);
    ui_comm_pwdedit_set_static_txt(p_psw[i], stxt[i]);
    ui_comm_pwdedit_set_param(p_psw[i], 6);
    //ui_comm_pwdedit_set_mask(p_psw[i], '-', '*');
    ui_comm_ctrl_set_keymap(p_psw[i], password_modify_pwdedit_keymap);
    ui_comm_ctrl_set_proc(p_psw[i], password_modify_pwdedit_proc);

    ui_comm_ctrl_set_cont_rstyle(p_psw[i], RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_TIP_BOX_BG

    ctrl_set_related_id(p_psw[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              SN_PSW_MODIFY_ITEM_CNT) %
                             SN_PSW_MODIFY_ITEM_CNT + IDC_LOCK_OLD_PSW),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % SN_PSW_MODIFY_ITEM_CNT + IDC_LOCK_OLD_PSW));/* down */

    y += PSW_MODIFY_ITEM_H + SN_PSW_MODIFY_ITEM_V_GAP;//PSW_MODIFY_ITEM_V_GAP
    if(i == 2)
    {
      y = PSW_MODIFY_ITEM_Y2;
    }

  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

    memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

  ctrl_default_proc(p_psw[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(password_modify_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(password_modify_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(password_modify_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_password_modify_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_password_modify_cont_focus_change)  
END_MSGPROC(password_modify_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(password_modify_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_password_modify_select_change)
  ON_COMMAND(MSG_DECREASE, on_password_modify_select_change)  
END_MSGPROC(password_modify_select_proc, cbox_class_proc)

BEGIN_KEYMAP(password_modify_pwdedit_keymap, ui_comm_edit_keymap)
  ON_EVENT(V_KEY_OK, MSG_YES)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(password_modify_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(password_modify_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_YES, on_password_set)
  ON_COMMAND(MSG_BACKSPACE, on_password_exit)
END_MSGPROC(password_modify_pwdedit_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(password_modify_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_YES)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
END_KEYMAP(password_modify_pwdlg_keymap, NULL)

BEGIN_MSGPROC(password_modify_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_password_modify_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_password_modify_pwdlg_exit)
  ON_COMMAND(MSG_YES, on_password_set)
  ON_COMMAND(MSG_BACKSPACE, on_password_exit)
END_MSGPROC(password_modify_pwdlg_proc, cont_class_proc)

