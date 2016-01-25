/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mem_manager.h"

#include "lib_memf.h"
#include "lib_rect.h"
#include "class_factory.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "common.h"
#include "gui_resource.h"
#include "surface.h"
#include "surface_priv.h"
#include "surface_v2.h"


//#define SURF_DEBUG

#ifdef SURF_DEBUG
#define SURF_PRINTF    OS_PRINTF
#else
#define SURF_PRINTF DUMMY_PRINTF
#endif

/*!
   This structrue is defined to collect the global information for this module.
  */
typedef struct
{
  /*!
     The memf object to alloc the structure of surface.
    */
  lib_memf_t surf_heap;
  /*!
     The buffer of the memf object.
    */
  void *p_heap_addr;
}surface_priv_v2_t;

/*!
   The structure is defined to descript a surface.
  */
typedef struct
{
  /*!
     The pixel format
    */
  pix_fmt_t format;
  /*!
     Bits per pixel
    */
  u8 bpp;
  /*!
     The global alpha
    */
  u8 alpha;
  /*!
     The attributes of the surface
    */
  u8 attr;
  /*!
     The width
    */
  u16 width;
  /*!
     The height
    */
  u16 height;
  /*!
     The pitch
    */
  u32 pitch;
  /*!
     colorkey
    */
  u32 colorkey;
  /*!
     Points to a palette
    */
  palette_t *p_pal;
  /*!
     The clipping rectangle
    */
  rect_t rc_clip;
  /*!
     The display rectangle
    */
  rect_t rc_src;
  /*!
     The handle of the OSD region.
    */
  s32 handle;
  /*!
     The display buffer odd & even.
    */
  u8 layer_id;
  /*!
     Points to a GPE object.
    */
  void *p_gpe_dev;
  /*!
    display device
    */
  void *p_disp;  
  /*!
     Point to the OSD region.
    */
  void *p_osd_rgn;  
} surface_v2_t;

#ifdef SURF_DEBUG

static void surface_dump_info_v2(surface_v2_t *p_info)
{
  SURF_PRINTF("dump surface info......\n");
  SURF_PRINTF("\t format   = %d\n", p_info->format);
  SURF_PRINTF("\t bpp      = %d\n", p_info->bpp);
  SURF_PRINTF("\t alpha    = 0x%x\n", p_info->alpha);
  SURF_PRINTF("\t attr     = %d\n", p_info->attr);
  SURF_PRINTF("\t width    = %d\n", p_info->width);
  SURF_PRINTF("\t height   = %d\n", p_info->height);
  SURF_PRINTF("\t pitch    = %d\n", p_info->pitch);
  SURF_PRINTF("\t colorkey = 0x%x\n", p_info->colorkey);
  SURF_PRINTF("\t pal cnt  = %d\n", p_info->p_pal->cnt);
  SURF_PRINTF("\t pal entry (%4d, %4d, %4d, %4d)\n",
              p_info->p_pal->p_entry[0].a, p_info->p_pal->p_entry[0].r,
              p_info->p_pal->p_entry[0].g, p_info->p_pal->p_entry[0].b);
  SURF_PRINTF("\t clip rect (%4d, %4d, %4d, %4d)\n",
              p_info->rc_clip.left, p_info->rc_clip.top,
              p_info->rc_clip.right, p_info->rc_clip.bottom);
  SURF_PRINTF("\t src  rect (%4d, %4d, %4d, %4d)\n",
              p_info->rc_src.left, p_info->rc_src.top,
              p_info->rc_src.right, p_info->rc_src.bottom);
  SURF_PRINTF("\t handle   = %d\n", p_info->handle);
//  SURF_PRINTF("\t vmem      (0x%.8x, 0x%.8x)\n", (u32)p_info->p_vmem[0],
//              (u32)p_info->p_vmem[1]);
}


#endif

static surface_v2_t *surface_alloc_v2(void)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v2_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v2_t *)(p_info->p_data);  

  return (surface_v2_t *)lib_memf_alloc(&p_priv->surf_heap);
}


static void surface_free_v2(void *p_addr)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v2_t *p_priv = NULL;

  MT_ASSERT(p_addr != NULL);

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v2_t *)(p_info->p_data);  

  lib_memf_free(&p_priv->surf_heap, p_addr);
}


static u32 calc_pitch_v2(u16 bpp, u16 width)
{
  return (u32)((width * bpp + 7) >> 3);
}



static BOOL get_effect_rc_v2(rect_t *p_erc, rect_t *p_crc)
{
  return intersect_rect(p_erc, p_erc, p_crc);
}

static void surface_release_v2(void)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v2_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v2_t *)(p_info->p_data);  

  // free surface
  lib_memf_destroy(&p_priv->surf_heap);
  mtos_free(p_priv->p_heap_addr);

  //free global infor
  mtos_free(p_priv);

  //free proc info
  mtos_free(p_info);
}


static void surface_enable_v2(void *p_surface, BOOL is_enable)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  void *p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);  
  
  disp_layer_show(p_disp, p_surf->layer_id, is_enable);
}


static RET_CODE surface_create_v2(void **pp_surf, surface_desc_t *p_desc)
{
  u16 width = 0, height = 0;
  surface_v2_t *p_surf = NULL;
  surface_proc_t *p_info = NULL;
  surface_priv_v2_t *p_priv = NULL;
  rect_size_t rect_size = {0};
  point_t pos;  
  RET_CODE ret = SUCCESS;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v2_t *)(p_info->p_data);  

  SURF_PRINTF("SURF ALLOC \n");
  if((p_surf = surface_alloc_v2()) == NULL)
  {
    return ERR_NO_MEM;
  }
  SURF_PRINTF("SURF ALLOC OK \n");

  memset(p_surf, 0, sizeof(surface_v2_t));

  if(p_desc->flag & SURFACE_DESC_FORMAT)
  {
    switch(p_desc->format)
    {
      case COLORFORMAT_RGB4BIT:
        p_surf->format = PIX_FMT_RGBPALETTE4;
        break;
      case COLORFORMAT_RGB8BIT:
        p_surf->format = PIX_FMT_RGBPALETTE8;
        break;
      case COLORFORMAT_RGB565:  
        p_surf->format = PIX_FMT_RGB565;
        break;
      case COLORFORMAT_RGBA5551:  
        p_surf->format = PIX_FMT_RGBA5551;
        break;
      case COLORFORMAT_ARGB1555:  
        p_surf->format = PIX_FMT_ARGB1555;
        break;
      case COLORFORMAT_RGBA8888:  
        p_surf->format = PIX_FMT_RGBA8888;
        break;
      case COLORFORMAT_ARGB8888:  
        p_surf->format = PIX_FMT_ARGB8888;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }
  else
  {
    p_surf->format = PIX_FMT_RGB565;
  }

  switch(p_surf->format)
  {
    case PIX_FMT_RGBPALETTE4:
      p_surf->bpp = 4;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;      
      break;
    case PIX_FMT_RGBPALETTE8:
      p_surf->bpp = 8;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;      
      break;      
    case PIX_FMT_RGB565:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_RGBA5551:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_ARGB1555:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_RGBA8888:
      p_surf->bpp = 32;
      break;
    case PIX_FMT_ARGB8888:
      p_surf->bpp = 32;
      break;
    default:
      SURF_PRINTF("surface_create: UNSUPPORT format.\n");
      return ERR_NOFEATURE;
  }

  /* link related gpe device */
  p_surf->p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_surf->p_gpe_dev);
  // TODO: ... multi-device support ?
  p_surf->p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_surf->p_disp);  

  /* get current width&height */
  p_surf->width = p_desc->flag & SURFACE_DESC_WIDTH ? p_desc->width : width;
  p_surf->height = p_desc->flag & SURFACE_DESC_HEIGHT ? p_desc->height : height;
  p_surf->pitch = p_desc->flag & SURFACE_DESC_PITCH ? \
                  p_desc->pitch : calc_pitch_v2(p_surf->bpp, p_surf->width);

  if(p_desc->flag & SURFACE_DESC_CAPS)
  {
    switch(p_desc->caps)
    {
      case SURFACE_CAPS_MEM_ASSIGN:
        //create region.
        SURF_PRINTF("create osd region...\n");
        pos.x = 0;
        pos.y = 0;
        rect_size.w = p_surf->width;
        rect_size.h = p_surf->height;
        p_surf->p_osd_rgn = region_create(&rect_size, p_surf->format);
        MT_ASSERT(NULL != p_surf->p_osd_rgn);
#if 0
        ret = disp_layer_add_region(
          p_surf->p_disp, DISP_LAYER_ID_OSD1, p_surf->p_osd_rgn, &pos, NULL);
        MT_ASSERT(SUCCESS == ret);

        if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
        {
          MT_ASSERT(p_surf->p_pal != NULL);
          region_set_palette(p_surf->p_osd_rgn, (u32 *)p_surf->p_pal->p_entry, p_surf->p_pal->cnt);
        }

        disp_layer_alpha_onoff(p_surf->p_disp, DISP_LAYER_ID_OSD1, TRUE);
        region_alpha_onoff(p_surf->p_osd_rgn, FALSE);

        disp_layer_show(p_surf->p_disp, DISP_LAYER_ID_OSD1, FALSE);

        p_surf->layer_id = DISP_LAYER_ID_OSD1;
#else
        p_surf->handle = -1;
        ((region_t *)(p_surf->p_osd_rgn))->p_buf_odd = p_desc->p_vmem[0];
        ((region_t *)(p_surf->p_osd_rgn))->p_buf_even = p_desc->p_vmem[1];
#endif
        break;
      case SURFACE_CAPS_MEM_SYSTEM:
        //create region.
        SURF_PRINTF("create osd region...\n");
        pos.x = 0;
        pos.y = 0;
        rect_size.w = p_surf->width;
        rect_size.h = p_surf->height;
        p_surf->p_osd_rgn = region_create(&rect_size, p_surf->format);
        MT_ASSERT(NULL != p_surf->p_osd_rgn);

        ret = disp_layer_add_region(
          p_surf->p_disp, DISP_LAYER_ID_OSD0, p_surf->p_osd_rgn, &pos, NULL);
        MT_ASSERT(SUCCESS == ret);

        if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
        {
          MT_ASSERT(p_surf->p_pal != NULL);
          region_set_palette(p_surf->p_osd_rgn, (u32 *)p_surf->p_pal->p_entry, p_surf->p_pal->cnt);
        }

        disp_layer_alpha_onoff(p_surf->p_disp, DISP_LAYER_ID_OSD0, TRUE);
        region_alpha_onoff(p_surf->p_osd_rgn, FALSE);

        p_surf->layer_id = DISP_LAYER_ID_OSD0;

        p_surf->handle = 0;
        break;
      default:
        SURF_PRINTF("surface_create: UNSUPPORT caps.\n");
        return ERR_NOFEATURE;
    }
  }
  p_surf->alpha = 0xFF;
  set_rect(&p_surf->rc_src, 0, 0, p_surf->width, p_surf->height);
  /* set surface */
  *pp_surf = (void *)p_surf;

#ifdef SURF_DEBUG
  surface_dump_info_v2(p_surf);
#endif

  return SUCCESS;
}


static RET_CODE surface_delete_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    if(p_surf->p_osd_rgn != NULL)
    {
      region_show(p_surf->p_osd_rgn, FALSE);
      p_surf->attr &= (~SURFACE_ATTR_EN_DISPLAY);
    }
  }

  if(p_surf->p_osd_rgn != NULL && p_surf->handle != -1)
  {
    //remove region from display layer.
    disp_layer_remove_region(p_surf->p_disp, p_surf->layer_id, p_surf->p_osd_rgn);
  }

  if(p_surf->p_osd_rgn != NULL)
  {
    //delete region
    region_delete(p_surf->p_osd_rgn);
  }

  if(p_surf->p_gpe_dev != NULL)
  {
    dev_close(p_surf->p_gpe_dev);
  }

  /* release */
  surface_free_v2(p_surf);
  return SUCCESS;
}


static RET_CODE surface_set_display_v2(void *p_surface, BOOL is_display)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if((p_surf->attr & SURFACE_ATTR_EN_DISPLAY) == is_display)
  {
    return SUCCESS;
  }

  if(p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }

  if(is_display)
  {
    p_surf->attr |= SURFACE_ATTR_EN_DISPLAY;
    /* DO SOMETHING */
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_DISPLAY);
    /* DO SOMETHING */
  }

  region_show(p_surf->p_osd_rgn, is_display);

  return SUCCESS;
}


static RET_CODE surface_set_alpha_v2(void *p_surface, u8 alpha)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  p_surf->alpha = alpha;

  disp_layer_alpha_onoff(p_surf->p_disp, p_surf->layer_id, TRUE);
  region_alpha_onoff(p_surf->p_osd_rgn, FALSE);

  disp_layer_set_alpha(p_surf->p_disp, p_surf->layer_id, alpha);

  return SUCCESS;
}

RET_CODE surface_set_trans_v2(void *p_surf, u32 index, u8 alpha)
{
  return SUCCESS;
}

static RET_CODE surface_init_palette_v2(void *p_surface, palette_t *p_pal)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
  {
    p_surf->p_pal = p_pal;
  }
  else
  {
    return ERR_FAILURE;
  }
  region_set_palette(p_surf->p_osd_rgn, (u32 *)p_pal->p_entry, p_pal->cnt);

  return SUCCESS;
}


RET_CODE surface_set_palette_v2(void *p_surface, u16 start, u16 len, color_t *p_entry)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  palette_t *p_pal = p_surf->p_pal;

  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE && p_pal != NULL)
  {
    memcpy(&p_pal->p_entry[start], p_entry, sizeof(color_t) * len);
  }
  else
  {
    return ERR_FAILURE;
  }

  region_set_palette(p_surf->p_osd_rgn, (u32 *)p_pal->p_entry, p_pal->cnt);

  return SUCCESS;
}


RET_CODE surface_get_palette_v2(void *p_surface, u16 start,
  u16 len, color_t *p_entry)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  palette_t *p_pal = p_surf->p_pal;

  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE && p_pal != NULL)
  {
    memcpy(p_entry, &p_pal->p_entry[start], sizeof(color_t) * len);
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static RET_CODE surface_set_colorkey_v2(void *p_surface, u32 ckey)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(ckey != SURFACE_INVALID_COLORKEY)
  {
    p_surf->colorkey = ckey;
    p_surf->attr |= SURFACE_ATTR_EN_COLORKEY;
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_COLORKEY);
  }
  
  return SUCCESS;
}


static RET_CODE surface_set_trans_clr_v2(void *p_surface, u32 trans_clr)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;

  if(trans_clr != SURFACE_INVALID_COLORKEY)
  {  
    if(p_surf->handle != -1)
    {
      disp_layer_color_key_onoff(p_surf->p_disp, p_surf->layer_id, TRUE);    
      disp_layer_set_color_key(p_surf->p_disp, p_surf->layer_id, trans_clr);
    }
  }
  else
  {
    if(p_surf->handle != -1)
    {
      disp_layer_color_key_onoff(p_surf->p_disp, p_surf->layer_id, FALSE);    
    }
  }
  return SUCCESS;
}

static RET_CODE surface_get_colorkey_v2(void *p_surface, u32 *p_ckey)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(p_surf->attr & SURFACE_ATTR_EN_COLORKEY)
  {
    *p_ckey = p_surf->colorkey;
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}

static RET_CODE surface_set_cliprect_v2(void *p_surface, rect_t *p_rc)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(p_rc != NULL)
  {
    p_surf->attr |= SURFACE_ATTR_EN_CLIP;
    copy_rect(&p_surf->rc_clip, p_rc);
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_CLIP);
  }

  return SUCCESS;
}


static RET_CODE surface_get_cliprect_v2(void *p_surface, rect_t *p_rc)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    copy_rect(p_rc, &p_surf->rc_clip);
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static RET_CODE surface_set_srcrect_v2(void *p_surface, rect_t *p_rc)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  point_t pos = {0};

  if(RECTWP(p_rc) != RECTW(p_surf->rc_src)
    || RECTHP(p_rc) != RECTH(p_surf->rc_src))
  {
    return ERR_FAILURE;
  }

  copy_rect(&p_surf->rc_src, p_rc);

  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    /* DO SOMETHING */
    region_show(p_surf->p_osd_rgn, FALSE);

    pos.x = p_surf->rc_src.left;
    pos.y = p_surf->rc_src.top;
    disp_layer_move_region(p_surf->p_disp, p_surf->p_osd_rgn, &pos);

    region_show(p_surf->p_osd_rgn, TRUE);
  }
  else
  {
    pos.x = p_surf->rc_src.left;
    pos.y = p_surf->rc_src.top;
    disp_layer_move_region(p_surf->p_disp, p_surf->p_osd_rgn, &pos);
  }

  return SUCCESS;
}


static RET_CODE surface_get_srcrect_v2(void *p_surface, rect_t *p_rc)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  copy_rect(p_rc, &p_surf->rc_src);
  return SUCCESS;
}


static RET_CODE surface_fill_rect_v2(void *p_surface, rect_t *p_rc, u32 value)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  rect_t erc = *p_rc;

  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc_v2(&erc, &p_surf->rc_clip))
    {
      return ERR_PARAM;
    }
  }

  gpe_draw_rectangle_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, value);

  return SUCCESS;
}

static RET_CODE surface_fill_bmp_v2(void *p_surface, rect_t *p_rc, bitmap_t *p_bmp)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  gpe_param_vsb_t param = {0};
  rect_t orc, erc = *p_rc;
  rect_t fill_rect;

  set_rect(&orc, p_rc->left, p_rc->top, p_rc->left + p_bmp->width,
           p_rc->top + p_bmp->height);

  /* with bitmap */
  if(!get_effect_rc_v2(&erc, &orc))
  {
    SURF_PRINTF("surface_fill_bmp(%d)\n", __LINE__);
    return ERR_PARAM;
  }

  /* with cliprect */
  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc_v2(&erc, &p_surf->rc_clip))
    {
      SURF_PRINTF("surface_fill_bmp(%d)\n", __LINE__);
      return ERR_PARAM;
    }
  }

  fill_rect.left = erc.left - p_rc->left;
  fill_rect.top = erc.top - p_rc->top;
  fill_rect.right = fill_rect.left + RECTW(erc);//RECTW(erc);//erc.right - p_rc->right;
  fill_rect.bottom = fill_rect.top + RECTH(erc);//RECTH(erc);//erc.bottom - p_rc->bottom;

  param.colorkey = p_bmp->colorkey;
  param.enable_colorkey = (u32)p_bmp->enable_ckey;
  param.is_font = p_bmp->is_font;

  if(p_bmp->p_alpha != NULL)
  {
    param.cmd = GPE_CMD_ALPHA_MAP;
    param.p_alpha = p_bmp->p_alpha;
    param.alpha_pitch = p_bmp->alpha_pitch;
  }

  //if dummy region exist, paint on the dummy region first, else paint on the osd region.
  gpe_draw_image_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, p_bmp->p_bits, 
    NULL, 0, p_bmp->pitch, 
    (p_bmp->pitch * p_bmp->height), p_surf->format, &param, &fill_rect);

  if(p_bmp->p_strok_alpha != NULL)
  {
    param.cmd = GPE_CMD_ALPHA_MAP;
    param.p_alpha = p_bmp->p_strok_alpha;
    param.alpha_pitch = p_bmp->alpha_pitch;

    gpe_draw_image_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, p_bmp->p_strok_char, 
      NULL, 0, p_bmp->pitch, 
      (p_bmp->pitch * p_bmp->height), p_surf->format, &param, &fill_rect);
  }

  return SUCCESS;
}


static RET_CODE surface_get_bits_v2(void *p_surface,
  rect_t *p_rc, void *p_buf, u32 pitch)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  u32 entries_num = 0;

  if(!is_rect_covered(p_rc, &p_surf->rc_src))
  {
    SURF_PRINTF("surface_get_bits: out of range(%d, %d, %d, %d)\n",
              p_rc->left, p_rc->top, p_rc->right, p_rc->bottom);
    return ERR_PARAM;
  }

  
  gpe_dump_image_vsb(p_surf->p_gpe_dev, 
    p_surf->p_osd_rgn, p_rc, p_buf, p_surf->p_pal->p_entry, &entries_num, p_surf->format);

  return SUCCESS;
}


static RET_CODE surface_bitblt_v2(void *p_src_surface,
  u16 sx, u16 sy, u16 sw, u16 sh,
  void *p_dst_surface, u16 dx, u16 dy, surface_rop_type_t rop)
{
  surface_v2_t *p_src_surf = (surface_v2_t *)p_src_surface;
  surface_v2_t *p_dst_surf = (surface_v2_t *)p_dst_surface;
  gpe_param_vsb_t param;
  rect_t src_rc, dst_rc;

  memset(&param, 0, sizeof(gpe_param_vsb_t));
  param.cmd = GPE_CMD_RASTER_OP;
  
  switch(rop)
  {
    case SURFACE_ROP_AND:
      param.rop3 = SRCAND;
      break;
      
    case SURFACE_ROP_OR:
      param.rop3 = SRCPAINT;
      break;      

    case SURFACE_ROP_XOR:
      param.rop3 = SRCINVERT;
      break;      

    default:
      param.rop3 = SRCCOPY;
      break;
  }
  
  if(p_dst_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    param.cmd |= GPE_CMD_CLIP_RECT;
    //param.p_cliprc = &p_dst_surf->rc_clip;
  }

  if(p_src_surf->attr & SURFACE_ATTR_EN_COLORKEY)
  {
    param.cmd |= GPE_CMD_COLORKEY;
    param.colorkey = p_src_surf->colorkey;
  }

  src_rc.left = sx;
  src_rc.top = sy;
  src_rc.right = (sw + sx);
  src_rc.bottom = (sh + sy);
  
  if(p_dst_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    dst_rc.left = dx;
    dst_rc.top = dy;  
    dst_rc.right = (sw + dx);
    dst_rc.bottom = (sh + dy);
    if(!get_effect_rc_v2(&dst_rc, &p_dst_surf->rc_clip))
    {
      SURF_PRINTF("surface_fill_bmp(%d)\n", __LINE__);
      return ERR_PARAM;
    }

	src_rc.left += (dst_rc.left - dx);
	src_rc.top += (dst_rc.top - dy);
	src_rc.right = src_rc.left + RECTW(dst_rc);
	src_rc.bottom  = src_rc.top + RECTH(dst_rc);		
  }
  else
  {
    dst_rc.left = dx;
    dst_rc.top = dy;
    dst_rc.right = (sw + dx);
    dst_rc.bottom = (sh + dy);
  }


  return gpe_bitblt_vsb(p_dst_surf->p_gpe_dev, p_dst_surf->p_osd_rgn,
    &dst_rc, NULL, NULL, p_src_surf->p_osd_rgn, &src_rc, &param);  
}

static u8 surface_get_attr_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  return p_surf->attr;
}

static s32 surface_get_handle_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  return (u32)p_surf->p_osd_rgn;
}

static u8 surface_get_bpp_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  return p_surf->bpp;
}

static u8 surface_get_format_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  u8 format = 0;

  switch(p_surf->format)
  {
    case PIX_FMT_RGBPALETTE4:
      format = COLORFORMAT_RGB4BIT;
      break;
    case PIX_FMT_RGBPALETTE8:
      format = COLORFORMAT_RGB8BIT;
      break;
    case PIX_FMT_RGB565:
      format = COLORFORMAT_RGB565;
      break;
    case PIX_FMT_RGBA5551:
      format = COLORFORMAT_RGBA5551;
      break;
    case PIX_FMT_ARGB1555:
      format = COLORFORMAT_ARGB1555;
      break;
    case PIX_FMT_RGBA8888:
      format = COLORFORMAT_RGBA8888;
      break;
    case PIX_FMT_ARGB8888:
      format = COLORFORMAT_ARGB8888;
      break;      
    default:
      MT_ASSERT(0);
      break;
  }
  return format;
}

static u32 surface_get_palette_addr_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
  
  return (u32)p_surf->p_pal;
}

static void surface_start_batch_v2(void *p_surface)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;

  gpe_start_batch_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn);
  
}

static void surface_end_batch_v2(void *p_surface, BOOL is_sync, rect_t *p_rect)
{
  surface_v2_t *p_surf = (surface_v2_t *)p_surface;
#ifdef WARRIORS

  rect_t rect = {0};

  rect.left = p_rect->left;
  rect.top = p_rect->top;
  rect.right = p_rect->right;
  rect.bottom = p_rect->bottom;
  
  //OS_PRINTF("\nsurface_end_batch_v2 111: p_rect(%d, %d, %d, %d)\n", 
  //p_rect->left, p_rect->top, p_rect->right, p_rect->bottom);

  if(rect.left < 4)
  {
    rect.left = 0;
  }
  else
  {
    rect.left = (rect.left + 3) / 4 * 4 - 4;
  }

  if(rect.right < 4)
  {
    rect.right = 0;
  }
  else
  {
    rect.right = (rect.right + 3) / 4 * 4;
  }
/*
  // because gpe warriors has a scale bug. the top need is 0
  if(rect.top < 4)
  {
    rect.top = 0;
  }
  else
  {
    rect.top = (rect.top + 3) / 4 * 4 - 4;
  }
*/

  rect.top = 0;
  //rect.bottom = (rect.bottom + 3) / 4 * 4;
  rect.bottom = ((region_t *)(p_surf->p_osd_rgn))->rect.h;
  
  disp_layer_update_region(p_surf->p_disp, p_surf->p_osd_rgn, &rect);

  //OS_PRINTF("surface_end_batch_v2 222: rect(%d, %d, %d, %d)\n", 
  //rect.left, rect.top, rect.right, rect.bottom);
  
  //disp_layer_update_region(p_surf->p_disp, p_surf->p_osd_rgn, NULL);
#else
  gpe_end_batch_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, is_sync);
#endif
}

RET_CODE surface_init_v2(u32 *p_odd_addr,
                      u32 odd_size,
                      u32 *p_even_addr,
                      u32 even_size,
                      u8 cnt)
{
  u32 slice = sizeof(surface_v2_t);
  surface_proc_t *p_info = NULL;
  surface_priv_v2_t *p_priv = NULL;
  
  // alloc global infor
  p_info = (surface_proc_t *)mtos_malloc(sizeof(surface_proc_t));
  MT_ASSERT(p_info != NULL);
  memset(p_info, 0, sizeof(surface_proc_t));

  //Malloc private data for data manager32
  p_info->p_data = mtos_malloc(sizeof(surface_priv_v2_t));
  MT_ASSERT(p_info->p_data != NULL);
  memset(p_info->p_data, 0, sizeof(surface_priv_v2_t));

  p_priv = (surface_priv_v2_t *)(p_info->p_data);

  class_register(SURFACE_CLASS_ID, (class_handle_t)(p_info));

  // alloc surface
  slice = ((slice + 3) / 4) * 4;
  p_priv->p_heap_addr = mtos_malloc(cnt * slice);
  MT_ASSERT(p_priv->p_heap_addr != NULL);

  if(lib_memf_create(&p_priv->surf_heap, (u32)p_priv->p_heap_addr,
                     cnt * slice, slice) != SUCCESS)
  {
    MT_ASSERT(0);
  }

  //initial interface.
  p_info->surface_bitblt = surface_bitblt_v2;
  p_info->surface_create = surface_create_v2;
  p_info->surface_delete = surface_delete_v2;
  p_info->surface_enable = surface_enable_v2;
  p_info->surface_fill_bmp = surface_fill_bmp_v2;
  p_info->surface_fill_rect = surface_fill_rect_v2;
  p_info->surface_fill_round_rect = NULL;
  p_info->surface_get_bits = surface_get_bits_v2;
  p_info->surface_get_cliprect = surface_get_cliprect_v2;
  p_info->surface_get_colorkey = surface_get_colorkey_v2;
  p_info->surface_get_palette = surface_get_palette_v2;
  p_info->surface_get_srcrect = surface_get_srcrect_v2;
  p_info->surface_init_palette = surface_init_palette_v2;
  p_info->surface_release = surface_release_v2;
  p_info->surface_set_alpha = surface_set_alpha_v2;
  p_info->surface_set_cliprect = surface_set_cliprect_v2;
  p_info->surface_set_colorkey = surface_set_colorkey_v2;
  p_info->surface_set_display = surface_set_display_v2;
  p_info->surface_set_palette = surface_set_palette_v2;
  p_info->surface_set_srcrect = surface_set_srcrect_v2;
  p_info->surface_set_trans = surface_set_trans_v2;
  p_info->surface_get_attr = surface_get_attr_v2;
  p_info->surface_get_handle = surface_get_handle_v2;
  p_info->surface_get_bpp = surface_get_bpp_v2;
  p_info->surface_get_format = surface_get_format_v2;
  p_info->surface_get_palette_addr = surface_get_palette_addr_v2;
  p_info->surface_start_batch = surface_start_batch_v2;
  p_info->surface_end_batch = surface_end_batch_v2;
  p_info->surface_set_trans_clr = surface_set_trans_clr_v2;
  
  return SUCCESS;
}


