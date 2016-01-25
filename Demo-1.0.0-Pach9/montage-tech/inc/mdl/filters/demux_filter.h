/******************************************************************************/
/******************************************************************************/
#ifndef __DEMUX_FILTER_H_
#define __DEMUX_FILTER_H_


/*!
  the record buffer config parameter
  */
typedef struct tag_rec_buf_cfg
{
  /*!
    record buffer
    */
  u8 *p_rec_buffer;
  /*!
    total buffer size
    */
  u32 total_buf_len;
  /*!
    atom size, depends on hardware
    */
  u32 atom_size;
}rec_buf_cfg_t;

/*!
  demux pin type define
  */
typedef enum tag_dmx_pin_type
{
  /*!
    default type, invalid
    */
  DMX_PIN_UNKNOWN_TYPE,
  /*!
    psi type, include section and ts packet
    */
  DMX_PIN_PSI_TYPE,
  /*!
    record type
    */
  DMX_PIN_REC_TYPE
}dmx_pin_type_t;

/*!
  demux filter command define
  */
typedef enum tag_dmx_filter_cmd
{
  /*!
    set pin type
    */
  DMX_CFG_PIN_TYPE,
  /*!
    condif pid
    */
  DMX_MAP_PID,
  /*!
    clear pid
    */
  DMX_CLEAR_PID,
  /*!
    set parameter
    */
  DMX_CFG_PARA,
}dmx_filter_cmd_t;

#endif // End for __DEMUX_FILTER_H_

