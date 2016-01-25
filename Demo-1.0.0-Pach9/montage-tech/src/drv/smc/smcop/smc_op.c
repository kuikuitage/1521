/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "smc_op_priv.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "../../drvbase/drv_dev_priv.h"
#include "mtos_printk.h"
#include "sys_define.h"

RET_CODE scard_active(scard_device_t *p_dev, scard_atr_desc_t *p_atr)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->active) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->active(p_dev->p_priv, p_atr);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_deactive(scard_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->deactive) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->deactive(p_dev->p_priv);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_reset(scard_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->reset) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->reset(p_dev->p_priv);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE scard_set_config(scard_device_t *p_dev, scard_config_t *p_cfg)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->set_config) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->set_config(p_dev->p_priv, p_cfg);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_get_config(scard_device_t *p_dev, scard_config_t *p_cfg)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->get_config) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->get_config(p_dev->p_priv, p_cfg);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_rw_transaction(scard_device_t *p_dev, scard_opt_desc_t *p_rwopt)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->rw_transaction) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->rw_transaction(p_dev->p_priv, p_rwopt);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_set_protocol(scard_device_t *p_dev, scard_protocol_t protocol)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->set_protocol) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->set_protocol(p_dev->p_priv, protocol);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE scard_register_notify(scard_device_t *p_dev, smc_op_notify func)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->register_notify) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->register_notify(p_dev->p_priv, func);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_register_term_check(scard_device_t *p_dev, smc_op_term_check func)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->register_term_check) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->register_term_check(p_dev->p_priv, func);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_read(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  if (p_lld_dev->read) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->read(p_lld_dev, p_buf, size, p_actlen);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_write(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  //OS_PRINTF("scard_write:p_dev address=0x%x\n", p_dev);
  //OS_PRINTF("scard_write:dev_get_state(p_dev)=0x%x\n", dev_get_state(p_dev));
  if ((dev_get_state(p_dev) & DEV_STATE_MASK_WORK) == 0)
  {
    return ERR_STATUS;
  }

  //OS_PRINTF("scard_write:p_lld_dev->register_term_check=0x%x\n", p_lld_dev->register_term_check);
  if (p_lld_dev->register_term_check) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld_dev->write(p_lld_dev, p_buf, size, p_actlen);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

RET_CODE scard_io_ctrl(scard_device_t *p_dev, u32 cmd, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_smc_t *p_lld_dev = (lld_smc_t *)(p_dev->p_priv);
  device_base_t *p_base = (device_base_t *)(p_dev->p_base);

  if (p_base->io_ctrl) {
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_base->io_ctrl(p_lld_dev, cmd, param);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}
