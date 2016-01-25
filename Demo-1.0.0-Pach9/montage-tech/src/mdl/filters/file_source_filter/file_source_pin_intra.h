/******************************************************************************/
/******************************************************************************/
#ifndef __FILE_SRC_PIN_INTRA_H_
#define __FILE_SRC_PIN_INTRA_H_

/*!
  private data length
  */
#define FSRC_PIN_PRIVAT_DATA (1024)

/*!
  the file source pin define
  */
typedef struct tag_fsrc_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER source_pin_t m_pin;
  /*!
    private data buffer
    */
  u8 private_data[FSRC_PIN_PRIVAT_DATA];

  /*!
    set a file_name
    */
  void (*set_file_name)(handle_t _this, char *p_file_name);
  /*!
    set read from buffer
    */
  void (*set_buffer_read)(handle_t _this, u32 read_data);
  /*!
    config source pin attribute
    */
  void (*config)(handle_t _this, src_pin_attr_t *p_attr);
}fsrc_pin_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL fsrc_pin_check_parameter(void);

/*!
  create a pin

  \return return the instance of usb_pin_t
  */
fsrc_pin_t * fsrc_pin_create(fsrc_pin_t *p_fsrc_pin, interface_t *p_owner);

#endif // End for __FILE_SRC_PIN_INTRA_H_

