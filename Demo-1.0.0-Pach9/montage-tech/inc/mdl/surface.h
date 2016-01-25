/******************************************************************************/
/******************************************************************************/
#ifndef __SURFACE_H__
#define __SURFACE_H__

/*!
   Color definition in red/green/blue/alpha format
  */
typedef struct
{
  /*!
     Red
    */
  u8 r;
  /*!
     Green
    */
  u8 g;
  /*!
     Blue
    */
  u8 b;
  /*!
     Alpha: 0 for transparent, and 255 for solid
    */
  u8 a;
} color_t;

/*!
   Defines raster operation type
  */
typedef enum
{
  /*!
     Set to the new value, erase the original.
    */
  SURFACE_ROP_SET = 0x01,
  /*!
     AND'd the new value with the original.
    */
  SURFACE_ROP_AND,
  /*!
     OR'd the new value with the original.
    */
  SURFACE_ROP_OR,
  /*!
     XOR'd the new value with the original.
    */
  SURFACE_ROP_XOR
} surface_rop_type_t;

/*!
   The structure is defined to descript a palette.
  */
typedef struct
{
  /*!
     The number of palette entries.
    */
  u16 cnt;
  /*!
     the entries of palette.
    */
  color_t *p_entry;
} palette_t;

/*!
   The structure is defined to descript a bitmap.
  */
typedef struct
{
  /*!
     The pixel format of the bitmap.
    */
  u8 format;
  /*!
     The flag for use color key.
    */
  u8 enable_ckey;
  /*!
     Bits per piexel.
    */
  u8 bpp;
  /*!
     The palette of the bitmap.
    */
  palette_t pal;
  /*!
     The width of the bitmap.
    */
  u16 width;
  /*!
     The height of the bitmap.
    */
  u16 height;
  /*!
     The bits of the bitmap.
    */
  u8 *p_bits;
  /*!
     The pitch of the bitmap.
    */
  u32 pitch;
  /*!
    The alpha map.
    */
  u8 *p_alpha; 
  /*!
    alpha pitch
    */
  u32 alpha_pitch;    
  /*!
    color
    */
  u32 color;    
  /*!
     The color key value.
    */
  u32 colorkey;
  /*!
    is font
    */
  BOOL is_font;  
  /*!
    strok alpha
    */
  u8 *p_strok_alpha;
  /*!
    stroke char
    */
  u8 *p_strok_char;    
} bitmap_t;

/*!
   The macro indicates enable display or NOT.
  */
#define SURFACE_ATTR_EN_DISPLAY     0x01
/*!
   The macro indicates enable clipping or NOT.
  */
#define SURFACE_ATTR_EN_CLIP        0x02
/*!
   The macro indicates enable palette or NOT.
  */
#define SURFACE_ATTR_EN_PALETTE     0x04
/*!
   The macro indicates enable colorkey or NOT.
  */
#define SURFACE_ATTR_EN_COLORKEY    0x08

/*!
   Invalid color key for all format.
  */
#define SURFACE_INVALID_COLORKEY    (~1)



/*!
   Specifis the capabilities of descriptor.
  */
#define SURFACE_DESC_CAPS          0x01
/*!
   Specifis the fixel format of descriptor.
  */
#define SURFACE_DESC_FORMAT        0x02
/*!
   Specifis the width of descriptor.
  */
#define SURFACE_DESC_WIDTH         0x04
/*!
   Specifis the height of descriptor.
  */
#define SURFACE_DESC_HEIGHT        0x08
/*!
   Specifis the pitch of descriptor.
  */
#define SURFACE_DESC_PITCH         0x10

/*!
   Assign the display buffer directly.
  */
#define SURFACE_CAPS_MEM_ASSIGN    0x01
/*!
   Allocate the display buffer from system memory.
  */
#define SURFACE_CAPS_MEM_SYSTEM    0x02

/*!
   The structure is defined to descript a surface descriptor.
  */
typedef struct
{
  /*!
     The flag of create surface
    */
  u8 flag;
  /*!
     The capability
    */
  u8 caps;
  /*!
     The pixel format
    */
  u8 format;
  /*!
     The width
    */
  u16 width;
  /*!
     The height
    */
  u16 height;
  /*!
     The pitch
    */
  u32 pitch;
  /*!
     The palette
    */
  palette_t *p_pal;
  /*!
     The display buffer odd & even
    */
  u8 *p_vmem[2];
} surface_desc_t;

/*!
   Release the surface module.
  */
void surface_release(u32 handle);

/*!
   Enable display for all surface.

   \param[in] is_enable Eanble or NOT.
  */
void surface_enable(u32 handle, void * p_surf, BOOL is_enable);

/*!
   Creates a surface with a specifies descriptor.

   \param[out] pp_surf Points to a address of surface.
   \param[in] p_desc Specifies the descriptor.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_create(u32 handle, void **pp_surf, surface_desc_t *p_desc);

/*!
   Deletes a surface.

   \param[in] p_surf Points to a surface.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_delete(u32 handle, void *p_surf);

/*!
   Display a surface.

   \param[in] p_surf Points to a surface.
   \param[in] is_display Display the surface or NOT.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_display(u32 handle, void *p_surf, BOOL is_display);

/*!
   Sets the global alpha blend of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] alpha Specifies the alpha value.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_alpha(u32 handle, void *p_surf, u8 alpha);

/*!
   Sets the transparence of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] index Specifies the index of the transparent color.
   \param[in] alpha Specifies the alpha level of transparency.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_trans(u32 handle, void *p_surf, u32 index, u8 alpha);

/*!
   Initialize the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_pal Points to the specifies palette.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_init_palette(u32 handle, void *p_surf, palette_t *p_pal);

/*!
   Sets the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] start Specifies the index of beginning.
   \param[in] len Specifies the length of entrys be set.
   \param[in] p_entry Points to the entrys.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_palette(u32 handle, void *p_surf, u16 start, u16 len, color_t *p_entry);

/*!
   Gets the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] start Specifies the index of beginning.
   \param[in] len Specifies the length of entrys be set.
   \param[in] p_entry Points to the entrys.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_palette(u32 handle, void *p_surf, u16 start, u16 len, color_t *p_entry);

/*!
   Sets the colorkey of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] ckey Specifies the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_colorkey(u32 handle, void *p_surf, u32 ckey);

/*!
   Sets the transparent key of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] ckey Specifies the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_trans_clr(u32 handle, void *p_surf, u32 trans_clr);

/*!
   Gets the transparence of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_ckey Points to the address of the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_colorkey(u32 handle, void *p_surf, u32 *p_ckey);

/*!
   Sets the clipping rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_cliprect(u32 handle, void *p_surf, rect_t *p_rc);

/*!
   Gets the clipping rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_cliprect(u32 handle, void *p_surf, rect_t *p_rc);

/*!
   Sets the display rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_srcrect(u32 handle, void *p_surf, rect_t *p_rc);

/*!
   Gets the display rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_srcrect(u32 handle, void *p_surf, rect_t *p_rc);

/*!
   Fill a rectangle with the specified color.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to a rectangle.
   \param[in] value Specifies the value of the color.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_fill_rect(u32 handle, void *p_surf, rect_t *p_rc, u32 value);

/*!
   Fill a round rectangle with the specified color.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to a rectangle.
   \param[in] value Specifies the value of the color.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_fill_round_rect(u32 handle, void *p_surf, rect_t *p_rc, u32 value);

/*!
   Fill a rectangle with the specified bitmap.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to a rectangle.
   \param[in] p_bmp Points to a bitmap.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_fill_bmp(u32 handle, void *p_surf, rect_t *p_rc, bitmap_t *p_bmp);

/*!
   Gets the image from a surface with the specified rectangle.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to a rectangle.
   \param[out] p_buf Points to a buffer.
   \param[in] pitch Specifies the patch of buffer.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_bits(u32 handle, void *p_surf, rect_t *p_rc, void *p_buf, u32 pitch);

/*!
   Bitblt, performs a bit-block transfer of the color data corresponding to
   a rectangle of pixels from the specified source surface into a
   destination surface.

   \param[in] p_src_surf Points to a source surface.
   \param[in] sx Specifies the x-coordinate of the upper-left corner
                of the source rectangle.
   \param[in] sy Specifies the y-coordinate of the upper-left corner
                of the source rectangle.
   \param[in] sw Specifies the width of the source rectangles.
   \param[in] sh Specifies the height of the source rectangles.
   \param[in] p_dst_surf Points to a destination surface.
   \param[in] dx Specifies the x-coordinate of the upper-left corner
                of the destination rectangle.
   \param[in] dy Specifies the y-coordinate of the upper-left corner
                of the destination rectangle.
   \param[in] rop Specifies the raster operation type.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_bitblt(u32 handle, void *p_src_surf,
  u16 sx, u16 sy, u16 sw, u16 sh, void *p_dst_surf, u16 dx, u16 dy, surface_rop_type_t rop);

/*!
   Gets surface attr.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return attr.
  */
u8 surface_get_attr(u32 handle, void *p_surf);

/*!
   Gets surface handle.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return handle.
  */
s32 surface_get_handle(u32 handle, void *p_surf);

/*!
   Gets surface bpp.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return bpp.
  */
u8 surface_get_bpp(u32 handle, void *p_surf);

/*!
   Gets surface format.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return format.
  */
u8 surface_get_format(u32 handle, void *p_surf);

/*!
   Gets palette address.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return palette.
  */
palette_t *surface_get_palette_addr(u32 handle, void *p_surf);

/*!
   surface start batch.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return NULL.
  */
void surface_start_batch(u32 handle, void *p_surf);

/*!
   surface start batch.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.
   \param[in] is_sync : sync paint or not

   \return NULL.
  */
void surface_end_batch(u32 handle, void *p_surf, BOOL is_sync, rect_t *p_rect);

#endif
