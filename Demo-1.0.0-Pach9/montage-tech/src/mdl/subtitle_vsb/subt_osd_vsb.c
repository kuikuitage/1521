/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "hal_base.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mem_manager.h"

#include "sub.h"
#include "lib_rect.h"
#include "dmx.h"
#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "drv_misc.h"
#include "subt_priv_vsb.h"

subt_rc_t subt_layer_select_osd(subt_render_t *p_render, disp_layer_id_t layer)
{
    p_render->lay_id = layer;
    return SUBT_RC_SUCCESS;
}

subt_rc_t subt_osd_init_vsb(subt_render_t *p_render)
{
    RET_CODE ret = 0;

    p_render->p_disp = (sub_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_render->p_disp);

    p_render->p_gpe = (sub_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_render->p_gpe);

    ret = disp_layer_show(p_render->p_disp, p_render->lay_id, TRUE);
    MT_ASSERT(SUCCESS == ret);

    return SUBT_RC_SUCCESS;
}

subt_rc_t subt_osd_deinit_vsb(subt_render_t *p_render)
{
    return SUBT_RC_SUCCESS;
}

/*!
  Convert 2/4/8bpp to continue buffer to do gpe ops  
  */
subt_rc_t subt_osd_buf_convert(dvb_subtitle_vsb_t *p_subt,
                                  u8 *p_in_buf,
                                  u8 *p_out_buf,
                                  u8  pix_in_bytes,
                                  u32 op_size)
{
  s32 i = (s32)op_size;

  //4bpp: 1byte lowest 4bpp is valid, need 2bytes into 1bytes
  if(2 == pix_in_bytes)
  {
    while(i > 0)
    {
        *p_out_buf = (p_in_buf[0] << 4) + p_in_buf[1];
        p_out_buf ++;
        p_in_buf += 2;
        i -= 2;
    }

  }
  //2bpp: 1byte lowest 2bpp is valid, need 4bytes into 1bytes
  else if(4 == pix_in_bytes)
  {
    while(i > 0)
    {
        *p_out_buf = (p_in_buf[0] << 6)
            + (p_in_buf[1] << 4)
            + (p_in_buf[2] << 2) + p_in_buf[3];
        p_out_buf ++;
        p_in_buf += 4;
        i -= 4;
    }
  }

  return SUBT_RC_SUCCESS;
}

/*!
  Copy a line buffer to the region  
  */
subt_rc_t subt_osd_copy_lines_vsb(dvb_subtitle_vsb_t *p_subt,
                                  subt_osd_region_t *p_osd_region,
                                  rect_t *p_rect,
                                  u8 *p_pix_buf,
                                  u32 pitch,
                                  u32 in_size)
{
  RET_CODE ret = SUCCESS;
  void *p_gpe = p_subt->p_render->p_gpe;
  region_t *p_rgn_hdl = p_osd_region->p_rgn_hdl;
  gpe_param_vsb_t gpe_param = {0, 0, 0, 0, 0, 0};
  rect_t fill_rect = {0};

  MT_ASSERT(NULL != p_gpe);
  if(NULL == p_rgn_hdl)
    return SUBT_RC_SUCCESS;
  
  ret = gpe_draw_image_vsb(p_gpe, 
                           p_rgn_hdl,
                           p_rect,
                           p_pix_buf,
                           NULL,
                           0,
                           pitch,
                           in_size,
#ifdef WARRIORS
                           PIX_FMT_YUVPALETTE8,
#else
                           PIX_FMT_MAX,
#endif
                           &gpe_param,
                           &fill_rect);
  
  //MT_ASSERT(SUCCESS == ret);
  if(ret != SUCCESS)
    {
      OS_PRINTF("SUBT:  copy lines error !!! \n");
    }   //for error byte break bug 13135

  return SUBT_RC_SUCCESS;
}

/*!
  Fill the whole region with default color  
  */
subt_rc_t subt_osd_fill_region_vsb(dvb_subtitle_vsb_t *p_subt, subt_region_t *p_region)
{
    u8  color = 0;
    RET_CODE ret = SUCCESS;
    rect_t rect;
  
    if(p_region->osd_region.p_rgn_hdl == NULL)
        return SUBT_RC_SUCCESS;

    if(p_region->depth == 1)
    {
        color = p_region->pixel_code_2_bit;
    }
    else if(p_region->depth == 2)
    {
        color = p_region->pixel_code_4_bit;
    }
    else if(p_region->depth == 3)
    {
        color = p_region->pixel_code_8_bit;
    }
    else
    {
        MT_ASSERT(0);
        return SUBT_RC_INVALID_DATA;
    }

    //rect null, default fill the whole region
    rect.left = 0;
    rect.top = 0;
    rect.right = p_region->width;
    rect.bottom = p_region->height;
    ret = gpe_draw_rectangle_vsb(p_subt->p_render->p_gpe, p_region->osd_region.p_rgn_hdl, 
                                &rect,color);
    MT_ASSERT(SUCCESS == ret);

    return SUBT_RC_SUCCESS;
}

/*!
  Move the region to a place. 
  */
subt_rc_t subt_osd_move_region_vsb(subt_render_t *p_render,
                                          subt_region_t *p_region,
                                          point_t *p_pos)
{
    RET_CODE ret = 0;

    if(p_region->osd_region.p_rgn_hdl != NULL)
    {
        ret = disp_layer_move_region(p_render->p_disp, p_region->osd_region.p_rgn_hdl, p_pos);
        MT_ASSERT(SUCCESS == ret);
    }

    return SUBT_RC_SUCCESS;
}

/*!
  Display the region. 
  */
subt_rc_t subt_osd_dispaly_region_vsb(subt_render_t *p_render
    , subt_region_t *p_region, subt_clut_t *p_clut)
{
    u32 *p_osd_clut = NULL;
    RET_CODE ret = 0;
    u32 index = 0;

    if(p_region->osd_region.p_rgn_hdl == NULL)
        return SUBT_RC_SUCCESS;

    if(p_region->depth == 1)
    {
        p_osd_clut = p_clut->clut4;
        index = 4;
        OS_PRINTF("SUBT:    4-CLUT\n");
    }
    else if(p_region->depth == 2)
    {
        p_osd_clut = p_clut->clut16;
        index = 16;
        OS_PRINTF("SUBT:    16-CLUT\n");
    }
    else if(p_region->depth == 3)
    {
        p_osd_clut = p_clut->clut256;
        index = 256;
        OS_PRINTF("SUBT:    256-CLUT\n");
    }
    else
    {
        MT_ASSERT(0);
        return SUBT_RC_FAILED;
    }

    ret = region_set_palette(p_region->osd_region.p_rgn_hdl, p_osd_clut, index);
    MT_ASSERT(SUCCESS == ret);

    ret = region_show(p_region->osd_region.p_rgn_hdl, TRUE);
    MT_ASSERT(SUCCESS == ret);

#ifdef WARRIORS    
    p_render->p_disp = (sub_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_render->p_disp);
    p_render->p_gpe = (sub_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_render->p_gpe);
    ret = disp_layer_show(p_render->p_disp, p_render->lay_id, TRUE);
    MT_ASSERT(SUCCESS == ret);
#endif

    return SUBT_RC_SUCCESS;
}

/*!
  Hide the region. 
  */
subt_rc_t subt_osd_hide_region_vsb(subt_render_t *p_render, subt_region_t *p_region)
{
    RET_CODE ret = 0;

    if(p_region->osd_region.p_rgn_hdl == NULL)
        return SUBT_RC_SUCCESS;

    ret = region_show(p_region->osd_region.p_rgn_hdl, FALSE);
    MT_ASSERT(SUCCESS == ret);

    return SUBT_RC_SUCCESS;
}

/*!
  Create a region and add it to osd layer. 
  */
subt_rc_t subt_osd_create_region_vsb(subt_render_t *p_render, 
                                           rect_t *p_rect,
                                           pix_fmt_t color_fmt, 
                                           const u32 *p_clut, 
                                           u32 clut_indexs, 
                                           subt_osd_region_t *p_osd_region)
{
    RET_CODE ret = SUBT_RC_FAILED;
    point_t pos = {0, 0};
    rect_size_t rect;
    u32 align = 0;
    u32 rgn_size = 0;

    rect.w = p_rect->right - p_rect->left + 1;
#ifdef WARRIORS
    rect.w = ((rect.w + 7) & (~7));
#endif
    rect.h = p_rect->bottom - p_rect->top + 1;    
    pos.x = p_rect->left;
    pos.y = p_rect->top;
#ifdef WARRIORS
    //graphic scaler limit, region width should be in 8 pixel align
    rect.w = (rect.w + 7) & (~7);
#endif
    p_osd_region->p_rgn_hdl = region_create(&rect, PIX_FMT_YUVPALETTE8);
    MT_ASSERT(NULL != p_osd_region->p_rgn_hdl);

    ret = disp_calc_region_size(p_render->p_disp, p_render->lay_id,
                                p_osd_region->p_rgn_hdl, &align, &rgn_size);
    MT_ASSERT(SUCCESS == ret);

#ifdef WARRIORS
    p_osd_region->p_buf = NULL;
#else
#ifndef WIN32
    {
        void *p_buf_align = NULL;
        p_osd_region->p_buf_actual = mtos_align_malloc((rgn_size + align), 8);
        MT_ASSERT(NULL != p_osd_region->p_buf_actual);
        p_buf_align = p_osd_region->p_buf_actual;
        p_buf_align = (void *)hal_addr_nc((u32) p_osd_region->p_buf_actual);
        memset(p_buf_align, 0, (rgn_size + align));
        p_buf_align = (void *)((((u32) p_buf_align + align - 1) & (~(align - 1))));
        p_osd_region->p_buf = p_buf_align;
    }
#endif
#endif
    //the buffer NULL means to use the configured buffer 
    ret = disp_layer_add_region(p_render->p_disp, p_render->lay_id,
        p_osd_region->p_rgn_hdl, &pos, p_osd_region->p_buf);
#ifdef WARRIORS
    MT_ASSERT(SUCCESS == ret);
#else
    if(SUCCESS != ret)
    {
        ret = region_delete(p_osd_region->p_rgn_hdl);
        MT_ASSERT(SUCCESS == ret);
        if (NULL != p_osd_region->p_buf)
        {
            MT_ASSERT(NULL != p_osd_region->p_buf_actual);
   
            mtos_align_free(p_osd_region->p_buf_actual);
            p_osd_region->p_buf_actual = NULL;
            p_osd_region->p_buf = NULL;
            p_osd_region->p_rgn_hdl = NULL;
        }
        return SUBT_RC_FAILED;
    }
#endif

    ret = region_set_palette(p_osd_region->p_rgn_hdl, (u32 *)p_clut, clut_indexs);
    MT_ASSERT(SUCCESS == ret);

    ret = region_show(p_osd_region->p_rgn_hdl, FALSE);
    MT_ASSERT(SUCCESS == ret);

    return SUBT_RC_SUCCESS;
}

/*!
  Delete a region and remove it from osd layer. 
  */
subt_rc_t subt_osd_delete_region_vsb(subt_render_t *p_render
    , subt_osd_region_t *p_osd_region)
{
    if (p_osd_region->p_rgn_hdl == NULL)
        return SUBT_RC_SUCCESS;

    if(p_render != NULL && p_osd_region->p_rgn_hdl != NULL)
    {
        RET_CODE ret = SUBT_RC_FAILED;
        ret = disp_layer_remove_region(p_render->p_disp, 
                                       p_render->lay_id,
                                       p_osd_region->p_rgn_hdl);
        MT_ASSERT(SUCCESS == ret);
        ret = region_delete(p_osd_region->p_rgn_hdl);
        MT_ASSERT(SUCCESS == ret);
        p_osd_region->p_rgn_hdl = NULL;
        if(NULL != p_osd_region->p_buf)
        {
#ifdef WARRIORS
            mtos_free(p_osd_region->p_buf);
#else
            MT_ASSERT(NULL != p_osd_region->p_buf_actual);

            mtos_align_free(p_osd_region->p_buf_actual);
            p_osd_region->p_buf_actual = NULL;
#endif
            p_osd_region->p_buf = NULL;
        }
    }

    return SUBT_RC_SUCCESS;
}
