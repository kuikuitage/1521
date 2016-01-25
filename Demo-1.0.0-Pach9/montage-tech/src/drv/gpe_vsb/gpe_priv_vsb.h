/******************************************************************************/
/******************************************************************************/

#ifndef __GPE_PRIV_VSB_H__
#define __GPE_PRIV_VSB_H__

/*!
  The most significant bit being left.
  */
#define BEING_LEFT             0
/*!
  The most significant bit being right.
  */
#define BEING_RIGHT            1

/*!
  The macro is defined to determines the most significant bit is
  being left or right.
  */
#define GPE_SIGNIFICANT_BIT    BEING_LEFT


/*!
  gpe device structure
*/
typedef struct gpe_device
{
  /*!
    device base control
    */
  void *p_base;
  /*!
    device private handle
    */
  void *p_priv;
} gpe_device_t;


/*!
  This structure defines the type of the private data in a GPE device.
  */
typedef struct hld_gpe
{
  /*!
  The pointer to the private variables and structures.
  */
  void *p_priv;  
  /*!
  The pointer to the function of draw pixel. 
  */
  RET_CODE (*draw_pixel)(struct hld_gpe *p_gpe, region_t *p_region, pos_t *p_pos, u32 c);
  /*!
  The pointer to the function of draw horizontal line. 
  */
  RET_CODE (*draw_h_line)(struct hld_gpe *p_gpe, region_t *p_region, pos_t *p_pos, u32 w, u32 c);
  /*!
  The pointer to the function of draw vertical line. 
  */
  RET_CODE (*draw_v_line)(struct hld_gpe *p_gpe, region_t *p_region, pos_t *p_pos, u32 h, u32 c);
  /*!
  The pointer to the function of draw rectangle. 
  */
  RET_CODE (*draw_rectangle)(struct hld_gpe *p_gpe, region_t *p_region, rect_vsb_t *p_rect, u32 c);
  /*!
  The pointer to the function of draw image. 
  */
  RET_CODE (*draw_image)(struct hld_gpe *p_gpe, 
                          region_t *p_region,
                          rect_vsb_t *p_rect,
                          void *p_buf,
                          void *p_palette,
                          u32 entries_num,
                          u32 pitch,
                          u32 in_size,
                          pix_fmt_t fmt,
                          gpe_param_vsb_t *p_param,
                          rect_vsb_t *p_fill_rect);
  /*!
  The pointer to the function of dump image. 
  */
  RET_CODE (*dump_image)(struct hld_gpe *p_gpe, 
                          region_t *p_region,
                          rect_vsb_t *p_rect,
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
RET_CODE (*dump_image_v2)(struct hld_gpe *p_gpe,  
                        region_t *p_region,
                        rect_vsb_t *p_rect,
                        void *p_buf,
                        u32 buf_size,
                        void *p_pal_buf,
                        u32 pal_buf_size,
                        pix_fmt_t fmt,
                        gpe_dump_param_t *p_dump);
                        
  /*!
  The pointer to the function of bitblt. 
  */
  RET_CODE (*bitblt)(struct hld_gpe *p_gpe,
                      region_t *p_dst,
                      rect_vsb_t *p_dst_rc,
                      region_t *p_ref,
                      rect_vsb_t *p_ref_rc,
                      region_t *p_src,
                      rect_vsb_t *p_src_rc,
                      gpe_param_vsb_t *p_param);

  /*!
  The pointer to the function of mixer. 
  */
  RET_CODE (*mix)(struct hld_gpe *p_gpe,
                      region_t *p_dst,
                      rect_vsb_t *p_dst_rc,
                      region_t *p_src1,
                      rect_vsb_t *p_src1_rc,
                      region_t *p_src2,
                      rect_vsb_t *p_src2_rc,
                      region_t *p_src3,
                      rect_vsb_t *p_src3_rc,
                      gpe_mix_param_t *p_param);
  
  /*!
  The pointer to the function of draw pixel. 
  */
  RET_CODE (*start_batch)(struct hld_gpe *p_gpe, region_t *p_region);
  /*!
  The pointer to the function of draw pixel. 
  */
  RET_CODE (*end_batch)(struct hld_gpe *p_gpe, region_t *p_region, BOOL is_sync);  


  /*!
  The pointer to the function of shar scale. 
  */
  RET_CODE (*share_scale)(struct hld_gpe *p_gpe, 
                         region_t *p_src_rgn, 
                         rect_vsb_t *p_rect,
                         region_t *p_dst_rgn,
                         rect_vsb_t *p_dst_rect,
                         u32 param,
                         BOOL matrix);

  /*!
  The pointer to the function of scale mix. 
  */
  RET_CODE (*scale_mix)(struct hld_gpe *p_gpe,
                         region_t *p_src_rgn,
                         rect_vsb_t *p_src_rect,
                         region_t *p_dst_rgn,
                         rect_vsb_t *p_dst_rect,
                         gpe_scale_param_t *p_param);
                         
  /*!
  The pointer to the function of check color 
  */
  RET_CODE (*check_color)(struct hld_gpe *p_gpe, region_t *p_region, rect_vsb_t *p_rect, u32 color);


  /*!
  The pointer to the function of check color 
  */
  RET_CODE (*cmdfifo_set)(struct hld_gpe *p_gpe, BOOL is_cmd);
  
  /*!
  The pointer to the function of check color 
  */
  RET_CODE (*cmdfifo_start)(struct hld_gpe *p_gpe);

}hld_gpe_t;



/*!
  This structure defines the type of the private data in a GPE device.
  */
typedef hld_gpe_t lld_gpe_t;

/*!
  Gets the effected rectange if this block has GPE_CMD_CLIP_RECT attribute.
  \param[in] p_src1 Pointer to the region rectange.
  \param[in] p_src2 Points to the clip rect.
  \param[out] p_dst Points to the dst rect.

  return TRUE if sucess otherwize not effective rect clip.
*/
BOOL gpe_priv_get_effect_rc(rect_vsb_t *p_dst, rect_vsb_t *p_src1, rect_vsb_t *p_src2);


#endif // __GPE_PRIV_H__

