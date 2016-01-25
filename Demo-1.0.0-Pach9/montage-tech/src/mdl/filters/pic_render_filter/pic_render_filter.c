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
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "pic_render_pin_intra.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"
#include "pic_render_filter.h"
#include "pic_render_filter_intra.h"

typedef struct tag_pic_render_filter_private
{
  /*!
    this point !!
    */
  pic_render_filter_t *p_this;
  /*!
    input pin
    */
  pic_render_pin_t m_pin;
}pic_render_filter_private_t;

static pic_render_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (pic_render_filter_private_t *)((pic_render_filter_t *)_this)->private_data;
}

static RET_CODE on_command(handle_t _this, icmd_t *p_cmd)
{
  pic_render_filter_private_t *p_priv = get_priv(_this);
  
  switch (p_cmd->cmd)
  {
    case PIC_RENDER_CFG_RECT:
      pic_render_cfg_rect(&p_priv->m_pin, p_cmd->p_para);
      break;
    case PIC_RENDER_CFG_RGN:
      pic_render_cfg_rgn(&p_priv->m_pin, p_cmd->p_para);
      break;
    case PIC_RENDER_CFG_CLR_KEY:
     pic_render_cfg_clr_key(&p_priv->m_pin, p_cmd->p_para);
    	break;
    
    default:
      return ERR_FAILURE;
  }
  
  return SUCCESS;
}

BOOL pic_render_filter_check_parameter(void)
{
  //check pin
  if (pic_render_pin_check_parameter() == FALSE)
  {
    return FALSE;
  }
  
  //must be sure the buffer size is enough
  if (PIC_RENDER_FILTER_PRIVAT_DATA < sizeof(pic_render_filter_private_t))
  {
    return FALSE;
  }

  //ready all
  return TRUE;
}

ifilter_t * pic_render_filter_create(void)
{
  pic_render_filter_private_t *p_priv = NULL;
  pic_render_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(pic_render_filter_t));
  MT_ASSERT(p_ins != NULL);
  
  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = (pic_render_filter_private_t *)p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_render_filter_private_t));
  p_priv->p_this = p_ins;
  
  //init member function

  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = on_command;

  //create child pin
  pic_render_pin_create(&p_priv->m_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}

//end of file

