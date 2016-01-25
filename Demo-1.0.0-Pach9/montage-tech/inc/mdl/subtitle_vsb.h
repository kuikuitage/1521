/******************************************************************************/
/******************************************************************************/

#ifndef __SUBTITLE_VSB_H__
#define __SUBTITLE_VSB_H__

/*!
  SUBT event define
  */
typedef enum
{
  /*!
    SUBT event define
    */
  SUBT_EVT_BEGIN = (MDL_SUBT << 16),  
  /*!
    SUBT STOP COMMAND is ready
    */
  SUBT_EVT_STOPPED,
  /*!
    SUBT event end
    */
  SUBT_EVT_END
}subt_evt_t;


/*!
   initialize subtitle module

   \param[in] prio the task's priority
   \param[in] stack_size the size of the stack

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_init_vsb(u32 prio, u32 stack_size, disp_layer_id_t layer);

/*!
   Set the PID and page ID that want to display

   \param[in] pid PID of subtilte PES
   \param[in] composition_page Page id of composition page
   \param[in] ancillary_page Page id of ancillary page

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_page_vsb(u16 pid, u16 composition_page, u16 ancillary_page);

/*!
   Start subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_start_vsb(void);

/*!
   Stop subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_stop_vsb(void);

/*!
   Set subtitle decoder pause ON/OFF

   \param[in] is_pause TRUE for pause on, and FALSE for pause off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_pause_vsb(BOOL is_pause);

/*!
   Set subtitle display ON/OFF

   \param[in] is_pause TRUE for display on, and FALSE for display off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_display_vsb(BOOL is_display);

/*!
   Set video standard.

   \param[in] std standar of video

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_video_std_vsb(video_std_t std);


#endif

