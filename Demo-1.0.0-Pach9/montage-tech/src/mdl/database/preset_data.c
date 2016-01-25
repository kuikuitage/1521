/******************************************************************************/
/******************************************************************************/
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "class_factory.h"
#include "mdl.h"
#include "lib_char.h"
#include "lib_util.h"

#include "lzmaIf.h"


#include "drv_dev.h"
#include "pti.h"
#include "nim.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "nit.h"
#include "pmt.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "dvbs_util.h"

#include "preset_data.h"
#include "data_manager.h"
#include "data_base.h"

#include "lib_memp.h"

#ifdef PRESET_DEBUG
/*!
  Macro for debug
  */
#define PRESET_PRINTF    OS_PRINTF
#else
/*!
  Macro for debug
  */
#define PRESET_PRINTF DUMMY_PRINTF
#endif



#define START_FLAG    0x80

static lib_memp_t g_ext_heap;
static BOOL g_is_use_ext_buf = FALSE;



typedef struct
{
  void *p_sat;
  void *p_tp;
  void *p_pro;
  preset_node_type_t node_type;
}preset_node_t;

typedef void (*get_field_data_func)(void *p_node,
              u8 *p_buf, u16 length, u16 filedcnt);

static u32 _uni_len(const u16 *p_str)// a trick for byte alignment 
{
  const u8 *p_eos = (u8 *)p_str;
  const u8 *pp_str = (u8 *)p_str;
  while(*p_eos != 0)
  {
    p_eos +=2;
  }

  return (u32)((p_eos - pp_str)/2);
}

static u32 _uni_2_asc(u8 *p_ascstr, const u16 *p_unistr)
{
  u32 i = 0,j = 0;
  u8 *pp_unistr = (u8 *)p_unistr;

  MT_ASSERT(p_ascstr != NULL && p_unistr != NULL);

  while(pp_unistr[j] != 0)
  {
    p_ascstr[i] = (u8)pp_unistr[j];
    i ++;
    j += 2;
  }
  p_ascstr[i] = 0;

  return i;
}

void _uni_strncpy(u16 *p_dst, const u16 *p_src, u32 count)
{
  u8 *pp_src = (u8 *)p_src;
  u8 *pp_dst = (u8 *)p_dst;
  
  if((NULL == p_dst) || (NULL == p_src))
  {
    return;
  }
  while(count != 0)
  {
    *pp_dst++ = *pp_src++;
    *pp_dst++ = *pp_src++;
    count--;
  }

  *pp_dst = 0;
}


void _uni_strcpy(u16 *p_dst, const u16 *p_src)
{
  //MT_ASSERT(p_dst != NULL && p_src != NULL);
  u8 *pp_src = (u8 *)p_src;
  u8 *pp_dst = (u8 *)p_dst;
  
  if((NULL == p_dst) || (NULL == p_src))
  {
    return;
  }
  while(*pp_src != 0)
  {
    *pp_dst++ = *pp_src++;
    *pp_dst++ = *pp_src++;
    /* copy p_src over p_dst */
  }
}

 
static int uni_2_int(const u16 *p_unistr)
{
  u8 conv_buf[255] = {0};
  u16 value = 0;
  
  _uni_2_asc(conv_buf,p_unistr);
  value = atoi((char *)conv_buf);

  return value;
}

static void get_dvbs_field_data(preset_node_t *p_node, u8 *p_buf, u16 length, u16 filedcnt)
{
  char *p_temp = (char *)p_buf;
  u8 len = 0;
  u8 conv_buf[255] = {0};
  u8 pos = filedcnt;// convert excel first column to 0
  preset_dvbs_sat_t *p_sat = (preset_dvbs_sat_t *)p_node->p_sat;
  preset_dvbs_tp_t *p_tp = (preset_dvbs_tp_t *)p_node->p_tp;
  preset_dvbs_pg_t *p_pro = (preset_dvbs_pg_t *)p_node->p_pro;

  
  MT_ASSERT(p_node != NULL && p_buf != NULL);
  
  switch(pos)
  {
    case 0:
      p_node->node_type = ((*(p_temp) == 0x2A) ? PRESET_SAT: PRESET_PRO);
      if(p_node->node_type == PRESET_SAT)
      {
        _uni_strncpy((u16 *)p_sat->sat_name, (u16 *)(p_temp + 2),
            (length > MAX_NAME_LENGTH) ? MAX_NAME_LENGTH : length);
        p_sat->sat_name[MAX_NAME_LENGTH] = 0;
      }

      if(p_node->node_type == PRESET_PRO)
      {
        _uni_strncpy((u16 *)p_pro->service_name, (u16 *)p_temp,
            (length > MAX_NAME_LENGTH) ? MAX_NAME_LENGTH : length);
        p_pro->service_name[MAX_NAME_LENGTH] = 0;
      }
      break;
    case 1:
      if(p_node->node_type == PRESET_SAT)
      {
        p_sat->lnb_low = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 2:
      if(p_node->node_type == PRESET_SAT)
      {
        p_sat->lnb_high = (u32)uni_2_int((u16 *)p_temp);
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->video_pid = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 3:
      if(p_node->node_type == PRESET_SAT)
      {
        _uni_2_asc(conv_buf, (u16 *)p_temp);
        len = (u8)strlen((char *)conv_buf);
        
        if(conv_buf[len - 1] == 'W' || conv_buf[len - 1] == 'w')
        {
          conv_buf[len - 1] = '\0';
          p_sat->longitude = 0x8000 | (u16)(mt_atof((char *)conv_buf) * 100);
        }
        else
        {
          conv_buf[len - 1] = '\0';
          p_sat->longitude = (u16)(mt_atof((char *)conv_buf)*100);
        }
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->audio_pid = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 4:
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->pcr_pid = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 5:
      if(p_node->node_type == PRESET_PRO)
      {
        p_tp->frq = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 6:
      if(p_node->node_type == PRESET_PRO)
      {
        p_tp->sym = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 7:
      if(p_node->node_type == PRESET_PRO)
      {
        _uni_2_asc(conv_buf, (u16 *)p_temp);
        if(conv_buf[0] == 'H')
        {
          p_tp->pol = 0;
        }
        else if(conv_buf[0] == 'V')
        {
          p_tp->pol = 1;
        }
        else
        {
          p_tp->pol = 0;
        }
      }
      break;
    case 8:
      if(p_node->node_type == PRESET_PRO)
      {
        _uni_2_asc(conv_buf, (u16 *)p_temp);
        if(conv_buf[0] == 'L')
        {
          p_pro->audio_track = 1;
        }
        else if(conv_buf[0] == 'R')
        {
          p_pro->audio_track = 2;
        }
        else
        {
          p_pro->audio_track = 0;
        }
      }
      break;
    case 9:
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->s_id = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
    case 10:
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->net_id = (u32)uni_2_int((u16 *)p_temp);
      }
      break; 
    case 11:
      if(p_node->node_type == PRESET_PRO)
      {
        p_pro->ts_id = (u32)uni_2_int((u16 *)p_temp);
      }
      break;
  }
}

static u32 ignore_header(u32 *p_addr, u8 *p_line, u8 *p_buf,u32 size)
{
  u16 i =0, offset = 0, cur_pos = 0;
  u8 pos_info = 0;
  
  cur_pos = 14;
  
  while(cur_pos < size)
  {
    pos_info = p_buf[cur_pos];
    cur_pos ++;
    if(pos_info & 0x80)
    {
      if(p_buf[cur_pos] == 0x2A)
      {
        i++;
        offset = cur_pos - 1;
      }
      else
      {
        break;
      }
    }    
  }
  *p_line = i -1;
  *p_addr = offset;
  return offset;
}

static void preset_mem_init(preset_mem_config_t mem_cfg)
{
  if (mem_cfg.use_extern_buf)
  {
    if(lib_memp_create(&g_ext_heap,
      mem_cfg.extern_buf_addr, mem_cfg.extern_buf_size) != SUCCESS)
    {
      MT_ASSERT(0);
    }
    g_is_use_ext_buf = TRUE;
  }
  else
  {
    g_is_use_ext_buf = FALSE;
  }
}

static void preset_mem_release(void)
{
  if (g_is_use_ext_buf)
  {
    lib_memp_destroy(&g_ext_heap);
  }
  g_is_use_ext_buf = FALSE;
}


static void * preset_mem_alloc(u32 size)
{
  void *p_buf = NULL;
  
  if (g_is_use_ext_buf)
  {
    p_buf = lib_memp_alloc(&g_ext_heap, size);
  }
  else
  {
    p_buf = mtos_malloc(size);
  }

  MT_ASSERT(p_buf != NULL);
  return p_buf;
}

static void preset_mem_free(void *p_buf)
{
  RET_CODE ret = SUCCESS;
  
  if (g_is_use_ext_buf)
  {
    ret = lib_memp_free(&g_ext_heap, p_buf);
    MT_ASSERT(ret == SUCCESS);
  }
  else
  {
    mtos_free(p_buf);
  }
}


static BOOL check_is_extern_data(preset_type_t type, preset_node_type_t node_type, u8 pos)
{
  if((node_type == PRESET_SAT && (pos > 3) && type == PRESET_DVBS)
    || (node_type == PRESET_PRO && (pos > 11) && type == PRESET_DVBS))
  {
    // cur pos data is extern data by customer define. so pass it to impl
    return TRUE;
  }
  return FALSE;
}

static u8 check_data_is_valid(u8 *p_buf_start)
{
  return memcmp(p_buf_start,"presetdata",10);
}

void db_load_preset(u8 blockid, preset_type_t type,db_preset_policy_t *p_policy)
{
  u32 blocksize = 0;
  u8 *p_buf = NULL, *p_in_buf = NULL;
  u16 length = 0;
  u8 head_rows = 0;
  u8 *p_zip_sys = NULL;
  // ur preset data size must less than 384 * KBYTES
  u32 out_size = 384 * KBYTES;
  u32 zip_sys_size = 64 * KBYTES;
  BOOL ret = 0;
  u32 offset = 0;
  u8 pos_info = 0, cur_pos = 0;
  get_field_data_func parse_func = NULL;
  u16 sat_length = 0, tp_length = 0, pg_length = 0;
  preset_node_t prenode = {0}; 
  
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  // init ext buf, at first
  preset_mem_init(p_policy->mem_cfg);

  switch(type)
  {
    case PRESET_DVBS:
      parse_func = (get_field_data_func)get_dvbs_field_data;
      sat_length = sizeof(preset_dvbs_sat_t);
      prenode.p_sat = preset_mem_alloc(sat_length);
      memset(prenode.p_sat, 0, sat_length);

      tp_length = sizeof(preset_dvbs_tp_t);
      prenode.p_tp = preset_mem_alloc(tp_length);
      memset(prenode.p_tp, 0, tp_length);

      pg_length = sizeof(preset_dvbs_pg_t);
      prenode.p_pro = preset_mem_alloc(pg_length);
      memset(prenode.p_pro, 0, pg_length);
      break;
    case PRESET_DVBC:
      MT_ASSERT(0);
      break;
    case PRESET_DVBT:
      MT_ASSERT(0);
      break;
  }

  blocksize = dm_get_block_size(p_dm_handle, blockid);
  p_in_buf = preset_mem_alloc(blocksize);
  MT_ASSERT(p_in_buf != NULL);
  dm_read(p_dm_handle, blockid, 0, 0, blocksize,p_in_buf);

  p_buf = preset_mem_alloc(out_size);
  MT_ASSERT(p_buf != NULL);
  p_zip_sys = preset_mem_alloc(zip_sys_size);
  MT_ASSERT(p_zip_sys != NULL);
#if 0
  ret = gzip_uncompress(p_buf, &out_size, p_in_buf, blocksize, p_zip_sys, zip_sys_size);
  MT_ASSERT(ret == 0); 
#else
  init_fake_mem_lzma(p_zip_sys, zip_sys_size);
  ret = lzma_decompress(p_buf, &out_size, p_in_buf, blocksize);
#endif
  preset_mem_free(p_zip_sys);
  preset_mem_free(p_in_buf);

  if(check_data_is_valid(p_buf))
  {
    OS_PRINTF("ERROR::input data is not preset \n");
    MT_ASSERT(0);
  }
  
  ignore_header(&offset, &head_rows, p_buf,out_size);
  pos_info = p_buf[offset];
  offset ++;

  MT_ASSERT(p_policy->pre_process != NULL);
  p_policy->pre_process();

  while(offset < out_size)
  {    
   // prenode.node_type = check_node_type(pos_info);
    length = 2*(u16)(_uni_len((u16 *)&p_buf[offset])+1);
    
    cur_pos = pos_info & (~START_FLAG);
    if(check_is_extern_data(type, prenode.node_type, cur_pos))
    {
      MT_ASSERT(p_policy->process_custom_data != NULL);
      p_policy->process_custom_data(prenode.node_type, cur_pos, &p_buf[offset]);
    }
    else
    {
      parse_func(&prenode, &p_buf[offset], length, cur_pos);
    }
    offset += length;
    pos_info = p_buf[offset];
    offset ++;
    if(pos_info & START_FLAG || offset >= out_size)//find next row start flag, save current row info
    {
      switch(prenode.node_type)
      {
        case PRESET_SAT:
          MT_ASSERT(p_policy->process_sat != NULL);
          p_policy->process_sat(prenode.p_sat);
          break;
        case  PRESET_PRO:
          MT_ASSERT(p_policy->process_tp != NULL);
          p_policy->process_tp(prenode.p_tp);

          MT_ASSERT(p_policy->process_pg != NULL);
          p_policy->process_pg(prenode.p_pro);
          break;
        default:
          break;
      }
      
      if(pg_length > 0)
      {
        memset(prenode.p_pro, 0, pg_length);
      }
      if(sat_length > 0)
      {
        memset(prenode.p_sat, 0, sat_length);
      }
      if(tp_length > 0)
      {
        memset(prenode.p_tp, 0, tp_length);
      }
    }
  }
 
  preset_mem_free(p_buf);
  preset_mem_free(prenode.p_pro);
  preset_mem_free(prenode.p_sat);
  preset_mem_free(prenode.p_tp);  
  //impl save
  MT_ASSERT(p_policy->process_done != NULL);
  p_policy->process_done();

  
  // sort tp : NEED more time and more stksize
//  db_dvbs_tp_sort(vv_id_tp,DB_DVBS_DEFAULT_ORDER_MODE);
//  db_dvbs_save(vv_id_tp);

  // release ext buf
  preset_mem_release();

}


