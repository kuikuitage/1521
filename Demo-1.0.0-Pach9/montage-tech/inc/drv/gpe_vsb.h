/******************************************************************************/
/******************************************************************************/
#ifndef __GPE_VSB_H__
#define __GPE_VSB_H__

/*!
   The flag of non-cacheble writing
  */
#define NON_CACHE_FLAG    0x10000000
/*!
   Converts a given normal address to a non-cacheble writing address
  */
#define CONVERT_TO_NON_CACHE(x)    (x = (void *)((u32)x | NON_CACHE_FLAG))


/****************************************************************************
*full version of rop3 ops in http://www.vckbase.com/document/viewdoc/?id=509
****************************************************************************/

/*!
  dest = source
  */
#define SRCCOPY             (u8)0xCC
/*!
  dest = source OR dest           
  */
#define SRCPAINT            (u8)0xEE 
/*!
  dest = source AND dest          
  */
#define SRCAND              (u8)0x88 
/*!
  dest = source XOR dest          
  */
#define SRCINVERT           (u8)0x66 
/*!
  dest = source AND (NOT dest)   
  */
#define SRCERASE            (u8)0x44 
/*!
  dest = (NOT source)             
  */
#define NOTSRCCOPY          (u8)0x33 
/*!
  dest = (NOT src) AND (NOT dest) 
  */
#define NOTSRCERASE         (u8)0x11 
/*!
  dest = (source AND pattern)     
  */
#define MERGECOPY           (u8)0xC0 
/*!
  dest = (NOT source) OR dest     
  */
#define MERGEPAINT          (u8)0xBB 
/*!
  dest = pattern                  
  */
#define PATCOPY             (u8)0xF0 
/*!
  dest = DPSnoo    (NOT Src) OR Patten OR Dest               
  */
#define PATPAINT            (u8)0xFB 
/*!
  dest = pattern XOR dest         
  */
#define PATINVERT           (u8)0x5A 
/*!
  dest = (NOT dest)               
  */
#define DSTINVERT           (u8)0x55 
/*!
  dest = BLACK                    
  */
#define BLACKNESS           (u8)0x00 
/*!
  dest = WHITE                    
  */
#define WHITENESS           (u8)0xFF 

/*!
  The command of alpha map
  */
#define GPE_CMD_ALPHA_MAP    0x00000010
/*!
  The command of raster operation
  */
#define GPE_CMD_RASTER_OP      0x00000020
/*!
  The command of plane alpha blending
  */
#define GPE_CMD_ALPHA_BLEND    0x00000040
/*!
  The command of color key
  */
#define GPE_CMD_COLORKEY       0x00000080
/*!
  The command of clipping rectangle
  */
#define GPE_CMD_CLIP_RECT      0x00000100
/*!
  The command of color expand
  */
#define GPE_CMD_COLOR_EXPAND   0x00000200



/*!
  The structure is defined to descript the paramter of GPE.
  */
typedef struct
{
  /*!
    The command
    */
  u32 cmd;
  /*!
    The raster operation type
    */
  u32 rop3;
  /*!
    The color expand palette pointer
    */
  void *p_palette;
  /*!
    The alpha map buffer pointer
    */
  void *p_alpha;

  /*!
    The alpha map buffer picth
    */
  u32  alpha_pitch;
  /*!
    The plane alpha value
    */
  u32 plane_alpha;
  /*!
    Enable the src color key
    */
  u32 enable_colorkey;
  /*!
    The src color key
    */
  u32 colorkey;
  /*!
    Enable the dst color key 
    */
  u32 enable_dst_ck;
  /*!
    The color key
    */
  u32 dst_colorkey;
  /*!
    is font
    */
  BOOL is_font;    
  /*!
    pattern data for rop3 operation
    */
  u32 pattern;
} gpe_param_vsb_t;


/*!
  The structure is defined to descript the output paramter of dump image
  */
typedef struct
{
  /*!
    indicate the w of image
    */
  u32   w;
  /*!
    indicate the w of image
    */
  u32   h;
  /*!
    indicate the pitch of image
    */
  u32   pitch;
  /*!
    has palette or not
    */
  BOOL  b_pal;
  /*!
    if has palette, the palette entry cunt
    */
  u32   pal_cunt;
  /*!
    indicate the buf start byte is the valid data
    */
  u32   start_byte;
  /*!
    indicate used the image buf size
    */
  u32   buf_size;
  /*!
    indicate used the palette buf size
    */
  u32   pal_size;
  
}gpe_dump_param_t;



/*!
  indicate the src1 lay take apart in the mix operation
  */
#define MIX_SRC1_FLAG        0x00000001

/*!
  indicate the src2 lay take apart in the mix operation
  */
#define MIX_SRC2_FLAG        0x00000002

/*!
  indicate the src3 lay take apart in the mix operation
  */
#define MIX_SRC3_FLAG        0x00000004

/*!
  indicate the alpha map lay take apart in the mix operation
  */
#define MIX_ALPHA_MAP_FLAG   0x00000008


/*!
  The alpha map channel 
  */
typedef enum
{
  /*!
     no alpha map channel 
    */
  NO_ALPHA_MAP_CHANNEL,

  /*!
    The alpha map channel 1
    */
  ALPHA_MAP_CHANNEL_1,

  /*!
    The alpha map channel 2
    */
  ALPHA_MAP_CHANNEL_2,

  /*!
    The alpha map channel 3
    */
  ALPHA_MAP_CHANNEL_3,
  
}alpha_map_ch_t;



/*!
  The structure is defined to descript the paramter of GPE scale
  */
typedef struct
{
  /*!
    When scaled, continue do the mix with dst 
    */
  BOOL enable_mix;    
  /*!
    The scale filter table id
    */
  u32  filter_table;
  /*!
    The scale use the matrix or not
    */
  BOOL enable_matrix;

  /*!
    When mix, the src mix start x
    */
  u32  src_mix_x;
  /*!
    When mix, the src mix start y
    */
  u32  src_mix_y;
  /*!
    When mix, the dst mix start x
    */
  u32  dst_mix_x;
  /*!
    When mix, the dst mix start y
    */
  u32  dst_mix_y;
  /*!
    When mix, enable the plane alpha of src layer
    */
  BOOL plane_alpha_en;
  /*!
    When mix, the src plane alpha value
    */
  u32  plane_alpha;
  /*!
    When mix, enable the color key of src layer
    */
  BOOL src_ck_en;
  /*!
    When mix,  the color key of src layer
    */
  u32  src_ck;
  /*!
    When mix, enable the color key of dst layer
    */
  BOOL dst_ck_en;
  /*!
    When mix,  the color key of dst layer
    */
  u32  dst_ck;
  /*!
    When mix,  the bg color value
    */
  u32  mix_bg_color;
  
}gpe_scale_param_t;


/*!
  The structure is defined to descript the paramter of GPE mixer
  */
typedef struct
{
  /*!
    The mix flag, MIX_SRC1_FLAG, MIX_SRC2_FLAG, MIX_SRC3_FLAG
    */
  u32 mix_flag;
  /*!
    The layer switch 
    */
  u32 layer_switch;
  /*!
    The alpha_map_ch
    */
  u32 alpha_map_ch;

  /*!
    The alpha_map_buf
    */
  u32 alpha_map_buf;

  /*!
    The alpha_map_buf pitch
    */
  u32 map_pitch;
  
  /*!
    The alpha_map w
    */
  u32 map_w;
  
  /*!
    The alpha_map h
    */
  u32 map_h;
  
  /*!
    The bg_color
    */
  u32 bg_color;

  /*!
    The src1_mix_x pos
    */
  u32 src1_mix_x;

  /*!
    The src1_mix_y pos
    */
  u32 src1_mix_y;

  /*!
    The src2_mix_x pos
    */
  u32 src2_mix_x;

  /*!
    The src2_mix_y pos
    */
  u32 src2_mix_y;

  /*!
    The src3_mix_x pos
    */
  u32 src3_mix_x;

  /*!
    The src3_mix_y pos
    */
  u32 src3_mix_y;

  /*!
    The src1_alpha_en
    */
  BOOL src1_alpha_en;

  /*!
    The src1_plane_alpha
    */
  u8 src1_plane_alpha;

  /*!
    The src2_alpha_en
    */
  BOOL src2_alpha_en;

  /*!
    The src2_plane_alpha
    */
  u8 src2_plane_alpha;

  /*!
    The src3_alpha_en
    */
  BOOL src3_alpha_en;

  /*!
    The src3_plane_alpha
    */
  u8 src3_plane_alpha;

  /*!
    The src1_ck_en
    */
  BOOL src1_ck_en;

  /*!
    The src1_colorkey
    */
  u32 src1_colorkey;

  /*!
    The src2_ck_en
    */
  BOOL src2_ck_en;

  /*!
    The src2_colorkey
    */
  u32 src2_colorkey;

  /*!
    The src3_ck_en
    */
  BOOL src3_ck_en;

  /*!
    The src3_colorkey
    */
  u32 src3_colorkey;
 
}gpe_mix_param_t;


/*!
  The structure is defined for gpe config info 
  */
typedef struct
{
  /*!
    The gpe align buffer is user config or auto config by gra engine
    */
  BOOL user_cfg;
  /*!
    The gpe align buffer address
    */
  u32 align_buf;

  /*!
    The gpe align buffer size
    */
  u32 buf_size;
}gpe_cfg_t;


/*!
  start batch.
  
  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  */
RET_CODE gpe_start_batch_vsb(void *p_dev, void *p_region);


/*!
  end batch.
  
  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] is_sync sync paint or not
  */
RET_CODE gpe_end_batch_vsb(void *p_dev, void *p_region, BOOL is_sync);

/*!
  Draws a pixel with the specifies color.
  
  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to  the x-coordinate and the y-coordinate.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_pixel_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 c);

/*!
  Draws a horizontal line with the specifies color.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] w Specifies the width.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_h_line_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 w, u32 c);

/*!
  Draws a vertical line with the specifies color.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] h Specifies the height.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_v_line_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 h, u32 c);

/*!
  Draws a rectangle with the specifies color.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect Points to the rectangle position and size.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_rectangle_vsb(void *p_dev, void *p_region, rect_t *p_rect, u32 c);

/*!
  Draws a image on a region.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect pointer to the image position and size in dest region
  \param[in] p_buf Specifies the image buf pointer.Only frame.
  \param[in] p_palette Specifies the image palette pointer if null use region palette.
  \param[in] entries_num The number of palette entries.
  \param[in] pitch The input buffer pitch.
  \param[in] in_size The input buffer size in bytes.
  \param[in] fmt Specifies the image pixel format.
  \param[in] p_param Points to the parameter package, colorkey and alpha is valid in this function.
  \param[in] src rect of image region
  */
RET_CODE gpe_draw_image_vsb(void *p_dev, 
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        void *p_palette,
                        u32 entries_num,
                        u32 pitch,
                        u32 in_size,
                        pix_fmt_t fmt,
                        gpe_param_vsb_t *p_param,
                        rect_t *p_fill_rect);
/*!
  Dump a region image to a buffer.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the src region
  \param[in] p_rect pointer to  the image position and size.
  \param[in] p_buf Specifies the dst buf pointer. Only frame
  \param[in] p_palette Specifies the dst palette pointer.
  \param[out] p_entries_num Points to the number of palette entries.  
  \param[in] fmt Specifies the dst buffer pixel format.
  */
RET_CODE gpe_dump_image_vsb(void *p_dev, 
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        void *p_palette,
                        u32 *p_entries_num,
                        pix_fmt_t fmt);


/*!
  Dump a region image to a buffer.
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the src region
  \param[in] p_rect pointer to the dump rect in the region.
  \param[in] p_buf Specifies the dst buf pointer. Only frame
  \param[in] buf_size Specifies the dst buf size.
  \param[in] p_palette Specifies the dst palette pointer.
  \param[in] pal_buf_size Specifies the palette buf size.
  \param[in] fmt Specifies the dst buffer pixel format.
  \param[out] p_dump Points to the number of palette entries.  
  */
RET_CODE gpe_dump_image_vsb_v2(void *p_dev, 
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        u32 buf_size,
                        void *p_pal_buf,
                        u32 pal_buf_size,
                        pix_fmt_t fmt,
                        gpe_dump_param_t *p_dump);

/*!
  Performs a bit-block transfer of the color data corresponding to a rectangle
  of pixels from the specified block into a destination region
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_dst Points to the destination region
  \param[in] p_dst_rc Points to the destination rectangle.
  \param[in] p_ref Points to the reference region
  \param[in] p_ref_rc Points to the reference rectangle.
  \param[in] p_src Points to the source region
  \param[in] p_src_rc Points to the source rectangle.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_bitblt_vsb(void *p_dev,
                void *p_dst,
                rect_t *p_dst_rc,
                void *p_ref,
                rect_t *p_ref_rc,
                void *p_src,
                rect_t *p_src_rc,
                gpe_param_vsb_t *p_param);



/*!
  Performs 3 regions mix,withc color key and alpah
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_dst Points to the destination region
  \param[in] p_dst_rc Points to the destination rectangle.
  \param[in] p_src1 Points to the reference region
  \param[in] p_src1_rc Points to the reference rectangle.
  \param[in] p_src2 Points to the source region
  \param[in] p_src2_rc Points to the source rectangle.
  \param[in] p_src3 Points to the source region
  \param[in] p_src3_rc Points to the source rectangle.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_mix_vsb(void *p_dev,
                void *p_dst,
                rect_t *p_dst_rc,
                void *p_src1,
                rect_t *p_src1_rc,
                void *p_src2,
                rect_t *p_src2_rc,
                void *p_src3,
                rect_t *p_src3_rc,
                gpe_mix_param_t *p_param);

                
/*!
   do scale  the src region into a destination region
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_rgn Points to the source region
  \param[in] p_src_rect Points to the rect of src region to scale
  \param[in] p_dst_rgn Points to the destination region.
  \param[in] p_dst_rect Points to the rect of dst region to scale
  \param[in] table_id: scale coeff table id
  \param[in] matrix indicate use the matrix or not
  */
RET_CODE gpe_share_scale(void *p_dev,
                         void *p_src_rgn,
                         rect_t *p_src_rect,
                         void *p_dst_rgn,
                         rect_t *p_dst_rect,
                         u32 table_id,
                         BOOL matrix);


/*!
   do scale the src region and then mix into a destination region
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_rgn Points to the source region
  \param[in] p_src_rect Points to the rect of src region to scale
  \param[in] p_dst_rgn Points to the destination region.
  \param[in] p_dst_rect Points to the rect of dst region to scale
  \param[in] p_param: scale param
  */
RET_CODE gpe_share_scale_mix(void *p_dev,
                         void *p_src_rgn,
                         rect_t *p_src_rect,
                         void *p_dst_rgn,
                         rect_t *p_dst_rect,
                         gpe_scale_param_t *p_param);

/*!
   check the region color
   this function just used for halping warirors to do the color check
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the region
  \param[in] p_rect the checked rect in the region
  \param[in] color a color.
  */
RET_CODE gpe_check_color(void *p_dev, void *p_region, rect_t *p_rect, u32 color);


/*!
   set enable the cmdfifo
  \param[in] p_dev Points to the gpe device.
  \param[in] is_cmd indicate cmd fifo enable or not
  */
RET_CODE gpe_cmdfifo_set(void *p_dev, BOOL is_cmd);


/*!
   start run the cmd fifo
  \param[in] p_dev Points to the gpe device.
  */
RET_CODE gpe_cmdfifo_start(void *p_dev);


#endif //__GPE_H__

