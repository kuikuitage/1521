/******************************************************************************/
/******************************************************************************/

#ifndef __TTX_RENDER_H__
#define __TTX_RENDER_H__



/*!
  Index of background color in CLUT
  */
#define SCREEM_COLOR_INDEX      0xff




/*!
  ABC
  */
typedef struct ttx_render
{
    void           *p_osd;
    video_std_t     video_std;
    u32             page_x;
    u32             page_y;
    u32             page_w;
    u32             page_h;
    u8              osd_hdl;
    u8              sub_hdl;
    u32             *p_odd_addr;
    u32             *p_even_addr;
    u32             region_w;
    u32             region_h;
    u32             clut[256];
} ttx_render_t;


/*!
  draw a page
  
  \param[in] p_osd_page
  \param[in] first_row
  \param[in] first_column
  \param[in] last_row
  \param[in] last_column
  \param[in] conceal
  \param[in] flash_on
  */
extern void ttx_render_draw_page(ttx_render_t *p_render, BOOL subtile
    , ttx_osd_page_t *p_osd_page
    , u8 first_row, u8 last_row
    , u8 first_column, u8 last_column
    , BOOL conceal, BOOL flash_on);

/*!
  draw the page number
  
  \param[in] page_no
  \param[in] sub_no
  */
extern void ttx_render_draw_page_no(ttx_render_t *p_region
    , u16 page_no, u16 sub_no, BOOL b_dec);

/*!
  set transparent of background
  
  \param[in] p_osd_page
  \param[in] percent
  */
void ttx_render_set_bg_transparent(ttx_render_t *p_render
    , ttx_osd_page_t *p_osd_page, u8 percent);

/*!
  create a OSD region
  
  \param[in] p_render
  */
extern vbi_rc_t ttx_render_create_region(ttx_render_t *p_render
    , video_std_t std);
/*!
  delete a OSD region
  
  \param[in] p_render
  */
extern vbi_rc_t ttx_render_delete_region(ttx_render_t *p_render);

/*!
  create a SUB region
  
  \param[in] p_render
  */
extern vbi_rc_t ttx_render_create_sub_region(ttx_render_t *p_render
    , video_std_t std);
/*!
  delete a SUB region
  
  \param[in] p_render
  */
extern vbi_rc_t ttx_render_delete_sub_region(ttx_render_t *p_render);


/*!
  clear page on screem
  
  \param[in] color
  */
extern void ttx_render_clear_page(ttx_render_t *p_region, u8 color);

/*!
  clear header on screem
  
  \param[in] color
  */
extern void ttx_render_clear_header(ttx_render_t *p_render, u8 color);

#endif

