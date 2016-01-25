/******************************************************************************/
/******************************************************************************/
#ifndef __AP_PLAYBACK_H_
#define __AP_PLAYBACK_H_

/*!
  Max message number supported in playback
  */
#define MAX_PB_MSG_NUM (32)

/*!
  command id list
  */
typedef enum
{
  /*!
    Stop play
    */
  PB_CMD_STOP = ASYNC_CMD,
  /*!
    Play a program with a program id
    */
  PB_CMD_PLAY,
  /*!
    pause playback, without parameter 
    */
  PB_CMD_PAUSE,
  /*!
    resume playback, without parameter 
    */
  PB_CMD_RESUME,
  /*!
    switch audio channel, parameter1: the new audio channel
    audio channel option, 0:audio_pid1, 1: audio_pid2, 2: audio_pid3
    */
  PB_CMD_SWITCH_AUDIO_CHANNEL,
  /*!
    Start teletext
    parameter1:	teletext preferred language
    */
  PB_CMD_START_TTX,
  /*!
    Stop teletext
    */
  PB_CMD_STOP_TTX,
  /*!
    Show teletext
    parameter1:	teletext page number, INVALID_TTX_PAGE for defaulte
    */
  PB_CMD_SHOW_TTX,
  /*!
    Send a teletext key to playback.
    parameter1: key value, pls see ttx_key_t
    */
  PB_CMD_POST_TTX_KEY,
  /*!
    select a teletext language.
    parameter1: language code
    */
  PB_CMD_SET_TTX_LANG,
  /*!
    Start VBI insertion
    */
  PB_CMD_START_VBI_INSERTER,
  /*!
    Stop VBI insertion
    */
  PB_CMD_STOP_VBI_INSERTER,
  /*!
    Start subtitle
    parameter1:	preferred language
    parameter2:	preferred subtittle type, see subt_type_t
    */
  PB_CMD_START_SUBT,
  /*!
    Stop subtitle
    */
  PB_CMD_STOP_SUBT,
  /*!
    Show subtitle
    */
  PB_CMD_SHOW_SUBT,
  /*!
    set subtitle service
    */
  PB_CMD_SET_SUBT_SERVICE,
  /*!
    reset tv mode
    */
  PB_CMD_RESET_TVMODE,
  /*!
    reset video aspect mode
    */
  PB_CMD_RESET_VIDEO_ASPECT_MODE,
  /*!
    Set Mute ,It's the first sync cmd
    */
  PB_CMD_SET_MUTE = SYNC_CMD,
  /*!
    Hide teletext
    */
  PB_CMD_HIDE_TTX,
  /*!
    hide subtitle
    */
  PB_CMD_HIDE_SUBT,
  /*!
    Set Volume
    */
  PB_CMD_SET_VOL,
  /*!
    Do stop by sync
    */
  PB_CMD_STOP_SYNC,
  /*!
    Stop subtitle by sync
    */
  PB_CMD_STOP_SUBT_SYNC,
  /*!
   flag MAX pb cmd
   */ 
  PB_CMD_END
} pb_cmd_t;

/*!
  event id list
  the event notify ui frame work, something done
  */
typedef enum
{
  /*!
    the first event of pb module,
    if create new event, you must insert it between BEGIN and END
    */
  PB_EVT_BEGIN = ((APP_PLAYBACK << 16) | ASYNC_EVT),
  /*!
    the program playing, with a parameter : pg_id
    */
  PB_EVT_PLAYING,
  /*!
    TTX ready
    */
  PB_EVT_TTX_READY,
  /*!
    SUB-TITLE ready
    */
  PB_EVT_SUB_READY,
  /*!
    SCART-VCR detected
    */
  PB_EVT_SCART_VCR_DETECTED,
  /*!
    dynamic pid found
    */
  PB_EVT_DYNAMIC_PID,
  /*!
    update version num
    */
  PB_EVT_UPDATE_VERSION,
  /*!
    update program name
    */
  PB_EVT_UPDATE_PG_NAME,
  /*!
    revise eid found
    */
  PB_EVT_REVISE_EID,
  /*!
    descramble is success.
    */
  PB_DESCRAMBL_SUCCESS,
  /*!
    descramble is failed.
    */
  PB_DESCRAMBL_FAILED,
  /*!
   find version nit is switch
    */
  PB_EVT_NIT_VERSION_SWITCH,
  /*!
    playback ready
    */
  PB_PLAYBACK_READY,    
  /*!
    the program stopped, without parameter
    */
  PB_EVT_LOCK_RSL,
   /*!
    ota upgrade new version
    */
  PB_EVT_NIT_OTA_UPGRADE,
  /*!
    the video/audio error
    */
  PB_EVT_AV_ERROR,
  /*!
    ack the sync cmd PB_CMD_SET_MUTE
    */
  PB_EVT_SET_MUTE = ((APP_PLAYBACK << 16) | SYNC_EVT),
  /*!
    ack the sync cmd PB_CMD_HIDE_TTX
    */
  PB_EVT_HIDE_TTX, 
  /*!
    ack the sync cmd PB_CMD_HIDE_SUBT
    */
  PB_EVT_HIDE_SUBT, 
  /*!
    ack the sync cmd PB_CMD_SET_VOL
    */
  PB_EVT_SET_VOL, 
  /*!
    the program stopped, without parameter
    */
  PB_EVT_STOPPED,
  /*!
    ack the sync cmd PB_CMD_STOP_SUBT_SYNC
    */
  PB_EVT_SUBT_STOPED,
  /*!
    the last event of pb module,
    if create new event, you must insert it between BEGIN and END
    */  
  PB_EVT_END
} pb_evt_t;

/*!
  playback current status information
  */
typedef enum
{
  /*!
    current program id
    */
  //u16 cur_pg_id;
  /*!
    pb is playing flag, 
    */
  //BOOL pb_playing;
  /*!
    subtitle information
    */
  //pb_subt_info_t subt_info;
  /*!
    teletext information
    */
  //pb_ttx_info_t ttx_info;
  /*!
    pb is stoping flag, 
    */
    PB_STOP = 0,
  /*!
    pb lock end flag, 
    */
    PB_LOCK_END,
   /*!
    pb is playing flag, 
    */
    PB_PLAYING,
} pb_status_t;

/*!
  Option of stop playback.do some especial operation before stop play
  */
typedef enum 
{
  /*!
    freeze video before stop play
    */
  STOP_PLAY_FREEZE = 0,
  /*!
    show black screen before stop play
    */
  STOP_PLAY_BLACK = 1,
  /*!
    don't any especial operation before stop play
    */
  STOP_PLAY_NONE
} stop_mode_t;

/*!
  Parameter inner union for PB_CMD_PLAY
  */
typedef union tag_play_param_inner_t
{
  /*!
    DVBS lock info
    */
  dvbs_lock_info_t dvbs_lock_info;
  /*!
    DVBC lock info
    */
  dvbc_lock_info_t dvbc_lock_info;
}play_param_inner_t;

/*!
  Parameter for PB_CMD_PLAY
  */
typedef struct
{
  /*!
    Lock mode
    */
  sys_signal_t lock_mode;
  /*!
    Lock info
    */
  play_param_inner_t inner;
  /*!
    DVBS program info
    */
  dvbs_program_t pg_info;
}play_param_t;

/*!
  Playback instance policy interface declaration
  */
typedef struct 
{
  /*!
    \see _play
    */
  void (*on_play)(void *p_data, play_param_t *p_play_param);
    /*!
    \see _check
    */
  void (*check_video)(play_param_t play_param);
  /*!
    \see _process_pmt_info
    */
  BOOL (*process_pmt_info)(void *p_data, pmt_t *p_pmt, 
          u16 *p_v_pid, u16 *p_pcr_pid, u16 *p_a_pid, u16 *p_a_type);
  /*!
    \see _process_sdt_info
    */
  BOOL (*process_sdt_info)(void *p_data, sdt_t *p_sdt);
  /*!
      \see _process_nit_info
    */
  BOOL (*process_nit_info)(void *p_data, nit_t *p_nit);
  /*!
    \see _process_sdt_info
    */
  BOOL (*monitor_scart)(void *p_data);
  /*!
    playback policy private data
    */
  void *p_data;
} pb_policy_t;

/*!
  Call this method, load playback instance by the policy.
  
  \param[in] p_policy The policy of application playback
  \return Return app_t instance address
  */
app_t *construct_ap_playback(pb_policy_t *p_policy); 

/*!
  Call this method, load playback instance by the policy.
  
  \param[in] p_policy The policy of application playback
  \return Return app_t instance address
  */
app_t *construct_ap_playback_1(pb_policy_t *p_policy); 
/*!
  Call this method, load playback instance by the policy.
  
  \param[in] p_policy The policy of application playback
  \return Return app_t instance address
  */
app_t *construct_ap_playback_16(pb_policy_t *p_policy); 

#endif // End for __AP_PLAYBACK_H_
