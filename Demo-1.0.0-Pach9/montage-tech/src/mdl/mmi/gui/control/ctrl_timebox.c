/******************************************************************************/
/******************************************************************************/
/*!
   \file ctrl_numbox.c
   this file  implement the functions defined in  ctrl_numbox.h, also it
   implement some internal used   function. All these functions are about how to
   decribe, set and draw a numbox control.
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
#include "lib_util.h"
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
#include "mt_time.h"
#include "ctrl_timebox.h"

/*!
  Macro of NOT
  */
#define NOT(x) ((x) == 0)
/*!
  Macro of noninline
  */
#if defined(NXP)
/*!
  Macro for noninline function:no need in NXP
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#elif defined(WIN32)
/*!
  Macro for noninline function:no need in x86
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#else
/*!
  Macro for noninline function
  */
#define NONINLINE __attribute__ ((noinline))
#endif


/*!
   get base of a certain bit.

   \param[in] bits			: to specified a bit.
   \return					: base of the specified bit.
  */
static NONINLINE u32 _tbox_get_base(u32 bits)
{
  u32 i = 0, base = 1;

  for(i = 0; i < bits; i++)
  {
    base = base * 10;
  }
  return base;
}


/*!
   get the number of current bit.

   \param[in] p_data			: private data of nbox.
   \return					: the number of current bit.
  */
static u8 _tbox_get_curn_bit(control_t *p_ctrl);

/*!
   set the number of current bit.

   \param[in] p_data			: private data of nbox.
   \param[in] num			: number to be set.
   \return					: the number of current bit.
  */
static void _tbox_set_curn_bit(control_t *p_ctrl, u8 num);

/*!
   increase the number of current bit.

   \param[in] ctrl				: nbox control.
   \return					: TRUE for success, else return FALSE.
  */
static BOOL _num_increase(control_t *p_ctrl);

/*!
   decrease the number of current bit.

   \param[in] ctrl				: nbox control.
   \return					: TRUE for success, else return FALSE.
  */
static BOOL _num_decrease(control_t *p_ctrl);

static void _tbox_draw(control_t *p_ctrl, hdc_t hdc);

static u16 _tbox_get_total_width(control_t *p_ctrl);

static u8 _tbox_get_fstyle(u8 attr, ctrl_tbox_data_t *p_data);

RET_CODE tbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_tbox_data_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;

  // initialize the default control of timebox class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER |
                              TBOX_YEAR | TBOX_MONTH | TBOX_DAY;
  p_default_ctrl->p_proc = tbox_class_proc;
  p_default_ctrl->p_paint = _tbox_draw;

  // initalize the default data of timebox class

  if(ctrl_register_ctrl_class((u8 *)CTRL_TBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static u32 _get_draw_style(ctrl_tbox_data_t *p_data, u8 item_index)
{
  if(item_index == p_data->curn_item)
  {
    return MAKE_DRAW_STYLE(STRDRAW_WITH_UNDERLINE,
                           p_data->curn_bit);
  }
  else
  {
    return MAKE_DRAW_STYLE(STRDRAW_NORMAL,
                           0);
  }
}


static u8 _tbox_get_item_bit(control_t *p_ctrl, u8 index)
{
  u32 tbox_attr[TBOX_MAX_ITEM_NUM] =
  {
    TBOX_YEAR, TBOX_MONTH, TBOX_DAY,
    TBOX_HOUR, TBOX_MIN, TBOX_SECOND
  };
  u8 item_bit = 0;

  if(p_ctrl->priv_attr & tbox_attr[index])
  {
    item_bit = ((index == 0) ? 4 : 2);
  }

  return item_bit;
}


static s32 _tbox_shift_offset(control_t *p_ctrl, s32 offset)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  if(offset > 0) //move left
  {
    if(offset > p_data->curn_bit) //out of curn item
    {
      offset -= p_data->curn_bit;

      do
      {
        if(p_data->curn_item == 0)
        {
          p_data->curn_item = (TBOX_MAX_ITEM_NUM - 1);
        }
        else
        {
          p_data->curn_item--;
        }
      }
      while(_tbox_get_item_bit(p_ctrl, p_data->curn_item) == 0);


      p_data->curn_bit = (_tbox_get_item_bit(p_ctrl, p_data->curn_item) - 1);

      (offset--);

      return offset;
    }
    else //move in curn item
    {
      p_data->curn_bit -= offset;

      return 0;
    }
  }
  else if(offset < 0)
  {
    offset = (0 - offset);

    if(offset > (_tbox_get_item_bit(p_ctrl, p_data->curn_item)
                 - (p_data->curn_bit + 1))) //out of current item.
    {
      offset -= (_tbox_get_item_bit(p_ctrl, p_data->curn_item)
                 - (p_data->curn_bit + 1));

      do
      {
        if(p_data->curn_item == (TBOX_MAX_ITEM_NUM - 1))
        {
          p_data->curn_item = 0;
        }
        else
        {
          p_data->curn_item++;
        }
      }
      while(_tbox_get_item_bit(p_ctrl, p_data->curn_item) == 0);

      p_data->curn_bit = 0;

      (offset--);

      return 0 - offset;
    }
    else
    {
      p_data->curn_bit += offset;

      return 0;
    }
  }
  return 0;
}


static u8 _tbox_get_curn_bit(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;
  u16 data = 0;

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  switch(p_data->curn_item)
  {
    case 0: //year
      data = p_data->ctrl_time.year;
      break;
    case 1: //month
      data = p_data->ctrl_time.month;
      break;
    case 2: //day
      data = p_data->ctrl_time.day;
      break;
    case 3: //hour
      data = p_data->ctrl_time.hour;
      break;
    case 4: //min
      data = p_data->ctrl_time.minute;
      break;
    case 5: //sec
      data = p_data->ctrl_time.second;
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  return (u8)(data %
              _tbox_get_base(_tbox_get_item_bit(p_ctrl, p_data->curn_item)
                             - p_data->curn_bit) /
              _tbox_get_base(_tbox_get_item_bit(p_ctrl, p_data->curn_item) -
                             (p_data->curn_bit + 1)));
}


static void _tbox_set_curn_bit(control_t *p_ctrl, u8 num)
{
  ctrl_tbox_data_t *p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  u32 base = _tbox_get_base(
    _tbox_get_item_bit(p_ctrl, p_data->curn_item) - (p_data->curn_bit + 1));

  switch(p_data->curn_item)
  {
    case 0: //year
      p_data->ctrl_time.year -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.year += num * base;
      break;
    case 1: //month
      p_data->ctrl_time.month -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.month += num * base;
      break;
    case 2: //day
      p_data->ctrl_time.day -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.day += num * base;
      break;
    case 3: //hour
      p_data->ctrl_time.hour -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.hour += num * base;
      break;
    case 4: //min
      p_data->ctrl_time.minute -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.minute += num * base;
      break;
    case 5: //sec
      p_data->ctrl_time.second -= _tbox_get_curn_bit(p_ctrl) * base;
      p_data->ctrl_time.second += num * base;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}


static u16 _tbox_get_total_width(control_t *p_ctrl)
{
  u8 attr = 0, font = 0;
  ctrl_tbox_data_t *p_data = NULL;
  u16 num[TBOX_MAX_ITEM_NUM];
  u16 total_width = 0;
  rsc_bitmap_t hdr_bmp;
  u8 *p_bmp_data = NULL;
  u8 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  //draw string
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _tbox_get_fstyle(attr, p_data);

  num[0] = p_data->ctrl_time.year;
  num[1] = p_data->ctrl_time.month;
  num[2] = p_data->ctrl_time.day;
  num[3] = p_data->ctrl_time.hour;
  num[4] = p_data->ctrl_time.minute;
  num[5] = p_data->ctrl_time.second;

  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    if(_tbox_get_item_bit(p_ctrl, i) != 0)
    {
      total_width +=
        (p_data->max_num_width * _tbox_get_item_bit(p_ctrl, i));

      switch(p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK)
      {
        case TBOX_SEPARATOR_TYPE_STRID:
          total_width += rsc_get_strid_width(gui_get_rsc_handle(), (u16)p_data->separator[i], font);
          break;
        case TBOX_SEPARATOR_TYPE_EXTSTR:
          total_width +=
            rsc_get_unistr_width(gui_get_rsc_handle(), (u16 *)(p_data->separator[i]), font);
          break;
        case TBOX_SEPARATOR_TYPE_BMPID:
          rsc_get_bmp(gui_get_rsc_handle(), (u16)p_data->separator[i], &hdr_bmp, &p_bmp_data);
          total_width += hdr_bmp.width;
          break;
      }
    }
  }

  return total_width;
}


void tbox_set_time_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_TIME_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TBOX_TIME_MASK);    //set new type
}


void tbox_set_separator_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_SEPARATOR_TYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TBOX_SEPARATOR_TYPE_MASK);    //set new type
}


BOOL tbox_set_separator_by_strid(control_t *p_ctrl, u8 index, u16 strid)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_STRID)
  {
    return FALSE;
  }

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->separator[index] = strid;

  return TRUE;
}


BOOL tbox_set_separator_by_extstr(control_t *p_ctrl, u8 index, u16 *p_str)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_EXTSTR)
  {
    return FALSE;
  }
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->separator[index] = (u32)p_str;

  return TRUE;
}


BOOL tbox_set_separator_by_ascchar(control_t *p_ctrl, u8 index, u8 asc_char)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  p_data->separator[index] = (u32)asc_char;

  return TRUE;
}


BOOL tbox_set_separator_by_unichar(control_t *p_ctrl, u8 index, u16 uni_char)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  p_data->separator[index] = (u32)uni_char;

  return TRUE;
}


BOOL tbox_set_separator_by_bmpid(control_t *p_ctrl, u8 index, u16 bmpid)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_BMPID)
  {
    return FALSE;
  }

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->separator[index] = bmpid;

  return TRUE;
}


u32 tbox_get_separator(control_t *p_ctrl, u8 index)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  return p_data->separator[index];
}


void tbox_set_time(control_t *p_ctrl, utc_time_t *p_time)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  //p_data->year = time.y;
  //p_data->month = time.mon;
  //p_data->day = time.d;
  //p_data->hour = time.h;
  //p_data->minute = time.min;
  //p_data->second = time.sec;
  memcpy(&(p_data->ctrl_time), p_time, sizeof(utc_time_t));
}


void tbox_get_time(control_t *p_ctrl, utc_time_t *p_time)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  //get year
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) != 0)
  {
    p_time->year = p_data->ctrl_time.year;
  }

  //get month
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MONTH) != 0)
  {
    p_time->month = p_data->ctrl_time.month;
  }

  //get day
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_DAY) != 0)
  {
    p_time->day = p_data->ctrl_time.day;
  }

  //get hour
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_HOUR) != 0)
  {
    p_time->hour = p_data->ctrl_time.hour;
  }

  //get minute
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MIN) != 0)
  {
    p_time->minute = p_data->ctrl_time.minute;
  }

  //get second
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_SECOND) != 0)
  {
    p_time->second = p_data->ctrl_time.second;
  }
}


void tbox_set_font_style(control_t *p_ctrl, u8 n_fstyle, u8 h_fstyle, u8 g_fstyle)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->n_fstyle = n_fstyle;
  p_data->h_fstyle = h_fstyle;
  p_data->g_fstyle = g_fstyle;
}


void tbox_set_max_num_width(control_t *p_ctrl, u8 width)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->max_num_width = width;
}


u8 tbox_get_max_num_width(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  return p_data->max_num_width;
}


void tbox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_data->str_left = left;
  p_data->str_top = top;
}


void tbox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & TBOX_ALIGN_MASK); //set new style
}


static u8 _tbox_get_fstyle(u8 attr, ctrl_tbox_data_t *p_data)
{
  u8 font = 0;

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


static void _tbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_tbox_data_t *p_data = NULL;
  u8 attr = 0, font = 0;
  u16 str_num[NUM_STRING_LEN + 1];
  rect_t str_rect;
  u16 total_width = 0, width = 0, height = 0;
  rsc_bitmap_t hdr_bmp;
  u8 *p_bmp_data = NULL;
  u8 i = 0;
  u16 num[TBOX_MAX_ITEM_NUM];
  rsc_fstyle_t *p_fstyle = NULL;
  u16 uni_char[2];//for BigEdian and LittleEdian

  MT_ASSERT(p_ctrl != NULL);


  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _tbox_get_fstyle(attr, p_data);

  ctrl_get_client_rect(p_ctrl, &str_rect);

  num[0] = p_data->ctrl_time.year;
  num[1] = p_data->ctrl_time.month;
  num[2] = p_data->ctrl_time.day;
  num[3] = p_data->ctrl_time.hour;
  num[4] = p_data->ctrl_time.minute;
  num[5] = p_data->ctrl_time.second;

  total_width = _tbox_get_total_width(p_ctrl);

  if(p_ctrl->priv_attr & STL_RIGHT)
  {
    str_rect.left = str_rect.right - p_data->str_left - total_width;
  }
  else if(p_ctrl->priv_attr & STL_LEFT)
  {
    str_rect.left = str_rect.left + p_data->str_left;
  }
  else
  {
    str_rect.left =
      str_rect.left + ((str_rect.right - str_rect.left - total_width) >> 1);
  }

  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    if(_tbox_get_item_bit(p_ctrl, i) != 0)
    {
      convert_i_to_dec_str_ex(str_num, num[i], ((i == 0) ? 4 : 2));

      width = (p_data->max_num_width * _tbox_get_item_bit(p_ctrl, i));
      str_rect.right = str_rect.left + width;

      //draw number
      if(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK)
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_data->str_top, 0, str_num, font,
                        _get_draw_style(p_data, i));
      }
      else
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_data->str_top, 0, str_num, font,
                        MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
      }
      str_rect.left += width; //width of number.

      //draw separator
      switch(p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK)
      {
        case TBOX_SEPARATOR_TYPE_STRID:
          width = rsc_get_strid_width(gui_get_rsc_handle(), (u16)p_data->separator[i], font);
          str_rect.right = str_rect.left + width;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_data->str_top, 0,
                          (u16 *)gui_get_string_addr((u16)p_data->separator[i]),
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
          str_rect.left += width;
          break;
        case TBOX_SEPARATOR_TYPE_EXTSTR:
          width = rsc_get_unistr_width(gui_get_rsc_handle(), (u16 *)(p_data->separator[i]), font);
          str_rect.right = str_rect.left + width;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_data->str_top, 0, (u16 *)(p_data->separator[i]),
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
          str_rect.left += width;
          break;
        case TBOX_SEPARATOR_TYPE_BMPID:
          rsc_get_bmp(gui_get_rsc_handle(), (u16)p_data->separator[i], &hdr_bmp, &p_bmp_data);
          width = hdr_bmp.width;
          str_rect.right = str_rect.left + width;
          gui_draw_picture(hdc, &str_rect, p_ctrl->priv_attr, 0,
                           p_data->str_top, (u16)p_data->separator[i]);
          str_rect.left += width;
          break;
        case TBOX_SEPARATOR_TYPE_UNICODE:
          p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), font);
          rsc_get_char_attr(gui_get_rsc_handle(), (u16)p_data->separator[i], p_fstyle, &width, &height);

          //gui_draw_char(hdc, p_fstyle,
          //              (u16)p_data->separator[i], str_rect.left, str_rect.top);

          str_rect.right = str_rect.left + width;
          uni_char[0] = (u16)p_data->separator[i];//for BigEdian and LittleEdian
          uni_char[1] = 0;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_data->str_top, 0,
                          uni_char,
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));          

          str_rect.left += width;
          break;
      }
    }
  }
}


BOOL tbox_enter_edit(control_t *p_ctrl)
{
  u32 tbox_attr[TBOX_MAX_ITEM_NUM] =
  {
    TBOX_YEAR, TBOX_MONTH, TBOX_DAY,
    TBOX_HOUR, TBOX_MIN, TBOX_SECOND
  };
  ctrl_tbox_data_t *p_data = NULL;
  u8 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;
  p_ctrl->priv_attr |= TBOX_HL_STATUS_MASK;

  // goto the first bit
  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    if(p_ctrl->priv_attr & tbox_attr[i])
    {
      p_data->curn_item = i;
      break;
    }
  }
  p_data->curn_bit = 0; //first bit of year item

  /* DO SOMETHING */
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL tbox_is_invalid_time(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;
  BOOL is_leap = FALSE;
  u16 year = 0;
  u8 days = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  //check year
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) != 0)
  {
    //check year
    if((p_data->ctrl_time.year < 2000) || (p_data->ctrl_time.year > 2100))
    {
      return TRUE;
    }
  }

  //check month
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MONTH) != 0)
  {
    //check month
    if((p_data->ctrl_time.month > 12) || (p_data->ctrl_time.month <= 0))
    {
      return TRUE;
    }
  }

  //check day
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_DAY) != 0)
  {
    year = p_data->ctrl_time.year;

    is_leap = NOT(year % 400) || ((year % 100) && NOT(year % 4));

    switch(p_data->ctrl_time.month)
    {
      case 11:
      case 9:
      case 6:
      case 4:
        days = 30;
        break;
      case 12:
      case 10:
      case 8:
      case 7:
      case 5:
      case 3:
      case 1:
        days = 31;
        break;
      case 2:
        days = is_leap ? 29 : 28;
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    //check day
    if((p_data->ctrl_time.day > days) || (p_data->ctrl_time.day <= 0))
    {
      return TRUE;
    }
  }

  //check hour
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_HOUR) != 0)
  {
    if(p_data->ctrl_time.hour >= 24) //check hour
    {
      return TRUE;
    }
  }

  //check minute
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MIN) != 0)
  {
    if(p_data->ctrl_time.minute >= 60) //check minute
    {
      return TRUE;
    }
  }

  //check second
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_SECOND) != 0)
  {
    if(p_data->ctrl_time.second >= 60) //check second
    {
      return TRUE;
    }
  }

  return FALSE;
}


void tbox_exit_edit(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;
  utc_time_t tm = {0};

  MT_ASSERT(p_ctrl != NULL);
  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  if(tbox_is_invalid_time(p_ctrl))
  {
    // set num to border
    //time_get(&curn_time, TRUE);
    tm.year = 2009;
    tm.month = 1;
    tm.day = 1;
    //tm.hour = 24;
    //tm.minute = 59;
    //tm.second = 59;
    if(p_data->ctrl_time.hour >= 24) //check hour
    {
      tm.hour = 23;
      tm.minute = 59;
      tm.second = 59;
    }
    else if(p_data->ctrl_time.minute >= 60) 
    {
      tm.hour = p_data->ctrl_time.hour;
      tm.minute = 59;
      tm.second  = 59;
    }
    else
    {
      tm.hour = p_data->ctrl_time.hour;
      tm.minute = p_data->ctrl_time.minute;
      tm.second = 59;
    }
    
    tbox_set_time(p_ctrl, &tm);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    // notify out of range
    //p_ctrl->p_proc(p_ctrl, MSG_OUTRANGE, p_data->num, border);
  }
  else
  {
    p_ctrl->priv_attr &= (~TBOX_HL_STATUS_MASK);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    tbox_get_time(p_ctrl, &tm);

    // notify leave edit
    p_ctrl->p_proc(p_ctrl, MSG_CHANGED, (u32)(&tm), 0);
  }
}


BOOL tbox_input_number(control_t *p_ctrl, u16 msg)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  _tbox_set_curn_bit(p_ctrl, (u8)(msg & MSG_DATA_MASK));

  // move to next bit
  //p_data->curn_bit =
  //  (p_data->curn_bit + TBOX_BITLEN - 1) % p_data->bit_length;
  tbox_shift_focus(p_ctrl, -1);

  return TRUE;
}


static BOOL _num_increase(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  switch(p_data->curn_item)
  {
    case TBOX_ITEM_YEAR:
      //OS_PRINTF("before year[%d]\n", p_data->year);
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.year -= 9 * _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.year += _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_data->curn_bit + 1));
      }
      //OS_PRINTF("after year[%d]\n", p_data->year);
      break;
    case TBOX_ITEM_MONTH:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.month -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.month += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_DAY:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.day -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.day += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_HOUR:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.hour -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.hour += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MIN:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.minute -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.minute += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_SECOND:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_data->ctrl_time.second -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.second += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_data->curn_bit + 1));
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  return TRUE;
}


static BOOL _num_decrease(control_t *p_ctrl)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  switch(p_data->curn_item)
  {
    case TBOX_ITEM_YEAR:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.year += 9 * _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.year -= _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MONTH:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.month += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.month -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_DAY:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.day += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.day -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_HOUR:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.hour += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.hour -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MIN:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.minute += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.minute -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_data->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_SECOND:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_data->ctrl_time.second += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_data->curn_bit + 1));
      }
      else
      {
        p_data->ctrl_time.second -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_data->curn_bit + 1));
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  return TRUE;
}


BOOL tbox_set_focus(control_t *p_ctrl, u8 bit, u8 item)
{
  ctrl_tbox_data_t *p_data = NULL;
  BOOL ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  if(bit < _tbox_get_item_bit(p_ctrl, item))
  {
    p_data->curn_bit = bit;
    p_data->curn_item = item;
    ret = TRUE;
  }

  return ret;
}


void tbox_get_focus(control_t *p_ctrl, u8 *p_bit, u8 *p_item)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

  *p_bit = p_data->curn_bit;
  *p_item = p_data->curn_item;
}


BOOL tbox_shift_focus(control_t *p_ctrl, s8 offset)
{
  ctrl_tbox_data_t *p_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_data = (ctrl_tbox_data_t *)p_ctrl->priv_data;

//  p_data->curn_bit =
//    (p_data->curn_bit + TBOX_BITLEN + offset) % TBOX_BITLEN;

  while(offset != 0)
  {
    offset = _tbox_shift_offset(p_ctrl, offset);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL tbox_is_on_edit(control_t *p_ctrl)
{
  return (BOOL)(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK);
}


static RET_CODE on_tbox_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    tbox_enter_edit(p_ctrl);
  }

  if(tbox_input_number(p_ctrl, msg))
  {
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_tbox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    tbox_enter_edit(p_ctrl);
  }
  else
  {
    tbox_exit_edit(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_tbox_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  s8 offset = (s8)(msg == MSG_FOCUS_LEFT ? 1 : -1);

  MT_ASSERT(p_ctrl != NULL);

  tbox_shift_focus(p_ctrl, offset);

  return SUCCESS;
}


static RET_CODE on_tbox_increase(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_increase(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_tbox_decrease(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_decrease(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


BEGIN_CTRLPROC(tbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_NUMBER, on_tbox_number)
ON_COMMAND(MSG_SELECT, on_tbox_select)
ON_COMMAND(MSG_FOCUS_LEFT, on_tbox_focus_change)
ON_COMMAND(MSG_FOCUS_RIGHT, on_tbox_focus_change)
ON_COMMAND(MSG_INCREASE, on_tbox_increase)
ON_COMMAND(MSG_DECREASE, on_tbox_decrease)
END_CTRLPROC(tbox_class_proc, ctrl_default_proc)
