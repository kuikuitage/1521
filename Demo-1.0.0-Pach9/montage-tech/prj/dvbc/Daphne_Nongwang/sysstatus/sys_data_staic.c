/*
*********************************************************************************************************
*  
* File : sys_data_staic.c
* By  : guangfu.chen@montage-tech.com, Montage, ShangHai, China
* note:the function is save same data on flash at 0X20000 through staic mode;other area don't write!
*********************************************************************************************************
*/
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "unzip.h"

// driver
#include "common.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "sys_regs_jazz.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "vdec.h"
#include "display.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "avsync.h"


#include "nim.h"

#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager32.h"
#include "data_manager_v3.h"
#include "data_base.h"


#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "ts_packet.h"
#include "eit.h"
#include "epg_data4.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "fcrc.h"
#include "ss_ctrl.h"

#ifndef OTA_IMG
#include "avctrl1.h"

#include "db_dvbs.h"
#include "mem_cfg.h"
#include "sys_status.h"
#endif
#include "sys_data_staic.h"
#include "customer_def.h"


dvbc_ota_tp_t ota_default_tp_info = {{307000,6875,NIM_MODULA_QAM64,0,0x1b58},
                                                               {307000,6875,NIM_MODULA_QAM64,0,0x1b58}};

/***don't set it nomal,it user by ota old ota flash***/
static BOOL disable_use_backup_data = FALSE; 
/***************************************/

#ifdef OTA_IMG
static u8 ca_block_number = 1;
void sys_static_set_ota_jump_last_block_number(u8 number)
{
   ca_block_number = number;
}

u8 sys_static_get_ota_jump_last_block_number(void)
{
  return ca_block_number;
}
void sys_staic_set_ota_disable_backup_data(void)
{
   disable_use_backup_data = TRUE;
}
  
 BOOL sys_static_read_block_data(u8 *p_block_buffer,u32 size)
{
  BOOL ret = SUCCESS;
  void *p_dev = NULL;
  #ifdef WIN32
  return ret;
  #endif
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);
   
  mtos_task_lock();
  ret = charsto_read(p_dev,FLASH_SYSTEM_STAITC_SAVE_ADDRR, p_block_buffer,size);
  if (ret != SUCCESS)
  {
    mtos_task_unlock();
    return ERR_FAILURE;
  }
 mtos_task_unlock();
 return SUCCESS;
}
#endif
void sys_static_set_ota_default_main_tp(u32 freq,u32 sym,u8 modulate,u16 pid)
{
   ota_default_tp_info.main_tp.tp_freq= freq;
   ota_default_tp_info.main_tp.tp_sym = sym;
   ota_default_tp_info.main_tp.nim_modulate = modulate;
   ota_default_tp_info.main_tp.pid = pid;
}

void sys_static_set_ota_default_upg_tp(u32 freq,u32 sym,u8 modulate,u16 pid)
{
   ota_default_tp_info.upg_tp.tp_freq = freq;
   ota_default_tp_info.upg_tp.tp_sym = sym;
   ota_default_tp_info.upg_tp.nim_modulate = modulate;
   ota_default_tp_info.upg_tp.pid = pid;
}

void debug_data_printf(u8 * buff,u32 size)
{
    u32 i =0;
    u8 *print_buff = buff;
    if(NULL == print_buff)
     return ;
    OS_PRINTF("\n\n");
    for(i=0;i<size;i++)
      {
          if((i%16) == 0)
          OS_PRINTF("\n");
          OS_PRINTF(" 0x%2x  ",print_buff[i]);
      }
    OS_PRINTF("\n\n");
}

static void sys_static_block_crc_init(void)
{
  RET_CODE ret;
  
   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret); 
}

static u32 sys_static_block_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
  
}

static u32 sys_static_block_charsto_write(u32 part_pos,u8 *data,u32 size)
{
  BOOL ret = SUCCESS;
  void *p_dev = NULL;
  u8 *p_block_buffer = NULL;
  u32 crc_32_read = 0;
  u32 crc_32_write = 0;
  u8 *p_read_crc_buffer = NULL;
  u8  p_write_crc_buffer[4] ;
  #ifdef WIN32
  return ret;
  #endif
    p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
    if(p_block_buffer == NULL)
    {
       return ERR_FAILURE; 
    }
   memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);
   
  mtos_task_lock();
  /**read one block 64kbyte one time****/
  ret = charsto_read(p_dev,FLASH_SYSTEM_STAITC_SAVE_ADDRR, p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
  {
    mtos_free(p_block_buffer);
    mtos_task_unlock();
    return ERR_FAILURE;
  }
  p_read_crc_buffer = p_block_buffer+part_pos+size;
  crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                         +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));
  sys_static_block_crc_init();
  crc_32_write = sys_static_block_generate(data, size);

  if(crc_32_read == crc_32_write)
  {
    OS_PRINTF("note:the flash static block write data is same,don't to write flash\n",__FUNCTION__);
    mtos_free(p_block_buffer);
    mtos_task_unlock();
    return SUCCESS;
  } 
  OS_PRINTF("### %s read crc =0x%x,write crc =0x%x\n",
                                         __FUNCTION__, crc_32_read,crc_32_write);
 /***reback write data***/
 memcpy(p_block_buffer+part_pos,data,size);
 /***reback crc data***/
 p_write_crc_buffer[0] = (crc_32_write >>24)&0xff;
 p_write_crc_buffer[1] = (crc_32_write >>16)&0xff;
 p_write_crc_buffer[2] = (crc_32_write >>8)&0xff;
 p_write_crc_buffer[3] = (crc_32_write >>0)&0xff;
 memcpy(p_block_buffer+part_pos+size,p_write_crc_buffer,4);

 ret = charsto_erase(p_dev,FLASH_SYSTEM_STAITC_SAVE_ADDRR, 1);
  if (ret != SUCCESS)
    {

     if(disable_use_backup_data == FALSE)
      {
          /***writw back block****/
        charsto_erase(p_dev,FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR, 1);
        charsto_writeonly(p_dev, FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR,p_block_buffer,CHARSTO_SECTOR_SIZE);
      }
       mtos_free(p_block_buffer);
       mtos_task_unlock();
       return ERR_FAILURE;
    } 
  //debug_data_printf(p_block_buffer+part_pos,size+4);
  OS_PRINTF("note:the flash static block data save mode  write flash\n");
  ret = charsto_writeonly(p_dev, FLASH_SYSTEM_STAITC_SAVE_ADDRR,p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
    {
       if(disable_use_backup_data == FALSE)
      {
        /***writw back block****/
        charsto_erase(p_dev,FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR, 1);
        charsto_writeonly(p_dev, FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR,p_block_buffer,CHARSTO_SECTOR_SIZE);
       }
       mtos_free(p_block_buffer);
       mtos_task_unlock();
       return ERR_FAILURE;
    }

  if(disable_use_backup_data == FALSE)
  {
    /***writw back block****/
    charsto_erase(p_dev,FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR, 1);
    charsto_writeonly(p_dev, FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR,p_block_buffer,CHARSTO_SECTOR_SIZE);
  }
 mtos_free(p_block_buffer);
 mtos_task_unlock();
 return SUCCESS;
}

static u32 sys_static_block_charsto_read(u32 part_pos,u8 *data,u32 size)
{
  BOOL ret = SUCCESS;
  void *p_dev = NULL;
  u8 *p_block_buffer = NULL;
  u32 crc_32_read = 0;
  u32 crc_32_check = 0;
  u8 *p_read_crc_buffer = NULL;
  #ifdef WIN32
  return ret;
  #endif
    p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
    if(p_block_buffer == NULL)
    {
       return ERR_FAILURE; 
    }

   memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);
   
  mtos_task_lock();
  /**read one block 64kbyte one time****/
  ret = charsto_read(p_dev,FLASH_SYSTEM_STAITC_SAVE_ADDRR, p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
  {
    mtos_free(p_block_buffer);
    mtos_task_unlock();
    return ERR_FAILURE;
  }
  p_read_crc_buffer = p_block_buffer+part_pos+size;
  crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                         +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));
  //SYS_DATE_STAIC_DBG("### %s read crc =0x%x\n",
  //                                       __FUNCTION__, crc_32_read);
 memset(data,0,size);
 memcpy(data,p_block_buffer+part_pos,size);


  /****check crc and read back block*****/
 sys_static_block_crc_init();
 crc_32_check = sys_static_block_generate(data,size);
 if((crc_32_read != crc_32_check) && (disable_use_backup_data == FALSE))
  {
      OS_PRINTF("###%s check is err! read crc =0x%x,check crc =0x%x  it read back block!\n",
                                         __FUNCTION__, crc_32_read,crc_32_check);
      memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
      ret = charsto_read(p_dev,FLASH_SYSTEM_STAITC_SAVE_BACK_ADDRR, p_block_buffer,CHARSTO_SECTOR_SIZE);
      if (ret == SUCCESS)
      {
          p_read_crc_buffer = p_block_buffer+part_pos+size;
          crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                                 +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));
          memset(data,0,size);
          memcpy(data,p_block_buffer+part_pos,size);
      }
  }
 
 //debug_data_printf(p_block_buffer+part_pos,size+4);
 mtos_free(p_block_buffer);
 mtos_task_unlock();
 return crc_32_read;
}

static void sys_static_get_ota_default_tp(void)
{
  #if 0
  customer_cfg_t customer_cfg = {0};
  get_customer_config(&customer_cfg);
  memset((u8 *)&ota_default_tp_info,0,sizeof(dvbc_ota_tp_t));
  
   ota_default_tp_info.main_tp.tp_freq= 307000;
   ota_default_tp_info.main_tp.tp_sym = 6875;
   ota_default_tp_info.main_tp.nim_modulate = NIM_MODULA_QAM64;
   ota_default_tp_info.main_tp.pid = 0x1b58;
   
   ota_default_tp_info.upg_tp.tp_freq = 307000;
   ota_default_tp_info.upg_tp.tp_sym = 6875;
   ota_default_tp_info.upg_tp.nim_modulate = NIM_MODULA_QAM64;
   ota_default_tp_info.upg_tp.pid = 0x1b58;
   #endif
    
}

#ifndef OTA_IMG
BOOL sys_static_set_status_ota_upg_tp(void)
{
    BOOL ret = SUCCESS;
    dvbc_lock_t upg_tp = {0};
    dvbc_lock_t main_tp = {0};
    dvbc_ota_tp_t ota_tp_info;
    sys_status_get_upgrade_tp(&upg_tp);
    sys_status_get_main_tp1(&main_tp);
    memset(&ota_tp_info,0,sizeof(dvbc_ota_tp_t));
    
    ota_tp_info.upg_tp.tp_freq = upg_tp.tp_freq;
    ota_tp_info.upg_tp.tp_sym = upg_tp.tp_sym;
    ota_tp_info.upg_tp.nim_modulate = upg_tp.nim_modulate;
    ota_tp_info.upg_tp.pid = (u16)((upg_tp.reserve1 << 8) | upg_tp.reserve2);

    ota_tp_info.main_tp.tp_freq = main_tp.tp_freq;
    ota_tp_info.main_tp.tp_sym = main_tp.tp_sym;
    ota_tp_info.main_tp.nim_modulate = main_tp.nim_modulate;
    ota_tp_info.main_tp.pid = (u16)((upg_tp.reserve1 << 8) | upg_tp.reserve2);
    ret = sys_static_write_ota_tp_info(&ota_tp_info);
    return ret;
}
#endif

BOOL sys_static_read_ota_tp_info(dvbc_ota_tp_t *tp_info)
{

  BOOL ret = SUCCESS;
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  //dvbc_ota_tp_t read_tp_info;
  dvbc_ota_parameter_t ota_para;

  #ifdef WIN32
  return ret;
  #endif
  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
   sys_static_block_crc_init();
   crc_32_check = sys_static_block_generate((u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  if(crc_32 != crc_32_check)
  {
    OS_PRINTF("###%s check is err! read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
    sys_static_get_ota_default_tp();
    memcpy((u8 *)tp_info,(u8 *)&ota_default_tp_info,sizeof(dvbc_ota_tp_t));
    ret = ERR_FAILURE;
  }
else
  {
      ret = SUCCESS;
      memcpy((u8 *)tp_info,(u8 *)&ota_para.ota_tp_info,sizeof(dvbc_ota_tp_t));
  }
  return ret;

}

BOOL sys_static_write_ota_tp_info(dvbc_ota_tp_t *tp_info)
{
  BOOL ret = SUCCESS;
  dvbc_ota_parameter_t ota_para;
  #ifdef WIN32
  return ret;
  #endif
  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  memcpy((u8 *)&ota_para.ota_tp_info, (u8 *)tp_info,sizeof(dvbc_ota_tp_t));
  ret = sys_static_block_charsto_write(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
 return ret;
}

BOOL sys_static_read_ota_info(ota_info_t *p_otai)
{
  BOOL ret = SUCCESS;
  //ota_info_t ota_info ;
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  dvbc_prv_ota_info_t prv_ota_info;

  #ifdef WIN32
  return ret;
  #endif
    memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
    crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    sys_static_block_crc_init();
    crc_32_check = sys_static_block_generate( (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    
    if(crc_32 != crc_32_check)
    {
      OS_PRINTF("###%s check is err!read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
      memset((u8 *)&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
      prv_ota_info.ota_info.ota_tri = OTA_TRI_NONE;
      ret = ERR_FAILURE;
    }
    else
    {
       ret =  SUCCESS;
    }
  memcpy((u8 *)p_otai,(u8 *)&prv_ota_info.ota_info,sizeof(ota_info_t));
  return ret;
}


BOOL sys_static_write_ota_info(ota_info_t *p_otai)
{
  BOOL ret = SUCCESS;
  dvbc_prv_ota_info_t prv_ota_info;  
  #ifdef WIN32
  return ERR_FAILURE;
  #endif
    memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
    sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
  
    memcpy( (u8 *)&prv_ota_info.ota_info, (u8 *)p_otai,sizeof(ota_info_t));
    ret = sys_static_block_charsto_write(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                 (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    return ret;
}

 
u32 sys_static_read_ota_burn_flag(void)
{
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  dvbc_prv_ota_info_t prv_ota_info;
  u32 burn_flag = 0xFFFFFFFF;

  #ifdef WIN32
  return burn_flag;
  #endif
    memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
    crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    sys_static_block_crc_init();
    crc_32_check = sys_static_block_generate( (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    
    if(crc_32 != crc_32_check)
    {
      OS_PRINTF("###%s check is err!read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
      burn_flag = OTA_BURN_DEFAULT_FLAG;
    }
    else
    {
       burn_flag = prv_ota_info.ota_burn_flag;
    }
  return burn_flag;
}

BOOL sys_static_write_ota_burn_flag(u32 flag)
{
  BOOL ret = SUCCESS;
  dvbc_prv_ota_info_t prv_ota_info;  
  #ifdef WIN32
  return ERR_FAILURE;
  #endif
    memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
    sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    prv_ota_info.ota_burn_flag = flag;
    ret = sys_static_block_charsto_write(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                 (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    return ret;
}


BOOL sys_static_check_and_repair_ota_info(void)
{
  BOOL ret = SUCCESS;
  //ota_info_t ota_info ;
  dvbc_prv_ota_info_t prv_ota_info;  
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  misc_options_t misc;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
 
  #ifdef WIN32
  return ret;
  #endif
    memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
    crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    sys_static_block_crc_init();
    crc_32_check = sys_static_block_generate( (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
    
    if(crc_32 != crc_32_check)
    {
      OS_PRINTF("###%s check is err!read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
      sys_static_get_ota_default_tp();
      memset((u8 *)&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
      ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)(&misc));
      MT_ASSERT(ret != ERR_FAILURE);
      OS_PRINTF("revert ota info write flash!!!!\n");
      prv_ota_info.ota_info.orig_software_version = misc.ota_tdi.sw_version;
      prv_ota_info.ota_info.new_software_version = misc.ota_tdi.sw_version + 1;
      prv_ota_info.ota_info.lockc.tp_freq = ota_default_tp_info.upg_tp.tp_freq;
      prv_ota_info.ota_info.lockc.tp_sym= ota_default_tp_info.upg_tp.tp_sym;
      prv_ota_info.ota_info.lockc.nim_modulate = ota_default_tp_info.upg_tp.nim_modulate;
      prv_ota_info.ota_info.download_data_pid = ota_default_tp_info.upg_tp.pid;
      prv_ota_info.ota_info.sys_mode = SYS_DVBC;
      prv_ota_info.ota_info.ota_tri = OTA_TRI_AUTO;
      ret = sys_static_write_ota_info(&prv_ota_info.ota_info);
      sys_static_write_ota_tp_info(&ota_default_tp_info);
      sys_static_write_into_ota_times(0);
      sys_static_write_ota_burn_flag(OTA_BURN_DONE_FLAG);
    }
    return  ret;
}

u32 sys_static_read_into_ota_times(void)
{
  dvbc_ota_parameter_t ota_para;
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  u32 ota_times = 0;
  #ifdef WIN32
  return 0;
  #endif

  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
   sys_static_block_crc_init();
   crc_32_check = sys_static_block_generate((u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  if(crc_32 != crc_32_check)
  {
    OS_PRINTF("###%s check is err! read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
    ota_times = 0;
  }
else
  {
      ota_times = ota_para.ota_times;
      if(ota_times > 10)
      {
          ota_times = 0;
      }
  }
  return ota_times;
}

BOOL sys_static_write_into_ota_times(u32 times)
{
  dvbc_ota_parameter_t ota_para;
  BOOL ret = SUCCESS;
  #ifdef WIN32
  return ret;
  #endif
  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  ota_para.ota_times = times;
  ret = sys_static_block_charsto_write(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  return ret;
}

BOOL sys_static_write_ota_language(u32 lang)
{
  dvbc_ota_parameter_t ota_para;
  BOOL ret = SUCCESS;
  #ifdef WIN32
  return ret;
  #endif
  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  ota_para.ota_lang = lang;
  ret = sys_static_block_charsto_write(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  return ret;
}

u32 sys_static_read_ota_language(void)
{
  dvbc_ota_parameter_t ota_para;
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  u32 ota_lang = 0;
  #ifdef WIN32
  return 0;
  #endif

  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
   sys_static_block_crc_init();
   crc_32_check = sys_static_block_generate((u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  if(crc_32 != crc_32_check)
  {
    OS_PRINTF("###%s check is err! read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
    ota_lang = 0;
  }
else
  {
      ota_lang = ota_para.ota_lang;
  }
  return ota_lang;
}


#ifndef OTA_IMG
static video_out_format_t sys_status_get_video_out(u8 format)
{
  video_out_format_t mode = VIDEO_OUT_VID_YUV;
  customer_cfg_t cfg = {0};
  //OS_PRINTF("########sys_status_get_video_out_cfg   focus %d\n",format);
  get_customer_config(&cfg);
  if((CUSTOMER_JINGGONG == cfg.customer) ||
        (CUSTOMER_BOYUAN == cfg.customer) ||
        (CUSTOMER_ZHILING== cfg.customer) ||
        (CUSTOMER_ZHILING_KF== cfg.customer) ||
        (CUSTOMER_ZHILING_LQ== cfg.customer) ||
        (CUSTOMER_SHENZHOU_QN== cfg.customer) )
  {
    return VIDEO_OUT_VID_CVBS;
  }
 if((CUSTOMER_ZHONGDA == cfg.customer)||
 	(CUSTOMER_FANTONG_KFAJX== cfg.customer)||
 	(CUSTOMER_FANTONG_BYAJX== cfg.customer)||
 	(CUSTOMER_FANTONG_XSMAJX== cfg.customer))
  {
     return 7;// CVBS+CVBS, according boot define
  }
  switch(format)
  {
    case 0: //YUV and CVBS
      mode = VIDEO_OUT_VID_CVBS_YUV;
      break;
    case 1: // S_VIDEO and CVBS
      mode = VIDEO_OUT_VID_CVBS_SVDIEO;
      break;
    case 2:  //only RGB
      mode = VIDEO_OUT_VID_RGB;
      break;
    case 3: // only CVBS
      mode = VIDEO_OUT_VID_CVBS;
      break;
    case 4: // only YUV
      mode = VIDEO_OUT_VID_YUV;
    break;
    case 5: // only S_VIDE
      mode = VIDEO_OUT_VID_SVDIEO;
    break;
    default:
     // MT_ASSERT(0);
      mode = VIDEO_OUT_VID_CVBS_YUV;
    break;
  }

  return mode;
}
BOOL sys_static_write_set_boot_status(void)
{

  BOOL ret = 0;
  av_set_t av_set = {0};
  osd_set_t osd_set = {0};
  sys_boot_status_t boot_status = {0};
  customer_cfg_t cfg = {0};
  customer_cfg_t customer_cfg = {0};
  get_customer_config(&customer_cfg);
  #ifdef WIN32
  return ret;
  #endif

   get_customer_config(&cfg);
   sys_status_get_osd_set(&osd_set);
   sys_status_get_av_set(&av_set);
   
   memset(&boot_status,0,sizeof(sys_boot_status_t));
   
   boot_status.tv_mode = av_set.tv_mode;
   boot_status.tv_ratio = av_set.tv_ratio;
   if((CUSTOMER_YINHE == customer_cfg.customer)||(CUSTOMER_AOJIEXUN == customer_cfg.customer))
   {
        boot_status.video_output = 7; //dual cvbs
   }
   else
    {
      boot_status.video_output = (u8)sys_status_get_video_out(av_set.video_output);//av_set.video_output;
    }
   boot_status.bright = osd_set.bright;
   boot_status.contrast = osd_set.contrast;
   boot_status.chroma = osd_set.chroma;
   
   #ifdef OTA_IMG
    boot_status.vdec_addr = 0xc60000;
   #else
   boot_status.vdec_addr = get_vdec_buff_addr();
   #endif
   
   boot_status.logo_display_bit = 1;

    ret = sys_static_block_charsto_write(SYS_DATA_STAIC_BOOT_STATUS_POS,
                                                                 (u8 *)&boot_status,sizeof(sys_boot_status_t));
   return ret;

}

BOOL sys_static_reset_boot_flag(void)
{

  BOOL ret = 0;
  sys_boot_status_t boot_status = {0};
  u32 crc_32 = 0;

  #ifdef WIN32
  return ret;
  #endif

  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_BOOT_STATUS_POS,
                                                                 (u8 *)&boot_status,sizeof(sys_boot_status_t));
  boot_status.logo_display_bit = 0xff;
  ret = sys_static_block_charsto_write(SYS_DATA_STAIC_BOOT_STATUS_POS,
                                                               (u8 *)&boot_status,sizeof(sys_boot_status_t));
  return ret;
}


u8 get_sys_static_boot_flag(void)
{

    u8 flag = 0;
    BOOL ret = 0;
    sys_boot_status_t boot_status = {0};
#ifdef WIN32
    return flag;
#endif
  ret = sys_static_block_charsto_read(SYS_DATA_STAIC_BOOT_STATUS_POS,
                                                               (u8 *)&boot_status,sizeof(sys_boot_status_t));
  if(ret != ERR_FAILURE)
  {
      flag = boot_status.logo_display_bit;
  }
  return flag;

}

BOOL sys_static_set_status_ota_init_info(void)
{
    BOOL ret = SUCCESS;
    language_set_t ota_lang_set = {0};
    ota_info_t write_ota_info ;
    misc_options_t misc;
    void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    #ifdef WIN32
    return ret;
    #endif
    ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)(&misc));
    MT_ASSERT(ret != ERR_FAILURE);
    OS_PRINTF("init_ota_info  misc oui = 0x%x\n", misc.ota_tdi.oui);
    OS_PRINTF("init_ota_info  misc hw mod id = 0x%x\n", misc.ota_tdi.hw_mod_id);
    OS_PRINTF("init_ota_info  misc  sw mod id = 0x%x\n", misc.ota_tdi.sw_mod_id);
    OS_PRINTF("init_ota_info  misc  hw version = 0x%x\n", misc.ota_tdi.hw_version);
    OS_PRINTF("init_ota_info  misc  sw version = 0x%x\n", misc.ota_tdi.sw_version);
    OS_PRINTF("init_ota_info  misc  reserved = 0x%x\n", misc.ota_tdi.reserved);
    OS_PRINTF("init_ota_info  misc  manufacture_id = 0x%x\n", misc.ota_tdi.manufacture_id);
    memset((u8 *)&write_ota_info,0,sizeof(ota_info_t));
    sys_static_get_ota_default_tp();
    ret = sys_static_read_ota_info(&write_ota_info);
    OS_PRINTF("init_ota_info  flash  orig_software_version = 0x%x\n",write_ota_info.orig_software_version);
    if((write_ota_info.orig_software_version == 0xffff) ||
        (write_ota_info.orig_software_version <= misc.ota_tdi.sw_version))
      {
        OS_PRINTF("init_ota_info write flash!!!!\n");
        write_ota_info.orig_software_version = misc.ota_tdi.sw_version;
      }
    
    write_ota_info.lockc.tp_freq = ota_default_tp_info.upg_tp.tp_freq;
    write_ota_info.lockc.tp_sym= ota_default_tp_info.upg_tp.tp_sym;
    write_ota_info.lockc.nim_modulate = ota_default_tp_info.upg_tp.nim_modulate;
    write_ota_info.download_data_pid = ota_default_tp_info.upg_tp.pid;
    write_ota_info.ota_tri = OTA_TRI_NONE; 
    write_ota_info.sys_mode = SYS_DVBC;
    ret = sys_static_write_ota_info(&write_ota_info);
        
    sys_static_write_ota_tp_info(&ota_default_tp_info);
    sys_static_write_into_ota_times(0);
    sys_static_write_ota_burn_flag(OTA_BURN_DONE_FLAG);
    sys_status_get_lang_set(&ota_lang_set);
    sys_static_write_ota_language(ota_lang_set.osd_text);
    return ret;
}

BOOL sys_static_set_status_ota_revert_sw_on_ap(void)
{
    BOOL ret = SUCCESS;
    ota_info_t write_ota_info ;
    misc_options_t misc;
    void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    #ifdef WIN32
    return ret;
    #endif
    ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)(&misc));
    MT_ASSERT(ret != ERR_FAILURE);
    OS_PRINTF("init_ota_info  misc oui = 0x%x\n", misc.ota_tdi.oui);
    OS_PRINTF("init_ota_info  misc hw mod id = 0x%x\n", misc.ota_tdi.hw_mod_id);
    OS_PRINTF("init_ota_info  misc  sw mod id = 0x%x\n", misc.ota_tdi.sw_mod_id);
    OS_PRINTF("init_ota_info  misc  hw version = 0x%x\n", misc.ota_tdi.hw_version);
    OS_PRINTF("init_ota_info  misc  sw version = 0x%x\n", misc.ota_tdi.sw_version);
    OS_PRINTF("init_ota_info  misc  reserved = 0x%x\n", misc.ota_tdi.reserved);
    OS_PRINTF("init_ota_info  misc  manufacture_id = 0x%x\n", misc.ota_tdi.manufacture_id);
    memset((u8 *)&write_ota_info,0,sizeof(ota_info_t));
    ret = sys_static_read_ota_info(&write_ota_info);
    OS_PRINTF("init_ota_info  flash  orig_software_version = 0x%x\n",write_ota_info.orig_software_version);
    OS_PRINTF("init_ota_info write flash!!!!\n");
    write_ota_info.orig_software_version = misc.ota_tdi.sw_version;
    write_ota_info.ota_tri = OTA_TRI_NONE;
    ret = sys_static_write_ota_info(&write_ota_info);
    return ret;
}


BOOL sys_static_check_and_repair_data_block(void)
{
  BOOL ret = SUCCESS;
  sys_boot_status_t boot_status = {0};
  dvbc_ota_parameter_t ota_para;
  u32 crc_32 = 0;
  u32 crc_32_check = 0;
  dvbc_prv_ota_info_t prv_ota_info;  
;
  #ifdef WIN32
  return ret;
  #endif
   
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_BOOT_STATUS_POS,
                                                                 (u8 *)&boot_status,sizeof(sys_boot_status_t));
  sys_static_block_crc_init();
  crc_32_check = sys_static_block_generate( (u8 *)&boot_status,sizeof(sys_boot_status_t));
  if(crc_32 != crc_32_check)
  {
   OS_PRINTF("###%s:boot status read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
    sys_static_write_set_boot_status();
  }


  
  memset((u8 *)&ota_para,0,sizeof(dvbc_ota_parameter_t));
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_PARAMETER_POS,
                                                                       (u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
   sys_static_block_crc_init();
   crc_32_check = sys_static_block_generate((u8 *)&ota_para,sizeof(dvbc_ota_parameter_t));
  if(crc_32 != crc_32_check)
  {
    OS_PRINTF("###%s ota tp read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32,crc_32_check);
    sys_static_get_ota_default_tp();
    sys_static_write_ota_tp_info(&ota_default_tp_info);
    sys_static_write_into_ota_times(0);
  }


  memset(&prv_ota_info,0,sizeof(dvbc_prv_ota_info_t));
  crc_32 = sys_static_block_charsto_read(SYS_DATA_STAIC_OTA_INFO_POS,
                                                                       (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
  sys_static_block_crc_init();
  crc_32_check = sys_static_block_generate( (u8 *)&prv_ota_info,sizeof(dvbc_prv_ota_info_t));
  if(crc_32 != crc_32_check)
  {
    OS_PRINTF("###%s:ota info read crc =0x%x,check crc =0x%x\n",
                                       __FUNCTION__, crc_32,crc_32_check);
    sys_static_set_status_ota_init_info();
    //sys_static_write_ota_burn_flag(OTA_BURN_DONE_FLAG);
  }
  return ret;
}

void sys_static_data_restore_factory(void)
{
    ota_info_t ota_info = {0} ;
    language_set_t ota_lang_set = {0};
    sys_static_set_status_ota_upg_tp();
    sys_static_read_ota_info(&ota_info);
    sys_status_set_sw_version(ota_info.orig_software_version);
    sys_static_write_set_boot_status();
    sys_status_get_lang_set(&ota_lang_set);
    sys_static_write_ota_language(ota_lang_set.osd_text);
}
#endif
