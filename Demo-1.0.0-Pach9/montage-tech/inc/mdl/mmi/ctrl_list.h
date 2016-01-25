/******************************************************************************/
/******************************************************************************/
#ifndef __LIST_CTRL_H__
#define __LIST_CTRL_H__
/*!
   \file ctrl_list.h

   This file defined some interfaces about list control for application layer
   modules.
  */

/*!
   Class name of list control class
  */
#define CTRL_LIST    ("list")


/*!
   This enum defines return value of some function.
  */
typedef enum
{
  /*!
     List error code. means the new operation is not allow.
    */
  LIST_FALSE = 0,
  /*!
     List operation success.
    */
  LIST_SUCCESS,
  /*!
     Reload data for one item
    */
  LIST_RELOAD_ONE,
  /*!
     Reload data for all items.
    */
  LIST_RELOAD_ALL
}list_ret_t;


/*!
   If focus item is not in current page,
   we should decide how to change current page.
  */
typedef enum
{
  /*!
     Used by page up/down
    */
  LIST_FOCUS_KEEP_POS = 0,
  /*!
     Used by step up
    */
  LIST_FOCUS_ON_TOP,
  /*!
     Used by step down
    */
  LIST_FOCUS_ON_BOTTOM,
  /*!
     Used by jump
    */
  LIST_FOCUS_JUMP
}list_focus_mode_t;


/*!
   The status of list item
  */
typedef enum
{
  /*!
     List item unselected
    */
  LIST_ITEM_NORMAL = 0,
  /*!
     List item selected
    */
  LIST_ITEM_SELECTED,
  /*!
     List item disableed
    */
  LIST_ITEM_DISABLED
}list_item_status_t;

/*!
   List select mode, 0 don't support select, 1 support select
  */
#define LIST_SELECT_MODE         0x00000001
/*!
   List cycle mode. 0 normally, 1 circularly
  */
#define LIST_CYCLE_MODE          0x00000002
/*!
   List cycle mode. 0 normally, 1 circularly
  */
#define LIST_PAGE_MODE           0x00000004
/*!
  List field scroll mode
  */
#define LISTFIELD_SCROLL        0x00000008

/*!
   List select mode mask
  */
#define LIST_SELECT_MASK         0x00F00000
/*!
   Single select mode
  */
#define LIST_SINGLE_SELECT       0x00000000
/*!
   Multi select mode
  */
#define LIST_MULTI_SELECT        0x00100000

/*!
   List field align mask
  */
#define LISTFIELD_ALIGN_MASK     0xF0000000
/*!
   List field content type mask
  */
#define LISTFIELD_TYPE_MASK      0x0F000000
/*!
   List field content is set by unicode string
  */
#define LISTFIELD_TYPE_UNISTR    0x00000000
/*!
   List field content is set by static unicode string
  */
#define LISTFIELD_TYPE_EXTSTR    0x01000000
/*!
   List field content is set by string id
  */
#define LISTFIELD_TYPE_STRID     0x02000000
/*!
   List field content is set by bitmap
  */
#define LISTFIELD_TYPE_ICON      0x03000000
/*!
   List field content is set by decimal number
  */
#define LISTFIELD_TYPE_DEC       0x04000000
/*!
   List field content is set by hexadecimal number
  */
#define LISTFIELD_TYPE_HEX       0x05000000

/*!
   Default focus position.
  */
#define LIST_INVALID_FOCUS       0xFFFF

/*!
   Font style for a field
  */
typedef struct
{
  /*!
     Gray color
    */
  u8 g_xstyle;
  /*!
     Normal color
    */
  u8 n_xstyle;
  /*!
     Focus color
    */
  u8 f_xstyle;
  /*!
     Select color
    */
  u8 s_xstyle;
  /*!
     Select and focus color
    */
  u8 sf_xstyle;
}list_xstyle_t;

/*!
   List field attrib
  */
typedef struct
{
  /*!
     Attribute of field
    */
  u32 attr;
  /*!
     Width of a field
    */
  u16 width;
  /*!
     Left shift of content, referrence is item
    */
  u16 shift_left;
  /*!
     Top shift of content , referrence is item
    */
  u16 shift_top;
  /*!
     Max number string length
    */
  u8 bit_length;
  /*!
     Rect style for a field
    */
  list_xstyle_t rstyle;
  /*!
     Font style for a field
    */
  list_xstyle_t fstyle;
}list_field_t;


/*!
   Defines the callback function of list, it will be called for reloading data.
  */
typedef  RET_CODE (*list_update_t)(control_t *p_ctrl, u16 valid_pos, u16 size,
                                   u32 context);

/*!
   This structure defines private data of list control
  */
typedef struct
{
  /*!
     Each list contains a scroo bar control
    */
  control_t *p_scroll_bar;
  /*!
     Items per page
    */
  u16 page;
  /*!
     Totoal item numbers
    */
  u16 total;
  /*!
     Item buffer size, useless in full automatical mode
    */
  u16 size;
  /*!
     Current position
    */
  u16 cur_pos;
  /*!
     Valid positon, useless in full automatical mode
    */
  u16 valid_pos;
  /*!
     Focus positon
    */
  u16 focus;
  /*!
     Item status buffer , 2 bits for every item
     00 is normal , 01 is selected , 10 is disable
     only work when LIST_SELECT_MODE
    */
  u32 *p_status;
  /*!
     Field content buffer
    */
  u32 *p_content;
  /*!
     Real item area , referrence is list
    */
  rect_t mid_rect;
  /*!
     List field buffer
    */
  list_field_t *p_field;
  /*!
     Interval between items
    */
  u8 interval;
  /*!
     Field number per item
    */
  u8 field_num;
  /*!
     Color style for field
    */
  list_xstyle_t rstyle;
  /*!
     Callback function for update the content of the list
    */
  list_update_t update_cb;
  /*!
     The context of update function
    */
  u32 context;
}ctrl_list_data_t;

/*!
   Register list control class to system

   \param[in] max_cnt : max list control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE list_register_class(u16 max_cnt);

/*!
   Draw field

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] is_force : update invalid region before draw
   \return : NULL
  */
void list_draw_field_ext(control_t *p_ctrl,
                         u16 item_idx,
                         u8 field_idx,
                         BOOL is_force);

/*!
   Draw item.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] is_force : update invalid region before draw
   \return : NULL
  */
void list_draw_item_ext(control_t *p_ctrl, u16 item_idx, BOOL is_force);

/*!
   Set list total count and page count.

   \param[in] p_ctrl : list control
   \param[in] total : total count
   \param[in] page : page count
   \return : TRUE/FALSE
  */
BOOL list_set_count(control_t *p_ctrl, u16 total, u16 page);

/*!
   Set item status.

   \param[in] p_ctrl : list control
   \param[in] idx : item index
   \param[in] status : new status
   \return : TRUE/FALSE
  */
BOOL list_set_item_status(control_t *p_ctrl, u16 idx, u8 status);

/*!
   Set current pos.

   \param[in] p_ctrl : list control
   \param[in] cur_pos : new current pos
   \return : NULL
  */
void list_set_current_pos(control_t *p_ctrl, u16 cur_pos);

/*!
   Set valid pos in half mode.

   \param[in] p_ctrl : list control
   \param[in] valid_pos : first valid data index
   \return : NULL
  */
void list_set_valid_pos(control_t *p_ctrl, u16 valid_pos);

/*!
   Set focus pos.

   \param[in] p_ctrl : list control
   \param[in] focus : new focus pos
   \return : reload data or not
  */
list_ret_t list_set_focus_pos(control_t *p_ctrl, u16 focus);

/*!
   Set field with unicode string.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] p_unistr : unicode string
   \return : TRUE/FALSE
  */
BOOL list_set_field_content_by_unistr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_unistr);

/*!
  Set field with char string.

  \param[in] p_ctrl : list control
  \param[in] item_idx : item index
  \param[in] field_idx : field index
  \param[in] p_ascstr : char string
  \return : TRUE/FALSE
  */
BOOL list_set_field_content_by_ascstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u8 *p_ascstr);

/*!
  Set field with string id.

  \param[in] p_ctrl : list control
  \param[in] item_idx : item index
  \param[in] field_idx : field index
  \param[in] str_id : string id
  \return : NULL
  */
void list_set_field_content_by_strid(control_t *p_ctrl,
                                     u16 item_idx,
                                     u8 field_idx,
                                     u16 str_id);

/*!
   Set field with bitmap id.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] bmp_id : bitmap id
   \return : NULL
  */
void list_set_field_content_by_icon(control_t *p_ctrl,
                                    u16 item_idx,
                                    u8 field_idx,
                                    u16 bmp_id);

/*!
   Set field with static string buffer.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] p_extstr : static string buffer
   \return : NULL
  */
void list_set_field_content_by_extstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_extstr);

/*!
   Set field with number.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] dec : dec number
   \return : NULL
  */
void list_set_field_content_by_dec(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 dec);

/*!
   Set field with number.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \param[in] hex : hex number
   \return : NULL
  */
void list_set_field_content_by_hex(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 hex);

/*!
   Set item color.

   \param[in] p_ctrl : list control
   \param[in] p_rstyle : rstyle for list item.
   \return : NULL
  */
void list_set_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_rstyle);

/*!
   Set field attribute.

   \param[in] p_ctrl : list control
   \param[in] field_idx : field index
   \param[in] attr : field attribute
   \param[in] width : field width
   \param[in] left : left shift
   \param[in] top : top shift
   \return : NULL
  */
void list_set_field_attr(control_t *p_ctrl,
                         u8 field_idx,
                         u32 attr,
                         u16 width,
                         u16 left,
                         u16 top);

/*!
   Set field rect style.

   \param[in] p_ctrl : list control
   \param[in] field_idx : field index
   \param[in] p_rstyle : rstyle for list field.
   \return : NULL
  */
void list_set_field_rect_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_rstyle);

/*!
   Set field font style.

   \param[in] p_ctrl : list control
   \param[in] field_idx : field index
   \param[in] p_fstyle : rstyle for list field.
   \return : NULL
  */
void list_set_field_font_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_fstyle);

/*!
   Set field number.

   \param[in] p_ctrl : list control
   \param[in] field_cnt : field number
   \param[in] size : size
   \return : TRUE/FALSE
  */
BOOL list_set_field_count(control_t *p_ctrl, u8 field_cnt, u16 size);

/*!
   Set scroll bar.

   \param[in] p_ctrl : list control
   \param[in] p_ctrl_bar : scroll bar control
   \return : NULL
  */
void list_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar);

/*!
   Get scroll bar.

   \param[in] p_ctrl : list control
   \return : scroll bar control
  */
control_t *list_get_scrollbar(control_t *p_ctrl);

/*!
   Set select mode.

   \param[in] p_ctrl : list control
   \param[in] enable : support select
   \return : NULL
  */
void list_enable_select_mode(control_t *p_ctrl, BOOL enable);

/*!
   Set select mode.

   \param[in] p_ctrl : list control
   \param[in] type : type
   \return : NULL
  */
void list_set_select_mode(control_t *p_ctrl, u32 type);


/*!
   Set scroll mode.

   \param[in] p_ctrl : list control
   \param[in] enable : move focus circularly
   \return : NULL
  */
void list_enable_cycle_mode(control_t *p_ctrl, BOOL enable);

/*!
   Set page mode.

   \param[in] p_ctrl : list control
   \param[in] is_enable : move focus by page
   \return : NULL
  */
void list_enable_page_mode(control_t *p_ctrl, BOOL is_enable);

/*!
   Select focus item.

   \param[in] p_ctrl : list control
   \return : need redraw
  */
BOOL list_select(control_t *p_ctrl);

/*!
   Select an item.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \return : TRUE/FALSE
  */
BOOL list_select_item(control_t *p_ctrl, u16 item_idx);

/*!
   Unselect an item

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \return : TRUE/FALSE
  */
BOOL list_unselect_item(control_t *p_ctrl, u16 item_idx);

/*!
   Clean all selected flags.

   \param[in] p_ctrl : list control
   \return : NULL
  */
void list_clear_status(control_t *p_ctrl);

/*!
   Set callback function for reload data.

   \param[in] p_ctrl : list control
   \param[in] p_cb : update callback function
   \param[in] context : the context of function
   \return : NULL
  */
void list_set_update(control_t *p_ctrl, list_update_t p_cb, u32 context);

/*!
  Get callback function for reload data.

  \param[in] p_ctrl : list control
  \param[out] p_context : the context of function
  \return : update callback function
  */
list_update_t list_get_update(control_t *p_ctrl,  u32 *p_context);


/*!
   Set number bit number.

   \param[in] p_ctrl : list control
   \param[in] field_idx : field index
   \param[in] bit_length : bit number
   \return : NULL
  */
void list_set_field_bit_length(control_t *p_ctrl, u8 field_idx, u8 bit_length);

/*!
   Set middle rect.

   \param[in] p_ctrl : list control
   \param[in] left : left of middle rect
   \param[in] top : top of middle rect
   \param[in] width : width of middle rect
   \param[in] height : height of middle rect
   \param[in] interval : interval between items
   \return : NULL
  */
void list_set_mid_rect(control_t *p_ctrl,
                       s16 left,
                       s16 top,
                       u16 width,
                       u16 height,
                       u8 interval);

/*!
   Get item status.

   \param[in] p_ctrl : list control
   \param[in] idx : item index
   \return : item status
  */
u8 list_get_item_status(control_t *p_ctrl, u16 idx);

/*!
   Get current pos.

   \param[in] p_ctrl : list control
   \return : current pos
  */
u16 list_get_current_pos(control_t *p_ctrl);

/*!
   Get valid pos in half mode.

   \param[in] p_ctrl : list control
   \return : valid pos
  */
u16 list_get_valid_pos(control_t *p_ctrl);

/*!
   Get focus position.

   \param[in] p_ctrl : list control
   \return : focus pos
  */
u16 list_get_focus_pos(control_t *p_ctrl);

/*!
   Get content.

   \param[in] p_ctrl : list control
   \param[in] item_idx : item index
   \param[in] field_idx : field index
   \return : content
  */
u32 list_get_field_content(control_t *p_ctrl, u16 item_idx, u8 field_idx);

/*!
   Get first selected item index from a begin pos.

   \param[in] p_ctrl : list control
   \param[in] begin : begin pos
   \return : first selected item index
  */
u16 list_get_the_first_selected(control_t *p_ctrl, u16 begin);

/*!
   Get list total count

   \param[in] p_ctrl : list control
   \return : total count
  */
u16 list_get_count(control_t *p_ctrl);

/*!
   Get item count in page

   \param[in] p_ctrl : list control
   \return : item count
  */
u16 list_get_page(control_t *p_ctrl);


/*!
   Get the new focus

   \param[in] p_ctrl : list control
   \param[in] offset : the offset
   \param[in] p_is_reverse : reverse or not
   \return : total count
  */
s16 list_get_new_focus(control_t *p_ctrl, s16 offset, BOOL *p_is_reverse);

/*!
   Scroll up focus

   \param[in] p_ctrl : list control
   \return : reload data or not
  */
list_ret_t list_scroll_up(control_t *p_ctrl);

/*!
   Scroll down focus.

   \param[in] p_ctrl : list control
   \return : reload data or not
  */
list_ret_t list_scroll_down(control_t *p_ctrl);

/*!
   Page up focus.

   \param[in] p_ctrl : list control
   \return : reload data or not
  */
list_ret_t list_page_up(control_t *p_ctrl);

/*!
   Page down focus.

   \param[in] p_ctrl : list control
   \return : reload data or not
  */
list_ret_t list_page_down(control_t *p_ctrl);


/*!
   Control proc.

   \param[in] p_ctrl : list control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE list_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


#endif
