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
#include "mtos_task.h"
#include "mtos_misc.h"

#include "common.h"
#include "drv_dev.h"
#include "../src/drv/pdec/alg/wa_djpeg/jpeglib.h"
#include "pdec.h"

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
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "pic_input_pin_intra.h"
#include "pic_output_pin_intra.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "pic_filter.h"
#include "pic_filter_intra.h"

typedef struct tag_pic_filter_private
{
  /*!
    this point !!
    */
  pic_filter_t *p_this;

  /*!
    it's input pin
    */
  pic_in_pin_t m_in_pin;

  /*!
    it's output pin
    */
  pic_out_pin_t m_out_pin;

  /*!
    using sample
    */
  media_sample_t *p_output_sample;
  /*!
    scale
    */
  rect_size_t size;
  /*!
    pdec mode
    */
  dec_mode_t dec_mode;
  /*!
    flip
    */
  u32 flip;
  /*!
    output format
    */
  u32 out_fmt;
  /*!
    input buffer size
    */
  u32 input_buffer_size;
  /*!
    output buffer size
    */
  u32 output_buffer_size;
  /*!
    current postion for receive.
    */
  u32 input_current;
  /*!
    frame index, for gif
    */
  u16 frm_index;
  /*!
    frame cnt, for gif
    */
  u16 frm_cnt;
  /*!
    frm_delay_time, for gif
    */
  u32 frm_delay_time;
  /*!
    for pdec
    */
  u32 *pdec_ptr;
  /*!
    for gif frames
    */
  u8 *p_gif;
  /*!
    for draw
    */
  u8 *p_draw;
  /*!
    input buffer
    */
  void *p_input;
  /*!
    output buffer
    */
  u8 *p_output;
  /*!
      last ticks
      */
  u32 last_ticks;
}pic_filter_private_t;

static pic_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return (pic_filter_private_t *)((pic_filter_t *)_this)->private_data;
}

static RET_CODE pic_on_command(handle_t _this, icmd_t *p_cmd)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  pic_filter_cfg_t *p_cfg = NULL;
  switch (p_cmd->cmd)
  {
    case PIC_CFG_SIZE:
      p_priv->size.w = ((rect_size_t *)p_cmd->p_para)->w;
      p_priv->size.h = ((rect_size_t *)p_cmd->p_para)->h;
      break;
    case PIC_CFG_FLIP:
      p_priv->flip = p_cmd->lpara;
      break;
    case PIC_CFG_OUT_FMT:
      p_priv->out_fmt = p_cmd->lpara;
      break;
    case PIC_CFG_DEC_MODE:
      p_priv->dec_mode = p_cmd->lpara;
      break;
    case PIC_CFG_INPUT_BUFFER:
      p_priv->input_buffer_size = p_cmd->lpara;
      p_priv->p_input = p_cmd->p_para;
      break;
    case PIC_CFG_OUTPUT_BUFFER:
      p_priv->output_buffer_size = p_cmd->lpara;
      p_priv->p_output = p_cmd->p_para;
      //pic_out_pin_cfg(&p_priv->m_out_pin, p_cmd->lpara, p_cmd->p_para);
      pic_out_pin_cfg(&p_priv->m_out_pin, sizeof(gif_output_t) + 12, NULL);
      break;
    case PIC_FILTER_CFG:
      p_cfg = (pic_filter_cfg_t *)p_cmd->p_para;
      p_priv->size.w = p_cfg->w;
      p_priv->size.h = p_cfg->h;
      p_priv->flip = p_cfg->flip;
      p_priv->out_fmt = p_cfg->out_fmt;
      p_priv->dec_mode = p_cfg->dec_mode;
      p_priv->input_buffer_size = p_cfg->input_buf_size;
      p_priv->p_input = p_cfg->p_input_buf;
      p_priv->p_output = p_cfg->p_out_buf;
      p_priv->output_buffer_size = p_cfg->out_buf_size;
      pic_out_pin_cfg(&p_priv->m_out_pin, sizeof(gif_output_t) + 12, NULL);
      break;
    default:
      return ERR_FAILURE;
  }

  return SUCCESS;
}

static RET_CODE pic_on_start(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);

  p_priv->input_current = 0;
  p_priv->frm_index = 0;
  p_priv->frm_cnt = 0;
  p_priv->p_gif = NULL;
  p_priv->pdec_ptr = NULL;
  p_priv->p_draw = NULL;
  p_priv->last_ticks = 0;
  return SUCCESS;
}

static RET_CODE pic_on_stop(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = NULL;
  drv_dev_t *p_pic_dev = NULL;

  if (p_priv->frm_cnt == 1)
  {
    p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
    pdec_stop(p_pic_dev, p_priv->pdec_ptr);
    p_priv->frm_cnt = 0;
  }

  if (p_priv->p_gif)
  {
    mtos_align_free(p_priv->p_gif);
    p_priv->p_gif = NULL;
  }
  p_ifilter = (ifilter_t *)p_priv->p_this;
  p_ifilter->set_active_enter(_this, NULL);

  return SUCCESS;
}

#ifdef WIN32
static pic_scale_t pic_cacl_scale(rect_size_t *p_dst, rect_size_t *p_src)
{
  u32 dst_w = 0;
  u32 dst_h = 0;
  u32 src_w = 0;
  u32 src_h = 0;
  pic_scale_t scale = JPEG_SCALE_ERROR;

  MT_ASSERT(p_dst != NULL);
  MT_ASSERT(p_src != NULL);

  dst_w = p_dst->w;
  dst_h = p_dst->h;
  src_w = p_src->w;
  src_h = p_src->h;

  MT_ASSERT(dst_w != 0);
  MT_ASSERT(dst_h != 0);
  MT_ASSERT(src_w != 0);
  MT_ASSERT(src_h != 0);

  if(src_w <= dst_w)
  {
    if(src_h <= dst_h)
    {
      scale = JPEG_SCALE_W11_H11;
    }
    else if(src_h <= dst_h * 2)
    {
      scale = JPEG_SCALE_W12_H12;
    }
    else if(src_h <= dst_h * 4)
    {
      scale = JPEG_SCALE_W14_H14;
    }
  }
  else if(src_w <= dst_w * 2)
  {
    if(src_h <= dst_h * 2)
    {
      scale = JPEG_SCALE_W12_H12;
    }
    else if(src_h <= dst_h * 4)
    {
      scale = JPEG_SCALE_W14_H14;
   }
  }
  else if(src_w <= dst_w * 4)
  {
    if(src_h <= dst_h * 4)
    {
      scale = JPEG_SCALE_W14_H14;
    }
 }

 return scale;
}


static void pic_calc_dest_rect(rect_size_t *p_dest, rect_size_t *p_src, pic_scale_t scale)
{
  switch(scale)
  {
    case JPEG_SCALE_W11_H11:
      p_dest->w = p_src->w;
      p_dest->h = p_src->h;
      break;
    case JPEG_SCALE_W12_H12:
      p_dest->w = (int)((p_src->w / 2 + 3) / 4) * 4;
      p_dest->h = (int)((p_src->h / 2) / 4) * 4;
      break;

    case JPEG_SCALE_W14_H14:
      p_dest->w = (int)((p_src->w / 4 + 3) / 4) * 4;
      p_dest->h = (int)((p_src->h / 4) / 4) * 4;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  MT_ASSERT(p_dest->w != 0);
  MT_ASSERT(p_dest->h != 0);
}
#endif

static void jpeg_transform(handle_t _this, media_sample_t *p_out_sample,
  pic_param_t *p_pic_param, rect_size_t *p_src, rect_size_t *p_dst)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  pic_scale_t scale = JPEG_SCALE_ERROR;

#ifdef WIN32
  scale = pic_cacl_scale(&p_priv->size, p_src);

  MT_ASSERT(scale != JPEG_SCALE_ERROR);

  pic_calc_dest_rect(p_dst, p_src, scale);
#else
  if(p_src->w <= p_priv->size.w)
  {
    if(p_src->h <= p_priv->size.h)
    {
      p_pic_param->scale_w_num = 1;
      p_pic_param->scale_w_demo = 1;
      p_pic_param->scale_h_num = 1;
      p_pic_param->scale_h_demo = 1;

      p_dst->w = p_src->w;
      p_dst->h = p_src->h;
    }
    else
    {
      p_pic_param->scale_w_num = p_priv->size.h;
      p_pic_param->scale_w_demo = p_src->h;
      p_pic_param->scale_h_num = p_priv->size.h;
      p_pic_param->scale_h_demo = p_src->h;

      p_dst->w = p_src->w * p_priv->size.h / p_src->h;
      p_dst->h = p_priv->size.h;
    }
  }
  else
  {
    if(p_src->h <= p_priv->size.h)
    {
      p_pic_param->scale_w_num = p_priv->size.w;
      p_pic_param->scale_w_demo = p_src->w;
      p_pic_param->scale_h_num = p_priv->size.w;
      p_pic_param->scale_h_demo = p_src->w;

      p_dst->w = p_priv->size.w;
      p_dst->h = p_priv->size.h * p_priv->size.w / p_src->w;
    }
    else
    {
      if(p_src->w * p_priv->size.h >= p_src->h * p_priv->size.w)
      {
        p_pic_param->scale_w_num = p_priv->size.w;
        p_pic_param->scale_w_demo = p_src->w;
        p_pic_param->scale_h_num = p_priv->size.w;
        p_pic_param->scale_h_demo = p_src->w;

        p_dst->w = p_priv->size.w;
        p_dst->h = p_priv->size.h * p_priv->size.w / p_src->w;
      }
      else
      {
        p_pic_param->scale_w_num = p_priv->size.h;
        p_pic_param->scale_w_demo = p_src->h;
        p_pic_param->scale_h_num = p_priv->size.h;
        p_pic_param->scale_h_demo = p_src->h;

        p_dst->w = p_src->w * p_priv->size.h / p_src->h;
        p_dst->h = p_priv->size.h;
      }
    }
  }
#endif

  *((u32 *)(p_out_sample->p_data)) = p_dst->w;
  *((u32 *)(p_out_sample->p_data + 4)) = p_dst->h;
#ifdef WIN32
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_ARGB8888;
#else
#ifdef JAZZ
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_ARGB8888;
#else
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_AYCBCR8888;//PIX_FMT_CBY0CRY18888;
#endif
#endif
  p_pic_param->src_rect = p_priv->p_input;
  p_pic_param->dest_rect = p_out_sample->p_data + 12;
  p_pic_param->scale = scale;
  p_pic_param->flip = p_priv->flip;
  p_pic_param->output_format = p_priv->out_fmt;
  p_pic_param->src_size = p_priv->input_current;
  p_pic_param->disp_stride = p_dst->w;

  p_pic_param->disp_width = p_dst->w ;
  p_pic_param->disp_height = p_dst->h ;
}

static void gif_transform(handle_t _this,
  pic_param_t *p_pic_param, rect_size_t *p_src, rect_size_t *p_dst, pic_info_t *p_pic_info)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  gif_output_t *p_gif_dst = NULL;
  u32 loopi = 0;
  u32 malloc_cnt = 0;
  u8 *p_buffer = NULL;
  u32 output_left = 0;
  u32 draw_need = 0;

  p_buffer = (u8 *)p_priv->p_output + 12;
  output_left = p_priv->output_buffer_size;
  p_gif_dst = (gif_output_t *)p_buffer;
  p_buffer += (sizeof(gif_output_t) + 12);
  output_left -= (sizeof(gif_output_t) + 12);
  loopi = (u32)p_buffer % 4;

  if (loopi)
  {
    output_left -= loopi;
    p_buffer += loopi;
  }

  switch (p_priv->dec_mode)
  {
  case DEC_LINE_MODE:
    draw_need = p_pic_info->src_width * 50 * 4 + 4;
    break;
  default:
    draw_need = p_pic_info->src_width * p_pic_info->src_height * 4 + 4;
    break;
  }

  if (output_left < (draw_need + p_priv->frm_cnt * p_pic_info->dst_frm_size))
  {
    malloc_cnt = p_priv->frm_cnt - output_left / p_pic_info->dst_frm_size;
    OS_PRINTF("\n##debug: gif_transform malloc [%dX%d]\n", malloc_cnt, p_pic_info->dst_frm_size);
    p_priv->p_gif = mtos_align_malloc(draw_need + malloc_cnt * p_pic_info->dst_frm_size + 4, 4);
    MT_ASSERT(p_priv->p_gif != NULL);

    memset(p_priv->p_gif, 0, draw_need + malloc_cnt * p_pic_info->dst_frm_size);
    p_priv->p_draw = p_priv->p_gif + malloc_cnt * p_pic_info->dst_frm_size;
  }
  else
  {
    p_priv->p_draw = p_buffer + p_priv->frm_cnt * p_pic_info->dst_frm_size;
  }
  p_gif_dst->frmsize = p_pic_info->dst_frm_size;
  p_gif_dst->biHeight = p_pic_info->src_height;
  p_gif_dst->biWidth = p_pic_info->src_width;
  p_gif_dst->oncedecnum = p_priv->frm_cnt;
  for(loopi = 0; loopi < (p_priv->frm_cnt - malloc_cnt); loopi ++)
  {
    p_gif_dst->frmdata[loopi] = p_buffer;
    p_buffer += p_gif_dst->frmsize;
  }

  if ((u32)p_priv->p_draw % 4)
  {
    p_priv->p_draw += ((u32)p_priv->p_draw % 4);
  }

  if (malloc_cnt)
  {
    p_buffer = p_priv->p_gif;
    for(loopi = p_priv->frm_cnt - malloc_cnt; loopi < p_priv->frm_cnt; loopi ++)
    {
      p_gif_dst->frmdata[loopi] = p_buffer;
      p_buffer += p_gif_dst->frmsize;
    }
  }
  *((u32 *)(p_priv->p_output)) = p_src->w;
  *((u32 *)(p_priv->p_output + 4)) = p_src->h;
  *((u32 *)(p_priv->p_output + 8)) = PIX_FMT_RGBPALETTE8;

  p_pic_param->dest_rect =(void *)p_gif_dst;
}

static void gif_frame_loop_transform(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  gif_output_t *p_gif_dst = NULL;
  drv_dev_t *p_pic_dev = NULL;
  ifilter_t *p_ifilter = NULL;

  if ((mtos_ticks_get() - p_priv->last_ticks) < (p_priv->frm_delay_time / 10))
  {
    return;
  }

  p_priv->last_ticks = mtos_ticks_get();

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  switch (p_priv->dec_mode)
  {
  case DEC_LINE_MODE:
    break;

  case DEC_FRAME_MODE:
    {
      p_priv->frm_index ++;

      pdec_start(p_pic_dev, (void *)p_priv->pdec_ptr);

      p_gif_dst = (gif_output_t *)(p_priv->p_output + 12);
      if (!p_gif_dst->realdecnum)
      {
        p_ifilter = (ifilter_t *)p_priv->p_this;
        p_ifilter->set_active_enter(_this, NULL);
        p_priv->frm_cnt = 0;
        pdec_stop(p_pic_dev, (void *)p_priv->pdec_ptr);
        return ;
      }

      p_out_pin->create_new_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);
      p_out_sample->payload = sizeof(gif_output_t) + 12;
      p_out_sample->p_data = p_priv->p_draw;

      memcpy(p_out_sample->p_data, p_priv->p_output, p_out_sample->payload);

    }
    break;

  case DEC_ALL_MODE:
    {
      p_priv->frm_index ++;
      p_priv->frm_index %= p_priv->frm_cnt;
      p_out_pin->create_new_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);
      p_out_sample->payload = sizeof(gif_output_t) + 12;
      p_out_sample->p_data = p_priv->p_draw;
      memcpy(p_out_sample->p_data, p_priv->p_output, p_out_sample->payload);
      p_gif_dst = (gif_output_t *)(p_out_sample->p_data + 12);
      p_gif_dst->frmdata[0] = ((gif_output_t *)(p_priv->p_output + 12))->frmdata[p_priv->frm_index];
    }
    break;
  default:
    break;
  }

  p_out_sample->context = p_priv->frm_delay_time;
  p_out_pin->push_down(p_out_pin, p_out_sample);
}

static void png_transform(handle_t _this, media_sample_t *p_out_sample,
  pic_param_t *p_pic_param, rect_size_t *p_src, rect_size_t *p_dst)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  pic_scale_t scale = JPEG_SCALE_ERROR;

#ifdef WIN32
  scale = pic_cacl_scale(&p_priv->size, p_src);

  MT_ASSERT(scale != JPEG_SCALE_ERROR);

  pic_calc_dest_rect(p_dst, p_src, scale);
#else
  if(p_src->w <= p_priv->size.w)
  {
    if(p_src->h <= p_priv->size.h)
    {
      p_pic_param->scale_w_num = 1;
      p_pic_param->scale_w_demo = 1;
      p_pic_param->scale_h_num = 1;
      p_pic_param->scale_h_demo = 1;

      p_dst->w = p_src->w;
      p_dst->h = p_src->h;
    }
    else
    {
      p_pic_param->scale_w_num = p_priv->size.h;
      p_pic_param->scale_w_demo = p_src->h;
      p_pic_param->scale_h_num = p_priv->size.h;
      p_pic_param->scale_h_demo = p_src->h;

      p_dst->w = p_src->w * p_priv->size.h / p_src->h;
      p_dst->h = p_priv->size.h;
    }
  }
  else
  {
    if(p_src->h <= p_priv->size.h)
    {
      p_pic_param->scale_w_num = p_priv->size.w;
      p_pic_param->scale_w_demo = p_src->w;
      p_pic_param->scale_h_num = p_priv->size.w;
      p_pic_param->scale_h_demo = p_src->w;

      p_dst->w = p_priv->size.w;
      p_dst->h = p_priv->size.h * p_priv->size.w / p_src->w;
    }
    else
    {
      if(p_src->w * p_priv->size.h >= p_src->h * p_priv->size.w)
      {
        p_pic_param->scale_w_num = p_priv->size.w;
        p_pic_param->scale_w_demo = p_src->w;
        p_pic_param->scale_h_num = p_priv->size.w;
        p_pic_param->scale_h_demo = p_src->w;

        p_dst->w = p_priv->size.w;
        p_dst->h = p_priv->size.h * p_priv->size.w / p_src->w;
      }
      else
      {
        p_pic_param->scale_w_num = p_priv->size.h;
        p_pic_param->scale_w_demo = p_src->h;
        p_pic_param->scale_h_num = p_priv->size.h;
        p_pic_param->scale_h_demo = p_src->h;

        p_dst->w = p_src->w * p_priv->size.h / p_src->h;
        p_dst->h = p_priv->size.h;
      }
    }
  }
#endif

  *((u32 *)(p_out_sample->p_data)) = p_dst->w;
  *((u32 *)(p_out_sample->p_data + 4)) = p_dst->h;
#ifdef WIN32
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_ARGB8888;
#else
#ifdef JAZZ
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_ARGB8888;
#else
  *((u32 *)(p_out_sample->p_data + 8)) = PIX_FMT_AYCBCR8888;//PIX_FMT_CBY0CRY18888;
#endif
#endif
  p_pic_param->src_rect = p_priv->p_input;
  p_pic_param->dest_rect = p_out_sample->p_data + 12;
  p_pic_param->scale = scale;
  p_pic_param->flip = p_priv->flip;
  p_pic_param->output_format = p_priv->out_fmt;
  p_pic_param->src_size = p_priv->input_current;
  p_pic_param->disp_stride = p_dst->w;

  p_pic_param->disp_width = p_dst->w ;
  p_pic_param->disp_height = p_dst->h ;
}

static RET_CODE pic_transform(handle_t _this, media_sample_t *p_in)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  drv_dev_t *p_pic_dev = NULL;
  pic_info_t pic_info = {0};
  pic_param_t pic_param = {0};
  rect_size_t src_size = {0};
  rect_size_t dst_size = {0};
  ifilter_t *p_ifilter = NULL;

  if (p_in->payload
    && p_in->payload == p_in->data_offset
    && p_in->payload == p_in->context)
  {
    p_priv->p_input = p_in->p_data;
    p_priv->input_current = p_in->payload;
  }
  else
  {
    MT_ASSERT((p_in->payload + p_priv->input_current) <= p_priv->input_buffer_size);
    if (p_priv->p_gif)
    {
      mtos_align_free(p_priv->p_gif);
      p_priv->p_gif = NULL;
    }

    if (p_in->payload == 0)
    {
      OS_PRINTF("payload 0 return \n");
      return ERR_FAILURE;
    }
    memcpy(((u8 *)p_priv->p_input + p_priv->input_current),
      (p_in->p_data + p_in->data_offset), p_in->payload);

    p_priv->input_current += p_in->payload;
  }
  if (p_in->state != SAMP_STATE_INSUFFICIENT)
  {
    //to be continued......
    return ERR_FAILURE;
  }

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);

  p_out_pin->create_new_sample(p_out_pin, &p_out_sample);
  pic_info.dec_mode = p_priv->dec_mode;
  pdec_getinfo(p_pic_dev, p_priv->p_input, p_priv->input_current, &pic_info, (void **)(&p_priv->pdec_ptr));

  switch (pic_info.image_format)
  {
  case IMAGE_FORMAT_JPEG:
    {
      src_size.w = pic_info.src_width;
      src_size.h = pic_info.src_height;

      jpeg_transform(_this, p_out_sample, &pic_param, &src_size, &dst_size);
      pdec_setinfo(p_pic_dev, &pic_param, (void *)p_priv->pdec_ptr);
      if (p_priv->dec_mode == DEC_LINE_MODE)
      {
        p_out_sample->payload = 12;
      }
      else
      {
        pdec_start(p_pic_dev, (void *)p_priv->pdec_ptr);

        pdec_stop(p_pic_dev, (void *)p_priv->pdec_ptr);
      }
      p_out_pin->push_down(p_out_pin, p_out_sample);
    }
    break;
  case IMAGE_FORMAT_GIF:
    {
      src_size.w = pic_info.src_width;
      src_size.h = pic_info.src_height;
      p_out_sample->context = 0;
      p_priv->frm_index = 0;
      if (p_priv->dec_mode == DEC_ALL_MODE)
      {
        p_priv->frm_cnt = pic_info.framenums;
      }
      else
      {
        p_priv->frm_cnt = 1;
      }
      p_priv->frm_delay_time = pic_info.daleytime;
      p_ifilter = (ifilter_t *)p_priv->p_this;

      gif_transform(_this, &pic_param, &src_size, &dst_size, &pic_info);
      pdec_setinfo(p_pic_dev, &pic_param, (void *)p_priv->pdec_ptr);

      switch (p_priv->dec_mode)
      {
      case DEC_LINE_MODE:
        {
          p_out_sample->payload = 12;
          *((u32 *)(p_priv->p_output + 8)) = (u32)p_priv->pdec_ptr;
          p_out_sample->p_data = p_priv->p_draw;
          memcpy(p_out_sample->p_data, p_priv->p_output, p_out_sample->payload);
        }
        break;

      case DEC_FRAME_MODE:
        {
          pdec_start(p_pic_dev, (void *)p_priv->pdec_ptr);
          p_ifilter->set_active_enter(_this, gif_frame_loop_transform);
          p_priv->frm_cnt = 1;
          p_out_sample->payload = sizeof(gif_output_t) + 12;
          p_out_sample->p_data = p_priv->p_draw;
          memcpy(p_out_sample->p_data, p_priv->p_output, p_out_sample->payload);
        }
        break;

      case DEC_ALL_MODE:
      default:
        {
          pdec_start(p_pic_dev, (void *)p_priv->pdec_ptr);
          if (p_priv->frm_cnt > 1)
          {
            p_ifilter->set_active_enter(_this, gif_frame_loop_transform);
          }
          pdec_stop(p_pic_dev, (void *)p_priv->pdec_ptr);
          p_out_sample->p_data = p_priv->p_draw;
          p_out_sample->payload = sizeof(gif_output_t) + 12;
          memcpy(p_out_sample->p_data, p_priv->p_output, p_out_sample->payload);
        }
        break;
      }
      p_out_pin->push_down(p_out_pin, p_out_sample);
    }
    break;
  case IMAGE_FORMAT_PNG:
    {
      png_transform(_this, p_out_sample, &pic_param, &src_size, &dst_size);
      pdec_setinfo(p_pic_dev, &pic_param, (void *)p_priv->pdec_ptr);

      pdec_start(p_pic_dev, (void *)p_priv->pdec_ptr);

      pdec_stop(p_pic_dev, (void *)p_priv->pdec_ptr);

      p_out_pin->push_down(p_out_pin, p_out_sample);
    }
    break;
  default:
    return ERR_FAILURE;
  }

  return SUCCESS;
}

BOOL pic_filter_check_parameter(void)
{
  //check input pin
  if(!pic_in_pin_check_parameter())
  {
    return FALSE;
  }
  //check output pin
  if(!pic_out_pin_check_parameter())
  {
    return FALSE;
  }

  //must be sure the buffer size is enough
  if(PIC_FILTER_PRIVAT_DATA < sizeof(pic_filter_private_t))
  {
    EVA_ERROR("jpeg filter private data size %d\n",sizeof(pic_filter_private_t));
    return FALSE;
  }

  //other check ....

  //ready all
  return TRUE;
}

ifilter_t * pic_filter_create(void)
{
  pic_filter_private_t *p_priv = NULL;
  pic_filter_t *p_ins = NULL;
  transf_filter_t *p_transf_filter = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para;

  transf_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(pic_filter_t));
  MT_ASSERT(p_ins != NULL);
  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = (pic_filter_private_t *)p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_filter_private_t));

  p_priv->p_this = p_ins; //this point
  p_priv->p_output_sample = NULL;

  p_priv->input_buffer_size = 0;
  p_priv->p_input = NULL;
  p_priv->output_buffer_size = 0;
  p_priv->p_output = NULL;
  p_priv->flip = PIC_FLIP_0;
  p_priv->out_fmt = PIC_OUTFMT_422;
  p_priv->input_current = 0;

  //init member function

  //over loading the virtual function
  p_transf_filter = (transf_filter_t *)p_ins;
  p_transf_filter->transform = pic_transform;

  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = pic_on_command;
  p_ifilter->on_start = pic_on_start;
  p_ifilter->on_stop = pic_on_stop;

  //create it's pin
  pic_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  pic_out_pin_create(&p_priv->m_out_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}


