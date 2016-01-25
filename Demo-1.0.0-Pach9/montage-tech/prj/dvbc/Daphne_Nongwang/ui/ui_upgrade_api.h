/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef _UI_UPGRADE_API_H
#define _UI_UPGRADE_API_H

enum upg_msg
{
  MSG_UPG_UPDATE_STATUS = MSG_EXTERN_BEGIN + 400,
  MSG_UPG_QUIT,
  MSG_UPG_OK,
  MSG_UPG_EXIT,
};

void ui_init_upg(void);

void ui_release_upg(void);

void ui_start_upgrade(upg_mode_t md, u8 block_num, upg_block_t *block);

void ui_exit_upgrade(BOOL is_exit_all);

#endif
