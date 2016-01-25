/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#ifndef __MEM_CFG_H__
#define __MEM_CFG_H__

typedef enum
{
  MEMCFG_T_NORMAL,
}mem_cfg_t;

/*! block classification.*/
typedef enum
{
  BLOCK_OSD0_8BIT_BUFFER = 0,
} block_id_t;

/*!
   Memory configuration
  */
#define SYS_MEMORY_END     SYS_MEMORY_TOTALT_SIZE

#define VIDEO_PARTITION_SIZE  ((3456+256) * KBYTES)// 4 vframe + ves +aes +vbi

#define VIDEO_BUFFER_ADDR    (SYS_MEMORY_END - VIDEO_PARTITION_SIZE) 
/*!
   Block configuration on normal
  */
#define STATIC_BLOCK_NUM    1

#define OSD0_8BIT_BUFFER_SIZE (820 * KBYTES)
#define OSD0_8BIT_BUFFER_ADDR (VIDEO_BUFFER_ADDR - OSD0_8BIT_BUFFER_SIZE)

#define OSD0_ODD_MEMSIZE (OSD0_8BIT_BUFFER_SIZE/2)
#define OSD0_EVEN_MEMSIZE (OSD0_8BIT_BUFFER_SIZE/2)

#define GUI_PARTITION_SIZE   (960 * KBYTES)//(64 * KBYTES + 384 * KBYTES)



#define SYS_PARTITION_ATOM           64


/*!
  Set memory config mode
  \param[in] cfg parameter for memory confi
  */
void mem_cfg(mem_cfg_t cfg);


#endif
