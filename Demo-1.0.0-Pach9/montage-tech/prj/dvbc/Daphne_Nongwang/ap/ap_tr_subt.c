/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
// std headers
#include  "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_fifo.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "lib_util.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "subt_pic_ware.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "monitor_service.h"

// ap headers
#include "ap_framework.h"
#include "ap_tr_subt.h"


typedef enum
{
  SP_SM_IDLE,
  SP_SM_START,
  SP_SM_STOP

}subt_state_t;

typedef struct
{
  subt_policy_t *p_policy;
  app_t instance;
  subt_state_t state;
}subt_priv_t;

static void subt_pic_send_evt_to_ui(u32 evt_id, u32 para1, u32 para2)
{
  event_t evt = {0, 0, 0};

  evt.id = evt_id;
  evt.data1 = para1;
  evt.data2 = para2;

  ap_frm_send_evt_to_ui(APP_RESERVED2, &evt);
}


static void subt_process_msg(handle_t handle, os_msg_t *p_msg)
{
  subt_priv_t  *p_priv = (subt_priv_t *)handle;
  switch(p_msg->content)
  {
    case SUBT_PIC_CMD_START:
      p_priv->state = SP_SM_START;
      subt_pic_open(p_msg->para1);
      break;
    case SUBT_PIC_CMD_START_SYNC:
      p_priv->state = SP_SM_START;
      subt_pic_open(p_msg->para1);
      subt_pic_send_evt_to_ui(SUBT_PIC_EVT_START, 0, 0);
      break;
    case SUBT_PIC_CMD_STOP_SYNC:
      p_priv->state = SP_SM_STOP;
      subt_pic_stop(p_msg->para1);
      subt_pic_send_evt_to_ui(SUBT_PIC_EVT_STOP, 0, 0);
      break;
    default:
      break;    
  }
}

static void subt_state_machine(handle_t handle)
{
  subt_priv_t  *p_priv = (subt_priv_t *)handle;
  switch(p_priv->state)
  {
    default:
      break;
  }
}

static void init(handle_t handle)
{


}

app_t *construct_ap_tr_subt(subt_policy_t *p_policy)
{
  subt_priv_t *p_priv = mtos_malloc(sizeof(subt_priv_t));
  MT_ASSERT(NULL != p_priv);
  memset(p_priv, 0, sizeof(subt_priv_t));
  
  MT_ASSERT(p_policy != NULL);

  p_priv->state = SP_SM_IDLE;

  p_priv->p_policy = p_policy;

  p_priv->instance.init = init;
  p_priv->instance.process_msg = subt_process_msg;
  p_priv->instance.state_machine = subt_state_machine;
  p_priv->instance.get_msgq_timeout = NULL;
  p_priv->instance.p_data = (void *)p_priv;
  return &p_priv->instance;
}
