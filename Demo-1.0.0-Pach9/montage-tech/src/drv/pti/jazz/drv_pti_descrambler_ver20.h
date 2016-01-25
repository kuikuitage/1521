/******************************************************************************/
/******************************************************************************/

#ifndef __DRV_PTI_DESCRAMBLER_H__
#define __DRV_PTI_DESCRAMBLER_H__


/*!
  Global Config Item:SWITCH_ENABLE * SWITCH_ENABLE: 1, enable  * SWITCH_ENABLE: 0, disable
  */
#define SWITCH_ENABLE  1

/*!
  Definition of PTI  config register
  */
#define PTI_SLOT_BASE_ADDR                      0x61000000

/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE0_ADDR                   0x610000c0
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE1_ADDR                   0x61000180
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE2_ADDR                   0x61000240
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE3_ADDR                   0x61000300
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE4_ADDR                   0x610003c0
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE5_ADDR                   0x61000480
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE6_ADDR                   0x61000540
/*!
  Definition of PTI  config register
  */
#define PTI_FILTER_BASE7_ADDR                   0x61000600

/*!
  Definition of PTI  config register
  */
#define PTI_DES_KEY_BASE_ADDR                   0x610006c0

/*!
  Definition of PTI  config register
  */
#define PTI_REG_GLOBAL                          0x61000740
/*!
  Definition of PTI  config register
  */
#define PTI_REG_INT_STAH                        0x61000744
/*!
  Definition of PTI  config register
  */
#define PTI_REG_INT_STAL                        0x61000748
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT_STAH                       0x6100074c
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT_STAL                       0x61000750
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FILTER_STAH                     0x61000754
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FILTER_STAL                     0x61000758
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FIL_SFPROTECT                   0x61000760
/*!
  Definition of PTI  config register
  */
#define PTI_REG_MEMFREE_PROTECT_H               0x61000764
/*!
  Definition of PTI  config register
  */
#define PTI_REG_MEMFREE_PROTECT_L               0x61000768
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT_SFPROTECT                  0x6100076c
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SWITCHSTA_H           0x61000770
/*!
  Definition of PTI  config register
  */
#define PTI_REG_SWITCHSTA_L           0x61000774
/*!
  Definition of PTI  config register
  */
#define PTI_REG_ENDCRC_PROTECT                  0x61000778
/*!
  Definition of PTI  config register
  */
#define PTI_REG_DEBUG1                          0x6100078c
/*!
  Definition of PTI  config register
  */
#define PTI_CPU_PID_SET0            0X6F8A0008
/*!
  Definition of PTI  config register
  */
#define PTI_CPU_PID_SET1            0X6F8A000C
/*!
  Definition of PTI  config register
  */
#define PTI_ES_PLAYER_SET           0X6F8A0004


/*!
  Definition of PTI  config register
  */
#define PTI_REG_SLOT(x)                         ((PTI_SLOT_BASE_ADDR) + (4) * (x))

/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD0(x)                     ((PTI_FILTER_BASE0_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD1(x)                     ((PTI_FILTER_BASE1_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD2(x)                     ((PTI_FILTER_BASE2_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD3(x)                     ((PTI_FILTER_BASE3_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD4(x)                     ((PTI_FILTER_BASE4_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD5(x)                     ((PTI_FILTER_BASE5_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD6(x)                     ((PTI_FILTER_BASE6_ADDR) + (4) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_FLTR_WD7(x)                     ((PTI_FILTER_BASE7_ADDR) + (4) * (x))

/*!
  Definition of PTI  config register
  */
#define PTI_REG_ODD_KEY03(x)                    ((PTI_DES_KEY_BASE_ADDR) + (0x0) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_ODD_KEY47(x)                    ((PTI_DES_KEY_BASE_ADDR) + (0x4) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_EVEN_KEY03(x)                   ((PTI_DES_KEY_BASE_ADDR) + (0x8) + (16) * (x))
/*!
  Definition of PTI  config register
  */
#define PTI_REG_EVEN_KEY47(x)                   ((PTI_DES_KEY_BASE_ADDR) + (0xc) + (16) * (x))

/*!
  Definition of PTI  config register
  */
#define SECT_LEN(x)  (((*((volatile U8 *)(x) + 1) & 0xf) << 8) + (*((volatile U8 *)(x) + 2)) + 3)

/*!
  Slot const
  */
#define SLOT_CONST_INIT                         0xFFFFFFFFUL

/*!
  Slot const
  */
#define SLOT_CONST_DISABLE_SLOT                 0x3UL
/*!
  Slot const
  */
#define SLOT_CONST_ENABLE_SLOT                  0x2UL

/*!
  Slot const
  */
#define SLOT_CONST_DECMODE_TS                   0x1UL
/*!
  Slot const
  */
#define SLOT_CONST_DECMODE_PES                  0x0

/*!
  lu_yuefei 2007-5-22 调整默认解扰ID,原来的ID为0,可能有影响,如果音视频通道不在前面,可能会引起解扰失败
  */
#define SLOT_CONST_DECNUM_INIT                  0x7UL 


/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_ZERO                  0x0
/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_PID_REVISE            0x1UL
/*!
  Slot const
  */
#define SLOT_CONST_RAMIDX_PID_NOT_REVISE        0x0UL

/*!
  Slot const
  */
#define SLOT_CONST_ENABLE_NEW_FILTER            0x3FUL

/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_UNKNOWN             0x7UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_VBI                  0x5UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_TS                  0x4UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_PCR                 0x3UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_AUDIO               0x2UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_VIDEO               0x1UL
/*!
  Slot const
  */
#define SLOT_CONST_CONTTYPE_SECT                0x0
/*!
  Slot const
  */
#define SLOT_CONST_CPU_PID_SET_MASK             0x1fff


/*!
  filter const
  */
#define FLTR_CONST_WD6_INIT                     0xFFFFFFFFUL
/*!
  filter const
  */
#define FLTR_CONST_WD7_INIT                     0x00000000UL

/*!
  filter const
  */
#define FLTR_CONST_MASK_INIT                    0x0
/*!
  filter const
  */
#define FLTR_CONST_FLTR_INIT                    0x0

/*!
  filter const
  */
#define FLTR_CONST_RELEASE_SLOT                 0x3FUL
/*!
  filter const
  */
#define FLTR_CONST_SECTION_NUM_INIT             0x0
/*!
  filter const
  */
#define FLTR_CONST_BUF_LEN_INIT                 0x0

/*!
  filter const
  */
#define FLTR_CONST_CRC_ERROR                    0x1
/*!
  filter const
  */
#define FLTR_CONST_CRC_CORRECT                  0x0

/*!
  filter const
  */
#define FLTR_CONST_CRC_ENABLE                   0x1
/*!
  filter const
  */
#define FLTR_CONST_CRC_DISABLE                  0x0

/*!
  filter const
  */
#define FLTR_CONST_SECTION_MULTI                0x1
/*!
  filter const
  */
#define FLTR_CONST_SECTION_SINGLE               0x0

/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_1                   0x0
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_2                   0x1
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_4                   0x2
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_8                   0x3
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_16                  0x4
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_32                  0x5
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_64                  0x6
/*!
  filter const
  */
#define FLTR_CONST_INT_FREQ_128                 0x7

/*!
  filter const
  */
#define FLTR_CONST_TS_MODE_FILTER               0x0
/*!
  filter const
  */
#define FLTR_CONST_TS_MODE_NOFILTER             0x1

/*!
  filter const
  */
#define FLTR_CONST_TS_MULTI                     0x1
/*!
  filter const
  */
#define FLTR_CONST_TS_SINGLE                    0x0

/*!
  filter const
  */
#define FLTR_CONST_TS_CIRCLE_BUF                0x1
/*!
  filter const
  */
#define FLTR_CONST_TS_LINE_BUF                  0x0

/*!
  filter const
  */
#define FLTR_CONST_INT_FRQ_NORMAL               0x4


/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_SERIAL               0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_PARALEL              0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_MODE                 GLOBAL_CONST_TUNER_SERIAL

/*!
  global regist const
  */
#define GLOBAL_CONST_TSOUT_POSEDGE              0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_TSOUT_NEGEDGE              0x1

/*!
  global regist const
  */
#define GLOBAL_CONST_DISCARD_ERR_TS             0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_PRESERVE_ERR_TS            0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_TSSAMP_RST_ENABLE  0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_TSSAMP_RST_DISABLE  0x1

/*!
  global regist const
  */
#define GLOBAL_CONST_TSLOST_RSTEN               0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_TSLOST_RSTDIS              0x1

/*!
  global regist const
  */
#define GLOBAL_CONST_SWITCH_GLOBALRST           0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_SWITCH_SELF                0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_PESDEC_SOFTMODE            0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_PESDEC_AUTOMODE            0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_DATA_ENABLE          0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_TUNER_DATA_DISABLE         0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_SECERR_DEALEN              0x0
/*!
  global regist const
  */
#define GLOBAL_CONST_SECERR_DEALDIS             0x1

/*!
  global regist const
  */
#define GLOBAL_CONST_BYTE_PTIMODE               0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_BYTE_SDMODE                0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_GLOBAL_RST_NEW    0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_GLOBAL_RST_OLD    0x0

/*!
  global regist const
  */
#define GLOBAL_CONST_RAM_LOCK                   0x1
/*!
  global regist const
  */
#define GLOBAL_CONST_RAM_UNLOCK                 0x0

/*!
  约600us
  */
#define GLOBAL_CONST_TSDLEV_16BITCNT            0x0
/*!
  10ms
  */
#define GLOBAL_CONST_TSDLEV_20BITCNT            0x1
/*!
  约160ms
  */
#define GLOBAL_CONST_TSDLEV_24BITCNT            0x2
/*!
  约2.5s
  */
#define GLOBAL_CONST_TSDLEV_28BITCNT            0x3

/*!
  commments
  */
#define GLOBAL_CONST_TSDSRC_TSCLK               0x0
/*!
  commments
  */
#define GLOBAL_CONST_TSDSRC_TSSAMP              0x1
/*!
  commments
  */
#define GLOBAL_CONST_TSDSRC_TSSYNC              0x2
/*!
  commments
  */
#define GLOBAL_CONST_TSDSRC_TSSYNCIN            0x3

/*!
  key des const
  */
#define DES_KEY_CONST_INIT                      0x0

/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_ENABLE_SLOT                  30
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_DEC_MODE                     29
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_DEC_NUM                      26
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_MUL_DES                      23
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_SRC_STA                      22
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_ENABLE_FILTER_NUM            16
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_CONT_TYPE                    13
/*!
  Slot Bits Shift
  */
#define SLOT_SHIFT_PID                          0

/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SLOT_NUM                   26
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_BUF_LENGTH                 9
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SECTION_NUM                0

/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_CRCERR_FLAG                30
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_CRC_ENABLE                 29
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SM_CTRK                    28
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_BUF_END                    26
/*!
  filter mode section bits shift
  */
#define FILTER_SHIFT_SDRAM_STADDR               0

/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_TS_NUM                     0
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_INT_NUM                    29
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_MULTI                      28
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_CIRCLE                     28
/*!
  ts mode bits shift
  */
#define FILTER_SHIFT_TS_MODE                    27

/*!
  global shift
  */
#define GLOBAL_SHIFT_TUNER_MODE                 0
/*!
  global shift
  */
#define GLOBAL_SHIFT_TSOUT_EDGE                 1
/*!
  global shift
  */
#define GLOBAL_SHIFT_ERR_DEAL                   2
/*!
  global shift
  */
#define GLOBAL_SHIFT_TSSAMP_RST                 3
/*!
  global shift
  */
#define GLOBAL_SHIFT_TSLOST_RSTEN               4
/*!
  global shift
  */
#define GLOBAL_SHIFT_SWITCH_RSTMODE             5
/*!
  global shift
  */
#define GLOBAL_SHIFT_PESDEC_CTRLMODE            6
/*!
  global shift
  */
#define GLOBAL_SHIFT_TUNER_ENABLE               8
/*!
  global shift
  */
#define GLOBAL_SHIFT_SECERR_DETECTDIS           9
/*!
  global shift
  */
#define GLOBAL_SHIFT_BYTE_TRANSMODE             10
/*!
  global shift
  */
#define GLOBAL_SHIFT_RAM_LOCK                   16
/*!
  global shift
  */
#define GLOBAL_SHIFT_GLOBAL_RST_MODE    17
/*!
  global shift
  */
#define GLOBAL_SHIFT_TSDETECT_LEVEL             24
/*!
  global shift
  */
#define GLOBAL_SHIFT_TSDETECT_SRC               26

/*!
  filter software protect shift
  */
#define FILPROTECT_SHIFT_UPDATE_FLAG            7
/*!
  filter software protect shift
  */
#define FILPROTECT_SHIFT_UPDATE_NUM             0

/*!
  slot software protect shift
  */
#define SLOTPROTECT_SHIFT_UPDATE_FLAG           7
/*!
  slot software protect shift
  */
#define SLOTPROTECT_SHIFT_UPDATE_NUM            0
/*!
  Slot const
  */
#define SLOT_SHIFT_CPU_PID_SET_VEDIO  0
/*!
  Slot const
  */
#define SLOT_SHIFT_CPU_PID_SET_AUDIO  16
/*!
  Slot const
  */
#define SLOT_SHIFT_CPU_PID_SET_PCR    0
/*!
  Slot const
  */
#define SLOT_SHIFT_CPU_PID_SET_VBI    16


/*!
  PTI debug1 register shift
  */
#define DEBUG1_SHIFT_SFERR                      20

/*!
  slot bit mask
  */
#define SLOT_MASK_ENABLE_SLOT                   0xC0000000
/*!
  slot bit mask
  */
#define SLOT_MASK_DEC_MODE                      0x20000000
/*!
  slot bit mask
  */
#define SLOT_MASK_DEC_NUM                       0x1C000000
/*!
  slot bit mask
  */
#define SLOT_MASK_RAM_IDX                       0x03C00000
/*!
  slot bit mask
  */
#define SLOT_MASK_SWITCH_RST                    0x00400000
/*!
  slot bit mask
  */
#define SLOT_MASK_FILTER_NUM                    0x003F0000
/*!
  slot bit mask
  */
#define SLOT_MASK_DATA_TYPE                     0x0000D000
/*!
  slot bit mask
  */
#define SLOT_MASK_PID                           0x00001FFF

/*!
  filter bit mask
  */
#define FILTER_MASK_SLOT_NUM                    0xFC000000
/*!
  filter bit mask
  */
#define FILTER_MASK_BUF_LENGTH                  0x03FFFE00
/*!
  filter bit mask
  */
#define FILTER_MASK_SECTION_NUM                 0x000001FF

/*!
  filter bit mask
  */
#define FILTER_MASK_CRCERR_FLAG                 0x40000000
/*!
  filter bit mask
  */
#define FILTER_MASK_CRC_ENABLE                  0x20000000
/*!
  filter bit mask
  */
#define FILTER_MASK_SM_CTRK                     0x10000000
/*!
  filter bit mask
  */
#define FILTER_MASK_SDRAM_STADDR                0x03FFFFFF
/*!
  filter bit mask
  */
#define FILTER_MASK_BUF_END                     0x04000000

/*!
  filter bit mask
  */
#define FILTER_MASK_TS_NUM                      0x000001FF
/*!
  filter bit mask
  */
#define FILTER_MASK_INT_NUM                     0xE0000000
/*!
  filter bit mask
  */
#define FILTER_MASK_TS_MODE                     0x08000000
/*!
  filter bit mask
  */
#define FILTER_MASK_CIRCLE                      0x04000000

/*!
  filter bit mask
  */
#define DEBUG1_MASK_SFERR                       0x00f00000

/*!
  filter bit mask
  */
#define PTI_RESET_DECODER                       0x6f800004

/*!
  *lu_yuefei 为什么要加上188字节? 因为根据IC手册，当buff小于188字节时，会触发一个中断
  *这个中断频繁产生会影响效率。所以增加188字节防止无用的中断产生.
  */
#define MAX_PTI_LOOPBUFF_FILTER_BUF_LEN         (64 * 1024+188)
/*!
  有效filter,包含单表和多表
  */
#define MAX_PTI_VALID_FILTER_BUF_LEN            (8 * 1024)
/*!
  commments
  */
#define MAX_PTI_DOUBLE_FILTER_BUF_LEN           (32 * 1024)
/*!
  commments
  */
#define LOOP_BUFF_UNIT_LEN                      ((MAX_PTI_LOOPBUFF_FILTER_BUF_LEN) / 4)  
/*!
  commments
  */
#define DOUBLE_SUBFILT_RST_LEN                  ((MAX_PTI_DOUBLE_FILTER_BUF_LEN) / 4)


/*!
  commments
  */
#define MAX_MASK_DATA_LEN                       (MAX_PTI_MASK_LEN)




/*!
  传输层强制为32个了
  */
#define MAX_PTI_SLOT_NUM                        48
/*!
  传输层强制为32个了
  */
#define MAX_PTI_FILTER_NUM                      48

/*!
  实际物理filter个数
  */
#define MAX_PTI_FILTER_HAL_NUM                  48

/*!
  *lu_yuefei 2007-4-18 实际测试发现，这个buff只需要很小就够了
  *只要4K,现在设为10K应该保险
  */
#define PTI_CIR_BUFF_LEN                        (1 * 1024 * 1024)


/*!
  有效filter的个数
  */
#define PTI_VALID_FILTER_NUM                    24
/*!
  补丁filter个数
  */
#define PTI_PATCH_FILTER_NUM                    (MAX_PTI_FILTER_HAL_NUM - PTI_VALID_FILTER_NUM)


/*!
  commments
  */
#define MAX_PTI_CW_NUM                          0x08
/*!
  commments
  */
#define INV_PTI_FLTR                            0xFF
/*!
  commments
  */
#define INV_PTI_SLOT                            0xFF
/*!
  commments
  */
#define MAX_PTI_CW_LEN                          0x08



/*!
  commments
  */
#define PTI_SLOT_MAX_PID                        0x1FFF
/*!
  commments
  */
#define PTI_DVB_EMPTY_PID                       (PTI_SLOT_MAX_PID)

/*!
  patch_level0,定于370，用大小做到即刻复位
  */
#define PATCH_FILTER_HAL_LEVEL0_BUFFER_SIZE     370




/*!
  commments
  */
#define ERROR_PLATFORM_DRV_PTI_BASE             0x100a0000
/*!
  commments
  */
#define PTI_ERROR_NO_CHANNEL                    ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 1)
/*!
  commments
  */
#define PTI_ERROR_NO_FILTER                     ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 2)
/*!
  commments
  */
#define PTI_ERROR_NO_FILTER_AT_CHANNEL          ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 3)
/*!
  commments
  */
#define PTI_ERROR_NO_MEMORY                     ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 4)
/*!
  commments
  */
#define PTI_INVALID_CHANNEL_HANDLE              ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 5)
/*!
  commments
  */
#define PTI_INVALID_FILTER_HANDLE               ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 6)
/*!
  commments
  */
#define PTI_FILTER_NOT_LINK_TO_CHANNEL          ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 7)
/*!
  commments
  */
#define PTI_POINTER_IS_NULL                     ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 8)
/*!
  commments
  */
#define PTI_ERROR_PARAMS                        ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 9)
/*!
  commments
  */
#define PTI_CHANNEL_LOGIC_ERROR                 ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 10)
/*!
  commments
  */
#define PTI_REPEAT_PID_AT_CHANNEL               ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 11)
/*!
  commments
  */
#define PTI_CHANNEL_NOT_CREATE                  ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 12)
/*!
  commments
  */
#define PTI_CHANNEL_OUT_OF_FILTER_COUNT         ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 13)
/*!
  commments
  */
#define PTI_CHANNEL_CREATE_FILTER_FAILED        ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 14)
/*!
  commments
  */
#define PTI_CHANNEL_FREE_FILTER_FAILED          ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 15)
/*!
  commments
  */
#define PTI_FILTER_OUT_OF_CHANNEL               ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 16)
/*!
  commments
  */
#define PTI_HANDLI_IS_NULL                      ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 17)
/*!
  commments
  */
#define PTI_NOT_SUPPORT                         ((U32)ERROR_PLATFORM_DRV_PTI_BASE | 100)




#endif //__DRV_PTI_DESCRAMBLER_H__
