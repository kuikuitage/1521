/******************************************************************************/
/******************************************************************************/
#ifndef __DATA_MANAGER32_H_
#define __DATA_MANAGER32_H_

/*!
  Initialize data manager32 and init all the variables and attach process 
  functions 
  
  \param[in] max_blk_num max block number managed by data manager32
  \param[in] flash_base_addr flash base address
  */
void dm_init_32(u16 max_blk_num, u32 flash_base_addr);

#endif //__DATA_MANAGER32_H_
