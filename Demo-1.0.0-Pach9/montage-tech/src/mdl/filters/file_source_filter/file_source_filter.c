/******************************************************************************/
/******************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"

#include "ifilter.h"
#include "src_filter.h"

//filter
#include "eva_filter_factory.h"

#include "file_source_filter.h"
#include "file_source_filter_intra.h"

#include "file_source_pin_intra.h"

typedef struct tag_fsrc_filter_private
{
  /*!
    this point !!
    */
  fsrc_filter_t *p_this;

  /*!
    it's output pin
    */
  fsrc_pin_t m_pin;
}fsrc_filter_private_t;

static fsrc_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (fsrc_filter_private_t *)((fsrc_filter_t *)_this)->private_data;
}

static RET_CODE fsrc_on_command(handle_t _this, icmd_t *p_cmd)
{
  fsrc_filter_private_t *p_priv = get_priv(_this);
  fsrc_pin_t *p_pin = &p_priv->m_pin;
  
  switch (p_cmd->cmd)
  {
    case FSRC_CFG_FILE_NAME:
      if (p_cmd->p_para)
      {
        p_pin ->set_file_name(p_pin, (char *)p_cmd->p_para);
      }
      else if (p_cmd->lpara)
      {
        p_pin->set_buffer_read(p_pin, p_cmd->lpara);
      }
      break;
    case FSRC_CFG_SOURCE_PIN:
      p_pin->config(p_pin, (src_pin_attr_t *)p_cmd->p_para);
      break;
    default:
      return ERR_FAILURE;
  }
  return SUCCESS;
}

BOOL fsrc_filter_check_parameter(void)
{
  //check pin
  if(!fsrc_pin_check_parameter())
  {
    return FALSE;
  }
  
  //must be sure the buffer size is enough
  if(FSRC_FILTER_PRIVAT_DATA < sizeof(fsrc_filter_private_t))
  {
    return FALSE;
  }

  //other check ....

  //ready all
  return TRUE;
}

ifilter_t * fsrc_filter_create(void)
{
  fsrc_filter_private_t *p_priv = NULL;
  fsrc_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;
  src_filter_para_t src_filter_para;

  src_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(fsrc_filter_t));
  MT_ASSERT(p_ins != NULL);
  
  //create base class
  src_filter_create(&p_ins->m_filter, &src_filter_para);

  //init private date
  p_priv = (fsrc_filter_private_t *)p_ins->private_data;
  p_priv->p_this = p_ins; //this point

  //init member function

  //over loading the virtual function
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = fsrc_on_command;

  //create it's pin
  fsrc_pin_create(&p_priv->m_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}

