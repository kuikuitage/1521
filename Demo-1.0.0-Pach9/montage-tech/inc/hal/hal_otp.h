/******************************************************************************/
/******************************************************************************/
#ifndef __HAL_OTP_H__
#define __HAL_OTP_H__

/*!
  comments
  */
enum otp_region_e
{
  /*!
    comments
    */
  E_OTP_REGION_1,
  /*!
    comments
    */
  E_OTP_REGION_2,
  /*!
    comments
    */
  E_OTP_REGION_3,
  /*!
    comments
    */
  E_OTP_REGION_4,
  /*!
    comments
    */
  E_OTP_CTRL_REGION,
  /*!
    comments
    */
  E_OTP_REGION_1_2_3_4
};

/*!
  comments
  */
enum otp_ioctrl_option
{
  /*!
    comments
    */
  E_OTP_IOC_READONLY,
  /*!
    comments
    */
  E_OTP_IOC_WRITEONLY,
  /*!
    comments
    */
  E_OTP_IOC_WR,
  /*!
    comments
    */
  E_OTP_IOC_WR_UNALLOW
};
/*!
  comments
  */
s32 hal_otp_read(u8 region, u16 *p_data);
/*!
  comments
  */
s32 hal_otp_write(u8 region, u16 data);
/*!
  comments
  */
void hal_otp_init(void);
/*!
  comments
  */
s32 hal_otp_ioctrl(u8 opt, u8 region);
/*!
  comments
  */
void hal_otp_get_chip_id(char * p_chipid);
/*!
  comments
  */
u8 hal_otp_get_thirdpart_id(void);


#endif //__HAL_OTP_H__
