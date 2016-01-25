/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <string.h>
#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "class_factory.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "data_manager.h"
#include "ap_framework.h"
#include "ap_upgrade.h"
#include "ap_uio.h"
#if WIN32
#include "uart_win32.h"
#else
#include "hal_uart.h"
#endif

#define BAUDRATE_NORMAL       115200

#define BAUDRATE_SYNC       115200

#ifdef DISABLE_FP_ON_UPG  
#define BAUDRATE_UPG  115200/2
#else
#define BAUDRATE_UPG  115200/2
#endif

#define UPG_INFLATE_SIZE  (32 * KBYTES)

#define UPG_UNZIP_SIZE (80 * KBYTES)
extern u32 get_flash_addr(void);
/*!
  init upgrade basic information
  */
upg_ret_t ap_upg_init(upg_config_t *p_cfg)
{
  class_handle_t dm_hdl;

  dm_hdl = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(NULL != p_cfg);

  p_cfg->flash_size = CHARSTO_SIZE;
  p_cfg->uart_id = 0;
  p_cfg->zip_type = UPG_ZIP_NONE;
  p_cfg->upg_client_addr = (u32)(dm_get_block_addr(dm_hdl, UPG_TOOL_BLOCK_ID)) - get_flash_addr();

  p_cfg->upg_client_size = dm_get_block_size(dm_hdl, UPG_TOOL_BLOCK_ID);   
  p_cfg->upg_baudrate = BAUDRATE_UPG;

  p_cfg->normal_baudrate = BAUDRATE_NORMAL;

  p_cfg->sync_baudrate = BAUDRATE_SYNC;

  p_cfg->inflate_size = UPG_INFLATE_SIZE;

  p_cfg->unzip_size = UPG_UNZIP_SIZE;

  p_cfg->zip_size = p_cfg->upg_client_size;

#ifdef WIN32
  uartwin_init(6);
#endif

  return UPG_RET_SUCCESS;
}

/*!
  check if the block id exist in slave side
  */
upg_ret_t ap_upg_block_process(u32 block_num, upg_block_t *p_block, slave_info_t *p_info)
{
  u32 i = 0, j = 0;
  u32 flag = FALSE;

  for(j = 0; j < block_num; j++)
  {
    flag = FALSE;
    for(i = 0; i < p_info->block_num; i++)  
    {
      //find if the block ID exists in slave side
      if(p_block->master_block_id == p_info->blocks[i].id)
      {
        p_block->slave_block_id = p_info->blocks[i].id;
        flag = TRUE;
        break;
      }
    }
    if(FALSE == flag)
      return UPG_RET_ERROR;
  }
  return UPG_RET_SUCCESS;
}

/*!
  clean the enviroment
  */
void ap_upg_pre_start(upg_config_t *p_cfg)
{
  u32 total_size = 0;

  total_size = p_cfg->inflate_size + p_cfg->unzip_size + p_cfg->upg_client_size;
  //alloc client used buffer
  p_cfg->p_inflate_buffer = (u8 *)mtos_malloc(total_size);
  MT_ASSERT(p_cfg->p_inflate_buffer != NULL);
  memset(p_cfg->p_inflate_buffer, 0, total_size);

  p_cfg->p_unzip_buffer =p_cfg->p_inflate_buffer + p_cfg->inflate_size; 
  p_cfg->p_zip_buffer = p_cfg->p_unzip_buffer + p_cfg->unzip_size;
  //alloc info buffer
  p_cfg->p_slave_info = (slave_info_t*)mtos_malloc(sizeof(slave_info_t));
  MT_ASSERT(p_cfg->p_slave_info != NULL);
  memset(p_cfg->p_slave_info, 0, sizeof(slave_info_t));
}

/*!
  post exit handle
  */
extern void uart_set_param(u8 id, u32 baudrate, u8 databits, u8 stopbits, u8 parity);
void ap_upg_post_exit(upg_config_t *p_cfg)
{
  uart_set_param(0, p_cfg->normal_baudrate, 0, 0, 0);

  if(p_cfg->p_inflate_buffer != NULL)
  {
    mtos_free(p_cfg->p_inflate_buffer);
    p_cfg->p_inflate_buffer = NULL;
  }
  if (p_cfg->p_slave_info != NULL)
  {
    mtos_free(p_cfg->p_slave_info);
    p_cfg->p_slave_info = NULL;
  }
}

extern u8 get_key_value(u8 key_set, u8 vkey);

/*!
  find hardware code for the exit/power key
  */
u16 ap_upg_get_code(u32 index, u16 *code)
{
  u8 v_key = V_KEY_CANCEL;
  u16 cnt = 2;
  
  switch(index)
  {
    case UPGRD_CODE_EXIT:
      v_key = V_KEY_CANCEL;
      break;
    case UPGRD_CODE_POWER:
      v_key = V_KEY_POWER;
      break;
    case UPGRD_CODE_MENU:
      v_key = V_KEY_MENU;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  
  cnt = 1;
  //code[0] = get_key_value(0, v_key);
  code[0] = get_key_value(0, v_key);
#ifndef WIN32
  code[1] = get_key_value(1, v_key);
#endif
    
  return cnt;
}

/*!
  construct the upg policy
  */
upg_policy_t *construct_upg_policy(void)
{
  upg_policy_t *p_upg_policy = mtos_malloc(sizeof(upg_policy_t));
  MT_ASSERT(p_upg_policy != NULL);

  p_upg_policy->p_init = ap_upg_init;
  p_upg_policy->p_block_process = ap_upg_block_process;
  p_upg_policy->p_pre_start = ap_upg_pre_start;
  p_upg_policy->p_post_exit = ap_upg_post_exit;
  p_upg_policy->p_get_code = ap_upg_get_code;

  return p_upg_policy;
}

