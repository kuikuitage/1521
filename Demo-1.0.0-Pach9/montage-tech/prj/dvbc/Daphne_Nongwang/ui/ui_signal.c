/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"

#include "ui_mute.h"
#include "ui_ca_public.h"
#include "config_cas.h"
#if(CONFIG_CAS_ID!=CONFIG_CAS_ID_XSM)
#if((CONFIG_CAS_ID != CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_QL))
#include "ui_ca_finger.h"
#else
#include "ui_finger_print.h"
#endif
#endif
#include "ui_common.h"
#include "ui_comm_dlg.h"

#include "ui_epg.h"


enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CONTENT,
  IDC_ICON,
};

static u16 g_strID_CA = RSC_INVALID_ID;
static u16 g_strID_empty = RSC_INVALID_ID;
static u16 g_strID_sig = RSC_INVALID_ID;
static u16 g_strID_userlock = RSC_INVALID_ID;
static BOOL g_show_sig = TRUE;

u16 signal_set_cont_keymap(u16 key);
RET_CODE singal_set_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

void set_signal_flag(BOOL flag)
{
  g_show_sig = flag;
}


u16 get_singal_strid(void)
{
  u16 pgid, strid = RSC_INVALID_ID;
  u8 curn_mode = 0;
  u16 curn_group = 0;
  u16 pg_id, pg_pos = 0xFFFF;
  u32 group_context = 0;
  u8 view_id = 0xFF;
  u16 view_count = 0;
  
  pgid = sys_status_get_curn_group_curn_prog_id();
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  UI_PRINTF("view_id= %d \n",view_id);
  if(!ui_signal_is_lock())
  {
    
    if(sys_status_get_curn_prog_mode() != CURN_MODE_NONE)
    {
      strid = IDS_SYS_NO_SIGNAL;
    }
	else if((view_count == 0)&&(curn_mode == CURN_MODE_RADIO))  //
	{                                                        //
		strid = IDS_NO_RADIO_PROG;                           //
	}                                                        //
    else
    {
      strid = IDS_NO_PROG;//IDS_NO_RADIO_PROG
    }
	
  }
  else
  {
    if(pgid != INVALIDID)
    {
      u8 view_id = ui_dbase_get_pg_view_id();
      u16 pos = db_dvbs_get_view_pos_by_id(view_id, pgid);

      if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) == TRUE)
      {
        if(ui_is_chk_pwd() && (!ui_is_pass_chkpwd(pgid)))
        {
          strid = IDS_LOCK;
        }
      }
    }
  }
  return strid;
}

void clear_signal_strid(u16 *str)
{
  *str = RSC_INVALID_ID;
}

void set_signal_strid(u16 *str, u16 id)
{
  *str = id;
}


u16 get_message_strid()
{
  u16 ret = RSC_INVALID_ID;

  if(g_strID_empty != RSC_INVALID_ID)
  {
    ret = g_strID_empty;
  }
  else if(g_strID_userlock != RSC_INVALID_ID)
  {
    ret = g_strID_userlock;
  }
  else if(g_strID_sig != RSC_INVALID_ID)
  {
    ret = g_strID_sig;
  }
  else if(g_strID_CA != RSC_INVALID_ID)
  {
    ret = g_strID_CA;
  }
  
  return ret;
}


static void open_signal(u16 content)
{
  control_t *p_content, *p_icon, *p_cont = NULL;
  u16 left, top, width, height;

  if((ui_is_fullscreen_menu(fw_get_focus_id())) && (g_strID_userlock !=IDS_LOCK))//YYF_2014/01/12
  {
    p_cont = fw_create_mainwin(ROOT_ID_SIGNAL,
                               SIGNAL_CONT_FULL_X+29, SIGNAL_CONT_FULL_Y-8,
                               SIGNAL_CONT_FULL_W-55, SIGNAL_CONT_FULL_H,
                               ROOT_ID_INVALID, 0,
                               OBJ_ATTR_INACTIVE, 0);
    if(p_cont == NULL)
    {
      return;
    }
    ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);
	ctrl_set_keymap(p_cont, signal_set_cont_keymap);
    ctrl_set_proc(p_cont, singal_set_cont_proc);
    
    //content
    p_content = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                           SIGNAL_CONTENT_FULL_X+40, (SIGNAL_CONT_FULL_H-SIGNAL_CONTENT_FULL_H)/2,
                           SIGNAL_CONTENT_FULL_W-110, SIGNAL_CONTENT_FULL_H,
                           p_cont, 0);
    ctrl_set_rstyle(p_content, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_content, FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);
    text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_content, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_content, content);

    
    //icon
    p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_ICON,
                           0, (SIGNAL_CONT_FULL_H-40)/2,
                           60, 40,
                           p_cont, 0);
    bmap_set_content_by_id(p_icon, IM_SN_ICON_WARNING_2);
  }
  else if(manage_get_preview_rect(fw_get_focus_id(),
                                  &left, &top,
                                  &width, &height))
  {
    p_cont = fw_create_mainwin(ROOT_ID_SIGNAL,
                               (u16)(left + SIGNAL_CONT_PRE_VGAP),
                               (u16)(top + (height - SIGNAL_CONT_PREVIEW_H) / 2),
                               width - SIGNAL_CONT_PRE_VGAP*2, SIGNAL_CONT_PREVIEW_H,
                               ROOT_ID_INVALID, 0,
                               OBJ_ATTR_INACTIVE, 0);
    if(p_cont == NULL)
    {
      return;
    }
    ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);//RSI_SIGNAL_TXT

    p_content = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CONTENT,
                           SIGNAL_TXT_PREVIEW_X, SIGNAL_TXT_PREVIEW_Y,
                           width - SIGNAL_CONT_PRE_VGAP*2, SIGNAL_TXT_PREVIEW_H,
                           p_cont, 0);
    ctrl_set_rstyle(p_content, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);//RSI_SIGNAL_TXT
    text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_content, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
    text_set_content_type(p_content, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_content, content);
  }
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(p_cont != NULL)
  {
    ctrl_paint_ctrl(p_cont, FALSE);
  }

  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

}


void close_signal(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_SIGNAL);
  if(ui_is_finger_show() && ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    UI_PRINTF("[UI_Signal]redraw finger message \n");
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
    open_finger(0, 0); // don't remove!!!
    #endif
  }
}

u16 update_signal_check(u16 strid)
{
  u16 strid1;
  menu_attr_t *p_attr;
  
  p_attr = manage_get_curn_menu_attr();
  if(!ui_is_fullscreen_menu(p_attr->root_id)
    && !ui_is_preview_menu(p_attr->root_id)
    && !ui_is_popup_menu(p_attr->root_id))    
  {
    return 0;
  }

  strid1 = get_singal_strid();

  if(strid1 == RSC_INVALID_ID)  /* it should be closed */
  {
    close_signal();
  }
  return strid1;
}



static void update_signal_userlock_message(void)
{
  u16 pgid;

  pgid = sys_status_get_curn_group_curn_prog_id();

  if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
  {
    clear_signal_strid(&g_strID_empty);
  }
  else
  {
    set_signal_strid(&g_strID_empty, IDS_NO_PROG);
  }

  if(ui_signal_is_lock())
  {
    clear_signal_strid(&g_strID_sig);
  }
  else
  {
    if(g_show_sig)
    {
      set_signal_strid(&g_strID_sig, IDS_SYS_NO_SIGNAL);
    }
  }

  if(get_message_strid() == IDS_LOCK && ui_is_pass_chkpwd(pgid))
  {
    clear_signal_strid(&g_strID_userlock);
  }
  else
  {
    if(pgid != INVALIDID)
    {
      u8 view_id = ui_dbase_get_pg_view_id();
      u16 pos = db_dvbs_get_view_pos_by_id(view_id, pgid);

      if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) == TRUE)
      {
        if(ui_is_chk_pwd() && (!ui_is_pass_chkpwd(pgid)))
        {
          set_signal_strid(&g_strID_userlock, IDS_LOCK);
        }
        else
        {
          if(get_message_strid() == IDS_LOCK)
          {
            clear_signal_strid(&g_strID_userlock);
          }
        }
      }
    }
  }
}

static void refresh_signal()
{
  control_t *p_cont, *p_content;
  u16 strid;
  menu_attr_t *p_attr;
 
  
  p_attr = manage_get_curn_menu_attr();
  if(!ui_is_fullscreen_menu(p_attr->root_id)
    && !ui_is_preview_menu(p_attr->root_id)
    && !ui_is_popup_menu(p_attr->root_id))    
  {
    return;
  }

  strid = get_message_strid();

  /*!
  for bug 18945 fixed:unexpected showing of channel lock when enter mosaic menu*/
  if((strid == g_strID_userlock) && (fw_get_focus_id() == ROOT_ID_MOSAIC)) 
  {
    return;
  }
  
  OS_PRINTF("strid = %d\n", strid);
  
  if(strid == RSC_INVALID_ID)
  {
  #if 0 //(CONFIG_CAS_ID == CONFIG_CAS_ID_TF)
     /*****test ****/
   control_t * p_ctrl = NULL;
   u32 ca_last_msg = CAS_S_CLEAR_DISPLAY;
   
     ca_last_msg = ui_get_ca_last_msg();

    if(CAS_S_CLEAR_DISPLAY != ca_last_msg)
    { 
        p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
        if(NULL != p_ctrl)
        {
            ctrl_process_msg(p_ctrl, MSG_RECOVER_CA_PROMPT, 0, ca_last_msg);
        }
    }
    else
    {
      close_signal();
    }
     /**********/
     #else
    close_signal();
    #endif
  }
  else
  {
    if((p_cont = fw_find_root_by_id(ROOT_ID_SIGNAL)) != NULL)
    {
      p_content = ctrl_get_child_by_id(p_cont, IDC_CONTENT);
      if(p_content == NULL)
      {
        return;
      }

      if((u16)text_get_content(p_content) != strid)
      {
        text_set_content_by_strid(p_content, strid);
        ctrl_paint_ctrl(p_cont, TRUE);
      }
    }
    else
    {
      if((p_cont = fw_find_root_by_id(ROOT_ID_PASSWORD)) != NULL)
      {
        return ;
      }
      else
      {
        open_signal(strid);
      }
    }
  }
}

void update_ca_message(u16 strid)
{
  OS_PRINTF("update signal\n");
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
	if(IDS_E00 == strid 
		|| IDS_E23 == strid)
	{ 
		if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL && get_message_strid() == IDS_E31) 
		{
			ui_comm_pwdlg_close();
		}
		clear_signal_strid(&g_strID_CA);
	}
	else
	{
		set_signal_strid(&g_strID_CA, strid);
	}

	if(strid != IDS_E00)
	{
		refresh_signal();
	}
#else
  set_signal_strid(&g_strID_CA, strid);
  
  refresh_signal();
#endif

  return ;
}

void update_signal(void)
{
  update_signal_userlock_message();

  refresh_signal();
}

void clean_ca_prompt(void)
{
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  clean_cas_preview();
  close_ca_finger_menu();
  #endif
}

static RET_CODE on_singal_exit (control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
   close_signal();
   return SUCCESS;
}

BEGIN_KEYMAP(signal_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_BACK,MSG_BACKSPACE)
END_KEYMAP(signal_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(singal_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_BACKSPACE,on_singal_exit)
END_MSGPROC(singal_set_cont_proc, ui_comm_root_proc)


