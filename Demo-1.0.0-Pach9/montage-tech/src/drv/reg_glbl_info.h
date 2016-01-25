/******************************************************************************/
/******************************************************************************/
#ifndef __REG_GLBL_INFO_H__
#define __REG_GLBL_INFO_H__




/*!
  ABC
  */
#define GLBL_INFO_BASE                                  0x3f000000
/*!
   Register: info_mask_n
   Initialization Value: 0x00000000
   Initialization Mask: 0xffffffff
  */

/*!
   |---------------------------------------------------------------|
   |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
   |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
   |---------------------------------------------------------------|
   |                          info_mask_n                          |
   |---------------------------------------------------------------|
  */
/*!
  ABC
  */
#define GLBL_INFO_INFO_MASK_N_REG                ((GLBL_INFO_BASE) + 0x00000000)
/*!
  ABC
  */
#define GLBL_INFO_INFO_MASK_N_INITVALUE                 0x00000000
/*!
  ABC
  */
#define GLBL_INFO_INFO_MASK_N_INITMASK                  0xffffffff

/*!
  ABC
  */
#define GLBL_INFO_INFO_MASK_N_INFO_MASK_N_MASK          0xffffffff
/*!
  ABC
  */
#define GLBL_INFO_INFO_MASK_N_INFO_MASK_N_SHIFT         0

/*!
   Register: global_info
   Initialization Value: 0x00000000
   Initialization Mask: 0xfffffff9
  */

/*
   |---------------------------------------------------------------|
   |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
   |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
   |---------------------------------------------------------------|
   |   i   |t|d|d|p|o|a|a|t|a|v|a|v|t|a|  f  |t|t|d|v|a|v| v |  v  |
   |   n   |i|e|e|i|t|m|m|u|e|e|u|i|s|u|  r  |s|s|m|o|s|s| o |  i  |
   |   t   |m|c|c|c|h|b|b|n|s|s|d|d|_|d|  a  |_|_|a|u|y|y| u |  d  |
   |   _   |e|o|o|_|e|a|a|e|_|_|i|e|e|i|  m  |f|f|_|t|n|n| t |  e  |
   |   n   |c|d|d|s|r|_|_|r|o|o|o|o|r|o|  e  |i|i|b|_|c|c| _ |  o  |
   |   u   |h|e|e|i|_|e|e|_|v|v|d|d|r|_|  _  |f|f|u|d|_|_| c |  _  |
   |   m   |e|_|_|z|v|r|r|n|e|e|e|e|o|v|  n  |o|o|s|e|a|a| h |  f  |
   |   b   |c|o|a|e|i|r|r|s|r|r|c|c|r|o|  u  |_|_|y|a|b|b| r |  o  |
   |   e   |k|n|f|_|d|o|o| |f|f|_|_| |l|  m  |h|f| |t|n|n| o |  r  |
   |   r   |_|e|f|e|m|r|r| |l|l|d|d| |_|  b  |a|u| |h|o|o| m |  m  |
   |       |c|f|r|r|o|_|_| |o|o|i|i| |o|  e  |l|l| |_|r|r| a |  a  |
   |       |o|r|a|r|d|1|0| |w|w|e|e| |f|  r  |f|l| |f|m|m| _ |  t  |
   |       |m|a|m|o|e| | | | | | | | |l|     |_| | |l|a|a| f |     |
   |       |p|m|e|r|_| | | | | | | | |o|     |f| | |a|l|l| o |     |
   |       |l|e| | |i| | | | | | | | |w|     |u| | |g| | | r |     |
   |       |e| | | |n| | | | | | | | | |     |l| | | | | | m |     |
   |       |t| | | |t| | | | | | | | | |     |l| | | | | | a |     |
   |       |e| | | | | | | | | | | | | |     | | | | | | | t |     |
   |---------------------------------------------------------------|
 */

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_REG                ((GLBL_INFO_BASE) + 0x00000004)
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_INITVALUE                 0x00000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_INITMASK                  0xfffffff9

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_INT_NUMBER_MASK           0xf0000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_INT_NUMBER_SHIFT          28

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TIMECHECK_COMPLETE_MASK   0x08000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TIMECHECK_COMPLETE_SHIFT  27

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DECODE_ONEFRAME_MASK      0x04000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DECODE_ONEFRAME_SHIFT     26

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DECODE_AFFRAME_MASK       0x02000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DECODE_AFFRAME_SHIFT      25

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_PIC_SIZE_ERROR_MASK       0x01000000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_PIC_SIZE_ERROR_SHIFT      24

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_OTHER_VIDMODE_INT_MASK    0x00800000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_OTHER_VIDMODE_INT_SHIFT   23

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AMBA_ERROR_1_MASK         0x00400000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AMBA_ERROR_1_SHIFT        22

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AMBA_ERROR_0_MASK         0x00200000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AMBA_ERROR_0_SHIFT        21

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TUNER_NS_MASK             0x00100000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TUNER_NS_SHIFT            20

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AES_OVERFLOW_MASK         0x00080000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AES_OVERFLOW_SHIFT        19

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VES_OVERFLOW_MASK         0x00040000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VES_OVERFLOW_SHIFT        18

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AUDIODEC_DIE_MASK         0x00020000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AUDIODEC_DIE_SHIFT        17

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VIDEODEC_DIE_MASK         0x00010000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VIDEODEC_DIE_SHIFT        16

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_ERROR_MASK             0x00008000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_ERROR_SHIFT            15

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AUDIO_VOL_OFLOW_MASK      0x00004000
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_AUDIO_VOL_OFLOW_SHIFT     14

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_FIELD_NUMBER_MASK         0x00003800
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_FIELD_NUMBER_SHIFT        11

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_FIFO_HALF_FULL_MASK    0x00000400
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_FIFO_HALF_FULL_SHIFT   10

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_FIFO_FULL_MASK         0x00000200
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_TS_FIFO_FULL_SHIFT        9

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DMA_BUSY_MASK             0x00000100
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_DMA_BUSY_SHIFT            8

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VOUT_DEATH_FLAG_MASK      0x00000080
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VOUT_DEATH_FLAG_SHIFT     7

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_ASYNC_ABNORMAL_MASK       0x00000040
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_ASYNC_ABNORMAL_SHIFT      6

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VSYNC_ABNORMAL_MASK       0x00000020
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VSYNC_ABNORMAL_SHIFT      5

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VOUT_CHROMA_FORMAT_MASK   0x00000018
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VOUT_CHROMA_FORMAT_SHIFT  3

/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VIDEO_FORMAT_MASK         0x00000001
/*!
  ABC
  */
#define GLBL_INFO_GLOBAL_INFO_VIDEO_FORMAT_SHIFT        0

/*!
   Register: gint_state_clr
   Initialization Value: 0x00000000
   Initialization Mask: 0xffffffff
 */

/*!
   |---------------------------------------------------------------|
   |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
   |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
   |---------------------------------------------------------------|
   |                       gint_state_clear                        |
   |---------------------------------------------------------------|
  */

/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_CLR_REG             ((GLBL_INFO_BASE) + 0x00000008)
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_CLR_INITVALUE              0x00000000
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_CLR_INITMASK               0xffffffff

/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_CLR_GINT_STATE_CLEAR_MASK  0xffffffff
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_CLR_GINT_STATE_CLEAR_SHIFT 0

/*!
   Register: gint_state
   Initialization Value: 0x00000000
   Initialization Mask: 0xffffffff
  */

/*!
   |---------------------------------------------------------------|
   |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
   |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
   |---------------------------------------------------------------|
   |                          gint_state                           |
   |---------------------------------------------------------------|
  */

/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_REG                 ((GLBL_INFO_BASE) + 0x0000000c)
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_INITVALUE                  0x00000000
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_INITMASK                   0xffffffff

/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_GINT_STATE_MASK            0xffffffff
/*!
  ABC
  */
#define GLBL_INFO_GINT_STATE_GINT_STATE_SHIFT           0







#endif

