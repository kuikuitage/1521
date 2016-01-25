/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_FILTER_INTRA_H_
#define __PIC_FILTER_INTRA_H_

/*!
  private data length
  */
#define PIC_FILTER_PRIVAT_DATA (8*1024)

/*!
  the pic filter define
  */
typedef struct tag_pic_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  u8 private_data[PIC_FILTER_PRIVAT_DATA];
}pic_filter_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL pic_filter_check_parameter(void);

/*!
  create a file source instance
  
  \return return the instance of pic_filter_t
  */
ifilter_t * pic_filter_create(void);

#endif // End for __PIC_FILTER_INTRA_H_

