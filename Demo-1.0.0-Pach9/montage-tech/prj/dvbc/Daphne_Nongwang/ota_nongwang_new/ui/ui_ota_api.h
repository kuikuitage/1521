/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_OTA_API_H__
#define __UI_OTA_API_H__

enum ota_msg
{
  MSG_OTA_PROGRESS = MSG_EXTERN_BEGIN + 200,
  MSG_OTA_LOCK,
  MSG_OTA_UNLOCK,
  MSG_OTA_START_DL,
  MSG_OTA_CHECKING,
  MSG_OTA_BURNING,
  MSG_OTA_FINISH,
  MSG_OTA_UPGRADE,
  MSG_OTA_UNZIP_OK,
};

void ui_ota_relock(ota_info_t *p_info);
  
void ui_enable_ota(BOOL is_enable);

void ui_ota_start(void);
void ui_ota_stop(void);

void ui_ota_burn_flash(BOOL is_burn);

void ui_enable_uio(BOOL is_enable);
#endif
