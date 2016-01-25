/******************************************************************************/
/******************************************************************************/
#ifndef __HAL_BASE_H__
#define __HAL_BASE_H__


/*!
  The max hal module number
  */
#define HAL_MAX_MODULE    16

/*!
  Write 8 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
static inline void hal_put_u8(volatile unsigned char *p_addr, 
                                               unsigned char  data)
{
  /*!
      Write 8 bits register
    */
  *p_addr = data;
}

/*!
  Write 16 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
static inline void hal_put_u16(volatile unsigned short *p_addr, 
                                                 unsigned short data)
{
  /*!
      Write 16 bits register
    */
  *p_addr = data;
}

/*!
  Write 32 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
static inline void hal_put_u32(volatile unsigned long *p_addr, 
                                                 unsigned long data)
{
  /*!
      Write 32 bits register
    */
  *p_addr = data;
}

/*!
  Get 8 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
static inline unsigned char hal_get_u8(volatile unsigned char *p_addr)
{
  /*!
      Get 8 bits register value
    */
  return *p_addr;
}

/*!
  Get 16 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
static inline unsigned short hal_get_u16(volatile unsigned short *p_addr)
{
  /*!
      Get 16 bits register value
    */
  return *p_addr;
}

/*!
  Get 32 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
static inline unsigned long hal_get_u32(volatile unsigned long *p_addr)
{
  /*!
      Get 32 bits register value
    */
  return *p_addr;
}

/*!
  register hal module
  
  \param[in] type the module type, refer to SYS_HAL_TYPE_XXX
  \param[in] p_priv the module private info

  \return TRUE if success, else FALSE
  */
int hal_register_module(unsigned long type, void *p_priv);

/*!
  Get hal module private info
  
  \param[in] type the module type, refer to SYS_HAL_TYPE_XXX

  \return the pointer to module private info, NULL if not found
  */
void *hal_find_module(unsigned long type);

#endif  //end of __HAL_BASE_H__
