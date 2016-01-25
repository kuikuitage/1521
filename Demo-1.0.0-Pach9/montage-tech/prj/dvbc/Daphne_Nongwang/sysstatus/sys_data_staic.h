/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __SYS_DATA_STAIC_H__
#define __SYS_DATA_STAIC_H__

#define OTA_BURN_DEFAULT_FLAG (0xFFFFFFFF)
#define OTA_BURN_START_FLAG (0xA5A5A5A5)
#define OTA_BURN_DONE_FLAG (0x5A5A5A5A)


/**boot:0~0x20000  static save date 0x20000 static save back date 0x3000***/
#define FLASH_SYSTEM_STAITC_SAVE_ADDRR  0X20000
#define FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR  0X30000

#define SYS_DATA_STAIC_BOOT_STATUS_POS      0
#define SYS_DATA_STAIC_OTA_INFO_POS      SYS_DATA_STAIC_BOOT_STATUS_POS + (1 * KBYTES)
#define SYS_DATA_STAIC_OTA_PARAMETER_POS      SYS_DATA_STAIC_OTA_INFO_POS + (1 * KBYTES)


/*!
  DVBC OTA lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u8 nim_modulate;
  u8 reserve1;
  u16 pid;
} dvbc_ota_lock_t;

/*!
  DVBC OTA tp info
  */
typedef struct
{
  dvbc_ota_lock_t main_tp;
  dvbc_ota_lock_t upg_tp;
}dvbc_ota_tp_t;

/*
  DVB OTA SPREAD INFO 
  */
typedef struct
{
  u32 ota_burn_flag;
  ota_info_t ota_info;
  u32 reserve[100]; /**prevent stuct spread**/
}dvbc_prv_ota_info_t;

/*
  DVB OTA PROCESS parameter 
  */
typedef struct
{
  dvbc_ota_tp_t ota_tp_info;
  u32 ota_times;
  u32 ota_lang;
  u32 reserve[99]; /** prevent ota_info_t spread**/
}dvbc_ota_parameter_t;


/*!
 * system setting boot status
 */
typedef struct
{
    /* av settting */
    /**1:NTSC 2:PAL other:PAL**/
    u8 tv_mode;
    /**av_set.tv_ratio   0:4:3   1:16:9 other 4:3**/
    u8 tv_ratio;
    u8 video_output;
    /**av_set.video_output  0:YUV  1:S_VIDEO   2:RGB  other YUV***/
    /* osd settting */
    u8 bright;
    u8 contrast;
    u8 chroma;
    /* vdec buff addr */
    u32 vdec_addr;
     /* boot logo display ctrl 0:OFF 1:display */
    u8 logo_display_bit;
    u32 crc_32;
}sys_boot_status_t;


#ifdef OTA_IMG
void sys_static_set_ota_jump_last_block_number(u8 number);
u8 sys_static_get_ota_jump_last_block_number(void);
void sys_staic_set_ota_disable_backup_data(void);
 BOOL sys_static_read_block_data(u8 *p_block_buffer,u32 size);
#endif
void sys_static_set_ota_default_main_tp(u32 freq,u32 sym,u8 modulate,u16 pid);
void sys_static_set_ota_default_upg_tp(u32 freq,u32 sym,u8 modulate,u16 pid);

BOOL sys_static_read_ota_tp_info(dvbc_ota_tp_t *tp_info);
BOOL sys_static_write_into_ota_times(u32 times);
u32 sys_static_read_into_ota_times(void);
BOOL sys_static_write_ota_tp_info(dvbc_ota_tp_t *tp_info);
BOOL sys_static_read_ota_info(ota_info_t *p_otai);
BOOL sys_static_write_ota_info(ota_info_t *p_otai);
u32 sys_static_read_ota_burn_flag(void);
BOOL sys_static_write_ota_burn_flag(u32 flag);
BOOL sys_static_check_and_repair_ota_info(void);

BOOL sys_static_write_ota_language(u32 lang);
u32 sys_static_read_ota_language(void);

#ifndef OTA_IMG
BOOL sys_static_set_status_ota_upg_tp(void);
BOOL sys_static_write_set_boot_status(void);
BOOL sys_static_reset_boot_flag(void);
u8 get_sys_static_boot_flag(void);
BOOL sys_static_set_status_ota_init_info(void);
BOOL sys_static_set_status_ota_revert_sw_on_ap(void);
BOOL sys_static_check_and_repair_data_block(void);
void sys_static_data_restore_factory(void);
#endif
#endif

