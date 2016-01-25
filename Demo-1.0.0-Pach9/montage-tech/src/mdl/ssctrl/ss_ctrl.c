/******************************************************************************/
/******************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_define.h"
#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

#ifdef PRINT_ON
//#define SC_DEBUG
#endif

#ifdef PRINT_ON
#define SSC_CHK_NI
#endif

#ifdef SS_DEBUG
#define SSC_DEBUG OS_PRINTF
#else
#define SSC_DEBUG DUMMY_PRINTF
#endif

#ifdef SSC_CHK_NI
#define SSC_CHK_NI_DBG OS_PRINTF
#else
#define SSC_CHK_NI_DBG DUMMY_PRINTF
#endif

/*!
  checksum
  */
#define SS_CHECKSUM_VALUE (0x47A547A5)
#define SC_DEF_ATOM_SIZE (40)

typedef struct tag_sc_priv_data
{
  ss_public_t public_data;  //public data
  u16 public_start; //public data node id from
  u16 public_num;//public data node id number
  
  u8 *p_copy_buf;  //user data
  u16 node_start; //user data node id from
  u16 node_num; //user data node id number

  u32 checksum; //checksum must be SS_CHECKSUM_VALUE
  u16 checksum_ni;  //the checksum node id
  u8 block_id;  //ss block id
  u8 atom_size;
  
  os_sem_t sem;  //semphore
  BOOL miss_data;
}sc_priv_data_t;


void ss_ctrl_public_init(void *p_data, u8 block_id)
{
  u8 atom_size = SC_DEF_ATOM_SIZE;
  u16 i = 0;
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  u16 last_node_id = 0;
  u16 last_node_len = 0;
  
  MT_ASSERT(p_sc_handle != NULL);
  
  p_sc_handle->atom_size = atom_size;
  p_sc_handle->block_id = block_id;

  //init public data
  p_sc_handle->public_start = SS_CTRL_FIRST_NI;
  p_sc_handle->public_num = (sizeof(ss_public_t) + atom_size - 1) / atom_size;

  last_node_id = p_sc_handle->public_num - 1;
  last_node_len = sizeof(ss_public_t) - (last_node_id * atom_size);

  SSC_DEBUG("public data debug : last_node_id %d, last_node_len %d\n",
      last_node_id, last_node_len);
  
  for(i = 0; i < p_sc_handle->public_num; i ++)
  {
    p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);
    if(i == last_node_id)
    {
      dm_read(p_dm_handle, block_id, p_sc_handle->public_start + i, 0, 
      last_node_len, (u8 *)(&p_sc_handle->public_data) + i * atom_size);
      break;
    }
    dm_read(p_dm_handle, block_id, p_sc_handle->public_start + i, 0, 
      atom_size, (u8 *)(&p_sc_handle->public_data) + i * atom_size);
  }
#ifdef SEM_DEBUG
    strcpy(p_sc_handle->sem.m_name, "ss_ctrl");
#endif
    mtos_sem_create(&p_sc_handle->sem, TRUE);
}

BOOL ss_ctrl_init(u8 block_id, u8 *p_buffer, u32 buf_len)
{
  u16 i = 0;
  u32 length = 0;
  u8 atom_size = SC_DEF_ATOM_SIZE;
  //static u8 is_initialized = FALSE;
  sc_priv_data_t *p_sc_handle = NULL;
  void *p_dm_handle = NULL;
  u16 view_blk_node_num = 0;
  u16 read_len = 0;
  BOOL b_complete_data = TRUE;
  
  MT_ASSERT(p_buffer!= NULL);

  p_sc_handle = mtos_malloc(sizeof(sc_priv_data_t));
  MT_ASSERT(p_sc_handle != NULL);

  memset(p_sc_handle, 0, sizeof(sc_priv_data_t));
  SSC_DEBUG("%d", sizeof(sc_priv_data_t));
  //if(is_initialized == TRUE)
  //{
  //  return ss_ctrl_get_checksum();
  //}
  
  //ss ctrl public init
  ss_ctrl_public_init(p_sc_handle, block_id);
  
  atom_size = p_sc_handle->atom_size;
  length = ((buf_len + atom_size - 1) / atom_size * atom_size);

  //init user data
  p_sc_handle->p_copy_buf = (u8 *)mtos_malloc(length);
  MT_ASSERT(p_sc_handle->p_copy_buf != NULL);

  memset(p_sc_handle->p_copy_buf, 0, length);
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  p_sc_handle->node_start = 
                p_sc_handle->public_start + p_sc_handle->public_num;
  p_sc_handle->node_num = length / atom_size;
  
  for(i = 0; i < p_sc_handle->node_num; i ++)
  {
    read_len = dm_read(p_dm_handle, block_id, p_sc_handle->node_start + i, 0, atom_size,
     p_sc_handle->p_copy_buf + i * atom_size);

    if(read_len == 0)
    {
      b_complete_data = FALSE;
      break;
    }
  }
  memcpy(p_buffer, p_sc_handle->p_copy_buf, buf_len);

  //init checksum
  p_sc_handle->checksum_ni = p_sc_handle->node_start + p_sc_handle->node_num;

  view_blk_node_num = dm_get_blk_node_num(p_dm_handle, IW_VIEW_BLOCK_ID);

  if(p_sc_handle->checksum_ni >= view_blk_node_num)
  {
    //Notes: if checksum ni >= view_blk_node_num, the checksum data can't save
    //       to flash. because data_manager thinks this node id is invalid.
    //       you must reconfig iwview node bigger than checksum_ni in the flash.cfg
    SSC_CHK_NI_DBG("%s checksum_ni %d, view_blk_node_num %d\n", 
                  __FUNCTION__, p_sc_handle->checksum_ni, view_blk_node_num);
    MT_ASSERT(0);
  }
  
  dm_read(p_dm_handle, block_id, p_sc_handle->checksum_ni, 0, sizeof(u32), 
    (u8 *)(&p_sc_handle->checksum));

  class_register(SC_CLASS_ID, p_sc_handle);

  if(b_complete_data)
  {
    b_complete_data = ss_ctrl_get_checksum(p_sc_handle);
  }
  p_sc_handle->miss_data = !b_complete_data;
  return b_complete_data;
}

BOOL ss_ctrl_sync(void *p_data, u8 *p_buffer)
{
  u16 i = 0;
  u16 node_id = 0;
  u32 offset = 0; 
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  
  MT_ASSERT(p_sc_handle != NULL);
  
  mtos_sem_take(&p_sc_handle->sem, 0);
  for(i = 0; i < p_sc_handle->node_num; i++)
  {
    offset = i * p_sc_handle->atom_size;
    if((0 != memcmp(p_sc_handle->p_copy_buf + offset, 
                       p_buffer + offset, p_sc_handle->atom_size)) ||
        p_sc_handle->miss_data)
    {
      memcpy(p_sc_handle->p_copy_buf + offset, p_buffer + offset, 
                                                      p_sc_handle->atom_size);
      node_id = p_sc_handle->node_start + i;

      p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
      MT_ASSERT(p_dm_handle != NULL);
      
      dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
                      (u8 *)(p_buffer + offset), p_sc_handle->atom_size);
    }
  }
  p_sc_handle->miss_data = FALSE;
  mtos_sem_give(&p_sc_handle->sem);
  return TRUE;
}


/*!
   Get sysstatus checksum flag. if flag is error, it means some data is iffy 
   possibly
   \return Return TRUE if the checksum is right. or not FALSE.
   */
BOOL ss_ctrl_get_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);
  
  return (p_sc_handle->checksum == SS_CHECKSUM_VALUE);
}

/*!
   Set right checksum to flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_set_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;

  MT_ASSERT(p_sc_handle != NULL);
  p_sc_handle->checksum = SS_CHECKSUM_VALUE;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  dm_write_node(p_dm_handle, p_sc_handle->block_id, 
    &p_sc_handle->checksum_ni, (u8 *)(&p_sc_handle->checksum), 4);
  return TRUE;
}

/*!
   clean the checksum from flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_clr_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  
  MT_ASSERT(p_sc_handle != NULL);

  p_sc_handle->checksum = ~0;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  dm_write_node(p_dm_handle, p_sc_handle->block_id, 
    &p_sc_handle->checksum_ni, (u8 *)(&p_sc_handle->checksum), 4);
  return TRUE;
}

/*!
   get the public struct
 \return the addr of public data
 */
ss_public_t *ss_ctrl_get_public(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);
  
  return &p_sc_handle->public_data;
}

/*!
   save the public data to flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_update_public(void *p_data)
{
  u16 i = 0;
  u16 node_id = 0;
  u16 last_node_id = 0;
  u16 last_node_len = 0;
  void *p_dm_handle = NULL;
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);

  last_node_id = p_sc_handle->public_num - 1;
  last_node_len = sizeof(ss_public_t) - (last_node_id * p_sc_handle->atom_size);
  
  mtos_sem_take(&p_sc_handle->sem, 0);
  for(i = 0; i < p_sc_handle->public_num; i ++)
  {
    node_id = p_sc_handle->public_start + i;
    p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);

    if(i == last_node_id)
    {
      dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
        (u8 *)(&p_sc_handle->public_data) + i * p_sc_handle->atom_size,
        last_node_len);
      break;
    }
    dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
      (u8 *)(&p_sc_handle->public_data) + i * p_sc_handle->atom_size,
        p_sc_handle->atom_size);
  }
  
  mtos_sem_give(&p_sc_handle->sem);
  return TRUE;
}

