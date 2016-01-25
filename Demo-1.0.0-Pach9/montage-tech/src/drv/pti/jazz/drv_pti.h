/******************************************************************************/
/******************************************************************************/

#ifndef __DRV_PTI_H__
#define __DRV_PTI_H__

/*!
  comments
  */
typedef unsigned short         pti_pid_t;
/*!
  comments
  */
typedef unsigned char          pti_channelhandle_t;
/*!
  comments
  */
typedef unsigned char          pti_filterhandle_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
  */
  FILTER_SINGLE,
  /*!
    comments
  */
  FILTER_DOUBLE,
  /*!
    comments
  */
  FILTER_TYPE_END
} filter_type_t;

/*!
  comments
  */
typedef enum tagpti_mul_sec_dis_e
{
  /*!
    对于正常的TS流建议填0
  */
    DRV_PTI_MUL_SEC_DIS_ENABLE = 0x00,
  /*!
    对于TS填充不为0xff的TS流需要填1
  */
    DRV_PTI_MUL_SEC_DIS_DISABLE,
  /*!
    comments
  */
    DRV_PTI_MUL_SEC_DIS_END,
} pti_mulsecdis_t;

/*!
  comments
  */
typedef enum tagpti_soft_protect_e
{
  /*!
    comments
  */
    DRV_PTI_SOFT_PROTECT_DISABLE = 0x00,
  /*!
    comments
  */
    DRV_PTI_SOFT_PROTECT_ENABLE,
  /*!
    comments
  */
    DRV_PTI_SOFT_PROTECT_END,
} pti_soft_protect_t;

/*!
  comments
  */
typedef enum tagpti_double_state_e
{
  /*!
    comments
  */
    DRV_PTI_FILTER_STATE_IDLE = 0x00,
  /*!
    comments
  */
    DRV_PTI_FILTER_PATCH_START,
} pti_double_state_t;

/*!
  comments
  */
typedef enum tagpti_status_e
{
  /*!
    comments
  */
  DRV_PTI_OPEN = 0x00,
  /*!
    comments
  */
  DRV_PTI_START,
  /*!
    comments
  */
  DRV_PTI_STOP,
  /*!
    comments
  */
  DRV_PTI_RESET,
  /*!
    comments
  */
  DRV_PTI_CLOSE,
  /*!
    comments
  */
  DRV_PTI_END
} pti_status_t;

/*!
  comments
  */
typedef enum tagpti_section_crc_status_e
{
   /*!
     comments
   */
  CRC_DISABLE     = 0x00,
   /*!
     comments
   */
  CRC_ENABLE      = 0x01
} pti_section_crc_status_t;

/*!
  Callback function Type
*/
typedef ERRORCODE_T (*PTI_CallBack_t)
(
  /*!
    comments
  */
  pti_channelhandle_t,
  /*!
    comments
  */
  pti_filterhandle_t,
  /*!
    comments
  */
  U8 *,
  /*!
    comments
  */
  U16
);

/*!
  Configrable to hw
  */
#define MAX_PTI_MASK_LEN              12
/*!
  comments
  */
typedef struct tagmaskdata
{
  /*!
    comments
  */
  U8  m_mask   [MAX_PTI_MASK_LEN];
  /*!
    comments
  */
  U8  m_filter [MAX_PTI_MASK_LEN];
  /*!
    comments
  */
  U32 m_filter_len;
} pti_maskdata_t;

/*!
  comments
  */
typedef enum tagreqmode_e
{
  /*!
    单section模式
  */
  PTI_REQ_MODE_SINGLE_SECTION,     
  /*!
    连续section模式
  */
  PTI_REQ_MODE_CONTINUOUS_SECTION, 
  /*!
    环形section模式
  */
  PTI_REQ_MODE_FILTER_LOOPBUFF,    
  /*!
    单表模式搜取ts包
  */
  PTI_REQ_MODE_SINGLE_TS,          
  /*!
    连续模式搜取ts包
  */
  PTI_REQ_MODE_CONTINUOUS_TS,      
  /*!
    带过滤的ts单表模式(软件组section包)
  */
  PTI_REQ_MODE_FTS_SINGLE, 
  /*!
    带过滤的ts多表模式(软件组section包)
  */
  PTI_REQ_MODE_FTS_MUL, 
  /*!
    不带过滤的ts模式(软件组section包)
  */
  PTI_REQ_MODE_NFTS,
  /*!
    双buffer模式，用多表进行构造(不丢包)
  */
  PTI_REQ_MODE_SECTION_DOUBLE,
  /*!
    不带过滤TS LOOP模式，送给传输层的是section（不丢包，用于后备）
  */
  PTI_REQ_MODE_SECTION_NFTS,
  /*!
    comments
  */
  PTI_REQ_MODE_UNKNOWN
} pti_req_mode_t;

/*!
  comments
  */
typedef enum tagpti_channeltype_e
{
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_PSI_SI = 0x00,
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_AUDIO,
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_VIDEO,
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_PCR,
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_VBI,
  /*!
    comments
  */
  DRV_PTI_CHANNEL_TYPE_END
} pti_channeltype_t;

/*!
  PTI Driver Interface
*/
ERRORCODE_T drv_pti_init (PTI_CallBack_t pCallBack);

/*!
  comments
  */
ERRORCODE_T drv_pti_term(void);

/*!
  comments
  */
ERRORCODE_T drv_pti_createchannel(U32 max_filter_number, 
                                  U32 max_filter_size,   
                                  U32 buffer_size,  
                                  filter_type_t filter_type,  
                                  pti_channelhandle_t * pChannelHandle);






/*!
  comments
  */
ERRORCODE_T drv_pti_destorychannel
(
  pti_channelhandle_t ulChannelHandle
);

/*!
  comments
  */
ERRORCODE_T drv_pti_setchannelpid
(
  pti_channelhandle_t ulChannelHandle,
  pti_pid_t           ulPid,
  pti_channeltype_t   type
);

/*!
  comments
  */
ERRORCODE_T drv_pti_getchannelpid
(
  pti_channelhandle_t ulChannelHandle,
  pti_pid_t *         ulPid
);

/*!
  comments
  */
ERRORCODE_T drv_pti_querychannelformpid
(
  pti_channelhandle_t * pulChannelHandle,
  pti_pid_t             ulPid
);

/*!
  comments
  */
ERRORCODE_T drv_pti_controlchannel
(
  pti_channelhandle_t ulChannelHandle,
  pti_status_t        ulChannelStatus
);

/*!
  comments
  */
ERRORCODE_T drv_pti_createfilter
(
  pti_channelhandle_t  ulChannelHandle,
  pti_filterhandle_t * pFilterHandle
);

/*!
  comments
  */
ERRORCODE_T drv_pti_destoryfilter
(
  pti_channelhandle_t ulChannelHandle,
  pti_filterhandle_t  ulFilterHandle
);

/*!
  comments
  */
ERRORCODE_T drv_pti_setfilter
(
  pti_channelhandle_t      ulChannelHandle,
  pti_filterhandle_t       ulFilterHandle,
  pti_maskdata_t           maskdata, 
  pti_section_crc_status_t crc_check,
  pti_req_mode_t           mode
);

/*!
  comments
  */
ERRORCODE_T drv_pti_getfilter
(
  pti_channelhandle_t        ulChannelHandle,
  pti_filterhandle_t         ulFilterHandle,
  pti_maskdata_t *           pMaskdata, 
  pti_section_crc_status_t * pCrc_check,
  pti_req_mode_t *           pMode
);

/*!
  comments
  */
ERRORCODE_T drv_pti_controlfilter
(
  pti_channelhandle_t ulChannelHandle,
  pti_filterhandle_t  ulFilterHandle,
  pti_status_t        ulStatus
);

/*!
  comments
  */
ERRORCODE_T drv_pti_readsection
(
  U32                   ulReadTimeout,
  U8 *                  pucBuffer,
  U32 *                 ulBufferLenght,
  pti_channelhandle_t * pChannelHandle,
  pti_filterhandle_t *  FilterHandle
);

#endif //__DRV_PTI_H__

