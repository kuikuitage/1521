/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Shanghai Montage Microelectronics Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_i2c.h
*
* Current version:	1.01.00
*
* Description:		I2C function prototype for FE module.
*
* Log:	Description			Version			Date			Author
*		---------------------------------------------------------------------
*		Create				1.00.00			2008.08.21		Rick
*		Modify				1.00.01			2010.03.24		GQ.Wen
*		Modify				1.01.00			2012.02.28		YZ.Huang
****************************************************************************/
#ifndef __MT_FE_I2C_H__
#define __MT_FE_I2C_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "mt_fe_def.h"

MT_FE_RET _mt_fe_dmd_set_reg(void *dev_handle, U8 reg_index, U8 data);
MT_FE_RET _mt_fe_dmd_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf);
MT_FE_RET _mt_fe_tn_set_reg(void *dev_handle, U8 reg_index, U8 data);
MT_FE_RET _mt_fe_tn_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf);
MT_FE_RET _mt_fe_tn_write(void *dev_handle, U8 *p_buf, U16 n_byte);
MT_FE_RET _mt_fe_tn_read(void *dev_handle, U8 *p_buf, U16 n_byte);

void _mt_sleep(U32 ticks_ms);

#ifdef __cplusplus
}
#endif

#endif /* __MT_FE_I2C_H__ */

