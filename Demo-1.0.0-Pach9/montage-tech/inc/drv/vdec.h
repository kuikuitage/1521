/******************************************************************************/
/******************************************************************************/

#ifndef __VDEC_H__
#define __VDEC_H__

/*!
  Initialization parameters for video decoder.
  */
typedef struct 
{
  /*! 
    If compression is used for video reconstruction
    */
  BOOL is_compression;
  /*! 
    Autotest flag
    */
  BOOL is_autotest;
}vdec_cfg_t;

/*!
  The video decoder may support several decoding modes,
  which consume different memory resource and consequently provide different video quality.
  The mode with less memory consumption provides lower video quality.
  It is helpful for memory sensitive solution.
  */
typedef enum
{
  /*!
    With three full size frame buffers to ensure normal reconstruction quality
    */
  VDEC_QAULITY_AD,
  /*!
    With three compressed frame buffers to save buffer consumption.
    The reconstruction video quatliy is slightly degraded.
    */
  VDEC_BUFFER_AD,
  /*!
    Only for multi-picture mode:
    */
  VDEC_MULTI_PIC_AD
} vdec_buf_policy_t;

/*!
  This structure defines the supported formats of video source
  */
typedef enum
{
  /*!
    MPEG
    */
  VIDEO_MPEG,
  /*!
    H.264/AVC
    */
  VIDEO_H264,
  /*!
    AVS
    */
  VIDEO_AVS,
    /*!
    MPEG ES
    */
  VIDEO_MPEG_ES,
  /*!
    H.264/AVC ES
    */
  VIDEO_H264_ES,
  /*!
    AVS ES
    */
  VIDEO_AVS_ES,
  /*!
    Unknown video format
    */
  VIDEO_UNKNOWN
}vdec_src_fmt_t;

/*!
  This structure defines the behavior of video display when video decoding is ready.
  */
typedef enum
{
  /*!
    The state of video layer is handled by user
    */
  VID_UNBLANK_USER,
  /*!
    The video layer will be displayed When AV is sync
    */
  VID_UNBLANK_SYNC,
  /*!
    The video layer will be displayed when video decoding is ready for display.
    */
  VID_UNBLANK_STABLE
}vdec_start_mode_t;

/*!
  This structure defines the change channel mode. single field or double filed
  */
typedef enum
{
  /*!
    change channel mode.single field
    */
  VDEC_CHCHG_SINGLE,
  /*!
    change channel mode.double field
    */
  VDEC_CHCHG_DOUBLE,
}vdec_chchg_mode_t;

/*!
  This structure defines the supported trick modes.
  */
typedef enum
{
  /*!
    Fast forward mode
    */
  VDEC_TM_FFWD,
  /*!
    Fast reverse mode
    */
  VDEC_TM_FREV,
  /*!
    Slow forward mode
    */
  VDEC_TM_SFWD,
  /*!
    Slow reverse mode
    */
  VDEC_TM_SREV,
  /*!
    Normal play mode
    */
  VDEC_TM_NORMAL
}trick_mode_t  ;

/*!
  This structure defines the video decoding error state
  */
typedef enum
{
  /*!
    No error
    */
  VDEC_ERROR_NONE,
  /*!
    The ES buffer is underflow
    */
  VDEC_ERROR_THIRSTY,
  /*!
    Picture size error
    */
  VDEC_ERROR_SIZE,
  /*!
    Exception happens in video decoder. Need reset!
    */
  VDEC_ERROR_DIE,
  /*!
    Unknown decoding error
    */
  VDEC_ERROR_UNKNOWN
}vdec_err_t;


/*!
  This structure defines the format of input video
  */
typedef enum
{
  /*!
    PAL
    */
  VDEC_FORMAT_PAL,
  /*!
    NTSC
    */
  VDEC_FORMAT_NTSC,  
  /*!
    SECAM
    */
  VDEC_FORMAT_SECAM,
  /*!
    Unspecified video format
    */
  VDEC_FORMAT_UNSPECIFIED
}vdec_video_format_t;


/*!
  This structure defines the video decoding state
  */
typedef struct
{
  /*!
    decoding error info
    */
  vdec_err_t err;
/*!
  The aspect ratio
  */
  aspect_ratio_t  ar;
  /*!
    current source data is supported
    */
  BOOL is_sup;
  /*!
    video decoding is ready for display
    */
  BOOL is_stable;
  /*!
    AV Sync is OK.
    */
  BOOL is_sync;
  //add by HY 2012-3-19 begin
  /*!
    DMA overflow
    */
  BOOL is_overflow;
    /*!
  current source format is supported by video decoder
  */
  BOOL is_format;
  //add by HY 2012-3-19 end
  //add by HY for autotest 2012-3-28 begin
  /*!
    Key frame.
    */
  int is_key_frame[20];
  /*!
    luma address
    */
  unsigned int luma_addr[20];
  /*!
    chroma address
    */
  unsigned int chroma_addr[20];
  /*!
    heigth
    */
  unsigned int heigth;
  /*!
    width
    */
  unsigned int width;
  //add by HY for autotest 2012-3-28 end
  /*!
    the format of input video
    */
  disp_sys_t vid_format;
  /*! 
    Autotest flag
    */
  BOOL is_autotest;
  /*! 
    frames statistic, have display frames
    */
    u32 frames_stat;
  /*! 
    open or close di flage
  */
  BOOL open_di;
  
} vdec_info_t;


/*!
  Start video decoding.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] format The input video coding format.
  \param[in] mode This parameter indicates how to handle the display of video layer.
  */
RET_CODE vdec_start(void *p_dev, vdec_src_fmt_t format, vdec_start_mode_t mode);

/*!
  Stop video decoding. HW will no longer write video buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_stop(void *p_dev);

/*!
  Pause video decoding. It's only effective for stream play
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_pause(void *p_dev);

/*!
  Set frozen picture.
  
  \param[in] p_dev The pointer to the video decoder.
  
  \Return Fail if picture can not be freezed till time out, keep status when stop.
  */
RET_CODE vdec_freeze(void *p_dev);

/*!
  Resume from playback pause state or picture freezing mode.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_resume(void *p_dev);

/*!
  Get video decoding state: for cable plug in/out , display on/off
  
  \param[in] p_dev The pointer to the video decoder.
  \param[out] p_state The state of video decoding.
  */
RET_CODE vdec_get_info(void *p_dev, vdec_info_t *p_state);

/*!
   This function updates the vdec info.

   \param[in] p_dev The private data for video decoder.
*/
RET_CODE vdec_info_update(void *p_dev);

/*!
  Get the required buffer size of the specific memory policy. 
  The buffer size includes both size of video es buffer and frame buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] policy The memory policy for video decoding:  frame buf & es buf
  \param[out] p_size The pointer to the required buffer size
  \param[out] p_align The pointer to the required alignment of the start buffer address.
  */
RET_CODE vdec_get_buf_requirement(void *p_dev, vdec_buf_policy_t policy, u32 *p_size, u32 *p_align);

/*!
  Set the es buffer and frame buffer for video decoder.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] policy The memory policy for video decoding
  \param[in] addr The start address of the allocated buffer
  */
RET_CODE vdec_set_buf(void *p_dev, vdec_buf_policy_t policy, u32 addr);

/*!
  decode one frame data.

  \param[in] p_dev The pointer to the video decoder.
  \param[in] p_data The pointer to the input ES video data.
  \param[in] size Size of the input ES video data.  
  */
RET_CODE vdec_dec_one_frame(void *p_dev, u8 *p_data, u32 size);

/*!
  Change video decoder to multi-picture mode.
  
  \param[in] p_dev The pointer to the video decoder.
  
  \return Fail if buffer config or video decoder work state error
  */
RET_CODE vdec_multipic_enter(void *p_dev);

/*!
  Video decoder ruturn to normal mode. 

  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_multipic_leave(void *p_dev);

/*!
  Set trick mode for video decoder. Only effective for stream file play

  \param[in] p_dev The pointer to the video decoder.
  \param[in] mode The trick mode to set
  \param[in] sr The play speed ratio. The decoder will play at 
     sr (fast trick mode) or 1/sr (slow trick mode) times of the normal play speed.
  */
RET_CODE vdec_set_trick_mode(void *p_dev, trick_mode_t mode, u8 sr);

/*!
  Video decoder set data input mode. 

  \param[in] p_dev The pointer to the video decoder.
  \param[in] is_cpu TRUE: means data come from cpu dma mode, FALSE: means data come from demux ts
  */
RET_CODE vdec_set_data_input(void *p_dev, BOOL is_cpu);

/*!
  Video decoder clear video buffer. 

  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_clear_videobuf(void *p_dev);

/*!
  set chchg mode

  \param[in] 
  \return

  */
RET_CODE vdec_chchg_mode(void *p_dev, vdec_chchg_mode_t mode);

#endif //__VDEC_H__

