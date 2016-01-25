/******************************************************************************/
/******************************************************************************/
#ifndef __GPE_1BPP_H__
#define __GPE_1BPP_H__

/*!
  Draws a pixel with the specifies color.
  
  \param[in] p_gpe the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to  the x-coordinate and the y-coordinate.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_soft_draw_pixel_1bpp(struct hld_gpe *p_gpe,
                                            region_t *p_region,
                                            pos_t *p_pos,
                                            u32 c);

/*!
  Draws a horizontal line with the specifies color.
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] w Specifies the width.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_soft_draw_h_line_1bpp(struct hld_gpe *p_gpe,
                                            region_t *p_region,
                                            pos_t *p_pos,
                                            u32 w,
                                            u32 c);

/*!
  Draws a vertical line with the specifies color.
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] h Specifies the height.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_soft_draw_v_line_1bpp(struct hld_gpe *p_gpe,
                                            region_t *p_region,
                                            pos_t *p_pos,
                                            u32 h,
                                            u32 c);

/*!
  Draws a rectangle with the specifies color.
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect Points to the rectangle position and size.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_soft_draw_rectangle_1bpp(struct hld_gpe *p_gpe,
                                            region_t *p_region,
                                            rect_vsb_t *p_rect,
                                            u32 c);

/*!
  Draws a image on a region.
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect pointer to  the image position and size.
  \param[in] p_buf Specifies the image buf pointer.Only frame.
  \param[in] p_palette Specifies the image palette pointer.
  \param[in] entries_num The number of palette entries.
  \param[in] pitch The input buffer pitch.
  \param[in] in_size The input buffer size in bytes.
  \param[in] fmt Specifies the image pixel format.
  \param[in] p_param Points to the parameter package, colorkey and alpha is valid in this function.
  */
RET_CODE gpe_soft_draw_image_1bpp(struct hld_gpe *p_gpe,
                                        region_t *p_region,
                                        rect_vsb_t *p_rect,
                                        void *p_buf,
                                        void *p_palette,
                                        u32 entries,
                                        u32 pitch,
                                        u32 in_size,
                                        pix_fmt_t fmt,
                                        gpe_param_vsb_t *p_param,
                                        rect_vsb_t *p_fill_rect);
/*!
  Dump a region image to a buffer.
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_region Points to the src region
  \param[in] p_rect pointer to  the image position and size.
  \param[in] p_buf Specifies the dst buf pointer. Only frame
  \param[in] p_palette Specifies the dst palette pointer.
  \param[out] p_entries_num Points to the number of palette entries.  
  \param[in] fmt Specifies the dst buffer pixel format.
  */
RET_CODE gpe_soft_dump_image_1bpp(struct hld_gpe *p_gpe,
                                        region_t *p_region,
                                        rect_vsb_t *p_rect,
                                        void *p_buf,
                                        void *p_palette,
                                        u32 *p_entries,
                                        pix_fmt_t fmt);

/*!
  Performs a bit-block transfer of the color data corresponding to a rectangle
  of pixels from the specified block into a destination region
  
  \param[in] p_gpe Points to the gpe device.
  \param[in] p_dst Points to the destination region
  \param[in] p_dst_rc Points to the destination rectangle.
  \param[in] p_ref Points to the reference region
  \param[in] p_ref_rc Points to the reference rectangle.
  \param[in] p_src Points to the source region
  \param[in] p_src_rc Points to the source rectangle.
  \param[in] clip_w Effective clip width.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_soft_bitblt_1bpp(struct hld_gpe *p_gpe,
                                    region_t *p_dst,
                                    rect_vsb_t *p_dst_rc,
                                    region_t *p_ref,
                                    rect_vsb_t *p_ref_rc,
                                    region_t *p_src,
                                    rect_vsb_t *p_src_rc,
                                    u32 clip_w,
                                    gpe_param_vsb_t *p_param);

#endif
