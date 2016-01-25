/******************************************************************************/
/******************************************************************************/
#ifndef __GUI_SCRIPT_H__
#define __GUI_SCRIPT_H__
/*!
  \file gui_script.h

  This file defined data structure of script control.
  And defines interfaces for users to open a menu by script data.

  Development policy:
  */

/*!
  Script type.
  */
typedef enum
{
  /*!
    Error type.
    */
  TYPE_ERR = 0x00,
  /*!
    Specify script for bitmap control.
    */
  TYPE_BMAP,
  /*!
    Specify script for combo box control.
    */
  TYPE_CBOX,
  /*!
    Specify script for container control.
    */
  TYPE_CONT,
  /*!
    Specify script for editbox control.
    */
  TYPE_EBOX,
  /*!
    Specify script for list control.
    */  
  TYPE_LIST,
  /*!
    Specify script for matrix box control.
    */  
  TYPE_MBOX,
  /*!
    Specify script for number box control.
    */  
  TYPE_NBOX,
  /*!
    Specify script for progress bar control.
    */  
  TYPE_PBAR,
  /*!
    Specify script for scroll bar control.
    */  
  TYPE_SBAR,
  /*!
    Specify specify script for textfield control.
    */  
  TYPE_TEXT,
  /*!
    Specify script for time box control.
    */  
  TYPE_TBOX
}ctrl_type_t;

/*!
  This structure define information about a script control
  */
typedef struct
{
  /*!
    Defined control type
    */
  u8 type;
  /*!
    Defined in form OBJ_ATTR_XXX.
    */
  u8 attr;
  /*!
    The styles of control.
    */
  u8 style;
  /*!
    Every ctrl has its exclusive id in one window except 0.
    */
  u8  id;
  /*!
    The left control id.
    */
  u8  left;
  /*!
    The up control id.
    */
  u8  up;
  /*!
    The right control id.
    */
  u8  right;
  /*!
    The down control id.
    */
  u8  down;
  /*!
    The rectangle in parent's coordinate, left.
    */
  s16 frame_left;
  /*!
    The rectangle in parent's coordinate, top.
    */  
  s16 frame_top;
  /*!
    the rectangle in parent's coordinate, right.
    */  
  s16 frame_right;
  /*!
    The rectangle in parent's coordinate, bottom.
    */  
  s16 frame_bottom;
  /*!
    The parent of this control.
    */
  u16 parent;
  /*!
    The child of this control.
    */
  u16 child;
  /*!
    The next sibling control.
    */
  u16 next;
  /*!
    The prev sibling control.
    */
  u16 prev;
  /*!
    Control private attribute.
    */
  u32 priv_attr;

  /*!
    Rectangle style of a control, r-style on show.
    */
  u8 show_idx;
  /*!
    Rectangle style of a control, r-style on highlight.
    */
  u8 hl_idx;
  /*!
    Rectangle style of a control, r-style on gray.
    */
  u8 gray_idx;
}spt_ctrl_t;

/*!
  Private data of bitmap in script.
  */
typedef struct 
{
  /*!
    Bitmap data, external bitmap or bitmap id.
    */
  u32 data;
  /*!
    Distance to left side
    */
  u16 left;      
  /*!
    Distance to top side
    */
  u16 top;       
}spt_bmap_data_t;

/*!
  Private data of combo box(static) in script.
  */
typedef struct 
{
  /*!
    Distance to left side
    */  
  u16 str_left;
  /*!
    Distance to top side
    */  
  u16 str_top;
  /*!
    Font style on normal state.
    */
  u8  n_fstyle;
  /*!
    Font style on highlight state.
    */  
  u8  h_fstyle;
  /*!
    Font style on gray state.
    */  
  u8  g_fstyle;
}spt_cbox_static_data_t;

/*!
  Private data of combo box(dynamic type) in script.
  */
typedef struct 
{
  /*!
    Distance to left side
    */  
  u16 str_left;
  /*!
    Ddistance to top side
    */  
  u16 str_top;

  /*!
    Total number of dynamic combo box.
    */
  u32 total;

  /*!
    Font style on normal state.
    */
  u8  n_fstyle;
  /*!
    Font style on highlight state.
    */  
  u8  h_fstyle;
  /*!
    Font style on gray state.
    */  
  u8  g_fstyle;
}spt_cbox_dync_data_t;

/*!
  Private data of combo box(number type) in script.
  */
typedef struct 
{
  /*!
    Distance to left side(number type).
    */  
  u16 str_left;
  /*!
    Distance to top side(number type).
    */  
  u16 str_top;

  /*!
    Max value of combo box(number type).
    */
  s16 max;
  /*!
    Min value of combo box(number type).
    */  
  s16 min;
  /*!
    Current of combo box(number type).
    */  
  s16 curn;
  /*!
    Step of combo box(number type).
    */  
  s16 step;

  /*!
    Prefix of combo box(number type).
    */
  u32 prefix;
  /*!
    Postfix of combo box(number type).
    */  
  u32 postfix;

  /*!
    Bit length of combo box(number type).
    */
  u8  bit_length;
  /*!
    Font style on normal state(number type).
    */
  u8  n_fstyle;
  /*!
    Font style on highlight state(number type).
    */  
  u8  h_fstyle;
  /*!
    Font style on gray state(number type).
    */  
  u8  g_fstyle;
}spt_cbox_num_data_t;

/*!
  Private data of editor in script.
  */
typedef struct 
{
  /*!
    Normal color of string
    */
  u8  n_color;
  /*!
    Highlight color of string
    */
  u8  h_color;
  /*!
    Gray color of string
    */
  u8  g_color;
  /*!
    Distance between lines
    */  
  u8  str_l_space;
  /*!
    Distance from left side of textfield to left side of string.
    */
  u16 str_left;
  /*!
    Distance from top side of textfield to top side of string.
    */
  u16 str_top;
  /*!
    Maximal text length. zero means no limit.
    */
  u16 str_maxtext;
}spt_ebox_data_t;

/*!
  Private data of list in script.
  */
typedef struct 
{
  /*!
    Left coordinate of mid-rect.
    */
  s16 mid_rect_left;
  /*!
    Top coordinate of mid-rect.
    */
  s16 mid_rect_top;
  /*!
    Right coordinate of mid-rect.
    */
  s16 mid_rect_right;
  /*!
    Bottom coordinate of mid-rect.
    */
  s16 mid_rect_bottom;
  /*!
    Items per page
    */
  u16 page;
  /*!
    Interval between items
    */  
  u8 interval;
  /*!
    Gray color
    */  
  u8  item_g_idx;
  /*!
    Normal color
    */
  u8  item_n_idx;
  /*!
    Focus color
    */  
  u8  item_f_idx;
  /*!
    Select color
    */  
  u8  item_s_idx;
  /*!
    Select and focus color
    */
  u8  item_sf_idx;
}spt_list_data_t;

/*!
  Private data of matrix box in script.
  */
typedef struct 
{
  /*!
    Left coordinate of mid-rect.
    */
  s16 mid_rect_left;
  /*!
    Top coordinate of mid-rect.
    */
  s16 mid_rect_top;
  /*!
    Right coordinate of mid-rect.
    */
  s16 mid_rect_right;
  /*!
    Bottom coordinate of mid-rect.
    */
  s16 mid_rect_bottom;

  /*!
    Rows of matrix box.
    */
  u8  row;
  /*!
    Columns of matrix box.
    */
  u8  col;
  /*!
    Horizontal interval of matrix box.
    */  
  u8  h_interval;
  /*!
    Vertical interval of matrix box.
    */  
  u8  v_interval;
  /*!
    Left offset of matrix box string.
    */    
  u16 str_left;
  /*!
    Top offset of matrix box string.
    */    
  u16 str_top;
  /*!
    Left offset of matrix box icon.
    */      
  u8  icon_left;
  /*!
    Top offset of matrix box icon.
    */      
  /*!
    Item rstyle when item is in gray state.
    */
  u8 item_g_idx;
  /*!
    Item rstyle when item is in normal state.
    */
  u8 item_n_idx;
  /*!
    Item rstyle when item is in focus state.
    */
  u8 item_f_idx;
  /*!
    Font style of item in gary state.
    */
  u8 g_color;
  /*!
    Font style of item in normal state.
    */
  u8 n_color;
  /*!
    Font style of item in focus state.
    */
  u8 f_color;
}spt_mbox_data_t;

/*!
  Private data of number box in script.
  */
typedef struct 
{
  /*!
    Current value of number box.
    */
  u32 num;
  /*!
    Min value of number box.
    */
  u32 min;
  /*!
    Max value of number box.
    */
  u32 max;

  /*!
    Current bit of number box.
    */
  u8  curn_bit;
  /*!
    Bit length of number box.
    */  
  u8  bit_length;
  /*!
    Normal rstyle of number box.
    */  
  u8  n_color;
  /*!
    Highlight rstyle of number box.
    */  
  u8  h_color;
  /*!
    Edit rstyle of number box.
    */  
  u8  e_color;
  /*!
    Inactive rstyle of number box.
    */    
  u8  g_color;
  /*!
    Left offset of number box string.
    */    
  u16 str_left;
  /*!
    Top offset of number box string.
    */    
  u16 str_top;

}spt_nbox_data_t;

/*!
  Private data of progress bar in script.
  */
typedef struct 
{
  /*!
    Left coordinate of mid-rect.
    */
  s16 mid_rect_left;
  /*!
    Top coordinate of mid-rect.
    */
  s16 mid_rect_top;
  /*!
    Right coordinate of mid-rect.
    */
  s16 mid_rect_right;
  /*!
    Bottom coordinate of mid-rect.
    */
  s16 mid_rect_bottom;

  /*!
    Min value of progress bar.
    */
  u16 min;
  /*!
    Max value of progress bar.
    */
  u16 max;

  /*!
    Current value of progress bar.
    */
  u16 curn;

  /*!
    Steps of scroll bar.
    */
  u16 step;
  /*!
    Interval between blocks.
    */
  u8  interval;
  /*!
    Color style at minimum value side, 0xff means invalid.
    */  
  u8  min_color;
  /*!
    Ccolor style at maximum value side, 0xff means invalid.
    */  
  u8  max_color;
  /*!
    Color style at current point, 0xff means invalid.
    */  
  u8  cur_color;
}spt_pbar_data_t;

/*!
  Private data of scroll bar in script.
  */
typedef struct 
{
  /*!
    Left coordinate of mid-rect.
    */
  s16 mid_rect_left;
  /*!
    Top coordinate of mid-rect.
    */
  s16 mid_rect_top;
  /*!
    Right coordinate of mid-rect.
    */
  s16 mid_rect_right;
  /*!
    Bbottom coordinate of mid-rect.
    */
  s16 mid_rect_bottom;
  /*!
    Rstyle for mid-rect on normal state.
    */
  u8  n_mid_color;
  /*!
    Rstyle for mid-rect on highlight state.
    */
  u8  h_mid_color;
  /*!
    Rstyle for mid-rect on inactive state.
    */  
  u8  g_mid_color;

  /*!
    Page of scroll bar.
    */
  u16 page;

  /*!
    Total number of scroll bar.
    */
  u16 total;
}spt_sbar_data_t;

/*!
  Private data of textfiled control in script.
  */
typedef struct 
{
  /*!
    String data, string id, ascii string, unicode string....
    */
  u32 str_char;

  /*!
    Font style on normal state.
    */
  u8  n_fstyle;
  /*!
    Font style on hightlight state.
    */
  u8  h_fstyle;
  /*!
    Font style on inactive state.
    */  
  u8  g_fstyle;
  /*!
    Gap between lines.
    */  
  u8  str_l_space;
  /*!
    Left offset of the string.
    */
  u16 str_left;
  /*!
    Top offset of the string.
    */  
  u16 str_top;
}spt_text_data_t;

/*!
  Create a menu by load menu data from script.

  \param[in] root_id : root id of the menu.
  \return : root container of the menu.
  */
control_t *spt_load_menu(u8 root_id);

#endif

