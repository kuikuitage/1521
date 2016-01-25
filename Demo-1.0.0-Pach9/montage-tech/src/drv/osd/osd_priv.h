/******************************************************************************/
/******************************************************************************/

#ifndef __OSD_PRIV_H__
#define __OSD_PRIV_H__


/*!
  This structure defines the type of the private data in a RF device.
  */
typedef struct hld_osd
{
  /*!
    The pointer to the private variables and structures.
    */
  void *p_priv;  
  /*!
    The pointer to the function of creating an OSD region. 
    */
  RET_CODE (*create_region)(struct hld_osd *p_osd, 
    const osd_region_desc_t *p_desc, u32 *p_palette_in, u8 *p_handle);
  /*!
    The pointer to the function of deleting an OSD region. 
    */
  RET_CODE (*delete_region)(struct hld_osd *p_osd);
  /*!
    The pointer to the function of moving an osd region. 
    */
  RET_CODE (*move_region)(struct hld_osd *p_osd, u32 handle, u16 x, u16 y);
  /*!
    The pointer to the function of getting the region info. 
    */
  RET_CODE (*get_region_desc)
    (struct hld_osd *p_osd, u32 handle, osd_region_desc_t *p_region_desc);
  /*!
    The pointer to the function of getting the start address of the color data in a frame. 
    */
  RET_CODE (*get_region_buffer)
    (struct hld_osd *p_osd, u32 handle, u32 *p_odd_addr, u32 *p_even_addr);
  /*!
    The pointer to the function of displaying/hiding an OSD region. 
    */
  RET_CODE (*set_region_display)
    (struct hld_osd *p_osd, u32 handle, BOOL is_show);
  /*!
    The pointer to the function of setting the alpha mode for an OSD region. 
    */
  RET_CODE (*set_region_alphamode)(struct hld_osd *p_osd, 
    u32 handle, BOOL is_plane_enable, BOOL is_region_enable);
  /*!
    The pointer to the function of setting the alpha value for an OSD region. 
    */
  RET_CODE (*set_region_alpha)(struct hld_osd *p_osd, u32 handle, u8 alpha);
  /*!
    The pointer to the function of  setting transparent color in a region. 
    (ONLY for color modes with palettes.) 
    */
  RET_CODE (*set_region_trans)
    (struct hld_osd *p_osd, u32 handle, u32 index, u8 alpha);
  /*!
    The pointer to the function of setting the palette of an OSD region
    */
  RET_CODE (*set_region_palette)
    (struct hld_osd *p_osd, u32 handle, u32 *p_palette);
  /*! 
    The pointer to the function of setting a palette entry of an OSD region.
    */
  RET_CODE (*set_region_palette_entry)
     (struct hld_osd *p_osd, u32 handle, u32 entry, u32 color);
  /*! 
    The pointer to the function of getting a palette entry of an OSD region.
    */
  RET_CODE (*get_region_palette_entry)
     (struct hld_osd *p_osd, u32 handle, u32 index, u32 *color);
  /*!
    The pointer to the function of setting the edge weight of the OSD layer. 
    */
  void (*set_edge_weight)(struct hld_osd *p_osd, u8 top_weight, u8 bot_weight);
  /*!
    The pointer to the function of setting the alpha level of the OSD layer. 
    */
  void (*set_plane_alpha)(struct hld_osd *p_osd, u8 alpha);
  /*!
    The pointer to the function of enabling/disabling color key. 
    */
  void (*color_key_en)(struct hld_osd *p_osd, BOOL is_colorkey);
  /*!
    The pointer to the function of setting color key. 
    */
  void (*set_color_key)(struct hld_osd *p_osd, u8 r, u8 g, u8 b);
}hld_osd_t;

#endif // __OSD_PRIV_H__

