/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_textfield.c
   this file  implement the functions defined in  ctrl_textfield.h, also it
   implement some internal used   function. All these functions are about how to
   decribe, set and draw a pbar control.
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
#include "gui_roll.h"

#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"

/*!
   page infor
  */
typedef struct page_info
{
  /*!
     the address of string in current page
    */
  u32 str_addr;
  /*!
     the previous page infor
    */
  struct page_info *p_prev;
  /*!
     the next page infor
    */
  struct page_info *p_next;
} page_info_t;

static u32 *_text_get_content_addr(control_t *p_ctrl, u32 type)
{
  ctrl_text_data_t *p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  return (u32 *)&p_data->str_char;
}


static void _text_set_static_content(control_t *p_ctrl, u32 type, u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _text_get_content_addr(p_ctrl, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = (u32)val;
}


static u8 _text_get_fstyle(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;
  u8 font = 0;
  u8 attr = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }
  
  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_data->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_data->g_fstyle;
      break;

    default:
      font = p_data->n_fstyle;
  }

  return font;
}

static void _text_get_draw_rect(control_t *p_ctrl, rect_t *p_rc)
{
  ctrl_text_data_t *p_data = NULL;
  rect_t client_rect = {0};
  
  MT_ASSERT(p_ctrl != NULL);

  ctrl_get_client_rect(p_ctrl, &client_rect);
  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  
  p_rc->left = client_rect.left + p_data->mid_rect.left;
  p_rc->top = client_rect.top + p_data->mid_rect.top;
  p_rc->right = p_rc->left + RECTW(p_data->mid_rect);
  p_rc->bottom = p_rc->top + RECTH(p_data->mid_rect);
}

static u16 *_text_get_str_buf(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;
  u16 *p_str_buf = NULL;
  static u16 str_num[NUM_STRING_LEN + 1];
  
  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  switch(p_ctrl->priv_attr & TEXT_STRTYPE_MASK)
  {
    case TEXT_STRTYPE_STRID:
      p_str_buf = (u16 *)gui_get_string_addr((u16)p_data->str_char);
      break;
      
    case TEXT_STRTYPE_UNICODE:
      p_str_buf = (u16 *)p_data->str_char;
      break;
      
    case TEXT_STRTYPE_EXTSTR:
      p_str_buf = (u16 *)p_data->str_char;
      
      if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
      {
        if(p_data->p_line_addr != NULL)
        {
          p_str_buf = (u16 *)*(p_data->p_line_addr + p_data->curn);
        }
      }
      break;
      
    case TEXT_STRTYPE_DEC:
      convert_i_to_dec_str(str_num, p_data->str_char);
      p_str_buf = str_num;
      break;
      
    case TEXT_STRTYPE_HEX:
      convert_i_to_hex_str(str_num, p_data->str_char);
      p_str_buf = str_num;
      break;
      
    default:
      p_str_buf = NULL;
      break;
  }

  return p_str_buf;
}

static void _text_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_text_data_t *p_data = NULL;
  u8 font = 0;
  rect_t str_rect;
  u16 *p_str_buf = NULL;

  if(ctrl_is_rolling(p_ctrl))
  {
    return;
  }

  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  font = _text_get_fstyle(p_ctrl);
  
  _text_get_draw_rect(p_ctrl, &str_rect);

  p_str_buf = _text_get_str_buf(p_ctrl);

  p_data->status =
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                    p_data->str_left, p_data->str_top, p_data->str_l_space,
                    p_str_buf, font, p_data->draw_style);
}


static void _text_free_ctrl(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_UNICODE)
  {
    if(p_data->str_char != 0)
    {
      ctrl_unistr_free((void *)p_data->str_char);
      p_data->str_char = 0;
    }
  }

  //page mode, should free line addr buffer.
  if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
  {
    if(p_data->p_line_addr != NULL)
    {
      mmi_free_buf(p_data->p_line_addr);
      p_data->p_line_addr = NULL;
    }
  }
}

static void _text_draw_scroll_bar(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {  
      if(p_data->p_scroll_bar != NULL)
      {
        ctrl_paint_ctrl(p_data->p_scroll_bar, TRUE);
      }
    }
  }
}

RET_CODE text_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_text_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of textfield class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER |
                              TEXT_STRTYPE_STRID;
  p_default_ctrl->p_proc = text_class_proc;
  p_default_ctrl->p_paint = _text_draw;

  // initalize the default data of textfield class
  
  if(ctrl_register_ctrl_class((u8 *)CTRL_TEXT, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void text_set_content_by_ascstr(control_t *p_ctrl, u8 *p_ascstr)
{
  u16 **p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = (u16 **)_text_get_content_addr(p_ctrl, TEXT_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp = (u16 *)ctrl_unistr_realloc((void *)(*p_temp), strlen((char *)p_ascstr));
  if(*p_temp == NULL)
  {
    return;
  }
  str_asc2uni(p_ascstr, (u16 *)(*p_temp));
}


void text_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr)
{
  u16 **p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = (u16 **)_text_get_content_addr(p_ctrl, TEXT_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp =
    (u16 *)ctrl_unistr_realloc((void *)(*p_temp), uni_strlen(p_unistr));
  if(*p_temp == NULL)
  {
    return;
  }
  uni_strcpy((u16 *)(*p_temp), p_unistr);
}


void text_set_content_by_strid(control_t *p_ctrl, u16 str_id)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_STRID, str_id);
}


void text_set_content_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_EXTSTR, (u32)p_extstr);
}


void text_set_content_by_dec(control_t *p_ctrl, s32 dec)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_DEC, dec);
}


void text_set_content_by_hex(control_t *p_ctrl, s32 hex)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_HEX, hex);
}


u32 text_get_content(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  return p_data->str_char;
}


u32 *text_get_content_buf(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  return &p_data->str_char;
}


u32 text_get_status(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  return p_data->status;
}


void text_set_font_style(control_t *p_ctrl,
                         u8 n_fstyle,
                         u8 h_fstyle,
                         u8 g_fstyle)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  p_data->n_fstyle = n_fstyle;
  p_data->h_fstyle = h_fstyle;
  p_data->g_fstyle = g_fstyle;
}


void text_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  p_data->str_left = left;
  p_data->str_top = top;
}

void text_set_mid_rect(control_t *p_ctrl,
  s16 left, s16 top, u16 width, u16 height)
{
  ctrl_text_data_t *p_data = NULL;
  rect_t frame;

  MT_ASSERT(p_ctrl != NULL);

  ctrl_get_frame(p_ctrl, &frame);

  MT_ASSERT((left + width) <= RECTW(frame));
  MT_ASSERT((top + height) <= RECTH(frame));

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  set_rect(&p_data->mid_rect, left, top, (s16)(left + width),
           (s16)(top + height));
}

void text_set_line_gap(control_t *p_ctrl, u8 l_space)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  p_data->str_l_space = l_space;
}


void text_enable_page(control_t *p_ctrl, BOOL enable)
{
  MT_ASSERT(p_ctrl != NULL);

  //only support page mode when string type is external string.
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != TEXT_STRTYPE_EXTSTR)
  {
    return;
  }

  if(enable)
  {
    p_ctrl->priv_attr |= TEXT_PAGE_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~TEXT_PAGE_MASK);
  }
}

static u16 text_reset_page(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;
  rect_t str_rect = {0};
  u32 font = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  if(p_data->curn >= p_data->total)
  {
    return 0;
  }
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      font = _text_get_fstyle(p_ctrl);
      
      _text_get_draw_rect(p_ctrl, &str_rect);

      if((u16 *)(p_data->p_line_addr + p_data->curn) == NULL)
      {
        return 0;
      }      

      return gui_get_lines_per_page(font, p_ctrl->priv_attr, &str_rect,
        (u16 *)(*(p_data->p_line_addr + p_data->curn)), p_data->str_left, p_data->str_top,
        p_data->str_l_space, p_data->draw_style);      
    }
  }

  return 0;
}

void text_reset_param(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;
  rect_t str_rect = {0};
  u16 *p_str_buf = NULL;
  u16 i = 0;
  u8 font = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      font = _text_get_fstyle(p_ctrl);
      
      _text_get_draw_rect(p_ctrl, &str_rect);

      //caculate total line.
      p_data->total = gui_get_unistr_total_lines(font, p_ctrl->priv_attr, &str_rect,
        (u16 *)p_data->str_char, p_data->str_left, p_data->draw_style);

      if(p_data->total != 0)
      {
        p_data->p_line_addr = mmi_realloc_buf(p_data->p_line_addr, sizeof(u32) * p_data->total);
      }
      else
      {
        if(p_data->p_line_addr != NULL)
        {
          mmi_free_buf(p_data->p_line_addr);
          p_data->p_line_addr = NULL;
        }
      }

      //caculate each line start.
      p_str_buf = (u16 *)p_data->str_char;

      for(i = 0; i < p_data->total; i++)
      {
        *(p_data->p_line_addr + i) = (u32)p_str_buf;
        p_str_buf = gui_get_next_line(font, p_ctrl->priv_attr, &str_rect,
          p_str_buf, p_data->str_left, p_data->draw_style);
      }

      //caculate page line.
      p_data->page = text_reset_page(p_ctrl);
      
      p_data->curn = 0;
      if(p_data->p_scroll_bar != NULL)
	    {
        sbar_set_count(p_data->p_scroll_bar, p_data->page, p_data->total);
        sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
      }
    }
  }
}

void text_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {  
      p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
      p_data->p_scroll_bar = p_ctrl_bar;

      if(p_ctrl_bar != NULL)
      {
        sbar_set_count(p_ctrl_bar, p_data->page, p_data->total);
        sbar_set_current_pos(p_ctrl_bar, p_data->curn);
      }
    }
  }
}


void text_set_breakword(control_t *p_ctrl, BOOL enable)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;
  p_data->draw_style = enable ? STRDRAW_BREAK_WORD : STRDRAW_NORMAL;
}


u32 text_get_breakword(control_t *p_ctrl)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  return p_data->draw_style;
}


void text_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TEXT_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & TEXT_ALIGN_MASK); //set new style
}


void text_set_content_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TEXT_STRTYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TEXT_STRTYPE_MASK);    //set new type
}

static RET_CODE on_text_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t parent_dc = (hdc_t)(para2);

  ctrl_default_proc(p_ctrl, msg, para1, para2);

  //this bar isn't text's child, when parent_dc isn't 0,
  //parent control will draw bar
  if(parent_dc == HDC_INVALID)
  {
    _text_draw_scroll_bar(p_ctrl);
  }

  return SUCCESS;
}

static RET_CODE on_text_increase(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

      if(p_data->curn > 0)
      {
        p_data->curn--;

        p_data->page = text_reset_page(p_ctrl);
		
        if(p_data->p_scroll_bar != NULL)
        {
          sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
        }

        ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
      }  
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_decrease(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

      if(p_data->total > p_data->page)
      {
        if(p_data->curn < (p_data->total - p_data->page))
        {
          p_data->curn++;
		  
          p_data->page = text_reset_page(p_ctrl);

          if(p_data->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
          }

          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_page_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

      if(p_data->curn >= p_data->page)
      {
        p_data->curn -= p_data->page;
		
        p_data->page = text_reset_page(p_ctrl);

        if(p_data->p_scroll_bar != NULL)
        {
          sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
        }

        ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
      }
      else 
      {
        if(p_data->curn != 0)
        {
          p_data->curn = 0;
		  
          p_data->page = text_reset_page(p_ctrl);

          if(p_data->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
          }
          
          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_page_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

      if(p_data->total > p_data->page)
      {
        if((p_data->curn + p_data->page) < p_data->total)
        {
          p_data->curn += p_data->page;
		  
          p_data->page = text_reset_page(p_ctrl);

          if(p_data->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_data->p_scroll_bar, p_data->curn);
          }

          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}


static RET_CODE on_text_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _text_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}

static RET_CODE on_text_created(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  rect_t frame = {0};
  MT_ASSERT(p_ctrl != NULL);

  ctrl_get_frame(p_ctrl, &frame);
  
  text_set_mid_rect(p_ctrl, 0, 0, RECTW(frame), RECTH(frame));

  return ERR_NOFEATURE;
}

static RET_CODE on_text_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_data_t *p_data = NULL;
  u8 font = 0;
  rect_t src_rect;
  u16 *p_str_buf = NULL;
    
  p_data = (ctrl_text_data_t *)p_ctrl->priv_data;

  _text_get_draw_rect(p_ctrl, &src_rect);

  font = _text_get_fstyle(p_ctrl);

  p_str_buf = _text_get_str_buf(p_ctrl);
  
  gui_create_rsurf(p_ctrl, font, p_str_buf, &src_rect,
    p_data->draw_style, (roll_param_t *)para1, 0);
  return SUCCESS;;
}

static RET_CODE on_text_rolling(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t hdc = 0;  
  roll_node_t *p_node = NULL;

  p_node = (roll_node_t *)para1;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_node != NULL);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  hdc = gui_begin_paint(p_ctrl, 0);
  gui_paint_frame(hdc, p_ctrl);  

  gui_rolling_node(p_node, hdc);

  gui_end_paint(p_ctrl, hdc);  
  
  return SUCCESS;;
}

// define the default msgmap of class
BEGIN_CTRLPROC(text_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_PAINT, on_text_paint)
  ON_COMMAND(MSG_INCREASE, on_text_increase)
  ON_COMMAND(MSG_DECREASE, on_text_decrease)
  ON_COMMAND(MSG_DESTROY, on_text_destroy)
  ON_COMMAND(MSG_CREATED, on_text_created)
  ON_COMMAND(MSG_PAGE_DOWN, on_text_page_down)
  ON_COMMAND(MSG_PAGE_UP, on_text_page_up)
  ON_COMMAND(MSG_START_ROLL, on_text_roll_start)
  ON_COMMAND(MSG_ROLLING, on_text_rolling)
END_CTRLPROC(text_class_proc, ctrl_default_proc)
