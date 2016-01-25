/******************************************************************************/
/******************************************************************************/

#ifndef __TTX_HAMM_H__
#define __TTX_HAMM_H__

/*!
   TODO: fix me!

   \param[in] c
  */
u8 vbi_rev8(u8 c);


/*!
   TODO: fix me!

   \param[in] c
  */
u16 vbi_rev16(u16 c);

/*!
   TODO: fix me!

   \param[in] p_p
  */
u16 vbi_rev16p(const u8 *p_p);



/*!
   TODO: fix me!

   \param[in] c
  */
u32 vbi_par8(u32 c);



/*!
   TODO: fix me!

   \param[in] c
  */
s32 vbi_unpar8(u32 c);



/*!
   TODO: fix me!

   \param[in] c
  */
u32 vbi_ham8(u32 c);



/*!
   TODO: fix me!

   \param[in] c
  */
s8 vbi_unham8(u8 c);



/*!
   TODO: fix me!

   \param[in] p_p
  */
s16 vbi_unham16p(const u8 *p_p);


/*!
   ABC

   \param[in] c
  */
extern void vbi_par(u8 *p_p, u32 n);
/*!
   ABC

   \param[in] c
  */
extern s32 vbi_unpar(u8 *p_p, u32 n);

/*!
   ABC

   \param[in] c
  */
extern void vbi_ham24p(u8 *p_p, u32 c);

/*!
   ABC

   \param[in] c
  */
extern s32 vbi_unham24p(const u8 *p_p);
#endif
