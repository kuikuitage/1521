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

#include "osd.h"
#include "gpe.h"
#include "surface.h"
#include "surface_priv.h"
#include "gui_resource.h"
#include "surface_v1.h"


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
  /*!
     The osd device handle.
    */
  osd_device_t *p_osd_dev;
}surface_priv_v1_t;

/*!
   The structure is defined to descript a surface.
  */
typedef struct
{
  /*!
     The pixel format
    */
  u8 format;
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
  u8 *p_vmem[2];
  /*!
     Points to a GPE object.
    */
  gpe_device_t *p_gpe_dev;
} surface_v1_t;

#ifdef SURF_DEBUG

static void surface_dump_info_v1(surface_v1_t *p_info)
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
  SURF_PRINTF("\t vmem      (0x%.8x, 0x%.8x)\n", (u32)p_info->p_vmem[0],
              (u32)p_info->p_vmem[1]);
}


#endif

static surface_v1_t *surface_alloc_v1(void)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  return (surface_v1_t *)lib_memf_alloc(&p_priv->surf_heap);
}


static void surface_free_v1(void *p_addr)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  MT_ASSERT(p_addr != NULL);

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  lib_memf_free(&p_priv->surf_heap, p_addr);
}


static u32 calc_pitch_v1(u16 bpp, u16 width)
{
  /* 4byte alignment */
//  return (u32)((((width * bpp + 7) >> 3) + 3) >> 2) << 2;
/* 1byte alignment */
  return (u32)((width * bpp + 7) >> 3);
}


#if 0
static u32 calc_size(u16 bpp, u16 width, u16 height)
{
  return (u32)(calc_pitch(bpp, width) * height);
}


#endif

static void init_gpe_desc_v1(gpe_desc_t *p_desc, surface_v1_t *p_surf)
{
  memset(p_desc, 0, sizeof(gpe_desc_t));
  p_desc->bpp = p_surf->bpp;
  p_desc->width = p_surf->width;
  p_desc->height = p_surf->height;
  p_desc->pitch = p_surf->pitch;
  p_desc->p_vmem[0] = p_surf->p_vmem[0];
  p_desc->p_vmem[1] = p_surf->p_vmem[1];
}


static BOOL get_effect_rc_v1(rect_t *p_erc, rect_t *p_crc)
{
  return intersect_rect(p_erc, p_erc, p_crc);
}

static void surface_release_v1(void)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  // free surface
  lib_memf_destroy(&p_priv->surf_heap);
  mtos_free(p_priv->p_heap_addr);

  //free global infor
  mtos_free(p_priv);

  //free proc info
  mtos_free(p_info);
}


static void surface_enable_v1(void *p_surface, BOOL is_enable)
{
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  dev_io_ctrl(p_priv->p_osd_dev, OSD_CMD_SET_DISPLAY, is_enable);
  //osd_set_display(is_enable);
}


static RET_CODE surface_create_v1(void **pp_surf, surface_desc_t *p_desc)
{
  osd_region_desc_t region_desc;
  u16 width = 0, height = 0;
  surface_v1_t *p_surf = NULL;
  u32 *p_entry = NULL;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  SURF_PRINTF("SURF ALLOC \n");
  if((p_surf = surface_alloc_v1()) == NULL)
  {
    return ERR_NO_MEM;
  }
  SURF_PRINTF("SURF ALLOC OK \n");

  memset(p_surf, 0, sizeof(surface_v1_t));

  if(p_desc->flag & SURFACE_DESC_FORMAT)
  {
    switch(p_desc->format)
    {
      case COLORFORMAT_RGB4BIT:
        p_surf->format = OSD_COLORFORMAT_RGB4BIT;
        break;
      case COLORFORMAT_RGB8BIT:
        p_surf->format = OSD_COLORFORMAT_RGB8BIT;
        break;
      case COLORFORMAT_RGB565:  
        p_surf->format = OSD_COLORFORMAT_RGB565;
        break;
      case COLORFORMAT_RGBA8888:  
        p_surf->format = OSD_COLORFORMAT_ARGB8888;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }
  else
  {
    p_surf->format = OSD_COLORFORMAT_RGB8BIT;
  }
  
  switch(p_surf->format)
  {
    /* no color format for 1 bit
        p_surf->bpp = 1;
        p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
        p_surf->p_pal = p_desc->p_pal;
      */
    case OSD_COLORFORMAT_RGB2BIT:
      p_surf->bpp = 2;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;
      break;
    case OSD_COLORFORMAT_RGB4BIT:
      p_surf->bpp = 4;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;
      break;
    case OSD_COLORFORMAT_RGB8BIT:
      p_surf->bpp = 8;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;
      break;
    case OSD_COLORFORMAT_ARGB1555:
      p_surf->bpp = 16;
      break;
    case OSD_COLORFORMAT_ARGB4444:
      p_surf->bpp = 16;
      break;
    case OSD_COLORFORMAT_ARGB8888:
      p_surf->bpp = 32;
      break;
    default:
      SURF_PRINTF("surface_create: UNSUPPORT format.\n");
      return ERR_NOFEATURE;
  }

  /* link related gpe device */
  p_surf->p_gpe_dev = (gpe_device_t *)dev_find_identifier(NULL,
                                                          DEV_IDT_TYPE,
                                                          SYS_DEV_TYPE_GPE);
  MT_ASSERT(NULL != p_surf->p_gpe_dev);
  // TODO: ... multi-device support ?
  dev_open(p_surf->p_gpe_dev, NULL);

  /* get current width&height */
  p_surf->width = p_desc->flag & SURFACE_DESC_WIDTH ? p_desc->width : width;
  p_surf->height = p_desc->flag & SURFACE_DESC_HEIGHT ? p_desc->height : height;
  p_surf->pitch = p_desc->flag & SURFACE_DESC_PITCH ? \
                  p_desc->pitch : calc_pitch_v1(p_surf->bpp, p_surf->width);

  if(p_desc->flag & SURFACE_DESC_CAPS)
  {
    switch(p_desc->caps)
    {
      case SURFACE_CAPS_MEM_ASSIGN:
        p_surf->p_vmem[0] = p_desc->p_vmem[0];
        p_surf->p_vmem[1] = p_desc->p_vmem[1];
        p_surf->handle = -1;
        break;
      case SURFACE_CAPS_MEM_SYSTEM:
        /* create osd region */
        memset(&region_desc, 0, sizeof(osd_region_desc_t));
        region_desc.width = p_surf->width;
        region_desc.height = p_surf->height;
        region_desc.bit_count = p_surf->bpp;
        region_desc.mode = (osd_color_mode_t)p_surf->format;
        region_desc.is_frame = FALSE;

        if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
        {
          MT_ASSERT(p_surf->p_pal != NULL);
          p_entry = (u32 *)p_surf->p_pal->p_entry;
        }
        SURF_PRINTF("create osd region...\n");
        SURF_PRINTF("\t width = %d\n", region_desc.width);
        SURF_PRINTF("\t height = %d\n", region_desc.height);
        SURF_PRINTF("\t mode = %d\n", region_desc.mode);
        SURF_PRINTF("\t bpp = %d\n", region_desc.bit_count);
        osd_create_region(p_priv->p_osd_dev,
                          &region_desc,
                          p_entry,
                          (u8 *)&p_surf->handle);
        MT_ASSERT(p_surf->handle != INVALID);
        osd_get_region_buffer(p_priv->p_osd_dev,
                              p_surf->handle,
                              (u32 *)&p_surf->p_vmem[0],
                              (u32 *)&p_surf->p_vmem[1]);

        osd_set_region_alphamode(p_priv->p_osd_dev, p_surf->handle,
                                 TRUE, FALSE);

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
  surface_dump_info(p_surf);
#endif

  return SUCCESS;
}


static RET_CODE surface_delete_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    if(p_surf->handle != -1)
    {
      osd_set_region_display(p_priv->p_osd_dev, p_surf->handle, FALSE);
      p_surf->attr &= (~SURFACE_ATTR_EN_DISPLAY);
    }
  }

  if(p_surf->handle != -1)
  {
    /* delete the last region */
    osd_delete_region(p_priv->p_osd_dev);
  }

  if(p_surf->p_gpe_dev != NULL)
  {
    dev_close(p_surf->p_gpe_dev);
  }

  /* release */
  surface_free_v1(p_surf);
  return SUCCESS;
}


static RET_CODE surface_set_display_v1(void *p_surface, BOOL is_display)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  if((p_surf->attr & SURFACE_ATTR_EN_DISPLAY) == is_display)
  {
    return SUCCESS;
  }

  if(p_surf->handle == -1)
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

  osd_set_region_display(p_priv->p_osd_dev, p_surf->handle, is_display);

  return SUCCESS;
}


static RET_CODE surface_set_alpha_v1(void *p_surface, u8 alpha)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  p_surf->alpha = alpha;

//  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY && p_surf->handle != -1)
//  {
    /* DO SOMETHING */
    osd_set_region_alphamode(p_priv->p_osd_dev, p_surf->handle, TRUE, FALSE);
    //osd_set_mix_weight(alpha);
    osd_set_plane_alpha(p_priv->p_osd_dev, alpha);
//    osd_set_region_alpha(p_surf->handle, p_surf->alpha);
//  }

  return SUCCESS;
}


static RET_CODE surface_set_trans_v1(void *p_surface, u32 index, u8 alpha)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  osd_set_region_trans(p_priv->p_osd_dev, p_surf->handle, index, alpha);
  return SUCCESS;
}


static RET_CODE surface_init_palette_v1(void *p_surface, palette_t *p_pal)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
  {
    p_surf->p_pal = p_pal;
  }
  else
  {
    return ERR_FAILURE;
  }

//  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
//  {
    /* DO SOMETHING */
    osd_set_region_palette(p_priv->p_osd_dev,
                           p_surf->handle,
                           (u32 *)p_pal->p_entry);
//  }

  return SUCCESS;
}


static RET_CODE surface_set_palette_v1(void *p_surface,
                             u16 start,
                             u16 len,
                             color_t *p_entry)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  palette_t *p_pal = p_surf->p_pal;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE && p_pal != NULL)
  {
    memcpy(&p_pal->p_entry[start], p_entry, sizeof(color_t) * len);
  }
  else
  {
    return ERR_FAILURE;
  }

//  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
//  {
    /* DO SOMETHING */
    osd_set_region_palette(p_priv->p_osd_dev,
                           p_surf->handle,
                           (u32 *)p_pal->p_entry);
//  }

  return SUCCESS;
}


static RET_CODE surface_get_palette_v1(void *p_surface,
  u16 start, u16 len, color_t *p_entry)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
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


static RET_CODE surface_set_colorkey_v1(void *p_surface, u32 ckey)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  if(ckey != SURFACE_INVALID_COLORKEY)
  {
    p_surf->colorkey = ckey;
    p_surf->attr |= SURFACE_ATTR_EN_COLORKEY;
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_COLORKEY);
  }

//  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
//  {
    /* DO SOMETHING */
//  }

  return SUCCESS;
}


static RET_CODE surface_get_colorkey_v1(void *p_surface, u32 *p_ckey)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
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


static RET_CODE surface_set_cliprect_v1(void *p_surface, rect_t *p_rc)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
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


static RET_CODE surface_get_cliprect_v1(void *p_surface, rect_t *p_rc)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
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


static RET_CODE surface_set_srcrect_v1(void *p_surface, rect_t *p_rc)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;

  p_info = (surface_proc_t *)class_get_handle_by_id(SURFACE_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_priv = (surface_priv_v1_t *)(p_info->p_data);  

  if(RECTWP(p_rc) != RECTW(p_surf->rc_src)
    || RECTHP(p_rc) != RECTH(p_surf->rc_src))
  {
    return ERR_FAILURE;
  }

  copy_rect(&p_surf->rc_src, p_rc);

  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    /* DO SOMETHING */
    osd_set_region_display(p_priv->p_osd_dev, p_surf->handle, FALSE);
    osd_move_region(p_priv->p_osd_dev, p_surf->handle,
                    p_surf->rc_src.left, p_surf->rc_src.top);
    osd_set_region_display(p_priv->p_osd_dev, p_surf->handle, TRUE);
  }
  else
  {
    osd_move_region(p_priv->p_osd_dev, p_surf->handle,
                    p_surf->rc_src.left, p_surf->rc_src.top);
  }

  /* reset addr */
  osd_get_region_buffer(p_priv->p_osd_dev, p_surf->handle,
                        (u32 *)&p_surf->p_vmem[0], (u32 *)&p_surf->p_vmem[1]);

  return SUCCESS;
}


static RET_CODE surface_get_srcrect_v1(void *p_surface, rect_t *p_rc)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  copy_rect(p_rc, &p_surf->rc_src);
  return SUCCESS;
}


static RET_CODE surface_fill_rect_v1(void *p_surface, rect_t *p_rc, u32 value)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  u16 x = 0, y = 0, w = 0;
  rect_t erc = *p_rc;
  gpe_desc_t p_desc;

  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc_v1(&erc, &p_surf->rc_clip))
    {
      return ERR_PARAM;
    }
  }
  /* init p_desc */
  init_gpe_desc_v1(&p_desc, p_surf);
  w = RECTW(erc);
  x = erc.left;
  for(y = erc.top; y < erc.bottom; y++)
  {
    gpe_draw_h_line(p_surf->p_gpe_dev, &p_desc, x, y, w, value);
  }
  return SUCCESS;
}


/*
   static const u8 bevel_info[] =
  #if 1
   {
   5, 3, 2, 1, 1
   };
  #else
   {
   8, 5, 3, 2, 2, 1, 1, 1
   };
  #endif
  #define BEVEL_RANGE    (sizeof(bevel_info) / sizeof(u8))

   RET_CODE surface_fill_round_rect(surface_v1_t *p_surf, rect_t *p_rc, u32 value)
   {
   u16 x, y, w, h;
   u16 r_x, r_w, gap;
   rect_t erc = *p_rc;
   gpe_desc_t p_desc;

   if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
   {
    if(!get_effect_rc(&erc, &p_surf->rc_clip))
    {
      return ERR_PARAM;
    }
   }

   // init p_desc
   init_gpe_desc(&p_desc, p_surf);

   w = RECTW(erc), h = RECTH(erc);
   x = erc.left;

   for(y = erc.top; y < erc.bottom; y++)
   {
    if((gap = y - erc.top) < BEVEL_RANGE)
    {
      r_x = x + bevel_info[gap];
      r_w = w - (bevel_info[gap] << 1);
    }
    else if((gap = erc.bottom - y) < BEVEL_RANGE)
    {
      r_x = x + bevel_info[gap];
      r_w = w - (bevel_info[gap] << 1);
    }
    else
    {
      r_x = x, r_w = w;
    }

    gpe_draw_h_line(p_surf->p_gpe_dev, &p_desc, r_x, y, r_w, value);
   }

   return SUCCESS;
   }
  */

static RET_CODE surface_fill_bmp_v1(void *p_surface, rect_t *p_rc, bitmap_t *p_bmp)
{
  surface_v1_t * p_surf = (surface_v1_t *)p_surface;
  rect_t orc, erc = *p_rc;
  point_t pt;
  gpe_desc_t p_desc;

  set_rect(&orc, p_rc->left, p_rc->top, p_rc->left + p_bmp->width,
           p_rc->top + p_bmp->height);

  /* with bitmap */
  if(!get_effect_rc_v1(&erc, &orc))
  {
    SURF_PRINTF("surface_fill_bmp(%d)\n", __LINE__);
    return ERR_PARAM;
  }

  /* with cliprect */
  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc_v1(&erc, &p_surf->rc_clip))
    {
      SURF_PRINTF("surface_fill_bmp(%d)\n", __LINE__);
      return ERR_PARAM;
    }
  }

  /* init p_desc */
  init_gpe_desc_v1(&p_desc, p_surf);

  pt.x = erc.left - p_rc->left;
  pt.y = erc.top - p_rc->top;

  if(p_bmp->enable_ckey)
  {
    gpe_put_image_mask(p_surf->p_gpe_dev,
                       &p_desc,
                       &erc,
                       &pt,
                       p_bmp->p_bits,
                       p_bmp->pitch,
                       p_bmp->colorkey);
  }
  else
  {
    gpe_put_image(p_surf->p_gpe_dev,
                  &p_desc,
                  &erc,
                  &pt,
                  p_bmp->p_bits,
                  p_bmp->pitch);
  }

  return SUCCESS;
}


static RET_CODE surface_get_bits_v1(void *p_surface,
                          rect_t *p_rc,
                          void *p_buf,
                          u32 pitch)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  point_t pt;
  gpe_desc_t p_desc;

  if(!is_rect_covered(p_rc, &p_surf->rc_src))
  {
    SURF_PRINTF("surface_get_bits: out of range(%d, %d, %d, %d)\n",
              p_rc->left, p_rc->top, p_rc->right, p_rc->bottom);
    return ERR_PARAM;
  }

  /* init p_desc */
  init_gpe_desc_v1(&p_desc, p_surf);
  pt.x = pt.y = 0;
  gpe_get_image(p_surf->p_gpe_dev, &p_desc, p_rc, &pt, p_buf, pitch);

  return SUCCESS;
}


static RET_CODE surface_bitblt_v1(void *p_src_surface,
                        u16 sx,
                        u16 sy,
                        u16 sw,
                        u16 sh,
                        void *p_dst_surface,
                        u16 dx,
                        u16 dy,
                        surface_rop_type_t rop)
{
  surface_v1_t *p_src_surf = (surface_v1_t *)p_src_surface;
  surface_v1_t *p_dst_surf = (surface_v1_t *)p_dst_surface;
  rect_t src_rc, dst_rc;
  gpe_desc_t src_desc, dst_desc;
  gpe_param_t param;

  memset(&param, 0, sizeof(gpe_param_t));
  param.cmd = GPE_CMD_DIRECT_FILL;

  if(p_dst_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    param.cmd |= GPE_CMD_CLIP_RECT;
    param.p_cliprc = &p_dst_surf->rc_clip;
  }

  if(p_src_surf->attr & SURFACE_ATTR_EN_COLORKEY)
  {
    param.cmd |= GPE_CMD_COLORKEY;
    param.colorkey = p_src_surf->colorkey;
  }

  set_rect(&src_rc, sx, sy, sx + sw, sy + sh);
  set_rect(&dst_rc, dx, dy, dx + sw, dy + sh);

  /* init p_desc */
  init_gpe_desc_v1(&src_desc, p_src_surf);
  init_gpe_desc_v1(&dst_desc, p_dst_surf);

  return gpe_bitblt(p_dst_surf->p_gpe_dev,
                    &dst_desc,
                    &dst_rc,
                    &src_desc,
                    &src_rc,
                    &param);
}

static u8 surface_get_attr_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  return p_surf->attr;
}

static s32 surface_get_handle_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  return p_surf->handle;
}

static u8 surface_get_bpp_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  return p_surf->bpp;
}

static u8 surface_get_format_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  u8 format = 0;

  switch(p_surf->format)
  {
    case OSD_COLORFORMAT_RGB4BIT:
      format = COLORFORMAT_RGB4BIT;
      break;
    case OSD_COLORFORMAT_RGB8BIT:
      format = COLORFORMAT_RGB8BIT;
      break;
    case OSD_COLORFORMAT_RGB565:
      format = COLORFORMAT_RGB565;
      break;
    case OSD_COLORFORMAT_ARGB8888:
      format = COLORFORMAT_ARGB8888;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return format;
}

static u32 surface_get_palette_addr_v1(void *p_surface)
{
  surface_v1_t *p_surf = (surface_v1_t *)p_surface;
  
  return (u32)p_surf->p_pal;
}

RET_CODE surface_init_v1(u32 *p_odd_addr,
                      u32 odd_size,
                      u32 *p_even_addr,
                      u32 even_size,
                      u8 cnt)
{
  u32 slice = sizeof(surface_v1_t);
  osd_cfg_t cfg;
  surface_proc_t *p_info = NULL;
  surface_priv_v1_t *p_priv = NULL;
  
  cfg.even_addr = (u32)p_even_addr;
  cfg.odd_addr = (u32)p_odd_addr;
  cfg.even_size = even_size;
  cfg.odd_size = odd_size;

  // alloc global infor
  p_info = (surface_proc_t *)mtos_malloc(sizeof(surface_proc_t));
  MT_ASSERT(p_info != NULL);
  memset(p_info, 0, sizeof(surface_proc_t));
  
  //Malloc private data for data manager32
  p_info->p_data = mtos_malloc(sizeof(surface_priv_v1_t));
  MT_ASSERT(p_info->p_data != NULL);
  memset(p_info->p_data, 0, sizeof(surface_priv_v1_t));

  p_priv = (surface_priv_v1_t *)(p_info->p_data);

  class_register(SURFACE_CLASS_ID, (class_handle_t)(p_info));

  /* init osd region buffer */
  p_priv->p_osd_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_OSD);
  MT_ASSERT(p_priv->p_osd_dev != NULL);
  SURF_PRINTF("\r\nsurface_init p_odd_addr=0x%x, p_even_addr=0x%x",
            (u32)p_odd_addr, (u32)p_even_addr);
  dev_open(p_priv->p_osd_dev, &cfg);

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
  p_info->surface_bitblt = surface_bitblt_v1;
  p_info->surface_create = surface_create_v1;
  p_info->surface_delete = surface_delete_v1;
  p_info->surface_enable = surface_enable_v1;
  p_info->surface_fill_bmp = surface_fill_bmp_v1;
  p_info->surface_fill_rect = surface_fill_rect_v1;
  p_info->surface_fill_round_rect = NULL;
  p_info->surface_get_bits = surface_get_bits_v1;
  p_info->surface_get_cliprect = surface_get_cliprect_v1;
  p_info->surface_get_colorkey = surface_get_colorkey_v1;
  p_info->surface_get_palette = surface_get_palette_v1;
  p_info->surface_get_srcrect = surface_get_srcrect_v1;
  p_info->surface_init_palette = surface_init_palette_v1;
  p_info->surface_release = surface_release_v1;
  p_info->surface_set_alpha = surface_set_alpha_v1;
  p_info->surface_set_cliprect = surface_set_cliprect_v1;
  p_info->surface_set_colorkey = surface_set_colorkey_v1;
  p_info->surface_set_display = surface_set_display_v1;
  p_info->surface_set_palette = surface_set_palette_v1;
  p_info->surface_set_srcrect = surface_set_srcrect_v1;
  p_info->surface_set_trans = surface_set_trans_v1;
  p_info->surface_get_attr = surface_get_attr_v1;
  p_info->surface_get_handle = surface_get_handle_v1;
  p_info->surface_get_bpp = surface_get_bpp_v1;
  p_info->surface_get_format = surface_get_format_v1;
  p_info->surface_get_palette_addr = surface_get_palette_addr_v1;
  
  return SUCCESS;
}


