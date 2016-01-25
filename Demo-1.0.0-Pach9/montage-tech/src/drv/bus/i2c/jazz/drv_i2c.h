/******************************************************************************/
/******************************************************************************/
#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

/*!
  #include "drv_type.h"
  #include "drv_test_cfg.h"
  */

/*!
    Register Defination
  */
#define  I2C0_BASE_ADDR            0x43000000
/*!
    Register Defination
  */
#define  I2C1_BASE_ADDR            0x4b000000
/*!
    Register Defination
  */
#define  I2C2_BASE_ADDR            0x46000000

/*!
    I2C0
  */

/*!
    0x4300001a
  */
#define  I2C0_PRER_H_REG            (I2C0_BASE_ADDR + 0x1a)
/*!
    0x4300001e
  */
#define  I2C0_PRER_L_REG            (I2C0_BASE_ADDR + 0x1e)
/*!
    0x4300000a
  */
#define  I2C0_TXR_REG               (I2C0_BASE_ADDR + 0x0a)  
/*!
    0x4300000e
  */
#define  I2C0_RXR_REG              (I2C0_BASE_ADDR + 0x0e)
/*!
    0x43000006
  */
#define  I2C0_SR_REG               (I2C0_BASE_ADDR + 0x06)  
/*!
    0x43000012
  */
#define  I2C0_CTR_REG               (I2C0_BASE_ADDR + 0x12)  
/*!
    0x43000016
  */
#define  I2C0_CR_REG               (I2C0_BASE_ADDR + 0x16)  

/*!
    0x4300001a
  */
#define  I2C1_PRER_H_REG            (I2C1_BASE_ADDR + 0x1a)  
/*!
    0x4300001e
  */
#define  I2C1_PRER_L_REG            (I2C1_BASE_ADDR + 0x1e)  
/*!
    0x4300000a
  */
#define  I2C1_TXR_REG               (I2C1_BASE_ADDR + 0x0a)  
/*!
    0x4300000e
  */
#define  I2C1_RXR_REG              (I2C1_BASE_ADDR + 0x0e)
/*!
    0x43000006
  */
#define  I2C1_SR_REG               (I2C1_BASE_ADDR + 0x06)  
/*!
    0x43000012
  */
#define  I2C1_CTR_REG               (I2C1_BASE_ADDR + 0x12)  
/*!
    0x43000016
  */
#define  I2C1_CR_REG               (I2C1_BASE_ADDR + 0x16)  

/*!
    0x4300001e
  */
#define  I2C2_PRER_H_REG            (I2C2_BASE_ADDR + 0x1b)  
/*!
    0x4300001e
  */
#define  I2C2_PRER_L_REG            (I2C2_BASE_ADDR + 0x1f)  
/*!
    0x4300001e
  */
#define  I2C2_TXR_REG               (I2C2_BASE_ADDR + 0x0b)  
/*!
    0x4300001e
  */
#define  I2C2_RXR_REG              (I2C2_BASE_ADDR + 0x0f)
/*!
    0x4300001e
  */
#define  I2C2_SR_REG               (I2C2_BASE_ADDR + 0x07)  
/*!
    0x4300001e
  */
#define  I2C2_CTR_REG               (I2C2_BASE_ADDR + 0x13)  
/*!
    0x4300001e
  */
#define  I2C2_CR_REG               (I2C2_BASE_ADDR + 0x17)  


/*!
    Register Bit Defination 
  */
/*!
    I2c interface enable
  */
#define I2C_CTR_EN            0x80  
/*!
    I2c interface enable
  */
#define I2C_CTR_IEN            0x40
/*!
    Reserved
  */
#define I2C_CTR_RSV6_0        0x3F  

/*!
    Start Transmit
  */
#define I2C_CR_STA            0x80  
/*!
    Stop Transmit
  */
#define I2C_CR_STP            0x40
/*!
    Wr start
  */
#define I2C_CR_WR            0x20   
/*!
    Rd start
  */
#define I2C_CR_RD            0x10   
/*!
    Intterrupt acknowledge
  */
#define I2C_CR_IACK            0x08
/*!
    Acknowledge to slave
  */
#define I2C_CR_NACK            0x04
/*!
    Reserved
  */
#define I2C_CR_RSV1_0        0x03  

/*!
    The recieve ACK from slave
  */
#define I2C_SR_RXACK        0x80  
/*!
    I2c bus busy flag
  */
#define I2C_SR_BUSY            0x40
/*!
    I2c bus arbitrage invalidation flag
  */
#define I2C_SR_AL            0x20    
/*!
    Read or Write transmit run flag
  */
#define I2C_SR_TIP            0x10 
/*!
    Interrupts Request flag
  */
#define I2C_SR_IFLAG        0x08   
/*!
    Interrupts Query flag
  */
#define I2C_SR_IFLAG_QUERY    0x04
/*!
    Reserved
  */
#define I2C_SR_RSV2_0        0x07

/*!
    I2c Clock Frequency Caption : PRER = F_sysclk / (5 * F_scl) - 1 (KHz)
  */
#define I2C_PRER_CAPS(x)    ((SYS_APB_CLOCK) / ((5000) * (x)) - (1))
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_READ_MODE0          0x00
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_READ_MODE1          0x01
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_READ_MODE2          0x02
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_READ_MODE3      0x03
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_WRITE_MODE0          0x08
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define SINGLE_WRITE_MODE1          0x09
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define MULTI_READ_MODE0          0x10
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define MULTI_READ_MODE1          0x11
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define MULTI_WRITE_MODE0          0x18
/*!
    Definition of I2C0 and I2C1 control Functions
  */
#define MULTI_WRITE_MODE1          0x19
/*!
    Definition of the overtime which the i2c wait for
  */
#define TRANS_TIMEOUT         0x880000

/*!
    I2C0
  */
extern void i2c0init(u32 busclk_khz);
/*!
  comments
  */
extern s32  i2c0read (u32 mode, u8 addr, u32 reg, u8 *regval);
/*!
  comments
  */
extern s32  i2c0write (u32 mode, u8 addr, u32 reg, u8 val);
/*!
  comments
  */
extern s32  i2c0burstread (u32 mode, u8 addr, u32 reg, u32 rdlen, u8 *regval);
/*!
  comments
  */
extern s32  i2c0burstwrite (u32 mode, u8 addr, u32 reg, u32 wrlen, u8 *regval);

/*!
  //#ifdef DRV_I2C0_TEST
  */
/*!
  comments
  */
extern s32  i2c0dumpreg(void);
/*!
  //#endif
  */
  
/*!
    I2C0
  */
extern void i2c0init(u32 busclk_khz);
/*!
  comments
  */
extern s32  i2c0read (u32 mode, u8 addr, u32 reg, u8 *regval);
/*!
  comments
  */
extern s32  i2c0write (u32 mode, u8 addr, u32 reg, u8 val);
/*!
  comments
  */
extern s32  i2c0burstread (u32 mode, u8 addr, u32 reg, u32 rdlen, u8 *regval);
/*!
  comments
  */
extern s32  i2c0burstwrite (u32 mode, u8 addr, u32 reg, u32 wrlen, u8 *regval);

/*!
  //#ifdef DRV_I2C1_TEST
  */
/*!
  comments
  */
extern s32  i2c0dumpreg(void);
/*!
  //#endif
  */


#endif //__DRV_I2C_H__
