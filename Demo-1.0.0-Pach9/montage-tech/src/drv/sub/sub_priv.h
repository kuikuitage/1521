/******************************************************************************/
/******************************************************************************/

#ifndef __SUB_PRIV_H__
#define __SUB_PRIV_H__


/*!
  This structure defines the type of the private data in a RF device.
  */
typedef struct hld_sub
{
  /*!
    The pointer to the private variables and structures.
    */
  void *p_priv;  
  /*!
    Create a sub region. 
    */
  RET_CODE (*create_region)(struct hld_sub *p_sub, 
    const sub_region_desc_t *p_desc, u32 *p_palette_in, u8 *p_handle);
  /*!
    Delete a sub region. 
    */
  RET_CODE (*delete_region)(struct hld_sub *p_sub);
  /*!
    Move a sub region. 
    */
  RET_CODE (*move_region)(struct hld_sub *p_sub, u32 handle, u16 x, u16 y);
  /*!
    Get the region info. 
    */
  RET_CODE (*get_region_desc)
    (struct hld_sub *p_sub, u32 handle, sub_region_desc_t *p_region_desc);
  /*!
    Get the start address of the color data in a frame. 
    */
  RET_CODE (*get_region_buffer)
    (struct hld_sub *p_sub, u32 handle, u32 *p_odd_addr, u32 *p_even_addr);
  /*! 
    Display/hide a sub region. 
    */
  void (*set_region_display)(struct hld_sub *p_sub, u32 handle, BOOL is_show);
  /*!
    Set alpha mode for a sub region.
    */
  RET_CODE (*set_region_alphamode)(struct hld_sub *p_sub, 
    u32 handle, BOOL is_plane_enable, BOOL is_region_enable);
  /*!
    Set the alpha value for a sub region.
    */
  RET_CODE (*set_region_alpha)(struct hld_sub *p_sub, u32 handle, u8 alpha);
  /*!
    Set transparent color in a region. (ONLY for color modes with palettes.) 
    */
  RET_CODE (*set_region_trans)
    (struct hld_sub *p_sub, u32 handle, u32 index, u8 alpha);
  /*!
    Set the palette of a sub region
    */
  RET_CODE (*set_region_palette)
    (struct hld_sub *p_sub, u32 handle, u32 *p_palette);
  /*! 
    Set the edge weight of sub layer.
    */
  RET_CODE (*set_edge_weight)
    (struct hld_sub *p_sub, u8 top_weight, u8 bot_weight);
  /*! 
    Set the alpha level of sub layer. 
    */
  void (*set_plane_alpha)(struct hld_sub *p_sub, u8 alpha);
  /*! 
    Enable color key. 
    */
  RET_CODE (*color_key_en)(struct hld_sub *p_sub, BOOL is_colorkey);
  /*!
    Set color key. 
    */
  RET_CODE (*set_color_key)(struct hld_sub *p_sub, u8 r, u8 g, u8 b);
}hld_sub_t;

#endif // __SUB_PRIV_H__

