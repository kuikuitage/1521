
/******************************************************************************/
/******************************************************************************/
#ifndef __BLOCK_H__
#define __BLOCK_H__
/*!
      BLOCK bus event to notify uplayer
  */
typedef enum {
/*!
      event to indicate device connection to bus
  */
  BLOCK_BUS_DEV_CONNECT = 1,
/*!
     event to indicate device removed from bus
  */
  BLOCK_BUS_DEV_REMOVE,
}block_dev_event_t;

// BLOCK device type
/*!
      block_dev_type
  */
typedef enum {
/*!
     usb-mass-storage class
  */
  BLOCK_DEV_USB_MASS = 1,
/*!
     memery-card class
  */
  BLOCK_DEV_MEM_CARD,
/*!
     nand flash on board
  */  
  BLOCK_DEV_NAND,
 /*!
     win 32 fake device
  */  
  BLOCK_DEV_WIN32_FAKE,
/*!
    the number of device type
  */  
  BLOCK_DEV_TYPE_NUM 
}block_dev_type_t;

/************************  for upper layer interface****************/
enum block_data_dir {
/*!
    data from BLOCK device to host
  */  
  BLOCK_DIR_IN = 1,
/*!
    data from host to BLOCK device
  */  
  BLOCK_DIR_OUT
};

/*!
    block request struct
  */
typedef struct block_xfer_req {
/*!
     data direction
  */  
  enum block_data_dir data_dir;
/*!
   start logic address of first block
  */  
  u32 start_sec;
/*!
   the block number
  */  
  u32 sec_num;
/*!
    data buffer, if NULL, indicate need push data
  */  
  u8 *p_buf;
}block_xfer_req_t;
/*!
    BLOCK command request block, used for asynchronous operation to BLOCK device
  */
typedef struct block_tr {
/*!
    crb list node
  */  
  struct list_head list;
/*!
    transfer request list
  */  
  block_xfer_req_t xfer_req;
/*!
   the context infomation to be transfered
  */  
  u32 context;
/*!
   priv 
  */  
  void *priv;
/*!
    command complete callback function
  */
  void(*complete)(struct block_tr *);
/*!
    command result
  */  
  RET_CODE result;
}block_tr_t;
/*!
     max num support block devices
  */  
#define NR_BLK_DEV 5   
/*!
   comlete callback function for BLOCK command request
  */  
typedef void (*BLOCK_TR_COMPLETE)(struct block_tr *trb);
/*!
   bus event notify function
  */  
typedef void (*BLOCK_EVT_NOTIFY)(char * name, block_dev_event_t event);

/*!
   block layer configure
  */
typedef struct ufs_register_notify
{
/*!
    when block driver  find block device inserted or remorved will callack this function  
  */  
  BLOCK_EVT_NOTIFY notify;
 }ufs_register_notify_t;
/*!
   block layer configure
  */
typedef struct block_device_config
{
/*!
   support TR mode
  */  
  #define SUPPORT_TR 1
/*!
  unsupport TR mode
  */  
  #define UNSUPPORT_TR 0
/*!
   config block layer support or unsupport tr mode
  */  
  char  cmd;                     
/*!
   block device scheduler priority
 */  
  u32 tr_submit_prio;  
 /*!
   block device bus priority  // sd or usb bus priority
 */  
  u32 bus_prio; 
  /*!
   block device stack size  // 
 */  
  u32 bus_thread_stack_size; 

}block_device_config_t;
/*!
  ufs register callback function 
  \param[in] config  config command
 */
RET_CODE register_notify(ufs_register_notify_t *p_config);


/*!
  config block device 
  \param[in] config  config command
 */
RET_CODE block_tr_task_config(drv_dev_t *p_dev, u32 tr_submit_prio);

/*!
  block sector read function
  
  \param[in] dev block device handle
  \param[in] sec_addr start  read sector address 
  \param[in] sec_addr need read sector numbers 
  \param[out] buf  point to read buff 
 */
RET_CODE block_sector_read(drv_dev_t *p_dev, u32 sec_addr, u32 sec_num, u8 *p_buf);

/*!
  block sector write function
  
  \param[in] dev block device handle
  \param[in] sec_addr start  read sector address 
  \param[in] sec_addr need read sector numbers 
  \param[in] buf  point to write buff 
 */
RET_CODE block_sector_write(drv_dev_t *p_dev, u32 sec_addr, u32 sec_num, u8 *p_buf);
/*!
  BLOCK command request list, used for asynchronous operation to BLOCK device
  \param[in] dev block device handle
  \param[in] request list
 */
RET_CODE block_tr_submit(drv_dev_t *p_dev, struct block_tr *p_btr);
/*!

 */
RET_CODE block_io_ctrl(drv_dev_t *p_dev, u32 cmd, u8 *p_buf);
#endif  // end of BLOCK_H
