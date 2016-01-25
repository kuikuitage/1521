/******************************************************************************/
/******************************************************************************/
#ifndef __MBOX_CTRL_H__
#define __MBOX_CTRL_H__
/*!
   \file ctrl_matrixbox.h

   This file defined some interfaces of matrixbox control for application layer
   modules.
  */

/*!
   Class name of matrixbox control class
  */
#define CTRL_MBOX               ("matrixbox")

/*!
   String align type mask of matrixbox control
  */
#define MBOX_STR_ALIGN_MASK     0xF0000000
/*!
   Icon align type mask of matrixbox control
  */
#define MBOX_ICON_ALIGN_MASK    0x0F000000
/*!
   String content type mask
  */
#define MBOX_STRTYPE_MASK       0x00F00001
/*!
   Set string content by unicode string
  */
#define MBOX_STRTYPE_UNICODE    0x00000001
/*!
   Set string content by static unicode string
  */
#define MBOX_STRTYPE_EXTSTR     0x00100001
/*!
   Set string content by string id
  */
#define MBOX_STRTYPE_STRID      0x00200001
/*!
   Set string content by decimal number
  */
#define MBOX_STRTYPE_DEC        0x00300001
/*!
   Set string content by hexadecimal number
  */
#define MBOX_STRTYPE_HEX        0x00400001

/*!
   Enable or disable string mode
  */
#define MBOX_STRING_MODE        0x00000001
/*!
   Enable or disable string mode
  */
#define MBOX_ICON_MODE          0x00000002


/*!
   The status of mbox item
  */
typedef enum
{
  /*!
     Mbox item normal
    */
  MBOX_ITEM_NORMAL = 0,
  /*!
     Mbox item disableed
    */
  MBOX_ITEM_DISABLED
}mbox_item_status_t;


/*!
   This sturcture defines private data of matrixbox
  */
typedef struct
{
  /*!
     Total item count
    */
  u16 total;
  /*!
     Focus item index
    */
  u16 focus;
  /*!
     Row number, how many items in one column.
    */
  u8 row;
  /*!
     Column number, how many items in one row.
    */
  u8 col;
  /*!
     Interval between items in horizontal direction.
    */
  u8 h_interval;
  /*!
     Interval between items in vertical direction.
    */
  u8 v_interval;
  /*!
     Left interval of string, the reference is left of item rectangle.
    */
  u8 str_left;
  /*!
     Top interval of string, the reference is top of item rectangle.
    */
  u8 str_top;
  /*!
     Left interval of icon, the reference is left of item rectangle.
    */
  u8 icon_left;
  /*!
     Top interval of icon, the reference is top of item rectangle.
    */
  u8 icon_top;
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
  u8 g_fstyle;
  /*!
     Font style of item in normal state.
    */
  u8 n_fstyle;
  /*!
     Font style of item in focus state.
    */
  u8 f_fstyle;
  /*!
     Real item area, referrence is checkbox.
    */
  rect_t mid_rect;
  /*!
     Item status buffer , 1 bits for every item. 0 is normal, 1 is disable.
    */
  u32 *p_status;
  /*!
     String content buffer.
    */
  u32 *p_content;
  /*!
     Icon buffer.
    */
  u32 *p_icon;
}ctrl_mbox_data_t;

/*!
   Register matrixbox control class to system.

   \param[in] max_cnt : max matrixbox control number
   \return : SUCCESS or ERR_FAILURE
  */
s32 mbox_register_class(u16 max_cnt);

/*!
   Draw an item

   \param[in] p_ctrl : matrixbox control
   \param[in] is_force : update invalid region before draw
   \param[in] index : item index
   \return : NULL
  */
void mbox_draw_item_ext(control_t *p_ctrl, u16 index, BOOL is_force);

/*!
   Set string color

   \param[in] p_ctrl : matrixbox control
   \param[in] f_fstyle : focus fstyle
   \param[in] n_fstyle : normal fstyle
   \param[in] g_fstyle : gray fstyle
   \return : NULL
  */
void mbox_set_string_fstyle(control_t *p_ctrl,
                            u8 f_fstyle,
                            u8 n_fstyle,
                            u8 g_fstyle);

/*!
   Set item color

   \param[in] p_ctrl : matrixbox control
   \param[in] f_rstyle : focus fstyle
   \param[in] n_rstyle : normal fstyle
   \param[in] g_rstyle : gray fstyle
   \return : NULL
  */
void mbox_set_item_rstyle(control_t *p_ctrl,
                          u8 f_rstyle,
                          u8 n_rstyle,
                          u8 g_rstyle);

/*!
  Set item rect

  \param[in] p_ctrl : matrixbox control
  \param[in] left : left
  \param[in] top : top
  \param[in] right : right
  \param[in] bottom : bottom
  \param[in] h_interval : horizontal interval
  \param[in] v_interval : vertical interval
  \return : NULL
  */
void mbox_set_item_rect(control_t *p_ctrl,
                        s16 left,
                        s16 top,
                        s16 right,
                        s16 bottom,
                        u8 h_interval,
                        u8 v_interval);

/*!
   Set string rect

   \param[in] p_ctrl : matrixbox control
   \param[in] str_left : left pos of string
   \param[in] str_top : top pos of string
   \return : NULL
  */
void mbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top);

/*!
   Set count.

   \param[in] p_ctrl : matrixbox control
   \param[in] total : total number
   \param[in] col : col number
   \param[in] row : row number
   \return : TRUE/FALSE
  */
BOOL mbox_set_count(control_t *p_ctrl, u16 total, u8 col, u8 row);

/*!
   Get count.

   \param[in] p_ctrl : matrixbox control
   \param[in] p_total : total number
   \param[in] p_col : col number
   \param[in] p_row : row number
   \return : TRUE/FALSE
  */
BOOL mbox_get_count(control_t *p_ctrl, u16 *p_total, u8 *p_col, u8 *p_row);


/*!
   Set item status.

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \param[in] status : item status
   \return : TRUE/FALSE
  */
BOOL mbox_set_item_status(control_t *p_ctrl, u16 index, u8 status);

/*!
   Set item content by unicode string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_unistr : unicode string
   \return : TRUE or FALSE
  */
BOOL mbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr);

/*!
   Set item content by char string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_ascstr : char string
   \return : TRUE or FALSE
  */
BOOL mbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr);

/*!
   Set item content by string id.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] strid : string id
   \return : NULL
  */
void mbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid);

/*!
   Set item content by static string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_extstr : static string buffer
   \return : NULL
  */
void mbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr);

/*!
   Set item content by icon id

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] f_icon_id : focus icon id
   \param[in] n_icon_id : normal icon id
   \return : NULL
  */
void mbox_set_content_by_icon(control_t *p_ctrl,
                              u16 item_idx,
                              u16 f_icon_id,
                              u16 n_icon_id);

/*!
   Set item content with hex number.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] hex : hex number
   \return : NULL
  */
void mbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex);

/*!
   Set item content with dec number.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] dec : number
   \return : NULL
  */
void mbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec);

/*!
   Set string align style.

   \param[in] p_ctrl : matrixbox control
   \param[in] style : align style
   \return : NULL
  */
void mbox_set_string_align_type(control_t *p_ctrl, u32 style);

/*!
   Set icon align style.

   \param[in] p_ctrl : matrixbox control
   \param[in] style : align style
   \return : NULL
  */
void mbox_set_icon_align_type(control_t *p_ctrl, u32 style);

/*!
   Set content type.

   \param[in] p_ctrl : matrixbox control
   \param[in] type : content type
   \return : NULL
  */
void mbox_set_content_strtype(control_t *p_ctrl, u32 type);

/*!
   Set focus.

   \param[in] p_ctrl : matrixbox control
   \param[in] focus : focus index
   \return : NULL
  */
void mbox_set_focus(control_t *p_ctrl, u16 focus);

/*!
   Set icon rect when use same position

   \param[in] p_ctrl : matrixbox control
   \param[in] icon_left : left pos of icon
   \param[in] icon_top : top pos of icon
   \return : NULL
  */
void mbox_set_icon_offset(control_t *p_ctrl, u8 icon_left, u8 icon_top);

/*!
  Set string work mode

  \param[in] p_ctrl : matrixbox control
  \param[in] work_mode : TRUE enable string work
  \return : NULL
  */
void mbox_enable_string_mode(control_t *p_ctrl, BOOL work_mode);

/*!
  Set icon work mode

  \param[in] p_ctrl : matrixbox control
  \param[in] work_mode : TRUE enable icon work
  \return : NULL
  */
void mbox_enable_icon_mode(control_t *p_ctrl, BOOL work_mode);

/*!
   Get item status

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \return : item status
  */
u8 mbox_get_item_status(control_t *p_ctrl, u16 index);

/*!
   Get item rect. didn't realized yet.

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \param[out] p_rect : return item rect
   \return : TRUE/FALSE
  */
BOOL mbox_get_item_rect(control_t *p_ctrl, u16 index, rect_t *p_rect);

/*!
   Get content. didn't realized yet.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : content string
  */
u32 mbox_get_content_str(control_t *p_ctrl, u16 item_idx);

/*!
   Get focus.

   \param[in] p_ctrl : matrixbox control
   \return : focus index
  */
u16 mbox_get_focus(control_t *p_ctrl);

/*!
   Get focus icon.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : focus icon id
  */
u16 mbox_get_focus_icon(control_t *p_ctrl, u16 item_idx);

/*!
   Get normal icon

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : normal icon id
  */
u16 mbox_get_normal_icon(control_t *p_ctrl, u16 item_idx);

/*!
   Get string pos in item.

   \param[in] p_ctrl : matrixbox control
   \param[out] p_left : return string left
   \param[out] p_top : return string top
   \return : NULL
  */
void mbox_get_string_offset(control_t *p_ctrl,
                            u8 *p_left,
                            u8 *p_top);

/*!
   Get icon pos in item.

   \param[in] p_ctrl : matrixbox control
   \param[out] p_left : return icon left
   \param[out] p_top : return icon top
   \return : NULL
  */
void mbox_get_icon_offset(control_t *p_ctrl,
                          u8 *p_left,
                          u8 *p_top);
/*!
   Move focus.

   \param[in] p_ctrl : matrixbox control
   \param[in] focus : new focus
   \return : TRUE or FALSE
  */
BOOL mbox_set_focus_ext(control_t *p_ctrl, u16 focus);

/*!
   Montrol proc

   \param[in] p_ctrl : matrixbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : NULL
  */
RET_CODE mbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#endif
