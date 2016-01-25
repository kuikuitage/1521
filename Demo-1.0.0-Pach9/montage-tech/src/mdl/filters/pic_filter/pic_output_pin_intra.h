/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_OUT_PIN_INTRA_H_
#define __PIC_OUT_PIN_INTRA_H_

/*!
  private data length
  */
#define PIC_OUT_PIN_PRIVAT_DATA (1024)

/*!
  the record output pin define
  */
typedef struct tag_pic_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data buffer
    */
  u8 private_data[PIC_OUT_PIN_PRIVAT_DATA];
}pic_out_pin_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL pic_out_pin_check_parameter(void);

/*!
  set properties.

  \param[in] p_output_pin   output pin.
  \param[in] buffer_size    buffer size.
  \param[in] p_buffer       buffer.
  */
void pic_out_pin_cfg(pic_out_pin_t *p_output_pin, u32 buffer_size, void *p_buffer);

/*!
  create a pin

  \return return the instance of pic_out_pin_t
  */
pic_out_pin_t * pic_out_pin_create(pic_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __PIC_OUT_PIN_INTRA_H_
