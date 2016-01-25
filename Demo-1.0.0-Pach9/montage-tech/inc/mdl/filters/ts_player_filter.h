/******************************************************************************/
/******************************************************************************/
#ifndef __TS_PLAYER_FILTER_H_
#define __TS_PLAYER_FILTER_H_

/*!
  ts player play mode
  */
typedef enum tag_ts_player_play_mode
{
  /*!
    normal play
    */
  NORMAL_PLAY = 0,
  /*!
    fast forward
    */
  FAST_FORWARD,
  /*!
    fast backward
    */
  FAST_BACKWARD,
  /*!
    time seek forward
    */
  TIME_SEEK_FORWARD,
  /*!
    time seek backward
    */
  TIME_SEEK_BACKWARD,
}ts_play_mode_t;

/*!
  ts player config
  */
typedef struct tag_ts_player_config
{
  /*!
    play mode
    */
  ts_play_mode_t play_mode;
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
  /*!
    is rec file
    */
  u16 is_rec_file;
  /*!
    played time
    */
  u32 played_time;
}ts_player_config_t;

/*!
  ts player filter command define
  */
typedef enum tag_ts_player_filter_cmd
{
  /*!
    config video pid...
    */
  TS_PLAYER_CFG,
  /*!
    end
    */
  TS_PLAYER_CFG_END
}ts_player_filter_cmd_t;

/*!
  ts player filter event define
  */
typedef enum tag_ts_player_filter_evt
{
  /*!
    play mode auto changed
    */
  PLAY_MODE_CHANGED = TS_PLAYER_FILTER << 16,
  
}ts_player_filter_evt_t;

#endif // End for __TS_PLAYER_FILTER_H_

