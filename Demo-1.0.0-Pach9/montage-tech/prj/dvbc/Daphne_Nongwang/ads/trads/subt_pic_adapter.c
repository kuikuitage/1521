/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
//#include "lib_bitops.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_event.h"
#include "mtos_sem.h"

#include "subt_pic_ware.h"
#include "subt_pic_adapter.h"

subt_pic_adapter_priv_t g_subt_pic_priv = {0};
//subt_pic_module_priv_t *subt_pic_priv = NULL;

RET_CODE subt_pic_detach(u32 adm_id)
{
  subt_pic_op_t *p_op = NULL;

  p_op = (subt_pic_op_t *)adm_id;
  
  if(p_op->inited == 1)
  {
    p_op->func.deinit();
  }   
  p_op->inited = 0;
  
  
  if(p_op->p_priv != NULL)
  {
    mtos_free(p_op->p_priv);
    p_op->p_priv = NULL;
  }
  p_op->attached = 0;
  
  return SUCCESS;
}

RET_CODE subt_pic_init(u32 adm_id, subt_pic_module_cfg_t *p_param)
{
  RET_CODE ret = ERR_FAILURE;

  if(g_subt_pic_priv.inited == 1)
  {
    ret = SUCCESS;
    return ret;
  }
  g_subt_pic_priv.inited = 1;
  MT_ASSERT(TRUE == mtos_sem_create(&g_subt_pic_priv.evt_mutex, TRUE));
  //ads_clr_events(adm_id);
  return g_subt_pic_priv.adm_op[adm_id].func.init(p_param);
}

RET_CODE subt_pic_deinit(u32 adm_id)
{
 MT_ASSERT(TRUE == mtos_sem_destroy(&g_subt_pic_priv.evt_mutex, 0));
  g_subt_pic_priv.inited = 0;
  //ads_clr_events(adm_id);
  return g_subt_pic_priv.adm_op[adm_id].func.deinit();
}

RET_CODE subt_pic_open(u32 adm_id)
{
  return g_subt_pic_priv.adm_op[adm_id].func.open();
}

RET_CODE subt_pic_close(u32 adm_id)
{
  return g_subt_pic_priv.adm_op[adm_id].func.close();
}

RET_CODE subt_pic_stop(u32 adm_id)
{
  return g_subt_pic_priv.adm_op[adm_id].func.stop();
}

RET_CODE subt_pic_io_ctrl(u32 adm_id, u32 cmd, void *p_param)
{
  if(g_subt_pic_priv.adm_op[adm_id].attached != 1)
  {
    return ERR_STATUS;
  }
  return g_subt_pic_priv.adm_op[adm_id].func.io_ctrl(cmd, p_param);
}
