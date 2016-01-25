/******************************************************************************/
/******************************************************************************/
#ifndef __EVA_FILTER_FAC_H_
#define __EVA_FILTER_FAC_H_

/*!
  filter id define
  */
typedef enum tag_eva_filter_id
{
  /*!
    undefined filter id,
    */
  UNKNOWN_FILTER_ID = 0x200,  //follow the APP_LAST //see it ap_framwork.h
  /*!
    file source filter
    */
  FILE_SOURCE_FILTER,
  /*!
    file sink filter
    */
  FILE_SINK_FILTER,
  /*!
    recording filter
    */
  RECORD_FILTER,
  /*!
    file sink filter
    */
  MP3_DECODE_FILTER,
  /*!
    record filter
    */
  MP3_TRANSFER_FILTER,
  /*!
    demux filter
    */
  DEMUX_FILTER,
  /*!
    ts player filter
    */
  TS_PLAYER_FILTER,
  /*!
    video render filter
    */
  AV_RENDER_FILTER,
  /*!
    jpeg filter
    */
  JPEG_FILTER,
  /*!
    pic render filter
    */
  PIC_RENDER_FILTER,    
  //new filters ...
  
  /*!
    max filter id flag, when you need add a filter id, insert it above
    */
  FILTER_MAX_ID
}filter_id_t;

/*!
  add filter function
  */
RET_CODE eva_add_filter_by_id(filter_id_t id, ifilter_t **pp_filter);

#endif // End for __EVA_FILTER_FAC_H_

