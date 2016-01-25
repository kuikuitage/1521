/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __GUI_ROLL_H__
#define __GUI_ROLL_H__

/*!
  roll style
  */
typedef enum
{
  /*!
    left to right
    */
  ROLL_LR = 0,
  /*!
    right to left
    */
  ROLL_RL,
  /*!
    left right cycle
    */
  ROLL_LRC,
  /*!
    top to bottom
    */
  ROLL_TB,
  /*!
    bottom to top
    */
  ROLL_BT,
  /*!
    top bottom cycle.
    */
  ROLL_TBC,
}roll_style_t;

/*!
  roll pace
  */
typedef enum
{
  /*!
    pace/1
    */
  ROLL_SINGLE = 1,
  /*!
    pace/2
    */
  ROLL_DOUBLE,
  /*!
    pace/3
    */
  ROLL_TRIPLE,
  /*!
    pace/4
    */
  ROLL_FOURFD,
}roll_pace_t;

/*!
  roll param
  */
typedef struct
{
  /*!
    style
    */
  roll_style_t style;
  /*!
    pace
    */
  roll_pace_t pace;
  /*!
    repeat times, 0 means forever
    */
  u32 repeats;
}roll_param_t;

/*!
  roll node
  */
typedef struct roll_node
{
  /*!
    blt x
    */
  s16 blt_x;
  /*!
    blt y
    */
  s16 blt_y;
  /*!
    rgn width
    */
  u16 rgn_w;
  /*!
    rgn height
    */
  u16 rgn_h;
  /*!
    control.
    */
  control_t *p_ctrl;
  /*!
    context
    */
  u32 context;
  /*!
    curn counts, for caculate pace.
    */
  u16 curn;
  /*!
    curn repeat times, for caculate repeat times.
    */
  u16 curn_times;
  /*!
    just for lrc & tbc to remeber direction.
    */
  BOOL direct;
  /*!
    dst rectangle
    */
  rect_t dst_rc;
  /*!
    surface handle
    */
  handle_t handle;
  /*!
    surface address
    */
  u32 addr;
  /*!
    roll parameter
    */
  roll_param_t param;
  /*!
    next roll node
    */
  struct roll_node *p_next;
}roll_node_t;

/*!
  create surface for roll & add control into roll list.

  \param[in] p_ctrl       control for roll
  \param[in] fstyle_idx   fstyle index
  \param[in] p_str        string
  \param[in] p_rc         src rect
  \param[in] draw_style   draw style
  \param[in] style        roll style
  \param[in] pace         roll pace
  \param[in] context      for some special use.

  \return NULL
  */
void gui_create_rsurf(control_t *p_ctrl, u8 fstyle_idx, u16 *p_str,
  rect_t *p_rc, u32 draw_style, roll_param_t *p_param, u32 context);

/*!
  start roll

  \param[in] p_ctrl       control for roll
  \param[in] style        roll style
  \param[in] pace        roll pace

  \return NULL
  */
void gui_start_roll(control_t *p_ctrl, roll_param_t *p_param);

/*!
  stop roll

  \param[in] p_ctrl       control for roll

  \return NULL
  */
void gui_stop_roll(control_t *p_ctrl);

/*!
  rolling

  \return NULL
  */
void gui_rolling(void);

/*!
  roll init

  \param[in] max_cnt                max roll control cnt.
  \param[in] pps                        pixel per step.

  \return NULL
  */
void gui_roll_init(u8 max_cnt, u8 pps);

/*!
  roll release
  */
void gui_roll_release(void);

/*!
  gui rolling node

  \param[in] p_node       node for roll
  \param[in] hdc          dc for roll

  \return NULL
  */
void gui_rolling_node(roll_node_t *p_node, hdc_t hdc);

/*!
  control is rolling.

  \param[in] p_ctrl : control

  \return TRUE or FALSE
  */
BOOL ctrl_is_rolling(control_t *p_ctrl);

/*!
  reset roll content

  \param[in] p_ctrl : control

  \return NULL
  */
void gui_roll_reset_content(control_t *p_ctrl);
#endif
