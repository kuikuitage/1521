/******************************************************************************/
/******************************************************************************/

#ifndef __VBI_VSB_H__
#define __VBI_VSB_H__



/*!
    NULL PAGE
  */
#define TTX_NULL_PAGE_NO        0x8FF


/*!
    teletext key
  */
typedef enum
{
    /*!
        TTX_KEY_0
      */
    TTX_KEY_0,
    /*!
        TTX_KEY_1
      */
    TTX_KEY_1,
    /*!
        TTX_KEY_2
      */
    TTX_KEY_2,
    /*!
        TTX_KEY_3
      */
    TTX_KEY_3,
    /*!
        TTX_KEY_4
      */
    TTX_KEY_4,
    /*!
        TTX_KEY_5
      */
    TTX_KEY_5,
    /*!
        TTX_KEY_6
      */
    TTX_KEY_6,
    /*!
        TTX_KEY_7
      */
    TTX_KEY_7,
    /*!
        TTX_KEY_8
      */
    TTX_KEY_8,
    /*!
        TTX_KEY_9
      */
    TTX_KEY_9,


    /*!
        TTX_KEY_SUBPAGE
      */
    TTX_KEY_SUBPAGE,
    /*!
        TTX_KEY_MIX
      */
    TTX_KEY_MIX,
    /*!
        TTX_KEY_HOLD
      */
    TTX_KEY_HOLD,
    /*!
        TTX_KEY_CONCEAL
      */
    TTX_KEY_CONCEAL,


    /*!
        TTX_KEY_RED
      */
    TTX_KEY_RED,
    /*!
        TTX_KEY_GREEN
      */
    TTX_KEY_GREEN,
    /*!
        TTX_KEY_YELLOW
      */
    TTX_KEY_YELLOW,
    /*!
        TTX_KEY_CYAN
      */
    TTX_KEY_CYAN,
    /*!
        TTX_KEY_INDEX
      */
    TTX_KEY_INDEX,


    /*!
        TTX_KEY_SIZE
      */
    TTX_KEY_SIZE,
    /*!
        TTX_KEY_CANCEL
      */
    TTX_KEY_CANCEL,


    /*!
        TTX_KEY_UP
      */
    TTX_KEY_UP,
    /*!
        TTX_KEY_DOWN
      */
    TTX_KEY_DOWN,
    /*!
        TTX_KEY_LEFT
      */
    TTX_KEY_LEFT,
    /*!
        TTX_KEY_RIGHT
      */
    TTX_KEY_RIGHT,
    /*!
        TTX_KEY_PAGE_UP
      */
    TTX_KEY_PAGE_UP,
    /*!
        TTX_KEY_PAGE_DOWN
      */
    TTX_KEY_PAGE_DOWN,
    /*!
        TTX_KEY_TRANSPARENT
      */
    TTX_KEY_TRANSPARENT
} ttx_key_t;

/*!
   Font size
*/
typedef enum
{
    TTX_FONT_NORMAL,
    TTX_FONT_SMALL,
}ttx_font_size_t;


/*!
  VBI event define
  */
typedef enum
{
  /*!
    VBI event define
    */
  VBI_EVT_BEGIN = (MDL_VBI << 16),  
  /*!
    VBI TTX HIDE COMMAND is ready
    */
  VBI_TTX_HIDED,
  /*!
    VBI TTX STOP COMMAND is ready
    */
  VBI_TTX_STOPPED,
  /*!
    VBI event end
    */
  VBI_EVT_END
}vbi_evt_t;


/*!
   initialize VBI module

   \param[in] prio the task's priority
   \param[in] stack_size the size of the stack

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_init_vsb(u32 prio, u32 stack_size);


/*!
   Set the PID of VBI PES

   \param[in] pid the PID of VBI PES

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_pid_vsb(u16 pid);

/*!
   Set the region handle

   \param[in] p_rgn: region handle
   
   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_region_handle_vsb(void *p_rgn);

/*!
   Start teletext decoder

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_start_vsb(u32 max_page_num, u32 max_sub_page_num);

/*!
   Stop teletext decoder

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_stop_vsb(void);

/*!
   Set teletext decoder pause ON/OFF

   \param[in] is_pause TRUE for pause ON, and FALSE for pause OFF

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_pause_vsb(void);

/*!
   Set teletext decoder pause ON/OFF

   \param[in] is_pause TRUE for pause ON, and FALSE for pause OFF

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_resume_vsb(void);


/*!
   Set teletext display page

   \param[in] page_no Page number of dispaly page.
   \param[in] is_subtitle TURE if the display page is a teletext
              subtitle page, else FALSE

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_show_vsb(BOOL is_subtitle, u32 page_no);

/*!
   Set teletext display ON/OFF

   \param[in] is_display TRUE set teletext display, and FALSE set hide
   \param[in] sync Call this function in synchronization (TRUE)
              of asynchronization (FALSE)

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_hide_vsb(BOOL sync);

/*!
   Post teletext key to subtitle task

   \param[in] key Teletext key

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_post_ttx_key_vsb(ttx_key_t key);

/*!
   Start vbi inserter.
  */
RET_CODE vbi_inserter_start_vsb(void);

/*!
   Stop vbi inserter.
  */
RET_CODE vbi_inserter_stop_vsb(void);

/*!
   Set video standar.

   \param[in] std standar of video

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_video_std_vsb(video_std_t std);

/*!
   Stop vbi font size(NORMAL size or SMALL size).
  */
RET_CODE vbi_set_font_size_vsb(ttx_font_size_t font_size);

#endif

