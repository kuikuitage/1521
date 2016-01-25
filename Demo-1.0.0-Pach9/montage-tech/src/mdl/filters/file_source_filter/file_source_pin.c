/******************************************************************************/
/******************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_mutex.h"
#include "mtos_sem.h"
#include "mtos_event.h"

#include "mt_time.h"
#include "vfs.h"

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
#include "ifilter.h"
#include "source_pin.h"
#include "ifilter.h"
#include "src_filter.h"

//filter
#include "eva_filter_factory.h"

#include "file_source_filter.h"

#include "file_source_pin_intra.h"
#include "chain.h"

/*!
  buffer read
  */
typedef u8 *(*buffer_get)(u32 *p_len);

typedef struct tag_fsrc_pin_private
{
  /*!
    this point !!
    */
  fsrc_pin_t *p_this;
  /*!
    file name
    */
  char file_name[MAX_FILE_PATH];
  /*!
    file handle
    */
  hfile_t file;
  /*!
    define format
    */
  list_media_format_t format_list;
  /*!
    circular
    */
  BOOL is_circular;
  /*!
    vfs handle
    */
  vfs_t *p_vfs;
  /*!
    is share read, default to engross
    */
  BOOL is_share;
  /*!
    Number of buffers created. default to 1, ignore 0.
    */
  u32 buffers;
  /*!
    Size of each buffer in bytes. default to KBYTE, ignore 0.
    */
  u32 buffer_size;
  /*!
    buffer read
    */
  buffer_get buf_get;
}fsrc_pin_private_t;

static fsrc_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (fsrc_pin_private_t *)((fsrc_pin_t *)_this)->private_data;
}

static void fsrc_pin_set_file_name(handle_t _this, char *p_file_name)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);

  MT_ASSERT(p_file_name != NULL);
  strncpy(p_priv->file_name, p_file_name, MAX_FILE_PATH);
  p_priv->buf_get = NULL;
}

static void fsrc_pin_set_buffer_read(handle_t _this, u32 read_data)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);

  p_priv->buf_get = (buffer_get) read_data;
  p_priv->file_name[0] = 0;
}

static void fsrc_pin_cfg(handle_t _this, src_pin_attr_t *p_attr)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  MT_ASSERT(p_attr != NULL);
  
  p_priv->is_circular = p_attr->is_circular;
  p_priv->is_share = p_attr->is_share;
  if(p_attr->buffers != 0)
  {
    p_priv->buffers = p_attr->buffers;
  }
  if(p_attr->buffer_size != 0)
  {
    p_priv->buffer_size = p_attr->buffer_size;
  }

}

static RET_CODE fsrc_pin_on_open(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  
  if (p_priv->buf_get)
  {
    return SUCCESS;
  }
  
  if (p_priv->is_share)
  {
    p_priv->file = p_priv->p_vfs->open((u8 *)p_priv->file_name, VFS_SHARE);
  }
  else
  {
    p_priv->file = p_priv->p_vfs->open((u8 *)p_priv->file_name, VFS_READ);
  }
  
  if (p_priv->file != NULL)
  {
    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}

static RET_CODE fsrc_pin_on_close(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  
  if (p_priv->buf_get)
  {
    return SUCCESS;
  }
  
  if(p_priv->file != NULL)
  {
    p_priv->p_vfs->close(p_priv->file);
    p_priv->file = NULL;
  }
  
  return SUCCESS;
}

static RET_CODE fsrc_pin_on_fill_sample(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, u32 position)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  u32 read_size = 0;
  u32 len = (p_sample->total_buffer_size < fill_len) ? p_sample->total_buffer_size : fill_len;

  if (!len)
  {
    return ERR_FAILURE;
  }
  
  if (p_priv->buf_get)
  {
    p_sample->p_data = p_priv->buf_get(&p_sample->payload);

    p_sample->data_offset = p_sample->payload;
    p_sample->context = p_sample->payload;
    p_sample->state = SAMP_STATE_INSUFFICIENT;
    p_priv->buf_get = NULL;

    return SUCCESS;
  }
  
  if (p_priv->file == NULL)
  {
    return ERR_FAILURE;
  }
  
  p_sample->filename =(u8 *) p_priv->file_name;
  
  if (position > 0)
  {
    p_priv->p_vfs->seek(p_priv->file, position, VFS_SEEK_HEAD);
  }

  read_size = p_priv->p_vfs->read(p_priv->file, p_sample->p_data, len);

  p_sample->payload = read_size;
  
  if (read_size > 0)
  {
    if (read_size == len)
    {
      p_sample->state = SAMP_STATE_GOOD;
    }
    else
    {
      p_sample->state = SAMP_STATE_INSUFFICIENT;
    }
    return SUCCESS;
  }
  else if(p_priv->is_circular)
  {
    p_sample->state = SAMP_STATE_FAIL;
    p_priv->p_vfs->seek(p_priv->file, 0, VFS_SEEK_HEAD);
    return ERR_FAILURE;
  }
  else
  {
    ipin_t *p_ipin = (ipin_t *)_this;
    os_msg_t msg = {0};
    msg.content = FILE_READ_END;
    
    p_ipin->send_message_out(p_ipin, &msg);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

static list_media_format_t *fsrc_pin_list_media_format(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  return &p_priv->format_list;
}

static RET_CODE fsrc_pin_decide_buffer_size(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  base_output_pin_t *p_output_pin = (base_output_pin_t *)_this;
  imem_allocator_t *p_alloc = NULL;
  allocator_properties_t properties;
  
  //config mem_alloc
  p_alloc = p_output_pin->get_interface_allocator(p_output_pin);
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = p_priv->buffers;
  properties.buffer_size = p_priv->buffer_size;
  properties.align_size = 8;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  
  return SUCCESS;
}

BOOL fsrc_pin_check_parameter(void)
{
  //must be sure the buffer size is enough
  if (FSRC_PIN_PRIVAT_DATA < sizeof(fsrc_pin_private_t))
  {
    return FALSE;
  }

  //other check ....

  //ready all
  return TRUE;
}

fsrc_pin_t * fsrc_pin_create(fsrc_pin_t *p_fsrc_pin, interface_t *p_owner)
{
  fsrc_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
  source_pin_t *p_source_pin = NULL;
  //interface_t *p_interface = NULL;
  source_pin_para_t src_pin_para = {0};

  //check input parameter
  MT_ASSERT(p_fsrc_pin != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  src_pin_para.p_filter = p_owner;
  src_pin_para.stream_mode = STREAM_MODE_PULL;
  src_pin_para.p_name = "fsrc_pin";
  source_pin_create(&p_fsrc_pin->m_pin, &src_pin_para);

  //init private date
  p_priv = (fsrc_pin_private_t *)(p_fsrc_pin->private_data);
  memset(p_priv, 0x0, sizeof(fsrc_pin_private_t));
  p_priv->p_this = p_fsrc_pin;
  p_priv->format_list.num = 1;
  memset(&p_priv->format_list.list[0], 0x0, sizeof(media_format_t));//fix me

  p_priv->p_vfs = class_get_handle_by_id(VFS_CLASS_ID);
  //MT_ASSERT(p_priv->p_vfs != NULL);
  

  p_priv->buffers = 1;
  p_priv->buffer_size = KBYTES;
  
  //init member function
  p_fsrc_pin->set_file_name = fsrc_pin_set_file_name;
  p_fsrc_pin->set_buffer_read = fsrc_pin_set_buffer_read;
  p_fsrc_pin->config = fsrc_pin_cfg;

  //over load virtual function
  p_source_pin = (source_pin_t *)p_fsrc_pin;
  p_source_pin->on_fill_sample = fsrc_pin_on_fill_sample;
  
  p_output_pin = (base_output_pin_t *)p_fsrc_pin;
  p_output_pin->decide_buffer_size = fsrc_pin_decide_buffer_size;
  
  p_ipin = (ipin_t *)p_fsrc_pin;
  p_ipin->list_media_format = fsrc_pin_list_media_format;
  p_ipin->on_open = fsrc_pin_on_open;
  p_ipin->on_close = fsrc_pin_on_close;

  return p_fsrc_pin;
}

//end of file

