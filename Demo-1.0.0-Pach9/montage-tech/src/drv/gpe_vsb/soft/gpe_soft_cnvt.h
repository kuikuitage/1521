/******************************************************************************/
/******************************************************************************/

#ifndef __GPE_SOFT_CNVT_H__
#define __GPE_SOFT_CNVT_H__



/*!
      declare the converter handle of GPE
  */
typedef u32  gpe_cnvt_handle_t;


/*!
      declare the invalid converter handle of GPE
  */
#define  INVALID_GPE_CNVT_HANDLE         (0x00)


/*!
      declare the structure of color converter of GPE 
  */
typedef struct
{
/*!
      the src region to convert 
  */
  region_t     *p_src_rgn;

/*!
      the src region rect to convert 
  */
  rect_vsb_t   *p_src_rc;

/*!
      the reference region to convert 
  */
  region_t     *p_reff_rgn;

/*!
      the reference region rect to convert 
  */
  rect_vsb_t   *p_reff_rc;

/*!
      indicate the p_reff_rgn  region is the dest region or not
      if it is, the convert will not create new  region 
      for save the result region.
      otherwise, the convert will do this
  */
  BOOL         reff_is_dest;
}gpe_cnvt_param_t;


/*!
   prepare the converter
  
  \param[in] p_param Points to gpe_cnvt_param_t.

  \return:  converter handle.
  */
gpe_cnvt_handle_t gpe_convert_prepare(const gpe_cnvt_param_t *p_param);


/*!
   start convert
  
  \param[in] cnvter  converter handle..
  \param[in] p_param Points to the dest region
  \param[out] p_result pointer to the result region

  \return:  SUCCESS if get ok
  */
RET_CODE gpe_convert(gpe_cnvt_handle_t cnvter, 
                          const gpe_cnvt_param_t *p_param, 
                          region_t **pp_result);


/*!
   release the convert result region

   \param[in] cnvter  converter handle.
   \return  SUCCESS if do ok
  */
RET_CODE gpe_convert_release(gpe_cnvt_handle_t cnvter);


#endif  // #ifndef __GPE_SOFT_CNVT_H__


