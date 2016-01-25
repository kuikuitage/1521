/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_numbox.c
   this file  implement the functions defined in  ctrl_numbox.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a numbox
   control.
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

#include "ctrl_numbox.h"

/*!
  Macro of noninline
  */
#if defined(NXP)
/*!
  Macro for noninline function:no need in NXP
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#elif defined(WIN32)
/*!
  Macro for noninline function:no need in x86
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#else
/*!
  Macro for noninline function
  */
#define NONINLINE __attribute__ ((noinline))
#endif


static NONINLINE u32 _nbox_get_base(u32 bits, BOOL is_hex)
{
  u32 i = 0, base = 1;

  if(is_hex)
  {
    for(i = 0; i < bits; i++)
    {
      base = base * 16;
    }
  }
  else
  {
    for(i = 0; i < bits; i++)
    {
      base = base * 10;
    }
  }
  return base;
}


static u8 _nbox_get_curn_bit(ctrl_nbox_data_t *p_data, BOOL is_hex)
{
  if(is_hex)
  {
    if(p_data->curn_bit == (p_data->bit_length - 1))
    {
      return (u8)(p_data->num / _nbox_get_base(p_data->curn_bit, TRUE));
    }
    else
    {
      return (u8)(p_data->num %
                  _nbox_get_base(p_data->curn_bit + 1, TRUE) /
                  _nbox_get_base(p_data->curn_bit, TRUE));
    }
  }
  else
  {
    if(p_data->curn_bit == (p_data->bit_length - 1))
    {
      return (u8)(p_data->num / _nbox_get_base(p_data->curn_bit, FALSE)); 
    }
    else
    {
      return (u8)(p_data->num %
                  _nbox_get_base(p_data->curn_bit + 1, FALSE) /
                  _nbox_get_base(p_data->curn_bit, FALSE)); 
    }
  }
}


static void _nbox_set_curn_bit(ctrl_nbox_data_t *p_data, u8 num, BOOL is_hex)
{
  p_data->num -= _nbox_get_curn_bit(p_data, is_hex) * _nbox_get_base(p_data->curn_bit, is_hex);
  p_data->num += num * _nbox_get_base(p_data->curn_bit, is_hex);
}


static u8 _nbox_get_underline_pos(ctrl_nbox_data_t *p_data)
{
  u8 pos = p_data->bit_length - p_data->curn_bit - 1;

  // skip decimal
  if(p_data->curn_bit < p_data->separator_pos)
  {
    pos++;
  }

  return pos;
}


static u8 _nbox_get_fstyle(u8 attr, ctrl_nbox_data_t *p_data)
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


static void _nbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_nbox_data_t *p_data = NULL;
  u8 attr = 0, font = 0;
  u16 str_num[NUM_STRING_LEN + 1];
  u16 *p_num_str = NULL;
  rect_t str_rect;

  MT_ASSERT(p_ctrl != NULL);

  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _nbox_get_fstyle(attr, p_data);

  ctrl_get_client_rect(p_ctrl, &str_rect);

  // alloc buffer
  p_num_str = ctrl_unistr_alloc(
    NUM_STRING_LEN + NBOX_ITEM_NUM_XFIX_LENGTH * 2);

  // convert num to str
  switch(p_ctrl->priv_attr & NBOX_NUMTYPE_MASK)
  {
    case NBOX_NUMTYPE_DEC:
      convert_i_to_dec_str_ex(str_num, p_data->num, p_data->bit_length);
      if(p_data->separator_pos != 0)
      {
        insert_aschar_into_unistr(str_num, p_data->separator,
                                  p_data->separator_pos, NUM_STRING_LEN);
      }
      break;
    case NBOX_NUMTYPE_HEX:
      convert_i_to_hex_str_ex(str_num, p_data->num, p_data->bit_length);
      break;
    default:
      MT_ASSERT(0);
  }

  // add prefix
  if(p_data->prefix != 0)
  {
    switch(p_ctrl->priv_attr & NBOX_ITEM_PREFIX_TYPE_MASK)
    {
      case NBOX_ITEM_PREFIX_TYPE_STRID:
        gui_get_string((u16)p_data->prefix, p_num_str,
                       NBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      case NBOX_ITEM_PREFIX_TYPE_EXTSTR:
        uni_strcat(p_num_str, (u16 *)p_data->prefix,
                   NBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      default:
        MT_ASSERT(0);
    }
  }

  // add content
  uni_strcat(&p_num_str[uni_strlen(p_num_str)],
             str_num,
             NUM_STRING_LEN + NBOX_ITEM_NUM_XFIX_LENGTH);

  // add postfix
  if(p_data->postfix != 0)
  {
    switch(p_ctrl->priv_attr & NBOX_ITEM_POSTFIX_TYPE_MASK)
    {
      case NBOX_ITEM_POSTFIX_TYPE_STRID:
        gui_get_string((u16)p_data->postfix,
                       &p_num_str[uni_strlen(p_num_str)],
                       NBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      case NBOX_ITEM_POSTFIX_TYPE_EXTSTR:
        uni_strcat(&p_num_str[uni_strlen(p_num_str)],
                   (u16 *)p_data->postfix,
                   NBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      default:
        MT_ASSERT(0);
    }
  }

  // draw
  if(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK)
  {
    if(p_ctrl->priv_attr & NBOX_HL_TYPE_MASK)
    {
      gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                      p_data->str_left, p_data->str_top,
                      0, p_num_str, font,
                      MAKE_DRAW_STYLE(STRDRAW_WITH_INVERT,
                                      _nbox_get_underline_pos(p_data)));
    }
    else
    {
      gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                      p_data->str_left, p_data->str_top,
                      0, p_num_str, font,
                      MAKE_DRAW_STYLE(STRDRAW_WITH_UNDERLINE,
                                      _nbox_get_underline_pos(p_data)));
    }
  }
  else
  {
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                    p_data->str_left, p_data->str_top,
                    0, p_num_str, font,
                    MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
  }

  // free buffer
  ctrl_unistr_free(p_num_str);
}


static BOOL _nbox_increase(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  switch(p_ctrl->priv_attr & NBOX_NUMTYPE_MASK)
  {
    case NBOX_NUMTYPE_DEC:
      if(_nbox_get_curn_bit(p_data, FALSE) == 9)
      {
        p_data->num -= 9 * _nbox_get_base(p_data->curn_bit, FALSE);
      }
      else
      {
        p_data->num += _nbox_get_base(p_data->curn_bit, FALSE);
      }
      break;
    case NBOX_NUMTYPE_HEX:
      if(_nbox_get_curn_bit(p_data, TRUE) == 15)
      {
        p_data->num -= 15 * _nbox_get_base(p_data->curn_bit, TRUE);
      }
      else
      {
        p_data->num += _nbox_get_base(p_data->curn_bit, TRUE);
      }
      break;
    default:
      MT_ASSERT(0);
  }

  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
  return TRUE;
}


static BOOL _nbox_decrease(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  switch(p_ctrl->priv_attr & NBOX_NUMTYPE_MASK)
  {
    case NBOX_NUMTYPE_DEC:
      if(_nbox_get_curn_bit(p_data, FALSE) == 0)
      {
        p_data->num += 9 * _nbox_get_base(p_data->curn_bit, FALSE);
      }
      else
      {
        p_data->num -= _nbox_get_base(p_data->curn_bit, FALSE);
      }
      break;
    case NBOX_NUMTYPE_HEX:
      if(_nbox_get_curn_bit(p_data, TRUE) == 0)
      {
        p_data->num += 15 * _nbox_get_base(p_data->curn_bit, TRUE);
      }
      else
      {
        p_data->num -= _nbox_get_base(p_data->curn_bit, TRUE);
      }      
      break;
    default:
      MT_ASSERT(0);
  }

  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
  return TRUE;
}


static BOOL _num_set_fix_by_strid(control_t *p_ctrl, BOOL is_prefix, u16 strid)
{
  ctrl_nbox_data_t *p_num_data = NULL;

  p_num_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  if(is_prefix)
  {
    if((p_ctrl->priv_attr & NBOX_ITEM_PREFIX_TYPE_MASK) !=
       NBOX_ITEM_PREFIX_TYPE_STRID)
    {
      return FALSE;
    }
    p_num_data->prefix = strid;
  }
  else
  {
    if((p_ctrl->priv_attr & NBOX_ITEM_POSTFIX_TYPE_MASK) !=
       NBOX_ITEM_POSTFIX_TYPE_STRID)
    {
      return FALSE;
    }
    p_num_data->postfix = strid;
  }
  return TRUE;
}


static BOOL _num_set_fix_by_extstr(control_t *p_ctrl,
                                   BOOL is_prefix,
                                   u16 *p_extstr)
{
  ctrl_nbox_data_t *p_num_data = NULL;

  p_num_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  if(is_prefix)
  {
    if((p_ctrl->priv_attr & NBOX_ITEM_PREFIX_TYPE_MASK) !=
       NBOX_ITEM_PREFIX_TYPE_EXTSTR)
    {
      return FALSE;
    }
    p_num_data->prefix = (u32)p_extstr;
  }
  else
  {
    if((p_ctrl->priv_attr & NBOX_ITEM_POSTFIX_TYPE_MASK) !=
       NBOX_ITEM_POSTFIX_TYPE_EXTSTR)
    {
      return FALSE;
    }
    p_num_data->postfix = (u32)p_extstr;
  }
  return TRUE;
}


RET_CODE nbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_nbox_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of numbox class
  p_default_ctrl->priv_attr = NBOX_NUMTYPE_DEC | STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = nbox_class_proc;
  p_default_ctrl->p_paint = _nbox_draw;

  // initalize the default data of numbox class
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_NBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void nbox_set_separator(control_t *p_ctrl, u8 separator)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  if((p_ctrl->priv_attr & NBOX_NUMTYPE_MASK) == NBOX_NUMTYPE_DEC)
  {
    p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
    p_data->separator = separator;
  }
}


u8 nbox_get_separator(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  if((p_ctrl->priv_attr & NBOX_NUMTYPE_MASK) == NBOX_NUMTYPE_DEC)
  {
    p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
    return p_data->separator;
  }

  return 0;
}


void nbox_set_separator_pos(control_t *p_ctrl, u8 pos)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  if((p_ctrl->priv_attr & NBOX_NUMTYPE_MASK) == NBOX_NUMTYPE_DEC)
  {
    p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

    if(pos < p_data->bit_length)
    {
      p_data->separator_pos = pos;
    }
  }
}


u8 nbox_get_separator_pos(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  if((p_ctrl->priv_attr & NBOX_NUMTYPE_MASK) == NBOX_NUMTYPE_DEC)
  {
    p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

    return p_data->separator_pos;
  }

  return 0;
}


void nbox_set_range(control_t *p_ctrl, s32 min, s32 max, u8 bit_length)
{
  ctrl_nbox_data_t *p_data = NULL;
  s32 base = 0;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_data->min = min;

  switch(p_ctrl->priv_attr & NBOX_NUMTYPE_MASK)
  {
    case NBOX_NUMTYPE_DEC:
      base = _nbox_get_base(bit_length, FALSE) - 1;
      break;
    case NBOX_NUMTYPE_HEX:
      base = _nbox_get_base(bit_length, TRUE) - 1;
      break;
    default:
      MT_ASSERT(0);
      break;
  }  
  
  if(max > base)
  {
    max = base;
  }
  p_data->max = max;
  p_data->bit_length = bit_length;
}


void nbox_get_range(control_t *p_ctrl, s32 *p_min, s32 *p_max)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  *p_min = p_data->min;
  *p_max = p_data->max;
}


void nbox_set_num_by_dec(control_t *p_ctrl, u32 dec)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_data->num = dec;
}


void nbox_set_num_by_hex(control_t *p_ctrl, u32 hex)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_data->num = hex;
}


u32 nbox_get_num(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  return p_data->num;
}


void nbox_set_font_style(control_t *p_ctrl,
                         u8 n_fstyle,
                         u8 h_fstyle,
                         u8 g_fstyle)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_data->n_fstyle = n_fstyle;
  p_data->h_fstyle = h_fstyle;
  p_data->g_fstyle = g_fstyle;
}


void nbox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  p_data->str_left = left;
  p_data->str_top = top;
}

void nbox_set_highlight_type(control_t *p_ctrl, BOOL is_underline)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~NBOX_HL_TYPE_MASK);        //clean old style
  
  if(!is_underline)
  {
    p_ctrl->priv_attr |= (NBOX_HL_TYPE_MASK); //set new style
  }
}

void nbox_set_num_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~NBOX_NUMTYPE_MASK);
  p_ctrl->priv_attr |= (type & NBOX_NUMTYPE_MASK);
}


void nbox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~NBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & NBOX_ALIGN_MASK); //set new style
}


BOOL nbox_enter_edit(control_t *p_ctrl, BOOL is_left)
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
  
  /* DO SOMETHING */
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL nbox_is_outrange(control_t *p_ctrl, u32 *p_border)
{
  ctrl_nbox_data_t *p_data = NULL;
  BOOL is_out = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  if(p_data->num < p_data->min)
  {
    *p_border = p_data->min;
    is_out = TRUE;
  }
  else if(p_data->num > p_data->max)
  {
    *p_border = p_data->max;
    is_out = TRUE;
  }
  else
  {
    is_out = FALSE;
  }

  return is_out;
}


void nbox_exit_edit(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;
  u32 border = 0;

  MT_ASSERT(p_ctrl != NULL);
  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  if(nbox_is_outrange(p_ctrl, &border))
  {
    // set num to border
    p_data->num = border;
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    // notify out of range
    ctrl_process_msg(p_ctrl, MSG_OUTRANGE, p_data->num, border);
  }
  else
  {
    p_ctrl->priv_attr &= (~NBOX_HL_STATUS_MASK);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    // notify leave edit
    ctrl_process_msg(p_ctrl, MSG_CHANGED, p_data->num, 0);
  }
}


BOOL nbox_input_num(control_t *p_ctrl, u16 msg)
{
  ctrl_nbox_data_t *p_data = NULL;
  RET_CODE ret = ERR_NOFEATURE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  switch(p_ctrl->priv_attr & NBOX_NUMTYPE_MASK)
  {
    case NBOX_NUMTYPE_DEC:
      _nbox_set_curn_bit(p_data, (u8)(msg & MSG_DATA_MASK), FALSE);
      break;
    case NBOX_NUMTYPE_HEX:
      _nbox_set_curn_bit(p_data, (u8)(msg & MSG_DATA_MASK), TRUE);
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  // move to next bit
  if(p_data->curn_bit == 0)
  {
    ret = ctrl_process_msg(p_ctrl, MSG_BORDER, FALSE, 0);
  }

  if(ret == ERR_NOFEATURE)
  {
    p_data->curn_bit =
      (p_data->curn_bit + p_data->bit_length - 1) % p_data->bit_length;
  }
  
  return TRUE;
}


BOOL nbox_set_focus(control_t *p_ctrl, u8 focus)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  if(focus < p_data->bit_length)
  {
    p_data->curn_bit = focus;
  }
  return TRUE;
}


u8 nbox_get_focus(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;
  return p_data->curn_bit;
}


BOOL nbox_shift_focus(control_t *p_ctrl, s8 offset)
{
  ctrl_nbox_data_t *p_data = NULL;
  RET_CODE ret = ERR_NOFEATURE;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

/* don't need, it is loop mode
   if(ABS(offset) >= p_data->bit_length)
   {
    return FALSE;
   }
  */
  if((offset > 0) && (p_data->curn_bit + offset) >= p_data->bit_length)
  {
    ret = ctrl_process_msg(p_ctrl, MSG_BORDER, TRUE, 0);
  }
  else if(offset < 0 && (p_data->curn_bit + offset) < 0)
  {
    ret = ctrl_process_msg(p_ctrl, MSG_BORDER, FALSE, 0);
  }

  if(ret == ERR_NOFEATURE)
  {
    p_data->curn_bit =
      (p_data->curn_bit + p_data->bit_length + offset) % p_data->bit_length;
  }
  
  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
  return TRUE;
}


BOOL nbox_is_on_edit(control_t *p_ctrl)
{
  return (BOOL)(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK);
}


void nbox_set_prefix_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~NBOX_ITEM_PREFIX_TYPE_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & NBOX_ITEM_PREFIX_TYPE_MASK);   //set new type
}


BOOL nbox_set_prefix_by_strid(control_t *p_ctrl, u16 strid)
{
  MT_ASSERT(p_ctrl != NULL);
  return _num_set_fix_by_strid(p_ctrl, TRUE, strid);
}


BOOL nbox_set_postfix_by_strid(control_t *p_ctrl, u16 strid)
{
  MT_ASSERT(p_ctrl != NULL);
  return _num_set_fix_by_strid(p_ctrl, FALSE, strid);
}


void nbox_set_postfix_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~NBOX_ITEM_POSTFIX_TYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & NBOX_ITEM_POSTFIX_TYPE_MASK);    //set new type
}


BOOL nbox_set_prefix_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  MT_ASSERT(p_ctrl != NULL);
  return _num_set_fix_by_extstr(p_ctrl, TRUE, p_extstr);
}


BOOL nbox_set_postfix_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  MT_ASSERT(p_ctrl != NULL);
  return _num_set_fix_by_extstr(p_ctrl, FALSE, p_extstr);
}


u32 nbox_get_pretfix(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  return p_data->prefix;
}


u32 nbox_get_postfix(control_t *p_ctrl)
{
  ctrl_nbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_nbox_data_t *)p_ctrl->priv_data;

  return p_data->postfix;
}


static RET_CODE on_nbox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(!(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    nbox_enter_edit(p_ctrl, TRUE);
  }
  else
  {
    nbox_exit_edit(p_ctrl);
  }
  return SUCCESS;
}


static RET_CODE on_nbox_change_focus(control_t *p_ctrl,
                                     u16 msg,
                                     u32 para1,
                                     u32 para2)
{
  s8 offset = (s8)(msg == MSG_FOCUS_LEFT ? 1 : -1);

  MT_ASSERT(p_ctrl != NULL);

  nbox_shift_focus(p_ctrl, offset);
  return SUCCESS;
}


static RET_CODE on_nbox_increase(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(!_nbox_increase(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_nbox_decrease(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(!_nbox_decrease(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_nbox_num(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    nbox_enter_edit(p_ctrl, TRUE);
  }

  if(nbox_input_num(p_ctrl, msg))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
  }
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(nbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_NUMBER, on_nbox_num)
ON_COMMAND(MSG_SELECT, on_nbox_select)
ON_COMMAND(MSG_FOCUS_LEFT, on_nbox_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_nbox_change_focus)
ON_COMMAND(MSG_INCREASE, on_nbox_increase)
ON_COMMAND(MSG_DECREASE, on_nbox_decrease)
END_CTRLPROC(nbox_class_proc, ctrl_default_proc)
