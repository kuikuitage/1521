/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "lib_util.h"

#include "string.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "hal_dma.h"


#include "common.h"
#include "drv_dev.h"
#include "lib_rect.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "avsync.h"
#include "drv_misc.h"


#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"
#include "subtitle_vsb.h"
#include "vbi_vsb.h"
#include "data_manager.h"
#include "data_base.h"
#include "db_dvbs.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "nit.h"
#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_i.h"
#include "ss_ctrl.h"
#include "ap_ota.h"
#include "sys_status.h"
#include "customer_def.h"

typedef struct
{
  u32 cur_pg_id;
  u32 s_nit_ver;
} pb_data_t;
update_t up_info = {0};

#define revert_32(x)  \
           (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))

extern void stc_set_pg_id_1(handle_t handle, u16 pg_id);
static void _play(void *p_data, play_param_t *p_play_param)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
  p_pb_data->cur_pg_id = p_play_param->pg_info.context1;
  stc_set_pg_id_1(handle, (u16)p_play_param->pg_info.context1);
}

static BOOL _process_pmt_info(void *p_data, pmt_t *p_pmt, 
              u16 *p_v_pid, u16 *p_pcr_pid, u16 *p_a_pid, u16 *p_a_type)
{

  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  BOOL data_changed = FALSE;
  BOOL audio_pid_changed = FALSE;
  u16 audio_count = 0;
  u16 i = 0;
  u16 old_a_pid = 0;
  u16 old_a_type = 0;
    
  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    return FALSE;
  }

  //param check
  if(p_pmt->audio_count > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    audio_count = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  else
  {
    audio_count = p_pmt->audio_count;
  }
  
  //video pid
  #if defined (FANTONG_KAIFENG) || defined (ZHILING_KAIFENG)
  
   if(pg.tv_flag == 0)
  {
    *p_v_pid = pg.video_pid ;
  }
 #else
  *p_v_pid = p_pmt->video_pid;
   if(pg.video_pid != p_pmt->video_pid)
   {
     pg.video_pid = p_pmt->video_pid;
     data_changed = TRUE;
   }
 #endif

  //prc pid
  *p_pcr_pid = p_pmt->pcr_pid;
  if(pg.pcr_pid != p_pmt->pcr_pid)
  {
    pg.pcr_pid = p_pmt->pcr_pid;
    data_changed = TRUE;
  }
  
  //audio pid
  old_a_pid = pg.audio[pg.audio_channel].p_id;
  old_a_type = pg.audio[pg.audio_channel].type;
  
  for(i = 0; i < audio_count; i++)
  {
    if((pg.audio[i].p_id != p_pmt->audio[i].p_id)
      || (pg.audio[i].type != p_pmt->audio[i].type))
    {
      audio_pid_changed = TRUE;
      break;
    }
  }

  if(pg.audio_ch_num != audio_count || audio_pid_changed)
  {
    for(i = 0; i < audio_count; i++)
    {
      pg.audio[i] = p_pmt->audio[i];
    }
    pg.audio_ch_num = audio_count;
    pg.audio_channel = 0;
    audio_pid_changed = TRUE;
    data_changed = TRUE;
  }

  if(audio_pid_changed)
  {
    BOOL has_sim_pid = FALSE;
    
    //find cur audio pid
    for(i = 0; i < audio_count; i++)
    {
      if((old_a_pid == pg.audio[i].p_id)
        && (old_a_type == pg.audio[i].type))
      {
        has_sim_pid = TRUE;
        pg.audio_channel = i;
        break;
      }
    }

    //find a new pid
    if(!has_sim_pid)
    {
      pg.audio_channel = get_audio_channel(&pg);
      MT_ASSERT(pg.audio_channel < pg.audio_ch_num);
    }
  }
  
  *p_a_pid = pg.audio[pg.audio_channel].p_id;
  *p_a_type = pg.audio[pg.audio_channel].type;

  //save new db info
  if(data_changed)
  {
    OS_PRINTF("data changed\n");
    db_dvbs_edit_program(&pg);
    return TRUE;
  }
 else
  {
    return FALSE;
  }
  
}

#if 1
/*!
  send pb's evt out.

  \param[in] evt_t current evt
  \param[in] data1 parameter 1
  \param[in] data2 parameter 2
  */
static void send_evt_to_ui(pb_evt_t evt_t, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = evt_t;
  evt.data1 = (u32)data1;
  evt.data2 = (u32)data2;
  ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
}
#endif
static void parse_nit_for_upgrade(u8 *p_buf)
{
  s16 network_desc_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  u16 offset = 0;

  u16 nxt_ts_start = 0;

  u8  link_desc_len = 0;
  u8  linkage_type = 0;
  u8  OUI_data_length = 0;
  u8  OUI_data_offset = 0;
  u8  selector_length = 0;
  u8  selector_offset = 0;
  u32 OUI = 0;
  u8 *p_buf_temp = NULL;
  u32 temp = 0;
  event_t evt = {0};

  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    OS_PRINTF("MDL: not nit\n");
    return;
  }

  if((p_buf[5] & 0x01) == 0)
  {
    OS_PRINTF("MDL: this nit is not usable\n");
    return;
  }
  
  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  offset = 10;

  offset += network_desc_len;

  ts_loop_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
  
  offset += 2;

  while(ts_loop_len > 0)
  {
    //OS_PRINTF("ts_loop_len[%d]\n", ts_loop_len);

    /*! ts id*/

    offset += 4;

    desc_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
    /*! Skip offset length*/
    offset += 2;
    
    ts_loop_len -= (desc_len + 6);
    /*! Saving next start position for next ts id*/
    nxt_ts_start = offset + desc_len; 

    while(desc_len > 0)
    {
      //OS_PRINTF("desc_len[%d]\n", desc_len);
      switch(p_buf[offset])
      {           
          case DVB_DESC_LINKAGE:
            //Skip tag
            offset ++;
            desc_len -- ;

            link_desc_len = (u8)p_buf[offset];
            if(link_desc_len>=7)
            {
              linkage_type = p_buf[offset+7];
              switch(linkage_type)
              {
                case 0x09:
                  p_buf_temp = &p_buf[offset+8];
                  OUI_data_length = p_buf_temp[0];
                  p_buf_temp++;
                  while(OUI_data_length>0)
                  {                                        
                    OUI = (p_buf_temp[0]<<16 | p_buf_temp[1]<<8 | p_buf_temp[2]);
                    up_info.oui = OUI;

                    OUI_data_length -= 3;
                    OUI_data_offset += 3;

                    p_buf_temp += 3;

                    selector_length = p_buf_temp[0];
                    OUI_data_length--;
                    OUI_data_offset++;

                    p_buf_temp++;

                    OUI_data_length -= selector_length;
                    OUI_data_offset += selector_length;

                    while(selector_length>0)
                    {
                      //Cable_delivery_desc
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;
                      
                      //DSysDes_Length
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Freq_desc
                      up_info.freq= make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Fec_outer
                      selector_offset++;
                      p_buf_temp++;

                      //QAM_Mode
                      up_info.qam_mode = (u8)p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Symbolrate_desc
                      up_info.symbol= make16(&p_buf_temp[0]);
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Fec_inner
                      selector_offset++;
                      p_buf_temp++;

                      //Download_Pid
                      up_info.data_pid = make16(&p_buf_temp[0]);
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Update_type
                      up_info.ota_type = (u8)p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Private_data_length
                      selector_offset++;
                      p_buf_temp++;

                      //Hardware_version
                      up_info.hwVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Software_type
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Software_version
                      up_info.swVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Serial_number_start
                      memcpy(up_info.Serial_number_start, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Serial_number_end
                      memcpy(up_info.Serial_number_end, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Private_data
                      selector_offset += 4;
                      p_buf_temp += 4;

                      selector_length -= selector_offset;
                      #if 1
                      OS_PRINTF("up_info.oui=%d\n", up_info.oui);
                      OS_PRINTF("up_info.freq=%d\n", up_info.freq);
                      OS_PRINTF("up_info.symbol=%d\n", up_info.symbol);
                      OS_PRINTF("up_info.qam_mode=%d\n", up_info.qam_mode);
                      OS_PRINTF("up_info.data_pid=%d\n", up_info.data_pid);
                      OS_PRINTF("up_info.ota_type=%d\n", up_info.ota_type);
                      OS_PRINTF("up_info.Serial_number_start=%s\n", up_info.Serial_number_start);
                      OS_PRINTF("up_info.Serial_number_end=%s\n", up_info.Serial_number_end);
                      OS_PRINTF("up_info.swVersion=%d\n", up_info.swVersion);
                      OS_PRINTF("up_info.hwVersion=%d\n", up_info.hwVersion);
                      #endif

                      {
                        s32 ret;
                        u32 stb_sw_ver = sys_status_get_sw_version();
                        char misc_info[200];
                        misc_options_t *misc;
                        void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
                        
                        ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)misc_info);
                        MT_ASSERT(ret != ERR_FAILURE);
                        misc = (misc_options_t *)misc_info;
                        misc->ota_tdi.oui = revert_32(misc->ota_tdi.oui);
                        //memcpy((ota_tdi_t *)p_tdi, (u8 *)&misc->ota_tdi, sizeof(ota_tdi_t));

                        #if 1
                        OS_PRINTF("misc->ota_tdi.oui=%d\n", misc->ota_tdi.oui);
                        OS_PRINTF("misc->ota_tdi.hw_mod_id=%d\n", misc->ota_tdi.hw_mod_id);
                        OS_PRINTF("misc->ota_tdi.hw_version=%d\n", misc->ota_tdi.hw_version);
                        OS_PRINTF("stb_sw_ver=%d\n", stb_sw_ver);
                        #endif

                        if(misc->ota_tdi.oui == up_info.oui
                          && misc->ota_tdi.hw_version == up_info.hwVersion
                          && stb_sw_ver < up_info.swVersion)
                        {
                          evt.id = PB_EVT_NIT_OTA_UPGRADE;
                          evt.data1 = (u32)(&up_info);
                          evt.data2 = 0;
                          ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
                        }
                      }
                    }
                  }
                  break;

                default:
                  break;
              }
            }

            desc_len -= link_desc_len;
            offset   += link_desc_len;   

            offset ++;
            desc_len -- ;
            break;
          
          default:
          /*! 
            Jump to unknown desc length
           */
          offset ++;
          desc_len --;

          //OS_PRINTF("MDL: parse nit tmp_desc_length %d\n",p_buf[offset]);
          //OS_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          //Skip content of unkown descriptor
          //if(desc_len > tmp_desc_length)
          {
            desc_len -=  (s16)p_buf[offset];
            offset   +=  p_buf[offset];   
          }

          //OS_PRINTF("MDL: parse nit offset %d\n",  offset);
          //OS_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          offset ++;
          desc_len -- ;
          break;
        }

    }
    
    offset = nxt_ts_start ;
  }
  
  return;  
}
static BOOL _process_nit_info(void *p_data, nit_t *p_nit )
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  customer_cfg_t cfg = {0};
  dvbc_lock_t main_tp = {0};
  nc_channel_info_t tp_info = {0};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
	
  get_customer_config(&cfg);
    
  if(p_pb_data->s_nit_ver != p_nit->version_num)
  {
    if(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312) 
    {
      nc_get_tp(nc_handle, &tp_info);
      sys_status_get_main_tp1(&main_tp);
      if((main_tp.tp_freq == tp_info.channel_info.frequency) &&
          (main_tp.tp_sym == tp_info.channel_info.param.dvbc.symbol_rate) &&
          (main_tp.nim_modulate == tp_info.channel_info.param.dvbc.modulation))
        {
            p_pb_data->s_nit_ver = p_nit->version_num;
            send_evt_to_ui(PB_EVT_NIT_VERSION_SWITCH, p_nit->network_id, p_pb_data->s_nit_ver);
            OS_PRINTF(" @@ _process_nit_info do search  line =%d\n",__LINE__);
         }
    }
    else
    {
       p_pb_data->s_nit_ver = p_nit->version_num;
       OS_PRINTF("PB DVB_NIT_FOUND version:%d\n",p_pb_data->s_nit_ver);
       if((cfg.customer == CUSTOMER_XINSHIDA) 
   		|| (cfg.customer == CUSTOMER_YINGJI) 
   		|| (cfg.customer == CUSTOMER_XINSIDA_LQ)
           || (cfg.customer == CUSTOMER_FANTONG)
           || (cfg.customer == CUSTOMER_FANTONG_KF)
           || (cfg.customer == CUSTOMER_FANTONG_KFAJX)
           || (cfg.customer == CUSTOMER_FANTONG_BYAJX)
           || (cfg.customer == CUSTOMER_FANTONG_XSMAJX)
           || (cfg.customer == CUSTOMER_XINSIDA_SPAISH)
           || (cfg.customer == CUSTOMER_HEBI))
      {
         send_evt_to_ui(PB_EVT_NIT_VERSION_SWITCH, p_nit->network_id, p_pb_data->s_nit_ver);
       }
     }
  }

  if((cfg.customer == CUSTOMER_XINSHIDA) || (cfg.customer == CUSTOMER_XINSIDA_LQ) || 
    (cfg.customer == CUSTOMER_XINSIDA_SPAISH) || (cfg.customer == CUSTOMER_HEBI))
 {
   if(p_data != NULL)
  {
    parse_nit_for_upgrade((u8*)p_nit->p_origion_data);
  }
 }
    return TRUE;
}

extern RET_CODE dvb_to_unicode(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen);
extern s32 uni_strcmp(const u16 *p_dst, const u16 *p_src);
extern void uni_strcpy(u16 *p_dst, const u16 *p_src);
static BOOL _process_sdt_info(void *p_data, sdt_t *p_sdt)
{
  BOOL bRet = FALSE;
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  event_t evt;
  u16 i = 0;
  OS_PRINTF("\n$$$SDT_REPAIRED!$$$\n");
  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    //ret = FALSE;
  }
  else
  {
    for(i=0; i<p_sdt->svc_count; i++)
    {
      if(p_sdt->svc_des[i].svc_id == pg.s_id)
      {
        dvb_to_unicode(p_sdt->svc_des[i].name, sizeof(p_sdt->svc_des[i].name), name, DB_DVBS_MAX_NAME_LENGTH);

        if(uni_strcmp(pg.name, name) != 0)
        {
#ifdef AUTO_FACTORY_DEFAULT
          if(get_factory_flag() == FALSE)
#endif
          {
            uni_strcpy(pg.name, name);
            OS_PRINTF("program name changed, name = %s\n", name);

            db_dvbs_edit_program(&pg);

            evt.id = PB_EVT_UPDATE_PG_NAME;
            evt.data1 = (u32)pg.id;
            evt.data2 = 0;
            ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);

            bRet = TRUE;
          }
        }

        break;
      }
    }
  }

  return bRet;
}

pb_policy_t *construct_pb_policy(void)
{
//  RET_CODE ret = ERR_FAILURE;
 // void *p_dvb_vbi = NULL;
 pb_data_t *p_pb_data = NULL;
  pb_policy_t *p_pb_pol = mtos_malloc(sizeof(pb_policy_t));

  MT_ASSERT(p_pb_pol != NULL);
  memset(p_pb_pol, 0, sizeof(pb_policy_t));

  //Alloc private data
  p_pb_pol->p_data = mtos_malloc(sizeof(pb_data_t));
  MT_ASSERT(p_pb_pol->p_data != NULL);
  memset(p_pb_pol->p_data, 0, sizeof(pb_data_t));
   p_pb_data = (pb_data_t *)p_pb_pol->p_data;
   p_pb_data ->s_nit_ver = 0xFFFFFFFF;

  p_pb_pol->on_play = _play;
  p_pb_pol->process_pmt_info = _process_pmt_info;
  p_pb_pol->process_nit_info = _process_nit_info;
  p_pb_pol->process_sdt_info = _process_sdt_info;
  #if 0
  ret = subt_init_vsb(MDL_SUBT_TASK_PRIORITY, MDL_SUBT_TASK_STKSIZE, DISP_LAYER_ID_SUBTITL);
  MT_ASSERT(ret == SUCCESS);
  ret = vbi_init_vsb(MDL_VBI_TASK_PRIORITY, MDL_VBI_TASK_STKSIZE);
  MT_ASSERT(ret == SUCCESS);
  p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);
  MT_ASSERT(p_dvb_vbi != NULL);
  //vbi_ttx_start(TELTEXT_MAX_PAGE_NUM);
  #endif

  return p_pb_pol;
}


void destruct_pb_policy(pb_policy_t *p_pb_policy)
{
  //Free private data
  mtos_free(p_pb_policy->p_data);

  //Free playback policy
  mtos_free(p_pb_policy);
}

