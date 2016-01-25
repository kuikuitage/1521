/******************************************************************************/
/******************************************************************************/
#ifndef __SMART_PRO_CARD_H__
#define __SMART_PRO_CARD_H__

/*! 
define max length of smc atr 
   */
#define SMC_ATR_MAX_LENGTH    32


/*! 
define the scard pro configuration struct 
   */
typedef struct
{
  /*!
  cwi default = 13
  */
  u8  cwi;
  /*!
  bwi default  = 4
  */
  u8  bwi;
  /*!
  Fi
  */
  u32 clk_fi;
  /*!
  Di
  */
  u32 baud_di;
  /*!
  Ni
  */
  u32 extra_gardtime;
  /*!
  smc supported protocol
  */
  scard_protocol_t protocol;
}scard_pro_cfg_t;


/*!
   Active a present smart card.


   \return Return SUCCESS for ATR reception; Others for failure.
 */
RET_CODE scard_pro_active(scard_atr_desc_t *p_atr);
//check if the card is present
//set time out 
//send activation sequence.
//wait ATR (take sempahore).
//According to reset results to deactive card or parse ATR.


/*!
   Parse ATR of the card.
 */
RET_CODE scard_pro_atr_parse(scard_atr_desc_t *p_atr, scard_pro_cfg_t *p_cfg);


/*!
   Cofigure the interface based on the parameters from ATR, including PTS.

   \param[in] p_cfg Parameters to configure the smart card device.
 */
RET_CODE scard_pro_interface_cfg(scard_pro_cfg_t *p_cfg);


/*!
   Deactive a preset smart card.
 */
RET_CODE scard_pro_deactive(void);

/*!
   Transact data transfer between the smart card and the interface, 
   including transmission and reception.

   \param[in] rwopt Descript a data transfer task.
   \return
 */
RET_CODE scard_pro_rw_transaction(scard_opt_desc_t *rwopt);

/*!
   Data transfer using protocol T = 0.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.
   \return
 */
RET_CODE scard_pro_t0_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Data transfer using protocol T = 1.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.   
   \return
 */
RET_CODE scard_pro_t1_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Data transfer using protocol T = 14.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.   
   \return
 */
RET_CODE scard_pro_t14_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Register the device handle of smart card operation layer

   \param[in] p_op_device The device handle of smart card operation layer.
   \return
 */
RET_CODE scard_pro_register_op(scard_device_t *p_op_device);

/*!
   Register the protocol to be supported in smart card protocol layer

   \param[in] protocol The protocol defined in spec of smart card 7816.
   \return
 */
RET_CODE scard_pro_set_protocol(scard_protocol_t protocol);

/*!
   Get smart card id.

   \param[out] the pointer of memory to store the card id
   \return
 */
RET_CODE scard_pro_get_card_type(u32 *card_type);


/*!
   PPS

   \param[in] the param for pps
   \return
 */RET_CODE smc7816_pps_req(scard_pro_cfg_t *p_cfg);

#endif //__SMART_PRO_CARD_H__
 
