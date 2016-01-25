/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

/*!
  subt policy
  */
typedef struct
{
  /*!
    on initialized
    */
  void (*on_init)(void);
} subt_policy_t;


/*!
  command id list
  */
typedef enum
{
  /*!
    Stop play
    */
  SUBT_PIC_CMD_START = ASYNC_CMD,
  
  SUBT_PIC_CMD_STOP,
  
  SUBT_PIC_CMD_START_SYNC = SYNC_CMD,
  
  SUBT_PIC_CMD_STOP_SYNC,
 
}subt_pic_cmd_t;


typedef enum
{
  /*!
    the first event of pb module,
    if create new event, you must insert it between BEGIN and END
    */
  SUBT_PIC_EVT_BEGIN = ((APP_RESERVED2<< 16) | ASYNC_EVT),
 
  /*!
    ack the sync cmd PB_CMD_SET_MUTE
    */
  SUBT_PIC_EVT_START = ((APP_RESERVED2 << 16) | SYNC_EVT),
  SUBT_PIC_EVT_STOP,  
  SUBT_PIC_EVT_END
} subt_pic_evt_t;

app_t *construct_ap_tr_subt(subt_policy_t *p_policy);


