/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_api.h"


u16 ui_ca_evtmap(u32 event);

char * g_cardtype_str[] =
{
  "AutoDetect",
  "Irdeto Card",
  "Viaccess Card",
  "Nagra Card",
  "Seca Card",
  "Conax Card",
  "Cryptowork Card",
  "Shl Card",
  "Dreamcrypt Card",
  "Xcrypto Card",
  "NDS Card",
  "Nagra2 Card",
  "Tricolor Card",
  "Tongfang Card",
  "Sumavision Card",
  "Tianbai Card",
  "Unknown Card",
};

#define SUPPORT_CARDTYPE_MAX_CNT (sizeof(g_cardtype_str)/sizeof(char*))

int snprintf(char *string, size_t count, const char *format, ...)
{
  char temp[1024] = {0};
  int ret = 0;

  ret = sprintf(temp, format);

  memcpy(string, temp, count);
  string[count] = 0;

  return ret;
}

void ui_init_ca(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_CA;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_CA, ui_ca_evtmap);
  fw_register_ap_msghost(APP_CA, ROOT_ID_BACKGROUND);
  //fw_register_ap_msghost(APP_CA, ROOT_ID_MULTIVIEW);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_ENTITLE_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_CARD_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_PAIR);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_MOTHER);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH);
  fw_register_ap_msghost(APP_CA, ROOT_ID_EMAIL_MESS);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_IPP_REAL_TIME);
  fw_register_ap_msghost(APP_CA, ROOT_ID_EMAIL_MESS_CONTENT);
  fw_register_ap_msghost(APP_CA, ROOT_ID_FORCE_EMAIL_CONTENT);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_O_S_IPP_PROG);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_PIN_IPP);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_HISTORY);
  fw_register_ap_msghost(APP_CA, ROOT_ID_HIDE_CA_MENU);
  #endif
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_LEVEL);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_FEED);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_PIN);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_MOTHER);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_IPPV_PPT_DLG);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_CARD_UPDATE);
  fw_register_ap_msghost(APP_CA, SN_ROOT_ID_PRO_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_PASSWORD);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
  fw_register_ap_msghost(APP_CA, ROOT_ID_SP_LIST);
  fw_register_ap_msghost(APP_CA, ROOT_ID_SP_LIST2);
  #endif
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV) || (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_OPE);
  #endif
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF)|| (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_CARD_INFO);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_IPP_REAL_TIME);
  fw_register_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT);
  #endif
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)	
  fw_register_ap_msghost(APP_CA, ROOT_ID_PROVIDER_INFO);
  #endif
  fw_register_ap_msghost(APP_CA, ROOT_ID_CHARACTER_CODE);
 
}


void ui_release_ca(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_CA;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);



  fw_unregister_ap_msghost(APP_CA, ROOT_ID_BACKGROUND);
  //fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_DETAIL);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_ENTITLE_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_CARD_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_PAIR);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_MOTHER);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_EMAIL_MESS);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_IPP_REAL_TIME);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_EMAIL_MESS_CONTENT);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_FORCE_EMAIL_CONTENT);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_O_S_IPP_PROG);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_PIN_IPP);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_HISTORY);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_HIDE_CA_MENU);
  #endif
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_LEVEL);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_FEED);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT_PIN);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_MOTHER);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_IPPV_PPT_DLG);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_CARD_UPDATE);
  fw_unregister_ap_msghost(APP_CA, SN_ROOT_ID_PRO_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_PASSWORD);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_SP_LIST);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_SP_LIST2);
  #endif
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF) || (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)|| (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_OPE);
  #endif
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_TF)|| (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_CARD_INFO);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CA_BOOK_IPP_REAL_TIME);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CONDITIONAL_ACCEPT);
  #endif
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)	
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_PROVIDER_INFO);
  #endif
  
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_CHARACTER_CODE);

  fw_unregister_ap_evtmap(APP_CA);
}


void ui_ca_get_info(u32 cmd_id, u32 para1, u32 para2)
{
  cmd_t cmd = {0};
  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | AP_CAS_ID);
  ap_frm_do_command(APP_CA, &cmd);
}

void ui_ca_set_language(language_set_t lang_set)
{
  switch(lang_set.osd_text)
  {
    case 0:
      ui_ca_get_info(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_CHINESE, 0);
      break;
    case 1:
      ui_ca_get_info(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_ENGLISH, 0);
      break;
    default:
      ui_ca_get_info(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_CHINESE, 0);
      break;
  }
}

char * ui_ca_get_cardtype(void)
{
  u8 cardtype = smcctrl_get_cardtype();

  if (cardtype < SUPPORT_CARDTYPE_MAX_CNT)
  {
    return g_cardtype_str[cardtype];
  }

  return g_cardtype_str[SUPPORT_CARDTYPE_MAX_CNT - 1]; // unknown
}

RET_CODE svstb_convert_time(u32 u32Time,utc_time_t *pUTCTime)
{
    utc_time_t start_utc_time = {1970,1,1,0};

    time_to_local(&start_utc_time, pUTCTime);
    time_up(pUTCTime, u32Time);

    return SUCCESS;
}


BEGIN_AP_EVTMAP(ui_ca_evtmap)
  CONVERT_EVENT(CAS_EVT_RST_SUCCESS,   MSG_CA_INIT_OK)
  CONVERT_EVENT(CAS_EVT_OPERATOR_INFO, MSG_CA_OPE_INFO)
  CONVERT_EVENT(CAS_EVT_ENTITLE_INFO, MSG_CA_ENT_INFO)
  CONVERT_EVENT(CAS_EVT_IPPV_INFO, MSG_CA_IPV_INFO)
  CONVERT_EVENT(CAS_EVT_ACLIST_INFO, MSG_CA_ACLIST_INFO)
  CONVERT_EVENT(CAS_EVT_READ_FEED_DATA, MSG_CA_READ_FEED_DATA)
  CONVERT_EVENT(CAS_EVT_BURSE_INFO, MSG_CA_BUR_INFO)
  CONVERT_EVENT(CAS_EVT_CARD_INFO, MSG_CA_CARD_INFO)
  CONVERT_EVENT(CAS_EVT_PAIRED_STA, MSG_CA_PAIRE_INFO)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_INFO, MSG_CA_MON_CHILD_INFO)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_FEED, MSG_CA_MON_CHILD_FEED)
  CONVERT_EVENT(CAS_EVT_MAIL_HEADER_INFO, MSG_CA_MAIL_HEADER_INFO)
  CONVERT_EVENT(CAS_EVT_MAIL_BODY_INFO, MSG_CA_MAIL_BODY_INFO)
  CONVERT_EVENT(CAS_EVT_MAIL_CHANGE_STATUS, MSG_CA_MAIL_CHANGE_STATUS)
  CONVERT_EVENT(CAS_EVT_MAIL_DEL_RESULT, MSG_CA_MAIL_DEL_RESULT)
  CONVERT_EVENT(CAS_EVT_WORK_TIME_SET, MSG_CA_WORK_TIME_INFO)
  CONVERT_EVENT(CAS_EVT_WORK_TIME_INFO, MSG_CA_GET_WORK_TIME_INFO)
  CONVERT_EVENT(CAS_EVT_PIN_SET, MSG_CA_PIN_SET_INFO)
  CONVERT_EVENT(CAS_EVT_PIN_VERIFY, MSG_CA_PIN_VERIFY_INFO)
  CONVERT_EVENT(CAS_EVT_PPV_BUY_INFO, MSG_CA_IPPV_BUY_INFO)
  CONVERT_EVENT(CAS_EVT_SHOW_IPP_BUY_INFO, MSG_CA_OPEN_IPPV_PPT)
  CONVERT_EVENT(CAS_EVT_PPV_SYNC_INFO,MSG_CA_OPEN_IPPV_PPT)
  CONVERT_EVENT(CAS_EVT_HIDE_PPV_BUY_INFO,MSG_CA_HIDE_IPPV_PPT)
  CONVERT_EVENT(CAS_EVT_SHOW_ERROR_PIN_CODE,MSG_CA_SHOW_ERROR_PIN_CODE)//YYF
  //CONVERT_EVENT(CAS_S_CLEAR_DISPLAY, MSG_CA_CLEAR_DISPLAY)
  CONVERT_EVENT(CAS_EVT_NOTIFY, MSG_CA_EVT_NOTIFY)
  CONVERT_EVENT(CAS_EVT_FORCE_CHANNEL_INFO, MSG_CA_FORCE_CHANNEL_INFO)
  CONVERT_EVENT(CAS_EVT_UNFORCE_CHANNEL_INFO, MSG_CA_UNFORCE_CHANNEL_INFO)
  CONVERT_EVENT(CAS_EVT_RATING_INFO, MSG_CA_RATING_GET)
  CONVERT_EVENT(CAS_EVT_RATING_SET, MSG_CA_RATING_SET)
  CONVERT_EVENT(CAS_EVT_SHOW_OSD_MSG, MSG_CA_SHOW_ROLL_MSG)
  CONVERT_EVENT(CAS_EVT_SHOW_OSD_DOWN, MSG_CA_SHOW_ROLL_MSG)
  CONVERT_EVENT(CAS_EVT_SHOW_OSD_UP, MSG_CA_SHOW_ROLL_MSG)
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_UP, MSG_CA_HIDE_ROLL_MSG)
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_TOP, MSG_CA_HIDE_ROLL_MSG_TOP)//lxd
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_BOTTOM, MSG_CA_HIDE_ROLL_MSG_BOTTOM)//lxd
  CONVERT_EVENT(CAS_EVT_SHOW_FINGER, MSG_CA_SHOW_FINGER)
  CONVERT_EVENT(CAS_EVT_HIDE_FINGER, MSG_CA_HIDE_FINGER)
  CONVERT_EVENT(CAS_EVT_SHOW_URGENT_MSG,MSG_CA_SHOW_URGENT)
  CONVERT_EVENT(CAS_EVT_HIDE_URGENT_MSG,MSG_CA_HIDE_URGENT)
  CONVERT_EVENT(CAS_EVT_SHOW_AUTHEN_EXPIRE_DAY, MSG_CA_AUTHEN_EXPIRE_DAY)
  CONVERT_EVENT(CAS_EVT_CARD_NEED_UPDATE, MSG_CA_CARD_NEED_UPDATE)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_BEGIN, MSG_CA_CARD_UPDATE_BEGIN)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_PROGRESS, MSG_CA_CARD_PROGRESS)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_ERR, MSG_CA_CARD_UPDATE_ERR)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_END, MSG_CA_CARD_UPDATE_END)
  //CONVERT_EVENT(CAS_EVT_MON_CHILD_FEED, MSG_CA_MON_CHILD_FEED)
  CONVERT_EVENT(CAS_CONDITION_SEARCH, MSG_CA_CONDITION_SEARCH)
  CONVERT_EVENT(CAS_EVT_OVERDUE,MSG_CA_OVERDUE_DATA)
  CONVERT_EVENT(CAS_EVT_IPP_BUY_RESULT, MSG_CA_IPP_BUY_OK)
  CONVERT_EVENT(CAS_EVT_UNLOCK_PARENTAL_RESULT, MSG_CA_UNLOCK_PARENTAL)
  CONVERT_EVENT(CAS_EVT_DEBUG_EN,MSG_CA_DEBUG)
  CONVERT_EVENT(CAS_EVT_IPP_REC_INFO, MSG_CA_IPP_REC_INFO)
  CONVERT_EVENT(CAS_EVT_IPP_PROG_NOTIFY,MSG_CA_IPP_PROG_NOTIFY)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_IDENTIFY,MSG_CA_JUDGE_MOTHER_CARD)
  CONVERT_EVENT(CAS_EVT_FORCE_MSG,MSG_CA_FORCE_MSG)
  CONVERT_EVENT(CAS_EVT_HIDE_FORCE_MSG,MSG_CA_HIDE_FORCE_MSG)
  CONVERT_EVENT(CAS_EVT_IEXIGENT_PROG_STOP, MSG_CA_STOP_CUR_PG)
  CONVERT_EVENT(CAS_EVT_DVB_RESUME_OSD_MSG, MSG_CA_DVB_RESUME_OSD)
  END_AP_EVTMAP(ui_ca_evtmap);

