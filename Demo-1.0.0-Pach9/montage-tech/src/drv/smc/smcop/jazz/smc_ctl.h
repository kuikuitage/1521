/******************************************************************************/
/******************************************************************************/

#ifndef __SMC_CTL_H__
#define __SMC_CTL_H__

/*!
  1518 registers address
  */
/*!
  ���ݼĴ���
  */
#define REG_WR_SMC_DATA    0x45000000
/*!
  ״̬�Ĵ���
  */
#define REG_RO_SMC_STATUS  0x45000004
/*!
  ����֧�֣���Ƶϵ��
  */
#define REG_WR_SMC_CONFIG  0x45000008
/*!
  ���Ᵽ��ʱ��
  */
#define REG_WR_SMC_NC    0x4500000c
/*!
  ����
  */
#define REG_WR_SMC_CTRL    0x45000010
/*!
  �������ÿ�Ƭ��etuֵ
  */
#define REG_WR_SMC_ETU1    0x45000014
/*!
  �������ÿ�Ƭ��etuֵ
  */
#define REG_WR_SMC_ETU2    0x45000018
/*!
  �������ÿ�Ƭ��etuֵ
  */
#define REG_WR_SMC_ETU3    0x4500001c
/*!
  ��Դ����λ���ƹ���
  */
#define REG_WR_SMC_RSTCTRL  0x45000020
/*!
  ����ȸ�λ
  */
#define REG_WR_SMC_CMDLEN_H  0x45000024
/*!
  ����ȵ�λ
  */
#define REG_WR_SMC_CMDLEN_L  0x45000028
/*!
  FIFO������
  */
#define REG_RO_SMC_FIFOCNT  0x4500002c



/*!
  commments
  */
#define SMC_FREQUENCY_DIV_DEFAULT  9
/*!
  commments
  */
#define SMC_FREQUENCY_DIV_T14    7

/*!
  620 for IS1521, 590 for IS1526
  */
#define SMC_ETU_T14    620
/*!
  commments
  */
#define SMC_ETU_DEFAULT  372


/*!
  SMC CONFIG VALUE
*/
typedef enum
{
  /*!
    commments
  */
  SMC_DISABLE = 0,
  /*!
    commments
  */
  SMC_ENABLE
}smc_cfg;

/*!
  SMC STOP BIT NUMBER
*/
typedef enum
{
  /*!
    commments
  */
  SMC_STOP_TWO = 0,
  /*!
    commments
  */
  SMC_STOP_ONE
}smc_bitstop_t;

/*!
  ���ܿ���������
*/
typedef enum
{
  /*!
    commments
  */
  SMC_SOCKET_CLOSED,
  /*!
    commments
  */
  SMC_SOCKET_OPEN
}smc_sock_types;

/*!
  VCC ģʽ
*/
typedef enum
{
  /*!
    commments
  */
  SMC_VCC_INTERNAL,
  /*!
    commments
  */
  SMC_VCC_EXTERNAL
}smc_vcc_mod;


/*!
  sc get status
  */
#define SMC_GET_STATUS()  (inb(REG_RO_SMC_STATUS) & 0x02)

/*!
  sc get rx status
  */
#define SMC_RX_NOTEMPTY()  (inb(REG_RO_SMC_STATUS) & 0x01)

/*!
  sc get one reply
  */
#define SMC_RX_GET()    (inb(REG_WR_SMC_DATA))

/*!
  sc set cmd length
  */
#define SMC_TX_CMDLEN(len)  \
  do { outb(((len & 0xff00) >> 8), REG_WR_SMC_CMDLEN_H);\
    outb(((len & 0xff)), REG_WR_SMC_CMDLEN_L); \
    } \
  while(0)

/*!
  sc set cmd data
  */
#define SMC_TX_CMD(data)    (outb(data, REG_WR_SMC_DATA))

/*!
  commments
  */
#define SMC_FIFO_CNT()      (inb(REG_RO_SMC_FIFOCNT))

/*!
  commments
  */
void smc_init(void (*smc_interrupt_handle)(void),int detect_pin_pol, int vcc_enable_pol);

/*!
  commments
  */
void smc_set_etu(u32 ETU);

/*!
  commments
  */
void smc_reset(u16 delays);

/*!
  commments
  */
void  smc_set_bitcoding(BOOL bT14);

/*!
  commments
  */
void smc_flush(void);

#endif //__SMC_CTL_H__


