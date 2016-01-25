/******************************************************************************/
/******************************************************************************/
#ifndef __LPOWER_PRIV_H__
#define __LPOWER_PRIV_H__

/*!
    power driver low level structure as the private data("priv") of struct "lld_magic_lpower_t"
  */
typedef struct lld_lpower
{
  /*!
      The private data of low level driver.
    */
  void *p_priv;
    /*!
      The low level function to implement the high level interface "charsto_writeonly".
    */   
  RET_CODE (*io_ctrl)(void *p_lld, u32 cmd, u32 param);
    /*!
      enter into standby.
    */   
  void (*enter_standby)(void *p_lld);

}lld_lpower_t;

/*!
   the private data of spi flash driver
  */
typedef struct lpower_priv
{
  /*!
      The low power module id
    */
  u32 dev_id;

  /*!
      If display.
  */
  u8 dis_flag;
  /*!
      How display time : standbytime or local time
  */
  u8 dis_how;
  /*!
      If into low power mode.
  */
  u32 low_power_flag;

  /*!
    standby param
  */
  utc_time_t *p_tm_out;
} lpower_priv_t;
#endif //__LPOWER_PRIV_H__
