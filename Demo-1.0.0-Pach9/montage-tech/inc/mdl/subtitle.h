/******************************************************************************/
/******************************************************************************/

#ifndef __SUBTITLE_H__
#define __SUBTITLE_H__



/*!
   initialize subtitle module

   \param[in] prio the task's priority
   \param[in] stack_size the size of the stack

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_init(u32 prio, u32 stack_size);

/*!
   Set the PID and page ID that want to display

   \param[in] pid PID of subtilte PES
   \param[in] composition_page Page id of composition page
   \param[in] ancillary_page Page id of ancillary page

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_page(u16 pid, u16 composition_page, u16 ancillary_page);

/*!
   Start subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_start(void);

/*!
   Stop subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_stop(void);

/*!
   Set subtitle decoder pause ON/OFF

   \param[in] is_pause TRUE for pause on, and FALSE for pause off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_pause(BOOL is_pause);

/*!
   Set subtitle display ON/OFF

   \param[in] is_pause TRUE for display on, and FALSE for display off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_display(BOOL is_display);

/*!
   Set video standard.

   \param[in] std standar of video

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_video_std(video_std_t std);


#endif

