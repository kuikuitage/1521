/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "db_dvbs.h"
#include "ap_framework.h"
#include "ap_satcodx.h"
#include "ap_satcodx_common.h"
#include "ap_satcodx_recv.h"
#include "ap_satcodx_parse.h"
#include "hal_uart.h"
#if WIN32
#include "uart_win32.h"
#endif
/*!
  \file ap_satcodx_parse.c
  This is the implementation of receive module in Satcodx app.  
*/
BOOL ap_satcodx_recv_alloc_buf(satcodx_impl_data_t *p_satcodx_impl_data)
{  
  /*
  p_satcodx_impl_data->p_static_buf =
		satcodx_sys_malloc(
      p_satcodx_impl_data,
      p_satcodx_impl_data->max_buf_size);
  */
  p_satcodx_impl_data->p_static_buf = 
    satcodx_block_malloc(
      p_satcodx_impl_data, 
      &(p_satcodx_impl_data->max_buf_size));
  if(p_satcodx_impl_data->p_static_buf != NULL)
  {
   return TRUE;
  }
  else
  {   
   return FALSE;
  }
}
void  ap_satcodx_recv_free_buf(satcodx_impl_data_t *p_satcodx_impl_data)
{
 if(p_satcodx_impl_data->p_static_buf)
 {
   //satcodx_sys_free(p_satcodx_impl_data, p_satcodx_impl_data->p_static_buf);
   satcodx_block_free(p_satcodx_impl_data, p_satcodx_impl_data->p_static_buf);
   p_satcodx_impl_data->p_static_buf = NULL;
 }
}
BOOL ap_satcodx_recv_alloc_uart(satcodx_impl_data_t *p_satcodx_impl_data)
{
#if WIN32
    uartwin_init(1);
    return TRUE;
#else
    //close shell uart interface!!!
    //clear uart fifo
    uart_flush(UART_ID_0);
    return TRUE;
#endif
}
void  ap_satcodx_recv_free_uart(satcodx_impl_data_t * p_satcodx_impl_data)
{
 #if WIN32
 //close UART port!
#else
 //enable shell uart interface!!!
#endif
}

#ifdef SATCODX_DEBUG
u32 last_timestamp = 0;
u32 max_interval = 0;
#endif

BOOL ap_satcodx_recv_read_uart(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       u8 *p_ch,
                       u32 time_out)
{
#ifdef SATCODX_DEBUG
u32 cur_timestamp = mtos_ticks_get();
if(last_timestamp == 0)
{
        last_timestamp = cur_timestamp;
        max_interval = 0;
}
else
{
        if((cur_timestamp - last_timestamp) > max_interval)
        {
                max_interval = cur_timestamp - last_timestamp;
        }
}
#endif

#if WIN32
if(uartwin_read_byte(p_ch,time_out) != 0)
{		
    return FALSE;
}
else
{
        return TRUE;
}
#else
if(uart_read_byte(UART_ID_0,p_ch, time_out) != 0)
{
        return FALSE;
}
else
{
        return TRUE;
}

#endif
}

BOOL ap_satcodx_recv_chk_sync_byte(satcodx_impl_data_t *p_satcodx_impl_data)
{
  u32 dwTimeoutCnt =  p_satcodx_impl_data->wait_host_start_timeout;
  p_satcodx_impl_data->sm = SATCODX_SM_WAIT_DATA;
  ap_satcodx_notify_ui(
    SATCODX_EVT_UPDATE_STATUS,
    SATCODX_STATUS_COUNTDOWN,
    dwTimeoutCnt);
  while(dwTimeoutCnt --)
  {
    if(ap_satcodx_recv_read_uart(
        p_satcodx_impl_data,
        p_satcodx_impl_data->p_recv_buf,
        p_satcodx_impl_data->duration_per_second))
    {//get sync byte!!
      p_satcodx_impl_data->cur_recv_buf_used_size ++;
      ap_satcodx_notify_ui(
        SATCODX_EVT_UPDATE_STATUS,
        SATCODX_STATUS_RECEIVE_UPDATE,
        0);
      return TRUE;
    }
    else
    {
      ap_satcodx_notify_ui(
        SATCODX_EVT_UPDATE_STATUS,
        SATCODX_STATUS_COUNTDOWN,
        dwTimeoutCnt);
    }
  }
  ap_satcodx_recv_free_uart(p_satcodx_impl_data);
  ap_satcodx_notify_ui(SATCODX_EVT_UPDATE_STATUS,SATCODX_STATUS_TIMEOUT, 0);
  return FALSE;
}

BOOL ap_satcodx_recv_transfer_data(satcodx_impl_data_t *p_satcodx_impl_data)
{
 u32 dwTimeoutCnt =  p_satcodx_impl_data->recv_host_stop_timeout * 
                         p_satcodx_impl_data->duration_per_second;
 p_satcodx_impl_data->sm = SATCODX_SM_RECV_DATA;
 while(
      p_satcodx_impl_data->cur_recv_buf_used_size <
      p_satcodx_impl_data->recv_buf_max_size)
 {
  if(ap_satcodx_recv_read_uart(
      p_satcodx_impl_data,
      p_satcodx_impl_data->p_recv_buf +
      p_satcodx_impl_data->cur_recv_buf_used_size,
      dwTimeoutCnt))
  {//get data!!!
   p_satcodx_impl_data->cur_recv_buf_used_size ++;
   if( (p_satcodx_impl_data->cur_recv_buf_used_size 
         % p_satcodx_impl_data->notify_per_size) == 0)
   {
    ap_satcodx_notify_ui(SATCODX_EVT_UPDATE_STATUS,
      SATCODX_STATUS_RECEIVE_UPDATE,
      p_satcodx_impl_data->cur_recv_buf_used_size);                
   }
  }
  else
  {//transfer stop!!!
   break;
  }
 } 
 
  //close com port!!!
  ap_satcodx_recv_free_uart(p_satcodx_impl_data);
  //notify UI transfer stop!!!
  ap_satcodx_notify_ui(
    SATCODX_EVT_UPDATE_STATUS,
    SATCODX_STATUS_RECEIVE_END,
    0);
 
 SATCODX_DBG((
    "ap_satcodx_recv_chk_sync_byte max interval is %d\n",
    max_interval));

#ifdef SATCODX_DEBUG
 last_timestamp = 0;
 max_interval = 0;
#endif

 return TRUE;
}

void ap_satcodx_recv_init(satcodx_impl_data_t *p_satcodx_impl_data)
{
  p_satcodx_impl_data->sm = SATCODX_SM_WAIT_DATA;
  //init memory
  ap_satcodx_recv_alloc_buf(p_satcodx_impl_data);
  //init com port!!       
  ap_satcodx_recv_alloc_uart(p_satcodx_impl_data);
  p_satcodx_impl_data->p_cur_pos_of_static_buf =
    p_satcodx_impl_data->p_static_buf;
  p_satcodx_impl_data->p_recv_buf = 
    p_satcodx_impl_data->p_static_buf + p_satcodx_impl_data->recv_buf_offset;
  p_satcodx_impl_data->cur_static_buf_left_size =
    p_satcodx_impl_data->max_buf_size;
  p_satcodx_impl_data->cur_recv_buf_used_size = 0;
  p_satcodx_impl_data->p_result_list = NULL;
  p_satcodx_impl_data->p_db_list = NULL;
}

void ap_satcodx_recv_deinit(satcodx_impl_data_t *p_satcodx_impl_data)
{
 //close com port!!!
 //ap_satcodx_recv_free_uart(p_satcodx_impl_data);
 ap_satcodx_recv_free_buf(p_satcodx_impl_data);
 
}

void ap_satcodx_recv_start(satcodx_impl_data_t *p_satcodx_impl_data)
{
 //firset get the sync byte in 20s!!!!
 ap_satcodx_recv_init(p_satcodx_impl_data);
 if(ap_satcodx_recv_chk_sync_byte(p_satcodx_impl_data))
 {
  //recv data until terminate!
  ap_satcodx_recv_transfer_data(p_satcodx_impl_data);
  //now parse data!!!
  ap_satcodx_parser_push_data(p_satcodx_impl_data);
 }

 
 
}

void ap_satcodx_recv_stop(satcodx_impl_data_t *p_satcodx_impl_data)
{
 ap_satcodx_recv_deinit(p_satcodx_impl_data);
}

