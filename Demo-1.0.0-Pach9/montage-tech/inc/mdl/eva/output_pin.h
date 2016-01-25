/******************************************************************************/
/******************************************************************************/
#ifndef __OUTPUT_PIN_H_
#define __OUTPUT_PIN_H_

/*!
  private data length
  */
#define BASE_OUTPUT_PIN_PRIVAT_DATA (640)

/*!
  base output pin define
  */
typedef struct tag_base_output_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER ipin_t _ipin;
  
  /*!
    private data buffer
    */
  u8 private_data[BASE_OUTPUT_PIN_PRIVAT_DATA];
  
  /*!
    connect with another pin.
    */
  PRIVATE RET_CODE (*connector)(handle_t _this, interface_t *p_peer, media_format_t *p_sample);

  /*!
    get interface imem_allocator_t. can't overload it.
    */
  imem_allocator_t * (*get_interface_allocator)(handle_t _this);

  /*!
    get interface iasync_reader_t. can't overload it.
    */
  iasync_reader_t * (*get_interface_reader)(handle_t _this);
  
  /*!
    redecide the buffer size.  can't overload it.
    sometime, the buffer need be changed. you can use this method when the pin is deactive
    */
  RET_CODE (*redecide_buffer_size)(handle_t _this, allocator_properties_t *p_pro);
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

  /*!
    decide the buffer size. Virtual.
    */
  VIRTUAL RET_CODE (*decide_buffer_size)(handle_t _this);
}base_output_pin_t;

/*!
  create output pin parameter define
  */
typedef struct tag_output_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
}output_pin_para_t;

/*!
  create a base_output_pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of base_output_pin
  */
base_output_pin_t * base_output_pin_create(
      base_output_pin_t *p_pin, output_pin_para_t *p_para);

#endif // End for __OUTPUT_PIN_H_


