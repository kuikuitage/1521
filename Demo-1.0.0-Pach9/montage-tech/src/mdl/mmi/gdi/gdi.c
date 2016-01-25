/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

#include "osd.h"
#include "gpe.h"
#include "surface_v1.h"
#include "surface_v2.h"

#include "mdl.h"
#include "mmi.h"
#include "surface.h"
#include "gdi.h"
#include "gdi_dc.h"

#include "gui_resource.h"
#include "gdi_private.h"
//u8 lock_gdi;  // mutex ensuring exclusive access to gdi.

//-dc_t screen_dc;
//-surface_t *p_screen_surface = NULL;

//-static dc_t *p_dc_slot = NULL;
//-static u16 dc_slot_cnt;
//static u8 lock_dc_slot;

//-static lib_memf_t cliprc_heap;
//-static void *p_cliprc_heap_addr = NULL;

//-static u8 screen_format;
//-static rect_t screen_rc;

//-static u32 screen_cdef, screen_ckey;

//-static palette_t global_pal;

#ifdef GDI_DEBUG
//-static u32 dc_cnt;
//-static u32 dc_max_cnt;
#endif


/*
   !static functions
  */
#ifdef GDI_DEBUG
void gdi_dc_usage(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  GDI_PRINTF("----------------------------\n");
  GDI_PRINTF("GDI dc max = %d\n", p_gdi_info->dc_max_cnt);
  GDI_PRINTF("----------------------------\n");
}


#endif

int dc_enter_drawing(dc_t *p_dc)
{
  if(!intersect_rect(&p_dc->rc_output, &p_dc->rc_output, &p_dc->ecrgn.bound))
  {
    return -1;
  }

  if(dc_is_virtual_dc(p_dc))
  {
    if(!is_rect_intersected(&p_dc->vtrl_rc, &p_dc->rc_output))
    {
      return -2;
    }
  }

//	LOCK (lock_gdi);

  return 0;
}


void dc_leave_drawing(dc_t *p_dc)
{
//	UNLOCK (lock_gdi);
}


static BOOL init_screen_surface(void)
{
  surface_desc_t desc;
  rect_t orc;
  s32 ret = SUCCESS;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  u8 attr = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  // set the surface descriptor
  desc.format = p_gdi_info->screen_format;

  desc.flag = SURFACE_DESC_CAPS | SURFACE_DESC_FORMAT | SURFACE_DESC_WIDTH |
              SURFACE_DESC_HEIGHT;
  desc.width = RECTW(p_gdi_info->screen_rc);
  desc.height = RECTH(p_gdi_info->screen_rc);
  desc.caps = SURFACE_CAPS_MEM_SYSTEM;
  desc.p_vmem[0] = desc.p_vmem[1] = NULL;
//	desc.pitch = pitch;
  desc.p_pal = &p_gdi_info->global_pal;

  ret = surface_create(handle, &p_gdi_info->p_screen_surface, &desc);
  MT_ASSERT(ret == SUCCESS);

  attr = surface_get_attr(handle, p_gdi_info->p_screen_surface);
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    ret = surface_set_palette(handle, p_gdi_info->p_screen_surface,
                              0,
                              p_gdi_info->global_pal.cnt,
                              p_gdi_info->global_pal.p_entry);
  }
  else
  {
    ret = SUCCESS;
  }
  MT_ASSERT(ret == SUCCESS);
  surface_set_srcrect(handle, p_gdi_info->p_screen_surface, &p_gdi_info->screen_rc);
  // set ckey for bitblt
  ret = surface_set_colorkey(handle, 
    p_gdi_info->p_screen_surface, p_gdi_info->screen_ckey);
  MT_ASSERT(ret == SUCCESS);

  ret = surface_set_trans_clr(handle, 
    p_gdi_info->p_screen_surface, p_gdi_info->screen_ckey);
  MT_ASSERT(ret == SUCCESS);
  
  // set ckey for overlay
  surface_set_trans(handle, 
    p_gdi_info->p_screen_surface, p_gdi_info->screen_cdef, 0);
  set_rect(&orc, 0, 0,
           RECTW(p_gdi_info->screen_rc), RECTH(p_gdi_info->screen_rc));
//	LOCK (lock_gdi);
  ret = surface_fill_rect(handle, 
    p_gdi_info->p_screen_surface, &orc, p_gdi_info->screen_cdef);
//	UNLOCK (lock_gdi);
  MT_ASSERT(ret == SUCCESS);
//	LOCK (lock_gdi);
  ret = surface_set_display(handle, p_gdi_info->p_screen_surface, TRUE);
//	UNLOCK (lock_gdi);
  MT_ASSERT(ret == SUCCESS);
  // enable osd display
  surface_enable(handle, p_gdi_info->p_screen_surface, TRUE);

  return TRUE;
}


// init screen DC
static void init_screen_dc(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  /* dc part */
  p_gdi_info->screen_dc.type = TYPE_SCREEN;

  p_gdi_info->screen_dc.p_curn_surface = p_gdi_info->p_screen_surface;
  p_gdi_info->screen_dc.p_back_surface = NULL;

  // init local clippping region
  gdi_init_cliprgn(&p_gdi_info->screen_dc.lcrgn, &p_gdi_info->cliprc_heap);
  // init effective clippping region
  gdi_init_cliprgn(&p_gdi_info->screen_dc.ecrgn, &p_gdi_info->cliprc_heap);

  // init global clip region information
  p_gdi_info->screen_dc.p_gcrgn_info = NULL;

  p_gdi_info->screen_dc.dev_rc.left = p_gdi_info->screen_dc.dev_rc.top = 0;
  p_gdi_info->screen_dc.dev_rc.right = RECTW(p_gdi_info->screen_rc);
  p_gdi_info->screen_dc.dev_rc.bottom = RECTH(p_gdi_info->screen_rc);

  // set effetive clippping region to the screen.
  gdi_set_cliprgn(&p_gdi_info->screen_dc.lcrgn, &p_gdi_info->screen_dc.dev_rc);
  gdi_set_cliprgn(&p_gdi_info->screen_dc.ecrgn, &p_gdi_info->screen_dc.dev_rc);
}


// this function init clip region in all DC slots.
static void dc_init_cliprgn(void)
{
  int i = 0;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    // Local clip region
    gdi_init_cliprgn(&p_gdi_info->p_dc_slot[i].lcrgn,
                     &p_gdi_info->cliprc_heap);

    // Global clip region info
    p_gdi_info->p_dc_slot[i].p_gcrgn_info = NULL;

    // Effective clip region
    gdi_init_cliprgn(&p_gdi_info->p_dc_slot[i].ecrgn,
                     &p_gdi_info->cliprc_heap);
  }
}


// This function init DC.
// set the default parameters.
static void dc_init(dc_t *p_dc, rect_t *p_rc, crgn_info_t *p_gcrgn_info)
{
  // assume that the local clip region is empty.
  // get global clip region info and generate effective clip region.
  if(dc_is_general_dc(p_dc))
  {
    p_dc->p_gcrgn_info = p_gcrgn_info;

    LOCK_GCRINFO(p_dc);

    // copy window's cliprgn as initailze dc's cliprgn
    if(p_dc->p_gcrgn_info != NULL)
    {
      gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
    }
    else
    {
      gdi_set_cliprgn(&p_dc->ecrgn, p_rc);
    }

    p_dc->dev_rc = *p_rc;

    // intersect rgn with rect
    gdi_intersect_cliprect(&p_dc->ecrgn, p_rc);

    UNLOCK_GCRINFO(p_dc);
  }
  else if(dc_is_mem_dc(p_dc))
  {
    p_dc->p_gcrgn_info = p_gcrgn_info;

    LOCK_GCRINFO(p_dc);

    gdi_set_cliprgn(&p_dc->ecrgn, p_rc);
    p_dc->dev_rc = *p_rc;

    UNLOCK_GCRINFO(p_dc);
  }
}


// this function generates effective clip region from
// local clip region and global clip region.
// if the global clip region has a new age,
// this function empty effective clip region first,
// and then intersect local clip region and global clip region.

BOOL dc_generate_ecrgn(dc_t *p_dc, BOOL is_is_force)
{
  cliprc_t *p_cr = NULL;

  // is global clip region is empty?
  if((!is_is_force) && (!dc_is_visible(p_dc)))
  {
    return FALSE;
  }

  // need regenerate?
  if(is_is_force)
  {
    /* copy local clipping region to effective clipping region. */
    gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->lcrgn);

    /* transfer device coordinates to screen coordinates. */
    p_cr = p_dc->ecrgn.p_head;
    while(NULL != p_cr)
    {
      coor_dp2sp(p_dc, &p_cr->rc.left, &p_cr->rc.top);
      coor_dp2sp(p_dc, &p_cr->rc.right, &p_cr->rc.bottom);

      p_cr = p_cr->p_next;
    }
    coor_dp2sp(p_dc, &p_dc->ecrgn.bound.left, &p_dc->ecrgn.bound.top);
    coor_dp2sp(p_dc, &p_dc->ecrgn.bound.right, &p_dc->ecrgn.bound.bottom);


    /* intersect with global clipping region. */
    if(p_dc->lcrgn.p_head == NULL)
    {
      if(p_dc->p_gcrgn_info != NULL)
      {
        gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
      }
      else
      {
        gdi_set_cliprgn(&p_dc->ecrgn, &p_dc->dev_rc);
      }
    }
    else
    {
      if(p_dc->p_gcrgn_info != NULL)
      {
        gdi_intersect_cliprgn(
          &p_dc->ecrgn, &p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
      }
    }

    /*
      * update p_dc->dev_rc, and restrict the effective
      * clipping region more with p_dc->dev_rc.
      */
    gdi_intersect_cliprect(&p_dc->ecrgn, &p_dc->dev_rc);
  }

  return TRUE;
}


void dc_reset_lcrgn(dc_t *p_dc)
{
  if(p_dc->lcrgn.p_head == NULL)
  {
    rect_t rc;
    set_rect(&rc, 0, 0, 0x7FFF, 0x7FFF);
    gdi_set_cliprgn(&p_dc->lcrgn, &rc);
  }
}


/*!
   end of static functions
  */

/*!
   general operations
  */

/*!
   this function init gdi module
  */
BOOL gdi_init(gdi_config_t *p_config) //, s16 pitch)
{
  u32 slice = 0;
  u32 size = 0;

  class_handle_t p_gdi_handle = NULL;
  gdi_main_t *p_gdi_info = NULL;

  //init global variable.
  p_gdi_handle = (void *)mmi_alloc_buf(sizeof(gdi_main_t));
  memset((void *)p_gdi_handle, 0, sizeof(gdi_main_t));
  class_register(GDI_CLASS_ID, p_gdi_handle);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  // init  cliprect heap
  slice = sizeof(cliprc_t);
  size = slice * p_config->max_cliprect_cnt;
  GDI_PRINTF("HEAP ADDR = 0x%.8x\n", &p_gdi_info->cliprc_heap);
  p_gdi_info->p_cliprc_heap_addr = mmi_create_memf(&p_gdi_info->cliprc_heap,
                                                   p_config->max_cliprect_cnt,
                                                   slice);
  MT_ASSERT(p_gdi_info->p_cliprc_heap_addr != NULL);
  GDI_PRINTF(" GDI: cliprc heap init done,  size = %d cnt = %d\n",
             size, p_config->max_cliprect_cnt);

  // init dc slot
  size = sizeof(dc_t) * p_config->max_dc_cnt;
  p_gdi_info->p_dc_slot = (dc_t *)mmi_alloc_buf(size);
  MT_ASSERT(p_gdi_info->p_dc_slot != NULL);
  memset(p_gdi_info->p_dc_slot, 0, size);
  p_gdi_info->dc_slot_cnt = p_config->max_dc_cnt;
  GDI_PRINTF(" GDI: dc slot init done,  size = %d cnt = %d\n",
             size, p_config->max_dc_cnt);

  if(surface_init_v1(p_config->p_odd_addr, p_config->odd_size,
                    p_config->p_even_addr, p_config->even_size,
                    p_config->max_surface_cnt) != SUCCESS)
  {
    MT_ASSERT(0);
  }                    

  GDI_PRINTF(" GDI: surface init done,  cnt = %d\n",
             p_config->max_surface_cnt);
  // set screen
  p_gdi_info->screen_format = p_config->format;
  copy_rect(&p_gdi_info->screen_rc, &p_config->osd_rect);
  if(p_config->p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_config->p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_config->p_pal->p_entry;
    //	memcpy (global_pal.palette_bits,
    //info->pal->palette_bits, sizeof(color_t)*global_pal.entry_num);
  }
  p_gdi_info->screen_cdef = p_config->cdef;
  p_gdi_info->screen_ckey = p_config->ckey;

  // init dc 's cliprgn infos
  dc_init_cliprgn();

  // create screen dc 's surface
  if(!init_screen_surface())
  {
    MT_ASSERT(0);
  }
  GDI_PRINTF(" GDI: screen surface init done\n");

  // init screen dc
  init_screen_dc();

  // init locks
//  INIT_LOCK (&lock_gdi);
//  INIT_LOCK (&lock_dc_slot);

  if(gdi_init_vsurf(p_config->vsurf_buf_addr, p_config->vsurf_buf_size) != SUCCESS)
  {
    /* if not malloc enough memory, then draw directly, it also works. */
//    DEBUGPRINTF ("virtual surface init failed. \n")
  }
  GDI_PRINTF(" GDI: virtual surface init done,  size = %d\n",
             p_config->vsurf_buf_size);

  return TRUE;
}


/*!
   this function init gdi module
  */
BOOL gdi_init_v2(gdi_config_t *p_config) //, s16 pitch)
{
  u32 slice = 0;
  u32 size = 0;

  class_handle_t p_gdi_handle = NULL;
  gdi_main_t *p_gdi_info = NULL;

  //init global variable.
  p_gdi_handle = (void *)mmi_alloc_buf(sizeof(gdi_main_t));
  memset((void *)p_gdi_handle, 0, sizeof(gdi_main_t));
  class_register(GDI_CLASS_ID, p_gdi_handle);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  // init  cliprect heap
  slice = sizeof(cliprc_t);
  size = slice * p_config->max_cliprect_cnt;
  GDI_PRINTF("HEAP ADDR = 0x%.8x\n", &p_gdi_info->cliprc_heap);
  p_gdi_info->p_cliprc_heap_addr = mmi_create_memf(&p_gdi_info->cliprc_heap,
                                                   p_config->max_cliprect_cnt,
                                                   slice);
  MT_ASSERT(p_gdi_info->p_cliprc_heap_addr != NULL);
  GDI_PRINTF(" GDI: cliprc heap init done,  size = %d cnt = %d\n",
             size, p_config->max_cliprect_cnt);

  // init dc slot
  size = sizeof(dc_t) * p_config->max_dc_cnt;
  p_gdi_info->p_dc_slot = (dc_t *)mmi_alloc_buf(size);
  MT_ASSERT(p_gdi_info->p_dc_slot != NULL);
  memset(p_gdi_info->p_dc_slot, 0, size);
  p_gdi_info->dc_slot_cnt = p_config->max_dc_cnt;
  GDI_PRINTF(" GDI: dc slot init done,  size = %d cnt = %d\n",
             size, p_config->max_dc_cnt);

  // init surface
  if(surface_init_v2(p_config->p_odd_addr, p_config->odd_size,
                  p_config->p_even_addr, p_config->even_size,
                  p_config->max_surface_cnt) != SUCCESS)
  {
    MT_ASSERT(0);
  } 

  GDI_PRINTF(" GDI: surface init done,  cnt = %d\n",
             p_config->max_surface_cnt);
  // set screen
  p_gdi_info->screen_format = p_config->format;
  copy_rect(&p_gdi_info->screen_rc, &p_config->osd_rect);
  if(p_config->p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_config->p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_config->p_pal->p_entry;
    //	memcpy (global_pal.palette_bits,
    //info->pal->palette_bits, sizeof(color_t)*global_pal.entry_num);
  }
  p_gdi_info->screen_cdef = p_config->cdef;
  p_gdi_info->screen_ckey = p_config->ckey;

  // init dc 's cliprgn infos
  dc_init_cliprgn();

  // create screen dc 's surface
  if(!init_screen_surface())
  {
    MT_ASSERT(0);
  }
  GDI_PRINTF(" GDI: screen surface init done\n");

  // init screen dc
  init_screen_dc();

  // init locks
//  INIT_LOCK (&lock_gdi);
//  INIT_LOCK (&lock_dc_slot);

  if(gdi_init_vsurf(p_config->vsurf_buf_addr, p_config->vsurf_buf_size) != SUCCESS)
  {
    /* if not malloc enough memory, then draw directly, it also works. */
//    DEBUGPRINTF ("virtual surface init failed. \n")
  }
  GDI_PRINTF(" GDI: virtual surface init done,  size = %d\n",
             p_config->vsurf_buf_size);

  if(p_config->roll_buffer_size != 0)
  {
    gdi_rsurf_init(p_config->roll_buffer_size);
  }

  return TRUE;
}

void gdi_release(void)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  // release locks
//  DESTROY_LOCK (lock_dc_slot);
//  DESTROY_LOCK (lock_gdi);

  // release vsurf
  gdi_release_vsurf();

  gdi_rsurf_release();

  // release dc
  mmi_free_buf(p_gdi_info->p_dc_slot);

  // delete surface
  surface_release(handle);

  // release cliprc heap
  mmi_destroy_memf(&p_gdi_info->cliprc_heap, p_gdi_info->p_cliprc_heap_addr);

  //release global variable.
  mmi_free_buf((void *)p_gdi_info);
}


BOOL gdi_reset_screen(u8 format,
                      palette_t *p_pal,
                      rect_t *p_rc,
                      u32 cdef,
                      u32 ckey)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);  

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  // set params
  p_gdi_info->screen_format = format;
  copy_rect(&p_gdi_info->screen_rc, p_rc);

  p_gdi_info->screen_cdef = cdef;
  p_gdi_info->screen_ckey = ckey;
  // delete original surface
  surface_enable(handle, p_gdi_info->p_screen_surface, FALSE);
  surface_set_display(handle, p_gdi_info->p_screen_surface, FALSE);
  surface_delete(handle, p_gdi_info->p_screen_surface);
  mtos_task_delay_ms(50);

  if(p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_pal->p_entry;
  }
  // create screen dc 's surface
  if(!init_screen_surface())
  {
    return FALSE;
  }

  // empty cliprgn at first
  gdi_empty_cliprgn(&p_gdi_info->screen_dc.lcrgn);
  gdi_empty_cliprgn(&p_gdi_info->screen_dc.ecrgn);

  // init screen dc
  init_screen_dc();

  return TRUE;
}

void gdi_release_screen(void)

{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);  

  surface_delete(handle, p_gdi_info->p_screen_surface);
  p_gdi_info->p_screen_surface = NULL;
}

void gdi_recreate_screen(void)

{
  init_screen_surface();
}

void gdi_clear_screen(void)
{
  rect_t orc;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  set_rect(&orc, 0, 0,
           RECTW(p_gdi_info->screen_rc), RECTH(p_gdi_info->screen_rc));
  surface_set_cliprect(handle, p_gdi_info->p_screen_surface, NULL);
  surface_fill_rect(handle, p_gdi_info->p_screen_surface,
                    &orc, p_gdi_info->screen_cdef);
}


void gdi_reset_trans(void)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  surface_set_trans(handle, p_gdi_info->p_screen_surface, p_gdi_info->screen_cdef, 0);
}


lib_memf_t *gdi_get_cliprc_heap(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  return &p_gdi_info->cliprc_heap;
}


s32 gdi_get_screen_handle(void)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  return surface_get_handle(handle, p_gdi_info->p_screen_surface);
}


/*!
   end of general operations
  */

/*!
   general screen operations
  */
void gdi_set_global_alpha_montage(u8 alpha)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

//	LOCK (lock_gdi);
  surface_set_alpha(handle, p_gdi_info->p_screen_surface, alpha);
//	UNLOCK (lock_gdi);
}


void gdi_set_enable(BOOL is_enable)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->p_screen_surface == NULL)
  {
    return;
  }

//	LOCK (lock_gdi);
  surface_set_display(handle, p_gdi_info->p_screen_surface, is_enable);
//	UNLOCK (lock_gdi);
}


BOOL gdi_offset_screen(s16 x, s16 y)
{
  rect_t rc;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  surface_get_srcrect(handle, p_gdi_info->p_screen_surface, &rc);
  offset_rect(&rc, x, y);
  surface_set_srcrect(handle, p_gdi_info->p_screen_surface, &rc);

  return TRUE;
}


BOOL gdi_move_screen(s16 x, s16 y)
{
  rect_t rc;
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);
  
  if(p_gdi_info->p_screen_surface == NULL)
  {
    return FALSE;
  }

  surface_get_srcrect(handle, p_gdi_info->p_screen_surface, &rc);
  offset_rect(&rc, x - rc.left, y - rc.top);
  surface_set_srcrect(handle, p_gdi_info->p_screen_surface, &rc);

  return TRUE;
}


BOOL gdi_crop_screen(rect_t *p_rc)
{
  gdi_main_t *p_gdi_info = NULL;
//  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

//	surface_set_croprect (handle, screen_surface, rc);
  return TRUE;
}


u8 gdi_get_screen_pixel_type(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  return p_gdi_info->screen_format;
}


void gdi_get_screen_rect(rect_t *p_rc)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  copy_rect(p_rc, &p_gdi_info->screen_rc);
}


/*!
   end of screen operations
  */

/*!
   general dc operations
  */

/*!
   gets a DC.
  */
hdc_t gdi_get_dc(rect_t *p_src,
                 rect_t *p_vrc,
                 crgn_info_t *p_gcrgn_info,
                 hdc_t parent)
{
  int i = 0;
  dc_t *p_cdc = NULL;
  dc_t *p_pdc = dc_hdc2pdc(parent);
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

#ifdef GDI_DEBUG
  p_gdi_info->dc_cnt++;
  if(p_gdi_info->dc_cnt > p_gdi_info->dc_max_cnt)
  {
    p_gdi_info->dc_max_cnt = p_gdi_info->dc_cnt;
  }
#endif

  // allocate an empty dc slot exclusively
//	LOCK (lock_dc_slot);
  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    p_cdc = &p_gdi_info->p_dc_slot[i];

    if(!p_cdc->is_used)
    {
      p_cdc->is_used = TRUE;

      if(p_pdc == NULL)
      {
        p_cdc->type = TYPE_GENERAL;

        p_cdc->p_curn_surface = p_gdi_info->p_screen_surface;
        p_cdc->p_back_surface = NULL;

        // try to create virtual surface
        if(gdi_create_vsurf((hdc_t)p_cdc, (s16)RECTWP(p_vrc),
                            (s16)RECTHP(p_vrc)) == SUCCESS)
        {
          u32 cck = 0;
          // initialise virtual device rect
          copy_rect(&p_cdc->vtrl_rc, p_vrc);

          // disable cck
          surface_get_colorkey(handle, p_cdc->p_back_surface, &cck);
          surface_set_colorkey(handle, p_cdc->p_back_surface, SURFACE_INVALID_COLORKEY);

          // copy actual bits to virtual surface
          surface_bitblt(handle, p_cdc->p_back_surface,
                         p_vrc->left, p_vrc->top, RECTWP(p_vrc), RECTHP(p_vrc),
                         p_cdc->p_curn_surface, 0, 0, SURFACE_ROP_SET);

          // enable ckk, again
          surface_set_colorkey(handle, p_cdc->p_back_surface, cck);
        }

        // start gpe batch, if dc type is general
        gdi_start_batch();
      }
      else
      {
        p_cdc->type = p_pdc->type | TYPE_INHERIT;

        p_cdc->p_curn_surface = p_pdc->p_curn_surface;
        p_cdc->p_back_surface = NULL;

        // chk, if need to update the first parent dev rect
        if(is_empty_rect(&p_pdc->dev_parent))
        {
          copy_rect(&p_cdc->dev_parent, &p_pdc->dev_rc);
        }
        else
        {
          copy_rect(&p_cdc->dev_parent, &p_pdc->dev_parent);
        }

        // chk, if need to update the first vitrual dev rect
        if(p_pdc->type & TYPE_VIRTUAL)
        {
          copy_rect(&p_cdc->vtrl_rc, &p_pdc->vtrl_rc);
        }
      }

      break;
    }
  }
//	UNLOCK(lock_dc_slot);

  if(i >= p_gdi_info->dc_slot_cnt)
  {
    return HDC_INVALID;
  }

  dc_init(&p_gdi_info->p_dc_slot[i], p_src, p_gcrgn_info);

  return (hdc_t)(&p_gdi_info->p_dc_slot[i]);
}


#if 0
/*
  *   Clean dc, fill dc with the color key. Usually, the dc is created on
 *virtual surface.
  */
s32 gdi_clean_dc(hdc_t hdc, BOOL is_full)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
  rect_t rc;

  if(p_dc->type & TYPE_VIRTUAL)
  {
    if(is_full)
    {
      copy_rect(&rc, &p_dc->dev_rc);
    }
    else
    {
      copy_rect(&rc, &p_dc->ecrgn.bound);
    }

    /* transfer screen coordinate to surface coordinate. */
    coor_sp2sp(p_dc, &rc.left, &rc.top);
    coor_sp2sp(p_dc, &rc.right, &rc.bottom);
    normalize_rect(&rc);

    surface_fill_rect(handle, p_dc->curn_surface, &rc, screen_ckey);

    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}


#endif

/*
   BOOL gdi_check_inherit_dc(hdc_t hdc)
   {
   dc_t *p_dc = dc_hdc2pdc (hdc);
   struct canvas * canv = p_dc->canv;

   // chk, if need update
   if((canv->type & TYPE_VIRTUAL) && !(p_dc->type & TYPE_INHERIT))
    return TRUE;
   else
    return FALSE;
   }
  */
/*
  * releases a DC.
  */
void gdi_release_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
#ifdef GDI_DEBUG
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  (p_gdi_info->dc_cnt--);
#endif

  // chk, if need update
  if((p_dc->type & TYPE_VIRTUAL) && !(p_dc->type & TYPE_INHERIT))
  {
    void *p_src_surf = NULL;
    void *p_dst_surf = NULL;
    rect_t orc, eff_rc;
    cliprc_t *p_crc = NULL;
    BOOL is_vsurf_delete = FALSE;

    p_src_surf = p_dc->p_curn_surface, p_dst_surf = p_dc->p_back_surface;

    // bitblt to the target dc
    // chk, lock rgn
    if(dc_is_general_dc(p_dc))
    {
      LOCK_GCRINFO(p_dc);
      if(!dc_generate_ecrgn(p_dc, FALSE))
      {
        UNLOCK_GCRINFO(p_dc);
        //delete vsurf.
        gdi_delete_vsurf(hdc);

        is_vsurf_delete = TRUE;
      }
    }

    if(!is_vsurf_delete)
    {
      /* virtual device coordinate is already screen coordinate,
         so, it don't need to convert */
      copy_rect(&orc, &p_dc->vtrl_rc);

      /* set output rect */
      copy_rect(&p_dc->rc_output, &orc);
      normalize_rect(&p_dc->rc_output);

      //ENTER_DRAWING(p_dc);
      if(dc_enter_drawing(p_dc) >= 0)
      {
        p_crc = p_dc->ecrgn.p_head;
        while(NULL != p_crc)
        {
          if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
          {
            surface_set_cliprect(handle, p_dst_surf, &eff_rc);

            surface_bitblt(handle, p_src_surf, 0, 0,
                           (u16)RECTW(orc), (u16)RECTH(
                             orc),
                           p_dst_surf, (u16)orc.left, (u16)orc.top,
                           SURFACE_ROP_SET);
          }

          p_crc = p_crc->p_next;
        }

        #ifdef CACHE_ON
        extern void flush_dcache_all();
        flush_dcache_all();
        #endif

        /* disable clipper */
        surface_set_cliprect(handle, p_dc->p_curn_surface, NULL);

        //LEAVE_DRAWING(p_dc);
        dc_leave_drawing(p_dc);
      }

      UNLOCK_GCRINFO(p_dc);

      // delete virtual surface
      gdi_delete_vsurf(hdc);
    }
  }

  if(p_dc->type & TYPE_GENERAL 
    && !(p_dc->type & TYPE_INHERIT)) /* is parent dc */
  {
    // end gpe batch, if dc type is general
    gdi_end_batch(TRUE, &p_dc->ecrgn.bound);
    //gdi_end_batch(TRUE, &p_dc->dev_rc);
  }
  
#ifdef CACHE_ON
  extern void flush_dcache_all();

  if(p_dc->type & TYPE_GENERAL) /* is parent dc */
  {
    flush_dcache_all();
  }
#endif

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

//	LOCK (lock_dc_slot);
  p_dc->is_used = FALSE;
  empty_rect(&p_dc->dev_parent);
//	UNLOCK (lock_dc_slot);
}


#if 0
/*
  * creates a memory DC.
  *   - MEMDC_FLAG_SWSURFACE\n
  *     Creates the surface of memory DC in the system memory.
  *   - MEMDC_FLAG_HWSURFACE\n
  *     Creates the surface of memory DC in the video memory.
  *   - MEMDC_FLAG_SRCCOLORKEY\n
  *     The created memory DC will use a source color key to blit to other DC.
  *   - MEMDC_FLAG_SRCALPHA\n
  *     The created memory DC will use a source alpha blending to blit to
  *     other DC.
  *   - MEMDC_FLAG_RLEACCEL\n
  *     The memory DC will be RLE encoded
  */
hdc_t gdi_create_mem_dc(s16 width,
                        s16 height,
                        pixel_type_t format,
                        s16 pitch,
                        u32 flags)
{
  dc_t *p_mem_dc = NULL;
  cdc_info_t cinfo;
  void *surface;
  surface_desc_t desc;

  s32 ret = SUCCESS;

  if(NULL == (p_mem_dc = (dc_t *)MALLOC(sizeof(dc_t))))
  {
    return HDC_INVALID;
  }

  // set the surface descriptor
  desc.format = format;

  desc.flag = DSD_CAPS | DSD_PIXELFORMAT | DSD_PITCH | DSD_WIDTH | DSD_HEIGHT;
  desc.width = width;
  desc.height = height;

  desc.caps = DSCAPS_SYSTEMMEMORY;
  desc.bits = NULL;
  desc.pitch = pitch;

//	LOCK (lock_gdi);
  ret = surface_create(&surface, &desc);
//	UNLOCK (lock_gdi);

  if(ret != SUCCESS)
  {
    FREE(p_mem_dc);
    return HDC_INVALID;
  }

  p_mem_dc->type = TYPE_MEMORY;
  p_mem_dc->is_used = TRUE;
  p_mem_dc->surface = surface;

  // set init dc infor
  set_rect(&cinfo.rc, 0, 0, width, height);
  cinfo.crgn_info = NULL;
  cinfo.lfont = NULL;

  dc_init(p_mem_dc, &cinfo);

  gdi_init_cliprgn(&p_mem_dc->ecrgn, &cliprc_heap);

  p_mem_dc->gcrgn_info = NULL;
  p_mem_dc->dev_rc.left = 0;
  p_mem_dc->dev_rc.top = 0;
  p_mem_dc->dev_rc.right = width;
  p_mem_dc->dev_rc.bottom = height;

  gdi_set_cliprgn(&p_mem_dc->ecrgn, &p_mem_dc->dev_rc);

  return (hdc_t)p_mem_dc;
}


/*
  * creates a memory DC which is compatible with the specified reference DC.
  */
hdc_t gdi_create_compatible_dc(hdc_t hdc)
{
  dc_t *p_dc;
  dc_t *p_mem_dc = NULL;
  s16 width, height, bpp;

  void *surface;
  surface_desc_t desc;

  s32 ret = SUCCESS;

  p_dc = dc_hdc2pdc(hdc);

  if(NULL == (p_mem_dc = (dc_t *)MALLOC(sizeof(dc_t))))
  {
    return HDC_INVALID;
  }

  width = RECTW(p_dc->dev_rc);
  height = RECTH(p_dc->dev_rc);

  // set the surface descriptor
  desc.format = p_dc->surface->format;

  desc.flag = DSD_CAPS | DSD_PIXELFORMAT | DSD_WIDTH | DSD_HEIGHT;
  desc.width = width;
  desc.height = height;

  desc.caps = DSCAPS_SYSTEMMEMORY;
  desc.bits = NULL;
//	desc.pitch = pitch;

//	LOCK (lock_gdi);
  ret = surface_create(&surface, &desc);
//	UNLOCK (lock_gdi);

  if(ret != SUCCESS)
  {
    FREE(p_mem_dc);
    return HDC_INVALID;
  }

  /* set surface attributes */
  bpp = p_dc->surface->bpp;
  if(bpp <= 8)
  {
    surface_set_palette(handle, surface, 0, 1 << bpp, p_dc->surface->palette_bits);
  }

  memcpy(p_mem_dc, p_dc, sizeof(dc_t));

  p_mem_dc->type = TYPE_MEMORY;
  p_mem_dc->is_used = TRUE;
  p_mem_dc->surface = surface;

  // clip region info
  gdi_init_cliprgn(&p_mem_dc->ecrgn, &cliprc_heap);
  p_mem_dc->gcrgn_info = NULL;

  p_mem_dc->dev_rc.left = 0;
  p_mem_dc->dev_rc.top = 0;
  p_mem_dc->dev_rc.right = width;
  p_mem_dc->dev_rc.bottom = height;

  gdi_set_cliprgn(&p_mem_dc->ecrgn, &p_mem_dc->dev_rc);

  return (hdc_t)p_mem_dc;
}


/*
  * deletes a memory DC.
  */
void gdi_delete_mem_dc(hdc_t hdc)
{
  dc_t *p_mem_dc;

  p_mem_dc = dc_hdc2pdc(hdc);

  surface_delete(p_mem_dc->surface);

  gdi_empty_cliprgn(&p_mem_dc->ecrgn);

  FREE(p_mem_dc);
}


/*
  * locks a dc to get direct access to pixels in the DC.
  */
//u8 *gdi_lock_dc (hdc_t hdc, const rect_t *rw_rc,
s16 *width, s16 *height, u16 *pitch);
/*
  * unlocks a locked DC.
  */
//void gdi_unlock_dc (hdc_t hdc);
#endif

/*!
   end of general dc operations
  */

/*!
   clipping operations
  */

/*!
   sets the visible region of a DC to be a region.
  */
void gdi_select_cliprgn(hdc_t hdc, const cliprgn_t *p_rgn)
{
  dc_t *p_dc = NULL;

  p_dc = dc_hdc2pdc(hdc);
  if(dc_is_general_dc(p_dc))
  {
    gdi_copy_cliprgn(&p_dc->lcrgn, p_rgn);

    /* for general DC, regenerate effective region. */
    LOCK_GCRINFO(p_dc);
    dc_generate_ecrgn(p_dc, TRUE);
    UNLOCK_GCRINFO(p_dc);
  }
  else
  {
    gdi_copy_cliprgn(&p_dc->ecrgn, p_rgn);
    gdi_intersect_cliprect(&p_dc->ecrgn, &p_dc->dev_rc);
  }
}


#if 0

/*
  * excludes the specified rectangle from the current visible region of a DC.
  */
void gdi_exclude_cliprect(hdc_t hdc, const rect_t *rc)
{
  dc_t *p_dc;
  rect_t orc;

  p_dc = dc_hdc2pdc(hdc);

  orc = *rc;
  normalize_rect(&orc);
  if(is_empty_rect(&orc))
  {
    return;
  }

  if(dc_is_general_dc(p_dc))
  {
    dc_reset_lcrgn(p_dc);
    gdi_subtract_cliprect(&p_dc->lcrgn, &orc);

    // transfer device coordinate to screen coordinate.
    coor_dp2sp(p_dc, &orc.left, &orc.top);
    coor_dp2sp(p_dc, &orc.right, &orc.bottom);
  }

  gdi_subtract_cliprect(&p_dc->ecrgn, &orc);
}


/*
  * includes the specified rectangle to the current visible region of a DC.
  */
void gdi_include_cliprect(hdc_t hdc, const rect_t *rc)
{
  dc_t *p_dc;
  rect_t orc;

  p_dc = dc_hdc2pdc(hdc);

  orc = *rc;
  normalize_rect(&orc);
  if(is_empty_rect(&orc))
  {
    return;
  }

  if(dc_is_general_dc(p_dc))
  {
    if(p_dc->lcrgn.head != NULL)
    {
      gdi_add_cliprect(&p_dc->lcrgn, &orc);
    }
    else
    {
      gdi_set_cliprgn(&p_dc->lcrgn, &orc);
    }

    // transfer device coordinate to screen coordinate.
    coor_dp2sp(p_dc, &orc.left, &orc.top);
    coor_dp2sp(p_dc, &orc.right, &orc.bottom);

    LOCK_GCRINFO(p_dc);
    dc_generate_ecrgn(p_dc, TRUE);
    UNLOCK_GCRINFO(p_dc);
  }
  else
  {
    gdi_add_cliprect(&p_dc->ecrgn, &orc);
  }
}


/*
  * sets the visible region of a DC to be a rectangle.
  */
void gdi_select_cliprect(hdc_t hdc, const rect_t *rc)
{
  dc_t *p_dc;
  rect_t orc;

  p_dc = dc_hdc2pdc(hdc);

  if(rc != NULL)
  {
    orc = *rc;
    normalize_rect(&orc);
    if(is_empty_rect(&orc))
    {
      return;
    }
  }
  else
  {
    orc = p_dc->dev_rc;
  }

  if(dc_is_general_dc(p_dc))
  {
    if(rc != NULL)
    {
      gdi_set_cliprgn(&p_dc->lcrgn, &orc);
    }
    else
    {
      gdi_empty_cliprgn(&p_dc->lcrgn);
    }

    /* for general DC, regenerate effective region. */
    LOCK_GCRINFO(p_dc);
    dc_generate_ecrgn(p_dc, TRUE);
    UNLOCK_GCRINFO(p_dc);
  }
  else
  {
    if(intersect_rect(&orc, &orc, &p_dc->dev_rc))
    {
      gdi_set_cliprgn(&p_dc->ecrgn, &orc);
    }
    else
    {
      gdi_empty_cliprgn(&p_dc->ecrgn);
    }
  }
}


/*
  * retrives the bounding rectangle of the current visible region of a DC.
  */
void gdi_get_boundrect(hdc_t hdc, rect_t *rect)
{
  dc_t *p_dc;

  p_dc = dc_hdc2pdc(hdc);

  if(dc_is_general_dc(p_dc))
  {
    *rect = p_dc->lcrgn.bound;
  }
  else
  {
    *rect = p_dc->ecrgn.bound;
  }
}


/*
  * determines whether a point is visible.
  */
BOOL gdi_is_visible_pt(hdc_t hdc, s16 x, s16 y)
{
  dc_t *p_dc;

  p_dc = dc_hdc2pdc(hdc);

  if(dc_is_general_dc(p_dc))
  {
    if(p_dc->lcrgn.head == NULL)
    {
      return TRUE;
    }
    return gdi_is_pt_in_cliprgn(&p_dc->lcrgn, x, y);
  }

  return gdi_is_pt_in_cliprgn(&p_dc->ecrgn, x, y);
}


/*
  * determines whether the specified rectangle is visible.
  */
BOOL gdi_is_visible_rect(hdc_t hdc, const rect_t *rc)
{
  dc_t *p_dc;

  p_dc = dc_hdc2pdc(hdc);

  if(dc_is_general_dc(p_dc))
  {
    if(p_dc->lcrgn.head == NULL)
    {
      return TRUE;
    }
    return gdi_is_rect_in_cliprgn(&p_dc->lcrgn, rc);
  }

  return gdi_is_rect_in_cliprgn(&p_dc->ecrgn, rc);
}


#endif
/*!
   end of clipping operations
  */


/*!
   drawing operations
  */
typedef s32 (*fill_func_t)(u32 handle, void *p_surf, rect_t *p_rc, u32 color);

/*!
   fills a rectangle box.
  */
static void _fill_x_rect(hdc_t hdc,
                         rect_t *p_rc,
                         u32 color,
                         fill_func_t p_fill_func)
{
  dc_t *p_dc = NULL;
  cliprc_t *p_clip_rc = NULL;
  rect_t eff_rc, orc = *p_rc;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return;
  }

  p_dc = dc_hdc2pdc(hdc);
  // chk, lock rgn
  if(dc_is_general_dc(p_dc))
  {
    LOCK_GCRINFO(p_dc);
    if(!dc_generate_ecrgn(p_dc, FALSE))
    {
      UNLOCK_GCRINFO(p_dc);
      return;
    }
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  p_dc->cur_pixel = color;

  //ENTER_DRAWING(p_dc);
  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return;
  }

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  p_clip_rc = p_dc->ecrgn.p_head;
  while(NULL != p_clip_rc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_clip_rc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      surface_set_cliprect(handle, p_dc->p_curn_surface, &eff_rc);

      p_fill_func(handle, p_dc->p_curn_surface, &orc, p_dc->cur_pixel);
    }
    p_clip_rc = p_clip_rc->p_next;
  }
  /* disable clipper */
  surface_set_cliprect(handle, p_dc->p_curn_surface, NULL);

  //LEAVE_DRAWING(p_dc);
  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);
}


/*
  * fills a rectangle box.
  */
void gdi_fill_rect(hdc_t hdc, rect_t *p_rc, u32 color)
{
  _fill_x_rect(hdc, p_rc, color, surface_fill_rect);
}


/*
   void gdi_fill_round_rect(hdc_t hdc, rect_t *p_rc, u32 color)
   {
   _fill_x_rect(hdc, p_rc, color, surface_fill_round_rect);
   }
  */


/*!
   end of drawing operations
  */

/*!
   general operations
  */
u8 gdi_get_bpp(u8 format)
{
  u8 bpp = 0;

  switch(format)
  {
    case COLORFORMAT_RGB4BIT:
      bpp = 4;
      break;
    case COLORFORMAT_RGB8BIT:
      bpp = 8;
      break;
    case COLORFORMAT_RGB565:
      bpp = 16;
      break;
    case COLORFORMAT_RGBA5551:
      bpp = 16;
      break;
    case COLORFORMAT_ARGB1555:
      bpp = 16;
      break;
    case COLORFORMAT_RGBA8888:
      bpp = 32;
      break;
    case COLORFORMAT_ARGB8888:
      bpp = 32;
      break;
    default:
      bpp = 0;
      MT_ASSERT(0);
  }

  return bpp;
}

void gdi_start_batch(void)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  surface_start_batch(handle, p_gdi_info->p_screen_surface);
}

void gdi_end_batch(BOOL is_sync, rect_t *p_rect)
{
  gdi_main_t *p_gdi_info = NULL;
  u32 handle = (u32)class_get_handle_by_id(SURFACE_CLASS_ID);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  surface_end_batch(handle, p_gdi_info->p_screen_surface, is_sync, p_rect);
}

/*!
   end of general operations
  */
