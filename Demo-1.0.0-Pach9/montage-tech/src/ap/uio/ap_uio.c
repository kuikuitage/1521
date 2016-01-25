/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"

#include "drv_dev.h"
#include "uio.h"
#include "class_factory.h"
#include "mdl.h"
#include "data_manager.h"

#include "ap_framework.h"
#include "ap_uio.h"
//#include "transport_shell.h"


/*!
  UIO data
  */
typedef struct
{
  /*!
    UIO device 
    */
  uio_device_t *p_dev;
  /*!
    Debug only
    */
  u8 user_debug;
  /*!
    Message list buffer
    */
  u32 msg_id_list[MAX_UIO_MSG_NUM];
  /*!
    Valid message number
    */
  u8 msg_num;
} uio_data_t;

/*!
  UIO private information 
  */
typedef struct 
{
  /*!
    UIO private data
    */
  uio_data_t uio_data;
  /*!
    UIO implement information
    */
  ap_uio_policy_t *p_impl_info;
  /*!
    UIO instance 
    */
  app_t uio_instance;
}ap_uio_priv_t;


static BOOL get_key_code(void *p_handle, v_key_t *p_key)
{
  ap_uio_priv_t *p_uio_priv = (ap_uio_priv_t *)p_handle;
  uio_data_t *p_uio_data = &p_uio_priv->uio_data;
  
  MT_ASSERT(p_key != NULL);
  p_key->type = IRDA_KEY;
  if(uio_get_code(p_uio_data->p_dev, &p_key->v_key) == SUCCESS)
  {
    //OS_PRINTF("uio one key detect 0x%x\n",p_key->v_key);
  }
  else
  {
    return FALSE;
  }

  if((p_key->v_key & 0x0100) != 0)
  {
    p_key->type = FP_KEY;
  }
  else
  {
    p_key->usr = (p_key->v_key >> (UIO_USR_CODE_SHIFT + 8)) & 0x7; 
  }
  p_key->v_key &= ((p_key->v_key) & 0xFF);

  /*!
    clear key buf
    */
  uio_clear_key(p_uio_data->p_dev);
  return TRUE;
}

static void init(void *p_handle)
{
  ap_uio_priv_t *p_uio_priv = (ap_uio_priv_t *)p_handle;
  uio_data_t *p_uio_data = &p_uio_priv->uio_data;

  ///void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 msg_id_list[] = {
    AP_UIO_CMD_FP_DISPLAY
  };

  //MT_ASSERT(p_dm_handle != NULL);
  MT_ASSERT(p_handle != NULL);

  p_uio_data->p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_UIO);
  MT_ASSERT(p_uio_data->p_dev != NULL);

  //Initialize message list
  memcpy(p_uio_priv->uio_data.msg_id_list, msg_id_list, sizeof(msg_id_list));
  p_uio_priv->uio_data.msg_num = sizeof(msg_id_list) / sizeof(u32);

  p_uio_priv->p_impl_info->p_init_kep_map();
}

static void uio_single_step(void *p_handle, os_msg_t *p_msg)
{
  ap_uio_priv_t *p_uio_priv = (ap_uio_priv_t *)p_handle;
  uio_data_t *p_uio_data = &p_uio_priv->uio_data;
  MT_ASSERT(p_uio_priv != NULL);
  
  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
    case AP_UIO_CMD_FP_DISPLAY:
      uio_display(p_uio_data->p_dev,(u8 *)&(p_msg->para1), p_msg->para2);
      break;
    default:
      break;
    }    
  }
  else
  {
    v_key_t key = {0, INVALID_KEY, INVALID_KEY, INVALID_KEY, INVALID_KEY, 0};

    if(get_key_code(p_handle, &key) == TRUE
      && p_uio_priv->p_impl_info->p_translate_key(&key) == TRUE)
    {
      event_t evt = {0};
      evt.id = UIO_EVT_KEY;
      evt.data1 = key.v_key;

      ap_frm_send_evt_to_ui(APP_UIO, &evt);

      if(key.v_key_2 != INVALID_KEY)
      {
        evt.id = UIO_EVT_KEY;
        evt.data1 = key.v_key_2;

        ap_frm_send_evt_to_ui(APP_UIO, &evt);
      }

      if(key.v_key_3 != INVALID_KEY)
      {
        evt.id = UIO_EVT_KEY;
        evt.data1 = key.v_key_3;

        ap_frm_send_evt_to_ui(APP_UIO, &evt);
      }

      if(key.v_key_4 != INVALID_KEY)
      {
        evt.id = UIO_EVT_KEY;
        evt.data1 = key.v_key_4;

        ap_frm_send_evt_to_ui(APP_UIO, &evt);
      }      
      
      return;
    }

    if(p_uio_priv->p_impl_info->p_set_irda_led != NULL)
    {
      p_uio_priv->p_impl_info->p_set_irda_led();
    }
  }
}

app_t *construct_ap_uio(ap_uio_policy_t *p_uio_policy)
{
  ap_uio_priv_t *p_uio_priv = mtos_malloc(sizeof(ap_uio_priv_t));
  MT_ASSERT(p_uio_priv != NULL);

  //Attach implement policy
  MT_ASSERT(p_uio_policy != NULL);
  p_uio_priv->p_impl_info = p_uio_policy;
  memset(&(p_uio_priv->uio_instance), 0, sizeof(app_t));

  //Attach uio instance
  p_uio_priv->uio_instance.get_msgq_timeout = NULL;
  p_uio_priv->uio_instance.init  = init;
  p_uio_priv->uio_instance.task_single_step = uio_single_step;
  p_uio_priv->uio_instance.p_data   = p_uio_priv;

  return &p_uio_priv->uio_instance;
}

