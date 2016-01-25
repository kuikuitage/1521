/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_RENDER_FILTER_INTRA_H_
#define __PIC_RENDER_FILTER_INTRA_H_

/*!
  private data length
  */
#define PIC_RENDER_FILTER_PRIVAT_DATA (4 * 1024)

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_pic_render_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_filter_t _sink_filter;
  /*!
    private data buffer
    */
  u8 private_data[PIC_RENDER_FILTER_PRIVAT_DATA];
}pic_render_filter_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL pic_render_filter_check_parameter(void);

/*!
  create a video pic_render filter instance

  \return return the instance of filter
  */
ifilter_t * pic_render_filter_create(void);

#endif // End for __PIC_RENDER_FILTER_INTRA_H_
