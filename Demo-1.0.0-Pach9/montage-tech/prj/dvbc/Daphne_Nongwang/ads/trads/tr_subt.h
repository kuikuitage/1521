/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/



#ifndef __SUBT_TR__H__
#define __SUBT_TR__H__





/*!
    ABC
  */
#define TR_SUBT_TASK_STKSIZE     (32*1024)

/*!
    ABC
  */
#define TR_SUBT_TASK_NUM     (5)

/*!
    ABC
  */
#define TR_SUBT_SEM_MAX_NUM    (8)

/*!
    ABC
  */
#define TR_SUBT_TIMER_MAX_NUM    (16)

/*!
    ABC
  */
#define TR_SUBT_CHANNEL_MAX_NUM    (4)

/*!
    ABC
  */
#define TR_SUBT_FILTER_MAX_NUM    (2)

/*!
    ABC
  */
#define TR_SUBT_INVALID_CHANNEL_ID (0xffff)

/*!
    ABC
  */
#define TR_SUBT_DMX_BUF_SIZE  (32 * KBYTES + 188)

#define SUBT_MAX_BUFFER_NUM (8)


/*!
    ABC
  */
typedef enum
{
/*!
    free
  */
    TR_FREE = 0,
/*!
    used
  */
    TR_USED
}tr_status_t;

/*!
    ABC
  */
typedef struct
{
/*!
    ABC
  */
  tr_status_t m_TaskStatus;
/*!
    ABC
  */
  u32 m_uTaskId;
/*!
    ABC
  */
  u32 m_uTaskPrio;
}s_tr_task_struct_t;


/*!
    ABC
  */
typedef struct
{
/*!
    semphore init value
  */
    u32 init_value;
/*!
    wait mode
  */
    u32 wait_mode;
/*!
    semphore id for cas
  */
    u32 sem_id;
/*!
    semphore status
  */
    u32 sem_status;
}tr_sem_node_t;


/*!
    ABC
  */
typedef struct
{
/*!
    timer handle for TR
  */
    s32 timer_handle;
/*!
    timer id
  */
    s32 timer_id;
/*!
    queue status
  */
    u32 timer_status;
}tr_timer_t;

/*!
    ABC
  */
typedef enum
{
/*!
    close
  */
    TR_CLOSE = 0,
/*!
    start
  */
    TR_START,
}tr_dmx_status_t;


/*!
    subt_buffer
  */
typedef struct{
    u32 used;
    u8 filter_buf[TR_SUBT_DMX_BUF_SIZE];
}subt_buffer_t;


/*!
    filter information
  */
typedef struct
{
    u8 used;
    dmx_chanid_t handle;
    dmx_filter_setting_t filter_setting;
    u16 filter_data_size;
    subt_buffer_t *p_filter_buf;
    //u8 *p_data_buf_for_free;
}tr_subt_filter_info_t;

/*!
    channel information
  */
typedef struct
{
    u8 used;
    u16 chan_pid;
    u32 chan_buffer_size;
    u8   filter_num_max;
    u8   filter_num_actual;
    //u16 filter_data_size;
    tr_dmx_status_t chan_status;    
    tr_subt_filter_info_t filter_info[TR_SUBT_FILTER_MAX_NUM];
    void (*upcall_func)(u32 nChannelId,u8 *p_data,u32 nlength);
}tr_subt_channel_info_t;



MTRDRV_VOID tr_subt_monitor_prio_set(MTRDRV_U32 priority);



MTRDRV_VOID MTR_TITLE_Creat_Task(MTRDRV_S32 TitlePriority,MTRDRV_S32 ScreenPriority);

MTRDRV_VOID MTR_TITLE_Message_Stop(MTRDRV_VOID);

MTRDRV_VOID MTR_TT_StopScroll(MTRDRV_VOID);

MTRDRV_VOID MTR_TITLE_Start(MTRDRV_VOID);

MTRDRV_VOID receive_data_task_prio_set(u8 task_prio);

MTRDRV_VOID tr_roll_title_init_set(void);
#endif

