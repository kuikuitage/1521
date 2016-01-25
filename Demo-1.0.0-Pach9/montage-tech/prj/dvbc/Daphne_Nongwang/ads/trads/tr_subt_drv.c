/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"

#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_timer.h"
#include "mtos_printk.h"
#include "charsto.h"

#include "mem_manager.h"
#include "dmx.h"

#include "tr_api.h"
#include "tr_subt.h"
#include "lib_memp.h"
#include "hal_timer.h"

//#define SUBT_TR_DEBUG
//#define SUBT_TR_LIB_DEBUG
//#define SUBT_TR_TEST

#ifdef SUBT_TR_DEBUG
#define    SUBT_TR_PRINTF OS_PRINTF
#else
#define    SUBT_TR_PRINTF DUMMY_PRINTF
#endif

#ifdef SUBT_TR_TEST
#define    SUBT_TR_TEMP OS_PRINTF
#else
#define    SUBT_TR_TEMP DUMMY_PRINTF
#endif


#define MEM_SIZE_TR_TITLE   (1024*1024*2)

static s_tr_task_struct_t s_task_record[TR_SUBT_TASK_NUM] = {{0,},};
static tr_sem_node_t s_sem_list[TR_SUBT_SEM_MAX_NUM] = {{0,0,0,0},};
static tr_timer_t s_tr_timer_list[TR_SUBT_TIMER_MAX_NUM] = {{0,},};
static tr_timer_t s_tr_timer_hal = {0,};
static MTRDRV_U8 queue_index = 0;
static tr_subt_channel_info_t subt_chan_info[TR_SUBT_CHANNEL_MAX_NUM] = {{0,},};
dmx_device_t *p_tr_subt_device = NULL;
u8 g_rec_data_temp[4096] = {0};
static u8 receive_data_task_prio = 0;
static u32 s_subt_pic_lock = 0;
static u32 s_tr_title_lock = 0;
static lib_memp_t g_tr_title_memp;


static subt_buffer_t subt_buffer[SUBT_MAX_BUFFER_NUM];


extern int dmx_jazz_wait_for_data(int timeout);

#define tr_subt_lock()   LOCK(__LINE__)
#define tr_subt_unlock() UNLOCK(__LINE__)
void LOCK(int line)
{
    mtos_sem_take((os_sem_t *)&s_subt_pic_lock, 0);
    //SUBT_TR_PRINTF("taked %d\n", line);
}

void UNLOCK(int line)
{
    //SUBT_TR_PRINTF(" give %d\n", line);
    mtos_sem_give((os_sem_t *)&s_subt_pic_lock);
}

#define tr_title_timer_lock()   TIMER_LOCK(__LINE__)
#define tr_title_timer_unlock() TIMER_UNLOCK(__LINE__)
void TIMER_LOCK(int line)
{
    mtos_sem_take((os_sem_t *)&s_tr_title_lock, 0);
    //SUBT_TR_PRINTF("taked %d\n", line);
}

void TIMER_UNLOCK(int line)
{
    //SUBT_TR_PRINTF(" give %d\n", line);
    mtos_sem_give((os_sem_t *)&s_tr_title_lock);
}

static u16 tr_subt_get_emptybuffer(void)
{
     u16 idx;
     for(idx = 0; idx < SUBT_MAX_BUFFER_NUM; idx++)
     {
     	    if(subt_buffer[idx].used == 0)
                break;
     }
     return idx;
}


MTRDRV_VOID tr_subt_monitor_prio_set(MTRDRV_U32 priority)
{
    MTRDRV_U8 i = 0;

    for(i = 0; i < TR_SUBT_TASK_NUM; i++)
    {
        if(TR_FREE == s_task_record[i].m_TaskStatus)
        {
            s_task_record[i].m_uTaskId = i;
            s_task_record[i].m_uTaskPrio = priority;
            s_task_record[i].m_TaskStatus = TR_USED;
            break;
        }
    }

    return ;
}

static void tr_subt_monitor(void *p_param)
{
    MTRDRV_S16 ret = 0;
    MTRDRV_U32 chan_idx = 0;
    MTRDRV_U32 filter_idx = 0;
    tr_subt_channel_info_t *p_chan_info = NULL;
    tr_subt_filter_info_t *p_filter_info = NULL;
    MTRDRV_U8 *p_data = NULL;
    MTRDRV_U32 data_len = 0;

    SUBT_TR_PRINTF("\n [sub],tr_subt_monitor \n");
    while(1)
    {
        mtos_task_sleep(10);
        //dmx_jazz_wait_for_data(50);
        
        tr_subt_lock();
        for(chan_idx = 0; chan_idx < TR_SUBT_CHANNEL_MAX_NUM; chan_idx++)
        {
            p_chan_info = &subt_chan_info[chan_idx];
            if(0 == p_chan_info->used || TR_CLOSE == p_chan_info->chan_status)
            {
                continue;
            }

            SUBT_TR_PRINTF(" [sub],get filter data, chan_idx=%d ,filter_num_actual=%d \n",
                                                                     chan_idx,p_chan_info->filter_num_actual);
            for(filter_idx = 0; filter_idx < p_chan_info->filter_num_actual; filter_idx++)
            {
                p_data = NULL;
                p_filter_info = &p_chan_info->filter_info[filter_idx];
                MT_ASSERT(NULL != p_filter_info);
                SUBT_TR_PRINTF(" [sub],p_filter_info->handle =%d \n",p_filter_info->handle);

                ret = dmx_si_chan_get_data(p_tr_subt_device, p_filter_info->handle, &p_data, &data_len);

                if(SUCCESS == ret)
                {
                    if(NULL != p_chan_info->upcall_func && data_len > 0 && p_data != NULL)
                    {
                        memcpy(g_rec_data_temp, p_data, data_len);
                        tr_subt_unlock();
                        p_chan_info->upcall_func(chan_idx,g_rec_data_temp,data_len);
                        tr_subt_lock();
                        SUBT_TR_PRINTF("[sub],data_len=%d \n",data_len);
                        SUBT_TR_PRINTF("[sub],data dump \n");
                    }
                }
            }

        }
        tr_subt_unlock();
    }

}

#if 0
static u8 *dmx_memory_malloc(u32 buf_size)
{
   u32 buf = 0;
   buf = (u32)mtos_align_malloc(buf_size, 16);

   return (u8 *)buf;
}

static void dmx_memory_free(u8 *addr)
{
    mtos_align_free(addr);
}
#endif
MTRDRV_VOID receive_data_task_prio_set(u8 task_prio)
{
    receive_data_task_prio = task_prio;
    SUBT_TR_PRINTF("[sub],receive_data_task_prio=%d \n",receive_data_task_prio);
}

MTRDRV_VOID tr_roll_title_init_set(void)
{
  MTRDRV_S32 err = MTRDRV_FAILURE;
  void *pBuf = NULL;
  s32 ret = SUCCESS;
  err = mtos_sem_create((os_sem_t *)&s_tr_title_lock, 1);
  if(!err)
  {
    SUBT_TR_PRINTF("%Ss, create sem error ! \n",__FUNCTION__);
    MT_ASSERT(0);
  }
  
  pBuf = (u32*)mtos_malloc(MEM_SIZE_TR_TITLE);
  MT_ASSERT(pBuf != NULL);

  memset(&g_tr_title_memp, 0, sizeof(lib_memp_t));
  ret = lib_memp_create(&g_tr_title_memp, (u32)pBuf, MEM_SIZE_TR_TITLE);
  MT_ASSERT(ret == SUCCESS);
}

MTRDRV_VOID MTR_TITLE_Creat_ReceiveData_Task(MTRDRV_VOID)
{
    MTRDRV_S32 err = MTRDRV_FAILURE;
    MTRDRV_U32 *p_stack = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_TITLE_Creat_ReceiveData_Task \n");

    p_tr_subt_device = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_tr_subt_device);

    err = mtos_sem_create((os_sem_t *)&s_subt_pic_lock, 1);
    if(!err)
    {
         SUBT_TR_PRINTF("tr subt pic , create sem error ! \n");
         MT_ASSERT(0);
    }

    p_stack = (u32 *)mtos_malloc(TR_SUBT_TASK_STKSIZE);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)"subt_monitor",
                                            tr_subt_monitor,
                                            (void *)0,
                                            receive_data_task_prio,
                                            p_stack,
                                            TR_SUBT_TASK_STKSIZE);

    if(!err)
    {
        SUBT_TR_PRINTF("[sub],MTR_TITLE_Creat_ReceiveData_Task error ! \n");
     }

}

MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateSectionChannel(
                                                                                       MTRDRV_U8 nFilterNumber,
                                                                                       MTRDRV_U32 nChannelBufferSize)
{
    MTRDRV_U32 chan_idx = 0;
    tr_subt_channel_info_t *p_chan_info = NULL;
    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_AllocateSectionChannel \n");
    SUBT_TR_PRINTF("[sub],nFilterNumber=%d,BufferSize=%d \n",nFilterNumber,nChannelBufferSize);

    tr_subt_lock();

    for(chan_idx = 0; chan_idx < TR_SUBT_CHANNEL_MAX_NUM; chan_idx++)
    {
        p_chan_info = &subt_chan_info[chan_idx];
        if(0 == p_chan_info->used)
        {
            break;
        }
    }

    if(chan_idx >= TR_SUBT_CHANNEL_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],allocate channel is full ! \n");
        MT_ASSERT(0);
    }

    MT_ASSERT(NULL != p_chan_info);
    memset(p_chan_info, 0, sizeof(tr_subt_channel_info_t));

    p_chan_info->chan_buffer_size = nChannelBufferSize;
    p_chan_info->filter_num_max = nFilterNumber;
    p_chan_info->used = TRUE;
    tr_subt_unlock();
    return chan_idx;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeChannel(MTRDRV_U32 channelID)
{
    //MTRDRV_U32 ret = 0;
    //MTRDRV_U32 filter_idx = 0;
    tr_subt_channel_info_t *p_chan_info = NULL;
    //u16 filter_idx;
    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_FreeChannel \n");
    SUBT_TR_PRINTF("[sub],channelID=%d \n",channelID);

    tr_subt_lock();

    if(channelID >= TR_SUBT_CHANNEL_MAX_NUM)
    {
        MT_ASSERT(0);
    }

    p_chan_info = &subt_chan_info[channelID];
    if(NULL != p_chan_info)
    {
        #if 0
        for(filter_idx = 0; filter_idx < p_chan_info->filter_num_actual; filter_idx++)
        {
            MTR_DRV_TITLE_DMX_FreeFilter(channelID, filter_idx);
        }
        #endif
        memset(p_chan_info, 0, sizeof(tr_subt_channel_info_t));
        p_chan_info->used = 0;
     }
    
    tr_subt_unlock();
     
    return 0;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateFilter(MTRDRV_U32 channelID,
                                                                                            MTRDRV_U32 nFilterSize)
{
    MTRDRV_U32 filter_idx = 0;
    tr_subt_channel_info_t *p_chan_info = NULL;
    tr_subt_filter_info_t *p_filter_info = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_AllocateFilter \n");
    SUBT_TR_PRINTF("[sub],channelID=%d,nFilterSize=%d \n",channelID,nFilterSize);

    tr_subt_lock();
    p_chan_info = &subt_chan_info[channelID];
    if(channelID >= TR_SUBT_CHANNEL_MAX_NUM
      || !p_chan_info->used)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_DMX_SetChannelPid error! \n");
        MT_ASSERT(0);
        tr_subt_unlock();
        return -1;
    }

    while(1 == p_chan_info->filter_info[filter_idx].used)
    {
        filter_idx++;
    }

    if(filter_idx >= TR_SUBT_FILTER_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],filter is full ! \n");
        MT_ASSERT(0);
    }

    p_filter_info = &p_chan_info->filter_info[filter_idx];
    MT_ASSERT(NULL != p_filter_info);
    memset(p_filter_info, 0, sizeof(tr_subt_filter_info_t));

    p_filter_info->handle = TR_SUBT_INVALID_CHANNEL_ID;
    p_filter_info->filter_data_size = nFilterSize;
    p_filter_info->used =  TRUE;
    
  //  p_chan_info->filter_info[filter_idx] = p_filter_info;
    p_chan_info->filter_num_actual++;
    SUBT_TR_PRINTF("[sub],channelID=%d,filter_num_actual=%d \n",
                                channelID,p_chan_info->filter_num_actual);

    tr_subt_unlock();
    return filter_idx;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeFilter(MTRDRV_U32 channelID,
                                                                                       MTRDRV_U32 filterID)
{
    tr_subt_channel_info_t *p_chan_info = NULL;
    tr_subt_filter_info_t *p_filter_info = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_FreeFilter \n");
    SUBT_TR_PRINTF("[sub],channelID=%d,filterID =%d \n",channelID,filterID);

    if(channelID >= TR_SUBT_CHANNEL_MAX_NUM
        || filterID > TR_SUBT_FILTER_MAX_NUM)
    {
        MT_ASSERT(0);
    }

    tr_subt_lock();
    p_chan_info = &subt_chan_info[channelID];
    
    MT_ASSERT(NULL != p_chan_info);    
    if(NULL != p_chan_info)
    {
        if(0 >= p_chan_info->filter_num_actual)
        {
            tr_subt_unlock();
            SUBT_TR_PRINTF("[sub], all filter has been freed  \n");
            return 0;
        }

        p_filter_info = &p_chan_info->filter_info[filterID];
        if(0 == p_filter_info->used)
        {
            tr_subt_unlock();
            SUBT_TR_PRINTF("[sub], filter[%d] of channel[%d] has been freed  \n",filterID,channelID);
            return 0;
        }

        if(TR_SUBT_INVALID_CHANNEL_ID != p_filter_info->handle)
        {
            SUBT_TR_PRINTF("[sub], dmx_chan_close  p_filter_info->handle=0x%x \n",p_filter_info->handle);
            dmx_chan_stop(p_tr_subt_device, p_filter_info->handle);
            dmx_chan_close(p_tr_subt_device, p_filter_info->handle);
            p_filter_info->handle = TR_SUBT_INVALID_CHANNEL_ID;
            p_filter_info->used = FALSE;
        } 

        if(NULL != p_filter_info->p_filter_buf)
        {
           // dmx_memory_free(p_filter_info->p_filter_buf);
            p_filter_info->p_filter_buf->used = 0;
            p_filter_info->p_filter_buf = NULL;
        }
        p_chan_info->filter_num_actual--;
    }
    tr_subt_unlock();

    return 0;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_SetChannelPid(MTRDRV_U32 channelID,
                                                                                                 MTRDRV_U16 nPid)
{
    //RET_CODE ret = -1;
    //dmx_chanid_t si_channel_id = TR_SUBT_INVALID_CHANNEL_ID;
    //dmx_slot_setting_t slot;
    tr_subt_channel_info_t *p_chan_info = NULL;

    tr_subt_lock();
    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_SetChannelPid \n");
    SUBT_TR_PRINTF("[sub],channelID=%d,nPid =0x%x \n",channelID,nPid);

    if(0 == subt_chan_info[channelID].used
        || channelID >= TR_SUBT_CHANNEL_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_DMX_SetChannelPid error! \n");
        MT_ASSERT(0);
        return -1;
    }

    p_chan_info = &subt_chan_info[channelID];
    p_chan_info->chan_pid = nPid;
    tr_subt_unlock();

    return 0;
}

static RET_CODE MTR_Subt_Set_Filter(tr_subt_filter_info_t *p_filter, u16 pid)
{
    RET_CODE ret = ERR_FAILURE;
    dmx_chanid_t si_channel_id = TR_SUBT_INVALID_CHANNEL_ID;
    dmx_slot_setting_t slot = {0,};
//    u8 *p_data = NULL;
 //   u8 *p_buf_align = NULL;
   subt_buffer_t *p_filter_buf = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_Subt_Set_Filter \n");
    SUBT_TR_PRINTF("\n [sub],set channel pid=0x%x \n",pid);

    MT_ASSERT(NULL != p_tr_subt_device);
    MT_ASSERT(NULL != p_tr_subt_device->p_base);

     memset(&slot, 0, sizeof(dmx_slot_setting_t));
     slot.in = DMX_INPUT_EXTERN0;
     slot.pid   = pid;
     slot.type = DMX_CH_TYPE_SECTION;

     if(TR_SUBT_INVALID_CHANNEL_ID == p_filter->handle)
     {
         ret = dmx_si_chan_open(p_tr_subt_device, &slot, &si_channel_id);
         SUBT_TR_PRINTF("\n [sub],dmx_si_chan_open si_channel_id=0x%x \n",si_channel_id);
         MT_ASSERT(SUCCESS == ret);
         
         p_filter->handle = si_channel_id;         
     }

     if(NULL == p_filter->p_filter_buf)
     {
          ret = tr_subt_get_emptybuffer();
          if(ret < SUBT_MAX_BUFFER_NUM)
            p_filter_buf = &subt_buffer[ret];
         // else
          MT_ASSERT(p_filter_buf != NULL);

          p_filter_buf->used = 1;
          p_filter->p_filter_buf = p_filter_buf;
      //   p_data = (u8 *)dmx_memory_malloc(TR_SUBT_DMX_BUF_SIZE);
      //   MT_ASSERT(NULL != p_data);
         
      //   p_buf_align = (u8 *)(((u32)p_data + 7) & 0xfffffff8);
      //   p_filter->p_filter_buf = p_data;

         //p_filter->p_data_buf_for_free = p_data;
       //  memset(p_filter->p_filter_buf,0,TR_SUBT_DMX_BUF_SIZE);
     }
     ret = dmx_si_chan_set_buffer(p_tr_subt_device, p_filter->handle,
                                        p_filter->p_filter_buf->filter_buf, TR_SUBT_DMX_BUF_SIZE);
     MT_ASSERT(SUCCESS == ret);

     //这几个参数的设置需要研究一下
     p_filter->filter_setting.continuous = TRUE;
     p_filter->filter_setting.en_crc = FALSE;
     /*for ads it must be CONTINUOUS_SECTION*/
     p_filter->filter_setting.req_mode = DMX_REQ_MODE_SECTION_DOUBLE;
     p_filter->filter_setting.ts_packet_mode = DMX_ONE_MODE;

     ret = dmx_si_chan_set_filter(p_tr_subt_device, p_filter->handle, &(p_filter->filter_setting));
     MT_ASSERT(SUCCESS == ret);
          
     return SUCCESS;
}

static RET_CODE MTR_Subt_Enable_Channel(u32 channelID)
{
     tr_subt_channel_info_t *p_chan = NULL;
     tr_subt_filter_info_t *p_filter = NULL;
     u16 filter_idx = 0;
     RET_CODE ret = ERR_FAILURE;

    SUBT_TR_PRINTF("\n [sub],MTR_Subt_Enable_Channel \n");
    
     MT_ASSERT(NULL != p_tr_subt_device);
     MT_ASSERT(NULL != p_tr_subt_device->p_base);
     p_chan = &subt_chan_info[channelID];
     MT_ASSERT(NULL != p_chan);

     for(filter_idx = 0; filter_idx < p_chan->filter_num_actual; filter_idx++)
     {
         p_filter = &p_chan->filter_info[filter_idx];
         MT_ASSERT(NULL != p_filter);
         
         MTR_Subt_Set_Filter(p_filter, p_chan->chan_pid);

         
         if(TR_SUBT_INVALID_CHANNEL_ID != p_filter->handle)
         {
             ret = dmx_chan_start(p_tr_subt_device, p_filter->handle);
             MT_ASSERT(SUCCESS == ret);
         }
     }
    return SUCCESS;
}

static RET_CODE MTR_Subt_Disable_Channel(u32 channelID)
{
     tr_subt_channel_info_t *p_chan = NULL;
     tr_subt_filter_info_t *p_filter = NULL;
     u16 filter_idx = 0;
     RET_CODE ret = ERR_FAILURE;

    SUBT_TR_PRINTF("\n [sub],MTR_Subt_Disable_Channel \n");

     MT_ASSERT(NULL != p_tr_subt_device);
     MT_ASSERT(NULL != p_tr_subt_device->p_base);
     p_chan = &subt_chan_info[channelID];
     
     MT_ASSERT(NULL != p_chan);

     for(filter_idx = 0; filter_idx < p_chan->filter_num_max; filter_idx++)
     {
         p_filter = &p_chan->filter_info[filter_idx];
         if(NULL == p_filter)
             continue;
         if(TR_SUBT_INVALID_CHANNEL_ID != p_filter->handle)
         {
             ret = dmx_chan_stop(p_tr_subt_device, p_filter->handle);
             MT_ASSERT(SUCCESS == ret);

            #if 0
             /*!
               Add these because topreal change pid, but only call disable channel
               We should close channel here
               */
             if(0 >= p_chan->filter_num_actual)
             {
                OS_PRINTF("%s %d All filter in channel %d are freed.\n",__FUNCTION__, __LINE__);
                return 0;
             }
             #endif
             
             dmx_chan_close(p_tr_subt_device, p_filter->handle);
             p_filter->handle = TR_SUBT_INVALID_CHANNEL_ID;
             //if(NULL != p_filter->p_filter_buf)
             //{
                //memset(p_filter->p_filter_buf,0,TR_SUBT_DMX_BUF_SIZE);
             //}

         }
     }

     return SUCCESS;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_ControlChannel(MTRDRV_U32 channelID,
                                                                MTR_DRV_DMX_ChannelAction_t channelAction)
{
    RET_CODE tmp_ret = 1;
    u32 ret = 0;
    tr_subt_channel_info_t *p_chan_info = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_ControlChannel \n");
    SUBT_TR_PRINTF("[sub],channelID=%d,channelAction =%d \n",channelID,channelAction);

    tr_subt_lock();
    if(0 == &subt_chan_info[channelID].used
        || channelID >= TR_SUBT_CHANNEL_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_DMX_SetChannelPid error! \n");
        MT_ASSERT(0);
        return -1;
    }
    p_chan_info = &subt_chan_info[channelID];

    switch(channelAction)
    {
        case MTR_DRV_DMX_START_CHANNEL:
        {
            if(TR_START == p_chan_info->chan_status)
                break;
            tmp_ret = MTR_Subt_Enable_Channel(channelID);
            MT_ASSERT(-1 != tmp_ret);
            if(tmp_ret < 0)
                ret = 1;
            p_chan_info->chan_status = TR_START;
        }
        break;

        case MTR_DRV_DMX_STOP_CHANNEL:
        {
            if(TR_CLOSE == p_chan_info->chan_status)
                break;
            tmp_ret = MTR_Subt_Disable_Channel(channelID);
            MT_ASSERT(-1 != tmp_ret);
            if(tmp_ret < 0)
                ret = 1;
            p_chan_info->chan_status = TR_CLOSE;
        }
        break;

        case MTR_DRV_DMX_RESET_CHANNEL:
        {
            if(TR_START == p_chan_info->chan_status)
            {
                p_chan_info->chan_status = TR_CLOSE;
                tmp_ret = MTR_Subt_Disable_Channel(channelID);
            }
            tmp_ret = MTR_Subt_Enable_Channel(channelID);
            MT_ASSERT(-1 != tmp_ret);
            if(tmp_ret < 0)
                ret = 1;
            p_chan_info->chan_status = TR_START;
         }
         break;

        default:
            SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_DMX_SetChannelPid oter chanaction!\n");
            break;
    }
    tr_subt_unlock();
    
    return 0;
}

MTRDRV_SINT MTR_DRV_TITLE_DMX_SetFilter(MTRDRV_U32 nChannelId,
                                                                                      MTRDRV_U32 nFilterId,
                                                                                      MTRDRV_U8 Table_id,
                                                                                      MTRDRV_U8 Table_idMask,
                                                                                      MTRDRV_U8* puiMatch ,
                                                                                      MTRDRV_U8 *puiMask,
                                                                                      MTRDRV_U8 DataLen)
{
    tr_subt_channel_info_t *p_chan_info = NULL;
    tr_subt_filter_info_t *p_filter_info = NULL;
    MTRDRV_U8 filter_data_index = 0;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_SetFilter \n");
    SUBT_TR_PRINTF("[sub],nChannelId=%d,nFilterId =%d \n",nChannelId,nFilterId);
    SUBT_TR_PRINTF("[sub],Table_id=0x%x,Table_idMask =0x%x \n",Table_id,Table_idMask);
    SUBT_TR_PRINTF("[sub],DataLen =%d \n",DataLen);

    tr_subt_lock();
    if(NULL == puiMatch || NULL == puiMask
        || 0 == subt_chan_info[nChannelId].used
        || nChannelId >= TR_SUBT_CHANNEL_MAX_NUM
        || nFilterId >= TR_SUBT_FILTER_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_DMX_SetFilter error! \n");
        MT_ASSERT(0);
    }

    p_chan_info = &subt_chan_info[nChannelId];
    MT_ASSERT(NULL != p_chan_info);
    p_filter_info = &p_chan_info->filter_info[nFilterId];
    MT_ASSERT(NULL != p_filter_info);

    p_filter_info->filter_setting.value[0] = Table_id;
    p_filter_info->filter_setting.mask[0] = Table_idMask;
    
    SUBT_TR_PRINTF("data:0x%x,    mask:0x%x \n",Table_id,Table_idMask);

    if(DataLen > DMX_SECTION_FILTER_SIZE)
    {
        SUBT_TR_PRINTF("[sub],filter data size more than 12 ! \n");
        MT_ASSERT(0);
    }

    for(filter_data_index = 0; filter_data_index < DataLen; filter_data_index++)
    {
       p_filter_info->filter_setting.mask[filter_data_index + 1] = puiMask[filter_data_index];
       p_filter_info->filter_setting.value[filter_data_index + 1] = puiMatch[filter_data_index];
       SUBT_TR_PRINTF("data:0x%x, mask:0x%x \n",puiMatch[filter_data_index],
                                                                            puiMask[filter_data_index]);
    }
    tr_subt_unlock();
    
    return 0;
}

MTRDRV_U32 MTR_DRV_TITLE_DMX_RegistChannelCallback(MTRDRV_U32 channelID,
           MTRDRV_VOID(* pfnCallBack)(MTRDRV_U32 channelID,MTRDRV_U8* data,MTRDRV_U32 len))
{
    tr_subt_channel_info_t *p_chan_info = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_RegistChannelCallback \n");
    SUBT_TR_PRINTF("[sub],channelID =%d ,pfnCallBack=0x%x \n",channelID,pfnCallBack);

    if(NULL != pfnCallBack && TR_SUBT_CHANNEL_MAX_NUM > channelID)
    {
       tr_subt_lock();
       p_chan_info = &subt_chan_info[channelID];
       MT_ASSERT(NULL != p_chan_info);

       p_chan_info->upcall_func = pfnCallBack;
       tr_subt_unlock();
    }
    return 0;
}

MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskCreate(MTR_DRV_OD_ThreadEntryFunc func,
                                                                                                        PMTR_DRV_OS_VOID arg,
                                                                                                        MTR_DRV_OS_PRIORITY priority,
                                                                                                        PMTR_DRV_OS_VOID stack,
                                                                                                        MTR_DRV_OS_U32 stack_size)
{
    //static MTRDRV_U8 task_num = 0;
    MTRDRV_U8 task_name[5][6] = {"task1","task2","task3","task4","task5"};
    u32 *p_stack = NULL;
    MTRDRV_U8 i = 0;
    MTRDRV_S32 err = MTRDRV_FAILURE;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_TaskCreate \n");
    if(func == NULL || stack == NULL)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_TaskCreate param error ! \n");
        return 0;
    }

    for(i = 0; i < TR_SUBT_TASK_NUM; i++)
    {
        if(TR_FREE == s_task_record[i].m_TaskStatus)
        {
            s_task_record[i].m_uTaskId = priority;
            s_task_record[i].m_uTaskPrio = priority;
            s_task_record[i].m_TaskStatus = TR_USED;
            SUBT_TR_PRINTF("s_task_record[%d].m_uTaskPrio=%d \n",i,priority);
            break;
        }
    }

    if(i >= TR_SUBT_TASK_NUM)
    {
        SUBT_TR_PRINTF("[sub],task id is full !! ! \n");
        MT_ASSERT(0);
    }

    //sprintf(task_name,"task_%d",priority);
    SUBT_TR_PRINTF("[sub], func=0x%x, priority=%d ,task_name:%s \n",func,priority,task_name[i]);
    SUBT_TR_PRINTF("[sub], stack=0x%x, stack_size=%d \n",stack,stack_size);

    p_stack = (u32 *)mtos_malloc(stack_size*2);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)task_name[i],
                                            func,
                                            (void *)arg,
                                            priority,
                                            p_stack,
                                            stack_size*2);

    if(!err)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_TaskCreate error ! \n");
     }

    SUBT_TR_PRINTF("s_task_record[%d].m_uTaskId =%d \n",i,s_task_record[i].m_uTaskId);
    
    return s_task_record[i].m_uTaskId;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskDelete(MTR_DRV_OS_THREADID taskid)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_TaskDelete don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_PRIORITY MTR_DRV_TITLE_OS_TaskGetPriority(MTRDRV_VOID)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_TaskGetPriority don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid,
                                                                                                            MTR_DRV_OS_PRIORITY priority)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_TaskSetPriority don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskGetCurTaskId(MTRDRV_VOID)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_TaskGetCurTaskId don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid)
{
    //u8 i = 0;
    //MTR_DRV_OS_PRIORITY priority = 0;

    SUBT_TR_PRINTF("\n [subt],MTR_DRV_TITLE_OS_TaskSuspend taskid = %d \n",taskid);

#if 0
    for(i = 0; i < TR_SUBT_TASK_NUM; i++)
    {
        if(TR_USED == s_task_record[i].m_TaskStatus
            && s_task_record[i].m_uTaskId == taskid)
        {
            SUBT_TR_PRINTF("s_task_record[%d].m_uTaskPrio=%d \n",i,priority);
            break;
        }
    }

    if(i >= TR_SUBT_TASK_NUM)
    {
        SUBT_TR_PRINTF("[sub],task id is full !! ! \n");
        MT_ASSERT(0);
    }
#endif

    //priority = s_task_record[taskid].m_uTaskPrio;
    mtos_task_suspend(taskid);

    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskResume(MTR_DRV_OS_THREADID taskid)
{
    //MTR_DRV_OS_PRIORITY priority = 0;

    SUBT_TR_PRINTF("\n [subt],MTR_DRV_TITLE_OS_TaskResume taskid = %d \n",taskid);

    //priority = s_task_record[taskid].m_uTaskPrio;
    mtos_task_resume(taskid);

    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds)
{

    //SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_TaskSleep=%d  \n",dMilliSeconds);

    mtos_task_sleep(dMilliSeconds);

    return MTR_DRV_OS_OK;

}

MTR_DRV_OS_SEMPHORE MTR_DRV_TITLE_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no)
{
    MTR_DRV_OS_U8 i = 0;
    MTRDRV_S32 err = MTRDRV_FAILURE;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_SemaphoreCreate \n");
    SUBT_TR_PRINTF("[sub],semphore_init_no = %d \n",semphore_init_no);
    
    for(i = 0; i < TR_SUBT_SEM_MAX_NUM; i++)
    {
        if(s_sem_list[i].sem_status == TR_FREE)
        {
            s_sem_list[i].sem_id = 0;
            s_sem_list[i].init_value = semphore_init_no;
            s_sem_list[i].sem_status = TR_USED;
            break;
        }
    }

    if(i > TR_SUBT_SEM_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],semph is full !! ! \n");
        MT_ASSERT(0);
    }

    err = mtos_sem_create((os_sem_t *)&s_sem_list[i].sem_id, semphore_init_no);
    if (!err)
    {
        SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_SemaphoreCreate error !\n");
         return 0;
    }

    SUBT_TR_PRINTF("[sub],s_sem_list[%d].sem_id = %d \n",i,s_sem_list[i].sem_id);
    return s_sem_list[i].sem_id;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_SemaphoreDelete don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,
                                                                MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time)
{
    MTR_DRV_OS_U8 i = 0;
    MTRDRV_S32 err = MTRDRV_FAILURE;
    os_sem_t *p_Semaphore = (os_sem_t *)&semphoreid;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_SemaphoreGet \n");
    SUBT_TR_PRINTF("[sub],semphoreid=%d ,wait_ornot=%d,wait_time=%d \n",semphoreid,wait_ornot,wait_time);

    for(i = 0; i < TR_SUBT_SEM_MAX_NUM; i++)
    {
        if(s_sem_list[i].sem_status == TR_USED
            && s_sem_list[i].sem_id == semphoreid)
        {
            break;
        }
    }

    if(i >= TR_SUBT_SEM_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],semph don't get !! ! \n");
        MT_ASSERT(0);
    }

#if 0
     if(MTR_DRV_OS_NOT_WAIT == wait_ornot)
     {
           err = mtos_sem_take(p_Semaphore, 1);
            if (!err)
            {
                SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_SemaphoreGet error !\n");
                return MTR_DRV_OS_ERR;
            }
     }
     else if(MTR_DRV_OS_WAIT == wait_ornot)
     {
           err = mtos_sem_take(p_Semaphore, wait_time);
            if (!err)
            {
                SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_SemaphoreGet error !\n");
                return MTR_DRV_OS_TIMEOUT;
            }
     }
#else
       err = mtos_sem_take(p_Semaphore, wait_time);
       if (!err)
       {
            SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_SemaphoreGet error !\n");
            return MTR_DRV_OS_TIMEOUT;
        }
#endif
    
     return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreRelease(
                                                            MTR_DRV_OS_SEMPHORE semphoreid)
{
    MTR_DRV_OS_U8 i = 0;
    MTRDRV_S32 err = MTRDRV_FAILURE;
    os_sem_t *p_Semaphore = (os_sem_t *)&semphoreid;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_SemaphoreRelease \n");
    SUBT_TR_PRINTF("[sub],release semphoreid=%d \n",semphoreid);

    for(i = 0; i < TR_SUBT_SEM_MAX_NUM; i++)
    {
        if(s_sem_list[i].sem_status == TR_USED
            && s_sem_list[i].sem_id == semphoreid)
        {
            break;
        }
    }

    if(i >= TR_SUBT_SEM_MAX_NUM)
    {
        SUBT_TR_PRINTF("[sub],semph don't get !! ! \n");
        MT_ASSERT(0);
    }

    err = mtos_sem_give(p_Semaphore);
    if (!err)
    {
        SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_SemaphoreRelease error !\n");
         return MTR_DRV_OS_ERR;
    }

    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_QUEUEID MTR_DRV_TITLE_OS_QueueCreate(MTR_DRV_OS_U32 queue_element)
{
    u32 ret = 0;
    //MTRDRV_S32 err = MTRDRV_FAILURE;
    MTRDRV_S32 message_id = 0;
    //MTRDRV_VOID *p_quenue_buf = NULL;
    //MTRDRV_U32 quenue_buf_size = 0;
    MTRDRV_U8 queue_name[4][8] = {"queue1","queue2","queue3","queue4"};
    //MTRDRV_U8 *p_name = NULL;
    void *p_message_buf = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_QueueCreate message_num=%d \n",queue_element);
    SUBT_TR_PRINTF("[sub],queue_name[%d]:%s \n",queue_index,(u8 *)queue_name[queue_index]);
    
    //sprintf((u8 *)queue_name,"qname_%d",queue_index);
    if(queue_index > 3)
    {
        SUBT_TR_PRINTF("[sub],queue > 3  \n");
        MT_ASSERT(0);
    }

    message_id = mtos_messageq_create(queue_element,(u8 *)queue_name[queue_index]);
    MT_ASSERT(message_id != INVALID_MSGQ_ID);

    p_message_buf = mtos_malloc(sizeof(MTR_DRV_OS_Msg) * queue_element);
    MT_ASSERT(p_message_buf != NULL);
    memset(p_message_buf, 0 ,sizeof(MTR_DRV_OS_Msg) * queue_element);

    ret = mtos_messageq_attach(message_id,
                                                    p_message_buf,
                                                    sizeof(MTR_DRV_OS_Msg),
                                                    queue_element);

    if(ret == FALSE)
    {
        mtos_free(p_message_buf);
        SUBT_TR_PRINTF("[sub],attach message queue failed !  \n");
        return 0;
    }
    
    //TR request message_id > 0, but we message_id >= 0.
    message_id += 10;
    queue_index++;

    SUBT_TR_PRINTF("[sub],message_id=%d \n",message_id);
    
    return message_id;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid)
{
    SUBT_TR_PRINTF("\n MTR_DRV_TITLE_OS_QueueDelete don't be used \n");
    return MTR_DRV_OS_OK;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueSendMessage(
                                                                        MTR_DRV_OS_QUEUEID queueid,
                                                                        MTR_DRV_OS_VOID * pmessage)
{
    MTRDRV_S32 err = MTRDRV_FAILURE;
    MTRDRV_S32 message_id = 0;
    os_msg_t msg = {0};

    //SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_QueueSendMessage \n");
    if(pmessage == NULL)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_QueueSendMessage pmessage = NULL !\n");
        return MTR_DRV_OS_ERR;
    }

    message_id = queueid - 10;
    //SUBT_TR_PRINTF("[sub],MTR_QueueSendMessage queueid=%d ,pmessage=0x%x \n",message_id,pmessage);
    if(message_id > MTOS_MAX_MSGQ)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_QueueSendMessage queueid=%d message_id =%d > 32 !\n",queueid, message_id);
        return SUCCESS;
    }

    msg.is_ext = 1;
    msg.para1 = (u32)pmessage;
    msg.para2 = sizeof(MTR_DRV_OS_Msg);

    #ifdef SUBT_TR_DEBUG
    u8 i = 0;
    MTR_DRV_OS_Msg *p_msg;
    p_msg = (MTR_DRV_OS_Msg*)pmessage;
    
    SUBT_TR_PRINTF("\n [sub],Send msg, Type=%d, Len=%d \n data:",p_msg->Type,p_msg->Length);
    for(i = 0; i < p_msg->Length; i++)
    {
        SUBT_TR_PRINTF(" 0x%x ",p_msg->DataBuffer[i]);
    }
    SUBT_TR_PRINTF("\n");
    #endif
    
    err = mtos_messageq_send(message_id, &msg);
    if(!err)
    {
         SUBT_TR_PRINTF("MTR_DRV_TITLE_OS_QueueSendMessage, send msg error ! \n");
         MT_ASSERT(0);
         return MTR_DRV_OS_ERR;
    }

    return SUCCESS;
}

PMTR_DRV_OS_VOID MTR_DRV_TITLE_OS_QueueReceiveMessage(
                                                                        MTR_DRV_OS_QUEUEID queueid,
                                                                        MTR_DRV_OS_WAITMODE wait_ornot,
                                                                        MTR_DRV_OS_U32 wait_time)
{
    MTRDRV_S32 err = MTRDRV_FAILURE;
    MTRDRV_VOID * pmessage = NULL;
    MTRDRV_S32 message_id = 0;
    os_msg_t msg = {0,};

    //SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_QueueReceiveMessage \n");

    message_id = queueid - 10;
    //SUBT_TR_PRINTF("message_id:%d,wait_mode:%d, wait_time:%d  \n",message_id,wait_ornot,wait_time);

    err = mtos_messageq_receive(message_id,&msg,0);    
    if(!err)
    {
         SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_QueueReceiveMessage error ! \n");
         return 0;
    }

    pmessage = (MTRDRV_VOID*)msg.para1;
    #ifdef SUBT_TR_DEBUG
    u8 i = 0;
    MTR_DRV_OS_Msg *p_msg;
    p_msg = (MTR_DRV_OS_Msg*)pmessage;    
    
    SUBT_TR_PRINTF("\n [sub],Rev msg, Type=%d, Len=%d \n data:",p_msg->Type,p_msg->Length);
    for(i = 0; i < p_msg->Length; i++)
    {
        SUBT_TR_PRINTF(" 0x%x ",p_msg->DataBuffer[i]);
    }
    SUBT_TR_PRINTF("\n");
    #endif

    //SUBT_TR_PRINTF("[sub],message_id=%d, pmessage=0x%x \n",message_id,pmessage);

    return (PMTR_DRV_OS_VOID)(msg.para1);
}

MTR_DRV_OS_VOID *MTR_DRV_TITLE_OS_MemoryAllocate(MTR_DRV_OS_U32 size)
{
    void *pBuf = NULL;

    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_MemoryAllocate size %d\n", size);

    #if 1
    pBuf = lib_memp_alloc(&g_tr_title_memp, size);
    //pBuf = mtos_malloc(size);
    if(NULL == pBuf)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_MemoryAllocate error \n");
        //MT_ASSERT(0);
        return MTR_DRV_NULL;
    }
    #else
    mem_mgr_alloc_param_t param = {0};
    param.id = MEM_ADS_PARTITION;
    param.size = size;
    param.user_id = 18;
    pBuf = mem_mgr_alloc(&param);
    MT_ASSERT(pBuf != NULL);
    #endif
    SUBT_TR_PRINTF("[sub], allocate buf address=0x%x,size:%d \n",pBuf,size);
    return pBuf;
}

MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_MemoryFree(MTR_DRV_OS_VOID *paddress)
{
    //SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_MemoryFree \n");

    #if 1
    if(NULL == paddress)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_MemoryFree paramter error !\n");
        return MTR_DRV_OS_ERR;
    }
    //mtos_free(paddress);
    lib_memp_free(&g_tr_title_memp, paddress);
    #else
    mem_mgr_free_param_t param = {0};
    param.id = MEM_ADS_PARTITION;
    param.p_addr = (void *)paddress;
    param.user_id = 18;
    mem_mgr_free(&param);
    #endif
    SUBT_TR_PRINTF("[sub], free buf address=0x%x \n",paddress);
    return MTR_DRV_OS_OK;
}

MTRDRV_VOID* MTR_DRV_TITLE_OS_MemoryCopy( MTRDRV_VOID* pdst,
                                                                                                MTRDRV_VOID* psrc,
                                                                                                MTRDRV_U32 bytes )
{
    //SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_MemoryCopy \n");
    
    if(NULL == pdst || NULL == psrc)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_MemoryCopy paramter error !\n");
        return NULL;
    }
    memcpy(pdst,psrc,bytes);

    SUBT_TR_PRINTF("[sub], memcpy psrc=0x%x,pdst=0x%x, bytes:%d \n",psrc,pdst,bytes);
    return pdst;
}

MTRDRV_VOID MTR_DRV_TITLE_OS_SystemHalt(MTRDRV_VOID)
{
    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_SystemHalt !\n");

}

MTRDRV_VOID* MTR_DRV_TITLE_OS_MemorySet( MTRDRV_VOID* mem,
                                                                                            MTRDRV_U8 value,
                                                                                            MTRDRV_U32 bytes )
{
    SUBT_TR_PRINTF("\n [sub],MTR_DRV_TITLE_OS_MemorySet \n");

    if(NULL == mem)
    {
        SUBT_TR_PRINTF("[sub],MTR_DRV_TITLE_OS_MemorySet paramter error !\n");
        return NULL;
    }
    memset(mem,value,bytes);

    SUBT_TR_PRINTF("[sub], memory set address=0x%x,value=0x%x, size:%d \n",mem,value,bytes);

    return mem;
}

MTRDRV_S32  MTR_DRV_TITLE_TIMER_Create(MTRDRV_HANDLE* handle)
{
    MTRDRV_U8 i = 0;
    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Create \n");

    tr_title_timer_lock();
    if(NULL == handle)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Create paramter error !\n");
        tr_title_timer_unlock();
        return -1;
    }

    for(i = 0; i < TR_SUBT_TIMER_MAX_NUM; i++)
    {
        if(s_tr_timer_list[i].timer_status == TR_FREE)
        {
            s_tr_timer_list[i].timer_status = TR_USED;
            s_tr_timer_list[i].timer_id = -1;
            //TR request return timer_id > 0
            s_tr_timer_list[i].timer_handle = i + 10;
            *handle = i + 10;
    
            SUBT_TR_TEMP("\n [sub],create timer handle=%d  \n",*handle);
            tr_title_timer_unlock();
            return 0;
        }
    }

    tr_title_timer_unlock();
    return -1;
}

MTRDRV_S32	MTR_DRV_TITLE_TIMER_Free(MTRDRV_HANDLE* handle)
{
    MTRDRV_U32 index = 0;
    MTRDRV_S32 timer_id = -1;

    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Free \n");
    SUBT_TR_TEMP("[sub],free timer handle=%d  \n",*handle);

    tr_title_timer_lock();
    
    if(NULL == handle)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Free paramter error !\n");
        tr_title_timer_unlock();
        return -1;
    }

    #if 0
    index = *handle - 10;
    #else
    for(index = 0; index < TR_SUBT_TIMER_MAX_NUM; index++)
    {
        if(s_tr_timer_list[index].timer_status == TR_USED
            && s_tr_timer_list[index].timer_handle == *handle)
            break;
    }

    if(index >= TR_SUBT_TIMER_MAX_NUM)
    {
        SUBT_TR_TEMP("=FREE=don't found request free handle=%d \n", *handle);
        //MT_ASSERT(0);
        tr_title_timer_unlock();
        return 0;
    }
    #endif
    
    timer_id = s_tr_timer_list[index].timer_id;
    mtos_timer_delete(timer_id);
    SUBT_TR_TEMP("[sub],free timer index=%d, handle=%d, timer_id=%d \n ",
                                                       index,s_tr_timer_list[index].timer_handle,timer_id);
    
    s_tr_timer_list[index].timer_status = TR_FREE;
    s_tr_timer_list[index].timer_id = -1;
    s_tr_timer_list[index].timer_handle = -1;
    *handle = 0;
    tr_title_timer_unlock();
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_SetParams(MTRDRV_HANDLE* handle,
                                                                                             MTRDRV_U32 TimeValue,
                                                                                             MTRDRV_U32 TimerMode,
                                                                                             MTRDRV_VOID (*CallBackFunc)())
{
    MTRDRV_S32 timer_id = -1;
    MTRDRV_U32 index = 0;
    MTRDRV_S32 time_mode = FALSE;

    tr_title_timer_lock();
    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_SetParams \n");
    if(NULL == handle || NULL == CallBackFunc)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_SetParams paramter error !\n");
        tr_title_timer_unlock();
        return -1;
    }
    SUBT_TR_TEMP("handle=%d,timevalue=%d,timermode=%d,CallBackFunc=0x%x \n",
                                *handle, TimeValue, TimerMode, CallBackFunc);

    #if 0
    index = *handle - 10;
    #else
    for(index = 0; index < TR_SUBT_TIMER_MAX_NUM; index++)
    {
        if(s_tr_timer_list[index].timer_status == TR_USED
            && s_tr_timer_list[index].timer_handle == *handle)
            break;
    }

    if(index >= TR_SUBT_TIMER_MAX_NUM)
    {
        SUBT_TR_TEMP("=SetParam=don't found request free handle=%d \n", *handle);
        //MT_ASSERT(0);
        tr_title_timer_unlock();
        return 0;
    }

    if(TimeValue <= 0)
    {
        s_tr_timer_list[index].timer_status = TR_FREE;
        s_tr_timer_list[index].timer_id = -1;
        s_tr_timer_list[index].timer_handle = -1;
        tr_title_timer_unlock();
        
        return 0;
    }
    #endif

    if(TimerMode == MTR_DRV_TIMER_ONE_SHOT)
    {
        time_mode = FALSE;
    }
    else if(TimerMode == MTR_DRV_TIMER_REPEAT)
    {
        time_mode = TRUE;
    }

    if(s_tr_timer_list[index].timer_id < 0)
    {
    #ifndef WIN32
        timer_id = mtos_timer_create(TimeValue,CallBackFunc,0,time_mode);
    #endif
        if(timer_id < 0)
        {
            SUBT_TR_TEMP("[sub],create timer error ! \n");
            //return -1;
            MT_ASSERT(0);
        }
        s_tr_timer_list[index].timer_id = timer_id;
    }
    else
    {
         mtos_timer_reset(s_tr_timer_list[index].timer_id,TimeValue);
         SUBT_TR_TEMP("[sub],======mtos_timer_reset ! \n");
    }
    
    SUBT_TR_TEMP("[sub],create handle=%d, timer_id =%d  \n",
                                s_tr_timer_list[index].timer_handle,s_tr_timer_list[index].timer_id);
    tr_title_timer_unlock();
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_Start(MTRDRV_HANDLE* handle)
{
    MTRDRV_U32 index = 0;
    MTRDRV_S32 timer_id = -1;

    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Start handle=%d \n", *handle);

    tr_title_timer_lock();
    
    #if 0
    index = *handle - 10;
    if(s_tr_timer_list[index].timer_status != TR_USED)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Start error !\n");
        tr_title_timer_unlock();
        return -1;
    }

    #else
    for(index = 0; index < TR_SUBT_TIMER_MAX_NUM; index++)
    {
        if(s_tr_timer_list[index].timer_status == TR_USED
            && s_tr_timer_list[index].timer_handle == *handle)
            break;
    }

    if(index >= TR_SUBT_TIMER_MAX_NUM)
    {
        SUBT_TR_TEMP("=start=don't found request free handle=%d \n", *handle);
        //MT_ASSERT(0);
        tr_title_timer_unlock();
        return 0;
    }
    #endif
    
    timer_id = s_tr_timer_list[index].timer_id;

    mtos_timer_start(timer_id);

    SUBT_TR_TEMP("[sub],start timer handle=%d, timer_id =%d  \n",
                                s_tr_timer_list[index].timer_handle,s_tr_timer_list[index].timer_id);
    tr_title_timer_unlock();
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_Stop(MTRDRV_HANDLE* handle)
{
    MTRDRV_U32 index = 0;
    MTRDRV_S32 timer_id = -1;
    MTRDRV_S32 err = MTRDRV_FAILURE;

    SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Stop handle=%d \n", *handle);
    tr_title_timer_lock();

    #if 0
    index = *handle - 10;
    if(s_tr_timer_list[index].timer_status != TR_USED)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Stop error1 !\n");
        tr_title_timer_unlock();
        return -1;
    }
    #else
    for(index = 0; index < TR_SUBT_TIMER_MAX_NUM; index++)
    {
        if(s_tr_timer_list[index].timer_status == TR_USED
            && s_tr_timer_list[index].timer_handle == *handle)
            break;
    }

    if(index >= TR_SUBT_TIMER_MAX_NUM)
    {
        SUBT_TR_TEMP("=stop=don't found request free handle=%d \n", *handle);
        //MT_ASSERT(0);
        tr_title_timer_unlock();
        return 0;
    }
    #endif
    
    timer_id = s_tr_timer_list[index].timer_id;

    err = mtos_timer_stop(timer_id);
    if(SUCCESS != err)
    {
         SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Stop timer_id=%d error2 ! \n",timer_id);
         tr_title_timer_unlock();
         return -1;
    }

    SUBT_TR_TEMP("[sub],stop timer handle=%d, timer_id =%d  \n",
                                s_tr_timer_list[index].timer_handle,s_tr_timer_list[index].timer_id);
    tr_title_timer_unlock();
    return 0;
}

MTRDRV_S32  MTR_DRV_TITLE_TIMER_Create_Halt(MTRDRV_HANDLE* handle)
{
    MTRDRV_U8 i = 0;
    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Create_Halt \n");
    
    if(NULL == handle)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Create_Halt paramter error !\n");
        return -1;
    }

    if(s_tr_timer_hal.timer_status == TR_FREE)
    {
        s_tr_timer_hal.timer_status = TR_USED;
        s_tr_timer_hal.timer_id = -1;
        //TR request return timer_id > 0
        s_tr_timer_hal.timer_handle = i + 10;
        

        SUBT_TR_TEMP("\n [sub],create timer handle=%d  \n",*handle);
    }
    *handle = s_tr_timer_hal.timer_handle;
    return 0;
}

MTRDRV_S32	MTR_DRV_TITLE_TIMER_Free_Halt(MTRDRV_HANDLE* handle)
{
    s32 timer_id = -1;

    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Free_Halt \n");
    SUBT_TR_TEMP("[sub],free timer handle=%d  \n",*handle);

    if(NULL == handle)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Free_Halt paramter error !\n");
        return -1;
    }
    timer_id = s_tr_timer_hal.timer_id;
    hal_timer_stop((u8)timer_id);
    *handle = 0;
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_SetParams_Halt(MTRDRV_HANDLE* handle,
                                               MTRDRV_U32 TimeValue,
                                               MTRDRV_U32 TimerMode,
                                               MTRDRV_VOID (*CallBackFunc)(void))
{
    s32 timer_id = -1;
    BOOL time_mode = FALSE;
    TimeValue = 20;
    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_SetParams_Halt \n");
    if(NULL == handle || NULL == CallBackFunc)
    {
        SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_SetParams_Halt paramter error !\n");
        return -1;
    }
    SUBT_TR_TEMP("handle=%d,timevalue=%d,timermode=%d,CallBackFunc=0x%x \n",
                                *handle, TimeValue, TimerMode, CallBackFunc);

    if(TimerMode == MTR_DRV_TIMER_ONE_SHOT)
    {
        time_mode = FALSE;
    }
    else if(TimerMode == MTR_DRV_TIMER_REPEAT)
    {
        time_mode = TRUE;
    }

    if(s_tr_timer_hal.timer_id < 0)
    {
        timer_id = hal_timer_request(TimeValue, CallBackFunc, time_mode);
        if(timer_id < 0)
        {
            SUBT_TR_TEMP("[sub],create timer error ! \n");
            //return -1;
            MT_ASSERT(0);
        }
        s_tr_timer_hal.timer_id = timer_id;
    }
    SUBT_TR_TEMP("[sub],create handle=%d, timer_id =%d  \n",
                                s_tr_timer_hal.timer_handle,s_tr_timer_hal.timer_id);
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_Start_Halt(MTRDRV_HANDLE* handle)
{
    s32 timer_id = -1;
    u32 frame_num = 0;
    SUBT_TR_TEMP("\n [sub],MTR_DRV_TITLE_TIMER_Start_Halt handle=%d \n", *handle);

    timer_id = s_tr_timer_hal.timer_id;
    frame_num = *((volatile u32 *)0x6F910000) & 0xFFFF0000;
    while (frame_num == (*((volatile u32 *)0x6F910000) & 0xFFFF0000))
    {;}
    mtos_task_delay_ms(5);
    hal_timer_start((u8)timer_id);
    SUBT_TR_TEMP("[sub],start timer handle=%d, timer_id =%d  \n",
                                s_tr_timer_hal.timer_handle,s_tr_timer_hal.timer_id);
    return 0;
}

MTRDRV_S32 MTR_DRV_TITLE_TIMER_Stop_Halt(MTRDRV_HANDLE* handle)
{
    s32 timer_id = -1;

    SUBT_TR_TEMP("[sub],MTR_DRV_TITLE_TIMER_Stop_Halt handle=%d \n", *handle);

    timer_id = s_tr_timer_hal.timer_id;
    hal_timer_stop((u8)timer_id);

    SUBT_TR_TEMP("[sub],stop timer handle=%d, timer_id =%d  \n",
                                s_tr_timer_hal.timer_handle,s_tr_timer_hal.timer_id);
    return 0;
}

typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);

MTRDRV_SINT MTR_DRV_TITLE_Print (MTRDRV_CHAR *format, ...)
{
#ifdef SUBT_TR_LIB_DEBUG
    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200] = {0};

    CK_VA_START(p_args, format);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);

    CK_VA_END(p_args);
    mtos_printk(printk_buf);
#endif
    return MTRDRV_OK;
}
//fix the timer destory issue ---Temporary solution
void MTR_DRV_TITLE_Timer_Process(u32 para1)
{
  u8 i=0;
  for(i=0;i<MTOS_MAX_SYSTIMER;i++)
    {
       if( s_tr_timer_list[i].timer_id == (s32)para1)
        {
          
            s_tr_timer_list[i].timer_status = TR_FREE;
            s_tr_timer_list[i].timer_id = -1;
            s_tr_timer_list[i].timer_handle = -1;
            //OS_PRINTK("\n TR subt delete the destroyed timerid");
         }
    }
}