/******************************************************************************/
/******************************************************************************/
#ifndef __MEDIA_FORMAT_DEF_H_
#define __MEDIA_FORMAT_DEF_H_

/*!
  private data length
  */
#define MAX_SUP_MEDIA (20)

/*!
  stream type define
  */
typedef enum
{
  /*!
    unknown type
    */
  MT_UNKNOWN = 0,
  /*!
    PSI type
    */
  MT_PSI,
  /*!
    Video type
    */
  MT_VIDEO,
  /*!
    Audio type
    */
  MT_AUDIO,
}stream_type_t;

/*!
  media sub type define
  */
typedef enum
{
  /*!
    unknown type
    */
  ST_UNKNOWN = 0,
  /*!
    mp3 type
    */
  ST_STREAM_MP3,
  /*!
    video 442 type
    */
  ST_VIDEO_422,
  /*!
    video h264 type
    */
  ST_VIDEO_H264,
  /*!
    audio pcm type
    */
  ST_AUDIO_PCM = 201,
  /*!
    audio ac3 type
    */
  ST_AUDIO_AC3,
}media_sub_type_t;

/*!
  media sub type define
  */
typedef enum
{
  /*!
    unknown type
    */
  MF_UNKNOWN = 0,
  /*!
    video type
    */
  MF_VIDEO,
  /*!
    audio type
    */
  MF_AUDIO,
  /*!
    jpeg type
    */
  MF_JPEG,
  /*!
    mp3 type
    */
  MF_MP3,
}media_type_t;

/*!
  media format define
  */
typedef struct tag_media_format
{
  /*!
    \see stream_type_t
    */
  stream_type_t stream_type;
  /*!
    \see media_type_t
    */
  media_type_t media_type;
  /*!
    \see media_sub_type_t
    */
  media_sub_type_t sub_type;
}media_format_t;


/*!
  media sub type define
  */
typedef enum tag_sample_state
{
  /*!
    unknown state
    */
  SAMP_STATE_UNKNOWN = 0,
  /*!
    sample is good
    */
  SAMP_STATE_GOOD,
  /*!
    sample fail
    */
  SAMP_STATE_FAIL,
    /*!
    sample is insufficient
    */
  SAMP_STATE_INSUFFICIENT,
  /*!
    sample timeout
    */
  SAMP_STATE_TIMEOUT,
}sample_state_t;

/*!
  media sample define
  */
typedef struct tag_media_sample
{
  /*!
    \see media_format_t
    */
  media_format_t format;
  /*!
    buffer's state
    */
  sample_state_t state;
  /*!
    total buffer size
    */
  u32 total_buffer_size;
  /*!
    offset
    */
  u32 data_offset;
  /*!
    sample size
    */
  u32 payload;
  /*!
    sample buffer
    */
  u8 *p_data;
  /*!
    this media sample's owner, NOTE: the field is read only. can't modify it!!
    */
  void *p_owner;
  /*!
    sample context, using to pull mode, decide by the user.
    */
  u32 context;
  /*!
    sample file name.
  */
  u8 *filename;
}media_sample_t;

/*!
  enum media sample define
  */
typedef struct tag_list_media_format
{
  /*!
    media format list
    */
  media_format_t list[MAX_SUP_MEDIA];
  /*!
    total num
    */
  u32 num;
}list_media_format_t;

#endif // End for __MEDIA_FORMAT_DEF_H_

