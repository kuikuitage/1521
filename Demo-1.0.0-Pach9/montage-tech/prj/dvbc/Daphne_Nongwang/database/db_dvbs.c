/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "stdio.h"
#include "lib_util.h"
#include "lib_unicode.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
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
#include "ss_ctrl.h"

#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "db_dvbs.h"
#include "db_preset.h"
#include "data_manager.h"
#include "data_base.h"
#include "data_base16v2.h"
#include "sys_status.h"
#include "iso_639_2.h"
#include "lib_char.h"
#include <stdlib.h>

#define  MDL_DBG
#ifdef   MDL_DBG
#define  mdl_dbg    OS_PRINTF
#else
#ifndef WIN32
#define  mdl_dbg(x)    do { } while (0)
#else
#define  mdl_dbg
#endif
#endif

#define PG_MAX_LEN (sizeof(pg_data_t) \
                  + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16) \
                  + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL \
                  + sizeof(mosaic_t) \
                  + sizeof(cas_desc_t) * MAX_ECM_DESC_NUM \
                  + 4)
#define TABLE_BLOCK_ID    (IW_TABLE_BLOCK_ID)
#define VIEW_BLOCK_ID     (IW_VIEW_BLOCK_ID)
#define PG_NODE_VALID_CACHE_DATA_LEN (12)

#define DATA_CACHE_UNIT_SIZE (40)
#define VIEW_CACHE_UNIT_SIZE (sizeof(item_type_t))

//static u8 g_sat_table_id = 0;
static u8 g_tp_table_id = 0;
static u8 g_pg_table_id = 0;

static item_type_t g_all_tp_view_buf[DB_DVBS_MAX_TP];
//static item_type_t g_sat_tp_view_buf[DB_DVBS_MAX_TP];
static item_type_t g_pg_view_buf[DB_DVBS_MAX_PRO];

static u8 *g_p_data_buf;
static u16 g_data_buf_len;
static u8 *g_find_view_buf;
static db_filter_proc_t filter_array[DB_DVBS_INVALID_VIEW];
static db_filter_proc_new_t filter_new_array[DB_DVBS_INVALID_VIEW];

static pro_sort_t *p_sort_buf = NULL;
static pro_sort_id_t *p_sort_id_buf = NULL;
static pro_view_id_t g_name_view;
static pro_view_id_t g_free_view;
static pro_view_id_t g_lock_view;
static pro_view_id_t g_default_view;

static u32 default_order = 0;
static dvbs_view_t g_vv_id_name_map[MAX_VIRTUAL_VIEW_NUM];

typedef struct
{
  u32 diseqc_type         : 2;
  u32 diseqc_port          : 4;
  u32 diseqc11_type     : 2;
  u32 diseqc11_port      : 4;
  u32 lnb_power           : 2;
  u32 lnb_type             : 2;
  u32 position              : 8;
  u32 polarity              : 2;
  u32 k22                    : 1;
  u32 name_length      : 5;

  u32 lnb_high            : 16;
  u32 lnb_low             : 16;
  
  u32 longitude           : 16; 
  u32 positioner_type  : 2;
  u32 v12                    : 1;
  u32 toneburst           : 2;
  u32 reserved            : 11;
   /* 
  ---------------------------------------------------------
  above into data cache
   */
  u32 user_band           : 4;
  u32 band_freq           : 12;
  /*! unicable type. 0: unicable A; 1: unicable B */
  u32 unicable_type      : 1;
  u32 band_reserved       : 15;
} sat_data_t;

typedef struct
{
  /*!Each bit of fav_grp_flag represents a single favorite group*/
  u32 fav_grp_flag;

  u32 tv_flag                 : 1;
  u32 lck_flag                : 1;
  u32 skp_flag                : 1;
  /*! audio volume */
  u32 volume                  : 5;
  /*! audio channel option, 0: channel 1 \sa audio_pid1, 1: channel 2
   \sa audio_pid2 */
  u32 audio_channel           : 5;
  /*! scrambled flag 0: not scrambled, 1: scrambled */
  u32 is_scrambled            : 1;
  u32 audio_ch_num            : 5;
  /*! video pid */
  u32 video_pid               : 13;
  
  /*! pcr pid */
  u32 pcr_pid                 : 13;
  /*! audio track option, 0: stereo, 1: left, 2: right */
  u32 audio_track             : 2;
  u32 reserved                : 1;
  u32 ca_system_id            : 16;
  
  u32 sat_id          : 16;
  u32 tp_id           : 16;
  
  /* 
  ---------------------------------------------------------
  above into data cache
   */
  u32 service_type :8;
  u32 mosaic_flag     :8;
  u32 default_order   :16;
  
  u32 orig_net_id     : 16;
  u32 name_length     : 5;
  u32 nvod_reference_svc_cnt        : 11;

  u32 s_id            : 16;
  u32 ts_id           : 16;
  u32 pmt_pid              :16;
  u32 ecm_num         :6;
  u32 reserve            :10;
  u16 logic_ch_num;
} pg_data_t;

//loal function
int service_id_compare( const void *arg1, const void *arg2 );


typedef BOOL (*db_dvbs_sort_func)(u8* prev_node, u8* cur_node, 
  dvbs_sort_node_type_t node_type);

typedef BOOL (*db_dvbs_qsort_func)(pro_sort_t* prev_node, pro_sort_t* cur_node);

static db_dvbs_ret_t translate_return_value(db_ret_t db_value)
{
  if (db_value == DB_FAIL)
  {
    return DB_DVBS_FAILED;
  }
  if (db_value == DB_FULL)
  {
    return DB_DVBS_FULL;
  }
  if (db_value == DB_NOT_FOUND)
  {
    return DB_DVBS_NOT_FOUND;
  }
  return DB_DVBS_OK;
}


static BOOL all_radio_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( (pg_data.service_type == SVC_TYPE_RADIO) && (pg_data.skp_flag ==0) );
}

static BOOL all_radio_ignore_skip_flag_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.service_type == SVC_TYPE_RADIO);
}

static u32 all_radio_ignore_skip_flag_filter1(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.service_type == SVC_TYPE_RADIO);
}

static u32 all_radio_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( pg_data.service_type == SVC_TYPE_RADIO);
}

static BOOL all_tv_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( (pg_data.service_type == SVC_TYPE_TV) && (pg_data.skp_flag ==0) );
}

static BOOL all_tv_ignore_skip_flag_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.service_type == SVC_TYPE_TV);
}

static u32 all_tv_ignore_skip_flag_filter1(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.service_type == SVC_TYPE_TV);
}

static u32 all_tv_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));

  return(pg_data.service_type == SVC_TYPE_TV);
}

static BOOL fav_radio_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return (((pg_data.fav_grp_flag & (1<<context))!= 0)&&(pg_data.service_type == SVC_TYPE_RADIO)&&(pg_data.skp_flag == 0));
}

static u32 fav_radio_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  for(i = 0; i < context; i++)
  {
    if(((pg_data.fav_grp_flag & (1<<i))!= 0)&&(pg_data.service_type == SVC_TYPE_RADIO))
      ret |= 1 << i;
  }
  return ret;
  //return (((pg_data->fav_grp_flag & (1<<context))!= 0)&&( pg_data->tv_flag == 0));
}


static BOOL fav_tv_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return (((pg_data.fav_grp_flag & (1<<context))!= 0)&&(pg_data.service_type == SVC_TYPE_TV)&&(pg_data.skp_flag == 0));
}

static u32 fav_tv_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  for(i = 0; i < context; i++)
  {
    if(((pg_data.fav_grp_flag & (1<<i))!= 0)&&(pg_data.service_type == SVC_TYPE_TV))
      ret |= 1 << i;
  }
  return ret;
  //return (((pg_data->fav_grp_flag & (1<<context))!= 0)&&( pg_data->tv_flag == 1));
}

static BOOL fav_all_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(((pg_data.fav_grp_flag & (1<<context)) != 0) && (pg_data.skp_flag == 0));
}

static u32 fav_all_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  for(i = 0; i < context; i++)
  {
    if((pg_data.fav_grp_flag & (1<<i)) != 0)
      ret |= 1 << i;
  }
  return ret;
  //return((pg_data->fav_grp_flag & (1<<context)) != 0);
}

static BOOL all_pg_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.skp_flag == 0);
}

static BOOL tv_radio_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( ((pg_data.service_type == SVC_TYPE_TV) || (pg_data.service_type == SVC_TYPE_RADIO)) && (pg_data.skp_flag ==0) );
}

static BOOL tv_radio_ignore_skip_flag_filter(u16 bit_mask,u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( (pg_data.service_type == SVC_TYPE_TV) || (pg_data.service_type == SVC_TYPE_RADIO) );
}

static BOOL mosaic_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( ((pg_data.service_type == SVC_TYPE_MOSAIC) && pg_data.mosaic_flag) && (pg_data.skp_flag == 0));
}

static u32 mosaic_new_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( ((pg_data.service_type == SVC_TYPE_MOSAIC) && pg_data.mosaic_flag) && (pg_data.skp_flag == 0));
}


static BOOL nvod_time_shift_event_prog_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( (pg_data.service_type == SVC_TYPE_NVOD_REFRENCE) && (pg_data.skp_flag == 0) );
}

static u32 nvod_time_shift_event_prog_new_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( (pg_data.service_type == SVC_TYPE_NVOD_REFRENCE) && (pg_data.skp_flag == 0) );
}


static BOOL tp_pg_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return((pg_data.tp_id == context) && (pg_data.skp_flag ==0));
}

static BOOL tp_pg_ignore_skip_flag_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.tp_id == context);
}

static u32 tp_pg_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return((pg_data.tp_id == context) && (pg_data.skp_flag ==0));;
}

static u32 tp_pg_ignore_skip_flag_new_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return(pg_data.tp_id == context);
}

static BOOL string_find_filter(u16 bit_mask,u32 context, u8 block_id, u16 node_id)
{
  u32 element_buf[sizeof(dvbs_prog_node_t) /4] = {0};
  pg_data_t* p_pg_node = (pg_data_t*)element_buf;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  u8 *p_pg_name = NULL;
  const u16 * string_to_be_matched = (u16*)context;
  
  db_get_element(p_db_handle, block_id, node_id, (u8 *)element_buf, 0,
                     sizeof(element_buf));
  
  p_pg_name = 
      (u8*)p_pg_node + sizeof(pg_data_t)
        + p_pg_node->audio_ch_num * sizeof(audio_t);
  MT_ASSERT( context != 0);

  //Case is not sensitive
  return(match_string((u16*)p_pg_name, string_to_be_matched, TRUE) == TRUE);
}
/*!
  \param[in] prev_node: previous node to be sorted
  \param[in] cur_node:  current node to be sorted
  \param[in] node type: node type of input ( No meaning in this api)
  return : TRUE >exchange pre_node with current node
           FALSE >continue
 */
static BOOL fta_first_sort(u8* prev_node, u8* cur_node, dvbs_sort_node_type_t node_type )
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
      {
        dvbs_prog_node_t* p_pre_pg_node = (dvbs_prog_node_t*)prev_node;
        dvbs_prog_node_t* p_cur_pg_node = (dvbs_prog_node_t*)cur_node;

        /*!Fix me about is scrambled and found scrambled*/
        ret = (p_pre_pg_node->is_scrambled == FALSE 
              && p_cur_pg_node->is_scrambled == TRUE);
      }
      break;
    case DB_DVBS_TP_NODE:
      {
      dvbs_tp_node_t* p_pre_tp_node = (dvbs_tp_node_t*)prev_node;
      dvbs_tp_node_t* p_cur_tp_node = (dvbs_tp_node_t*)cur_node;

      /*!Fix me about is scrambled and found scrambled*/
      ret = (p_pre_tp_node->is_scramble == FALSE 
            && p_cur_tp_node->is_scramble == TRUE);
      }
      break;
  }

  return ret;
}

static BOOL name_a_z_sort(u8* prev_node, u8* cur_node, dvbs_sort_node_type_t node_type )
{
  BOOL ret =  FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
      {
        dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
        dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
        if(p_cur_pg_node->name[0] == 0
          || p_pre_pg_node->name[0] == 0)
        {
          ret = TRUE;
        }
        else
        {
          // ret = uni_strcmp(p_cur_pg_node->name, p_pre_pg_node->name) > 0 ;
          ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) > 0 ;
        }
      }
      break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0);//No tp name existing
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return ret;
}

static BOOL name_z_a_sort(u8* prev_node, u8* cur_node,
dvbs_sort_node_type_t node_type)
{
  BOOL ret =  FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) < 0;
    }
    break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0);//No tp name existing
    break;
    default:
      MT_ASSERT(0);
    break;
  }
  return ret;

}


static BOOL lock_first_sort(u8* prev_node, u8* cur_node, 
dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t* p_cur_pg_node = (dvbs_prog_node_t*)cur_node;
  dvbs_prog_node_t* p_prev_pg_node = (dvbs_prog_node_t*)prev_node;
  return( p_cur_pg_node->lck_flag == 1 && p_prev_pg_node->lck_flag == 0);
}

static BOOL default_order_sort(u8* prev_node, u8* cur_node, 
dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t* p_cur_pg_node = (dvbs_prog_node_t*)cur_node;
  dvbs_prog_node_t* p_pre_pg_node = (dvbs_prog_node_t*)prev_node;
  return (p_cur_pg_node->default_order > p_pre_pg_node->default_order);
}

u8 db_dvbs_create_view(dvbs_view_t name, u32 context, u8 *buf_ex)
{  
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8   table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
//  u16 view_count = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  switch (name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_MOSAIC:
    case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
      break;
    case DB_DVBS_TP_PG:
    case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
      bit_only = FALSE;
      break;
    case DB_DVBS_ALL_TP:
      table_id   = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if (buf_ex != NULL)
  {
    p_view_buf = (item_type_t*)buf_ex;
  }
  
  db_create_virtual_view(p_db_handle, table_id, filter_array[name], bit_only,
                         (u8*)p_view_buf, context, &vv_id);
  g_vv_id_name_map[vv_id] = name;
//  view_count = db_dvbs_get_count(vv_id);
//  qsort((void *)p_view_buf, view_count, sizeof(u16), service_id_compare);

  return vv_id;
}

u8 db_dvbs_create_view_all(dvbs_view_t name, u16 *p_context, u16 fav_count,u8 *buf_ex)
{  
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8   table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  switch (name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_MOSAIC:
    case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
      break;
    case DB_DVBS_TP_PG:
    case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
      bit_only = FALSE;
      break;
    case DB_DVBS_ALL_TP:
      table_id   = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if (buf_ex != NULL)
  {
    p_view_buf = (item_type_t*)buf_ex;
  }
  
  db_create_virtual_view_all(p_db_handle, table_id, filter_new_array[name], bit_only,
                         (u8*)p_view_buf, p_context,fav_count, &vv_id);
  g_vv_id_name_map[vv_id] = name;
  
  return vv_id;
}

void db_dvbs_destroy_view(u8 view_id)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_destroy_virtual_view(p_db_handle, view_id);
}

BOOL db_dvbs_get_view_del_flag(u8 view_id)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  return db_get_view_del_flag(p_db_handle, view_id);
}

dvbs_view_t db_dvbs_get_view_type(u8 vv_id)
{
  return g_vv_id_name_map[vv_id];
}

db_dvbs_ret_t db_dvbs_init(void)
{
  static BOOL is_initialized = FALSE;
  u8  vv_cnt = 0;
  void *p_db_handle = NULL;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  g_data_buf_len = dm_get_max_node_len(p_dm_handle);

  if(is_initialized == FALSE)
  {
    g_p_data_buf = mtos_malloc(g_data_buf_len);
    MT_ASSERT(g_p_data_buf != NULL);

    //Initialize database 16v2
    db_init_database_16v2();

    //Get database handle
    p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
    MT_ASSERT(p_db_handle != NULL);
    
    g_tp_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID, 
    DB_DVBS_MAX_TP);
    
    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_tp_table_id);
    g_pg_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID, 
    DB_DVBS_MAX_PRO);
    
    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_pg_table_id);

    //Initaialize find view buffer
    g_find_view_buf = mtos_malloc(MAX_VIEW_BUFFER_SIZE);
    MT_ASSERT(g_find_view_buf != NULL);

    //Initialize virtual view filter
    filter_array[DB_DVBS_ALL_RADIO] = all_radio_filter;
    filter_array[DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG] =all_radio_ignore_skip_flag_filter;
    filter_array[DB_DVBS_ALL_TV] = all_tv_filter;
    filter_array[DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG] = all_tv_ignore_skip_flag_filter;
    filter_array[DB_DVBS_FAV_RADIO] = fav_radio_filter;
    filter_array[DB_DVBS_FAV_TV] = fav_tv_filter;
    filter_array[DB_DVBS_FAV_ALL] = fav_all_filter;
    filter_array[DB_DVBS_TP_PG] = tp_pg_filter;
    filter_array[DB_DVBS_TP_PG_IGNORE_SKIP_FLAG] = tp_pg_ignore_skip_flag_filter;
    filter_array[DB_DVBS_ALL_PG] = all_pg_filter;
    filter_array[DB_DVBS_ALL_TP] = NULL;
    filter_array[DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG] = NULL;
    filter_array[DB_DVBS_TV_RADIO] = tv_radio_filter;
    filter_array[DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG] = tv_radio_ignore_skip_flag_filter;
    filter_array[DB_DVBS_MOSAIC] = mosaic_filter;
    filter_array[DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG] = nvod_time_shift_event_prog_filter;

    filter_new_array[DB_DVBS_ALL_RADIO] = all_radio_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG] = all_radio_ignore_skip_flag_filter1;
    filter_new_array[DB_DVBS_ALL_TV] = all_tv_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG] = all_tv_ignore_skip_flag_filter1;
    filter_new_array[DB_DVBS_FAV_RADIO] = fav_radio_new_filter;
    filter_new_array[DB_DVBS_FAV_TV] = fav_tv_new_filter;
    filter_new_array[DB_DVBS_FAV_ALL] = fav_all_new_filter;
    filter_new_array[DB_DVBS_TP_PG] = tp_pg_new_filter;
    filter_new_array[DB_DVBS_TP_PG_IGNORE_SKIP_FLAG] = tp_pg_ignore_skip_flag_new_filter;
    filter_new_array[DB_DVBS_ALL_PG] = NULL;
    filter_new_array[DB_DVBS_ALL_TP] = NULL;
    filter_new_array[DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG] = NULL;
    filter_new_array[DB_DVBS_TV_RADIO] = NULL;
    filter_new_array[DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG] = NULL;
    filter_new_array[DB_DVBS_MOSAIC] = mosaic_new_filter;
    filter_new_array[DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG] = nvod_time_shift_event_prog_new_filter;

    //initialize virtual view id and name map 
    for( vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
    {
      g_vv_id_name_map[vv_cnt] = DB_DVBS_INVALID_VIEW;
    }

    is_initialized = TRUE;
  }
  return DB_DVBS_OK;
}


void db_dvbs_del_tp(u8 view_id, u16 pos)
{
  u8 pg_view_id = 0;
  u16 pg_total = 0;
  u16 i = 0;
  u16 tp_id = 0XFFFF;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  tp_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  pg_view_id = db_dvbs_create_view(DB_DVBS_TP_PG_IGNORE_SKIP_FLAG, tp_id, NULL);
  pg_total = db_dvbs_get_count(pg_view_id);

  for (i = 0; i < pg_total; i++)
  {
    db_dvbs_change_mark_status(pg_view_id, i, DB_DVBS_DEL_FLAG, param);
    //db_dvbs_view_mark(pg_view_id, i, DB_DVBS_MARK_DEL, TRUE);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, pg_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, pg_view_id);
  
  db_del_view_item(p_db_handle, view_id, pos);
}



db_dvbs_ret_t db_dvbs_edit_program(dvbs_prog_node_t *p_node)
{
  u16 i, len;
  db_ret_t ret;
  u32 element_buf[PG_MAX_LEN/4] = {0};
  pg_data_t *p_pg = (pg_data_t*)element_buf;
  audio_t* p_audio = (audio_t*)(p_pg + 1);
  u16 ext_len = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  
  if(p_node->video_pid == 0 && p_node->audio_ch_num == 0)
  {
    OS_PRINTF("video pid[0]\n");
    return DB_DVBS_FAILED;
  }

  OS_PRINTF("edit id %d, name %s\n", p_node->id, p_node->name);
  p_pg->tv_flag   = p_node->tv_flag;
  p_pg->lck_flag  = p_node->lck_flag;
  p_pg->skp_flag  = p_node->skp_flag;

  p_pg->default_order = p_node->default_order;
  p_pg->fav_grp_flag = p_node->fav_grp_flag;
  p_pg->audio_channel = p_node->audio_channel;

  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;
    
  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->service_type = p_node->service_type;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->video_pid = p_node->video_pid;
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->ecm_num = p_node->ecm_num;
  p_pg->mosaic_flag = p_node->mosaic_flag;
  p_pg->logic_ch_num = p_node->logic_ch_num;
  p_pg->nvod_reference_svc_cnt = p_node->nvod_reference_svc_cnt;
  
  len = (u16)uni_strlen(p_node->name);
  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH 
    ? DB_DVBS_MAX_NAME_LENGTH : len;

  /*!Modify audio type*/
   for (i = 0; i < p_pg->audio_ch_num; i++, p_audio++)
  {
    memcpy(p_audio, p_node->audio + i, sizeof(audio_t));
  }

  if (p_pg->name_length > 0)
  {
    uni_strncpy((u16*)p_audio, p_node->name, p_pg->name_length);
  }

  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
              + p_pg->audio_ch_num * sizeof(audio_t));

  if (p_pg->mosaic_flag)
  {
    //ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX - p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
   // memcpy((u8 *)p_pg + len, &p_node->mosaic, ext_len);
    //len += ext_len;
  }

  if (p_pg->ecm_num)
  {
    ext_len = (u16)p_pg->ecm_num * sizeof(cas_desc_t);
    memcpy((u8 *)p_pg + len, &p_node->cas_ecm, ext_len);
    len += ext_len;
  }

  ret = db_edit_element(p_db_handle, g_pg_table_id, (u16)p_node->id, 
      (u8 *)element_buf, len);
  return translate_return_value(ret);
}

void db_dvbs_save_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  
  db_save_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}

void db_dvbs_undo_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_undo_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}

db_dvbs_ret_t db_dvbs_edit_tp(dvbs_tp_node_t *p_node)
{
  db_ret_t ret = DB_FAIL;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_edit_element(p_db_handle, g_tp_table_id, (u16)p_node->id, (u8*)p_node,
    sizeof(dvbs_tp_node_t));
                    
  return translate_return_value(ret);
}

void db_dvbs_save_tp_edit(dvbs_tp_node_t *p_tp_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  db_save_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}

void db_dvbs_undo_tp_edit(dvbs_tp_node_t *p_tp_node)
{ 
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  db_undo_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}

u16 db_dvbs_get_count(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  return db_get_virtual_view_count(p_db_handle, view_id);
}


void db_previosly_program_for_ae(void)
{

  u8 vv_id_tp = 0 ,vv_id_pro = 0;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  u32 cur_tp_id = 0;
  u32 i = 0;
  u32 j = 0;
  u32 frqtable_cnk = 0;
  u8 pgname[32] = {0};
  u32 FreqTable[] = {107000, 115000,123000, 
      131000, 139000, 147000, 155000, 163000, 171000, 179000,187000, 
      195000, 203000, 211000, 219000, 227000, 235000, 243000,251000,
      259000, 267000, 275000, 283000, 291000, 299000, 307000,315000,
      323000, 331000, 339000, 347000, 355000, 363000, 371000,379000, 
      387000, 395000, 403000, 411000, 419000, 427000, 435000,443000, 
      451000, 459000, 467000,474000, 482000, 490000, 498000, 506000,
      514000, 522000, 530000, 538000, 546000, 554000, 562000, 570000,
      578000, 586000, 594000, 602000, 610000, 618000, 626000, 634000,
      642000, 650000, 658000, 666000, 674000, 682000, 690000, 698000,
      706000, 714000, 722000, 730000, 738000, 746000, 754000, 762000,
      770000, 778000, 786000, 794000, 802000, 810000, 818000, 826000,
      834000, 842000, 850000, 858000, 866000, 874000, 882000, 890000, };

  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  
  frqtable_cnk = sizeof(FreqTable) /sizeof(u32);
  for(j = 0;j < 2;j ++)
  {
    if(j == 0)
      {
         tp.nim_modulate = NIM_MODULA_QAM64;
         
      }
    else
      {
        tp.nim_modulate = NIM_MODULA_QAM256;
       
      }
    for(i = 0;i < frqtable_cnk;i ++)
      {
          tp.freq = FreqTable[i];
          tp.sym = 6900;
          if(tp.nim_modulate == NIM_MODULA_QAM64)
            {
              sprintf((char *)pgname,"%ld_%s",tp.freq/1000,(u8 *)"QAM64");
            }
          else
            {
               sprintf((char *)pgname,"%ld_%s",tp.freq/1000,(u8 *)"QAM256");
            }
          db_dvbs_add_tp(vv_id_tp,&tp) ;
          cur_tp_id = tp.id;
          pg.s_id = 1;
          pg.ts_id = 2241;
          pg.orig_net_id = 1999;
          pg.pcr_pid = 256;
          pg.video_pid = 256;
          pg.is_scrambled = 0;
          pg.ca_system_id = 0;
          pg.pmt_pid = 32;
          pg.audio_ch_num = 1;
          pg.audio[0].p_id = 272;
          pg.audio[0].type = 2; 
          pg.tp_id = cur_tp_id;
          pg.volume = 16;
          pg.service_type = SVC_TYPE_TV;
          pg.audio_track = 1;
          pg.tv_flag = (pg.video_pid != 0) ? 1 : 0;
          #ifdef LCN_SWITCH
          pg.logic_ch_num++;
          #endif
          pg.skp_flag = 0;
          pg.mosaic_flag = 0;
          dvb_to_unicode(pgname, sizeof(pgname),pg.name, DB_DVBS_MAX_NAME_LENGTH);
          db_dvbs_add_program(vv_id_pro, &pg) ;   
          }
    }
  
  db_dvbs_save(vv_id_tp);
  db_dvbs_save(vv_id_pro);

}
db_dvbs_ret_t db_dvbs_restore_to_factory(u8 blk_id)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  void *p_sc_handle  = class_get_handle_by_id(SC_CLASS_ID);
  
  //MT_ASSERT(p_db_handle != NULL);
  //MT_ASSERT(p_sc_handle != NULL);
  if((p_db_handle == NULL) || (p_sc_handle == NULL))
    {
      return DB_DVBS_FAILED;   /***make sure ota can runing****/
    }
  
  mtos_task_lock();
  //ss_set_status(SS_CHECKOUT, 0xFF); //write it to invalid
  ss_ctrl_clr_checksum(p_sc_handle);
  OS_PRINTF("load default start...\n");
  db_clean(p_db_handle);

  // Todo: preset data to db data staff here ...
//  db_dvbs_load_preset(blk_id, PRESET_PG_TYPE_ALL, PRESET_PG_AREA_ALL);

  //ss_set_status(SS_CHECKOUT, SS_CHECKOUT_VALUE); //write it back
  ss_ctrl_set_checksum(p_sc_handle);
  OS_PRINTF("load default finish.\n");
  mtos_task_unlock();
  return DB_DVBS_OK;
}

db_dvbs_ret_t db_dvbs_get_pg_by_id_sort(u16 id, dvbs_prog_node_t *p_pg)
{
  u16 i = 0;
  audio_t *p_audio = NULL;
  u16 len = 0;
  u16 ext_len = 0;
  u32 element_buf[PG_MAX_LEN/4] = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  
  
  if (db_get_element(p_db_handle, g_pg_table_id, id, (u8 *)element_buf, 0,
                     sizeof(pg_data_t) 
                     + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16)
                     + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL)
                     + sizeof(mosaic_t) + sizeof(cas_desc_t) * MAX_ECM_DESC_NUM)
  {
    pg_data_t *p_node = (pg_data_t*)element_buf;
    p_audio = (audio_t*)(p_node + 1);

    memset(p_pg, 0x00, sizeof(dvbs_prog_node_t));
    p_pg->id = id;
    p_pg->audio_channel = p_node->audio_channel;
    p_pg->audio_track = p_node->audio_track;
    p_pg->is_scrambled = p_node->is_scrambled;
    p_pg->ca_system_id = p_node->ca_system_id;
    p_pg->service_type = p_node->service_type;
    p_pg->pcr_pid = p_node->pcr_pid;
    p_pg->s_id = p_node->s_id;
    p_pg->tp_id = p_node->tp_id;
    p_pg->video_pid = p_node->video_pid;
    p_pg->volume = p_node->volume;
    p_pg->audio_ch_num = p_node->audio_ch_num <
                         DB_DVBS_MAX_AUDIO_CHANNEL ? p_node->audio_ch_num :
                         DB_DVBS_MAX_AUDIO_CHANNEL;
    p_pg->ts_id = p_node->ts_id;
    p_pg->orig_net_id = p_node->orig_net_id;
    p_pg->pmt_pid = p_node->pmt_pid;

    p_pg->lck_flag = p_node->lck_flag;
    p_pg->skp_flag = p_node->skp_flag;
    p_pg->tv_flag  = p_node->tv_flag;
    p_pg->logic_ch_num = p_node->logic_ch_num;
    
    p_pg->fav_grp_flag  = p_node->fav_grp_flag;
    p_pg->default_order = p_node->default_order;
    p_pg->ecm_num = p_node->ecm_num;
    p_pg->mosaic_flag = p_node->mosaic_flag;
    p_pg->nvod_reference_svc_cnt = p_node->nvod_reference_svc_cnt;
    
    for (i = 0; i < p_pg->audio_ch_num; i++)
    {
      memcpy((p_pg->audio+i), p_audio, sizeof(audio_t));
      p_audio++;
    }

    if (p_node->name_length > 0)
    {
      uni_strncpy(p_pg->name, (u16*)p_audio, DB_DVBS_MAX_NAME_LENGTH);
    }
    else
    {
      //strcpy((char*)p_pg->name, "noname");
      str_nasc2uni((u8 *)"noname", p_pg->name, DB_DVBS_MAX_NAME_LENGTH);
    }
    
    len = (u16)(sizeof(pg_data_t) + p_pg->audio_ch_num * sizeof(audio_t)
        + (p_node->name_length + 1) * sizeof(u16));
    
    if (p_pg->mosaic_flag)
    {
      //ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX -p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
      //memcpy(&p_pg->mosaic, (u8 *)p_node + len, ext_len);
      //len += ext_len;
    }

    if (p_pg->ecm_num)
    {
      ext_len = (u16)p_pg->ecm_num * sizeof(cas_desc_t);
      memcpy(&p_pg->cas_ecm, (u8 *)p_node + len, ext_len);
      len += ext_len;
    }

    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


db_dvbs_ret_t db_dvbs_get_pg_by_id(u16 id, dvbs_prog_node_t *p_pg)
{
  u16 i = 0;
  audio_t *p_audio = NULL;
  u16 len = 0;
  u16 ext_len = 0;
  u32 read_len = 0;
  u32 element_buf[PG_MAX_LEN/4] = {0};
  
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  

  read_len = sizeof(pg_data_t) + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16)
    + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL + sizeof(mosaic_t)
    + sizeof(cas_desc_t) * MAX_ECM_DESC_NUM;
  if (db_get_element(p_db_handle, g_pg_table_id, id, (u8 *)element_buf, 0, (u16)read_len))
  {
    pg_data_t *p_node = (pg_data_t*)element_buf;
    p_audio = (audio_t*)(p_node + 1);

    memset(p_pg, 0x00, sizeof(dvbs_prog_node_t));
    p_pg->id = id;
    p_pg->audio_channel = p_node->audio_channel;
    p_pg->audio_track = p_node->audio_track;
    p_pg->is_scrambled = p_node->is_scrambled;
    p_pg->ca_system_id = p_node->ca_system_id;
    p_pg->service_type = p_node->service_type;
    p_pg->pcr_pid = p_node->pcr_pid;
    p_pg->s_id = p_node->s_id;
    p_pg->tp_id = p_node->tp_id;
    p_pg->video_pid = p_node->video_pid;
    p_pg->volume = p_node->volume;
    p_pg->audio_ch_num = p_node->audio_ch_num <
                         DB_DVBS_MAX_AUDIO_CHANNEL ? p_node->audio_ch_num :
                         DB_DVBS_MAX_AUDIO_CHANNEL;
    p_pg->ts_id = p_node->ts_id;
    p_pg->orig_net_id = p_node->orig_net_id;
    p_pg->pmt_pid = p_node->pmt_pid;

    p_pg->lck_flag = p_node->lck_flag;
    p_pg->skp_flag = p_node->skp_flag;
    p_pg->tv_flag  = p_node->tv_flag;

    p_pg->fav_grp_flag  = p_node->fav_grp_flag;
    p_pg->default_order = p_node->default_order;

    p_pg->mosaic_flag = p_node->mosaic_flag;
    p_pg->nvod_reference_svc_cnt = p_node->nvod_reference_svc_cnt;
    p_pg->ecm_num = p_node->ecm_num;
    p_pg->logic_ch_num = p_node->logic_ch_num;
    
    for (i = 0; i < p_pg->audio_ch_num; i++)
    {
      memcpy((p_pg->audio+i), p_audio, sizeof(audio_t));
      p_audio++;
    }
#if 0
    if (p_node->name_length > 0)
    {
      memset(p_pg->name, 0, DB_DVBS_MAX_NAME_LENGTH);
      strncpy((char*)p_pg->name, (char*)p_audio, p_node->name_length);
      if(p_node->name_length >= DB_DVBS_MAX_NAME_LENGTH)
      {
        p_pg->name[p_node->name_length - 1] = 0;
      }
      else
      {
        p_pg->name[p_node->name_length] = 0;
      }
    }
    else
    {
      strcpy((char*)p_pg->name, "noname");
    }
#else
    if (p_node->name_length > 0)
    {
      uni_strncpy(p_pg->name, (u16*)p_audio, DB_DVBS_MAX_NAME_LENGTH);
    }
    else
    {
      //strcpy((char*)p_pg->name, "noname");
      str_nasc2uni((u8 *)"noname", p_pg->name, DB_DVBS_MAX_NAME_LENGTH);
    }
    
    len = (u16)(sizeof(pg_data_t) + p_pg->audio_ch_num * sizeof(audio_t)
        + (p_node->name_length + 1) * sizeof(u16));
#endif
    if (p_pg->mosaic_flag)
    {
      //ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX -p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
     // memcpy(&p_pg->mosaic, (u8 *)p_node + len, ext_len);
      //len += ext_len;
    }

    if (p_pg->ecm_num)
    {
      ext_len = (u16)p_pg->ecm_num * sizeof(cas_desc_t);
      memcpy(&p_pg->cas_ecm, (u8 *)p_node + len, ext_len);
      len += ext_len;
    }
    
    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


db_dvbs_ret_t db_dvbs_get_tp_by_id(u16 id, dvbs_tp_node_t *p_tp)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  if (db_get_element(p_db_handle, g_tp_table_id, id, (u8*)p_tp,  0, 
        sizeof(dvbs_tp_node_t)))
  {
    p_tp->id = id;
    return DB_DVBS_OK;
  } 
  return DB_DVBS_FAILED;
}




u16 db_dvbs_get_view_pos_by_id(u8 view_id, u16 id)
{
  u16 i = 0;
  u16 ret_id = 0;
  u16 count  = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  count = db_get_virtual_view_count(p_db_handle, view_id);

  for (i = 0; i < count; i++)
  {
    ret_id = db_get_element_id_by_pos(p_db_handle, view_id, i);
    if (ret_id == id)
    {
      return i;
    }
  }
  return INVALIDID;
}


u16 db_dvbs_get_id_by_view_pos(u8 view_id, u16 pos)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  return db_get_element_id_by_pos(p_db_handle, view_id, pos);
}


db_dvbs_ret_t db_dvbs_move_item_in_view(u8 view_id, u16 pos_src, u16 pos_des)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_move_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}


db_dvbs_ret_t db_dvbs_exchange_item_in_view(u8 view_id, u16 pos_src,
                                            u16 pos_des)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_exchange_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}

db_dvbs_ret_t db_dvbs_dump_item_in_view(u8 view_id, void *p_mem, u16 len)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_dump_view_item(p_db_handle, view_id, p_mem, len);
  return DB_DVBS_OK;
}


void db_dvbs_save(u8 view_id)
{
  dvbs_view_t view_name = g_vv_id_name_map[view_id];
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  
  switch (view_name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_MOSAIC:
    case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
      break;
    case DB_DVBS_ALL_TP:
    case DB_DVBS_TP_PG:
    case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
      {
        u16 tp_cnt = 0;
        u16 total_tp_cnt = db_dvbs_get_count(view_id);
        for( tp_cnt = 0; tp_cnt < total_tp_cnt; tp_cnt++)
        {
          if(db_dvbs_get_mark_status(view_id, tp_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
          {
            db_dvbs_del_tp(view_id, tp_cnt);
          }
        }
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, view_id);

}
 
void db_dvbs_undo(u8 view_id)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  db_undo_view(p_db_handle, view_id);
  db_undo_element(p_db_handle, view_id);
}


BOOL is_tp_full(void)
{
  u8 view_id = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  if (db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_TP)
  {
    return TRUE;
  }
  return FALSE;
}


BOOL is_pg_full(void)
{
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  u8 view_id = 0;
  
  MT_ASSERT(p_db_handle != NULL);
  //view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, 0, NULL);
  if (db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_PRO)
  {
    return TRUE;
  }
  return FALSE;
}

db_dvbs_ret_t db_dvbs_add_program(u8 view_id, dvbs_prog_node_t *p_node)
{
  u16 pos = 0;
  u16 loopi = 0;
  u16 len = 0;
  u8 *p_pg_name = NULL;
  db_ret_t ret = DB_SUC;
  u32 element_buf[PG_MAX_LEN/4] = {0};
  pg_data_t *p_pg = (pg_data_t*)element_buf;
  audio_t* p_audio = (audio_t*)(p_pg + 1);
  u16 ext_len = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  /*!
    Pmt parse error
    */
  //if (p_node->video_pid == 0 && p_node->audio_ch_num == 0)
  //{
  //  return DB_DVBS_FAILED;
  //}
  default_order ++;
  
  p_pg->audio_channel = p_node->audio_channel;
  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;
  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->service_type = p_node->service_type;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->video_pid = p_node->video_pid;
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->default_order = default_order;
  len = (u16)uni_strlen(p_node->name);
  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH
    ? DB_DVBS_MAX_NAME_LENGTH : len;

  p_pg->tv_flag = p_node->tv_flag;
  p_pg->lck_flag = p_node->lck_flag;
  p_pg->skp_flag = p_node->skp_flag;
  p_pg->mosaic_flag = p_node->mosaic_flag;
  p_pg->nvod_reference_svc_cnt = p_node->nvod_reference_svc_cnt;
  p_pg->ecm_num = p_node->ecm_num;
   p_pg->logic_ch_num = p_node->logic_ch_num;
   
  if (p_pg->ecm_num > MAX_ECM_DESC_NUM)
  {
    p_pg->ecm_num = MAX_ECM_DESC_NUM;
  }
  
  if(p_pg->audio_ch_num > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    p_pg->audio_ch_num = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  for (loopi = 0; loopi < p_pg->audio_ch_num; loopi++)
  {
    memcpy(p_audio, (p_node->audio + loopi), sizeof(audio_t));
    //*p_audio++ = p_node->audio_pid[i];
    p_audio++;
  }

  if (p_pg->name_length > 0)
  {
    p_pg_name = (u8*)p_pg + sizeof(pg_data_t) + 
      p_pg->audio_ch_num * sizeof(audio_t);
    uni_strncpy((u16*)p_pg_name, p_node->name, p_pg->name_length);
  }
  else
  {
  
    u16 p_no_name[LEN_OF_NO_NAME] = {
                                      'n', 'o', ' ', 'n', 'a', 'm', 'e',0
                                    };
    
    //Add the unicode of noname
    p_pg_name = (u8*)p_pg + sizeof(pg_data_t)
                              + p_pg->audio_ch_num * sizeof(audio_t);
    p_pg->name_length =  LEN_OF_NO_NAME;    
    uni_strncpy((u16*)p_pg_name, p_no_name, LEN_OF_NO_NAME);
    uni_strncpy(p_node->name, p_no_name, LEN_OF_NO_NAME);
  }

  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
    + p_pg->audio_ch_num * sizeof(audio_t));
  if (p_pg->mosaic_flag)
  {
    //ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX - p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
   // memcpy((u8 *)p_pg + len, &p_node->mosaic, ext_len);
   // len += ext_len;
  }

  if (p_pg->ecm_num)
  {
    ext_len = (u16)p_pg->ecm_num * sizeof(cas_desc_t);
    memcpy((u8 *)p_pg + len, &p_node->cas_ecm, ext_len);
    len += ext_len;
  }

  ret = db_add_view_item(p_db_handle, view_id, (u8*)p_pg, len, &pos);
  if (ret != DB_SUC)
  {
    return translate_return_value(ret);
  }
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  if (p_node->service_type == SVC_TYPE_TV)
  {
    db_dvbs_change_mark_status(view_id, pos, DB_DVBS_MARK_TV, 
      DB_DVBS_PARAM_ACTIVE_FLAG);
  }
  return translate_return_value(ret);
}


db_dvbs_ret_t db_dvbs_add_tp(u8 view_id, dvbs_tp_node_t *p_node)
{
  db_ret_t ret = {DB_FAIL};
  u16 pos = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  ret = db_add_view_item(p_db_handle, view_id, (u8*)p_node, 
      sizeof(dvbs_tp_node_t), &pos);
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  return translate_return_value(ret);
}


BOOL db_dvbs_get_special_pg(u16 ts_id, u16 s_id, dvbs_prog_node_t*p_pg)
{
  BOOL ret = FALSE;
  u16 count = 0;
  u16 pg_id = 0;
  u16 i = 0;
  u8 view_id = 0;
  item_type_t *p_array = NULL; 
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  
  MT_ASSERT(p_db_handle != NULL);
  MT_ASSERT(p_pg != NULL);

  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_array != NULL);
  
  //view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, (u8*)p_array);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, 0, (u8*)p_array);
  count = db_dvbs_get_count(view_id);
  for (i = 0; i < count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, p_pg);
    if ((p_pg->ts_id == ts_id) && (p_pg->s_id == s_id))
    {
      ret = TRUE;
      break;
    }
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  return ret;
}


void db_dvbs_change_mark_status(u8 view_id, u16 item_pos, 
   dvbs_element_mark_digit	 dig, u16 param)
{
  BOOL  is_set = ((param&0x8000) != 0)? TRUE:FALSE;
  // Operation can be completed based on view id
  BOOL  view_only = FALSE;     
  BOOL  is_del_operation = FALSE;
  // Parameter for view operation
  u8  	mask_shift  = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
    case DB_DVBS_MARK_LCK:
      break;
    case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param &0x7FFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      is_del_operation = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;
  }

  if(view_only == TRUE)
  {
    if(is_del_operation == TRUE)
    {
      if( is_set == TRUE)
      {
        db_del_view_item(p_db_handle, view_id, item_pos);
      }
      else
      {
        db_active_view_item(p_db_handle, view_id, item_pos);
      }
    }
    else
    {
      db_set_mask_view_item(p_db_handle, view_id, item_pos, mask_shift, 
      is_set);
    }
    //db_update_view(view_id);
  } //View operation
  else
  {
  //PG only
  pg_data_t *p_pg = NULL;
  u16 element_id 	= db_get_element_id_by_pos(p_db_handle, view_id, item_pos);
  u16   data_len  = sizeof(pg_data_t);
  u16   cache_data_len = 0;

  memset(g_p_data_buf, 0, g_data_buf_len);
  db_get_element(p_db_handle, g_pg_table_id, element_id, g_p_data_buf, 0, 
  (u16)data_len);
  p_pg = (pg_data_t*)g_p_data_buf;

  if( p_pg->audio_ch_num > 0)
  {
    //Add audio channel
    data_len += (u16)p_pg->audio_ch_num * sizeof(audio_t);
  }

  //Add name length
  MT_ASSERT(p_pg->name_length <= DB_DVBS_MAX_NAME_LENGTH);
  data_len += (u16)(p_pg->name_length + 1)* sizeof(u16);

  //memset(data_buf, 0, sizeof(data_buf));
  db_get_element(p_db_handle, g_pg_table_id, element_id, g_p_data_buf, 
  0, data_len);
  p_pg = (pg_data_t*)g_p_data_buf;

  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      p_pg->tv_flag = is_set;
      //cache_data_len = data_len;
      cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
      break;
    case DB_DVBS_MARK_LCK:
      p_pg->lck_flag = is_set;
      cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
      break;
    case DB_DVBS_MARK_SKP:
      p_pg->skp_flag = is_set;
      cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
      break;
    case DB_DVBS_FAV_GRP:
      if(is_set == TRUE)
      {
        p_pg->fav_grp_flag |= 1 << mask_shift;
      }
      else
      {
        p_pg->fav_grp_flag &= ~(1 << mask_shift);
      }
      cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  db_edit_element(p_db_handle, g_pg_table_id, element_id, g_p_data_buf, 
  cache_data_len);
  }

}


BOOL db_dvbs_get_mark_status(u8 view_id, u16 item_pos, 
dvbs_element_mark_digit dig, u16 param)
{
  // Operation can be completed based on view id
  BOOL  view_only = FALSE;     
  BOOL  is_del_operation = FALSE;
  u8 mask_shift = 0;  // Parameter for view operation
  BOOL ret = FALSE;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  //Differenciate view only and not
  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
      case DB_DVBS_MARK_LCK:
    break;
      case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param &0x7FFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      is_del_operation = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;
  }

  if(view_only == TRUE)
  {
    //View only
    if(dig == DB_DVBS_DEL_FLAG)
    {
      ret = !(db_get_mask_view_item(p_db_handle, view_id, item_pos,
                  mask_shift));
    }
    else
    {
      ret = db_get_mask_view_item(p_db_handle, view_id, item_pos, mask_shift);
    }
  }
  else
  {
    //PG only
    pg_data_t *p_pg = NULL;
    u16 element_id 	= db_get_element_id_by_pos(p_db_handle, view_id, item_pos);
    u16   data_len  = sizeof(pg_data_t);

    memset(g_p_data_buf, 0, g_data_buf_len);
    db_get_element(p_db_handle, g_pg_table_id, element_id, g_p_data_buf, 
      0, data_len);
    p_pg = (pg_data_t*)g_p_data_buf;

    if( p_pg->audio_ch_num > 0)
    {
      //Add audio channel
      data_len += (u16)p_pg->audio_ch_num * sizeof(audio_t);
    }

    //Add name length
    MT_ASSERT(p_pg->name_length <= DB_DVBS_MAX_NAME_LENGTH);
    data_len += (u16)(p_pg->name_length + 1)* sizeof(u16);

    //memset(data_buf, 0, sizeof(data_buf));
    db_get_element(p_db_handle, g_pg_table_id, element_id, g_p_data_buf, 0,
      data_len);
    p_pg = (pg_data_t*)g_p_data_buf;

    switch(dig)
    {
      case DB_DVBS_MARK_TV:
        ret = p_pg->tv_flag;
        break;
      case DB_DVBS_MARK_LCK:
        ret = p_pg->lck_flag;
        break;
      case DB_DVBS_MARK_SKP:
        ret = p_pg->skp_flag;
        break;
      case DB_DVBS_FAV_GRP:
        ret = (p_pg->fav_grp_flag&(1 << mask_shift))? TRUE:FALSE;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }

  return ret;
}

/*!
  \param[in]parent_view_id: id of the view to be searched for the input 
      string
  \param[in]p_string_in: string to be searched for in given view
  \param[in/out]p_sub_view_id: the sub-view id 
  return    TRUE:  input string existing
            FALSE: input string not found
 */
BOOL db_dvbs_find(u8 parent_view_id, const u16 * p_string_in, 
u8* p_sub_view_id)
{
  u16  view_item_cnt = 0;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

 // str_uni2asc(asc_str, (const u16 *) p_string_in);
  memset(g_find_view_buf, 0, MAX_VIEW_BUFFER_SIZE);	
  view_item_cnt = db_create_sub_virtual_view(p_db_handle, string_find_filter, 
    FALSE, g_find_view_buf, (u32)p_string_in, p_sub_view_id, parent_view_id);

  if(view_item_cnt == 0)
  {
    //Invalid sub-view is found
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

#if 0
/*!
  \param[in]parent_view_id: view to be sorted
  \param[in]sort_type:			sort type to be selected
  \param[in]p_sub_view_id:	the sub-view created based on sort result
  \param[in]view:						view to be sorted
 */
void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  db_dvbs_sort_func sort_func = NULL;

  s16 view_cnt  = 0;

  s16 inner_cnt = 0;
  s16 outer_cnt = 0;

  u16 prv_element_id = 0;

  dvbs_prog_node_t prv_prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;

  u8 *p_sort_buf = NULL;
  u32 pos_off = 0;
  pro_sort_t *p_pro_item = NULL;
  pro_sort_t *p_pro_item_tmp = NULL;
  pro_sort_t *p_pro_item_prv = NULL;
  pro_sort_t *p_pro_item_cur = NULL;
  pro_sort_t *p_pro_item_prv_tmp = NULL;
  pro_sort_id_t id_array[52];
  s16 i = 0 , j = 0;
  u16 m = 0 , n = 0 , k = 0;
  u16 count = 0;
  u16 temp = 0;
  u8 *p_mem = NULL;
  u8 *p_mem_tmp = NULL;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);
  memset(id_array, 0, 52 * sizeof(pro_sort_id_t));

#if 1

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort_new;
      n = 52;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort_new;
      n = 0;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
  }

  view_cnt = db_dvbs_get_count(view_id);
  p_sort_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  memset(p_sort_buf, 0, DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  p_pro_item = (pro_sort_t *)p_sort_buf;
  pos_off = 0;
  count = 0;
  p_mem = mtos_malloc(view_cnt * sizeof(u16));

  //a-z
  for( outer_cnt = view_cnt ; outer_cnt > 0; outer_cnt--)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id,outer_cnt-1);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    for(i = 0;i < 52; i++)
    {
      if(i < 26)
      {
        if((prv_prog_node.name[0] == (0x41 + i) 
        || prv_prog_node.name[0]== (0x61 + i))
          && prv_prog_node.is_scrambled == FALSE)
        {
          if(id_array[i].id_init == 0)
          {

            p_pro_item->node_id = prv_element_id;
            memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
            p_pro_item->p_next = NULL;
            p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
            
            id_array[i].p_start = p_sort_buf + pos_off;
            id_array[i].id_init = 1;
            id_array[i].p_last = p_sort_buf + pos_off;
            id_array[i].id_count ++;
            
          }
          else if(id_array[i].p_last != NULL)
          {

            p_pro_item->node_id = prv_element_id;
            memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
            p_pro_item->p_next = NULL;
            p_pro_item->is_scrambled = prv_prog_node.is_scrambled;

            p_pro_item_tmp = (pro_sort_t *)(id_array[i].p_last);
            p_pro_item_tmp->p_next = p_sort_buf + pos_off;
            
            id_array[i].p_last = p_sort_buf + pos_off;
            id_array[i].id_count ++;
          }
          break;
        }
      }
      else
      {
        if((prv_prog_node.name[0] == (0x41 + i - 26) 
        || prv_prog_node.name[0]== (0x61 + i - 26))
          && prv_prog_node.is_scrambled == TRUE)
        {
          if(id_array[i].id_init == 0)
          {

            p_pro_item->node_id = prv_element_id;
            memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
            p_pro_item->p_next = NULL;
            p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
            
            id_array[i].p_start = p_sort_buf + pos_off;
            id_array[i].id_init = 1;
            id_array[i].p_last = p_sort_buf + pos_off;
            id_array[i].id_count ++;
            
          }
          else if(id_array[i].p_last != NULL)
          {

            p_pro_item->node_id = prv_element_id;
            memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
            p_pro_item->p_next = NULL;
            p_pro_item->is_scrambled = prv_prog_node.is_scrambled;

            p_pro_item_tmp = (pro_sort_t *)(id_array[i].p_last);
            p_pro_item_tmp->p_next = p_sort_buf + pos_off;
            
            id_array[i].p_last = p_sort_buf + pos_off;
            id_array[i].id_count ++;
          }
          break;
        }
      }
      
    }

    p_pro_item ++;
    pos_off += sizeof(pro_sort_t);

  }

  count = 0;
  for(i = (n == 52)?(0):(51); (n == 52)?(i < n):(i >= 0); (n == 52)?(i ++):(i --))
  {
    if(n==0)
    {
      if(i>25)
        k = i - 26;
      else
        k = i + 26;
    }
    else
    {
      k = i;
    }
    if(id_array[k].id_init == 1)
    {
      p_mem_tmp = mtos_malloc(id_array[k].id_count * sizeof(u16));
      j = 0;
      
      p_pro_item_tmp = id_array[k].p_start;
      *((u16 *)p_mem_tmp + j) = p_pro_item_tmp->node_id;
      j = 1;

      while(p_pro_item_tmp->p_next != NULL)
      {
        p_pro_item_tmp = p_pro_item_tmp->p_next;
        *((u16 *)p_mem_tmp + j) = p_pro_item_tmp->node_id;
        j++;
      }

      for( outer_cnt = j ; outer_cnt >= 0; outer_cnt--)
      {
        for(inner_cnt = 0 ; inner_cnt < outer_cnt - 1; inner_cnt++)
        {
          if(inner_cnt == 0)
          {
            p_pro_item_prv_tmp = id_array[k].p_start;
            while(p_pro_item_prv_tmp != NULL)
            {
              if(p_pro_item_prv_tmp->node_id  == (*(u16 *)(p_mem_tmp + (inner_cnt) * 2)))
              {
                p_pro_item_prv = p_pro_item_prv_tmp;
                break;
              }
              p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
            }

          }

          p_pro_item_prv_tmp = id_array[k].p_start;
          while(p_pro_item_prv_tmp != NULL)
          {
            if(p_pro_item_prv_tmp->node_id  == (*(u16 *)(p_mem_tmp + (inner_cnt + 1) * 2)))
            {
              p_pro_item_cur = p_pro_item_prv_tmp;
              break;
            }
            p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
          }

          if(sort_func((u8*)p_pro_item_cur, (u8*)p_pro_item_prv, DB_DVBS_PG_NODE) 
                                                                  == TRUE)
          {
            //Remain current node status unchanged
            temp = *(u16 *)(p_mem_tmp + (inner_cnt + 1) * 2);
            *(u16 *)(p_mem_tmp + (inner_cnt + 1) * 2) = *(u16 *)(p_mem_tmp + (inner_cnt) * 2);
            *(u16 *)(p_mem_tmp + (inner_cnt) * 2) = temp;
          }
          else
          {
            //Set current node to previous
            p_pro_item_prv = p_pro_item_cur;
          }
        }
      }

      if(j > 0)
      {
        for(m = 0;m < j; m ++)
        {
          temp = *(u16 *)(p_mem_tmp + m * 2) | 0x8000;
          *(u16 *)(p_mem_tmp + m * 2) = temp;
        }
        
        memcpy(p_mem + count * 2 ,p_mem_tmp,j * 2);
        count += j;
      }

      mtos_free(p_mem_tmp);
    }
    
  }

  db_dvbs_dump_item_in_view(view_id, p_mem, count * 2);
  mtos_free(p_sort_buf);
  mtos_free(p_mem);
  
#else

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
  }

  view_cnt = db_dvbs_get_count(view_id);
  
  for( outer_cnt = view_cnt ; outer_cnt >= 0; outer_cnt--)
  {
    for(inner_cnt = 0 ; inner_cnt < outer_cnt -1; inner_cnt++)
    {
      if(inner_cnt == 0)
      {
        prv_element_id = db_dvbs_get_id_by_view_pos(view_id,inner_cnt);
        ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
        MT_ASSERT(ret == DB_DVBS_OK);
      }

      cur_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt + 1);
      ret = db_dvbs_get_pg_by_id_sort(cur_element_id, &cur_prog_node);
      MT_ASSERT(ret == DB_DVBS_OK);

      if(sort_func((u8*)&cur_prog_node, (u8*)&prv_prog_node, DB_DVBS_PG_NODE) 
                                                                    == TRUE)
      {
        //Remain current node status unchanged
        db_dvbs_exchange_item_in_view(view_id, inner_cnt + 1, inner_cnt);
      }
      else
      {
        //Set current node to previous
        memcpy(&prv_prog_node, &cur_prog_node, sizeof(dvbs_prog_node_t));
      }
    }
  }
#endif
}

#endif

void db_dvbs_pg_sort_init(u8 view_id)
{
  s16 view_cnt  = 0;
  s16 outer_cnt = 0;
  u16 prv_element_id = 0;
  dvbs_prog_node_t prv_prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  u32 pos_off = 0;
  u16 * p_mem_tmp = NULL;
  pro_sort_t *p_pro_item = NULL;
  s16 i = 0,j = 0;
  u16 count = 0;
  dvbs_sort_type_t sort_type;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  p_sort_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  p_sort_id_buf = mtos_malloc(31 * sizeof(pro_sort_id_t));
  MT_ASSERT(p_sort_id_buf != NULL);
  
  memset(p_sort_buf, 0, DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  memset(p_sort_id_buf, 0, 31 * sizeof(pro_sort_id_t));

  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  g_name_view.p_mem= mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_name_view.p_mem != NULL);

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  g_free_view.p_mem= mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_free_view.p_mem != NULL);

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  g_lock_view.p_mem= mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_lock_view.p_mem != NULL);

  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  g_default_view.p_mem= mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_default_view.p_mem != NULL);
  
  view_cnt = db_dvbs_get_count(view_id);
  p_pro_item = (pro_sort_t *)p_sort_buf;
  
  pos_off = 0;
  count = 0;
  sort_type = DB_DVBS_A_Z_MODE;
 
  for( outer_cnt = 0 ; outer_cnt < view_cnt; outer_cnt ++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id,outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    
    //a-z
    for(i = 0;i < 26; i ++)
    {
      if(prv_prog_node.name[0] == (0x41 + i) 
      || prv_prog_node.name[0]== (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 0)
        {
          p_sort_id_buf[i].id_init = 1;
          p_sort_id_buf[i].id_count  = 1;
          p_sort_id_buf[i].id_offset = 0;
          p_sort_id_buf[i].id_start = 0;
          break;
        }
        else
        {            
          p_sort_id_buf[i].id_count ++;
          break;
        }
      }
    }

    //other char
    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 0)
      {
        p_sort_id_buf[i].id_init = 1;
        p_sort_id_buf[i].id_count  = 1;
        p_sort_id_buf[i].id_offset = 0;
        p_sort_id_buf[i].id_start = 0;
      }
      else
      {          
        p_sort_id_buf[i].id_count ++;
      }
    }

    //fta
    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 0)
      {
        p_sort_id_buf[27].id_init = 1;
        p_sort_id_buf[27].id_count  = 1;
        p_sort_id_buf[27].id_offset = 0;
        p_sort_id_buf[27].id_start = 0;
      }
      else
      {            
        p_sort_id_buf[27].id_count ++;
      }
    }
    else
    {
      if(p_sort_id_buf[28].id_init == 0)
      {
        p_sort_id_buf[28].id_init = 1;
        p_sort_id_buf[28].id_count  = 1;
        p_sort_id_buf[28].id_offset = 0;
        p_sort_id_buf[28].id_start = 0;
      }
      else
      {            
        p_sort_id_buf[28].id_count ++;
      }
    }

    //lock
    if(prv_prog_node.lck_flag== FALSE)
    {
      if(p_sort_id_buf[29].id_init == 0)
      {
        p_sort_id_buf[29].id_init = 1;
        p_sort_id_buf[29].id_count  = 1;
        p_sort_id_buf[29].id_offset = 0;
        p_sort_id_buf[29].id_start = 0;
      }
      else
      {            
        p_sort_id_buf[29].id_count ++;
      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 0)
      {
        p_sort_id_buf[30].id_init = 1;
        p_sort_id_buf[30].id_count  = 1;
        p_sort_id_buf[30].id_offset = 0;
        p_sort_id_buf[30].id_start = 0;
      }
      else
      {            
        p_sort_id_buf[30].id_count ++;
      }
    }
    
    p_pro_item ++;
    pos_off += sizeof(pro_sort_t);

  }

  for(i = 0;i < 27; i ++)
  {
    count = 0;
    for(j = 0;j < i; j ++)
    {
      if(p_sort_id_buf[j].id_init == 1)
      {
        count += p_sort_id_buf[j].id_count;
      }
    }

    if(p_sort_id_buf[i].id_init == 1)
       p_sort_id_buf[i].id_start = count;
  }

  if(p_sort_id_buf[27].id_init == 1)
       p_sort_id_buf[27].id_start = 0;

  if(p_sort_id_buf[28].id_init == 1)
       p_sort_id_buf[28].id_start += p_sort_id_buf[27].id_count;

  if(p_sort_id_buf[29].id_init == 1)
       p_sort_id_buf[29].id_start = 0;

  if(p_sort_id_buf[30].id_init == 1)
       p_sort_id_buf[30].id_start += p_sort_id_buf[29].id_count;
  
  pos_off = 0;
  for( outer_cnt = 0 ; outer_cnt < view_cnt; outer_cnt ++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id,outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    for(i = 0;i < 26; i ++)
    {
      if(prv_prog_node.name[0] == (0x41 + i) 
      || prv_prog_node.name[0]== (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 1)
        {
          p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
          p_pro_item->node_id = prv_element_id;
          memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
          p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
          p_pro_item->lck_flag = prv_prog_node.lck_flag;
          p_pro_item->default_order = prv_prog_node.default_order;
          
          p_sort_id_buf[i].id_offset ++;
          
        }
        break;
      }
    }

    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 1)
      {
        p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
        p_pro_item->node_id = prv_element_id;
        memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
        p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
        p_pro_item->lck_flag = prv_prog_node.lck_flag;
        p_pro_item->default_order = prv_prog_node.default_order;
        
        p_sort_id_buf[i].id_offset ++;
        
      }
    }

    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[27].id_start + p_sort_id_buf[27].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[27].id_offset ++;
        
      }

    }
    else
    {
      if(p_sort_id_buf[28].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[28].id_start + p_sort_id_buf[28].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[28].id_offset ++;
        
      }
    }

    if(prv_prog_node.lck_flag== FALSE)
    {
      if(p_sort_id_buf[29].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[29].id_start + p_sort_id_buf[29].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[29].id_offset ++;

      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[30].id_start + p_sort_id_buf[30].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[30].id_offset ++;

      }
    }

    pos_off += sizeof(pro_sort_t);
  }
  
  //OS_PRINTF("#####init delta tick = %d \n", mtos_ticks_get()-ticks);
}

#if 0
s16 partitions(u8 view_id,u16 *p_mem,void *p_start,s16 low,s16 high,db_dvbs_sort_func sort_func)
{

  u16 tmp = 0;

  pro_sort_t *p_pro_item_prv = NULL;
  pro_sort_t *p_pro_item_cur = NULL;
  pro_sort_t *p_pro_item_prv_tmp = NULL;

  u16 pivotkey = 0;

  pivotkey = *(p_mem + low);
  //db_dvbs_getp_item_in_view(view_id,low,&pivotkey);
  
  //prv_element_id = db_dvbs_get_id_by_view_pos(view_id,low);
  //ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
  //MT_ASSERT(ret == DB_DVBS_OK);

  p_pro_item_prv_tmp = p_start;
  while(p_pro_item_prv_tmp != NULL)
  {
    if(p_pro_item_prv_tmp->node_id  == *(p_mem + low))
    {
      p_pro_item_prv = p_pro_item_prv_tmp;
      break;
    }
    //p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
  }

  while(low < high)
  {
    //cur_element_id = db_dvbs_get_id_by_view_pos(view_id,high);
    //ret = db_dvbs_get_pg_by_id(cur_element_id, &cur_prog_node);
    //MT_ASSERT(ret == DB_DVBS_OK);

    p_pro_item_prv_tmp = p_start;
    while(p_pro_item_prv_tmp != NULL)
    {
      if(p_pro_item_prv_tmp->node_id  == *(p_mem + high))
      {
        p_pro_item_cur = p_pro_item_prv_tmp;
        break;
      }
      p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
    }
    
    while(low < high &&
      sort_func((u8*)p_pro_item_cur, (u8*)p_pro_item_prv, DB_DVBS_PG_NODE) != TRUE)
    {
      --high;
      
      //cur_element_id = db_dvbs_get_id_by_view_pos(view_id,high);
      //ret = db_dvbs_get_pg_by_id(cur_element_id, &cur_prog_node);
      //MT_ASSERT(ret == DB_DVBS_OK);
      p_pro_item_prv_tmp = p_start;
      while(p_pro_item_prv_tmp != NULL)
      {
        if(p_pro_item_prv_tmp->node_id  == *(p_mem + high))
        {
          p_pro_item_cur = p_pro_item_prv_tmp;
          break;
        }
        p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
      }
    }

    tmp = *(p_mem + high);
    *(p_mem + low) = tmp;
    //db_dvbs_getp_item_in_view(view_id,high,&tmp);
    //db_dvbs_setp_item_in_view(view_id,low,tmp);

    //cur_element_id = db_dvbs_get_id_by_view_pos(view_id,low);
    //ret = db_dvbs_get_pg_by_id(cur_element_id, &cur_prog_node);
    //MT_ASSERT(ret == DB_DVBS_OK);

    p_pro_item_prv_tmp = p_start;
    while(p_pro_item_prv_tmp != NULL)
    {
      if(p_pro_item_prv_tmp->node_id  == *(p_mem + low))
      {
        p_pro_item_cur = p_pro_item_prv_tmp;
        break;
      }
      p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
    }
    while(low < high &&
      sort_func((u8*)p_pro_item_cur, (u8*)p_pro_item_prv, DB_DVBS_PG_NODE) == TRUE)
    {
      ++low;
      
      //cur_element_id = db_dvbs_get_id_by_view_pos(view_id,low);
      //ret = db_dvbs_get_pg_by_id(cur_element_id, &cur_prog_node);
      //MT_ASSERT(ret == DB_DVBS_OK);
      p_pro_item_prv_tmp = p_start;
      while(p_pro_item_prv_tmp != NULL)
      {
        if(p_pro_item_prv_tmp->node_id  == *(p_mem + low))
        {
          p_pro_item_cur = p_pro_item_prv_tmp;
          break;
        }
        p_pro_item_prv_tmp = p_pro_item_prv_tmp->p_next;
      }
    }

    tmp = *(p_mem + low);
    *(p_mem + high) = tmp;
    //db_dvbs_getp_item_in_view(view_id,low,&tmp);
    //db_dvbs_setp_item_in_view(view_id,high,tmp);
  }

  *(p_mem + low) = pivotkey;
  //db_dvbs_setp_item_in_view(view_id,low,pivotkey);
  return low;
}

void qsort(u8 view_id,u16 *p_mem,void *p_start,s16 low,s16 high,db_dvbs_sort_func sort_func)
{
  s16 pivottag;
  if(low < high)
  { 
    pivottag=partitions(view_id,p_mem,p_start,low,high,sort_func);
    qsort(view_id,p_mem,p_start,low,pivottag-1,sort_func);
    qsort(view_id,p_mem,p_start,pivottag+1,high,sort_func);
  }
}
#endif

void q_sort( pro_sort_t *p_sort, pro_sort_id_t *p_group,dvbs_sort_type_t sort_type);

/*!
  \param[in]parent_view_id: view to be sorted
  \param[in]sort_type:			sort type to be selected
  \param[in]p_sub_view_id:	the sub-view created based on sort result
  \param[in]view:						view to be sorted
 */
void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  s16 view_cnt  = 0;
  u32 pos_off = 0;
  pro_sort_t *p_pro_item = NULL;
  s16 i = 0;
  u16 n = 0 , k = 0;
  u16 count = 0;
  u16 temp = 0;
  u16 *p_mem = NULL;
  pro_sort_id_t all = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  //Set sort function
  switch(sort_type )
  {
    case DB_DVBS_A_Z_MODE:
      n = 27;
      break;
    case DB_DVBS_Z_A_MODE:
      n = 27;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      break;
  }

  p_pro_item = (pro_sort_t *)p_sort_buf;
  pos_off = 0;
  count = 0;
  view_cnt = db_dvbs_get_count(view_id);
  
  p_mem = mtos_malloc(view_cnt * sizeof(u16));

  if(sort_type == DB_DVBS_A_Z_MODE || sort_type == DB_DVBS_Z_A_MODE)
  {
    if(g_name_view.view_init == 0)
    {
      count = 0;
      for(i = 0; i < n; i ++)
      {
        k = i;
        if(p_sort_id_buf[k].id_init == 1)
        {
          q_sort(p_sort_buf,&p_sort_id_buf[k],DB_DVBS_A_Z_MODE);
        }
      }

      for(i = 0; i < view_cnt;i++)
      {
        *(p_mem + i) = p_sort_buf[i].node_id | 0x8000;
      }

      if(sort_type != DB_DVBS_A_Z_MODE)
      {
        for(i = 0;i < view_cnt / 2 ;i ++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (view_cnt - 1 - i));
          *(p_mem + (view_cnt - 1 - i)) = temp;
        }
      }

      g_name_view.view_init = 1;
      g_name_view.view_count= view_cnt;
      g_name_view.sort_type = sort_type;
      
      for(i = 0; i < view_cnt;i ++)
      {
        *(g_name_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_name_view.sort_type == sort_type)
      {
        for(i = 0; i < g_name_view.view_count;i ++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
      }
      else
      {
        for(i = 0; i < g_name_view.view_count;i ++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
        for(i = 0;i < g_name_view.view_count / 2 ;i ++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_name_view.view_count - 1 - i));
          *(p_mem + (g_name_view.view_count - 1 - i)) = temp;
        }
      }
    }

    
  }
  else if(sort_type == DB_DVBS_FTA_FIRST_MODE)
  {
    if(g_free_view.view_init == 0)
    {
      count = 0;

      g_free_view.view_init = 1;
      g_free_view.view_count= view_cnt;
      g_free_view.sort_type = sort_type;

      for(i = 0; i < g_free_view.view_count;i ++)
      {
        *(p_mem + i) = *(g_free_view.p_mem + i);
      }
      
    }
    else
    {
      if(g_free_view.sort_type == sort_type)
      {
        for(i = 0; i < g_free_view.view_count;i ++)
        {
          *(p_mem + i) = *(g_free_view.p_mem + i);
        }
        for(i = 0;i < g_free_view.view_count / 2 ;i ++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_free_view.view_count - 1 - i));
          *(p_mem + (g_free_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt;i ++)
        {
          *(g_free_view.p_mem + i) = *(p_mem + i);
        }
      }
      count = g_free_view.view_count;
    }
    
  }
  else if(sort_type == DB_DVBS_LOCK_FIRST_MODE)
  {
    if(g_lock_view.view_init == 0)
    {
      count = 0;

      g_lock_view.view_init = 1;
      g_lock_view.view_count= view_cnt;
      g_lock_view.sort_type = sort_type;

      for(i = 0; i < g_lock_view.view_count;i ++)
      {
        *(p_mem + i) = *(g_lock_view.p_mem + i);
      }
    }
    else
    {
      if(g_lock_view.sort_type == sort_type)
      {
        for(i = 0; i < g_lock_view.view_count;i++)
        {
          *(p_mem + i) = *(g_lock_view.p_mem + i);
        }
        for(i = 0;i < g_lock_view.view_count / 2 ;i ++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_lock_view.view_count - 1 - i));
          *(p_mem + (g_lock_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt;i ++)
        {
          *(g_lock_view.p_mem + i) = *(p_mem + i);
        }
      }
      count = g_lock_view.view_count;
    }
    
  }
  if(sort_type == DB_DVBS_DEFAULT_ORDER_MODE )
  {

    if(g_default_view.view_init == 0)
    {
      count = 0;
      
      all.id_count = view_cnt;
      all.id_offset = view_cnt;
      all.id_start = 0;
      all.id_init = 1;

      if(all.id_init == 1)
      {
        q_sort(p_sort_buf,&all,sort_type);
      }

      for(i = 0; i < view_cnt;i ++)
      {
        *(p_mem + i) = p_sort_buf[i].node_id | 0x8000;
      }

      g_default_view.view_init = 1;
      g_default_view.view_count= view_cnt;
      g_default_view.sort_type = sort_type;
      
      for(i = 0; i < view_cnt;i ++)
      {
        *(g_default_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_default_view.sort_type == sort_type)
      {
        for(i = 0; i < g_default_view.view_count; i ++)
        {
          *(p_mem + i) = *(g_default_view.p_mem + i);
        }
      }
    }
    
  }

  db_dvbs_dump_item_in_view(view_id, p_mem, view_cnt);

  mtos_free(p_mem);
}

void db_dvbs_pg_sort_deinit()
{
  if(p_sort_buf != NULL)
    mtos_free(p_sort_buf);
  if(p_sort_id_buf != NULL)
    mtos_free(p_sort_id_buf);
  
  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  if(g_name_view.p_mem != NULL)
    mtos_free(g_name_view.p_mem);
  g_name_view.p_mem= NULL;

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  if(g_free_view.p_mem != NULL)
    mtos_free(g_free_view.p_mem);
  g_free_view.p_mem= NULL;

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  if(g_lock_view.p_mem != NULL)
    mtos_free(g_lock_view.p_mem);
  g_lock_view.p_mem= NULL;

  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  if(g_default_view.p_mem != NULL)
    mtos_free(g_default_view.p_mem);
  g_default_view.p_mem= NULL;
}

/*!
  \param[in]parent_view_id: view to be sorted
  \param[in]sort_type:			sort type to be selected
  \param[in]p_sub_view_id:	the sub-view created based on sort result
  \param[in]view:						view to be sorted
 */
void db_dvbs_tp_sort(u8 view_id, dvbs_sort_type_t sort_type )
{
  db_dvbs_sort_func sort_func = NULL;	
  u16 view_cnt  = 0;

  u16 inner_cnt = 0;
  u16 outer_cnt = 0;

  u16 cur_element_id = 0;
  u16 prv_element_id = 0;

  dvbs_tp_node_t cur_tp_node = {0};
  dvbs_tp_node_t prv_tp_node = {0};
  db_dvbs_ret_t  ret= DB_DVBS_OK;

  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
  }

  //Create view
  view_cnt = db_dvbs_get_count(view_id);

  for( outer_cnt = view_cnt ; outer_cnt > 0; outer_cnt--)
  {
    for(inner_cnt = 0 ; inner_cnt < outer_cnt; inner_cnt++)
    {
      if(inner_cnt == 0)
      {
        prv_element_id = db_dvbs_get_id_by_view_pos(view_id,inner_cnt);
        ret = db_dvbs_get_tp_by_id(prv_element_id, &prv_tp_node);
        MT_ASSERT(ret == DB_DVBS_OK);
      }

      cur_element_id = db_dvbs_get_id_by_view_pos(view_id,inner_cnt + 1);
      ret	= db_dvbs_get_tp_by_id(cur_element_id, &cur_tp_node);
      MT_ASSERT(ret == DB_DVBS_OK);

      if(sort_func((u8*)&cur_tp_node, (u8*)&prv_tp_node,DB_DVBS_PG_NODE) 
            == TRUE)
      {
        //Remain current node status unchanged
        db_dvbs_exchange_item_in_view(view_id, outer_cnt, inner_cnt);
      }
      else
      {
        //Set current node to previous
        memcpy(&prv_tp_node, &cur_tp_node, sizeof(dvbs_prog_node_t));
      }
    }
  }

  //db_dvbs_destroy_view(view_id);

}

u32 get_audio_channel(dvbs_prog_node_t *p_pg)
{
  language_set_t lang_set;
  char **p_lang_code_list_b = sys_status_get_lang_code(TRUE);
  char **p_lang_code_list_t = sys_status_get_lang_code(FALSE);
  char *p_cur_lang_code_b = NULL;
  char *p_cur_lang_code_t = NULL;
  u16 b_type = 0;
  u16 t_type = 0;
  BOOL found_trace = FALSE;
  u16 pg_lang = 0;
  u32 i = 0;
  
  //get current language code
  sys_status_get_lang_set(&lang_set);
  
  //search first audio trace
  p_cur_lang_code_b = p_lang_code_list_b[lang_set.first_audio];
  p_cur_lang_code_t = p_lang_code_list_t[lang_set.first_audio];
  b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
  t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));

  for(i=0; i<p_pg->audio_ch_num; i++)
  {
     //match language code
    //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
    //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
    pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);
    
    if((pg_lang == b_type) || (pg_lang == t_type))
    {
      found_trace = TRUE;
      break;
    }
  }

  //search second audio trace
  if(!found_trace)
  {
    p_cur_lang_code_b = p_lang_code_list_b[lang_set.second_audio];
    p_cur_lang_code_t = p_lang_code_list_t[lang_set.second_audio];
    b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
    t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));
    
    for(i=0; i<p_pg->audio_ch_num; i++)
    {
       //match language code
      //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
      //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
      pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);
       
      if((pg_lang == b_type) || (pg_lang == t_type))
      {
        found_trace = TRUE;
        break;
      }
    }
  }

  return found_trace ? i : p_pg->audio_channel;
}

#define ROOT (0)
typedef struct 
{
  int nim;
  int max;
}stack_info_t;

BOOL str_cmp_a_z(pro_sort_t* prev_node, pro_sort_t* cur_node)
{
  return uni_strcmp(prev_node->name,cur_node->name);
}

BOOL str_cmp_z_a(pro_sort_t* prev_node, pro_sort_t* cur_node)
{
  return uni_strcmp(cur_node->name,prev_node->name);
}

BOOL default_order_cmp(pro_sort_t* prev_node, pro_sort_t* cur_node)
{
  return prev_node->default_order < cur_node->default_order;
}

void str_change(pro_sort_t *p_1, pro_sort_t *p_2)
{
  pro_sort_t temp;
  int size = sizeof(pro_sort_t);

  memcpy(&temp, p_1, size);
  memcpy(p_1, p_2, size);
  memcpy(p_2, &temp, size);
}

BOOL division(pro_sort_t *p_sort, stack_info_t *p_s_info,dvbs_sort_type_t sort_type)
{
  int tag = p_s_info->nim;
  int front = p_s_info->nim;
  int end = p_s_info->max;
  int front_bak = p_s_info->nim;
  int end_bak = p_s_info->max;
  int ret = 0;
  BOOL along = TRUE;
  db_dvbs_qsort_func sort_func  = NULL;

  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = str_cmp_a_z;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = str_cmp_z_a;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = default_order_cmp;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_cmp;
      break;
  }

  while(front < end)
  {
    if(along) //using the end
    {

      ret = sort_func( &p_sort[end], &p_sort[tag]); //compare the end
      if(ret > 0) //tag > end
      {
        str_change(p_sort+ tag, p_sort + end);
        tag = end;
        along = FALSE;
        front ++;
      }
      else
      {
        end --;
      }
    }
    else
    {

      ret = sort_func(&p_sort[tag], &p_sort[front]); //compare the front
      if(ret > 0) //front > tag
      {
        str_change(p_sort + front, p_sort + tag);
        tag = front;
        along = TRUE;
        end --;
      }
      else
      {
        front ++;
      }
    }
  }

  //info
  if(p_s_info->nim == front) //tag is nim
  {
    p_s_info->nim ++;
    return FALSE;
  }
  else if(p_s_info->max == end) //tag is max
  {
    p_s_info->max --;
    return FALSE;
  }
  else
  {
    //push big array first
    if(front - p_s_info->nim > p_s_info->max - front)
    {
      p_s_info->nim = front_bak;
      p_s_info->max = front - 1;

      p_s_info ++;
      p_s_info->nim = front + 1;
      p_s_info->max = end_bak;

    }
    else
    {
      p_s_info->nim = front + 1;
      p_s_info->max = end_bak;

      p_s_info ++;
      p_s_info->nim = front_bak;
      p_s_info->max = front - 1;
    }
    return TRUE;
  }
}

void baobao_sort(pro_sort_t *p_sort, int cnt,dvbs_sort_type_t sort_type)
{
  int i = 0;
  int j = 0;
  db_dvbs_qsort_func sort_func = NULL;

  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = str_cmp_a_z;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = str_cmp_z_a;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = default_order_cmp;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_cmp;
      break;
  }

  for(i = 0; i < cnt; i ++)
  {
    for(j= i + 1; j < cnt; j ++)
    {

      if(sort_func(&p_sort[j], &p_sort[i]) > 0)
      {
        str_change(p_sort + i, p_sort + j);
      }

    }
  }
}

void q_sort( pro_sort_t *p_sort, pro_sort_id_t *p_group,dvbs_sort_type_t sort_type)
{
  pro_sort_t * tmp1;
  pro_sort_t * tmp2;
  stack_info_t s_info[32] = {{0}};
  int cur_stack = ROOT;

  if(NULL == p_group || NULL == p_sort)
  {
    return;
  }

  if(0 == p_group->id_init || 0 == p_group->id_offset)
  {
    return;
  }

  //init the first stack
  s_info[cur_stack].nim = p_group->id_start;
  s_info[cur_stack].max = p_group->id_start + p_group->id_offset - 1;

  tmp1 = p_sort + s_info[cur_stack].nim;
  tmp2 = p_sort + s_info[cur_stack].max;

  while(TRUE)
  {
    if((s_info[cur_stack].max - s_info[cur_stack].nim) < 4)
    {
      //use baobao sort
      baobao_sort(p_sort + s_info[cur_stack].nim,
      s_info[cur_stack].max - s_info[cur_stack].nim + 1,sort_type);
      if (ROOT == cur_stack)
      {
        break;
      }
      else
      {
        cur_stack --;
      }
    }
    else
    {
      if(division(p_sort, s_info+cur_stack,sort_type))
      {
        cur_stack ++;
      }
      else
      {
        //division fail, the tag is nim or max. the stack depth unchanged
      }
    }
  }
  
  //OS_PRINTF("q_sort cnt %d, ticks %d!!!!!!!!!!!!!!!!!\n", 
    //p_group->id_offset, mtos_ticks_get()-tick);
}

int service_id_compare( const void *arg1, const void *arg2 )
{
    dvbs_prog_node_t pg_node1 = {0};
    dvbs_prog_node_t pg_node2 = {0};
    u16 pg_id1 = (*(u16*)arg1)&0x0fff;
    u16 pg_id2 = (*(u16*)arg2)&0x0fff;
    db_dvbs_ret_t  db_ret1 = DB_DVBS_OK;
    db_dvbs_ret_t  db_ret2 = DB_DVBS_OK;
    
    db_ret1 = db_dvbs_get_pg_by_id(pg_id1, &pg_node1);
    db_ret2 = db_dvbs_get_pg_by_id(pg_id2, &pg_node2);

    if((DB_DVBS_OK != db_ret1) || (DB_DVBS_OK != db_ret2))
    {
        return 0;
    }
    if(pg_node1.s_id < pg_node2.s_id)
    {
        return -1;
    }
    else if(pg_node1.s_id == pg_node2.s_id)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


