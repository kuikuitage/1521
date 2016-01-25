/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_TASK_H__ 
#define __MTOS_TASK_H__ 


/*!
  Create a new task
  
  \param[in] p_taskname the task name
  \param[in] p_taskproc the task main process function
  \param[in] p_param parameters for task process function
  \param[in] nprio task priority
  \param[in] pstk pointer to task stack
  \param[in] nstksize ths task stack size

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_task_create(u8   *p_taskname,
                     void (*p_taskproc)(void *p_param),
                     void  *p_param,
                     u32    nprio,
                     u32   *pstk,
                     u32    nstksize);

/*!
  Delete a task
  
  \param[in] ms the time to sleep, unit: ms
  */
void mtos_task_exit(void);


/*!
  Suspend a task for some time
  
  \param[in] ms the time to sleep, unit: ms
  */
void mtos_task_sleep(u32 ms);

/*!
  Lock a task, the os will not dispatch the tasks any longer
  
  \param[in] None
  */
void mtos_task_lock(void);

/*!
  Unlock the task, and the os will start dispatch again
  
  \param[in] None
  */
void mtos_task_unlock(void);

/*!
  Resume task

  \param[in] prio Task priority
  */
void mtos_task_resume(u8 prio);

/*!
  Suspend task

  \param[in] prio Task priority
  */
void mtos_task_suspend(u8 prio);

/*!
  change task priority

  \param[in] oldprio Task priority
  \param[in] newprio Task priority
  */
void mtos_task_change_prio(u8 oldprio, u8 newprio);

#endif //__MTOS_TASK_H__ 

