/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_list.c
   this file  implement the functions defined in  ctrl_list.h, also it implement
   some internal used
   function. All these functions are about how to decribe, set and draw a list
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
#include "gui_roll.h"

#include "ctrl_scrollbar.h"
#include "ctrl_list.h"

static void _list_get_item_rect(control_t *p_ctrl,
                                u16 index,
                                rect_t *p_item_rect)
{
  ctrl_list_data_t *p_data = NULL;
  s16 top = 0, height = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(index < p_data->cur_pos || index > (p_data->cur_pos + p_data->page - 1))
  {
    empty_rect(p_item_rect);
    return;
  }

  height =
    (p_data->mid_rect.bottom - p_data->mid_rect.top - p_data->interval *
     (p_data->page - 1)) / p_data->page;
  top = (p_data->mid_rect.top +
         (index - p_data->cur_pos) * (p_data->interval + height));

  set_rect(p_item_rect, p_data->mid_rect.left, top,
           p_data->mid_rect.right, (s16)(top + height));
}


static BOOL _list_check_valid(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return FALSE;
  }
  if((item_idx < p_data->valid_pos)
    || (item_idx >= p_data->valid_pos + p_data->size))
  {
    return FALSE;
  }
  return TRUE;
}


static void _list_free_string(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;
  u16 i = 0, j = 0;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  for(i = 0; i < p_data->size; i++)
  {
    for(j = 0; j < p_data->field_num; j++)
    {
      p_field = p_data->p_field + j;
      if((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_UNISTR)
      {
        continue;
      }
      p_temp = p_data->p_content +
               i * p_data->field_num + j;
      ctrl_unistr_free((void *)*p_temp);
      *p_temp = 0;
    }
  }
}


static BOOL _list_calc_curn_pos(control_t *p_ctrl,
                                u16 new_pos,
                                list_focus_mode_t mode)
{
  ctrl_list_data_t *p_data = NULL;
  u16 shift = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  shift = p_data->focus - p_data->cur_pos;
  p_data->focus = new_pos;

  if(p_data->total <= p_data->page)
  {
    if(p_data->cur_pos == 0)
    {
      return FALSE;
    }
    else  //sobody changed total
    {
      p_data->cur_pos = 0;
      return TRUE;
    }
  }

  if((p_data->focus < p_data->cur_pos)
    || (p_data->focus >= p_data->cur_pos + p_data->page))
  {
    if(p_ctrl->priv_attr & LIST_PAGE_MODE)
    {
      p_data->cur_pos = p_data->focus - p_data->focus % p_data->page;
    }
    else
    {
      switch(mode)
      {
        case LIST_FOCUS_ON_TOP:
          if(p_data->focus + p_data->page - 1 < p_data->total)
          {
            p_data->cur_pos = p_data->focus;
          }
          else
          {
            p_data->cur_pos = p_data->total - p_data->page;
          }
          break;
        case LIST_FOCUS_ON_BOTTOM:
          if(p_data->focus >= p_data->page - 1)
          {
            p_data->cur_pos = p_data->focus + 1 - p_data->page;
          }
          else
          {
            p_data->cur_pos = 0;
          }
          break;
        case LIST_FOCUS_KEEP_POS:
          if(p_data->focus >= shift)
          {
            if(p_data->focus + p_data->page <= p_data->total + shift)
            {
              p_data->cur_pos = p_data->focus - shift;
            }
            else
            {
              p_data->cur_pos = p_data->total - p_data->page;
            }
          }
          else
          {
            p_data->cur_pos = 0;
          }
          break;
        case LIST_FOCUS_JUMP:
          if(p_data->focus + p_data->page - 1 < p_data->total)
          {
            p_data->cur_pos = p_data->focus;
          }
          else
          {
            p_data->cur_pos = p_data->total - p_data->page;
          }
          break;
        default:
          MT_ASSERT(0);
      }
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static BOOL _list_calc_valid_pos(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(p_data->valid_pos > p_data->cur_pos)
  {
    p_data->valid_pos = p_data->cur_pos;
    return TRUE;
  }
  else if(p_data->valid_pos + p_data->size >= p_data->total)
  {
    return FALSE;
  }
  else if(p_data->valid_pos + p_data->size < p_data->cur_pos + p_data->page)
  {
    p_data->valid_pos = p_data->cur_pos + p_data->page - p_data->size;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static u32 *_list_get_field_content_buf(control_t *p_ctrl,
                                        u16 item_idx,
                                        u8 field_idx,
                                        u32 type)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if((item_idx < p_data->valid_pos)
    || (item_idx >= (p_data->valid_pos + p_data->size)))
  {
    return NULL;
  }

  if(field_idx >= p_data->field_num)
  {
    return NULL;
  }
  p_field = p_data->p_field + field_idx;

  if((p_field->attr & LISTFIELD_TYPE_MASK) != type)
  {
    return NULL;
  }

  return p_data->p_content +
         (item_idx - p_data->valid_pos) * p_data->field_num + field_idx;
}


static void _list_set_field_content(control_t *p_ctrl,
                                    u16 item_idx,
                                    u8 field_idx,
                                    u32 type,
                                    u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx, type);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp = (u32)val;
}


static u8 _list_get_style_by_attr(u8 attr,
                                  control_t *p_ctrl,
                                  u8 status,
                                  BOOL is_onfocus,
                                  list_xstyle_t *p_xstyle)
{
  u8 style = 0;

  MT_ASSERT(p_ctrl != NULL);

  switch(attr)
  {
    case OBJ_ATTR_HL:
      if(is_onfocus)
      {
        if((p_ctrl->priv_attr & LIST_SELECT_MODE)
          && (status == LIST_ITEM_SELECTED))  //focus and select
        {
          style = p_xstyle->sf_xstyle;
        }
        else //focus
        {
          style = p_xstyle->f_xstyle;
        }
      }
      else
      {
        if((p_ctrl->priv_attr & LIST_SELECT_MODE)
          && (status == LIST_ITEM_SELECTED))  //select
        {
          style = p_xstyle->s_xstyle;
        }
        else if(status == LIST_ITEM_DISABLED) //gray
        {
          style = p_xstyle->g_xstyle;
        }
        else //normal
        {
          style = p_xstyle->n_xstyle;
        }
      }
      break;

    case OBJ_ATTR_INACTIVE:
      style = p_xstyle->g_xstyle; //gray
      break;
    case OBJ_ATTR_ACTIVE:
      if((p_ctrl->priv_attr & LIST_SELECT_MODE)
        && (status == LIST_ITEM_SELECTED))
      {
        style = p_xstyle->s_xstyle;
      }
      else if(status == LIST_ITEM_DISABLED) //gray
      {
        style = p_xstyle->g_xstyle;
      }
      else //normal
      {
        style = p_xstyle->n_xstyle;
      }
      break;
    default:
      if(status == LIST_ITEM_DISABLED) //gray
      {
        style = p_xstyle->g_xstyle;
      }
      else //normal
      {
        style = p_xstyle->n_xstyle;
      }
      break;
  }

  return style;
}


static u8 _list_get_style_id(control_t *p_ctrl,
                             u8 status,
                             BOOL is_onfocus,
                             list_xstyle_t *p_xstyle)
{
  u8 attr = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  return _list_get_style_by_attr(attr, p_ctrl, status, is_onfocus, p_xstyle);
}

static void _list_draw_field_frame(control_t *p_ctrl,
                             hdc_t hdc,
                             u16 item_idx,
                             u8 field_idx,
                             u8 status,
                             rect_t *p_item_rect)
{
  list_field_t *p_field = NULL;
  ctrl_list_data_t *p_data = NULL;
  u8 rstyle = 0, fstyle = 0;
  u32 content = 0;
  rect_t rc_field;
  rsc_rstyle_t *p_rstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(field_idx >= p_data->field_num)
  {
    return;
  }

  p_field = p_data->p_field + field_idx;

  rstyle = _list_get_style_id(p_ctrl, status, p_data->focus == item_idx,
                              &p_field->rstyle);
  fstyle = _list_get_style_id(p_ctrl, status, p_data->focus == item_idx,
                              &p_field->fstyle);

  content = list_get_field_content(p_ctrl, item_idx, field_idx);

  rc_field = *p_item_rect;
  rc_field.left += p_field->shift_left;
  rc_field.right = rc_field.left + p_field->width;

  // draw field rectangle
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle);
  gui_draw_style_rect(hdc, &rc_field, p_rstyle);
}


static void _list_draw_item_frame(control_t *p_ctrl, hdc_t hdc, u16 index)
{
  ctrl_list_data_t *p_data = NULL;
  rect_t item_rect;
  u8 item_status = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  u16 color_idx = 0;

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  //draw frame without field content.
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  _list_get_item_rect(p_ctrl, index, &item_rect);
  item_status = list_get_item_status(p_ctrl, index);

  color_idx = _list_get_style_id(p_ctrl, item_status, p_data->focus == index,
                                 &p_data->rstyle);
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &item_rect, p_rstyle);
}


static void _list_draw_scroll_bar(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(p_data->p_scroll_bar != NULL)
  {
    ctrl_paint_ctrl(p_data->p_scroll_bar, TRUE);
  }
}


static void _list_draw_field(control_t *p_ctrl,
                             hdc_t hdc,
                             u16 item_idx,
                             u8 field_idx,
                             u8 status,
                             rect_t *p_item_rect)
{
  list_field_t *p_field = NULL;
  ctrl_list_data_t *p_data = NULL;
  u8 rstyle = 0, fstyle = 0;
  u32 content = 0;
  rect_t rc_field;
  rsc_rstyle_t *p_rstyle = NULL;
  u16 str_number[NUM_STRING_LEN + 1];

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(field_idx >= p_data->field_num)
  {
    return;
  }

  p_field = p_data->p_field + field_idx;

  rstyle = _list_get_style_id(p_ctrl, status, p_data->focus == item_idx,
                              &p_field->rstyle);
  fstyle = _list_get_style_id(p_ctrl, status, p_data->focus == item_idx,
                              &p_field->fstyle);

  content = list_get_field_content(p_ctrl, item_idx, field_idx);

  rc_field = *p_item_rect;
  rc_field.left += p_field->shift_left;
  rc_field.right = rc_field.left + p_field->width;

  // draw field rectangle
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle);
  gui_draw_style_rect(hdc, &rc_field, p_rstyle);

  // draw field content
  switch(p_field->attr & LISTFIELD_TYPE_MASK)
  {
    case LISTFIELD_TYPE_UNISTR:
    case LISTFIELD_TYPE_EXTSTR:
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      (u16 *)content, fstyle, STRDRAW_NORMAL | STRDRAW_BREAK_WORD);
      break;
    case LISTFIELD_TYPE_STRID:
      gui_draw_strid(hdc, &rc_field, p_field->attr, 0, 0, 0,
                     (u16)content, fstyle, STRDRAW_NORMAL | STRDRAW_BREAK_WORD);
      break;
    case LISTFIELD_TYPE_ICON:
      gui_draw_picture(hdc, &rc_field, p_field->attr, 0, 0, (u16)content);
      break;
    case LISTFIELD_TYPE_DEC:
      convert_i_to_dec_str_ex(str_number, content, p_field->bit_length);
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      str_number, fstyle, STRDRAW_NORMAL | STRDRAW_BREAK_WORD);
      break;
    case LISTFIELD_TYPE_HEX:
      convert_i_to_hex_str_ex(str_number, content, p_field->bit_length);
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      str_number, fstyle, STRDRAW_NORMAL | STRDRAW_BREAK_WORD);
      break;
  }
}


static void _list_draw_item(control_t *p_ctrl, hdc_t hdc, u16 item_idx)
{
  u8 j = 0;
  rect_t item_rect;
  u8 item_status = 0;
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!_list_check_valid(p_ctrl, item_idx))
  {
    return;
  }
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  item_status = list_get_item_status(p_ctrl, item_idx);

  _list_draw_item_frame(p_ctrl, hdc, item_idx);

  for(j = 0; j < p_data->field_num; j++)
  {
    _list_draw_field(p_ctrl, hdc, item_idx, j, item_status, &item_rect);
  }
}


static void _list_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_list_data_t *p_data = NULL;
  u16 i = 0, j = 0;
  u8 item_status = 0;
  rect_t item_rect = {0};

  gui_paint_frame(hdc, p_ctrl);
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  for(i = p_data->cur_pos; i < (p_data->cur_pos + p_data->page); i++)
  {
    if(i < p_data->total)
    {
      _list_draw_item(p_ctrl, hdc, i);
    }
    else
    {
      _list_draw_item_frame(p_ctrl, hdc, i);
      for(j = 0; j < p_data->field_num; j++)
      {
        _list_get_item_rect(p_ctrl, i, &item_rect);
        item_status = list_get_item_status(p_ctrl, i);        
        _list_draw_field_frame(p_ctrl, hdc, i, j, item_status, &item_rect);
      }
    }
  }
}


static BOOL _list_change_focus(control_t *p_ctrl,
                               u16 new_pos,
                               list_focus_mode_t mode,
                               BOOL paint)
{
  u16 old_focus = 0;
  ctrl_list_data_t *p_data = NULL;
  rect_t temp;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  old_focus = p_data->focus;
  if(_list_calc_curn_pos(p_ctrl, new_pos, mode))
  {
    if(NULL != p_data->p_scroll_bar)
    {
      sbar_set_current_pos(p_data->p_scroll_bar, p_data->cur_pos);
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    if(_list_calc_valid_pos(p_ctrl))
    {
      return TRUE;
    }
    if(paint)
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  else
  {
    if(_list_check_valid(p_ctrl, old_focus))
    {
      if(paint)
      {
        list_draw_item_ext(p_ctrl, old_focus, TRUE);
      }
      else
      {
        _list_get_item_rect(p_ctrl, old_focus, &temp);
        ctrl_add_rect_to_invrgn(p_ctrl, &temp);
      }
    }
    if(paint)
    {
      list_draw_item_ext(p_ctrl, p_data->focus, TRUE);
    }
    else
    {
      _list_get_item_rect(p_ctrl, old_focus, &temp);
      ctrl_add_rect_to_invrgn(p_ctrl, &temp);
    }
  }
  return FALSE;
}


static list_ret_t _list_change_scroll(control_t *p_ctrl, BOOL is_scroll_up)
{
  ctrl_list_data_t *p_data = NULL;
  s16 new_focus = 0;
  list_focus_mode_t mode = LIST_FOCUS_KEEP_POS;
  BOOL is_reverse = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(p_data->total == 0)
  {
    return LIST_FALSE;
  }

  mode = is_scroll_up ? LIST_FOCUS_ON_TOP : LIST_FOCUS_ON_BOTTOM;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_scroll_up ? -1 : 1,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(is_reverse)
  {
    mode = is_scroll_up ? LIST_FOCUS_ON_BOTTOM : LIST_FOCUS_ON_TOP;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_data->update_cb != NULL)
    {
      (*p_data->update_cb)(p_ctrl, p_data->valid_pos, p_data->size,
                           p_data->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}


static list_ret_t _list_change_page(control_t *p_ctrl, BOOL is_page_up)
{
  ctrl_list_data_t *p_data = NULL;
  s16 new_focus = 0;
  BOOL is_reverse = FALSE;
  list_focus_mode_t mode;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(p_data->total == 0)
  {
    return LIST_FALSE;
  }

  if(p_data->total <= p_data->page)
  {
    return LIST_FALSE;
  }

  mode = LIST_FOCUS_KEEP_POS;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_page_up ? 0 - p_data->page : p_data->page,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_data->update_cb != NULL)
    {
      (*p_data->update_cb)(p_ctrl, p_data->valid_pos, p_data->size,
                           p_data->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}


static void _list_free_ctrl(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  _list_free_string(p_ctrl);
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if((p_ctrl->priv_attr & LIST_SELECT_MODE) && (p_data->p_status))
  {
    mmi_free_buf(p_data->p_status);
    p_data->p_status = 0;
  }

  if(p_data->p_field != NULL)
  {
    mmi_free_buf(p_data->p_field);
    p_data->p_field = 0;
  }

  if(p_data->p_content != NULL)
  {
    mmi_free_buf(p_data->p_content);
    p_data->p_content = 0;
  }
}


RET_CODE list_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_list_data_t *p_default_data = NULL;

  register_info.data_size = sizeof(ctrl_list_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_list_data_t *)register_info.p_default_data;
  
  // initialize the default control of list class
  p_default_ctrl->priv_attr = LIST_CYCLE_MODE;
  p_default_ctrl->p_proc = list_class_proc;
  p_default_ctrl->p_paint = _list_draw;

  // initalize the default data of list class
  p_default_data->focus = LIST_INVALID_FOCUS;
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_LIST, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void list_set_update(control_t *p_ctrl, list_update_t p_cb, u32 context)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  p_data->update_cb = p_cb;
  p_data->context = context;
}


list_update_t list_get_update(control_t *p_ctrl,  u32 *p_context)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  *p_context = p_data->context;
  return p_data->update_cb;
}


BOOL list_set_count(control_t *p_ctrl, u16 total, u16 page)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  //cannot init 2 times
  //  if((ctrl->priv_attr & LIST_SELECT_MODE) && (p_data->status))
  //  {
  //    return FALSE;
  //  }

  // free content at first
  _list_free_string(p_ctrl);

  p_data->total = total;
  p_data->page = page;

  if(p_data->p_scroll_bar != NULL)
  {
    sbar_set_count(p_data->p_scroll_bar, p_data->page, p_data->total);
  }

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    p_data->p_status =
      mmi_realloc_buf(p_data->p_status, sizeof(u32) * (total / 16 + 1));
    MT_ASSERT(p_data->p_status != NULL);

    if(p_data->p_status != NULL)
    {
      memset(p_data->p_status, 0, sizeof(u32) * (total / 16 + 1));
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return TRUE;
  }
}


u16 list_get_count(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->total;
}


u16 list_get_page(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->page;
}


BOOL list_set_item_status(control_t *p_ctrl, u16 idx, u8 status)
{
  ctrl_list_data_t *p_data = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(idx >= p_data->total)
  {
    return FALSE;
  }
  temp = *(p_data->p_status + idx / 16);
  status &= 0x03;                      //clean useless information
  temp &= ~(0x3 << ((idx % 16) * 2));  //clean old status
  temp |= status << ((idx % 16) * 2);  //set new status
  *(p_data->p_status + idx / 16) = temp;
  return TRUE;
}


u8 list_get_item_status(control_t *p_ctrl, u16 idx)
{
  ctrl_list_data_t *p_data = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return LIST_ITEM_NORMAL;
  }

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(idx >= p_data->total)
  {
    return LIST_ITEM_NORMAL;
  }
  temp = *(p_data->p_status + idx / 16);
  temp = (temp >> ((idx % 16) * 2)) & 0x03;
  return (u8)temp;
}


void list_set_current_pos(control_t *p_ctrl, u16 cur_pos)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  p_data->cur_pos = cur_pos;

  if(p_data->p_scroll_bar != NULL)
  {
    sbar_set_current_pos(p_data->p_scroll_bar, p_data->cur_pos);
  }
}


u16 list_get_current_pos(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->cur_pos;
}


void list_set_valid_pos(control_t *p_ctrl, u16 valid_pos)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  p_data->valid_pos = valid_pos;
}


u16 list_get_valid_pos(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->valid_pos;
}


list_ret_t list_set_focus_pos(control_t *p_ctrl, u16 focus)
{
  ctrl_list_data_t *p_data = NULL;
  u8 status = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(focus >= p_data->total)
  {
    p_data->focus = LIST_INVALID_FOCUS;
    return LIST_FALSE;
  }

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    status = list_get_item_status(p_ctrl, focus);
    if(status == LIST_ITEM_DISABLED)
    {
      return LIST_FALSE;
    }
  }
  if(p_data->focus == focus)
  {
    return LIST_SUCCESS;
  }

  if(_list_change_focus(p_ctrl, focus, LIST_FOCUS_JUMP, FALSE))
  {
    if(p_data->update_cb != NULL)
    {
      (*p_data->update_cb)(p_ctrl, p_data->valid_pos, p_data->size,
                           p_data->context);
    }
    return LIST_RELOAD_ALL;
  }

  return LIST_SUCCESS;
}


u16 list_get_focus_pos(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->focus;
}


void list_set_field_bit_length(control_t *p_ctrl, u8 field_idx, u8 bit_length)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(field_idx >= p_data->field_num)
  {
    return;
  }
  p_field = p_data->p_field + field_idx;

  if(((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_DEC)
    && ((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_HEX))
  {
    return;
  }
  p_field->bit_length = bit_length;
}


BOOL list_set_field_count(control_t *p_ctrl, u8 field_cnt, u16 size)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
/*
   if(p_data->p_content)  //cannot init 2 times
   {
    return FALSE;
   }
   if(p_data->p_field)  //cannot init 2 times
   {
    return FALSE;
   }
  */
  p_data->field_num = field_cnt;
  p_data->p_field = mmi_realloc_buf(p_data->p_field,
                                    sizeof(list_field_t) * field_cnt);
  MT_ASSERT(p_data->p_field != NULL);

  if(p_data->p_field == NULL)
  {
    return FALSE;
  }
  memset(p_data->p_field, 0, sizeof(list_field_t) * field_cnt);

  // set size
  p_data->size = size;

  p_data->p_content = mmi_realloc_buf(p_data->p_content,
                                      sizeof(u32) * field_cnt * p_data->size);
  MT_ASSERT(p_data->p_content != NULL);

  if(p_data->p_content == NULL)
  {
    mmi_free_buf(p_data->p_field);
    p_data->p_field = 0;
    return FALSE;
  }

  memset(p_data->p_content, 0, sizeof(u32) * field_cnt * p_data->size);

  return TRUE;
}


void list_set_field_attr(control_t *p_ctrl,
                         u8 field_idx,
                         u32 attr,
                         u16 width,
                         u16 left,
                         u16 top)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(field_idx >= p_data->field_num)
  {
    return;
  }

  if(left + width > RECTW(p_data->mid_rect))
  {
    MT_ASSERT(0);
  }
  
  p_field = p_data->p_field + field_idx;
  p_field->attr = attr;
  p_field->width = width;
  p_field->shift_left = left;
  p_field->shift_top = top;
  p_field->bit_length = 0;    //auto size
}


void list_set_field_font_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_fstyle)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(field_idx >= p_data->field_num)
  {
    return;
  }
  p_field = p_data->p_field + field_idx;

  if(p_fstyle != NULL)
  {
    memcpy(&p_field->fstyle, p_fstyle, sizeof(list_xstyle_t));
  }
}


void list_set_field_rect_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_rstyle)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(field_idx >= p_data->field_num)
  {
    return;
  }
  p_field = p_data->p_field + field_idx;

  if(p_rstyle != NULL)
  {
    memcpy(&p_field->rstyle, p_rstyle, sizeof(list_xstyle_t));
  }
}


BOOL list_set_field_content_by_unistr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx,
                                       LISTFIELD_TYPE_UNISTR);
  if(p_temp == NULL)
  {
    return FALSE;
  }
  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, uni_strlen(p_unistr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  uni_strcpy((u16 *)(*p_temp), p_unistr);
  return TRUE;
}


BOOL list_set_field_content_by_ascstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx,
                                       LISTFIELD_TYPE_UNISTR);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, strlen((char *)p_ascstr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }

  str_asc2uni(p_ascstr, (u16 *)(*p_temp));
  return TRUE;
}


void list_set_field_content_by_strid(control_t *p_ctrl,
                                     u16 item_idx,
                                     u8 field_idx,
                                     u16 str_id)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_STRID, (u32)str_id);
}


void list_set_field_content_by_icon(control_t *p_ctrl,
                                    u16 item_idx,
                                    u8 field_idx,
                                    u16 bmp_id)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_ICON, (u32)bmp_id);
}


void list_set_field_content_by_extstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_extstr)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_EXTSTR, (u32)p_extstr);
}


void list_set_field_content_by_dec(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 dec)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_DEC, (u32)dec);
}


void list_set_field_content_by_hex(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 hex)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_HEX, (u32)hex);
}


u32 list_get_field_content(control_t *p_ctrl, u16 item_idx, u8 field_idx)
{
  ctrl_list_data_t *p_data = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return 0;
  }
  if(field_idx >= p_data->field_num)
  {
    return 0;
  }
  if(item_idx < p_data->valid_pos)
  {
    //		DEBUG_PRINTF("list_get_field_content: ERROR, out of valid range\n");
    return 0;
  }

  p_temp = p_data->p_content +
           (item_idx - p_data->valid_pos) * p_data->field_num + field_idx;
  return *p_temp;
}


void list_set_mid_rect(control_t *p_ctrl,
                       s16 left,
                       s16 top,
                       u16 width,
                       u16 height,
                       u8 interval)
{
  ctrl_list_data_t *p_data = NULL;
  rect_t frame;

  MT_ASSERT(p_ctrl != NULL);

  ctrl_get_frame(p_ctrl, &frame);

  MT_ASSERT((left + width) <= RECTW(frame));
  MT_ASSERT((top + height) <= RECTH(frame));

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  set_rect(&p_data->mid_rect, left, top, (s16)(left + width),
           (s16)(top + height));
  p_data->interval = interval;
}


void list_set_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_rstyle)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  memcpy(&p_data->rstyle, p_rstyle, sizeof(list_xstyle_t));
}


void list_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  p_data->p_scroll_bar = p_ctrl_bar;

  if(p_ctrl_bar != NULL)
  {
    sbar_set_count(p_ctrl_bar, p_data->page, p_data->total);
    sbar_set_current_pos(p_ctrl_bar, p_data->cur_pos);
  }
}


control_t *list_get_scrollbar(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  return p_data->p_scroll_bar;
}


//This function should be called before list_set_count(),
//in order to alloc memory.
void list_enable_select_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(is_enable)
  {
    if(p_data->total > 0)
    {
      CTRL_PRINTF("can NOT enable select mode after u call list_set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= LIST_SELECT_MODE;
  }
  else
  {
    if(p_data->p_status != 0)
    {
      CTRL_PRINTF("can NOT disable select mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= ~LIST_SELECT_MODE;
  }
}


void list_set_select_mode(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~LIST_SELECT_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & LIST_SELECT_MASK);   //set new type
}


void list_enable_cycle_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(is_enable)
  {
    p_ctrl->priv_attr |= LIST_CYCLE_MODE;
  }
  else
  {
    p_ctrl->priv_attr &= ~LIST_CYCLE_MODE;
  }
}


void list_enable_page_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(is_enable)
  {
    p_ctrl->priv_attr |= LIST_PAGE_MODE;
  }
  else
  {
    p_ctrl->priv_attr &= ~LIST_PAGE_MODE;
  }
}


BOOL list_select_item(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_data_t *p_data = NULL;
  u8 status = 0;
  u16 first = 0;
  rect_t item_rect;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return FALSE;
  }

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }
  status = list_get_item_status(p_ctrl, item_idx);
  if(status == LIST_ITEM_DISABLED)
  {
    return FALSE;
  }
  else if(status == LIST_ITEM_SELECTED)
  {
    return TRUE;
  }

  if((p_ctrl->priv_attr & LIST_SELECT_MASK) == LIST_SINGLE_SELECT)
  {
    first = list_get_the_first_selected(p_ctrl, 0);
    if(first != LIST_INVALID_FOCUS)
    {
      list_set_item_status(p_ctrl, first, LIST_ITEM_NORMAL);
      _list_get_item_rect(p_ctrl, first, &item_rect);
      ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);
    }
  }

  list_set_item_status(p_ctrl, item_idx, LIST_ITEM_SELECTED);
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);

  return TRUE;
}


BOOL list_unselect_item(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_data_t *p_data = NULL;
  rect_t item_rect;
  u8 status = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return FALSE;
  }

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  status = list_get_item_status(p_ctrl, item_idx);
  if(status != LIST_ITEM_SELECTED)
  {
    return FALSE;
  }

  list_set_item_status(p_ctrl, item_idx, LIST_ITEM_NORMAL);
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);

  return TRUE;
}


void list_clear_status(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return;
  }
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  memset(p_data->p_status, 0, sizeof(u32) * (p_data->total / 16 + 1));
}


u16 list_get_the_first_selected(control_t *p_ctrl, u16 begin)
{
  ctrl_list_data_t *p_data = NULL;
  u16 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  for(i = begin; i < p_data->total; i++)
  {
    if(list_get_item_status(p_ctrl, i) == LIST_ITEM_SELECTED)
    {
      return i;
    }
  }
  return LIST_INVALID_FOCUS;
}


/*!
   draw field

   \param[in] ctrl				: list control
   \param[in] item_idx		: item index
   \param[in] field_idx		: field index
   \param[in] is_force			: update invalid region before draw
   \return					: NULL
  */
void list_draw_field_ext(control_t *p_ctrl,
                         u16 item_idx,
                         u8 field_idx,
                         BOOL is_force)
{
  hdc_t hdc = 0;
  rect_t item_rect, field_rect;
  u8 item_status = 0;
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;
  u16 color_idx = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return;
  }
  if(field_idx >= p_data->field_num)
  {
    return;
  }
  p_field = p_data->p_field + field_idx;
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);

  copy_rect(&field_rect, &item_rect);
  field_rect.left += p_field->shift_left;
  field_rect.right = field_rect.left + p_field->width;

  if(is_force)
  {
    ctrl_add_rect_to_invrgn(p_ctrl, &field_rect);
  }

  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }

  // draw frame
  gui_paint_frame(hdc, p_ctrl);

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    item_status = list_get_item_status(p_ctrl, item_idx);
  }
  else
  {
    item_status = LIST_ITEM_NORMAL;
  }
  // draw field
  color_idx = _list_get_style_id(p_ctrl, item_status, p_data->focus == item_idx,
                                 &p_data->rstyle);
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &field_rect, p_rstyle);

  _list_draw_field(p_ctrl, hdc, item_idx, field_idx, item_status, &item_rect);
  gui_end_paint(p_ctrl, hdc);
}


void list_draw_item_ext(control_t *p_ctrl, u16 item_idx, BOOL is_force)
{
  hdc_t hdc = 0;
  rect_t temp;
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;
  if(item_idx >= p_data->total)
  {
    return;
  }
  if(is_force)
  {
    _list_get_item_rect(p_ctrl, item_idx, &temp);
    ctrl_add_rect_to_invrgn(p_ctrl, &temp);
  }
  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }
  // draw frame
  gui_paint_frame(hdc, p_ctrl);
  // draw item
  _list_draw_item(p_ctrl, hdc, item_idx);
  gui_end_paint(p_ctrl, hdc);
}


s16 list_get_new_focus(control_t *p_ctrl, s16 offset, BOOL *p_is_reverse)
{
  ctrl_list_data_t *p_data = NULL;
  s16 new_focus = 0;
  s16 i = 0;
  BOOL find_ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  *p_is_reverse = FALSE;
  if(offset == 0)
  {
    return p_data->focus;
  }

  i = p_data->focus + offset;
  find_ret = FALSE;
  while(offset < 0 ? i >= 0 : i < p_data->total)
  {
    if(list_get_item_status(p_ctrl, i) != LIST_ITEM_DISABLED)
    {
      new_focus = i;
      find_ret = TRUE;
      break;
    }
    (offset < 0) ? (i--) : (i++);
  }

  if(!find_ret)
  {
    if(p_ctrl->priv_attr & LIST_CYCLE_MODE)
    {
      if(p_ctrl->priv_attr & LIST_PAGE_MODE)
      {
        if((offset < 0 && p_data->focus + offset < 0)
          || (offset > 0 && p_data->focus >= (p_data->total / p_data->page) *
              p_data->page + p_data->total % p_data->page - 1))
        {
          offset = 0 - offset;
        }
      }
      else
      {
        if((offset < 0 && p_data->focus == 0)
          || (offset > 0 && p_data->focus == p_data->total - 1))
        {
          offset = 0 - offset;
        }
      }
    }

    i = offset < 0 ? 0 : (p_data->total - 1);
    while(offset < 0 ? i < p_data->focus : i > p_data->focus)
    {
      if(list_get_item_status(p_ctrl, i) != LIST_ITEM_DISABLED)
      {
        new_focus = i;
        find_ret = TRUE;
        *p_is_reverse = TRUE;
        break;
      }
      (offset < 0) ? (i++) : (i--);
    }
  }


  if(!find_ret)
  {
    return -1;
  }

  return new_focus;
}


list_ret_t list_scroll_up(control_t *p_ctrl)
{
  return _list_change_scroll(p_ctrl, TRUE);
}


list_ret_t list_scroll_down(control_t *p_ctrl)
{
  return _list_change_scroll(p_ctrl, FALSE);
}


list_ret_t list_page_up(control_t *p_ctrl)
{
  return _list_change_page(p_ctrl, TRUE);
}


list_ret_t list_page_down(control_t *p_ctrl)
{
  return _list_change_page(p_ctrl, FALSE);
}


BOOL list_select(control_t *p_ctrl)
{
  ctrl_list_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  switch(list_get_item_status(p_ctrl, p_data->focus))
  {
    case LIST_ITEM_NORMAL:                                 //normal
      list_select_item(p_ctrl, p_data->focus);
      break;
    case LIST_ITEM_SELECTED:                               //select
      list_unselect_item(p_ctrl, p_data->focus);
      break;
    default:
      return FALSE;
  }

  switch(p_ctrl->priv_attr & LIST_SELECT_MASK)
  {
    case LIST_SINGLE_SELECT:
      ctrl_paint_ctrl(p_ctrl, FALSE);
      break;
    case LIST_MULTI_SELECT:
      list_draw_item_ext(p_ctrl, p_data->focus, FALSE);
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }

  return TRUE;
}


static RET_CODE on_list_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t parent_dc = (hdc_t)(para2);

  ctrl_default_proc(p_ctrl, msg, para1, para2);

  //this bar isn't list's child, when parent_dc isn't 0,
  //parent control will draw bar
  if(parent_dc == HDC_INVALID)
  {
    _list_draw_scroll_bar(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_list_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _list_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_list_focus_up(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_scroll_up(p_ctrl);

  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset_content(p_ctrl);
  }
  return SUCCESS;
}


static RET_CODE on_list_focus_down(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_scroll_down(p_ctrl);
  
  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset_content(p_ctrl);
  }  
  return SUCCESS;
}


static RET_CODE on_list_page_up(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_page_up(p_ctrl);

  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset_content(p_ctrl);
  }    
  return SUCCESS;
}


static RET_CODE on_list_page_down(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_page_down(p_ctrl);

  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset_content(p_ctrl);
  }      
  return SUCCESS;
}


static RET_CODE on_list_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  list_select(p_ctrl);
  return SUCCESS;
}

static RET_CODE on_list_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_list_data_t *p_data = NULL;
  u8 i = 0, status = 0;
  u8 font = 0;
  rect_t item_rect = {0};
  rect_t field_rect = {0};
  list_field_t *p_field = NULL;
  u32 content = 0;
  

  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  MT_ASSERT(p_data != NULL);
  
  _list_get_item_rect(p_ctrl, p_data->focus, &item_rect);

  for(i = 0; i < p_data->field_num; i++)
  {
    p_field = p_data->p_field + i;

    status = list_get_item_status(p_ctrl, i); 
    font = _list_get_style_id(p_ctrl, status, TRUE, &p_field->fstyle);

    if(p_field->attr & LISTFIELD_SCROLL)
    {
      field_rect.left = item_rect.left + p_field->shift_left;
      field_rect.top = item_rect.top + p_field->shift_top;
      field_rect.right = field_rect.left + p_field->width;
      field_rect.bottom = item_rect.bottom;

      content = list_get_field_content(p_ctrl, p_data->focus, i);

      gui_create_rsurf(p_ctrl, font, (u16 *)content, &field_rect, 0, (roll_param_t *)para1, i);
    }
  }

  return SUCCESS;;
}

static RET_CODE on_list_rolling(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_list_data_t *p_data = NULL;
  list_field_t *p_field = NULL;
  u8 field_idx = 0, status = 0;
  rect_t item_rect = {0}, field_rect = {0};
  hdc_t hdc = 0;  
  roll_node_t *p_node = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_node = (roll_node_t *)para1;
  p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_node != NULL);

  field_idx = p_node->context;

  MT_ASSERT(field_idx < p_data->field_num);

  _list_get_item_rect(p_ctrl, p_data->focus, &item_rect);

  p_field = p_data->p_field + field_idx;
  
  field_rect.left = item_rect.left + p_field->shift_left;
  field_rect.top = item_rect.top + p_field->shift_top;
  field_rect.right = field_rect.left + p_field->width;
  field_rect.bottom = item_rect.bottom;

  ctrl_add_rect_to_invrgn(p_ctrl, &field_rect);

  hdc = gui_begin_paint(p_ctrl, 0);
  gui_paint_frame(hdc, p_ctrl);  

  _list_draw_item_frame(p_ctrl, hdc, p_data->focus);

  status = list_get_item_status(p_ctrl, p_data->focus);        
  _list_draw_field_frame(p_ctrl, hdc, p_data->focus, field_idx, status, &item_rect);

  gui_rolling_node(p_node, hdc);

  gui_end_paint(p_ctrl, hdc);

  return SUCCESS;;
}

// define the default msgmap of class
BEGIN_CTRLPROC(list_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_PAINT, on_list_paint)
  ON_COMMAND(MSG_FOCUS_UP, on_list_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_list_focus_down)
  ON_COMMAND(MSG_PAGE_UP, on_list_page_up)
  ON_COMMAND(MSG_PAGE_DOWN, on_list_page_down)
  ON_COMMAND(MSG_SELECT, on_list_select)
  ON_COMMAND(MSG_DESTROY, on_list_destroy)
  ON_COMMAND(MSG_START_ROLL, on_list_roll_start)
  ON_COMMAND(MSG_ROLLING, on_list_rolling)
END_CTRLPROC(list_class_proc, ctrl_default_proc)
