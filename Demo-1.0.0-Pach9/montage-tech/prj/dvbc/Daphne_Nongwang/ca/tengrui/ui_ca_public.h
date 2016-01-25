/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_PUBLIC_H__
#define __UI_CA_PUBLIC_H__

void ui_reset_ca_last_msg(void);
u32 ui_get_ca_last_msg(void);
void ui_set_ca_last_msg_valid(u32 i);

void ui_ca_set_sid(cas_sid_t *p_ca_sid);
void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2);
void on_ca_zone_check_end(void);
void on_ca_zone_check_stop(void);
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2);
RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y );
void ui_set_ca_last_msg(u32 last_msg);
void on_ca_frm_btn_select_number(u8 ctrlID, u32 para1 );
void clean_cas_preview(void);

#if((CONFIG_CAS_ID == CONFIG_CAS_ID_SV)||(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB) || CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
BOOL ui_is_smart_card_insert(void);
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE on_ca_card_update_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2);

#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
void  redraw_finger_again(void);
void redraw_pop_osd_again(void);
void clean_cas_preview(void);
BOOL ui_is_ca_free_preview(void);
RET_CODE sv_nit_area_lock(u32 para1, BOOL get_nit_flag);
BOOL vkey_is_need(u32 vkey);
void sv_osd_set_key_enable(BOOL flag);
BOOL sv_osd_get_key_enable(void);
void on_desktop_start_pop_osd(msg_info_t * p_osd_msg, u16 *p_unistr);
void sv_osd_set_exit_key_enable(BOOL flag);
BOOL sv_osd_get_exit_key_enable(void);
#endif

#if((CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG) || (CONFIG_CAS_ID == CAS_ID_LX) || (CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA))
  BOOL ui_get_ecm_msg(void);
  void ui_set_ecm_msg(BOOL p_ecm_msg);
#endif

RET_CODE open_conditional_accept_feed(u32 para1, u32 para2);
RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);
RET_CODE open_conditional_accept_pin(u32 para1, u32 para2);
RET_CODE open_conditional_accept_pair(u32 para1, u32 para2);
RET_CODE open_conditional_accept_mother(u32 para1, u32 para2);
RET_CODE open_conditional_accept_level(u32 para1, u32 para2);
RET_CODE open_conditional_accept_info(u32 para1, u32 para2);
RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2);

#endif

