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

/*
 * match an RGBA value to a particular palette index
 */
u8 gdi_find_color(palette_t *p_pal, u8 r, u8 g, u8 b, u8 a)
{
  /* do colorspace distance matching */
  u32 smallest = 0;
  u32 distance = 0;
  s32 rd = 0, gd = 0, bd = 0, ad = 0;
  u16 i = 0, index = 0;

  smallest = ~0;
  for(i = 0; i < p_pal->cnt; i++)
  {
    rd = p_pal->p_entry[i].r - r;
    gd = p_pal->p_entry[i].g - g;
    bd = p_pal->p_entry[i].b - b;
    ad = p_pal->p_entry[i].a - a;
    // reduce the weight of alpha
    distance = ((rd * rd) + (gd * gd) + (bd * bd) + (ad * ad)) >> 4; 
    if(distance < smallest)
    {
      index = i;
      if(distance == 0)   /* perfect match! */
      {
        break;
      }
      smallest = distance;
    }
  }
  return (u8)(index);
}


/* find the opaque pixel value corresponding to an RGBA triple */
u32 gdi_map_RGBA(hdc_t hdc, u8 r, u8 g, u8 b, u8 a)
{
  u32 pix_val = 0;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  void *p_surf = dc_hdc2pdc(hdc)->p_curn_surface;

  switch(surface_get_format(handle, p_surf))
  {
    case OSD_COLORFORMAT_RGB4BIT:
    case OSD_COLORFORMAT_RGB8BIT:
      pix_val = gdi_find_color(surface_get_palette_addr(handle, p_surf), r, g, b, a);
      break;
      /*
         case PIX_16ARGB1555:
         pix_val = ((a&0x01)<<15) | ((r&0x1F) <<10) | ((g&0x1F) <<5) | (b&0x1F);
         break;

         case PIX_16ARGB0565:
         pix_val = ((r&0x1F) <<11) | ((g&0x3F) <<5) | (b&0x1F);
         break;

         case PIX_16ARGB4444:
         pix_val = ((a&0x0F)<<12) | ((r&0x0F) <<8) | ((g&0x0F) <<4) | (b&0x0F);
         break;

         case PIX_24ARGB0888:
         pix_val = ((r&0xFF) <<16) | ((g&0xFF) <<8) | (b&0xFF);
         break;

         case PIX_32ARGB8888:
         pix_val = ((a&0xFF)<<24) | ((r&0xFF) <<16) | ((g&0xFF) <<8) | (b&0xFF);
         break;
       */
    default:
      pix_val = 0;
  }

  return pix_val;
}


void gdi_get_RGBA(hdc_t hdc, u32 pix_val, u8 *p_r, u8 *p_g, u8 *p_b, u8 *p_a)
{
  void *p_surf = dc_hdc2pdc(hdc)->p_curn_surface;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  palette_t *p_pal = surface_get_palette_addr(handle, p_surf);
  
  switch(surface_get_format(handle, p_surf))
  {
    case OSD_COLORFORMAT_RGB4BIT:
    case OSD_COLORFORMAT_RGB8BIT:
      if(pix_val < p_pal->cnt)
      {
        *p_a = p_pal->p_entry[pix_val].a;
        *p_r = p_pal->p_entry[pix_val].r;
        *p_g = p_pal->p_entry[pix_val].g;
        *p_b = p_pal->p_entry[pix_val].b;
      }
      break;
      /*
         case PIX_16ARGB1555:
         pix_val = pix_val&0xFFFF;

       *a = (u8)(pix_val>>15)&0x01;
       *r = (u8)(pix_val>>10)&0x1F;
       *g = (u8)(pix_val>>5)&0x1F;
       *b = (u8)(pix_val)&0x1F;
         break;

         case PIX_16ARGB0565:
         pix_val = pix_val&0xFFFF;

       *a = 0xFF;
       *r = (u8)(pix_val>>11)&0x1F;
       *g = (u8)(pix_val>>5)&0x3F;
       *b = (u8)(pix_val)&0x1F;
         break;

         case PIX_16ARGB4444:
         pix_val = pix_val&0xFFFF;

       *a = (u8)(pix_val>>12)&0x0F;
       *r = (u8)(pix_val>>8)&0x0F;
       *g = (u8)(pix_val>>4)&0x0F;
       *b = (u8)(pix_val)&0x0F;
         break;

         case PIX_24ARGB0888:
         pix_val = pix_val&0xFFFFFF;

       *a = 0xFF;
       *r = (u8)(pix_val>>16)&0xFF;
       *g = (u8)(pix_val>>8)&0xFF;
       *b = (u8)(pix_val)&0xFF;
         break;

         case PIX_32ARGB8888:
       *a = (u8)(pix_val>>24)&0xFF;
       *r = (u8)(pix_val>>16)&0xFF;
       *g = (u8)(pix_val>>8)&0xFF;
       *b = (u8)(pix_val)&0xFF;
         break;
       */
    default:
      *p_a = *p_r = *p_g = *p_b = 0xFF;
  }
}


u8 gdi_get_invert_color(hdc_t hdc, u32 color)
{
  u8 r = 0, g = 0, b = 0, a = 0;
  dc_t *p_dc = (dc_t *)hdc;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  palette_t *p_pal = surface_get_palette_addr(handle, p_dc->p_curn_surface);
  
  gdi_get_RGBA(hdc, color, &r, &g, &b, &a);

  r = 0xFF - r;
  g = 0xFF - g;
  b = 0xFF - b;

  return gdi_find_color(p_pal, r, g, b, a);
}

