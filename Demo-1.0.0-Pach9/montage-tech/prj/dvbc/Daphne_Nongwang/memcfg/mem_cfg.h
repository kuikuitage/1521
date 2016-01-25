/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#ifndef __MEM_CFG_H__
#define __MEM_CFG_H__

typedef enum
{
  MEMCFG_T_NORMAL,
  MEMCFG_T_MULTIVIEW,
  MEMCFG_T_NO_AV,
}mem_cfg_t;

/*! block classification.*/
typedef enum
{
  BLOCK_ES_BUFFER = 0,
  BLOCK_I0_BUFFER = 1,
  BLOCK_P0_BUFFER = 2,
  BLOCK_B0_BUFFER = 3,
  BLOCK_TAG_BUFFER = 4,
  BLOCK_STP_BUFFER = 5,
  BLOCK_PIC9_BUFFER = 6,
  BLOCK_COMMON_BUFFER = 7,
  BLOCK_SUB_16BIT_BUFFER = 8,
  BLOCK_OSD0_16BIT_BUFFER = 9,
  BLOCK_OSD1_16BIT_BUFFER = 10,
  BLOCK_UNION_BUFFER = 11,
} block_id_t;

/*!
   Memory configuration
  */

#define SYS_MEMORY_END     SYS_MEMORY_TOTALT_SIZE //0x2000000

#ifdef WIN32
/*!
   Block configuration on normal
  */
#define STATIC_BLOCK_NUM    11

#define VES_BUF_SIZE      384 * KBYTES    // Should align with 8kb.
#define AES_BUF_SIZE      32 * KBYTES // Should align with 8kb.

#define VDEC_FRAME_SIZE_KB ((((DISP_FULLSCR_WIDTH) * \
                    (DISP_FULLSCR_HEIGHT_PAL) * 3 / 2) >> 10) + 1)

/*!
   Block configuration on normal
  */
#define SINGLE_FRAME_SIZE    (VDEC_FRAME_SIZE_KB * KBYTES)
#define I0_BUFFER_ADDR       (SYS_MEMORY_END - SINGLE_FRAME_SIZE)
#define P0_BUFFER_ADDR       (I0_BUFFER_ADDR - SINGLE_FRAME_SIZE)
#define B0_BUFFER_ADDR       (P0_BUFFER_ADDR - SINGLE_FRAME_SIZE)
//#define VIDEO_BUFFER_SIZE    ((SINGLE_FRAME_SIZE) * 3)
#define VIDEO_BUFFER_ADDR    B0_BUFFER_ADDR

#define ES_BUFFER_SIZE       ((VES_BUF_SIZE + AES_BUF_SIZE))
#define ES_BUFFER_ADDR       (VIDEO_BUFFER_ADDR - ES_BUFFER_SIZE)
#define VES_BUFFER_ADDR       (VIDEO_BUFFER_ADDR - VES_BUF_SIZE)

/*!
   Block configuration on multipic
  */
#define STP_BUFFER_SIZE      (343 * KBYTES) //180*144*9
#define STP_BUUFER_ADDR      (I0_BUFFER_ADDR + (123 + 24) * KBYTES)
//#define STP_BUUFER_ADDR      (P0_BUFFER_ADDR - STP_BUFFER_SIZE)

#define TAG_BUFFER_ADDR      (B0_BUFFER_ADDR - 24 * KBYTES)
#define TAG_BUFFER_SIZE      (24 * KBYTES)

#define PIC_9_START_B_ADDR   (TAG_BUFFER_ADDR - 41 * KBYTES)
#define PIC_9_START_P_ADDR   (PIC_9_START_B_ADDR - 41 * KBYTES)
#define PIC_9_START_I_ADDR   (PIC_9_START_P_ADDR - 41 * KBYTES)
#define PIC_9_BUFFER_ADDR    (PIC_9_START_I_ADDR)
#define PIC_9_BUFFER_SIZE    (123 * KBYTES)

#define PIC_9_ES_BUFFER_ADDR       (PIC_9_BUFFER_ADDR - ES_BUFFER_SIZE)
#define PIC_9_VES_BUFFER_ADDR       (PIC_9_BUFFER_ADDR - VES_BUF_SIZE)

/*!
   Block configuration on others
  */


#define SUB_16BIT_BUFFER_SIZE (820 * KBYTES)
#define SUB_16BIT_BUFFER_ADDR (ES_BUFFER_ADDR - SUB_16BIT_BUFFER_SIZE)

#define SUB_ODD_MEMSIZE (SUB_16BIT_BUFFER_SIZE/2)
#define SUB_EVEN_MEMSIZE (SUB_16BIT_BUFFER_SIZE/2)

#define COMMON_BUFFER_SIZE   (1024 * KBYTES)
#define COMMON_BUFFER_ADDR   (SUB_16BIT_BUFFER_ADDR - COMMON_BUFFER_SIZE)

#define OSD0_16BIT_BUFFER_SIZE (820 * KBYTES)
#define OSD0_16BIT_BUFFER_ADDR (COMMON_BUFFER_ADDR - OSD0_16BIT_BUFFER_SIZE)

#define OSD0_ODD_MEMSIZE (OSD0_16BIT_BUFFER_SIZE/2)
#define OSD0_EVEN_MEMSIZE (OSD0_16BIT_BUFFER_SIZE/2)

#define OSD1_16BIT_BUFFER_SIZE (64 * KBYTES)
#define OSD1_16BIT_BUFFER_ADDR (OSD0_16BIT_BUFFER_ADDR - OSD1_16BIT_BUFFER_SIZE)

#define OSD1_ODD_MEMSIZE (OSD1_16BIT_BUFFER_SIZE/2)
#define OSD1_EVEN_MEMSIZE (OSD1_16BIT_BUFFER_SIZE/2)

/*!
   Block configuration on no AV
  */
#define UNION_BUFFER_SIZE    (SYS_MEMORY_END - COMMON_BUFFER_ADDR)
#define UNION_BUFFER_ADDR    (COMMON_BUFFER_ADDR)
/*!
   Other partition configuration
  */
#define VIDEO_PARTITION_SIZE \
  (ES_BUFFER_SIZE + SINGLE_FRAME_SIZE * 3 + \
   SUB_8BIT_BUFFER_SIZE + OSD0_8BIT_BUFFER_SIZE + OSD1_8BIT_BUFFER_SIZE)
   
#define GUI_PARTITION_SIZE   (960 * KBYTES)//(64 * KBYTES + 384 * KBYTES)

#define SYS_PARTITION_SIZE       ((7 * 1024) * KBYTES)
#define SYS_PARTITION_ATOM           64

#else

/*!
   Block configuration on normal
  */
#define STATIC_BLOCK_NUM    5



/*!
   Block configuration on normal Should align with 8kb.
  */
#define VIDEO_PARTITION_SIZE  ((3456+256) * KBYTES)// 4 vframe + ves +aes +vbi

#define VIDEO_BUFFER_ADDR    (SYS_MEMORY_END - VIDEO_PARTITION_SIZE) 


/*!
   Block configuration on others
  */

#define SUB_16BIT_BUFFER_SIZE (1620 * KBYTES)//(820 * KBYTES)

#define SUB_16BIT_BUFFER_ADDR (VIDEO_BUFFER_ADDR - SUB_16BIT_BUFFER_SIZE)

#define SUB_ODD_MEMSIZE (SUB_16BIT_BUFFER_SIZE/2)
#define SUB_EVEN_MEMSIZE (SUB_16BIT_BUFFER_SIZE/2)

#define COMMON_BUFFER_SIZE   (3*1024 * KBYTES)
#define COMMON_BUFFER_ADDR   (SUB_16BIT_BUFFER_ADDR - COMMON_BUFFER_SIZE)

#define OSD0_16BIT_BUFFER_SIZE (820 * KBYTES)
#define OSD0_16BIT_BUFFER_ADDR (COMMON_BUFFER_ADDR - OSD0_16BIT_BUFFER_SIZE)

#define OSD0_ODD_MEMSIZE (OSD0_16BIT_BUFFER_SIZE/2)
#define OSD0_EVEN_MEMSIZE (OSD0_16BIT_BUFFER_SIZE/2)

#define OSDD_16BIT_BUFFER_SIZE (820 * KBYTES)
#define OSDD_16BIT_BUFFER_ADDR (OSD0_16BIT_BUFFER_ADDR - OSDD_16BIT_BUFFER_SIZE)

#define OSDD_ODD_MEMSIZE (OSDD_16BIT_BUFFER_SIZE / 2)
#define OSDD_EVEN_MEMSIZE (OSDD_16BIT_BUFFER_SIZE / 2)

/*!
   Other partition configuration
  */
#ifdef ChangJiang_WanFa
#define GUI_PARTITION_SIZE   (1536 * KBYTES)
#else
#define GUI_PARTITION_SIZE   (1024 * KBYTES)
#endif

#ifdef VFONT
#if(SYS_MEMORY_TOTALT_SIZE >= SYS_MEMORY_TOTALT_SIZE_32M)
#define SYS_PARTITION_SIZE       (10 * MBYTES)
#else
 #define SYS_PARTITION_SIZE       (7 * MBYTES)
#endif
 
#else
#define SYS_PARTITION_SIZE       (7 * MBYTES)
#endif

#define SYS_PARTITION_ATOM           64


#endif
/*!
  Set memory config mode
  \param[in] cfg parameter for memory confi
  */
void mem_cfg(mem_cfg_t cfg);

/*!
  Set normal buffer
  */
void set_normal_buf(void);

#ifdef WIN32
/*!
  Set multi view buffer
  */
void set_multi_view_buf(void);
#endif

/*!
   set_vdec_buff_addr
  */
void set_vdec_buff_addr(u32 addr);

/*!
   set_vdec_buff_addr
  */
u32 get_vdec_buff_addr(void);

#endif
