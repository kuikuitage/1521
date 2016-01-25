/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "sys_devs.h"
#include "sys_cfg.h"

#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_mem.h"

#include "mem_manager.h"
#include "data_manager32.h"
#include "fcrc.h"
#include "lib_util.h"

#include "drv_dev.h"
#include "charsto.h"
#include "data_manager.h"
#include "nim.h"
#include "pti.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

#include "ap_framework.h"
#include "ap_kingvon_ota.h"
#include "sys_cfg.h"

#include "sys_data_staic.h"
/*!
   the buffer number of DSMCC
  */
#define DSMCC_BUF_NUM      20
/*!
   the buffer size of DSMCC DDM
  */
#define DSMCC_DDM_BUF_SIZE  (5 * KBYTES)

static void ota_crc_init()
{
  RET_CODE ret;

   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret);
}

static u32 ota_crc32_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}

 #define revert_endian_16(x)  (((x&0xff)<<8)|(x>>8))

#define revert_endian_32(x)  \
           (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))


static void ota_init_tdi(void *p_tdi)
{
  s32 ret;
  misc_options_t misc;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
 // size = dm_get_block_size(p_dm_handle, MISC_OPTION_BLOCK_ID);
  memset(&misc,0,sizeof(misc_options_t));
  ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)&misc);
  MT_ASSERT(ret != ERR_FAILURE);


  misc.ota_tdi.oui = revert_endian_32(misc.ota_tdi.oui);
  misc.ota_tdi.hw_mod_id = revert_endian_16(misc.ota_tdi.hw_mod_id);
  misc.ota_tdi.hw_version = revert_endian_16(misc.ota_tdi.hw_version);
  misc.ota_tdi.manufacture_id = revert_endian_16(misc.ota_tdi.manufacture_id);
  misc.ota_tdi.sw_mod_id = revert_endian_16(misc.ota_tdi.sw_mod_id);
  
  memcpy((ota_tdi_t *)p_tdi, (u8 *)&misc.ota_tdi, sizeof(ota_tdi_t));
}

void ota_read_otai(ota_info_t *p_otai)
{
  //memcpy((u8 *)p_otai, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));
  sys_static_read_ota_info(p_otai);
}

void ota_write_otai(ota_info_t *p_otai)
{
  //sys_status_set_ota_info(p_otai);
  //sys_status_save();
  sys_static_write_ota_info(p_otai);
}

/*!
  all & unzip : save buffer need only.
  mod & unzip : save buffer & burn buffer needed.
  all & zip   : save buffer ,burn buffer, fake buffetr  needed.
  mod & zip   : save buffer, unzip buffer ,burn buffer & fake bufer needed.
  */
static void ota_cfg(ota_cfg_t *p_ota_cfg, BOOL is_upgrade_all, BOOL is_zipped)
{
  filter_ext_buf_t *p_temp = NULL;

  MT_ASSERT(p_ota_cfg != NULL);

  memset((void *)p_ota_cfg, 0, sizeof(ota_cfg_t));

  //set flash size
  p_ota_cfg->flash_size = CHARSTO_SIZE;

  //dsmcc buffer init, alloc buffer ring for dvb svc works.
  p_ota_cfg->p_dsmcc_extern_buf = mtos_malloc(sizeof(filter_ext_buf_t));
  MT_ASSERT(p_ota_cfg->p_dsmcc_extern_buf != NULL);

  p_temp =  p_ota_cfg->p_dsmcc_extern_buf;

#ifdef WIN32  //dvb_init()  use ring buffer
  for(buf_cnt = 0; buf_cnt < DSMCC_BUF_NUM; buf_cnt++)
  {
    if(p_temp == NULL)
    {
      p_temp = mtos_malloc(sizeof(filter_ext_buf_t));
      MT_ASSERT(p_temp != NULL);
      p_last->p_next = p_temp;
    }

    p_temp->p_buf = mtos_malloc(DSMCC_DDM_BUF_SIZE);
    MT_ASSERT(p_temp->p_buf != NULL);

    p_temp->size = DSMCC_DDM_BUF_SIZE;
    p_last = p_temp;
    p_temp = NULL;
  }

  p_last->p_next = p_ota_cfg->p_dsmcc_extern_buf;
#else //dvb_init_1()  use one big buffer
  #if(SYS_MEMORY_TOTALT_SIZE >= SYS_MEMORY_TOTALT_SIZE_32M)
  //p_temp->p_buf = mtos_malloc(1900*KBYTES);
  p_temp->p_buf = mtos_malloc(6*MBYTES);
  MT_ASSERT(p_temp->p_buf != NULL);
  //p_temp->size = 1900*KBYTES;
  p_temp->size = 6*MBYTES;
 #elif(SYS_MEMORY_TOTALT_SIZE == SYS_MEMORY_TOTALT_SIZE_16M)
  p_temp->p_buf = mtos_malloc(1024* KBYTES );/*SMCC_DDM_BUF_SIZE*DSMCC_BUF_NUM*/
  MT_ASSERT(p_temp->p_buf != NULL);
  p_temp->size = 1024* KBYTES;                            /*DSMCC_DDM_BUF_SIZE*DSMCC_BUF_NUM*/
 #endif
#endif

  //alloc section buffer for parse ddm.
  p_ota_cfg->p_sec_buf = (dvb_section_t *)mtos_malloc(sizeof(dvb_section_t));
  MT_ASSERT(p_ota_cfg->p_sec_buf != NULL);

  p_ota_cfg->p_sec_buf->p_buffer = (u8 *)mtos_malloc(4 * KBYTES);
  MT_ASSERT(NULL != p_ota_cfg->p_sec_buf->p_buffer);

  memset(p_ota_cfg->p_sec_buf->p_buffer, 0, 4 * KBYTES);

  //alloc buffer for download data or unzip data.
  if(is_upgrade_all)
  {
	OS_PRINTF("UPG ALL, and ");
    if(is_zipped)
    {
      //MT_ASSERT(0);
      
      OS_PRINTF("ZIPPED\n");

      #if(SYS_MEMORY_TOTALT_SIZE >= SYS_MEMORY_TOTALT_SIZE_32M)
      //p_ota_cfg->p_save_buf = mtos_malloc(2*MBYTES);
      p_ota_cfg->p_save_buf = mtos_malloc(8 * MBYTES + 100 * KBYTES);
      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);
      //p_ota_cfg->save_size = 2*MBYTES;
      p_ota_cfg->save_size = (8 * MBYTES + 100 * KBYTES);

      /***it don't malloc,it will user 0x400~0x900000****/
      //p_ota_cfg->p_burn_buf = mtos_malloc(4100*KBYTES);
      p_ota_cfg->p_burn_buf = (u8 *)0x400;
      MT_ASSERT(p_ota_cfg->p_burn_buf != NULL);
      //p_ota_cfg->burn_size = 4100*KBYTES;
      p_ota_cfg->burn_size = (8 * MBYTES + 100 * KBYTES);

      p_ota_cfg->p_unzip_buf = NULL;
      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = mtos_malloc(400 * KBYTES);
      MT_ASSERT(p_ota_cfg->p_fake_buf != NULL);
      p_ota_cfg->fake_size = 400 * KBYTES;
     #elif(SYS_MEMORY_TOTALT_SIZE == SYS_MEMORY_TOTALT_SIZE_16M)
     p_ota_cfg->p_save_buf = mtos_malloc(1536 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 1536* KBYTES;

      p_ota_cfg->p_burn_buf = mtos_malloc(2560 * KBYTES);

      p_ota_cfg->burn_size = 2560 * KBYTES;

      p_ota_cfg->p_unzip_buf = NULL;

      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = mtos_malloc(400 * KBYTES);

      p_ota_cfg->fake_size = 400 * KBYTES;
     #endif
      
    }
    else
    {
      OS_PRINTF("NOT ZIPPED\n");

      #if(SYS_MEMORY_TOTALT_SIZE >= SYS_MEMORY_TOTALT_SIZE_32M)
      //p_ota_cfg->p_save_buf = mtos_malloc(4100 * KBYTES);
      p_ota_cfg->p_save_buf = mtos_malloc(8 * MBYTES + 100 * KBYTES);
      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      //p_ota_cfg->save_size = 4100 * KBYTES;
      p_ota_cfg->save_size = (8 * MBYTES + 100 * KBYTES);
      #elif(SYS_MEMORY_TOTALT_SIZE == SYS_MEMORY_TOTALT_SIZE_16M)
      p_ota_cfg->p_save_buf = mtos_malloc(2100 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 2100 * KBYTES;
      #endif
      p_ota_cfg->p_burn_buf = NULL;

      p_ota_cfg->burn_size = 0;

      p_ota_cfg->p_unzip_buf = NULL;

      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = NULL;

      p_ota_cfg->fake_size = 0;
    }
  }
  else
  {
    MT_ASSERT(0);
  }
  return;
}



u32 get_block_addr(u32 flash_start, u32 block_id)
{
 #define DMH_OFFSET_ADDR 8
 #define DMH_BLOCKNUM_OFFSET (4*2)
 #define DMH_BHSIZE_OFFSET (DMH_BLOCKNUM_OFFSET+2)
 #define BLOCK_ID_OFFSET 0
 #define BLOCK_BASEADDR_OFFSET 4
 #define BLOCK_SIZE_OFFSET 8

  u32 dmh_offset_addr = flash_start + DMH_OFFSET_ADDR;
  u32 dmh_start_addr = *((u32 *)dmh_offset_addr);
  u32 block_num = *(u16 *)(dmh_start_addr + DMH_BLOCKNUM_OFFSET);
  u32 bh_size = *(u16 *)(dmh_start_addr + DMH_BHSIZE_OFFSET);
  u32 i;
  u32 base_addr = dmh_start_addr + 12; //ignore first 12 B
  u32 bid, baddr, bsize;
  //OS_PRINTF("dmh start[0x%x] bnum[%d] bhsize[%d]\n",
  //	dmh_start_addr, block_num);

  for(i = 0; i < block_num; i++)
  {
    bid = *(u8 *)(base_addr + i * bh_size + BLOCK_ID_OFFSET);
    baddr = *(u32 *)(base_addr + i * bh_size + BLOCK_BASEADDR_OFFSET);
    bsize = *(u32 *)(base_addr + i * bh_size + BLOCK_SIZE_OFFSET);
    if(bid == block_id)
    {
    	//OS_PRINTF("found block, size[%d]\n", bsize);
	    return baddr;
    }
  }

  return 0;
}

typedef void (*FUNC_NOTIFY_PROGRESS)(u32 cur_size, u32 full_size);

#define BL_OFFSET 8
static BOOL ota_burn_file(u8 *buf, u32 size, void *notify_progress)
{
  struct charsto_device *norf_dev;
  u32 bl_size;
  BOOL jump_ca_block = TRUE;
  u32 burn_offset = 0;
  u32 burn_size = 0;
  RET_CODE ret = SUCCESS;
  u32 charsto_size = CHARSTO_SIZE;
  u32 i, sec_cnt;
  FUNC_NOTIFY_PROGRESS func;
  
  bl_size = FLASH_LOGIC_ADRR_DM;
  
  OS_PRINTF("size=%d, charsto_size=%d, bl_size=%d\n",size,charsto_size,bl_size);

  if(size == charsto_size)  /***upg all flash**/
  { 
      OS_PRINTF("burn all flash but ca block\n");
      burn_offset = 0;
      burn_size = charsto_size;
      jump_ca_block = FALSE;
  }
  else if(size == (charsto_size-bl_size))  /***upg maincode but ca block**/
  {
    OS_PRINTF("burn maincode but ca block\n");
    burn_offset = bl_size;
    burn_size = charsto_size - bl_size;
    jump_ca_block = TRUE;
  }
  else
 {
      return FALSE;
  }
 
  sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;

  norf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  charsto_unprotect_all(norf_dev);
  //OS_PRINTF("burn, start[0x%x], size[0x%x]\n", burn_offset, burn_size);
  func = (FUNC_NOTIFY_PROGRESS)notify_progress;

  if(jump_ca_block == TRUE)
  {
      // ###do not burn last section which store CA information###
      sec_cnt -= sys_static_get_ota_jump_last_block_number();  
  }

  for(i=0; i<sec_cnt; i++)
  {
    #ifndef WIN32
    //OS_PRINTF("burn sector[%d]\n", i);
    charsto_erase(norf_dev, burn_offset+i*CHARSTO_SECTOR_SIZE, 1);
    ret = charsto_writeonly(norf_dev, burn_offset+i*CHARSTO_SECTOR_SIZE,
                  buf+i*CHARSTO_SECTOR_SIZE, CHARSTO_SECTOR_SIZE);
    #endif
    if(ret != SUCCESS)
    {
      charsto_protect_all(norf_dev);
      return FALSE;
    }
    func((i+1)*100/sec_cnt, 100);
    #ifdef WIN32
    mtos_task_sleep(500);
    #else
    //mtos_task_sleep(2000);
    #endif
  }

  charsto_protect_all(norf_dev);
  return TRUE;
}

static BOOL ota_burn_file_group(u8 *buf, u32 start, u32 size, void *notify_progress)
{
  struct charsto_device *norf_dev;
  u32 bl_size;
  u32 burn_offset = 0;
  u32 burn_size = 0;
  RET_CODE ret = SUCCESS;
  u32 charsto_size = CHARSTO_SIZE;
  u32 i, sec_cnt;
  FUNC_NOTIFY_PROGRESS func;

  norf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  if(SUCCESS != charsto_read(norf_dev, BL_OFFSET, (u8 *)&bl_size, 4))
  {
    return FALSE;
  }

  if(bl_size % CHARSTO_SECTOR_SIZE != 0)
  {
    bl_size += CHARSTO_SECTOR_SIZE - (bl_size % CHARSTO_SECTOR_SIZE);
  }

  if(start < bl_size && 0x0 != start )
  {
    //bootloader shouldn't be modified.
    return FALSE;
  }
  OS_PRINTF("size=%d, charsto_size=%d, bl_size=%d\n",size,charsto_size,bl_size);

  if(start == 0xFFFFFFFF)
  {
    OS_PRINTF("OTA BY MODULE!!!\n");
    //ota by module
    if(size != charsto_size && size != (charsto_size - bl_size))
    {
      return FALSE;
    }

    burn_offset = bl_size;

    if(size == charsto_size)
    {
      buf += bl_size;
      burn_size = charsto_size - bl_size;
    }
    else
    {
      burn_size = size;
    }

    sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;

  }else if(0x0== start){
    //upg all
    OS_PRINTF("abc\n");
    burn_offset = bl_size;
    buf+=bl_size;
    burn_size = charsto_size-bl_size;
    sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;
  } else {
    if(size % CHARSTO_SECTOR_SIZE != 0)
    {
      return FALSE;
    }

    sec_cnt = size / CHARSTO_SECTOR_SIZE;
    burn_offset = start;
  }

  OS_PRINTF("burn_size = %d, sec_cnt = %d\n", burn_size, sec_cnt);

  charsto_unprotect_all(norf_dev);
  //OS_PRINTF("burn, start[0x%x], size[0x%x]\n", burn_offset, burn_size);
  func = (FUNC_NOTIFY_PROGRESS)notify_progress;
  for(i = 0; i < sec_cnt; i++)
  {
    #ifndef WIN32
    //OS_PRINTF("burn sector[%d]\n", i);
    charsto_erase(norf_dev, burn_offset + i * CHARSTO_SECTOR_SIZE, 1);
    ret = charsto_writeonly(norf_dev, burn_offset + i * CHARSTO_SECTOR_SIZE,
                  buf + i * CHARSTO_SECTOR_SIZE, CHARSTO_SECTOR_SIZE);
    #endif

    if(ret != SUCCESS)
    {
      charsto_protect_all(norf_dev);

      return FALSE;
    }
    func((i + 1) * 100 / sec_cnt, 100);

    #ifdef WIN32
    mtos_task_sleep(500);
    #else
    //mtos_task_sleep(2000);
    #endif
  }

  charsto_protect_all(norf_dev);
  return TRUE;
}


ota_policy_t *construct_mingxin_ota_policy(void)
{
  ota_policy_t *p_ota_impl = mtos_malloc(sizeof(ota_policy_t));
  MT_ASSERT(p_ota_impl != NULL);

  p_ota_impl->ota_crc_init = ota_crc_init;
  p_ota_impl->ota_crc32_generate = ota_crc32_generate;
  p_ota_impl->ota_init_tdi = ota_init_tdi;
  p_ota_impl->ota_read_otai = ota_read_otai;
  p_ota_impl->ota_write_otai = ota_write_otai;
  p_ota_impl->ota_burn_file = ota_burn_file;
  p_ota_impl->ota_cfg = ota_cfg;
  p_ota_impl->ota_burn_file_group = ota_burn_file_group;

  p_ota_impl->p_data = NULL;

  return p_ota_impl;
}

void destruct_mingxin_ota_policy(ota_policy_t *p_ota_impl)
{
  //Free private data
  mtos_free(p_ota_impl->p_data);

  //Free implement policy
  mtos_free(p_ota_impl);
}
