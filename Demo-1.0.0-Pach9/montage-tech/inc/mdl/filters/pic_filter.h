/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_FILTER_H_
#define __PIC_FILTER_H_


typedef struct
{
  u32 w;
  u32 h;
  u32 flip;
  u32 out_fmt;
  u32 dec_mode;
  u32 input_buf_size;
  u8 *p_input_buf;
  u32 out_buf_size;
  u8 *p_out_buf;
}pic_filter_cfg_t;

/*!
  jpeg filter command define
  */
typedef enum tag_pic_filter_cmd
{
  /*!
    config all the below 
    */
  PIC_FILTER_CFG,
  /*!
    scale
    */
  PIC_CFG_SIZE,
  /*!
    flip
    */
  PIC_CFG_FLIP,
  /*!
    out format
    */
  PIC_CFG_OUT_FMT,
  /*!
    input buffer
    */
  PIC_CFG_INPUT_BUFFER,
  /*!
    output buffer
    */
  PIC_CFG_OUTPUT_BUFFER,
  /*!
    pdec mode
    */
  PIC_CFG_DEC_MODE
}pic_filter_cmd_t;

/*!
  flip
  */
typedef enum tag_pic_flip
{
  /*!
    PIC_FLIP_0
    */
    PIC_FLIP_0 = 0,  
  /*!
    PIC_FLIP_90
    */
    PIC_FLIP_90,
  /*!
    PIC_FLIP_180
    */
    PIC_FLIP_180,
  /*!
    PIC_FLIP_270
    */
    PIC_FLIP_270,    
}pic_flip_t;


/*!
  out format
  */
typedef enum tag_pic_outfmt
{
  /*!
    PIC_OUTFMT_422
    */
    PIC_OUTFMT_422 = 0,  
  /*!
    JPEG_OUTFMT_444
    */
    JPEG_OUTFMT_444,
}pic_outfmt_t;

/*!
  scale  dest: src
 */
typedef enum tag_pic_scale
{
  /*!
    JPEG_SCALE W1:1 H1:1
    */
    JPEG_SCALE_W11_H11 = 0,
  /*!
    JPEG_SCALE W1:2 H1:2
    */
    JPEG_SCALE_W12_H12 = 1,
  /*!
    JPEG_SCALE W1:2 H1:4
    */
    JPEG_SCALE_W12_H14 = 2,
  /*!
    JPEG_SCALE W1:4 H1:2
    */
    JPEG_SCALE_W14_H12 = 3,
  /*!
    JPEG_SCALE W1:4 H1:4
    */
    JPEG_SCALE_W14_H14 = 4,
  /*!
    JPEG_SCALE W1:27 H1:1
    */
    JPEG_SCALE_W127_H11 = 5,
  /*!
    JPEG_SCALE W1:1 H27:20
    */
    JPEG_SCALE_W11_H2720 = 6,
  /*!
    JPEG_SCALE W27:20 H1:1
    */
    JPEG_SCALE_W2720_H11 = 7,
  /*!
    JPEG_SCALE W1:2 H1:1
    */
    JPEG_SCALE_W12_H11 = 8,
  /*!
    JPEG_SCALE W1:2 H1:27
    */
    JPEG_SCALE_W11_H127 = 9,
  /*!
    JPEG_SCALE W1:1 H1:2
    */
    JPEG_SCALE_W11_H12 = 10,
  /*!
    JPEG_SCALE W1024:515 H512:257
    */
    JPEG_SCALE_W1024515_H512257 = 11,
  /*!
    JPEG_SCALE W1:3 H1:3
    */
    JPEG_SCALE_W13_H13 = 12,
  /*!
    JPEG_SCALE W1:9 H1:9
    */
    JPEG_SCALE_W19_H19 = 13,
  /*!
    JPEG_SCALE ERROR
    */    
    JPEG_SCALE_ERROR = 14,    
}pic_scale_t;

#endif // End for __PIC_FILTER_H_
