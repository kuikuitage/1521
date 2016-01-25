/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_editbox.c
   this file  implement the functions defined in  ctrl_editbox.h .
  */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "lib_util.h"

#include "osd.h"
#include "gpe.h"
#include "surface.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_editbox.h"

static void _ebox_alloc_buf(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_str = (u16 *)ctrl_unistr_alloc(EBOX_MAX_STRLEN + 1);
  p_str[0] = '\0';

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_data->str_char = (u32)p_str;
}


static void _ebox_free_buf(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  if(p_data->str_char != 0)
  {
    ctrl_unistr_free((void *)p_data->str_char);
    p_data->str_char = 0;
  }

  if((p_ctrl->priv_attr & EBOX_ARABIC_MASK) && (p_data->str_arabic != 0))
  {
    ctrl_unistr_free((void *)p_data->str_arabic);
    p_data->str_arabic = 0;
  }
}


static u8 _ebox_get_fstyle(u8 attr, ctrl_ebox_data_t *p_data)
{
  u8 font = 0;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_data->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_data->g_fstyle;
      break;

    default:
      font = p_data->n_fstyle;
  }

  return font;
}


static void _ebox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_ebox_data_t *p_data = NULL;
  u8 attr = 0, font = 0;
  u32 len = 0;
  rect_t str_rect;
  u16 content[EBOX_MAX_STRLEN + 1] = {0};

  MT_ASSERT(p_ctrl != NULL);

  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  //draw title string
  if((ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl)))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _ebox_get_fstyle(attr, p_data);

  ctrl_get_client_rect(p_ctrl, &str_rect);

  // prepare string buffer for drawing
  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_HIDE)
  {
    // on hide mode
    len = uni_strlen((u16 *)p_data->str_char);
    uni_strnset(&content[0], char_asc2uni(p_data->aft_hmask), len);

    if(p_data->str_maxtext > 0)
    {
      len = uni_strlen(content);
      if(len < p_data->str_maxtext)
      {
        uni_strnset(&content[len], char_asc2uni(p_data->bef_hmask),
                    p_data->str_maxtext - len);
      }
    }
  }
  else
  {
    uni_strncpy(content, (u16 *)p_data->str_char, EBOX_MAX_STRLEN);

    if((p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
    {
      // on edit stage
      u16 post[2];

      post[0] = char_asc2uni('_');
      post[1] = '\0';

      uni_strcat(content, post, EBOX_MAX_STRLEN);
    }
  }

  gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr, p_data->str_left,
                  p_data->str_top, p_data->str_l_space,
                  content, font, STRDRAW_NORMAL);
}


RET_CODE ebox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_ebox_data_t *p_default_data = NULL;

  register_info.data_size = sizeof(ctrl_ebox_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_ebox_data_t *)register_info.p_default_data;
  
  // initialize the default control of editbox class
  p_default_ctrl->priv_attr = EBOX_WORKTYPE_EDIT | STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = ebox_class_proc;
  p_default_ctrl->p_paint = _ebox_draw;

  // initalize the default data of editbox class
  p_default_data->bef_hmask = char_asc2uni('-');
  p_default_data->aft_hmask = char_asc2uni('*');
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_EBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void ebox_set_content_by_ascstr(control_t *p_ctrl, char *p_ascstr)
{
  u32 len = strlen(p_ascstr);
  u16 *p_content = ebox_get_content(p_ctrl);

  MT_ASSERT(p_ctrl != NULL && p_ascstr != NULL);

  if(p_content == NULL)
  {
    return;
  }

  if(len > EBOX_MAX_STRLEN)
  {
    len = EBOX_MAX_STRLEN;
  }

  str_nasc2uni((u8 *)p_ascstr, p_content, len);
}


void ebox_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr)
{
  u32 len = uni_strlen(p_unistr);
  u16 *p_content = ebox_get_content(p_ctrl);

  MT_ASSERT(p_ctrl != NULL && p_unistr != NULL);

  if(p_content == NULL)
  {
    return;
  }

  if(len > EBOX_MAX_STRLEN)
  {
    len = EBOX_MAX_STRLEN;
  }

  uni_strncpy(p_content, p_unistr, len);
}


u16 *ebox_get_content(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  return (u16 *)p_data->str_char;
}


void ebox_set_line_gap(control_t *p_ctrl, u8 l_space)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_data->str_l_space = l_space;
}


void ebox_set_font_style(control_t *p_ctrl,
                         u8 n_fstyle,
                         u8 h_fstyle,
                         u8 g_fstyle)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_data->n_fstyle = n_fstyle;
  p_data->h_fstyle = h_fstyle;
  p_data->g_fstyle = g_fstyle;
}


void ebox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_data->str_left = left;
  p_data->str_top = top;
}


void ebox_set_maxtext(control_t *p_ctrl, u16 max)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  if(max > EBOX_MAX_STRLEN)
  {
    max = EBOX_MAX_STRLEN;
  }
  p_data->str_maxtext = max;
}


u16 ebox_get_maxtext(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;

  return p_data->str_maxtext;
}


void ebox_set_worktype(control_t *p_ctrl, u32 worktype)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~EBOX_WORKTYPE_MASK);
  p_ctrl->priv_attr |= (worktype & EBOX_WORKTYPE_MASK);
}


void ebox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~EBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & EBOX_ALIGN_MASK); //set new style
}

void ebox_set_arabic_type(control_t *p_ctrl, u32 style)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str_ara = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~EBOX_ARABIC_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & EBOX_ARABIC_MASK); //set new style

  if(p_ctrl->priv_attr & EBOX_ARABIC_MASK) //alloc buffer to save arabic base code
  {
    p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
    p_str_ara = (u16 *)ctrl_unistr_alloc(EBOX_MAX_STRLEN * 2 + 1);
    p_str_ara[0] = '\0';
    p_data->str_arabic = (u32)p_str_ara;
  }

}


BOOL ebox_set_hide_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_HIDE)
  {
    // on hide mode
    return FALSE;
  }

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;

  p_data->bef_hmask = bef_mask;
  p_data->aft_hmask = aft_mask;

  return TRUE;
}


BOOL ebox_enter_edit(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_ctrl->priv_attr |= EBOX_HL_STATUS_MASK;

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  return TRUE;
}


BOOL ebox_exit_edit(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_ctrl->priv_attr &= (~EBOX_HL_STATUS_MASK);

  // notify leave edit
  ctrl_process_msg(p_ctrl, MSG_CHANGED, p_data->str_char, 0);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  return TRUE;
}


BOOL ebox_back_space(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }
  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  if(p_ctrl->priv_attr & EBOX_ARABIC_MASK)
  {
    p_str = (u16 *)p_data->str_arabic;
    len = uni_strlen(p_str);
    if(len > 0)
    {
      *(p_str + len - 1) = '\0';
      ara_str_convert((u16 *)p_data->str_arabic, (u16 *)p_data->str_char, EBOX_MAX_STRLEN);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      return TRUE;
    }
  }
  else
  {
    p_str = (u16 *)p_data->str_char;
    len = uni_strlen(p_str);
    if(len > 0)
    {
      *(p_str + len - 1) = '\0';
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      return TRUE;
    }
  }

  return FALSE;
}


BOOL ebox_empty_content(control_t *p_ctrl)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;
  u32 length = 0;

  MT_ASSERT(p_ctrl != NULL);

//  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
//  {
//    return FALSE;
//  }
  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_str = (u16 *)p_data->str_char;
  length = uni_strlen(p_str);
  if(length > 0)
  {
    p_str[0] = '\0';
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    return TRUE;
  }

  return FALSE;
}

BOOL ebox_input_ara_uni(control_t *p_ctrl, u16 uni_code)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_str = (u16 *)p_data->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if((len >= p_data->str_maxtext) && (p_data->str_maxtext != 0)) /*zero means no limit*/
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
    return FALSE;
  }
  ara_input_letter_convert((u16 *)p_data->str_arabic, p_str, uni_code);

  len = uni_strlen(p_str);
  *(p_str + len) = '\0';

  // check for maxtext
  if((len >= p_data->str_maxtext) && (p_data->str_maxtext != 0)) /*zero means no limit*/
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}

BOOL ebox_input_uchar(control_t *p_ctrl, u16 uchar)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_str = (u16 *)p_data->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if(((len >= p_data->str_maxtext) && p_data->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
    return FALSE;
  }

  *(p_str + len) = uchar;
  *(p_str + len + 1) = '\0';

  // check for maxtext
  len = uni_strlen(p_str);
  if(((len >= p_data->str_maxtext) && p_data->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}

BOOL ebox_input_char(control_t *p_ctrl, u16 msg)
{
  ctrl_ebox_data_t *p_data = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_ebox_data_t *)p_ctrl->priv_data;
  p_str = (u16 *)p_data->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if(((len >= p_data->str_maxtext) && p_data->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
    return FALSE;
  }

  *(p_str + len) = char_asc2uni((u8)(msg & MSG_DATA_MASK));
  *(p_str + len + 1) = '\0';

  // check for maxtext
  len = uni_strlen(p_str);
  if(((len >= p_data->str_maxtext) && p_data->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_data->str_char, p_data->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}


static RET_CODE on_ebox_create(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_ctrl != NULL);

  ret = ctrl_default_proc(p_ctrl, msg, para1, para2);
  MT_ASSERT(ret == SUCCESS);

  _ebox_alloc_buf(p_ctrl);
  return SUCCESS;
}


static RET_CODE on_ebox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _ebox_free_buf(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_ebox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
  {
    if(ebox_enter_edit(p_ctrl))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  else
  {
    if(ebox_exit_edit(p_ctrl))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}


static RET_CODE on_ebox_backspace(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(ebox_back_space(p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_ebox_empty(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(ebox_empty_content(p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_ebox_char(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK)
  {
    if(ebox_input_char(p_ctrl, msg))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}

static RET_CODE on_ebox_ara_uni(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & EBOX_HL_STATUS_MASK) && (p_ctrl->priv_attr & EBOX_ARABIC_MASK))
  {
    if(ebox_input_ara_uni(p_ctrl, para1))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(ebox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_CHAR, on_ebox_char)
ON_COMMAND(MSG_ARA_UNI, on_ebox_ara_uni)
ON_COMMAND(MSG_SELECT, on_ebox_select)
ON_COMMAND(MSG_BACKSPACE, on_ebox_backspace)
ON_COMMAND(MSG_EMPTY, on_ebox_empty)
ON_COMMAND(MSG_CREATE, on_ebox_create)
ON_COMMAND(MSG_DESTROY, on_ebox_destroy)
END_CTRLPROC(ebox_class_proc, ctrl_default_proc)
