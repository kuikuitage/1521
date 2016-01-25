/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __AP_PLAY_API_H__
#define __AP_PLAY_API_H__

#define PWD_DLG_FOR_PLAY_X      ((SCREEN_WIDTH-PWDLG_W)/2-140)
#define PWD_DLG_FOR_PLAY_Y    ((SCREEN_HEIGHT-PWDLG_H)/2)

enum playback_msg
{
  MSG_TTX_READY = MSG_EXTERN_BEGIN + 450,
  MSG_SUBT_READY,
  MSG_DESCRAMBLE_SUCCESS,
  MSG_DESCRAMBLE_FAILED,
  MSG_DYNC_PID_UPDATE,
  MSG_SCART_VCR_DETECTED,
  MSG_LOCK_RSL,
  MSG_PB_NIT_FOUND,
  MSG_NIT_OTA_UPGRADE,
};


BOOL ui_recall(BOOL is_play, u16 *p_curn);

void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn);

u16 ui_tvradio_switch(BOOL is_play, u16 *p_curn);

u16 ui_find_prog_pos_in_cur_group(dvbs_prog_node_t *p_prog);

BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play);

BOOL ui_play_prog(u16 prog_id, BOOL is_force);

void ui_stop_play(stop_mode_t type, BOOL is_sync);

void ui_set_audio_channel(u8 channel);

void ui_pause_play(void);

void ui_resume_play(void);

void ui_enable_playback(BOOL is_enable);

void ui_clear_play_history(void);

BOOL ui_play_curn_pg(void);

void ui_set_frontpanel_by_curn_pg(void);

BOOL ui_is_pass_chkpwd(u16 prog_id);

void ui_enable_chk_pwd(BOOL is_enable);

BOOL ui_is_chk_pwd(void);

void ui_set_chk_pwd(u16 prog_id, BOOL is_pass);

BOOL ui_is_playing(void);

BOOL ui_is_blkscr(void);


BOOL ui_is_subt_data_ready(u16 prog_id);

BOOL ui_is_tvradio_switch(u16 pgid);

BOOL ui_is_playpg_scrambled(void);

BOOL ui_get_curpg_scramble_flag(void);

void ui_set_playpg_scrambled(BOOL is_scramble);

void ui_book_play(u16 pgid);

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp);

void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg);

void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id);

void do_lock_pg(u16 pg_id);

void ui_reset_tvmode(u16 av_mode);
void ui_reset_video_aspect_mode(u32 aspect);

BOOL is_enable_vbi_on_setting(void);

void ui_play_api_factory(void);
#endif

