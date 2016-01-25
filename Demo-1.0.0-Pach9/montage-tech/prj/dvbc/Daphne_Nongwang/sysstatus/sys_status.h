/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

#define MAX_FAV_CNT          1   //1for dvb-c; 32 for dvb-s
#define MAX_BOOK_PG          60//60for dvb-c; 20 for dvb-s

#define MAX_GROUP_CNT        (MAX_FAV_CNT + 0x1)  /*all group*/

#define EVENT_NAME_LENGTH    (32)         // TO BE CHECKED
#define MAX_FAV_NAME_LEN    16
#define INFO_STRING_LEN  16
#define MAX_CHANGESET_LEN 20


typedef enum
{
  BS_LNB_POWER = 0,
  BS_BEEPER,
  BS_ANTENNA_CONNECT,
  BS_MENU_LOCK,
  BS_PROG_LOCK,
  BS_SLEEP_LOCK,
  BS_OSD_VER,
  BS_NIT_RECIEVED,
  BS_MAX_CNT
}bit_status_t;

typedef enum
{
  BOOK_TMR_OFF = 0,
  BOOK_TMR_ONCE,
  BOOK_TMR_DAILY,
  BOOK_TMR_WEEKLY
}book_tmr_mode_t;

typedef enum
{
  TV_RATIO_4_3 = 0,
  TV_RATIO_16_9,
}tv_ratio_t;

/*!
 * book pg information
 */
typedef struct
{
  u16 pgid;
  u8 svc_type:4;
  u8 timer_mode:4;
  u16 event_name[EVENT_NAME_LENGTH + 1];
  utc_time_t start_time;
  utc_time_t drt_time;
} book_pg_t;

/*!
 * book information
 */
typedef struct
{
  book_pg_t pg_info[MAX_BOOK_PG];
} book_info_t;

/*!
* favorite group
*/
typedef struct
{
	u16 fav_name[MAX_FAV_NAME_LEN];
} fav_group_t;

typedef struct
{
  fav_group_t fav_group[MAX_FAV_CNT];
} fav_set_t;

/*!
 * group type
 */
enum
{
  GROUP_T_ALL = 0,
  GROUP_T_SAT,
  GROUP_T_FAV
};

/*!
 * scan mode
 */
enum
{
  SCAN_MODE_ALL = 0,
  SCAN_MODE_FREE,
};

/*!
 * curn mode
 */
enum
{
  CURN_MODE_NONE = 0,
  CURN_MODE_TV,
  CURN_MODE_RADIO,

  CURN_MODE_CNT,
};


/*auto search finish flg finish_flag*/
typedef struct
{
  u8 printf_flag;
} printf_switch_flag_t;

/*!
 * language setting
 */
typedef struct
{
  u8 osd_text;
  u8 first_audio;
  u8 second_audio;
  u8 tel_text;
  u8 sub_title;
} language_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 tv_mode;
  u8 tv_ratio;
  u8 video_output;
  u8 rf_system;
  u8 rf_channel;
} av_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 is_global_volume;
  u8 global_volume;
  u8 is_global_track;
  u8 global_track;
} audio_set_t;

/*!
 * osd setting
 */
typedef struct
{
  u8 palette;
  u8 transparent;
  u8 chroma;
  u8 bright;
  u8 contrast;
  u8 reserve;
  u8 timeout;
  u8 enable_teltext;
  u8 enable_subtitle;
//  u8 enable_clock;
  u8 enable_vbinserter;
} osd_set_t;

/*!
 * password setting
 */
typedef struct
{
  u32 normal;
  u32 super;
  u8 enable_psw_poweron;
  u8 enable_psw_lockchannel;
  u8 reserve1;
  u8 reserve2;
  u32 psw_poweron;
  u32 psw_lockchannel;
} pwd_set_t;

/*!
 * current programinfor
 */
typedef struct
{
  u16 id;
  u16 pos;
} curn_pg_info_t;

/*!
 * program group infor
 */
typedef struct
{
  curn_pg_info_t curn_tv;
  curn_pg_info_t curn_radio;
  u32          context;
} curn_info_t;

/*!
 * program group setting
 */
typedef struct
{
  u8           curn_mode;
  u16          curn_group;
  curn_info_t group_info[MAX_GROUP_CNT];
} group_set_t;


#define W_LONGITUDE_MASK 0x8000

#define IS_W_LONGITUDE(x) (BOOL)(x & W_LONGITUDE_MASK)
#define SET_LONGITUDE(x, w) (x = w ? (x | W_LONGITUDE_MASK) : (x & (~W_LONGITUDE_MASK)))
#define LONGITUDE_NORMALIZE(x) (x &= (~W_LONGITUDE_MASK))

#define S_LATITUDE_MASK 0x8000

#define IS_S_LATITUDE(x) (BOOL)(x & S_LATITUDE_MASK)
#define SET_LATITUDE(x, s) (x = s ? (x | S_LATITUDE_MASK) : (x & (~S_LATITUDE_MASK)))
#define LATITUDE_NORMALIZE(x) (x &= (~S_LATITUDE_MASK))

/*!
 * local setting
 */
typedef struct
{
  u8   positioner_type; // 0 DiSEqC1.2 1 USALS
  u8   lnb_type;

  u16 longitude;//use this u16 for smsx ca network_id
  u16 latitude;

  u16 lnb_h;
  u16 lnb_l;
} local_set_t;

/*!
 * time setting
 */
typedef struct
{
  utc_time_t sys_time;
  u8 gmt_usage  : 1;
  u8 gmt_offset :  6;
  u8 summer_time :1;
} time_set_t;


/*!
 * play type
 */
enum
{
  PLAY_T_ALL = 0,
  PLAY_T_FREE,
  PLAY_T_SCRAMBLE,
};

/*!
 * channel change mode
 */
enum
{
  CHCHANGE_M_FREEZE = 0,
  CHCHANGE_M_BLACKSCR,
};

/*!
 * channel play setting
 */
typedef struct
{
  /* all free scramble */
  u8 type;
  /* black freeze */
  u8 mode;
} play_set_t;

/*!
 * preset setting
 */
typedef struct
{
  u8 enable;
  u8 tv_track;
  u8 radio_track;
} preset_set_t;

/*version info*/
typedef struct
{
  u16 bl_ver[INFO_STRING_LEN];
  u16 bl_time[INFO_STRING_LEN];
  u16 main_ver[INFO_STRING_LEN];
  u16 main_time[INFO_STRING_LEN];
  u16 rback_ver[INFO_STRING_LEN];
  u16 rback_time[INFO_STRING_LEN];
  u16 vinfo_ver[INFO_STRING_LEN];
  u16 vinfo_time[INFO_STRING_LEN];
  u16 ddb_ver[INFO_STRING_LEN];
  u16 ddb_time[INFO_STRING_LEN];
  u16 udb_ver[INFO_STRING_LEN];
  u16 udb_time[INFO_STRING_LEN];
}sw_info_t;


/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u8 nim_modulate;

  u8 reserve1;
  u8 reserve2;
  u8 reserve3;
} dvbc_lock_t;

#ifndef OTA_DM_ON_STATIC_FLASH
/*!
  DVBC OTA lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u8 nim_modulate;
  u8 reserve1;
  u16 pid;
} dvbc_ota_lock_t;

/*!
  DVBC OTA tp info
  */
typedef struct
{
  dvbc_ota_lock_t main_tp;
  dvbc_ota_lock_t upg_tp;
}dvbc_ota_tp_t;
#endif


#define UCAS_KEY_SIZE (32*KBYTES)
#define UCAS_KEY_SYNC_HDR_SIZE 4

/*!
 * system setting
 */
typedef struct
{
  /* change set.no */
  u8 changeset[MAX_CHANGESET_LEN];

  /*version number*/
  u32 sw_version;
 
  /*printf_switch_flag*/ 
  printf_switch_flag_t    printf_switch_set;

  /* language settting */
  language_set_t lang_set;

  /* av settting */
  av_set_t       av_set;

  /* audio settting */
  audio_set_t    audio_set;

  /* play settting */
  play_set_t play_set;

  /* osd settting */
  osd_set_t      osd_set;

  /* password settting */
  pwd_set_t      pwd_set;

  /* preset setting */
  preset_set_t   preset_set;

  /* time settting */
  time_set_t     time_set;

  /* group settting */
  group_set_t    group_set;

  /* book infor */
  book_info_t book_info;

  /* local setting */
  local_set_t local_set;//use this struct longitude u16 for smsx ca network_id

  /*fav group*/
  fav_set_t fav_set;

  /*advertisment TP*/
  dvbc_lock_t ad_tp;

  /*upgrade TP*/
  dvbc_lock_t upgrade_tp;

  /*mian TP 1*/
  dvbc_lock_t main_tp1;

  /*mian TP 2*/
  dvbc_lock_t main_tp2;

  /*NVOD TP*/
  dvbc_lock_t nvod_tp;

  /*information*/
  sw_info_t ver_info;

  /* bit status */
  u32 bit_status;

  /*sleep time*/
  utc_time_t sleep_time;

  /*nit version*/
  u32 nit_ver;

  /*channel version */
  u8 channel_version;

  /*force key*/
  u16 force_key;
  
  #ifdef OTA_DM_ON_STATIC_FLASH
  /* below is backup size,if you add one parameter,please dec back group reserve_group mumber,
                 the total number is 100;make sure ss_status.bin total size is stationary*/
  u32 reserve_group[100];
  /****above is backup size****/
  #else
  /* ota info */
  ota_info_t ota_info;
  #endif
  u32 card_upg_state;
  utc_time_t card_upg_time;
} sys_status_t;

typedef struct
{
  u32 oui;
  u16 symbol;
  u32 freq;
  u8  qam_mode;
  u16  data_pid;
  u8  ota_type;
  u32 hwVersion;
  u32 swVersion;
  u8  Serial_number_start[16];
  u8  Serial_number_end[16];
}update_t;

#ifndef OTA_DM_ON_STATIC_FLASH
/*!
 * system setting boot status
 */
typedef struct
{
    /* av settting */
    /**1:NTSC 2:PAL other:PAL**/
    u8 tv_mode;
    /**av_set.tv_ratio   0:4:3   1:16:9 other 4:3**/
    u8 tv_ratio;
    u8 video_output;
    /**av_set.video_output  0:YUV  1:S_VIDEO   2:RGB  other YUV***/
    /* osd settting */
    u8 bright;
    u8 contrast;
    u8 chroma;
    /* vdec buff addr */
    u32 vdec_addr;
     /* boot logo display ctrl 0:OFF 1:display */
    u8 logo_display_bit;
    dvbc_ota_tp_t ota_tp_info;
    /***osd_set.brigh 0~100***/
}sys_boot_status_t;
#endif
void sys_status_init(void);

void sys_status_load(void);
void sys_group_reset(void);
void sys_status_save(void);

sys_status_t *sys_status_get(void);

u32 sys_status_get_sw_version(void);

void sys_status_set_sw_version(u32 new_version);
void sys_status_get_sw_changeset(u8 *changeset);

void  sys_status_add_book_node(u8 index, book_pg_t *node);

void sys_status_delete_book_node(u8 index);

void sys_status_delete_book_node_all(void);

void sys_status_get_book_node(u8 index, book_pg_t *node);

BOOL sys_status_get_status(u8 type, BOOL *p_status);

BOOL sys_status_set_status(u8 type, BOOL enable);

void sys_status_set_pwd_set(pwd_set_t *p_set);

void sys_status_get_pwd_set(pwd_set_t *p_set);

void sys_status_set_time(time_set_t *p_set);

void sys_status_get_time(time_set_t *p_set);

void sys_status_set_play_set(play_set_t *p_set);

void sys_status_get_play_set(play_set_t *p_set);

void sys_status_set_av_set(av_set_t *p_set);

void sys_status_get_av_set(av_set_t *p_set);

void sys_status_set_audio_set(audio_set_t *p_set);

void sys_status_get_audio_set(audio_set_t *p_set);

void sys_status_set_osd_set(osd_set_t *p_set);

void sys_status_get_osd_set(osd_set_t *p_set);

void sys_status_set_adver_tp(dvbc_lock_t *p_set);

void sys_status_get_adver_tp(dvbc_lock_t *p_set);

void sys_status_set_upgrade_tp(dvbc_lock_t *p_set);

void sys_status_get_upgrade_tp(dvbc_lock_t *p_set);

void sys_status_set_main_tp1(dvbc_lock_t *p_set);

void sys_status_get_main_tp1(dvbc_lock_t *p_set);

void sys_status_set_main_tp2(dvbc_lock_t *p_set);

void sys_status_get_main_tp2(dvbc_lock_t *p_set);

void sys_status_set_nvod_tp(dvbc_lock_t *p_set);

void sys_status_get_nvod_tp(dvbc_lock_t *p_set);

void sys_status_set_lang_set(language_set_t *p_set);

void sys_status_get_lang_set(language_set_t *p_set);

BOOL sys_status_get_fav_name(u8 index, u16 *name);

void sys_status_set_local_set(local_set_t *p_set);

BOOL sys_status_set_fav_name(u8 index, u16 *name);

void sys_status_get_local_set(local_set_t *p_set);

void sys_status_get_sw_info(sw_info_t *p_sw);

void sys_status_set_sw_info(sw_info_t *p_sw);

void sys_status_set_time_zone(void);

void sys_status_get_utc_time(utc_time_t *p_time);

void sys_status_set_utc_time(utc_time_t *p_time);

void sys_status_get_sleep_time(utc_time_t *p_time);

void sys_status_set_sleep_time(utc_time_t *p_time);

void sys_status_get_nit_version(u32 *p_nit_ver);

void sys_status_set_nit_version(u32 nit_ver);

void sys_status_reload_ucas_key(void);

void sys_status_store_ucas_key(void);

char **sys_status_get_lang_code(BOOL is_2_b);

/*!
   convert unit of longitude from unsigned int to double.

   \param[in] u_longitude west longitude with W_LONGITUDE_MASK
   \return longitude 0 to 180 east, -180 to 0 west.
  */
double sys_status_convert_longitude(u32 u_longitude);

/*!
   convert unit of latitude from unsigned int to double.

   \param[in] u_longitude sorth latitude with S_LATITUDE_MASK
   \return longitude 0 to 90 north, -90 to 0 sorth.
  */
double sys_status_convert_latitude(u32 u_latitude);

u32 sys_get_adver_logo_addr(void);
u32 sys_get_adver_logo_size(void);
void sys_reset_adver_logo(void);

#ifndef OTA_DM_ON_STATIC_FLASH
ota_info_t *sys_status_get_ota_info(void);
void sys_status_set_ota_info(ota_info_t *p_otai);
void get_boot_ota_tp_info(dvbc_ota_tp_t *tp_info);
void reset_boot_logo_flag(void);
u8 get_boot_logo_flag(void);
BOOL sys_status_set_boot_status(void);
#endif
u8 get_printf_switch_flag(void);
void set_printf_switch_flag(u8 flag);
u8 sys_status_get_channel_version(void);
void sys_status_set_channel_version(u8 new_version);
void sys_status_reset_channel_and_nit_version(void);
void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size);
void sys_status_get_all_hw_version(u8 *str,u32 str_size);
void sys_status_get_all_sw_version(u8 *str,u32 str_size);
void sys_status_get_all_boot_version_info(u8 *str,u32 str_size);
u8 get_audio_global_volume(void);
void set_audio_global_volume(u8 volume);
#endif
utc_time_t sys_status_get_card_upg_time(void);
void sys_status_set_card_upg_time(utc_time_t card_upg_time);
u32 sys_status_get_card_upg_state(void);
void sys_status_set_card_upg_state(u32 card_upg_state);
void sys_status_get_all_oui_version(u8 *str,u32 str_size);


