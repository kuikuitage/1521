/******************************************************************************/
/******************************************************************************/
#ifndef __CLASS_FACTORY_H_
#define __CLASS_FACTORY_H_

/*!
  Class handle
  */
typedef void * class_handle_t;

/*!
  Complete id set supported in current system
  */
typedef enum 
{
  /*!
    Invalid 
    */
  IN_VALID_CLASS_ID = 0,
  /*!
    Class id for gb2312
    */
  GB2312_CLASS_ID = 1,

  /*!
    Class id for big5
    */
  BIG5_CLASS_ID = 2,

  /*!
    Class id for surface
    */
  SURFACE_CLASS_ID = 3,

  /*!
    Class id for mmi
    */
  MMI_CLASS_ID = 4,

  /*!
    Class id for gdi
    */
  GDI_CLASS_ID = 5,

  /*!
    Class id for gui control
    */
  GUI_CLASS_ID = 6,

  /*!
    Class id for gui paint
    */
  PAINT_CLASS_ID = 7,

  /*!
    Class id for gui scrip
    */
  SCRIPT_CLASS_ID = 8,

  /*!
    Class id for resource
    */
  RESOURCE_CLASS_ID = 9,

  /*!
    Class id for framework
    */
  UIFRM_CLASS_ID = 10,

  /*!
    Class id for AV control
    */
  AVC_CLASS_ID = 11,

  /*!
    Class id for database
    */
  DB_CLASS_ID = 12,

  /*!
    Class id for data manager
    */
  DM_CLASS_ID = 13,

  /*!
    Class id for DVB
    */
  DVB_CLASS_ID = 14,

  /*!
    Class id for NIM control
    */
  NC_CLASS_ID = 15,

  /*!
    Class id for system status control
    */
  SC_CLASS_ID = 16, 

  /*!
    Class id for time
    */
  TIME_CLASS_ID = 17,

  /*!
    Class id for epg
    */
  EPG_CLASS_ID = 18,

  /*!
    Class id for simple queue
    */
  SIMPLE_Q_CLASS_ID = 19,

  /*!
    Class id for app framework
    */
  AP_FRM_CLASS_ID = 20,

  /*!
    Class id for middleware utility
    */
  MDL_CLASS_ID = 21,

  /*!
    Class id for subtitle
    */
  SUBT_CLASS_ID = 22,
  /*!
    Class id VBI/Teletex
    */
  VBI_CLASS_ID = 23,
  /*!
    Class id VBI/Teletex ctrl
    */
  VBI_SUBT_CTRL_CLASS_ID = 24,  
  /*!
    Class id for AP EPG private
    */
  AP_EPG_PRV_CLASS_ID = 25,
  /*!
    CLASS ID for saving playback impl data
  */
  MAGIC_KEY_CLASS_ID = 26,
  /*!
    CLASS ID for smart card control
    */
  SMART_CARD_CLASS_ID = 27,
  /*!
    CLASS ID for scan  impl default
    */
  SCAN_IMPL_CLASS_ID = 28,
  /*!
    CLASS ID for file list
    */
  FILE_LIST_CLASS_ID = 29,
  /*!
    VFA class ID
    */
  VFS_CLASS_ID = 30,
  /*!
    NVOD class ID
    */
  NVOD_CLASS_ID = 31,   

  /*!
    Class id for lib char
    */
  LIB_CHAR_CLASS_ID = 32,

  /*!
    monitor service class ID
    */
  M_SVC_CLASS_ID,    
  /*!
    pnp service class ID
    */
  PNP_SVC_CLASS_ID,    
  /*!
     SOCKET class ID
     */
    SOCKET_CLASS_ID,  
  /*!
    CLASS ID for saving playback impl data
    */
    PRESET_IMPL_CLASS_ID,
  /*!
    Class id for NVOD mosaic
    */
  NVOD_MOSAIC_CLASS_ID,
  /*!
    CLASS ID for saving playback impl data
    */
  MDX_CLASS_ID,
  /*!
    Roll class id
    */
  ROLL_CLASS_ID,  
  /*!
    vector font class id
    */
  VFONT_CLASS_ID,     
  /*!
    log manager class id
  */
  LOG_MGR_CLASS_ID = 60,
  
  /*!
    Max item number supported in class factory
    */
  CLASS_ID_CNT
}class_id_t;


/*!
  Register class
  \param[in] class_id for all valid module in system
  \param[in] p_class_handle size of class in class factory
  */
void class_register(u8 class_id, class_handle_t p_class_handle);

/*!
  Get the proc function
  \param[in] class_id registered class id
  \param[out] class handle in function sets
  */
class_handle_t class_get_handle_by_id(u8 class_id);

#endif
