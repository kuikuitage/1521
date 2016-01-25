/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Shanghai Montage Microelectronics Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_i2c.c
*
* Current version:	1.00.00
*
* Description:		Define all i2c function for FE module.
*
* Log:	Description			Version		Date		Author
*		---------------------------------------------------------------------
*		Create				1.00.00		2008.08.21	Rick
*		Modify				1.00.01		2010.03.24	GQ.Wen
****************************************************************************/
#include "mt_fe_def.h"
#include "mtos_task.h"



extern u8 dc2800_dmd_reg_read(u8 addr, u8 reg);
extern void dc2800_tn_reg_write(u8 addr, u8 reg, u8 data);
extern u8 dc2800_tn_reg_read(u8 addr, u8 reg);
extern void dc2800_tn_reg_write(u8 addr, u8 reg, u8 data);
extern void dc2800_dmd_reg_write(u8 addr, u8 reg, u8 data);
extern s32 dc2800_tn_seq_write(u8 addr, u8 *p_buf, u32 len);
extern s32 dc2800_tn_seq_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
/*****************************************************************
** Function: _mt_fe_dmd_set_reg
**
**
** Description:	write data to demod register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**	data			U8			value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_set_reg(void *dev_handle, U8 reg_index, U8 data)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

   dc2800_dmd_reg_write(handle->demod_dev_addr, reg_index, data);
	/*
		TODO:
			Obtain the i2c mutex
	*/



	/*
		TODO:
			write data to demodulator register
	*/



	/*
		TODO:
			Release the i2c mutex
	*/



	return MtFeErr_Ok;
}


/*****************************************************************
** Function: _mt_fe_dmd_get_reg
**
**
** Description:	read data from demod register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	p_buf			U8*			register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

   *p_buf = dc2800_dmd_reg_read(handle->demod_dev_addr, reg_index);
	/*
		TODO:
			Obtain the i2c mutex
	*/



	/*
		TODO:
			read demodulator register value
	*/



	/*
		TODO:
			Release the i2c mutex
	*/


	return MtFeErr_Ok;
}


/*****************************************************************
** Function: _mt_fe_tn_set_reg
**
**
** Description:	write data to tuner register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**	data			U8			value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_set_reg(void *dev_handle, U8 reg_index, U8 data)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	//MT_FE_RET ret = MtFeErr_I2cErr;
	//U8 value;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

    dc2800_tn_reg_write(handle->tuner_settings.tuner_dev_addr, reg_index, data);
#if 0

	/*
		TODO:
			Obtain the i2c mutex
	*/


	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
	{
		_mt_fe_dmd_get_reg(handle, 0x87, &value);
		value &= 0x0F;
		value |= 0x90;	// bit7		 = 1, Enable I2C repeater
						// bit[6:4]	 = 1, Enable I2C repeater for 1 time
		_mt_fe_dmd_set_reg(handle, 0x87, value);
	}
	else if(handle->on_board_settings.chip_mode == 2)
	{
		_mt_fe_dmd_get_reg(handle, 0x86, &value);
		value |= 0x80;
		_mt_fe_dmd_set_reg(handle, 0x86, value);
	}
	else
	{
		return MtFeErr_NoSupportFunc;
	}


	/*Do not sleep any time after I2C repeater is opened.*/
	/*please set tuner register at once.*/


	/*
		TODO:
			write value to tuner register
	*/



	/*
		TODO:
			Release the i2c mutex
	*/

#endif

	return MtFeErr_Ok;
}




/*****************************************************************
** Function: _mt_fe_tn_get_reg
**
**
** Description:	get tuner register data
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	register		U8			register address
**
**
** Outputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	p_buf			U8*			register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	//MT_FE_RET	ret = MtFeErr_I2cErr;
	//U8 value;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

    *p_buf = dc2800_tn_reg_read(handle->tuner_settings.tuner_dev_addr, reg_index);
#if 0
	/*
		TODO:
			Obtain the i2c mutex
	*/


	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
	{
		_mt_fe_dmd_get_reg(handle, 0x87, &value);
		value &= 0x0F;
		value |= 0xA0;	// bit7		 = 1, Enable I2C repeater
						// bit[6:4]	 = 2, Enable I2C repeater for 2 times if there're 2 stops after the repeater
						// bit[6:4]  = 1, Enable I2C repeater for 1 time if there's only 1 stop after the repeater
		_mt_fe_dmd_set_reg(handle, 0x87, value);
	}
	else if(handle->on_board_settings.chip_mode == 2)
	{
		_mt_fe_dmd_get_reg(handle, 0x86, &value);
		value |= 0x80;
		_mt_fe_dmd_set_reg(handle, 0x86, value);
	}
	else
	{
		return MtFeErr_NoSupportFunc;
	}
	/*Do not sleep any time after I2C repeater is opened.*/
	/*please read tuner register at once.*/


	/*
		TODO:
			read tuner register value
	*/


	/*
		TODO:
			Release the i2c mutex
	*/

#endif
	return MtFeErr_Ok;
}



MT_FE_RET _mt_fe_tn_write(void *dev_handle, U8 *p_buf, U16 n_byte)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
#if 1
   dc2800_tn_seq_write(handle->tuner_settings.tuner_dev_addr, p_buf, n_byte);
#else
	//MT_FE_RET	ret = MtFeErr_Ok;
	U8			value;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	/*
		TODO:
			Obtain the i2c mutex
	*/


	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
	{
		_mt_fe_dmd_get_reg(handle, 0x87, &value);
		value &= 0x0F;
		value |= 0x90;	// bit7		 = 1, Enable I2C repeater
						// bit[6:4]	 = 1, Enable I2C repeater for 1 time
		_mt_fe_dmd_set_reg(handle, 0x87, value);
	}
	else if(handle->on_board_settings.chip_mode == 2)
	{
		_mt_fe_dmd_get_reg(handle, 0x86, &value);
		value |= 0x80;
		_mt_fe_dmd_set_reg(handle, 0x86, value);
	}
	else
	{
		return MtFeErr_NoSupportFunc;
	}

	/*Do not sleep any time after I2C repeater is opened.*/
	/*please write N bytes to register at once.*/


	/*
		TODO:
			write N bytes to tuner
	*/



	/*
		TODO:
			Release the i2c mutex
	*/

#endif

	return MtFeErr_Ok;
}


MT_FE_RET _mt_fe_tn_read(void *dev_handle, U8 *p_buf, U16 n_byte)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
#if 1
    dc2800_tn_seq_read(handle->tuner_settings.tuner_dev_addr, p_buf, 0, n_byte);
#else
	//MT_FE_RET	ret = MtFeErr_Ok;
	U8			value;

	/*
		TODO:
			Obtain the i2c mutex
	*/


	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
	{
		_mt_fe_dmd_get_reg(handle, 0x87, &value);
		value &= 0x0F;
		value |= 0x90;	// bit7		 = 1, Enable I2C repeater
						// bit[6:4]	 = 1, Enable I2C repeater for 1 time
		_mt_fe_dmd_set_reg(handle, 0x87, value);
	}
	else if(handle->on_board_settings.chip_mode == 2)
	{
		_mt_fe_dmd_get_reg(handle, 0x86, &value);
		value |= 0x80;
		_mt_fe_dmd_set_reg(handle, 0x86, value);
	}
	else
	{
		return MtFeErr_NoSupportFunc;
	}


	/*Do not sleep any time after I2C repeater is opened.*/
	/*please read N bytes.*/


	/*
		TODO:
			write N bytes to tuner
	*/



	/*
		TODO:
			Release the i2c mutex
	*/

#endif
	return MtFeErr_Ok;
}


void _mt_sleep(U32 ticks_ms)
{
	/*TODO*/

	/*
		Wait for ticks_ms time, the time unit is millisecond.
	*/
  mtos_task_sleep(ticks_ms);
}

