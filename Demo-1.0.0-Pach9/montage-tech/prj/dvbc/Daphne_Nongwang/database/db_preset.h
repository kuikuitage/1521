/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#ifndef __PRESET_DATA_H__
#define __PRESET_DATA_H__

#define MAX_NAME_LENGTH 16

typedef enum
{
  PRESET_SAT = 0,
  PRESET_PRO,
  PRESET_TP
}preset_type_t;

typedef enum
{
  PRESET_PG_AREA_DOMESTIC = 0, 
  PRESET_PG_AREA_OVERSEA,
  PRESET_PG_AREA_ALL
}preset_pg_area_t;

/*!
 * load preset config 
 */
typedef enum
{
  PRESET_PG_TYPE_ABS = 0, 
  PRESET_PG_TYPE_DVBS,
  PRESET_PG_TYPE_ALL
}preset_pg_type_t;

typedef struct
{
  u32 diseqc_port;
  u32 votage;
  u32 k_option;

  u32 lnb_power;
  u32 lnb_type;

  u32 lnb_high;
  u32 lnb_low;
  u8  sat_name[MAX_NAME_LENGTH];
  u16 longitude;
  u8  is_abs;
} preset_sat_node_t;

typedef struct
{
  u32 pol;
  u32 frq;
  u32 sym;
  u32 tp_id;
} preset_tp_node_t;

typedef struct
{
  u32 audio_pid1;
  u32 audio_channel;


  u32 audio_pid2;
  u32 video_pid;

  u32 audio_pid3;
  u32 pcr_pid;
  u32 s_id;
  u16 is_domestic;
  u8 is_9b;
  u8 service_name[MAX_NAME_LENGTH];
} preset_pro_node_t;

typedef struct
{
  preset_sat_node_t *p_sat;
  preset_tp_node_t *p_tp;
  preset_pro_node_t *p_pro;
  preset_type_t node_type;
}preset_node_t;

void db_dvbs_load_preset(u8 blockid, preset_pg_type_t pg_type, preset_pg_area_t pg_area);

#endif
