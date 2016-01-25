/******************************************************************************/
/******************************************************************************/
#ifndef __LIB_SEARCH_H__
#define __LIB_SEARCH_H__


/*!
   Quick array sort function

   \param[in] arr The array start address
   \param[in] num The array element number.
   \param[in] wid The array element size.
   \param[in] comp Compare callback function
  */
void lib_lsort(void *arr, u32 num, u32 wid, int (*comp)(const void *,
                                                        const void *));

/*!
   Binary search function

   \param[in] key The key of compare.
   \param[in] arr The array start address
   \param[in] num The array element number.
   \param[in] wid The array element size.
   \param[in] comp Compare callback function

   \return NULL for can't find the element. Other value for the finded element
   address.
  */
void *lib_bsearch(const void *key,
                  const void *arr,
                  u32 num,
                  u32 wid,
                  int (*comp)(const void *, const void *));

#endif //__LIB_SEARCH_H__
