/******************************************************************************/
/******************************************************************************/

#ifndef __PTI_PRIV_H__
#define __PTI_PRIV_H__


/*!
  PTI
  */
typedef struct s_pti
{
//priv:
    /*!
      priv
      */
    void *p_priv;

//public:

    /*!
      Get the size of buffer for slot and filter management.
      */
    RET_CODE (*p_get_mem_request)(void *p_priv
        , BOOL dynamic, s8 filters, u32 *p_size);

    /*!
      Allocate memory for blind scan: buffer for slot and filter management.
      */
    RET_CODE (*p_parser_init)(void *p_priv
        , BOOL dynamic, s8 filters, u8 *p_buf, u32 buf_size);

    /*!
      Release the memory allocated for blind scan.
      */
    RET_CODE (*p_parser_deinit)(void *p_priv);

    /*!
      Allocate a filter with a slot for a section.
      */
    RET_CODE (*p_filter_alloc)(void *p_priv
        , const pti_alloc_param_t *p_param, h_pti_channel_t *p_channel);

    /*!
      Release a filter.
      */
    RET_CODE (*p_filter_free)(void *p_priv, h_pti_channel_t channel);

    /*!
      Get data from a specifically filter.
      */
    RET_CODE (*p_get_data)(void *p_priv
        , h_pti_channel_t channel, pti_data_block_t *p_block);

    /*!
      Free data that got from a specifically filter.
      */
    RET_CODE (*p_free_data)(void *p_priv
        , h_pti_channel_t channel, pti_data_block_t *p_block);

    /*!
      Set the PID of video.
      */
    RET_CODE (*p_set_video_pid)(void *p_priv, u16 pid);

    /*!
      Set the PID of audio.
      */
    RET_CODE (*p_set_audio_pid)(void *p_priv, u16 pid);

    /*!
      Set the PID of pcr.
      */
    RET_CODE (*p_set_pcr_pid)(void *p_priv, u16 pid);

    /*!
      Get the PIDs of video, audio and pcr that pre-set.
      */
    RET_CODE (*p_get_pids)(void *p_priv
        , u16 *p_v_pid, u16 *p_a_pid, u16 *p_pcr_pid);

    /*!
      Get the PIDs of video, audio and pcr that pre-set.
      */
    RET_CODE (*p_desc_clear_key)(void *p_priv, u16 pid);
    /*!
      Get the PIDs of video, audio and pcr that pre-set.
      */
    RET_CODE (*p_desc_set_odd_key)(void *p_priv, u16 pid, u8 *p_key, BOOL enable_pes);
    /*!
      Get the PIDs of video, audio and pcr that pre-set.
      */
    RET_CODE (*p_desc_set_even_key)(void *p_priv, u16 pid, u8 *p_key, BOOL enable_pes);

    /*! 
      Set the PID of pcr.
      */
    RET_CODE (*p_av_enable)(void *p_priv, BOOL enable);

    /*! 
      Set the PID of pcr.
      */
    RET_CODE (*p_is_av_enable)(void *p_priv, BOOL *enable);

    /*!
      Reset pti, ES buffer and A/V decoder for channel switch.
      */
    RET_CODE (*p_av_reset)(void *p_priv);

    /*!
      Enable/disable PTI module.
      */
    RET_CODE (*p_select_source)(void *p_priv, pti_source_t source);

    /*!
      Enable/disable PTI module.
      */
    RET_CODE (*p_get_source)(void *pti, pti_source_t *p_source);
}  pti_t;

#endif

