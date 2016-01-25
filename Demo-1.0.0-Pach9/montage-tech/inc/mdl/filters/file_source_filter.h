/******************************************************************************/
/******************************************************************************/
#ifndef __FILE_SRC_FILTER_H_
#define __FILE_SRC_FILTER_H_

/*!
  define source pin attribute
  */
typedef struct tag_src_pin_attr
{
  /*!
    is circular read, default to no circular
    */
  BOOL is_circular;
  /*!
    is share read, default to engross
    */
  BOOL is_share;
  /*!
    Number of buffers created. default to 1, ignore 0.
    */
  u32 buffers;
  /*!
    Size of each buffer in bytes. default to KBYTE, ignore 0.
    */
  u32 buffer_size;
}src_pin_attr_t;

/*!
  file source filter command define
  */
typedef enum tag_fsrc_filter_cmd
{
  /*!
    config read file' name
    */
  FSRC_CFG_FILE_NAME,
  /*!
    config source pin attribute //see src_pin_attr_t
    */
  FSRC_CFG_SOURCE_PIN,
  
}fsrc_filter_cmd_t;

/*!
  file source filter event define
  */
typedef enum tag_fsrc_filter_evt
{
  /*!
    config read file' name
    */
  FILE_READ_END = FILE_SOURCE_FILTER << 16,
  
}fsrc_filter_evt_t;


#endif // End for __FILE_SRC_FILTER_H_

