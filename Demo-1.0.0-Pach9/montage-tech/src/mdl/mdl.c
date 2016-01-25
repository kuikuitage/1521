/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"

#include "mtos_msg.h"
#include "mtos_mem.h"

#include "class_factory.h"
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
typedef struct 
{
  u32 msgq_id;
} mdl_priv_t;

void mdl_init(void)
{
  mdl_priv_t *p_data = mtos_malloc(sizeof(mdl_priv_t));
  MT_ASSERT(p_data != NULL);
  class_register(MDL_CLASS_ID, p_data);
  logger_init();
}
void mdl_init1(void)
{
  mdl_priv_t *p_data = mtos_malloc(sizeof(mdl_priv_t));
  MT_ASSERT(p_data != NULL);
  class_register(MDL_CLASS_ID, p_data);
  logger_init();
}
void mdl_set_msgq(handle_t handle, u32 id)
{
  mdl_priv_t *p_data = (mdl_priv_t *)handle;
  p_data->msgq_id = id;
}

void mdl_broadcast_msg(os_msg_t *p_msg)
{
  mdl_priv_t *p_data = class_get_handle_by_id(MDL_CLASS_ID);
  MT_ASSERT(p_msg != NULL);
  p_msg->is_brdcst = TRUE;
  p_msg->is_sync = FALSE;
  p_msg->extand_data = ~0;
  mtos_messageq_send(p_data->msgq_id, p_msg);
  //OS_PRINTF("Leave mdl_broadcast_msg\n");
}

void mdl_send_msg(os_msg_t *p_msg)
{
  mdl_priv_t *p_data = class_get_handle_by_id(MDL_CLASS_ID);
  MT_ASSERT(p_msg != NULL);
  p_msg->is_brdcst = FALSE;
  p_msg->is_sync = FALSE;
  mtos_messageq_send(p_data->msgq_id, p_msg);
}

