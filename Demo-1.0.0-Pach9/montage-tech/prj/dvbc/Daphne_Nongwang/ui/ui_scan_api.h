/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_SCAN_API_H__
#define __UI_SCAN_API_H__

enum scan_msg
{
  MSG_SCAN_PG_FOUND = MSG_EXTERN_BEGIN + 250,
  MSG_SCAN_TP_FOUND,
  MSG_SCAN_PROGRESS,
  MSG_SCAN_SAT_FOUND,
  MSG_SCAN_NIT_FOUND,
  MSG_SCAN_DB_FULL,
  MSG_SCAN_FINISHED,  
};

enum scan_type
{
  USC_BLIND_SCAN,
  USC_PRESET_SCAN,
  USC_TP_SCAN,
  USC_PID_SCAN,
  USC_DVBC_FULL_SCAN,
  USC_DVBC_MANUAL_SCAN,
  
  USC_INVALID,
};

enum go_to_scan
{
  SCAN_TYPE_MANUAL,
  SCAN_TYPE_AUTO,
  SCAN_TYPE_FULL,
  SCAN_TYPE_RANGE,
};


// set scan parameters
void ui_scan_param_init(void);

void ui_scan_param_set_type(u8 type, u8 chan_type, u8 free_only, nit_scan_type_t nit_type, 
                            scan_scramble_origin_t scramble, cat_scan_type_t cat_type,bat_scan_type_t bat_type);

u8 ui_scan_param_get_type(void);

nit_scan_type_t ui_scan_param_nit_type(void);

u16 ui_scan_param_get_sat_num(void);

BOOL ui_scan_param_add_tp(dvbs_tp_node_t *p_tp_info);

void ui_scan_param_set_pid(u16 v_pid, u16 a_pid, u16 pcr_pid);

void ui_init_scan(void);

void ui_release_scan(void);

BOOL ui_start_scan(void);

void ui_stop_scan(void);

void ui_pause_scan(void);

void ui_resume_scan(void);
BOOL ui_get_scan_pause_status(void);
void ui_set_dvbc_allfreq_symb(u16 symb);
u32 ui_get_index_by_freq(u32 freq);

#endif


