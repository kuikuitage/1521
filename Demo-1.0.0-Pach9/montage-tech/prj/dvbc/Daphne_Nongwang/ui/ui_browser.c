/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_browser.h"
#include "ui_browser_api.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_ROOT_CONT,
};

RET_CODE open_browser(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont = NULL;
  
  p_cont = ui_comm_root_create(ROOT_ID_BROWSER,
                             RSI_IGNORE,
                             BROWSER_L, BROWSER_T,
                             BROWSER_W, BROWSER_H,
                             0, RSI_IGNORE);
  if(NULL == p_cont)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_BROWSER_CONT, RSI_BROWSER_CONT, RSI_BROWSER_CONT);
    
  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);
  
  ui_browser_enter();
#endif
  /*enter ipanel browser*/
  return SUCCESS;
  
}

