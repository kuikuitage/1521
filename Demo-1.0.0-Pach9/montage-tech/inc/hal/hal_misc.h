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
  The max board option num
  */
#define BOARD_MAX_OPTION_NUM 20

/*!
  The max board regset num
  */
#define BOARD_MAX_OPTION_REG_NUM 20

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
  /*!
    For chip Warriors
    */
  IC_WARRIORS = CHIP_VER_ID | 0x80,    
  /*!
    For chip Sonata
    */
  IC_SONATA = CHIP_VER_ID | 0x100,
  /*!
    For chip Jazz
    */
  IC_JAZZ = CHIP_VER_ID | 0x200,
  /*!
    For chip Concerto
    */
  IC_CONCERTO = CHIP_VER_ID | 0x400,
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
  /*!
    note: add the last version in front of this defination
    */
  VER_WIZARDS_MAX
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
  /*!
    note: add the last version in front of this defination
    */
  VER_MAGIC_MAX
}chip_magic_id_t;

/*!
  List the IC's version id for Warriors, it is returned by hal_get_chip_ver_id().
  note that the id should be located in [15:0] of chip id.
  */
typedef enum
{
  /*!
    For Warriors/A0
    */
  VER_WARRIORS_A0 = IC_WARRIORS, 
}chip_warriors_id_t;

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
    Warriors A0 for Public
    */  
  CHIP_WARRIORS_A0 = CUS_PUBLIC | VER_WARRIORS_A0,  
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
  List the hw modules in soc
  */
typedef enum
{
/*!
  CPU0
 */  
  HAL_CPU0 = 0,
/*!
  CPU1
 */  
  HAL_CPU1,  
/*!
  High speed bus  
 */
  HAL_HB,
/*!
  Peripheral low speed bus 
 */  
  HAL_PB,
/*!
  Graphic
 */
  HAL_GPE,
/*!
  Audio output
 */  
  HAL_AUDIO_OUT,
/*!
  SD video
 */  
  HAL_SD_VIDEO,
/*!
  HD video
 */  
  HAL_HD_VIDEO,

/*!
  Maximal module number
 */  
  HAL_MODULES_NUM,
}hal_module_id_t;

/*!
  The modules clock setting
  */
typedef enum
{
/*!
  The default clock setting
 */
  HAL_CLK_DEF = 0,
/*!
  The fixed clock setting
 */  
  HAL_CLK_FIXED,
}hal_clk_setting_t;

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

/*!
  This structure defines the option of dma.
  */
typedef struct hal_mclk_settings
{
/*!
  clk
  */
  u32 clk[HAL_MODULES_NUM];
}hal_mclk_settings_t;

/*!
 cache information
  */
typedef struct
{
  /*!
    dcache line size
    */
  u32 dcache_line_size;
}hal_cache_info_t;

/*!
 spin lock flags
  */
typedef enum
{
  /*!
    spin lock will only be used in task.
    */
  SPIN_LOCK_TASK = 0,
  /*!
    spin lock can be used in interrupt.
    */
  SPIN_LOCK_IRQ = 1,
}hal_spin_lock_flags_t;

/*!
 hal_audio_format_t
  */
typedef enum
{
  /*!
    PMW
    */
  HAL_PMW = 0,
  /*!
    PMW
    */
  HAL_SPDIF,
  /*!
    PMW
    */
  HAL_I2S,
}hal_audio_format_t;

/*!
 front panel type used in board config
  */
typedef enum
{
  /*!
    CT1642
    */
  HAL_CT1642 = 0,
  /*!
    HC164
    */
  HAL_HC164,
  /*!
    FD650
    */
  HAL_FD650,
  /*!
    TM1637
    */
  HAL_TM1637,
  /*!
    TM1635
    */
  HAL_TM1635,
  /*!
    FD620
    */
  HAL_FD620,
  /*!
    TM1618
    */
  HAL_TM1618
}hal_fp_type_t;

/*!
 gpio function definition
  */
typedef enum
{
  /*!
    used for front panel key scan 1
    */
  FP_KEY_SCAN_1 = 0,
  /*!
    used for front panel key scan 2
    */
  FP_KEY_SCAN_2,
  /*!
    used for front panel led segment com 1
    */
  FP_LED_SEG_C1,
  /*!
    used for front panel led segment com 2
    */
  FP_LED_SEG_C2,
  /*!
    used for front panel led segment com 3
    */
  FP_LED_SEG_C3,
  /*!
    used for front panel led segment com 4
    */
  FP_LED_SEG_C4,
  /*!
    used in front panel key for CT1642
    */
  FP_CT1642_KEY,
  /*!
    used for front panel clock pin
    */
  FP_CLK,
  /*!
    used for front panel I/O pin
    */
  FP_IO,

  /*!
    used for front panel key "volume up"
    */
  FP_KEY_VOL_UP,
  /*!
    used for front panel key "volume down"
    */
  FP_KEY_VOL_DOWN,
  /*!
    used for front panel key "channel up"
    */
  FP_KEY_CH_UP,
  /*!
    used for front panel key "channel down"
    */
  FP_KEY_CH_DOWN,
  /*!
    used for front panel key "stand by"
    */
  FP_KEY_STAND_BY,
  /*!
    used for front panel key "power"
    */
  FP_KEY_POWER,
  /*!
    used for front panel key "memu"
    */
  FP_KEY_MENU,
  /*!
    used for front panel key "ok"
    */
  FP_KEY_OK,
  /*!
    used for front panel key "exit"
    */
  FP_KEY_EXIT,

  /*!
    used for front panel led "signal lock"
    */
  FP_LED_LOCK,
  /*!
    used for front panel led "power"
    */
  FP_LED_POWER,

  /*!
    used for the i2c SCL pin to demod
    */
  DEM_SCL,
  /*!
    used for the i2c SDA pin to demod
    */
  DEM_SDA,

  /*!
    used for the i2c SCL pin to tuner
    */
  TUN_SCL,
  /*!
    used for the i2c SDA pin to tuner
    */
  TUN_SDA,

  /*!
    used for the i2c SCL pin to e2prom
    */
  E2P_SCL,
  /*!
    used for the i2c SDA pin to e2prom
    */
  E2P_SDA,

  /*!
    used for the clock pin to ts interface
    */
  TS_CLK,
  /*!
    used for the data pin to ts interface
    */
  TS_DATA,
  /*!
    used for the valid pin to ts interface
    */
  TS_VAL,
  /*!
    used for the sync pin to ts interface
    */
  TS_SYNC,

  /*!
    used for the audio mute
    */
  AUDIO_MUTE
}hal_gpio_usage_t;

/*!
 video DAC output mode for board config
  */
typedef enum
{
  /*!
    used for CVBS output
    */
  HAL_VDAC_CVBS,
  /*!
    used for Y in YPbPr output
    */
  HAL_VDAC_Y,
  /*!
    used for Pb in YPbPr output
    */
  HAL_VDAC_PB,
  /*!
    used for Pr in YPbPr output
    */
  HAL_VDAC_PR,
  /*!
    used for Y in S-Video output
    */
  HAL_VDAC_SVIDEO_Y,
  /*!
    used for Component in S-Video output
    */
  HAL_VDAC_SVIDEO_C,
  /*!
    used for R in RGB output
    */
  HAL_VDAC_R,
  /*!
    used for G in RGB output
    */
  HAL_VDAC_G,
  /*!
    used for B in RGB output
    */
  HAL_VDAC_B,
}hal_vdac_mode_t;

/*!
 hal_pin_audio_format_t
  */
typedef struct
{
  /*!
    gpio number
    */
  u8 gpio;
  /*!
    audio format, refer to hal_audio_format_t
    */
  u8 format;
}hal_pin_audio_format_t;

/*!
 audio out descriptor used in board config
  */
typedef struct
{
  /*!
    the pin count need to configed, the the unit is hal_pin_audio_format_t
    */
  u8 out_pin_cnt;
  /*!
    the flag to state if use 3db compensate
    */
  u8 db_compensate;
  /*!
    reserved0
    */
  u8 reserved0;
  /*!
    reserved1
    */
  u8 reserved1;
}hal_audio_out_t;

/*!
 video DAC option
  */
typedef struct
{
  /*!
    the output mode of the 1st VDAC, refer to hal_vdac_mode_t
    */
  u8 vdac_0;
  /*!
    the output mode of the 2nd VDAC, refer to hal_vdac_mode_t
    */
  u8 vdac_1;
  /*!
    the output mode of the 3rd VDAC, refer to hal_vdac_mode_t
    */
  u8 vdac_2;
  /*!
    the output mode of the 4th VDAC, refer to hal_vdac_mode_t
    */
  u8 vdac_3;
}hal_vdac_option_t;

/*!
 video out descriptor used in board config
  */
typedef struct
{
  /*!
    the video DAC mode option count, the unit is hal_vdac_option_t
    */
  u8 vdac_option_cnt;
  /*!
    reserved0
    */
  u8 reserved0;
  /*!
    reserved1
    */
  u8 reserved1;
  /*!
    reserved2
    */
  u8 reserved2;
}hal_video_out_t;

/*!
 pin function descriptor
  */
typedef struct
{
  /*!
    gpio number
    */
  u8 gpio;
  /*!
    gpio usage for this pin, refer to hal_gpio_usage_t
    */
  u8 usage;
  /*!
    gpio validity, 0: low voltage level active; 1: high voltage level active
    */
  u8 validity;
  /*!
    reserved
    */
  u8 reserved;
}hal_pin_func_t;

/*!
 gpio config used in board config
  */
typedef struct
{
  /*!
    front panel type, refer to hal_fp_type_t
    */
  u8 fp_type;
  /*!
    pin count need to configed, the unit is hal_pin_func_t
    */
  u8 pin_config_cnt;
}hal_gpio_config_t;

/*!
 register setting used in board config
  */
typedef struct
{
  /*!
    register address
    */
  u32 reg;
  /*!
    the value need to set
    */
  u32 val;
}hal_reg_setting_t;

/*!
 AV performance descriptor used in board config
  */
typedef struct
{
  /*!
    the option number of the based av register settings relative to the different requirment
    */
  u8 option;
  /*!
    the changed register setting count in this option, the unit is hal_reg_setting_t
    */
  u8 changed_param_cnt;
  /*!
    the option number of the based av register settings relative to the different requirment in PAL mode
    */
  u8 pal_option;
  /*!
    the changed register setting count in this option, the unit is hal_reg_setting_t in PAL mode
    */
  u8 pal_changed_param_cnt;  
  /*!
    the option number of the based av register settings relative to the different requirment in NTSC mode
    */
  u8 ntsc_option;
  /*!
    the changed register setting count in this option, the unit is hal_reg_setting_t in NTSC mode
    */
  u8 ntsc_changed_param_cnt;    
}hal_av_perf_t;

/*!
 ddr changed parameter setting used in board config
  */
typedef struct
{
  /*!
    the changed register setting count, the unit is hal_reg_setting_t
    */
  u8 changed_param_cnt;
  /*!
    reserved0
    */
  u8 reserved0;
  /*!
    reserved1
    */
  u8 reserved1;
  /*!
    reserved2
    */
  u8 reserved2;
}hal_ddr_param_t;

/*!
 misc config used in board config
  */
typedef struct
{
  /*!
    smart card power validity, 0: high active, 1: low active
    */
  u8 smc_power_validity;
  /*!
    smart card detect pin validity, 0: high active, 1: low active
    */
  u8 smc_detect_validity;
  /*!
    the flag to indicate if tuner enable loopthrough
    */
  u8 is_tn_loopthrough;
  /*!
    the flag to indicate if clock out from tuner clk_out pin
    */
  u8 tn_clk_out;
  /*!
    reserved0
    */
  u8 reserved0;
  /*!
    reserved1
    */
  u8 reserved1;
}hal_board_misc_cfg_t;

/*!
 board config descriptor
  */
typedef struct
{
  /*!
    audio out config
    */
  hal_audio_out_t audio_out;
  /*!
    video out config
    */
  hal_video_out_t video_out;
  /*!
    gpio config
    */
  hal_gpio_config_t gpio_config;
  /*!
    ddr config
    */
  hal_ddr_param_t ddr_param;
  /*!
    av performance config
    */
  hal_av_perf_t av_perf;
  /*!
    misc config
    */
  hal_board_misc_cfg_t misc_config;
}hal_board_config_t;

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
  return (hal_get_chip_rev() & CHIP_VER_ID_MASK);
}

/*!
  match to uncached memory addr.

  \param[in] addr memory addr
  
  \return uncached memory addr  
  */
u32 hal_addr_nc(u32 addr);

/*!
  Start bus monitor.

  \param[in] cmd see hal_bus_mon_cmd_t
  \param[in] param1 see hal_bus_mon_cmd_t
  \param[in] param2 see hal_bus_mon_cmd_t  
  
  \return None  
  */
void hal_bus_mon_start(u32 cmd, u32 param1, u32 param2);


/*!
  Stop bus monitor and show result.

  \param[in] cmd seem as the "cmd" in hal_bus_mon_start
  
  \return None  
  */
void hal_bus_mon_stop(u32 cmd);


/*!
  poll bus monitor status.

  \param[in] cmd see hal_bus_mon_cmd_t
  \param[in] p_info the status or information need to polling
  
  \return None  
  */
s32 hal_bus_mon_polling(u32 cmd, void *p_info);

/*!
  spin lock init .
 
  */
u32 *hal_spin_lock_init(void);
/*!
  Spin lock operation.

  \param[in] lock
  
  \return None  
  */
void hal_spin_lock(u32 *p_lock);
/*!
  Spin unlock operation

  \param[in] lock
  
  \return None  
  */
void hal_spin_unlock(u32 *p_lock);

/*!
  hw module working clock setting 

  \param[in] m_id module id, see hal_module_id_t
  \param[in] clk the clock(in Hz) to be set, 
                   the value in hal_clk_setting_t or a specific clock frequency
  
  \return None  
  */
s32 hal_module_clk_set(u32 m_id, u32 clk);

/*!
  hw module working clock getting 

  \param[in] m_id module id, see hal_module_id_t
  \param[out] clk the clock(in Hz) setting to be get, 
                     the value in hal_clk_setting_t or a specific clock frequency
  
  \return None  
  */
s32 hal_module_clk_get(u32 m_id, u32 *p_clk);
  
/*!
  Flush all data cache
  
  \return None  
  */
extern void   hal_dcache_flush_all(void);
/*!
  Invalidate all data cache
  
  \return None  
  */
extern void   hal_dcache_invalidate_all(void);
/*!
  Flush specificated memory space dcache

  \param[in] p_addr the start address of memory block to be flushed  
  \param[in] len the length(in bytes) of memory block to be flushed  
  
  \return None  
  */
extern void   hal_dcache_flush(void *p_addr, u32 len);
/*!
  Invalidate specificated memory space dcache

  \param[in] p_addr the start address of memory block to be invalidated
  \param[in] len the length(in bytes) of memory block to be invalidated  
  
  \return None  
  */
extern void   hal_dcache_invalidate(void *p_addr, u32 len);

/*!
 spinlock 
  */
typedef struct raw_spinlock_t{
  /*!
    For chip warriros
    */
  unsigned int lock;
} raw_spinlock_t;
/*!
  Board relative config

  \param[in] p_cfg the buffer pointer to the structure of configuration

  \return None
  */
extern s32 hal_board_config(u8 *p_cfg);
  
#endif //__HAL_MISC_H__
