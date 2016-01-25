/******************************************************************************/
/******************************************************************************/
#include <assert.h>


#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "../drvbase/drv_dev_priv.h"
#include "../charsto.h"
#include "charsto_priv.h"

RET_CODE charsto_read(charsto_device_t *p_dev, u32 addr, 
                   u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->read != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->read(p_lld, addr, p_buf, len);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE charsto_writeonly(charsto_device_t *p_dev, u32 addr, 
                  u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->writeonly != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->writeonly(p_lld, addr, p_buf, len);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE charsto_erase(charsto_device_t *p_dev, u32 addr, u32 sec_cnt)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->erase != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->erase(p_lld, addr, sec_cnt);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE charsto_protect_all(charsto_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->protect_all != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->protect_all(p_lld);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE charsto_unprotect_all(charsto_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->unprotect_all != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->unprotect_all(p_lld);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}


RET_CODE charsto_get_cacapcity(charsto_device_t *p_dev, u32 *p_cap)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->get_capacity != NULL);
    mtos_sem_take(&p_base->mutex, 0);
    ret = p_lld->get_capacity(p_lld,p_cap);
    mtos_sem_give(&p_base->mutex);
  }

  return ret;
}

