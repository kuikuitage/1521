/******************************************************************************/
/******************************************************************************/

#ifndef __DRV_PTI_DESCRAMBLER_H__
#define __DRV_PTI_DESCRAMBLER_H__

/*!
  #include "drv_cfg.h"
  #include "drv_api.h"
  
  #include "drv_pti.h"
  #include "drv_descrambler.h"
  */


/*!
  Global Config Item:SWITCH_ENABLE * SWITCH_ENABLE: 1, enable  * SWITCH_ENABLE: 0, disable
  */
#define SWITCH_ENABLE  1

/*!
  commments
  */
#define PTI_VERSION     100

/*!
  Definition of PTI  config register
  */
#define PTI_SLOT_BASE_ADDR              0x61000000
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE0_ADDR           0x610000c0
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE1_ADDR           0x61000180
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE2_ADDR           0x61000240
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE3_ADDR           0x61000300
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE4_ADDR           0x610003c0
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE5_ADDR           0x61000480
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE6_ADDR           0x61000540
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE7_ADDR           0x61000600
/*!
  Definition of PTI  config register
  */
#define PTI_DES_KEY_BASE_ADDR         0x610006c0
/*!
  Definition of PTI  config register
  */
#define PTI_REG_GLOBAL           0x61000740
/*!
  Definition of PTI  config register
  */
#define PTI_REG_INT_STAH           0x61000744
/*!
  Definition of PTI  config register
  */
#define PTI_REG_INT_STAL           0x61000748
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT_STAH         0x6100074c
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT_STAL         0x61000750
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FILTER_STAH         0x61000754
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FILTER_STAL         0x61000758

/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT(x)              ((PTI_SLOT_BASE_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD0(x)         ((PTI_FILTER_BASE0_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD1(x)         ((PTI_FILTER_BASE1_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD2(x)         ((PTI_FILTER_BASE2_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD3(x)         ((PTI_FILTER_BASE3_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD4(x)         ((PTI_FILTER_BASE4_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD5(x)         ((PTI_FILTER_BASE5_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD6(x)         ((PTI_FILTER_BASE6_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD7(x)         ((PTI_FILTER_BASE7_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_ODD_KEY03(x)        ((PTI_DES_KEY_BASE_ADDR) + (0x0) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_ODD_KEY47(x)        ((PTI_DES_KEY_BASE_ADDR) + (0x4) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_EVEN_KEY03(x)       ((PTI_DES_KEY_BASE_ADDR) + (0x8) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_EVEN_KEY47(x)       ((PTI_DES_KEY_BASE_ADDR) + (0xc) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define SECT_LEN(x)   (((*((volatile U8 *)(x) + 1) & 0xf) << 8) + (*((volatile U8 *)(x) + 2)) + 3)

/*!
  Slot const
  */
#define SLOT_CONST_INIT            0xFFFFFFFF  
/*!
  Slot const
  */
#define SLOT_CONST_DISABLE_SLOT        0x3
/*!
  Slot const
  */
#define SLOT_CONST_ENABLE_SLOT        0x2
/*!
  Slot const
  */
#define SLOT_CONST_DECMODE_TS        0x1
/*!
  Slot const
  */
#define SLOT_CONST_DECMODE_PES         0x0
/*!
  Slot const
  */
#define SLOT_CONST_DECNUM_INIT         0x0
/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_ZERO        0x0
/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_PID_REVISE    0x1
/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_PID_NOT_REVISE  0x2
/*!
  Slot const
  */
#define SLOT_CONST_ENABLE_NEW_FILTER    0x3F
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_UNKNOWN      0x7
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_TS        0x4
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_PCR        0x3
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_AUDIO      0x2
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_VIDEO      0x1
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_SECT      0x0

/*!
  filter const
  */
#define FLTR_CONST_WD6_INIT          0xFC000000
/*!
  filter const
  */
#define FLTR_CONST_WD7_INIT          0xFFFFFFFF
/*!
  filter const
  */
#define FLTR_CONST_MASK_INIT        0x0
/*!
  filter const
  */
#define FLTR_CONST_FLTR_INIT        0x0
/*!
  filter const
  */
#define FLTR_CONST_RELEASE_SLOT        0x3F
/*!
  filter const
  */
#define FLTR_CONST_SECTION_NUM_INIT      0x0
/*!
  filter const
  */
#define FLTR_CONST_BUF_LEN_INIT        0x0
/*!
  filter const
  */
#define FLTR_CONST_CRC_ERROR        0x1
/*!
  filter const
  */
#define FLTR_CONST_CRC_CORRECT        0x0
/*!
  filter const
  */
#define FLTR_CONST_CRC_ENABLE        0x1
/*!
  filter const
  */
#define FLTR_CONST_CRC_DISABLE        0x0
/*!
  filter const
  */
#define FLTR_CONST_SECTION_MULTI      0x1
/*!
  filter const
  */
#define FLTR_CONST_SECTION_SINGLE      0x0
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_1               0x0
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_2               0x1
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_4               0x2
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_8               0x3
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_16              0x4
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_32              0x5
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_64              0x6
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_128             0x7
/*!
  filter const
  */
#define FLTR_CONST_TS_MODE_FILTER      0x0
/*!
  filter const
  */
#define FLTR_CONST_TS_MODE_NOFILTER      0x1
/*!
  filter const
  */
#define FLTR_CONST_TS_MULTI              0x1
/*!
  filter const
  */
#define FLTR_CONST_TS_SINGLE        0x0
/*!
  filter const
  */
#define FLTR_CONST_TS_CIRCLE_BUF      0x1
/*!
  filter const
  */
#define FLTR_CONST_TS_LINE_BUF        0x0
/*!
  filter const
  */
#define FLTR_CONST_INT_FRQ_NORMAL      0x4

/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_SERIAL      0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_PARALEL      0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_MODE             GLOBAL_CONST_TUNER_SERIAL
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_DATA_ENABLE    0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_DATA_DISABLE    0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_RAM_LOCK        0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_RAM_UNLOCK        0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_DISCARD_ERR_TS      0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_PRESERVE_ERR_TS    0x0

/*!
  key des const
  */
#define DES_KEY_CONST_INIT          0x0

/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_ENABLE_SLOT          30
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_DEC_MODE               29
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_DEC_NUM                 26
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_RAM_IDX               22
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_ENABLE_FILTER_NUM    16
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_CONT_TYPE        13
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_PID            0

/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SLOT_NUM        26
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_BUF_LENGTH            9
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SECTION_NUM      0
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_CRCERR_FLAG      30
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_CRC_ENABLE          29
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SM_CTRK               28
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SDRAM_STADDR          0

/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_TS_NUM            0
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_INT_NUM               29
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_MULTI               28
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_TS_MODE               27
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_CIRCLE                26

/*!
  global shift
  */
#define GLOBAL_SHIFT_TUNER_MODE          0
/*!
  global shift
  */
#define GLOBAL_SHIFT_ERR_DEAL        2 
/*!
  global shift
  */
#define GLOBAL_SHIFT_TUNER_ENABLE      8
/*!
  global shift
  */
#define GLOBAL_SHIFT_RAM_LOCK        16

/*!
  slot bit mask
  */
#define SLOT_MASK_ENABLE_SLOT        0xC0000000
/*!
  slot bit mask
  */
#define SLOT_MASK_DEC_MODE               0x20000000
/*!
  slot bit mask
  */
#define SLOT_MASK_DEC_NUM                 0x1C000000
/*!
  slot bit mask
  */
#define SLOT_MASK_RAM_IDX               0x03C00000
/*!
  slot bit mask
  */
#define SLOT_MASK_FILTER_NUM        0x003F0000
/*!
  slot bit mask
  */
#define SLOT_MASK_DATA_TYPE            0x0000D000
/*!
  slot bit mask
  */
#define SLOT_MASK_PID            0x00001FFF

/*!
  filter bit mask
  */
#define FILTER_MASK_SLOT_NUM        0xFC000000
/*!
  filter bit mask
  */
#define FILTER_MASK_BUF_LENGTH            0x03FFFE00
/*!
  filter bit mask
  */
#define FILTER_MASK_SECTION_NUM          0x000001FF
/*!
  filter bit mask
  */
#define FILTER_MASK_CRCERR_FLAG          0x40000000
/*!
  filter bit mask
  */
#define FILTER_MASK_CRC_ENABLE          0x20000000
/*!
  filter bit mask
  */
#define FILTER_MASK_SM_CTRK               0x10000000
/*!
  filter bit mask
  */
#define FILTER_MASK_SDRAM_STADDR          0x03FFFFFF
/*!
  filter bit mask
  */
#define FILTER_MASK_TS_NUM            0x000001FF
/*!
  filter bit mask
  */
#define FILTER_MASK_INT_NUM               0xE0000000
/*!
  filter bit mask
  */
#define FILTER_MASK_TS_MODE               0x08000000
/*!
  filter bit mask
  */
#define FILTER_MASK_CIRCLE                0x04000000


/*!
  comments
  */
typedef unsigned int               pti_slot_t;
/*!
  comments
  */
#define PTI_SOLT_T                  pti_slot_t 
/*!
  comments
  */
#define DRV_PTI_INIT                DRV_PTI_STOP
/*!
  comments
  */
#define PTI_CHANSTATUS_T            pti_status_t
/*!
  comments
  */
#define BOOL                        BOOLEAN
/*!
  comments
  */
#define MAX_PTI_BUF_LEN            0x4000  
/*!
  comments
  */
#define MAX_MASK_DATA_LEN           MAX_PTI_MASK_LEN


/*!
  comments
  */
#define MAX_PTI_SLOT_NUM        0x30
/*!
  comments
  */
#define MAX_PTI_FILTER_NUM        0x30
/*!
  comments
  */
#define MAX_PTI_CW_NUM          0x08
/*!
  comments
  */
#define INV_PTI_FLTR                    0xFF
/*!
  comments
  */
#define INV_PTI_SLOT                    0xFF
/*!
  comments
  */
#define MAX_PTI_CW_LEN          0x08

/*!
  comments
  */
#define PTI_SLOT_MAX_PID            0x1FFF
/*!
  comments
  */
#define PTI_DVB_EMPTY_PID            PTI_SLOT_MAX_PID
 

/*!
  comments
  */
#define DESCRAMBLER_LEVELTYPE_T descrambler_level_t


/*!
  comments
*/
typedef enum
{
  /*!
    comments
  */
  DESCRAMBLER_OBJECT_PES,
  /*!
    comments
  */
  DESCRAMBLER_OBJECT_SECTION,
  /*!
    comments
  */
  DESCRAMBLER_OBJECT_TYPE_END
} descrambler_objecttype_t;




/*!
  comments
*/
typedef struct 
{
  /*!
    comments
  */
  BOOL           m_Used;
  /*!
    comments
  */
  U32             m_ChanlId;
  /*!
    comments
  */
  U16            m_SlotType;
  /*!
    comments
  */
  U16           m_Pid;
  /*!
    comments
  */
  U8              m_PtiMode;
  /*!
    comments
  */
  U8            m_State; 
} ptislot_t;


/*!
  comments
  */
typedef struct 
{
  /*!
    comments
  */
  U32        m_ReadPtr;    
  /*!
    comments
  */
  U32             m_LogFltrId;
  /*!
    comments
  */
  BOOL       m_Used; 
  /*!
    comments
  */
  U8            m_State; 
  /*!
    comments
  */
  BOOL            m_dummy;
  /*!
    comments
  */
  BOOL            m_need;
  /*!
    comments
  */
  U32             m_WrTo;
  /*!
    comments
  */
  U32             m_Write[2];
  /*!
    comments
  */
  U32             m_Read[2];
  /*!
    comments
  */
  U32             m_times;
} ptifltr_t;

/*!
  comments
  */
typedef struct 
{
  /*!
    comments
  */
  BOOL            m_Used;
  /*!
    comments
  */
  U16           m_Type;
  /*!
    comments
  */
  U32             m_Slot;
  /*!
    comments
  */
  descrambler_level_t m_level;
} ptides_t;


/*!
  comments
  */
typedef struct 
{
  /*!
    comments
  */
  ptislot_t           m_PtiSlot[MAX_PTI_SLOT_NUM];
  /*!
    comments
  */
  ptifltr_t           m_PtiFltr[MAX_PTI_FILTER_NUM];
  /*!
    comments
  */
  ptides_t        m_PtiDes[MAX_PTI_CW_NUM];
} ptiresdata_t;




/*!
  comments
  */
#define CHANNEL_NUM     MAX_PTI_SLOT_NUM
/*!
  comments
  */
#define FILTER_NUM      MAX_PTI_FILTER_NUM
/*!
  comments
  */
typedef struct
{
  /*!
    comments
  */
  BOOL               m_Used;
  /*!
    comments
  */
  U32                 m_Slot;
  /*!
    comments
  */
  U32                 m_FltrNum;    
  /*!
    comments
  */
  U32                 m_Fltr1st;
  /*!
    comments
  */
  U32                m_MaxFltr;     
  /*!
    comments
  */
  U32                m_MaxFltrSize; 
  /*!
    comments
  */
  U32                m_MaxBuf;      
  /*!
    comments
  */
  U32                m_Pid;
  /*!
    comments
  */
  U8                m_State; 
  /*!
    comments
  */
  filter_type_t       m_FilterType;
  /*!
    comments
  */
  pti_channeltype_t   m_type;
  /*!
    comments
  */
  BOOL                m_typeset;
} channel_t;

/*!
  comments
  */
typedef struct
{
  /*!
    comments
  */
  U32             m_FilerId;
  /*!
    comments
  */
  BOOL      m_Used;
  /*!
    comments
  */
  BOOL            m_hasRept;
  /*!
    comments
  */
  U32             m_ChanId;     
  /*!
    comments
  */
  U32             m_Next; 
  /*!
    comments
  */
  U32             m_PtiFilt; 
  /*!
    comments
  */
  U8        m_ReqMode;
  /*!
    comments
  */
  U8              m_CRCCheck;
  /*!
    comments
  */
  BOOL      m_Valid;
  /*!
    comments
  */
  U8        m_State;
  /*!
    comments
  */
  U32             m_Master;
  /*!
    comments
  */
  U32             m_Slave;
  /*!
    comments
  */
  pti_maskdata_t  m_MaskData;
  /*!
    comments
  */
  PTI_CallBack_t call_back;  
} filter_t;

/*!
  comments
  */
typedef struct 
{
  /*!
    comments
  */
  U32                 m_UsedChan;
  /*!
    comments
  */
  U32                 m_UsedFilt;
  /*!
    comments
  */
  channel_t           m_Channel[CHANNEL_NUM];
  /*!
    comments
  */
  filter_t           m_Filter[FILTER_NUM];
  /*!
    comments
  */
  PTI_CallBack_t      m_pCallBackFunc;
  /*!
    comments
  */
  U32            m_TaskPtiMonitor; 
  /*!
    comments
  */
  U32            m_Sem;    
  /*!
    comments
  */
  BOOL                m_Inited;
} logresdata_t;

/*!
  comments
  */
#define TSK_PRIO_PTIMONITOR     TASK_PRIO_PTIRECV

/*!
  comments
  */
#define DRV_PTI_CREATECHANNEL  drv_pti_createchannel
/*!
  comments
  */
#define DRV_PTI_DESTORYCHANNEL drv_pti_destorychannel
/*!
  comments
  */
#define DRV_PTI_SETCHANNELPID  drv_pti_setchannelpid
/*!
  comments
  */
#define DRV_PTI_CONTROLCHANNEL drv_pti_controlchannel
/*!
  comments
  */
#define DRV_PTI_CREATEFILTER   drv_pti_createfilter
/*!
  comments
  */
#define DRV_PTI_DESTORYFILTER  drv_pti_destoryfilter
/*!
  comments
  */
#define DRV_PTI_SETFILTER      drv_pti_setfilter
/*!
  comments
  */
#define DRV_PTI_CONTROLFILTER  drv_pti_controlfilter
/*!
  comments
  */
#define DESCRAMBLER_ASSOCIATION_SLOT_TYPE DESCRAMBLER_ASSOCIATION_CHANNEL_TYPE
/*!
  comments
  */
#define DRV_PTI_CREATEDESCRAMBLER drv_pti_createdescrambler
/*!
  comments
  */
#define DRV_PTI_DELETEDESCRAMBLER drv_pti_deletedescrambler
/*!
  comments
  */
#define DRV_PTI_ATTACHDESCRAMBLER drv_pti_attachdescrambler
/*!
  comments
  */
#define DRV_PTI_SETDESCRAMBLERCW  drv_pti_setdescramblercw
/*!
  comments
  */
#define DRV_PTI_INIT drv_pti_init
/*!
  comments
  */
#define DRV_PTI_TERM drv_pti_term

#endif //__DRV_PTI_DESCRAMBLER_H__
