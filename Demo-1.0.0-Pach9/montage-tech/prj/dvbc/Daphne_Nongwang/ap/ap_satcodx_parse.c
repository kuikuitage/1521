/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "string.h"

#include "sys_types.h"
#include "sys_define.h"

#include "lib_unicode.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_printk.h"

#include "mdl.h"
#include "class_factory.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "db_dvbs.h"
#include "data_base.h"

#include "ap_framework.h"
#include "ap_satcodx.h"
#include "ap_satcodx_common.h"
#include "ap_satcodx_parse.h"
#include "lib_unicode.h"

/*!
  \file ap_satcodx_parse.c
  This is the implementation of parse module in Satcodx app.  
*/

u32 ap_satcodx_parse_get_u32_from_data_buf(u8 *p_buf, u32 buf_size)
{
  u32 result = 0, index;
  u8 temp;

  for(index = 0; index < buf_size; index++)
  {
    temp = p_buf[index];

    if(temp >= '0' && temp <= '9')
    {
      result = result * 10 + temp - '0';
    }
    else
    {
      break;
    }
  }

  return result;
}

BOOL ap_satcodx_parse_create_db_list(satcodx_impl_data_t *p_satcodx_impl_data)
{
  u16 element_id;
  u32 index;                     
  u32 size = sizeof(satcodx_db_sat_list_t);
  p_satcodx_impl_data->p_db_list = 
       (satcodx_db_sat_list_t*) satcodx_sys_malloc(p_satcodx_impl_data, size);
  if(p_satcodx_impl_data->p_db_list == NULL)
  {
    return FALSE;
  }
  else
  {
    memset(p_satcodx_impl_data->p_db_list,0,size); 
  }
  SATCODX_DBG(("ap_satcodx_parse_create_db_list++++++timestamp=%d\n",
  mtos_ticks_get()));
  p_satcodx_impl_data->p_db_list->sat_view_id =
                       db_dvbs_create_view(DB_DVBS_SAT, 0, NULL);
  p_satcodx_impl_data->p_db_list->tp_view_id   = 
    db_dvbs_create_view(DB_DVBS_ALL_TP,
                       p_satcodx_impl_data->p_db_list->sat_view_id,
                       NULL);
  p_satcodx_impl_data->p_db_list->prog_view_id =
    db_dvbs_create_view(DB_DVBS_ALL_PG,
                       p_satcodx_impl_data->p_db_list->tp_view_id,
                       NULL);
  p_satcodx_impl_data->p_db_list->cur_db_sat_cnt =
    db_dvbs_get_count(p_satcodx_impl_data->p_db_list->sat_view_id);
	// there' none of satellite
	if (p_satcodx_impl_data->p_db_list->cur_db_sat_cnt == 0)
	{
		p_satcodx_impl_data->p_db_list->p_sat_list = NULL;
		return TRUE;
	}

  //allocate memory
  size = p_satcodx_impl_data->p_db_list->cur_db_sat_cnt * sizeof(sat_node_t);
  p_satcodx_impl_data->p_db_list->p_sat_list = 
    (sat_node_t*)satcodx_sys_malloc(p_satcodx_impl_data, size);
  if(p_satcodx_impl_data->p_db_list->p_sat_list == NULL)
  {
    ap_satcodx_parse_free_db_list(p_satcodx_impl_data);
    return FALSE;
  }
  else
  {
    memset(p_satcodx_impl_data->p_db_list->p_sat_list,0,size);
  }
  for(index = 0;
       index < p_satcodx_impl_data->p_db_list->cur_db_sat_cnt;
       index++)
  {
    element_id = 
      db_dvbs_get_id_by_view_pos(p_satcodx_impl_data->p_db_list->sat_view_id,
                       (u16)index);
    db_dvbs_get_sat_by_id(element_id, 
                       &p_satcodx_impl_data->p_db_list->p_sat_list[index]);
  }
  SATCODX_DBG(("ap_satcodx_parse_create_db_list----timestamp=%d\n",
                       mtos_ticks_get()));
  
  return TRUE;
}

void   ap_satcodx_parse_free_db_list(satcodx_impl_data_t *p_satcodx_impl_data)
{
 if(p_satcodx_impl_data->p_db_list != NULL)
  {   
   if(p_satcodx_impl_data->p_db_list->p_sat_list != NULL)
    {
     satcodx_sys_free(p_satcodx_impl_data,
                       p_satcodx_impl_data->p_db_list->p_sat_list);
     p_satcodx_impl_data->p_db_list->p_sat_list = NULL;
    }
   ///////////////////////////
   satcodx_sys_free(p_satcodx_impl_data, p_satcodx_impl_data->p_db_list);
   p_satcodx_impl_data->p_db_list = NULL;
  }
}
BOOL ap_satcodx_parse_create_tp_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t * p_sat_node)
{
 return TRUE;
}
BOOL ap_satcodx_parse_add_tp_node_to_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t *p_sat_node,
                       dvbs_tp_node_t *p_dvbs_tp_node)
{
  satcodx_tp_node_t *p_new_tp_element = NULL;
  p_new_tp_element = 
    (satcodx_tp_node_t*) satcodx_static_malloc(p_satcodx_impl_data,
                                               sizeof(satcodx_tp_node_t));
  if(p_new_tp_element == NULL)
  {
    //memory full!
    return FALSE;
  }
  else
  {
    memset(p_new_tp_element,0,sizeof(satcodx_tp_node_t));
  }
  //fill this node!
  p_new_tp_element->id = (u16)p_dvbs_tp_node->id;
  p_new_tp_element->freq = p_dvbs_tp_node->freq;
  p_new_tp_element->polarity  = (u16)p_dvbs_tp_node->polarity;
  p_new_tp_element->sym = p_dvbs_tp_node->sym;
  if(p_sat_node->p_tp_node_list != NULL)
  {
    p_sat_node->p_last_node_in_tp_list->p_next_tp_node = p_new_tp_element;
  }
  else
  {
    p_sat_node->p_tp_node_list = p_new_tp_element;
  }
  p_sat_node->p_last_node_in_tp_list = p_new_tp_element;
  return TRUE;
}

void  ap_satcodx_parse_free_tp_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t *p_sat_node)
{
  satcodx_tp_node_t *p_next_element = NULL;
  satcodx_tp_node_t *p_cur_element =  p_sat_node->p_tp_node_list;
  while(p_cur_element != NULL)
  {
    p_next_element = p_cur_element->p_next_tp_node;
    satcodx_static_free(p_satcodx_impl_data, p_cur_element);
    p_cur_element = p_next_element;
  }
  p_sat_node->p_tp_node_list = NULL;
  p_sat_node->p_last_node_in_tp_list = NULL;
  p_sat_node->cur_tp_node_cnt = 0;
}

BOOL ap_satcodx_parse_create_sat_list(
                       satcodx_impl_data_t *p_satcodx_impl_data)
{
  return TRUE;
}
BOOL ap_satcodx_parse_add_sat_node_to_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       sat_node_t *p_sat_node)
{
        satcodx_sat_node_t *p_new_sat_node = NULL;
  //allocate memory for this node!
  p_new_sat_node = (satcodx_sat_node_t*) satcodx_static_malloc(
                       p_satcodx_impl_data,
                       sizeof(satcodx_sat_node_t));
  if(p_new_sat_node == NULL)
        {
         //memory full!
         return FALSE;
        }
  else
    {
     memset(p_new_sat_node,0,sizeof(satcodx_sat_node_t));
    }
 //fill this node!
 p_new_sat_node->id  = (u16)p_sat_node->id;
 uni_strncpy(p_new_sat_node->name, 
             p_sat_node->name, 
             DB_DVBS_MAX_NAME_LENGTH);
 p_satcodx_impl_data->p_result_list->cur_sat_node_cnt ++;
 //push the new node to the end of sat list!
 if(p_satcodx_impl_data->p_result_list->p_sat_node_list != NULL)
 {
  p_satcodx_impl_data->p_result_list->p_last_node_in_sat_list->p_next_sat_node
    = p_new_sat_node;
 }
 else
 {
         p_satcodx_impl_data->p_result_list->p_sat_node_list = p_new_sat_node;
 }
 p_satcodx_impl_data->p_result_list->p_last_node_in_sat_list = p_new_sat_node;
 return TRUE;
}

void  ap_satcodx_parse_free_sat_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_result_list_t *p_result_list)
{
 satcodx_sat_node_t *p_next_element = NULL;
 satcodx_sat_node_t *p_cur_element =  p_result_list->p_sat_node_list;
 while(p_cur_element != NULL)
 {
  ap_satcodx_parse_free_tp_list(p_satcodx_impl_data, p_cur_element);
  p_next_element = p_cur_element->p_next_sat_node;
  satcodx_static_free(p_satcodx_impl_data, p_cur_element);
  p_cur_element = p_next_element;
 }
 p_result_list->p_sat_node_list = NULL;
 p_result_list->p_last_node_in_sat_list = NULL;
 p_result_list->cur_sat_node_cnt = 0;
}

BOOL ap_satcodx_parse_create_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data)
{
  u32 size = 0;
  size = sizeof(satcodx_result_list_t);
  p_satcodx_impl_data->p_result_list = 
    (satcodx_result_list_t*)satcodx_static_malloc(p_satcodx_impl_data,size);
  if(p_satcodx_impl_data->p_result_list == NULL)
  {
    return FALSE;
  }
  else
  {
    memset(p_satcodx_impl_data->p_result_list,0,size);
  }
  if(!ap_satcodx_parse_create_sat_list(p_satcodx_impl_data))
  {
    ap_satcodx_parse_free_result_list(p_satcodx_impl_data);
    return FALSE;
  }

  return TRUE;
}

void   ap_satcodx_parse_free_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data)
{
  if(p_satcodx_impl_data->p_result_list)
  {
    ap_satcodx_parse_free_sat_list(
          p_satcodx_impl_data,
          p_satcodx_impl_data->p_result_list);
   /////////////////////////
   satcodx_static_free(
          p_satcodx_impl_data,
          p_satcodx_impl_data->p_result_list);
   p_satcodx_impl_data->p_result_list = NULL;
   /////////////////////////
  }
}

BOOL ap_satcodx_parser_filter_data(PSDX3 pSdx3)
{
  if(pSdx3->TR != SATCODX_CHANNEL_TYPE_TV 
    && pSdx3->TR != SATCODX_CHANNEL_TYPE_RADIO)
  {
    return FALSE;
  }

  if((ap_satcodx_parse_get_u32_from_data_buf(
          pSdx3->VPID,
          SATCODX_VPID_LEN) == 0)
   && (ap_satcodx_parse_get_u32_from_data_buf(
          pSdx3->APID, 
          SATCODX_APID_LEN) == 0))
  {
    return FALSE;
  }
  return TRUE;
}

BOOL ap_satcodx_parser_get_sat_node(
                       PSDX3 pSdx3,
                       sat_node_t *p_sat_node)
{
  u32 pos_code;
  u32 dwCpysize = 
    (SATCODX_SAT_NAME_LEN > DB_DVBS_MAX_NAME_LENGTH)
    ? DB_DVBS_MAX_NAME_LENGTH 
    : SATCODX_SAT_NAME_LEN;  
  u32 index = dwCpysize;

  memset(p_sat_node, 0, sizeof(sat_node_t));

  
  // -- memcpy(p_sat_node->name, pSdx3->SatName, dwCpysize);
  str_nasc2uni(
    pSdx3->SatName, 
    p_sat_node->name, 
    dwCpysize);
  
  index--;
  while(index > 0)
  {
        if(p_sat_node->name[index] != (u16)SATCODX_CHAR_SPACE)
        {
                break;
        }
        else
        {
                p_sat_node->name[index] = (u16)SATCODX_CHAR_EOS;
                index--;
        }
  }

  p_sat_node->lnb_type = SATCODX_UNIVERSAL_LNB_TYPE;
  p_sat_node->lnb_low = SATCODX_UNIVERSAL_LNB_LOW;
  p_sat_node->lnb_high = SATCODX_UNIVERSAL_LNB_HIGH;
  
  pos_code = ap_satcodx_parse_get_u32_from_data_buf(
                       pSdx3->PosCode,
                       SATCODX_POS_CODE_LEN) * SATCODX_POS_CODE_PRESICION;
  if(pos_code <= SATCODX_POS_CODE_EAST_LIMIT)
  {
        p_sat_node->longitude = SATCODX_POS_CODE_EAST_FLAG | (u16) pos_code;
  }
  else
  {
        pos_code = SATCODX_POS_CODE_ROUND - pos_code;
        p_sat_node->longitude = SATCODX_POS_CODE_WEST_FLAG | (u16) pos_code;
  }
  return TRUE;
}

BOOL ap_satcodx_parser_get_tp_node(
                       PSDX3 pSdx3,
                       dvbs_tp_node_t *p_tp_node)
{
  memset(p_tp_node, 0, sizeof(dvbs_tp_node_t));

  switch(pSdx3->Pol)
  {
    case SATCODX_POL_TYPE_H:
    case SATCODX_POL_TYPE_RIGHT_CIRCULAR:
            p_tp_node->polarity = 0;                              // horizontal
            break;
        case SATCODX_POL_TYPE_V:
        case SATCODX_POL_TYPE_LEFT_CIRCULAR:
                p_tp_node->polarity = 1;                          // vertical
                break;  
        default:
                p_tp_node->polarity = 0;
  }
    
  p_tp_node->nit_pid = ap_satcodx_parse_get_u32_from_data_buf(
                       pSdx3->NID,
                       SATCODX_NID_LEN);
 
  p_tp_node->freq = ap_satcodx_parse_get_u32_from_data_buf(
                       pSdx3->QRG,
                       SATCODX_FREQ_VALID_LEN);
    
  p_tp_node->sym = ap_satcodx_parse_get_u32_from_data_buf(
                       pSdx3->SRate,
                       SATCODX_SYMBOL_RATE_LEN);

  return TRUE;
}
 
BOOL ap_satcodx_parser_get_prog_node(
                       PSDX3 pSdx3,
                       dvbs_prog_node_t *p_prog_node)
{
  u32 cp_name_size, cp_name_ext_size, index;
        
  cp_name_size =
    (SATCODX_PROG_NAME_LEN > DB_DVBS_MAX_NAME_LENGTH) ? 
    DB_DVBS_MAX_NAME_LENGTH : SATCODX_PROG_NAME_LEN;
  cp_name_ext_size = 
      (cp_name_size + SATCODX_PROG_NAME_EXT_LEN > DB_DVBS_MAX_NAME_LENGTH)
   ? (DB_DVBS_MAX_NAME_LENGTH - cp_name_size)
   : (cp_name_size + SATCODX_PROG_NAME_EXT_LEN);
  index = cp_name_size + cp_name_ext_size;
        
  memset(p_prog_node, 0, sizeof(dvbs_prog_node_t));
        
  // -- memcpy(p_prog_node->name, pSdx3->Name, cp_name_size);
  // -- memcpy(p_prog_node->name + cp_name_size, pSdx3->Name2, cp_name_ext_size);
  str_nasc2uni(
    pSdx3->Name, 
    p_prog_node->name,
    cp_name_size);  

  str_nasc2uni(
    pSdx3->Name2, 
    p_prog_node->name + cp_name_size,
    cp_name_ext_size);   
  
  index--;
  while(index > 0)
  {
    if(p_prog_node->name[index] != (u16)SATCODX_CHAR_SPACE)
    {
      break;
    }
    else
    {
      p_prog_node->name[index] = (u16)SATCODX_CHAR_EOS;
      index--;
    }       
  }
        
  if(pSdx3->TR == SATCODX_CHANNEL_TYPE_TV)
  {
    p_prog_node->tv_flag = 1;
    p_prog_node->video_pid =
      ap_satcodx_parse_get_u32_from_data_buf(
        pSdx3->VPID,
        SATCODX_VPID_LEN);
  }
  else if(pSdx3->TR == SATCODX_CHANNEL_TYPE_RADIO)
  {
    p_prog_node->tv_flag = 0;
    p_prog_node->video_pid = 0;
  }
        
  p_prog_node->s_id = 
    ap_satcodx_parse_get_u32_from_data_buf(pSdx3->SID, SATCODX_SID_LEN);
  p_prog_node->pcr_pid = 
    ap_satcodx_parse_get_u32_from_data_buf(pSdx3->PCR, SATCODX_PCR_PID_LEN);

  p_prog_node->audio[0].p_id = 
    (u16)ap_satcodx_parse_get_u32_from_data_buf(pSdx3->APID, SATCODX_APID_LEN);
  p_prog_node->audio_ch_num = 1;

  return TRUE;
}
BOOL ap_satcodx_parser_init(satcodx_impl_data_t *p_satcodx_impl_data)
{
  if(ap_satcodx_parse_create_db_list(p_satcodx_impl_data))
  {
    if(ap_satcodx_parse_create_result_list(p_satcodx_impl_data))
    {
      return TRUE;
    }
    else
    {
      ap_satcodx_parse_free_db_list(p_satcodx_impl_data);
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  } 
}
void ap_satcodx_parser_deinit(satcodx_impl_data_t *p_satcodx_impl_data)
{
 ap_satcodx_parse_free_result_list(p_satcodx_impl_data);
 ap_satcodx_parse_free_db_list(p_satcodx_impl_data);
}

BOOL ap_satcodx_parse_search_sync_header(satcodx_node_pos_t* p_satcodx_node)
{
  BOOL bFound = FALSE;
  while(p_satcodx_node->cur_buf_size)
  {
    if(p_satcodx_node->cur_buf_size >= SATCODX_SYNC_BYTE_LEN)
    {
      if(strncmp(
        p_satcodx_node->p_buf_pos,
        SATCODX_HEADER_STR, 
        SATCODX_HEADER_LEN) == 0)
      {
        //found satcodx name header!!!
        if(strncmp(
          p_satcodx_node->p_buf_pos+ SATCODX_SYNC_VER_OFFSET,
          SATCODX_V3_STR,
          SATCODX_VERSION_LEN) == 0)
        {
        //find one right node!!!
          bFound = TRUE;
          p_satcodx_node->cur_node_size = SATCODX_V3_BLOCK_SIZE;
        }
        else if(strncmp(p_satcodx_node->p_buf_pos + SATCODX_SYNC_VER_OFFSET,
          SATCODX_V4_STR,
          SATCODX_VERSION_LEN) == 0)
        {
        //find one right node!!!
          bFound = TRUE;
          p_satcodx_node->cur_node_size = SATCODX_V4_BLOCK_SIZE;
        }
        else
        {//error case!!
          p_satcodx_node->cur_buf_size -= SATCODX_SYNC_BYTE_LEN;
          p_satcodx_node->p_buf_pos += SATCODX_SYNC_BYTE_LEN;
        }
        if(bFound)
        {
          if(p_satcodx_node->cur_buf_size >= p_satcodx_node->cur_node_size)
          {//valid node
            p_satcodx_node->p_node = p_satcodx_node->p_buf_pos;
            p_satcodx_node->p_buf_pos +=
            p_satcodx_node->cur_node_size;
            p_satcodx_node->cur_buf_size -=
            p_satcodx_node->cur_node_size;
            return TRUE;
          }
          else
          {
            p_satcodx_node->cur_buf_size = 0;
            break;
          }
        }
      }
    }
    else
    {
      p_satcodx_node->cur_buf_size = 0;
      break;
    }
    p_satcodx_node->cur_buf_size --;
    p_satcodx_node->p_buf_pos ++;
  }
  return FALSE;
}

void ap_satcodx_parser_del_satellite(u8 view_id, u16 pos)
{
        u8 tp_view_id = 0;
        u16 i = 0;
        u16 tp_total = 0;
        void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
        u16 sat_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);

        tp_view_id = db_dvbs_create_view(DB_DVBS_SAT_TP, sat_id, NULL);
        tp_total = db_dvbs_get_count(tp_view_id);

        for(i = 0; i < tp_total; i++)
        {
                db_dvbs_del_tp(tp_view_id, i);
        }

        //System stores modifications of element to flash
        db_update_element(p_db_handle, tp_view_id); 
        //System stores modifications of view to flash
        db_update_view(p_db_handle, tp_view_id);
}

BOOL ap_satcodx_parser_store_sat_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       sat_node_t *p_sat_node)
{
  u16 item_pos;
  u32 index;
  db_dvbs_ret_t ret;
  satcodx_sat_node_t * p_cur_node 
  = p_satcodx_impl_data->p_result_list->p_sat_node_list;

  SATCODX_DBG(
    ("ap_satcodx_parser_store_sat_node find sat node++++timestamp=%d\n",
    mtos_ticks_get()));
  while(p_cur_node != NULL)
  {
    if(uni_strncmp(
        p_sat_node->name,
        p_cur_node->name,
        DB_DVBS_MAX_NAME_LENGTH) == 0)
    {
     p_sat_node->id = p_cur_node->id;
     return TRUE;
    }
   p_cur_node = p_cur_node->p_next_sat_node;
  }
  SATCODX_DBG(
    ("ap_satcodx_parser_store_sat_node find sat node----timestamp=%d\n",
    mtos_ticks_get()));
        
  for(index=0;index<p_satcodx_impl_data->p_db_list->cur_db_sat_cnt;index++)
  {
      if(uni_strncmp(
          p_sat_node->name,
          p_satcodx_impl_data->p_db_list->p_sat_list[index].name,
          DB_DVBS_MAX_NAME_LENGTH) == 0)
      {
        SATCODX_DBG(
          ("satcodx parser delete old sat node++++timestamp=%d\n",
          mtos_ticks_get()));
         //delete this sat
         item_pos = db_dvbs_get_view_pos_by_id(
                 p_satcodx_impl_data->p_db_list->sat_view_id,
                 (u16)p_satcodx_impl_data->p_db_list->p_sat_list[index].id);
         p_sat_node->id = p_satcodx_impl_data->p_db_list->p_sat_list[index].id;
         ap_satcodx_parser_del_satellite(
           p_satcodx_impl_data->p_db_list->sat_view_id,
           item_pos);
                                  
         SATCODX_DBG(
           ("satcodx parser delete old sat node----timestamp=%d\n",
           mtos_ticks_get()));
         return ap_satcodx_parse_add_sat_node_to_result_list(
           p_satcodx_impl_data,
           p_sat_node);
      }
  }
        
  SATCODX_DBG(
    ("ap_satcodx_parser_store_sat_node add sat node++++timestamp=%d\n",
    mtos_ticks_get()));
  ret = db_dvbs_add_satellite(
            p_satcodx_impl_data->p_db_list->sat_view_id,
            p_sat_node);
  SATCODX_DBG(
    ("ap_satcodx_parser_store_sat_node add sat node----timestamp=%d\n",
    mtos_ticks_get()));
  if(DB_DVBS_OK == ret)
  {
   db_dvbs_save(p_satcodx_impl_data->p_db_list->sat_view_id);
   return ap_satcodx_parse_add_sat_node_to_result_list(
            p_satcodx_impl_data,
            p_sat_node);
  }
        return FALSE;
}

BOOL ap_satcodx_parser_store_tp_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       dvbs_tp_node_t *p_tp_node)
{
        db_dvbs_ret_t ret;
  satcodx_sat_node_t * p_cur_sat_node;
  satcodx_tp_node_t * p_cur_tp_node;
  p_cur_sat_node = p_satcodx_impl_data->p_result_list->p_sat_node_list;
  //first compare with result list
  while(p_cur_sat_node != NULL)
  {
   if(p_tp_node->sat_id == p_cur_sat_node->id)
   {//found the sat!!
    break;
   }
   p_cur_sat_node = p_cur_sat_node->p_next_sat_node;
  }
  if(p_cur_sat_node == NULL)
  {
   return FALSE;
  }
  p_cur_tp_node = p_cur_sat_node->p_tp_node_list;
  while(p_cur_tp_node != NULL)
  {
     if((p_tp_node->freq == p_cur_tp_node->freq) && 
                          (p_tp_node->sym ==  p_cur_tp_node->sym) &&
                          (p_tp_node->polarity == p_cur_tp_node->polarity))
     {//found the TP
      p_tp_node->id = p_cur_tp_node->id;
      return TRUE;
     }
     p_cur_tp_node = p_cur_tp_node->p_next_tp_node;
  }
  ret = db_dvbs_add_tp(p_satcodx_impl_data->p_db_list->tp_view_id, p_tp_node);
  if(DB_DVBS_OK == ret)
  {
   db_dvbs_save(p_satcodx_impl_data->p_db_list->tp_view_id);
   return ap_satcodx_parse_add_tp_node_to_result_list(
            p_satcodx_impl_data,
            p_cur_sat_node,
            p_tp_node);
  }
        return FALSE;
}

BOOL ap_satcodx_parser_store_prog_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       dvbs_prog_node_t *p_prog_node)
{
        db_dvbs_ret_t ret;      
        
        ret = db_dvbs_add_program(
            p_satcodx_impl_data->p_db_list->prog_view_id,
            p_prog_node);
        db_dvbs_save(p_satcodx_impl_data->p_db_list->prog_view_id);

        SATCODX_DBG(("satcodx::ap prog vpid=%d apid=%d pcrpid=%d\n",
            p_prog_node->video_pid,
            p_prog_node->audio[0].p_id,
            p_prog_node->pcr_pid));        
        
        return (DB_DVBS_OK == ret);
}

BOOL ap_satcodx_parser_push_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_node_pos_t *p_satcodx_node)
{
        BOOL ret;
        sat_node_t sat_node;
        dvbs_tp_node_t tp_node;
        dvbs_prog_node_t prog_node;     
        PSDX3 pSdx3 = (PSDX3) p_satcodx_node->p_node;
  SATCODX_DBG(("ap_satcodx_parser_push_node\n"));
        ret = ap_satcodx_parser_filter_data(pSdx3);
        if(!ret)
        {
                SATCODX_DBG(("satcodx::ap Non-TV/Radio program filter\n"));
                return TRUE;    
        }
        
        ap_satcodx_parser_get_sat_node(pSdx3, &sat_node);
        ap_satcodx_parser_get_tp_node(pSdx3, &tp_node); 
        ap_satcodx_parser_get_prog_node(pSdx3, &prog_node);

        SATCODX_DBG(("ap_satcodx_parser_store_sat_node++++timestamp=%d\n",
            mtos_ticks_get()));
        ret = ap_satcodx_parser_store_sat_node(p_satcodx_impl_data,&sat_node);
        SATCODX_DBG(("ap_satcodx_parser_store_sat_node----timestamp=%d\n",
            mtos_ticks_get()));
        if(!ret)
        {               
                SATCODX_DBG(("satcodx::ap sat view is full\n"));
                return ret;
        }       
        
        tp_node.sat_id = sat_node.id;
        SATCODX_DBG(("ap_satcodx_parser_store_tp_node++++timestamp=%d\n",
            mtos_ticks_get()));
        ret = ap_satcodx_parser_store_tp_node(p_satcodx_impl_data,&tp_node);
        SATCODX_DBG(("ap_satcodx_parser_store_tp_node----timestamp=%d\n",
            mtos_ticks_get()));
        if(!ret)
        {               
                SATCODX_DBG(("satcodx::ap tp view is full\n"));
                return ret;
        }       
        
        prog_node.sat_id = sat_node.id;
        prog_node.tp_id = tp_node.id;
        SATCODX_DBG(("ap_satcodx_parser_store_prog_node++++timestamp=%d\n",
            mtos_ticks_get()));
        ret = ap_satcodx_parser_store_prog_node(
            p_satcodx_impl_data,
            &prog_node);
        SATCODX_DBG(("ap_satcodx_parser_store_prog_node----timestamp=%d\n",
            mtos_ticks_get()));
        if(!ret)
        {
                SATCODX_DBG(("satcodx::ap prog view is full\n"));
                return ret;
        }
        
        p_satcodx_impl_data->update_param.sat_id = sat_node.id;
        p_satcodx_impl_data->update_param.tp_id = tp_node.id;
        p_satcodx_impl_data->update_param.prog_id = prog_node.id;
        ap_satcodx_notify_ui(
                     SATCODX_EVT_UPDATE_STATUS,
                     SATCODX_STATUS_UPDATE_CHANNEL,
                     (u32) &p_satcodx_impl_data->update_param);
        return ret;
}

void ap_satcodx_parser_push_data(satcodx_impl_data_t *p_satcodx_impl_data)
{
 BOOL ret = TRUE;
 satcodx_node_pos_t satcodx_node;
 p_satcodx_impl_data->sm = SATCODX_SM_PARSE_DATA;
 satcodx_node.p_buf_pos = p_satcodx_impl_data->p_recv_buf; 
 satcodx_node.cur_buf_size = p_satcodx_impl_data->cur_recv_buf_used_size;
 SATCODX_DBG(("ap_satcodx_parser_push_data:data length=%d timstamp=%d\n",
            satcodx_node.cur_buf_size,
            mtos_ticks_get()));
 
 ap_satcodx_parser_deinit(p_satcodx_impl_data); 

 if(!ap_satcodx_parser_init(p_satcodx_impl_data))
 {
   ap_satcodx_notify_ui(
                     SATCODX_EVT_UPDATE_STATUS,
                     SATCODX_STATUS_REACH_MAX_CHANNEL,
                     0);
   return ;
 }
 
 while(satcodx_node.cur_buf_size)
 {
  if(ap_satcodx_parse_search_sync_header(&satcodx_node))
  {   
   ret = ap_satcodx_parser_push_node(p_satcodx_impl_data,&satcodx_node);
   if(!ret)
   {
        break;
   }
  }
  else
  {
        SATCODX_DBG(("satcodx::ap sync header not found\n"));
  }
 }
 
 ap_satcodx_parser_deinit(p_satcodx_impl_data);
 p_satcodx_impl_data->sm = SATCODX_SM_ALL_FINISH;

 if(ret)
 {
        ap_satcodx_notify_ui(
                     SATCODX_EVT_UPDATE_STATUS,
                     SATCODX_STATUS_FINISHED,
                     0);
 }
 else
 {
        ap_satcodx_notify_ui(
                     SATCODX_EVT_UPDATE_STATUS,
                     SATCODX_STATUS_REACH_MAX_CHANNEL,
                     0);
 } 
}



