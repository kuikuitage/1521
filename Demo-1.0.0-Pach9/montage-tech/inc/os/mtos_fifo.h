/******************************************************************************/
/******************************************************************************/
#ifndef  __MTOS_FIFO_H__
#define  __MTOS_FIFO_H__

/*!
  MTOS fifo structure
  */
typedef struct tagFifo
{
  /*!
    pionter of fifo data
    */
  u16   *p_buffer; 
  /*!
    header of fifo
    */
  u32   m_head;
  /*!
    tail of fifo
    */
  u32   m_tail;
  /*!
    counter of fifo data
    */
  s32   m_cnt;
  /*!
    size of fifo buffer
    */
  u32   m_size;
  /*!
    overlay flag
    */
  BOOL  m_overlay;
}os_fifo_t;


/*!
  Clear fifo data
  
  \param[in] p_fifo pointer to fifo
  */
void mtos_fifo_flush(os_fifo_t *p_fifo);

/*!
  Put data to fifo
  
  \param[in] p_fifo pointer to fifo
  \param[in] data data to store
  */
void mtos_fifo_put(os_fifo_t *p_fifo, u16 data);

/*!
  Get fifo data
  
  \param[in] p_fifo pointer to fifo
  \param[out] p_data store the data got

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_fifo_get(os_fifo_t *p_fifo, u16 *p_data);

#endif //__MTOS_FIFO_H__
