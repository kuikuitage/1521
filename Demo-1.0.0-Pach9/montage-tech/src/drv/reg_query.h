/******************************************************************************/
/******************************************************************************/
#ifndef __REG_QUERY_H__
#define __REG_QUERY_H__




/*!
  ABC
  */
#define QUERY_BASE                              (0x3e000000)


/*
 * Register: stc32
 * Initialization Value: 0x00000000
 * Initialization Mask: 0xffffffff
 */

/*
 * |---------------------------------------------------------------|
 * |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
 * |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 * |---------------------------------------------------------------|
 * |                             stc32                             |
 * |---------------------------------------------------------------|
 */

/*!
  ABC
  */
#define QUERY_STC32_REG                                (QUERY_BASE + 0x00000080)
/*!
  ABC
  */
#define QUERY_STC32_INITVALUE                            0x00000000
/*!
  ABC
  */
#define QUERY_STC32_INITMASK                             0xffffffff

/*!
  ABC
  */
#define QUERY_STC32_STC32_MASK                           0xffffffff
/*!
  ABC
  */
#define QUERY_STC32_STC32_SHIFT                          0

/*
 * Register: stc_ext
 * Initialization Value: 0x00000000
 * Initialization Mask: 0x0001ff01
 */

/*
 * |---------------------------------------------------------------|
 * |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
 * |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 * |---------------------------------------------------------------|
 * |                             |                 |             |s|
 * |                             |                 |             |t|
 * |                             |     stc_ext     |             |c|
 * |                             |                 |             |3|
 * |                             |                 |             |3|
 * |---------------------------------------------------------------|
 */

/*!
  ABC
  */
#define QUERY_STC_EXT_REG                              (QUERY_BASE + 0x00000084)
/*!
  ABC
  */
#define QUERY_STC_EXT_INITVALUE                          0x00000000
/*!
  ABC
  */
#define QUERY_STC_EXT_INITMASK                           0x0001ff01

/*!
  ABC
  */
#define QUERY_STC_EXT_STC_EXT_MASK                       0x0001ff00
/*!
  ABC
  */
#define QUERY_STC_EXT_STC_EXT_SHIFT                      8

/*!
  ABC
  */
#define QUERY_STC_EXT_STC33_MASK                         0x00000001
/*!
  ABC
  */
#define QUERY_STC_EXT_STC33_SHIFT                        0

/*
 * |---------------------------------------------------------------|
 * |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
 * |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 * |---------------------------------------------------------------|
 * |                          pcr_base32                           |
 * |---------------------------------------------------------------|
 */

/*!
  ABC
  */
#define QUERY_PCR_BASE32_REG                           (QUERY_BASE + 0x00000088)
/*!
  ABC
  */
#define QUERY_PCR_BASE32_INITVALUE                       0x00000000
/*!
  ABC
  */
#define QUERY_PCR_BASE32_INITMASK                        0xffffffff

/*!
  ABC
  */
#define QUERY_PCR_BASE32_PCR_BASE32_MASK                 0xffffffff
/*!
  ABC
  */
#define QUERY_PCR_BASE32_PCR_BASE32_SHIFT                0

/*
 * Register: pcr_ext
 * Initialization Value: 0x00000000
 * Initialization Mask: 0x0001ff01
 */

/*
 * |---------------------------------------------------------------|
 * |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
 * |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 * |---------------------------------------------------------------|
 * |                             |                 |             |p|
 * |                             |                 |             |c|
 * |                             |                 |             |r|
 * |                             |                 |             |_|
 * |                             |                 |             |b|
 * |                             |     pcr_ext     |             |a|
 * |                             |                 |             |s|
 * |                             |                 |             |e|
 * |                             |                 |             |3|
 * |                             |                 |             |3|
 * |---------------------------------------------------------------|
 */

/*!
  ABC
  */
#define QUERY_PCR_EXT_REG                              (QUERY_BASE + 0x0000008c)
/*!
  ABC
  */
#define QUERY_PCR_EXT_INITVALUE                          0x00000000
/*!
  ABC
  */
#define QUERY_PCR_EXT_INITMASK                           0x0001ff01

/*!
  ABC
  */
#define QUERY_PCR_EXT_PCR_EXT_MASK                       0x0001ff00
/*!
  ABC
  */
#define QUERY_PCR_EXT_PCR_EXT_SHIFT                      8

/*!
  ABC
  */
#define QUERY_PCR_EXT_PCR_BASE33_MASK                    0x00000001
/*!
  ABC
  */
#define QUERY_PCR_EXT_PCR_BASE33_SHIFT                   0

#endif

