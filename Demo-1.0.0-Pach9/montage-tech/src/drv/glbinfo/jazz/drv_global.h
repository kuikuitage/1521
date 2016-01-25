/******************************************************************************/
/******************************************************************************/

#ifndef __DRV_GLOBAL_H__
#define __DRV_GLOBAL_H__

/*!
  comments
  */
#define   GLB_DEV_NUM      3


/*!
  Definition of INFO register
  */
#define    INFO_BASE_ADDR          0x6f890000

/*!
  0x6f890000
  */
#define    INFO_0_MASK_REG                 (INFO_BASE_ADDR + 0x00)
/*!
  0x6f890004
  */
#define    INFO_0_GLOBAL_REG               (INFO_BASE_ADDR + 0x04)
/*!
  0x6f890008
  */
#define    INFO_0_GINT_STAT_CLR_REG        (INFO_BASE_ADDR + 0x08)
/*!
  0x6f89000c
  */
#define    INFO_0_GINT_STAT_REG            (INFO_BASE_ADDR + 0x0c)
/*!
  0x6f890010
  */
#define    INFO_1_MASK_REG                 (INFO_BASE_ADDR + 0x10)
/*!
  0x6f890014
  */
#define    INFO_1_GLOBAL_REG               (INFO_BASE_ADDR + 0x14)
/*!
  0x6f890018
  */
#define    INFO_1_GINT_STAT_CLR_REG        (INFO_BASE_ADDR + 0x18)
/*!
  0x6f89001c
  */
#define    INFO_1_GINT_STAT_REG            (INFO_BASE_ADDR + 0x1c)
/*!
  0x6f890020
  */
#define    INFO_2_MASK_REG                 (INFO_BASE_ADDR + 0x20)
/*!
  0x6f890024
  */
#define    INFO_2_GLOBAL_REG               (INFO_BASE_ADDR + 0x24)
/*!
  0x6f890028
  */
#define    INFO_2_GINT_STAT_CLR_REG        (INFO_BASE_ADDR + 0x28)
/*!
  0x6f89002c
  */
#define    INFO_2_GINT_STAT_REG            (INFO_BASE_ADDR + 0x2c)
/*!
  debug
  */
#define   BUSREQ_MASK_REG          0x6f80000c
/*!
  debug
  */
#define   BUSREQ_RESET_REG          0x6f800010


/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_SYNC_ABNORMAL_MASK      0x00000020
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_AUDIO_SYNC_ABNORMAL_MASK      0x00000040
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_OUT_ABNORMAL_MASK      0x00000080
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_DMA_OP_COMPLETE_MASK        0x00000100
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_TS_FIFO_FULL_MASK        0x00000200
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_TS_FIFO_HALF_FULL_MASK      0x00000400
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_AUDIO_VOLUME_OVERFLOW_MASK    0x00004000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_TS_ERROR_MASK          0x00008000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_DECODE_ERROR_MASK      0x00010000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_AUDIO_DECODE_ERROR_MASK      0x00020000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_ES_OVERFLOW_MASK      0x00040000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_AUDIO_ES_OVERFLOW_MASK      0x00080000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_TUNER_PULL_OUT_MASK        0x00100000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_UNSURPPORT_WIDTH_MASK    0x00800000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_IMAGE_REBUILD_ERROR_MASK      0x01000000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_AUDIO_FIRST_FRAME_MASK      0x02000000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_VIDEO_FIRST_FRAME_MASK      0x04000000
/*!
  Definition of Global0 interrupt bits Masks
  */
#define    GLB0_P_N_MASK      0x00000007

/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_0_FIFO_EMPTY_MASK          0x00000001
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_0_FIFO_FULL_MASK          0x00000002
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_1_FIFO_EMPTY_MASK          0x00000004
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_1_FIFO_FULL_MASK          0x00000008
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_V_BUF_FULL_MASK          0x00000010
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_V_BUF_EMPTY_MASK          0x00000020
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_A_BUF_FULL_MASK          0x00000040
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_A_BUF_EMPTY_MASK          0x00000080
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_VBI_BUF_FULL_MASK          0x00000100
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_ES_VBI_BUF_EMPTY_MASK        0x00000200
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_A_PARSE_ERROR_MASK        0x00000400
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_V_PARSE_ERROR_MASK        0x00000800
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_CONTINUITY_CNT_ERROR_MASK      0x00001000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_DISCONTINUITY_BIT_ERROR_MASK    0x00002000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_TS_DISCONTINUITY_COUNTER_MASK    0x00004000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_VPTS_DIS_INCREASE_MASK        0x00008000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_STREAM_A_HEAD_PARSE_ERROR_MASK    0x00010000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_STREAM_A_CRC_ERROR_MASK        0x00020000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_STREAM_A_ALLOC_PARSE_ERROR_MASK    0x00040000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_STREAM_A_ERROR_MASK          0x00080000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_PTS_V_RECEIVE_MASK          0x00100000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_PTS_A_RECEIVE_MASK          0x00200000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_PTS_VBI_RECEIVE_MASK          0x00400000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_RESERVED_23_MASK            0x00800000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_VID_PARSE_ERROR_MASK          0x01000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_RESERVED_25_MASK            0x02000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_PCR_INTERRUPT_MASK            0x04000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_RESERVED_27_MASK            0x08000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_RESERVED_28_MASK            0x10000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_MUTE_RESET_REQ_MASK          0x20000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_BUSY_DMA_0_MASK            0x40000000
/*!
  Definition of Global1 interrupt bits Masks
  */
#define   GLB1_BUSY_DMA_1_MASK            0x80000000

/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_00_MASK          0x00000001
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_01_MASK          0x00000002
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_02_MASK          0x00000004
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_03_MASK          0x00000008
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_04_MASK          0x00000010
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_05_MASK          0x00000020
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_USER_DATA_06_MASK          0x00000040
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_07_MASK          0x00000080
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_08_MASK          0x00000100
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_09_MASK          0x00000200
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_10_MASK          0x00000400
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_11_MASK          0x00000800
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_12_MASK          0x00001000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_13_MASK          0x00002000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_14_MASK          0x00004000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_15_MASK          0x00008000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_16_MASK          0x00010000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_17_MASK          0x00020000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_18_MASK          0x00040000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_19_MASK          0x00080000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_20_MASK               0x00100000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_21_MASK          0x00200000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_22_MASK          0x00400000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_23_MASK          0x00800000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_24_MASK          0x01000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_25_MASK          0x02000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_26_MASK          0x04000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_27_MASK          0x08000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_28_MASK          0x10000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_29_MASK          0x20000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_UD_ES_30_MASK                0x40000000
/*!
  Definition of Global2 interrupt bits Masks
  */
#define   GLB2_RESERVED_31_MASK          0x80000000

/*!
  comments
  */
enum
{
  /*!
    comments
    */
  GLB0_VIDEO_SYNC_ABNORMAL,
  /*!
    comments
    */
  GLB0_AUDIO_SYNC_ABNORMAL,
  /*!
    comments
    */
  GLB0_VIDEO_OUT_ABNORMAL,
  /*!
    comments
  */
  GLB0_DMA_OP_COMPLETE,
  /*!
    comments
  */
  GLB0_TS_FIFO_FULL,
  /*!
    comments
  */
  GLB0_TS_FIFO_HALF_FULL,
  /*!
    comments
  */
  GLB0_AUDIO_VOLUME_OVERFLOW,
  /*!
    comments
  */
  GLB0_TS_ERROR,
  /*!
    comments
  */
  GLB0_VIDEO_DECODE_ERROR,
  /*!
    comments
  */
  GLB0_AUDIO_DECODE_ERROR,
  /*!
    comments
  */
  GLB0_VIDEO_ES_OVERFLOW,
  /*!
    comments
  */
  GLB0_AUDIO_ES_OVERFLOW,
  /*!
    comments
  */
  GLB0_TUNER_PULL_OUT,
  /*!
    comments
  */
  GLB0_VIDEO_UNSURPPORT_WIDTH,
  /*!
    comments
  */
  GLB0_IMAGE_REBUILD_ERROR,
  /*!
    comments
  */
  GLB0_AUDIO_FIRST_FRAME,
  /*!
    comments
  */
  GLB0_VIDEO_FIRST_FRAME,
  /*!
    comments
  */
  GLB0_P_N,
  /*!
    comments
  */
  GLB0_END
};

/*!
  comments
  */
enum
{
  /*!
    comments
    */
  GLB1_TS_0_FIFO_EMPTY,
  /*!
    comments
    */
  GLB1_TS_0_FIFO_FULL,
  /*!
    comments
    */
  GLB1_TS_1_FIFO_EMPTY,
  /*!
    comments
  */
  GLB1_TS_1_FIFO_FULL,
  /*!
    comments
  */
  GLB1_ES_V_BUF_FULL,
  /*!
    comments
  */
  GLB1_ES_V_BUF_EMPTY,
  /*!
    comments
  */
  GLB1_ES_A_BUF_FULL,
  /*!
    comments
  */
  GLB1_ES_A_BUF_EMPTY,
  /*!
    comments
  */
  GLB1_ES_VBI_BUF_FULL,
  /*!
    comments
  */
  GLB1_ES_VBI_BUF_EMPTY,
  /*!
    comments
  */
  GLB1_TS_A_PARSE_ERROR,
  /*!
    comments
  */
  GLB1_TS_V_PARSE_ERROR,
  /*!
    comments
  */
  GLB1_TS_CONTINUITY_CNT_ERROR,
  /*!
    comments
  */
  GLB1_TS_DISCONTINUITY_BIT_ERROR,
  /*!
    comments
  */
  GLB1_TS_DISCONTINUITY_COUNTER,
  /*!
    comments
  */
  GLB1_VPTS_DIS_INCREASE,
  /*!
    comments
  */
  GLB1_STREAM_A_HEAD_PARSE_ERROR,
  /*!
    comments
  */
  GLB1_STREAM_A_CRC_ERROR,
  /*!
    comments
  */
  GLB1_STREAM_A_ALLOC_PARSE_ERROR,
  /*!
    comments
  */
  GLB1_STREAM_A_ERROR,
  /*!
    comments
  */
  GLB1_PTS_V_RECEIVE,
  /*!
    comments
  */
  GLB1_PTS_A_RECEIVE,
  /*!
    comments
  */
  GLB1_PTS_VBI_RECEIVE,
  /*!
    comments
  */
  GLB1_RESERVED_23,
  /*!
    comments
  */
  GLB1_VID_PARSE_ERROR,
  /*!
    comments
  */
  GLB1_RESERVED_25,
  /*!
    comments
  */
  GLB1_RESERVED_26,
  /*!
    comments
  */
  GLB1_RESERVED_27,
  /*!
    comments
  */
  GLB1_RESERVED_28,
  /*!
    comments
  */
  GLB1_MUTE_RESET_REQ,
  /*!
    comments
  */
  GLB1_BUSY_DMA_0,
  /*!
    comments
  */
  GLB1_BUSY_DMA_1,
};

/*!
  comments
  */
enum
{
  /*!
    comments
  */
  GLB2_SHIFT_BIT0,
  /*!
    comments
  */
  GLB2_SHIFT_BIT1,
  /*!
    comments
  */
  GLB2_SHIFT_BIT2,
  /*!
    comments
  */
  GLB2_SHIFT_BIT3,
  /*!
    comments
  */
  GLB2_SHIFT_BIT4,
  /*!
    comments
  */
  GLB2_SHIFT_BIT5,
  /*!
    comments
  */
  GLB2_WR_USER_DATA_BIT6,
  /*!
    comments
  */
  GLB2_SHIFT_BIT7,
  /*!
    comments
  */
  GLB2_SHIFT_BIT8,
  /*!
    comments
  */
  GLB2_SHIFT_BIT9,
  /*!
    comments
  */
  GLB2_SHIFT_BIT10,
  /*!
    comments
  */
  GLB2_SHIFT_BIT11,
  /*!
    comments
  */
  GLB2_SHIFT_BIT12,
  /*!
    comments
  */
  GLB2_SHIFT_BIT13,
  /*!
    comments
  */
  GLB2_SHIFT_BIT14,
  /*!
    comments
  */
  GLB2_SHIFT_BIT15,
  /*!
    comments
  */
  GLB2_SHIFT_BIT16,
  /*!
    comments
  */
  GLB2_SHIFT_BIT17,
  /*!
    comments
  */
  GLB2_SHIFT_BIT18,
  /*!
    comments
  */
  GLB2_SHIFT_BIT19,
  /*!
    comments
  */
  GLB2_SHIFT_BIT20,
  /*!
    comments
  */
  GLB2_SHIFT_BIT21,
  /*!
    comments
  */
  GLB2_SHIFT_BIT22,
  /*!
    comments
  */
  GLB2_SHIFT_BIT23,
  /*!
    comments
  */
  GLB2_SHIFT_BIT24,
  /*!
    comments
  */
  GLB2_SHIFT_BIT25,
  /*!
    comments
  */
  GLB2_SHIFT_BIT26,
  /*!
    comments
  */
  GLB2_SHIFT_BIT27,
  /*!
    comments
  */
  GLB2_SHIFT_BIT28,
  /*!
    comments
  */
  GLB2_SHIFT_BIT29,
  /*!
    comments
  */
  GLB2_UD_ES_RST_BIT30,
  /*!
    comments
  */
  GLB2_SHIFT_BIT31,
};


/*!
  comments
  */
enum
{
  /*!
    comments
  */
  MASTER_ID_VIDEO_DECODE,
  /*!
    comments
  */
  MASTER_ID_AUDIO_DECODE,
  /*!
    comments
  */
  MASTER_ID_OSD,
  /*!
    comments
  */
  MASTER_ID_VIDEO_OUT,
  /*!
    comments
  */
  MASTER_ID_AV_ES_RD,
  /*!
    comments
  */
  MASTER_ID_AV_ES_WR,
  /*!
    comments
  */
  MASTER_ID_AV_ES,
  /*!
    comments
  */
  MASTER_ID_PTI,
  /*!
    comments
  */
  MASTER_ID_AUDIO_OUT,
  /*!
    comments
  */
  MASTER_ID_I2C,
  /*!
    comments
  */
  MASTER_ID_DMA = 12,
  /*!
    comments
  */
  MASTER_ID_AVSYNC,
  /*!
    comments
  */
  MASTER_ID_UART1,
  /*!
    comments
  */
  MASTER_ID_SMARTCARD = 16,
  /*!
    comments
  */
  MASTER_ID_LEDKB,
  /*!
    comments
  */
  MASTER_ID_IRDA,
  /*!
    comments
  */
  MASTER_ID_SYSTERM = 31,
  /*!
    comments
  */
  MASTER_ID_MAX
};

/*!
  comments
  */
typedef struct
{
  /*!
    comments
  */
  u32  m_Mask;
  /*!
    comments
  */
  char  *p_Info;
  /*!
    comments
  */
  void (*GlobalCb)(void);
}glbinfo_t;

/*!
  Definition of GLOBAL ErrorCode
  */
#define ERR_GLOBAL_NOERR  0x0
/*!
  Definition of GLOBAL ErrorCode
  */
#define ERR_GLOBAL_RESULT  0x1
/*!
  Definition of GLOBAL ErrorCode
  */
#define ERR_GLOBAL_PARAM  0x2

/*!
  comments
  */
void drv_globalinit(void);
/*!
  comments
  */
ERRORCODE_T drv_glbalhandlerregister(u8 devid, u8 GlbInx, void (*p_handler)(void));
/*!
  comments
  */
ERRORCODE_T drv_globalsetinton(u8 devid, u32 IrqInx);
/*!
  comments
  */
ERRORCODE_T drv_globalsetintoff(u8 devid, u32 IrqInx);
/*!
  comments
  */
ERRORCODE_T drv_globalgetintstate(u8 devid, u32 IrqInx, u8 *p_State);
/*!
  comments
  */
ERRORCODE_T drv_globalclrintstate(u8 devid, u32 IrqInx);
/*!
  comments
  */
ERRORCODE_T drv_globalgetintinfo(u8 devid, u32 IrqInx, u8 *p_State);
/*!
  comments
  */
ERRORCODE_T drv_globalmasterrestet(u8 masterid);

#endif //__DRV_GLOBAL_H__
