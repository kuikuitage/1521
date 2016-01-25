/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/



#ifndef __XSM_CAS_INCLUDE_H__
#define __XSM_CAS_INCLUDE_H__


/*!
    ABC
  */
#define CAS_SECTION_TASK_PRIO        32
/*!
    ABC
  */
#define XSM_CAS_TASK_NUM          6
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_0        33
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_1        34
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_2        35
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_3        36
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_4        37
/*!
    ABC
  */
#define XSM_CAS_TASK_PRIO_5        38

/*!
    ABC
  */
#define XSM_CAS_TASK_STKSIZE     8192
/*!
    ABC
  */
#define XSM_CARD_SN_LEN     8
/*!
    ABC
  */
#define XSM_PRIVATE_DATA_LEN_MAX     256

/*!
    ABC
  */
#define NIT_PID  0x10

/*!
    ABC
  */
#define NIT_TABLE_ID  0x40

/*!
    ABC
  */
typedef enum
{
/*!
    free
  */
    XSM_FREE = 0,
/*!
    used
  */
    XSM_USED
}xsm_status_t;


/*!
    ABC
  */
typedef struct
{
/*!
    ABC
  */
  xsm_status_t m_TaskStatus;
/*!
    ABC
  */
  u32 m_uTaskId;
/*!
    ABC
  */
  u32 m_uTaskPrio;
}s_xsm_task_struct_t;


/*!
    ABC
  */
#define XSM_CAS_SEM_MAX_NUM    (16)

/*!
    ABC
  */
typedef struct
{
/*!
    semphore name
  */
    UINT8 name[4];
/*!
    semphore init value
  */
    UINT32 init_value;
/*!
    wait mode
  */
    UINT32 wait_mode;
/*!
    semphore id for cas
  */
    UINT32 sem_id;
/*!
    semphore status
  */
    UINT32 sem_status;
}xsm_sem_node_t;

/*!
    ABC
  */
#define XSM_CAS_QUEUE_MAX_NUM    (64)

/*!
    ABC
  */
#define XSM_CAS_QUEUE_MAX_DEPTH    (32)

/*!
    ABC
  */
typedef struct
{
/*!
    queue status
  */
    UINT32 queue_status;
/*!
    message id for mtos
  */
    UINT32 queue_id;
/*!
    message context
  */
    XinShiMao_Queue_message message;

}xsm_queue_t;

/*!
  abc
  */
typedef struct
{
  /*!
    abc
    */
  UINT8 slot;
  /*!
    1:current program scramble, 0:current program not scaramble
    */
  UINT8 scramble_flag;
  /*!
    abc
    */
  UINT16 network_id;
  /*!
    abc
    */
  UINT16 server_id;
  /*!
    abc
    */
   UINT16 ecm_pid;
  /*!
    abc
    */
   UINT16 emm_pid;
  /*!
    abc
    */
  UINT16 v_pid;
  /*!
    abc
    */
  UINT16 a_pid;
  /*!
    The callback of nvram reading function
    */
  RET_CODE (*nvram_read)(UINT32 offset,
              UINT8 *p_buf, UINT32 *size);
  /*!
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(UINT32 offset,
              UINT8 *p_buf, UINT32 size);
     /*!
   flash_start_adr
    */
  UINT32 flash_start_adr;
    /*!
   flash_size
    */
  UINT32 flash_size;
}cas_xsm_priv_t;

/*!
   filter depth size
  */
#define XSM_FILTER_DEPTH_SIZE          13

/*!
   software filter byte
  */
#define XSM_SOFTWARE_FILTER_BYTE          12

/*!
   filter max num
  */
#define XSM_FILTER_MAX_NUM              10

/*!
   filter buffer size
  */
#define XSM_FILTER_BUFFER_SIZE        (16 * (KBYTES) + 188)

/*!
  abc
  */
typedef struct
  {
  /*!
  abc
    */
  xsm_status_t  m_FilterStatus;
  /*!
  abc
    */
  u8  m_xsmReqId;
  /*!
  abc
    */
  u16 m_PID;
  /*!
  abc
    */
  u8  m_uFilterData[XSM_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u8  m_uFilterMask[XSM_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u16 m_dwReqHandle;
  /*!
  abc
    */
  u32 m_dwStartMS;
  /*!
  abc
    */
  u32 m_timeout;
  /*!
  abc
    */
  u8  *p_buf;
  }xsm_filter_struct_t;

  /*!
  abc
  */
  typedef struct
  {
/*!
    ABC
  */
  u8    req_id;
/*!
    ABC
  */
  u8    u_ok;
/*!
    ABC
  */
  u16            pid;
  /*!
    ABC
  */
  u8    *p_data;
  /*!
    ABC
  */
  u8    revered;
  /*!
    data length
  */
  u16    length;
  }xsm_data_got_t;

/*!
    ABC
  */
typedef enum
{
  /*!
    ABC
  */
   XSM_CA_CARD_UNKNOWN = 0,
  /*!
    ABC
  */
    XSM_CA_CARD_INSERT,
  /*!
    ABC
  */
    XSM_CA_CARD_REMOVE
}xsm_ca_card_status_t;

/*!
    ABC
  */
typedef struct _CA_DESC_S
{
  /*!
    ABC
  */
    UINT32   CA_system_id        :16;
  /*!
    ABC
  */
    UINT32                                  :3;
  /*!
    ABC
  */
    UINT32   CA_PID                   :13;
}ca_desc_t;

  /*!
    ABC
  */
#define XSM_CAS_SECTION_TASK_STACK_SIZE    (16 * 1024)
  /*!
    ABC
  */
#define XSM_CAS_SECTION_TASK_PRIO        36


  /*!
    ABC
  */
#define XSM_MONITOR_OVERDUE_SECTION_TASK_STACK_SIZE    (4 * 1024)
  /*!
    ABC
  */
#define XSM_MONITOR_OVERDUE_SECTION_TASK_PRIO        39

  /*!
    ABC
  */
#define XSM_CARD_PIN_LENXSMH  (6)

  /*!
    ABC
  */
UINT16 cas_xsm_get_v_pid(void);
  /*!
    ABC
  */
UINT16 cas_xsm_get_a_pid(void);
  /*!
    ABC
  */
INT32 xsm_cas_drv_client_init();
  /*!
    ABC
  */
void xsm_cas_all_emm_req_free();
  /*!
    ABC
  */
void xsm_cas_all_ecm_req_free();
  /*!
    ABC
  */
UINT16 cas_xsm_get_ecm_pid(void);
  /*!
    ABC
  */
UINT16 cas_xsm_get_emm_pid(void);
  /*!
    ABC
  */
UINT16 cas_xsm_get_server_id(void);
/*!
  Get network id
  
  \return network id.
  */
UINT16 cas_xsm_get_network_id(void);
/*!
  Set network id
 */
void cas_xsm_set_network_id(UINT16 network_id);
/*!
  Send zone check end message
 */
void cas_xsm_zone_check_end(UINT32 event);
/*!
  get card status
 */
xsm_ca_card_status_t xsm_get_card_status();

#endif

