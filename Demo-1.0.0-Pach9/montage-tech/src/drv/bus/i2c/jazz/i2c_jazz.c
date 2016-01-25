/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "hal_irq.h"
#include "hal_base.h"
#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "../inc/drv/bus/i2c.h"
#include "../i2c_priv.h"

#define I2C_DEBUG

#ifdef I2C_DEBUG
#define I2C_DBG_PRINTF OS_PRINTF
#define ENTER_FUNC  OS_PRINTF("\n enter %s \n", __FUNCTION__)
#define LEAVE_FUNC  OS_PRINTF("\n leave %s \n", __FUNCTION__)
#else
#define I2C_DBG_PRINTF(x) do{}while(0)
#define ENTER_FUNC(x) do{}while(0)
#define LEAVE_FUNC(x) do{}while(0)
#endif

#undef FPGA_PLATFORM_DBG

typedef struct i2c_ctrller_wa_priv
{
  u16 i2c_clk;
  u8 i2c_id;
}i2c_ctrller_jazz_priv_t;

typedef struct i2c_ctrller_addr_priv
{
  u32 i2c_prer_h;
  u32 i2c_prer_l;
  u32 i2c_txr;
  u32 i2c_rxr;
  u32 i2c_sr;
  u32 i2c_ctr;
  u32 i2c_cr;
}i2c_ctrller_addr_priv_t;

i2c_ctrller_addr_priv_t i2c_address_table[] =
{
  {
    R_I2C_MASTER0_PRER_H,
    R_I2C_MASTER0_PRER_L,
    R_I2C_MASTER0_TXR,
    R_I2C_MASTER0_RXR,
    R_I2C_MASTER0_SR,
    R_I2C_MASTER0_CTR,
    R_I2C_MASTER0_CR
  },
  {
    R_I2C_MASTER1_PRER_H,
    R_I2C_MASTER1_PRER_L,
    R_I2C_MASTER1_TXR,
    R_I2C_MASTER1_RXR,
    R_I2C_MASTER1_SR,
    R_I2C_MASTER1_CTR,
    R_I2C_MASTER1_CR
  },
  {
    R_I2C_MASTER2_PRER_H,
    R_I2C_MASTER2_PRER_L,
    R_I2C_MASTER2_TXR,
    R_I2C_MASTER2_RXR,
    R_I2C_MASTER2_SR,
    R_I2C_MASTER2_CTR,
    R_I2C_MASTER2_CR
  }
};

#define R_I2C_PRER_H                              (i2c_address_table[i2c_id].i2c_prer_h)
#define R_I2C_PRER_L                              (i2c_address_table[i2c_id].i2c_prer_l)
#define R_I2C_TXR                                 (i2c_address_table[i2c_id].i2c_txr)
#define R_I2C_RXR                                 (i2c_address_table[i2c_id].i2c_rxr)
#define R_I2C_SR                                  (i2c_address_table[i2c_id].i2c_sr)
#define R_I2C_CTR                                 (i2c_address_table[i2c_id].i2c_ctr)
#define R_I2C_CR                                  (i2c_address_table[i2c_id].i2c_cr)

/*!
  APB clock
  */
//#define SYS_APB_CLOCK (mtos_cpu_freq_get() / 2)

/*!
  I2C register bit defination
  */

/*!
  I2C controller enable
  */
#define I2C_CTR_EN          0x80
/*!
  I2C interrupts enable
  */
#define I2C_CTR_IEN        0x40

/*!
  I2C cmd: Start Transmit
  */
#define I2C_CR_STA                0x80
/*!
  I2C cmd: Stop Transmit
  */
#define I2C_CR_STP                0x40
/*!
  I2C writing command
  */
#define I2C_CR_WR                 0x20
/*!
  I2C reading command
  */
#define I2C_CR_RD                 0x10
/*!
  I2C cmd: Intterrupt acknowledge
  */
#define I2C_CR_IACK               0x08
/*!
  I2C cmd: Not acknowledge to slave
  */
#define I2C_CR_NACK               0x04

/*!
  I2C status: not recieve ACK from slave
  */
#define I2C_SR_RXNACK              0x80
/*!
  I2C status: I2c bus arbitrage invalidation flag
  */
#define I2C_SR_AL                 0x20
/*!
  I2C status: Read or Write transmit running flag
  */
#define I2C_SR_TIP                0x10
/*!
  I2C status: Interrupts Request flag
  */
#define I2C_SR_IFLAG              0x08

/*!
  I2c Clock Frequency(KHz) Caption : PRER = F_sysclk / (5 * F_scl) - 1
  */
#define I2C_PRER_CAPS(x)  (((SYS_APB_CLOCK) / (5000 * (x))) - 1)

/*!
  define I2C delay time in us under polling mode
  */
#define I2C_TIME_OUT   20000

/*!
  define I2C default clock frequency in KHz
  */
#define I2C_DEF_CLK_KHZ 200

/*!
  define I2C delay function in us
  */
#define I2C_DELAY_US mtos_task_delay_us

static void i2c0_init(u8 i2c_id, u32 busclk_khz);

// The warriors there is no i2c master reset function
static void i2c0_master_reset(u8 i2c_id, u16 i2c_clock)
{
  u32 mask = 0;
  u32 ctrl = 0;
  u32 bit = 0;
  u32 rtmp = 0;
  switch(i2c_id)
  {
  case 0:
    mask = R_RST_GLOBAL0_MASK;
    ctrl = R_RST_GLOBAL0_CTRL;
    bit = 9;
    break;
  case 1:
    mask = R_RST_GLOBAL0_MASK;
    ctrl = R_RST_GLOBAL0_CTRL;
    bit = 10;
    break;
  case 2:
    mask = R_RST_GLOBAL1_MASK;
    ctrl = R_RST_GLOBAL1_CTRL;
    bit = 4;
    break;
  }
  rtmp = hal_get_u32((volatile unsigned long *) mask);
  rtmp &= ~(1 << bit);
  hal_put_u32((volatile unsigned long *) mask, rtmp);

  rtmp = hal_get_u32((volatile unsigned long *) ctrl);
  rtmp &= ~(1 << bit);
  hal_put_u32((volatile unsigned long *) ctrl, rtmp);
  
  I2C_DELAY_US(1000);
  
  rtmp = hal_get_u32((volatile unsigned long *) mask);
  rtmp |= (1 << bit);
  hal_put_u32((volatile unsigned long *) mask, rtmp);

  rtmp = hal_get_u32((volatile unsigned long *) ctrl);
  rtmp |= (1 << bit);
  hal_put_u32((volatile unsigned long *) ctrl, rtmp);
  
  if(i2c_clock != 0)
  {
    i2c0_init(i2c_id,i2c_clock);
  }
  else
  {
    i2c0_init(i2c_id, I2C_DEF_CLK_KHZ);
  }
  
  I2C_DELAY_US(1000);

}


static s32 i2c0_check_opdone_status(u8 i2c_id, u8 cmd)
{
  u8 data = 0;

  data = hal_get_u8((u8 *)R_I2C_SR);
  if(I2C_SR_AL == (data & I2C_SR_AL))
  {
    OS_PRINTF("i2c0_error: AL\n");
    return ERR_HARDWARE;
  }

  if(cmd == I2C_CR_WR)
  {
    /* check if receive ACK from slave */
    if(I2C_SR_RXNACK == (data & I2C_SR_RXNACK)) /* bit 9 */
    {
      //OS_PRINTF("i2c0_error: WR without rcv ACK\n");
      return ERR_FAILURE;
    }
  }

  return SUCCESS;
}


static s32 i2c0_stop(u8 i2c_id)
{
  volatile u32 retry_cnt = 0;
  s32 ret = 0;

  hal_put_u8((u8 *)R_I2C_CR, I2C_CR_STP);
  while(hal_get_u8((u8 *)R_I2C_CR) != 0x04) // default value
  {
    if(retry_cnt == I2C_TIME_OUT)
    {
      /* time out */
      //I2C_DBG_PRINTF("i2c0 stop time out\n");
      //I2C_DBG_PRINTF("i2c0_stop: retry_cnt: %d\n", retry_cnt);
      //I2C_DBG_PRINTF("CR<0x%x>\n", hal_get_u8((u8 *)R_I2C_CR));
      //I2C_DBG_PRINTF("SR<0x%x>\n", hal_get_u8((u8 *)R_I2C_SR));
      return ERR_HARDWARE;
    }
    retry_cnt++;
  }

  /* check result status */
  ret = i2c0_check_opdone_status(i2c_id, I2C_CR_STP);

  return ret;
}


static void i2c0_fatal_err_restore(u8 i2c_id, u16 i2c_clock)
{
  i2c0_master_reset(i2c_id, i2c_clock);
}


static s32 i2c0_wbyte(u8 i2c_id, u8 ucWdata, u8 bSetStart)
{
  u8 data = 0;
  volatile u32 retry_cnt = 0;
  s32 ret = 0;

  hal_put_u8((u8 *)R_I2C_TXR, ucWdata);
  if(1 == bSetStart)
  {
    data = I2C_CR_WR | I2C_CR_STA; // Write|Start command.
  }
  else
  {
    data = I2C_CR_WR;  // Write command.
  }

  /* start write */
  hal_put_u8((u8 *)R_I2C_CR, data);
  /* Wait write byte done. */
  while(hal_get_u8((u8 *)R_I2C_CR) != 0x04) // default value
  {
    if(retry_cnt == I2C_TIME_OUT)
    {
      /* time out */
      //I2C_DBG_PRINTF("i2c0_wbyte: retry_cnt: %d\n", retry_cnt);
      //I2C_DBG_PRINTF("i2c0 wbyte time out\n");
      //I2C_DBG_PRINTF("CR<0x%x>\n", hal_get_u8((u8 *)R_I2C_CR));
      //I2C_DBG_PRINTF("SR<0x%x>\n", hal_get_u8((u8 *)R_I2C_SR));
      return ERR_TIMEOUT;
    }
    I2C_DELAY_US(10);
    retry_cnt += 10;
  }

  /* check result status */
  ret = i2c0_check_opdone_status(i2c_id, I2C_CR_WR);

  return ret;
}


static s32 i2c0_rbyte(u8 i2c_id, u8 *p_value, s8 nack)
{
  u8 data = 0;
  volatile u32 retry_cnt = 0;
  s32 ret = 0;

  if(1 == nack)
  {
    data = I2C_CR_RD | I2C_CR_NACK;  // Read data without ACK.
  }
  else
  {
    data = I2C_CR_RD;  // Read data with ACK.
  }

  /* start read */
  hal_put_u8((u8 *)R_I2C_CR, data);
  /* Wait read byte done. */
  while(hal_get_u8((u8 *)R_I2C_CR) != 0x04) // default value
  {
    if(retry_cnt == I2C_TIME_OUT)
    {
      /* time out */
      //I2C_DBG_PRINTF("i2c0_rbyte: retry_cnt: %d\n", retry_cnt);
      //I2C_DBG_PRINTF("i2c0 rbyte time out\n");
      //I2C_DBG_PRINTF("CR<0x%x>\n", hal_get_u8((u8 *)R_I2C_CR));
      //I2C_DBG_PRINTF("SR<0x%x>\n", hal_get_u8((u8 *)R_I2C_SR));
      return ERR_TIMEOUT;
    }
    I2C_DELAY_US(10);
    retry_cnt += 10;
  }

  /* check result status */
  ret = i2c0_check_opdone_status(i2c_id, I2C_CR_RD);

  if(SUCCESS == ret)
  {
    /* get read data */
    *p_value = hal_get_u8((u8 *)R_I2C_RXR);
  }
  return ret;
}


static s32 i2c0_stop_retry(u8 i2c_id, u8 times)
{
  u8 retry_cnt = 0;
  s32 ret = 0;

  for(retry_cnt = 0; retry_cnt < times; retry_cnt++)
  {
    ret = i2c0_stop(i2c_id);
    if(ERR_HARDWARE == ret)
    {
      I2C_DBG_PRINTF("i2c0_stop HW error, master reset, retry\n");
      return ERR_HARDWARE;
    }
    else
    {
      break;
    }
  }

  if(retry_cnt == times)
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static void i2c0_init(u8 i2c_id, u32 busclk_khz)
{
  u8 data = 0;
  u32 tmp = 0;

  /* Set working clock */
  tmp = I2C_PRER_CAPS(busclk_khz);
  hal_put_u8((u8 *)R_I2C_PRER_L, (u8)tmp);
  hal_put_u8((u8 *)R_I2C_PRER_H, (u8)(tmp >> 8));

  /* i2c function enable and if use interrupt mode, enable i2c interrupt */
  data = I2C_CTR_EN;
  hal_put_u8((u8 *)R_I2C_CTR, data);
}


static s32 i2c0_read_no_stop(u8 i2c_id, u8 slv_addr, u8 *p_buf, u32 len)
{
  int i = 3;
  s32 ret = 0;
  
  slv_addr |= 1;            /* Read */
  while(i != 0)        /* Ack polling !! */
  {
    //OS_PRINTF("r id[0x%x]\n", slv_addr);
    ret = i2c0_wbyte(i2c_id, slv_addr, 1);
    if(SUCCESS == ret)
    {
      break;
    }
    if(ERR_HARDWARE == ret)
    {
      return ERR_HARDWARE;
    }
    /* device is busy, issue stop and chack again later */
    ret = i2c0_stop_retry(i2c_id, 3);
    if(SUCCESS != ret)
    {
      return ERR_HARDWARE;
    }
    I2C_DELAY_US(10);     /* wait for 10uS */
    i -= 1;
  }

  if(i == 0)
  {
    return ERR_TIMEOUT;
  }

  for(i = 0; i < (len - 1); i++)
  {
    /*with ack to continue reading */
    ret = i2c0_rbyte(i2c_id, &p_buf[i], 0);
    if(SUCCESS != ret)
    {
      return ERR_HARDWARE;
    }
  }
  /*with no ack to stop process */
  ret = i2c0_rbyte(i2c_id, &p_buf[i], 1);
  if(SUCCESS != ret)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}


static s32 i2c0_write_no_stop(u8 i2c_id, u8 slv_addr, u8 *p_buf, u32 len)
{
  int i = 3;
  s32 ret = 0;
  
  slv_addr &= 0xFE;         /*Write*/
  while(i != 0)              /* Ack polling !! */
  {
//   //OS_PRINTF("w id =%d [0x%x], t[%d]\n", i2c_id, slv_addr, i);
    ret = i2c0_wbyte(i2c_id, slv_addr, 1);
////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);
    if(SUCCESS == ret)
    {
      break;
    }
    if(ERR_HARDWARE == ret)
    {
      return ERR_HARDWARE;
    }
    /* device is busy, issue stop and chack again later */
    ret = i2c0_stop_retry(i2c_id, 3);
    //OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);
    if(SUCCESS != ret)
    {
      return ERR_HARDWARE;
    }
    I2C_DELAY_US(50);       /* wait for 10uS */
    i -= 1;
  }

  if(i == 0)
  {
    OS_PRINTF("i2c_write_no_stop ERR_TIMEOUT\n");
  
    return ERR_TIMEOUT;
  }

  /* write data */
  for(i = 0; i < len; i++)
  {
    //OS_PRINTF("w data[0x%x]\n", p_buf[i]);
    ret = i2c0_wbyte(i2c_id, p_buf[i], 0);
    //OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);
    if(SUCCESS != ret)
    {
      return ERR_HARDWARE;
    }
  }

//   //OS_PRINTF("i2c0_write_no_stop success i=%d\n",i);
  return SUCCESS;
}

static s32 i2c0_read(lld_i2c_t *p_lld, u8 slv_addr, u8 *p_buf, 
                                             u32 len)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  s32 ret = 0;
  
  for(times = 0; times < 3; times++)
  {
    /* read without stop */
    ret = i2c0_read_no_stop(p_priv->i2c_id, slv_addr, p_buf, len);
    if(SUCCESS != ret)
    {
      I2C_DBG_PRINTF("i2c0_read: read error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    /* stop */
    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      //I2C_DBG_PRINTF("i2c0_read: stop error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    break;
  }

  if(times == 3)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}

static s32 i2c0_write(lld_i2c_t *p_lld, u8 slv_addr, u8 *p_buf, 
                                             u32 len)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  s32 ret = 0;
    ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

  for(times = 0; times < 3; times++)
  {
    /* write without stop */
    ret = i2c0_write_no_stop(p_priv->i2c_id, slv_addr, p_buf, len);
    if(SUCCESS != ret)
    {
    ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);
      I2C_DBG_PRINTF("i2c0_write: write error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }
  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

    /* stop */
    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      //I2C_DBG_PRINTF("i2c0_write: stop error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }
  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

    break;
  }

  if(times == 3)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}

inline static void i2c_jazz_pinmux(u8 i2c_id)
{
  u32 dtmp = 0;

  dtmp = hal_get_u32((volatile unsigned long *)0x6f880000);
  if(i2c_id == 0)
  {
      dtmp &= ~(1 << 10);
      dtmp |= (1 << 21);
      hal_put_u32((volatile unsigned long *)0x6f880000, dtmp);
   }
   else if(i2c_id == 2)
  {
      dtmp |= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8);
      dtmp &= ~(1 << 9);
      hal_put_u32((volatile unsigned long *)0x6f880000,dtmp);
  }
}

static s32 i2c_jazz_read(lld_i2c_t *p_lld, u8 slv_addr, u8 *p_buf,
                                             u32 len, u32 param)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  s32 ret = 0;

  i2c_jazz_pinmux(p_priv->i2c_id);
  for(times = 0; times < 3; times++)
  {
    /* read without stop */
    ret = i2c0_read_no_stop(p_priv->i2c_id, slv_addr, p_buf, len);
    if(SUCCESS != ret)
    {
      I2C_DBG_PRINTF("i2c0_read: read error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    /* stop */
    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      //I2C_DBG_PRINTF("i2c0_read: stop error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    break;
  }

  if(times == 3)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}


s32 i2c_jazz_write(lld_i2c_t *p_lld, u8 slv_addr, u8 *p_buf,
                                             u32 len, u32 param)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  s32 ret = 0;
  i2c_jazz_pinmux(p_priv->i2c_id);
  //////OS_PRINTF("slv[0x%x]\n", slv_addr);
  for(times = 0; times < 3; times++)
  {
    /* write without stop */
    ret = i2c0_write_no_stop(p_priv->i2c_id, slv_addr, p_buf, len);
    if(SUCCESS != ret)
    {
      I2C_DBG_PRINTF("i2c0_write: write error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    /* stop */
    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      //I2C_DBG_PRINTF("i2c0_write: stop error\n");
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    break;
  }

  if(times == 3)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}


/*!
   Implement i2c sequential read function.
   start-slave-data(w)-...-start-slave-data(r)-...-stop
  */
static RET_CODE i2c_jazz_seq_read(lld_i2c_t *p_lld,
                                       u8 slv_addr,
                                       u8 *p_buf,
                                       u32 wlen,
                                       u32 rlen,
                                       u32 param)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  
  i2c_jazz_pinmux(p_priv->i2c_id);
  if(wlen == 0)
  {
    return i2c0_read(p_lld, slv_addr, p_buf, rlen);
  }

  for(times = 0; times < 3; times++)
  {
    if(SUCCESS != i2c0_write_no_stop(p_priv->i2c_id, slv_addr, p_buf, wlen))
    {
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    if(SUCCESS != i2c0_read_no_stop(p_priv->i2c_id, slv_addr, p_buf, rlen))
    {
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    break;
  }

  if(times == 3)
  {
    return ERR_HARDWARE;
  }

  return SUCCESS;
}


/*!
   Receive from appointed slave with sending firstly, and with stop operation after sending.
   start-slave-data(w)-...-stop-start-slave-data(r)-...-stop
  */
static RET_CODE i2c_jazz_std_read(lld_i2c_t *p_lld,
                                       u8 slv_addr,
                                       u8 *p_buf,
                                       u32 wlen,
                                       u32 rlen,
                                       u32 param)
{

  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  u8 times = 0;
  
  i2c_jazz_pinmux(p_priv->i2c_id);
  if(wlen == 0)
  {
    return i2c0_read(p_lld, slv_addr, p_buf, rlen);
  }
    ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

  for(times = 0; times < 3; times++)
  {
    if(SUCCESS != i2c0_write(p_lld, slv_addr, p_buf, wlen))
    {
      ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

      return ERR_HARDWARE;
    }
  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

    if(SUCCESS != i2c0_read_no_stop(p_priv->i2c_id, slv_addr, p_buf, rlen))
    {
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);


    if(SUCCESS != i2c0_stop_retry(p_priv->i2c_id, 3))
    {
      i2c0_fatal_err_restore(p_priv->i2c_id, p_priv->i2c_clk);
      continue;
    }

    break;
  }
  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);

  if(times == 3)
  {
    return ERR_HARDWARE;
  }
  ////OS_PRINTF(" i2c_jazz_std_read  1111 %d %s\n",__LINE__, __FILE__);
  // Todo: fix me! control reaches end of non-void function
  return SUCCESS;
}


static RET_CODE i2c_jazz_open(lld_i2c_t *p_lld, i2c_cfg_t *p_cfg)
{
  i2c_ctrller_jazz_priv_t *p_priv = NULL;
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  ////OS_PRINTF("In i2c_wactrller_open\n");
  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;
  
  if(p_lld->p_priv != NULL)
  {
    mtos_free(p_lld->p_priv);
    p_lld->p_priv = NULL;
  }
  p_priv = p_lld->p_priv = mtos_malloc(sizeof(i2c_ctrller_jazz_priv_t));
  if(NULL == p_cfg)
  {
    p_priv->i2c_clk = I2C_DEF_CLK_KHZ;
    p_priv->i2c_id = 0;
    p_base->lock_mode = OS_MUTEX_LOCK;
  }
  else
  {
    p_priv->i2c_clk = p_cfg->bus_clk_khz;
    p_priv->i2c_id = p_cfg->i2c_id;
    p_base->lock_mode = p_cfg->lock_mode;
  }

  i2c0_init(p_priv->i2c_id, p_priv->i2c_clk);
  return SUCCESS;
}


static RET_CODE i2c_jazz_close(lld_i2c_t *p_lld)
{
  return SUCCESS;
}


static void i2c_jazz_detach(lld_i2c_t *p_lld)
{
}


static RET_CODE i2c_jazz_ioctrl(lld_i2c_t *p_lld, u32 cmd, u32 param)
{
  i2c_ctrller_jazz_priv_t *p_priv = (i2c_ctrller_jazz_priv_t *)p_lld->p_priv;
  switch(cmd)
  {
    case I2C_IOCTRL_SET_CLOCK:
      i2c0_init(p_priv->i2c_id, param);
      break;

    case I2C_IOCTRL_STOP:
      i2c0_stop(p_priv->i2c_id);
      break;
      
    default:  
      break;
  }
  
  return SUCCESS;
}


RET_CODE i2c_jazz_attach(char *p_name)
{
  i2c_bus_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  lld_i2c_t *p_lld = NULL;
  
  if((p_dev = dev_allocate(p_name, SYS_BUS_TYPE_I2C,
                           sizeof(i2c_bus_t),
                           sizeof(lld_i2c_t))) == NULL)
  {
    return ERR_FAILURE;
  }

  p_base = (device_base_t *)(p_dev->p_base);
  p_base->open = (RET_CODE (*)(void *, void *))i2c_jazz_open;
  p_base->close = (RET_CODE (*)(void *))i2c_jazz_close;
  p_base->detach = (void (*)(void *))i2c_jazz_detach;
  p_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))i2c_jazz_ioctrl;

  p_lld = (lld_i2c_t *)p_dev->p_priv;
  p_lld->write = i2c_jazz_write;
  p_lld->read = i2c_jazz_read;
  p_lld->seq_read = i2c_jazz_seq_read;
  p_lld->std_read = i2c_jazz_std_read;

  return SUCCESS;
}
