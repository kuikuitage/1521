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

#include "drv_dev.h"
#include "../src/drv/pdec/alg/wa_djpeg/jpeglib.h"
#include "pdec.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "pic_input_pin_intra.h"

typedef struct tag_pic_in_pin_private
{
  /*!
    this point !!
    */
  pic_in_pin_t *p_this;
  /*!
    define format
    */
  list_media_format_t format_list;
  /*!
    jpeg info
    */
  pic_info_t jpeg_info;    
}pic_in_pin_private_t;

static pic_in_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (pic_in_pin_private_t *)((pic_in_pin_t *)_this)->private_data;
}


static void request_sample(handle_t _this, media_sample_t *p_sample)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  base_output_pin_t *p_connecter = (base_output_pin_t *)p_ipin->get_connected(_this);
  iasync_reader_t *p_reader = p_connecter->get_interface_reader(p_connecter);

  p_reader->request(p_reader, &p_sample->format, 100 * KBYTES, 0, 0, 0);
}

static RET_CODE pic_in_pin_on_start(handle_t _this)
{
  media_sample_t sample = {{0}};
  
  request_sample(_this, &sample);
  return SUCCESS;
}

static void pic_in_pin_on_receive(handle_t _this, media_sample_t * p_sample)
{
  request_sample(_this, p_sample);
}


static list_media_format_t * pic_in_pin_list_media_format(handle_t _this)
{
  pic_in_pin_private_t *p_priv = get_priv(_this);
  return &p_priv->format_list;
}

static BOOL pic_in_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

BOOL pic_in_pin_check_parameter(void)
{
  //must be sure the buffer size is enough
  if(PIC_IN_PIN_PRIVAT_DATA < sizeof(pic_in_pin_private_t))
  {
    return FALSE;
  }

  //other check ....

  //ready all
  return TRUE;
}

pic_in_pin_t * pic_in_pin_create(pic_in_pin_t *p_pin, interface_t *p_owner)
{
  pic_in_pin_private_t *p_priv = NULL;
//  interface_t *p_interface = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;

  //check input parameter
  MT_ASSERT(p_pin != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "pic_in_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = (pic_in_pin_private_t *)p_pin->private_data;
  memset(p_priv, 0, sizeof(pic_in_pin_private_t));
  p_priv->p_this = p_pin; //this point
  p_priv->format_list.num = 1;
  memset(&p_priv->format_list.list[0], 0x0, sizeof(media_format_t));//fix me

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->notify_allocator = pic_in_pin_notify_allocator;
  p_input_pin->on_receive = pic_in_pin_on_receive;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->list_media_format = pic_in_pin_list_media_format;
  p_ipin->on_start = pic_in_pin_on_start;

  return p_pin;
}

