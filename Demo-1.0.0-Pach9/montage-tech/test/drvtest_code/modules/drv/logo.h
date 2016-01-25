/*****************************************************************************/
/* Owner: Jason Lin <jason.lin@montage-tech.com>                             */
/*****************************************************************************/

#ifndef __LOGO_H__
#define __LOGO_H__


/*!
  logo data
  */
extern u8 logo_data[];


/*!
  logo data size
  */
u32 get_logo_size(void);


/*!
  ts logo data
  */
extern u8 ts_logo_data[];


/*!
  ts logo data size
  */
u32 get_ts_logo_size(void);  
#endif // End for __LOGO_H__
