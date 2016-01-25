/******************************************************************************/
/******************************************************************************/
#ifndef __TEXT_CTRL_H__
#define __TEXT_CTRL_H__
/*!
   \file ctrl_textfield.h

   This file defined some interfaces of textfield control for application layer
   modules.
  */

/*!
   Class name of textfield control class
  */
#define CTRL_TEXT                ("textfield")

/*!
   String type mask of textfield control
  */
#define TEXT_STRTYPE_MASK        0x0000000F   // string type
/*!
   Set the textfield string by unicode string.
  */
#define TEXT_STRTYPE_UNICODE     0x00000000
/*!
   Set the textfield string by static unicode string.
  */
#define TEXT_STRTYPE_EXTSTR      0x00000001
/*!
   Set the textfield string by string id.
  */
#define TEXT_STRTYPE_STRID       0x00000002
/*!
   Set the textfield string by decimal number.
  */
#define TEXT_STRTYPE_DEC         0x00000003
/*!
   Set the textfield string by hex number.
  */
#define TEXT_STRTYPE_HEX         0x00000004

/*!
   Textfield page mask, 0 is disable; 1 is enable.
  */
#define TEXT_PAGE_MASK           0x00000010

/*!
   Textfield string align mask
  */
#define TEXT_ALIGN_MASK          0xF0000000


/*!
   This structure defines private data of textfield control.
  */
typedef struct
{
  /*!
     String content of textfield control.
    */
  u32 str_char;
  /*!
     R-style of string in normal state.
    */
  u8 n_fstyle;
  /*!
     R-style of string in highlight color.
    */
  u8 h_fstyle;
  /*!
     R-style of string in gray color.
    */
  u8 g_fstyle;
  /*!
     Distance between lines.
    */
  u8 str_l_space;
  /*!
     Distance from left side of textfield to left side of string
    */
  u16 str_left;
  /*!
     Distance from top side of textfield to top side of string
    */
  u16 str_top;
  /*!
     String is eof | indicate how many chars has been drawed
    */
  u32 status;
  /*!
     Point to the curn line.
    */
  u32 curn;
  /*!
     Specified the drawing style
    */
  u32 draw_style;
  /*!
    Specified the total lines of the textfield.
    */
  u32 total;
  /*!
    Specified the lines per page of the textfield.
    */
  u32 page;
  /*!
    Specified the scroll bar for textfield in page mode.
    */
  control_t *p_scroll_bar;
  /*!
     Real item area , referrence is list
    */
  rect_t mid_rect;
  /*!
     Line start address.(only used in page mode.)
    */
  u32 *p_line_addr;    
}ctrl_text_data_t;

/*!
   Register textfield control class to system

   \param[in] max_cnt : max textfield control number
   \return : SUCCESS or ERR_FAILURE.
  */
RET_CODE text_register_class(u16 max_cnt);

/*!
   Set text content with static string

   \param[in] p_ctrl : the handle of textfield control
   \param[in] p_extstr : string buffer
   \return : NULL
  */
void text_set_content_by_extstr(control_t *p_ctrl, u16 *p_extstr);

/*!
   Set text content with string id

   \param[in] p_ctrl : the handle of textfield control
   \param[in] str_id : string id
   \return : NULL
  */
void text_set_content_by_strid(control_t *p_ctrl, u16 str_id);

/*!
   Set item content with number

   \param[in] p_ctrl : the handle of textfield control
   \param[in] dec : dec number
   \return : NULL
  */
void text_set_content_by_dec(control_t *p_ctrl, s32 dec);

/*!
   Set item content with number

   \param[in] p_ctrl : the handle of textfield control
   \param[in] hex : hex number
   \return : NULL
  */
void text_set_content_by_hex(control_t *p_ctrl, s32 hex);

/*!
   Set middle rect.

   \param[in] p_ctrl : list control
   \param[in] left : left of middle rect
   \param[in] top : top of middle rect
   \param[in] width : width of middle rect
   \param[in] height : height of middle rect
   \return : NULL
  */
void text_set_mid_rect(control_t *p_ctrl,
                       s16 left,
                       s16 top,
                       u16 width,
                       u16 height);

/*!
   Set text content with char string

   \param[in] p_ctrl : the handle of textfield control
   \param[in] p_ascstr : char string
   \return : NULL
  */
void text_set_content_by_ascstr(control_t *p_ctrl, u8 *p_ascstr);

/*!
   Set text content with unicode string

   \param[in] p_ctrl : the handle of textfield control
   \param[in] p_unistr : unicode string
   \return : NULL
  */
void text_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr);

/*!
   Set text font style.

   \param[in] p_ctrl : the handle of textfield control
   \param[in] n_fstyle : normal font fstyle
   \param[in] h_fstyle : high light font fstyle
   \param[in] g_fstyle : gray font fstyle
   \return : NULL
  */
void text_set_font_style(control_t *p_ctrl,
                         u8 n_fstyle,
                         u8 h_fstyle,
                         u8 g_fstyle);

/*!
   Set text position

   \param[in] p_ctrl : the handle of textfield control
   \param[in] left : left shift of string
   \param[in] top : top shift of string
   \return : NULL
  */
void text_set_offset(control_t *p_ctrl, u16 left, u16 top);


/*!
   Enable page support

   \param[in] p_ctrl : the handle of textfield control
   \param[in] enable : TRUE/FALSE
   \return : NULL
  */
void text_enable_page(control_t *p_ctrl, BOOL enable);


/*!
   Set align style.

   \param[in] p_ctrl : the handle of textfield control
   \param[in] style : text align style
   \return : NULL
  */
void text_set_align_type(control_t *p_ctrl, u32 style);

/*!
   Set string type

   \param[in] p_ctrl : the handle of textfield control
   \param[in] type : string type
   \return : NULL
  */
void text_set_content_type(control_t *p_ctrl, u32 type);

/*!
   Set distance between lines           .

   \param[in] p_ctrl : the handle of textfield control
   \param[in] l_space : distance between lines
   \return : NULL
  */
void text_set_line_gap(control_t *p_ctrl, u8 l_space);

/*!
   Get text content.

   \param[in] p_ctrl : the handle of textfield control
   \return : content decided by content type. May be string id, may be
   numbers,.....
  */
u32 text_get_content(control_t *p_ctrl);

/*!
   Get address of text content.

   \param[in] p_ctrl : the handle of textfield control
   \return : content buffer, decided by content type. May be string id, may be
   numbers,.....
  */
u32 *text_get_content_buf(control_t *p_ctrl);

/*!
   Get the status of paintting.

   \param[in] p_ctrl : the handle of textfield control
   \return : status, use STRING_IS_EOF /GET_DRAWED_CHARS to get detail infor.
  */
u32 text_get_status(control_t *p_ctrl);

/*!
   Get the status of breakword.

   \param[in] p_ctrl : the handle of textfield control
   \param[in] enable : enable or not
  */
void text_set_breakword(control_t *p_ctrl, BOOL enable);

/*!
   Get the status of breakword.

   \param[in] p_ctrl : the handle of textfield control
   \return : status, STRDRAW_BREAK_WORD or STRDRAW_NORMAL.
  */
u32 text_get_breakword(control_t *p_ctrl);

/*!
   Textfield control message process function.

   \param[in] p_ctrl : textfield control
   \param[in] msg : msg
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE text_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
   reset total lines/page/curn of textfield control.

   \param[in] p_ctrl : textfield control
   \return : NULL
  */
void text_reset_param(control_t *p_ctrl);

/*!
   Set scroll bar.

   \param[in] p_ctrl : text control
   \param[in] p_ctrl_bar : scroll bar control
   \return : NULL
  */
void text_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar);
#endif
