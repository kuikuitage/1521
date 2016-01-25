/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
//#include "sys_regs_magic.h"
#include "mtos_misc.h"
#include "mtos_fifo.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "drv_dev.h"
#include "hal_timer.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "uio.h"
#include "../uio_priv.h"

#if 0
 #define FP_PRINTF mtos_printk
#else
 #define FP_PRINTF DUMMY_PRINTF
#endif

#define READ_SLAVE_ADDR 0xA1
#define WRITE_SLAVE_ADDR 0xA0


#define FP_INVALIDE_KEY             0xFFFF
#define FP_JITTER_THRESHOLD         60  //unit: ms, to avoid jitter
#define FP_MAX_COM_NUM              4
#define FP_LOCK_LED_INDEX           0
#define FP_POWER_LED_INDEX          1
#define FP_COLON_LED_INDEX      2

/*!
 * FP Type
 */
typedef enum
{
    FP_TYPE_UNDEFINE = 0,
    FP_TYPE_HD1650,
    FP_TYPE_TM1635
} fp_hw_type_t;

static fp_hw_type_t fp_type = FP_TYPE_UNDEFINE;
/*!
 * EM153 register
 */
typedef enum
{
  GMI_MCU_REG_VERSION = 0x0,
  GMI_MCU_REG_CONTROL,
  GMI_MCU_REG_KEY_PRESS,
  GMI_MCU_REG_RESERVED_3,
  GMI_MCU_REG_TIMER_COUNT,
  GMI_MCU_REG_RESERVED_5,
  GMI_MCU_REG_RESERVED_6,
  GMI_MCU_REG_RESERVED_7,
  GMI_MCU_REG_RESERVED_8,
  GMI_MCU_REG_RESERVED_9,
  GMI_MCU_REG_RESERVED_10,
  GMI_MCU_REG_IR_POWER_KEY_1 = 0x11,
  GMI_MCU_REG_IR_POWER_KEY_2,
  GMI_MCU_REG_NUM
}gmi_mcu_reg_t;

static s32 fp_timer_id = 0xFFFF;

static led_bitmap_t fp_bitmap[] =
{
  {'.', 0x00},
  {'0', 0x3F}, {'1', 0x06}, {'2', 0x5B}, {'3', 0x4F},
  {'4', 0x66}, {'5', 0x6D}, {'6', 0x7D}, {'7', 0x07},
  {'8', 0x7F}, {'9', 0x6F}, {'a', 0x77}, {'A', 0x77},
  {'b', 0x7C}, {'B', 0x7C}, {'c', 0x39}, {'C', 0x39},
  {'d', 0x5E}, {'D', 0x5E}, {'e', 0x79}, {'E', 0x79},
  {'f', 0x71}, {'F', 0x71}, {'g', 0x6F}, {'G', 0x3D},
  {'h', 0x76}, {'H', 0x76}, {'i', 0x04}, {'I', 0x30},
  {'j', 0x0E}, {'J', 0x0E}, {'l', 0x38}, {'L', 0x38},
  {'n', 0x54}, {'N', 0x37}, {'o', 0x5C}, {'O', 0x3F},
  {'p', 0x73}, {'P', 0x73}, {'q', 0x67}, {'Q', 0x67},
  {'r', 0x50}, {'R', 0x77}, {'s', 0x6D}, {'S', 0x6D},
  {'t', 0x78}, {'T', 0x31}, {'u', 0x3E}, {'U', 0x3E},
  {'v', 0x3E}, {'V', 0x3E}, {'x', 0x76}, {'X', 0x76},
  {'y', 0x6E}, {'Y', 0x6E}, {'z', 0x5B}, {'Z', 0x5B},
  {':', 0x00}, {'-', 0x40}, {'_', 0x08}, {' ', 0x00},
};
#define FP_TABLE_SIZE sizeof(fp_bitmap) / sizeof(led_bitmap_t)

typedef struct
{
  u16 dis_buff[FP_MAX_COM_NUM];    /* Buffer for LED display scan */
  u16 last_key_code;     /* last record keycode*/
  u8 lock_en;            /* enable lock LED or not*/
  u8 lock_status;        /* lock LED is light(1) or dark(0)*/
  u8 colon_en;
  u8 colon_status;
  u8 power_en;           /* enable POWER LED or not*/
  u8 power_status;       /* power LED is light(1) or dark(0)*/
  u32 key_cnt;           /* key_cnt*info->com_scan_intv = key jitter threshold*/
  u32 rpt_cnt;           /* rpt_cnt*info->com_scan_intv = rpt interval*/
  s32 timer_id;          /* front panel timer id*/
  u8 signal_strength; /* signal strength */
  fp_cfg_t cfg;          /* panel configuration*/
}fp_priv_t;

extern BOOL uio_check_rpt_key(uio_type_t uio, u8 code, u8 ir_num);

#define HD1650_SYSOFF   0x4800
#define HD1650_SYSON    0x4801
#define HD1650_SLEEPOFF   0x4800
#define HD1650_SLEEPON    0x4804

#define HD1650_8SEG_1_ON    0x4811
#define HD1650_8SEG_2_ON    0x4821
#define HD1650_8SEG_3_ON    0x4831
#define HD1650_8SEG_4_ON    0x4841
#define HD1650_8SEG_5_ON    0x4851
#define HD1650_8SEG_6_ON    0x4861
#define HD1650_8SEG_7_ON    0x4871
#define HD1650_8SEG_8_ON    0x4801

#define HD1650_DIG0   0x68
#define HD1650_DIG1   0x6A
#define HD1650_DIG2   0x6C
#define HD1650_DIG3   0x6E
#define HD1650_GETKEY   0x4F


#define TM1635_DATA_CMD   (0x1 << 6)
#define TM1635_OPT_W  0x0
#define TM1635_OPT_R  0x2
#define TM1635_OPT_ADDR_AUTO  0x0
#define TM1635_OPT_ADDR_FIXED  (0x1 << 2)
#define TM1635_OPT_NORMAL_MODE  0x0
#define TM1635_OPT_TEST_MODE  (0x1 << 3)
#define TM1635_ADDR_CMD   (0x3 << 6)
#define TM1635_SHOW_CMD   (0x2 << 6)
#define TM1635_OPT_SHOW_CLOSE   0x0
#define TM1635_OPT_SHOW_OPEN   (0x1 << 3)

#if 1 //fd650k
/*
****************************hardware related**********************************/

/*
****************************************************************************/

//set the command of system parameter

#define FD650_BIT_ENABLE  0x01 // open/close bit
#define FD650_BIT_SLEEP   0x04 // sleep control bit
#define FD650_BIT_7SEG    0x08 // 7 segment control bit
#define FD650_BIT_INTENS1 0x10 // 1 level brightness
#define FD650_BIT_INTENS2 0x20 // 2 level brightness
#define FD650_BIT_INTENS3 0x30 // 3 level brightness
#define FD650_BIT_INTENS4 0x40 // 4 level brightness
#define FD650_BIT_INTENS5 0x50 // 5 level brightness
#define FD650_BIT_INTENS6 0x60 // 6 level brightness
#define FD650_BIT_INTENS7 0x70 // 7 level brightness
#define FD650_BIT_INTENS8 0x00 // 8 level brightness

#define FD650_SYSOFF 0x0400 // disable display and key input
// enable display and key input
#define FD650_SYSON  (FD650_SYSOFF | FD650_BIT_ENABLE)
#define FD650_SLEEPOFF FD650_SYSOFF // close sleep
#define FD650_SLEEPON (FD650_SYSOFF | FD650_BIT_SLEEP) // open sleep
#define FD650_7SEG_ON (FD650_SYSON | FD650_BIT_7SEG) // enable 7seg mode
#define FD650_8SEG_ON (FD650_SYSON | 0x00) // enable 8seg mode
// enable display, key input, 1 level brightness
#define FD650_SYSON_1 (FD650_SYSON | FD650_BIT_INTENS1)
//......
// enable display, key input, 4 level brightness
#define FD650_SYSON_4 (FD650_SYSON | FD650_BIT_INTENS4)
//......
// enable display, key input, 8 level brightness
#define FD650_SYSON_8 (FD650_SYSON | FD650_BIT_INTENS8)


// command of set data
#define FD650_DIG0 0x1400 // display data on seg 0(need 8-bits-data)
#define FD650_DIG1 0x1500 // display data on seg 1(need 8-bits-data)
#define FD650_DIG2 0x1600 // display data on seg 2(need 8-bits-data)
#define FD650_DIG3 0x1700 // display data on seg 3(need 8-bits-data)

#define FD650_DOT 0x0080 // display radix point

// command of read key input
#define FD650_GET_KEY 0x0700 // read key input and return the key value

#endif


/******************************************************
 *   Internal functions for 2pin MCU: CLK/DATA
 ******************************************************/
#define EM153_DELAY_TIME            25 //unit: us
#define MCU_DELAY_TIME            1 //unit: us
#define MCU_TIMES_OUT             5
#define MCU_DELAY_US              mtos_task_delay_us
#define MCU_DELAY_MS              mtos_task_delay_ms
#define SET_SDA_OUT(p_info) gpio_set_dir(p_info->fdata.pos, GPIO_DIR_OUTPUT)
#define SET_SDA_IN(p_info)  gpio_set_dir(p_info->fdata.pos, GPIO_DIR_INPUT)
#define SET_SDA_HI(p_info)  gpio_set_value(p_info->fdata.pos, GPIO_LEVEL_HIGH)
#define SET_SDA_LO(p_info)  gpio_set_value(p_info->fdata.pos, GPIO_LEVEL_LOW)
#define SET_SCL_OUT(p_info) gpio_set_dir(p_info->fclock.pos, GPIO_DIR_OUTPUT)
#define SET_SCL_IN(p_info)  gpio_set_dir(p_info->fclock.pos, GPIO_DIR_INPUT)
#define SET_SCL_HI(p_info)  gpio_set_value(p_info->fclock.pos, GPIO_LEVEL_HIGH)
#define SET_SCL_LO(p_info)  gpio_set_value(p_info->fclock.pos, GPIO_LEVEL_LOW)

static inline u8 GET_SDA(pan_hw_info_t *p_info)
{
  u8 val = 0;
  gpio_get_value(p_info->fdata.pos, &val);
  return val;
}
static inline u8 GET_SCL(pan_hw_info_t *p_info)
{
  u8 val = 0;
  gpio_get_value(p_info->fclock.pos, &val);
  return val;
}
static RET_CODE mcu_phase_start(pan_hw_info_t *p_info)
{
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);
  SET_SDA_HI(p_info);   /* Set SDA high */
  if(!GET_SCL(p_info))
  {
    MCU_DELAY_US(MCU_DELAY_TIME);
  }
  SET_SCL_HI(p_info);   /* Set SCL high */
  MCU_DELAY_US(MCU_DELAY_TIME);
  if(!GET_SCL(p_info))
  {
    return ERR_STATUS;
  }
  if(!GET_SDA(p_info))
  {
    return ERR_STATUS;
  }
  SET_SDA_LO(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  SET_SCL_LO(p_info);

  return SUCCESS;
}

static RET_CODE mcu_phase_stop(pan_hw_info_t *p_info)
{
  /* Make sure is out */
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);

  SET_SDA_LO(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  SET_SCL_HI(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  if(!GET_SCL(p_info))
  {
    return ERR_STATUS;
  }

  SET_SDA_HI(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  if(!GET_SDA(p_info))
  {
    return ERR_STATUS;
  }

  return SUCCESS;
}

static void mcu_phase_set_bit(pan_hw_info_t *p_info, int val)
{
  /* Make sure is out */
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);

  SET_SCL_LO(p_info);
  if(val)
  {
    SET_SDA_HI(p_info);
  }
  else
  {
    SET_SDA_LO(p_info);
  }
  MCU_DELAY_US(MCU_DELAY_TIME);

  SET_SCL_HI(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  SET_SCL_LO(p_info);

  return;
}


static int mcu_phase_get_bit(pan_hw_info_t *p_info)
{
  int ret = 0;

  SET_SDA_IN(p_info);
  SET_SDA_HI(p_info);       /* Hi Ind */

  MCU_DELAY_US(MCU_DELAY_TIME);
  SET_SCL_HI(p_info);
  MCU_DELAY_US(MCU_DELAY_TIME);
  ret = GET_SDA(p_info);
  SET_SCL_LO(p_info);

  return ret;
}

static RET_CODE mcu_phase_set_byte(pan_hw_info_t *p_info, u8 data)
{
  int i = 0;

  for(i = 0; i < 8; i++)
  {
    if(!(p_info->status & 0x08) || fp_type == FP_TYPE_TM1635)/*For non-fd650k cases*/
    {
      if(data & 0x01)
      {
        mcu_phase_set_bit(p_info, 1);
      }
      else
      {
        mcu_phase_set_bit(p_info,0);
      }

      data >>= 1;
    }
    else
    {
      /*Just for FD650K case*/
      if (data & 0x80)
      {
        mcu_phase_set_bit(p_info, 1);
      }
      else
      {
        mcu_phase_set_bit(p_info, 0);
      }

      data <<= 1;
    }
  }

  return(mcu_phase_get_bit(p_info));
}

static u8 mcu_phase_get_byte(pan_hw_info_t *p_info, int ack)
{
  u8 ret = 0;
  int i = 0;

  for(i = 0; i < 8; i++)
  {
    ret <<= 1;
    ret |= mcu_phase_get_bit(p_info);
  }

  if(!(p_info->status & 0x08) || fp_type == FP_TYPE_TM1635)/*For non-fd650k cases*/
    mcu_phase_set_bit(p_info, ack);//after reading, send  a ack(high_level) to mcu  ???

  return ret;
}

/********************************TM 1635 functions***************************/

static void tm1635_show_string(pan_hw_info_t *p_info, u8 *p_str, u8 led_num)
{
    u8 i = 0;
   // I2C0RawTxWithStop(cmd, 1);
    mcu_phase_start(p_info);
    mcu_phase_set_byte(p_info, TM1635_DATA_CMD | TM1635_OPT_W |
    TM1635_OPT_ADDR_AUTO | TM1635_OPT_NORMAL_MODE);
    mcu_phase_stop(p_info);

    mcu_phase_start(p_info);
    mcu_phase_set_byte(p_info, TM1635_ADDR_CMD);
    for(i = 0; i < led_num; i ++)
    {
        mcu_phase_set_byte(p_info, p_str[i]);
    }
    mcu_phase_stop(p_info);
   // I2C0RawTxWithStop(cmd, 1 + led_num);
    mcu_phase_start(p_info);
    mcu_phase_set_byte(p_info, TM1635_SHOW_CMD | TM1635_OPT_SHOW_OPEN | 0x7);
    mcu_phase_stop(p_info);
    // I2C0RawTxWithStop(cmd,1);
}

static u8 tm1635_readkey(pan_hw_info_t *p_info)
{
    u8 key = 0;
    mcu_phase_start(p_info);
    mcu_phase_set_byte(p_info, TM1635_DATA_CMD | TM1635_OPT_R);
    key = mcu_phase_get_byte(p_info, 1);
    mcu_phase_stop(p_info);
   // I2C0RawTxWithoutStop(cmd, 1);
   // I2C0RawRxWithStop(&key, 1);
    return key;
}

/********************************TM 1635 functions END ***************************/


/********************************HD1650 functions***************************/

/****************************************************************
 *
 *Function Name:HD1650_Write
 *
 *Description:send the command
 *
 *  Parameter:
 *
 *  return: null
****************************************************************/
void hd1650_write(pan_hw_info_t *p_info, u16 cmd) //write command
{
  mcu_phase_start(p_info);
  mcu_phase_set_byte(p_info, (u8)(cmd >> 8) & 0xff);
  mcu_phase_set_byte(p_info, (u8)cmd);
  mcu_phase_stop(p_info);

  return;
}

/****************************************************************
 *
 *Function Name:HD1650_Read
 *
 *Description:read and return the key value, return 0 if no key input
 *
 *  Parameter:
 *
 *  return: key value
****************************************************************/
u8 hd1650_read(pan_hw_info_t *p_info) //read the key value
{
  u8 keycode = 0;

  mcu_phase_start(p_info);
  mcu_phase_set_byte(p_info, (FD650_GET_KEY >> 8) & 0xff);
  keycode = mcu_phase_get_byte(p_info, 1);
  mcu_phase_stop(p_info);

  if((keycode & 0x40) == 0)
    keycode = 0;

  return keycode;
}
/********************************FD650 functions***************************/

/****************************************************************
 *
 *Function Name:FD650_Write
 *
 *Description:send the command
 *
 *  Parameter: cmd¼ûFD650.H
 *
 *  return: null
****************************************************************/
void fd650_write(pan_hw_info_t *p_info, u16 cmd) //write command
{
  mcu_phase_start(p_info);
  mcu_phase_set_byte(p_info, ((u8)(cmd >> 7) & 0x3E) | 0x40);
  mcu_phase_set_byte(p_info, (u8)cmd);
  mcu_phase_stop(p_info);

  return;
}

/****************************************************************
 *
 *Function Name:FD650_Read
 *
 *Description:read and return the key value, return 0 if no key input
 *
 *  Parameter: ÎÞ
 *
 *  return: key value
****************************************************************/
u8 fd650_read(pan_hw_info_t *p_info) //read the key value
{
  u8 keycode = 0;

  mcu_phase_start(p_info);
  mcu_phase_set_byte(p_info, ((FD650_GET_KEY >> 7) & 0x3E) | 0x01 | 0x40);
  keycode = mcu_phase_get_byte(p_info, 1);
  mcu_phase_stop(p_info);

  if((keycode & 0x40) == 0)
    keycode = 0;

  return keycode;
}

static RET_CODE mcu_read_data(pan_hw_info_t *p_info, u8 *p_data)
{
  int i = MCU_TIMES_OUT;
  u8 cmd = 0x42;        //read key command

  if(!(p_info->status & 0x08))
  {
    while(--i)           // Ack polling
    {
      mcu_phase_start(p_info);
      //send cmd data byte out and get ack
      if(!mcu_phase_set_byte(p_info, cmd))
      {
        break;
      }
      // no ack, stop the command and chack again later
      mcu_phase_stop(p_info);
      //os_task_sleep(1);       // wait for 1mS
    }

    if(i == 0)
    {
      return ERR_TIMEOUT;
    }

    *p_data = mcu_phase_get_byte(p_info,1);
  }
  else
  {
    /*Just for fd650k*/
    if(fp_type == FP_TYPE_HD1650)
    {
       *p_data = hd1650_read(p_info);
    }
    else if(fp_type == FP_TYPE_TM1635)
    {
       *p_data = tm1635_readkey(p_info);
    }
    else
    {
        *p_data = fd650_read(p_info);
    }

  }

  return SUCCESS;
}

static RET_CODE mcu_write_data(pan_hw_info_t *p_info,u8 cmd,u8 *p_data, int len)
{
  int i = MCU_TIMES_OUT;

  while(--i)             // Ack polling
  {
    mcu_phase_start(p_info);
    //send cmd data byte out and get ack
    if(!mcu_phase_set_byte(p_info,cmd))
    {
      break;
    }
    // no ack, stop the command and chack again later
    mcu_phase_stop(p_info);
    //os_task_sleep(1);       /* wait for 1mS */
  }

  if(i == 0)
  {
    return ERR_TIMEOUT;
  }

  for(i = 0; i < len; i++){
    mcu_phase_set_byte(p_info,p_data[i]);
  }

  mcu_phase_stop(p_info);

  return SUCCESS;
}

static u8 mcu_rcv_serial_data(pan_hw_info_t *p_info)
{
  u8 data = 0;
  if(SUCCESS == mcu_read_data(p_info,&data))
  {
    return data;
  }
  else
  {
    return 0xFF;
  }
}

static s32 mcu_com_display(pan_hw_info_t *p_info, u16 *p_data, u8 len)
{
  u8 mode = 0x40;    //write mode, auto inccrese
  u8 addr = 0xC0;     //base addr 0
  u8 ctrl = 0x8F;      //diplay on
  u8 i = 0;
  u8 tmp[20] = {0,};

  for(i = 0; i < len; i++)
  {
    if(!(p_info->status & 0x02))
    {
      tmp[i] = p_data[i] & 0xFF;
    }
    else
    {
      tmp[i + 1] = p_data[i] & 0xFF;
    }
  }

  if(!(p_info->status & 0x08))
  {
    if(SUCCESS != mcu_write_data(p_info,mode, tmp, 0))
    {
      //OS_PRINTF("send command mode fail!\n");
      //MT_ASSERT(0);
      return ERR_FAILURE;
    }
    if(SUCCESS != mcu_write_data(p_info, addr, tmp, len))
    {
        return ERR_FAILURE;
    }
    if(SUCCESS != mcu_write_data(p_info, ctrl, tmp, 0))
    {
      //OS_PRINTF("send command ctrl fail!\n");
      //MT_ASSERT(0);
      return ERR_FAILURE;
    }
  }
  else
  {
    /*Just for fd650k*/
    // enable display, key, 7 seg mode and 4 level brightness
    if(fp_type == FP_TYPE_HD1650)
    {
        hd1650_write(p_info, HD1650_8SEG_8_ON);

        hd1650_write(p_info, HD1650_DIG0 | tmp[0]);
        hd1650_write(p_info, HD1650_DIG1 | tmp[1]);
        hd1650_write(p_info, HD1650_DIG2 | tmp[2]);
        hd1650_write(p_info, HD1650_DIG3 | tmp[3]);
    }
    else if(fp_type == FP_TYPE_TM1635)
    {
        tm1635_show_string(p_info, tmp, 4);
    }
    else
    {
        fd650_write(p_info, FD650_SYSON_4|FD650_8SEG_ON);

        fd650_write(p_info, FD650_DIG0 | tmp[0]);
        fd650_write(p_info, FD650_DIG1 | tmp[1]);
        fd650_write(p_info, FD650_DIG2 | tmp[2]);
        fd650_write(p_info, FD650_DIG3 | tmp[3]);
    }

  }
  return SUCCESS;
}
/******************************************************
 *   Internal for 3pin MCU: STB/CLK/DATA
 ******************************************************/

/*!
  Send out serial data byte at close rise edge

  \param[data] A byte to send out
  \param[cs] Enable the chip select pin or not
 */
static void mcu3_send_serial_data(pan_hw_info_t *p_info, u8 data, u8 cs)
{
  u8 i = 0;
  if(TRUE == cs)
    gpio_set_value(p_info->flatch.pos, p_info->flatch.polar);

  //FP_PRINTF("output data 0x%x:", data);
  for(i = 0; i < 8; i++)
  {
    gpio_set_value(p_info->fclock.pos, !p_info->fclock.polar);
    gpio_set_value(p_info->fdata.pos, (data >> i) & 0x1 ?
                    p_info->fclock.polar : !p_info->fclock.polar);
    //FP_PRINTF(" %d ",  (data>>i)&0x1);
    gpio_set_value(p_info->fclock.pos, p_info->fclock.polar);
  }
  FP_PRINTF("\n");
  if(TRUE == cs)
    gpio_set_value(p_info->flatch.pos, !p_info->flatch.polar);
}

static u8 mcu3_rcv_serial_data(pan_hw_info_t *p_info)
{
  u8 i = 0, tmp = 0, j = 0;
  u16 data = 0, key = 0;
  u8 mode = 0x42;

  gpio_set_value(p_info->flatch.pos, p_info->flatch.polar);    //enable cs
  mcu3_send_serial_data(p_info, mode, FALSE);       //send rcv cmd

  MCU_DELAY_US(3);
  gpio_set_value(p_info->fclock.pos, !p_info->fclock.polar);

  data = 0;
  gpio_set_dir(p_info->fdata.pos, GPIO_DIR_INPUT); //set to input
  for(j = 0; j < FP_MAX_COM_NUM; j++)
  {
    data = 0;
    //FP_PRINTF("\ntmp: ");
    for(i = 0; i < 8; i++)
    {
      tmp = 0;
      //read data @rise edge
      gpio_set_value(p_info->fclock.pos, p_info->fclock.polar);
      gpio_get_value(p_info->fdata.pos, &tmp);
      data |= (tmp << i);
      //FP_PRINTF(" %d ", tmp);
      //MCU_DELAY_US(5);
      gpio_set_value(p_info->fclock.pos, !p_info->fclock.polar);
      //MCU_DELAY_US(5);
    }
    if(data)
    {
      //fixme: don't know the reason
      if(p_info->status & 0x04)
      {
        key = data | (j << 6);
      }
      else
      {
        key = data | (j << 4);
      }
      //FP_PRINTF("\n##get key 0x%x\n", key);
    }
  }
  gpio_set_dir(p_info->fdata.pos, GPIO_DIR_OUTPUT);  //restore to output
  gpio_set_value(p_info->flatch.pos, !p_info->flatch.polar);

  return key;
}


static void mcu3_send_data(pan_hw_info_t *p_info, u8 addr, u8 *p_data, u8 len)
{
  u8 i = 0;

  gpio_set_value(p_info->flatch.pos, p_info->flatch.polar);

  //send addr
  mcu3_send_serial_data(p_info,addr, FALSE);

  //send data
  for(i = 0; i < len; i++)
  {
    mcu3_send_serial_data(p_info,p_data[i * 2], FALSE);             //send Dig1-7
    mcu3_send_serial_data(p_info,p_data[i * 2 + 1], FALSE);          //send Dig8-14
  }

  gpio_set_value(p_info->flatch.pos, !p_info->flatch.polar);
}
static void mcu3_com_display(pan_hw_info_t *p_info, u16 *p_data, u8 len)
{
  u8 led = 0x0;          // 4 com, 8segments
  u8 mode = 0x40;    //write mode, auto inccrese
  u8 addr = 0xC0;     //base addr 0
  u8 ctrl = 0x8F;      //diplay on,

  u16 i = 0, tmp[8] = {0,};

  if(p_info->status & 0x02)
  {
    for(i = 0; i < len; i++)
    {
       tmp[i] = p_data[i];
    }
    //for Gospell HT1628B panel, only 6*12 and 7*11 is supported
    //if default led=0, there would be some issue with key scanning, garbage!
    led = 0x2;
    len = 6;
  }
  //FP_PRINTF("led:%x\n", led);
  mcu3_send_serial_data(p_info, led, TRUE);
  //FP_PRINTF("mode:%x\n", mode);
  mcu3_send_serial_data(p_info, mode, TRUE);
  //FP_PRINTF("send data...\n");

  if(p_info->status & 0x02)
  {
      mcu3_send_data(p_info, addr, (u8 *)tmp, len);
  }
  else
  {
      mcu3_send_data(p_info, addr, (u8 *)p_data, len);
  }
  //FP_PRINTF("ctrl:%x\n",ctrl);
  mcu3_send_serial_data(p_info, ctrl, TRUE);
}

/******************************************************
 *   Internal common functions
 ******************************************************/



static void mcu_panel_init(pan_hw_info_t *p_hwinfo, u8 en)
{
  u32 i = 0;
  u8 len = 0;
  pan_gpio_info_t *p_info = &(p_hwinfo->flatch);
  pan_gpio_info_t *p_lbd = &(p_hwinfo->lbd[0]);

  //lbd[3]== (~0x01) means use gpio to ctl lock led,such as PANTE fp
  if(((p_lbd + 3)->pos & 0x01) == 0)
  {
    len = 16;
  }
  else
  {
    len = 13;
  }
  for (i = 0; i < len; i++)
  {
    if(63 != p_info->pos)
    {
      hal_pinmux_gpio_enable(p_info->pos, TRUE);
      gpio_ioctl(GPIO_CMD_IO_ENABLE, p_info->pos, en); //enable GPIO pin
    }
    gpio_set_value(p_info->pos, !p_info->polar);
    gpio_set_dir(p_info->pos, p_info->io);
    p_info++;
  }
}


/******************************************************
 *   External APIs
 ******************************************************/
static RET_CODE mcu_panel_display(lld_uio_t *p_lld,u8 *p_data, u32 len)
{
  int i = 0, j = 0;
  u8 tmp[FP_MAX_COM_NUM] = {0, };
  u8 offset = 0;
  u8 lock_pos = 0, power_pos = 0, colon_pos = 0;
  fp_priv_t *p_priv = (fp_priv_t *)p_lld->p_fp;
  u8 buf1 = 0;

  if(len > FP_MAX_COM_NUM)
  {
    return ERR_FAILURE;
  }

  if(((pan_hw_info_t *)p_priv->cfg.p_info)->num_com == 0x0F) //em153 do not have
  {
     return SUCCESS;
  }
  memset(tmp, 0x20, sizeof(tmp));//space ascii value == 0x20

  len = (len > p_priv->cfg.p_info->num_com) ? p_priv->cfg.p_info->num_com : len;

  if(0 != (p_priv->cfg.p_info->status & 0x1))  //revert display order
  {
    if (p_priv->cfg.p_info->status & 0x40) //change display order
    {
      buf1 = p_data[0];
      for(i = 0; i < (len - 1); i++)
      {
        p_data[i] = p_data[i + 1];
      }
      p_data[len - 1] = buf1;
    }

    if ((len < p_priv->cfg.p_info->num_com) && (p_priv->cfg.p_info->status & 0x10))
    {
       for (i = 0; i < len; i++)
      {
        tmp[i + 1] = p_data[len - 1 - i];
      }
    }
     else if(p_priv->cfg.p_info->status & 0x20)
    {
      for (i = 0; i < 3; i++)
      {
        tmp[i] = p_data[len - 1 - i];
      }
    }
     else
    {
      for (i = 0; i < len; i++)
      {
        tmp[i] = p_data[len - 1 - i];
      }
    }
  }
  else  //normal order
  {
    if (p_priv->cfg.p_info->status & 0x40) //change display order
    {
      buf1 = p_data[len - 1];
      for(i = (len - 1); i > 0; i--)
      {
        p_data[i] = p_data[i - 1];
      }
      p_data[0] = buf1;
    }
     if(p_priv->cfg.p_info->status & 0x20)
    {
      for (i = 0; i < 3; i++)
      {
        tmp[i + 1] = p_data[i];
      }
      tmp[0] = p_data[i];
    }
    else if(p_priv->cfg.p_info->status & 0x10)
    {
      for (i = 0; i < 3; i++)
      {
        tmp[i] = p_data[i + 1];
      }
      tmp[i] = p_data[0];
    }
    else
    {
      for (i = 0; i < len; i++)
      {
        tmp[i] = p_data[i];
      }
    }
  }

  //find the lock bit
  offset = p_priv->cfg.p_info->lbd[FP_LOCK_LED_INDEX].pos & 0x0F;
  lock_pos = (p_priv->cfg.p_info->lbd[FP_LOCK_LED_INDEX].pos & 0x30) >> 4;
  power_pos = (p_priv->cfg.p_info->lbd[FP_POWER_LED_INDEX].pos & 0x30) >> 4;
  colon_pos = (p_priv->cfg.p_info->lbd[FP_COLON_LED_INDEX].pos & 0x30) >> 4;

  for (i = 0; i < FP_MAX_COM_NUM; i++)//old: for (i = 0; i < len; i++)
  {
    for(j = 0; j < p_priv->cfg.map_size; j++)
    {
      if(p_priv->cfg.p_map[j].ch == tmp[i])
      {
        p_priv->dis_buff[i] = p_priv->cfg.p_map[j].bitmap;
        break;
      }
    }
    // if lock led is enabled, set the corresponding bit
    if(TRUE == p_priv->lock_en)
    {
      if((i == lock_pos) && !(p_priv->cfg.p_info->status & 0x04))
      {
        p_priv->dis_buff[i] = p_priv->lock_status ?
            (p_priv->dis_buff[i]|(1 << offset)):(p_priv->dis_buff[i] & ~(1 << offset));
      }
    }

    if(TRUE == p_priv->colon_en)
    {
      if(i == colon_pos)
      {
        offset = p_priv->cfg.p_info->lbd[FP_COLON_LED_INDEX].pos & 0x0F;
        p_priv->dis_buff[i] = p_priv->colon_status ?
            (p_priv->dis_buff[i]|(1 << offset)):(p_priv->dis_buff[i] & ~(1 << offset));
      }
    }


    // if power led is enabled, set the corresponding bit
    if(TRUE == p_priv->power_en)
    {
      if(i == power_pos)
      {
        offset = p_priv->cfg.p_info->lbd[FP_POWER_LED_INDEX].pos & 0x0F;
        p_priv->dis_buff[i] = p_priv->power_status ?
     (p_priv->dis_buff[i]|(1 << offset)):(p_priv->dis_buff[i] & ~(1 << offset));
      }
    }
  }

  // for the special gospell frontpanel
  if(p_priv->cfg.p_info->status & 0x1)
  {
    if(p_priv->cfg.p_info->status & 0x10)
    {
      //if(fpriv.signal_strength != 0)
      {
        p_priv->dis_buff[0]  = p_priv->signal_strength;
      }
    }
  }
  else
  {
    if(p_priv->cfg.p_info->status & 0x20)
    {
      //if(fpriv.signal_strength != 0)
      {
        p_priv->dis_buff[0]  = p_priv->signal_strength;
      }
    }
    else if(p_priv->cfg.p_info->lbd[1].pos == 0)
    {
      p_priv->dis_buff[0]  = p_priv->signal_strength;
    }
  }

  if(63 == p_priv->cfg.p_info->flatch.pos) // for 2pin MCU
  {
    mcu_com_display(p_priv->cfg.p_info, p_priv->dis_buff, FP_MAX_COM_NUM);
  }
  else  // for 3pin MCU
  {
    mcu3_com_display(p_priv->cfg.p_info, p_priv->dis_buff, FP_MAX_COM_NUM);
  }
  return SUCCESS;
}

void mcu_set_bitmap(lld_uio_t *p_lld, led_bitmap_t *p_map, u32 size)
{
  fp_priv_t *p_panel = p_lld->p_fp;
  if(NULL == p_map)
  {
    p_panel->cfg.p_map = fp_bitmap;
    p_panel->cfg.map_size = FP_TABLE_SIZE;
  }
  else
  {
    p_panel->cfg.p_map = p_map;
    p_panel->cfg.map_size = size;
  }
}

static RET_CODE mcu_panel_stop(lld_uio_t *p_lld)
{
  fp_priv_t *p_priv = (fp_priv_t *)p_lld->p_fp;

  hal_timer_release(p_priv->timer_id);
  mcu_panel_init(p_priv->cfg.p_info, 0);
  if(NULL != p_lld->p_fp)
  {
      mtos_free(p_lld->p_fp);
      p_lld->p_fp = NULL;
  }
  return SUCCESS;
}
//em153 driver not more than 25K

static RET_CODE em153_phase_start(pan_hw_info_t *p_info)
{
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);
  SET_SDA_HI(p_info);   /* Set SDA high */
  if(!GET_SCL(p_info))
  {
    MCU_DELAY_US(EM153_DELAY_TIME);
  }
  SET_SCL_HI(p_info);   /* Set SCL high */
  MCU_DELAY_US(EM153_DELAY_TIME);
  if(!GET_SCL(p_info))
  {
    return ERR_STATUS;
  }
  if(!GET_SDA(p_info))
  {
    return ERR_STATUS;
  }
  SET_SDA_LO(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  SET_SCL_LO(p_info);

  return SUCCESS;
}

static RET_CODE em153_phase_stop(pan_hw_info_t *p_info)
{
  /* Make sure is out */
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);

  SET_SDA_LO(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  SET_SCL_HI(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  if(!GET_SCL(p_info))
  {
    return ERR_STATUS;
  }

  SET_SDA_HI(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  if(!GET_SDA(p_info))
  {
    return ERR_STATUS;
  }

  return SUCCESS;
}

static void em153_phase_set_bit(pan_hw_info_t *p_info, int val)
{
  /* Make sure is out */
  SET_SDA_OUT(p_info);
  SET_SCL_OUT(p_info);

  SET_SCL_LO(p_info);
  if(val)
  {
    SET_SDA_HI(p_info);
  }
  else
  {
    SET_SDA_LO(p_info);
  }
  MCU_DELAY_US(EM153_DELAY_TIME);

  SET_SCL_HI(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  SET_SCL_LO(p_info);

  return;
}


static int em153_phase_get_bit(pan_hw_info_t *p_info)
{
  int ret = 0;

  SET_SDA_IN(p_info);
  SET_SDA_HI(p_info);       /* Hi Ind */

  MCU_DELAY_US(EM153_DELAY_TIME);
  SET_SCL_HI(p_info);
  MCU_DELAY_US(EM153_DELAY_TIME);
  ret = GET_SDA(p_info);
  SET_SCL_LO(p_info);

  return ret;
}

static RET_CODE em153_phase_set_byte(pan_hw_info_t *p_info, u8 data)
{
  int i = 0;

  for(i = 0; i < 8; i++)
  {
      if (data & 0x80)
      {
        em153_phase_set_bit(p_info, 1);
      }
      else
      {
        em153_phase_set_bit(p_info, 0);
      }

      data <<= 1;
  }

  return(em153_phase_get_bit(p_info));
}

static u8 em153_phase_get_byte(pan_hw_info_t *p_info, int ack)
{
  u8 ret = 0;
  int i = 0;

  for(i = 0; i < 8; i++)
  {
    ret <<= 1;
    ret |= em153_phase_get_bit(p_info);
  }

    em153_phase_set_bit(p_info, ack);//after reading, send  a ack(high_level) to mcu  ???

  return ret;
}





static RET_CODE em153_read_no_stop(pan_hw_info_t *p_info, u8 slave_addr, u8 *p_buf,  int len)
{
  int i = MCU_TIMES_OUT;

  slave_addr |= 0x01;     /* Read */
  while(--i)        /* Ack polling !! */
  {
    em153_phase_start(p_info);
    /* has /ACK => i2c_gpio_phase_start transfer */
    if(!em153_phase_set_byte(p_info, slave_addr))
    {
      break;
    }
    /* device is busy, issue i2c_gpio_phase_stop and chack again later */
    em153_phase_stop(p_info);
    MCU_DELAY_US(100);  /* wait for 100uS */
  }

  if(i == 0)
  {
    return ERR_TIMEOUT;
  }

  for(i = 0; i < (len - 1); i++)
  {
    /*with no /ack to stop process */
    p_buf[i] = em153_phase_get_byte(p_info, 0);
  }
  p_buf[len - 1] = em153_phase_get_byte(p_info, 1);

  return SUCCESS;
}

static RET_CODE em153_write_no_stop(pan_hw_info_t *p_info,u8 slave_addr, u8 *p_buf,  int len)
{
  int i = MCU_TIMES_OUT;
  slave_addr &= 0xFE;   /*Write*/

  while(--i)       /* Ack polling !! */
  {
    em153_phase_start(p_info);
    /* has /ACK => em153_phase_start transfer */
    if(0 == em153_phase_set_byte(p_info, slave_addr))
    {
      break;
    }
    /* device is busy, issue em153_phase_stop and chack again later */
    em153_phase_stop(p_info);
    MCU_DELAY_US(100);  /* wait for 100uS */
  }
  if(i == 0)
  {
    return ERR_TIMEOUT;
  }

  for(i = 0; i < len; i++)
  {
    em153_phase_set_byte(p_info, p_buf[i]);
  }

  return SUCCESS;
}


static RET_CODE em153_read(pan_hw_info_t *p_info, u8 slave_addr, u8 *p_buf, u32 size, u32 param)
{
  RET_CODE ret = 0;

  if((ret = em153_read_no_stop(p_info, slave_addr, p_buf, size)) != SUCCESS)
  {
    FP_PRINTF("@@i2c read error!\n");
    return ret;
  }
  em153_phase_stop(p_info);

  return SUCCESS;
}
static RET_CODE em153_write(pan_hw_info_t *p_info,
                      u8 slave_addr, u8 *p_buf, u32 size, u32 param)
{
  RET_CODE ret = 0;

  if((ret = em153_write_no_stop(p_info, slave_addr, p_buf, size)) != SUCCESS)
  {
    FP_PRINTF("@@i2c write error!\n");
    return ret;
  }
  em153_phase_stop(p_info);

  return SUCCESS;
}

static RET_CODE em153_seq_read(pan_hw_info_t *p_info,
                                       u8 slave_addr,
                                       u8 *p_buf,
                                       u32 wlen,
                                       u32 rlen,
                                       u32 param)
{
  RET_CODE ret = 0;

  if(wlen == 0)
  {
    return em153_read(p_info, slave_addr, p_buf, rlen, param);
  }

  ret = em153_write_no_stop(p_info, slave_addr, p_buf, wlen);
  if(SUCCESS != ret)
  {
    FP_PRINTF("@@i2c seq_read error1!\n");
    return ret;
  }

  ret = em153_read_no_stop(p_info, slave_addr, p_buf, rlen);
  if(SUCCESS != ret)
  {
    FP_PRINTF("@@i2c seq_read error2!\n");
    return ret;
  }

  em153_phase_stop(p_info);

  return SUCCESS;
}
static void em153_get_version(pan_hw_info_t *p_info)
{
u8 buf[6] ={0},ret = 0;
u32 wlen = 0;
u32 rlen = 0;

  buf[0] = GMI_MCU_REG_VERSION;
    buf[1] = 0x00;
    buf[2] = 0x00;

    buf[3] = 0x00;
    buf[4] = 0x00;
    wlen = 1;
    rlen = 4;

    ret = em153_seq_read(p_info, READ_SLAVE_ADDR, buf, wlen, rlen, 0);
    FP_PRINTF(" ret =%d , gmi version: 0x%x %x %x %x \n ",ret, \
        buf[0], buf[1], buf[2], buf[3]);
}
/******************************************************
 *   Internal common functions
 ******************************************************/
static void em153_set_key_press(pan_hw_info_t *p_info, u32 key)
{
  u8 buf[6] = {0};
  u32 size = 0;

  buf[0] = GMI_MCU_REG_KEY_PRESS;
  buf[1] = key >> 24;
  buf[2] = key >> 16;
  buf[3] = key >> 8;
  buf[4] = key ;
 // buf[1] = 0x80;
 // buf[2] = 0x80;
 // buf[3] = 0x00;
//buf[4] = 0x00;
  size = 5;

  em153_write(p_info, WRITE_SLAVE_ADDR, buf, size, 0);
}

static void em153_set_timer_count(pan_hw_info_t *p_info, u32 timerout)
{
  u8 buf[6] = {0};
  u32 size = 0;
  buf[0] = GMI_MCU_REG_TIMER_COUNT;
  buf[1] = timerout >> 24;
  buf[2] = timerout >> 16;
  buf[3] = timerout >> 8;
  buf[4] = timerout ;
  size = 5;

  em153_write(p_info, WRITE_SLAVE_ADDR, buf, size, 0);
}

static void em153_set_ir_power_key1(pan_hw_info_t *p_info, u32 key)
{
  u8 buf[6] = {0};
  u32 size = 0;

  buf[0] = GMI_MCU_REG_IR_POWER_KEY_1;
  /*
  buf[1] = 0x00;
  buf[2] = 0xFF;
  buf[3] = 0x78;
  buf[4] = 0x87;
  */
  buf[1] = key >> 24;
  buf[2] = key >> 16;
  buf[3] = key >> 8;
  buf[4] = key ;
  size = 5;

  em153_write(p_info, WRITE_SLAVE_ADDR, buf, size, 0);
}

static void em153_set_ir_power_key2(pan_hw_info_t *p_info, u32 key)
{
  u8 buf[6] = {0};
  u32 size = 0;

  buf[0] = GMI_MCU_REG_IR_POWER_KEY_2;
  /*
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x1e;
  */
  buf[1] = key >> 24;
  buf[2] = key >> 16;
  buf[3] = key >> 8;
  buf[4] = key ;
  size = 5;

  em153_write(p_info, WRITE_SLAVE_ADDR, buf, size, 0);
}

static void em153_stby_led_disable(pan_hw_info_t *p_info, u8 param)
{
  u8 buf[6] = {0};
  u32 size = 0;

  buf[0] = GMI_MCU_REG_CONTROL;
  buf[1] = 0x80;
  buf[2] = 0x00;
  buf[3] = 0x00;
  if(param & 0x01)
  {
    buf[4] = 0x00;
  }
  else
  {
    buf[4] = 0x10;
  }
  size = 5;
  em153_write(p_info, WRITE_SLAVE_ADDR, buf, size, 0);
}


static void mcu_panel_timer_cb(void)
{
  u16 data = 0;
  static u32 cnt = 0;
  static u16 valid_data = FP_INVALIDE_KEY;
  u8 buf[5];
  u16 wlen = 0;
  u16 rlen = 0;
  u32 sr = 0;
  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
  fp_priv_t *p_fp = (fp_priv_t *)p_lld->p_fp;
  pan_hw_info_t *p_info = p_fp->cfg.p_info;

  if(p_fp->cfg.p_info->num_com == 0x0F)
  {

        buf[0] = GMI_MCU_REG_KEY_PRESS;
        buf[1] = 0x80;
        buf[2] = 0x00;

        buf[3] = 0x00;
        buf[4] = 0x00;
        wlen = 1;
        rlen = 4;
      mtos_critical_enter(&sr);
        em153_seq_read(p_info, READ_SLAVE_ADDR, buf, wlen, rlen, 0);
      mtos_critical_exit(sr);
        data = buf[3];

    }
  else
  {
  if(63 == p_fp->cfg.p_info->flatch.pos) // for 2pin MCU
  {
    if(SUCCESS != mcu_com_display(p_info, p_fp->dis_buff, FP_MAX_COM_NUM))
      return;
    data = mcu_rcv_serial_data(p_info);
  }
  else  // for 3pin MCU
  {
    mcu3_com_display(p_info, p_fp->dis_buff, FP_MAX_COM_NUM);
    data = mcu3_rcv_serial_data(p_info);
  }
  }


  if((0 == data) || (0xFF == data))
  {
    p_fp->last_key_code = FP_INVALIDE_KEY;
    cnt = 0;
    valid_data = FP_INVALIDE_KEY;
    return;
  }

  if(data == p_fp->last_key_code)
  {
    cnt++;
    if((cnt > p_fp->rpt_cnt)
      && (TRUE == uio_check_rpt_key(UIO_FRONTPANEL, (u8)data, 0)))
    {
      mtos_fifo_put(&p_priv->fifo, data | (UIO_FRONTPANEL << 8));
      //OS_PRINTF("Got repeat key: %x\n", data);
      cnt = 0;
    }
  }
  else if(valid_data != data)
  {
    valid_data = data;
    cnt = 1;
  }
  else
  {
  cnt++;
  if(cnt >= p_fp->key_cnt)
  {
      mtos_fifo_put(&p_priv->fifo, data | (UIO_FRONTPANEL << 8));
      p_fp->last_key_code = data;
      //OS_PRINTF("Got first key: %x\n", data);
    }
  }
  }

static RET_CODE mcu_panel_io_ctrl(lld_uio_t *p_lld,u32 cmd, u32 param)
{
  RET_CODE ret = SUCCESS;
  u16 data = 0;
  fp_priv_t *p_priv = (fp_priv_t *)p_lld->p_fp;
  pan_hw_info_t *p_info = p_priv->cfg.p_info;
  pan_gpio_info_t *p_lbd = &(p_info->lbd[0]);
  u16 offset = 0;
  u32 sr = 0;
  switch(cmd)
  {
    case UIO_FP_SET_FW_TYPE:
      fp_type = param;
      break;
    case UIO_FP_POWER_KEY_PRESS:
      if(p_info->num_com == 0x0F) //0x0F is em153
      {
        mtos_critical_enter(&sr);
        em153_set_key_press(p_info,param);
        mtos_critical_exit(sr);
      }
      break;

    case UIO_FP_IR_KEY_ONE:
      if(p_info->num_com == 0x0F) //0x0F is em153
      {
        mtos_critical_enter(&sr);
        em153_set_ir_power_key1(p_info,param);
        mtos_critical_exit(sr);
      }
      break;

    case UIO_FP_IR_KEY_TWO:
      if(p_info->num_com == 0x0F) //0x0F is em153
      {
        mtos_critical_enter(&sr);
        em153_set_ir_power_key2(p_info,param);
        mtos_critical_exit(sr);
      }
      break;

    case UIO_FP_TIME_OUTO:
      if(p_info->num_com == 0x0F) //0x0F is em153
      {
         mtos_critical_enter(&sr);
         em153_set_timer_count(p_info,param);
         mtos_critical_exit(sr);
      }
      break;

    case UIO_FP_SET_POWER_LBD:
      /* POWER led pos, 6 bit, low 4bit:which bit is used to control the Lock LED,
       * high 2bit: which digit is linked to Lock LED*/
      mtos_task_lock();
      if(p_info->num_com == 0x0F) //0x0F is em153
      {
        mtos_critical_enter(&sr);
        em153_stby_led_disable(p_info, param);
        mtos_critical_exit(sr);
      }
      else
      {
        if(p_info->lbd[FP_POWER_LED_INDEX].pos == 63 || p_info->lbd[FP_POWER_LED_INDEX].pos == 0)
        {
          break;
        }
        p_priv->power_en = TRUE;
        offset = p_info->lbd[FP_POWER_LED_INDEX].pos & 0x0F;
        data = p_priv->dis_buff[(p_info->lbd[FP_POWER_LED_INDEX].pos & 0x30) >> 4];

        if(0 != param)
        {
          data |= (1 << offset);
          p_priv->power_status = TRUE;
        }
        else
        {
          data &= ~(1 << offset);
          p_priv->power_status = FALSE;
        }
        p_priv->dis_buff[(p_info->lbd[FP_POWER_LED_INDEX].pos & 0x30) >> 4] = data;
      }
      mtos_task_unlock();
      break;

    case UIO_FP_SET_LOCK_LBD:
      mtos_task_lock();
      /*GPIO control lock LED*/
      // note: default value of lbd[3] is 63
      if(((p_lbd + 3)->pos & 0x01) == 0)//lbd[3]!= 0x01 means use gpio to ctl lock led
      {
        p_priv->lock_en = FALSE;
        if(((p_lbd + 3)->pos & 0x02) == 0)//lbd[3]!= 0x2 means low_level lighten the lock led
        {
          gpio_set_value(p_lbd->pos, !param);
        }
        else
        {
          gpio_set_value(p_lbd->pos, param);
        }
        /*
        if(p_info->status & 0x04)
        {
          if(param)
          {
            gpio_set_value(p_info->lbd[FP_LOCK_LED_INDEX].pos, GPIO_LEVEL_HIGH);
          }
          else
          {
            gpio_set_value(p_info->lbd[FP_LOCK_LED_INDEX].pos, GPIO_LEVEL_LOW);
          }
          break;
        }*/
      }
      else
      {
        /* panel MCU control LOCK,
       * pos usage(6 bit) - low 4bit:which bit is used to control the Lock LED,
       * high 2bit: which digit is linked to Lock LED */
        p_priv->lock_en = TRUE;
        offset = p_info->lbd[FP_LOCK_LED_INDEX].pos & 0x0F;
        data = p_priv->dis_buff[(p_info->lbd[FP_LOCK_LED_INDEX].pos & 0x30) >> 4];

        if(0 != param)
        {
          data |= (1 << offset);
          p_priv->lock_status = TRUE;
        }
        else
        {
          data &= ~(1 << offset);
          p_priv->lock_status = FALSE;
        }
        p_priv->dis_buff[(p_info->lbd[FP_LOCK_LED_INDEX].pos & 0x30) >> 4] = data;
      }

      mtos_task_unlock();

      break;
    case UIO_FP_SET_COLON:
      mtos_task_lock();
      if(p_priv->cfg.p_info->lbd[2].pos == 0x63)
      {
        break;
      }
      p_priv->colon_en = TRUE;

      offset = p_info->lbd[FP_COLON_LED_INDEX].pos & 0x0F;
      data = p_priv->dis_buff[(p_info->lbd[FP_COLON_LED_INDEX].pos & 0x30) >> 4];

      if(0 != param)
      {
        data |= (1 << offset);
        p_priv->colon_status = TRUE;
      }
      else
      {
        data &= ~(1 << offset);
        p_priv->colon_status = FALSE;
      }

      p_priv->dis_buff[(p_info->lbd[FP_COLON_LED_INDEX].pos & 0x30) >> 4] = data;

      mtos_task_unlock();
      break;

    case UIO_FP_SET_SIGNAL:
      {
        // for the special gospell frontpanel
        int i = 0, j = param;
        p_priv->signal_strength = 0;
        for(i = 7; j > 0; j--,i--)
        {
          p_priv->signal_strength |= (0x80 >> i);
        }

        if(p_info->status & 0x01)
        {
          if(p_info->status & 0x10)
          {
            p_priv->dis_buff[0] = p_priv->signal_strength;
          }
        }
        else
        {
          if(p_info->status & 0x20)
          {
            p_priv->dis_buff[0] = p_priv->signal_strength;
          }
          else if(p_info->lbd[1].pos == 0)
          {
            p_priv->dis_buff[0] = p_priv->signal_strength;
          }
        }
      }
      break;
    case UIO_FP_NO_INT_RUN:
        mcu_panel_timer_cb();
    case UIO_FP_KEY_ENABLE:
    case UIO_FP_SET_SCAN_FREQ:
    default:
      ret = ERR_NOFEATURE;
      break;
  }

  return ret;
}


void mcu_fp_start_work(void)
{
  hal_timer_start(fp_timer_id);
}

void mcu_fp_stop_work(void)
{
  hal_timer_stop(fp_timer_id);
}


static RET_CODE mcu_panel_open(lld_uio_t *p_lld, fp_cfg_t *p_cfg)
{
  fp_priv_t *p_panel = NULL;

  FP_PRINTF("Open MCU frontpanel\n");
  MT_ASSERT((NULL != p_cfg) && (NULL != p_cfg->p_info));
  if(NULL != p_lld->p_fp)
  {
    return SUCCESS;
  }

  p_panel = (fp_priv_t *)mtos_malloc(sizeof(fp_priv_t));
  MT_ASSERT(NULL != p_panel);
  memset(p_panel, 0, sizeof(fp_priv_t));
  p_panel->cfg.p_info = p_cfg->p_info;
  p_lld->p_fp = p_panel;

  if(NULL == p_cfg->p_map)
  {
    p_panel->cfg.p_map = fp_bitmap;
    p_panel->cfg.map_size = FP_TABLE_SIZE;
  }
  else
  {
    p_panel->cfg.p_map = p_cfg->p_map;
    p_panel->cfg.map_size = p_cfg->map_size;
  }

  p_panel->last_key_code = FP_INVALIDE_KEY;
  p_panel->key_cnt = FP_JITTER_THRESHOLD / p_panel->cfg.p_info->com_scan_intv;
  p_panel->rpt_cnt = p_panel->cfg.p_info->repeat_intv
                      / p_panel->cfg.p_info->com_scan_intv;

  FP_PRINTF("@@Frontpanel key_cnt=%d, rpt_cnt=%d\n",
            p_panel->key_cnt, p_panel->rpt_cnt);

      p_panel->timer_id = hal_timer_request(p_panel->cfg.p_info->com_scan_intv,
                                     mcu_panel_timer_cb, TRUE);
  fp_timer_id = p_panel->timer_id;
  if(p_panel->timer_id == ERR_NO_RSRC)
  {
    FP_PRINTF("request timer failed!\n");
    MT_ASSERT(0);
  }
  else
  {
    FP_PRINTF("request timer id is %d\n", p_panel->timer_id);
  }

  mcu_panel_init(p_panel->cfg.p_info, 1);
  if(p_panel->cfg.p_info->num_com != 0x0F)
  {
    if(63 == p_panel->cfg.p_info->flatch.pos) // for 2pin MCU
    {
    mcu_com_display(p_panel->cfg.p_info,p_panel->dis_buff, FP_MAX_COM_NUM);
    }
    else  // for 3pin MCU
    {
    mcu3_com_display(p_panel->cfg.p_info,p_panel->dis_buff, FP_MAX_COM_NUM);
    }
  }
   if(p_panel->cfg.p_info->num_com == 0x0F)
  {
     mtos_task_delay_ms(3000);
     mtos_task_lock();
    em153_get_version(p_panel->cfg.p_info);
    mtos_task_unlock();
  }
  hal_timer_start(p_panel->timer_id);
  return SUCCESS;
}

void mcu_panel_attach(lld_uio_t *p_lld)
{
  p_lld->fp_open = mcu_panel_open;
  p_lld->fp_stop = mcu_panel_stop;
  p_lld->fp_io_ctrl = mcu_panel_io_ctrl;
  p_lld->display = mcu_panel_display;
  p_lld->fp_start_work = mcu_fp_start_work;
  p_lld->fp_stop_work = mcu_fp_stop_work;
  p_lld->fp_set_bitmap = mcu_set_bitmap;
}

