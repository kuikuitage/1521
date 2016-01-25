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
#include "transf_output_pin.h"
#include "pic_output_pin_intra.h"

typedef struct tag_pic_out_pin_private
{
  /*!
    this point !!
    */
  pic_out_pin_t *p_this;
  /*!
    define format
    */
  list_media_format_t format_list;
  /*!
    define format
    */
  allocator_properties_t properties;
}pic_out_pin_private_t;

static pic_out_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (pic_out_pin_private_t *)((pic_out_pin_t *)_this)->private_data;
}

static RET_CODE pic_out_decide_buffer_size(handle_t _this)
{
  pic_out_pin_private_t *p_priv = get_priv(_this);
  base_output_pin_t *p_output_pin = (base_output_pin_t *)_this;
  imem_allocator_t *p_alloc = NULL;

  p_alloc = p_output_pin->get_interface_allocator(p_output_pin);
  return p_alloc->set_properties(p_alloc, &p_priv->properties, NULL);  
}

void pic_out_pin_cfg(pic_out_pin_t *p_output_pin, u32 buffer_size, void *p_buffer)
{
  base_output_pin_t *p_base_output_pin = (base_output_pin_t *)p_output_pin;
  pic_out_pin_private_t *p_priv = get_priv(p_output_pin);
  allocator_properties_t *p_prop = &p_priv->properties;
  imem_allocator_t *p_alloc = NULL;

  //config mem_alloc
  p_alloc = p_base_output_pin->get_interface_allocator(p_base_output_pin);
  p_alloc->get_properties(p_alloc, p_prop);
  p_prop->buffers = 1;
  p_prop->align_size = 8;
  p_prop->buffer_size = buffer_size;
  p_prop->extant_buffer.p_buffer = p_buffer;
  p_prop->extant_buffer.buffer_len = buffer_size;
}


static list_media_format_t * pic_out_pin_list_media_format(handle_t _this)
{
  pic_out_pin_private_t *p_priv = get_priv(_this);
  return &p_priv->format_list;
}

BOOL pic_out_pin_check_parameter(void)
{
  //must be sure the buffer size is enough
  if(PIC_OUT_PIN_PRIVAT_DATA < sizeof(pic_out_pin_private_t))
  {
    return FALSE;
  }

  //other check ....

  //ready all
  return TRUE;
}

pic_out_pin_t * pic_out_pin_create(pic_out_pin_t *p_pin, interface_t *p_owner)
{
  pic_out_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
//  interface_t *p_interface = NULL;
  transf_output_pin_para_t transf_pin_para = {0};

  //check input parameter
  MT_ASSERT(p_pin != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "pic_out_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = (pic_out_pin_private_t *)(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(pic_out_pin_private_t));
  p_priv->p_this = p_pin;
  p_priv->format_list.num = 1;
  memset(&p_priv->format_list.list[0], 0x0, sizeof(media_format_t));//fix me
  
  //init member function

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = pic_out_decide_buffer_size;
  
  p_ipin = (ipin_t *)p_pin;
  p_ipin->list_media_format = pic_out_pin_list_media_format;

  return p_pin;
}

