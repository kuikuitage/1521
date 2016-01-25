/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_list.c
   this file  implement the functions defined in  ctrl_matrixbox.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a
   matrixbox control.
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

#include "ctrl_matrixbox.h"

static BOOL _mbox_get_item_rect(control_t *p_ctrl, u16 index, rect_t *p_rect)
{
  ctrl_mbox_data_t *p_data = NULL;
  u16 left = 0, top = 0, height = 0, width = 0;
  u8 x = 0, y = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  if(index >= p_data->total)
  {
    return FALSE;
  }
  x = index % (p_data->col);
  y = index / (p_data->col);

  height =
    (p_data->mid_rect.bottom - p_data->mid_rect.top -
     (p_data->row - 1) * p_data->v_interval) / p_data->row;
  width =
    (p_data->mid_rect.right - p_data->mid_rect.left -
     (p_data->col - 1) * p_data->h_interval) / p_data->col;

  left = p_data->mid_rect.left + x * (width + p_data->h_interval);
  top = p_data->mid_rect.top + y * (height + p_data->v_interval);

  set_rect(p_rect, left, top, (s16)(left + width), (s16)(top + height));
  return TRUE;
}


static u32 *_mbox_get_content_strbuf(control_t *p_ctrl, u16 item_idx, u32 type)
{
  ctrl_mbox_data_t *p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  if((p_ctrl->priv_attr & MBOX_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  if(item_idx >= p_data->total)
  {
    return NULL;
  }

  return p_data->p_content + item_idx;
}


static void _mbox_set_static_content(control_t *p_ctrl,
                                     u16 item_idx,
                                     u32 type,
                                     u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = val;
}


static void _mbox_get_style_by_attr(u8 attr,
                                    control_t *p_ctrl,
                                    u16 index,
                                    u8 *p_rstyle_idx,
                                    u8 *p_fstyle_idx,
                                    u16 *p_icon_idx)
{
  u8 item_status = 0;
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  item_status = mbox_get_item_status(p_ctrl, index);

  switch(attr)
  {
    case OBJ_ATTR_HL:
      if(p_data->focus == index) //focus
      {
        *p_rstyle_idx = p_data->item_f_idx;
        *p_fstyle_idx = p_data->f_fstyle;
        *p_icon_idx = mbox_get_focus_icon(p_ctrl, index);
      }
      else
      {
        if(item_status == MBOX_ITEM_NORMAL) //normal
        {
          *p_rstyle_idx = p_data->item_n_idx;
          *p_fstyle_idx = p_data->n_fstyle;
        }
        else //gray
        {
          *p_rstyle_idx = p_data->item_g_idx;
          *p_fstyle_idx = p_data->g_fstyle;
        }
        *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
      }
      break;
    case OBJ_ATTR_INACTIVE:
      // gray
      *p_rstyle_idx = p_data->item_g_idx;
      *p_fstyle_idx = p_data->g_fstyle;
      *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
      break;
    default:
      if(item_status == MBOX_ITEM_NORMAL) //normal
      {
        *p_rstyle_idx = p_data->item_n_idx;
        *p_fstyle_idx = p_data->n_fstyle;
      }
      else //gray
      {
        *p_rstyle_idx = p_data->item_g_idx;
        *p_fstyle_idx = p_data->g_fstyle;
      }
      *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
  }
}


static u32 _mbox_set_content_strbuf(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_STRING_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  p_temp = p_data->p_content + item_idx;

  return *p_temp;
}


static void _mbox_free_str_buf(control_t *p_ctrl)
{
  ctrl_mbox_data_t *p_data = NULL;
  u16 i = 0;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
    for(i = 0; i < p_data->total; i++)
    {
      if((p_ctrl->priv_attr & MBOX_STRTYPE_MASK) == MBOX_STRTYPE_UNICODE)
      {
        p_temp = p_data->p_content + i;
        ctrl_unistr_free((void *)*p_temp);
        *p_temp = 0;
      }
    }
  }
}


static void _mbox_draw_item(control_t *p_ctrl, hdc_t hdc, u16 index)
{
  u8 color_idx = 0;
  u16 icon_idx = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  u32 content = 0;
  u16 str_number[NUM_STRING_LEN + 1];
  u8 left = 0, top = 0;
  u8 attr = 0, font = 0;
  rect_t temp_rect;


  if(!_mbox_get_item_rect(p_ctrl, index, &temp_rect))
  {
    return;
  }
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }
  _mbox_get_style_by_attr(attr, p_ctrl, index, &color_idx, &font, &icon_idx);

  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &temp_rect, p_rstyle);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    mbox_get_icon_offset(p_ctrl, &left, &top);
    gui_draw_picture(hdc, &temp_rect, (p_ctrl->priv_attr) << 4, left, top,
                     icon_idx);
  }

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    content = _mbox_set_content_strbuf(p_ctrl, index);
    mbox_get_string_offset(p_ctrl, &left, &top);
    switch(p_ctrl->priv_attr & MBOX_STRTYPE_MASK)
    {
      case MBOX_STRTYPE_UNICODE:
      case MBOX_STRTYPE_EXTSTR:
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, (u16 *)content,
                        font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_STRID:
        gui_draw_strid(hdc, &temp_rect, p_ctrl->priv_attr,
                       left, top, 0, (u16)content,
                       font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_DEC:
        convert_i_to_dec_str(str_number, content);
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, str_number,
                        font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_HEX:
        convert_i_to_hex_str(str_number, content);
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, str_number,
                        font, STRDRAW_NORMAL);
        break;
      default:
        MT_ASSERT(0);
    }
  }
}


static void _mbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_mbox_data_t *p_data = NULL;
  u16 i = 0;

  gui_paint_frame(hdc, p_ctrl);
  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  for(i = 0; i < p_data->total; i++)
  {
    _mbox_draw_item(p_ctrl, hdc, i);
  }
}


static u16 _mbox_get_next_item(control_t *p_ctrl,
                               u16 curn,
                               s16 offset,
                               BOOL is_hori)
{
  u16 z = 0, o = 0;
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  z = curn / p_data->col, o = curn % p_data->col;

  if(is_hori)
  {
    o = (o + offset + p_data->col) % p_data->col;
  }
  else
  {
    z = (z + offset + p_data->row) % p_data->row;
  }

  return z * p_data->col + o;
}


static void _mbox_free_ctrl(control_t *p_ctrl)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  _mbox_free_str_buf(p_ctrl);
  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(p_data->p_status != NULL)
  {
    mmi_free_buf(p_data->p_status);
    p_data->p_status = 0;
  }
  if(p_data->p_content != NULL)
  {
    mmi_free_buf(p_data->p_content);
    p_data->p_content = 0;
  }
  if(p_data->p_icon != NULL)
  {
    mmi_free_buf(p_data->p_icon);
    p_data->p_icon = 0;
  }
}


static void _mbox_switch_focus(control_t *p_ctrl,
                               ctrl_mbox_data_t *p_data,
                               u16 new_focus)
{
  u16 old_focus = 0;

  old_focus = p_data->focus;
  p_data->focus = new_focus;
  mbox_draw_item_ext(p_ctrl, old_focus, TRUE);
  mbox_draw_item_ext(p_ctrl, new_focus, TRUE);
}


static BOOL _mbox_change_focus(control_t *p_ctrl, s16 offset, BOOL is_hori)
{
  ctrl_mbox_data_t *p_data = NULL;
  u16 z = 0;
  BOOL find_ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  if((is_hori ? p_data->col : p_data->row) <= 1)
  {
    return FALSE;
  }

  z = p_data->focus;
  find_ret = FALSE;
  do
  {
    z = _mbox_get_next_item(p_ctrl, z, offset, is_hori);
    if(mbox_get_item_status(p_ctrl, z) == MBOX_ITEM_NORMAL)
    {
      if(z != p_data->focus)
      {
        find_ret = TRUE;
      }
      break;
    }
  }
  while(z != p_data->focus);

  if(find_ret)
  {
    _mbox_switch_focus(p_ctrl, p_data, z);
    return TRUE;
  }
  return FALSE;
}


RET_CODE mbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_mbox_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of matrixbox class
  p_default_ctrl->priv_attr = MBOX_STRTYPE_UNICODE |
                              MBOX_STRING_MODE | MBOX_ICON_MODE;;
  p_default_ctrl->p_proc = mbox_class_proc;
  p_default_ctrl->p_paint = _mbox_draw;

  // initalize the default data of matrixbox class
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_MBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void mbox_set_string_fstyle(control_t *p_ctrl,
                            u8 f_fstyle,
                            u8 n_fstyle,
                            u8 g_fstyle)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  p_data->f_fstyle = f_fstyle;
  p_data->n_fstyle = n_fstyle;
  p_data->g_fstyle = g_fstyle;
}


void mbox_set_item_rstyle(control_t *p_ctrl,
                          u8 f_rstyle,
                          u8 n_rstyle,
                          u8 g_rstyle)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

  p_data->item_f_idx = f_rstyle;
  p_data->item_n_idx = n_rstyle;
  p_data->item_g_idx = g_rstyle;
}


void mbox_set_item_rect(control_t *p_ctrl,
                        s16 left,
                        s16 top,
                        s16 right,
                        s16 bottom,
                        u8 h_interval,
                        u8 v_interval)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  set_rect(&p_data->mid_rect, left, top, right, bottom);
  p_data->h_interval = h_interval;
  p_data->v_interval = v_interval;
}


void mbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
    p_data->str_left = str_left;
    p_data->str_top = str_top;
  }
}


void mbox_set_icon_offset(control_t *p_ctrl, u8 icon_left, u8 icon_top)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
    p_data->icon_left = icon_left;
    p_data->icon_top = icon_top;
  }
}


BOOL mbox_set_count(control_t *p_ctrl, u16 total, u8 col, u8 row)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // free content at first
  _mbox_free_str_buf(p_ctrl);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  p_data->total = total;
  p_data->col = col;
  p_data->row = row;

  p_data->p_status =
    mmi_realloc_buf(p_data->p_status, sizeof(u32) * (total / 32 + 1));
  MT_ASSERT(p_data->p_status != NULL);

  if(p_data->p_status == NULL)
  {
    return FALSE;
  }
  memset(p_data->p_status, 0, sizeof(u32) * (total / 32 + 1));

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_data->p_content = mmi_realloc_buf(p_data->p_content, sizeof(u32) * total);
    MT_ASSERT(p_data->p_content != NULL);

    if(p_data->p_content == NULL)
    {
      if(p_data->p_status != NULL)
      {
        mmi_free_buf(p_data->p_status);
        p_data->p_status = 0;
      }
      if(p_data->p_content != NULL)
      {
        mmi_free_buf(p_data->p_content);
        p_data->p_content = 0;
      }
      if(p_data->p_icon != NULL)
      {
        mmi_free_buf(p_data->p_icon);
        p_data->p_icon = 0;
      }
      return FALSE;
    }
    memset(p_data->p_content, 0, sizeof(u32) * total);
  }

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_data->p_icon = mmi_realloc_buf(p_data->p_icon, sizeof(u32) * total);
    MT_ASSERT(p_data->p_icon != NULL);

    if(p_data->p_icon == NULL)
    {
      if(p_data->p_status != NULL)
      {
        mmi_free_buf(p_data->p_status);
        p_data->p_status = 0;
      }
      if(p_data->p_content != NULL)
      {
        mmi_free_buf(p_data->p_content);
        p_data->p_content = 0;
      }
      if(p_data->p_icon != NULL)
      {
        mmi_free_buf(p_data->p_icon);
        p_data->p_icon = 0;
      }
      return FALSE;
    }
    memset(p_data->p_icon, 0, sizeof(u32) * total);
  }

  return TRUE;
}


BOOL mbox_get_count(control_t *p_ctrl, u16 *p_total, u8 *p_col, u8 *p_row)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  *p_total = p_data->total;
  *p_col = p_data->col;
  *p_row = p_data->row;

  return TRUE;
}


void mbox_set_focus(control_t *p_ctrl, u16 focus)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(focus >= p_data->total)
  {
    return;
  }
  p_data->focus = focus;
}


u16 mbox_get_focus(control_t *p_ctrl)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  return p_data->focus;
}


BOOL mbox_set_item_status(control_t *p_ctrl, u16 index, u8 status)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(index >= p_data->total)
  {
    return FALSE;
  }
  temp = *(p_data->p_status + index / 32);
  status &= 0x01;                     //clean useless information
  temp &= ~(0x1 << (index % 32));     //clean old status
  temp |= status << (index % 32);     //set new status
  *(p_data->p_status + index / 32) = temp;
  return TRUE;
}


u8 mbox_get_item_status(control_t *p_ctrl, u16 index)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  temp = *(p_data->p_status + index / 32);
  temp = (temp >> (index % 32)) & 0x01;
  return (u8)temp;
}


BOOL mbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, MBOX_STRTYPE_UNICODE);
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


BOOL mbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, MBOX_STRTYPE_UNICODE);
  CTRL_PRINTF("\n%d  %d\n", item_idx, p_temp);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, strlen((char *)p_ascstr));
  CTRL_PRINTF("\n%d  %d\n", item_idx, *p_temp);
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  str_asc2uni(p_ascstr, (u16 *)(*p_temp));

  CTRL_PRINTF("\n%d  %d\n", item_idx, *p_temp);
  return TRUE;
}


void mbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_STRID, (u32)strid);
}


void mbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr)
{
  _mbox_set_static_content(p_ctrl,
                           item_idx, MBOX_STRTYPE_EXTSTR, (u32)p_extstr);
}


void mbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_HEX, (u32)hex);
}


void mbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_DEC, (u32)dec);
}


void mbox_set_content_by_icon(control_t *p_ctrl,
                              u16 item_idx,
                              u16 f_icon_id,
                              u16 n_icon_id)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;

    p_temp = p_data->p_icon + item_idx;
    *p_temp = (u32)(n_icon_id | (u32)(f_icon_id << 16));
  }
}


void mbox_enable_string_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(is_enable)
  {
    if(p_data->total > 0)
    {
      CTRL_PRINTF("can NOT enable string mode after _set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= MBOX_STRING_MODE;
  }
  else
  {
    if(p_data->p_content != 0)
    {
      CTRL_PRINTF("can NOT disable string mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= (~MBOX_STRING_MODE);
  }
}


void mbox_enable_icon_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(is_enable)
  {
    if(p_data->total > 0)
    {
      CTRL_PRINTF("can NOT enable icon mode after _set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= MBOX_ICON_MODE;
  }
  else
  {
    if(p_data->p_icon != 0)
    {
      CTRL_PRINTF("can NOT disable icon mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= (~MBOX_ICON_MODE);
  }
}


void mbox_set_string_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_STR_ALIGN_MASK);          //clean old style
  p_ctrl->priv_attr |= (style & MBOX_STR_ALIGN_MASK);   //set new style
}


void mbox_set_icon_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_ICON_ALIGN_MASK);               //clean old style
  p_ctrl->priv_attr |= ((style >> 4) & MBOX_ICON_ALIGN_MASK); //set new style
}


void mbox_set_content_strtype(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_STRTYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & MBOX_STRTYPE_MASK);    //set new type
}


void mbox_get_string_offset(control_t *p_ctrl,
                            u8 *p_left,
                            u8 *p_top)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(!(p_ctrl->priv_attr & MBOX_STRING_MODE))
  {
    *p_left = 0;
    *p_top = 0;
    return;
  }
  else
  {
    *p_left = p_data->str_left;
    *p_top = p_data->str_top;
  }
}


void mbox_get_icon_offset(control_t *p_ctrl,
                          u8 *p_left,
                          u8 *p_top)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    *p_left = 0;
    *p_top = 0;
    return;
  }
  else
  {
    *p_left = p_data->icon_left;
    *p_top = p_data->icon_top;
  }
}


u16 mbox_get_focus_icon(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  p_temp = p_data->p_icon + item_idx;

  return (u16)((*p_temp) >> 16) & 0xFFFF;
}


u16 mbox_get_normal_icon(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_data_t *p_data = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  p_temp = p_data->p_icon + item_idx;

  return (u16)((*p_temp) & 0xFFFF);
}


void mbox_draw_item_ext(control_t *p_ctrl, u16 index, BOOL is_force)
{
  hdc_t hdc = 0;
  ctrl_mbox_data_t *p_data = NULL;
  rect_t temp_rect;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(index >= p_data->total)
  {
    return;
  }

  if(is_force)
  {
    _mbox_get_item_rect(p_ctrl, index, &temp_rect);
    ctrl_add_rect_to_invrgn(p_ctrl, &temp_rect);
  }
  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }
  gui_paint_frame(hdc, p_ctrl);
  _mbox_draw_item(p_ctrl, hdc, index);
  gui_end_paint(p_ctrl, hdc);
}


BOOL mbox_set_focus_ext(control_t *p_ctrl, u16 focus)
{
  ctrl_mbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_mbox_data_t *)p_ctrl->priv_data;
  if(focus >= p_data->total)
  {
    return FALSE;
  }

  _mbox_switch_focus(p_ctrl, p_data, focus);
  return TRUE;
}


static RET_CODE on_mbox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _mbox_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_mbox_change_focus(control_t *p_ctrl,
                                     u16 msg,
                                     u32 para1,
                                     u32 para2)
{
  s16 offset = 1;
  BOOL is_hori = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  if(msg == MSG_FOCUS_LEFT
    || msg == MSG_FOCUS_RIGHT)
  {
    is_hori = TRUE;
  }

  if(msg == MSG_FOCUS_LEFT
    || msg == MSG_FOCUS_UP)
  {
    offset = -1;
  }

  _mbox_change_focus(p_ctrl, offset, is_hori);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(mbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_FOCUS_UP, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_LEFT, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_mbox_change_focus)
ON_COMMAND(MSG_DESTROY, on_mbox_destroy)
END_CTRLPROC(mbox_class_proc, ctrl_default_proc)
