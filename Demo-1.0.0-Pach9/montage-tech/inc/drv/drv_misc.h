/******************************************************************************/
/******************************************************************************/
#ifndef __DRV_MISC_H__
#define __DRV_MISC_H__




/*!
    Video field information
  */
typedef enum
{
    FILED_PARRITY_ODD  = 0,
    FILED_PARRITY_EVEN = 1
} filed_parity_t;

/*!
  Standard of video format 
  */
typedef enum
{
    VID_STD_PAL,
    VID_STD_NTSC
} video_std_t;


/*!
  Get video standard
  */
video_std_t vbi_get_video_std(void);

/*!
  Get the parity (even or odd) of the field to be displayed ????? 
  */
filed_parity_t field_parity_going_to_display(void);

/*!
  Get offset

  \param[in] pts
  \param[in] stc

  \return Return offset
  */
s32 vbi_get_pts_offset(u32 pts, u32 stc);

/*!
  Get STC
  */
u32 vbi_get_stc(void);



#endif

