/******************************************************************************/
/******************************************************************************/
#ifndef __MP3_PLAYER_FILTER_H_
#define __MP3_PLAYER_FILTER_H_

/*!
Task priority
*/
typedef enum
{
  MP3_LYRCIS_TASK_PRIORITY = 27,        //need to be fixed
}task_priority_t;

/*!
  lyrcis task size
  */
#define MP3_LYRCIS_TASK_STKSIZE        ((4) * (KBYTES))

/*!
  lyrics length
  */
#define LRCLEN sizeof(mp3_lrc_filter_t)
/*!
  the mp3_lrc filter define
  */
typedef struct tag_mp3_lrc_filter
{  
/*!
  lyric time
  */
  u32 time;
/*!
  lyrics
  */
  u8 lyric[1024];
/*!
  next lyrics 
  */
  struct tag_mp3_lrc_filter *p_next;
}mp3_lrc_filter_t;

/*!
  forward speed
  */
#define FORWARDSPEED 2

/*!
  max count
  */
#define CNTMAX 256

/*!
  size per request
  */
#define SIZEREQ (50 * 1024)

/*! 
  mp3 play mode 
  */
typedef enum tag_mp3_player_play_mode
{
  /*!
    normal play
    */
  MP3_NORMAL_PLAY = 0,
  /*!
    fast forward
    */
  MP3_FAST_FORWARD,
  /*!
    fast rewind
    */  
  MP3_FAST_BACKWARD,
  /*!
    time seek
    */
  MP3_TIME_SEEK,
  /*!
    play resume
    */
  MP3_PLAY_RESUME,
  /*!
    play pause
    */
  MP3_PLAY_PAUSE,
}mp3_play_mode_t;

/*!
  mp3 player filter command define
  */
typedef enum tag_mp3_player_filter_cmd
{
  /*!
    config ...
    */
  MP3_PLAYER_CFG,
  /*!
    end
    */
  MP3_PLAYER_CFG_END,
}mp3_player_filter_cmd_t;


#endif // End for __MP3_PLAYER_FILTER_H_

