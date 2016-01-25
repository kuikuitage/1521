/******************************************************************************/
/******************************************************************************/
#ifndef __PBAR_CTRL_H__
#define __PBAR_CTRL_H__
/*!
   \file ctrl_progressbar.h

   This file defined some interfaces of progressbar control for application
   layer modules.
  */

/*!
   Class name of progressbar control class
  */
#define CTRL_PBAR               ("progressbar")

/*!
   Progressbar direction mask, 0 is vertical, 1 is horizontal.
  */
#define PBAR_DIRECTION_MASK     0x00000001
/*!
   Progressbar workmode mask,
   0 : left/bottom is min value; 1 : right/top is min value.
  */
#define PBAR_WORKMODE_MASK      0x00000002
/*!
   Progressbar type mask, 0 : block bar; 1 : continuous bar;
  */
#define PBAR_CONTINUOUS_MASK    0x00000004


/*!
   Praogressbar control private data
  */
typedef struct
{
  /*!
     Rectangle of middle part.
    */
  rect_t mid_rect;
  /*!
     Minimum value of pbar.
    */
  u16 min;
  /*!
     Maximum value of pbar.
    */
  u16 max;
  /*!
     Current value of pbar.
    */
  u16 curn;
  /*!
     How many steps between max and min value,
     every step have at lease 1 pixel and 1 value
    */
  u16 step;
  /*!
     Interval between blocks.
    */
  u8 interval;
  /*!
     Rectangle style at minimum value side, 0xff means invalid.
    */
  u8 min_rstyle;
  /*!
     Rectangle style at maximum value side, 0xff means invalid.
    */
  u8 max_rstyle;
  /*!
     Rectangle style at current point, 0xff means invalid.
    */
  u8 cur_rstyle;
}ctrl_pbar_data_t;

/*!
   Register progress bar control class to system.

   \param[in] max_cnt : max progress bar control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE pbar_register_class(u16 max_cnt);

/*!
   Set middle rect.

   \param[in] p_ctrl : progress bar control
   \param[in] left : left of rect
   \param[in] top : top of rect
   \param[in] right : right of rect
   \param[in] bottom : bottom of rect.
   \return : NULL
  */
void pbar_set_mid_rect(control_t *p_ctrl,
                       s16 left,
                       s16 top,
                       s16 right,
                       s16 bottom);

/*!
   Set rectangle style.

   \param[in] p_ctrl : progress bar control
   \param[in] min_rstyle : color style at min side
   \param[in] max_rstyle : color style at max side
   \param[in] cur_rstyle : color style of current point
   \return : NULL
  */
void pbar_set_rstyle(control_t *p_ctrl,
                     u8 min_rstyle,
                     u8 max_rstyle,
                     u8 cur_rstyle);

/*!
   Set count.

   \param[in] p_ctrl : progress bar control
   \param[in] min : min value
   \param[in] max : max value
   \param[in] step : steps
   \return : NULL
  */
void pbar_set_count(control_t *p_ctrl, u16 min, u16 max, u16 step);

/*!
   Set current value.

   \param[in] p_ctrl : progress bar control
   \param[in] curn : current value
   \return : NULL
  */
void pbar_set_current(control_t *p_ctrl, u16 curn);

/*!
   Set direction.

   \param[in] p_ctrl : progress bar control
   \param[in] dir : 0 is vertical ; 1 is horizontal
   \return : NULL
  */
void pbar_set_direction(control_t *p_ctrl, u8 dir);

/*!
   Set continuous mode.

   \param[in] p_ctrl : progress bar control
   \param[in] continuous : 0 is block bar ; 1 is continuous bar
   \param[in] interval : interval between blocks
   \return : NULL
  */
void pbar_set_workmode(control_t *p_ctrl, u8 continuous, u8 interval);

/*!
   Get current value.

   \param[in] p_ctrl : progress bar control
   \return : current value
  */
u16 pbar_get_current(control_t *p_ctrl);

/*!
   Progress bar increase

   \param[in] p_ctrl : progress bar control
   \return : TRUE/FALSE
  */
BOOL pbar_increase(control_t *p_ctrl);

/*!
   Progress bar decrease

   \param[in] p_ctrl : progress bar control
   \return	 : TRUE/FALSE
  */
BOOL pbar_decrease(control_t *p_ctrl);


/*!
   Control proc.

   \param[in] p_ctrl : progress bar control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE.
  */
RET_CODE pbar_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#endif
