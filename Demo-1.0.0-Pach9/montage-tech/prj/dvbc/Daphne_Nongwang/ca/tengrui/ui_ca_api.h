/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_API_H__
#define __UI_CA_API_H__

#define UI_MAX_PIN_LEN (6)
enum ca_msg
{
  MSG_CA_INIT_OK= MSG_EXTERN_BEGIN + 500,   //0x61F4
  MSG_CA_OPE_INFO,
  MSG_CA_ENT_INFO,
  MSG_CA_IPV_INFO,
  MSG_CA_BUR_INFO,
  MSG_CA_CARD_INFO,
  MSG_CA_PAIRE_INFO,
  MSG_CA_MON_CHILD_INFO,
  MSG_CA_MAIL_HEADER_INFO,
  MSG_CA_MAIL_BODY_INFO,
  MSG_CA_WORK_TIME_INFO,
  MSG_CA_GET_WORK_TIME_INFO,
  MSG_CA_PIN_SET_INFO,
  MSG_CA_PIN_VERIFY_INFO,
  MSG_CA_CLEAR_DISPLAY,
  MSG_CA_EVT_NOTIFY,
  MSG_CA_FORCE_CHANNEL_INFO,
  MSG_CA_UNFORCE_CHANNEL_INFO,
  MSG_CA_RATING_GET,
  MSG_CA_RATING_SET,
  MSG_CA_SHOW_ROLL_MSG,
  MSG_CA_SHOW_FINGER,
  MSG_CA_HIDE_FINGER,
  MSG_CA_SHOW_URGENT,
  MSG_CA_HIDE_URGENT,
  MSG_CA_MON_CHILD_FEED,
  MSG_CA_OPEN_IPPV_PPT,
  MSG_CA_AUTHEN_EXPIRE_DAY,
  MSG_CA_CARD_UPDATE_BEGIN,
  MSG_CA_CARD_PROGRESS,
  MSG_CA_CARD_UPDATE_ERR,
  MSG_CA_CARD_UPDATE_END,
  MSG_CA_CARD_NEED_UPDATE,
  MSG_CA_BURSE_INFO,
  MSG_CA_IPPV_BUY_INFO,
  MSG_CA_CONDITION_SEARCH,
  MSG_CA_OVERDUE_DATA,
  MSG_CA_HIDE_IPPV_PPT,
  MSG_CA_HIDE_ROLL_MSG,
  MSG_CA_JUDGE_MOTHER_CARD,
  MSG_CA_IPP_BUY_OK,
  MSG_CA_UNLOCK_PARENTAL,
  MSG_CA_MAIL_CHANGE_STATUS,
  MSG_CA_MAIL_DEL_RESULT,
  MSG_CA_DEBUG,
  MSG_CA_IPP_REC_INFO,
  MSG_CA_IPP_PROG_NOTIFY,
  MSG_CA_PLATFORM_ID,
  MSG_CA_ACLIST_INFO,
  MSG_CA_READ_FEED_DATA,
  MSG_CA_FORCE_MSG,
  MSG_CA_HIDE_FORCE_MSG,
  MSG_CA_STOP_CUR_PG,
  MSG_CA_SHOW_BURSE_CHARGE,
  MSG_CA_CANCEL_PRE_AUTH,
  MSG_CA_CANCEL_PPV_ICON,
  MSG_CA_CANCEL_PAIR_ERROR,
  MSG_CA_SHOW_ERROR_PIN_CODE,
  MSG_CA_FEED_CARD_DLG,
  MSG_CA_DVB_RESUME_OSD,
  MSG_TIMER_CARD_OVERDUE,
  MSG_CA_EVT_LEVEL,
  MSG_CA_RATING_INFO,
  MSG_CA_ANNOUNCE_HEADER_INFO,
  MSG_CA_ANNOUNCE_BODY_INFO,
};

typedef struct burse_for_show
{
  u8 oper_name[50];
  u32 oper_id;
  u32 balance;
  u32 cash_value;
  BOOL flag;
}burse_for_show_t;

void ui_init_ca(void);

void ui_release_ca(void);

char * ui_ca_get_cardtype(void);

void ui_ca_get_info(u32 cmd_id, u32 para1, u32 para2);

void ui_ca_set_language(language_set_t lang_set);

int snprintf(char *string, size_t count, const char *format, ...);

#endif

