/******************************************************************************/
/******************************************************************************/
/*!
   \file gui_script.c
   this file  implement the functions defined in  gui_script.h, also it implement some internal used
   function. All these functions are about how to open a menu by script data.
  */
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

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

#include "osd.h"
#include "gpe.h"
#include "surface.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_container.h"
#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"
#include "ctrl_progressbar.h"
#include "ctrl_matrixbox.h"
#include "ctrl_list.h"
#include "ctrl_combobox.h"
#include "ctrl_bitmap.h"
#include "ctrl_editbox.h"
#include "ctrl_common.h"
#include "ctrl_string.h"
#include "ctrl_numbox.h"

#include "framework.h"
#include "gui_script.h"

//#define SPT_DEBUG
#ifdef SPT_DEBUG
#define SPT_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define SPT_PRINTF(x)    do {} while(0)
#else
#define SPT_PRINTF
#endif
#endif

#ifdef SPT_SUPPORT

typedef void (*GET_PRIV_DATA)(u32 spt_data, u32 priv_attr, u32 priv_data);

static void _get_bmap_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_cbox_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_cont_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_ebox_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_list_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_mbox_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_nbox_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_pbar_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_sbar_data(u32 spt_data, u32 priv_attr, u32 priv_data);
static void _get_text_data(u32 spt_data, u32 priv_attr, u32 priv_data);

static GET_PRIV_DATA g_class_get_pdata_func[] =
{
  NULL,
  _get_bmap_data,
  _get_cbox_data,
  _get_cont_data,
  _get_ebox_data,
  _get_list_data,
  _get_mbox_data,
  _get_nbox_data,
  _get_pbar_data,
  _get_sbar_data,
  _get_text_data,
};

BOOL _get_class_info(u8 name_id, u8 **p_class_name, u8 *p_data_size);

static void _get_priv_data(u32 spt_data, u8 ctrl_type, u32 priv_attr, u32 priv_data);


#ifdef SPT_DEBUG
void dump_spt_ctrl(spt_ctrl_t *p_spt_ctrl)
{
  u8 *const class_name[] =
  {
    "ERROR",
    "BITMAP",
    "COMBOBOX",
    "CONTAINER",
    "EDITBOX",
    "LIST",
    "MATRIXBOX",
    "NUMBOX",
    "PROGRESSBAR",
    "SCROLLBAR",
    "TEXTFIELD"
  };  
  
  SPT_PRINTF("dump spt control .... \n");
  SPT_PRINTF("\t type = %d[%s]\n", p_spt_ctrl->type,
             class_name[p_spt_ctrl->type > 10 ? 0 : p_spt_ctrl->type]);
  SPT_PRINTF("\t attr = %d\n", p_spt_ctrl->attr);
  SPT_PRINTF("\t style = %d \n", p_spt_ctrl->style);
  SPT_PRINTF("\t id = %d \n", p_spt_ctrl->id);
  SPT_PRINTF("\t left = %d \n", p_spt_ctrl->left);
  SPT_PRINTF("\t up = %d \n", p_spt_ctrl->up);
  SPT_PRINTF("\t right = %d \n", p_spt_ctrl->right);
  SPT_PRINTF("\t down = %d \n", p_spt_ctrl->down);
  SPT_PRINTF("\t frame left = %d\n", p_spt_ctrl->frame_left);
  SPT_PRINTF("\t frame top = %d\n", p_spt_ctrl->frame_top);
  SPT_PRINTF("\t frame right = %d\n", p_spt_ctrl->frame_right);
  SPT_PRINTF("\t frame bottom = %d\n", p_spt_ctrl->frame_bottom);
  SPT_PRINTF("\t parent = %d\n", p_spt_ctrl->parent);
  SPT_PRINTF("\t child = %d\n", p_spt_ctrl->child);
  SPT_PRINTF("\t next = %d\n", p_spt_ctrl->next);
  SPT_PRINTF("\t prev = %d\n", p_spt_ctrl->prev);
  SPT_PRINTF("\t priv_attr = 0x%.8x\n", p_spt_ctrl->priv_attr);
  SPT_PRINTF("\t rstyle sh = %d\n", p_spt_ctrl->show_idx);
  SPT_PRINTF("\t rstyle hl = %d\n", p_spt_ctrl->hl_idx);
  SPT_PRINTF("\t rstyle gay = %d\n", p_spt_ctrl->gray_idx);
}


#endif

static void _get_bmap_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_bmap_data_t bin_data;
  ctrl_bmap_data_t *p_bmap_data = (ctrl_bmap_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_bmap_data_t));

  p_bmap_data->data = bin_data.data;
  p_bmap_data->left = bin_data.left;
  p_bmap_data->top = bin_data.top;
}


static void _get_cbox_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_cbox_static_data_t bin_static_data;
  spt_cbox_num_data_t bin_num_data;
  spt_cbox_dync_data_t bin_dync_data;
  ctrl_cbox_data_t *p_cbox_data = (ctrl_cbox_data_t *)priv_data;

  switch(priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      rsc_read_data(spt_data, &bin_static_data, sizeof(spt_cbox_static_data_t));
      p_cbox_data->str_left = bin_static_data.str_left;
      p_cbox_data->str_top = bin_static_data.str_top;
      p_cbox_data->n_fstyle = bin_static_data.n_fstyle;
      p_cbox_data->g_fstyle = bin_static_data.g_fstyle;
      p_cbox_data->h_fstyle = bin_static_data.h_fstyle;
      break;
    case CBOX_WORKMODE_NUMBER:
      rsc_read_data(spt_data, &bin_num_data, sizeof(spt_cbox_num_data_t));
      p_cbox_data->str_left = bin_num_data.str_left;
      p_cbox_data->str_top = bin_num_data.str_top;
      p_cbox_data->n_fstyle = bin_num_data.n_fstyle;
      p_cbox_data->g_fstyle = bin_num_data.g_fstyle;
      p_cbox_data->h_fstyle = bin_num_data.h_fstyle;
      p_cbox_data->num_data.bit_length = bin_num_data.bit_length;
      p_cbox_data->num_data.curn = bin_num_data.curn;
      p_cbox_data->num_data.max = bin_num_data.max;
      p_cbox_data->num_data.min = bin_num_data.min;
      p_cbox_data->num_data.postfix = bin_num_data.postfix;
      p_cbox_data->num_data.prefix = bin_num_data.prefix;
      p_cbox_data->num_data.step = bin_num_data.step;
      break;
    case CBOX_WORKMODE_DYNAMIC:
      rsc_read_data(spt_data, &bin_dync_data, sizeof(spt_cbox_dync_data_t));
      p_cbox_data->str_left = bin_dync_data.str_left;
      p_cbox_data->str_top = bin_dync_data.str_top;
      p_cbox_data->n_fstyle = bin_dync_data.n_fstyle;
      p_cbox_data->g_fstyle = bin_dync_data.g_fstyle;
      p_cbox_data->h_fstyle = bin_dync_data.h_fstyle;
      p_cbox_data->dync_data.common.total = bin_dync_data.total;
      break;
    default:
      MT_ASSERT(0);
  }
}


static void _get_cont_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  ctrl_cont_data_t *p_cont_data = (ctrl_cont_data_t *)priv_data;

  p_cont_data->p_clip_info = NULL;
}


static void _get_ebox_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_ebox_data_t bin_data;
  ctrl_ebox_data_t *p_ebox_data = (ctrl_ebox_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_ebox_data_t));

  p_ebox_data->str_l_space = bin_data.str_l_space;
  p_ebox_data->str_left = bin_data.str_left;
  p_ebox_data->str_top = bin_data.str_top;
  p_ebox_data->str_maxtext = bin_data.str_maxtext;
  p_ebox_data->n_fstyle = bin_data.n_color;
  p_ebox_data->g_fstyle = bin_data.g_color;
  p_ebox_data->h_fstyle = bin_data.h_color;
}


static void _get_list_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_list_data_t bin_data;
  ctrl_list_data_t *p_list_data = (ctrl_list_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_list_data_t));

  p_list_data->mid_rect.left = bin_data.mid_rect_left;
  p_list_data->mid_rect.top = bin_data.mid_rect_top;
  p_list_data->mid_rect.right = bin_data.mid_rect_right;
  p_list_data->mid_rect.bottom = bin_data.mid_rect_bottom;
  p_list_data->page = bin_data.page;
  p_list_data->interval = bin_data.interval;
  p_list_data->rstyle.f_xstyle = bin_data.item_f_idx;
  p_list_data->rstyle.g_xstyle = bin_data.item_g_idx;
  p_list_data->rstyle.n_xstyle = bin_data.item_n_idx;
  p_list_data->rstyle.sf_xstyle = bin_data.item_sf_idx;
  p_list_data->rstyle.s_xstyle = bin_data.item_s_idx;
}


static void _get_mbox_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_mbox_data_t bin_data;
  ctrl_mbox_data_t *p_mbox_data = (ctrl_mbox_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_mbox_data_t));

  p_mbox_data->mid_rect.left = bin_data.mid_rect_left;
  p_mbox_data->mid_rect.top = bin_data.mid_rect_top;
  p_mbox_data->mid_rect.right = bin_data.mid_rect_right;
  p_mbox_data->mid_rect.bottom = bin_data.mid_rect_bottom;
  p_mbox_data->col = bin_data.col;
  p_mbox_data->row = bin_data.row;
  p_mbox_data->h_interval = bin_data.h_interval;
  p_mbox_data->v_interval = bin_data.v_interval;
  p_mbox_data->str_left = bin_data.str_left;
  p_mbox_data->str_top = bin_data.str_top;
  p_mbox_data->icon_left = bin_data.icon_left;
  p_mbox_data->icon_top = bin_data.icon_top;
  p_mbox_data->item_f_idx = bin_data.item_f_idx;
  p_mbox_data->item_g_idx = bin_data.item_g_idx;
  p_mbox_data->item_n_idx = bin_data.item_n_idx;
  p_mbox_data->f_fstyle = bin_data.f_color;
  p_mbox_data->n_fstyle = bin_data.n_color;
  p_mbox_data->g_fstyle = bin_data.g_color;
}


static void _get_nbox_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_nbox_data_t bin_data;
  ctrl_nbox_data_t *p_nbox_data = (ctrl_nbox_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_nbox_data_t));

  p_nbox_data->num = bin_data.num;
  p_nbox_data->max = bin_data.max;
  p_nbox_data->min = bin_data.min;
  p_nbox_data->curn_bit = bin_data.curn_bit;
  p_nbox_data->bit_length = bin_data.bit_length;
  p_nbox_data->n_fstyle = bin_data.n_color;
  p_nbox_data->h_fstyle = bin_data.h_color;
  p_nbox_data->g_fstyle = bin_data.g_color;
  p_nbox_data->str_left = bin_data.str_left;
  p_nbox_data->str_top = bin_data.str_top;
}


static void _get_pbar_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_pbar_data_t bin_data;
  ctrl_pbar_data_t *p_pbar_data = (ctrl_pbar_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_pbar_data_t));

  p_pbar_data->mid_rect.left = bin_data.mid_rect_left;
  p_pbar_data->mid_rect.top = bin_data.mid_rect_top;
  p_pbar_data->mid_rect.right = bin_data.mid_rect_right;
  p_pbar_data->mid_rect.bottom = bin_data.mid_rect_bottom;
  p_pbar_data->min = bin_data.min;
  p_pbar_data->max = bin_data.max;
  p_pbar_data->curn = bin_data.curn;
  p_pbar_data->step = bin_data.step;
  p_pbar_data->interval = bin_data.interval;
  p_pbar_data->max_rstyle = bin_data.max_color;
  p_pbar_data->min_rstyle = bin_data.min_color;
  p_pbar_data->cur_rstyle = bin_data.cur_color;
}


static void _get_sbar_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_sbar_data_t bin_data;
  ctrl_sbar_data_t *p_sbar_data = (ctrl_sbar_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_sbar_data_t));

  p_sbar_data->mid_rect.left = bin_data.mid_rect_left;
  p_sbar_data->mid_rect.top = bin_data.mid_rect_top;
  p_sbar_data->mid_rect.right = bin_data.mid_rect_right;
  p_sbar_data->mid_rect.bottom = bin_data.mid_rect_bottom;
  p_sbar_data->n_mid_rstyle = bin_data.n_mid_color;
  p_sbar_data->g_mid_rstyle = bin_data.g_mid_color;
  p_sbar_data->h_mid_rstyle = bin_data.h_mid_color;
  p_sbar_data->page = bin_data.page;
  p_sbar_data->total = bin_data.total;
}


static void _get_text_data(u32 spt_data, u32 priv_attr, u32 priv_data)
{
  spt_text_data_t bin_data;
  ctrl_text_data_t *p_text_data = (ctrl_text_data_t *)priv_data;

  rsc_read_data(spt_data, &bin_data, sizeof(spt_text_data_t));

  p_text_data->str_char = bin_data.str_char;
  p_text_data->n_fstyle = bin_data.n_fstyle;
  p_text_data->h_fstyle = bin_data.h_fstyle;
  p_text_data->g_fstyle = bin_data.g_fstyle;
  p_text_data->str_l_space = bin_data.str_l_space;
  p_text_data->str_left = bin_data.str_left;
  p_text_data->str_top = bin_data.str_top;
}


BOOL _get_class_info(u8 name_id, u8 **p_class_name, u8 *p_data_size)
{
  u8 class_pdata_size[] =
  {
    0,
    sizeof(ctrl_bmap_data_t),
    sizeof(ctrl_cbox_data_t),
    sizeof(ctrl_cont_data_t),
    sizeof(ctrl_ebox_data_t),
    sizeof(ctrl_list_data_t),
    sizeof(ctrl_mbox_data_t),
    sizeof(ctrl_nbox_data_t),
    sizeof(ctrl_pbar_data_t),
    sizeof(ctrl_sbar_data_t),
    sizeof(ctrl_text_data_t),
  };

  u8 *const class_name[] =
  {
    "ERROR",
    "BITMAP",
    "COMBOBOX",
    "CONTAINER",
    "EDITBOX",
    "LIST",
    "MATRIXBOX",
    "NUMBOX",
    "PROGRESSBAR",
    "SCROLLBAR",
    "TEXTFIELD"
  };  

  if(name_id > TYPE_TEXT)
  {
    SPT_PRINTF("_get_class_info: name_id(%d) > TYPE_TEXT, ERROR!\n", name_id);
    *p_class_name = NULL;
    *p_data_size = 0;
    return FALSE;
  }

  *p_class_name = class_name[name_id];
  *p_data_size = class_pdata_size[name_id];

  return TRUE;
}


//parameter: script data addr, control type,
//control private attribe, control private data addr
static void _get_priv_data(u32 spt_data, u8 ctrl_type, u32 priv_attr, u32 priv_data)
{
  if(ctrl_type > TYPE_TEXT)
  {
    SPT_PRINTF("_get_priv_data: ctrl type(%d) is UNSUPPORT, ERROR!\n",
               ctrl_type);
    MT_ASSERT(0);
    return;
  }

  (*g_class_get_pdata_func[ctrl_type])(spt_data, priv_attr, priv_data);
}


control_t *spt_create_ctrl(spt_ctrl_t *p_spt_ctrl, control_t *p_parent, u32 priv_data)
{
  control_class_t *oci;
  rect_t rc1;
  control_t *new_ctrl;
  u8 *class_name;
  u8 size;

  new_ctrl = (control_t *)lib_memf_alloc(ctrl_get_ctrl_heap());
  if(NULL == new_ctrl)
  {
    SPT_PRINTF("spt_create_ctrl: can NOT allocate buffer, ERROR!\n");
    return NULL;
  }

  // get class info
  _get_class_info(p_spt_ctrl->type, &class_name, &size);
  if((oci = ctrl_get_ctrl_class(class_name)) == NULL)
  {
    SPT_PRINTF("spt_create_ctrl: can NOT get class(%s), ERROR!\n", class_name);
    return NULL;
  }

  memcpy(new_ctrl, oci->p_default_ctrl, sizeof(control_t));

  new_ctrl->p_oci = oci;
  new_ctrl->attr = p_spt_ctrl->attr;
  new_ctrl->style = p_spt_ctrl->style;
  new_ctrl->id = p_spt_ctrl->id;
  new_ctrl->up = p_spt_ctrl->up;
  new_ctrl->left = p_spt_ctrl->left;
  new_ctrl->right = p_spt_ctrl->right;
  new_ctrl->down = p_spt_ctrl->down;
  new_ctrl->frame.left = p_spt_ctrl->frame_left;
  new_ctrl->frame.top = p_spt_ctrl->frame_top;
  new_ctrl->frame.right = p_spt_ctrl->frame_right;
  new_ctrl->frame.bottom = p_spt_ctrl->frame_bottom;
  new_ctrl->rstyle.gray_idx = p_spt_ctrl->gray_idx;
  new_ctrl->rstyle.hl_idx = p_spt_ctrl->hl_idx;
  new_ctrl->rstyle.show_idx = p_spt_ctrl->show_idx;
  new_ctrl->priv_attr = p_spt_ctrl->priv_attr;

  ctrl_add_child(p_parent, new_ctrl);

  if(SUCCESS != ctrl_process_msg(new_ctrl, MSG_CREATE, (u32)p_parent, 0))
  {
    SPT_PRINTF(
      "spt_create_ctrl: can NOT allocate private data(%s), ERROR!\n",
      class_name);
    ctrl_remove_child(p_parent, new_ctrl);
    lib_memf_free(ctrl_get_ctrl_heap(), new_ctrl);
    return NULL;
  }
  else
  {
#ifdef MUTI_THREAD_SUPPORT
    os_mutex_create(&new_ctrl->invrgn_info.lock);
#endif
    /* load private data */
    _get_priv_data(
      priv_data, p_spt_ctrl->type, new_ctrl->priv_attr, new_ctrl->priv_data);

    ctrl_init_invrgn(new_ctrl);

    return new_ctrl;
  }
}


control_t *spt_load_menu(u8 root_id)
{
  spt_ctrl_t spt_ctrl;
  control_t *cur_ctrl = NULL;
  rsc_script_t hdr_spt;
  u16 offset = 0;
  u8 *bits = NULL;
  u32 addr = 0;

  SPT_PRINTF("spt load menu: id = %d\n", root_id);
  if((cur_ctrl = fw_find_root_by_id(root_id)) != NULL)
  {
    SPT_PRINTF(
      "spt_load_menu: the menu (id=%d) is already existed, ERROR!\n",
      root_id);
    return cur_ctrl;
  }
  //get the start address of menu data.
  if(!rsc_get_script(root_id, &hdr_spt, (u8 **)&bits))
  {
    SPT_PRINTF(
      "spt_load_menu: can NOT get the menu (id=%d), ERROR!\n",
      root_id);
    return NULL;
  }
  addr = (u32)bits;
  //read spt control from flash/sram.
  rsc_read_data(addr, &spt_ctrl, sizeof(spt_ctrl_t));
  SPT_PRINTF("load_menu: read flash addr[0x%x], len[0x%x], mem_addr[0x%x]\n",
             addr, sizeof(spt_ctrl_t), &spt_ctrl);
#ifdef SPT_DEBUG
  dump_spt_ctrl(&spt_ctrl);
#endif
  cur_ctrl = spt_create_ctrl(&spt_ctrl, cur_ctrl, addr + sizeof(spt_ctrl_t));
#ifdef SPT_DEBUG
  if(cur_ctrl == NULL)
  {
    OS_PRINTF("spt_load_menu: spt_create_ctrl is failed, ERROR!\n");
    dump_spt_ctrl(&spt_ctrl);
  }
#endif
  MT_ASSERT(cur_ctrl != NULL);

  offset = spt_ctrl.child;
  while(1)
  {
    if(offset != 0)
    {
      rsc_read_data(offset + addr, &spt_ctrl, sizeof(spt_ctrl_t));
#ifdef SPT_DEBUG
      dump_spt_ctrl(&spt_ctrl);
#endif

      cur_ctrl = spt_create_ctrl(
        &spt_ctrl, cur_ctrl, offset + addr + sizeof(spt_ctrl_t));
      offset = (u16)(spt_ctrl.child);
    }
    else
    {
      if(spt_ctrl.next != 0)
      {
        offset = (u16)(spt_ctrl.next);
        cur_ctrl = cur_ctrl->p_parent;
      }
      else
      {
        if(spt_ctrl.parent != 0)
        {
          offset = spt_ctrl.parent;
          rsc_read_data(offset + addr, &spt_ctrl, sizeof(spt_ctrl_t));
#ifdef SPT_DEBUG
          dump_spt_ctrl(&spt_ctrl);
#endif

          cur_ctrl = cur_ctrl->p_parent;
          offset = spt_ctrl.next;
          if(offset != 0)
          {
            cur_ctrl = cur_ctrl->p_parent;
          }
        }
        else
        {
          return desktop_attach_root(cur_ctrl->p_parent, 0);
        }
      }
    }
  }
  return 0;
}


#endif
