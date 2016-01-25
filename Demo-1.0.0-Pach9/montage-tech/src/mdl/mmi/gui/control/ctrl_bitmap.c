/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_bitmap.c
   this file  implement the funcs defined in  ctrl_bitmap.h,and some internal
   used functions.
   These functions are about register,set feature and draw a bitmap control.
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

#include "ctrl_bitmap.h"

static void _bmap_draw(control_t *p_ctrl, hdc_t hdc)
{
  rect_t bmap_rect;
  ctrl_bmap_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  gui_paint_frame(hdc, p_ctrl);
  //draw bitmap
  p_data = (ctrl_bmap_data_t *)p_ctrl->priv_data;
  ctrl_get_client_rect(p_ctrl, &bmap_rect);

  if(p_ctrl->priv_attr & BMAP_DATAMODE_MASK)
  {
    u16 left = 0, top = 0;
    bitmap_t *p_bmp = (bitmap_t *)p_data->data;
    rect_t temp;
    gui_get_posi(&bmap_rect, p_ctrl->priv_attr, p_data->left,
                 p_data->top,
                 (u16)(p_bmp->width), (u16)(p_bmp->height), &left,
                 &top);
    set_rect(&temp, left, top, (s16)(left + p_bmp->width),
             (s16)(top + p_bmp->height));

    gdi_fill_bitmap(hdc, &temp, p_data->data, 0);
  }
  else
  {
    gui_draw_picture(hdc, &bmap_rect, p_ctrl->priv_attr, p_data->left,
                     p_data->top,
                     (u16)p_data->data);
  }
}


RET_CODE bmap_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_bmap_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of bitmap class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = bmap_class_proc;
  p_default_ctrl->p_paint = _bmap_draw;

  // initalize the default data of bitmap class

  if(ctrl_register_ctrl_class((u8 *)CTRL_BMAP, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void bmap_set_bmap_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_bmap_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_bmap_data_t *)p_ctrl->priv_data;
  p_data->left = left;
  p_data->top = top;
}


void bmap_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~BMAP_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & BMAP_ALIGN_MASK); //set new style
}


void bmap_set_content_by_id(control_t *p_ctrl, u16 bmap_id)
{
  ctrl_bmap_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

//  if(p_ctrl->priv_attr & BMAP_DATAMODE_MASK)
//  {
//    return;
//  }
  MT_ASSERT(!(p_ctrl->priv_attr & BMAP_DATAMODE_MASK));

  p_data = (ctrl_bmap_data_t *)p_ctrl->priv_data;
  p_data->data = bmap_id;
}


void bmap_set_content_by_data(control_t *p_ctrl, bitmap_t *p_bmp)
{
  ctrl_bmap_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

//  if(!(p_ctrl->priv_attr & BMAP_DATAMODE_MASK))
//  {
//    return;
//  }
  MT_ASSERT((p_ctrl->priv_attr & BMAP_DATAMODE_MASK));

  p_data = (ctrl_bmap_data_t *)p_ctrl->priv_data;
  p_data->data = (u32)p_bmp;
}


u16 bmap_get_content(control_t *p_ctrl)
{
  ctrl_bmap_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_bmap_data_t *)p_ctrl->priv_data;
  return (u16)(p_data->data);
}


void bmap_set_content_type(control_t *p_ctrl, BOOL enable)
{
  MT_ASSERT(p_ctrl != NULL);

  if(enable)
  {
    p_ctrl->priv_attr |= BMAP_DATAMODE_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~BMAP_DATAMODE_MASK);
  }
}


// define the default msgmap of class
BEGIN_CTRLPROC(bmap_class_proc, ctrl_default_proc)
END_CTRLPROC(bmap_class_proc, ctrl_default_proc)

