/******************************************************************************/
/******************************************************************************/
#ifndef __MDL_H_
#define __MDL_H_

/*!
  \file mdl.h
  This file provides the common definition used in all files of mid-ware.
  The interface from MDL to AP also defined here
  */
/*! MDL enum module*/
enum
{
  /*!
    MDL module list start
    */
  MDL_MODULE_START = 0,
  /*!
    MDL network interface module control module
    */  
  MDL_NIM_CTRL,
  /*!
    MDL engine module
    */
  MDL_DVB,
  /*!
    Transport shell module
    */
  MDL_TRANSPORT_SHELL,
  /*!
    MDL EPG data module
    */
  MDL_EPG_DATA,
  /*!
    MDL VBI module
    */
  MDL_VBI,
  /*!
    MDL PNP SERVICE module
    */
  MDL_PNP_SVC,
  /*!
    MDL SUBT module
    */
  MDL_SUBT,
  /*!
    MDL MONITOR SERVICE module
    */
  MDL_MONITOR_SVC,
  /*!
    MDL module list end
    */
  MDL_MODULE_END
};

/*!
  Initialize middleware utility module
  */
void mdl_init(void);
/*!
  Initialize middleware utility module
  */
void mdl_init1(void);
/*!
  Set message queue id

  \param[in] handle class handle
  \param[in] id queue id
  */
void mdl_set_msgq(handle_t handle, u32 id);

/*!
  Broadcast message

  \param[in] p_msg message information to be broadcast
  */
void mdl_broadcast_msg(os_msg_t *p_msg);

/*!
  Send message
  
  \param[in] p_msg message information to be sent
  */
void mdl_send_msg(os_msg_t *p_msg);

#endif // End for __MDL_H_
