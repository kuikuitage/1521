/******************************************************************************/
/******************************************************************************/
#ifndef __VBI_INSERTER_PRIV_H__
#define __VBI_INSERTER_PRIV_H__





/*!
  enable debug
  */
//#define DRV_VBI_DEBUG
#ifdef DRV_VBI_DEBUG
/*!
  ABC
  */
#define VBI_PRINTF    OS_PRINTF
#else
#ifdef WIN32
/*!
  ABC
  */
#define VBI_PRINTF
#else
/*!
  ABC
  */
#define VBI_PRINTF  DUMMY_PRINTF
#endif
#endif

/*!
  ABC
  */
#define VBI_FIFO_BUF_SIZE           (64*1024)
/*!
  ABC
  */
#define PTS_OFFSET_PER_FIELD        1500
/*!
  ABC
  */
#define VBI_UNUSED_PARAM(x)         ((void)(x))
/*!
  ABC
  */
#define VBI_MAKE_U16(high, low)     ((((u16)(high) << 8) & 0xff00) + (low))




/*!
    according to hardware
  */
typedef enum
{
    VBI_DATA_TYPE_UNDEF,
    VBI_DATA_TYPE_TTX,
    VBI_DATA_TYPE_VPS,
    VBI_DATA_TYPE_WSS,
    VBI_DATA_TYPE_CC,
    VBI_DATA_TYPE_MONOCHROME
} vbi_data_type_t;

/*!
    data of WSS
  */
typedef struct
{
    vbi_data_type_t type;
    u32             pts;
    u8              field_parity;
    u8              data_block[2];
} wss_data_t;

/*!
    data of CC
  */
typedef struct
{
    vbi_data_type_t type;
    u32             pts;
    u8              field_parity;
    u8              data_block[2];
} cc_data_t;

/*!
    data of TTX
  */
typedef struct
{
    vbi_data_type_t type;
    u32             pts;
    u8              field_parity;
    u8              line_offset;
    /*!
      first byte = framing_code
    */
    u32             data_block[11];
} ttx_data_t;

/*!
    data of VBI
  */
typedef union
{
    vbi_data_type_t type;
    wss_data_t      wss;
    cc_data_t       cc;
    ttx_data_t      ttx;
} vbi_data_t;



/*!
  ABC
  */
#define INVALID_BUF_INDEX        (0x7fff)


/*!
  ABC
  */
#define CC_BUF_SIZE             (1*8)
/*!
  ABC
  */
#define CC_BUF_CRITICAL_SIZE    (1*6)

/*!
  ABC
  */
#define TTX_BUF_SIZE            (16*100)
/*!
  ABC
  */
#define TTX_BUF_CRITICAL_SIZE   (16*2)

/*!
  ABC
  */
#define WSS_BUF_SIZE            (1*8)
/*!
  ABC
  */
#define WSS_BUF_CRITICAL_SIZE   (1*6)



/*!
  ABC
  */
#define CC_NEXT_BUF_INDEX(a)    ((a)+1 >= (CC_BUF_SIZE) ? 0 : (a)+1)
/*!
  ABC
  */
#define CC_PREV_BUF_INDEX(a)    ((a)-1 > 0 ? (a)-1 : (CC_BUF_SIZE)-1)

/*!
  ABC
  */
#define TTX_NEXT_BUF_INDEX(a)   ((a)+1 >= (TTX_BUF_SIZE) ? 0 : (a)+1)
/*!
  ABC
  */
#define TTX_PREV_BUF_INDEX(a)   ((a)-1 > 0 ? (a)-1 : (TTX_BUF_SIZE-1))

/*!
  ABC
  */
#define WSS_NEXT_BUF_INDEX(a)   ((a)+1 >= (WSS_BUF_SIZE) ? 0 : (a)+1)
/*!
  ABC
  */
#define WSS_PREV_BUF_INDEX(a)   ((a)-1 > 0 ? (a)-1 : (WSS_BUF_SIZE)-1)



/*!
  ABC
  */
#define CC_SET_LINE_ACTIVE(a, line)     ((a) |= (u16)((u16)0x1 << ((line) - 7)))
/*!
  ABC
  */
#define CC_SET_LINE_DEACTIVE(a, line)   \
    ((a) &= (u16)(~((u16)0x1 << ((line) - 7))))
/*!
  ABC
  */
#define CC_GET_LINE_ACTIVE(a, line)     ((a) & ((u16)0x1 << ((line) - 7)))

/*!
  ABC
  */
#define TTX_SET_LINE_ACTIVE(a, line)    ((a) |= (u16)((u16)0x1 << ((line) - 7)))
/*!
  ABC
  */
#define TTX_SET_LINE_DEACTIVE(a, line)  \
    ((a) &= (u16)(~((u16)0x1 << ((line) - 7))))
/*!
  ABC
  */
#define TTX_GET_LINE_ACTIVE(a, line)    ((a) & ((u16)0x1 << ((line) - 7)))

/*!
  ABC
  */
#define WSS_SET_LINE_ACTIVE(a, line)    \
    ((a) |= (u16)((u16)0x1 << ((line) - 8)))
/*!
  ABC
  */
#define WSS_SET_LINE_DEACTIVE(a, line)  \
    ((a) &= (u16)(~((u16)0x1 << ((line) - 8))))
/*!
  ABC
  */
#define WSS_GET_LINE_ACTIVE(a, line)    ((a) & ((u16)0x1 << ((line) - 8)))



/*!
  ABC
  */
#define VBI_STATE_INSERTION_INITIALIED  ((u32)1 << 0)
/*!
  ABC
  */
#define VBI_STATE_INSERTION_RUNING      ((u32)1 << 1)


/*!
  ABC
  */
typedef struct
{
    BOOL        is_running;
    u32         cfg;
    video_std_t vid_std;

    u8          data_buf[VBI_FIFO_BUF_SIZE];
    vbi_fifo_t  fifo;
} vbi_inserter_cb_t;




/*!
  get STC
  */
u32 vbi_get_stc(void);



/*!
  VBI Inserter
  */
typedef struct s_vbi_inserter
{
//priv:
    /*!
      priv
      */
    void *p_priv;

//public:
    /*!
      start inserte
      */
    RET_CODE (*p_start)(void *p_priv, u32 cfg);

    /*!
      stop inserte
      */
    RET_CODE (*p_stop)(void *p_priv, u32 cfg);

    /*!
      set current video standard
      */
    RET_CODE (*p_set_vid_std)(void *p_priv, video_std_t std);

    /*!
      set current video standard
      */
    RET_CODE (*p_inserte_data)(void *p_priv, u32 pts, u8 *p_pes_data_filed
        , u8 *p_pes_last_byte);

}  vbi_inserter_t;




#endif

