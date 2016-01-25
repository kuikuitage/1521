/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_MUTEX_H__ 
#define __MTOS_MUTEX_H__ 

/*!
  Create a mutex
  
  \param[in] mutex if it is a mutex type

  \return a handle for mutex
  */
void *mtos_mutex_create(unsigned int prio);

/*!
  Destroy a mutex
  
  \param[in] pevent
       
  \return TRUE if OK, FALSE if fail
  */
int mtos_mutex_delete(void *pevent);

/*!
  Singal a mutex
  
  \param[in] pevent

  \return TRUE if OK, FALSE if fail
  */
int mtos_mutex_give(void *pevent);

/*!
  Wait for a mutex
  
  \param[in] pevent
  
  \return TRUE if OK, FALSE if fail
  */
int mtos_mutex_take(void *pevent);

#endif //__MTOS_SEM_H__

