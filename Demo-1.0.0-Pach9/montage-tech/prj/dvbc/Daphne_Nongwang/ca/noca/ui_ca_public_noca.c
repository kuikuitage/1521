/****************************************************************************
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_prog_bar.h"
#include "ui_channel_warn.h"
#include "ui_nvod_api.h"
#include "ui_ca_public.h"

#define TONGFANG_CONDITIONAL_ACCEPT_BTN_CNT      8
enum ca_btn_id
{
  IDC_BTN_CA_INFO = 1,
  IDC_OPERATE_CA_INFO,
  IDC_BTN_CA_PAIR,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_MOTHER,
  IDC_BTN_CA_ANTIAUTH,
};
//#include "ui_ca_notify.h"
static u32 g_ca_last_msg = 0;
//static cas_sid_t g_ca_sid;
static BOOL g_is_smart_card_insert = FALSE;
static u32 g_smart_card_rate = 0;


u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
}

void ui_reset_ca_last_msg(void)
{
    g_ca_last_msg = INVALID_U32;
}
BOOL ui_set_smart_card_insert(BOOL status)
{
    g_is_smart_card_insert = status;
    return 0;
    
}
BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

u32 ui_get_smart_card_rate(void)
{
  return g_smart_card_rate;
}

void ui_set_smart_card_rate(u32 level)
{
  g_smart_card_rate = level;
}

RET_CODE on_conditional_accept_level_update_d(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if (para1 != SUCCESS)
  {
    return ERR_FAILURE;
  }
  g_smart_card_rate = para2;
  
  return SUCCESS;
}

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
   
}

void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{

}

control_t *conditional_accept_menu_info(control_t * p_ca_frm, u16 y )
{
  return NULL;
}

void on_ca_frm_btn_select_number(u8 ctrlID, u32 para1 )
{

}

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
   return SUCCESS;
}


RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  return SUCCESS;
}

RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{
  return SUCCESS;
}
void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
  //return SUCCESS;
}
void on_ca_zone_check_stop(void)
{
    /**boyuan ca will set message to do **/
}