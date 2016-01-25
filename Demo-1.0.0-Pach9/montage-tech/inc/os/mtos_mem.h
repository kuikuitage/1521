/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_MEM_H__ 
#define __MTOS_MEM_H__

/*!
  switch for debug
  */
//#define MTOS_MEM_DEBUG

/*!
  malloc function pointer
  */
typedef  void * (*pmalloc)(u32 size);
/*!
  free function pointer
  */
typedef  void  (*pfree)(void *p_addr);

/*!
  Initiate the memory 
  
  \param[in] m function pointer to malloc functino
  \param[in] f function pointer to free functino
  */
void  mtos_mem_init(pmalloc m, pfree f);

/*!
  Initiate the memory 
  
  \param[in] m function pointer to malloc functino
  \param[in] f function pointer to free functino
  */
void  mtos_nc_mem_init(pmalloc m, pfree f);


#ifdef MTOS_MEM_DEBUG
/*!
  malloc space from system partition 
  */
#define mtos_malloc(x) mtos_malloc_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  free memory 
  */
#define mtos_free(x) mtos_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  malloc space from system partition 
  
  \param[in] size the size to malloc
  \param[in] p_file file name
  \param[in] p_func function name
  \param[in] line line number
  */
void* mtos_malloc_dump(u32 size, char *p_file, char *p_func, int line);

/*!
  free memory
  
  \param[in] addr the memory address to free
  \param[in] p_file file name
  \param[in] p_func function name
  \param[in] line line number

  */
void  mtos_free_dump(void *p_addr, char *p_file, char *p_func, int line);

#else
/*!
  malloc space from system partition 
  
  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_malloc(u32 size);

/*!
  free memory
  
  \param[in] addr the memory address to free
  */
void  mtos_free(void *p_addr);
#endif

/*!
  malloc space from system partition 
  
  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_nc_malloc(u32 size);

/*!
  free memory
  
  \param[in] addr the memory address to free
  */
void  mtos_nc_free(void *p_addr);

/*!
  malloc space from system partition 
  
  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_malloc_alias(u32 size);

/*!
  free memory
  
  \param[in] addr the memory address to free
  */
void  mtos_free_alias(void *p_addr);
/*!
  free memory
  
  \param[in] addr the memory address to free
  */
void  mtos_nc_mem_cfg(pmalloc m, pfree f);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void * mtos_align_malloc(u32 size, u32 alignment);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_free(void *p_mem_ptr);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void * mtos_align_nc_malloc(u32 size, u8 alignment);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_nc_free(void *p_mem_ptr);
#if 0
/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  param[in] alignment: cashable address or non-cashable address
  return Allocated memory start address
  */
void * mtos_align_sel_malloc(u32 size, u8 alignment, u8 cashable);
/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_sel_free(void *p_mem_ptr);
#endif

void *mtos_realloc(void *p_readdr, u32 size);

#endif //__MTOS_MEM_H__ 
