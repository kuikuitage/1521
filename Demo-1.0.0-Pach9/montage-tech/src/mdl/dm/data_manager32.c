/******************************************************************************/
/******************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mem_manager.h"
#include "class_factory.h"
#include "drv_dev.h"
#include "charsto.h"
#include "video.h"
#include "pti.h"
#include "audio.h"
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_priv.h"
#include "data_manager32.h"

//#define DM_DEBUG
#ifdef DM_DEBUG
#define DMPRINTF OS_PRINTF
#else
#define DMPRINTF DUMMY_PRINTF
#endif

/*!
  Separate fields of this union
  */
typedef struct tag_node_header_inner_t
{
  /*!
    ID
    */
  u16 id;
  /*!
    Node length
    */
  u16 length;
  //u32 checksum: 16;
}node_header_inner_t;

/*!
  Data manager node header structure
  */
typedef union tag_node_header_t
{
  node_header_inner_t inner;
  /*!
    Merged variable
    */
  u32 value;
}node_header_t;

/*!
  Length of node header
  */
#define NODE_HEADER_SIZE              (sizeof(node_header_t))
/*!
  Max node length in data manager 32
  */
#define DM_MAX_NODE_LEN (MAX_NODE_LEN + NODE_HEADER_SIZE)
/*!
  Data manager atom size
  */
#define DM_ATOM_SIZE (4)
/*!
  Check whether this FLASH data is 32 align
  */
#define IS_FLASH_32ALIGN 1

#if IS_FLASH_32ALIGN
#define MIN_NODE_LEN (32)
#else
#define MIN_NODE_LEN (4)
#endif

#define ALIGN4(x)    (((x) + 3) & (~3))
#define ALIGN32(x)    (((x) + 0x1F) & (~0x1F))

//flash related define
#define FLASH_SECTION_SIZE            (64 *1024)
#define CRITICAL_SIZE                 (4*1024)
#define DMH_START_OFFSET               8 //where is the dmh start addr in flash
#define UNKNOWN_ID                    (0xFFFF)
#define ERASE_DATA                    (~0)

#define DM_TAG_SIZE (16)
#define DM_BASE_INFO_HEAD_LEN   12
#define DEFAULT_USED_CACHE_SIZE   0xFF
#define FLASH_BASE_SHIFT  (12)
#define FLASH_HEAD_MGR_UNIT_LENT  12
#define FLAHS_HEAD_MGR_UNIT_NUM 4
#define DM_MAX_HEADER_NUM 3

/*!
  Cache state
  */
typedef enum cache_state_e
{
  /*!
    2bit is '11"
    */
  CACHE_EMPTY = 0x3,
  /*!
    2bit is '01"  
    */
  CACHE_ASYNC = 0x1,
  /*!
    2bit is '00"
    */
  CACHE_SYNC = 0x0,
}cache_state_t;

typedef struct tag_node_info_t
{
  //malloc node_num space to store each node offset in flash
  u32 node_addr :21;  
  // used cache size in current node
  u32 use_cache_size:8; 
  //00b: is sync, 01b is async, 11b is empty
  u32 cache_state :2;
  //0: active 1: deleted
  u32 deleted :1; 
}node_info_t;

typedef struct tag_setcion_header_t
{
  /*!
    prior section
    */
  u32 prior_index:8;
  /*!
    next section
    */
  u32 next_index:8;
  /*!
    setcion init status
    */
  u32 status_init:1;
  /*!
    setcion head status
    */
  u32 status_head:1;
  /*!
    setcion cleanup status
    */
  u32 status_cleanup:2;
  /*!
    setcion cleanup backup section
    */
  u32 status_section:4;
  u32 status_rev:8;
  u8 rev[28];
}setcion_header_t;

/*!
  Data manager setcion information
  */
typedef struct tag_setcion_mgr_t
{
  /*!
    Data manager setcion base address
    */
  u32 cur_addr;
}setcion_mgr_t;

/*!
  Data manager block information
  */
typedef struct tag_block_mgr_t
{
  u32 dm_header_addr;
  /*!
    Block id
    */
  u8 id;
  //refer to enum tag_block_type
  u8 type; 
  /*!
    node number
    */
  u16 node_num;
  /*!
    Data manager base address
    */
  u32 base_addr;
  /*! 
    Data manager block size
    */
  u32 size;
  //below only for type = BLOCK_TYPE_IW
  node_info_t *p_node_info;  
  //current used bytes in total block
  u32 total_used;  
  //first unused node id
  u16 free_id;     
  //form [0:fix_id] is reserved. can't auto dispatch
  u16 fix_id;   
  u16 cache_size;
  //current section index
  u16 sec_index;
  //section info
  setcion_mgr_t *p_sec_info;
  u8 *p_cache;
}block_mgr_t;

typedef struct tag_dm_base_info_t
{
  u32 sdram_size;
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
  //above 12B matchs the struct in flash
  u32 flash_base_addr;
  u32 dmh_start_addr[DM_MAX_HEADER_NUM];
  u16 max_blk_num;
  s32 block_num_off;
  //block_mgr_t block[DM_MAX_BLOCK_NUM];
  block_mgr_t *p_blk_mgr;
}dm_base_info_t;


typedef struct tag_dm_var
{
  /*!
    Data manager information
    */
  dm_base_info_t dm_info;
  /*!
    Char storage device
    */
  charsto_device_t *p_charsto_dev;
}dm_var_t;


/********************************************************
                    Internal functions
 ********************************************************/
/*!
  find block index 
  \return ERR_FAILURE if fail, or index if success
  */
static s32 find_block_index(void *p_data, u8 block_id)
{
  u32 i = 0;
  dm_var_t *p_dm_data = NULL;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_data = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_data->dm_info != NULL);
  p_dm_info = &p_dm_data->dm_info;
  
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    if(block_id == p_dm_info->p_blk_mgr[i].id)
    {
      break;
    }
  }
  
  if(i >= p_dm_info->block_num)
  {
    //DMPRINTF(("%s error! block id[0x%x]\n", __FUNCTION__, block_id));
    //for debug only
    MT_ASSERT(0);
    return ERR_FAILURE;
  }
  else
  {
    return i;
  }
}

/*!
  find first unused id
  \return None
  */
static void find_free_id(void *p_data, u8 index)
{
  dm_var_t *p_dm_priv_data = NULL;
  dm_base_info_t *p_dm_info = NULL;
  block_mgr_t *p_block_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_priv_data = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_priv_data->dm_info != NULL);
  p_dm_info = &p_dm_priv_data->dm_info;
  
  p_block_info = p_dm_info->p_blk_mgr + index;
  
  while(p_block_info->free_id < p_block_info->node_num)
  {
    if(p_block_info->p_node_info[p_block_info->free_id].deleted == 1)
    {
      break;
    }
    p_block_info->free_id ++;
  }
}

/*!
  get dm header info
  \return None
  */
#if 0
static void parser_dm_header(void *p_data)
{
  dm_var_t *p_dm_var = NULL;
  dmh_block_info_t dmh = { 0 };
  dm_base_info_t  *p_dm_info = NULL;
  u32 start_addr = 0;
  u32 i = 0;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(p_dm_var->p_charsto_dev != NULL);

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  charsto_read(p_dm_var->p_charsto_dev, DMH_START_OFFSET, (u8 *)&start_addr, 
    sizeof(u32));
  DMPRINTF(("The DMH at 0x%x\n", start_addr));
  p_dm_info->dmh_start_addr = start_addr;
  
  //read dmh basic info(12B): sdram&flash size, block num, each block head size 
  charsto_read(p_dm_var->p_charsto_dev, start_addr, 
                        (u8 *)p_dm_info, DM_BASE_INFO_HEAD_LEN);

  ////check invalid data
  if(p_dm_info->block_num > p_dm_info->max_blk_num) 
  {
    DMPRINTF(("%s:too much block number! addr :0x%x, total : %d blocks\n",
      __FUNCTION__, start_addr, p_dm_info->block_num));
    MT_ASSERT(0);
  }

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    DMPRINTF(("%s: struct dmh_block_info_t size dismatch!\n",__FUNCTION__));
    MT_ASSERT(0);
  }

  DMPRINTF(("The DMH at 0x%x,blocks total %d\n", 
    start_addr, p_dm_info->block_num));
  start_addr += DM_BASE_INFO_HEAD_LEN;

  for(i = 0;  i < p_dm_info->block_num; i ++)
  {
    charsto_read(p_dm_var->p_charsto_dev, 
      start_addr + i * sizeof(dmh_block_info_t),
        (u8 *) &dmh, sizeof(dmh_block_info_t));

   DMPRINTF(("block id=0x%x,type=0x%x,node=0x%x,addr=0x%x,size=0x%x,crc=0x%x\n",
    dmh.id, dmh.type,dmh.node_num, dmh.base_addr,dmh.size,dmh.crc));

    p_dm_info->p_blk_mgr[i].id = dmh.id;
    p_dm_info->p_blk_mgr[i].type = dmh.type;
    p_dm_info->p_blk_mgr[i].node_num = dmh.node_num;
    p_dm_info->p_blk_mgr[i].base_addr = dmh.base_addr;
    p_dm_info->p_blk_mgr[i].size = dmh.size; 

    if(BLOCK_TYPE_IW == dmh.type)//init iw info
    {
      p_dm_info->p_blk_mgr[i].p_node_info = 
      mtos_malloc(dmh.node_num * sizeof(node_info_t));
      MT_ASSERT(NULL != p_dm_info->p_blk_mgr[i].p_node_info);
      p_dm_info->p_blk_mgr[i].p_sec_info = 
      mtos_malloc(((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE))
        * sizeof(setcion_mgr_t));
      MT_ASSERT(NULL != p_dm_info->p_blk_mgr[i].p_sec_info);
    }
  }
}
#endif

/*!
  find min index of each section 
  \return FALSE if fail, or index if success
  */
static BOOL find_min_addr(void *p_data, u32 lastAddr, u16 secIndex, u32 *p_node_index)
{
  block_mgr_t *p_block = (block_mgr_t *)p_data;
  node_info_t *p_node = NULL;
  u32 i = 0;
  u32 min_abs = ERASE_DATA;
  BOOL min_exist = FALSE;

  for(i = 0; i < p_block->node_num; i ++)
  {
    p_node = p_block->p_node_info + i;
    if(0 == p_node->deleted) //if a valid node
    {
      if((p_node->node_addr >= lastAddr) && (p_node->node_addr < (secIndex + 1) 
        * FLASH_SECTION_SIZE / DM_ATOM_SIZE) && (min_abs > p_node->node_addr - lastAddr))
      {
        min_abs = p_node->node_addr - lastAddr;
        *p_node_index = i;
      }
    }
  }

  if(ERASE_DATA != min_abs)
    min_exist = TRUE;

  return min_exist;
}

/*!
  clean up IW block
  \return FALSE if fail, or TRUE if success
  */
static void cleanup_block(void *p_data,  u8 index, u16 secIndex)
{
  node_header_t *p_header = NULL;
  //mem_mgr_alloc_param_t db_alloc_t = {0} ;
  //mem_mgr_free_param_t db_free_t = {0} ;
  u8 *p_block_buf = NULL; 
  u32 new_node_addr = 0;
  setcion_header_t set_header = {0};

  u16 next_sec_index = 0;

  u32 node_len = 0;
  u32 base_addr = 0;

  u32 pos = 0;
  u32 pos_tmp = 0;
  u32 rpt = 0;

  node_info_t *p_node = NULL;   
  dm_var_t *p_dm_var = NULL;

  dm_base_info_t *p_dm_info = NULL;
  BOOL func_ret = FALSE;
  u32 node_index = 0;
  u32 last_addr = 0;
  u8 cl_rw_buf[DM_MAX_NODE_LEN] = {0};
  u8 cl_tmp_rw_buf[DM_MAX_NODE_LEN] = {0};
 
#if 0 //ndef BOOTLOADER
  // stop video ,do restore
  video_device_t *p_video_dev = 
    (video_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                                           SYS_DEV_TYPE_VIDEO);
  pti_device_t *p_pti_dev = 
    (pti_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    
  audio_device_t *p_audio_dev = 
  (audio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_AUDIO);
  video_es_bus_req_t es_req = {0};
  video_es_bus_req_t es_req_new = {FALSE, FALSE} ;
  
  dev_io_ctrl(p_video_dev, VIDEO_CMD_ES_STATE_QUERY, (u32)&es_req);
  dev_io_ctrl(p_video_dev, VIDEO_CMD_ES_STATE_SET, (u32)&es_req_new);
  dev_io_ctrl(p_video_dev, VIDEO_CMD_VIDEO_DISPLAY, FALSE);
  pti_dev_av_enable(p_pti_dev, FALSE);
  dev_io_ctrl(p_video_dev, VIDEO_CMD_STOP, FALSE);
#endif

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);

  p_dm_info = &p_dm_var->dm_info;

  OS_PRINTF("**********clean block **********\n");
  mtos_task_lock();
  /*!
    wait for device stop, the waiting time must be insure device stop done,
    the time may refer bug#1649, 1522, 1566
    */
  //up the delay time to 100ms
  mtos_task_delay_ms(100);  
  //how many sections
  rpt = ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE)); 
  base_addr = p_dm_info->p_blk_mgr[index].base_addr;
  next_sec_index = (secIndex + 1) % ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE));
 
  /*db_alloc_t.id = MEM_VIDEO_PARTITION ; //mlloc memory as the block clone
  db_alloc_t.size = FLASH_SECTION_SIZE + DM_MAX_NODE_LEN; 
  db_alloc_t.user_id = SYS_MODULE_DVBS;
  p_block_buf = (u8*)MEM_ALLOC(&db_alloc_t);*/
  p_block_buf = mtos_malloc(FLASH_SECTION_SIZE + DM_MAX_NODE_LEN);
  MT_ASSERT(p_block_buf != NULL);
  memset(p_block_buf, 0xff, FLASH_SECTION_SIZE + DM_MAX_NODE_LEN);
  memset(cl_tmp_rw_buf, 0xff, DM_MAX_NODE_LEN);
  last_addr = (secIndex * FLASH_SECTION_SIZE + sizeof(setcion_header_t)) / DM_ATOM_SIZE;
  new_node_addr = secIndex * FLASH_SECTION_SIZE + sizeof(setcion_header_t);

  
  // point 1
  //set cleanup flag step1
  charsto_read(p_dm_var->p_charsto_dev, base_addr + secIndex * FLASH_SECTION_SIZE ,
    (u8 *)(&set_header), sizeof(setcion_header_t));
  set_header.status_cleanup = 0x2;
  set_header.status_section = secIndex;
  charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + secIndex * FLASH_SECTION_SIZE ,
    (u8 *)(&set_header), sizeof(setcion_header_t));

  if(p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)
  {
    // point 2

    //read cleanup section to buffer
    charsto_read(p_dm_var->p_charsto_dev, base_addr + secIndex * FLASH_SECTION_SIZE, 
      p_block_buf, FLASH_SECTION_SIZE);
    //erase last section
    charsto_erase(p_dm_var->p_charsto_dev, base_addr + 
      p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, 1);
    //save to last section
    charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + 
      p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, p_block_buf, FLASH_SECTION_SIZE);
  }
  
  // point 3
  //set cleanup flag step2
  charsto_read(p_dm_var->p_charsto_dev, base_addr + secIndex * FLASH_SECTION_SIZE ,
    (u8 *)(&set_header), sizeof(setcion_header_t));
  set_header.status_cleanup = 0x0;
  charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + secIndex * FLASH_SECTION_SIZE ,
    (u8 *)(&set_header), sizeof(setcion_header_t));
  
  // point 4
  while(TRUE)
  {
    func_ret = find_min_addr(&p_dm_info->p_blk_mgr[index], last_addr, secIndex, &node_index);
    if(FALSE == func_ret)
      break;

    p_node = p_dm_info->p_blk_mgr[index].p_node_info + node_index;
    pos = p_node->node_addr * DM_ATOM_SIZE;

    charsto_read(p_dm_var->p_charsto_dev, base_addr + pos, cl_rw_buf, NODE_HEADER_SIZE);

    p_header = (node_header_t *)(cl_rw_buf);
    MT_ASSERT((u16)(p_header->inner.id) == (node_index));
    node_len = p_header->inner.length * DM_ATOM_SIZE;

    charsto_read(p_dm_var->p_charsto_dev, base_addr + pos, cl_rw_buf , node_len);

    memcpy(p_block_buf + new_node_addr % FLASH_SECTION_SIZE, cl_rw_buf, node_len);

    p_node->node_addr = new_node_addr / DM_ATOM_SIZE;
    new_node_addr += node_len;
    last_addr += node_len / DM_ATOM_SIZE;

    if(0 == new_node_addr % FLASH_SECTION_SIZE)
    {
      #if 0
      charsto_erase(p_dm_var->p_charsto_dev, base_addr + new_node_addr - FLASH_SECTION_SIZE, 1);

      memset((u8 *)(&set_header), 0xff, sizeof(setcion_header_t));
      set_header.status_init = 0;
      memcpy(p_block_buf, (u8 *)(&set_header), sizeof(setcion_header_t));
      charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + new_node_addr - FLASH_SECTION_SIZE ,
        p_block_buf, FLASH_SECTION_SIZE);

      charsto_read(p_dm_var->p_charsto_dev,base_addr + next_sec_index * FLASH_SECTION_SIZE, 
        (u8 *)(&set_header), sizeof(setcion_header_t));

      set_header.status_init = 0;
      set_header.status_head = 0;

      charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + next_sec_index * FLASH_SECTION_SIZE,
        (u8 *)(&set_header), sizeof(setcion_header_t));

      //memset(p_block_buf,0xff,FLASH_SECTION_SIZE + DM_MAX_NODE_LEN);
      #else
      //erase and save cleanup data
      charsto_erase(p_dm_var->p_charsto_dev, base_addr + new_node_addr - FLASH_SECTION_SIZE, 1);
      // point 5
      memset((u8 *)(&set_header), 0xff, sizeof(setcion_header_t));
      set_header.status_init = 0;
      memcpy(p_block_buf, (u8 *)(&set_header), sizeof(setcion_header_t));
      charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + new_node_addr - FLASH_SECTION_SIZE,
        p_block_buf, FLASH_SECTION_SIZE);

      //set head flag for next section
      charsto_read(p_dm_var->p_charsto_dev,base_addr + next_sec_index * FLASH_SECTION_SIZE, 
        (u8 *)(&set_header), sizeof(setcion_header_t));
      set_header.status_init = 0;
      set_header.status_head = 0;
      charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + next_sec_index * FLASH_SECTION_SIZE,
        (u8 *)(&set_header), sizeof(setcion_header_t));

      if(p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)
      {
        //set cleanup flag step3
        charsto_read(p_dm_var->p_charsto_dev,base_addr + 
          p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, 
          (u8 *)(&set_header), sizeof(setcion_header_t));
        set_header.status_cleanup = 0x0;
        charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + 
          p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, 
          (u8 *)(&set_header), sizeof(setcion_header_t));
      }
  
      //memset(p_block_buf,0xff,FLASH_SECTION_SIZE + DM_MAX_NODE_LEN);
      #endif
    }

  }

  OS_PRINTF("**********new_node_addr = 0x%x.**********\n", new_node_addr);
  //reset current writeable address
  p_dm_info->p_blk_mgr[index].total_used -= 
    ((p_dm_info->p_blk_mgr[index].p_sec_info[secIndex].cur_addr * DM_ATOM_SIZE 
    - sizeof(setcion_header_t)) % FLASH_SECTION_SIZE) / DM_ATOM_SIZE;

  p_dm_info->p_blk_mgr[index].p_sec_info[secIndex].cur_addr = new_node_addr / DM_ATOM_SIZE; 

  p_dm_info->p_blk_mgr[index].total_used += 
    ((p_dm_info->p_blk_mgr[index].p_sec_info[secIndex].cur_addr * DM_ATOM_SIZE 
    - sizeof(setcion_header_t)) % FLASH_SECTION_SIZE) / DM_ATOM_SIZE;

  pos_tmp = new_node_addr;
  if(0 != pos_tmp % FLASH_SECTION_SIZE && pos_tmp % FLASH_SECTION_SIZE < FLASH_SECTION_SIZE)
  {
    OS_PRINTF("**********pos_tmp = 0x%x.**********\n", pos_tmp);

    #if 0
    charsto_erase(p_dm_var->p_charsto_dev, 
      base_addr + (pos_tmp / FLASH_SECTION_SIZE) * FLASH_SECTION_SIZE, 1);

    memset((u8 *)(&set_header), 0xff, sizeof(setcion_header_t));
    set_header.status_init = 0;
    memcpy(p_block_buf, (u8 *)(&set_header), sizeof(setcion_header_t));

    charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + 
      (pos_tmp / FLASH_SECTION_SIZE) * FLASH_SECTION_SIZE, 
      p_block_buf , pos_tmp  % FLASH_SECTION_SIZE);

    charsto_read(p_dm_var->p_charsto_dev, base_addr + next_sec_index * FLASH_SECTION_SIZE,
      (u8 *)(&set_header), sizeof(setcion_header_t));

    set_header.status_init = 0;
    set_header.status_head = 0;

    charsto_writeonly(p_dm_var->p_charsto_dev,base_addr + next_sec_index * FLASH_SECTION_SIZE,
      (u8 *)(&set_header), sizeof(setcion_header_t));
    #else
    //erase and save cleanup data
    charsto_erase(p_dm_var->p_charsto_dev, base_addr +
      (pos_tmp / FLASH_SECTION_SIZE) * FLASH_SECTION_SIZE, 1);

    memset((u8 *)(&set_header), 0xff, sizeof(setcion_header_t));
    set_header.status_init = 0;
    if(pos_tmp % FLASH_SECTION_SIZE <= FLASH_SECTION_SIZE / 2)
    {
      set_header.status_head = 0;
    }

    memcpy(p_block_buf, (u8 *)(&set_header), sizeof(setcion_header_t));
    charsto_writeonly(p_dm_var->p_charsto_dev, base_addr +
      (pos_tmp / FLASH_SECTION_SIZE) * FLASH_SECTION_SIZE,
      p_block_buf , pos_tmp  % FLASH_SECTION_SIZE);

    //set head flag for next section
    charsto_read(p_dm_var->p_charsto_dev,base_addr + next_sec_index * FLASH_SECTION_SIZE, 
      (u8 *)(&set_header), sizeof(setcion_header_t));
    set_header.status_init = 0;
    if(!(pos_tmp % FLASH_SECTION_SIZE <= FLASH_SECTION_SIZE / 2))
    {
      set_header.status_head = 0;
    }
    charsto_writeonly(p_dm_var->p_charsto_dev, base_addr + next_sec_index * FLASH_SECTION_SIZE,
      (u8 *)(&set_header), sizeof(setcion_header_t));

    if(p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)
    {
      //set cleanup flag step3
      charsto_read(p_dm_var->p_charsto_dev,base_addr +
        p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, 
        (u8 *)(&set_header), sizeof(setcion_header_t));
      set_header.status_cleanup = 0x0;
      charsto_writeonly(p_dm_var->p_charsto_dev, base_addr +
        p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, 
        (u8 *)(&set_header), sizeof(setcion_header_t));
    }
  
    #endif
  }

  mtos_free(p_block_buf);

  /*db_free_t.id = MEM_VIDEO_PARTITION;
  db_free_t.p_addr = p_block_buf;
  db_free_t.user_id = SYS_MODULE_DVBS;
  FREE_MEM(&db_free_t);*/

#if 0 //ndef BOOTLOADER      
  dev_io_ctrl(p_video_dev, VIDEO_CMD_ES_STATE_SET, (u32)&es_req);
  if(es_req.video_es_r_bus_req == TRUE)
  {
    pti_dev_av_reset(p_pti_dev);
    dev_io_ctrl(p_audio_dev, AUDIO_CMD_AUDIO_RESET, 0);
    pti_dev_av_enable(p_pti_dev, TRUE);
    dev_io_ctrl(p_video_dev, VIDEO_CMD_PLAY, 0);
    dev_io_ctrl(p_video_dev, VIDEO_CMD_START_WAIT_FRAME, 0);
  }

#endif
  mtos_task_unlock();
}
/*!
  Write a del operation to flash
  \return ERR_FAILURE if fail, or index if success
  */
static void write_del_node(void *p_data, u8 index, u16 node_id)
{
  u8 temp_buf[MIN_NODE_LEN] = {0};
  u32 write_len = MIN_NODE_LEN; //write a node with header only
  u32 cur_addr = 0;

  dm_var_t *p_dm_var = NULL;
  node_header_t *p_header = (node_header_t *)temp_buf;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  p_header->inner.id = node_id;
  p_header->inner.length = 0;

  MT_ASSERT(BLOCK_TYPE_IW == p_dm_info->p_blk_mgr[index].type);

  cur_addr = (u32)(p_dm_info->p_blk_mgr[index].p_sec_info[
    p_dm_info->p_blk_mgr[index].sec_index].cur_addr * DM_ATOM_SIZE);
  while((cur_addr % FLASH_SECTION_SIZE + write_len) >= FLASH_SECTION_SIZE)  //restore force
  {
    cleanup_block(p_data, index, p_dm_info->p_blk_mgr[index].sec_index);
    OS_PRINTF("############write_del_node .**********\n");

    //cleanup_block will call dm_write_node, nesting look out !!
    //reset
    MT_ASSERT(p_dm_info->p_blk_mgr[index].total_used * DM_ATOM_SIZE < 
      ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE):
        (p_dm_info->p_blk_mgr[index].size))); 

    cur_addr = (u32)(p_dm_info->p_blk_mgr[index].p_sec_info[
      p_dm_info->p_blk_mgr[index].sec_index].cur_addr * DM_ATOM_SIZE);
    
    if(cur_addr % FLASH_SECTION_SIZE <= FLASH_SECTION_SIZE / 2)
    {
       break;
    }
    else
    {
      p_dm_info->p_blk_mgr[index].sec_index = (p_dm_info->p_blk_mgr[index].sec_index + 1) % 
        ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
          (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
          (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE));

      cur_addr = (u32)(p_dm_info->p_blk_mgr[index].p_sec_info[
        p_dm_info->p_blk_mgr[index].sec_index].cur_addr * DM_ATOM_SIZE);
    }
    
  }

  if(SUCCESS != charsto_writeonly(p_dm_var->p_charsto_dev, 
    p_dm_info->p_blk_mgr[index].base_addr + cur_addr, temp_buf, write_len))
  {
    MT_ASSERT(0);
  }

  p_dm_info->p_blk_mgr[index].p_sec_info[
    p_dm_info->p_blk_mgr[index].sec_index].cur_addr += write_len / DM_ATOM_SIZE;
  p_dm_info->p_blk_mgr[index].total_used += write_len / DM_ATOM_SIZE;
}

/*!
  reconstruct IW block node info
  \return FALSE if fail, or TRUE if success
  */
static BOOL iw_restore_block(void *p_data, u8 index)
{
  node_info_t *p_node = NULL;
  setcion_mgr_t *p_sec = NULL;
  node_header_t header = {{0}};
  setcion_header_t set_header = {0};
  setcion_header_t set_header_rev = {0};
  u8 *p_block_buf = NULL; 
  
  u32 base_addr = 0;
  u32 cur_pos = 0;
  u32 step = 0;
  u32 i = 0;
  u32 tmp_index = 0;
  u32 sec_used = 0;

  dm_var_t *p_dm_priv_data = NULL;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_priv_data = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_priv_data->dm_info != NULL);
  p_dm_info = &p_dm_priv_data->dm_info;

  base_addr = p_dm_info->p_blk_mgr[index].base_addr;
  p_node = p_dm_info->p_blk_mgr[index].p_node_info;
  p_sec = p_dm_info->p_blk_mgr[index].p_sec_info;

  //set node to ~0 means: the node is deleted, the cache is empty
  //set default first
  memset(p_node, 0xFF, p_dm_info->p_blk_mgr[index].node_num * sizeof(p_node));
  memset(p_sec, 0x0, ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE)) * sizeof(setcion_mgr_t));

  p_dm_info->p_blk_mgr[index].total_used = 0;

  if(p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)
  {
    charsto_read(p_dm_priv_data->p_charsto_dev, base_addr + p_dm_info->p_blk_mgr[index].size - 
      FLASH_SECTION_SIZE,(u8 *)(&set_header_rev), sizeof(setcion_header_t));
    
    for(i = 0; i < (p_dm_info->p_blk_mgr[index].size / (FLASH_SECTION_SIZE) - 1); i ++)
    {
      cur_pos = i * FLASH_SECTION_SIZE ;
      //read a section header
      charsto_read(p_dm_priv_data->p_charsto_dev,
        base_addr + cur_pos,(u8 *)(&set_header), sizeof(setcion_header_t));
      

      if((0x0 == set_header.status_cleanup || 0x3 == set_header.status_cleanup)
        && 0x2 == set_header_rev.status_cleanup && i == set_header_rev.status_section)
      {
      
        charsto_erase(p_dm_priv_data->p_charsto_dev, base_addr + 
          i * FLASH_SECTION_SIZE, 1);

        p_block_buf = mtos_malloc(FLASH_SECTION_SIZE + DM_MAX_NODE_LEN);
        
        charsto_read(p_dm_priv_data->p_charsto_dev, base_addr + 
          p_dm_info->p_blk_mgr[index].size - FLASH_SECTION_SIZE, p_block_buf, FLASH_SECTION_SIZE);

        charsto_writeonly(p_dm_priv_data->p_charsto_dev, base_addr + 
          i * FLASH_SECTION_SIZE, p_block_buf, FLASH_SECTION_SIZE);
          
        mtos_free(p_block_buf);
      }
    }
  }
  

  for(i = 0; i < ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE)); i ++)
  {
    cur_pos = i * FLASH_SECTION_SIZE ;
    //read a section header
    charsto_read(p_dm_priv_data->p_charsto_dev,
      base_addr + cur_pos,(u8 *)(&set_header), sizeof(setcion_header_t));

    if(1 == set_header.status_init)
    {
      if(1 == set_header.status_head)
      {
        if(0 == i)
        {
          sec_used ++;
          p_dm_info->p_blk_mgr[index].sec_index = i;
        }
        break;
      }
    }
    else
    {
      if(1 == set_header.status_head)
      {
        sec_used ++;
      }
      else
      {
        sec_used ++;
        p_dm_info->p_blk_mgr[index].sec_index = i;
      }
    }
  }

  if(1 == sec_used)
  {
    set_header.status_init = 0;
    set_header.status_head = 0;
    charsto_writeonly(p_dm_priv_data->p_charsto_dev, 
      base_addr , (u8 *)(&set_header), sizeof(setcion_header_t));
  }

  if(sec_used != ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE)))
    tmp_index = 0;
  else
  {
    tmp_index = (p_dm_info->p_blk_mgr[index].sec_index + 1) %
      ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE));
  }

  for(i = 0; i < ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE)); i ++)
  {
    cur_pos = tmp_index * FLASH_SECTION_SIZE + sizeof(setcion_header_t);

    while(cur_pos < (tmp_index + 1) * FLASH_SECTION_SIZE)
    {
      //read a node
      charsto_read(p_dm_priv_data->p_charsto_dev,
        base_addr + cur_pos,(u8 *)(&header), sizeof(node_header_t));

      if(ERASE_DATA == header.value)  //find invalid data, means read finish
      {
        DMPRINTF("restore block %d section %d at end %d\n", index, tmp_index, cur_pos);
          p_sec[tmp_index].cur_addr = cur_pos / DM_ATOM_SIZE;
        break;
      }

      if(header.inner.id >= p_dm_info->p_blk_mgr[index].node_num)
      {
        //unexpected data
        return FALSE;
      }

      //ID is valid and length is 0, means a del node
      if(0 == header.inner.length)
      {
        p_node[header.inner.id].deleted = 1;  //set the node is deleted
        step = MIN_NODE_LEN;            //reserve free space
      }
      else
      {
        // a valid node
        step = (header.inner.length * DM_ATOM_SIZE);
        if(step > DM_MAX_NODE_LEN)
        {
          //unexpected data
          return FALSE;
        }
        p_node[header.inner.id].deleted = 0;
        p_node[header.inner.id].node_addr = cur_pos / DM_ATOM_SIZE;
      }

      cur_pos += step;
    }

    p_dm_info->p_blk_mgr[index].total_used += 
      (cur_pos - tmp_index * FLASH_SECTION_SIZE - sizeof(setcion_header_t)) / DM_ATOM_SIZE;

    tmp_index = (tmp_index + 1) % 
      ((p_dm_info->p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[index].size / FLASH_SECTION_SIZE));

  }

  return TRUE;
}


/********************************************************
                    API functions
 ********************************************************/

/*!
  \param[in] p_data Pointer to store data
  \param[in] length check length
  \return data summation
  */
/*
static u16 data_checksum(node_header_t *p_header, u8 *p_data)
{
  u32 i = 0;
  u16 sum = 0;
  u32 data_len = 0;
  MT_ASSERT(p_header != NULL);

  data_len = p_header->length*DM_ATOM_SIZE - NODE_HEADER_SIZE;
  for (i=0; i<data_len; i++)
  {
    sum += p_data[i];
  }
  return sum + p_header->id + p_header->length;
}
*/

/*!
  read data from flash block to ram buffer
  
  \param[in] block_id Block ID 
  \param[in] node_id node id for IW block , meaningless for RO block
  \param[in] offset Offset in block for RO block , offset in node for IW block
  \param[in] length read length
  \param[in] pbuffer Pointer to store data
  \return 0 if fail, actual length if success
  */
static u32 dm_read_32(void *p_data, u8 block_id, u16 node_id, u16 offset, 
  u32 length, u8 *p_buffer)
{
  u32 read_len = 0;
  u32 node_addr = 0;
  s32 bid = 0;
  
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  MT_ASSERT(p_buffer != NULL && length > 0);

  bid = find_block_index(p_data, block_id);

  if(ERR_FAILURE == bid)
  {
    return 0;
  }
  
  if(BLOCK_TYPE_RO == p_dm_info->p_blk_mgr[bid].type) // read only block
  {
    node_addr = p_dm_info->p_blk_mgr[bid].base_addr + offset;
    if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr, 
                                                             p_buffer, length))
    {
      read_len = length;
    }
  }
  else  //increasing write block
  {
    block_mgr_t *p_block = p_dm_info->p_blk_mgr + bid;
    node_info_t *p_node = p_block->p_node_info + node_id;

    if((node_id >= p_block->node_num) || p_node->deleted)
    {
      return 0; //invalid node id 
    }
    MT_ASSERT((offset + length) <= MAX_NODE_LEN);
    read_len = offset + length;
    //calculate read addr,ignore head and locate to offset start
    node_addr = p_block->base_addr + 
      p_node->node_addr * DM_ATOM_SIZE + NODE_HEADER_SIZE;

    if((p_block->cache_size >0) 
      &&(p_node->cache_state == CACHE_EMPTY))
    {
      //read the cache
      if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr,
        p_block->p_cache + p_block->cache_size * node_id, p_block->cache_size))
      {
        p_node->cache_state = CACHE_SYNC;
      }
      else
      {
        MT_ASSERT(0);
      }
    }

    if(read_len <= p_block->cache_size)  //all the data be in cache
    {
      memcpy(p_buffer, 
        p_block->p_cache + p_block->cache_size * node_id + offset,
        length);
      read_len = length;
    }
    else
    {
      if(p_block->cache_size >0)
      {
        memcpy(rw_buf, p_block->p_cache + p_block->cache_size * node_id,
          p_block->cache_size);
        read_len -= p_block->cache_size;
        node_addr += p_block->cache_size;
      }
      //how do check sum
      if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr, 
        rw_buf + p_block->cache_size, read_len))
      {
        memcpy(p_buffer, rw_buf + offset, length);
        read_len = length;
      }
      else
      {
        MT_ASSERT(0);
      }
    }
  }

  return read_len;
}

static dm_ret_t write_node_to_flash(void *p_data, u8 dm_inx, u16 node_id, 
u8 *p_buffer, u16 len)
{
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_priv_data = NULL;
  block_mgr_t *p_block = NULL;
  
  node_info_t *p_node = NULL;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  node_header_t *p_header = (node_header_t *)rw_buf;
  u32 cur_addr = 0;
  u16 write_len = ALIGN4(len + NODE_HEADER_SIZE);
  MT_ASSERT(p_buffer != NULL && len != 0);

  MT_ASSERT(p_data != NULL);
  p_dm_priv_data = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_priv_data->dm_info != NULL);
  p_dm_info = &p_dm_priv_data->dm_info;

  p_block = p_dm_info->p_blk_mgr + dm_inx;
  MT_ASSERT(p_block->type == BLOCK_TYPE_IW);

  cur_addr = (u32)(p_block->p_sec_info[p_block->sec_index].cur_addr * DM_ATOM_SIZE);
#if IS_FLASH_32ALIGN
  write_len = ALIGN32(write_len);
#endif

  MT_ASSERT(write_len <= ALIGN32(DM_MAX_NODE_LEN));

  while((cur_addr % FLASH_SECTION_SIZE + write_len) >= FLASH_SECTION_SIZE)  //restore force
  {
    cleanup_block(p_data, dm_inx, p_block->sec_index);
    OS_PRINTF("**********write_node_to_flash .**********\n");
    
    //cleanup_block will call dm_write_node, nesting look out !!
    //reset

    MT_ASSERT(p_block->total_used * DM_ATOM_SIZE < 
      ((p_block->size > FLASH_SECTION_SIZE)?
      (p_block->size - FLASH_SECTION_SIZE):
      (p_block->size))); 
    cur_addr = (u32)(p_block->p_sec_info[p_block->sec_index].cur_addr * DM_ATOM_SIZE);

    if(cur_addr % FLASH_SECTION_SIZE <= FLASH_SECTION_SIZE / 2)
    {
      break;
    }
    else
    {
      p_block->sec_index = (p_block->sec_index + 1) % 
        ((p_block->size > FLASH_SECTION_SIZE)?
        (p_block->size / FLASH_SECTION_SIZE - 1):
        (p_block->size / FLASH_SECTION_SIZE));

      cur_addr = (u32)(p_block->p_sec_info[p_block->sec_index].cur_addr * DM_ATOM_SIZE);
    }

 
  }

  p_header->inner.id = node_id;
  p_header->inner.length = write_len / DM_ATOM_SIZE;
  memcpy(rw_buf + NODE_HEADER_SIZE, p_buffer, len);
  //p_header->checksum = data_checksum(p_header, temp_buf+NODE_HEADER_SIZE);
  p_node = p_block->p_node_info + p_header->inner.id;
  
  if(p_block->cache_size > 0)
  {
    u16 cpy_size = (len > p_block->cache_size) ? p_block->cache_size : len;
    memcpy(p_block->p_cache + p_block->cache_size * p_header->inner.id, 
      p_buffer, cpy_size);
    p_node->cache_state = CACHE_SYNC;
    p_node->use_cache_size = 0;   //no cache need flush
  }

  if(SUCCESS == charsto_writeonly(p_dm_priv_data->p_charsto_dev, 
        p_block->base_addr + cur_addr, rw_buf, write_len))
  {
    p_node->deleted = 0;
    p_node->node_addr = cur_addr / DM_ATOM_SIZE;
    //increase pointer to free space
    p_block->p_sec_info[p_block->sec_index].cur_addr += write_len / DM_ATOM_SIZE; 
    p_block->total_used += write_len / DM_ATOM_SIZE; 

    return DM_SUC;
  }
  else  // write fail
  {
    MT_ASSERT(0);
    return DM_FAIL;
  }

}

/*!
  write data to a node
  
  \param[in] block_id Block ID 
  \param[in,out] p_node_id UNKNOWN_ID to write a new node, else modify a node
  \param[in] length Data length to write
  \param[in] pbuffer Data pointer
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_write_node_32(void *p_data, u8 block_id, 
u16 *p_node_id, u8 *p_buffer, u16 len)
{
  block_mgr_t *p_block = NULL;
  s32 index = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  
  MT_ASSERT(p_buffer != NULL && len > 0);
  MT_ASSERT(p_data != NULL);
  
  p_dm_var = (dm_var_t *)p_data;
  MT_ASSERT(&p_dm_var->dm_info != NULL);

  p_dm_info = &p_dm_var->dm_info;
  
  index = find_block_index(p_data, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  if((ERR_FAILURE == index) || (p_block->type == BLOCK_TYPE_RO))
  {
    return DM_FAIL;
  }
  
  if(UNKNOWN_ID == *p_node_id)  // add new node
  {
    find_free_id(p_data, index);
    if(p_block->free_id >= p_block->node_num)
    {
      DMPRINTF("%s:dm if full!\n", __FUNCTION__);
      return DM_FULL;
    }
    *p_node_id = p_block->free_id;
  }
  else  //edit a node
  {
    if(*p_node_id >= p_block->node_num)
    {
      DMPRINTF("%s: invalid node id[0x%x]!\n", __FUNCTION__, *p_node_id);
      return DM_FAIL; 
    }
  }

  if(len <= p_block->cache_size) //write cache
  {
    node_info_t *p_node = p_block->p_node_info + (*p_node_id);
    p_node->deleted = 0;
    memcpy(p_block->p_cache + p_block->cache_size * (*p_node_id), 
        p_buffer, len);
    p_block->p_node_info[*p_node_id].cache_state = CACHE_ASYNC;

    //Initialize node address
    //if(p_block->p_node_info[*p_node_id].use_cache_size 
    // == DEFAULT_USED_CACHE_SIZE)
    {
    //	p_block->p_node_info[*p_node_id].node_addr = 
    //  (u32)(p_block->cur_addr * DM_ATOM_SIZE);
    }

    if((p_block->p_node_info[*p_node_id].use_cache_size 
                          == DEFAULT_USED_CACHE_SIZE) //0xFF is init var
    ||(p_block->p_node_info[*p_node_id].use_cache_size < len))  //record the max
    {
      // need 4 byte alignment?
      //len = ALIGN4(len);
      p_block->p_node_info[*p_node_id].use_cache_size = len;
    }
  }
  else  //write flash
  {
    write_node_to_flash(p_data, index, *p_node_id, p_buffer, len);
  }
  return DM_SUC;
}

/*!
  Delete a node, only for IW block
  
  \param[in] block_id Block ID 
  \param[in] node_id Node ID
  \param[in] b_sync TRUE to write flash at once
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_del_node_32(void *p_data, u8 block_id, u16 node_id, 
BOOL b_sync)
{
  u32 index = 0;
  block_mgr_t *p_block = NULL;
  node_info_t *p_node = NULL;

  dm_var_t *p_dm_var = NULL;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  index = find_block_index(p_data, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  p_node = p_block->p_node_info + node_id;
  
  if((ERR_FAILURE == index)
    || (node_id >= p_block->node_num)
    || p_node->deleted)
  {
    return DM_FAIL;
  }
  
  p_node->deleted = 1;
  //reserve calc free space
  if(b_sync == TRUE)
  {
    write_del_node(p_data, index, node_id);
  }
  
  if(node_id < p_block->free_id)
  {
    p_block->free_id = node_id;
  }
  
  return DM_SUC;
}

/*!
  Active a node, only for IW block
  
  \param[in] block_id Block ID 
  \param[in] node_id Node ID
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_active_node_32(void *p_data, u8 block_id, u16 node_id)
{
  u32 index = 0;
  block_mgr_t *p_block = NULL;
  node_info_t *p_node = NULL;

  dm_var_t *p_dm_var = NULL;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  index = find_block_index(p_data, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  p_node = p_block->p_node_info + node_id;

  if((ERR_FAILURE == index) 
    || (node_id >= p_block->node_num)
    || (p_node->deleted == 0))
  {
    return DM_FAIL;
  }
  p_node->deleted = 0;
  return DM_SUC;
}

/*!
  Check block status, clean up blocks if needed
  
  \return None
  */
static void dm_check_block_32(void *p_data)
{
  u8 i = 0;
  u8 j = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    if((BLOCK_TYPE_IW == p_dm_info->p_blk_mgr[i].type)
      &&((p_dm_info->p_blk_mgr[i].total_used * DM_ATOM_SIZE) 
            > ((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
              (p_dm_info->p_blk_mgr[i].size - FLASH_SECTION_SIZE):
              (p_dm_info->p_blk_mgr[i].size))))
    {
      //todo:
      for(j = 0;
        j < ((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE));
        j++)
        cleanup_block(p_data, i,j);
      break;
    }
  }
}

/*!
  Declare all IW node ID are invalid
  
  \param[in] block_id Block ID 
  \return None
  */
static dm_ret_t dm_reset_iw_block_32(void *p_data, u8 block_id, BOOL is_sync)
{
  u32 index = 0;
  u32 i = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  
  MT_ASSERT(p_data != NULL);
  p_dm_var = p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  index = find_block_index(p_data, block_id);
  if(ERR_FAILURE == index)
  {
    return DM_FAIL;
  }
  
  for(i = 0; i < p_dm_info->p_blk_mgr[index].node_num; i ++)
  {
    p_dm_info->p_blk_mgr[index].p_node_info[i].deleted = 1;
    if(is_sync)
    {
      write_del_node(p_data, index, i);
    }
  }

  p_dm_info->p_blk_mgr[index].free_id = p_dm_info->p_blk_mgr[index].fix_id;
  return DM_SUC;
}

/*!
  get block start offset in flash
  
  \param[in] block_id Block ID 
  \return 0 if fail, offset if success, 
  */
static u32 dm_get_block_addr_32(void *p_data, u8 block_id)
{
  s32 i = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  i = find_block_index(p_data, block_id);

  if(ERR_FAILURE == i)
  {
    return 0;
  }
  else
  {
    return p_dm_info->flash_base_addr + p_dm_info->p_blk_mgr[i].base_addr;
  }
}

static u32 dm_get_block_size_32(void *p_data, u8 block_id)
{
  s32 i = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  i = find_block_index(p_data, block_id);
  if(ERR_FAILURE == i)
  {
    return 0;
  }
  else
  {
   return (BLOCK_TYPE_RO == p_dm_info->p_blk_mgr[i].type)?
     (p_dm_info->p_blk_mgr[i].size):((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
      (p_dm_info->p_blk_mgr[i].size - FLASH_SECTION_SIZE):
      (p_dm_info->p_blk_mgr[i].size));
  }
}
/*!
  get block total number and each block ID
  
  \param[out] block_num Total block ID 
  \param[out] id_list Block ID list
  \return None
  */
static dm_ret_t dm_get_block_info_32(void *p_data, u8 *p_block_num, 
u8 *p_id_list)
{
  u32 i = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  
  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  MT_ASSERT(p_block_num != 0 && p_id_list != NULL);
  
  *p_block_num = p_dm_info->block_num;
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    p_id_list[i] = p_dm_info->p_blk_mgr[i].id;
  }
  return DM_SUC;
}

/*!
  get block head info for a specific block id
  
  \param[in] block_id Block ID 
  \param[out] head return header info
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_get_block_header_32(void *p_data, u8 block_id, 
dmh_block_info_t *p_dm_head)
{
  s32 i = 0;
  u32 start = 0;
  s32 offset = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(&p_dm_var->dm_info != NULL);
  
  p_dm_info = &p_dm_var->dm_info;
  
  i = find_block_index(p_data, block_id);

  if(ERR_FAILURE == i)
  {
    return DM_FAIL;
  }

  start = p_dm_info->p_blk_mgr[i].dm_header_addr + DM_TAG_SIZE +
    DM_BASE_INFO_HEAD_LEN;

  if(i > p_dm_info->block_num_off)
  {
    offset = (i - p_dm_info->block_num_off) * (s32)sizeof(dmh_block_info_t);
  }
  else if(i == p_dm_info->block_num_off)
  {
    offset = (i - p_dm_info->block_num_off) * (s32)sizeof(dmh_block_info_t);
  }
  else
  {
    offset = i * (s32)sizeof(dmh_block_info_t);
  }

  start = start + offset;
  OS_PRINTF("dm_get_block_header_32 addr: 0x%x\n",
    start);
  if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, 
                              start,
                              (u8 *)p_dm_head, sizeof(dmh_block_info_t)))
  {
    return DM_SUC;
  }
  else 
  {
    return DM_FAIL;
  }
}

/*!
  display flash map info
  \return None
  */
static void dm_show_flash_map_32(void *p_data)
{
#ifdef ASSERT_ON  
  u32 offset = 0;
  u32 bl_sec_size = 0;
  u32 end = 0;
  u32 i = 0;
  u32 tmp[3] = {0};
  dmh_block_info_t info = {0};
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  charsto_read(p_dm_var->p_charsto_dev, offset,(u8 *)tmp, FLASH_BASE_SHIFT);
  //OS_PRINTF("bootloader: dm start[0x%x], 
  //                     bl sec size[0x%x]\n", tmp[1], tmp[2]);
  bl_sec_size = tmp[2];
  
  offset = FLASH_BASE_SHIFT;
  for(i = 0; i < FLAHS_HEAD_MGR_UNIT_NUM; i ++)
  {
    charsto_read(p_dm_var->p_charsto_dev, offset,(u8 *)tmp, 
                                                    FLASH_HEAD_MGR_UNIT_LENT);
    // bin file id exists
    if(tmp[0] == 1) 
    {
      //OS_PRINTF("bootloader: bin id[%d], start[0x%x],size[0x%x]\n", 
      //  tmp[0],tmp[1], tmp[2]);
      end = (tmp[1] + tmp[2]);
    }
    offset += FLASH_HEAD_MGR_UNIT_LENT;
  }

  //OS_PRINTF("bootloader: free space [0x%x]\n", bl_sec_size-end);

  end = 0;
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    dm_get_block_header(p_data, p_dm_info->p_blk_mgr[i].id, &info);

    // if there is a hole in flash
    if(end != 0 &&(p_dm_info->p_blk_mgr[i].base_addr - end) != 0) 
    {
      //OS_PRINTF("dm: flash free space [0x%x] here!\n", 
      // dm_info.block[i].base_addr-end);
    }
    
    // OS_PRINTF("dm:id[0x%x],type[%d],node[%d],start[0x%x],
    // size[0x%x],used[0x%x]\n",
    // dm_info.block[i].id, dm_info.block[i].type,dm_info.block[i].node_num,
    // dm_info.block[i].base_addr,dm_info.block[i].size, 
    // dm_info.block[i].cur_addr);

    end = p_dm_info->p_blk_mgr[i].base_addr + p_dm_info->p_blk_mgr[i].size;
  }

#endif
}

/*!
  Special function: read bootloader block data from bootloader section
  It is not a part of data manager header, just put here for use
  \param[in] bid bootloader bin file ID, see sys_define.h 
  \param[in] offset Offset from the bin file start address
  \param[in] length read length
  \param[in] pbuffer Pointer to store data
  \return actual length if success, else ERR_FAILURE
  */
static s32 dm_read_bl_block_32(void *p_handle, u32 bid, u32 offset, 
u32 len, u8 *p_buff)
{
  u32 base_addr = 0;
  u32 start = FLASH_BASE_SHIFT;
  u32 size = 0;
  u32 i = 0;
  u32 head[FLAHS_HEAD_MGR_UNIT_NUM] = {0,};    
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
                SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_charsto_dev != NULL);

  for(i = 0; i < FLAHS_HEAD_MGR_UNIT_NUM; i ++)
  {
    if(SUCCESS != charsto_read(p_charsto_dev, 
        start + i * FLASH_HEAD_MGR_UNIT_LENT, 
          (u8 *)head, FLASH_HEAD_MGR_UNIT_LENT))
    {
      return ERR_FAILURE;
    }
    
    if(bid == head[0])
    {
      base_addr = head[1];
      size = head[2];      
      break;
    }
  }

  if((i >= FLAHS_HEAD_MGR_UNIT_NUM) ||(offset > size))
    return ERR_FAILURE;

  len = ((size - offset) > len) ? len : (size - offset);
  if(SUCCESS != charsto_read(p_charsto_dev, base_addr + offset, 
          p_buff, len))
  {
    return ERR_FAILURE;
  }
  else 
  {
    return len;
  }
}

static void dm_set_fix_id_32(void *p_data, u8 block_id, u16 fix_id)
{
  s32 bid = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var =  (dm_var_t *) p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  bid = find_block_index(p_data,block_id);
  
  if((bid == ERR_FAILURE) 
    ||(p_dm_info->p_blk_mgr[bid].type != BLOCK_TYPE_IW))
  {
    return;
  }
  
  //use the max
  if(fix_id > p_dm_info->p_blk_mgr[bid].fix_id)
  {
     p_dm_info->p_blk_mgr[bid].fix_id = fix_id;
  }
  if(fix_id >  p_dm_info->p_blk_mgr[bid].free_id)
  {
     p_dm_info->p_blk_mgr[bid].free_id = fix_id;
  }
}

static dm_ret_t dm_restore_cache_32(void *p_data, u8 block_id, 
u16 node_id)
{
  s32 block_indx = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  block_indx = find_block_index(p_data, block_id);
  p_dm_info->p_blk_mgr[block_indx].p_node_info[node_id].cache_state 
      = CACHE_EMPTY;
  return DM_SUC;
}

static dm_ret_t dm_flush_cache_32(void *p_data, u8 block_id, u16 node_id)
{
  s32 block_indx = 0;
  dm_var_t *p_dm_var = NULL;
  dm_base_info_t *p_dm_info = NULL;
  block_mgr_t *p_block = NULL;
  node_info_t *p_node = NULL;
  
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  u8 *p_cache_buf = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  MT_ASSERT(p_dm_var->p_charsto_dev != NULL);
  
  p_dm_info = &p_dm_var->dm_info;
  
  block_indx = find_block_index(p_data, block_id);
  p_block = p_dm_info->p_blk_mgr + block_indx;
  p_node = p_block->p_node_info + node_id;
  p_cache_buf = p_block->p_cache + p_block->cache_size * node_id;
  
  //node_header_t *p_header = (node_header_t *)rw_buf;
  //the current node isn't exsit, write it
  if(p_node->deleted == 1)
  {
    //write_node_to_flash(p_data, block_indx, node_id, p_cache_buf, 
      //p_node->use_cache_size);
    write_del_node(p_data, block_indx, node_id);
  }
  else
  {
    if((p_block->cache_size >0)
    && (p_node->cache_state == CACHE_ASYNC)
    && (p_node->use_cache_size > 0)
    && (p_node->use_cache_size <= p_block->cache_size))
    {
      /*u32 node_addr = p_block->base_addr + p_node->node_addr * DM_ATOM_SIZE;
      s32 cmp_rslt = 0;

      charsto_read(p_dm_var->p_charsto_dev, node_addr, 
          rw_buf, DM_MAX_NODE_LEN);


      cmp_rslt = memcmp(rw_buf + NODE_HEADER_SIZE, 
      p_cache_buf, p_node->use_cache_size);
      if(cmp_rslt != 0)*/
      {
        node_header_t *p_head = (node_header_t *)rw_buf;
        u32 node_addr = 0;
        if(p_node->node_addr != 0x1fffff)
        {
          node_addr = p_block->base_addr + p_node->node_addr * DM_ATOM_SIZE;

          charsto_read(p_dm_var->p_charsto_dev, node_addr, 
            rw_buf, DM_MAX_NODE_LEN);
          memcpy(rw_buf + NODE_HEADER_SIZE, p_cache_buf, p_node->use_cache_size);
          write_node_to_flash(p_data, block_indx, node_id, 
            rw_buf + NODE_HEADER_SIZE,
            p_head->inner.length * DM_ATOM_SIZE - NODE_HEADER_SIZE);
        }
        else
        {
          //node not exist before
          memcpy(rw_buf + NODE_HEADER_SIZE, p_cache_buf, p_node->use_cache_size);
          write_node_to_flash(p_data, block_indx, node_id, 
            rw_buf + NODE_HEADER_SIZE,
            p_node->use_cache_size);
        }
        
        
      }
    }
  }

  //if the cache more than the data????
  return DM_SUC;
}

static dm_ret_t dm_set_cache_32(void *p_data, u8 block_id, u16 cache_size)
{
  s32 block_indx = 0;
  dm_var_t *p_dm_var = NULL;
  dm_base_info_t *p_dm_info = NULL;
  
  block_mgr_t *p_block_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  //the current using cache size is u8 : node_info_t:: use_cache_size
  MT_ASSERT(cache_size < DEFAULT_USED_CACHE_SIZE); 

  block_indx = find_block_index(p_data, block_id);
  p_block_info = p_dm_info->p_blk_mgr + block_indx;
  
  //Cache protection machanism
  //Avoid allocate cache for one table block for more times
  if(p_block_info->type == BLOCK_TYPE_RO || cache_size == 0)
  {
    return DM_FAIL;
  }
  
  p_block_info->p_cache =  mtos_malloc(p_block_info->node_num * cache_size);

  MT_ASSERT(NULL != p_block_info->p_cache);
  p_block_info->cache_size = cache_size;

  return DM_SUC;
}

// Todo: fix me, defined but not used
#if 0
static u16 dm_get_cache_size_32(void *p_data, u8 block_id)
{
  s32 block_indx = 0;
  dm_base_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  block_indx = find_block_index(p_data, block_id);
  return p_dm_info->p_blk_mgr[block_indx].cache_size;
}
#endif

static u16  dm_get_max_node_len_32(void *p_data)
{
  u16 max_node_len = MAX_NODE_LEN;

  return max_node_len;
}


static u16 dm_get_max_blk_num_32(void *p_data)
{
  dm_var_t *p_dm_var = NULL;
  dm_base_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;
  
  return p_dm_info->block_num;
}

static void set_header(void *p_data, u32 addr)
{
  dm_var_t *p_dm_var = NULL;
  dmh_block_info_t dmh = { 0 };
  dm_base_info_t temp_info = {0};
  dm_base_info_t  *p_dm_info = NULL;
  u32 start_addr = addr;
  u32 i = 0;
  u32 old_num = 0;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(p_dm_var->p_charsto_dev != NULL);

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &temp_info;
 
  //charsto_read(p_dm_var->p_charsto_dev, DMH_START_OFFSET, (u8*)&start_addr, 
  //  sizeof(u32));
  //p_dm_info->dmh_start_addr[] = start_addr;

  //read dmh basic info(12B): sdram&flash size, block num, each block head size 
  charsto_read(p_dm_var->p_charsto_dev, start_addr + DM_TAG_SIZE, 
                        (u8 *)p_dm_info, DM_BASE_INFO_HEAD_LEN);

  ////check invalid data
  if(p_dm_var->dm_info.block_num + p_dm_info->block_num >
    p_dm_var->dm_info.max_blk_num) 
  {
    DMPRINTF("%s:too much block number! addr :0x%x, total : %d blocks\n",
      __FUNCTION__, start_addr, p_dm_info->block_num);
    MT_ASSERT(0);
  }
  
  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    DMPRINTF("%s: struct dmh_block_info_t size dismatch!\n",__FUNCTION__);
    MT_ASSERT(0);
  }
  
  DMPRINTF("The DMH at 0x%x,blocks total %d\n", 
    start_addr, p_dm_info->block_num);
  //start_addr += DM_BASE_INFO_HEAD_LEN;
  old_num = p_dm_var->dm_info.block_num;
  if(0 == p_dm_var->dm_info.block_num_off)
    p_dm_var->dm_info.block_num_off = p_dm_info->block_num;
  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    u32 index = i + old_num;
    p_dm_var->dm_info.block_num++;
    charsto_read(p_dm_var->p_charsto_dev, 
      start_addr + DM_TAG_SIZE + DM_BASE_INFO_HEAD_LEN +
      i * sizeof(dmh_block_info_t),
        (u8 *)&dmh, sizeof(dmh_block_info_t));
     
    DMPRINTF("block id=0x%x,type=0x%x,node=0x%x,addr=0x%x,size=0x%x,"
      "crc=0x%x\n", dmh.id, dmh.type, dmh.node_num, dmh.base_addr,
      dmh.size, dmh.crc);

    p_dm_var->dm_info.p_blk_mgr[index].dm_header_addr = start_addr;
    p_dm_var->dm_info.p_blk_mgr[index].id = dmh.id;
    p_dm_var->dm_info.p_blk_mgr[index].type = dmh.type;
    p_dm_var->dm_info.p_blk_mgr[index].node_num = dmh.node_num;
    p_dm_var->dm_info.p_blk_mgr[index].base_addr = dmh.base_addr + start_addr;
    p_dm_var->dm_info.p_blk_mgr[index].size = dmh.size; 

    if(BLOCK_TYPE_IW == dmh.type)//init iw info
    {
      p_dm_var->dm_info.p_blk_mgr[index].p_node_info = 
      mtos_malloc(dmh.node_num * sizeof(node_info_t));
      MT_ASSERT(NULL != p_dm_var->dm_info.p_blk_mgr[index].p_node_info);
      p_dm_var->dm_info.p_blk_mgr[index].p_sec_info = 
        mtos_malloc(((p_dm_var->dm_info.p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
      (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
      (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE))
        * sizeof(setcion_mgr_t));
      MT_ASSERT(NULL != p_dm_var->dm_info.p_blk_mgr[index].p_sec_info);

      if(FALSE == iw_restore_block(p_data, index))
      {
        memset(p_dm_var->dm_info.p_blk_mgr[index].p_node_info, 0xFF, 
          p_dm_var->dm_info.p_blk_mgr[index].node_num * sizeof(node_info_t));
        //block_size must be multiple of 64K
        memset(p_dm_var->dm_info.p_blk_mgr[index].p_sec_info, 0x0, 
          ((p_dm_var->dm_info.p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
          (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
          (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE)) * sizeof(setcion_mgr_t));
        charsto_erase(p_dm_var->p_charsto_dev,
          p_dm_var->dm_info.p_blk_mgr[index].base_addr, 
          ((p_dm_var->dm_info.p_blk_mgr[index].size > FLASH_SECTION_SIZE)?
          (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE - 1):
          (p_dm_var->dm_info.p_blk_mgr[index].size / FLASH_SECTION_SIZE)));
      }
    }
  }
}

/*!
  Init dm: get dm header info and restore IW block info 
  \return None
  */
void dm_init_32(u16 max_blk_num, u32 flash_base_addr)
{
  mem_mgr_alloc_param_t mem_alloc_para = {0};
  dm_base_info_t *p_dm_info = NULL;
  charsto_device_t *p_charsto_dev = NULL;
  dm_var_t *p_dm_priv_var = NULL;
  dm_proc_t *p_dm_proc = NULL;

  //Malloc data manager process handle
  p_dm_proc = mtos_malloc(sizeof(dm_proc_t));
  MT_ASSERT(p_dm_proc != NULL);
  memset(p_dm_proc, 0, sizeof(dm_proc_t));

  //Malloc private data for data manager32
  p_dm_proc->p_priv_data = mtos_malloc(sizeof(dm_var_t));
  MT_ASSERT(p_dm_proc->p_priv_data != NULL);
  memset(p_dm_proc->p_priv_data, 0, sizeof(dm_var_t));

  //Attach process function
  p_dm_proc->active_node = dm_active_node_32;
  p_dm_proc->check_block = dm_check_block_32;
  p_dm_proc->del_node    = dm_del_node_32;
  p_dm_proc->flush_cache = dm_flush_cache_32;
  
  p_dm_proc->get_block_addr  = dm_get_block_addr_32;
  p_dm_proc->get_block_header = dm_get_block_header_32;
  p_dm_proc->get_block_info   = dm_get_block_info_32;
  
  p_dm_proc->get_block_size   = dm_get_block_size_32;
  p_dm_proc->read_bl_block    = dm_read_bl_block_32;
  
  p_dm_proc->reset_iw_block   = dm_reset_iw_block_32;
  p_dm_proc->restore_cache    = dm_restore_cache_32;
  p_dm_proc->set_cache        = dm_set_cache_32;
  p_dm_proc->set_fix_id       = dm_set_fix_id_32;
  
  p_dm_proc->show_flash_map   = dm_show_flash_map_32;
  p_dm_proc->write_node       = dm_write_node_32;
  
  p_dm_proc->get_max_node_len = dm_get_max_node_len_32;
  p_dm_proc->get_max_blk_num  = dm_get_max_blk_num_32;
  p_dm_proc->read_node        = dm_read_32;
  p_dm_proc->set_header       = set_header;

  p_dm_proc->direct_write = NULL;
  p_dm_proc->direct_erase = NULL;
  p_dm_proc->direct_read = NULL;
  p_dm_proc->get_node_id_list = NULL;
  p_dm_proc->get_blk_node_num = NULL;

  //Initialize field members of private data information
  p_dm_priv_var = (dm_var_t *)p_dm_proc->p_priv_data;

  //Initialize data manager base information 
  p_dm_info = &p_dm_priv_var->dm_info;
  memset(p_dm_info, 0, sizeof(dm_base_info_t));
  
  mem_alloc_para.id = MEM_SYS_PARTITION;
  mem_alloc_para.size = max_blk_num * sizeof(block_mgr_t);
  mem_alloc_para.user_id = SYS_MODULE_SYSTEM;

  p_dm_info->max_blk_num = max_blk_num;
  p_dm_info->flash_base_addr = flash_base_addr;
  //p_dm_info->p_blk_mgr = MEM_ALLOC(&mem_alloc_para);
  p_dm_info->p_blk_mgr = mtos_malloc(max_blk_num * sizeof(block_mgr_t));
  
  MT_ASSERT(p_dm_info->p_blk_mgr != NULL);
  memset(p_dm_info->p_blk_mgr, 0, max_blk_num * sizeof(block_mgr_t));

  //Charsto device init
  p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
    SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_charsto_dev != NULL);
  p_dm_priv_var->p_charsto_dev = p_charsto_dev;

  #if 0
  parser_dm_header(p_dm_proc->p_priv_data);
  
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    if((BLOCK_TYPE_IW == p_dm_info->p_blk_mgr[i].type)
      &&(FALSE == iw_restore_block(p_dm_proc->p_priv_data, i)))
    {
      OS_PRINTF("iw restore fail %x\n", p_dm_info->p_blk_mgr[i].id);
      memset(p_dm_info->p_blk_mgr[i].p_node_info, 0xFF, 
        p_dm_info->p_blk_mgr[i].node_num * sizeof(node_info_t));
      p_dm_info->p_blk_mgr[i].sec_index = 0;
      memset(p_dm_info->p_blk_mgr[i].p_sec_info, 0x0, 
      ((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
      (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE - 1):
      (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE)) * sizeof(setcion_mgr_t));
      //block_size must be multiple of 64K
      charsto_erase(p_charsto_dev, p_dm_info->p_blk_mgr[i].base_addr, 
        ((p_dm_info->p_blk_mgr[i].size > FLASH_SECTION_SIZE)?
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE - 1):
        (p_dm_info->p_blk_mgr[i].size / FLASH_SECTION_SIZE)));      
    }
  }
  #endif
  //Register handle
  class_register(DM_CLASS_ID, p_dm_proc);
}

