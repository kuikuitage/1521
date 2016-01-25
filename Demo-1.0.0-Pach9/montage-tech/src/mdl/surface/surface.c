/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mem_manager.h"

#include "lib_memf.h"
#include "lib_rect.h"
#include "class_factory.h"

#include "osd.h"
#include "gpe.h"

#include "surface.h"
#include "surface_priv.h"

void surface_release(u32 handle)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_release != NULL)
  {
    p_this->surface_release();
  }

  return;
}


void surface_enable(u32 handle, void *p_surf, BOOL is_enable)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_enable != NULL)
  {
    p_this->surface_enable(p_surf, is_enable);
  }

  return;
}


RET_CODE surface_create(u32 handle, void **pp_surf, surface_desc_t *p_desc)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_create != NULL)
  {
    return p_this->surface_create(pp_surf, p_desc);
  }

  return SUCCESS;
}


RET_CODE surface_delete(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_delete != NULL)
  {
    return p_this->surface_delete(p_surf);
  }
  
  return SUCCESS;
}


RET_CODE surface_set_display(u32 handle, void *p_surf, BOOL is_display)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_display != NULL)
  {
    return p_this->surface_set_display(p_surf, is_display);
  }

  return SUCCESS;
}


RET_CODE surface_set_alpha(u32 handle, void *p_surf, u8 alpha)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_alpha != NULL)
  {
    return p_this->surface_set_alpha(p_surf, alpha);
  }

  return SUCCESS;
}


RET_CODE surface_set_trans(u32 handle, void *p_surf, u32 index, u8 alpha)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_trans != NULL)
  {
    return p_this->surface_set_trans(p_surf, index, alpha);
  }

  return SUCCESS;
}


RET_CODE surface_init_palette(u32 handle, void *p_surf, palette_t *p_pal)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_init_palette != NULL)
  {
    return p_this->surface_init_palette(p_surf, p_pal);
  }

  return SUCCESS;
}


RET_CODE surface_set_palette(u32 handle, void *p_surf, u16 start, u16 len, color_t *p_entry)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_palette != NULL)
  {
    return p_this->surface_set_palette(p_surf, start, len, p_entry);
  }

  return SUCCESS;
}


RET_CODE surface_get_palette(u32 handle, void *p_surf, u16 start, u16 len, color_t *p_entry)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_palette != NULL)
  {
    return p_this->surface_get_palette(p_surf, start, len, p_entry);
  }

  return SUCCESS;
}


RET_CODE surface_set_colorkey(u32 handle, void *p_surf, u32 ckey)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_colorkey != NULL)
  {
    return p_this->surface_set_colorkey(p_surf, ckey);
  }
  
  return SUCCESS;
}

RET_CODE surface_set_trans_clr(u32 handle, void *p_surf, u32 trans_clr)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_trans_clr != NULL)
  {
    return p_this->surface_set_trans_clr(p_surf, trans_clr);
  }
  
  return SUCCESS;
}

RET_CODE surface_get_colorkey(u32 handle, void *p_surf, u32 *p_ckey)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_colorkey != NULL)
  {
    return p_this->surface_get_colorkey(p_surf, p_ckey);
  }

  return SUCCESS;
}


RET_CODE surface_set_cliprect(u32 handle, void *p_surf, rect_t *p_rc)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_cliprect != NULL)
  {
    return p_this->surface_set_cliprect(p_surf, p_rc);
  }

  return SUCCESS;
}


RET_CODE surface_get_cliprect(u32 handle, void *p_surf, rect_t *p_rc)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_cliprect != NULL)
  {
    return p_this->surface_get_cliprect(p_surf, p_rc);
  }

  return SUCCESS;
}


RET_CODE surface_set_srcrect(u32 handle, void *p_surf, rect_t *p_rc)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_set_srcrect != NULL)
  {
    return p_this->surface_set_srcrect(p_surf, p_rc);
  }  

  return SUCCESS;
}


RET_CODE surface_get_srcrect(u32 handle, void *p_surf, rect_t *p_rc)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_srcrect != NULL)
  {
    return p_this->surface_get_srcrect(p_surf, p_rc);
  }
  
  return SUCCESS;
}


RET_CODE surface_fill_rect(u32 handle, void *p_surf, rect_t *p_rc, u32 value)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_fill_rect != NULL)
  {
    return p_this->surface_fill_rect(p_surf, p_rc, value);
  }

  return SUCCESS;
}

RET_CODE surface_fill_round_rect(u32 handle, void *p_surf, rect_t *p_rc, u32 value)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_fill_round_rect != NULL)
  {
    return p_this->surface_fill_round_rect(p_surf, p_rc, value);
  }

  return SUCCESS;
}


RET_CODE surface_fill_bmp(u32 handle, void *p_surf, rect_t *p_rc, bitmap_t *p_bmp)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_fill_bmp != NULL)
  {
    return p_this->surface_fill_bmp(p_surf, p_rc, p_bmp);
  }

  return SUCCESS;
}


RET_CODE surface_get_bits(u32 handle, void *p_surf, 
  rect_t *p_rc, void *p_buf, u32 pitch)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_bits != NULL)
  {
    return p_this->surface_get_bits(p_surf, p_rc, p_buf, pitch);
  }

  return SUCCESS;
}


RET_CODE surface_bitblt(u32 handle, void *p_src_surf, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_surf, u16 dx, u16 dy, surface_rop_type_t rop)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_bitblt != NULL)
  {
    return p_this->surface_bitblt(p_src_surf, sx, sy, sw, sh, p_dst_surf, dx, dy, rop);
  }
  
  return ERR_FAILURE;
}

u8 surface_get_attr(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_attr != NULL)
  {
    return p_this->surface_get_attr(p_surf);
  }
  
  return 0;
}

s32 surface_get_handle(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_handle != NULL)
  {
    return p_this->surface_get_handle(p_surf);
  }
  
  return 0;  
}

u8 surface_get_bpp(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_bpp != NULL)
  {
    return p_this->surface_get_bpp(p_surf);
  }
  
  return 0;  
}

u8 surface_get_format(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_format != NULL)
  {
    return p_this->surface_get_format(p_surf);
  }
  
  return 0;  
}

palette_t *surface_get_palette_addr(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_get_palette_addr != NULL)
  {
    return (palette_t *)p_this->surface_get_palette_addr(p_surf);
  }
  
  return NULL;  
}

void surface_start_batch(u32 handle, void *p_surf)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_start_batch != NULL)
  {
    p_this->surface_start_batch(p_surf);
  }

  return;
}

void surface_end_batch(u32 handle, void *p_surf, BOOL is_sync, rect_t *p_rect)
{
  surface_proc_t *p_this = (surface_proc_t *)handle;
  
  MT_ASSERT(p_this != NULL);

  if(p_this->surface_end_batch != NULL)
  {
    p_this->surface_end_batch(p_surf, is_sync, p_rect);
  }

  return;
}

