/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "../inc/drv/bus/i2c.h"
#include "../../drvbase/drv_dev_priv.h"
#include "i2c_priv.h"

static void i2c_lock(device_base_t *p_base)
{
  if(p_base->lock_mode == OS_MUTEX_LOCK)
  {
    mtos_sem_take(&p_base->mutex, 0);
    return;
  }
  if(p_base->lock_mode == OS_TASK_LOCK)
  {
    mtos_task_lock();
    return;
  }

  mtos_sem_take(&p_base->mutex, 0);
}

static void i2c_unlock(device_base_t *p_base)
{
  if(p_base->lock_mode == OS_MUTEX_LOCK)
  {
    mtos_sem_give(&p_base->mutex);
    return;
  }
  if(p_base->lock_mode == OS_TASK_LOCK)
  {
    mtos_task_unlock();
    return;
  }

  mtos_sem_give(&p_base->mutex);
}

RET_CODE i2c_write(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, u32 len, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_i2c_t *p_lld = (lld_i2c_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->write != NULL);
    i2c_lock(p_base);
    ret = p_lld->write(p_lld, slv_addr, p_buf, len, param);
    i2c_unlock(p_base);
  }

  return ret;
}

RET_CODE i2c_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, u32 len, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_i2c_t *p_lld = (lld_i2c_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->read != NULL);
    i2c_lock(p_base);
    ret = p_lld->read(p_lld, slv_addr, p_buf, len, param);
    i2c_unlock(p_base);
  }

  return ret;
}

RET_CODE i2c_seq_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                   u32 wlen, u32 rlen, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_i2c_t *p_lld = (lld_i2c_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->seq_read != NULL);
    i2c_lock(p_base);
    ret = p_lld->seq_read(p_lld, slv_addr, p_buf, wlen, rlen, param);
    i2c_unlock(p_base);
  }

  return ret;
}

RET_CODE i2c_std_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                   u32 wlen, u32 rlen, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_i2c_t *p_lld = (lld_i2c_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->std_read != NULL);
    i2c_lock(p_base);
    ret = p_lld->std_read(p_lld, slv_addr, p_buf, wlen, rlen, param);
    i2c_unlock(p_base);
  }

  return ret;
}

