#ifndef _UI_OTA_API_H
#define _UI_OTA_API_H

enum ota_msg
{
  MSG_OTA_FOUND = MSG_EXTERN_BEGIN + 150,
  MSG_OTA_TRIGGER_RESET,
  MSG_OTA_TMOUT,  
  MSG_OTA_MON_FORCE_OTA,
  MSG_OTA_MON_MAIN_FREQ_FORCE_OTA,
  MSG_OTA_MON_MAIN_FREQ_TMOUT
};

void ui_ota_init(void);

void ui_ota_release(void);

void ui_ota_start(u32 para1);

void ui_ota_stop(BOOL is_save);
void  ui_ota_monitor_init(void);
void ui_ota_monitor_start(u32 para1);

#endif
