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

void gdi_rsurf_init(u32 size)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->p_roll_heap_addr =
    mmi_create_memp(&p_gdi_info->roll_heap, size);
  
  MT_ASSERT(p_gdi_info->p_roll_heap_addr != NULL);
}

void gdi_rsurf_release(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->p_roll_heap_addr != NULL)
  {
    mmi_destroy_memp(&p_gdi_info->roll_heap, p_gdi_info->p_roll_heap_addr);
  }
}

hdc_t gdi_get_roll_dc(rect_t *p_src, void *p_rsurf)
{
  int i = 0;
  dc_t *p_cdc = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    p_cdc = &p_gdi_info->p_dc_slot[i];

    if(!p_cdc->is_used)
    {
      p_cdc->is_used = TRUE;

      p_cdc->type = TYPE_GENERAL | TYPE_MEMORY;

      p_cdc->p_curn_surface = p_rsurf;
      p_cdc->p_back_surface = NULL;

      break;
    }
  }

  if(i >= p_gdi_info->dc_slot_cnt)
  {
    return HDC_INVALID;
  }

  //dc_init(&p_gdi_info->p_dc_slot[i], p_src, NULL);
  gdi_set_cliprgn(&p_gdi_info->p_dc_slot[i].ecrgn, p_src);

  p_gdi_info->p_dc_slot[i].dev_rc = *p_src;

  return (hdc_t)(&p_gdi_info->p_dc_slot[i]);
}


void gdi_release_roll_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

  p_dc->is_used = FALSE;
  empty_rect(&p_dc->dev_parent);  
}

handle_t gdi_create_rsurf(u16 width, u16 height, u32 *p_addr)
{
  void *p_vsurf = NULL, *p_ref_surf = NULL;
  surface_desc_t desc;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  u8 bpp = 0;
  u32 pitch = 0, size = 0;
  s32 ret = SUCCESS;
  gdi_main_t *p_gdi_info = NULL;
  void *p_buf = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((width == 0) || (height == 0))
  {
    return 0;
  }
  
  p_ref_surf = p_gdi_info->p_screen_surface;

  bpp = surface_get_bpp(handle, p_ref_surf);

  pitch = (((bpp >> 3) * width + 7) >> 3) << 3; 
  //pitch = (bpp * ((width +1) & (~1))) >> 3;
  size = pitch * height;

  p_buf = lib_memp_alloc(&p_gdi_info->roll_heap, size + 4);
  MT_ASSERT(p_buf != NULL);
  memset(p_buf, 0, size + 4);

  //set the surface descriptor
  desc.format = surface_get_format(handle, p_ref_surf);;
  desc.flag = SURFACE_DESC_CAPS | SURFACE_DESC_FORMAT | SURFACE_DESC_PITCH |
              SURFACE_DESC_WIDTH | SURFACE_DESC_HEIGHT;
  desc.width = width;
  desc.height = height;
  desc.caps = SURFACE_CAPS_MEM_ASSIGN;
  desc.p_vmem[0] = (u8 *)p_buf;
  desc.p_vmem[1] = (u8 *)(((u32)p_buf + size / 2 + 3) / 4 * 4);
  desc.p_pal = surface_get_palette_addr(handle, p_ref_surf);
  desc.pitch = pitch;

  ret = surface_create(handle, &p_vsurf, &desc);

  if(ret != SUCCESS)
  {
    return 0;
  }

 
  //disable the color key
  surface_set_colorkey(handle, p_vsurf, SURFACE_INVALID_COLORKEY);

  *p_addr = (u32)p_buf;

  return p_vsurf;
}

void gdi_delete_rsurf(handle_t handle, u32 addr)
{
  gdi_main_t *p_gdi_info = NULL;  
  u32 surface_handle = 0;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  surface_handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(surface_handle != 0);

  surface_delete(surface_handle, (void *)handle);

  lib_memp_free(&p_gdi_info->roll_heap, (void *)addr);
}
