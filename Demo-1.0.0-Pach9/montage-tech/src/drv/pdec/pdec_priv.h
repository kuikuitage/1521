/******************************************************************************/
/******************************************************************************/
#ifndef __PDEC_PRIV_H__
#define __PDEC_PRIV_H__

/*!
  The decoding state
  */
typedef enum
{
  /*!
    Picture decoding is running.
    */
  PIC_DEC_RUNNING,
  /*!
    Picture decoding is done with errors.
    */
  PIC_DEC_ERROR,
  /*!
    Picture decoding is done successfully
    */
  PIC_DEC_DONE
}pic_dec_state_t;

/*!
  The supported picture formats
  */
typedef enum
{
  /*!
    The input is bmp picture
    */
  PIC_BMP,
  /*!
    The input is JPEG picture
    */
  PIC_JPEG,
  /*!
    Unknown picture format.
    */
  PIC_UNKNOWN    
}pic_fmt_t;

/*!
  The descriptor of decoding JPEG picture
  */
typedef struct
{
  /*!
    Width of the JPEG picture
    */
  u32 width;
  /*!
    Height of the JPEG picture
    */
  u32 height;
  /*!
    The decoding state
    */
  pic_dec_state_t state;
  /*!
    The input picture format
    */
  pic_fmt_t fmt;
} pic_dec_info_t;

/*!
  The supported rotation modes. Always anticlockwise!
  */
typedef enum
{
  /*!
    Keep original direction
    */
  PIC_ROTATE_NONE,
  /*!
    Anticlockwise rotation for 90 degree
    */
  PIC_ROTATE_90,
  /*!
    Anticlockwise rotation for 180 degree
    */  
  PIC_ROTATE_180,
  /*!
    Anticlockwise rotation for 270 degree
    */  
  PIC_ROTATE_270
} pic_rotate_t;

/*!
  The supported flip modes
  */
typedef enum
{
  /*!
    Keep original direction
    */
  PIC_FLIP_NONE,
  /*!
    Flip vertically
    */  
  PIC_FLIP_V,
  /*!
    Flip horizontally
    */  
  PIC_FLIP_H
} pic_flip_t;

/*!
  This structure defines decoding settings.
  */
typedef struct
{
  /*!
    The input picture format
    */
  pic_fmt_t src_fmt;
  /*!
    The buffer address of input picture.
    */
  void *p_src;  
  /*!
    The size of input picture
    */
  u32 src_size;
  /*!
    The pointer to the handle of target region. 
    It defines the output format(422/444), display rectangle & frame buffer, etc.
    */
  void *p_dst;
  /*!
    Rotation setup
    */
  pic_rotate_t rotation;
  /*!
    Flip setup
    */  
  pic_flip_t flip;
} pic_cfg_t;

/*!
    ...
  */
typedef struct lld_pic
{
  /*!
      The private data of low level driver.
    */
  void *p_priv;
  /*!
      Jpeg start  
  */
  RET_CODE (*start_decode)(void *p_lld,void * ptr);
  /*!
      Jpeg stop  
  */
  RET_CODE (*stop)(void *p_lld,void * ptr);
  /*!
      Get jpeg info  
  */
  RET_CODE (*get_inf)(void *p_lld, unsigned char *p_data, 
    unsigned int data_size, pic_info_t *p_cur_info, void ** ptr);
  /*!
      Set decode param
  */
  RET_CODE (*set_inf)(void *p_lld, pic_param_t *p_info,void * ptr);

  /*!
      Get line data
  */
  RET_CODE (*get_line)(void *p_lld, unsigned char *p_dstline, unsigned int * p_line_num,void * ptr);
} lld_pic_t;

#endif

