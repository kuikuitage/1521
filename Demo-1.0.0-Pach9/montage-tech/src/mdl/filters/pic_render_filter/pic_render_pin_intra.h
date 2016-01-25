/******************************************************************************/
/******************************************************************************/
#ifndef __PIC_RENDER_PIN_INTRA_H_
#define __PIC_RENDER_PIN_INTRA_H_

/*!
  private data length
  */
#define PIC_RENDER_PIN_PRIVAT_DATA (256)

/*!
  the input pin
  */
typedef struct tag_pic_render_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data buffer
    */
  u8 private_data[PIC_RENDER_PIN_PRIVAT_DATA];
}pic_render_pin_t;

/*!
  check parameter state

  \return return the para is ok
  */
BOOL pic_render_pin_check_parameter(void);

/*!
  cfg display offset x

  \param[in] p_pin    pin
  \param[in] offsetx  offsetx
  */
void pic_render_cfg_rect(pic_render_pin_t *p_pin, void *p_rect);

/*!
  cfg display rgn

  \param[in] p_pin    pin
  \param[in] p_rgn    region
  */
void pic_render_cfg_rgn(pic_render_pin_t *p_pin, void *p_rgn);

/*!
  cfg display rgn

  \param[in] p_pin    pin
  \param[in] p_rgn    region
  */
void pic_render_cfg_clr_key(pic_render_pin_t *p_pin, void *p_clr_cfg);
	
/*!
  create a pin

  \return return the instance of video_pic_render_pin_t
  */
pic_render_pin_t * pic_render_pin_create(pic_render_pin_t *p_pin, interface_t *p_owner);

#endif // End for __PIC_RENDER_PIN_INTRA_H_
