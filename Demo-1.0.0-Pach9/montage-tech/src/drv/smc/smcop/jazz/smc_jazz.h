/******************************************************************************/
/******************************************************************************/

#ifndef __SMC_JAZZ_H__
#define __SMC_JAZZ_H__

/*!
  Card plug in
  */
#define SMC7816_INT_CARD_IN 0x01
/*!
  Card plug out
  */
#define SMC7816_INT_CARD_OUT 0x02
/*!
  all int mask bit
  */
#define SMC7816_INT_MASK 0x3FFF

/*! 
  smc private data definition
  */
typedef struct 
{
  /*!
    commments
  */
  drv_smartcardhandle_t SCHandler;
  /*!
    commments
  */
  scard_config_t  scard_cfg;
  /*!
    commments
  */
  smc_op_notify notify;
  /*!
    commments
  */
  smc_op_term_check term_check;
  /*!
    commments
  */
  drvsvc_handle_t *p_svc;
  /*!
    commments
  */
  u32 *p_stack;
}smc_priv_t;

/*!
  get card status
  */
#define SMC_IO_CTRL_GET_STATUS 0x01

/*!
  get card protocol
  */
#define SMC_IO_CTRL_GET_PROTOCOL 0x02


#endif //__SMC_JAZZ_H__

