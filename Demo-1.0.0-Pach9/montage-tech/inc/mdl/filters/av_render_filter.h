/******************************************************************************/
/******************************************************************************/
#ifndef __AV_RENDER_FILTER_H_
#define __AV_RENDER_FILTER_H_

/*!
  av render config
  */
typedef struct tag_av_render_config
{
  /*!
    video pid
    */
  u16 video_pid;
  /*!
    audio pid
    */
  u16 audio_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
}av_render_config_t;

/*!
  av render filter command define
  */
typedef enum tag_av_render_filter_cmd
{
  /*!
    config video/audio/pcr pid
    */
  AV_RENDER_CFG,
  /*!
    end
    */
  AV_RENDER_CFG_END
}av_render_filter_cmd_t;

/*!
  AV render filter event define
  */
typedef enum tag_av_render_evt
{
  /*!
    config read file' name
    */
  FILE_PLAY_END = AV_RENDER_FILTER << 16,
  
}av_render_evt_t;

#endif // End for __AV_RENDER_FILTER_H_
