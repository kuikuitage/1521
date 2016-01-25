/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/

#include "ui_common.h"

enum comm_dlg_idc
{
  IDC_BTN_YES = 1,
  IDC_BTN_NO,
  IDC_TITLE_BG,
  IDC_TITLE_TEXT,
  IDC_TITLE_ICON,
  IDC_TEXT,
  IDC_CONTENT,
};
enum prompt_dlg_idc
{
  PROMPT_IDC_BG = 1,
  PROMPT_IDC_ICON,
  PROMPT_IDC_TEXT,
  SN_IDC_SEARCH_PROMPT,
  SN_IDC_SEARCH_RADIO,
  SN_IDC_RADIO_PROMPT,
};

enum comm_dialog_idc
{
	DIALOG_IDC_BTN_YES = 1,
	DIALOG_IDC_BTN_NO,
	DIALOG_IDC_BG,
	DIALOG_IDC_TEXT,
	DIALOG_IDC_ICON,
};
enum prompt_dlg_msg
{
  MSG_PROMPT_TIME_OUT = MSG_LOCAL_BEGIN + 560
};

enum comm_dlg_msg
{
  MSG_DLG_TIME_OUT = MSG_LOCAL_BEGIN + 550
};
static dlg_ret_t g_dlg_ret;

u16 comm_dlg_keymap(u16 key);
void ui_comm_prompt_close(void)
{
	fw_tmr_destroy(ROOT_ID_PROMPT, MSG_PROMPT_TIME_OUT);
	manage_close_menu(ROOT_ID_PROMPT, 0, 0);
}
static u32 conver_prompt_bmp_id(const comm_prompt_data_t *p_data)
{
	if (p_data->icon_id != 0)
		return p_data->icon_id;

	switch (p_data->grade)
	{
		case PROMPT_INFO:
		case PROMPT_WARNING:
		case PROMPT_ERR:
		case PROMPT_DEADLY:
			return IM_SN_ICON_WARNING_2;
		default :
			return IM_SN_ICON_WARNING_2;
	}
		
}
static u32 conver_bmp_id(const comm_dialog_data_t *p_data)
{
	if (p_data->icon_id != 0)
		return p_data->icon_id;

	switch (p_data->grade)
	{
		case DIALOG_INFO:
		case DIALOG_WARNING:
		case DIALOG_ERR:
		case DIALOG_DEADLY:
			return IM_SN_ICON_WARNING_2;
		default :
			return IM_SN_ICON_WARNING_2;
	}
		
}

static u32 conver_bmp_id2(const comm_prompt_data *p_data)
{
	if (p_data->icon_id != 0)
		return p_data->icon_id;

	switch (p_data->grade)
	{
		case PROMPT_INFO:
		case PROMPT_WARNING:
		case PROMPT_ERR:
		case PROMPT_DEADLY:
			return IM_SN_ICON_WARNING_2;
		default :
			return IM_SN_ICON_WARNING_2;
	}
		
}

RET_CODE comm_dlg_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#define DO_DLG_FUNC(func) \
  if (func != NULL) (*func)(); 

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data)
{
  control_t *p_cont;
  control_t *p_btn, *p_txt;//, *p_icon;
  u16 i, cnt, x, y;
  u8 title_type, title_mode, text_type, text_mode;
  u16 btn[2] = { IDS_YES, IDS_NO };
  dlg_ret_t ret;

  if(fw_find_root_by_id(ROOT_ID_DIALOG) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
  }

  p_cont = p_btn = p_txt = NULL;
  x = y = cnt = 0;

  title_type = p_data->title_style & 0x0F;
  title_mode = p_data->title_style & 0xF0;

  text_type = p_data->text_style & 0x0F;
  text_mode = p_data->text_style & 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, ROOT_ID_DIALOG,
                            p_data->x, p_data->y, p_data->w, p_data->h+10,
                            NULL, 0);
  ctrl_set_attr(p_cont, (u8)(title_type == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap);
  ctrl_set_proc(p_cont, comm_dlg_proc);

  //title background
    p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE_BG,
                             0, 0,
                             p_data->w ,
                             (u16)DLG_TITLE_H+10,
                             p_cont, 0);
    ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_txt, FSI_DLG_TITLE, FSI_DLG_TITLE, FSI_DLG_TITLE);

  //title:text
  if(title_mode != DLG_STR_MODE_NULL)
  {
    p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE_TEXT,
                             25, 20,
                             p_data->w - 35,
                             (u16)DLG_TITLE_H,
                             p_cont, 0);
    
    ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_txt, FSI_DLG_TITLE, FSI_DLG_TITLE, FSI_DLG_TITLE);
  }
  
  if (title_mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->title_strID);
  }
  else if (title_mode == DLG_STR_MODE_EXTSTR)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_txt, (u16*)p_data->title_content);
  }

  //textÕýÎÄ
  
  if(text_mode != DLG_STR_MODE_NULL)
  {
    p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TEXT,
                             DLG_TEXT_X, DLG_TITLE_H+30,
                             p_data->w - 2*DLG_TEXT_X,
                             (u16)DLG_TEXT_H,
                             p_cont, 0);

    text_set_font_style(p_txt, FSI_DLG_TITLE, FSI_DLG_TITLE, FSI_DLG_TITLE);
  }
  
  if (text_mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->text_strID);
  }
  else if (text_mode == DLG_STR_MODE_EXTSTR)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_txt, (u16*)p_data->text_content);
  }
  
  //button
  switch (title_type)
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y+15, DLG_BTN_W, 25, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    SN_RSI_BNT_TEXT_SH, SN_RSI_BNT_TEXT_HL, SN_RSI_BNT_TEXT_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_N, FSI_DLG_BTN_HL, FSI_DLG_BTN_N);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn,
                              (u16)(cnt == 1 ? IDS_OK : btn[i]));

    ctrl_set_related_id(p_btn,
                        (u8)((i - 1 + cnt) % cnt + 1), /* left */
                        0,                             /* up */
                        (u8)((i + 1) % cnt + 1),       /* right */
                        0);                            /* down */

    x += p_data->w / 3;
  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  // paint
  if (cnt > 0)
  {
    p_btn = ctrl_get_child_by_id(p_cont,IDC_BTN_YES);
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_data->dlg_tmout != 0)
  {
    ret = fw_tmr_create(ROOT_ID_DIALOG, MSG_DLG_TIME_OUT, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (title_type == DLG_FOR_SHOW)
  {
    //ret = DLG_RET_NULL;
    fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }
  else
  {
    fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }

  return ret;
}



void ui_comm_dlg_close(void)
{
	fw_tmr_destroy(ROOT_ID_DIALOG, MSG_DLG_TIME_OUT);
	manage_close_menu(ROOT_ID_DIALOG, 0, 0);
}


static void fill_dlg_data(comm_dlg_data_t *p_data, u8 title_style, u16 title_str,
                          u8 text_style, u16 text_str, rect_t *p_dlg_rc, u32 tm_out)
{
  p_data->parent_root = 0;

  if((title_style&0xF0) == DLG_STR_MODE_STATIC)
  {
    p_data->title_strID= title_str; 
  }
  else if((title_style&0xF0) == DLG_STR_MODE_EXTSTR)
  {
    p_data->title_content = title_str; 
  }
  else
  {
    p_data->title_strID= RSC_INVALID_ID; 
    p_data->title_content = RSC_INVALID_ID; 
  }

  if((text_style&0xF0) == DLG_STR_MODE_STATIC)
  {
    p_data->text_strID= title_str; 
  }
  else if((text_style&0xF0) == DLG_STR_MODE_EXTSTR)
  {
    p_data->title_style = text_style;
    p_data->text_content = title_str; 
  }
  else
  {
    p_data->text_strID= RSC_INVALID_ID; 
    p_data->text_content = RSC_INVALID_ID; 
  }
  
  p_data->title_style = title_style;
  p_data->text_style = text_style;  
  p_data->dlg_tmout = tm_out;

  if (p_dlg_rc != NULL)
  {
    p_data->x = p_dlg_rc->left;
    p_data->y = p_dlg_rc->top;
    p_data->w = RECTWP(p_dlg_rc);
    p_data->h = RECTHP(p_dlg_rc);
  }
  else
  {
    p_data->x = COMM_DLG_X;
    p_data->y = COMM_DLG_Y;
    p_data->w = COMM_DLG_W;
    p_data->h = COMM_DLG_H;
  }
}

void ui_comm_cfmdlg_open2(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
                strid, DLG_STR_MODE_NULL, RSC_INVALID_ID, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
                strid, DLG_STR_MODE_NULL, RSC_INVALID_ID, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}


void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
                IDS_SAVING, DLG_STR_MODE_NULL, RSC_INVALID_ID, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(func);

  ui_comm_dlg_close();
}


void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;
  
  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, DLG_STR_MODE_NULL, RSC_INVALID_ID, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else
  {
    DO_DLG_FUNC(undo_func);
  }
}

static RET_CODE on_dlg_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_dlg_close();
  return SUCCESS;
}

static RET_CODE on_dlg_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  //ctrl_set_active_ctrl(p_ctrl, NULL);  
  ui_comm_dlg_close();
  return SUCCESS;
}
static RET_CODE on_prompt_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_btn;
	p_btn = ctrl_get_child_by_id(p_ctrl, PROMPT_IDC_TEXT);

	if(p_btn != NULL)
	{
		ctrl_set_active_ctrl(p_ctrl, p_btn);
	}

	ui_comm_prompt_close();
	return SUCCESS;
}

static RET_CODE on_dlg_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  
  p_btn = ctrl_get_child_by_id(p_ctrl, IDC_BTN_NO);

  if(p_btn != NULL)
  {
    ctrl_set_active_ctrl(p_ctrl, p_btn);
  }
  
  ui_comm_dlg_close();
  return SUCCESS;
}

static RET_CODE on_dlg_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  
  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if ((p_btn != NULL) && (ctrl_get_ctrl_id(p_btn) == IDC_BTN_YES))
  {
    g_dlg_ret = DLG_RET_YES;
  }
  else if((p_btn != NULL) && (ctrl_get_ctrl_id(p_btn) == IDC_BTN_NO))
  {
    g_dlg_ret = DLG_RET_NO;
  }
  else
  {
    g_dlg_ret = DLG_RET_NULL;
  }

  fw_tmr_destroy(ROOT_ID_DIALOG, MSG_DLG_TIME_OUT);

  return ERR_NOFEATURE;
}

void ui_comm_dialog_close(void)
{
  fw_tmr_destroy(ROOT_ID_DIALOG, MSG_DLG_TIME_OUT);
  manage_close_menu(ROOT_ID_DIALOG, 0, 0);
}

int ui_comm_prompt_open(const comm_prompt_data_t *p_data)
{
#define  PROMPT_ICON_W               		60
#define  PROMPT_OFFSET_Y            		12
#define  PROMPT_OFFSET_X           			16
	control_t *p_cont;
	control_t  *p_txt, *p_icon;
	u16  x, y;
	RET_CODE ret;
	DEBUG_ENABLE_MODE(PROMPT,INFO);
	if(fw_find_root_by_id(ROOT_ID_PROMPT) != NULL) // already opened
	{
		UI_PRINTF("UI: already open a dialog, force close it! \n");
		ui_comm_prompt_close();
	}

	p_cont = p_txt = NULL;
	x = y = 0;

	// create root at first
	p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, ROOT_ID_PROMPT,
				p_data->x, p_data->y, p_data->w, p_data->h,
				NULL, 0);
	ctrl_set_attr(p_cont,OBJ_ATTR_ACTIVE);
	ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

	ctrl_set_keymap(p_cont, comm_dlg_keymap);
	ctrl_set_proc(p_cont, comm_dlg_proc);

	//title:bmp icon :"!"
	p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, PROMPT_IDC_ICON,
					PROMPT_OFFSET_X, PROMPT_OFFSET_Y,
					PROMPT_ICON_W,
					p_data->h -PROMPT_OFFSET_Y*2 ,
					p_cont, 0);
	ctrl_set_rstyle(p_icon, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	bmap_set_content_by_id(p_icon, conver_prompt_bmp_id(p_data));
	//title background   
	p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, PROMPT_IDC_TEXT,
					PROMPT_OFFSET_X+PROMPT_ICON_W+p_data->offset, PROMPT_OFFSET_Y,
					p_data->w - p_data->offset-(PROMPT_OFFSET_X*2+PROMPT_ICON_W),
					(u16)p_data->h -PROMPT_OFFSET_Y*2 ,
					p_cont, 0);
	ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_txt, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
	text_set_align_type(p_txt,  STL_LEFT | STL_VCENTER);

	if (p_data->string_mode == STR_MODE_STATIC)
	{
		text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_txt, (u16)p_data->text_strID);
	}
	else
	{
		text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
		text_set_content_by_unistr(p_txt, (u16*)p_data->text_content);
	}
	
	// attach the root onto mainwin
	OS_PRINTF("1p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
	fw_attach_root(p_cont, p_data->parent_root);

	ctrl_paint_ctrl(p_cont, FALSE);

	if(p_data->timeout!= 0)
	{
		ret = fw_tmr_create(ROOT_ID_PROMPT, MSG_PROMPT_TIME_OUT, p_data->timeout, FALSE);
		if(SUCCESS!=ret){
			DEBUG(MAIN,ERR,"SUCCESS!=ret\n");
			fw_tmr_destroy(ROOT_ID_PROMPT, MSG_PROMPT_TIME_OUT);
			ret = fw_tmr_create(ROOT_ID_PROMPT, MSG_PROMPT_TIME_OUT, p_data->timeout, FALSE);
		}
		MT_ASSERT(ret == SUCCESS);
	}

	fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
	return 0;
}

int ui_comm_prompt_search_open(const comm_prompt_data *p_data)
{
#define  PROMPT_COM_X				145
#define  PROMPT_COM_Y				180
#define  PROMPT_COM_W				400
#define  PROMPT_COM_H				97
#define  PROMPT_ICON_W               		60
#define  SN_PROMPT_OFFSET_Y            		18
#define  SN_PROMPT_OFFSET_X           		16
	control_t *p_cont1,*p_mw,*p_cont2;
	control_t *p_num1,*p_num2;
	control_t  *p_txt1, *p_txt2,*p_icon1;//,*p_icon2;
	u16  x, y;
	u16 asc_str[5]={0};
	u16	unistr[5]={0};
	RET_CODE ret1;//,ret2;
	if(fw_find_root_by_id(ROOT_ID_PROMPT) != NULL) // already opened
	{
		UI_PRINTF("UI: already open a dialog, force close it! \n");
		ui_comm_prompt_close();
	}

	p_cont1 = p_txt1 = p_cont2 = p_txt2= NULL;
	x = y = 0;
	//create mainwin
	p_mw= ctrl_create_ctrl((u8 *)CTRL_CONT, ROOT_ID_PROMPT,
				PROMPT_COM_X, PROMPT_COM_Y, PROMPT_COM_W, PROMPT_COM_H,
				NULL, 0);
	ctrl_set_attr(p_mw,OBJ_ATTR_ACTIVE);
	ctrl_set_rstyle(p_mw, RSI_COMM_BG, RSI_COMM_BG, RSI_COMM_BG);

	ctrl_set_keymap(p_mw, comm_dlg_keymap);
	ctrl_set_proc(p_mw, comm_dlg_proc);
	// create root at first
	//title:bmp icon :"!"
	p_icon1 = ctrl_create_ctrl((u8 *)CTRL_BMAP, PROMPT_IDC_ICON,
					SN_PROMPT_OFFSET_X, SN_PROMPT_OFFSET_Y,
					PROMPT_ICON_W,
					p_data->h -SN_PROMPT_OFFSET_Y*2 ,
					p_mw, 0);
	ctrl_set_rstyle(p_icon1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	bmap_set_content_by_id(p_icon1, conver_bmp_id2(p_data));

	//title background   
	p_txt1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, PROMPT_IDC_TEXT,
					SN_PROMPT_OFFSET_X+PROMPT_ICON_W, SN_PROMPT_OFFSET_Y-18,
					p_data->w - (SN_PROMPT_OFFSET_X*2+PROMPT_ICON_W)+10,
					(u16)p_data->h -SN_PROMPT_OFFSET_Y*2 ,
					p_mw, 0);
	ctrl_set_rstyle(p_txt1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_txt1, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
	text_set_align_type(p_txt1,  STL_LEFT | STL_VCENTER);
	text_set_content_type(p_txt1, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_txt1, (u16)p_data->tv_text_strID);
	p_num1=ctrl_create_ctrl((u8*)CTRL_TEXT, SN_IDC_SEARCH_PROMPT, 
						p_data->w -SN_PROMPT_OFFSET_X+15, SN_PROMPT_OFFSET_Y-18, 
						40,(u16)p_data->h -SN_PROMPT_OFFSET_Y*2,
						p_mw,0);
	ctrl_set_rstyle(p_num1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_align_type(p_num1, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_num1, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
	text_set_content_type(p_num1, TEXT_STRTYPE_UNICODE);
	sprintf((char*)asc_str, "%ld", p_data->tv_num);
	str_asc2uni((u8*)asc_str,unistr);
	text_set_content_by_unistr(p_num1, unistr);

	p_txt2 = ctrl_create_ctrl((u8 *)CTRL_TEXT, SN_IDC_RADIO_PROMPT,
					SN_PROMPT_OFFSET_X+PROMPT_ICON_W, SN_PROMPT_OFFSET_Y+p_data->h -SN_PROMPT_OFFSET_Y*2-28,
					p_data->w - (SN_PROMPT_OFFSET_X*2+PROMPT_ICON_W),
					(u16)p_data->h -SN_PROMPT_OFFSET_Y*2-18 ,
					p_mw, 0);
	ctrl_set_rstyle(p_txt2, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_txt2, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
	text_set_align_type(p_txt2,  STL_LEFT | STL_VCENTER);
	text_set_content_type(p_txt2, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_txt2, (u16)p_data->radio_text_strID);
	p_num2=ctrl_create_ctrl((u8*)CTRL_TEXT, SN_IDC_SEARCH_RADIO, 
					SN_PROMPT_OFFSET_X+PROMPT_ICON_W+100,  SN_PROMPT_OFFSET_Y+p_data->h -SN_PROMPT_OFFSET_Y*2-28, 
					40,(u16)p_data->h -SN_PROMPT_OFFSET_Y*2-18,
					p_mw,0);
	ctrl_set_rstyle(p_num2, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_align_type(p_num2, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_num2, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
	text_set_content_type(p_num2, TEXT_STRTYPE_UNICODE);
	sprintf((char*)asc_str, "%ld", p_data->radio_num);
	str_asc2uni((u8*)asc_str,unistr);
	text_set_content_by_unistr(p_num2, unistr);
	// attach the root onto mainwin
	OS_PRINTF("2p_cont[0x%x], root[%d]\n", p_mw, p_data->parent_root);
	fw_attach_root(p_mw, p_data->parent_root);


	ctrl_paint_ctrl(p_mw, FALSE);

	if(p_data->timeout!= 0)
	{
		ret1 = fw_tmr_create(ROOT_ID_PROMPT, MSG_PROMPT_TIME_OUT, p_data->timeout, FALSE);
		MT_ASSERT(ret1 == SUCCESS);
	}

	fw_default_mainwin_loop(ctrl_get_ctrl_id(p_mw));
	return 0;
}

dlg_ret_t ui_comm_dialog_open(const comm_dialog_data_t *p_data)
{
	control_t *p_cont;
	control_t *p_btn, *p_txt, *p_icon;
	u16 i, cnt, x, y;
	u16 btn[2] = { IDS_YES, IDS_NO };
	dlg_ret_t ret;
#define   DIALOG_OFFSET_X					20 // ±ß¿ò¿í¶È
#define   DIALOG_OFFSET_Y					20
#define  	DIALOG_BUTTON_H    				25 
#define  	DIALOG_ICON_W		 				50
#define  	DIALOG_BUTTON_W					56


	if(fw_find_root_by_id(ROOT_ID_DIALOG) != NULL) // already opened
	{
		UI_PRINTF("UI: already open a dialog, force close it! \n");
		ui_comm_dialog_close();
	}

	p_cont = p_btn = p_txt = NULL;
	x = y = cnt = 0;

	// create root at first
	p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, ROOT_ID_DIALOG,
	                        p_data->x, p_data->y, p_data->w, p_data->h,
	                        NULL, 0);
	ctrl_set_attr(p_cont, OBJ_ATTR_ACTIVE);
	ctrl_set_rstyle(p_cont, RSI_DIG_BG, RSI_DIG_BG, RSI_DIG_BG);

	ctrl_set_keymap(p_cont, comm_dlg_keymap);
	ctrl_set_proc(p_cont, comm_dlg_proc);

	//title:bmp icon :"!"
	#if 1
	p_icon = ctrl_create_ctrl((u8 *)CTRL_BMAP, DIALOG_IDC_ICON,
	                       DIALOG_OFFSET_X, DIALOG_OFFSET_Y,
	                       DIALOG_ICON_W,
	                       p_data->h -(DIALOG_OFFSET_Y*2 + DIALOG_BUTTON_H),
	                       p_cont, 0);
	ctrl_set_rstyle(p_icon, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	bmap_set_content_by_id(p_icon, conver_bmp_id(p_data));
	#endif
	//text region
	p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, DIALOG_IDC_TEXT,
	                         DIALOG_OFFSET_X+DIALOG_ICON_W, DIALOG_OFFSET_Y,
	                         p_data->w - (DIALOG_OFFSET_X*2 + DIALOG_ICON_W) ,
	                         p_data->h - (DIALOG_OFFSET_Y*2 + DIALOG_BUTTON_H),
	                         p_cont, 0);
	ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_txt, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
	text_set_align_type(p_txt,  STL_LEFT| STL_VCENTER);

	if (p_data->string_mode == STR_MODE_STATIC)
	{
		text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_txt, (u16)p_data->text_strID);
	}
	else
	{
		text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
		text_set_content_by_unistr(p_txt, (u16*)p_data->text_content);
	}
	
	//button
	y = p_data->h - DIALOG_BUTTON_H - DIALOG_OFFSET_Y;
	switch (p_data->type)
	{
		case DLG_FOR_ASK:
			x = p_data->w / 3 - DIALOG_BUTTON_W / 2;
			cnt = 2;
			break;
		case DLG_FOR_CONFIRM:
			x = (p_data->w - DLG_BTN_W) / 2;
			cnt = 1;
			break;
		case DLG_FOR_SHOW:
			cnt = 0;
			break;
		default:
			MT_ASSERT(0);
	}

	for (i = 0; i < cnt; i++)
	{
	p_btn = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(DIALOG_IDC_BTN_YES + i),
	                         x, y, DIALOG_BUTTON_W, DIALOG_BUTTON_H, p_cont, 0);
	ctrl_set_rstyle(p_btn,
	                SN_RSI_BNT_TEXT_SH, SN_RSI_BNT_TEXT_HL, SN_RSI_BNT_TEXT_SH);
	text_set_font_style(p_btn,
	                    FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
	text_set_content_type(p_btn, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_btn,
	                          (u16)(cnt == 1 ? IDS_OK : btn[i]));
	text_set_align_type(p_txt,  STL_CENTER | STL_VCENTER);
	ctrl_set_related_id(p_btn,
	                    (u8)((i - 1 + cnt) % cnt + 1), /* left */
	                    0,                             /* up */
	                    (u8)((i + 1) % cnt + 1),       /* right */
	                    0);                            /* down */

	x += p_data->w / 3;
	}

	// attach the root onto mainwin
	OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
	fw_attach_root(p_cont, p_data->parent_root);

	// paint
	if (cnt > 0)
	{
		p_btn = ctrl_get_child_by_id(p_cont,DIALOG_IDC_BTN_YES);
		ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
	}
	ctrl_paint_ctrl(p_cont, FALSE);

	if(p_data->timeout != 0)
	{
		ret = fw_tmr_create(ROOT_ID_DIALOG, MSG_DLG_TIME_OUT, p_data->timeout, FALSE);
		MT_ASSERT(ret == SUCCESS);
	}

	// start loop to get msg
	if (p_data->type == DLG_FOR_SHOW)
	{
	//ret = DLG_RET_NULL;
		fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
		ret = g_dlg_ret;
	}
	else
	{
		fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
		ret = g_dlg_ret;
	}

	return ret;
}

BEGIN_KEYMAP(comm_dlg_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(comm_dlg_keymap, NULL)


BEGIN_MSGPROC(comm_dlg_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_dlg_select)
  ON_COMMAND(MSG_CANCEL, on_dlg_cancel)
  ON_COMMAND(MSG_DLG_TIME_OUT, on_dlg_timeout)
  ON_COMMAND(MSG_PROMPT_TIME_OUT, on_prompt_cancel)
  ON_COMMAND(MSG_DESTROY, on_dlg_destroy)
END_MSGPROC(comm_dlg_proc, cont_class_proc)


