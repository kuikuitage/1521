/******************************************************************************/
/******************************************************************************/
#ifndef __SURFACE_PRIV_H__
#define __SURFACE_PRIV_H__

/*!
  Surface handle t
  */
typedef struct
{
  /*!
     The memf object to alloc the structure of surface.
    */
  void *p_data;
  /*!
    surface release.
    */
  void (*surface_release)(void);
  /*!
    surface enable.
    */
  void (*surface_enable)(void *p_surf, BOOL is_enable);
  /*!
    surface create.
    */
  RET_CODE (*surface_create)(void **pp_surf, surface_desc_t *p_desc);
  /*!
    surface delete.
    */
  RET_CODE (*surface_delete)(void *p_surf);
  /*!
    surface set display
    */
  RET_CODE (*surface_set_display)(void *p_surf, BOOL is_display);
  /*!
     Sets the global alpha blend of a surface.
    */
  RET_CODE (*surface_set_alpha)(void *p_surf, u8 alpha);
  /*!
     Sets the transparence of a surface.
    */
  RET_CODE (*surface_set_trans)(void *p_surf, u32 index, u8 alpha);
  /*!
     Initialize the palette of a surface.
    */
  RET_CODE (*surface_init_palette)(void *p_surf, palette_t *p_pal);
  /*!
     Sets the palette of a surface.
    */
  RET_CODE (*surface_set_palette)(void *p_surf, u16 start, u16 len, color_t *p_entry);
  /*!
     Gets the palette of a surface.
    */
  RET_CODE (*surface_get_palette)(void *p_surf, u16 start, u16 len, color_t *p_entry);
  /*!
     Sets the colorkey of a surface.
    */
  RET_CODE (*surface_set_colorkey)(void *p_surf, u32 ckey);
  /*!
     Sets the transparent color of a surface.
    */
  RET_CODE (*surface_set_trans_clr)(void *p_surf, u32 trans_clr);  
  /*!
     Gets the transparence of a surface.
    */
  RET_CODE (*surface_get_colorkey)(void *p_surf, u32 *p_ckey);
  /*!
     Sets the clipping rectangle of a surface.
    */
  RET_CODE (*surface_set_cliprect)(void *p_surf, rect_t *p_rc);
  /*!
     Gets the clipping rectangle of a surface.
    */
  RET_CODE (*surface_get_cliprect)(void *p_surf, rect_t *p_rc);
  /*!
     Sets the display rectangle of a surface.
    */
  RET_CODE (*surface_set_srcrect)(void *p_surf, rect_t *p_rc);
  /*!
     Gets the display rectangle of a surface.
    */
  RET_CODE (*surface_get_srcrect)(void *p_surf, rect_t *p_rc);
  /*!
     Fill a rectangle with the specified color.
    */
  RET_CODE (*surface_fill_rect)(void *p_surf, rect_t *p_rc, u32 value);
  /*!
     Fill a round rectangle with the specified color.
    */
  RET_CODE (*surface_fill_round_rect)(void *p_surf, rect_t *p_rc, u32 value);
  /*!
     Fill a rectangle with the specified bitmap.
    */
  RET_CODE (*surface_fill_bmp)(void *p_surf, rect_t *p_rc, bitmap_t *p_bmp);
  /*!
     Gets the image from a surface with the specified rectangle.
    */
  RET_CODE (*surface_get_bits)(void *p_surf, rect_t *p_rc, void *p_buf, u32 pitch);
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  RET_CODE (*surface_bitblt)(void *p_src_surf, u16 sx, u16 sy, u16 sw,
    u16 sh, void *p_dst_surf, u16 dx, u16 dy, surface_rop_type_t rop);
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  u8 (*surface_get_attr)(void *p_surf);
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  s32 (*surface_get_handle)(void *p_surf);  
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  u8 (*surface_get_bpp)(void *p_surf);    
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  u8 (*surface_get_format)(void *p_surf);  
  /*!
     Bitblt, performs a bit-block transfer of the color data corresponding to
     a rectangle of pixels from the specified source surface into a
     destination surface.
    */
  u32 (*surface_get_palette_addr)(void *p_surf);    
  /*!
    surface start batch.
    */
  void (*surface_start_batch)(void *p_surf);
  /*!
    surface end batch.
    */
  void (*surface_end_batch)(void *p_surf, BOOL is_sync, rect_t *p_rect);
}surface_proc_t;

#endif
