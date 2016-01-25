/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/

#include "ui_common.h"

#include "ui_mainmenu.h"
#include "ui_sub_menu.h"
#include "ui_volume.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_prog_bar.h"
#include "ui_num_play.h"
#include "ui_signal.h"
#include "ui_do_search.h"
#include "ui_prog_list.h"
#include "ui_notify.h"
#include "ui_fav_set.h"
#include "ui_epg.h"
#include "ui_epg_detail.h"
#include "ui_fav_list.h"
#include "ui_ota.h"
#include "ui_ota_search.h"
#include "ui_display.h"
#include "ui_language_set.h"
#include "ui_track_set.h"
#include "ui_tp_set.h"
//lvcm
#include "ui_senior_sub_menu.h"
#if((CONFIG_CAS_ID != CONFIG_CAS_ID_XSM)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_QL))
#include "ui_ca_info.h"
#endif
//lvcm end

#if((CONFIG_CAS_ID == CONFIG_CAS_ID_DS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV) || (CONFIG_CAS_ID == CONFIG_CAS_ID_QL))
#include "ui_ca_entitle_info.h"
#include "ui_ca_card_info.h"
#endif
#if((CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG) || (CONFIG_CAS_ID == CONFIG_CAS_ID_GS))
#include "ui_ca_card_info.h"
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
#include "ui_dvb_conditional_accept_worktime.h"
#include "ui_dvb_conditional_accept_pin.h"
#include "ui_dvb_conditional_accept_level.h"
#include "ui_dvb_ca_entitle_info.h"
#include "ui_dvb_ca_operator_info.h"
#include "ui_dvb_ca_card_info.h"
#include "ui_dvbca_sp_id.h"
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
#include "ui_email_mess_content.h"
#include "ui_ca_pair.h"
#endif

#include "ui_pro_classify.h"
#include "ui_prebook_manage.h"
#include "ui_manual_search.h"
#include "ui_sys_info.h"
#include "ui_channel_edit.h"

#include "ui_email_mess.h"
#include "ui_email_picture.h"
#include "ui_password_modify.h"
#include "ui_manual_upgrade.h"
#include "ui_restore_factory.h"
#include "ui_signal_check.h"
#include "ui_conditional_accept.h"

#if((CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV))
#include "ui_ca_operate.h"
#include "ui_ca_card_info.h"
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
#include "ui_ca_info.h"
#include "ui_ca_card_info.h"
#endif

#include "ui_ca_public.h"
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_XSM)
#if((CONFIG_CAS_ID != CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_QL))
#include "ui_ca_ppt_dlg.h"
#else
#include "ui_ca_ipp.h"
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
#include "ui_provider_info.h"
#include "ui_ca_prompt.h"
#include "ui_alert_mess.h"
#endif
#endif
#endif

#include "ui_game_inn.h"
#include "ui_calendar_menu.h"
#include "ui_channel_numb.h"
#include "ui_pro_info.h"
#include "ui_conditional_entitle_expire.h"
#include "ui_conditional_accept_update.h"
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_XSM)
#if((CONFIG_CAS_ID != CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID != CONFIG_CAS_ID_QL))
#include "ui_ca_finger.h"
#else
#include "ui_finger_print.h"
#include "ui_ca_notify.h"
#endif
#endif


#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
#include "ui_email_mess_content.h"
#include "ui_conditional_accept_feed_passive.h"
#include "ui_conditional_accept_feed.h"
#include "ui_ca_ou_ipp.h"
#include "ui_ca_pin_ipp.h"
#include "ui_ca_book_history.h"
#include "ui_ca_hide_menu.h"
#include "ui_ca_urgency_broadcast.h"
#include "ui_ca_free_preview.h"
#include "ui_ca_order_ipp_rtime.h"
#include "ui_ca_force_msg.h"
#include "ui_ca_notify.h"
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
#include "ui_ca_mother.h"
#define CAS_MG_312_VER
#ifdef CAS_MG_312_VER
#include "ui_ca_character_code.h"
#include "ui_ca_burse_charge.h"
#include "ui_email_mess_content.h"
#include "ui_ca_pre_auth.h"
#include "ui_ca_ppv_icon.h"
#include "ui_ca_pair.h"
#endif
#endif

#if(CONFIG_CAS_ID ==CONFIG_CAS_ID_XSM)
#include "ui_conditional_accept_feed.h"
#endif

#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
#include "ui_full_search.h"
#endif

#if(CONFIG_CAS_ID == CAS_ID_LX)
#include "ui_ca_entitle_info.h"
#include "ui_conditional_accept_pin.h"
#include "ui_conditional_accept_level.h"
#include "ui_conditional_accept_pair.h"
#include "ui_conditional_accept_antiauth.h"
#include "ui_ca_book_history.h"
#include "ui_email_mess_content.h"
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
#include "ui_conditional_accept.h"
#include "ui_conditional_accept_feed.h"
static cas_mail_headers_t *p_cas_mail_headers = NULL;
extern cas_mail_headers_t *g_get_mail_header_info();
#endif


// the information of focus change
typedef struct
{
  // the focus is changed or not
  BOOL is_changed;

  // the focus is changed from...
  u8 from_id;

  // the focus will change to...
  u8 to_id;
}focus_change_info_t;


// the information of menu management
typedef struct
{
  // the attribute of current menu
  menu_attr_t curn_menu_attr;

  // the root which will be automatic close by sys tmr
  u8 pre_autoclose_root;

  // enable or not what process the event about focus changed
  u8 enable_autoswitch;

  // the focus change info between 2 menus
  focus_change_info_t pre_focus_change;
}menu_manage_info_t;

static menu_manage_info_t g_menu_manage =
{
  // curn menu attr
  {ROOT_ID_BACKGROUND, PS_PLAY, OFF, SM_LOCK, NULL},
  // enable autoswtich
  ROOT_ID_INVALID, TRUE,
  // focus changed info
  {FALSE, ROOT_ID_INVALID, ROOT_ID_INVALID}
};

#define PRE_AUTOCLOSE_ROOT (g_menu_manage.pre_autoclose_root)

#define CURN_MENU_ROOT_ID (g_menu_manage.curn_menu_attr.root_id)
#define CURN_MENU_PLAY_STATE (g_menu_manage.curn_menu_attr.play_state)
#define CURN_MENU_SIGN_TYPE (g_menu_manage.curn_menu_attr.signal_msg)
#define CURN_MENU_CLOSE_TYPE (g_menu_manage.curn_menu_attr.auto_close)


static const menu_attr_t all_menu_attr[] =
{
	//  root_id,            play_state,   auto_close, signal_msg,   open
	{ROOT_ID_BACKGROUND, PS_PLAY, OFF, SM_OFF, NULL},
	{SN_ROOT_ID_MAINMENU, PS_PREV, OFF, SM_OFF, open_main_menu},
	{SN_ROOT_ID_SUBMENU, PS_KEEP, OFF, SM_LOCK, open_sub_menu},

	//wanghm
	{ROOT_ID_SYS_INFO, PS_KEEP, OFF, SM_OFF, open_sys_info},
	{SN_ROOT_ID_TRACK_SET, PS_KEEP, OFF, SM_LOCK, open_track_set},
	{SN_ROOT_ID_LANGUAGE_SET, PS_KEEP, OFF, SM_LOCK, open_language_set},
	{SN_ROOT_ID_SEARCH, PS_STOP, OFF, SM_BAR, open_manual_search},
	{SN_ROOT_ID_DISPLAY_SET, PS_KEEP, OFF, SM_LOCK, open_display_set},
	//wanghm end

	//lvcm
	{ROOT_ID_SENIOR_SUBMENU, PS_PREV, OFF, SM_LOCK, open_senior_sub_menu},
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CA_IPPV_PPT_DLG, PS_KEEP, OFF, SM_BAR, open_ca_ippv_ppt_dlg},
	{SN_ROOT_ID_CARD_INFO, PS_PREV, OFF, SM_LOCK, open_sn_ca_card_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
	{SN_ROOT_ID_CARD_INFO, PS_PREV, OFF, SM_LOCK, open_ca_card_info_adt_mg},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CA_INFO, PS_PREV, OFF, SM_LOCK, open_dvb_ca_info},
#endif
	//lvcm end

	{SN_ROOT_ID_SEARCH_RESULT, PS_STOP, OFF, SM_OFF, open_do_search},

	{ROOT_ID_PROG_LIST, PS_PREV/*PS_PLAY*/, OFF, SM_OFF/*SM_LOCK*/, open_prog_list},


#ifdef OTA_SUPPORT
	{ROOT_ID_OTA_SEARCH, PS_KEEP, OFF, SM_OFF, open_ota_search},  
#endif

	{ROOT_ID_DIALOG, PS_KEEP, OFF, SM_OFF, NULL},
	{ROOT_ID_PASSWORD, PS_KEEP, OFF, SM_OFF, NULL},
	{ROOT_ID_VOLUME, PS_KEEP, ON, SM_OFF, open_volume},
	{ROOT_ID_PROG_BAR, PS_PLAY, ON, SM_BAR, open_prog_bar},
	{ROOT_ID_NUM_PLAY, PS_KEEP, OFF, SM_OFF, open_num_play},
	{ROOT_ID_FAV_SET, PS_KEEP, OFF, SM_OFF, open_fav_set},
	{ROOT_ID_EPG, PS_PREV, OFF, SM_OFF, open_epg},
	{ROOT_ID_EPG_DETAIL, PS_KEEP, OFF, SM_OFF, open_epg_detail},

	{ROOT_ID_FAV_LIST, PS_PLAY, OFF, SM_OFF, open_fav_list},

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
#ifndef WIN32
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
#endif
#endif 

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
#ifndef WIN32
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_entitle_info},
#endif
#endif 

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
#endif 
	{ROOT_ID_CHANNEL_EDIT, PS_KEEP, OFF, SM_OFF, open_channel_edit},
#ifndef WIN32
	{ROOT_ID_EMAIL_MESS, PS_KEEP, OFF, SM_OFF, open_email_mess},
#endif
	{ROOT_ID_EMAIL_MESS, PS_KEEP, OFF, SM_OFF, open_email_mess},

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
	{ROOT_ID_EMAIL_MESS_CONTENT, PS_KEEP, OFF, SM_OFF, open_email_mess_content},
	{ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE,PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed_passive},
	{ROOT_ID_FORCE_EMAIL_CONTENT,PS_KEEP, OFF, SM_OFF, open_ca_force_msg},
	{ROOT_ID_CA_O_S_IPP_PROG,PS_KEEP, OFF, SM_OFF, open_order_unsubscribe_ipp},
	{ROOT_ID_CA_PIN_IPP,PS_KEEP, OFF, SM_OFF, open_pin_ipp},
	{ROOT_ID_CA_BOOK_HISTORY,PS_KEEP, OFF, SM_OFF, open_book_history},
	{ROOT_ID_HIDE_CA_MENU,PS_KEEP, OFF, SM_OFF, open_ca_hide_menu},
	{ROOT_ID_CA_BOOK_IPP_REAL_TIME,PS_KEEP, OFF, SM_OFF, open_real_time_ipp},
	{ROOT_ID_CA_NOTIFY,PS_KEEP, OFF, SM_OFF, open_ca_notify},
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
	{ROOT_ID_CA_BOOK_IPP_REAL_TIME,PS_KEEP, OFF, SM_OFF, open_real_time_ipp},
#endif

	{SN_ROOT_ID_SEARCH, PS_STOP, OFF, SM_BAR, open_manual_search},
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
	{ROOT_ID_FULL_SEARCH, PS_STOP, OFF, SM_BAR, open_full_search},
#endif
	{ROOT_ID_SYS_INFO, PS_KEEP, OFF, SM_OFF, open_sys_info},
	{ROOT_ID_PASSWORD_MODIFY, PS_KEEP, OFF, SM_OFF, open_password_modify},
	{ROOT_ID_TP_SET, PS_PREV, OFF, SM_LOCK, open_tp_set},
	{ROOT_ID_MANUAL_UPGRADE, PS_KEEP, OFF, SM_OFF, open_manual_upgrade},
	{ROOT_ID_RESTORE_FACTORY, PS_KEEP, OFF, SM_OFF, open_restore_factory},
#if(CONFIG_CAS_ID==CONFIG_CAS_ID_XSM)
	{ROOT_ID_CONDITIONAL_ACCEPT, PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed},
#else
	{ROOT_ID_CONDITIONAL_ACCEPT, PS_KEEP, OFF, SM_OFF, open_conditional_accept},
#endif
	{ROOT_ID_SIGNAL_CHECK, PS_STOP, OFF, SM_BAR, open_signal_check},
	{ROOT_ID_PRO_CLASSIFY, PS_KEEP, OFF, SM_OFF, open_pro_classify},
	{ROOT_ID_PREBOOK_MANAGE, PS_KEEP, OFF, SM_OFF, open_prebook_manage},
#ifndef WIN32
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CONDITIONAL_ACCEPT_INFO, PS_KEEP, OFF, SM_OFF, open_conditional_accept_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_SP_LIST, PS_KEEP, OFF, SM_OFF, open_dvb_ca_sp_id_list},
	{ROOT_ID_SP_LIST2, PS_KEEP, OFF, SM_OFF, open_dvb_ca_sp_id_list2},
	{ROOT_ID_CONDITIONAL_ACCEPT_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_operator_info},
	{ROOT_ID_CONDITIONAL_ACCEPT_PAIR, PS_KEEP, OFF, SM_OFF, open_stb_card_pair_info},
#endif
//	{ROOT_ID_CONDITIONAL_ACCEPT_PAIR, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pair},
	#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_conditional_accept_level},
	{ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_conditional_accept_worktime},
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pin},
	#elif(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_worktime},
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_pin},
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_level},
	#endif
	{ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, PS_KEEP, OFF, SM_OFF, open_conditional_accept_mother},
	{ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, PS_KEEP, OFF, SM_OFF, open_conditional_accept_antiauth},
	{ROOT_ID_CONDITIONAL_ACCEPT_FEED, PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed},
#endif

//lvcm
#ifdef WIN32
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_conditional_accept_worktime},
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pin},
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_conditional_accept_level},
	#elif(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_worktime},
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_pin},
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_dvbcas_accept_level},
	#endif
	#if(CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CONDITIONAL_ACCEPT_INFO, PS_KEEP, OFF, SM_OFF, open_conditional_accept_info},
	#endif
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_SP_LIST, PS_KEEP, OFF, SM_OFF, open_dvb_ca_sp_id_list},
	{ROOT_ID_SP_LIST2, PS_KEEP, OFF, SM_OFF, open_dvb_ca_sp_id_list2},
	{ROOT_ID_CONDITIONAL_ACCEPT_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_operator_info},
    #endif
	{ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, PS_KEEP, OFF, SM_OFF, open_conditional_accept_antiauth},
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_entitle_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
	{ROOT_ID_CONDITIONAL_ACCEPT, PS_KEEP, OFF, SM_OFF, open_conditional_accept},
	{ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, PS_KEEP, OFF, SM_OFF, open_conditional_accept_mother},
#endif
#endif

#ifdef WIN32
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
	{ROOT_ID_CA_MOTHER, PS_KEEP, OFF, SM_OFF, open_ca_mother},
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
	{ROOT_ID_CHARACTER_CODE, PS_KEEP, OFF, SM_OFF, open_ca_character_code},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_card_info},
#endif
#endif

//lvcm end

#if((CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV))
	{ROOT_ID_CA_OPE, PS_KEEP, OFF, SM_OFF, open_ca_operate},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_QL)
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
	{ROOT_ID_CA_INFO, PS_KEEP, OFF, SM_OFF, open_ca_info},
	{ROOT_ID_CONDITIONAL_ACCEPT, PS_KEEP, OFF, SM_OFF, open_conditional_accept},
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_conditional_accept_level},
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pin},
	{ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_conditional_accept_worktime},
	{ROOT_ID_CONDITIONAL_ACCEPT_PAIR, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pair},
	{ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, PS_KEEP, OFF, SM_OFF, open_conditional_accept_mother},
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
	{ROOT_ID_PROVIDER_INFO, PS_KEEP, OFF, SM_OFF, open_provider_info},
	{ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_conditional_accept_level},	
	{ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pin},
	{ROOT_ID_CONDITIONAL_ACCEPT_FEED, PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed},
	{ROOT_ID_CA_IPP, PS_KEEP, OFF, SM_BAR, open_ca_ipp},
    {ROOT_ID_CA_PROMPT, PS_PLAY, OFF, SM_OFF, open_ca_prompt},
	{ROOT_ID_CA_NOTIFY,PS_KEEP, OFF, SM_OFF, open_ca_notify},
	{ROOT_ID_ALERT_MESS, PS_KEEP, OFF, SM_OFF, open_alert_mess},
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
	{ROOT_ID_CONDITIONAL_ACCEPT_FEED, PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed},
#endif

	//{ROOT_ID_GAME_INN, PS_PREV, OFF, SM_OFF, open_game_inn},
#ifndef WIN32
	{ROOT_ID_CALENDAR,    PS_KEEP,       OFF, SM_OFF,  open_calendar_menu},

#if(CONFIG_CAS_ID != CONFIG_CAS_ID_TF)
#endif
#endif
	//lvcm
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CA_CARD_INFO, PS_PREV, OFF, SM_OFF,  open_ca_card_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CA_CARD_INFO, PS_PREV, OFF, SM_OFF,  open_dvb_ca_card_info},
	{ROOT_ID_EMAIL_MESS_CONTENT, PS_KEEP, OFF, SM_OFF, open_email_mess_content},
#endif
	{ROOT_ID_CHANNEL_NUM, PS_KEEP, OFF, SM_OFF, NULL},
	{ROOT_ID_CHANNEL_WARN, PS_KEEP, ON, SM_OFF, NULL},
	{ROOT_ID_CHANNEL_SAVE, PS_KEEP, OFF, SM_OFF, NULL},
	{SN_ROOT_ID_PRO_INFO, PS_KEEP, OFF, SM_BAR, open_pro_info},
#ifndef WIN32
	{ROOT_ID_CA_ENTITLE_EXP_DLG, PS_KEEP, ON, SM_OFF, open_ca_entitle_expire_dlg},
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_XSM)
	{ROOT_ID_CA_CARD_UPDATE, PS_KEEP, OFF, SM_OFF, open_ca_card_update_info},
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	{ROOT_ID_CA_CARD_INFO, PS_PREV, OFF, SM_OFF, open_ca_card_info},
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	{ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_dvb_ca_card_info},
#endif
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
	{ROOT_ID_CA_MOTHER, PS_KEEP, OFF, SM_BAR,open_ca_mother},
#ifdef CAS_MG_312_VER
	{ROOT_ID_CHARACTER_CODE, PS_KEEP, OFF, SM_BAR,open_ca_character_code},
	{ROOT_ID_CA_BURSE_CHARGE, PS_KEEP, OFF, SM_BAR,open_ca_burse_dlg},
	{ROOT_ID_EMAIL_MESS_CONTENT, PS_KEEP, OFF, SM_OFF, open_email_mess_content},
	{ROOT_ID_CA_PRE_AUTH, PS_KEEP, OFF, SM_OFF, open_ca_pre_auth},
	{ROOT_ID_CA_PPV_ICON, PS_KEEP, OFF, SM_OFF, open_ca_ppv_icon},
	{ROOT_ID_CA_PAIR, PS_KEEP, OFF, SM_OFF, open_ca_pair_dlg},
#endif
#endif

#if(CONFIG_CAS_ID == CAS_ID_LX)
	{ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_BAR,open_ca_entitle_info},
	{ROOT_ID_CA_BOOK_HISTORY,PS_KEEP, OFF, SM_OFF, open_book_history},
	{ROOT_ID_EMAIL_MESS_CONTENT, PS_KEEP, OFF, SM_OFF, open_email_mess_content},
#endif

};

#define MENU_CNT    (sizeof(all_menu_attr) / sizeof(menu_attr_t))

#define MENU_GET_ROOT_ID(idx) (all_menu_attr[idx].root_id)
#define MENU_GET_PLAY_STATE(idx) (all_menu_attr[idx].play_state)
#define MENU_GET_SIGN_TYPE(idx) (all_menu_attr[idx].signal_msg)
#define MENU_GET_CLOSE_TYPE(idx) (all_menu_attr[idx].auto_close)
#define MENU_GET_OPEN_FUNC(idx) (all_menu_attr[idx].open_func)

static const logo_attr_t all_logo_attr[] =
{
  {ROOT_ID_INVALID, LOGO_BLOCK_ID_M0},
};
#define LOGO_CNT    (sizeof(all_logo_attr) / sizeof(logo_attr_t))

#define LOGO_GET_ROOT_ID(idx) (all_logo_attr[idx].root_id)
#define LOGO_GET_LOGO_ID(idx) ((u8)all_logo_attr[idx].logo_id)

static const preview_attr_t all_preview_attr[] =
{

  {ROOT_ID_PROG_LIST, {(SN_SUBMENU_CONT_X+ SN_PROG_PREV_X+35), SN_PROG_PREV_Y+SN_SUBMENU_CONT_Y+140/*(SUBMENU_CONT_Y + PLIST_PREV_Y)*/, SN_PROG_PREV_X + SN_PROG_PREV_W + 13/*(SUBMENU_CONT_X + PLIST_PREV_X + PLIST_PREV_W)*/, SN_PROG_PREV_Y + SN_PROG_PREV_H +88/*(SUBMENU_CONT_Y+ PLIST_PREV_Y + PLIST_PREV_H)*/} },


  {ROOT_ID_EPG, {SN_EPG_PREV_X+20/*(EPG_MENU_X + EPG_PREV_X)*/, SN_EPG_PREV_Y+102/*(EPG_MENU_Y + EPG_PREV_Y)*/, SN_EPG_PREV_X + SN_EPG_PREV_W+13/*(EPG_MENU_X + EPG_PREV_X + EPG_PREV_W)*/, SN_EPG_PREV_Y + SN_EPG_PREV_H + 90/*(EPG_MENU_Y + EPG_PREV_Y + EPG_PREV_H)*/} },


  {SN_ROOT_ID_MAINMENU, {(MAINMENU_CONT_X + MAINMENU_PREV_X) +20, (MAINMENU_CONT_Y + MAINMENU_PREV_Y) + 98, (MAINMENU_CONT_X + MAINMENU_PREV_X + MAINMENU_PREV_W)+13, (MAINMENU_CONT_Y + MAINMENU_PREV_Y + MAINMENU_PREV_H)+40+50} },


  {ROOT_ID_MANUAL_UPGRADE, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },
  {ROOT_ID_PROGRAM_SEARCH, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },
  {ROOT_ID_RESTORE_FACTORY, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },
 // {ROOT_ID_CONDITIONAL_ACCEPT, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },

  #if ((CONFIG_CAS_ID != CONFIG_CAS_ID_WF) && (CONFIG_CAS_ID != CONFIG_CAS_ID_XSM))
  #endif 
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV))
  {ROOT_ID_CA_OPE, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },
  #endif
//  {ROOT_ID_CONDITIONAL_ACCEPT_PAIR, {(SUBMENU_CONT_X + SUBMENU_PREV_X), (SUBMENU_CONT_Y + SUBMENU_PREV_Y), (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)} },

  {ROOT_ID_GAME_INN, {(GAME_INN_MENU_X + GAME_INN_PREV_X), (GAME_INN_MENU_Y + GAME_INN_PREV_Y), (GAME_INN_MENU_X + GAME_INN_PREV_X + GAME_INN_PREV_W), (GAME_INN_MENU_Y + GAME_INN_PREV_Y + GAME_INN_PREV_H)} },

};

#define PREVIEW_CNT    (sizeof(all_preview_attr) / sizeof(preview_attr_t))

static const u8 fullscreen_root[] =
{
  ROOT_ID_BACKGROUND,
  ROOT_ID_VOLUME,
  ROOT_ID_MOSNVOD_VOLUME,
  ROOT_ID_PROG_BAR,
  ROOT_ID_MUTE,
  ROOT_ID_PAUSE,
  ROOT_ID_SIGNAL,
  ROOT_ID_NOTIFY,
  ROOT_ID_NUM_PLAY,
  ROOT_ID_CA_FINGER,  
  ROOT_ID_CA_BROADCAST,
  ROOT_ID_CA_FREE_PREVIEW,
  #if (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  ROOT_ID_CA_BOOK_IPP_REAL_TIME,
  ROOT_ID_FORCE_EMAIL_CONTENT,
  ROOT_ID_EMAIL_PIC,
  #endif
 };

#define FULLSCREEN_CNT    (sizeof(fullscreen_root) / sizeof(u8))

static u8 popup_root[] =
{
  ROOT_ID_DIALOG,
  ROOT_ID_PASSWORD,
  ROOT_ID_FAV_SET,
  ROOT_ID_CA_IPPV_PPT_DLG,
};

#define POPUP_CNT    (sizeof(popup_root) / sizeof(u8))

BOOL ui_is_preview_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < PREVIEW_CNT; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


u8 ui_get_preview_menu(void)
{
  u8 i, root_id;

  for(i = 0; i < PREVIEW_CNT; i++)
  {
    root_id = all_preview_attr[i].root_id;
    if (fw_find_root_by_id(root_id) != NULL)
    {
      return root_id;
    }
  }
  return ROOT_ID_INVALID;
}

//Can show mute /signal window at the same time
//Other windows can be opened
BOOL ui_is_fullscreen_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < FULLSCREEN_CNT; i++)
  {
    if(fullscreen_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL ui_is_popup_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < POPUP_CNT; i++)
  {
    if(popup_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


static RET_CODE manage_tmr_create(void)
{
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  return fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                       osd_set.timeout * 1000, FALSE);
}


static RET_CODE manage_tmr_destroy(void)
{
  return fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU);
}


RET_CODE manage_tmr_reset(void)
{
#if 0
  manage_tmr_destroy();
  return manage_tmr_create();
#else
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                      osd_set.timeout * 1000) != SUCCESS)
  {
    return manage_tmr_create();
  }

  return SUCCESS;
#endif
}

static RET_CODE manage_tmr2_create(void)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if(cfg.customer == CUSTOMER_JINGHUICHENG_QZ)
  {
      return fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CLOSE_ALL_MENUS, 15 * 1000, FALSE);
  }
  else
  {
    return SUCCESS;
  }
}

RET_CODE manage_tmr2_reset(void)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if((cfg.customer == CUSTOMER_TAIHUI) ||
           (cfg.customer == CUSTOMER_JINGHUICHENG_QZ))
    {
      if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CLOSE_ALL_MENUS, 15 * 1000) != SUCCESS)
      {
        return manage_tmr2_create();
      }
      
    }
  return SUCCESS;
}


static u8 manage_find_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < MENU_CNT; i++)
  {
    if(MENU_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}


static u8 manage_find_logo(u8 root_id)
{
  u8 i;

  for(i = 0; i < LOGO_CNT; i++)
  {
    if(LOGO_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}


u8 manage_find_preview(u8 root_id, rect_t *orc)
{
  u8 i;
  s16 x, y;
  disp_sys_t std;

  empty_rect(orc);
  std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));

  for(i = 0; i < PREVIEW_CNT; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      copy_rect(orc, &all_preview_attr[i].position);
      #if 0
      /* convert coordinate to screen */
      switch(std)
      {
        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
          x = SCREEN_POS_NTSC_L, y = SCREEN_POS_PAL_T;//simon.lv
          break;
        default:
          x = SCREEN_POS_PAL_L, y = SCREEN_POS_PAL_T;
      }
      #else
      x = SCREEN_POS_PAL_L, y = SCREEN_POS_PAL_T;
      #endif
      offset_rect(orc, x, y);
      return i;
    }
  }
  return INVALID_IDX;
}


BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height)
{
  u8 i;

  for(i = 0; i < PREVIEW_CNT; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      *left = all_preview_attr[i].position.left;
      *top = all_preview_attr[i].position.top;
      *width = RECTW(all_preview_attr[i].position);
      *height = RECTH(all_preview_attr[i].position);
      return TRUE;
    }
  }
  return FALSE;
}


//we should make all background feature work well after register manager
void manage_init(void)
{
  manage_tmr_create();
  manage_tmr2_create();
}


//special : for change the timing of show logo and show OSD
void manage_logo2preview(u8 root_id)
{
  u8 idx;
  rect_t orc;

  idx = manage_find_logo(root_id);
  if(idx != INVALID_IDX)
  {
    ui_show_logo(LOGO_GET_LOGO_ID(idx));
  }
  idx = manage_find_preview(root_id, &orc);
  if(idx != INVALID_IDX)
  {
    avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                      orc.left, orc.top, orc.right, orc.bottom);
    avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                       orc.left, orc.top, orc.right, orc.bottom);
  }
//  av_video_output_enable();
}


static u8 _get_menu_type(u8 root_id)
{
  if(ui_is_fullscreen_menu(root_id))
  {
    return MENU_TYPE_FULLSCREEN;
  }
  else if(ui_is_preview_menu(root_id))
  {
    return MENU_TYPE_PREVIEW;
  }
  else if(ui_is_popup_menu(root_id))
  {
    return MENU_TYPE_POPUP;
  }
  else
  {
    return MENU_TYPE_NORMAL;
  }
}


void manage_enable_autoswitch(BOOL is_enable)
{
  g_menu_manage.enable_autoswitch = is_enable;
}


static void _switch_playstatus(u8 from_id, u8 to_id)
{
  u8 idx, new_ps;
  rect_t orc;
  menu_attr_t *p_curn_attr;
  u8 old_status = MENU_TYPE_NORMAL;

  old_status = _get_menu_type(from_id);
  p_curn_attr = &g_menu_manage.curn_menu_attr;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    return;
  }
  else
  {
    new_ps = MENU_GET_PLAY_STATE(idx);
  }
  if(new_ps == PS_KEEP)
  {
    return;
  }

  if(new_ps == CURN_MENU_PLAY_STATE)
  {
    if( (to_id == ROOT_ID_PROG_LIST)
      || (to_id == SN_ROOT_ID_SUBMENU)
      || (to_id == SN_ROOT_ID_MAINMENU)
      || (to_id == ROOT_ID_EPG)
      || (to_id == ROOT_ID_GAME_INN)
      || (to_id == ROOT_ID_TETRIS)
      || (to_id == ROOT_ID_GAME_BWC)
      || (to_id == ROOT_ID_GAME_SNAKE)
      || (to_id == ROOT_ID_PROGRAM_SEARCH))
    {
    }
    else
    {
      if(new_ps == PS_LOGO)
      {
        ui_set_front_panel_by_str("----");
      }
      return;
    }
  }

  //we should auto switch logo
  switch(new_ps)
  {
    case PS_PREV:
      OS_PRINTF("UI: switch to PREV..........\n");
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_LOGO:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
#if 0
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                              orc.left, orc.top, orc.right, orc.bottom);
            avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                               orc.left, orc.top, orc.right, orc.bottom);
#else
            fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_SWITCH_PREVIEW, to_id, 0);
#endif
            OS_PRINTF("UI: swithced to PREV........\n");
          }
          break;
        default:
          OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
#if 0
            OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                              orc.left, orc.top, orc.right, orc.bottom);
            OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
            avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                              orc.left, orc.top, orc.right, orc.bottom);
            OS_PRINTF("UI: swithced to PREV........\n");
#else
            if((old_status != MENU_TYPE_POPUP) && (from_id != to_id))
            {
              fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_SWITCH_PREVIEW, to_id, 0);
            }
#endif
          }
      }
      break;
    case PS_PLAY:
      OS_PRINTF("UI: switch to PLAY..........\n");
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_STOP:
        case PS_PREV:
          avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
          ui_play_curn_pg();
          break;
        case PS_LOGO:
          if((to_id == ROOT_ID_PROG_LIST)
            &&(sys_status_get_curn_prog_mode() == CURN_MODE_RADIO))
          {
            ui_show_logo(LOGO_BLOCK_ID_M1);
          }
          else
          {
            ui_play_curn_pg();
          }

          break;
        default:
          ui_play_curn_pg();
      }
      break;
    case PS_LOGO:
      ui_set_front_panel_by_str("----");
      OS_PRINTF("UI: switch to LOGO..........\n");
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));

          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        case PS_PLAY:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        default:
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
      }
      break;
    case PS_STOP:
      ui_set_front_panel_by_str("----");
      /* just stop playing */
      OS_PRINTF("UI: switch to STOP..........\n");
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
      /* stop monitor */
      break;

    default:
      /* do nothing */;
  }

 CURN_MENU_PLAY_STATE = new_ps;
}


static void _switch_signalcheck(u8 from_id, u8 to_id)
{
  u8 idx, new_sm;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    new_sm = SM_OFF;
  }
  else
  {
    new_sm = MENU_GET_SIGN_TYPE(idx);
  }

  CURN_MENU_SIGN_TYPE = new_sm;
}

static void _switch_autoopen(u8 from_id, u8 to_id)
{
  u8 old_status, new_status;
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
  u8 i;
  #endif

  BOOL is_chk_mute = FALSE;
  BOOL is_chk_pause = FALSE;
  BOOL is_chk_notify = FALSE;
  BOOL is_chk_signal = FALSE;
  BOOL is_chk_email = FALSE;
  BOOL is_chk_finger = FALSE;
  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);
  DEBUG(MAIN, INFO, "from_id:%d, to_id:%d\n", from_id, to_id);
  DEBUG(MAIN, INFO, "old_status:%d, new_status:%d\n", old_status, new_status);
  if(new_status != old_status
    && old_status != MENU_TYPE_POPUP)
  {
    switch(new_status)
    {
      case MENU_TYPE_FULLSCREEN:
        is_chk_mute = is_chk_pause = is_chk_notify = is_chk_signal = is_chk_email = TRUE;
        break;
      case MENU_TYPE_PREVIEW:
        is_chk_mute = is_chk_pause = is_chk_signal = TRUE;
        break;
      default:
        break;
    }
  }
  else if((MENU_TYPE_PREVIEW == new_status)
              &&(from_id != to_id)
              &&(old_status != MENU_TYPE_POPUP))
  {
    is_chk_mute = is_chk_pause = is_chk_signal = TRUE;
  }
else if((MENU_TYPE_FULLSCREEN == new_status)
            && (from_id != to_id))
{
    is_chk_finger = TRUE;
}
  if(ui_is_mute() && is_chk_mute)
  {
    open_mute(0, 0);
  }

  if(ui_is_pause() && is_chk_pause)
  {
    open_pause(0, 0);
  }
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_DS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG))
  if(ui_is_finger_show() && is_chk_finger)
  {
    UI_PRINTF("[Menu_Manage]redraw finger message \n");
    open_finger(0, 0);   //don't remove!!!!!!!!!
  }
  #endif
  if((is_chk_signal))
  {
    update_signal();
  }

  if((to_id == ROOT_ID_BACKGROUND)
    && (old_status != MENU_TYPE_FULLSCREEN)
    && (old_status != MENU_TYPE_POPUP)
    && (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {
    if(IDS_LOCK != get_singal_strid())
    {
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
    }
  }

 if(ui_is_email()
    && is_chk_email)
  {
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
    p_cas_mail_headers = g_get_mail_header_info();
    if( p_cas_mail_headers->max_num > 100)
    {
      p_cas_mail_headers->max_num = 0;
    }

    if(from_id != ROOT_ID_EMAIL_MESS)
    {
    #endif
      update_email_picture(EMAIL_PIC_SHOW);
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_GS)
    }

    OS_PRINTF("p_cas_mail_headers->max_num = %d\n",p_cas_mail_headers->max_num);
    for(i = 0; i < p_cas_mail_headers->max_num; i++)
    {
      OS_PRINTF("p_cas_mail_headers->p_mail_head[%d].new_email= %d\n",i,p_cas_mail_headers->p_mail_head[i].new_email);
       if((p_cas_mail_headers->p_mail_head[i].new_email  >0))
      {
         update_email_picture(EMAIL_PIC_SHOW);
      }
    }
    #endif
  }

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_BOYUAN)
   if(new_status == MENU_TYPE_FULLSCREEN)
  {
      OS_PRINTF("\n##debug DisplayCtrl(1) ui exist\n");
      ad_able_display_boyuan(TRUE);
   }
   else
  {
      OS_PRINTF("\n##debug DisplayCtrl(0) into ui\n");
     ad_able_display_boyuan(FALSE);
  }

#endif

}


static void _switch_autoclose(u8 from_id, u8 to_id)
{
  u8 idx, new_auto;
  u8 old_status, new_status;

  if(from_id == to_id)
  {
    if(from_id == ROOT_ID_PROG_BAR)
      CURN_MENU_CLOSE_TYPE = ON;
    return;
  }

  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);

  DEBUG(MAIN, INFO, "from_id:%d, to_id:%d\n", from_id, to_id);
  DEBUG(MAIN, INFO, "old_status:%d, new_status:%d\n", old_status, new_status);
  if((old_status == MENU_TYPE_FULLSCREEN || old_status == MENU_TYPE_PREVIEW)
    && (old_status != new_status) /*preview or normal*/
    && new_status != MENU_TYPE_POPUP)
  {
    if(ui_is_mute())
    {
      close_mute();
    }
    if(ui_is_pause())
    {
      close_pause();
    }
    if(ui_is_notify())
    {
      close_notify();
    }
    
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
    if(ui_is_free_pre())
    {
      clean_cas_preview();
    }
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
   if(ui_is_finger_show())
    {
      close_ca_finger_menu();
    }
#endif

    /* try to close */
    close_signal();
  }

  if(MENU_TYPE_PREVIEW == new_status)
  {
    if(ui_is_mute())
    {
      close_mute();
    }
    if(ui_is_pause())
    {
      close_pause();
    }
    if(ui_is_notify())
    {
      close_notify();
    }
    #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
    if(ui_is_finger_show())
    {
      close_ca_finger_menu();
    }
    #endif
    /* try to close */
    close_signal();
  }

  idx = manage_find_menu(to_id);
  DEBUG(MAIN, INFO, "idx:%d\n", idx);
  if(idx == INVALID_IDX)
  {
    new_auto = OFF;
  }
  else
  {
    new_auto = MENU_GET_CLOSE_TYPE(idx);
  }

  UI_PRINTF("line:%d switch_autoclose: from %d to %d \n", __LINE__, from_id, to_id);
  DEBUG(MAIN, INFO, "CURN_MENU_CLOSE_TYPE:%d\n", CURN_MENU_CLOSE_TYPE);
  DEBUG(MAIN, INFO, "PRE_AUTOCLOSE_ROOT:%d,new_auto:%d\n", PRE_AUTOCLOSE_ROOT, new_auto);
  if(CURN_MENU_CLOSE_TYPE == OFF)
  {
    if(new_auto == OFF)
    {
      PRE_AUTOCLOSE_ROOT = INVALID_IDX;
      return;
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: create tmr \n");
      manage_tmr_create();
    }
  }
  else
  {
    if(new_auto == OFF)
    {
      PRE_AUTOCLOSE_ROOT = INVALID_IDX;
      UI_PRINTF("switch_autoclose: destroy tmr \n");
      manage_tmr_destroy();
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: reset tmr \n");
      manage_tmr_reset();
    }
  }

  CURN_MENU_CLOSE_TYPE = new_auto;
}


void manage_auto_switch(void)
{
  focus_change_info_t *p_info = &g_menu_manage.pre_focus_change;
DEBUG(MAIN, INFO, "p_info->is_changed:%d\n", p_info->is_changed);
  DEBUG(MAIN, INFO, "p_info->from_id:%d\n", p_info->from_id);
  DEBUG(MAIN, INFO, "p_info->to_id:%d\n", p_info->to_id);
  if(p_info->is_changed)
  {
    p_info->is_changed = FALSE;
    _switch_autoopen(p_info->from_id, p_info->to_id);
    _switch_signalcheck(p_info->from_id, p_info->to_id);
  }
}


RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2)
{
  open_menu_t open_func;
  u8 idx;
  RET_CODE ret;
  OS_PRINTF("open menu id %d\n",root_id);
  idx = manage_find_menu(root_id);
  if(idx == INVALID_IDX)
  {
    return ERR_NOFEATURE;
  }

  open_func = MENU_GET_OPEN_FUNC(idx);
  if((open_func == NULL))
  {
    return ERR_NOFEATURE;
  }
DEBUG(MAIN, INFO, "\n");
  ret = (*open_func)(para1, para2);
  DEBUG(MAIN, INFO, "ret:%d\n", ret);
  if(ret == SUCCESS)
  {
    manage_auto_switch();
  }
  return ret;
}


RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2)
{
  if(!fw_destroy_mainwin_by_id(root_id))
  {
    return ERR_FAILURE;
  }

  manage_auto_switch();
  return SUCCESS;
}


RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;
  
  DEBUG_ENABLE_MODE(EPG,INFO);

  p_root = fw_find_root_by_id(root_id);
  if(p_root != NULL)
  {
    return ctrl_process_msg(p_root, msg, para1, para2);
  }

  return ERR_NOFEATURE;
}


void manage_autoclose(void)
{
  u8 root_id;
  control_t *p_obj;

  root_id = PRE_AUTOCLOSE_ROOT;

  if(fw_get_focus_id() == ROOT_ID_PROG_BAR)
  {
    p_obj = fw_find_root_by_id(ROOT_ID_PROG_BAR);
  }
  else
  {
    p_obj = fw_find_root_by_id(root_id);
  }

  if(NULL == p_obj)
  {
    return;
  }

  if(ctrl_process_msg(p_obj, MSG_EXIT, 0, 0) != SUCCESS)
  {
    fw_destroy_mainwin_by_root(p_obj);
  }
}


menu_attr_t *manage_get_curn_menu_attr(void)
{
  return &g_menu_manage.curn_menu_attr;
}


static void _pre_enter_preview(u8 root_id, u32 para1, u32 para2)
{
  u8 curn_mode = sys_status_get_curn_prog_mode();

  switch(root_id)
  {
    case ROOT_ID_PROG_LIST:
      if(((curn_mode == CURN_MODE_TV) && (para1 == 1))     //enter tv list on radio mode.
        ||((curn_mode == CURN_MODE_RADIO) && (para1 == 0)))//enter radio list on tv mode.
      {
        ui_enable_video_display(FALSE);
      }
      break;
    default:
      /* do nothing */;
  }
}


static void _before_open_menu(u8 old_root, u8 new_root, u32 para1, u32 para2)
{
  u8 old_status, new_status;

  old_status = _get_menu_type(old_root);
  new_status = _get_menu_type(new_root);

  if(new_status == MENU_TYPE_PREVIEW)
  {
    _pre_enter_preview(new_root, para1, para2);
  }
}


static void _before_close_menu(u8 root_id)
{
}

void ui_close_all_mennus(void)
{
  u8 to = ROOT_ID_INVALID, from = fw_get_focus_id();

  manage_enable_autoswitch(FALSE);
  to = fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);
DEBUG(MAIN, INFO, "from:%d, to:%d\n", from, to);
  ui_menu_manage(WINDOW_MANAGE_FOCUS_CHANGED, from, to);
  manage_auto_switch();
}

void ui_return_to_mainmenu(void)
{
  u8 from = fw_get_focus_id();

  if(from == SN_ROOT_ID_MAINMENU)
  {
    return;
  }

  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(FALSE);

  if(fw_find_root_by_id(SN_ROOT_ID_MAINMENU) == NULL)
  {
    manage_open_menu(SN_ROOT_ID_MAINMENU,0,0xFFFF);
  }
  manage_enable_autoswitch(TRUE);
DEBUG(MAIN, INFO, "\n");
  ui_menu_manage(WINDOW_MANAGE_FOCUS_CHANGED, from, SN_ROOT_ID_MAINMENU);
  manage_auto_switch();
}



static RET_CODE on_initialize(u32 event, u32 para1, u32 para2)
{
  // start app framework
  ap_frm_set_ui_msgq_id((s32)para1);
  ap_frm_run();
  
  ui_set_dvbc_allfreq_symb(0);  /***must is o for main tp synb***/
 OS_PRINTF("start ota monitor\n");
#ifndef OTA_CHECK_DEFAULT
  ui_enable_uio(TRUE);
  ui_enable_playback(TRUE);
  #if(CONFIG_CAS_ID  != CONFIG_CAS_ID_DS)
  #ifndef WIN32
  ui_init_ca();
  #endif
  #endif
  ui_time_init();

  /* activate EPG */
  ui_epg_init();
  ui_epg_start();

  // open signal monitor
  ui_init_signal();
#else
 OS_PRINTF("start ota monitor\n");
 ui_ota_monitor_init();
 ui_ota_monitor_start(0);
#endif

  return SUCCESS;
}


static RET_CODE on_pre_open(u32 event, u32 para1, u32 para2)
{
  u8 new_root = (u8)(para1);

  _before_open_menu(CURN_MENU_ROOT_ID, new_root, para1, para2);

  return SUCCESS;
}


static RET_CODE on_pre_close(u32 event, u32 para1, u32 para2)
{
  u8 old_root = (u8)(para1);

  _before_close_menu(old_root);

  return SUCCESS;
}

static RET_CODE on_after_close(u32 event, u32 para1, u32 para2)
{
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  return SUCCESS;
}

static RET_CODE on_focus_changed(u32 event, u32 para1, u32 para2)
{
  focus_change_info_t *p_info = NULL;
  u8 old_root, new_root;
  control_t *p_root = NULL;
  old_root = (u8)(para1);
  new_root = (u8)(para2);

  if(g_menu_manage.enable_autoswitch)
  {
    p_info = &g_menu_manage.pre_focus_change;

    if(p_info->is_changed)
    {
      /* just update to */
      p_info->to_id = new_root;
    }
    else
    {
      /* start to save */
      p_info->is_changed = TRUE;
      p_info->from_id = old_root;
      p_info->to_id = new_root;
    }
    /* because paint menu is too slow, so move it from auto_switch */
    _switch_playstatus(p_info->from_id, p_info->to_id);
	DEBUG(MAIN, INFO, "p_info->from_id:%d, p_info->to_id:%d\n", p_info->from_id, p_info->to_id);
    _switch_autoclose(p_info->from_id, p_info->to_id);
	DEBUG(MAIN, INFO, "\n");
  }
  #ifndef WIN32
  if( (p_info->to_id == SN_ROOT_ID_MAINMENU)
    && (p_info->from_id != p_info->to_id)
    && (p_info->from_id != ROOT_ID_BACKGROUND))
  {
    p_root = fw_find_root_by_id(SN_ROOT_ID_MAINMENU);
    if(NULL !=  p_root)
    {
        OS_PRINTF("send MSG_REFRESH_ADS_PIC to mainmenu\n");
        fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
    }
  }
  else   if( (p_info->to_id == SN_ROOT_ID_SUBMENU)
    && (p_info->from_id != p_info->to_id)
    && (p_info->from_id != ROOT_ID_BACKGROUND))
  {
    p_root = fw_find_root_by_id(SN_ROOT_ID_SUBMENU);
    if(NULL !=  p_root)
    {
        OS_PRINTF("send MSG_REFRESH_ADS_PIC to mainmenu\n");
        //fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
    }
  }
  else if((p_info->from_id == SN_ROOT_ID_MAINMENU)
    && (p_info->from_id != p_info->to_id)
    && (!ui_is_popup_menu(p_info->to_id)) )
  {
    OS_PRINTF("p_info->from_id=%d, p_info->to_id=%d\n",p_info->from_id,p_info->to_id);
    ui_pic_stop();
  }
  #endif
  DEBUG(MAIN, INFO, "new_root:%d\n", new_root);
  CURN_MENU_ROOT_ID = new_root;
  return SUCCESS;
}


static RET_CODE on_uio_event(u32 event, u32 para1, u32 para2)
{
  if(CURN_MENU_CLOSE_TYPE == ON)
  {
    manage_tmr_reset();
  }

  uio_reset_sleep_timer();

  manage_tmr2_reset();

  return SUCCESS;
}


BEGIN_WINDOW_MANAGE(ui_menu_manage)
ON_MENU_EVENT(WINDOW_MANAGE_ON_UIO_EVENT, on_uio_event)
ON_MENU_EVENT(WINDOW_MANAGE_FOCUS_CHANGED, on_focus_changed)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_OPEN, on_pre_open)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_CLOSE, on_pre_close)
ON_MENU_EVENT(WINDOW_MANAGE_INIT, on_initialize)
ON_MENU_EVENT(WINDOW_MANAGE_CLOSED, on_after_close)
END_WINDOW_MANAGE(ui_menu_manage)
