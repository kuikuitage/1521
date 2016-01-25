/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#include "ui_common.h"

enum comm_pwdlg_idc
{
	IDC_TXT = 1,
	IDC_EDIT,
	IDC_PROMPT,
	IDC_EDIT_BG
};

struct pwdlg_int_data
{
	u8  type;               // PWDLG_T_
	u8  total_bit;          // total bit
	u8  input_bit;          // input bit
	u32 total_value;        // total value
	u32 input_value;        // input value
};

static struct pwdlg_int_data g_pwdlg_idata;
static cas_pin_modify_t pin_veriify;

u16 pwdlg_edit_keymap(u16 key);

RET_CODE pwdlg_edit_proc(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2);

BOOL ui_comm_pwdlg_open(comm_pwdlg_data_t* p_data)
{
	// create window
	#define SN_PWDLG_W                       250
	#define SN_PWDLG_H                       135

	//password bar title
	#define SN_PWDLG_TXT1_L                  0
	#define SN_PWDLG_TXT1_T                  0
	#define SN_PWDLG_TXT1_W                  SN_PWDLG_W
	#define SN_PWDLG_TXT1_H                  50

	//password input backgound
	#define SN_PWDLG_EDIT_BG_L               SN_PWDLG_TXT1_L
	#define SN_PWDLG_EDIT_BG_R               SN_PWDLG_TXT1_T + SN_PWDLG_TXT1_H
	#define SN_PWDLG_EDIT_BG_W               SN_PWDLG_W
	#define SN_PWDLG_EDIT_BG_H               SN_PWDLG_H - SN_PWDLG_TXT1_H

	//password input
	#define SN_PWDLG_EDIT_L                  50
	#define SN_PWDLG_EDIT_R                  56
	#define SN_PWDLG_EDIT_W                  SN_PWDLG_W - 2*SN_PWDLG_EDIT_L
	#define SN_PWDLG_EDIT_H                  31



	control_t *p_cont, *p_title, *p_edit, *p_edit_bg;
	pwd_set_t pwd_set;
	sys_status_get_pwd_set(&pwd_set);

	if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL) // already opened
	{
		UI_PRINTF("UI: already open a password dialog, force close it! \n");
		ui_comm_pwdlg_close();
	}

	// init idata
	g_pwdlg_idata.type = PWDLG_T_COMMON;
	g_pwdlg_idata.total_bit = PWD_LENGTH_COMMON;
	g_pwdlg_idata.input_bit = 0;
	g_pwdlg_idata.input_value = 0;
	if(p_data->pwd_class == 0)
	{
		g_pwdlg_idata.total_value = pwd_set.psw_poweron;
	}
	else if(p_data->pwd_class == PWDLG_T_IPPV)
	{
		g_pwdlg_idata.type = PWDLG_T_IPPV;
		g_pwdlg_idata.total_bit = PWD_LENGTH_IPPV;
	}
	else
	{
		g_pwdlg_idata.total_value = pwd_set.psw_lockchannel;
	}

	// chk the parent
	if (p_data->parent_root == ROOT_ID_INVALID)
	{
		p_data->parent_root = fw_get_focus_id();
	}

	// create window
	p_cont = fw_create_mainwin(ROOT_ID_PASSWORD,
	                              p_data->left = 216, p_data->top = 183, 
	                              SN_PWDLG_W, SN_PWDLG_H,
	                              //p_data->parent_root = ROOT_ID_SENIOR_SUBMENU, 0,
								  p_data->parent_root, 0,
	                              OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return FALSE;
	}

	ctrl_set_rstyle(p_cont,
	              RSI_DIG_BG,
	              RSI_DIG_BG,
	              RSI_DIG_BG);//RSI_PWDLG_CONT
	ctrl_set_keymap(p_cont, p_data->keymap);
	ctrl_set_proc(p_cont, p_data->proc);

	//password bar title
	p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TXT,
	                         SN_PWDLG_TXT1_L, SN_PWDLG_TXT1_T,
	                         SN_PWDLG_TXT1_W, SN_PWDLG_TXT1_H,
	                         p_cont, 0);
	ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_DLG_TITLE
	text_set_font_style(p_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
	text_set_align_type(p_title, STL_BOTTOM| STL_VCENTER);
	text_set_content_type(p_title, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_title, p_data->strid);

	//password input backgound
	p_edit_bg = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_EDIT_BG,
	                         SN_PWDLG_EDIT_BG_L, SN_PWDLG_EDIT_BG_R,
	                         SN_PWDLG_EDIT_BG_W, SN_PWDLG_EDIT_BG_H,
	                         p_cont, 0);
	ctrl_set_rstyle(p_edit_bg, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_DLG_TITLE

	//password input
	p_edit = ctrl_create_ctrl((u8 *)CTRL_EBOX, IDC_EDIT,
	                        SN_PWDLG_EDIT_L, SN_PWDLG_EDIT_R,
	                        SN_PWDLG_EDIT_W, SN_PWDLG_EDIT_H,
	                        p_cont, 0);
	ctrl_set_rstyle(p_edit, RSI_OLD_PSW,
	              RSI_OLD_PSW, RSI_OLD_PSW);

	ctrl_set_keymap(p_edit, pwdlg_edit_keymap);
	ctrl_set_proc(p_edit, pwdlg_edit_proc);

	ebox_set_worktype(p_edit, EBOX_WORKTYPE_HIDE);
	ebox_set_align_type(p_edit, STL_CENTER | STL_VCENTER);
	ebox_set_font_style(p_edit, FSI_PWDLG_TXT,
	                  FSI_PWDLG_TXT, FSI_PWDLG_TXT);

	// Fix bug 3056
	if(p_data->pwd_class == PWDLG_T_IPPV)
	{ 
		ebox_set_maxtext(p_edit, PWD_LENGTH_IPPV);
	}
	else
	{
		ebox_set_maxtext(p_edit, 6);
	}
	ebox_set_hide_mask(p_edit, '-', '*');

#if 0
	p_help = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_PROMPT,
	                        PWDLG_HELP_L, PWDLG_HELP_T,
	                        PWDLG_HELP_W, PWDLG_HELP_H,
	                        p_cont, 0);
	ctrl_set_rstyle(p_help, RSI_IGNORE,
	              RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_help, FSI_PWDLG_HELP,
	                  FSI_PWDLG_HELP, FSI_PWDLG_HELP);
	text_set_align_type(p_help, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_help, TEXT_STRTYPE_UNICODE);
	str_asc2uni("0~9:", help_text);
	gui_get_string(IDS_INPUT, uni_str, 32);
	uni_strcat(help_text, uni_str, 64);
	str_asc2uni("      Menu/Cancle:", uni_str);
	uni_strcat(help_text, uni_str, 64);
	gui_get_string(IDS_EXIT, uni_str, 32);
	uni_strcat(help_text, uni_str, 64);
	text_set_content_by_unistr(p_help, help_text);
#endif

	/* enter edit status */
	ctrl_default_proc(p_edit, MSG_GETFOCUS, 0, 0);
	ebox_enter_edit(p_edit);

#ifdef SPT_DUMP_DATA
	spt_dump_menu_data(p_cont);
#endif

	ctrl_paint_ctrl(p_cont, FALSE);
	return TRUE;

}


void ui_comm_pwdlg_close(void)
{
	u8 root_id;
	manage_close_menu(ROOT_ID_PASSWORD, 0, 0);
	root_id = fw_get_focus_id();
	if((root_id == ROOT_ID_EPG) || (root_id == ROOT_ID_PROG_LIST) || (root_id == ROOT_ID_FAV_LIST))
    {
		ctrl_paint_ctrl(fw_find_root_by_id(root_id),TRUE);
	} 
}

static RET_CODE on_ca_ippv_pin_verify(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{   
	#if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
	control_t *p_parent, *p_txt;

	u16 pin_spare_num = 0;
	cas_pin_modify_t *ca_pin_info = NULL;


	comm_prompt_data_t dlg_data_t =
   {
	 ROOT_ID_PASSWORD,
	 STR_MODE_STATIC,
	 216,183,250,150,20,
	 RSC_INVALID_ID,RSC_INVALID_ID,
	 RSC_INVALID_ID,DIALOG_DEADLY,
	 3000
   };

	ca_pin_info = (cas_pin_modify_t *)para2;
	p_parent = ctrl_get_parent(p_edit);

	if(para1 == 2)
	{
		ctrl_process_msg(p_parent, MSG_CORRECT_PWD, 0, 0);
		return SUCCESS;
	}
	else if(para1 == 18)
	{
		dlg_data_t.text_strID = IDS_CA_PARAME_SET_ERROR;
	}
	else if(para1 == 1)
	{
		dlg_data_t.text_strID = IDS_CAS_E_IC_CMD_FAIL;
	}
	else if(para1 == 9)
	{
		dlg_data_t.text_strID = IDS_CA_PASSWORD_PIN_NUMB;
	}
	else if(para1 == 7) 
	{
		pin_spare_num = (u16)ca_pin_info->pin_spare_num;
		switch(pin_spare_num){
		case 1:
			dlg_data_t.text_strID = IDS_CA_PIN_ERROR_ONE_CHANCES_LEFT;
			break;
		case 2:
			dlg_data_t.text_strID = IDS_CA_PIN_ERROR_TWO_CHANCES_LEFT;
			break;
        }
	}

	ui_comm_prompt_open(&dlg_data_t);
	ebox_empty_content(p_edit);
	g_pwdlg_idata.input_bit = 0;
	g_pwdlg_idata.input_value = 0;
		// update view
	p_txt = ctrl_get_child_by_id(p_parent, IDC_TXT);
	MT_ASSERT(p_txt != NULL);
	text_set_content_by_strid(p_txt, IDS_INPUT_PASSWORD);
	ctrl_paint_ctrl(p_txt, TRUE);

	#else
	control_t *p_parent, *p_txt;
	RET_CODE ret;
	u16 pin_spare_num = 0;
	u8 asc_str[64];
	u16 uni_num[64];
	cas_pin_modify_t *ca_pin_info = NULL;

	comm_dlg_data_t dlg_data=
	{
		ROOT_ID_PASSWORD,
		DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
		DLG_STR_MODE_EXTSTR,
		160, 100,
		(COMM_DLG_W + 50),(COMM_DLG_WITHTEXT_H - 50),
		RSC_INVALID_ID,
		RSC_INVALID_ID,
		RSC_INVALID_ID,
		RSC_INVALID_ID,
		60000,
	};
	ca_pin_info = (cas_pin_modify_t *)para2;
	p_parent = ctrl_get_parent(p_edit);

	if(para1 == 2)
	{
		ctrl_process_msg(p_parent, MSG_CORRECT_PWD, 0, 0);
		return SUCCESS;
	}
	else if(para1 == 18)
	{
		gui_get_string(IDS_CA_PARAME_SET_ERROR,uni_num,64);
	}
	else if(para1 == 1)
	{
		gui_get_string(IDS_CAS_E_IC_CMD_FAIL,uni_num,64);
	}
	else if(para1 == 9)
	{
		gui_get_string(IDS_CA_PASSWORD_PIN_NUMB,uni_num,64);
	}
	else if(para1 == 7) 
	{
		pin_spare_num = (u16)ca_pin_info->pin_spare_num;
		if(LANGUAGE_CHINESE == rsc_get_curn_language(gui_get_rsc_handle()))
			sprintf((char *)asc_str, "PIN码错误, 还剩%d次机会", pin_spare_num);
		else
			sprintf((char *)asc_str, "PIN ERROR,%d chances left", pin_spare_num);
		
		gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,63);
	}

	dlg_data.text_content = (u32)&uni_num;
	ret = ui_comm_dlg_open(&dlg_data);
	if(ret == DLG_RET_YES)
	{
		ebox_empty_content(p_edit);
		g_pwdlg_idata.input_bit = 0;
		g_pwdlg_idata.input_value = 0;
		// update view
		p_txt = ctrl_get_child_by_id(p_parent, IDC_TXT);
		MT_ASSERT(p_txt != NULL);
		text_set_content_by_strid(p_txt, IDS_INPUT_PASSWORD);
		ctrl_paint_ctrl(p_txt, TRUE);
	}
	#endif
	return SUCCESS;

}
static RET_CODE on_pwdlg_char(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
	// process MSG_CHAR
	ebox_class_proc(p_edit, msg, para1, para2);
    if(g_pwdlg_idata.input_bit < 6){
		g_pwdlg_idata.input_bit++;
		g_pwdlg_idata.input_value = g_pwdlg_idata.input_value * 10 +
	              (msg & MSG_DATA_MASK) - '0';
    }
	return SUCCESS;
}

static RET_CODE on_pwdlg_verify(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
	control_t *p_parent, *p_txt;
	comm_prompt_data_t pwdlg_data_t = 
	{
	   ROOT_ID_PASSWORD,
	   STR_MODE_STATIC,
	   216,183,250,135,0,
	   RSC_INVALID_ID,RSC_INVALID_ID,
	   RSC_INVALID_ID,DIALOG_DEADLY,
	   2000
	 };
	if(g_pwdlg_idata.type == PWDLG_T_IPPV)
	{
	#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
		if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input 
		{
			pin_veriify.old_pin[0]=(u8)(g_pwdlg_idata.input_value/100000);
			pin_veriify.old_pin[1]=(u8)((g_pwdlg_idata.input_value/10000)%10);
			pin_veriify.old_pin[2]=(u8)(((g_pwdlg_idata.input_value/1000)%100)%10);
			pin_veriify.old_pin[3]=(u8)((((g_pwdlg_idata.input_value/100)%1000)%100)%10);
			pin_veriify.old_pin[4]=(u8)(((((g_pwdlg_idata.input_value/10)%10000)%1000)%100)%10);
			pin_veriify.old_pin[5]=(u8)(((((g_pwdlg_idata.input_value%100000)%10000)%1000)%100)%10);
			ui_ca_get_info((u32)CAS_CMD_PIN_VERIFY, (u32)&pin_veriify ,0);
		}
		else{
			ebox_empty_content(p_edit);
			g_pwdlg_idata.input_bit = 0;
			g_pwdlg_idata.input_value = 0;
			pwdlg_data_t.text_strID = IDS_PASSWORD_SIX_DIGIT;
			ui_comm_prompt_open(&pwdlg_data_t);
		}
	#else
		if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input 
		{
			pin_veriify.old_pin[0]=(u8)((g_pwdlg_idata.input_value & 0x00ff0000)>>16);
			pin_veriify.old_pin[1]=(u8)((g_pwdlg_idata.input_value & 0x0000ff00)>>8);
			pin_veriify.old_pin[2]=(u8)(g_pwdlg_idata.input_value & 0x000000ff);
			ui_ca_get_info((u32)CAS_CMD_PIN_VERIFY, (u32)&pin_veriify ,0);
		}
		else{
			ebox_empty_content(p_edit);
			g_pwdlg_idata.input_bit = 0;
			g_pwdlg_idata.input_value = 0;
			pwdlg_data_t.text_strID = IDS_PASSWORD_SIX_DIGIT;
			ui_comm_prompt_open(&pwdlg_data_t);
		}
	#endif
	}	
	else
	{
		if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input ?
		{
			p_parent = ctrl_get_parent(p_edit);
			if ((g_pwdlg_idata.input_value == g_pwdlg_idata.total_value) && (fw_find_root_by_id(ROOT_ID_SENIOR_SUBMENU) == NULL))
			{
				ctrl_process_msg(p_parent, MSG_CORRECT_PWD, 0, 0);
			}
			else if(g_pwdlg_idata.input_value == 0)
			{
				ctrl_process_msg(p_parent, MSG_PRESET_PROG, 0, 0);
			}
			else
			{
				ctrl_process_msg(p_parent, MSG_WRONG_PWD, 0, 0);
				// reset & reinput
				ebox_empty_content(p_edit);
				g_pwdlg_idata.input_bit = 0;
				g_pwdlg_idata.input_value = 0;
				// update view
				p_txt = ctrl_get_child_by_id(p_parent, IDC_TXT);
				MT_ASSERT(p_txt != NULL);
				pwdlg_data_t.text_strID = IDS_INPUT_PASSWORD_ERROR;
				ui_comm_prompt_open(&pwdlg_data_t);
			}
		}
		else{
			ebox_empty_content(p_edit);
			g_pwdlg_idata.input_bit = 0;
			g_pwdlg_idata.input_value = 0;
			pwdlg_data_t.text_strID = IDS_PASSWORD_SIX_DIGIT;
			ui_comm_prompt_open(&pwdlg_data_t);
		}
	}
	return SUCCESS;
}

BEGIN_KEYMAP(pwdlg_edit_keymap, NULL)
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
	ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(pwdlg_edit_keymap, NULL)

BEGIN_MSGPROC(pwdlg_edit_proc, ebox_class_proc)
	ON_COMMAND(MSG_CHAR, on_pwdlg_char)
	ON_COMMAND(MSG_CA_PIN_VERIFY_INFO, on_ca_ippv_pin_verify)
	ON_COMMAND(MSG_YES, on_pwdlg_verify)
END_MSGPROC(pwdlg_edit_proc, ebox_class_proc)




