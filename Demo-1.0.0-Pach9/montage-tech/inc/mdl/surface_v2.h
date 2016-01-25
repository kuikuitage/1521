/******************************************************************************/
/******************************************************************************/
#ifndef __SURFACE_V2_H__
#define __SURFACE_V2_H__

/*!
   Initialize the surface module.

   \param[in] p_odd_addr The address of odd field buffer.
   \param[in] odd_size The size of odd field buffer.
   \param[in] p_even_addr The address of even field buffer.
   \param[in] even_size The size of even field buffer.
   \param[in] cnt The maximal count of the surfaces.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_init_v2(u32 *p_odd_addr, u32 odd_size,
  u32 *p_even_addr, u32 even_size, u8 cnt);



#endif
