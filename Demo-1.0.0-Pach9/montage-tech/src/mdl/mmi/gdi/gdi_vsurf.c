/******************************************************************************/
/******************************************************************************/
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

#include "gdi_private.h"
#include "class_factory.h"

/*!
  virtual surface operations
  */  
//-static void * p_vsurf_buf[2];
//-static u32 buf_size;

s32 gdi_init_vsurf(u32 addr, u32 size)
{
//  u8 i = 0;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);
  
  if(size == 0 || addr == 0)
  {
    gdi_release_vsurf();
    return -1;
  }
#if 0
  for(i = 0; i < 2; i++)
  {
    if((p_gdi_info->p_vsurf_buf[i] = mmi_alloc_buf(size / 2)) == NULL)
    {
      //		DEBUGPRINTF ("can NOT alloc enough memory, ERROR!\n");
      gdi_release_vsurf();
      return -1;
    }
  }
#else
  p_gdi_info->p_vsurf_buf[0] = (void *)addr;
  p_gdi_info->p_vsurf_buf[1] = (void *)(addr + (size / 2));
#endif

  p_gdi_info->vsurf_buf_size = size;
  return 0;
}


void gdi_release_vsurf(void)
{
//  u8 i = 0;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

#if 0  
  for(i = 0; i < 2; i++)
  {
    if(p_gdi_info->p_vsurf_buf[i] != NULL)
    {
      mmi_free_buf(p_gdi_info->p_vsurf_buf[i]);
      p_gdi_info->p_vsurf_buf[i] = NULL;
    }
  }
#else
  p_gdi_info->p_vsurf_buf[0] = NULL;
  p_gdi_info->p_vsurf_buf[1] = NULL;
#endif
  p_gdi_info->vsurf_buf_size = 0;
}


s32 gdi_create_vsurf(hdc_t hdc, s16 width, s16 height)
{
  void *p_vsurf = NULL, *p_ref_surf = NULL;
  surface_desc_t desc;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  u8 bpp = 0;
  
  dc_t *p_dc = dc_hdc2pdc(hdc);

  u32 pitch = 0, size = 0;
  s32 ret = SUCCESS;

  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((width == 0) || (height == 0))
  {
    return -4;
  }
  
  if(p_gdi_info->p_vsurf_buf[0] == NULL || p_gdi_info->p_vsurf_buf[1] == NULL)
  {
    return -1;
  }

  p_ref_surf = p_dc->p_curn_surface;

  bpp = surface_get_bpp(handle, p_ref_surf);

  pitch = (bpp * width) >> 3;
//	pitch_t = (s16)calc_pitch (calc_line (width, ref_surf->bpp));
  size = pitch * height;

  if (size > p_gdi_info->vsurf_buf_size)
  {    
    return -2;
  }

  // set the surface descriptor
  desc.format = surface_get_format(handle, p_ref_surf);;

  desc.flag = SURFACE_DESC_CAPS | SURFACE_DESC_FORMAT | SURFACE_DESC_PITCH |
              SURFACE_DESC_WIDTH | SURFACE_DESC_HEIGHT;
  desc.width = width;
  desc.height = height;
  desc.caps = SURFACE_CAPS_MEM_ASSIGN;
  desc.p_vmem[0] = p_gdi_info->p_vsurf_buf[0];
  desc.p_vmem[1] = p_gdi_info->p_vsurf_buf[1];
  desc.p_pal = surface_get_palette_addr(handle, p_ref_surf);
  desc.pitch = pitch;


//	LOCK (lock_gdi);
  ret = surface_create(handle, &p_vsurf, &desc);
//	UNLOCK (lock_gdi);

  if(ret != SUCCESS)
  {
    return -3;
  }

  /* set surface attributes
     if(ref_surf->attr&SURFACE_ATTR_EN_PALETTE)
     {
     palette_t *pal = ref_surf->pal;

     if(pal->cnt> 0)
      surface_set_palette (handle, vsurf, 0, pal->cnt, pal->entry);
     }
   */
   
  // disable the color key
  surface_set_colorkey(handle, p_vsurf, SURFACE_INVALID_COLORKEY);

  // erase with color key
//	surface_fill_rect (handle, vsurf, &vsurf->rc_src, ckey);

  p_dc->type |= TYPE_VIRTUAL;

  // switch surface
  p_dc->p_curn_surface = p_vsurf;
  p_dc->p_back_surface = p_ref_surf;

  /*!
    unlock in delete vsurf
    */
  return 0;
}


s32 gdi_delete_vsurf(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
    u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
    
  if(p_dc->type & TYPE_VIRTUAL)
  {
    surface_delete(handle, p_dc->p_curn_surface);

    p_dc->p_curn_surface = p_dc->p_back_surface;
    p_dc->type &= ~TYPE_VIRTUAL;

    // unlock canvas

    return 0;
  }

  return -1;
}


/*!
  end of virtual surface operations
  */
