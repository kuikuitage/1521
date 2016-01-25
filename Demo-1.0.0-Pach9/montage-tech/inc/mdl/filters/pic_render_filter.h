/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_RENDER_FILTER_H_
#define __PIC_RENDER_FILTER_H_

typedef struct tag_pic_render_clr_key
{
  /*!
    enable_key
    */
  BOOL enable_key;
  /*!
    color_key
    */
  u32 color_key;
  
}pic_render_clr_key_t;

/*!
  jpeg pic_render filter command define
  */
typedef enum tag_pic_render_filter_cmd
{
  /*!
    scale
    */
  PIC_RENDER_CFG_RGN,
  /*!
    fill rect.
    */  
  PIC_RENDER_CFG_RECT,    
  /*!
   set color key
    */
  PIC_RENDER_CFG_CLR_KEY,
}pic_render_filter_cmd_t;

/*!
  pic_render filter event define
  */
typedef enum tag_pic_render_filter_evt
{
  /*!
    config read file' name
    */
  PIC_DRAW_END = PIC_RENDER_FILTER << 16,
  /*!
    dynamic draw end
    */
  PIC_DRAW_DYNAMIC_END
}pic_render_filter_evt_t;

#endif // End for __PIC_RENDER_FILTER_H_
