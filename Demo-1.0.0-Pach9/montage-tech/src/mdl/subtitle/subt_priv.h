/******************************************************************************/
/******************************************************************************/
#ifndef __SUBT_PRIV_H__
#define __SUBT_PRIV_H__






/*!
  enable debug
  */
//#define DRV_SUBT_DEBUG
/*!
  size of pixels buffer
  */
#define SUBT_PIXEL_BUF_SIZE     1
/*!
  size of pixels buffer
  */
//#define SUBT_PIXEL_BUF_SIZE   (420 * (KBYTES))

/*!
  size of PES buffer
  */
#define SUBT_PES_BUF_SIZE       (64/2 * (KBYTES))
/*!
  size of buffer for PTI channel
  */
#define SUBT_FILTER_BUF_SIZE    (512 * 188)

/*!
  invalid osd region handle
  */
#define INVALID_REGION_HANDLE   0xff

/*!
  ABC
  */
#define SUBT_PRINTF DUMMY_PRINTF

/*!
  ABC
  */
#define SUBT_MSG_DEPTH    20

/*!
  ABC
  */
#define SUBT_MAKE_U16(high, low)    ((((u16)(high) << 8) & 0xff00) + (low))

/*!
  width
  */
#define SUBT_MAX_DISPALY_WIDTH    720
/*!
  heigth
  */
#define SUBT_MAX_DISPALY_HEIGTH   576



/*!
  PCS
  */
#define SUBT_SEGMENT_PCS          0x10
/*!
  RCS
  */
#define SUBT_SEGMENT_RCS          0x11
/*!
  CDS
  */
#define SUBT_SEGMENT_CDS          0x12
/*!
  ODS
  */
#define SUBT_SEGMENT_ODS          0x13
/*!
  DDS
  */
#define SUBT_SEGMENT_DDS          0x14
/*!
  END
  */
#define SUBT_SEGMENT_END          0x80



/*!
  mode of color
  */
typedef enum
{
    /*!
      YUV2BIT
      */
    SUBT_COLOR_YUV2BIT,
    /*!
      YUV4BIT
      */
    SUBT_COLOR_YUV4BIT,
    /*!
      YUV8BIT
      */
    SUBT_COLOR_YUV8BIT,
} subt_color_mode_t;

/*!
  definition of render
  */
typedef struct
{
    /*!
      osd device
      */
    void    *p_osd;
    /*!
      pixel buffer
      */
    u8      pixel_buf[SUBT_PIXEL_BUF_SIZE];
} subt_render_t;

/*!
  definition of osd region
  */
typedef struct
{
    /*!
      handle of region
      */
    u8      handle;
    /*!
      odd field
      */
    u32     *p_odd_addr;
    /*!
      even field
      */
    u32     *p_even_addr;
} subt_osd_region_t;

/*!
  definition of subtitle windows
  */
typedef struct
{
    /*!
      width
      */
    u16   display_width;
    /*!
      heigth
      */
    u16   display_heigth;
    /*!
      top
      */
    u16   display_top;
    /*!
      bottom
      */
    u16   display_bottom;
    /*!
      left
      */
    u16   display_left;
    /*!
      right
      */
    u16   display_right;

    /*!
      _number
      */
    u8    version_number;
    /*!
      is this window supported
      */
    BOOL  is_supported;
} subt_window_t;




/*!
  return code
  */
typedef enum
{
    /*!
      SUCCESS
      */
    SUBT_RC_SUCCESS,
    /*!
      FAILED
      */
    SUBT_RC_FAILED,
    /*!
      INVALID_DATA
      */
    SUBT_RC_INVALID_DATA,
    /*!
      PAGE_ID_ERROR
      */
    SUBT_RC_PAGE_ID_ERROR,
    /*!
      BAD_BOUNDARY
      */
    SUBT_RC_PES_BAD_BOUNDARY
} subt_rc_t;

/*!
  information of region
  */
typedef struct
{
    /*!
      id
      */
    u8    id;
    /*!
      top
      */
    u16   top;
    /*!
      left
      */
    u16   left;
} subt_region_info_t;

/*!
  information of page
  */
typedef struct
{
    /*!
      page_id
      */
    u16                 page_id;
    /*!
      timeout
      */
    u8                  page_timeout;
    /*!
      version_number
      */
    u8                  page_version_number;
    /*!
      region_number
      */
    u8                  region_number;
    /*!
      info
      */
    subt_region_info_t  *p_region_info;
} subt_page_t;

/*!
  information of object
  */
typedef struct
{
    /*!
      id
      */
    u16   id;
    /*!
      type
      */
    u8    type;
    /*!
      provider_flag;
      */
    u8    provider_flag;
    /*!
      x
      */
    u16   x_position;
    /*!
      y
      */
    u16   y_position;
    /*!
      fg_pixel_code
      */
    u8    fg_pixel_code;
    /*!
      bg_pixel_code
      */
    u8    bg_pixel_code;
} subt_object_info_t;

/*!
  definition of region
  */
typedef struct subt_region
{
    /*!
      is_updated
      */
    BOOL                is_updated;
    /*!
      page_id
      */
    u16                 page_id;
    /*!
      region id
      */
    u8                  id;
    /*!
      version_number
      */
    u8                  version_number;
    /*!
      is_region_fill
      */
    BOOL                is_region_fill;

    /*!
      width
      */
    u16                 width;
    /*!
      height
      */
    u16                 height;
    /*!
      compatibility
      */
    u8                  compatibility;
    /*!
      color depth
      */
    u8                  depth;
    /*!
      clut_id
      */
    u8                  clut_id;
    /*!
      pixel_code_8_bit
      */
    u8                  pixel_code_8_bit;
    /*!
      pixel_code_4_bit
      */
    u8                  pixel_code_4_bit;
    /*!
      pixel_code_2_bit
      */
    u8                  pixel_code_2_bit;

    /*!
      object_number
      */
    u16                 object_number;
    /*!
      info
      */
    subt_object_info_t  *p_object_info;

    /*!
      osd_region
      */
    subt_osd_region_t   osd_region;

    /*!
      next
      */
    struct subt_region  *p_next;
} subt_region_t;

/*!
  definition of CLUT
  */
typedef struct subt_clut
{
    /*!
      page_id
      */
    u16               page_id;
    /*!
      clut_id
      */
    u8                clut_id;
    /*!
      version_number
      */
    u8                version_number;
    /*!
      clut4
      */
    u32               clut4[4];
    /*!
      clut16
      */
    u32               clut16[16];
    /*!
      clut256
      */
    u32               clut256[256];

    /*!
      p_next
      */
    struct subt_clut *p_next;
} subt_clut_t;

/*!
  definition of object
  */
typedef struct subt_object
{
    /*!
      page_id
      */
    u16                 page_id;
    /*!
      id
      */
    u8                  id;
    /*!
      version_number
      */
    u8                  version_number;
    /*!
      coding_method
      */
    u8                  coding_method;
    /*!
      non_modify_colour
      */
    u8                  non_modify_colour;

    /*!
      p_next
      */
    struct subt_object  *p_next;

    /*!
      p_object_data
      */
    u8                  *p_object_data;
} subt_object_t;


/*!
  stats of subtiel module
  */
typedef enum
{
    SUBT_STATS_IDLE,
    SUBT_STATS_RUNNING,
    SUBT_STATS_PAUSE,
} subt_stats_t;

/*!
  stats of subtiel decoder
  */
typedef enum
{
    SUBT_DEC_STATS_IDLE,
    SUBT_DEC_STATS_ACQUIRING,
    SUBT_DEC_STATS_ACQUIRED
} subt_dec_stats_t;

/*!
  command
  */
typedef enum
{
    SUBT_MSG_PAGE_UPDATE,
    SUBT_MSG_MAX
} subt_msg_t;


/*!
  notify
  */
typedef u32 (*subt_notify_t)(subt_msg_t msg, u32 param1, u32 param2);


/*!
  contral block of subtitle
  */
typedef struct
{
    s32                 callback_msg_id;
    s16                 msg_cnt;
    s32                 msg_hadle;

    subt_stats_t        stats;
    BOOL                is_display;
    BOOL                is_PES_ready;
    u16                 pid;
    h_pti_channel_t     pti_channel;
    subt_dec_stats_t    dec_stats;
    u32                 pts;

    subt_window_t       window;
    subt_page_t         composition_page;
    subt_page_t         ancillary_page;
    subt_region_t       *p_region_list;
    subt_clut_t         *p_clut_list;

    subt_notify_t       p_notify_func;
    s32                 page_timeout;

    subt_render_t       *p_render;
    u8                  *p_pes_buf;
    pti_ext_buf_t       *p_filter_buf;
    video_std_t         vid_std;

} dvb_subtitle_t;








/*!
  reset DB of subtitle
  
  \param[in] p_subt handle
  */
void subt_db_reset(dvb_subtitle_t *p_subt);

/*!
  create region information in subtitle DB
  
  \param[in] num
  \param[in] p_region_info
  \param[in] p_created_num
  */
void subt_db_creat_region_info(dvb_subtitle_t *p_subt
    , u16 num, subt_region_info_t **p_region_info, u16 *p_created_num);

/*!
  delete region information in subtitle DB

  \param[in] p_region_info
  */
void subt_db_delete_region_info(dvb_subtitle_t *p_subt
    , subt_region_info_t *p_region_info);

/*!
  create object information in subtitle DB
  
  \param[in] num
  \param[in] p_object_info
  \param[in] p_created_num
  */
void subt_db_creat_object_info(dvb_subtitle_t *p_subt
    , u16 num, subt_object_info_t **p_object_info, u16 *p_created_num);

/*!
  delete object information in subtitle DB
  
  \param[in] num
  \param[in] p_region_info
  */
void subt_db_delete_object_info(dvb_subtitle_t *p_subt
    , subt_object_info_t *p_object_info);

/*!
  delete get a entity of region from subtitle DB
  
  \param[in] page_id
  \param[in] region_id
  \param[in] p_region
  */
subt_rc_t subt_db_get_region(dvb_subtitle_t *p_subt
    , u16 page_id, u8 region_id, subt_region_t **p_region);

/*!
  create a region in subtitle DB

  \param[in] p_region
  */
subt_rc_t subt_db_create_region(dvb_subtitle_t *p_subt
    , subt_region_t **p_region);

/*!
  delete a region in subtitle DB
  
  \param[in] page_id
  \param[in] region_id
  */
subt_rc_t subt_db_delete_region(dvb_subtitle_t *p_subt
    , u16 page_id, u8 region_id);


/*!
  get a clut from subtitle DB
  
  \param[in] page_id
  \param[in] clut_id
  \param[in] p_clut
  */
subt_rc_t subt_db_get_clut(dvb_subtitle_t *p_subt
    , u16 page_id, u8 clut_id, subt_clut_t **p_clut);

/*!
  create a clut in subtitle DB

  \param[in] p_clut
  */
subt_rc_t subt_db_create_clut(dvb_subtitle_t *p_subt
    , subt_clut_t **p_clut);

/*!
  delete a clut from subtitle DB
  
  \param[in] page_id
  \param[in] clut_id
  */
subt_rc_t subt_db_delete_clut(dvb_subtitle_t *p_subt, u16 page_id, u8 clut_id);






/*!
  osd init
  
  \param[in] p_render
  */
subt_rc_t subt_osd_init(subt_render_t *p_render);
/*!
  osd de-init
  
  \param[in] p_render
  */
subt_rc_t subt_osd_deinit(subt_render_t *p_render);

/*!
  fill the region
  
  \param[in] p_region
  */
subt_rc_t subt_osd_fill_region(dvb_subtitle_t *p_subt, subt_region_t *p_region);

/*!
  move the region
  
  \param[in] p_render
  \param[in] p_region
  \param[in] x
  \param[in] y
  */
subt_rc_t subt_osd_move_region(subt_render_t *p_render
    , subt_region_t *p_region, u16 x, u16 y);

/*!
  display the region
  
  \param[in] p_render
  \param[in] p_region
  \param[in] p_clut
  */
subt_rc_t subt_osd_dispaly_region(subt_render_t *p_render
    , subt_region_t *p_region, subt_clut_t *p_clut);

/*!
  display the region
  
  \param[in] p_render
  \param[in] p_region
  */
subt_rc_t subt_osd_hide_region(subt_render_t *p_render
    , subt_region_t *p_region);

/*!
  create an OSD region
  
  \param[in] p_render
  \param[in] width
  \param[in] height
  \param[in] color_mode
  \param[in] p_clut
  \param[in] p_osd_region
  */
subt_rc_t subt_osd_create_region(subt_render_t *p_render, u16 width, u16 height
    , subt_color_mode_t color_mode, const u32 *p_clut
    , subt_osd_region_t *p_osd_region);
/*!
  create OSD region

  \param[in] p_osd_region
  */
subt_rc_t subt_osd_delete_region(subt_render_t *p_render
    , subt_osd_region_t *p_osd_region);

/*!
  render one line
  
  \param[in] p_osd_region
  \param[in] x
  \param[in] y
  \param[in] width
  \param[in] p_pix_buf
  \param[in] len
  */
subt_rc_t subt_osd_copy_h_line_2bpp(dvb_subtitle_t *p_subt
    , subt_osd_region_t *p_osd_region
    , u16 x, u16 y, u16 width, u8 *p_pix_buf, u16 len);
/*!
  render one line
  
  \param[in] p_osd_region
  \param[in] x
  \param[in] y
  \param[in] width
  \param[in] p_pix_buf
  \param[in] len
  */
subt_rc_t subt_osd_copy_h_line_4bpp(dvb_subtitle_t *p_subt
    , subt_osd_region_t *p_osd_region
    , u16 x, u16 y, u16 width, u8 *p_pix_buf, u16 len);
/*!
  render one line
  
  \param[in] p_osd_region
  \param[in] x
  \param[in] y
  \param[in] width
  \param[in] p_pix_buf
  \param[in] len
  */
subt_rc_t subt_osd_copy_h_line_8bpp(dvb_subtitle_t *p_subt
    , subt_osd_region_t *p_osd_region
    , u16 x, u16 y, u16 width, u8 *p_pix_buf, u16 len);


/*!
  initialize subtitle decoder
  */
extern void subt_dec_init(void);

/*!
  set page ID
  
  \param[in] composition_page_id
  \param[in] ancillary_page_id
  */
extern subt_rc_t subt_dec_set_page_id(dvb_subtitle_t *p_subt
    , u16 composition_page_id, u16 ancillary_page_id);

/*!
  set subtile display
  
  \param[in] p_subt
  */
extern subt_rc_t subt_dec_display_page(dvb_subtitle_t *p_subt);

/*!
  set subtile hide
  
  \param[in] p_subt
  */
extern subt_rc_t subt_dec_hide_page(dvb_subtitle_t *p_subt);

/*!
  parse subtilte PES header
  
  \param[in] p_subt
  \param[in] p_pes_pkt
  \param[in] p_pts
  \param[in] p_length
  \param[in] p_segment
  */
extern subt_rc_t subt_dec_check_pes_header(dvb_subtitle_t *p_subt
    , u8 *p_pes_pkt, u32 *p_pts, u16 *p_length, u8 **p_segment);

/*!
  decoder subtitle data(PES)
  
  \param[in] p_subt
  */
extern subt_rc_t subt_dec_process(dvb_subtitle_t *p_subt, u8 *p_pes_pkt);


#endif

