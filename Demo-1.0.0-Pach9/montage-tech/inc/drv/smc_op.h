/******************************************************************************/
/******************************************************************************/
#ifndef __SMART_CARD_H__
#define __SMART_CARD_H__

/*! 
  define the max size of smc atr 
  */
#define SMC_ATR_MAX_LENGTH    32

/*! 
  define the min size of smc atr 
  */
#define SMC_ATR_MIN_LENGTH    2

/*! 
  define the protocal of smart card 
  */
typedef enum
{
  /*!
    protocol T0
    */
  SMC_PROTOCOL_T0,
  /*!
    Protocol T1
    */
  SMC_PROTOCOL_T1,
  /*!
    Protocol T14
    */
  SMC_PROTOCOL_T14
}scard_protocol_t;

/*! 
  define the state of RW 
  */
typedef enum
{
  /*!
    Smart card read or write OK
    */
  SMC_RW_OK = 0,
  /*!
    Smart card receive procedure byte
    */
  SMC_RW_PROBYTE_IN,       
  /*!
    r/w a byte.
    */
  SMC_RW_DATABYTE_RW,
  /*!
    r/w all bytes. 
    */
  SMC_RW_DATABLOCK_RW,
  /*!
    Receiving status byte.
    */
  SMC_RW_SW_IN,
  /*!
    Invalid ack
    */
  SMC_RW_ERROR_ACK_INVALID,
  /*!
    Error command
    */
  SMC_RW_ERROR_CMD,
  /*!
    Error UNknown
    */
  SMC_RW_ERROR_UNKNOWN,
  /*!
    Error timeout
    */
  SMC_RW_ERROR_TIMEOUT,
  /*!
    Error parity
    */
  SMC_RW_ERROR_PARITY
}scard_rw_state_t;

/*! 
  state of a ATR session. 
  */
typedef enum
{
  /*!
    Send timeout
    */
  SMC_ATR_S_TIMEOUT,
  /*!
    Detect timeout
    */
  SMC_ATR_D_TIMEOUT = 16,
  /*!
    CH_TIMEOUT timout
    */
  SMC_ATR_CH_TIMEOUT,
  /*!
    parity error
    */
  SMC_ATR_PARITY_ERROR,
  /*!
    INTEGRITY_FAIL
    */
  SMC_ATR_INTEGRITY_FAIL,
  /*!
    INTEGRITY_OK
    */
  SMC_ATR_INTEGRITY_OK,
  /*!
    PARSE_ERROR
    */
  SMC_ATR_PARSE_ERROR,
  /*!
    PARSE_OK
    */
  SMC_ATR_PARSE_OK
}scard_atr_state_t;

/*! 
  state of a ATR session. 
  */
typedef enum
{
  /*!
    smart card insert 
    */
  SMC_CARD_INSERT = 0,
  /*!
    Smart card remove
    */
  SMC_CARD_REMOVE,
  /*!
    smart card reset ok 
    */
  SMC_CARD_RESET_OK,
  /*!
    Smart card init ok
    */
  SMC_CARD_INIT_OK,  
}scard_card_stat_t;

/*! 
  define scard atribution description 
  */
typedef struct
{
  /*! 
    Buffer for ATR. 
    */
  u8 *p_buf; 
  /*! 
    length of received ATR, including TS.
    */
  u8 atr_len; 
}scard_atr_desc_t;

/*! 
  define the struct of scard open state 
  */
typedef struct
{
  /*!
    smc op priority
    */
  u32 smc_op_pri;
  /*!
    Statck size
    */
  u32 smc_op_stksize;
  /*!
    Detect pin polarity, 0:low active, 1:high active
    */
  u32 detect_pin_pol : 1; 
  /*!
    If Auto-detect convention, 
    0:automatically update CONV_REG on TS reception, 
    1:do not update SMC_CONV_REG on TS reception
    */
  u32 convention : 1;  
  /*!
    The size of ring buffer used to reading data
    */
  u32 ring_buf_size : 16;  
  /*!
    Vcc enable polarity, 0: high enable 1:low enable
    */
  u32 vcc_enable_pol : 1;  
  /*!
    Reserved for furture using
    */  
  u32 reserved : 13;
}scard_open_t;

/*! 
  define scard operation description struct 
  */
typedef struct
{
  /*! 
    The buffer address of data from interface to smart card. 
    */
  u8 *p_buf_out;
  /*! 
    The size of data from interface to smart card. 
    */
  u32 bytes_out;
  /*! 
    The buffer address of data from smart card to interface. 
    */
  u8 *p_buf_in;
  /*! 
    The target size of data from smart card to interface. 
    */
  u32 bytes_in_target;
  /*!
    The size of actual received data from smart card.
    */
  u32 bytes_in_actual;
}scard_opt_desc_t;

/*! 
  define scard configuration struct 
  */
typedef struct
{
  /*!
    ch_guard
    */
  u8  ch_guard;
  /*!
    blk_guard
    */
  u8  blk_guard;
  /*!
    bit_d_rate
    */
  u8  bit_d_rate;
  /*!
    clock reference
    */
  u16 clk_ref; 
  /*!
    clk_f_rate
    */
  u16 clk_f_rate; 
  /*!
    Check time
    */
  u32 ch_time;
  /*!
    block time
    */
  u32 blk_time;
  /*!
    parity check
    */
  BOOL  parity_check;
  /*!
    conv_set
    */
  BOOL  conv_set;
}scard_config_t;

/*! 
  define the struct of scard device 
  */
typedef struct scard_device
{
  /*!
    Base handle
    */
  void *p_base;
  /*!
    Private handle
    */
  void *p_priv;
}scard_device_t;


/*! 
  scard state notify definition 
  */
typedef struct
{
  /*!
    Smart card slot index
    */
  u32 slot;
  /*!
    Smart card status
    */
  scard_card_stat_t card_stat; 
  /*!
    Smart card protocol, for Topreal CAS support
    */
  scard_protocol_t m_protocol;
  /*!
    Smart card attribute, for Topreal CAS support
  */
  scard_atr_desc_t *p_atropt;
}scard_notify_t;

/*!
  define the scard term check struct
  */
typedef struct
{
  /*!
    protocal
    */
  u32 protocol;
  /*!
    bytes_in_target
    */
  u32 bytes_in_target;
  /*!
    bytes_in_actual
    */
  u32 bytes_in_actual;
  /*!
    p_buf_in
    */
  u8 *p_buf_in;
}scard_term_check_t;

/*! 
    IO control command
  */
typedef enum
{
  /*!
    IO command to tell smart card driver if needs to filter the protocol(T0/T1/T14) data.
    */
   SMC_IO_PROTOCOL_FILTER,
  /*!
    config smart card write function using mode 1
    */
   SMC_WRITE_FUNCTION_CFG_1,   
} scard_ioctrl_t;

/*! 
  scard state notify definition 
  */
typedef void (*smc_op_notify)(scard_notify_t *);

/*!
  smc term check function prototype
  */
typedef BOOL (*smc_op_term_check)(scard_term_check_t *);

/*!
   Active(power up) a present smart card and get the ATR.

   \param[in] p_dev the handle of device driver
   \param[in] p_atr ATR information received from smart card.

   \return Return SUCCESS for ATR reception; Others for failure.
 */
RET_CODE scard_active(scard_device_t *p_dev, scard_atr_desc_t *p_atr);

/*!
   Deactive(power down) a present smart card

   \param[in] p_dev the handle of device driver

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_deactive(scard_device_t *p_dev);

/*!
   Set configuration before operation.

   \param[in] p_dev the handle of device driver
   \param[in] p_cfg the configuration parameters to be set

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_set_config(scard_device_t *p_dev, scard_config_t *p_cfg);

/*!
   Get current configuration

   \param[in] p_dev the handle of device driver
   \param[in] p_cfg the configuration parameters to be gotten

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_get_config(scard_device_t *p_dev, scard_config_t *p_cfg);

/*!
   Reset the smart card controller.

   \param[in] p_dev the handle of device driver

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_reset(scard_device_t *p_dev);

/*!
   Transact data transfer between the smart card and the interface, 
   including transmission and reception.

   \param[in] p_dev the handle of device driver
   \param[in] p_rwopt Descript a data transfer task.

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_rw_transaction(scard_device_t *p_dev, scard_opt_desc_t *p_rwopt);

/*!
   Tell protocal to the driver. 

   \param[in] p_dev the handle of device driver
   \param[in] protocol protocol defined in ISO7816 

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_set_protocol(scard_device_t *p_dev, scard_protocol_t protocol);

/*!
   Register the function to notify some events to outside. 

   \param[in] p_dev the handle of device driver
   \param[in] func the function to te registered

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_register_notify(scard_device_t *p_dev, smc_op_notify func);

/*!
   Register the function for outside to check if the operation is terminal 

   \param[in] p_dev the handle of device driver
   \param[in] func the function to te registered

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_register_term_check(scard_device_t *p_dev, smc_op_term_check func);

/*!
   Read data from smart card. 
   
   \param[in] p_dev the handle of device driver
   \param[in] p_buf the buffer to be read in
   \param[in] size the buffer size
   \param[out] p_actlen the actual length in reading
   
   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_read(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen); 

/*!
   Write data to smart card. 
   
   \param[in] p_dev the handle of device driver
   \param[in] p_buf the buffer to be write out
   \param[in] size the buffer size
   \param[out] p_actlen the actual length in writing
   
   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_write(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen);   

#endif //__SMART_CARD_H__
 
