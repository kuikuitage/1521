/******************************************************************************/
/******************************************************************************/
#ifndef __GDI_PRIVATE_H__
#define __DDI_PRIVATE_H__

/*!
  Structure for global variable.
  */
typedef struct
{
  /*!
    Screen dc.
    */
  dc_t screen_dc;
  /*!
    Screen surface.
    */
  void *p_screen_surface;
  /*!
    DC slot.
    */
  dc_t *p_dc_slot;
  /*!
    DC slot count.
    */
  u16 dc_slot_cnt;
  /*!
    Clip rectangle heap.
    */
  lib_memf_t cliprc_heap;
  /*!
    Clip rectangle heap address.
    */
  void *p_cliprc_heap_addr;
  /*!
    Screen format.
    */
  u8 screen_format;
  /*!
    Screen rectangle.
    */
  rect_t screen_rc;  
  /*!
    Screen default color.
    */
  u32 screen_cdef;
  /*!
    Screen color key.
    */
  u32 screen_ckey;
  /*!
    Global palette.
    */
  palette_t global_pal;  
  /*!
    DC count.
    */
  u32 dc_cnt;
  /*!
    DC max count.
    */
  u32 dc_max_cnt;
  /*!
    Clip rectangle max count.
    */
  u32 cliprc_max_cnt;
  /*!
    Clip rectangle current count.
    */
  u32 cliprc_curn_cnt;
  /*!
    Virtual surface buffer address.
    */
  void * p_vsurf_buf[2];
  /*!
    Virtual surface buffer size.
    */
  u32 vsurf_buf_size;

  void *p_roll_heap_addr;

  lib_memp_t roll_heap;

  u32 rsurf_heap_size;
}gdi_main_t;

#endif

