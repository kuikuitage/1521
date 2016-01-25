/******************************************************************************/
/******************************************************************************/
#ifndef __SMC_PRO_PRIV_H__
#define __SMC_PRO_PRIV_H__

/*!
define the size of the smc pro ct name 
   */
#define SMC_PRO_CT_NAME_SZ 20

/*! 
Card type enum definition
   */
typedef enum
{
  SMC_CARD_DEF = 0,
  SMC_CARD_CONAX,
  SMC_CARD_VIACCESS,
  SMC_CARD_IRDETO_162,
  SMC_CARD_DREAMCRYPT_223,
  SMC_CARD_DVN_TEST,
  SMC_CARD_IRDETO_557,
  SMC_CARD_IRDETO_178,
  SMC_CARD_LIHE_E02B,
  SMC_CARD_VIACCESS3585,
  SMC_CARD_VIACCESS7289,
  SMC_CARD_TONGFANG,
  SMC_CARD_ATOSCARD,
}scard_card_type_list_t;

/*!
Card type struct definition 
   */
typedef struct 
{
  scard_card_type_list_t ct_e;
  u8   *p_ct_atr; 
  u32 *p_ct_ck;
  u8    name[SMC_PRO_CT_NAME_SZ];
}scard_card_type_t;

/*! 
definition of lld_smc_pro 
   */
typedef struct lld_smc_pro
{
  void *priv;

  /*! 
  Active a present smart card.
     */
  RET_CODE (*active)(struct lld_smc_pro *p_lld, scard_atr_desc_t *p_atr);
  /*! 
  Parse ATR of the card.
     */
  RET_CODE (*atr_parse)(struct lld_smc_pro *p_lld, scard_atr_desc_t *p_atr, scard_pro_cfg_t *p_cfg);
  /*! 
  Cofigure the interface based on the parameters from ATR, including PTS.
     */
  RET_CODE (*interface_cfg)(struct lld_smc_pro *p_lld, scard_pro_cfg_t *p_cfg);
  /*! 
  Deactive a preset smart card.
     */
  RET_CODE (*deactive)(struct lld_smc_pro *p_lld);
  /*! 
  Data transfer using protocol T = 0. 
     */
  RET_CODE (*t0_transaction)(struct lld_smc_pro *p_lld, scard_opt_desc_t *rwopt);
  /*! 
  Data transfer using protocol T = 0. 
     */
  RET_CODE (*t1_transaction)(struct lld_smc_pro *p_lld, scard_opt_desc_t *rwopt);
  /*! 
  Data transfer using protocol T = 0. 
     */
  RET_CODE (*t14_transaction)(struct lld_smc_pro *p_lld, scard_opt_desc_t *rwopt);
  /*! 
  Data transfer using protocol T = 0. 
     */
  RET_CODE (*rw_transaction)(struct lld_smc_pro *p_lld, scard_opt_desc_t *rwopt);
}lld_smc_pro_t;

#endif //__SMC_PRO_PRIV_H__


