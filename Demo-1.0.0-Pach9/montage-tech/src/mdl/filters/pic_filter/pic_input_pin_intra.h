/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_IN_PIN_INTRA_H_
#define __PIC_IN_PIN_INTRA_H_

/*!
  private data length
  */
#define PIC_IN_PIN_PRIVAT_DATA (1024)

/*!
  the pic input pin
  */
typedef struct tag_pic_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data buffer
    */
  u8 private_data[PIC_IN_PIN_PRIVAT_DATA];
}pic_in_pin_t;


/*!
  check parameter state

  \return return the para is ok
  */
BOOL pic_in_pin_check_parameter(void);

/*!
  create a pin

  \return return the instance of pic_in_pin_t
  */
pic_in_pin_t * pic_in_pin_create(pic_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __PIC_IN_PIN_INTRA_H_
