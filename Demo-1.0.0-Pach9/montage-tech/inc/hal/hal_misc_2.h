/******************************************************************************/
/******************************************************************************/
#ifndef __HAL_MISC_H__
#define __HAL_MISC_H__

/*!
  The chip id's bit mask for custom id, the custom id located in [31:16] of chip id.
  */
#define CHIP_CUS_ID_MASK 0xFFFF0000

/*!
  The chip id's bit mask for version id, the version id located in [15:0] of chip id.
  */
#define CHIP_VER_ID_MASK 0xFFFF

/*!
  The chip id's bit mask for ic id, the ic id located in [15:4] of chip id.
  */
#define CHIP_IC_ID_MASK 0xFFF0

/*!
  The chip version bit mask for chip version id
  */
#define CHIP_VER_ID 0x8000

/*!
  The chip version bit mask for custom id
  */
#define CHIP_CUS_ID 0x80000000

/*!
  The max memory bus master number
  */
#define HAL_BUS_MASTER_MAX_NUM 16

/*!
  List the IC id located in chip versions, it is returned by hal_get_chip_ic_id().
  note that this id should be located in [15:4].
  */
typedef enum
{
  /*!
    For chip Wizards
    */
  IC_WIZARDS = CHIP_VER_ID | 0x10, 
  /*!
    For chip Magic
    */
  IC_MAGIC = CHIP_VER_ID | 0x20, 
  /*!
    For chip CX2448X
    */
  IC_CX2448X = CHIP_VER_ID | 0x40,   
}chip_ic_t;

/*!
  List the IC's version id for Wizards, it is returned by hal_get_chip_ver_id().
  note that the id should be located in [15:0] of chip id.
  */
typedef enum
{
  /*!
    For Wizards/A0
    */
  VER_WIZARDS_A0 = IC_WIZARDS, 
  /*!
    For Wizards/A1
    */
  VER_WIZARDS_A1, 
  /*!
    For Wizards/A2
    */
  VER_WIZARDS_A2,
  /*!
    For Wizards/A3
    */
  VER_WIZARDS_A3,  
}chip_wizards_id_t;

/*!
  List the IC's version id for Magic, it is returned by hal_get_chip_ver_id().
  note that the id should be located in [15:0] of chip id.
  */
typedef enum
{
  /*!
    For Magic/A0
    */
  VER_MAGIC_A0 = IC_MAGIC, 
  /*!
    For Magic/B0
    */
  VER_MAGIC_B0, 
  /*!
    For Magic/B1
    */
  VER_MAGIC_B1,
  /*!
    For Magic/B2
    */
  VER_MAGIC_B2,
}chip_magic_id_t;

/*!
  List the custom id located in chip versions, it is returned by hal_get_chip_cus_id().
  note that the id should be located in [31:16].
  */
typedef enum
{
  /*!
    For Public
    */
  CUS_PUBLIC = CHIP_CUS_ID | (0<<16), 
  /*!
    For C01
    */
  CUS_C01 = CHIP_CUS_ID | (1<<16), 
  /*!
    For C02
    */
  CUS_C02 = CHIP_CUS_ID | (2<<16),   
  /*!
    For C03
    */
  CUS_C03 = CHIP_CUS_ID | (4<<16),
}chip_cus_id_t;

/*!
  List the supported chip version ids, it is returned by hal_get_chip_rev().
  note that the id should be located in [31:0], include the custom id and IC's version id
  */
typedef enum
{
  /*!
    wizards A0
    */
  CHIP_WIZARDS_A0 = CUS_PUBLIC | VER_WIZARDS_A0, 
  /*!
    wizards A1
    */
  CHIP_WIZARDS_A1 = CUS_PUBLIC | VER_WIZARDS_A1, 
  /*!
    wizards A2
    */
  CHIP_WIZARDS_A2 = CUS_PUBLIC | VER_WIZARDS_A2, 
  /*!
    wizards A3
    */
  CHIP_WIZARDS_A3 = CUS_PUBLIC | VER_WIZARDS_A3,
  /*!
    magic A0
    */
  CHIP_MAGIC_A0 = CUS_PUBLIC | VER_MAGIC_A0,
  /*!
    CX2448X
    */
  CHIP_CX2448X = CUS_PUBLIC | IC_CX2448X,
  /*!
    magic B0 for Public
    */  
  CHIP_MAGIC_B0 = CUS_PUBLIC | VER_MAGIC_B0,
  /*!
    magic B1 for Public
    */  
  CHIP_MAGIC_B1 = CUS_PUBLIC | VER_MAGIC_B1,  
  /*!
    magic B2 for Public
    */  
  CHIP_MAGIC_B2 = CUS_PUBLIC | VER_MAGIC_B2,   
  /*!
    magic B0 for C01
    */  
  CHIP_MAGIC_B0_C01 = CUS_C01 | VER_MAGIC_B0,  
  /*!
    magic B1 for C01
    */  
  CHIP_MAGIC_B1_C01 = CUS_C01 | VER_MAGIC_B1, 
  /*!
    magic B2 for C01
    */  
  CHIP_MAGIC_B2_C01 = CUS_C01 | VER_MAGIC_B2,   
  /*!
    magic B0 for C02
    */  
  CHIP_MAGIC_B0_C02 = CUS_C02 | VER_MAGIC_B0,   
  /*!
    magic B1 for C02
    */  
  CHIP_MAGIC_B1_C02 = CUS_C02 | VER_MAGIC_B1,   
  /*!
    magic B2 for C02
    */  
  CHIP_MAGIC_B2_C02 = CUS_C02 | VER_MAGIC_B2,
  /*!
    magic B0 for C03
    */  
  CHIP_MAGIC_B0_C03 = CUS_C03 | VER_MAGIC_B0,   
  /*!
    magic B1 for C03
    */  
  CHIP_MAGIC_B1_C03 = CUS_C03 | VER_MAGIC_B1,   
  /*!
    magic B2 for C03
    */  
  CHIP_MAGIC_B2_C03 = CUS_C03 | VER_MAGIC_B2,
  /*!
    Unknown chip version
    */  
  CHIP_UNKNOWN = 0xFFFFFFFF,  
}chip_rev_t;


/*!
  Reset status, abnormal case
  */
#define RESET_STATUS_NONE         0x0
/*!
  Reset status, cold power on
  */
#define RESET_STATUS_POWER        0x1
/*!
  Reset status, watchdog reset
  */
#define RESET_STATUS_TIMEOUT      0x2
/*!
  Reset status, software reset
  */
#define RESET_STATUS_SOFTRST      0x3
/*!
  Reset status, PLL reset
  */
#define RESET_STATUS_PLL_CHANGE      0x4
/*!
  Reset status, smart card reset
  */
#define RESET_STATUS_SMARTCARD     0x5

/*!
  List the command for memory bus monitor function
  */
typedef enum
{
/*!
  bus monitor command for whole throughput
 */
  HAL_BUS_MON_WHOLE_THROUGHPUT = 1,
/*!
  bus monitor command for specific master throughput, 
  for hal_bus_monitor_start, param1 is the specific master id.
 */  
  HAL_BUS_MON_MASTER_THROUGHPUT,
/*!
  bus monitor command for the touch condition for specific address, 
  for hal_bus_monitor_start, param1 is the specific address need to be monitored,
  param2 is the written times alarmed on this memory address.
 */  
  HAL_BUS_MON_TOUCH_CONDITION,
/*!
  bus monitor command for memory accessing request, 
  for hal_bus_monitor_start, param1 specify whether has or hasn't assert a memory request for a long time
  0: hasn't, 1: has
  param2 is expiry period in CPU cycles. 
  */  
  HAL_BUS_MON_MEM_REQUEST,
/*!
  bus monitor command for memory accessing request deadlock, 
  for hal_bus_monitor_start, param1 is the deadlock exception period in unit of 1024 clock cycles, 
  param2 is the amount of continuous locked requests.
  */    
  HAL_BUS_MON_DEADLOCK,
}hal_bus_mon_cmd_t;

/*!
  bus monitor infomation
  */
typedef struct
{
/*!
  current master id monitored
  */
  u32 cur_master_id;
/*!
  current touch memory address monitored
  */
  u32 cur_touch_addr;
/*!
  current specific memory touch times by each master
  */
  u8 cur_touch_times[HAL_BUS_MASTER_MAX_NUM];
/*!
  current monitored master number 
  */
  u8 cur_master_num;
/*!
  max master number 
  */
  u8 max_master_num;
/*!
  0: hasn't assert a memory request for a long time
  1: has assert a memory request for a long time
  */  
  BOOL cur_mem_request;
/*!
  current memory request expiry cycles
  */
  u32 cur_expiry_cycles;
/*!
  current memory request master id's bit map
  */
  u32 cur_expiry_mid;
/*!
  current dead locked request exception period in unit of 1024 cycles.
  */
  u32 cur_deadlock_exp_period;
/*!
  current continues dead locked request times
  */
  u32 cur_deadlock_cont_times;
}hal_bus_mon_info_t;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//              APIs
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/*!
  Enable/Disable gpio function by pinmux setting
  
  \param[in] pin pin number
  \param[in] enable True:enable, False:disable  
  
  \return None
  */
void hal_pinmux_gpio_enable(u32 pin, BOOL enable);

/*!
  Clear reset status
  
  \param[in] None
  
  \return TRUE
  */
BOOL hal_pm_state_clr(void);

/*!
  Get the reset status
  
  \param[in] None
  
  \return the reset status, RESET_STATUS_XXX
  */
u32 hal_pm_get_state(void);

/*!
  Software reset
  
  \param[in] None
  
  \return TRUE
  */
BOOL hal_pm_reset(void);


/*!
  Get the chip version.
  */
chip_rev_t hal_get_chip_rev(void);

/*!
  Get the chip ic's id.
  */
static inline u32 hal_get_chip_ic_id(void)
{
  return (hal_get_chip_rev() & CHIP_IC_ID_MASK);
}

/*!
  Get the chip custom id.
  */
static inline u32 hal_get_chip_cus_id(void)
{
  return (hal_get_chip_rev() & CHIP_CUS_ID_MASK);
}

/*!
  Get the chip ic's version id.
  */
static inline u32 hal_get_chip_ver_id(void)
{
  return (0x8010 & CHIP_VER_ID_MASK);
}

#endif