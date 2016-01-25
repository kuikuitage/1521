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

extern int dc_enter_drawing(dc_t *p_dc);
extern void dc_leave_drawing(dc_t *p_dc);
extern BOOL dc_generate_ecrgn(dc_t *p_dc, BOOL is_is_force);

/*!
 Create a compitable bitmap control with the specified reference DC.
 */
hbitmap_t gdi_create_compatible_bitmap(hdc_t hdc, bitmap_t *p_bmp)
{
  return (hbitmap_t)(p_bmp);
}


/*!
 Delect a compitable bitmap control.
 */
void gdi_delete_compatible_bitmap(hbitmap_t bmp)
{
}


static void tile_bitblt(void *p_surf, rect_t *p_orc,
  bitmap_t *p_bmp, s32 flags)
{
  rect_t rc = *p_orc;
  s32 h = 0, v = 0, h_cnt = 0, v_cnt = 0;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  if((flags & FILL_DIRECTION_HORI) && (flags & FILL_DIRECTION_VERT))
  {
    h_cnt = RECTW(rc) / p_bmp->width + (RECTW(rc) % p_bmp->width ? 1 : 0);
    v_cnt = RECTH(rc) / p_bmp->height + (RECTH(rc) % p_bmp->height ? 1 : 0);
  }
  else if(flags & FILL_DIRECTION_HORI)
  {
    h_cnt = RECTW(rc) / p_bmp->width + (RECTW(rc) % p_bmp->width ? 1 : 0);
    v_cnt = 1;
  }
  else if(flags & FILL_DIRECTION_VERT)
  {
    h_cnt = 1;
    v_cnt = RECTH(rc) / p_bmp->height + (RECTH(rc) % p_bmp->height ? 1 : 0);
  }
  else
  {
    h_cnt = v_cnt = 1;
  }

  rc.right = rc.left + p_bmp->width;
  rc.bottom = rc.top + p_bmp->height;

  for(h = 0; h < h_cnt; h++)
  {
    for(v = 0; v < v_cnt; v++)
    {
      surface_fill_bmp(handle, p_surf, &rc, p_bmp);

      if(v + 1 != v_cnt)
      {
        offset_rect(&rc, 0, p_bmp->height);
//        rc.top += p_bmp->height;
      }
    }
    // reset top
    rc.top = p_orc->top;
    rc.bottom = rc.top + p_bmp->height;

    if(h + 1 != h_cnt)
    {
      offset_rect(&rc, p_bmp->width, 0);
//      rc.left += p_bmp->width;
    }
  }
}


/*!
 Fills a box with a BITMAP control.
 */
BOOL gdi_fill_bitmap(hdc_t hdc, rect_t *p_rc, const hbitmap_t bmp, u32 flags)
{
  bitmap_t *p_bmp = (bitmap_t *)(bmp);
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  gdi_main_t *p_gdi_info = NULL;
  cliprc_t *p_crc = NULL;
  rect_t eff_rc, orc = *p_rc;

  dc_t *p_dc = dc_hdc2pdc(hdc);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  if(p_gdi_info->p_screen_surface == NULL)
  {
    return FALSE;
  }

  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return FALSE;
  }

  // chk, if not fit to dc
  if(p_bmp->bpp != surface_get_bpp(handle, p_dc->p_curn_surface))
  {
    return FALSE;
  }

  // chk, lock rgn
  if(dc_is_general_dc(p_dc))
  {
    LOCK_GCRINFO(p_dc);
    if(!dc_generate_ecrgn(p_dc, FALSE))
    {
      UNLOCK_GCRINFO(p_dc);
      return FALSE;
    }
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  //ENTER_DRAWING(p_dc);
  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return FALSE;
  }  

    /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  p_crc = p_dc->ecrgn.p_head;
  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      surface_set_cliprect(handle, p_dc->p_curn_surface, &eff_rc);

      if(flags & FILL_FLAG_TILE)
      {
        tile_bitblt(p_dc->p_curn_surface, &orc, p_bmp, flags);
      }
      else if(flags & FILL_FLAG_STRETCH)
      {
      }
      else if(flags & FILL_FLAG_CENTER)
      {
        orc.left += (RECTW(orc) - p_bmp->width) / 2;
        orc.right = orc.left + p_bmp->width;

        orc.top += (RECTH(orc) - p_bmp->height) / 2;
        orc.bottom = orc.top + p_bmp->height;

        surface_fill_bmp(handle, p_dc->p_curn_surface, &orc, p_bmp);
      }
      else
      {
        surface_fill_bmp(handle, p_dc->p_curn_surface, &orc, p_bmp);
      }
    }

    p_crc = p_crc->p_next;
  }
  /* disable clipper */
  surface_set_cliprect(handle, p_dc->p_curn_surface, NULL);

  //LEAVE_DRAWING(p_dc);
  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);

  return TRUE;
}


/*!
 Get a box with a rectangle
 */
BOOL gdi_get_bits(hdc_t hdc, rect_t *p_rc, void *p_buf, u32 pitch)
{
  rect_t orc = *p_rc;
  dc_t *p_dc = dc_hdc2pdc(hdc);
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  if(p_gdi_info->p_screen_surface == NULL)
  {
    return FALSE;
  }
  
  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return FALSE;
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  surface_get_bits(handle, p_dc->p_curn_surface, &orc, p_buf, pitch);

  return TRUE;
}


/*!
 Performs a bit-block transfer from a device context into 
 another device context.
 */
void gdi_bitblt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc, u16 dx,
                u16 dy, u32 rop)
{
  dc_t *p_sdc = NULL;
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc, dst_orc, eff_rc;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  if(p_gdi_info->p_screen_surface == NULL)
  {
    return;
  }

  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + sw), (s16)(dy + sh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  //ENTER_DRAWING(p_ddc);
  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_surface == p_sdc->p_curn_surface && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      surface_set_cliprect(handle, p_ddc->p_curn_surface, &eff_rc);
      surface_bitblt(handle, p_sdc->p_curn_surface, 
        sx, sy, sw, sh, p_ddc->p_curn_surface, dx, dy, rop);
    }

    if(p_ddc->p_curn_surface == p_sdc->p_curn_surface && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  /* disable clipper */
  surface_set_cliprect(handle, p_ddc->p_curn_surface, NULL);

  //LEAVE_DRAWING(p_ddc);
  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}


/*!
 Copies a bit-block from a a device context into another device context, 
   streches it if necessary.
 */
void gdi_stretch_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, 
  hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh, u32 rop)
{
#if 0
  dc_t *psdc, *pddc;
  rect_t src_orc, dst_orc;
  BITMAP bmp;
  struct _SCALER_INFO_STRETCHBLT info;

  psdc = dc_hdc2pdc(hsdc);
  pddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(pddc))
  {
    LOCK_GCRINFO(pddc);
    if(!dc_generate_ecrgn(pddc, FALSE))
    {
      UNLOCK_GCRINFO(pddc);
      return;
    }
  }


  if(GAL_BitsPerPixel(psdc->surface) != GAL_BitsPerPixel(pddc->surface))
  {
    goto error_ret;
  }

  if(sx < 0)
  {
    sx = 0;
  }
  if(sy < 0)
  {
    sy = 0;
  }
  if(dx < 0)
  {
    dx = 0;
  }
  if(dy < 0)
  {
    dy = 0;
  }

  if(sx >= RECTW(psdc->DevRC))
  {
    goto error_ret;
  }
  if(sy >= RECTH(psdc->DevRC))
  {
    goto error_ret;
  }
  if(dx >= RECTW(pddc->DevRC))
  {
    goto error_ret;
  }
  if(dy >= RECTH(pddc->DevRC))
  {
    goto error_ret;
  }

  if(sw <= 0 || sw > RECTW(psdc->DevRC) - sx)
  {
    sw = RECTW(psdc->DevRC) - sx;
  }
  if(sh <= 0 || sh > RECTH(psdc->DevRC) - sy)
  {
    sh = RECTH(psdc->DevRC) - sy;
  }
  if(dw <= 0 || dw > RECTW(pddc->DevRC) - dx)
  {
    dw = RECTW(pddc->DevRC) - dx;
  }
  if(dh <= 0 || dh > RECTH(pddc->DevRC) - dy)
  {
    dh = RECTH(pddc->DevRC) - dy;
  }


  /* The coordinates should be in device space. */
#if 0
  // Transfer logical to device to screen here.
  sw += sx; sh += sy;
  coor_DP2CP(psdc, &sx, &sy);
  coor_DP2CP(psdc, &sw, &sh);
  set_rect(&src_orc, sx, sy, sw, sh);
  NormalizeRect(&src_orc);
  sw = RECTW(src_orc); sh = RECTH(src_orc);

  dw += dx; dh += dy;
  coor_DP2CP(pddc, &dx, &dy);
  coor_DP2CP(pddc, &dw, &dh);
  set_rect(&dst_orc, dx, dy, dw, dh);
  NormalizeRect(&dst_orc);
  dw = RECTW(dst_orc); dh = RECTH(dst_orc);
#else
  coor_DP2CP(psdc, &sx, &sy);
  set_rect(&src_orc, sx, sy, sx + sw, sy + sh);
/*
   if(!IsCovered (&src_orc, &psdc->DevRC))
    goto error_ret;
 */

  coor_DP2CP(pddc, &dx, &dy);
  set_rect(&dst_orc, dx, dy, dx + dw, dy + dh);
#endif

  info.p_dc = pddc;
  info.dst_x = dx; info.dst_y = dy;
  info.dst_w = dw; info.dst_h = dh;
  info.line_buff = malloc(GAL_BytesPerPixel(pddc->surface) * dw);

  if(info.line_buff == NULL)
  {
    goto error_ret;
  }

  memset(&bmp, 0, sizeof(bmp));
  bmp.bmType = BMP_TYPE_NORMAL;
  bmp.bmBitsPerPixel = GAL_BitsPerPixel(psdc->surface);
  bmp.bmBytesPerPixel = GAL_BytesPerPixel(psdc->surface);
  bmp.bmWidth = sw;
  bmp.bmHeight = sh;
  bmp.bmPitch = psdc->surface->pitch;
  bmp.bmBits = (unsigned char *)psdc->surface->pixels + sy *
               psdc->surface->pitch
               + sx * GAL_BytesPerPixel(psdc->surface);

  if(!dc_is_mem_dc(psdc) && !dc_is_mem_dc(pddc))
  {
    GetBoundRect(&pddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    pddc->rc_output = dst_orc;
  }

  ENTER_DRAWING(pddc);
  if(!dc_is_mem_dc(psdc) && dc_is_mem_dc(pddc))
  {
    ShowCursorForGDI(FALSE, &src_orc);
  }

  if(pddc->surface == psdc->surface && DoesIntersect(&src_orc, &dst_orc))
  {
    info.bottom2top = TRUE;
    BitmapDDAScaler2(&info, &bmp, dw, dh,
                     _get_line_buff_stretchblt, _line_scaled_stretchblt);
  }
  else
  {
    info.bottom2top = FALSE;
    BitmapDDAScaler(&info, &bmp, dw, dh,
                    _get_line_buff_stretchblt, _line_scaled_stretchblt);
  }

  if(!dc_is_mem_dc(psdc) && dc_is_mem_dc(pddc))
  {
    ShowCursorForGDI(TRUE, &src_orc);
  }
  LEAVE_DRAWING(pddc);

  free(info.line_buff);

 error_ret:
  UNLOCK_GCRINFO(pddc);
#endif
}


