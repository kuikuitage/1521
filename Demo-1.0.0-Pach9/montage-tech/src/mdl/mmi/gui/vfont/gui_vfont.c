/******************************************************************************/
/******************************************************************************/
/*!
 \file gui_vfont.c
   this file  implement the functions defined in  gui_vfont.h, also it implement some internal used
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

#include "gui_resource.h"
#include "gui_vfont.h"
#include "class_factory.h"

void gui_vfont_get_char_attr(u16 char_code, void *p_font, u32 size,
  rsc_fstyle_t *p_fstyle, u16 *p_width, u16 *p_height)
{
  gui_vfont_t *p_vfs_info = NULL;

  p_vfs_info = (gui_vfont_t *)class_get_handle_by_id(VFONT_CLASS_ID);
  if(p_vfs_info == NULL)
  {
    return;
  }

  if(p_vfs_info->vf_get_attr != NULL)
  {
    p_vfs_info->vf_get_attr(p_vfs_info->p_data,
      char_code, p_font, size, p_fstyle, p_width, p_height);
  }

  return;
}

BOOL gui_vfont_check_char(u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle, u16 font_lib_id)
{
  gui_vfont_t *p_vfs_info = NULL;

  p_vfs_info = (gui_vfont_t *)class_get_handle_by_id(VFONT_CLASS_ID);
  if(p_vfs_info == NULL)
  {
    return FALSE;
  }

  if(p_vfs_info->vf_check_char != NULL)
  {
    return p_vfs_info->vf_check_char(p_vfs_info->p_data, char_code, p_font, size, p_fstyle, font_lib_id);
  }

  return FALSE;
}

u32 gui_vfont_get_char(u16 char_code, void *p_font, u32 size,
  rsc_fstyle_t *p_fstyle, void *p_cache, rsc_char_info_t *p_info)
{
  gui_vfont_t *p_vfs_info = NULL;

  p_vfs_info = (gui_vfont_t *)class_get_handle_by_id(VFONT_CLASS_ID);
  if(p_vfs_info == NULL)
  {
    return 0;
  }

  if(p_vfs_info->vf_get_char != NULL)
  {
    return p_vfs_info->vf_get_char(p_vfs_info->p_data,
      char_code, p_font, size, p_fstyle, p_cache, p_info);
  }

  return 0;
}

BOOL gui_vfont_hit_cache(u16 char_code, rsc_fstyle_t *p_style, rsc_char_info_t *p_info)
{
  gui_vfont_t *p_vfs_info = NULL;

  p_vfs_info = (gui_vfont_t *)class_get_handle_by_id(VFONT_CLASS_ID);
  if(p_vfs_info == NULL)
  {
    return FALSE;
  }

  if(p_vfs_info->vf_hit_cache != NULL)
  {
    return p_vfs_info->vf_hit_cache(p_vfs_info->p_data, char_code, p_style, p_info);
  }

  return FALSE;
}

void *gui_vfont_insert_cache(u16 char_code, rsc_fstyle_t *p_style)
{
  gui_vfont_t *p_vfs_info = NULL;

  p_vfs_info = (gui_vfont_t *)class_get_handle_by_id(VFONT_CLASS_ID);
  if(p_vfs_info == NULL)
  {
    return NULL;
  }

  if(p_vfs_info->vf_insert_cache != NULL)
  {
    return p_vfs_info->vf_insert_cache(p_vfs_info->p_data, char_code, p_style);
  }

  return NULL;
}

