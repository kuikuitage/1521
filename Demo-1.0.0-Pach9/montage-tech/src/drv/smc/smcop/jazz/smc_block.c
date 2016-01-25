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

#include  "trunk/error_code.h"

#include "smc_ctl.h"
#include "smc_block.h"
#include "drv_smc.h"
#include "smc_t0.h"
#include "smc_t1.h"


#define SCBLOCK_DEBUG     0

#if SCBLOCK_DEBUG
 #define SCBLOCK_P(...)    OS_PRINTF(__VA_ARGS__)
#else
 #define SCBLOCK_P(...)    do{}while(0)
#endif


/****************************** SmartCard Device Block */
static  tscdevice g_SCDev = {0};


static void _Calculate_ETU(void)
{
    u16 InitialETU, WorkETU;
    u32 MaxClock;
    u32 WWT;

    /* Calculate work waiting time in work etus */
    WWT = 960 * g_SCDev.WInt;

    /* Next translate our rate conversion factor Interger into F.
     * Used conversion table in IS0 7816-3 for values, coded this way to
     * make it easier to read.
     */
    switch(g_SCDev.FInt)
    {
        case 0: /* Internal clock, treat differently */
            InitialETU = 372;
            MaxClock = 4*MHZ;
            break;
        case 1:
            InitialETU = 372;
            MaxClock = 5*MHZ;
            break;
        case 2:
            InitialETU = 558;
            MaxClock = 6*MHZ;
            break;
        case 3:
            InitialETU = 744;
            MaxClock = 8*MHZ;
            break;
        case 4:
            InitialETU = 1116;
            MaxClock = 12*MHZ;
            break;
        case 5:
            InitialETU = 1488;
            MaxClock = 16*MHZ;
            break;
        case 6:
            InitialETU = 1860;
            MaxClock = 20*MHZ;
            break;
        case 9:
            InitialETU = 512;
            MaxClock = 5*MHZ;
            break;
        case 10:
            InitialETU = 768;
            MaxClock = 7500000;
            break;
        case 11:
            InitialETU = 1024;
            MaxClock = 10*MHZ;
            break;
        case 12:
            InitialETU = 1536;
            MaxClock = 15*MHZ;
            break;
        case 13:
            InitialETU = 2048;
            MaxClock = 20*MHZ;
            break;
        default:
            InitialETU = 372;
            MaxClock = 0;
            break;
    }

    /* Now adjust using the Bit rate adjustment factor D.
     * This is also taken from ISO7816-3 and coded this way
     * to make it easier to read.
     */
    switch(g_SCDev.DInt)
    {
        case 1:
            WorkETU = InitialETU;
            break;
        case 2:
            WorkETU = InitialETU / 2;
            WWT = WWT * 2;
            break;
        case 3:
            WorkETU = InitialETU / 4;
            WWT = WWT * 4;
            break;
        case 4:
            WorkETU = InitialETU / 8;
            WWT = WWT * 8;
            break;
        case 5:
            WorkETU = InitialETU / 16;
            WWT = WWT * 16;
            break;
        case 6:
            WorkETU = InitialETU / 32;
            WWT = WWT * 32;
            break;
        case 8:
            WorkETU = InitialETU / 12;
            WWT = WWT * 12;
            break;
        case 9:
            WorkETU = InitialETU / 20;
            WWT = WWT * 20;
            break;
        case 10:
            WorkETU = InitialETU * 2;
            WWT = WWT / 2;
            break;
        case 11:
            WorkETU = InitialETU * 4;
            WWT = WWT / 4;
            break;
        case 12:
            WorkETU = InitialETU * 8;
            WWT = WWT / 8;
            break;
        case 13:
            WorkETU = InitialETU * 16;
            WWT = WWT / 16;
            break;
        case 14:
            WorkETU = InitialETU * 32;
            WWT = WWT / 32;
            break;
        case 15:
            WorkETU = InitialETU * 64;
            WWT = WWT / 64;
            break;
        default:
            WorkETU = InitialETU;
            break;
    }

    /* Now set the ETU */
  g_SCDev.WorkETU = WorkETU;
  smc_set_etu(g_SCDev.WorkETU);

    /* Set maximum clock frequency */
    g_SCDev.Fmax = MaxClock;

    /* Set subsequent clock frequency */
    g_SCDev.Fs = g_SCDev.Fi;

    /* Set new WWT */
    g_SCDev.WWT = WWT;


}


/************************************************************************
 * sc set bit coding
 * T14: no check, no parity
 * othes: check and parity
 ************************************************************************/

static void _Init_AnswerToReset(BOOL bT14)
{

  //mtos_task_sleep(100);

  sc_flush();

  /* T=1 defaults */
  g_SCDev.T1Details.TxMaxInfoSize = 32;
  g_SCDev.T1Details.OurSequence = 0;
  g_SCDev.T1Details.TheirSequence = 1;
  g_SCDev.CWInt = 13;
  g_SCDev.BWInt = 4;


  if (bT14)
  {
    g_SCDev.WorkETU = SMC_ETU_T14;
  }
  else
  {
    g_SCDev.WorkETU = SMC_ETU_DEFAULT;
  }


  smc_set_etu(g_SCDev.WorkETU);

  smc_reset(1);

}



/////////////////////////////////////////////////////////////////////////
/************************************************************************
 * SMC Interrupt Handler, Get the Card Status and the Receive Data
 ************************************************************************/

static void SC_Interrupt_Handle(void)
{
  tsmcrxio *pRX;

  /* SC Insert/Remove status Change */
  if (SMC_GET_STATUS())
  {
    if (g_SCDev.Status != STATUS_INSERT)
    {
      g_SCDev.Status = STATUS_INSERT;
      if (g_SCDev.SmartInsert)
      {
        g_SCDev.SmartInsert();
      }

     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<0);
    smc_delay_us(120); //50us
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<5);
    smc_delay_us(500); //200us
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<2);

//      SCBLOCK_P("SmartCard Embeded!!!\n");
    }
  }
  else
  {
    if (g_SCDev.Status != STATUS_REMOVE)
    {
      g_SCDev.Status = STATUS_REMOVE;
      if (g_SCDev.SmartRemove)
      {
        g_SCDev.SmartRemove();
      }

     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<0);
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) |= (1<<2);
     (*(volatile unsigned char *)REG_WR_SMC_RSTCTRL) &= ~(1<<5);

//      SCBLOCK_P("SmartCard Removed!!!\n");
    }
  }

  /* Read Data */
  pRX = &g_SCDev.rxfifo;
  while (SMC_RX_NOTEMPTY())
  {
      pRX->rx[pRX->end] = SMC_RX_GET();
      pRX->end = (pRX->end + 1) % SMC_MAX_IOFIFO;
      pRX->num++;
  }
}


/************************ Functions Exports ****************************/

/************************************************************************
 * sc : flush the sc hard fifo and soft fifo
 ************************************************************************/
void sc_flush(void)
{
  u32 sr = 0;

  smc_flush();

    mtos_critical_enter(&sr);

  g_SCDev.rxfifo.num   = 0;
  g_SCDev.rxfifo.start   = 0;
  g_SCDev.rxfifo.end   = 0;

  mtos_critical_exit(sr);

}


/**********************************************************************
 * get n bytes fron smartcard controller (by scint)
 **********************************************************************/


static BOOL _rxfifo_get_n_bytes(u8 *pBuf, u32 N, u32 *pN)
{
  tsmcrxio *pRX = &g_SCDev.rxfifo;
  u32 i;

  /*  read N > Fifo Len */
  if (N > pRX->num)
  {
    return FALSE;
  }

  for (i = 0; i < N; i++)
  {
    pBuf[i] = pRX->rx[pRX->start];
    pRX->start = (pRX->start + 1) % SMC_MAX_IOFIFO;
  }
  if (pN != NULL)
  {
    *pN = N;
  }
  pRX->num -= i;

  return TRUE;
}


static BOOL _rxfifo_get_all_bytes(u8 *pBuf, u32 N, u32 *pN)
{
  tsmcrxio *pRX = &g_SCDev.rxfifo;
  u32 i;

  for (i = 0; (i < pRX->num) && (i < N); i++)
  {
    pBuf[i] = pRX->rx[pRX->start];
    pRX->start = (pRX->start + 1) % SMC_MAX_IOFIFO;
  }

  if (pN != NULL)
  {
    *pN = i;
  }

  pRX->num = 0;
  pRX->start = 0;
  pRX->end = 0;

  return TRUE;
}

ERRORCODE_T sc_rx(u8 *pBuf, u32 nNumToRead, u32 * pNumRead)
{
  u32 tryTimes;
  u32 sr = 0;
  u8 bRxOK;

  if (pNumRead != NULL)
  {
    *pNumRead = 0;
  }

  tryTimes = 0;
    do
    {
    if (!SMC_GET_STATUS())
    {
      return ERR_SMC_OUT;
    }

      mtos_critical_enter(&sr);

    bRxOK = _rxfifo_get_n_bytes(pBuf, nNumToRead, pNumRead);

     mtos_critical_exit(sr);

    if (bRxOK)
    {
      return ERR_SMC_NO_ERR;
    }

        tryTimes ++;
        //mtos_task_sleep(10);
        mtos_task_delay_us(1000);

    }while (tryTimes < 2000);

    mtos_critical_enter(&sr);

  _rxfifo_get_all_bytes(pBuf, nNumToRead, pNumRead);

  mtos_critical_exit(sr);

  return ERR_SMC_RX_TIMEOUT;
}

ERRORCODE_T sc_rx_intimeout(u8 *pBuf, u32 nNumToRead, u32 *pNumRead, u32 Timeout)
{
  u32 tryTimes;
  u32 sr = 0;
  BOOL bRxOK;

  if (pNumRead != NULL)
  {
    *pNumRead = 0;
  }

  tryTimes = 0;
  do
  {
    if (!SMC_GET_STATUS())
    {
      return ERR_SMC_OUT;
    }

      mtos_critical_enter(&sr);

    bRxOK = _rxfifo_get_n_bytes(pBuf, nNumToRead, pNumRead);

    mtos_critical_exit(sr);

    if (bRxOK)
    {
      return ERR_SMC_NO_ERR;
    }

    tryTimes ++;
    mtos_task_sleep(10);

    }while (tryTimes < (Timeout / 10));

    mtos_critical_enter(&sr);

  _rxfifo_get_all_bytes(pBuf, nNumToRead, pNumRead);

  mtos_critical_exit(sr);

  return ERR_SMC_RX_TIMEOUT;
}


/************************************************************************
 * SMC Tx Data API
 ************************************************************************/
ERRORCODE_T sc_tx(u8 *pBuf, U16 nNumToWrite)
{
  u16 i;
  u16 rem_num = 0;

  if ((pBuf == NULL) || (nNumToWrite == 0))
  {
      return ERR_SMC_PARAM_INVALID;
  }

  sc_flush();

  SMC_TX_CMDLEN(nNumToWrite);

  if (nNumToWrite > 255)
  {
    rem_num = nNumToWrite - 255;

    for (i = 0; i < 255; i++)
    {
      SMC_TX_CMD(pBuf[i]);
    }

    for (i = 0; i < rem_num; i++)
    {
      while(SMC_FIFO_CNT() >= 255);
      SMC_TX_CMD(pBuf[255 + i]);
    }
  }
  else
  {
    for (i = 0; i < nNumToWrite; i++)
    {
      SMC_TX_CMD(pBuf[i]);
    }
  }

  return ERR_SMC_NO_ERR;
}

/************************************************************************
 * Get SMC Working Protocol
 *
 * param:  OUT Protocol
 *
 * return: Only Support T0, T1, T14
 ************************************************************************/
void sc_getworkingprotocol(u8 * pProtocol)
{

  SCBLOCK_P("\r\nSP=%02x, WP=%d", g_SCDev.SupportedProtocolTypes, g_SCDev.WorkingType);

  switch (g_SCDev.WorkingType)
  {
    case SC_T0_PROTOCOL:
    case SC_T1_PROTOCOL:
    case SC_T14_PROTOCOL:
      *pProtocol = g_SCDev.WorkingType;
    break;
    default:
      *pProtocol = SC_UNSUPPORT_PTOTOCOL;
    break;
  }
}


/***********************************************************************
 * Check the SC Device Block Pointer is Valid
 **********************************************************************/
BOOL sc_checkhandle(tscdevice * pSCDev)
{
  if (pSCDev != &g_SCDev)
  {
    return FALSE;
  }

  return TRUE;
}

/***********************************************************************
 * Get SC Device Block Pointer
 **********************************************************************/
tscdevice * sc_getscdevice(void)
{
  return &g_SCDev;
}


/***********************************************************************
 * Initialize SC Device Block
 **********************************************************************/
ERRORCODE_T sc_intialize(int detect_pin_pol, int vcc_enable_pol)
{
  g_SCDev.Status = STATUS_UNKNOWN;

  smc_init(SC_Interrupt_Handle,detect_pin_pol,vcc_enable_pol);

  return ERR_SMC_NO_ERR;
}

/***********************************************************************
 * Get SC ATR
 **********************************************************************/

static ERRORCODE_T SC_ReadATR(u8 *pBuf, u32 nNumToRead, u32 ATRPos)
{
  /* 检查读取越界 */
  if ((ATRPos + nNumToRead) > SMC_MAX_ATR)
  {
    return ERR_SMC_ATRDATA_PARSE_ERR;
  }

  return sc_rx(pBuf, nNumToRead, NULL);
}

ERRORCODE_T sc_answertoreset(u8 *pReadBuffer, u32 *panswer_len, BOOL bT14)
{
  u8 BitmaskYx = 0;
  u8 Historicalk = 0,Tck;
  u8 InterfaceX = 1;
  u8 TAx,TBx,TCx,TDx;
  u8 modeT = 0,FirstT = 0;
  u32 i;
  ERRORCODE_T err;
  u8 errflg;
  u8 *answer;
  BOOL MinGuardTime = FALSE;
  BOOL TA1Present = FALSE;

  g_SCDev.HistoricalSize = 0;
  g_SCDev.ATRMsgSize = 0;

  memset(&g_SCDev.ATRBytes, 0, SMC_MAX_ATR);
  answer = g_SCDev.ATRBytes;
  g_SCDev.SupportedProtocolTypes = 0;
  g_SCDev.SpecificMode = FALSE;

  smc_set_bitcoding(bT14);

   _Init_AnswerToReset(bT14);

  err = SC_ReadATR(answer, SMC_MIN_ATR, 0);
  if (err != ERR_SMC_NO_ERR)
  {
      SCBLOCK_P("\r\n Get TS,T0 err");
      return ERR_SMC_GETATR_MIN_FAIL;
  }

  i = 0;
  g_SCDev.TS = answer[i++];

  if ((g_SCDev.TS != ATR_INVERSE_CONVENTION) && (g_SCDev.TS != ATR_DIRECT_CONVENTION))
  {
      /*
       * TS is err
       */
      SCBLOCK_P("\r\n TS Err:%02x", g_SCDev.TS);
      return ERR_SMC_GETATR_MIN_FAIL;
  }

    BitmaskYx = answer[i++];

    /******************************************************************
     *              ___________________________________
     *        T0-> |_____|_____|_____|_____|__|__|__|__|
     *                |     |     |     |
     *               TD1   TC1   TB1   TA1      TK
     ******************************************************************/

    Historicalk = BitmaskYx & 0x0f;

    errflg = 0;

    while (BitmaskYx != 0)
    {
        if (BitmaskYx & ATR_TAX_MASK)
        {
            /*
             * Get TAx
             */
            err = SC_ReadATR(&answer[i], 1, i);
            if (err != ERR_SMC_NO_ERR)
            {
                SCBLOCK_P("\r\n Get TA%d fail", InterfaceX);
                errflg = 51;
                break;
            }

            TAx = answer[i++];

            if (InterfaceX == 1)
            {
                /******************************************************************
                 *              ____________________
                 *              | bit7-4  | bit3-0 |
                 *        TA1-> |---------+--------|
                 *                  |         |
                 *                 FI         DI
                 ******************************************************************/
                g_SCDev.FInt = (TAx & 0xf0) >> 4;
                g_SCDev.DInt = (TAx & 0x0f);
                TA1Present = TRUE;

            }
            else if (InterfaceX == 2)
            {
                /*
                 * TA2
                 */
                g_SCDev.SpecificMode = TRUE;
                g_SCDev.SpecificType = TAx&0x0f;
                g_SCDev.SpecificTypeChangable = (TAx & 0x80) ? TRUE : FALSE;
            }
            else if ((InterfaceX > 2) && (modeT == 1))
            {
              g_SCDev.IFSC = TAx;
              g_SCDev.T1Details.TxMaxInfoSize = g_SCDev.IFSC;
            }
        }

        if (BitmaskYx & ATR_TBX_MASK)
        {
            /*
             * Get TBx byte
             */
            err = SC_ReadATR(&answer[i], 1, i);
            if (err != ERR_SMC_NO_ERR)
            {
                SCBLOCK_P("\r\n Get TB%d fail", InterfaceX);
                errflg = 52;
                break;
            }

            TBx = answer[i++];

            if (InterfaceX == 1)
            {
                /******************************************************************
                 *              ____________________
                 *              | bit6-5  | bit4-0 |
                 *        TB1-> |---------+--------|
                 *                  |         |
                 *                 II         PI1
                 ******************************************************************/
                g_SCDev.IInt  = (TBx & 0x60) >> 5;
                g_SCDev.PInt1 = (TBx & 0x1f);
            }
            else if (InterfaceX == 2)
            {
                /******************************************************************
                 *              ____________________
                 *              | bit7  --       0 |
                 *        TB2-> |---------+--------|
                 *                        |
                 *                        PI2
                 ******************************************************************/
                g_SCDev.PInt2 = TBx;
            }
            else if ((InterfaceX > 2) && (modeT == 1))
            {
                g_SCDev.CWInt = TBx & 0x0f;
                g_SCDev.BWInt = TBx >> 4;
            }
        }

        if (BitmaskYx & ATR_TCX_MASK)
        {
            /*
             * Get TCx byte
             */
            err = SC_ReadATR(&answer[i], 1, i);
            if (err != ERR_SMC_NO_ERR)
            {
                SCBLOCK_P("\r\n Get TC%d fail", InterfaceX);
                errflg = 53;
                break;
            }

            TCx = answer[i++];
            if (InterfaceX == 1)
            {
                /******************************************************************
                 *              ____________________
                 *              | bit7  --       0 |
                 *        TC1-> |---------+--------|
                 *                        |
                 *                        N
                 ******************************************************************/
                if (TCx == 0xff)
                {
                  MinGuardTime = TRUE;
                    g_SCDev.N = SMART_N_DEFAULT;
                }
                else
                {
                    g_SCDev.N = TCx;
                }
            }
            else if (InterfaceX == 2)
            {
               /*
                * TC2
                */
                g_SCDev.WInt = TCx;
            }
            else if ((InterfaceX > 2) && (modeT == 1))
            {
                /* bit0==1 use CRC, ==0 use LRC */
                g_SCDev.RC = TCx & 0x01;
                SCBLOCK_P("\r\nT1 RC = %d", g_SCDev.RC);
            }
        }

        if (BitmaskYx & ATR_TDX_MASK)
        {
            /*
             * Get TDx byte
             */

            err = SC_ReadATR(&answer[i], 1, i);
            if (err != ERR_SMC_NO_ERR)
            {
                SCBLOCK_P("\r\n Get TD%d fail", InterfaceX);
                errflg = 54;
                break;
            }

            TDx   = answer[i++];
            /******************************************************************
             *              ___________________________________
             *       TDi-> |_____|_____|_____|_____|__|__|__|__|
             *                |     |     |     |
             *               TDi+1 TCi+1 TBi+1 TAi+1    modeT
             ******************************************************************/
            BitmaskYx   = TDx&0xf0;
            modeT     = TDx&0x0f;
        }
        else
        {
            /*
             * no TDx byte
             */
            BitmaskYx = 0;
            if (InterfaceX == 1)
            {
                /*
                 * TD1 不存在, modeT 只能是0
                 */
                modeT = 0;
            }
        }

        g_SCDev.SupportedProtocolTypes |= (0x01 << modeT);

        if (InterfaceX == 1)
        {
            FirstT = modeT;
        }

        InterfaceX++;
    }/* end of while(Yx != 0) */


    /* OK, now that that has been completed, read the historical
     * characters.
     */
    if (errflg == 0)
    {
        if (Historicalk > 0)
        {
            /*
             * get history bytes
             */
            err = SC_ReadATR(&answer[i], Historicalk, i);
            if (err == ERR_SMC_NO_ERR)
            {
              memcpy(g_SCDev.HistoricalBytes, &answer[i], Historicalk);
              g_SCDev.HistoricalSize = Historicalk;
                i += Historicalk;
            }
            else
            {
                SCBLOCK_P("\r\n Get history fail");
                errflg = 55;
            }
        }
        /* TCK character should also be present only if list of
         * supported protocol types is not t=0 by itself.
         */
        if (g_SCDev.SupportedProtocolTypes != 1)
        {
            /*
             * Get TCK
             */
            err = SC_ReadATR(&answer[i], 1, i);
            if (err == ERR_SMC_NO_ERR)
            {
                Tck = answer[i++];
            }
            else
            {
                errflg = 56;
                SCBLOCK_P("\r\n Get TCK fail");
            }
        }

        if (err == ERR_SMC_NO_ERR)
        {
          /* Set Atr Msg Size */
          g_SCDev.ATRMsgSize = i;

          /* Set caller's parameters */
          if (panswer_len != NULL)
          {
            *panswer_len = i;
          }
          if (pReadBuffer != NULL)
          {
            memcpy(pReadBuffer, answer, i);
          }
        }

        if (i > SMC_MAX_ATR)
        {
            errflg = 57;
        }
    }

    if (err == ERR_SMC_NO_ERR)
    {
        if (g_SCDev.SpecificMode)
        {
            /* Choose specific mode */
            g_SCDev.WorkingType = g_SCDev.SpecificType;
        }
        else
        {
            /* Choose first offered protocol */
            g_SCDev.WorkingType = FirstT;

            /* Use default FInt and Dint */
            if (TA1Present == FALSE)
            {
                g_SCDev.FInt = SMART_F_DEFAULT;
                g_SCDev.DInt = SMART_D_DEFAULT;
            }

        }

        /* Set default retries iff T0 */
        if (g_SCDev.WorkingType == 0)
            g_SCDev.Retries = T0_RETRIES_DEFAULT;
        else
            g_SCDev.Retries = 0; /* Zero otherwise */


        /* Calculate work etu */
    if (bT14)
    {
      g_SCDev.WorkETU = SMC_ETU_T14;
      smc_set_etu(g_SCDev.WorkETU);
    }
    else
    {
    /* Close the cal etu by lusson 2012.7.17
     * For most cards can use default to work,but some card like conax
     * it can't to use th TA1. it must be used PPS to set the ETU
     */
      if(0)
      {
        _Calculate_ETU();
      }
    }


    //util_print_buffer(g_SCDev.ATRBytes, g_SCDev.ATRMsgSize, "SMC Atr");
    //util_print_buffer(g_SCDev.HistoricalBytes, g_SCDev.HistoricalSize, "SMC His");
    SCBLOCK_P("\r\nSC Protocol=%d", g_SCDev.WorkingType);

    return ERR_SMC_NO_ERR;
    }

    return (ERR_SMC_ATRDATA_PARSE_ERR + errflg);
}


/************************************************************************
 * Get SMC Card Status
 *
 * param:  NULL
 *
 * return: TRUE: Insert FALSE: Remove
 ************************************************************************/
BOOL sc_getcardstatus(void)
{
  return SMC_GET_STATUS();
}



#endif //#ifdef DRV_SEL_SMC


