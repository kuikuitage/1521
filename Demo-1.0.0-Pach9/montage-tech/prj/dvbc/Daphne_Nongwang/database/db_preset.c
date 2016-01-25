/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
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
#include "unzip.h"


#include "drv_dev.h"
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
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "db_dvbs.h"
#include "db_preset.h"
#include "data_manager.h"
#include "data_base.h"


#define DEFAULT_VOLUME_STRONG (15)


static void get_field_by_column(preset_node_t *p_node, char *p_buf, u16 length, u16 filedcnt)
{
  char *p_temp = p_buf;
  u8 len = 0;
  
  
  MT_ASSERT(p_node != NULL && p_buf != NULL);
  
  switch(filedcnt+1)
  {
    case 1:
      p_node->node_type = ((*(p_temp) == 0x2A) ? PRESET_SAT: PRESET_PRO);
      if(p_node->node_type == PRESET_SAT)
      {
        strncpy(p_node->p_sat->sat_name,p_temp+1,
          (length-1)>MAX_NAME_LENGTH?MAX_NAME_LENGTH:(length-1));
      }

      if(p_node->node_type == PRESET_PRO)
      {
        strncpy(p_node->p_pro->service_name,p_temp,
          length>MAX_NAME_LENGTH?MAX_NAME_LENGTH:length);
      }
      break;
    case 2:
      if(p_node->node_type == PRESET_SAT)
      {
        p_node->p_sat->lnb_low = (u32)atoi(p_temp);
      }
      break;
    case 3:
      if(p_node->node_type == PRESET_SAT)
      {
        p_node->p_sat->lnb_high = (u32)atoi(p_temp);
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->video_pid = (u32)atoi(p_temp);
      }
      break;
    case 4:
      if(p_node->node_type == PRESET_SAT)
      {
        len = strlen(p_temp);
        if(p_temp[len-1] == 'W' ||p_temp[len-1] == 'w')
        {
          p_temp[len-1] = '\0';
          p_node->p_sat->longitude = 0x8000 |(u16)(mt_atof(p_temp)*100);
        }
        else
        {
          p_temp[len-1] = '\0';
          p_node->p_sat->longitude = (u16)(mt_atof(p_temp)*100);
        }
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->audio_pid1 = (u32)atoi(p_temp);
      }
      break;
    case 5:
      if(p_node->node_type == PRESET_SAT)
      {
        p_node->p_sat->diseqc_port = (u32)atoi(p_temp);
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->pcr_pid = (u32)atoi(p_temp);
      }
      break;
    case 6:
      if(p_node->node_type == PRESET_SAT)
      {
        p_node->p_sat->k_option = (u32)atoi(p_temp);
      }
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_tp->frq = (u32)atoi(p_temp);
      }
      break;
    case 7:
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_tp->sym = (u32)atoi(p_temp);
      }
      break;
    case 8:
      if(p_node->node_type == PRESET_PRO)
      {
        if(p_temp[0] == 'H')
        {
          p_node->p_tp->pol = 0;
        }
        else if(p_temp[0] == 'V')
        {
          p_node->p_tp->pol = 1;
        }
        else
        {
          p_node->p_tp->pol = 0;
        }
      }
      break;
    case 9:
      if(p_node->node_type == PRESET_PRO)
      {
        if(p_temp[0] == 'L')
        {
          p_node->p_pro->audio_channel = 0;
        }
        else if(p_temp[0] == 'R')
        {
          p_node->p_pro->audio_channel = 1;
        }
        else
        {
          p_node->p_pro->audio_channel = 2;
        }
      }
      break;
    case 10:
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->s_id = (u32)atoi(p_temp);
      }
      break;
    case 11:
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->audio_pid2 = (u32)atoi(p_temp);
      }
      break; 
    case 12:
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->audio_pid3 = (u32)atoi(p_temp);
      }
      break;
   case 16:
      if(p_node->node_type == PRESET_PRO)
      {
        p_node->p_pro->is_domestic = (u16)atoi(p_temp);
      }
      break;
   case 17:
      if(p_node->node_type == PRESET_PRO)
     {
       p_node->p_pro->is_9b = (u8)atoi(p_temp);
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
    cur_pos += strlen(&p_buf[cur_pos])+1;

  }
  *p_line = i -1;
  *p_addr = offset;
  return offset;
}


void db_dvbs_load_preset(u8 blockid, preset_pg_type_t pg_type, preset_pg_area_t pg_area)
{
  u32 blocksize = 0;
  u8 *p_buf = NULL, *p_in_buf = NULL;
  u16 max_rows =0, max_cols = 0;
  u16 i =0, length = 0;
  u32 cur_sat_id = 0,cur_tp_id = 0;
  u8 vv_id_sat = 0,vv_id_tp = 0 ,vv_id_pro = 0;
  preset_tp_node_t *tp_node = NULL, *p_cur_node = NULL;
  BOOL tp_flag = FALSE;
  u16 tp_num = 0;
  u8 head_rows = 0;
  u8 *p_zip_sys = NULL;
  u32 out_size = 0;
  BOOL ret = 0;
  u32 offset = 0;
  u8 pos_info = 0;

  preset_pro_node_t rawpro = {0};
  preset_sat_node_t rawsat = {0};
  preset_tp_node_t rawtp = {0};
  preset_node_t prenode = {0}; 
  
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  sat_node_t    sat = {0};
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  prenode.p_pro = &rawpro;
  prenode.p_tp = &rawtp;
  prenode.p_sat = &rawsat;
  
  blocksize = dm_get_block_size(p_dm_handle, blockid);
  p_in_buf = mtos_malloc(blocksize);
  MT_ASSERT(p_in_buf != NULL);

  vv_id_sat = db_dvbs_create_view(DB_DVBS_SAT, 0, NULL);
  //vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  
  dm_read(p_dm_handle, blockid, 0, 0, blocksize,p_in_buf);
  
  p_buf = mtos_malloc(512*1024);
  MT_ASSERT(p_buf != NULL);
  
  p_zip_sys = mtos_malloc(200*1024);
  MT_ASSERT(p_zip_sys != NULL);
  
  init_fake_mem(p_zip_sys);
  ret = gunzip((void *)p_in_buf, (void *)p_buf, blocksize, &out_size, memcpy);
  MT_ASSERT(ret == 0);  
  
  mtos_free(p_zip_sys);
  mtos_free(p_in_buf);

  if(0 != memcmp(p_buf,"presetdata",10))
  {
    MT_ASSERT(0);
  }
  max_rows = (p_buf[10]<<8) | p_buf[11];
  max_cols = (p_buf[12]<<8) | p_buf[13];

  tp_node = mtos_malloc(max_rows*sizeof(preset_tp_node_t));
  MT_ASSERT(tp_node != NULL);

  ignore_header(&offset, &head_rows, p_buf,out_size);

  pos_info = p_buf[offset];
  offset ++;

  while(offset < out_size)
  {    
    if((pos_info & 0x80) && ((pos_info & 0x7f) != 0x00))
    {
      prenode.node_type = PRESET_PRO;
    }    
    
    length = strlen(&p_buf[offset]);
    get_field_by_column(&prenode,&p_buf[offset],length,pos_info&0x7f);
    offset +=length+1;

    pos_info = p_buf[offset];
    offset ++;
    if(pos_info & 0x80 || offset >= out_size)//find next row start flag, save current row info
    {
      switch(prenode.node_type)
      {
      case PRESET_SAT:
        if(((1 == rawsat.is_abs) && (PRESET_PG_TYPE_DVBS != pg_type)) ||
          ((0 == rawsat.is_abs) && (PRESET_PG_TYPE_ABS != pg_type)))
        {
          memset((u8*)&sat,0,sizeof(sat_node_t));
          sat.diseqc_port = rawsat.diseqc_port ;
          sat.k22 =  rawsat.k_option ;
          sat.lnb_high =  rawsat.lnb_high ;
          sat.lnb_low = rawsat.lnb_low ;
          sat.user_band = 0; // not use
          sat.band_freq = 1210; 
          sat.unicable_type = 0; //define unicable A
          if(C_BAND == dvbs_detect_lnb_freq(sat.lnb_low))
          {
            if(sat.lnb_low == sat.lnb_high)
            {
              sat.lnb_type = 0;
            }
            else
            {
              sat.lnb_type = 1;
            }
          }
          else
          {
            if(sat.lnb_low == sat.lnb_high)
            {
              sat.lnb_type = 0;
            }
            else
            {
              sat.lnb_type = 2;
            }
          }
          sat.id = cur_sat_id;
          sat.lnb_power = 0;
          sat.longitude = rawsat.longitude;

          dvb_to_unicode(rawsat.sat_name,MAX_NAME_LENGTH,
            sat.name,DB_DVBS_MAX_NAME_LENGTH);

          db_dvbs_add_satellite(vv_id_sat, &sat);
          db_dvbs_change_mark_status(vv_id_sat, 0, DB_DVBS_SEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);

          cur_sat_id = sat.id;
          memset(tp_node+tp_num,0,tp_num*sizeof(preset_tp_node_t));
          tp_num = 0;
        }
        break;
#if 0
      case  PRESET_PRO:
        if((1 == rawpro.is_9b && PRESET_PG_TYPE_DVBS != pg_type) ||
          (0 == rawpro.is_9b && PRESET_PG_TYPE_ABS != pg_type &&
          ((PRESET_PG_AREA_DOMESTIC == pg_area && 1 == rawpro.is_domestic) ||
          (PRESET_PG_AREA_OVERSEA == pg_area && 0 == rawpro.is_domestic) ||
          PRESET_PG_AREA_ALL == pg_area )))
        {
          memset(&tp, 0, sizeof(dvbs_tp_node_t));
          tp.freq = rawtp.frq ;
          tp.sym = rawtp.sym ;
          tp.polarity = rawtp.pol ;
          tp.sat_id = cur_sat_id ;

          for(i = 0 ; i < tp_num ; i++)
          {
            p_cur_node = tp_node+i;
            if(prenode.p_tp->frq == p_cur_node->frq 
              && prenode.p_tp->sym == p_cur_node->sym 
              && prenode.p_tp->pol == p_cur_node->pol )
            {
              //find duplicate tp
              tp_flag = TRUE;
              cur_tp_id = p_cur_node->tp_id;
              break;
            }
          }

          if(tp_flag == FALSE)
          {  
            p_cur_node = tp_node+tp_num;
            memcpy(tp_node+tp_num,prenode.p_tp,sizeof(preset_tp_node_t));
            db_dvbs_add_tp(vv_id_tp,&tp) ;
            cur_tp_id = tp.id; 
            p_cur_node->tp_id = tp.id;            
            tp_num++;
          }
          tp_flag = FALSE;
 
          if((rawpro.audio_pid1 !=0 && rawpro.audio_pid1!= 8191)
            || (rawpro.video_pid != 0 && rawpro.video_pid != 8191))
          {
            memset(&pg,0,sizeof(dvbs_prog_node_t));
            pg.audio_channel = rawpro.audio_channel ;
            if ((pg.audio[0].p_id = (u16)rawpro.audio_pid1) != 8191)
            {
              pg.audio_ch_num++;
            }
            if ((pg.audio[1].p_id = (u16)rawpro.audio_pid2) != 8191)
            {
              pg.audio_ch_num++;
            }
            if ((pg.audio[2].p_id = (u16)rawpro.audio_pid3) != 8191)
            {
              pg.audio_ch_num++;
            }
            pg.pcr_pid= rawpro.pcr_pid;
            pg.sat_id = cur_sat_id;
            pg.tp_id = cur_tp_id;
            pg.video_pid = rawpro.video_pid ;
            pg.s_id = rawpro.s_id;
            pg.volume = DEFAULT_VOLUME_STRONG;

            dvb_to_unicode(rawpro.service_name,MAX_NAME_LENGTH,
              pg.name,DB_DVBS_MAX_NAME_LENGTH);
            db_dvbs_add_program(vv_id_pro, &pg) ;      
          }
        }
        break;
#endif
      default:
        break;
      }
      memset(prenode.p_pro, 0, sizeof(preset_pro_node_t));
      memset(prenode.p_sat, 0, sizeof(preset_sat_node_t));
    }

  }

  mtos_free(tp_node);
  mtos_free(p_buf);
  
  db_dvbs_save(vv_id_sat);
  db_dvbs_save(vv_id_tp);
  db_dvbs_save(vv_id_pro);

}


