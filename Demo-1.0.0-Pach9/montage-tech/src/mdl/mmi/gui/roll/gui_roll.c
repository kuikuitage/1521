/******************************************************************************/
/******************************************************************************/
/*!
 \file gui_resource.c
   this file  implement the functions defined in  gui_resource.h, also it implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar control.
 */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"
#include "charsto.h"

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
#include "class_factory.h"

/*!
   gui roll info.
  */
typedef struct
{
  /*!
    first node
    */
  roll_node_t *p_first;
  /*!
    node heap
    */
  lib_memf_t node_heap;
  /*!
    node heap addr
    */
  void *p_node_heap_addr;
  /*!
    roll pps
    */  
  u8 pps;    
}gui_roll_t;

void gui_roll_init(u8 max_cnt, u8 pps)
{
  class_handle_t p_handle = NULL;
  gui_roll_t *p_info = NULL;

  if((max_cnt == 0))
  {
    return;
  }

  p_handle = (void *)mmi_alloc_buf(sizeof(gui_roll_t));
  MT_ASSERT(p_handle != NULL);

  memset((void *)p_handle, 0, sizeof(gui_roll_t));
  class_register(ROLL_CLASS_ID, p_handle);

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->p_node_heap_addr =
    mmi_create_memf(&p_info->node_heap, max_cnt, sizeof(roll_node_t));
  MT_ASSERT(p_info->p_node_heap_addr != NULL);

  p_info->pps = pps;
}

void gui_roll_release(void)
{
  gui_roll_t *p_info = NULL;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  mmi_destroy_memf(&p_info->node_heap, p_info->p_node_heap_addr);
}

static void gui_roll_add(control_t *p_ctrl, roll_param_t *p_param,
  handle_t handle, u32 addr, u16 width, u16 height, rect_t *p_dst_rc, u32 context)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_last = NULL;
  roll_node_t *p_tmp = NULL;
  u16 tot_w = 0, tot_h = 0;
  u16 dst_w = 0, dst_h = 0;

  MT_ASSERT(p_dst_rc != NULL);
  MT_ASSERT(p_param != NULL);
  MT_ASSERT(width > 0);
  MT_ASSERT(height > 0);
  MT_ASSERT(handle != 0);
  MT_ASSERT(p_ctrl != NULL);  

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  //try to find the last node.
  p_last = p_info->p_first;

  while(p_last != NULL)
  {
    p_tmp = p_last;
    p_last = p_last->p_next;
  }

  p_last = p_tmp;

  //create a new node
  p_tmp = (roll_node_t *)lib_memf_alloc(&p_info->node_heap);
  MT_ASSERT(p_tmp != NULL);

  memset(p_tmp, 0, sizeof(roll_node_t));

  p_tmp->p_ctrl = p_ctrl;
  p_tmp->blt_x = 0;
  p_tmp->blt_y = 0;
  p_tmp->param.style = p_param->style;
  p_tmp->param.pace = p_param->pace;
  p_tmp->param.repeats = p_param->repeats;
  p_tmp->context = context;
  p_tmp->handle = handle;
  p_tmp->addr = addr;
  p_tmp->rgn_w = width;
  p_tmp->rgn_h = height;
  p_tmp->dst_rc = *p_dst_rc;
  p_tmp->curn = 0;
  p_tmp->curn_times = 0;
  p_tmp->direct = FALSE;

  dst_w = RECTW(p_tmp->dst_rc);
  dst_h = RECTH(p_tmp->dst_rc);  
  
  tot_w = p_tmp->rgn_w + dst_w;
  tot_h = p_tmp->rgn_h + dst_h;

  switch(p_tmp->param.style)
  {
    case ROLL_LR:
    case ROLL_LRC:
      p_tmp->blt_x = 0;
      break;

    case ROLL_RL:
      p_tmp->blt_x = (tot_w - 1);
      break;
      
    case ROLL_TB:
    case ROLL_TBC:

      p_tmp->blt_y = 0;
      break;

    case ROLL_BT:
      p_tmp->blt_y = (tot_h - 1);
      break;

      
    default:
      break;
  }  
  
  //add new node.
  if(p_last == NULL)
  {
    p_info->p_first = p_tmp;
  }
  else
  {
    p_last->p_next = p_tmp;
  }
}

static void gui_roll_delete(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  roll_node_t *p_next = NULL;
  roll_node_t *p_prev = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);  

  //remove node from list.
  p_temp = p_info->p_first;
  p_prev = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_ctrl == p_temp->p_ctrl)
    {
      if(p_temp == p_info->p_first)
      {
        p_next = p_info->p_first->p_next;
        p_info->p_first = p_next;
      }
      else
      {
        p_prev->p_next = p_temp->p_next;
      }

      gdi_delete_rsurf(p_temp->handle, p_temp->addr);
      
      //release new node.
      lib_memf_free(&p_info->node_heap, p_temp);

    }
    else
    {
      p_prev = p_temp;
    }

    p_temp = p_temp->p_next;
  }
  
  return;
}

void gui_rolling_node(roll_node_t *p_node, hdc_t hdc)
{
  hdc_t sdc = 0;
  rect_t src_rect = {0};
  u16 tot_w = 0, tot_h = 0;
  u16 src_x = 0, src_y = 0;
  u16 src_w = 0, src_h = 0;
  u16 dst_x = 0, dst_y = 0;
  u16 dst_w = 0, dst_h = 0;
  gui_roll_t *p_info = NULL;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);  

  MT_ASSERT(p_node != NULL);
  
  dst_w = RECTW(p_node->dst_rc);
  dst_h = RECTH(p_node->dst_rc);  
  
  tot_w = p_node->rgn_w + dst_w;
  tot_h = p_node->rgn_h + dst_h;
      
  switch(p_node->param.style)
  {
    case ROLL_LR:
      p_node->blt_x += p_info->pps;
      if(p_node->blt_x > (tot_w - 1))
      {
        p_node->blt_x = 0;
        p_node->curn_times++;
      }
      break;

    case ROLL_RL:
      p_node->blt_x -= p_info->pps;
      
      if(p_node->blt_x < 0)
      {
        p_node->blt_x = (tot_w - 1);
        p_node->curn_times++;
      }
      break;
      
    case ROLL_LRC:
      if(p_node->direct == TRUE)
      {
        p_node->blt_x += p_info->pps;
        if(p_node->blt_x > (tot_w - 1))
        {
          p_node->blt_x = (tot_w - 1);
          p_node->direct = FALSE;
        }
      }
      else
      {
        p_node->blt_x -= p_info->pps;
        if(p_node->blt_x < 0)
        {
          p_node->blt_x = 0;
          p_node->direct = TRUE;
        }
      }
      break;

    case ROLL_TB:
      p_node->blt_y += p_info->pps;
      if(p_node->blt_y > (tot_h - 1))
      {
        p_node->blt_y = 0;
        p_node->curn_times++;
      }
      break;

    case ROLL_BT:
      p_node->blt_y -= p_info->pps;
      if(p_node->blt_y < 0)
      {
        p_node->blt_y = (tot_h - 1);
        p_node->curn_times++;
      }
      break;

    case ROLL_TBC:
      if(p_node->direct == TRUE)
      {
        p_node->blt_y += p_info->pps;
        if(p_node->blt_y > (tot_h - 1))
        {
          p_node->blt_y = (tot_h - 1);
          p_node->direct = FALSE;
        }
      }
      else
      {
        p_node->blt_y -= p_info->pps;
        if(p_node->blt_y < 0)
        {
          p_node->blt_y = 0;
          p_node->direct = TRUE;
        }
      }
      break;
      
    default:
      break;
  }

  if((p_node->param.repeats != 0) && (p_node->curn_times == p_node->param.repeats))
  {
    gui_stop_roll(p_node->p_ctrl);
    ctrl_process_msg((control_t *)p_node->p_ctrl, MSG_ROLL_STOPPED, 0, 0);

    return;
  }

  src_rect.right = p_node->rgn_w;
  src_rect.bottom = p_node->rgn_h;

  if((p_node->param.style == ROLL_TB)
    || (p_node->param.style == ROLL_BT)
    || (p_node->param.style == ROLL_TBC))
  {    
    if(p_node->rgn_w <= dst_w)
    {
      if(p_node->blt_y <= p_node->rgn_h)
      {
        src_x = p_node->blt_x;
        src_y = 0;
        src_w = dst_w - p_node->blt_x;
        src_h = p_node->blt_y;
        dst_x = 0;
        dst_y = dst_h - p_node->blt_y;
      }
      else if(p_node->blt_y <= dst_h)
      {
        src_x = p_node->blt_x;
        src_y = 0;        
        src_w = dst_w - p_node->blt_x;
        src_h = p_node->rgn_h;
        dst_x = 0;
        dst_y = dst_h - p_node->blt_y;
      }
      else
      {
        src_x = p_node->blt_x;
        src_y = (p_node->blt_y - dst_h);        
        src_w = dst_w - p_node->blt_x;
        src_h = tot_h - p_node->blt_y;
        dst_x = 0;
        dst_y = 0;
      }
    }
    else
    {
      if(p_node->blt_y <= dst_h)
      {
        src_x = p_node->blt_x;
        src_y = 0;       
        src_w = dst_w - p_node->blt_x;
        src_h = p_node->blt_y;
        dst_x = 0;
        dst_y = dst_h - p_node->blt_y;      
      }
      else if(p_node->blt_y <= p_node->rgn_h)
      {
        src_x = p_node->blt_x;
        src_y = p_node->blt_y - dst_h;       
        src_w = dst_w - p_node->blt_x;
        src_h = dst_h;
        dst_x = 0;
        dst_y = 0;      
      }
      else
      {
        src_x = p_node->blt_x;
        src_y = p_node->blt_y - dst_h;        
        src_w = dst_w - p_node->blt_x;
        src_h = tot_h - p_node->blt_y;
        dst_x = 0;
        dst_y = 0;        
      }    
    }
  }
  else if((p_node->param.style == ROLL_LR)
    || (p_node->param.style == ROLL_RL)
    || (p_node->param.style == ROLL_LRC))
  {
    if(p_node->rgn_w <= dst_w)
    {
      if(p_node->blt_x <= p_node->rgn_w)
      {
        src_x = 0;
        src_y = p_node->blt_y;
        src_w = p_node->blt_x;
        src_h = dst_h - p_node->blt_y;
        dst_x = dst_w - p_node->blt_x;
        dst_y = 0;
      }
      else if(p_node->blt_x <= dst_w)
      {
        src_x = 0;
        src_y = p_node->blt_y;        
        src_w = p_node->rgn_w;
        src_h = dst_h - p_node->blt_y;
        dst_x = dst_w - p_node->blt_x;
        dst_y = 0;
      }
      else
      {
        src_x = (p_node->blt_x - dst_w);
        src_y = p_node->blt_y;        
        src_w = tot_w - p_node->blt_x;
        src_h = dst_h - p_node->blt_y;
        dst_x = 0;
        dst_y = 0;
      }
    }
    else
    {
      if(p_node->blt_x <= dst_w)
      {
        src_x = 0;
        src_y = p_node->blt_y;        
        src_w = p_node->blt_x;
        src_h = dst_h - p_node->blt_y;
        dst_x = dst_w - p_node->blt_x;
        dst_y = 0;      
      }
      else if(p_node->blt_x <= p_node->rgn_w)
      {
        src_x = p_node->blt_x - dst_w;
        src_y = p_node->blt_y;        
        src_w = dst_w;
        src_h = dst_h - p_node->blt_y;
        dst_x = 0;
        dst_y = 0;      
      }
      else
      {
        src_x = p_node->blt_x - dst_w;
        src_y = p_node->blt_y;        
        src_w = tot_w - p_node->blt_x;
        src_h = dst_h - p_node->blt_y;
        dst_x = 0;
        dst_y = 0;        
      }    
    }
  }
  
  if((src_w != 0) && (src_h != 0))
  {
    dst_x += p_node->dst_rc.left;
    dst_y += p_node->dst_rc.top;
    
    sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
    gdi_bitblt(sdc, src_x, src_y,
      src_w, src_h, hdc, dst_x, dst_y, 0);
    gdi_release_roll_dc(sdc);
  }

  return;
}

void gui_rolling(void)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);  

  p_temp = p_info->p_first;

  while(p_temp != NULL)
  {
    MT_ASSERT(p_temp->curn < p_temp->param.pace);

    p_temp->curn++;
    
    if(p_temp->curn == p_temp->param.pace)
    {
      ctrl_process_msg(p_temp->p_ctrl, MSG_ROLLING, (u32)p_temp, 0);

      p_temp->curn = 0;
    }
    
    p_temp = p_temp->p_next;
  }
}

BOOL ctrl_is_rolling(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);

  if(p_info == NULL)
  {
    return FALSE;
  }

  p_temp = p_info->p_first;
  
  while(p_temp != NULL)
  {
    if(p_temp->p_ctrl == p_ctrl)
    {
      return TRUE;
    }
    
    p_temp = p_temp->p_next;
  }

  return FALSE;
}



void gui_start_roll(control_t *p_ctrl, roll_param_t *p_param)
{
  ctrl_process_msg(p_ctrl, MSG_START_ROLL, (u32)p_param, 0);  
}


void gui_stop_roll(control_t *p_ctrl)
{
  gui_roll_delete(p_ctrl);
}

void gui_roll_reset_content(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  roll_node_t *p_next = NULL;
  roll_node_t *p_prev = NULL;
  roll_param_t roll_param = {0};
  BOOL is_found = FALSE;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  if(p_info == NULL)
  {
    return;
  }
  //MT_ASSERT(p_info != NULL);  

  //remove node from list.
  p_temp = p_info->p_first;
  p_prev = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_ctrl == p_temp->p_ctrl)
    {
      if(p_temp == p_info->p_first)
      {
        p_next = p_info->p_first->p_next;
        p_info->p_first = p_next;
      }
      else
      {
        p_prev->p_next = p_temp->p_next;
      }

      is_found = TRUE;
      
      memcpy(&roll_param, &p_temp->param, sizeof(roll_param_t));

      gdi_delete_rsurf(p_temp->handle, p_temp->addr);
      
      //release new node.
      lib_memf_free(&p_info->node_heap, p_temp);
    }
    else
    {
      p_prev = p_temp;
    }

    p_temp = p_temp->p_next;
  }


  if(is_found)
  {
    gui_start_roll(p_ctrl, &roll_param);
  }
  return;  
}

void gui_create_rsurf(control_t *p_ctrl, u8 fstyle_idx, u16 *p_str,
  rect_t *p_rc, u32 draw_style, roll_param_t *p_param, u32 context)
{
  rsc_fstyle_t *p_fstyle = NULL;
  u16 width = 0, height = 0;
  handle_t handle = 0;
  rect_t str_rect = {0};
  hdc_t hdc = 0;
  u32 color_key = 0;
  u32 surf_handle = 0;
  u32 addr = 0;
  u8  ansstr[10]; 

  if(p_str == NULL)
  {
    return;
  }
  
  p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), fstyle_idx);

  color_key = (p_fstyle->color ? 0 : 1);

  rsc_get_string_attr(gui_get_rsc_handle(), p_str, p_fstyle, &width, &height);
  if(width == 0)
  {
    sprintf((char*)ansstr, "%s "," ");
    str_asc2uni(ansstr, p_str);
    rsc_get_string_attr(gui_get_rsc_handle(), p_str, p_fstyle, &width, &height);
  }

  handle = gdi_create_rsurf(width, RECTHP(p_rc), &addr);
  MT_ASSERT(handle != 0);
  MT_ASSERT(addr != 0);

  surf_handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  surface_set_colorkey(surf_handle, (void *)handle, color_key);

  set_rect(&str_rect, 0, 0, width, RECTHP(p_rc));

  surface_fill_rect(surf_handle, (void *)handle, &str_rect, color_key);

  hdc = gdi_get_roll_dc(&str_rect, handle);
  MT_ASSERT(hdc != HDC_INVALID);

  gui_draw_unistr(hdc, &str_rect, 0, 0, 0, 0,
    p_str, fstyle_idx, draw_style | STRDRAW_IGNORE_NLINE);  

  gdi_release_roll_dc(hdc);
  
  gui_roll_add(p_ctrl, p_param, handle, addr, width, RECTHP(p_rc), p_rc, context);

  return;
}
