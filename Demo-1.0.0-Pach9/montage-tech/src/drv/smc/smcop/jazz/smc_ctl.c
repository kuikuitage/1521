/******************************************************************************/
/******************************************************************************/
#ifdef DRV_SEL_SMC

#include <string.h>

#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"


#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_fifo.h"
#include "hal_base.h"
#include "hal_irq_jazz.h"
#include "sys_regs_jazz.h"

#include "smc_ctl.h"

#define SCCTL_DEBUG     0

#if SCCTL_DEBUG
 #define SCCTL_P(...)    OS_PRINTF(__VA_ARGS__)
#else
 #define SCCTL_P(...)    do{}while(0)
#endif


/************************************************************************/
static void smc_interrupt_set(void (*int_sc_handle)(void))
{
  if (int_sc_handle != NULL)
  {
    mtos_irq_request(IRQ_JAZZ_SMC_ID, int_sc_handle, IRQ_INT_TYPE_NORMAL);
  }
  else
  {
    mtos_irq_release(IRQ_JAZZ_SMC_ID,NULL);
  }
}

/************************************************************************
 * sc set ETU
 ************************************************************************/
static void smc_etu_set(u32 ETU)
{
  u8 data = 0;
  u8 etu1 = (u8)((ETU&0x030000) >> 16);
  u8 etu2 = (u8)((ETU&0xFF00) >> 8);
  u8 etu3 = (u8)((ETU&0xFF));

  data = inb (REG_WR_SMC_ETU1);
  data &= ~(3);
  data |= etu1;
  outb (data, REG_WR_SMC_ETU1);

  outb (etu2, REG_WR_SMC_ETU2);
  outb (etu3, REG_WR_SMC_ETU3);

}
/**************** 1518 unpdate smc ctrl part *************************/
/*
static void smc_vcc_set(u8 mode)
{
  DRV_GPIOModSet(6, mode);
}
*/

/**************** 1518 unpdate smc ctrl part *************************/
#if 0
static void smc_cvt_set(u8 mode, u8 value)
{
  u8 data = inb (REG_WR_SMC_ETU1);

  if (1 == mode)/*control by software*/
  {
    data |= 0x80;
    if (1 == value)/*positive*/
    {
      data |= 0x40;
    }
    else           /*negative*/
    {
      data &= 0xbf;
    }
  }
  else          /*check by hardware*/
  {
    data &= 0x7f;
  }

  outb (data, REG_WR_SMC_ETU1);
}
#endif

static void smc_conf_set_freqdiv(u8 freq_div)
{
  u8 data = inb (REG_WR_SMC_CONFIG);

  data &= ~(0x7E);
  data |= (freq_div << 1);

  outb(data, REG_WR_SMC_CONFIG);
}


static void smc_conf_set_socket(u8 socket_type)
{
  u8 data = inb (REG_WR_SMC_CONFIG);

  if (socket_type == SMC_SOCKET_CLOSED)
  {
    data &= 0xFE;
  }
  else
  {
    data |= 0x01;
  }

  outb(data, REG_WR_SMC_CONFIG);
}


static void smc_nc_set(u8 protect_time)
{
  outb (protect_time, REG_WR_SMC_NC);
}

static void smc_ctrl_set (smc_bitstop_t stop,
        u8 ctrl_nset_en,
        u8 ctrl_check_en,
        u8 ctrl_parity_en,
        u8 ctrl_ieinsert_en,
        u8 ctrl_ieremove_en,
        u8 ctrl_ierx_en,
        u8 ctrl_iecmd_en)
{
  u8 data = 0;

  data |= (stop << 7);
  data |= (ctrl_nset_en << 6);
  data |= (ctrl_check_en << 5);
  data |= (ctrl_parity_en << 4);
  data |= (ctrl_ieinsert_en << 3);
  data |= (ctrl_ieremove_en << 2);
  data |= (ctrl_ierx_en << 1);
  data |= (ctrl_iecmd_en);

  outb (data, REG_WR_SMC_CTRL);
}


static void smc_ctrl_set_stop(smc_bitstop_t stop)
{
  u8 data = inb (REG_WR_SMC_CTRL);

  if (stop == SMC_STOP_TWO)
  {
    data &= (~(1 << 7));
  }
  else
  {
    data |= (1 << 7);
  }

  outb (data, REG_WR_SMC_CTRL);
}


static void smc_ctrl_set_check(u8 ctrl_check_en)
{
  u8 data = inb (REG_WR_SMC_CTRL);

  if (ctrl_check_en == SMC_DISABLE)
  {
    data &= (~(1 << 5));
  }
  else
  {
    data |= (1 << 5);
  }

  outb (data, REG_WR_SMC_CTRL);
}


static void smc_ctrl_set_parity(u8 ctrl_parity_en)
{
  u8 data = inb (REG_WR_SMC_CTRL);

  if (ctrl_parity_en == SMC_DISABLE)
  {
    data &= (~(1 << 4));
  }
  else
  {
    data |= (1 << 4);
  }

  outb (data, REG_WR_SMC_CTRL);
}

#if 0
static void smc_rstctrl_set_powerctrl(u8 power_en)
{
  u8 data;

  data = inb(REG_WR_SMC_RSTCTRL);

  if (power_en == SMC_ENABLE)
  {
    data &= 0xFE;
  }
  else if (power_en == SMC_DISABLE)
  {
    data |= 0x01;
  }

  outb (data, REG_WR_SMC_RSTCTRL);
}
#endif

#define GPIO_MULTI_PIN    (*(volatile unsigned int *)0x6f880000)

static void set_vcc_smc(void)
{
  unsigned int pin_in = GPIO_MULTI_PIN;
  pin_in &=  ~(1 << 25);
  pin_in &=  ~(1 << 26);
  pin_in &=  ~(1 << 27);

}


#if 0
static void enable_smc()
{
  REG_WR_SMC_RSTCTRL = 0x38;
}
#endif

static void smc_delay_us(int t)
{
  int i = 0;
  int cnt = t * 8;

  for(i = 0;i < cnt;i++)
  {
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  }

}

/***********************************************************************
 * Initialize SC Device Block
 **********************************************************************/
void smc_init(void (*smc_interrupt_handle)(void),int detect_pin_pol, int vcc_enable_pol)
{
  smc_bitstop_t stop  = SMC_STOP_TWO;
  u8 ctrl_nset_en = SMC_DISABLE;
  u8 ctrl_check_en = SMC_ENABLE;
  u8 ctrl_parity_en = SMC_ENABLE;
  u8 ctrl_ieinsert_en = SMC_ENABLE;
  u8 ctrl_ieremove_en = SMC_ENABLE;
  u8 ctrl_ierx_en = SMC_ENABLE;
  u8 ctrl_iecmd_en = SMC_DISABLE;

SCCTL_P("smc_init()...\n");

  //smc_vcc_set(SMC_VCC_INTERNAL);
  set_vcc_smc();

 if(vcc_enable_pol)
 (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<3);
 else
 (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<3);

  smc_conf_set_socket((detect_pin_pol == 1) ? SMC_SOCKET_CLOSED : SMC_SOCKET_OPEN);

 (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<2);

 (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<5);

  smc_conf_set_freqdiv(SMC_FREQUENCY_DIV_DEFAULT);

  smc_nc_set (0);
  smc_ctrl_set (stop,
        ctrl_nset_en,
        ctrl_check_en,
        ctrl_parity_en,
        ctrl_ieinsert_en,
        ctrl_ieremove_en,
        ctrl_ierx_en,
        ctrl_iecmd_en);

  smc_interrupt_set(smc_interrupt_handle);

  smc_set_etu(SMC_ETU_DEFAULT);

  /* SC Insert/Remove status Change */
  if (SMC_GET_STATUS())
  {
      smc_delay_us(1000); //400us
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<0);
    smc_delay_us(120); //50us
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<5);
    smc_delay_us(500); //200us
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<2);

      SCCTL_P("SmartCard STATUS_INSERT!!!\n");

  }
  else
  {
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<0);
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<2);
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<5);

      SCCTL_P("SmartCard STATUS_REMOVE!!!\n");

  }
  //smc_rstctrl_set_powerctrl(SMC_ENABLE);
  //enable_smc();

}

/************************************************************************
 * SMC Soft Reset
 * param:  delays
 * return: <0: FALSE 0:TRUE
 ************************************************************************/
void smc_reset(u16 delays)
{
    u8 data = 0;

    data = inb(REG_WR_SMC_RSTCTRL);
    data |= (1 << 2);
    outb(data, REG_WR_SMC_RSTCTRL);

    mtos_task_sleep(delays);            //Delay Time is delaysms

    data = inb(REG_WR_SMC_RSTCTRL);
    data &=(~(1 << 2));
    outb(data, REG_WR_SMC_RSTCTRL);
}

/************************************************************************
 * SMC FIFO Reset
 * param:  void
 * return: void
 ************************************************************************/
void smc_flush(void)
{
  u8 data = 0;

  data = inb(REG_WR_SMC_RSTCTRL);
  data |= (1 << 1);
  outb (data, REG_WR_SMC_RSTCTRL);

  data &= (~(1 << 1));
  outb (data, REG_WR_SMC_RSTCTRL);
}

/************************************************************************
 * sc set bit coding
 * T14: no check, no parity
 * othes: check and parity
 ************************************************************************/
void  smc_set_bitcoding(BOOL bT14)
{
  if (bT14)
  {
    smc_conf_set_freqdiv(SMC_FREQUENCY_DIV_T14);
    smc_ctrl_set_stop(SMC_STOP_ONE);
    smc_ctrl_set_check(SMC_DISABLE);
    smc_ctrl_set_parity(SMC_DISABLE);
  }
  else
  {
    smc_conf_set_freqdiv(SMC_FREQUENCY_DIV_DEFAULT);
    smc_ctrl_set_stop(SMC_STOP_TWO);
    smc_ctrl_set_check(SMC_ENABLE);
    smc_ctrl_set_parity(SMC_ENABLE);
  }
}

void smc_set_etu(u32 ETU)
{
  smc_etu_set(ETU);
}

#endif //#ifdef DRV_SEL_SMC


