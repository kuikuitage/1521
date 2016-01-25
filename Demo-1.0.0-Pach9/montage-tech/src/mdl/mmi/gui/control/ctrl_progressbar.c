/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_progressbar.c
   this file  implement the functions defined in  ctrl_progressbar.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar
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

#include "ctrl_progressbar.h"

static u16 _pbar_get_mid_width(ctrl_pbar_data_t *p_data)
{
  u16 cur_size = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  if(p_data->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return 0;
  }
  else
  {
    cur_size = 0;
    p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_data->cur_rstyle);
    if(RSTYLE_IS_R_ICON(p_rstyle->left))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->left), &bmp_w, &bmp_h);
      cur_size += bmp_w;
    }
    if(RSTYLE_IS_R_ICON(p_rstyle->right))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->right), &bmp_w, &bmp_h);
      cur_size += bmp_w;
    }
    if(cur_size == 0)
    {
      cur_size =
        (p_data->mid_rect.right - p_data->mid_rect.left) / p_data->step;
    }
    return cur_size;
  }
}


static u16 _pbar_get_mid_height(ctrl_pbar_data_t *p_data)
{
  u16 cur_size = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  if(p_data->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return 0;
  }
  else
  {
    cur_size = 0;
    p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_data->cur_rstyle);
    if(RSTYLE_IS_R_ICON(p_rstyle->top))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->top), &bmp_w, &bmp_h);
      cur_size += bmp_h;
    }
    if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->bottom), &bmp_w, &bmp_h);
      cur_size += bmp_h;
    }
    if(cur_size == 0)
    {
      cur_size =
        (p_data->mid_rect.bottom - p_data->mid_rect.top) / p_data->step;
    }
    return cur_size;
  }
}


static BOOL _pbar_get_continuous_min_rect(control_t *p_ctrl, rect_t *p_min_rect)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 temp = 0, cur_size = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->min_rstyle == 0xFF)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK)
  {
    temp = p_data->mid_rect.right - p_data->mid_rect.left;
    temp = temp * (p_data->curn - p_data->min) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    cur_size = _pbar_get_mid_width(p_data);
    if(cur_size > temp)
    {
      return FALSE;
    }

    p_min_rect->top = p_data->mid_rect.top;
    p_min_rect->bottom = p_data->mid_rect.bottom;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)
    {
      p_min_rect->right = p_data->mid_rect.right;
      p_min_rect->left = p_min_rect->right - temp;
      if(p_data->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->left += cur_size;
      }
    }
    else                        //0 : left is min value
    {
      p_min_rect->left = p_data->mid_rect.left;
      p_min_rect->right = p_min_rect->left + temp;
      if(p_data->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->right -= cur_size;
      }
    }
    return TRUE;
  }
  else                          //0 is vertical
  {
    temp = p_data->mid_rect.bottom - p_data->mid_rect.top;
    temp = temp * (p_data->curn - p_data->min) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    cur_size = _pbar_get_mid_height(p_data);
    if(cur_size > temp)
    {
      return FALSE;
    }

    p_min_rect->left = p_data->mid_rect.left;
    p_min_rect->right = p_data->mid_rect.right;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)
    {
      p_min_rect->top = p_data->mid_rect.top;
      p_min_rect->bottom = p_min_rect->top + temp;
      if(p_data->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->bottom -= cur_size;
      }
    }
    else                        //0 : bottom is min value
    {
      p_min_rect->bottom = p_data->mid_rect.bottom;
      p_min_rect->top = p_min_rect->bottom - temp;
      if(p_data->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->top += cur_size;
      }
    }
    return TRUE;
  }
}


static BOOL _pbar_get_continuous_max_rect(control_t *p_ctrl, rect_t *p_max_rect)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->max_rstyle == 0xFF)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_data->mid_rect.right - p_data->mid_rect.left;
    temp = temp * (p_data->max - p_data->curn) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_max_rect->top = p_data->mid_rect.top;
    p_max_rect->bottom = p_data->mid_rect.bottom;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1, right is min value
    {
      p_max_rect->left = p_data->mid_rect.left;
      p_max_rect->right = p_max_rect->left + temp;
    }
    else                        //0 : left is min value
    {
      p_max_rect->right = p_data->mid_rect.right;
      p_max_rect->left = p_max_rect->right - temp;
    }
    return TRUE;
  }
  else                          //0 is vertical
  {
    temp = p_data->mid_rect.bottom - p_data->mid_rect.top;
    temp = temp * (p_data->max - p_data->curn) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_max_rect->left = p_data->mid_rect.left;
    p_max_rect->right = p_data->mid_rect.right;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1: top is min value
    {
      p_max_rect->bottom = p_data->mid_rect.bottom;
      p_max_rect->top = p_max_rect->bottom - temp;
    }
    else                        //0 : bottom is min value
    {
      p_max_rect->top = p_data->mid_rect.top;
      p_max_rect->bottom = p_max_rect->top + temp;
    }
    return TRUE;
  }
}


static BOOL _pbar_get_continuous_mid_rect(control_t *p_ctrl, rect_t *p_mid_rect)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 temp = 0, cur_size = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_data->mid_rect.right - p_data->mid_rect.left;
    temp = temp * (p_data->curn - p_data->min) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_mid_rect->top = p_data->mid_rect.top;
    p_mid_rect->bottom = p_data->mid_rect.bottom;

    cur_size = _pbar_get_mid_width(p_data);
    if(cur_size > temp)
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
      {
        p_mid_rect->right = p_data->mid_rect.right;
        p_mid_rect->left = p_mid_rect->right - temp;
      }
      else                        //0 : left is min value
      {
        p_mid_rect->left = p_data->mid_rect.left;
        p_mid_rect->right = p_mid_rect->left + temp;
      }
    }
    else
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
      {
        p_mid_rect->left = p_data->mid_rect.right - temp;
        p_mid_rect->right = p_mid_rect->left + cur_size;
      }
      else                        //0 : left is min value
      {
        p_mid_rect->right = p_data->mid_rect.left + temp;
        p_mid_rect->left = p_mid_rect->right - cur_size;
      }
    }

    return TRUE;
  }
  else                            //0 is vertical
  {
    temp = p_data->mid_rect.bottom - p_data->mid_rect.top;
    temp = temp * (p_data->curn - p_data->min) / (p_data->max - p_data->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_mid_rect->left = p_data->mid_rect.left;
    p_mid_rect->right = p_data->mid_rect.right;

    cur_size = _pbar_get_mid_height(p_data);
    if(cur_size > temp)
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
      {
        p_mid_rect->top = p_data->mid_rect.top;
        p_mid_rect->bottom = p_mid_rect->top + temp;
      }
      else                        //0 : bottom is min value
      {
        p_mid_rect->bottom = p_data->mid_rect.bottom;
        p_mid_rect->top = p_mid_rect->bottom - temp;
      }
    }
    else
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
      {
        p_mid_rect->bottom = p_data->mid_rect.top + temp;
        p_mid_rect->top = p_mid_rect->bottom - cur_size;
      }
      else                        //0 : bottom is min value
      {
        p_mid_rect->top = p_data->mid_rect.bottom - temp;
        p_mid_rect->bottom = p_mid_rect->top + cur_size;
      }
    }

    return TRUE;
  }
}


static u16 _pbar_get_cur_block_index(control_t *p_ctrl)
{
  u16 size = 0, temp1 = 0, temp2 = 0;
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;

  if(p_data->curn <= p_data->min)
  {
    return 0;
  }

  if(p_data->curn >= p_data->max)
  {
    return p_data->step;
  }

  size = p_data->max - p_data->min;
  temp1 = size / p_data->step;
  temp2 = size % p_data->step;
  //the block 0 ~ (temp2-1) have temp1+1
  //the block temp2 ~ (step - temp2 +1) have temp1
  if(temp2 * (temp1 + 1) > p_data->curn)
  {
    return p_data->curn / (temp1 + 1);
  }
  else
  {
    return (p_data->curn - temp2) / temp1;
  }
}


static u16 _pbar_calc_curn(control_t *p_ctrl, u16 index)
{
  u16 size = 0, temp1 = 0, temp2 = 0;
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;

  if(index >= (p_data->step))
  {
    return p_data->max;
  }

  size = p_data->max - p_data->min;
  temp1 = size / p_data->step;
  temp2 = size % p_data->step;
  //the block 0 ~ (temp2-1) have temp1+1
  //the block temp2 ~ (step - temp2 +1) have temp1
  if(index < temp2)
  {
    return p_data->min + index * (temp1 + 1);
  }
  else
  {
    return p_data->min + temp2 + index * temp1;
  }
}


//block index: from 0 ~ (p_data->step-1) , 0 is min size
static u16 _pbar_get_block_rstyle(control_t *p_ctrl, u16 index)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 cur_index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  cur_index = _pbar_get_cur_block_index(p_ctrl);
  if(index < cur_index)
  {
    return p_data->min_rstyle;
  }
  else if(index > cur_index)
  {
    return p_data->max_rstyle;
  }
  else if(cur_index == 0)
  {
    return p_data->min_rstyle;
  }
  else
  {
    return p_data->cur_rstyle;
  }
}


static BOOL _pbar_get_block_rect(control_t *p_ctrl,
                                 u16 index,
                                 rect_t *p_block_rect)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_data->mid_rect.right - p_data->mid_rect.left -
           (p_data->step - 1) * (p_data->interval);

    p_block_rect->top = p_data->mid_rect.top;
    p_block_rect->bottom = p_data->mid_rect.bottom;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
    {
      p_block_rect->right = p_data->mid_rect.right - temp * index /
                            p_data->step - index * (p_data->interval);
      p_block_rect->left = p_data->mid_rect.right - temp *
                           (index +
                            1) / p_data->step - index * (p_data->interval);
    }
    else                        //0 : left is min value
    {
      p_block_rect->left = p_data->mid_rect.left + temp * index /
                           p_data->step +
                           index * (p_data->interval);
      p_block_rect->right = p_data->mid_rect.left + temp *
                            (index +
                             1) / p_data->step + index * (p_data->interval);
    }
  }
  else                            //0 is vertical
  {
    temp = p_data->mid_rect.bottom - p_data->mid_rect.top -
           (p_data->step - 1) * (p_data->interval);

    p_block_rect->left = p_data->mid_rect.left;
    p_block_rect->right = p_data->mid_rect.right;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
    {
      p_block_rect->top = p_data->mid_rect.top + temp * index / p_data->step +
                          index * (p_data->interval);
      p_block_rect->bottom = p_data->mid_rect.top + temp *
                             (index +
                              1) / p_data->step + index * (p_data->interval);
    }
    else                        //0 : bottom is min value
    {
      p_block_rect->bottom = p_data->mid_rect.bottom - temp * index /
                             p_data->step - index * (p_data->interval);
      p_block_rect->top = p_data->mid_rect.bottom - temp *
                          (index +
                           1) / p_data->step - index * (p_data->interval);
    }
  }
  return TRUE;
}


static void _pbar_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_pbar_data_t *p_data = NULL;
  rect_t temp_rect;
  rsc_rstyle_t *p_rstyle = NULL;
  u16 i = 0;
  u16 color = 0;

  gui_paint_frame(hdc, p_ctrl);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->step == 0)
  {
    return;
  }

  if(p_ctrl->priv_attr & PBAR_CONTINUOUS_MASK) // 1 is continuous bar
  {
    if(_pbar_get_continuous_min_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_data->min_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }
    if(_pbar_get_continuous_max_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_data->max_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }
    if(TRUE == _pbar_get_continuous_mid_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_data->cur_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }
  }
  else                          // 0 is block bar
  {
    //	u16	index = _pbar_get_cur_block_index(ctrl);
    //	for(i=0; i<index; i++){
    for(i = p_data->min; i < p_data->max; i++)
    {
      if(_pbar_get_block_rect(p_ctrl, i, &temp_rect))
      {
        color = _pbar_get_block_rstyle(p_ctrl, i);
        p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color);
        gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
      }
    }
  }
}


RET_CODE pbar_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_pbar_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of progressbar class
  p_default_ctrl->priv_attr = PBAR_DIRECTION_MASK | PBAR_CONTINUOUS_MASK;
  p_default_ctrl->p_proc = pbar_class_proc;
  p_default_ctrl->p_paint = _pbar_draw;

  // initalize the default data of progressbar class
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_PBAR, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void pbar_set_mid_rect(control_t *p_ctrl,
                       s16 left,
                       s16 top,
                       s16 right,
                       s16 bottom)
{
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  p_data->mid_rect.left = left;
  p_data->mid_rect.top = top;
  p_data->mid_rect.right = right;
  p_data->mid_rect.bottom = bottom;
}


void pbar_set_rstyle(control_t *p_ctrl,
                     u8 min_rstyle,
                     u8 max_rstyle,
                     u8 cur_rstyle)
{
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  p_data->min_rstyle = min_rstyle;
  p_data->max_rstyle = max_rstyle;
  p_data->cur_rstyle = cur_rstyle;
}


void pbar_set_count(control_t *p_ctrl, u16 min, u16 max, u16 step)
{
  ctrl_pbar_data_t *p_data = NULL;

  if(p_ctrl == NULL)
  {
    return;
  }
  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  p_data->min = min;
  p_data->max = max;
  p_data->step = step;
}


void pbar_set_current(control_t *p_ctrl, u16 curn)
{
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  p_data->curn = curn;
}


u16 pbar_get_current(control_t *p_ctrl)
{
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  return p_data->curn;
}


void pbar_set_direction(control_t *p_ctrl, u8 dir)
{
  MT_ASSERT(p_ctrl != NULL);

  if(dir != 0)
  {
    p_ctrl->priv_attr |= PBAR_DIRECTION_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~PBAR_DIRECTION_MASK);
  }
}


void pbar_set_workmode(control_t *p_ctrl, u8 continuous, u8 interval)
{
  ctrl_pbar_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(continuous != 0)
  {
    p_ctrl->priv_attr |= PBAR_CONTINUOUS_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~PBAR_CONTINUOUS_MASK);
    p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
    p_data->interval = interval;
  }
}


BOOL pbar_increase(control_t *p_ctrl)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->curn < p_data->max)
  {
    index = _pbar_get_cur_block_index(p_ctrl);
    if(index == (p_data->step - 1))
    {
      p_data->curn = p_data->max;
    }
    else
    {
      p_data->curn = _pbar_calc_curn(p_ctrl, (u16)(index + 1));
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


BOOL pbar_decrease(control_t *p_ctrl)
{
  ctrl_pbar_data_t *p_data = NULL;
  u16 index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_pbar_data_t *)p_ctrl->priv_data;
  if(p_data->curn > p_data->min)
  {
    index = _pbar_get_cur_block_index(p_ctrl);
    if(index == 0)
    {
      p_data->curn = p_data->min;
    }
    else
    {
      p_data->curn = _pbar_calc_curn(p_ctrl, (u16)(index - 1));
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


static RET_CODE on_pbar_increase(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  pbar_increase(p_ctrl);
  return SUCCESS;
}


static RET_CODE on_pbar_decrease(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  pbar_decrease(p_ctrl);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(pbar_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_INCREASE, on_pbar_increase)
ON_COMMAND(MSG_DECREASE, on_pbar_decrease)
END_CTRLPROC(pbar_class_proc, ctrl_default_proc)
