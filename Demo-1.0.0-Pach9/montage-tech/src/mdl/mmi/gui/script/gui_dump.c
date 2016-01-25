/******************************************************************************/
/******************************************************************************/
/*!
\file gui_dump.c
this file  implement the functions defined in  gui_dump.h, also it implement
some internal used function. All these functions are about how to open a menu
by script data.
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

#include "ctrl_container.h"
#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"
#include "ctrl_progressbar.h"
#include "ctrl_matrixbox.h"
#include "ctrl_list.h"
#include "ctrl_combobox.h"
#include "ctrl_bitmap.h"
#include "ctrl_editbox.h"
#include "ctrl_common.h"
#include "ctrl_string.h"
#include "ctrl_numbox.h"

#ifdef SPT_DUMP_DATA
#include <stdio.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "gui_script.h"
#include "gui_dump.h"

#ifdef SPT_DUMP_DATA
void dump_control_common(control_t *p_ctrl, FILE *p_fd);
u8 check_control_type(char *p_type);
void dump_bmap(control_t *p_bmap, FILE *p_fd);
void dump_cbox(control_t *p_cbox, FILE *p_fd);
void dump_cont(control_t *p_cont, FILE *p_fd);
void dump_ebox(control_t *p_ebox, FILE *p_fd);
void dump_list(control_t *p_list, FILE *p_fd);
void dump_mbox(control_t *p_mbox, FILE *p_fd);
void dump_nbox(control_t *p_nbox, FILE *p_fd);
void dump_pbar(control_t *p_pbar, FILE *p_fd);
void dump_sbar(control_t *p_sbar, FILE *p_fd);
void dump_text(control_t *p_text, FILE *p_fd);
void dump_control(control_t *p_ctrl, FILE *p_fd);

u32 _cal_ctrl_length(u8 type, u32 priv_attr)
{
  u32 length = 0;

  switch(type)
  {
  case TYPE_CONT:
    length = sizeof(spt_ctrl_t);
    break;
  case TYPE_BMAP:
    length = sizeof(spt_ctrl_t) + sizeof(spt_bmap_data_t);
    break;
  case TYPE_CBOX:
    switch(priv_attr & CBOX_WORKMODE_MASK)
    {
      case CBOX_WORKMODE_STATIC:
        length = sizeof(spt_ctrl_t) + sizeof(spt_cbox_static_data_t);
        break;
      case CBOX_WORKMODE_NUMBER:
        length = sizeof(spt_ctrl_t) + sizeof(spt_cbox_num_data_t);
        break;
      case CBOX_WORKMODE_DYNAMIC:
        length = sizeof(spt_ctrl_t) + sizeof(spt_cbox_dync_data_t);
        break;
      default:
        break;
    }
    break;
  case TYPE_EBOX:
    length = sizeof(spt_ctrl_t) + sizeof(spt_ebox_data_t);
    break;
  case TYPE_LIST:
    length = sizeof(spt_ctrl_t) + sizeof(spt_list_data_t);
    break;
  case TYPE_MBOX:
    length = sizeof(spt_ctrl_t) + sizeof(spt_mbox_data_t);
    break;
  case TYPE_NBOX:
    length = sizeof(spt_ctrl_t) + sizeof(spt_nbox_data_t);
    break;
  case TYPE_PBAR:
    length = sizeof(spt_ctrl_t) + sizeof(spt_pbar_data_t);
    break;
  case TYPE_SBAR:
    length = sizeof(spt_ctrl_t) + sizeof(spt_sbar_data_t);
    break;
  case TYPE_TEXT:
    length = sizeof(spt_ctrl_t) + sizeof(spt_text_data_t);
    break;
  default:
    length = 0;
    break;
  }
  return length;  
}

u32 _cal_offset(control_t *p_dst, control_t *p_src)
{
  control_t *p_tmp = NULL;
  control_t *p_ctrl = NULL;
  u8 type = 0;
  u32 offset = 0;

  if(p_dst == NULL)
  {
    return 0;             //destination control is NULL, return 0;
  }

  if(p_src == NULL)
  {
    return 0;//source control is NULL, return 0;
  }

  p_ctrl = p_src;

  while(1)
  {
    if(p_ctrl != NULL)
    {
      if(p_ctrl == p_dst)
      {
        return offset;
      }

      type = check_control_type(p_ctrl->p_oci->name);
      offset += _cal_ctrl_length(type, p_ctrl->priv_attr);
      p_tmp = p_ctrl;
      p_ctrl = p_ctrl->p_child;
    }
    else
    {
      if(p_tmp->p_next != NULL)
      {
        p_ctrl = p_tmp->p_next;
      }
      else 
      {
        while(p_tmp->p_parent->p_next == NULL)
        {
          p_tmp = p_tmp->p_parent;
        }
        p_ctrl = p_tmp->p_parent->p_next;
      }
    }
  }   
}
u32 relative_control_offset(control_t *p_ctrl, u8 type)
{
  control_t *p_root_ctrl = NULL;

  MT_ASSERT(p_ctrl != NULL);
  //search for the root control.
  p_root_ctrl = p_ctrl;
  while(p_root_ctrl->p_parent != NULL)
  {
    p_root_ctrl = p_root_ctrl->p_parent;
  }

  switch(type)
  {
  case PARENT_OFFSET:
    return _cal_offset(p_ctrl->p_parent, p_root_ctrl); 
  case CHILD_OFFSET:
    return _cal_offset(p_ctrl->p_child, p_root_ctrl);
  case NEXT_OFFSET:
    if(p_root_ctrl == p_ctrl)
    {
      return 0;
    }
    else
    {
      return _cal_offset(p_ctrl->p_next, p_root_ctrl);
    }
  case PREV_OFFSET:
    if(p_root_ctrl == p_ctrl)
    {
      return 0;
    }
    else
    {      
      return _cal_offset(p_ctrl->p_prev, p_root_ctrl);
    }
  case ACTIVE_CHILD_OFFSET:
    return _cal_offset(p_ctrl->p_active_child, p_root_ctrl);
  default:
    return 0xFFFFFFFF;
  }
}

u8 check_control_type(char *p_type)
{
  if(!strcmp(p_type, "BITMAP"))
    return TYPE_BMAP;
  else if(!strcmp(p_type, "COMBOBOX"))
    return TYPE_CBOX;
  else if(!strcmp(p_type, "CONTAINER"))
    return TYPE_CONT;
  else if(!strcmp(p_type, "EDITBOX"))
    return TYPE_EBOX;
  else if(!strcmp(p_type, "LIST"))
    return TYPE_LIST;
  else if(!strcmp(p_type, "MATRIXBOX"))
    return TYPE_MBOX;
  else if(!strcmp(p_type, "NUMBOX"))
    return TYPE_NBOX;
  else if(!strcmp(p_type, "PROGRESSBAR"))
    return TYPE_PBAR;
  else if(!strcmp(p_type, "SCROLLBAR"))
    return TYPE_SBAR;
  else if(!strcmp(p_type, "TEXTFIELD"))
    return TYPE_TEXT;
  else
    return TYPE_ERR;
}

void dump_control_common(control_t *p_ctrl, FILE *p_fd)
{
  u8 type = 0;

  type = check_control_type(p_ctrl->p_oci->name);

  //type string
  fprintf(p_fd, "%s\n\t", p_ctrl->p_oci->name);

  //attr, style, id
  fprintf(p_fd, "%d %d  %d  ", (p_ctrl->attr), (p_ctrl->style), (p_ctrl->id));
  
  //related control id
  fprintf(p_fd, "%d %d  %d  %d  ",
    (p_ctrl->left), (p_ctrl->up), (p_ctrl->right), (p_ctrl->down));
  
  //frame	
  fprintf(p_fd, "%d %d  %d  %d  ", (p_ctrl->frame.left), 
    (p_ctrl->frame.top), (p_ctrl->frame.right), (p_ctrl->frame.bottom));
  
  //parent, child, next, prev
  fprintf(p_fd, "%d  ", relative_control_offset(p_ctrl , PARENT_OFFSET));  
  fprintf(p_fd, "%d  ", relative_control_offset(p_ctrl , CHILD_OFFSET));  
  fprintf(p_fd, "%d  ", relative_control_offset(p_ctrl , NEXT_OFFSET));  
  fprintf(p_fd, "%d  ", relative_control_offset(p_ctrl , PREV_OFFSET));  
  
  //private attribe
  fprintf(p_fd, "%d ", (p_ctrl->priv_attr));
  
  //rectangle style.
  fprintf(p_fd,"%d  %d  %d  ", 
    (p_ctrl->rstyle.show_idx),
    (p_ctrl->rstyle.hl_idx),
    (p_ctrl->rstyle.gray_idx));  
  return;
}

void dump_bmap_priv_data(control_t *p_bmap, FILE *p_fd)
{
  ctrl_bmap_data_t *p_bmap_data = NULL;

  p_bmap_data = (ctrl_bmap_data_t *)(p_bmap->priv_data);
  if(p_bmap->priv_attr & BMAP_DATAMODE_MASK)
  {
    p_bmap_data->data = 0;                      
  }  
  
  //bmp data, bmp left, bmp top
  fprintf(p_fd,"%d  %d  %d  ", 
    (p_bmap_data->data), (p_bmap_data->left), (p_bmap_data->top));    

  return;
}

void dump_bmap(control_t *p_bmap, FILE *p_fd)
{
  dump_control_common(p_bmap, p_fd);

  dump_bmap_priv_data(p_bmap, p_fd);

  return;
}

void dump_cbox_priv_data(control_t *p_cbox, FILE *p_fd)
{
  ctrl_cbox_data_t *p_cbox_data = NULL;
  u32 min = 0, max = 0, bit_length = 0;  

  p_cbox_data = (ctrl_cbox_data_t *)(p_cbox->priv_data);

  //string left, string top
  fprintf(p_fd, "%d %d  ", (p_cbox_data->str_left), (p_cbox_data->str_top));
  
  switch(p_cbox->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      break;
    case CBOX_WORKMODE_NUMBER:
      bit_length = p_cbox_data->num_data.bit_length;
      max = p_cbox_data->num_data.max;
      min = p_cbox_data->num_data.min;

      //max, min, curn, step
      fprintf(p_fd, "%d %d  %d  %d  ", 
        max, min, (p_cbox_data->num_data.curn), (p_cbox_data->num_data.step));

      //prefix
      if((p_cbox->priv_attr & CBOX_ITEM_PREFIX_TYPE_MASK) == 
        CBOX_ITEM_PREFIX_TYPE_EXTSTR)
      {
        fprintf(p_fd, "%d ", 0);  
      }
      else
      {
        fprintf(p_fd, "%d ", (p_cbox_data->num_data.prefix));        
      }

      //postfix
      if((p_cbox->priv_attr & CBOX_ITEM_POSTFIX_TYPE_MASK) == 
        CBOX_ITEM_POSTFIX_TYPE_EXTSTR)
      {
        fprintf(p_fd, "%d ", 0);
      }   
      else
      {
        fprintf(p_fd, "%d ", (p_cbox_data->num_data.postfix));  
      }

      //bitlength
      fprintf(p_fd, "%d ", bit_length);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      //total
      fprintf(p_fd, "%d ", (p_cbox_data->dync_data.common.total));
      break;
    default:
      return;
  }

  //n_color, h_color, g_color
  fprintf(p_fd, "%d %d  %d  ", (p_cbox_data->n_fstyle), 
    (p_cbox_data->h_fstyle), (p_cbox_data->g_fstyle));
  return;
}

void dump_cbox(control_t *p_cbox, FILE *p_fd)
{
  dump_control_common(p_cbox, p_fd);

  dump_cbox_priv_data(p_cbox, p_fd);

  return;
}

void dump_cont_priv_data(control_t *p_cont, FILE *p_fd)
{
  ctrl_cont_data_t *p_cont_data = NULL;

  p_cont_data = (ctrl_cont_data_t *)(p_cont->priv_data);

  return;
}

void dump_cont(control_t *p_cont, FILE *p_fd)
{
  dump_control_common(p_cont, p_fd);

  dump_cont_priv_data(p_cont, p_fd);

  return;
}

void dump_ebox_priv_data(control_t *p_ebox, FILE *p_fd)
{
  ctrl_ebox_data_t *p_ebox_data = NULL;

  p_ebox_data = (ctrl_ebox_data_t *)(p_ebox->priv_data);

  //n_color, h_color, g_color, str_l_space
  fprintf(p_fd, "%d %d  %d  %d  ",
    (p_ebox_data->n_fstyle), (p_ebox_data->h_fstyle),
    (p_ebox_data->g_fstyle), (p_ebox_data->str_l_space));

  //str_left, str_top, str_maxtext
  fprintf(p_fd, "%d %d  %d  ", (p_ebox_data->str_left), 
    (p_ebox_data->str_top), (p_ebox_data->str_maxtext));

  return;
}

void dump_ebox(control_t *p_ebox, FILE *p_fd)
{
  dump_control_common(p_ebox, p_fd);

  dump_ebox_priv_data(p_ebox, p_fd);

  return;
}

void dump_list_priv_data(control_t *p_list, FILE *p_fd)
{
  ctrl_list_data_t *p_list_data = NULL;

  p_list_data = (ctrl_list_data_t *)(p_list->priv_data);
  //mid_rect_left, mid_rect_top, mid_rect_right, mid_rect_bottom
  fprintf(p_fd, "%d %d  %d  %d  ", (p_list_data->mid_rect.left), 
    (p_list_data->mid_rect.top), (p_list_data->mid_rect.right), 
    (p_list_data->mid_rect.bottom));

  //page, interval, g_color, n_color  
  fprintf(p_fd, "%d %d %d  %d  ", (p_list_data->page), (p_list_data->interval), 
    (p_list_data->rstyle.g_xstyle), (p_list_data->rstyle.n_xstyle));

  //f_color, s_color, sf_color
  fprintf(p_fd, "%d %d  %d  ", (p_list_data->rstyle.f_xstyle), 
    (p_list_data->rstyle.s_xstyle), (p_list_data->rstyle.sf_xstyle));

  return;
}

void dump_list(control_t *p_list, FILE *p_fd)
{
  dump_control_common(p_list, p_fd);

  dump_list_priv_data(p_list, p_fd);

  return;
}

void dump_mbox_priv_data(control_t *p_mbox, FILE *p_fd)
{
  ctrl_mbox_data_t *p_mbox_data = NULL;

  p_mbox_data = (ctrl_mbox_data_t *)(p_mbox->priv_data);

  //mid_rect_left, mid_rect_top, mid_rect_right, mid_rect_bottom
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_mbox_data->mid_rect.left), (p_mbox_data->mid_rect.top), 
    (p_mbox_data->mid_rect.right), (p_mbox_data->mid_rect.bottom));

  //row, col
  fprintf(p_fd, "%d %d  ", (p_mbox_data->row), (p_mbox_data->col));

  //h_interval, v_interval, str_left, str_top
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_mbox_data->h_interval), (p_mbox_data->v_interval), 
    (p_mbox_data->str_left), (p_mbox_data->str_top));

  //icon left, icon top, item_g_idx, item_n_idx
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_mbox_data->icon_left), (p_mbox_data->icon_top), 
    (p_mbox_data->item_g_idx), (p_mbox_data->item_n_idx));

  //item_f_idx, g_color, n_color, f_color
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_mbox_data->item_f_idx), (p_mbox_data->g_fstyle), 
    (p_mbox_data->n_fstyle), (p_mbox_data->f_fstyle));

  return;
}

void dump_mbox(control_t *p_mbox, FILE *p_fd)
{
  dump_control_common(p_mbox, p_fd);

  dump_mbox_priv_data(p_mbox, p_fd);

  return;
}

void dump_nbox_priv_data(control_t *p_nbox, FILE *p_fd)
{
  ctrl_nbox_data_t *p_nbox_data = NULL;

  p_nbox_data = (ctrl_nbox_data_t *)(p_nbox->priv_data);

  //num, min, max
  fprintf(p_fd, "%d %d  %d  ", 
    (p_nbox_data->num), (p_nbox_data->min), (p_nbox_data->max));

  //curn_bit, bit_length, n_color, h_color
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_nbox_data->curn_bit), (p_nbox_data->bit_length), 
    (p_nbox_data->n_fstyle), (p_nbox_data->h_fstyle));

  //e_color, g_color, str_left, str_top  
  fprintf(p_fd, "%d %d  %d  ", 
    (p_nbox_data->g_fstyle), (p_nbox_data->str_left), (p_nbox_data->str_top));

  //separator, place
//  fprintf(fd, "%d %d  ", (nbox_data->separator), (nbox_data->separator_pos));
  return;
}

void dump_nbox(control_t *p_nbox, FILE *p_fd)
{
  dump_control_common(p_nbox, p_fd);

  dump_nbox_priv_data(p_nbox, p_fd);

  return;
}

void dump_pbar_priv_data(control_t *p_pbar, FILE *p_fd)
{
  ctrl_pbar_data_t *p_pbar_data = NULL;

  p_pbar_data = (ctrl_pbar_data_t *)(p_pbar->priv_data);

  //mid_rect_left, mid_rect_top, mid_rect_right, mid_rect_bottom  
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_pbar_data->mid_rect.left), (p_pbar_data->mid_rect.top), 
    (p_pbar_data->mid_rect.right), (p_pbar_data->mid_rect.bottom));

  //min, max, curn, step
  fprintf(p_fd, "%d %d  %d %d  ", (p_pbar_data->min), 
    (p_pbar_data->max), (p_pbar_data->curn), (p_pbar_data->step));

  //interval, min_color, max_color, cur_color
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_pbar_data->interval), (p_pbar_data->min_rstyle), 
    (p_pbar_data->max_rstyle), (p_pbar_data->cur_rstyle));

  return;
}

void dump_pbar(control_t *p_pbar, FILE *p_fd)
{
  dump_control_common(p_pbar, p_fd);

  dump_pbar_priv_data(p_pbar, p_fd);

  return;
}

void dump_sbar_priv_data(control_t *p_sbar, FILE *p_fd)
{
  ctrl_sbar_data_t *p_sbar_data = NULL;

  p_sbar_data = (ctrl_sbar_data_t *)(p_sbar->priv_data);

  //mid_rect_left, mid_rect_top, mid_rect_right, mid_rect_bottom    
  fprintf(p_fd, "%d %d  %d  %d  ", 
    (p_sbar_data->mid_rect.left), (p_sbar_data->mid_rect.top), 
    (p_sbar_data->mid_rect.right), (p_sbar_data->mid_rect.bottom));

  //n_mid_color, h_mid_color, g_mid_color
  fprintf(p_fd, "%d %d  %d  ", 
    (p_sbar_data->n_mid_rstyle), (p_sbar_data->h_mid_rstyle), 
    (p_sbar_data->g_mid_rstyle));

  //page, total
  fprintf(p_fd, "%d %d  ", (p_sbar_data->page), (p_sbar_data->total));
  
  return;
}

void dump_sbar(control_t *p_sbar, FILE *p_fd)
{
  dump_control_common(p_sbar, p_fd);

  dump_sbar_priv_data(p_sbar, p_fd);

  return;
}

void dump_text_priv_data(control_t *p_text, FILE *p_fd)
{
  ctrl_text_data_t *p_text_data = NULL;

  p_text_data = (ctrl_text_data_t *)(p_text->priv_data);

  //string char
  switch(p_text->priv_attr & TEXT_STRTYPE_MASK)
  {
    case TEXT_STRTYPE_UNICODE:
    case TEXT_STRTYPE_EXTSTR:
      fprintf(p_fd,"%d  ", 0);
      break;
    default:
      fprintf(p_fd,"%d  ", p_text_data->str_char);
      break;
  }

  //n_color, h_color, g_color, str_l_space  
  fprintf(p_fd, "%d  %d %d  %d  ", 
    (p_text_data->n_fstyle), (p_text_data->h_fstyle), 
    (p_text_data->g_fstyle), (p_text_data->str_l_space));

  //str_left, str_top
  fprintf(p_fd, "%d %d  ", (p_text_data->str_left), (p_text_data->str_top));

  return;
}

void dump_text(control_t *p_text, FILE *p_fd)
{
  dump_control_common(p_text, p_fd);

  dump_text_priv_data(p_text, p_fd);

  return;
}

void dump_control(control_t *p_ctrl, FILE *p_fd)
{
  u8 type = 0;

  type = check_control_type(p_ctrl->p_oci->name);

  switch(type)
  {
  case TYPE_CONT:
    dump_cont(p_ctrl, p_fd);
    break;
  case TYPE_BMAP:
    dump_bmap(p_ctrl, p_fd);
    break;
  case TYPE_CBOX:
    dump_cbox(p_ctrl, p_fd);
    break;
  case TYPE_EBOX:
    dump_ebox(p_ctrl, p_fd);
    break;
  case TYPE_LIST:
    dump_list(p_ctrl, p_fd);
    break;
  case TYPE_MBOX:
    dump_mbox(p_ctrl, p_fd);
    break;
  case TYPE_NBOX:
    dump_nbox(p_ctrl, p_fd);
    break;
  case TYPE_PBAR:
    dump_pbar(p_ctrl, p_fd);
    break;
  case TYPE_SBAR:
    dump_sbar(p_ctrl, p_fd);
    break;
  case TYPE_TEXT:
    dump_text(p_ctrl, p_fd);
    break;
  default:
    return;
  }

  fprintf(p_fd, "\n");

}

void spt_dump_menu_data(control_t *p_ctrl)
{
  control_t *p_root = NULL;//root control
  control_t *p_tmp = NULL;
  control_t *p_control = NULL;
  u8 name[20];
  u8 id_str[5];

  FILE *p_fd = NULL;

  //search for the root control.
  p_control = p_ctrl;
  while(p_control != NULL)
  {
    p_tmp = p_control;
    p_control = p_control->p_parent;
  }
  p_control = p_tmp;//tmp is the root control.
  p_root = p_control;

  //create destination file name.  
  strncpy(name, "spt_file/", 20);  
  if(p_control->id < 10)
  {
    strcat(name, "0");
  }
  sprintf(id_str, "%d", p_control->id);
//  itoa(control->id, id_str, 10);
  strcat(name, id_str);
  strcat(name, ".spt");

  //open destination file for write.  
  p_fd = fopen(name, "wb");
  if(p_fd == NULL)
  {
    printf("****Open destination file error!****\n");
    return;
  }

  while(1)
  {
    if(p_control != NULL)
    {
      dump_control(p_control, p_fd);
      p_tmp = p_control;
      p_control = p_control->p_child;
      if(p_control != NULL)
      {
        fprintf(p_fd, "BEGIN\n");
      }
    }
    else
    {
      if(p_tmp->p_next != NULL)
      {
        p_control = p_tmp->p_next;
      }
      else 
      {
        if(p_tmp->p_parent != NULL)
        {
          fprintf(p_fd, "END\n");
          p_tmp = p_tmp->p_parent;
          p_control = p_tmp->p_next;
        }
        else
        {
          return;
        }              
      }
    }
  }

  fclose(p_fd);
}
#endif

