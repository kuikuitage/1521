/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#ifndef __SUBT_PIC_ADAPTER_H__
#define __SUBT_PIC_ADAPTER_H__


/*!
  ADS module function list
  */
typedef struct
{
  /*!
    Initialize the ADS module
    */
  RET_CODE (*init)(void *param);
  /*!
    De-Initialize the ADS module
    */
  RET_CODE (*deinit)();
  /*!
    open the subtitle pic module
    */
  RET_CODE (*open)();
  /*!
    close the subtitle pic module
    */
  RET_CODE (*close)();
  /*!
    stop the subtitle pic module
    */
  RET_CODE (*stop)();
  /*!
    IO control function.
    */    
  RET_CODE (*io_ctrl)(u32 cmd, void *param);  
}subt_pic_func_t;


/*!
  ADS module function list
  */
typedef struct
{
  /*!
    The flag to indicate the ads module has been attached
    */
  u8 attached;
  /*!
    The flag to indicate the ads module has been initialized
    */
  u8 inited;
  /*!
    The function list of this ADS module
    */  
  subt_pic_func_t func;
  /*!
    The private data internal used for the ads module
    */   
  void *p_priv;
}subt_pic_op_t;

/*!
  ADS adapter global info
  */
typedef struct
{
  /*!
    The flag to indicate if ads is inited
    */
  u32 inited;
  /*!
    The ads module's operation
    */  
  subt_pic_op_t adm_op[SUBT_PIC_ID_ADT_MAX_NUM];
  /*!
    The event call back infomation 
    */  
  //ads_event_notify_t notify;  
  /*!
    The current event happend
    */  
  //u32 events[ADS_MAX_SLOT_NUM][AD_EVT_U32_LEN];
  /*!
    The current event parameter
    */  
  //u32 event_param[ADS_MAX_SLOT_NUM];
  /*!
    The mutex used to lock the operation of events setting and getting 
    */   
  os_sem_t evt_mutex;
}subt_pic_adapter_priv_t;


#endif //__SUBT_PIC_ADAPTER_H__
