/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mem_manager.h"

#include "lib_rect.h"
#include "common.h"
#include "dmx.h"
#include "display.h"
#include "drv_misc.h"
#include "subt_priv_vsb.h"

/*!
  clear region list
  */
void subt_db_clear_regionlist(dvb_subtitle_vsb_t *p_subt)
{
    subt_region_t   *p_r = NULL, *p_r_next = NULL;
    p_r = p_subt->p_region_list;
    while(p_r != NULL)
    {
        p_r_next = p_r->p_next;

        if(p_r->p_object_info != NULL)
            mtos_free(p_r->p_object_info);

        if(p_r->osd_region.p_rgn_hdl != NULL)
        {
            subt_osd_hide_region_vsb(p_subt->p_render, p_r);
            subt_osd_delete_region_vsb(p_subt->p_render, &p_r->osd_region);
        }

        mtos_free(p_r);
        p_r = p_r_next;
    }
    p_subt->p_region_list = NULL;
}

/*!
  reset
  */
void subt_db_reset_vsb(dvb_subtitle_vsb_t *p_subt)
{
    subt_page_t     *p_p = NULL;
    subt_region_t   *p_r = NULL, *p_r_next = NULL;
    subt_clut_t     *p_c = NULL, *p_c_next = NULL;
    //subt_object_t   *p_o, *p_o_next;

    p_p = &(p_subt->composition_page);
    if (p_p->p_region_info != NULL)
        mtos_free(p_p->p_region_info);

    p_p->page_timeout        = 0;
    p_p->page_version_number = 0;
    p_p->p_region_info       = NULL;
    p_p->region_number       = 0;

    p_p = &(p_subt->ancillary_page);
    if(p_p->p_region_info != NULL)
        mtos_free(p_p->p_region_info);

    p_p->page_timeout        = 0;
    p_p->page_version_number = 0;
    p_p->p_region_info       = NULL;
    p_p->region_number       = 0;

    p_r = p_subt->p_region_list;
    while(p_r != NULL)
    {
        p_r_next = p_r->p_next;

        if(p_r->p_object_info != NULL)
            mtos_free(p_r->p_object_info);

        if(p_r->osd_region.p_rgn_hdl != NULL)
            subt_osd_delete_region_vsb(p_subt->p_render, &p_r->osd_region);

        mtos_free(p_r);
        p_r = p_r_next;
    }
    p_subt->p_region_list = NULL;

    p_c = p_subt->p_clut_list;
    while(p_c != NULL)
    {
        p_c_next = p_c->p_next;
        mtos_free(p_c);
        p_c = p_c_next;
    }
    p_subt->p_clut_list = NULL;

    #if 0
    p_o = p_subt->p_object_list;
    while(p_o != NULL)
    {
        p_o_next = p_o->p_next;
        mtos_free(p_o);
        p_o = p_o_next;
    }
    p_subt->p_object_list = NULL;
    #endif

    p_subt->dec_stats = SUBT_DEC_STATS_NORMAL_CASE;
}

/*!
  create region info which parsed from pcs
  */
void subt_db_creat_region_info_vsb(dvb_subtitle_vsb_t *p_subt
    , u16 num, subt_region_info_t **p_region_info, u16 *p_created_num)
{
    if(num != 0)
    {
        *p_region_info = (subt_region_info_t *)mtos_malloc(
            sizeof(subt_region_info_t) * num);
        memset(*p_region_info, 0, sizeof(subt_region_info_t) * num);
        *p_created_num = num;
    }
    else
    {
        *p_region_info = 0;
    }

    if(*p_region_info == NULL)
        *p_created_num = 0;
}

/*!
  delete region info
  */
void subt_db_delete_region_info_vsb(dvb_subtitle_vsb_t *p_subt
    , subt_region_info_t *p_region_info)
{
  if(p_region_info != NULL)
    mtos_free(p_region_info);
}

/*!
  create object info
  */
void subt_db_creat_object_info_vsb(dvb_subtitle_vsb_t *p_subt
    , u16 num, subt_object_info_t **pp_object_info, u16 *p_created_num)
{
    if(num != 0)
    {
        *pp_object_info = (subt_object_info_t *)mtos_malloc(
            sizeof(subt_object_info_t) * num);
        memset(*pp_object_info, 0, sizeof(subt_object_info_t) * num);
        *p_created_num = num;
    }
    else
    {
        *pp_object_info = NULL;
    }

    if(*pp_object_info == NULL)
        *p_created_num = 0;
}

/*!
  delete object info
  */
void subt_db_delete_object_info_vsb(dvb_subtitle_vsb_t *p_subt
    , subt_object_info_t *p_object_info)
{
    if(p_object_info != NULL)
        mtos_free(p_object_info);
}

/*!
  get the region RCS in subtitle spec requirement
  */
subt_rc_t subt_db_get_region_vsb(dvb_subtitle_vsb_t *p_subt
    , u16 page_id, u8 region_id, subt_region_t **pp_region)
{
    subt_region_t *p_r = p_subt->p_region_list;

    *pp_region = NULL;

    while(p_r != NULL)
    {
        if(p_r->page_id == page_id && p_r->id == region_id)
        {
            *pp_region = p_r;
            return SUBT_RC_SUCCESS;
        }

        p_r = p_r->p_next;
    }

  return SUBT_RC_FAILED;
}

/*!
  create the region RCS in subtitle spec requirement
  */
subt_rc_t subt_db_create_region_vsb(dvb_subtitle_vsb_t *p_subt
    , subt_region_t **pp_region)
{
    *pp_region = (subt_region_t *)mtos_malloc(sizeof(subt_region_t));
     memset(*pp_region, 0, sizeof(subt_region_t)); 

    if(*pp_region == NULL)
        return SUBT_RC_FAILED;

    (*pp_region)->p_object_info     = NULL;
    (*pp_region)->osd_region.p_rgn_hdl = NULL;

    if(p_subt->p_region_list == NULL)
        (*pp_region)->p_next = NULL;
    else
        (*pp_region)->p_next = p_subt->p_region_list;

    p_subt->p_region_list = *pp_region;

    return SUBT_RC_SUCCESS;
}

/*!
  delete the region RCS in subtitle spec requirement
  */
subt_rc_t subt_db_delete_region_vsb(dvb_subtitle_vsb_t *p_subt
    , u16 page_id, u8 region_id)
{
    subt_region_t *p_r = p_subt->p_region_list;
    subt_region_t *p_r_prev = NULL;

    while(p_r != NULL)
    {
        if(p_r->page_id == page_id && p_r->id == region_id)
        {
            if(p_r_prev == NULL)
                p_subt->p_region_list = p_r->p_next;
            else
                p_r_prev->p_next = p_r->p_next;

            mtos_free(p_r);
            break;
        }

        p_r = p_r->p_next;
    }

    return SUBT_RC_SUCCESS;
}

/*!
  get the clut CDS in subtitle spec requirement
  */
subt_rc_t subt_db_get_clut_vsb(dvb_subtitle_vsb_t *p_subt
    , u16 page_id, u8 clut_id, subt_clut_t **pp_clut)
{
    subt_clut_t *p_c = p_subt->p_clut_list;

    while(p_c != NULL)
    {
        if(p_c->page_id == page_id && p_c->clut_id == clut_id)
        {
            *pp_clut = p_c;
            return SUBT_RC_SUCCESS;
        }

        p_c = p_c->p_next;
    }

    return SUBT_RC_FAILED;
}

/*!
  create the clut CDS in subtitle spec requirement
  */
subt_rc_t subt_db_create_clut_vsb(dvb_subtitle_vsb_t *p_subt
    , subt_clut_t **pp_clut)
{
    *pp_clut = (subt_clut_t *)mtos_malloc(sizeof(subt_clut_t));
    memset(*pp_clut, 0, sizeof(subt_clut_t));

    if(*pp_clut == NULL)
        return SUBT_RC_FAILED;

    if(p_subt->p_clut_list == NULL)
        (*pp_clut)->p_next = NULL;
    else
        (*pp_clut)->p_next = p_subt->p_clut_list;

    p_subt->p_clut_list = *pp_clut;

    return SUBT_RC_SUCCESS;
}

/*!
  delete the clut CDS in subtitle spec requirement
  */
subt_rc_t subt_db_delete_clut_vsb(dvb_subtitle_vsb_t *p_subt, u16 page_id, u8 clut_id)
{
    subt_clut_t *p_c = p_subt->p_clut_list;
    subt_clut_t *p_c_prev = NULL;

    while(p_c != NULL)
    {
        if(p_c->page_id == page_id && p_c->clut_id == clut_id)
        {
            if(p_c_prev == NULL)
                p_subt->p_clut_list = p_c->p_next;
            else
                p_c_prev->p_next = p_c->p_next;

            mtos_free(p_c);
            break;
        }

        p_c = p_c->p_next;
    }

  return SUBT_RC_SUCCESS;
}

#if 0
subt_rc_t subt_db_get_object(dvb_subtitle_vsb_t *p_subt
    , u16 page_id, u16 object_id, subt_object_t **pp_object)
{
  subt_object_t *p_o = p_subt->p_object_list;

  while(p_o != NULL)
  {
    if(p_o->page_id == page_id && p_o->id == object_id)
    {
      *pp_object = p_o;
      return SUBT_RC_SUCCESS;
    }

    p_o = p_o->p_next;
  }

  return SUBT_RC_FAILED;
}

subt_rc_t subt_db_create_object(dvb_subtitle_vsb_t *p_subt
    , subt_object_t **pp_object, u32 size)
{
  *pp_object = (subt_object_t *)mtos_malloc(size);

  if(*pp_object == NULL)
    return SUBT_RC_FAILED;

  (*pp_object)->p_object_data = NULL;

  if(p_subt->p_object_list == NULL)
    (*pp_object)->p_next = NULL;
  else
    (*pp_object)->p_next = p_subt->p_object_list;

  p_subt->p_object_list = *pp_object;

  return SUBT_RC_SUCCESS;
}

subt_rc_t subt_db_delete_object(dvb_subtitle_vsb_t *p_subt
    , u16 page_id, u16 object_id)
{
  subt_object_t *p_o = p_subt->p_object_list;
  subt_object_t *p_o_prev = NULL;

  while(p_o != NULL)
  {
    if(p_o->page_id == page_id && p_o->id == object_id)
    {
      if(p_o_prev == NULL)
        p_subt->p_object_list = p_o->p_next;
      else
        p_o_prev->p_next = p_o->p_next;

      mtos_free(p_o);
      break;
    }

    p_o = p_o->p_next;
  }

  return SUBT_RC_SUCCESS;
}
#endif
