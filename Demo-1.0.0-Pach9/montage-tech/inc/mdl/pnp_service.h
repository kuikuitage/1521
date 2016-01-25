/******************************************************************************/
/******************************************************************************/
#ifndef __PNP_SERV_H__
#define __PNP_SERV_H__

/*!
  pnp service events
  */
typedef enum tag_pnp_svc_evt
{
  /*!
    Flag of evt start
    */
  PNP_SVC_EVT_START = (MDL_PNP_SVC << 16),
  /*!
    Invoke this event when found usb/sd card plug in 0x01
    */
  PNP_SVC_USB_PLUG_IN,
  /*!
    Invoke this event when found usb/sd card plug out
    */
  PNP_SVC_USB_PLUG_OUT,
  /*!
    Invoke this event when found internet plug in   0x03
    */
  PNP_SVC_INTERNET_PLUG_IN,
  /*!
    Invoke this event when found internet plug out
    */
  PNP_SVC_INTERNET_PLUG_OUT,
  /*!
    Invoke this event when found hdmi plug in     0x05
    */
  PNP_SVC_HDMI_PLUG_IN,
  /*!
    Invoke this event when found hdmi plug out
    */
  PNP_SVC_HDMI_PLUG_OUT,
  /*!
    Flag of evt end
    */
  PNP_SVC_EVT_END
}pnp_svc_event_t;


/*!
  init parameter pnp service.
  */
typedef struct tag_pnp_svc_init_para
{
  /*!
    service's priority.
    */
  u32 service_prio;
  /*!
    service's stack size.
    */
  u32 nstksize;
}pnp_svc_init_para_t;


/*!
  pnp svc class declaration
  */
typedef struct tag_pnp_svc
{
  /*!
    get a service handle from pnp svc module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);

  /*!
    pnp svc class privated data
    */
  void *p_data;
}pnp_svc_t;


/*!
  init pnp service.

  \param[in] service_prio   service task priority.

  \return TRUE or FALSE.
  */
handle_t pnp_service_init(pnp_svc_init_para_t *p_para);

#endif

