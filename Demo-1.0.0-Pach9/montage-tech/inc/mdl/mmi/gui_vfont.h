/******************************************************************************/
/******************************************************************************/
#ifndef __GUI_VFONT_H__
#define __GUI_VFONT_H__
/*!
   \file gui_vfont.h

   This file defined data structures for several kinds of resource head.
   And defines interfaces for users to get a specified resource.

   Development policy:
  */

/*!
  vfont config paramter
  */
typedef struct
{
  /*!
    max count
    */
  u32 max_cnt;
  /*!
    max width
    */
  u16 max_width;
  /*!
    max height
    */
  u16 max_height;    
  /*!
    is alpha blending enable(gpe support), if yes->gray mode, else mono mode.
    */
  BOOL is_alpha_spt;    
  /*!
    bytes per pixel.
    */
  u8 Bpp;    
}vfont_config_t;

/*!
  cache paramter.
  */
typedef struct
{
  /*!
    alpha map
    */
  void *p_alpha;
  /*!
    char data
    */
  void *p_char;
  /*!
    char height
    */
  u16 char_height;
  /*!
    char width
    */
  u16 char_width;
  /*!
    x offset
    */
  s16 xoff;
  /*!
    y offset
    */
  s16 yoff;    
  /*!
    pitch
    */
  u16 alpha_pitch;
  /*!
    char code
    */
  u16 char_code;
  /*!
    font id
    */
  u16 font_id;
  /*!
    face id
    */
  u8 face_id;
  /*!
    step width
    */
  u16 step_width;
  /*!
    step height
    */
  u16 step_height;  
  /*!
    x step
    */
  u16 x_step;  
  /*!
    char height
    */
  u16 height;
  /*!
    char width
    */
  u16 width;  
  /*!
    color
    */
  u32 color;
  /*!
    attr
    */
  u32 attr;    
  /*!
    stork alpha
    */
  void *p_strok_alpha;
  /*!
    stork char
    */
  void *p_strok_char;    
}vfont_cache_t;


/*!
   gui roll info.
  */
typedef struct
{
  /*!
     get char attribute.
    */
  void (*vf_get_attr)(void *p_priv,
    u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle,
    u16 *p_width, u16 *p_height);  

  /*!
     check char.
    */
  BOOL (*vf_check_char)(void *p_priv,
    u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle, u16 font_lib_id);  

  /*!
    get char.
    */
  u32 (*vf_get_char)(void *p_priv,
    u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle,
    void *p_cache, rsc_char_info_t *p_info);

  /*!
    hit cache.
    */
  BOOL (*vf_hit_cache)(void *p_priv, u16 char_code, rsc_fstyle_t *p_fstyle,
    rsc_char_info_t *p_info);

  /*!
    insert cache.
    */
  void *(*vf_insert_cache)(void *p_priv, u16 char_code, rsc_fstyle_t *p_fstyle);

  /*!
    private data.
    */
  void *p_data;  
}gui_vfont_t;

/*!
  Vector font init.
  
  \param[in] p_cfg : config parameter.

  \return : NULL
  */
void vfont_ft_init(vfont_config_t *p_cfg);

/*!
  Get char attribute.

  \param[in] char_code : unicode.
  \param[in] p_font : font data.
  \param[in] size : size.
  \param[in] p_fstyle : font style.
  \param[out] p_width : width.
  \param[out] p_height : height.

  \return NULL
  */
void gui_vfont_get_char_attr(u16 char_code, void *p_font, u32 size, 
  rsc_fstyle_t *p_fstyle, u16 *p_width, u16 *p_height);

/*!
  Check char.

  \param[in] char_code : unicode.
  \param[in] p_font : font data.
  \param[in] size : size.
  \param[in] p_fstyle : font style.

  \return True or False.
  */
BOOL gui_vfont_check_char(u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle, u16 font_lib_id);

/*!
  Get char.

  \param[in] char_code : unicode.
  \param[in] p_font : font data. 
  \param[in] size : size. 
  \param[in] p_fstyle : font style.
  \param[in] p_cache : cache. 
  \param[out] p_info : char info. 
  
  \return color key.
  */
u32 gui_vfont_get_char(u16 char_code, void *p_font, u32 size,
  rsc_fstyle_t *p_fstyle, void *p_cache, rsc_char_info_t *p_info);


/*!
  Hit cache.

  \param[in] char_code : unicode.
  \param[in] p_style : font style. 
  \param[out] p_info : char info. 

  \return True or False. 
  */
BOOL gui_vfont_hit_cache(u16 char_code, rsc_fstyle_t *p_style, rsc_char_info_t *p_info);

/*!
  Insert cache.

  \param[in] char_code : unicode.
  \param[in] p_style : font style. 

  \return cache.
  */
void *gui_vfont_insert_cache(u16 char_code, rsc_fstyle_t *p_style);

#endif

