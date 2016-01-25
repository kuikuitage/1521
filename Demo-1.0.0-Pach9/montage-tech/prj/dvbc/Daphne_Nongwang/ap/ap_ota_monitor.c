#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "ap_framework.h"
#include "ap_ota_monitor.h"

#include "dvb_protocol.h"
#include "mdl.h"
#include "drv_dev.h"
#include "nim.h"
#include "service.h"
#include "class_factory.h"
#include "nim_ctrl_svc.h"
#include "dvb_svc.h"
#include "nim_ctrl.h"
#include "dsmcc.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"

#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "db_dvbs.h"
#include "lib_util.h"
#include "ap_ota.h"

#ifdef OTA_DM_ON_STATIC_FLASH
#include "sys_data_staic.h"
#else
#include "sys_status.h"
#endif
#include "sys_cfg.h"
#include "data_manager.h"
//#include "config_customer.h"
#include "smc_op.h"
#include "uio.h"
#include "lpower.h"
#include "config_prj.h"
#include "customer_def.h"

typedef struct
{
  ota_mon_sm_t sm;
  service_t* p_dvb_svc;
  ota_tdi_t ota_tdi;
  u16 orig_software_version;
  u32 monitor_start_ticks;
#ifdef OTA_DM_ON_STATIC_FLASH
  ota_info_t ota_info;
  dvbc_ota_tp_t ota_tp_info;
#endif
}ota_monitor_priv_t;

typedef struct
{
  app_t instance;
  ota_monitor_priv_t priv;
}ota_monitor_handle_t;

static void init(handle_t p_handle) 
{
  s32 ret;
  misc_options_t misc;

  ota_monitor_handle_t* p_hdl = (ota_monitor_handle_t*)p_handle;
  dvb_t* p_dvb = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
OS_PRINTF("xuhd:2\n");

  p_hdl->priv.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_RESERVED1);

OS_PRINTF("xuhd:3\n");

  ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)(&misc));
  MT_ASSERT(ret != ERR_FAILURE);
OS_PRINTF("xuhd:4\n");
OS_PRINTF("@@@@@@@@@@@@@@@oui = 0x%x\n", misc.ota_tdi.oui);
OS_PRINTF("@@@@@@@@@@@@@@@hw mod id = 0x%x\n", misc.ota_tdi.hw_mod_id);
OS_PRINTF("@@@@@@@@@@@@@@@sw mod id = 0x%x\n", misc.ota_tdi.sw_mod_id);
OS_PRINTF("@@@@@@@@@@@@@@@hw version = 0x%x\n", misc.ota_tdi.hw_version);
OS_PRINTF("@@@@@@@@@@@@@@@reserved = 0x%x\n", misc.ota_tdi.reserved);
OS_PRINTF("@@@@@@@@@@@@@@@manufacture_id = 0x%x\n", misc.ota_tdi.manufacture_id);

  memcpy(&p_hdl->priv.ota_tdi, &misc.ota_tdi, sizeof(ota_tdi_t));
OS_PRINTF("xuhd:5\n");

#ifdef OTA_DM_ON_STATIC_FLASH
  sys_static_read_ota_tp_info(&p_hdl->priv.ota_tp_info); 
  sys_static_read_ota_info(&p_hdl->priv.ota_info);
  p_hdl->priv.orig_software_version = p_hdl->priv.ota_info.orig_software_version;
#else
  p_hdl->priv.orig_software_version = sys_status_get_sw_version();
#endif
OS_PRINTF("xuhd:6\n");




}

static void ota_request_dsi_single(service_t *p_dvb_svc, u16 pid)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_DSMCC_MSG;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = (u32)((pid << 16) | DSMCC_MSG_DSI);
  dvb_req.para2 = 0;
  dvb_req.context = (u32)((pid << 16) | DSMCC_MSG_DSI);
  p_dvb_svc->do_cmd(p_dvb_svc,
    DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
}

static void send_msg_to_ui(u32 id, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.id = id;
  evt.data1 = para1;
  evt.data2 = para2;
  ap_frm_send_evt_to_ui(APP_RESERVED1, &evt);
}

#define revert_endian_16(x) (((x&0xff)<<8)|(x>>8))

#define revert_endian_32(x) \
           (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))

static void task_single_step(void* p_handle, os_msg_t* p_msg)
{
  ota_monitor_handle_t* p_hdl = (ota_monitor_handle_t*)p_handle;
  ota_monitor_priv_t* p_priv = &(p_hdl->priv);
  service_t* p_nim_svc = NULL;
  nc_channel_info_t nc_info= {0};
  
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  dsmcc_dsi_info_t *p_dsi = NULL;
  static nc_channel_info_t nc_channel_info = {0};
  static ota_info_t ota_info = {0};
  
  u32 ticks = 0;
  u16 new_ver = 0, i = 0;
 #ifndef OTA_DM_ON_STATIC_FLASH
  dvbc_lock_t upgrade_tp = {0};
  dvbc_lock_t main_tp = {0};
  u16 pid = 0;
  #endif
  p_nim_svc  = nc_get_svc_instance(nc_handle, APP_RESERVED1);  
  
  if(p_msg != NULL)
  {
    switch( p_msg->content )
    {
      case OTA_MON_CMD_MONITOR_START:
        OS_PRINTF("xuhd:ota monitor start\n");
        p_priv->sm = OTA_MON_SM_CHECK_MAIN_FREQ1;
        break;

      case DVB_DSMCC_DSI_FOUND:
        OS_PRINTF("ota monitor dsi found\n");
        if( OTA_MON_SM_MAIN_FREQ_WAIT_DSI1 == p_priv->sm
          || OTA_MON_SM_MAIN_FREQ_WAIT_DSI2 == p_priv->sm)
        {
          p_dsi = (dsmcc_dsi_info_t *)p_msg->para1;

          OS_PRINTF("group_num = %d\n", p_dsi->group_num);

          for(i = 0; i < p_dsi->group_num; i++)
          {
            OS_PRINTF("local oui:%d, svc oui:%d\n",
            p_dsi->group_info[i].oui, revert_endian_32(p_priv->ota_tdi.oui));
            OS_PRINTF("local hw id:%d, svc hw id:%d\n",
            p_dsi->group_info[i].hw_mod_id, revert_endian_16(p_priv->ota_tdi.hw_mod_id));
            OS_PRINTF("local hw ver:%d, svc hw ver:%d\n",
            p_dsi->group_info[i].hw_version, revert_endian_16(p_priv->ota_tdi.hw_version));
            OS_PRINTF("local sw id:%d, svc sw id:%d\n",
            p_dsi->group_info[i].sw_mod_id, revert_endian_16(p_priv->ota_tdi.sw_mod_id));

            if((p_dsi->is_force == 1)
                && (p_dsi->group_info[i].hw_version == revert_endian_16(p_priv->ota_tdi.hw_version)))
              {
                 break;
              }
            else if(p_dsi->group_info[i].oui == revert_endian_32(p_priv->ota_tdi.oui)
            && p_dsi->group_info[i].hw_mod_id == revert_endian_16(p_priv->ota_tdi.hw_mod_id)
            && p_dsi->group_info[i].hw_version == revert_endian_16(p_priv->ota_tdi.hw_version)
            && p_dsi->group_info[i].sw_mod_id == revert_endian_16(p_priv->ota_tdi.sw_mod_id)
            && (p_dsi->is_force == 0))
            {
              break;
            }
          }

          if(i >= p_dsi->group_num)
          {
            if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI1 == p_priv->sm)
            {
              p_priv->sm = OTA_MON_SM_CHECK_MAIN_FREQ2;
            }
            else if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI2 == p_priv->sm)
            {
              send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
              p_priv->sm = OTA_MON_SM_IDLE;
            }
            break;
          }

          new_ver = p_dsi->group_info[0].sw_version;
          OS_PRINTF("software version, local:%d, new:%d\n",
          p_hdl->priv.orig_software_version, new_ver);

          if( p_hdl->priv.orig_software_version >= new_ver )
          {
            if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI1 == p_priv->sm)
            {
              p_priv->sm = OTA_MON_SM_CHECK_MAIN_FREQ2;
            }
            else if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI2 == p_priv->sm)
            {
              send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
              p_priv->sm = OTA_MON_SM_IDLE;
            }            
            break;
          }

          nc_get_tp(nc_handle, &nc_channel_info);

          memset(&ota_info, 0x0, sizeof(ota_info));
          
          if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI1 == p_priv->sm)
          {
            #ifdef OTA_DM_ON_STATIC_FLASH 
            ota_info.download_data_pid = p_priv->ota_tp_info.upg_tp.pid;
            #else
            sys_status_get_upgrade_tp(&upgrade_tp);
            //ota_info.download_data_pid = OTA_MONITOR_PID1;  
            ota_info.download_data_pid = (u16)((upgrade_tp.reserve1 << 8) | upgrade_tp.reserve2);
           #endif
          }
          else if(OTA_MON_SM_MAIN_FREQ_WAIT_DSI2 == p_priv->sm)
          {
             #ifdef OTA_DM_ON_STATIC_FLASH
              ota_info.download_data_pid = p_priv->ota_tp_info.main_tp.pid;
             #else
              ota_info.download_data_pid = OTA_MONITOR_PID2;
             #endif
          } 

          
          ota_info.ota_tri = OTA_TRI_AUTO;//tmp, it should be OTA_TRI_MONITOR
          ota_info.sys_mode = SYS_DVBC;
          ota_info.lockc.tp_freq = nc_channel_info.channel_info.frequency;
          ota_info.lockc.tp_sym = nc_channel_info.channel_info.param.dvbc.symbol_rate;

          #ifdef OTA_DM_ON_STATIC_FLASH
          ota_info.lockc.nim_modulate= nc_channel_info.channel_info.param.dvbc.modulation;
          ota_info.orig_software_version = p_hdl->priv.orig_software_version;
          ota_info.new_software_version = new_ver;
          //ota_info.lockc.nim_modulate = NIM_MODULA_QAM64;

          sys_static_write_ota_info(&ota_info);
          sys_static_write_into_ota_times(0);
          #else
          ota_info.lockc.nim_modulate = NIM_MODULA_QAM64;
          sys_status_set_ota_info(&ota_info);
          sys_status_save();
          #endif
          
          OS_PRINTF("xuhd:freq = %d\n", ota_info.locks.tp_rcv.freq);
          OS_PRINTF("xuhd:sym = %d\n", ota_info.locks.tp_rcv.sym);
          OS_PRINTF("xuhd:tri = %d\n", ota_info.ota_tri);
          OS_PRINTF("xuhd:pid = %d\n", ota_info.download_data_pid);

          if((OTA_MON_SM_MAIN_FREQ_WAIT_DSI1 == p_priv->sm)
            || (OTA_MON_SM_MAIN_FREQ_WAIT_DSI2 == p_priv->sm))
          {
            send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_FORCE_OTA, 0, 0);
          }

          p_priv->sm = OTA_MON_SM_IDLE;
        }
        break;

      case DVB_TABLE_TIMED_OUT:
        OS_PRINTF("dsi timeout\n");
        break;

      case NC_EVT_LOCKED:
        if( OTA_MON_SM_WAIT_LOCK1 == p_priv->sm )
        {
          #ifdef OTA_DM_ON_STATIC_FLASH
          ota_request_dsi_single(p_hdl->priv.p_dvb_svc, p_priv->ota_tp_info.upg_tp.pid);
          #else
          sys_status_get_upgrade_tp(&upgrade_tp);
          //ota_request_dsi_single(p_hdl->priv.p_dvb_svc, OTA_MONITOR_PID1);
          pid = (u16)((upgrade_tp.reserve1 << 8) | upgrade_tp.reserve2);
          OS_PRINTF("main freq 1 locked tp:%d sym =%d modulate=%d pid=%d\n",
                                   upgrade_tp.tp_freq,upgrade_tp.tp_sym,upgrade_tp.nim_modulate,pid);
          ota_request_dsi_single(p_hdl->priv.p_dvb_svc, pid);
          #endif
          p_priv->monitor_start_ticks = mtos_ticks_get();
          p_priv->sm = OTA_MON_SM_MAIN_FREQ_WAIT_DSI1;
        }
        else if(OTA_MON_SM_WAIT_LOCK2 == p_priv->sm)
        {
          OS_PRINTF("main freq 2 locked\n");
          #ifdef OTA_DM_ON_STATIC_FLASH
          ota_request_dsi_single(p_hdl->priv.p_dvb_svc, p_priv->ota_tp_info.main_tp.pid);
          #else
          ota_request_dsi_single(p_hdl->priv.p_dvb_svc, OTA_MONITOR_PID2);
          #endif
          p_priv->monitor_start_ticks = mtos_ticks_get();
          p_priv->sm = OTA_MON_SM_MAIN_FREQ_WAIT_DSI2;
        }
        break;

      case  NC_EVT_UNLOCKED:
        if( OTA_MON_SM_WAIT_LOCK1 == p_priv->sm )
        {
          OS_PRINTF("xuhd: main freq 1 unlock!\n");
#ifdef OTA_DM_ON_STATIC_FLASH
             if((p_priv->ota_tp_info.upg_tp.tp_freq == p_priv->ota_tp_info.main_tp.tp_freq) 
            && (p_priv->ota_tp_info.upg_tp.tp_sym == p_priv->ota_tp_info.main_tp.tp_sym)
            && (p_priv->ota_tp_info.upg_tp.nim_modulate == p_priv->ota_tp_info.main_tp.nim_modulate)
            && (p_priv->ota_tp_info.upg_tp.pid==p_priv->ota_tp_info.main_tp.pid))
#else
          sys_status_get_main_tp1(&main_tp);
          sys_status_get_upgrade_tp(&upgrade_tp); 
          if((main_tp.tp_freq == upgrade_tp.tp_freq) 
            && (main_tp.tp_sym == upgrade_tp.tp_sym)
            && (main_tp.nim_modulate == upgrade_tp.nim_modulate)
            && (OTA_MONITOR_PID2 == ((upgrade_tp.reserve1 << 8) | upgrade_tp.reserve2)))
#endif
          
            {
              OS_PRINTF("two ota freq is same,it will timeout\n");
              p_priv->sm = OTA_MON_SM_IDLE;        
              send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
            }
          else
            {
                p_priv->sm = OTA_MON_SM_CHECK_MAIN_FREQ2;
            }
          
        }
        else if(OTA_MON_SM_WAIT_LOCK2 == p_priv->sm)
        {
          OS_PRINTF("xuhd: main freq 2 unlock!\n");
          p_priv->sm = OTA_MON_SM_IDLE;        
          send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
        }
        break;
      default:
        break;
    }
  }
  else
  {
    switch(p_priv->sm)
    {
      case OTA_MON_SM_IDLE:
        break;

      case OTA_MON_SM_CHECK_MAIN_FREQ1: /***upg system set tp***/
#ifdef OTA_DM_ON_STATIC_FLASH
        nc_info.channel_info.frequency = p_priv->ota_tp_info.upg_tp.tp_freq;
        nc_info.channel_info.param.dvbc.symbol_rate = p_priv->ota_tp_info.upg_tp.tp_sym;
        nc_info.channel_info.param.dvbc.modulation = p_priv->ota_tp_info.upg_tp.nim_modulate;
        
        nc_info.polarization = NIM_PORLAR_HORIZONTAL;  
        nc_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
        nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;  
        OS_PRINTF("check ota1 freq: %d, sym:%d,modulate:%d\n",
                 nc_info.channel_info.frequency ,  
                nc_info.channel_info.param.dvbc.symbol_rate, 
                nc_info.channel_info.param.dvbc.modulation);
#else    
        sys_status_get_upgrade_tp(&upgrade_tp);
        nc_info.channel_info.frequency = upgrade_tp.tp_freq;
        nc_info.channel_info.param.dvbc.symbol_rate = upgrade_tp.tp_sym;
        nc_info.channel_info.param.dvbc.modulation = upgrade_tp.nim_modulate;
        OS_PRINTF("check upgrade freq: %d, sym:%d,modulate:%d\n",
                  upgrade_tp.tp_freq, upgrade_tp.tp_sym, upgrade_tp.nim_modulate);
        nc_info.polarization = NIM_PORLAR_HORIZONTAL;  
        nc_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
        nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK; 
#endif
        
        nc_set_tp(nc_handle,p_nim_svc, &nc_info);
        p_priv->sm = OTA_MON_SM_WAIT_LOCK1;
        break;

      case OTA_MON_SM_CHECK_MAIN_FREQ2: /***main tp set tp***/
 #ifdef OTA_DM_ON_STATIC_FLASH
        nc_info.channel_info.frequency = p_priv->ota_tp_info.main_tp.tp_freq;
        nc_info.channel_info.param.dvbc.symbol_rate = p_priv->ota_tp_info.main_tp.tp_sym;
        nc_info.channel_info.param.dvbc.modulation = p_priv->ota_tp_info.main_tp.nim_modulate;
        nc_info.polarization = NIM_PORLAR_HORIZONTAL;  
        nc_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
        nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;  
        OS_PRINTF("check ota2 freq: %d, sym:%d,modulate:%d\n",
                 nc_info.channel_info.frequency , 
                nc_info.channel_info.param.dvbc.symbol_rate, 
                nc_info.channel_info.param.dvbc.modulation);
  #else
        sys_status_get_main_tp1(&main_tp);
        nc_info.channel_info.frequency = main_tp.tp_freq;//OTA_MONITOR_DEF_FREQ2;
        nc_info.channel_info.param.dvbc.symbol_rate = main_tp.tp_sym;//OTA_MONITOR_DEF_SYM2;
        nc_info.channel_info.param.dvbc.modulation = main_tp.nim_modulate;//NIM_MODULA_QAM64;
        nc_info.polarization = NIM_PORLAR_HORIZONTAL;  
        nc_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
        nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK; 
       OS_PRINTF("check maintp freq: %d, sym:%d,modulate:%d\n",
                              main_tp.tp_freq, main_tp.tp_sym, main_tp.nim_modulate);

#endif
       
        nc_set_tp(nc_handle,p_nim_svc, &nc_info);
        p_priv->sm = OTA_MON_SM_WAIT_LOCK2;
        
        break;

      case OTA_MON_SM_WAIT_LOCK1:
      case OTA_MON_SM_WAIT_LOCK2:
        break;

      case OTA_MON_SM_MAIN_FREQ_WAIT_DSI1:
        ticks = mtos_ticks_get();
        if( (ticks - p_priv->monitor_start_ticks) > OTA_MONITOR_DSI_TMOUT)
        {
          OS_PRINTF("dsi 1 timeout\n");

#ifdef OTA_DM_ON_STATIC_FLASH
     if((p_priv->ota_tp_info.upg_tp.tp_freq == p_priv->ota_tp_info.main_tp.tp_freq) 
            && (p_priv->ota_tp_info.upg_tp.tp_sym == p_priv->ota_tp_info.main_tp.tp_sym)
            && (p_priv->ota_tp_info.upg_tp.nim_modulate == p_priv->ota_tp_info.main_tp.nim_modulate)
            && (p_priv->ota_tp_info.upg_tp.pid==p_priv->ota_tp_info.main_tp.pid))
#else          
          sys_status_get_main_tp1(&main_tp);
          sys_status_get_upgrade_tp(&upgrade_tp);
          if((main_tp.tp_freq == upgrade_tp.tp_freq) 
            && (main_tp.tp_sym == upgrade_tp.tp_sym)
            && (main_tp.nim_modulate == upgrade_tp.nim_modulate)
            && (OTA_MONITOR_PID2 == ((upgrade_tp.reserve1 << 8) | upgrade_tp.reserve2)))
#endif

          {
              OS_PRINTF("two ota freq is same,it will timeout\n");
              p_priv->sm = OTA_MON_SM_IDLE;        
              send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
            }
          else
            {
                p_priv->sm = OTA_MON_SM_CHECK_MAIN_FREQ2;
            }
        }
        break;

      case OTA_MON_SM_MAIN_FREQ_WAIT_DSI2:
        ticks = mtos_ticks_get();
        if((ticks - p_priv->monitor_start_ticks) > OTA_MONITOR_DSI_TMOUT)
        {
          OS_PRINTF("dsi 2 timeout\n");
          send_msg_to_ui(OTA_MON_EVT_MAIN_FREQ_TMOUT, 0, 0);
         p_priv->sm = OTA_MON_SM_IDLE;
        }
        break;
    }
  }
}


app_t* construct_ap_ota_monitor() 
{
  ota_monitor_handle_t *p_handle = mtos_malloc(sizeof(ota_monitor_handle_t));
  MT_ASSERT(p_handle != NULL);

  memset(p_handle, 0, sizeof(ota_monitor_handle_t));

  p_handle->instance.p_data = p_handle;
  p_handle->instance.init = init;
  p_handle->instance.task_single_step = task_single_step;

  return &p_handle->instance;
}

