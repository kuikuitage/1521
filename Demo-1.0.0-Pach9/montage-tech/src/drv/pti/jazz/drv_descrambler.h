/******************************************************************************/
/******************************************************************************/



#ifndef __DRV_DESCRAMBER_H__
#define __DRV_DESCRAMBER_H__

/*!
  comments
  */
typedef unsigned int descrambler_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
  */
  DESCRAMBLER_ASSOCIATION_CHANNEL_TYPE = 0x00,
  /*!
    comments
  */
  DESCRAMBLER_ASSOCIATION_PID_TYPE,
  /*!
    comments
  */
  DESCRAMBLER_ASSOCIATION_TYPE_END
} descrambler_associationtype_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
  */
  DESCRAMBLER_CW_ODD_TYPE = 0x00,
  /*!
    comments
  */
  DESCRAMBLER_CW_EVEN_TYPE,
  /*!
    comments
  */
  DESCRAMBLER_CW_BOTH_TYPE,
  /*!
    comments
  */
  DESCRAMBLER_CW_TYPE_END
} descrambler_cwtype_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
  */
  DESCRAMBLE_TYPE_DES,
  /*!
    comments
  */
  DESCRAMBLE_TYPE_DVB,
  /*!
    comments
  */
  DESCRAMBLE_TYPE_END
}descramblertype_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
  */
  DESCRAMBLER_LEVEL_PES,
  /*!
    comments
  */
  DESCRAMBLER_LEVEL_TS,
  /*!
    Auto adaption of TS/PES layer descrambling. Need hardware support(0x61000740 bit6)
  */
  DESCRAMBLER_LEVEL_ALL,
  /*!
    comments
  */
  DESCRAMBLER_LEVEL_END
} descrambler_level_t;

/*!
  comments
  */
typedef struct tagdescramblerassociation
{
  /*!
    comments
  */
  descrambler_associationtype_t m_DescramblerAssociationType;
  /*!
    comments
  */
  union 
  {
    /*!
      comments
    */
    pti_channelhandle_t m_DescramblerSolt;
    /*!
      comments
    */
    pti_pid_t           m_DescramblerPid;
  } m_DescramblerAssociationHandle;

} descramble_association_t;

/*!
  Descrambler Driver Interface
*/
ERRORCODE_T drv_pti_attachdescrambler
(
  /*!
    comments
  */
  descrambler_t            DescramblerObject,
  /*!
    comments
  */
  descramble_association_t DescramblerAssociation
);

ERRORCODE_T drv_pti_createdescrambler
(
  /*!
    comments
  */
  descrambler_t *   DescramblerObject,
  /*!
    comments
  */
  descramblertype_t DescramblerType
);

ERRORCODE_T drv_pti_deletedescrambler
(
  /*!
    comments
  */
  descrambler_t  DescramblerObject
);

ERRORCODE_T drv_pti_setdescramblercw
(
  /*!
    comments
  */
  descrambler_t        DescramblerHandle,
  /*!
    comments
  */
  descrambler_level_t  level,
  /*!
    comments
  */
  descrambler_cwtype_t CWType,
  /*!
    comments
  */
  U8 *                 pCWValue,
  /*!
    comments
  */
  U8                   cwValueLen
);

#endif //__DRV_DESCRAMBER_H__

