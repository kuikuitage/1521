/******************************************************************************/
/******************************************************************************/
#ifndef __GPE_PRIV_H__
#define __GPE_PRIV_H__

/*!
    gpe driver low level structure as the private data("priv") of struct
   "gpe_device_t"
  */
typedef struct lld_gpe
{
  /*!
      The private data of low level driver.
    */
  void *p_priv;

  /*!
      The low level function to implement the high level interface
     "draw_pixel".
    */
  RET_CODE (*draw_pixel)(struct lld_gpe *p_lld, gpe_desc_t *p_desc, u16 x,
                         u16 y, u32 c);
  /*!
      The low level function to implement the high level interface
     "draw_h_line".
    */
  RET_CODE (*draw_h_line)(struct lld_gpe *p_lld, gpe_desc_t *p_desc, u16 x,
                          u16 y, u16 w, u32 c);
  /*!
      The low level function to implement the high level interface
     "draw_v_line".
    */
  RET_CODE (*draw_v_line)(struct lld_gpe *p_lld, gpe_desc_t *p_desc, u16 x,
                          u16 y, u16 h, u32 c);
  /*!
      The low level function to implement the high level interface
     "put_image".
    */
  RET_CODE (*put_image)(struct lld_gpe *p_lld, gpe_desc_t *p_desc, rect_t *p_rc,
                        point_t *p_pt, void *p_buf, u32 pitch);
  /*!
      The low level function to implement the high level interface
     "get_image".
    */
  RET_CODE (*get_image)(struct lld_gpe *p_lld, gpe_desc_t *p_desc, rect_t *p_rc,
                        point_t *p_pt, void *p_buf, u32 pitch);
  /*!
      The low level function to implement the high level interface
     "put_image_mask".
    */
  RET_CODE (*put_image_mask)(struct lld_gpe *p_lld, gpe_desc_t *p_desc,
                             rect_t *p_rc, point_t *p_pt, void *p_buf,
                             u32 pitch, u32 mask);
  /*!
      The low level function to implement the high level interface
     "bitblit".
    */
  RET_CODE (*bitblt)(struct lld_gpe *p_lld, gpe_desc_t *p_dst_desc,
                     rect_t *p_dst_rc, gpe_desc_t *p_src_desc, rect_t *p_src_rc,
                     gpe_param_t *p_param);
}lld_gpe_t;

/*!
   Gets the effected rectange if this block has GPE_CMD_CLIP_RECT attribute.

   \param[in/out] p_erc Points to the effected rectange.
   \param[in] p_param Points to the block descriptor

   return If the effected rectangle is NOT empty return TRUE,
          Otherwise return FALSE.
  */
BOOL gpe_get_effect_rc(rect_t *p_erc, gpe_param_t *p_param);

#endif //__GPE_PRIV_H__
