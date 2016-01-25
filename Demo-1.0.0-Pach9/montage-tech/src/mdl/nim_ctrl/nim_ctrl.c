/******************************************************************************/
/******************************************************************************/
#include <assert.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_usals.h"
#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_set_tp(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_tp != NULL);
  p_this->set_tp(handle, p_extend, p_tp_info);
}

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_get_tp(class_handle_t handle, nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->get_tp != NULL);
  p_this->get_tp(handle, p_tp_info);
}

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  */
BOOL nc_get_lock_status(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->get_lock_status != NULL);
  return p_this->get_lock_status(handle);
}

/*!
  Determine the locking is finished or not
  \param[in] handle: nim ctrl handle
  */
BOOL nc_is_finish_locking(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->is_finish_locking != NULL);
  return p_this->is_finish_locking(handle);
}


/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[out] p_perf: channel performance
  */
void nc_perf_get(class_handle_t handle, nim_channel_perf_t *p_perf)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->perf_get != NULL);
  p_this->perf_get(handle, p_perf);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] bs TRUE is blind scan mode and false is not blind scan mode
  */
void nc_set_blind_scan_mode(class_handle_t handle, BOOL bs)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_blind_scan_mode != NULL);
  p_this->set_blind_scan_mode(handle, bs);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_monitor(class_handle_t handle, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->enable_monitor != NULL);
  p_this->enable_monitor(handle, enable);
}


/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_lnb_check(class_handle_t handle, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->lnb_check_enable != NULL);
  p_this->lnb_check_enable(handle, enable);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
BOOL nc_is_lnb_check_enable(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->is_lnb_check_enable != NULL);
  return p_this->is_lnb_check_enable(handle);
}

/*!
  Set nim_lnb_porlar_t mode
  \param[in] handle for nc 
  \param[in] polar nim_lnb_porlar_t mode
  */
void nc_set_polarization(class_handle_t handle, nim_lnb_porlar_t polar)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_polarization != NULL);
  p_this->set_polarization(handle, polar);
}

/*!
  Set 22k onoff
  \param[in] handle for nc 
  \param[in] onoff22k 1 is on and 0 is off
  */
void nc_set_22k(class_handle_t handle, u8 onoff22k)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_22k != NULL);
  p_this->set_22k(handle, onoff22k);
}

/*!
  Set 12v onoff
  \param[in] handle for nc 
  \param[in] onoff12v 1 is on and 0 is off
  */
void nc_set_12v(class_handle_t handle, u8 onoff12v)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_12v != NULL);
  p_this->set_12v(handle, onoff12v);
}


/*!
  Get the performance of certain channel
  
  \param[in] handle: nim ctrl handle
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
void nc_switch_ctrl(class_handle_t handle, nim_diseqc_level_t diseqc_level,
  rscmd_switch_mode_t mode, u8 port)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->switch_ctrl != NULL);
  p_this->switch_ctrl(handle, diseqc_level, mode, port);
}

/*!
  set position
  
  \param[in] handle: nim ctrl handle
  \param[in] cmd: the cmd to disqec
  \param[in] param: cmd with paramter
  */
void nc_positioner_ctrl(class_handle_t handle, rscmd_positer_t cmd, u32 param)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->positioner_ctrl != NULL);
  p_this->positioner_ctrl(handle, cmd, param);
}

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] sat_longitude: sat longitude
  \param[in] local_longitude: local longitude
  \param[in] local_latitude: local latitude
  */
void nc_usals_ctrl(class_handle_t handle, double sat_longitude, 
  double local_longitude, double local_latitude)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->usals_ctrl != NULL);
  p_this->usals_ctrl(handle, sat_longitude,
    local_longitude, local_latitude);
}

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  \param[in] polar nim_lnb_porlar_t mode
  \return TRUE: need wait time when moving dish FALSE: no need waiting
  */
BOOL nc_set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_diseqc != NULL);
  return p_this->set_diseqc(handle, p_diseqc);
}

/*!
  Clear diseqc info
  \param[in] handle for nc service 
  */
void nc_clr_diseqc_info(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->clr_diseqc_info != NULL);
  p_this->clr_diseqc_info(handle);
}


