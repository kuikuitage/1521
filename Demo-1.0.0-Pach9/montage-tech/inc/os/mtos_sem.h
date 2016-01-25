/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_SEM_H__ 
#define __MTOS_SEM_H__ 

/*!
  Delete semaphore ONLY if no task pending
  */
#define  MTOS_DEL_NO_PEND            0
/*!
  Deletes the semaphore even if tasks are waiting.
  */
#define  MTOS_DEL_ALWAYS             1

/*!
  define the semaphore type
  */
typedef unsigned int os_sem_t;

/*!
  Create a semaphore
  
  \param[out] p_sem the semaphore ID created
  \param[in] mutex if it is a mutex type

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_create(os_sem_t *p_sem, BOOL mutex);

/*!
  Destroy a semaphore
  
  \param[in] p_sem the semaphore ID to destroy
  \param[in] opt delete option:
            opt == MTOS_DEL_NO_PEND   Delete semaphore ONLY if no task pending
            opt == MTOS_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.In this case, all the tasks pending will be readied.

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_destroy(os_sem_t *p_sem, u8 opt);

/*!
  Singal a semaphore
  
  \param[in] p_sem the pointer to semaphore to signal

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_give(os_sem_t *p_sem);

/*!
  Wait for a semaphore
  
  \param[in] p_sem the pointer to semaphore to wait
  \param[in] ms time out value in ms, 0: wait forever

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_take(os_sem_t *p_sem, u32 ms);

#endif //__MTOS_SEM_H__
