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
#include "mtos_misc.h"
#include "mtos_task.h"

//drv
//#include "usb_drv.h"
#include "common.h"
#include "lib_rect.h"
#include "drv_dev.h"
#include "gpe_vsb.h"

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
#include "ifilter.h"
#include "eva_filter_factory.h"
#include "pic_render_pin_intra.h"
#include "pic_render_filter.h"
#include "pdec.h"
#include "display.h"

/*!
  dec line number
  */
#define DEC_LINE_NUM (50)

/*!
  dib_header_t for gif
  */
typedef struct tag_dib_header
{
  /*!
    header_sz
    */
  u32 header_sz;
  /*!
    width
    */
  s32 width;
  /*!
    height
    */
  s32 height;
  /*!
    nplanes
    */
  u16 nplanes;
  /*!
    bitspp
    */
  u16 bitspp;
  /*!
    compress_type
    */
  u32 compress_type;
  /*!
    bmp_bytesz
    */
  u32 bmp_bytesz;
  /*!
    hres
    */
  s32 hres;
  /*!
    vres
    */
  s32 vres;
  /*!
    ncolors
    */
  u32 ncolors;
  /*!
    nimpcolors
    */
  u32 nimpcolors;
}dib_header_t;

/*!
  video pic_render pin private data
  */
typedef struct tag_pic_render_pin_private
{
  /*!
    this point !!
    */
  pic_render_pin_t *p_this;
  /*!
    osd rgn
    */
  void *p_rgn;
  /*!
    postion
    */
  rect_t fill_rect;
  /*!
    last_tick
    */
  u32 last_tick;
  /*!
    enable key
    */
  BOOL enable_key;
  /*!
    color key
    */
  u32 clr_key;
  
}pic_render_pin_private_t;

static pic_render_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (pic_render_pin_private_t *)((pic_render_pin_t *)_this)->private_data;
}

void pic_render_cfg_rect(pic_render_pin_t *p_pin, void *p_rect)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);

  memcpy((void *)&p_priv->fill_rect, p_rect, sizeof(rect_t));
}

void pic_render_cfg_rgn(pic_render_pin_t *p_pin, void *p_rgn)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);

  MT_ASSERT(p_rgn != NULL);

  p_priv->p_rgn = p_rgn;
}

void pic_render_cfg_clr_key(pic_render_pin_t *p_pin, void *p_clr_cfg)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);

  MT_ASSERT(p_clr_cfg != NULL);
  
  p_priv->enable_key = ((pic_render_clr_key_t *)p_clr_cfg)->enable_key;
  p_priv->clr_key =  ((pic_render_clr_key_t *)p_clr_cfg)->color_key;
  
}

static RET_CODE on_stop(handle_t _this)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  //clear last picture.
  gpe_draw_rectangle_vsb(p_gpe_dev, p_priv->p_rgn, &p_priv->fill_rect, 0);

  return SUCCESS;
}


static RET_CODE on_open(handle_t _this)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);

  p_priv->last_tick = 0;

  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  return SUCCESS;
}

static void rgba_render(handle_t _this, media_sample_t *p_sample)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  gpe_param_vsb_t gpe_param = {0};
  u32 pitch = 0, in_size = 0, fmt = 0;
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  u8 *p_bmp = NULL;
  u32 width = 0, height = 0;
  rect_t rgn_rect = {0};
  rect_t fill_rect = {0};
  u32 rgn_width = 0;
  u32 rgn_height = 0;

  p_bmp = p_sample->p_data + p_sample->data_offset;

  width = *((u32 *)p_bmp);
  height = *((u32 *)(p_bmp + 4));
  fmt = *((u32 *)(p_bmp + 8));

  pitch = width * 4;
  in_size = height * pitch;

  rgn_width = RECTW(p_priv->fill_rect);
  rgn_height = RECTH(p_priv->fill_rect);

  gpe_draw_rectangle_vsb(p_gpe_dev, p_priv->p_rgn, &p_priv->fill_rect, 0x00000000);

  rgn_rect.left = p_priv->fill_rect.left + (rgn_width - width) / 2;
  rgn_rect.top = p_priv->fill_rect.top + (rgn_height - height) / 2;
  rgn_rect.right = rgn_rect.left + width;
  rgn_rect.bottom = rgn_rect.top + height;

  gpe_draw_image_vsb(p_gpe_dev, p_priv->p_rgn, &rgn_rect,
    (p_sample->p_data + p_sample->data_offset + 12), NULL, 0, pitch,
    in_size, (pix_fmt_t)fmt, &gpe_param, &fill_rect);
}

static void rgbapalette2argb(const u8 *dibaddr, u32 *p_argb)
{
  u8 *p_dib_data = NULL;
  dib_header_t *p_dib_header = NULL;
  s32 width_with_padding = 0;
  u32 *p_palette = NULL;
  s32 loopi = 0;
  s32 loopj = 0;
  s32 index = 0;

  p_dib_header = (dib_header_t *)dibaddr;

  p_palette = (u32 *)(dibaddr + sizeof(dib_header_t));
  
  p_dib_data = (u8 *)dibaddr + sizeof(dib_header_t) + p_dib_header->ncolors * 4;
  width_with_padding = 4 * ((p_dib_header->width + 3) / 4);

  for (loopi = 0; loopi < p_dib_header->height; loopi ++)
  {
    for (loopj = 0; loopj < p_dib_header->width; loopj ++)
    {
      p_argb[index ++] = p_palette[p_dib_data[loopi * width_with_padding + loopj]];
    }
  }
}

static void rgbapalette_render(handle_t _this, media_sample_t *p_sample, rect_t *p_rgn_rect)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  gpe_param_vsb_t gpe_param = {0};
  u32 pitch = 0, in_size = 0, fmt = 0;
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  u8 *p_bmp = NULL;
  u32 width = 0, height = 0;
  rect_t fill_rect = {0};
  u32 rgn_width = 0;
  u32 rgn_height = 0;
  gif_output_t *p_gif_dst = NULL;

  p_bmp = p_sample->p_data + p_sample->data_offset;
  p_gif_dst = (gif_output_t *)(p_bmp + 12);

  width = *((u32 *)p_bmp);
  height = *((u32 *)(p_bmp + 4));
  fmt = *((u32 *)(p_bmp + 8));

  pitch = width * 4;
  in_size = height * pitch;

  rgbapalette2argb(p_gif_dst->frmdata[0], (u32 *)p_sample->p_data);
  
  rgn_width = RECTW(p_priv->fill_rect);
  rgn_height = RECTH(p_priv->fill_rect);

  if (!p_sample->context)
  {
    gpe_draw_rectangle_vsb(p_gpe_dev, p_priv->p_rgn, &p_priv->fill_rect, 0x00000000);
  }

  p_rgn_rect->left = p_priv->fill_rect.left + (rgn_width - width) / 2;
  p_rgn_rect->top = p_priv->fill_rect.top + (rgn_height - height) / 2;
  p_rgn_rect->right = p_rgn_rect->left + width;
  p_rgn_rect->bottom = p_rgn_rect->top + height;
  gpe_param.enable_colorkey = p_priv->enable_key;
  gpe_param.colorkey = p_priv->clr_key;//0Xff000000

  gpe_draw_image_vsb(p_gpe_dev, p_priv->p_rgn, p_rgn_rect,
    p_sample->p_data, NULL, 0, pitch,
    in_size, PIX_FMT_ARGB8888, &gpe_param, &fill_rect);
}

static void dec_line_draw_line(handle_t _this, media_sample_t *p_sample, rect_t *p_rgn_rect)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  gpe_param_vsb_t gpe_param = {0};
  u32 pitch = 0;
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  void *p_pic_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  u8 *p_bmp = NULL;
  u32 width = 0, height = 0;
  rect_t fill_rect = {0};
  u32 rgn_width = 0;
  u32 rgn_height = 0;
  u32 loopi = 0;
  u32 loopj = 0;
  u32 *pdec_ptr = NULL;
  
  p_bmp = p_sample->p_data + p_sample->data_offset;

  width = *((u32 *)p_bmp);
  height = *((u32 *)(p_bmp + 4));
  pdec_ptr = (u32 *)(*((u32 *)(p_bmp + 8)));
  
  pitch = width * 4;

  rgn_width = RECTW(p_priv->fill_rect);
  rgn_height = RECTH(p_priv->fill_rect);

  //MT_ASSERT(rgn_width >= width);
  //MT_ASSERT(rgn_height >= height);

  p_rgn_rect->left = p_priv->fill_rect.left + (rgn_width - width) / 2;
  p_rgn_rect->top = p_priv->fill_rect.top + (rgn_height - height) / 2;
  p_rgn_rect->right = p_rgn_rect->left + width;
  p_rgn_rect->bottom = p_rgn_rect->top + DEC_LINE_NUM;
  //gpe_draw_rectangle_vsb(p_gpe_dev, p_priv->p_rgn, &p_priv->fill_rect, 0x00000000);
  
  while (loopi < height)
  {
    mtos_task_lock();
    loopj = ((height - loopi) > DEC_LINE_NUM ? DEC_LINE_NUM : (height - loopi));
    pdec_get_line(p_pic_dev, p_sample->p_data, (unsigned int *)(&loopj), pdec_ptr);
    loopi += loopj;
    
    gpe_draw_image_vsb(p_gpe_dev, p_priv->p_rgn, p_rgn_rect, 
      p_sample->p_data, NULL, 0, pitch, 
      loopj * pitch, PIX_FMT_ARGB8888, &gpe_param, &fill_rect);
    mtos_task_unlock();
    
    p_rgn_rect->top += loopj;
    p_rgn_rect->bottom += loopj;
#ifdef WIN32
    mtos_task_sleep(5);
#endif
  }
  pdec_stop(p_pic_dev, pdec_ptr);
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  os_msg_t msg = {0};
  void *p_disp = NULL;
  rect_t rgn_rect = {0};
  u32 fmt = *((u32 *)(p_sample->p_data + p_sample->data_offset + 8));

  if (p_sample->payload == 8)
  {
    dec_line_draw_line(_this, p_sample, &rgn_rect);
    msg.content = PIC_DRAW_DYNAMIC_END;
    {
      ipin_t *p_ipin = (ipin_t *)_this;
      p_ipin->send_message_out(p_ipin, &msg);
    }
    return ;
  }

  switch (fmt)
  {
  case PIX_FMT_ARGB8888:
  case PIX_FMT_AYCBCR8888:
    msg.content = PIC_DRAW_END;
    rgba_render(_this, p_sample);
    break;

  case PIX_FMT_RGBPALETTE8:
    msg.content = PIC_DRAW_DYNAMIC_END;
    rgbapalette_render(_this, p_sample, &rgn_rect);
    break;

  default:
    return ;
  }

#ifndef WIN32
  {
    // need update the region

    p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);
    if (p_sample->context)
    {
      disp_layer_update_region(p_disp, p_priv->p_rgn, &rgn_rect);
    }
    else
    {
      disp_layer_update_region(p_disp, p_priv->p_rgn, NULL);
    }
  }
#endif

  {
    ipin_t *p_ipin = (ipin_t *)_this;

    //OS_PRINTF("\n##debug: pic render end!!\n");
    p_ipin->send_message_out(p_ipin, &msg);
  }
}

static list_media_format_t * media_format(handle_t _this)
{
  return NULL;
}

static BOOL notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

BOOL pic_render_pin_check_parameter(void)
{
  //must be sure the buffer size is enough
  if (PIC_RENDER_PIN_PRIVAT_DATA < sizeof(pic_render_pin_private_t))
  {
    return FALSE;
  }

  //ready all
  return TRUE;
}

pic_render_pin_t * pic_render_pin_create(pic_render_pin_t *p_pin, interface_t *p_owner)
{
  pic_render_pin_private_t *p_priv = NULL;
  pic_render_pin_t *p_ins = p_pin;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;

  //check input parameter
  MT_ASSERT(p_ins != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "pic_render_pin";
  sink_pin_create(&p_ins->base_pin, &sink_pin_para);

  //init private date
  p_priv = (pic_render_pin_private_t *)p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_render_pin_private_t));
  p_priv->p_this = p_ins; //this point

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_ins;
  p_input_pin->on_receive = on_receive;
  p_input_pin->notify_allocator = notify_allocator;

  p_ipin = (ipin_t *)p_ins;
  p_ipin->list_media_format = media_format;
  p_ipin->on_open = on_open;
  p_ipin->on_close = on_close;
  p_ipin->on_stop = on_stop;

  return p_ins;
}

//end of file

