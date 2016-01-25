/******************************************************************************/
/******************************************************************************/
#ifndef __AVCTRL_H_
#define __AVCTRL_H_

/*!
  \file avctrl.h

  Simply wrapper interfaces for AV drivers and database
  */

/*!
  Picture rectangle parameters for multi pictures
  */
typedef struct 
{
  /*!
    Left position of video rectangle
    */
  u16 left;
  /*!
    Top position of video rectangle
    */
  u16 top;
  /*!
	  Width of video rectangle
    */
  u16 width;
  /*!
	  Height of video rectangle
    */
  u16 height;
} avc_rect_t;

/*!
  Audio channel mode
  */
typedef enum
{
  /*!
    Stereo audio
    */
  AVC_AUDIO_STEREO = 0,
  /*!
    Left audio channel 
    */
  AVC_AUDIO_LEFT,
  /*!
    Right audio channel
    */
  AVC_AUDIO_RIGHT,
  /*!
    Mono audio mode
    */
  AVC_AUDIO_MONO
} avc_audio_mode_t;

/*!
  Video display mode
  */
typedef enum
{
  /*!
    AV display mode. depend by stream standard
    */
  AVC_VIDEO_MODE_AUTO,
  /*!
    AV display mode. NTSC standard
    */
  AVC_VIDEO_MODE_NTSC,
  /*!
    AV display mode. PAL standard
    */
  AVC_VIDEO_MODE_PAL,
  /*!
    AV display mode. PAL N standard
    */
  AVC_VIDEO_MODE_PAL_M,
  /*!
    AV display mode. PAL M standard
    */
  AVC_VIDEO_MODE_PAL_N
}avc_video_mode_t;

/*!
  set video dac
  \param[in] p_data private data
  \param[in] mode: video format in magic chip
  */
void avc_cfg_scart_format(void *p_data, scart_v_format_t mode);

/*!
  set SOG on/off
  \param[in] p_data private data
  \param[in] is_enable: enable or not
  */
void avc_cfg_scart_sog(void *p_data, BOOL is_enable);

/*!
  config tv master
  \param[in] p_data private data
  \param[in] terminal scart terminal
  */
void avc_cfg_scart_select_tv_master(void *p_data, scart_terminal_t terminal);

/*!
  config video aspect
  \param[in] p_data private data
  \param[in] mode: aspect mode
  */
void avc_cfg_scart_aspect(void *p_data, scart_v_aspect_t mode);


/*!
  config vcr input
  \param[in] p_data private data
  \param[in] terminal scart terminal
  */
void avc_cfg_scart_vcr_input(void *p_data, scart_terminal_t terminal);

/*!
  detect scart vcr is activated or not
  \param[in] p_data private data
  */
BOOL avc_detect_scart_vcr(void *p_data);

/*!
  set rf system
  \param[in] p_data private data
  \param[in] sys: rf system
  */
BOOL avc_set_rf_system(void *p_data, rf_sys_t sys);

/*!
  set rf channel
  \param[in] p_data private data
  \param[in] channel: rf channel
  */
BOOL avc_set_rf_channel(void *p_data, u16 channel);

/*!
  Multi picture mode supported in AVC
  */
typedef enum
{
  /*!
    Normal AVC mode
    */
  AVC_NORMAL = 0,
  /*!
    9 pictures displaying mode
    */
  AVC_9_PICTURES, 
  /*!
    6 pictures supported
    */
  AVC_6_PICTURES
} avc_multi_pic_mode_t;

/*!
  Notification callback function type define
  Notify UI set OSD position after switch P/N
  */
typedef void (*avc_callback_screen_pos_t)(BOOL is_ntsc);
/*!
  AV control module initilization
  */
void avc_init(void);

/*!
  AV control module initilization
  */
BOOL avc_init_2m(void);

/*!
  Install notify callback functions.
  \param[in] p_data avc private data
  \param[in] p_proc: Function address.
  \return  TRUE if succeed. FALSE for failed.
  */
BOOL avc_install_screen_notification(void *p_data, 
avc_callback_screen_pos_t p_proc);

/*!
  PAL/NTSC switch for OSD layer. This fuction modify UI move osd region position
  according to the video PAL/NTSC status,
  \param[in] p_data avc private data
  \param[in] is_ntsc: TRUE is NTSC;
                      FALSE is PAL.
  */
void avc_reset_screen_pos(void *p_data, BOOL is_ntsc);

/*!
  Set audio volume
  
  \param[in] p_data avc private data
  \param[in] volume: audio volume, only use 5 bits \sa ProgNode
  */
void avc_setvolume(void *p_data, u8 volume);

/*!
  Change audio playing mode
  
  \param[in] p_data avc private data
  \param[in] mode:  0 stereo, 1 left, 2 right
  */
void avc_set_audio_mode(void *p_data, avc_audio_mode_t mode);

/*!
  Check the mute status of avc module
  
  \param[in] p_data avc private data
  \param[out] : TURE avc is mute, FALSE avc is not mute ;
  */
BOOL avc_is_mute(void *p_data);

/*!
  Set mute
  
  \param[in] p_data avc private data
  \param[in] is_mute 0: unmute, 1: mute;
  \param[in] is_update 1: change flag, 0: unchanged
  */
void avc_set_mute(void *p_data, BOOL is_mute, BOOL is_update);
/*!
  Enter preview
  
  \param[in] p_data avc private data
  \param[in] left: left position of rectangle 
  \param[in] top:  top position of rectangle
  \param[in] height: height of rectangle
  \param[in] width:  width of rectangle
  */
void avc_enter_preview(void *p_data, u16 left, u16 top, u16 width, 
u16 height);

/*!
  Config a program in an window, it works after pb_enter_preview
  
  \param[in] p_data avc private data
  \param[in] left Left position of window in pixel
  \param[in] top Top position of window in pixel
  \param[in] width Width of window in pixel
  \param[in] height Height of window in pixel
  */
void avc_config_preview(void *p_data, u16 left, u16 top, u16 width, 
u16 height);

/*!
  Leave preview mode
  \param[in] p_data avc private data
  */
void avc_leave_preview(void *p_data);

/*!
  Set video mode
  \param[in] p_data avc private data
  \param[in] mode: new avc mode to be set
  */
void avc_switch_video_mode(void *p_data, avc_video_mode_t mode);

/*!
  Get video mode
  \param[in] p_data avc private data
  \return current video steam mode.
  */
video_standard_t avc_get_video_mode(void *p_data);


/*!
  Set video aspect
  \param[in] p_data avc private data
  \param[in] mode: new aspect mode to be set
  */
void avc_set_video_aspect_mode(void *p_data, video_aspect_t mode);


/*!
  Pause video
  \param[in] p_data avc private data
  */
BOOL avc_video_pause(void *p_data);

/*!
  Resume video
  
  \param[in] p_data avc private data
  \param[out] : TRUE video resume, FALSE video not resume
  */
BOOL avc_video_resume(void *p_data);

/*!
  Enter vide preview by compress mode
  \param[in] p_data avc private data
  \param[in] p_av_rect config parameter for avc display
  \param[in] buf_addr buffer address 
  */
void avc_enter_compress_preview(void *p_data, avc_rect_t *p_av_rect,
  u32 buf_addr);

/*!
  Leave compressed preview mode
  
  \param[in] p_data avc private data
  \param[in] b_replay: TRUE-> play video when leave preview
                       False-> do not play video when leave preview
  */
void avc_leave_compress_preview(void *p_data, BOOL b_replay);

#endif // End for __AVCTRL_H_
