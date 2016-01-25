
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "unzip.h"

// driver
#include "common.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "sys_regs_jazz.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "vdec.h"
#include "display.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "avsync.h"


#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager32.h"
//#include "data_manager_v2.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "ts_packet.h"
#include "eit.h"
//#include "epg_data.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "avctrl1.h"

#include "db_dvbs.h"
#include "mem_cfg.h"

#include "sys_status.h"

#ifdef OTA_DM_ON_STATIC_FLASH
#include "sys_data_staic.h"
#endif

#include "mt_time.h"
#include "customer_def.h"
#include "sys_dbg.h"

#include "config_cas.h"
//#include "sowell.h"
//#include "ucas.h"
//#include "ucas_key.h"

/*!
   which block store sysstatus data
 */
#define SS_VAULE_BLOCK_ID       (IW_VIEW_BLOCK_ID)

/*!
   which block store sysstatus default data
 */
#define SS_DEFAULT_BLOCK_ID    (SS_DATA_BLOCK_ID)

/*!
   language code list
 */
static char * g_lang_code_b[] =
{
  "eng",
  "ara",
  "per",
  "fre",
  "ger",
  "ita",
  "spa",
  "por",
  "pol",
  "rus",
  "tur",
};


/*!
   language code list
 */
static char * g_lang_code_t[] =
{
  "eng",
  "ara",
  "fas",
  "fra",
  "deu",
  "ita",
  "spa",
  "por",
  "pol",
  "rus",
  "tur",
};

/************************************************************************
* NOTICE:
*   1. curn_group must be set as zero when initialize.
*   2. sys_status_check_group will cause recreate prog views.
*
************************************************************************/

static sys_status_t g_status;

extern int uncompress(char *dest,
                        unsigned long *destLen,
                        const char *source,
                        unsigned long sourceLen);
#ifndef WIN32
extern void set_ota_config(void);
#endif
void  sys_printf_misc_data(void)
{
  u32 read_len = 0;
  u8 *p = NULL;
  misc_options_t misc;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);
  p = (u8 *)&misc;
  OS_PRINTF("misc addr[0x%04x]\n",p);
  OS_PRINTF("str_sw_version[0x%04x] ->%s \n", (u8 *)&misc.str_sw_version[0]-p,misc.str_sw_version);
  OS_PRINTF("str_hw_version[0x%04x] ->%s \n", (u8 *)&misc.str_hw_version[0]-p,misc.str_hw_version);
  OS_PRINTF("str_last_upgrade_date[0x%04x] ->%s \n", (u8 *)&misc.str_last_upgrade_date[0]-p,misc.str_last_upgrade_date);
  OS_PRINTF("str_0_gain[0x%04x] ->0x%x \n", (u8 *)&misc.str_0_gain-p,misc.str_0_gain);
  OS_PRINTF("ota_tdi.oui[0x%04x] ->0x%x \n", (u8 *)&misc.ota_tdi.oui-p,misc.ota_tdi.oui);
  OS_PRINTF("ota_tdi.hw_version[0x%04x] ->0x%x \n", (u8 *)&misc.ota_tdi.hw_version-p,misc.ota_tdi.hw_version);
  #ifdef OTA_DM_ON_STATIC_FLASH
  OS_PRINTF("ota_tdi.sw_version[0x%04x] ->0x%x \n", (u8 *)&misc.ota_tdi.sw_version-p,misc.ota_tdi.sw_version);

  #endif
}





void sys_status_init(void)
{
  static u8 is_initialized = FALSE;
  BOOL ret = TRUE;

  if (is_initialized == TRUE)
  {
    return;
  }
   sys_printf_misc_data();
  ret = ss_ctrl_init(SS_VAULE_BLOCK_ID, (u8*)&g_status, sizeof(sys_status_t));
  #ifdef OTA_DM_ON_STATIC_FLASH
  if (!ret)
  {
        OS_PRINTF("ss ctrl init first run\n");
        db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
        sys_status_load();
        sys_static_write_set_boot_status();
        sys_static_set_status_ota_init_info();
  }

{
      ota_info_t ota_info = {0} ;
      #ifndef WIN32
       set_ota_config();
      #endif
      sys_static_set_status_ota_upg_tp();
      sys_static_check_and_repair_data_block();
      sys_static_read_ota_info(&ota_info);
      sys_status_set_sw_version(ota_info.orig_software_version);
}
  #else
  if (!ret)
  {
     /************it will do at ota first run****************/
    sys_status_load();
    reset_boot_logo_flag();
  }
  else
  {
      if(get_boot_logo_flag() == 0xff) /**upg first boot***/
        {
            db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
            //set boot status
            sys_status_set_boot_status();
            sys_reset_adver_logo();
        }
  }

 #endif

  is_initialized = TRUE;

#if 0
  p = (u8*)(&g_status);
  OS_PRINTF("\n=========start=========");
  for(i=0; i<total; i++)
  {
    if(i%16==0)
    {
        OS_PRINTF("\n%06xh:",i);
    }
    OS_PRINTF("%02x ", p[i]);
  }
  OS_PRINTF("\n=========end=========");

  OS_PRINTF("normal = %04x\n", g_status.pwd_set.normal);
  OS_PRINTF("super = %04x\n", g_status.pwd_set.super);
  OS_PRINTF("enable_psw_poweron = %04x\n", g_status.pwd_set.enable_psw_poweron);
  OS_PRINTF("enable_psw_lockchannel = %04x\n", g_status.pwd_set.enable_psw_lockchannel);
  OS_PRINTF("reserve1 = %04x\n", g_status.pwd_set.reserve1);
  OS_PRINTF("reserve2 = %04x\n", g_status.pwd_set.reserve2);
  OS_PRINTF("psw_poweron = %04x\n", g_status.pwd_set.psw_poweron);
  OS_PRINTF("psw_lockchannel = %04x\n", g_status.pwd_set.psw_lockchannel);
  OS_PRINTF("main_tp1.tp_freq = %04x\n", g_status.main_tp1.tp_freq);
  OS_PRINTF("main_tp1.tp_sym = %04x\n", g_status.main_tp1.tp_sym);
  OS_PRINTF("main_tp1.nim_modulate = %04x\n", g_status.main_tp1.nim_modulate);
  OS_PRINTF("main_tp2.tp_freq = %04x\n", g_status.main_tp2.tp_freq);
  OS_PRINTF("main_tp2.tp_sym = %04x\n", g_status.main_tp2.tp_sym);
  OS_PRINTF("main_tp2.nim_modulate = %04x\n", g_status.main_tp2.nim_modulate);
  OS_PRINTF("nvod_tp.tp_freq = %04x\n", g_status.nvod_tp.tp_freq);
  OS_PRINTF("nvod_tp.tp_sym = %04x\n", g_status.nvod_tp.tp_sym);
  OS_PRINTF("nvod_tp.nim_modulate = %04x\n", g_status.nvod_tp.nim_modulate);
  OS_PRINTF("ota_info.ota_tri = %04x\n", g_status.ota_info.ota_tri);
  OS_PRINTF("ota_info.sys_mode = %04x\n", g_status.ota_info.sys_mode);
  OS_PRINTF("download_data_pid = %04x\n", g_status.ota_info.download_data_pid);
  OS_PRINTF("new_software_version = %04x\n", g_status.ota_info.new_software_version);
  OS_PRINTF("orig_software_version = %04x\n", g_status.ota_info.orig_software_version);
  OS_PRINTF("lockc.tp_freq = %04x\n", g_status.ota_info.lockc.tp_freq);
  OS_PRINTF("lockc.tp_sym = %04x\n", g_status.ota_info.lockc.tp_sym);
  OS_PRINTF("lockc.nim_modulate = %04x\n", g_status.ota_info.lockc.nim_modulate);
  OS_PRINTF("channel_version = %04x\n", g_status.channel_version);
#endif
}




void sys_status_load(void)
{
  int i =0;
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);

  dm_read(handle, SS_DEFAULT_BLOCK_ID, 0, 0,
  	sizeof(sys_status_t),
  	(u8*)(&g_status));

  sys_status_set_status(BS_LNB_POWER, TRUE);
  sys_status_set_status(BS_BEEPER, TRUE);
  sys_status_set_status(BS_ANTENNA_CONNECT, TRUE);

  for (i = 0; i < MAX_GROUP_CNT; i++)
  {
    g_status.group_set.group_info[i].curn_tv.id = INVALIDID;
    g_status.group_set.group_info[i].curn_tv.pos = INVALIDPOS;
    g_status.group_set.group_info[i].curn_radio.id = INVALIDID;
    g_status.group_set.group_info[i].curn_radio.pos = INVALIDPOS;
  }

  sys_status_save();  //need save
  ss_ctrl_set_checksum(class_get_handle_by_id(SC_CLASS_ID));
}

void sys_group_reset(void)
{
  int i =0;
  memset(&g_status.fav_set,0,sizeof(fav_set_t));
  memset(&g_status.book_info,0,sizeof(book_info_t));
  memset(&g_status.group_set,0,sizeof(group_set_t));

    for (i = 0; i < MAX_GROUP_CNT; i++)
    {
      g_status.group_set.group_info[i].curn_tv.id = INVALIDID;
      g_status.group_set.group_info[i].curn_tv.pos = INVALIDPOS;
      g_status.group_set.group_info[i].curn_radio.id = INVALIDID;
      g_status.group_set.group_info[i].curn_radio.pos = INVALIDPOS;
    }
    sys_status_save();  //need save
}


void sys_status_save(void)
{
  ss_ctrl_sync(class_get_handle_by_id(SC_CLASS_ID),
    (u8*)&g_status);
}

sys_status_t *sys_status_get(void)
{
#if 0
  OS_PRINTF("- SYS STATUS INFO - \n");
  OS_PRINTF("\t OSD PAL   = %d\n", g_status.osd_set.palette);
  OS_PRINTF("\t OSD TMOUT = %d\n", g_status.osd_set.timeout);
  OS_PRINTF("\t OSD TRANS = %d\n", g_status.osd_set.transparent);
  OS_PRINTF("\t LANGUAGE  = %d\n", g_status.lang_set.osd_text);
  OS_PRINTF("\t CURN MODE = %d\n", g_status.group_set.curn_mode);
  OS_PRINTF("\t CURN GROP = %d\n", g_status.group_set.curn_group);
  OS_PRINTF("\t TV MODE   = %d\n", g_status.av_set.tv_mode);
  OS_PRINTF("\t TV RATIO  = %d\n", g_status.av_set.tv_ratio);
#endif
  return &g_status;
}

u8 get_audio_global_volume(void)
{
  u8 volume = 0;
  sys_status_t *status;
  status = sys_status_get();
  volume = status->audio_set.global_volume;
  return volume;
}

void set_audio_global_volume(u8 volume)
{
  sys_status_t *status;
  status = sys_status_get();
  status->audio_set.global_volume = volume;
  sys_status_save();
}

u32 sys_status_get_sw_version(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->sw_version;
}

void sys_status_get_sw_changeset(u8 *changeset)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(changeset, status->changeset, sizeof(u8) * MAX_CHANGESET_LEN);
}

void sys_status_set_sw_version(u32 new_version)
{
  sys_status_t *status;

  status = sys_status_get();

  status->sw_version = new_version;

  sys_status_save();
}


void sys_status_add_book_node(u8 index, book_pg_t *node)
{
  sys_status_t *status;
  book_pg_t *pg_info;

  MT_ASSERT(index <= MAX_BOOK_PG);

  status = sys_status_get();

  pg_info = &status->book_info.pg_info[index];

  memcpy(pg_info, node, sizeof(book_pg_t));

  sys_status_save();
}


void sys_status_delete_book_node(u8 index)
{
  sys_status_t *status;
  book_pg_t *pg_info;

  MT_ASSERT(index < MAX_BOOK_PG);

  status = sys_status_get();

  pg_info = &status->book_info.pg_info[index];

  memset(pg_info, 0, sizeof(book_pg_t));

  pg_info->start_time.year = 2009;
  pg_info->start_time.month = 1;
  pg_info->start_time.day = 1;

  sys_status_save();
}

void sys_status_delete_book_node_all(void)
{
  sys_status_t *status;
  book_pg_t *pg_info;
  u8 i = 0;

  status = sys_status_get();

  for(i = 0; i < MAX_BOOK_PG; i++)
  {
      pg_info = &status->book_info.pg_info[i];
      memset(pg_info, 0, sizeof(book_pg_t));

      pg_info->start_time.year = 2009;
      pg_info->start_time.month = 1;
      pg_info->start_time.day = 1;
  }

  sys_status_save();
}

void sys_status_get_book_node(u8 index, book_pg_t *node)
{
  sys_status_t *status;

  MT_ASSERT(index <= MAX_BOOK_PG);

  status = sys_status_get();

  memcpy(node, &(status->book_info.pg_info[index]), sizeof(book_pg_t));
}


BOOL sys_status_get_status(u8 type, BOOL *p_status)
{
  sys_status_t *status;

  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();

    *p_status = (BOOL)SYS_GET_BIT(status->bit_status, type);
    return TRUE;
  }

  return FALSE;
}


BOOL sys_status_set_status(u8 type, BOOL enable)
{
  sys_status_t *status;

  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();
    if (enable)
    {
      SYS_SET_BIT(status->bit_status, type);
    }
    else
    {
      SYS_CLR_BIT(status->bit_status, type);
    }
    return TRUE;
  }

  return FALSE;
}

void sys_status_set_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pwd_set = *p_set;
}

void sys_status_get_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->pwd_set;
}


void sys_status_set_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->time_set = *p_set;
}

void sys_status_get_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->time_set;
}

void sys_status_set_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->play_set = *p_set;
}

void sys_status_get_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->play_set;
}

void sys_status_set_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->av_set = *p_set;
}

void sys_status_get_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->av_set;
}

//in jazz project,the global volume is used for nvod program
void sys_status_set_audio_set(audio_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->audio_set = *p_set;
}

//in jazz project,the global volume is used for nvod program
void sys_status_get_audio_set(audio_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->audio_set;
}

void sys_status_set_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->osd_set = *p_set;
}

void sys_status_get_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->osd_set;
}

void sys_status_set_adver_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->ad_tp = *p_set;
}

void sys_status_get_adver_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->ad_tp;
}

void sys_status_set_upgrade_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->upgrade_tp = *p_set;
}

void sys_status_get_upgrade_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->upgrade_tp;
}

void sys_status_set_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->main_tp1 = *p_set;
}

void sys_status_get_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->main_tp1;
}

void sys_status_set_main_tp2(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->main_tp2 = *p_set;
}

void sys_status_get_main_tp2(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->main_tp2;
}

void sys_status_set_nvod_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->nvod_tp = *p_set;
}

void sys_status_get_nvod_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->nvod_tp;
}

void sys_status_set_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->lang_set = *p_set;
}

void sys_status_get_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->lang_set;
}

BOOL sys_status_get_fav_name(u8 index, u16 *name)
{
  sys_status_t *status;

  if(index > MAX_FAV_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(name, status->fav_set.fav_group[index].fav_name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}

BOOL sys_status_set_fav_name(u8 index, u16 *name)
{
  sys_status_t *status;

  if(index > MAX_FAV_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->fav_set.fav_group[index].fav_name, name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}
void sys_status_set_local_set(local_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->local_set = *p_set;

}

void sys_status_get_local_set(local_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->local_set;
}

void sys_status_get_sw_info(sw_info_t *p_sw)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_sw = status->ver_info;
}

void sys_status_set_sw_info(sw_info_t *p_sw)
{
  sys_status_t *status;

  status = sys_status_get();

  status->ver_info = *p_sw;
}


void sys_status_set_time_zone(void)
{
  time_set_t time_set;
  s8 zone;

  sys_status_get_time(&time_set);

  zone = (s8)(time_set.gmt_offset - 23);

  time_set_zone(zone);
  time_set_summer_time((BOOL)time_set.summer_time);
}

void sys_status_get_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;
  
  sys_status_get_time(&t_set);

  memcpy(p_time, &(t_set.sys_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;

  sys_status_get_time(&t_set);

  memcpy(&(t_set.sys_time), p_time, sizeof(utc_time_t));

  sys_status_set_time(&t_set);

  //sys_status_save();
}
void sys_status_get_nit_version(u32 *p_nit_ver)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  *p_nit_ver = p_status->nit_ver;
}

void sys_status_set_nit_version(u32 nit_ver)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  p_status->nit_ver = nit_ver;

  sys_status_save();
}
void sys_status_get_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(p_time, &(p_status->sleep_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(&(p_status->sleep_time), p_time, sizeof(utc_time_t));

  sys_status_save();
}

char **sys_status_get_lang_code(BOOL is_2_b)
{
  return is_2_b ? g_lang_code_b : g_lang_code_t;
}


/*!
   convert unit of longitude from unsigned int to double.

   \param[in] u_longitude west longitude with W_LONGITUDE_MASK
   \return longitude 0 to 180 east, -180 to 0 west.
  */
double sys_status_convert_longitude(u32 u_longitude)
{
  double d_longitude = 0;

  if (IS_W_LONGITUDE(u_longitude))
  {
    LONGITUDE_NORMALIZE(u_longitude);
    d_longitude = 0 - (double)u_longitude;
  }
  else
  {
    d_longitude = (double)u_longitude;
  }
  d_longitude = d_longitude / 100.0;
  return d_longitude;
}

/*!
   convert unit of latitude from unsigned int to double.

   \param[in] u_longitude sorth latitude with S_LATITUDE_MASK
   \return longitude 0 to 90 north, -90 to 0 sorth.
  */
double sys_status_convert_latitude(u32 u_latitude)
{
  double d_latitude = 0;

  if (IS_S_LATITUDE(u_latitude))
  {
    LATITUDE_NORMALIZE(u_latitude);
    d_latitude = 0 - (double)u_latitude;
  }
  else
  {
    d_latitude = (double)u_latitude;
  }
  d_latitude = d_latitude / 100.0;
  return d_latitude;
}

u32 sys_get_adver_logo_addr(void)
{
    u32 flash_addr = 0;

    flash_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),LOGO_BLOCK_ID_M3)
                 - FLASH_BASE_ADDR;

    //OS_PRINTF("###debug adver_logo_addr = 0x%x\n",flash_addr);
    return flash_addr;
}
void sys_reset_adver_logo(void)
{
    u32 flash_addr = 0;
    charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
    flash_addr = sys_get_adver_logo_addr();
    if(flash_addr < CHARSTO_SIZE)
    {
       mtos_task_lock();
       charsto_erase(p_charsto_dev,flash_addr,1);
       mtos_task_unlock();
    }
}
u8 get_printf_switch_flag(void)
{
  u8 flag = 0;
  sys_status_t *status;
  status = sys_status_get();
  flag = status->printf_switch_set.printf_flag;
  return flag;
}
void set_printf_switch_flag(u8 flag)
{
  sys_status_t *status;
  status = sys_status_get();
  status->printf_switch_set.printf_flag= flag;
  sys_status_save();
}
#ifndef OTA_DM_ON_STATIC_FLASH
ota_info_t *sys_status_get_ota_info(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return &status->ota_info;
}

void sys_status_set_ota_info(ota_info_t *p_otai)
{
  sys_status_t *status;

  MT_ASSERT(p_otai != NULL);

  status = sys_status_get();

  memcpy((void *)&status->ota_info, (void *)p_otai, sizeof(ota_info_t));

  return;
}

#endif

u8 sys_status_get_channel_version(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->channel_version;
}

void sys_status_set_channel_version(u8 new_version)
{
  sys_status_t *status;

  status = sys_status_get();

  status->channel_version = new_version;

  sys_status_save();
}

void sys_status_reset_channel_and_nit_version(void)
{
  sys_status_set_channel_version(0xff);
  sys_status_set_nit_version(0xffffffff);
}

#ifndef OTA_DM_ON_STATIC_FLASH
static video_out_format_t sys_status_get_video_out(u8 format)
{
  video_out_format_t mode = VIDEO_OUT_VID_YUV;
  customer_cfg_t cfg = {0};
  OS_PRINTF("########sys_status_get_video_out_cfg   focus %d\n",format);
  get_customer_config(&cfg);
  if((CUSTOMER_JINGGONG == cfg.customer) ||
             (CUSTOMER_BOYUAN == cfg.customer) ||
             (CUSTOMER_ZHILING== cfg.customer) ||
             (CUSTOMER_ZHILING_KF== cfg.customer) ||
             (CUSTOMER_ZHILING_LQ== cfg.customer) ||
             (CUSTOMER_SHENZHOU_QN== cfg.customer))
  {
    return VIDEO_OUT_VID_CVBS;
  }
  if((CUSTOMER_FANTONG_KFAJX == cfg.customer)||
  	(CUSTOMER_FANTONG_BYAJX == cfg.customer)||
  	(CUSTOMER_FANTONG_XSMAJX== cfg.customer)||
  	(CUSTOMER_TDX_PAKISTAN == cfg.customer)||
  	(CUSTOMER_ZHONGDA == cfg.customer))
  {
    return 7;// CVBS+CVBS, according boot define
  }
  if(CUSTOMER_JIUZHOU == cfg.customer)
  {
        return 0;
  }
  switch(format)
  {
    case 0: //YUV and CVBS
      mode = VIDEO_OUT_VID_CVBS_YUV;
      break;
    case 1: // S_VIDEO and CVBS
      mode = VIDEO_OUT_VID_CVBS_SVDIEO;
      break;
    case 2:  //only RGB
      mode = VIDEO_OUT_VID_RGB;
      break;
    case 3: // only CVBS
      mode = VIDEO_OUT_VID_CVBS;
      break;
    case 4: // only YUV
      mode = VIDEO_OUT_VID_YUV;
    break;
    case 5: // only S_VIDE
      mode = VIDEO_OUT_VID_SVDIEO;
    break;
    default:
     // MT_ASSERT(0);
      mode = VIDEO_OUT_VID_CVBS_YUV;
    break;
  }

  return mode;
}

BOOL sys_status_set_boot_status(void)
{

  BOOL ret = 0;
  av_set_t av_set = {0};
  osd_set_t osd_set = {0};
  sys_boot_status_t boot_status = {0};
  dvbc_lock_t main_tp = {0};
  dvbc_lock_t upg_tp = {0};
  customer_cfg_t cfg = {0};
  void *p_dev = NULL;

  #ifdef WIN32
  return ret;
  #endif
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);

   get_customer_config(&cfg);

   sys_status_get_osd_set(&osd_set);
   sys_status_get_av_set(&av_set);
   sys_status_get_main_tp1(&main_tp);
   sys_status_get_upgrade_tp(&upg_tp);

   memset(&boot_status,0,sizeof(sys_boot_status_t));

   boot_status.tv_mode = av_set.tv_mode;
   boot_status.tv_ratio = av_set.tv_ratio;
   boot_status.video_output = (u8)sys_status_get_video_out(av_set.video_output);//av_set.video_output;
   boot_status.bright = osd_set.bright;
   boot_status.contrast = osd_set.contrast;
   boot_status.chroma = osd_set.chroma;
   /*****ota main upg tp info******/
   boot_status.ota_tp_info.main_tp.tp_freq= main_tp.tp_freq;
   boot_status.ota_tp_info.main_tp.tp_sym = main_tp.tp_sym;
   boot_status.ota_tp_info.main_tp.nim_modulate = main_tp.nim_modulate;
   boot_status.ota_tp_info.main_tp.pid = 0x1b58;

   boot_status.ota_tp_info.upg_tp.tp_freq = upg_tp.tp_freq;
   boot_status.ota_tp_info.upg_tp.tp_sym = upg_tp.tp_sym;
   boot_status.ota_tp_info.upg_tp.nim_modulate = upg_tp.nim_modulate;
   boot_status.ota_tp_info.upg_tp.pid = (u16)((upg_tp.reserve1 << 8) | upg_tp.reserve2);
   /***********/
   #ifdef OTA_IMG
   #if (SYS_FLASH_TOTALT_SIZE == SYS_FLASH_TOTALT_SIZE_4M)  //DESAI_ADV
   boot_status.vdec_addr = 0x1c60000;
   #else
    boot_status.vdec_addr = 0xc60000;
   #endif
   #else
   boot_status.vdec_addr = get_vdec_buff_addr();
   #endif

   boot_status.logo_display_bit = 1;

   if(cfg.customer == CUSTOMER_BOYUAN ||cfg.customer == CUSTOMER_ZHILING
    ||cfg.customer == CUSTOMER_ZHILING_KF||cfg.customer == CUSTOMER_ZHILING_LQ
    || cfg.customer == CUSTOMER_SHENZHOU_QN)
    {
      //boot_status.logo_display_bit = 0;
      boot_status.logo_display_bit = 1;
    }

 mtos_task_lock();
  ret = charsto_erase(p_dev,FLASH_LOGIC_ADRR_BOOT_STATUS, 1);
  if (ret != SUCCESS)
    {
       mtos_task_unlock();
       return ret;
    }
  ret = charsto_writeonly(p_dev, FLASH_LOGIC_ADRR_BOOT_STATUS, (u8 *)&boot_status,sizeof(sys_boot_status_t));
  if (ret != SUCCESS)
    {
       mtos_task_unlock();
       return ret;
    }
 mtos_task_unlock();
return ret;

}
void reset_boot_logo_flag(void)
{
   void *p_dev = NULL;
    BOOL ret_t = 0;
    sys_boot_status_t boot_status = {0};

#ifdef WIN32
    return;
#endif

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    mtos_task_lock();
    ret_t = charsto_read(p_dev,FLASH_LOGIC_ADRR_BOOT_STATUS, (u8 *)&boot_status,sizeof(sys_boot_status_t));
    if (ret_t != SUCCESS)
    {
      mtos_task_unlock();
      return;
    }
    boot_status.logo_display_bit = 0xff;
    ret_t = charsto_erase(p_dev,FLASH_LOGIC_ADRR_BOOT_STATUS, 1);
    if (ret_t != SUCCESS)
      {
         mtos_task_unlock();
         return;
      }
    ret_t = charsto_writeonly(p_dev, FLASH_LOGIC_ADRR_BOOT_STATUS, (u8 *)&boot_status,sizeof(sys_boot_status_t));
    if (ret_t != SUCCESS)
    {
       mtos_task_unlock();
       return;
    }
    mtos_task_unlock();
    return;

}
void get_boot_ota_tp_info(dvbc_ota_tp_t *tp_info)
{
    void *p_dev = NULL;
    BOOL ret_t = 0;
    sys_boot_status_t boot_status = {0};

#ifdef WIN32
    return;
#endif

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    mtos_task_lock();
    ret_t = charsto_read(p_dev,FLASH_LOGIC_ADRR_BOOT_STATUS, (u8 *)&boot_status,sizeof(sys_boot_status_t));
   if (ret_t != SUCCESS)
    {
      mtos_task_unlock();
      return;
    }
   mtos_task_unlock();

   tp_info->main_tp.tp_freq = boot_status.ota_tp_info.main_tp.tp_freq;
   tp_info->main_tp.tp_sym = boot_status.ota_tp_info.main_tp.tp_sym;
   tp_info->main_tp.nim_modulate = boot_status.ota_tp_info.main_tp.nim_modulate;
   tp_info->main_tp.pid = boot_status.ota_tp_info.main_tp.pid;

   tp_info->upg_tp.tp_freq = boot_status.ota_tp_info.upg_tp.tp_freq;
   tp_info->upg_tp.tp_sym = boot_status.ota_tp_info.upg_tp.tp_sym;
   tp_info->upg_tp.nim_modulate = boot_status.ota_tp_info.upg_tp.nim_modulate;
   tp_info->upg_tp.pid = boot_status.ota_tp_info.upg_tp.pid;

    return;
}
u8 get_boot_logo_flag(void)
{

    u8 flag = 0;
    void *p_dev = NULL;
    BOOL ret_t = 0;
    sys_boot_status_t boot_status = {0};
#ifdef WIN32
    return flag;
#endif
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    mtos_task_lock();
    ret_t = charsto_read(p_dev,FLASH_LOGIC_ADRR_BOOT_STATUS, (u8 *)&boot_status,sizeof(sys_boot_status_t));
    if (ret_t == SUCCESS)
    {
     flag = boot_status.logo_display_bit;
    }
    else
    {
      flag = 0;
      }
    mtos_task_unlock();
    return flag;

}

#endif

void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size)
{
    u32 read_len = 0;
    u32 id_len = 0;
    u8 cfg_tmp[64] = {0};
    u8 id_tmp[64] = {0};
    u8 cnk = 0;
    u8 distance = 0;

    distance = 12; /***note  it is distance from 0x7fc00 for tcon  tools***/
    memset(cfg_tmp,0,sizeof(cfg_tmp));
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
              IDENTITY_BLOCK_ID, 0, 0,
              64,
              (u8 *)cfg_tmp);
	OS_PRINTF("read_len = %d,cfg_tmp = %s\n",read_len,cfg_tmp);
    if(read_len == 0)
    {
      strcpy((char *)&cfg_tmp[distance],(char *)"100001");
    }

   for(cnk = 0;cnk < idsize;cnk ++)
    {
      if((cfg_tmp[distance + cnk] < 0x30) ||(cfg_tmp[distance + cnk] > 0x39))
        {
          memset(cfg_tmp,0,sizeof(cfg_tmp));
		  OS_PRINTF("data err!\n");
          strcpy((char *)&cfg_tmp[distance],(char *)"100001");
          break;
        }
    }
    memset(id_tmp,0,sizeof(id_tmp));
    memcpy(id_tmp,&cfg_tmp[distance],idsize);

    id_len = strlen((char *)id_tmp);
    if(id_len > size)
      {
        id_len = size;
      }
    strncpy((char *)idbuf,(char *)id_tmp,id_len);
}

static u16 htons(u16 n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}
static u32 htonl(u32 n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}
void sys_status_get_all_hw_version(u8 *str,u32 str_size)
{
  u8 ansstr[64] = {0};
  misc_options_t misc = {0};
  u32 read_len = 0;
  u32 size = 0;
  customer_cfg_t cfg = {0};
  if(NULL == str)
  {
     return;
  }
  get_customer_config(&cfg);
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

  if(CUSTOMER_YINGJI == cfg.customer)
  {
     sprintf((char*)ansstr, "%s",misc.str_hw_version);
  }else
  {
     sprintf((char*)ansstr, "%s%03x",misc.str_hw_version,
                            htons(misc.ota_tdi.hw_version));
  }
  size = strlen((char *)ansstr);
  if(size > str_size)
    {
      size = str_size;
    }

  strncpy((char *)str,(char *)ansstr,size);
}
void sys_status_get_all_sw_version(u8 *str,u32 str_size)
{
  u8 ansstr[64] = {0};
  misc_options_t misc = {0};
  u32 read_len = 0;
  u32 size = 0;
  customer_cfg_t cfg = {0};
  if(NULL == str)
  {
     return;
  }
  get_customer_config(&cfg);
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
//  sys_status_get_sw_changeset(changeset); 
#endif
  if(CUSTOMER_YINGJI == cfg.customer)
  {
      sprintf((char*)ansstr, "%s",misc.str_sw_version);
  }
  else {
      sprintf((char*)ansstr, "%s%lx",misc.str_sw_version,
                                            sys_status_get_sw_version());
  }
  size = strlen((char *)ansstr);
  if(size > str_size)
    {
      size = str_size;
    }
  strncpy((char *)str,(char *)ansstr,size);
}

#define FLASH_SYSTEM_BOOT_INFO_ADDRR 0x1ffd0

typedef struct 
{
   /* boot change set.no */
  u8 changeset[12];
   /**boot version no***/
  u32 version_no;
  /**boot release time***/
  u32 time;
}boot_info_t;

void sys_status_get_all_boot_version_info(u8 *str,u32 str_size)
{
  BOOL ret = SUCCESS;
  void *p_dev = NULL;
  boot_info_t boot_info;
  u8 ansstr[20] = {0};
  u32 ansstr_size = 0;

  memset(&boot_info,0,sizeof(boot_info_t));
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);
  mtos_task_lock();
  ret = charsto_read(p_dev,FLASH_SYSTEM_BOOT_INFO_ADDRR, (u8 *)&boot_info,sizeof(boot_info_t));
  mtos_task_unlock();
  if (ret != SUCCESS)
  {
    sprintf((char*)ansstr, "unknown");
  }
  else
  {
    //sprintf((char*)ansstr, "%ld_%s_%ld",boot_info.version_no,boot_info.changeset,boot_info.time);
    sprintf((char*)ansstr, "Loader-%ld",boot_info.version_no);
  }
  ansstr_size = strlen((char *)ansstr);
  if(ansstr_size > str_size)
  {
      ansstr_size = str_size;
  }
  strncpy((char *)str,(char *)ansstr,ansstr_size); 
}

void sys_status_get_all_oui_version(u8 *str,u32 str_size)
{
  u8 ansstr[64] = {0};
  misc_options_t misc = {0};
  u32 read_len = 0;
  u32 size = 0;
  customer_cfg_t cfg = {0};
  if(NULL == str)
  {
     return;
  }
  get_customer_config(&cfg);
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

  
      sprintf((char*)ansstr, "%04lx",htonl(misc.ota_tdi.oui));

  size = strlen((char *)ansstr);
  if(size > str_size)
    {
      size = str_size;
    }
  strncpy((char *)str,(char *)ansstr,size);
}


utc_time_t sys_status_get_card_upg_time(void)
{
  sys_status_t *status;
 

  status = sys_status_get();
 

  return status->card_upg_time;
}

void sys_status_set_card_upg_time(utc_time_t card_upg_time)
{
  sys_status_t *status;
 

  status = sys_status_get();
 

  memcpy(&(status->card_upg_time), &card_upg_time, sizeof(utc_time_t));
 

  sys_status_save();
}
 

u32 sys_status_get_card_upg_state(void)
{
  sys_status_t *status;
 

  status = sys_status_get();
 

  return status->card_upg_state;
}
 

void sys_status_set_card_upg_state(u32 card_upg_state)
{
  sys_status_t *status;
 

  status = sys_status_get();
 

  status->card_upg_state = card_upg_state;
 

  sys_status_save();
}


