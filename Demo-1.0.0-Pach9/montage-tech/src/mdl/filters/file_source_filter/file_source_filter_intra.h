/******************************************************************************/
/******************************************************************************/
#ifndef __FILE_SRC_FILTER_INTRA_H_
#define __FILE_SRC_FILTER_INTRA_H_

/*!
  private data length
  */
#define FSRC_FILTER_PRIVAT_DATA (3*1024)

/*!
  the file source filter define
  */
typedef struct tag_fsrc_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER src_filter_t m_filter;
  /*!
    private data buffer
    */
  u8 private_data[FSRC_FILTER_PRIVAT_DATA];
}fsrc_filter_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL fsrc_filter_check_parameter(void);

/*!
  create a file source instance
  
  \return return the instance of fsrc_filter
  */
ifilter_t * fsrc_filter_create(void);

#endif // End for __FILE_SRC_FILTER_INTRA_H_

