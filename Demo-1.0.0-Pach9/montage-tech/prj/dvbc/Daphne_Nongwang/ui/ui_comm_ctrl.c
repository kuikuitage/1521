/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "string.h"
#include "stdio.h"


typedef void (*SET_ALIGN_TYPE)(control_t *, u32);
typedef void (*SET_FONT_STYLE)(control_t *, u8, u8, u8);

static control_t *_comm_ctrl_create(u8 type,
                                    control_t *parent,
                                    u8 ctrl_id,
                                    u16 x,
                                    u16 y,
                                    u16 lw,
                                    u16 rw)
{
  control_t *cont, *txt, *p_ctrl = NULL;
  u8 rsi_sh, rsi_hl, rsi_gray;
  u8 *class_name;
  SET_ALIGN_TYPE set_align_type;
  SET_FONT_STYLE set_font_style;
  keymap_t keymap;

  cont = ctrl_create_ctrl((u8 *)CTRL_CONT, ctrl_id,
                     x, y, (u16)(lw + rw), COMM_CTRL_H,
                     parent, 0);
//  ctrl_set_style (cont, STL_EX_WHOLE_HL);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
  ctrl_set_rstyle(cont,
                  RSI_IGNORE,
                  RSI_IGNORE,
                  RSI_IGNORE);         
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
  ctrl_set_style(cont, STL_EX_WHOLE_HL);

  if(lw > 0)
  {
    txt =
      ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_TXT,
                       0, 0, lw, COMM_CTRL_H,
                       cont, 0);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
    ctrl_set_rstyle(txt,
                    RSI_COMM_TXT_SH,  
                    RSI_COMM_TXT_HL, 
                    RSI_COMM_TXT_GRAY);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
    text_set_content_type(txt, TEXT_STRTYPE_STRID);
    text_set_font_style(txt,
					FSI_COMM_BTN, //LXD CHANGE
					FSI_COMM_BTN, //LXD CHANGE
					FSI_COMM_TXT_GRAY);
    text_set_align_type(txt, STL_LEFT | STL_VCENTER);
    text_set_offset(txt, COMM_CTRL_OX, 0);
  }

  switch(type)
  {
    case COMM_STATIC:
      class_name = (u8 *)CTRL_TEXT;
      rsi_sh = RSI_COMM_STATIC_SH;
      rsi_hl = RSI_COMM_STATIC_HL;
      rsi_gray = RSI_COMM_STATIC_GRAY;      
      keymap = ui_comm_static_keymap;
      set_align_type = text_set_align_type;
      set_font_style = text_set_font_style;
      break;
    case COMM_SELECT:
      class_name = (u8 *)CTRL_CBOX;
      rsi_sh = RSI_IGNORE;
      rsi_hl = RSI_IGNORE;
      rsi_gray = RSI_IGNORE;
      keymap = ui_comm_select_keymap;
      set_align_type = cbox_set_align_style;
      set_font_style = cbox_set_font_style;
      break;
    case COMM_TIMEDIT:
      class_name = (u8 *)CTRL_TBOX;
      rsi_sh = RSI_COMM_TIMEDIT_SH;
      rsi_hl = RSI_COMM_TIMEDIT_HL;
      rsi_gray = RSI_COMM_TIMEDIT_GRAY;
      keymap = ui_comm_tbox_keymap;
      set_align_type = tbox_set_align_type;
      set_font_style = tbox_set_font_style;
      break;
    case COMM_NUMEDIT:
      class_name = (u8 *)CTRL_NBOX;
      rsi_sh = RSI_COMM_NUMEDIT_SH;
      rsi_hl = RSI_COMM_NUMEDIT_HL;
      rsi_gray = RSI_COMM_NUMEDIT_GRAY;
      keymap = ui_comm_num_keymap;
      set_align_type = nbox_set_align_type;
      set_font_style = nbox_set_font_style;
      break;
    case COMM_PWDEDIT:
      class_name = (u8 *)CTRL_EBOX;
      rsi_sh = RSI_COMM_PWDEDIT_SH;
      rsi_hl = RSI_COMM_PWDEDIT_HL;
      rsi_gray = RSI_COMM_PWDEDIT_GRAY;
      keymap = ui_comm_edit_keymap;
      set_align_type = ebox_set_align_type;
      set_font_style = ebox_set_font_style;
      break;
    case COMM_T9EDIT:
      class_name = (u8 *)CTRL_EBOX;
      rsi_sh = RSI_COMM_T9EDIT_SH;
      rsi_hl = RSI_SN_BAR_YELLOW_HL;
      rsi_gray = RSI_COMM_T9EDIT_GRAY;
      keymap = ui_comm_t9_keymap;
      set_align_type = ebox_set_align_type;
      set_font_style = ebox_set_font_style;
      break;
    default:
      rsi_sh = rsi_hl = rsi_gray = 0;
      class_name = NULL;
      keymap = NULL;
      set_align_type = NULL;
      set_font_style = NULL;
      MT_ASSERT(0);
  }

  p_ctrl = ctrl_create_ctrl((u8 *)class_name, IDC_COMM_CTRL,
                            lw, 0, rw, COMM_CTRL_H,
                            cont, 0);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
  ctrl_set_rstyle(p_ctrl, rsi_sh, rsi_hl, rsi_gray);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create......\n");
  ctrl_set_keymap(p_ctrl, keymap);

  set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  set_font_style(p_ctrl,
                 FSI_COMM_BTN,
                 FSI_COMM_BTN,
                 FSI_COMM_BTN);

  /* after creating */
  switch(type)
  {
    case COMM_NUMEDIT:
      /* set default proc for chk data */
      ctrl_set_proc(p_ctrl, ui_comm_num_proc);
      break;
    case COMM_TIMEDIT:
      ctrl_set_proc(p_ctrl, ui_comm_time_proc);
      break;
    case COMM_PWDEDIT:
      ebox_set_worktype(p_ctrl, EBOX_WORKTYPE_HIDE);
      ctrl_set_proc(p_ctrl, ui_comm_edit_proc);
      break;
    case COMM_T9EDIT:
      ctrl_set_proc(p_ctrl, ui_comm_t9_proc);
      break;
    default:
      /* do nothing */;
  }

  ctrl_set_active_ctrl(cont, p_ctrl);
  return cont;
}


static control_t *_comm_ctrl_create_withposix(u8 type,
                                    control_t *parent,
                                    u8 ctrl_id,
                                    u16 x,
                                    u16 y,
                                    u16 lw,
                                    u16 mw,
                                    u16 rw)
{
  control_t *cont, *txt, *p_ctrl = NULL;
  u8 rsi_sh, rsi_hl, rsi_gray;
  u8 *class_name;
  SET_ALIGN_TYPE set_align_type;
  SET_FONT_STYLE set_font_style;
  keymap_t keymap;

  cont =
    ctrl_create_ctrl((u8 *)CTRL_CONT, ctrl_id,
                     x, y, (u16)(lw + mw + rw), COMM_CTRL_H,
                     parent, 0);
//  ctrl_set_style (cont, STL_EX_WHOLE_HL);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
  ctrl_set_rstyle(cont,
                  RSI_IGNORE,
                  RSI_IGNORE,
                  RSI_IGNORE);
  ctrl_set_style(cont, STL_EX_WHOLE_HL);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");

  if(lw > 0)
  {
    txt =
      ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_TXT,
                       0, 0, lw, COMM_CTRL_H,
                       cont, 0);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
    ctrl_set_rstyle(txt,
                    RSI_COMM_TXT_SH,
                    RSI_COMM_TXT_HL,
                    RSI_COMM_TXT_GRAY);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
    text_set_content_type(txt, TEXT_STRTYPE_STRID);
    text_set_font_style(txt,
                        FSI_COMM_TXT_N,
                        FSI_COMM_TXT_HL,
                        FSI_COMM_TXT_GRAY);
    text_set_align_type(txt, STL_LEFT | STL_VCENTER);
    text_set_offset(txt, COMM_CTRL_OX, 0);
  }

  if(rw > 0)
  {
    txt =
      ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_POSIX,
                       lw + mw, 0, rw, COMM_CTRL_H,
                       cont, 0);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
    ctrl_set_rstyle(txt,
                    RSI_IGNORE,
                    RSI_IGNORE,
                    RSI_IGNORE);
	//DEBUG_ENABLE_MODE(COMM_CTL, INFO);
	//DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
    text_set_content_type(txt, TEXT_STRTYPE_STRID);
    text_set_font_style(txt,
                        FSI_COMM_TXT_N,
                        FSI_COMM_TXT_HL,
                        FSI_COMM_TXT_GRAY);
    text_set_align_type(txt, STL_LEFT | STL_VCENTER);
    //text_set_offset(txt, COMM_CTRL_OX, 0);
  }

  switch(type)
  {
    case COMM_STATIC:
      class_name = (u8 *)CTRL_TEXT;
      rsi_sh = RSI_COMM_STATIC_SH;
      rsi_hl = RSI_COMM_STATIC_HL;
      rsi_gray = RSI_COMM_STATIC_GRAY;
      keymap = ui_comm_static_keymap;
      set_align_type = text_set_align_type;
      set_font_style = text_set_font_style;
      break;
    case COMM_SELECT:
      class_name = (u8 *)CTRL_CBOX;
      rsi_sh = RSI_COMM_SELECT_SH;
      rsi_hl = RSI_IGNORE;
      rsi_gray = RSI_COMM_SELECT_GRAY;
      keymap = ui_comm_select_keymap;
      set_align_type = cbox_set_align_style;
      set_font_style = cbox_set_font_style;
      break;
    case COMM_TIMEDIT:
      class_name = (u8 *)CTRL_TBOX;
      rsi_sh = RSI_COMM_TIMEDIT_SH;
      rsi_hl = RSI_COMM_TIMEDIT_HL;
      rsi_gray = RSI_COMM_TIMEDIT_GRAY;
      keymap = ui_comm_tbox_keymap;
      set_align_type = tbox_set_align_type;
      set_font_style = tbox_set_font_style;
      break;
    case COMM_NUMEDIT:
      class_name = (u8 *)CTRL_NBOX;
      rsi_sh = RSI_COMM_NUMEDIT_SH;
      rsi_hl = RSI_COMM_NUMEDIT_HL;
      rsi_gray = RSI_COMM_NUMEDIT_GRAY;
      keymap = ui_comm_num_keymap;
      set_align_type = nbox_set_align_type;
      set_font_style = nbox_set_font_style;
      break;
    case COMM_PWDEDIT:
      class_name = (u8 *)CTRL_EBOX;
      rsi_sh = RSI_COMM_PWDEDIT_SH;
      rsi_hl = RSI_COMM_PWDEDIT_HL;
      rsi_gray = RSI_COMM_PWDEDIT_GRAY;
      keymap = ui_comm_edit_keymap;
      set_align_type = ebox_set_align_type;
      set_font_style = ebox_set_font_style;
      break;
    case COMM_T9EDIT:
      class_name = (u8 *)CTRL_EBOX;
      rsi_sh = RSI_COMM_T9EDIT_SH;
      rsi_hl = RSI_SN_BAR_YELLOW_HL;
      rsi_gray = RSI_COMM_T9EDIT_GRAY;
      keymap = ui_comm_t9_keymap;
      set_align_type = ebox_set_align_type;
      set_font_style = ebox_set_font_style;
      break;
    default:
      rsi_sh = rsi_hl = rsi_gray = 0;
      class_name = NULL;
      keymap = NULL;
      set_align_type = NULL;
      set_font_style = NULL;
      MT_ASSERT(0);
  }

  p_ctrl = ctrl_create_ctrl((u8 *)class_name, IDC_COMM_CTRL,
                            lw, 0, mw, COMM_CTRL_H,
                            cont, 0);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
  ctrl_set_rstyle(p_ctrl, rsi_sh, rsi_hl, rsi_gray);
  //DEBUG_ENABLE_MODE(COMM_CTL, INFO);
  //DEBUG(COMM_CTL, INFO, "_comm_ctrl_create_withposix......\n");
  ctrl_set_keymap(p_ctrl, keymap);

  set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  set_font_style(p_ctrl,FSI_COMM_TXT_N,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);

  /* after creating */
  switch(type)
  {
    case COMM_NUMEDIT:
      /* set default proc for chk data */
      ctrl_set_proc(p_ctrl, ui_comm_num_proc);
      break;
    case COMM_TIMEDIT:
      ctrl_set_proc(p_ctrl, ui_comm_time_proc);
      break;
    case COMM_PWDEDIT:
      ebox_set_worktype(p_ctrl, EBOX_WORKTYPE_HIDE);
      ctrl_set_proc(p_ctrl, ui_comm_edit_proc);
      break;
    case COMM_T9EDIT:
      ctrl_set_proc(p_ctrl, ui_comm_t9_proc);
      break;
    default:
      /* do nothing */;
  }

  ctrl_set_active_ctrl(cont, p_ctrl);
  return cont;
}

void _comm_ctrl_set_static_txt(control_t *p_ctrl, u16 strid)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_TXT);
  if(txt != NULL)
  {
    text_set_content_by_strid(txt, strid);
  }
}


void _comm_ctrl_set_posix_txt(control_t *p_ctrl, u16 strid)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_POSIX);
  if(txt != NULL)
  {
    text_set_content_by_strid(txt, strid);
  }
}


void ui_comm_ctrl_set_txt_font_style(control_t *cont,
                                 u8 sh_fstyle,
                                 u8 hl_fstyle,
                                 u8 gr_fstyle)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  text_set_font_style(p_txt, sh_fstyle, hl_fstyle, gr_fstyle);
}


void ui_comm_ctrl_set_ctrl_font_style(u8 type, control_t *cont,
                                  u8 sh_fstyle,
                                  u8 hl_fstyle,
                                  u8 gr_fstyle)
{
  control_t *p_ctrl;
  SET_FONT_STYLE set_font_style = NULL;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);

  switch(type)
  {
    case COMM_STATIC:
      set_font_style = text_set_font_style;
      break;
    case COMM_SELECT:
      set_font_style = cbox_set_font_style;
      break;
    case COMM_TIMEDIT:
      set_font_style = tbox_set_font_style;
      break;
    case COMM_NUMEDIT:
      set_font_style = nbox_set_font_style;
      break;
    case COMM_PWDEDIT:
      set_font_style = ebox_set_font_style;
      break;
    case COMM_T9EDIT:
      set_font_style = ebox_set_font_style;
      break;
    default:
      break;
  }
  
  if(NULL != set_font_style)
  {
    set_font_style(p_ctrl, sh_fstyle, hl_fstyle, gr_fstyle);
  }
}


void ui_comm_ctrl_set_txt_align_tyle(control_t *cont, u32 style)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  text_set_align_type(p_txt, style);
}


void ui_comm_ctrl_set_ctrl_align_tyle(u8 type, control_t *cont, u32 style)
{
  control_t *p_ctrl;
  SET_ALIGN_TYPE set_align_type = NULL;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);

  switch(type)
  {
    case COMM_STATIC:
      set_align_type = text_set_align_type;
      break;
    case COMM_SELECT:
      set_align_type = cbox_set_align_style;
      break;
    case COMM_TIMEDIT:
      set_align_type = tbox_set_align_type;
      break;
    case COMM_NUMEDIT:
      set_align_type = nbox_set_align_type;
      break;
    case COMM_PWDEDIT:
      set_align_type = ebox_set_align_type;
      break;
    case COMM_T9EDIT:
      set_align_type = ebox_set_align_type;
      break;
    default:
      break;
  }
  if(NULL != set_align_type)
  {
    set_align_type(p_ctrl, style);
  }
}

void ui_comm_ctrl_set_cont_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle)
{
  ctrl_set_rstyle(cont, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_txt_rstyle(control_t *cont,
                                 u8 sh_rstyle,
                                 u8 hl_rstyle,
                                 u8 gr_rstyle)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  ctrl_set_rstyle(p_txt, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_ctrl_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ctrl_set_rstyle(p_ctrl, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_keymap(control_t *p_ctrl, keymap_t keymap)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_set_keymap(sub_ctrl, keymap);
}


void ui_comm_ctrl_set_proc(control_t *p_ctrl, msgproc_t proc)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_set_proc(sub_ctrl, proc);
}


void ui_comm_ctrl_update_attr(control_t *p_ctrl, BOOL is_enable)
{
  control_t *sub_ctrl;
  rect_t rc;
  u8 attr = ctrl_get_attr(p_ctrl);

  if(is_enable)
  {
    if(attr == OBJ_ATTR_INACTIVE)
    {
      ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
    }
  }
  else
  {
    ctrl_set_attr(p_ctrl, OBJ_ATTR_INACTIVE);
  }

  if(attr != ctrl_get_attr(p_ctrl))
  {
    /*paint all*/
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }
  else
  {
    /*only p_ctrl*/
    sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
    ctrl_get_frame(sub_ctrl, &rc);
    ctrl_add_rect_to_invrgn(p_ctrl, &rc);
  }
}


void ui_comm_ctrl_paint_ctrl(control_t *p_ctrl, BOOL is_force)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_paint_ctrl(sub_ctrl, is_force);
}

control_t * ui_comm_ctrl_get_ctrl(control_t *p_ctrl)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return sub_ctrl;
}

control_t * ui_comm_ctrl_get_txt(control_t *p_ctrl)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_TXT);

  return sub_ctrl;
}

/*********************************************************************
 * COMMON SELECT
 *********************************************************************/
control_t *ui_comm_select_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw)
{
  return _comm_ctrl_create(COMM_SELECT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_select_set_param(control_t *p_ctrl,
                              BOOL is_cycle,
                              u32 workmode,
                              u16 total,
                              u32 content_type,
                              cbox_dync_update_t callback)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_enable_cycle_mode(cbox, is_cycle);
  cbox_set_work_mode(cbox, workmode);

  switch(workmode)
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_count(cbox, total);
      cbox_static_set_content_type(cbox, content_type);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_count(cbox, total);
      cbox_dync_set_update(cbox, callback);
      break;
    case CBOX_WORKMODE_NUMBER:
      cbox_num_set_type(cbox, content_type);
      break;
    default:
      /* do nothing */;
  }
}


static list_xstyle_t dlist_item_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,//#if 0 rsc
    RSI_IGNORE,
    RSI_IGNORE,
};

static list_xstyle_t dlist_field_fstyle =
{
  FSI_LIST_TXT_G,
  FSI_LIST_TXT_N,
  FSI_LIST_BTN_HL,
  FSI_LIST_TXT_SEL,
  FSI_LIST_BTN_HL,
};


static list_xstyle_t dlist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};


void ui_comm_select_create_droplist(control_t *p_ctrl, u16 page)
{
  control_t *cbox;
  rect_t rc_cbox;

  if (page == 0)
  {
    /* don't need drop-down list */
    return;
  }

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_get_frame(cbox, &rc_cbox);

  cbox_create_droplist(cbox, page, 12, 4);
  cbox_droplist_set_rstyle(cbox,
                           RSI_WINDOW_1,
                           RSI_WINDOW_1,
                           RSI_WINDOW_1);
  cbox_droplist_set_mid_rect(cbox, 8, 8, RECTW(rc_cbox) - 16 - 16,
                             RECTH(rc_cbox) * page - 16, 0);
  cbox_droplist_set_item_rstyle(cbox, &dlist_item_rstyle);
  cbox_droplist_set_field_attr(cbox, STL_LEFT | STL_VCENTER,
                               RECTW(rc_cbox) - 8, 16, 0);
  cbox_droplist_set_field_rect_style(cbox, &dlist_field_rstyle);
  cbox_droplist_set_field_font_style(cbox, &dlist_field_fstyle);
  
  cbox_droplist_set_sbar_rstyle(cbox,
                                RSI_IGNORE,
                                RSI_IGNORE,
                                RSI_IGNORE);
  cbox_droplist_set_sbar_mid_rect(cbox, 0, 12, 12, RECTH(rc_cbox) * page - 8 - 12);
  cbox_droplist_set_sbar_mid_rstyle(cbox,
                                    RSI_IGNORE,
                                    RSI_IGNORE,
                                    RSI_IGNORE);
}

void ui_comm_select_droplist_set_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_rstyle(p_cbox, sh_style, hl_style, gr_style);
}

void ui_comm_select_droplist_set_item_rstyle(control_t *p_ctrl,
  list_xstyle_t *p_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_item_rstyle(p_cbox, p_style);
}

void ui_comm_select_droplist_set_sbar_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_sbar_rstyle(p_cbox,  sh_style, hl_style, gr_style);
}

void ui_comm_select_droplist_set_sbar_mid_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_sbar_mid_rstyle(p_cbox,  sh_style, hl_style, gr_style);
}

void ui_comm_select_set_num_range(control_t *p_ctrl,
                                  s32 min,
                                  s32 max,
                                  u32 step,
                                  u8 bit_length,
                                  u16 *post_fix)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_set_range(cbox, min, max, step, bit_length);
  if(post_fix != NULL)
  {
    cbox_num_set_postfix_type(cbox, CBOX_ITEM_POSTFIX_TYPE_EXTSTR);
    cbox_num_set_postfix_by_extstr(cbox, post_fix);
  }
}


void ui_comm_select_get_num_range(control_t *p_ctrl,
                                  s32 *min,
                                  s32 *max,
                                  u32 *step)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_get_range(cbox, min, max, step);
}


void ui_comm_select_set_num(control_t *p_ctrl, s32 num)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_set_curn(cbox, num);
}


s32 ui_comm_select_get_num(control_t *p_ctrl)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return cbox_num_get_curn(cbox);
}


void ui_comm_select_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_select_set_content(control_t *p_ctrl, u8 idx, u16 strid)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_static_set_content_by_strid(cbox, idx, strid);
}


void ui_comm_select_set_focus(control_t *p_ctrl, u16 focus)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_focus(cbox, focus);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_focus(cbox, focus);
      break;
    default:
      MT_ASSERT(0);
  }
}


u16 ui_comm_select_get_focus(control_t *p_ctrl)
{
  u16 focus = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      focus = cbox_static_get_focus(cbox);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      focus = cbox_dync_get_focus(cbox);
      break;
    default:
      MT_ASSERT(0);
  }
  return focus;
}


u16 ui_comm_select_get_count(control_t *p_ctrl)
{
  u16 count = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      count = cbox_static_get_count(cbox);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      count = cbox_dync_get_count(cbox);
      break;
    default:
      MT_ASSERT(0);
  }
  return count;
}


u32 ui_comm_select_get_content(control_t *p_ctrl)
{
  u32 content = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      content = cbox_get_content(cbox, cbox_static_get_focus(p_ctrl));
      break;
    case CBOX_WORKMODE_DYNAMIC:
      content = cbox_get_content(cbox, cbox_dync_get_focus(p_ctrl));
      break;
    case CBOX_WORKMODE_NUMBER:
      content = cbox_num_get_curn(cbox);
    default:
      MT_ASSERT(0);
  }
  return content;
}


/*********************************************************************
 * COMMON TIMEBOX
 *********************************************************************/
static RET_CODE on_time_unselect(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(tbox_is_on_edit(p_ctrl))
  {
    if(tbox_is_invalid_time(p_ctrl))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_time_out_range(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, 0);
  return SUCCESS;
}


control_t *ui_comm_timedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_TIMEDIT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_timedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_timedit_set_param(control_t *p_ctrl,
                               u8 bit,
                               u8 item,
                               u32 time_type,
                               u32 sepa_type,
                               u8 num_width)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  tbox_set_time_type(p_tbox, time_type);
  tbox_set_focus(p_tbox, bit, item);
  tbox_set_separator_type(p_tbox, sepa_type);
  tbox_set_max_num_width(p_tbox, num_width);
}


void ui_comm_timedit_set_separator_by_bmapid(control_t *p_ctrl,
                                             u8 index,
                                             u16 bmapid)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_bmpid(p_tbox, index, bmapid);
}


void ui_comm_timedit_set_separator_by_extstr(control_t *p_ctrl,
                                             u8 index,
                                             u16 *p_str)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_extstr(p_tbox, index, p_str);
}


void ui_comm_timedit_set_separator_by_strid(control_t *p_ctrl,
                                            u8 index,
                                            u16 strid)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_strid(p_tbox, index, strid);
}


void ui_comm_timedit_set_separator_by_ascchar(control_t *p_ctrl,
                                              u8 index,
                                              u8 ascchar)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_ascchar(p_tbox, index, ascchar);
}


void ui_comm_timedit_set_separator_by_unichar(control_t *p_ctrl,
                                              u8 index,
                                              u16 unichar)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_unichar(p_tbox, index, unichar);
}


void ui_comm_timedit_set_time(control_t *p_ctrl, utc_time_t *p_time)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_time(p_tbox, p_time);
}


void ui_comm_timedit_get_time(control_t *p_ctrl, utc_time_t *p_time)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_get_time(p_tbox, p_time);
}


/*********************************************************************
 * COMMON NUMEDIT
 *********************************************************************/
static RET_CODE num_spin(control_t *p_ctrl, BOOL is_up)
{
  s32 min, max;
  s32 num;

  if(nbox_is_on_edit(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  num = (s32)nbox_get_num(p_ctrl);
  nbox_get_range(p_ctrl, &min, &max);

  if(is_up)
  {
    if((num <= min) || (num > max))
    {
      num = max;
    }
    else
    {
      num--;
    }
  }
  else
  {
    if((num >= max) || (num < min))
    {
      num = min;
    }
    else
    {
      num++;
    }
  }
  nbox_set_num_by_dec(p_ctrl, num);
  ctrl_paint_ctrl(p_ctrl, TRUE);

  return SUCCESS;
}


static RET_CODE on_num_spin_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  return num_spin(p_ctrl, TRUE);
}

static RET_CODE on_num_focus_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_ctrl, &border);
    
    nbox_exit_edit(p_ctrl);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_num_focus_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_ctrl, &border);
    
    nbox_exit_edit(p_ctrl);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}

/*for jazz special spec*/
static void on_num_set_edit_sign(control_t *p_ctrl, BOOL is_left)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_ctrl->priv_attr |= NBOX_HL_STATUS_MASK;

  if(is_left)
  {
    // goto the first bit
    p_data->curn_bit = p_data->bit_length - 1;
  }
  else
  {
    p_data->curn_bit = 0;
  }

  return ;
}

static RET_CODE on_num_get_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  
  MT_ASSERT(p_ctrl != NULL);
  nbox_class_proc(p_ctrl, msg, para1, para2);
  //nbox_class_proc(p_ctrl, MSG_SELECT, para1, para2);
  on_num_set_edit_sign(p_ctrl, TRUE); /*for jazz special spec*/
  return ERR_NOFEATURE;
}


static RET_CODE on_num_spin_down(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  return num_spin(p_ctrl, FALSE);
}


static RET_CODE on_num_unselect(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  RET_CODE ret = SUCCESS;
  u32 border;

  if(nbox_is_on_edit(p_ctrl))
  {
    if(nbox_is_outrange(p_ctrl, &border))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_num_out_range(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, OUTRANGE_TMOUT);
  return SUCCESS;
}


control_t *ui_comm_numedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_NUMEDIT, parent, ctrl_id, x, y, lw, rw);
}

control_t *ui_comm_numedit_create_withposix(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 mw,
                                  u16 rw)
{
  return _comm_ctrl_create_withposix(COMM_NUMEDIT, parent, ctrl_id, x, y, lw, mw, rw);
}


void ui_comm_numedit_set_param(control_t *p_ctrl,
                               u32 type,
                               u32 min,
                               u32 max,
                               u8 bit_length,
                               u8 focus)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_num_type(nbox, type);
  nbox_set_postfix_type(nbox, type);
  nbox_set_range(nbox, min, max, bit_length);
  nbox_set_focus(nbox, focus);
}


void ui_comm_numedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_numedit_set_posix_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_posix_txt(p_ctrl, strid);
}

void ui_comm_numedit_set_num(control_t *p_ctrl, u32 num)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_num_by_dec(nbox, num);
}


u32 ui_comm_numedit_get_num(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_num(nbox);
}


void ui_comm_numedit_set_postfix(control_t *p_ctrl, u16 strid)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_postfix_type(nbox, NBOX_ITEM_POSTFIX_TYPE_STRID);
  nbox_set_postfix_by_strid(nbox, strid);
}


u32 ui_comm_numedit_get_postfix(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_postfix(nbox);
}


void ui_comm_numedit_set_decimal_places(control_t *p_ctrl, u8 places)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_separator(nbox, '.');
  nbox_set_separator_pos(nbox, places);
}


u8 ui_comm_numedit_get_decimal_places(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_separator_pos(nbox);
}


void ui_comm_numedit_set_separator(control_t *p_ctrl, u8 separator, u8 places)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_separator(nbox, separator);
  nbox_set_separator_pos(nbox, places);
}


BOOL ui_comm_numedit_is_on_edit(control_t *p_ctrl)
{
  control_t *nbox;
  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_is_on_edit(nbox);
}


/*********************************************************************
 * COMMON STATIC
 *********************************************************************/
control_t *ui_comm_static_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw)
{
  return _comm_ctrl_create(COMM_STATIC, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_static_set_param(control_t *p_ctrl, u32 content_type)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_type(txt, content_type);
}


void ui_comm_static_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_static_set_content_by_strid(control_t *p_ctrl, u16 strid)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_strid(txt, strid);
}


void ui_comm_static_set_content_by_dec(control_t *p_ctrl, s32 num)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_dec(txt, num);
}


void ui_comm_static_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_ascstr(txt, ascstr);
}


void ui_comm_static_set_content_by_unistr(control_t *p_ctrl, u16 *unistr)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_unistr(txt, unistr);
}


void ui_comm_static_set_rstyle(control_t *cont,
                               u8 rsi_cont,
                               u8 rsi_stxt,
                               u8 rsi_ctrl)
{
  control_t *p_ctrl;

  ctrl_set_rstyle(cont, rsi_cont, rsi_cont, rsi_cont);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  ctrl_set_rstyle(p_ctrl, rsi_stxt, rsi_stxt, rsi_stxt);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ctrl_set_rstyle(p_ctrl, rsi_ctrl, rsi_ctrl, rsi_ctrl);
}


void ui_comm_static_set_align_type(control_t *cont,
                                   u16 stxt_ox,
                                   u32 stxt_align,
                                   u16 ctrl_ox,
                                   u32 ctrl_align)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  text_set_align_type(p_ctrl, stxt_align);
  text_set_offset(p_ctrl, stxt_ox, 0);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  text_set_align_type(p_ctrl, ctrl_align);
  text_set_offset(p_ctrl, ctrl_ox, 0);
}


u32 ui_comm_static_get_content(control_t *cont)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);

  return (u32)text_get_content(p_ctrl);
}


/**************************************************************
   COMMON PASSWORD EDIT
 **************************************************************/
static RET_CODE on_edit_get_focus(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ebox_enter_edit(p_ctrl);
  return ERR_NOFEATURE;
}


static RET_CODE on_edit_lost_focus(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  ebox_exit_edit(p_ctrl);
  return ERR_NOFEATURE;
}


control_t *ui_comm_pwdedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_PWDEDIT, parent, ctrl_id, x, y, lw, rw);
  ;
}


void ui_comm_pwdedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_pwdedit_set_param(control_t *p_ctrl, u16 maxtext)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_maxtext(ebox, maxtext);
}

void ui_comm_pwdedit_set_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_hide_mask(ebox, bef_mask, aft_mask);
}


u32 ui_comm_pwdedit_get_value(control_t *p_ctrl)
{
  control_t *ebox;
  u16 buf[SS_PWD_LENGTH + 1];
  u16 *str = NULL;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);

  memcpy(buf, str, SS_PWD_LENGTH * sizeof(u16));
  buf[SS_PWD_LENGTH] = '\0';

  return (u32)convert_dec_str_to_i(buf);
}

u32 ui_comm_pwdedit_get_value_sv(control_t *p_ctrl)
{
  control_t *ebox;
  u16 buf[CAS_MAX_PIN_LEN + 1];
  u16 *str = NULL;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);

  memcpy(buf, str, CAS_MAX_PIN_LEN * sizeof(u16));
  buf[CAS_MAX_PIN_LEN] = '\0';

  return (u32)convert_dec_str_to_i(buf);
}

u32 ui_comm_is_pwdedit_enable(control_t *p_ctrl)
{
  control_t *ebox;
  u16 *str = NULL;
  u16 max_len = 0xFFFF;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);
  max_len = ebox_get_maxtext(ebox);

  if(uni_strlen(str) == max_len)
  {
    return PWD_LENTH_OK;
  }
  else if(uni_strlen(str) == 0)
  {
    return PWD_LENTH_NONE;
  }
  else
  {
    return PWD_LENTH_ERR;
  }
}


void ui_comm_pwdedit_empty_value(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_empty_content(ebox);
}

BOOL ui_comm_pwdedit_is_full(control_t *p_ctrl)
{
  control_t *ebox;
  u16 *str = NULL;
  u16 leng = 0;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);
  leng = ebox_get_maxtext(ebox);
  if(uni_strlen(str) == leng)
    return TRUE;
  else 
    return FALSE;
}


#define T9_INVALID        0xFF

#define T9_MAX_KEY        10
#define T9_MAX_OPT        7

#define T9_INPUT_TMOUT    2000

enum local_msg
{
  MSG_TIMEOUT = MSG_LOCAL_BEGIN + 1000,
};

static struct t9_curninfo
{
  u8 root_id;
  BOOL is_upper;

  u8 curn_bit;
  u8 curn_key;
} g_t9_curninfo = {T9_INVALID, FALSE, T9_INVALID, T9_INVALID};

static u8 t9_opt[T9_MAX_KEY][T9_MAX_OPT] =
{
  {'0', ' ', '_', '-', },
  {'1', '?', '!', '@', '.', ',', '*'},
  {'2', 'a', 'b', 'c', },
  {'3', 'd', 'e', 'f', },
  {'4', 'g', 'h', 'i', },
  {'5', 'j', 'k', 'l', },
  {'6', 'm', 'n', 'o', },
  {'7', 'p', 'q', 'r', 's', },
  {'8', 't', 'u', 'v', },
  {'9', 'w', 'x', 'y', 'z'},
};

static u8 t9_bit_cnt[T9_MAX_KEY] =
{
  4,
  7,
  4,
  4,
  4,
  4,
  4,
  5,
  4,
  5,
};

static u8 t9edit_get_ascchar(u16 key)
{
  u8 ch;

  g_t9_curninfo.curn_bit %= t9_bit_cnt[key];

  ch = t9_opt[key][g_t9_curninfo.curn_bit];

  if(isalpha(ch))
  {
    if(g_t9_curninfo.is_upper)
    {
      ch = toupper(ch);
    }
  }

  return ch;
}


static void t9edit_notify_root(u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;
  msgproc_t proc;

  p_root = fw_find_root_by_id(g_t9_curninfo.root_id);
  if(p_root != NULL)
  {
    proc = ctrl_get_proc(p_root);
    proc(p_root, msg, para1, para2);
  }
}


static RET_CODE on_t9_input(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 key, asc_char;
  u32 strlen;

  key = msg & MSG_DATA_MASK;
  strlen = uni_strlen((u16 *)ebox_get_content(p_ctrl));

  if((g_t9_curninfo.curn_bit == T9_INVALID)
    && (strlen == ebox_get_maxtext(p_ctrl)))
  {
    //do nothing, max length
  }
  else
  {
    if((key == g_t9_curninfo.curn_key)
      && (g_t9_curninfo.curn_bit != T9_INVALID))
    {
      //input the same char before time out.
      g_t9_curninfo.curn_bit++;
      fw_tmr_reset(g_t9_curninfo.root_id, MSG_TIMEOUT, T9_INPUT_TMOUT);
      ebox_back_space(p_ctrl);
      asc_char = t9edit_get_ascchar(key);
      ebox_input_char(p_ctrl, MSG_CHAR | asc_char);
    }
    else
    {
      //another key, or input the same key after timeout.
      g_t9_curninfo.curn_bit = 0;
      asc_char = t9edit_get_ascchar(key);
      ebox_input_char(p_ctrl, MSG_CHAR | asc_char);
      fw_tmr_destroy(g_t9_curninfo.root_id, MSG_TIMEOUT);
      fw_tmr_create(g_t9_curninfo.root_id, MSG_TIMEOUT, T9_INPUT_TMOUT, FALSE);
    }
    t9edit_notify_root(MSG_T9_CHANGED, (u32)ebox_get_content(p_ctrl), 0);
    //paint control
    ctrl_paint_ctrl(p_ctrl, FALSE);
    g_t9_curninfo.curn_key = key;
  }

  return SUCCESS;
}


static RET_CODE on_t9_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  g_t9_curninfo.root_id = T9_INVALID;
  return ERR_NOFEATURE;
}


static RET_CODE on_t9_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  g_t9_curninfo.curn_bit = T9_INVALID;
  fw_tmr_destroy(g_t9_curninfo.root_id, MSG_TIMEOUT);
  t9edit_notify_root(MSG_T9_CHANGED,
                     (u32)ebox_get_content(p_ctrl), 0);
  return SUCCESS;
}


control_t *ui_comm_t9edit_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw,
                                 u8 root_id)
{
  control_t *p_ctrl;

  if(g_t9_curninfo.root_id != T9_INVALID)  // is used
  {
    MT_ASSERT(0);
    return NULL;
  }

  if((p_ctrl =
        _comm_ctrl_create(COMM_T9EDIT, parent, ctrl_id, x, y, lw,
                          rw)) != NULL)
  {
    g_t9_curninfo.root_id = root_id;
    g_t9_curninfo.is_upper = 0;
    g_t9_curninfo.curn_bit = T9_INVALID;
    g_t9_curninfo.curn_key = T9_INVALID;
  }

  return p_ctrl;
}


void ui_comm_t9edit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_t9edit_set_param(control_t *p_ctrl, u16 maxtext)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_maxtext(ebox, maxtext);
}


void ui_comm_t9edit_set_upper(control_t *p_ctrl, BOOL is_upper)
{
  if(g_t9_curninfo.root_id != T9_INVALID)
  {
    g_t9_curninfo.is_upper = is_upper;
  }
}


void ui_comm_t9edit_set_content_by_unistr(control_t *p_ctrl, u16 *unistr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_unistr(ebox, unistr);
}


void ui_comm_t9edit_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_ascstr(ebox, (char *)ascstr);
}


u16 *ui_comm_t9edit_get_content(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return ebox_get_content(ebox);
}


BOOL ui_comm_t9edit_backspace(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  if(ebox_back_space(ebox))
  {
    t9edit_notify_root(MSG_T9_CHANGED, (u32)ebox_get_content(ebox), 0);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*********************************************************************
 * COMMON PROGRESS BAR
 *********************************************************************/
control_t *ui_comm_bar_create(control_t *parent,
                              u8 ctrl_id,
                              u16 x,
                              u16 y,
                              u16 w,
                              u16 h,
                              u16 txt_x,
                              u16 txt_y,
                              u16 txt_w,
                              u16 txt_h,
                              u16 percent_x,
                              u16 percent_y,
                              u16 percent_w,
                              u16 percent_h)
{
  rect_t rc_cont, rc_tmp;
  control_t *cont, *bar, *txt, *percent;

  // calc the rect of container
  set_rect(&rc_cont, (s16)x, (s16)y,
           (s16)(x + w), (s16)(y + h));
  set_rect(&rc_tmp, (s16)txt_x, (s16)txt_y,
           (s16)(txt_x + txt_w), (s16)(txt_y + txt_h));
  generate_boundrect(&rc_cont, &rc_cont, &rc_tmp);

  set_rect(&rc_tmp, (s16)percent_x, (s16)percent_y,
           (s16)(percent_x + percent_w), (s16)(percent_y + percent_h));
  generate_boundrect(&rc_cont, &rc_cont, &rc_tmp);

  // create
  cont = ctrl_create_ctrl((u8 *)CTRL_CONT, ctrl_id,
                          (u16)(rc_cont.left), (u16)(rc_cont.top),
                          (u16)RECTW(rc_cont), (u16)RECTH(rc_cont),
                          parent, 0);
  ctrl_set_rstyle(cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_BAR_TXT,
                         (u16)(txt_x - rc_cont.left), (u16)(txt_y - rc_cont.top),
                         txt_w, txt_h,
                         cont, 0);
  text_set_align_type(txt, STL_LEFT | STL_VCENTER);

  bar = ctrl_create_ctrl((u8 *)CTRL_PBAR, IDC_COMM_BAR,
                         (u16)(x - rc_cont.left), (u16)(y - rc_cont.top), w, h,
                         cont, 0);
  pbar_set_mid_rect(bar,
                    COMM_BAR_BORDER, 0,
                    (s16)(w - COMM_BAR_BORDER), (s16)(h));

  percent = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_BAR_PERCENT,
                             (u16)(percent_x - rc_cont.left),
                             (u16)(percent_y - rc_cont.top),
                             percent_w, percent_h,
                             cont, 0);
  text_set_align_type(percent, STL_RIGHT | STL_VCENTER);
  text_set_content_type(percent, TEXT_STRTYPE_UNICODE);
  //text_set_content_by_ascstr(percent, " ");

  return cont;
}


void ui_comm_bar_set_style(control_t *cont,
                           u8 rsi_bar,
                           u8 rsi_mid,
                           u8 rsi_txt,
                           u8 fsi_txt,
                           u8 rsi_percent,
                           u8 fsi_percent)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_TXT);
  ctrl_set_rstyle(p_ctrl, rsi_txt, rsi_txt, rsi_txt);
  text_set_font_style(p_ctrl, fsi_txt, fsi_txt, fsi_txt);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  ctrl_set_rstyle(p_ctrl, rsi_bar, rsi_bar, rsi_bar);
  pbar_set_rstyle(p_ctrl, rsi_mid, RSI_IGNORE, INVALID_RSTYLE_IDX);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
  ctrl_set_rstyle(p_ctrl, rsi_percent, rsi_percent, rsi_percent);
  text_set_font_style(p_ctrl, fsi_percent, fsi_percent, fsi_percent);
}


void ui_comm_bar_set_param(control_t *cont,
                           u16 rsc_id,
                           u16 min,
                           u16 max,
                           u16 step)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_TXT);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, rsc_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  pbar_set_count(p_ctrl, min, max, step);
}

BOOL ui_comm_bar_update(control_t *cont, u16 val, BOOL is_force)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, val);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char *)str_buf, "%d%%", val);
    text_set_content_by_ascstr(p_ctrl, str_buf);

    is_redraw = TRUE;
  }

  return is_redraw;
}
BOOL ui_comm_intensity_bar_update(control_t *cont, u8 val,u8 percent, BOOL is_force)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, percent);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char *)str_buf, "%ddBuV", val);
    text_set_content_by_ascstr(p_ctrl, str_buf);

    is_redraw = TRUE;
  }

  return is_redraw;
}

BOOL ui_comm_snr_bar_update(control_t *cont, u8 val,u8 percent, BOOL is_force)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, percent);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char *)str_buf, "%ddB", val);
    text_set_content_by_ascstr(p_ctrl, str_buf);

    is_redraw = TRUE;
  }

  return is_redraw;
}
BOOL ui_comm_get_ber_string(double ber_value,u8 *str)
{
    double value = 0;
    u8 cnk = 0;
    u8 string_ber[20] = {0};
    value = ber_value;

    if(value >= 1) /*****/
    {
     value = value * 10;
     while(value >= 10)
     {
       value = value / 10;
       cnk ++;
     }
     if(cnk > 0)
     {
        sprintf((char *)string_ber,"%ld.%ldE+%d",(u32)(value * 10) / 10,(u32)(value * 10) % 10,cnk - 1);
     }
     else
     {
         sprintf((char *)string_ber,"0E+0");
     }
    }
    else if(value == 0)
    {
        sprintf((char *)string_ber,"0E+0");
    }
    else
    {
       value = value * 10;
       while(value < 10)
       {
            value = value * 10;
        cnk ++;
       }
       sprintf((char *)string_ber,"%ld.%ldE-%d",(u32)value / 10,(u32)value % 10,cnk);
    }
    strncpy((char *)str,(char *)string_ber,strlen((char *)string_ber));

    return TRUE;
}
BOOL ui_comm_ber_string_update(control_t *cont, double val, BOOL is_force)
{
  control_t *p_ctrl;
  u8 str_buf[20] = {0};
  BOOL is_redraw = FALSE;
  static double ber_c = 1.0;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(ber_c != val || is_force)
  {
    ber_c = val;
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    //sprintf((char *)str_buf, "%2.1E", ber_c);
    ui_comm_get_ber_string(ber_c,str_buf);
    text_set_content_by_ascstr(p_ctrl, str_buf);
    is_redraw = TRUE;
  }

  return is_redraw;
}

BOOL ui_comm_tp_bar_update(control_t *cont, u16 val, BOOL is_force,u8 *pox)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, val);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char *)str_buf, "%d%s", val,pox);
    text_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
    text_set_content_by_ascstr(p_ctrl, str_buf);

    is_redraw = TRUE;
  }

  return is_redraw;
}


void ui_comm_bar_paint(control_t *cont, BOOL is_force)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  ctrl_paint_ctrl(p_ctrl, is_force);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
  ctrl_paint_ctrl(p_ctrl, is_force);
}


void ui_comm_set_tp_postfix(u16 *postfix, u32 tp_cnt, u32 max_length)
{
  u8 ascstr[20 + 1];

  sprintf((char *)ascstr, "/%d", (u16)tp_cnt);
  if(max_length < 20)
  {
    ascstr[max_length] = '\0';
  }
  str_asc2uni(ascstr, postfix);
}


// common controls keymap
BEGIN_KEYMAP(ui_comm_select_keymap, NULL)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
END_KEYMAP(ui_comm_select_keymap, NULL)

BEGIN_KEYMAP(ui_comm_static_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ui_comm_static_keymap, NULL)

BEGIN_KEYMAP(ui_comm_t9_keymap, NULL)
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
END_KEYMAP(ui_comm_t9_keymap, NULL)

BEGIN_KEYMAP(ui_comm_num_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_EXIT, MSG_UNSELECT)
ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_comm_num_keymap, ui_comm_t9_keymap)

BEGIN_KEYMAP(ui_comm_tbox_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_UP, MSG_INCREASE)
ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)

ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_EXIT, MSG_UNSELECT)
ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_comm_tbox_keymap, ui_comm_t9_keymap)

BEGIN_KEYMAP(ui_comm_edit_keymap, NULL)
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
END_KEYMAP(ui_comm_edit_keymap, NULL)

// common control's msgproc
BEGIN_MSGPROC(ui_comm_num_proc, nbox_class_proc)
ON_COMMAND(MSG_UNSELECT, on_num_unselect)
ON_COMMAND(MSG_OUTRANGE, on_num_out_range)
ON_COMMAND(MSG_FOCUS_LEFT, on_num_spin_down)
ON_COMMAND(MSG_FOCUS_RIGHT, on_num_spin_up)
ON_COMMAND(MSG_FOCUS_UP, on_num_focus_up)
ON_COMMAND(MSG_FOCUS_DOWN, on_num_focus_down)
ON_COMMAND(MSG_GETFOCUS, on_num_get_focus)
END_MSGPROC(ui_comm_num_proc, nbox_class_proc)


BEGIN_MSGPROC(ui_comm_edit_proc, ebox_class_proc)
ON_COMMAND(MSG_GETFOCUS, on_edit_get_focus)
ON_COMMAND(MSG_LOSTFOCUS, on_edit_lost_focus)
END_MSGPROC(ui_comm_edit_proc, ebox_class_proc)

BEGIN_MSGPROC(ui_comm_t9_proc, ui_comm_edit_proc)
ON_COMMAND(MSG_NUMBER, on_t9_input)
ON_COMMAND(MSG_DESTROY, on_t9_destory)
ON_COMMAND(MSG_TIMEOUT, on_t9_timeout)
END_MSGPROC(ui_comm_t9_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(ui_comm_time_proc, tbox_class_proc)
ON_COMMAND(MSG_UNSELECT, on_time_unselect)
ON_COMMAND(MSG_OUTRANGE, on_time_out_range)
END_MSGPROC(ui_comm_time_proc, tbox_class_proc)

