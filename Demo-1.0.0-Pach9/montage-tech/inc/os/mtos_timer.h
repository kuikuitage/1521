/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_TIMER_H__
#define __MTOS_TIMER_H__

/*!
  the minimum time(ms) for OS timer task count
  */
#define MTOS_SYSTIMER_INTERVAL       10
/*!
  the timer count provided by OS
  */
#define MTOS_MAX_SYSTIMER            16

/*!
  Create a software timer
  
  \param[in] ntimerval the timer time out value, in ms
  \param[in] p_timerproc the time out callback function
  \param[in] context the parameter to callback funciton
  \param[in] bcycleen single or repeat timer, FALSE: singal, TRUE:repeat

  \return timer id if >=0, error if <0
  */
s32  mtos_timer_create(u32   ntimerval,
                    void  (*p_timerproc)(u32 funcontext),
                    u32   context,
                    BOOL  bcycleen);
/*!
  Create a software timer,when bcycleen is FALSE,
  timer isn't autodeleted.
  
  \param[in] ntimerval the timer time out value, in ms
  \param[in] p_timerproc the time out callback function
  \param[in] context the parameter to callback funciton
  \param[in] bcycleen single or repeat timer, FALSE: singal, TRUE:repeat

  \return timer id if >=0, error if <0
  */
s32  mtos_timer_create_unautodel(u32   ntimerval,
                    void  (*p_timerproc)(u32 funcontext),
                    u32   context,
                    BOOL  bcycleen);

/*!
  Delete a timer
  
  \param[in] ntimerid the timer id
  */
void mtos_timer_delete(s32 ntimerid);

/*!
  Start a timer
  
  \param[in] ntimerid the timer id
  */
void mtos_timer_start(s32 ntimerid);

/*!
  Stop a timer
  
  \param[in] ntimerid the timer id
  */
s32 mtos_timer_stop(s32 ntimerid);

/*!
  Reset the time out value of a time in use
  
  \param[in] ntimerid the timer id
  \param[in] ntimerval the new time out
  */
void mtos_timer_reset(s32 ntimerid,u32 ntimerval);
/*!
  is a timer runing!
  
  \param[in] ntimerid the timer id
  */
s32 mtos_timer_running(s32 ntimerid);
/*!
  Get the time to next time out
  
  \param[in] ntimerid the timer id
  \param[out] nsparetimerval the time to next timeout, in ms
  */
void mtos_timer_get_sparetime(s32 ntimerid,u32 *p_sparetime);

#endif //__MTOS_TIMER_H__

