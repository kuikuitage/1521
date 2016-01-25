/******************************************************************************/
/******************************************************************************/

#ifndef __AUDIO_H__
#define __AUDIO_H__



/*!
  This structure defines audio commands by dev_io_ctrl. 
  */
typedef enum
{
  /*!
    Set the audio channel.
    */
  AUDIO_CMD_SET_CHANNEL = DEV_IOCTRL_TYPE_UNLOCK + 0,
  /*!
    Enable/disable mute state
    */
  AUDIO_CMD_SET_MUTE,
  /*!
    Set volume level
    */
  AUDIO_CMD_SET_VOLUME,
  /*!
    Get current volume level
    */
  AUDIO_CMD_GET_VOLUME,
  /*!
    Trigger of audio DAC
    */
  AUDIO_CMD_DAC_ONOFF,
  /*!
    Direct play audio ES data or PCM data.
    */
  AUDIO_CMD_PLAY_MEDIA,
  /*!
    Get current audio channel setting
    */
  AUDIO_CMD_GET_CHANNEL,
  /*!
    Reset audio PCM output and audio DAC
    */
  AUDIO_CMD_AUDIO_RESET,
  /*!
    Check PCM empty number to reset audio decoder & PCM output 
    to keep audio phase difference stable.
    */
  AUDIO_CMD_PCM_EMPTY_MONITOR,
  /*!
    Enable/disable I2s output.
    */
  AUDIO_CMD_I2S_EN
}audio_cmd_t;

/*!
  This structure defines the supported PCM sample rates.
  */
typedef enum
{
  /*! 
    Sample rate is 48k 
    */
  AUDIO_SAMPLE_48 = 5,
  /*! 
    Sample rate is 44.1k 
    */
  AUDIO_SAMPLE_44 = 4,
  /*! 
    Sample rate is 32k
    */
  AUDIO_SAMPLE_32 = 6,
  /*! 
    Sample rate is 24k 
    */
  AUDIO_SAMPLE_24 = 1,
  /*!
    Sample rate is 22.05k 
    */
  AUDIO_SAMPLE_22 = 0,
  /*!
    Sample rate is 16k 
    */
  AUDIO_SAMPLE_16 = 2
} audio_sample_rate_t;

/*!
  This structure defines the supported audio channel state.
  */
typedef enum
{
  /*! 
    Stereo channel 
    */
  AUDIO_CHANNEL_STEREO,
  /*! 
    Left channel 
    */
  AUDIO_CHANNEL_LEFT,
  /*!
    Right channel 
    */
  AUDIO_CHANNEL_RIGHT,
  /*!
    Mono channel
    */
  AUDIO_CHANNEL_MONO,
  /*! 
    Both channels are closed. 
    */
  AUDIO_CHANNEL_CLOSED
} audio_channel_t;

/*!
  This structure defines the supported audio compression standards.
  */
typedef enum
{
  /*!
    MPEG 1/2 layer I/II
    */
  AUDIO_MPEG,
  /*!
    AC 3
    */
  AUDIO_AC3
}audio_type_t;


/*!
  Parameters for direct play audio ES data or PCM data.
  */
typedef struct
{
  /*!
    If the input audio data is PCM. TRUE for PCM and FALSE for MP1/2 ES data.
    */
  BOOL is_pcm;
  /*! 
    Endian of the PCM data. TRUE for little endian and FALSE for big endian 
    */
  BOOL is_little_endian;
  /*!
    Audio channel setting for the PCM data: Stereo or Mono 
    */
  BOOL is_stereo;
  /*! 
    The sample rate for the input PCM data 
    */
  audio_sample_rate_t sample;
  /*! 
    The start address of the input audio data
    */
  u8 *p_data;
  /*! 
    The size of the input audio data 
    */
  u32 size;
} audio_media_param_t;

/*!
  Initialization parameters for a audio device.
  */
typedef struct 
{
  /*! 
    configure the pinmux port
    true: config pin for audio module 
    false: config pin for extern para ts port
    */
  BOOL pinmux_flag;
}audio_config_t;

/*!
  This structure defines an audio device.
  */
typedef struct 
{
  /*!
    Pointer to device head
    */
  void *p_base;
  /*!
    Pointer to private data
    */
  void *p_priv;
}audio_device_t;

#endif //__AUDIO_H__

