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

//-extern void *p_screen_surface;
/*!
  gets palette entries.
  */
BOOL gdi_get_palette(u16 start, u16 len, color_t *p_colors)
{
  void *p_surf = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  u8 attr = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_surf = p_gdi_info->p_screen_surface;

  attr = surface_get_attr(handle, p_surf);
  
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    return (BOOL)(surface_get_palette(handle, p_surf, start, len, p_colors) == SUCCESS);
  }

  return FALSE;
}


/*!
  sets palette entries.
  */
BOOL gdi_set_palette(u16 start, u16 len, color_t *p_colors)
{
  void *p_surf = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  u8 attr = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_surf = p_gdi_info->p_screen_surface;

  attr = surface_get_attr(handle, p_surf);
  
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    return (BOOL)(surface_set_palette(handle, p_surf, start, len, p_colors) == SUCCESS);
  }

  return FALSE;
}


BOOL gdi_set_colorkey(u32 ckey)
{
  void *p_surf = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_surf = p_gdi_info->p_screen_surface;
  
  return (BOOL)(surface_set_colorkey(handle, p_surf, ckey) == SUCCESS);
}


BOOL gdi_get_colorkey(u32 *p_ckey)
{
  void *p_surf = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_surf = p_gdi_info->p_screen_surface;
  
  return (BOOL)(surface_get_colorkey(handle, p_surf, p_ckey) == SUCCESS);
}


