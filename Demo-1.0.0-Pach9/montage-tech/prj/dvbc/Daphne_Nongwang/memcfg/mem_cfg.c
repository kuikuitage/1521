/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "string.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"

//drv
#include "common.h"

#include "mem_manager.h"

#include "drv_dev.h"
#include "vdec.h"

#include "mem_cfg.h"

extern u32 get_mem_addr(void);
#ifdef WIN32
static const partition_block_configs_t block_cfg_normal[STATIC_BLOCK_NUM] =
{
  {BLOCK_ES_BUFFER, ES_BUFFER_ADDR, ES_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_I0_BUFFER, I0_BUFFER_ADDR, SINGLE_FRAME_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_P0_BUFFER, P0_BUFFER_ADDR, SINGLE_FRAME_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_B0_BUFFER, B0_BUFFER_ADDR, SINGLE_FRAME_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_SUB_16BIT_BUFFER, SUB_16BIT_BUFFER_ADDR, SUB_16BIT_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_COMMON_BUFFER, COMMON_BUFFER_ADDR, COMMON_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_OSD0_16BIT_BUFFER, OSD0_16BIT_BUFFER_ADDR, OSD0_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_OSD1_16BIT_BUFFER, OSD1_16BIT_BUFFER_ADDR, OSD1_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},    
};

static const partition_block_configs_t block_cfg_9pic[STATIC_BLOCK_NUM] =
{
  {BLOCK_ES_BUFFER, ES_BUFFER_ADDR, ES_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_I0_BUFFER, I0_BUFFER_ADDR, SINGLE_FRAME_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_P0_BUFFER, P0_BUFFER_ADDR, SINGLE_FRAME_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_PIC9_BUFFER, PIC_9_BUFFER_ADDR, PIC_9_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_TAG_BUFFER, TAG_BUFFER_ADDR, TAG_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_STP_BUFFER, STP_BUUFER_ADDR, STP_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_SUB_16BIT_BUFFER, SUB_16BIT_BUFFER_ADDR, SUB_16BIT_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_COMMON_BUFFER, COMMON_BUFFER_ADDR, COMMON_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_OSD0_16BIT_BUFFER, OSD0_16BIT_BUFFER_ADDR, OSD0_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_OSD1_16BIT_BUFFER, OSD1_16BIT_BUFFER_ADDR, OSD1_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},    
};

static const partition_block_configs_t block_cfg_noav[STATIC_BLOCK_NUM] =
{
  {BLOCK_UNION_BUFFER, UNION_BUFFER_ADDR, UNION_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_OSD0_16BIT_BUFFER, OSD0_16BIT_BUFFER_ADDR, OSD0_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_OSD1_16BIT_BUFFER, OSD1_16BIT_BUFFER_ADDR, OSD1_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED}  
};
#else
static const partition_block_configs_t block_cfg_normal[STATIC_BLOCK_NUM] =
{
  {BLOCK_ES_BUFFER, VIDEO_BUFFER_ADDR, VIDEO_PARTITION_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_SUB_16BIT_BUFFER, SUB_16BIT_BUFFER_ADDR, SUB_16BIT_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_COMMON_BUFFER, COMMON_BUFFER_ADDR, COMMON_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_OSD0_16BIT_BUFFER, OSD0_16BIT_BUFFER_ADDR, OSD0_16BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
};

#endif
void mem_cfg(mem_cfg_t cfg)
{
  u16 i = 0;
  u32 base = 0;
  BOOL ret = FALSE;
  partition_block_configs_t curn_blk_cfg[STATIC_BLOCK_NUM] = {{0}};

  switch(cfg)
  {
    case MEMCFG_T_NORMAL:
      memcpy(curn_blk_cfg, block_cfg_normal, sizeof(curn_blk_cfg));
      break;
#ifdef WIN32
    case MEMCFG_T_MULTIVIEW:
      memcpy(curn_blk_cfg, block_cfg_9pic, sizeof(curn_blk_cfg));
      break;  

    case MEMCFG_T_NO_AV:
      memcpy(curn_blk_cfg, block_cfg_noav, sizeof(curn_blk_cfg));
      break;  
#endif
    default:
      MT_ASSERT(0);
      return;
  }
  // offset to base
  base = get_mem_addr();
  for (i = 0; i < STATIC_BLOCK_NUM; i++)
  {
    curn_blk_cfg[i].addr += base;
  }

  ret = mem_mgr_config_blocks(curn_blk_cfg, STATIC_BLOCK_NUM);
  MT_ASSERT(ret != FALSE);
}

void set_vdec_buff_addr(u32 addr);
void set_normal_buf(void)
{
  u32 p_addr = 0;
  void *p_video = NULL;
  //u32 size = 0, align = 0;
  RET_CODE ret = SUCCESS;
    
  p_video = dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);


  p_addr = mem_mgr_require_block(
    BLOCK_ES_BUFFER,
    SYS_MODULE_MEM_VIDEO);
  MT_ASSERT(p_addr != 0);
  mem_mgr_release_block(BLOCK_ES_BUFFER);
  //ret = vdec_get_buf_requirement(p_video, VDEC_QAULITY_AD, &size, &align);
  //MT_ASSERT(SUCCESS == ret);
  
#ifdef WIN32
  ret = vdec_set_buf(p_video, VDEC_QAULITY_AD, (u32)p_addr);
#else
  ret = vdec_set_buf(p_video, VDEC_BUFFER_AD, (u32)p_addr);
  set_vdec_buff_addr((u32)p_addr);
  OS_PRINTF("\n set vdec_buff_addr[0x%x]\n",(u32)p_addr); 
#endif
  MT_ASSERT(SUCCESS == ret);
}

#ifdef WIN32
void set_multi_view_buf(void)
{
  u32 p_addr = 0;
  void *p_video = NULL;
  //u32 size = 0, align = 0;
  RET_CODE ret = SUCCESS;

  p_video = dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);


  p_addr = mem_mgr_require_block(
    BLOCK_ES_BUFFER,
    SYS_MODULE_MEM_VIDEO);
  MT_ASSERT(p_addr != 0);
  mem_mgr_release_block(BLOCK_ES_BUFFER);
  //ret = vdec_get_buf_requirement(p_video, VDEC_MULTI_PIC_AD, &size, &align);
  //MT_ASSERT(SUCCESS == ret);

  ret = vdec_set_buf(p_video, VDEC_MULTI_PIC_AD, (u32)p_addr);
  MT_ASSERT(SUCCESS == ret);
}
#endif

/*!
   save vdec buff addr
 */
static u32 vdec_buff_addr = 0;

void set_vdec_buff_addr(u32 addr)
{
      vdec_buff_addr = addr;
}
u32 get_vdec_buff_addr(void)
{
    return vdec_buff_addr;
}

