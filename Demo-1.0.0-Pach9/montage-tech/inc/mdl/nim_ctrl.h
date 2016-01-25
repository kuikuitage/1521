/******************************************************************************/
/******************************************************************************/
#ifndef __NIM_CTRL_H_
#define __NIM_CTRL_H_

/*!
  Nim control class declaration
  */
typedef struct
{
  /*!
    \see nc_set_tp
    */
  void (*set_tp)(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info);
  /*!
    \see nc_get_tp
    */
  void (*get_tp)(class_handle_t handle, nc_channel_info_t *p_tp_info);                  
  /*!
    \see nc_get_lock_status
    */
  BOOL (*get_lock_status)(class_handle_t handle);
  /*!
    \see nc_is_finish_locking
    */
  BOOL (*is_finish_locking)(class_handle_t handle);
  /*!
    \see nc_perf_get
    */
  void (*perf_get)(class_handle_t handle, nim_channel_perf_t *p_perf);
  /*!
    \see nc_set_blind_scan_mode
    */
  void (*set_blind_scan_mode)(class_handle_t handle, BOOL bs);
  /*!
    \see nc_enable_monitor
    */
  void (*enable_monitor)(class_handle_t handle, BOOL enable);
  /*!
    \see lnb_check_monitor
    */
  void (*lnb_check_enable)(class_handle_t handle, BOOL enable);
  /*!
    \see lnb_check_monitor
    */
  BOOL (*is_lnb_check_enable)(class_handle_t handle);  
  /*!
    \see nc_set_polarization
    */
  void (*set_polarization)(class_handle_t handle, nim_lnb_porlar_t polar);
  /*!
    \see nc_set_22k
    */
  void (*set_22k)(class_handle_t handle, u8 onoff22k);
  /*!
    \see nc_set_12v
    */
  void (*set_12v)(class_handle_t handle, u8 onoff12v);
  /*!
    \see nc_tone_burst
    */
  BOOL (*tone_burst)(class_handle_t handle, nim_diseqc_mode_t mode);
  /*!
    \see nc_switch_ctrl
    */
  void (*switch_ctrl)(class_handle_t handle,
          nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port);
  /*!
    \see nc_positioner_ctrl
    */
  void (*positioner_ctrl)(class_handle_t handle,
        rscmd_positer_t cmd, u32 param);
  /*!
    \see nc_usals_ctrl
    */
  void (*usals_ctrl)(class_handle_t handle, double sat_longitude, 
          double local_longitude, double local_latitude);
  /*!
    \see nc_set_diseqc
    */
  BOOL (*set_diseqc)(class_handle_t handle, nc_diseqc_info_t *p_diseqc);

  /*!
    \see nc_set_diseqc
    */
  void (*clr_diseqc_info)(class_handle_t handle);

  /*!
    Nim ctrl private data
    */
  void *p_data;
} nim_ctrl_t;

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_set_tp(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info);
/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_get_tp(class_handle_t handle, nc_channel_info_t *p_tp_info);

/*!
  Determine the locking is finished or not
  \param[in] handle: nim ctrl handle
  */
BOOL nc_is_finish_locking(class_handle_t handle);

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  */
BOOL nc_get_lock_status(class_handle_t handle);

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[out] p_perf: channel performance
  */
void nc_perf_get(class_handle_t handle, nim_channel_perf_t *p_perf);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] bs TRUE is blind scan mode and false is not blind scan mode
  */
void nc_set_blind_scan_mode(class_handle_t handle, BOOL bs);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_monitor(class_handle_t handle, BOOL enable);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_lnb_check(class_handle_t handle, BOOL enable);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] enable TRUE is enable nc monitor
  */
BOOL nc_is_lnb_check_enable(class_handle_t handle);

/*!
  Set nim_lnb_porlar_t mode
  \param[in] handle for nc 
  \param[in] polar nim_lnb_porlar_t mode
  */
void nc_set_polarization(class_handle_t handle, nim_lnb_porlar_t polar);

/*!
  Set 22k onoff
  \param[in] handle for nc 
  \param[in] onoff22k 1 is on and 0 is off
  */
void nc_set_22k(class_handle_t handle, u8 onoff22k);

/*!
  Set 12v onoff
  \param[in] handle for nc 
  \param[in] onoff12v 1 is on and 0 is off
  */
void nc_set_12v(class_handle_t handle, u8 onoff12v);

/*!
  Get the performance of certain channel
  
  \param[in] handle: nim ctrl handle
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
void nc_switch_ctrl(class_handle_t handle, nim_diseqc_level_t diseqc_level,
  rscmd_switch_mode_t mode, u8 port);

/*!
  set position
  
  \param[in] handle: nim ctrl handle
  \param[in] cmd: the cmd to disqec
  \param[in] param: cmd with paramter
  */
void nc_positioner_ctrl(class_handle_t handle, rscmd_positer_t cmd, u32 param);

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] sat_longitude: sat longitude
  \param[in] local_longitude: local longitude
  \param[in] local_latitude: local latitude
  */
void nc_usals_ctrl(class_handle_t handle, double sat_longitude, 
                    double local_longitude, double local_latitude);

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  \param[in] polar nim_lnb_porlar_t mode
  */
BOOL nc_set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc);

/*!
  Clear diseqc info
  \param[in] handle for nc service 
  */
void nc_clr_diseqc_info(class_handle_t handle);


#endif // End for __NIM_CTRL_H_

