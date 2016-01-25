/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_int.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mem_manager.h"
#include "drv_dev.h"
#include "../../drvbase/drv_dev_priv.h"

#include "hal_base.h"
#include "hal_irq.h"
#include "hal_gpio.h"

#include "smc_op.h"
#include "smc_pro.h"

//#define PPS
//#define SMC_PRO_PRINT_ON

#ifdef SMC_PRO_PRINT_ON
#define SMC_PRO_PRINTF     OS_PRINTF
#define SMC_PRO_DUMP(data, len) { const int l = (len); int i = 0;\
                         for(i = 0 ; i < l ; i ++) SMC_PRO_PRINTF(" %02x",*((data) + i)); \
                         SMC_PRO_PRINTF("\n"); }
#else
#define SMC_PRO_PRINTF DUMMY_PRINTF
#define SMC_PRO_DUMP(data,len) ;
#endif

#define SMC_GT_T0 12
#define SMC_GT_T1 11
#define SMC_WT 9600
#define SMC_MAX_ATR 32
#define SMC_PPSS_CLA 0xff
#define SMC_DEFAULT_WI 10
#define SMC_CMD_P3_SPE 0x00
#define SMC_STATUS_BTYE_OK 0x9000

#define RFU -1
#define SMC_FS -1
#define SMC_PRO_CT_CK_SIZE 4

/*!
Clock rate table - indicates the F value to be used.
   */
typedef struct
{
   int fi;
   int fmax;
} smc_clk_rate_t;


/*!
smc pro private struct
   */
typedef struct
{
  u32 card_type;
  scard_device_t *p_smc;
  scard_protocol_t m_protocol;
}smc_pro_priv_t;

/*!
define the size of the smc pro ct name
   */
#define SMC_PRO_CT_NAME_SZ 20

/*!
Card type definition
   */
typedef enum
{
  SMC_CARD_DEF = 0,
  SMC_CARD_CONAX,
  SMC_CARD_VIACCESS,
  SMC_CARD_IRDETO_162,
  SMC_CARD_DREAMCRYPT_223,
  SMC_CARD_DVN_TEST,
  SMC_CARD_IRDETO_557,
  SMC_CARD_IRDETO_178,
  SMC_CARD_LIHE_E02B,
  SMC_CARD_VIACCESS3585,
  SMC_CARD_VIACCESS7289,
  SMC_CARD_TONGFANG,
  SMC_CARD_ATOSCARD,
}scard_card_type_list_t;

/*!
Card type struct definition
   */
typedef struct
{
  scard_card_type_list_t ct_e;
  u8   *p_ct_atr;
  u32 *p_ct_ck;
  u8    name[SMC_PRO_CT_NAME_SZ];
}scard_card_type_t;

smc_pro_priv_t g_smc_pro_priv = {0};

/*!
Values of D defined in protocol.
   */
static u16 baud_rate_table[] = { RFU, 1, 2, 4, 8, 16, 32,
                                   RFU, 12, 20, RFU, RFU,
                                   RFU, RFU, RFU, RFU};
/*!
Values of Fi, Fmax defined in protocol.
   */
static smc_clk_rate_t clock_rate_table[] = { { 372, 4 },
                                               { 372, 5 },
                                               { 558, 6 },
                                               { 744, 8 },
                                               { 1116, 12 },
                                               { 1488, 16 },
                                               { 1860, 20 },
                                               { RFU, RFU },
                                               { RFU, RFU },
                                               { 512, 5 },
                                               { 768, 7 },
                                               { 1024, 10 },
                                               { 1536, 15 },
                                               { 2048, 20 },
                                               { RFU, RFU },
                                               { RFU, RFU } };


/*!
   Initialize HW to be ready for ATR reception.
 */
static void itn_proll_config_def(void)
{
  scard_config_t smc_cfg;
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  smc_cfg.bit_d_rate = 1;
  smc_cfg.clk_f_rate = 372;
  smc_cfg.blk_guard = 0xff;
  smc_cfg.blk_time = 19200;
  smc_cfg.ch_guard = 12;
  smc_cfg.ch_time = 19200;
  smc_cfg.clk_ref = 19;
  smc_cfg.conv_set = FALSE;
  smc_cfg.parity_check = FALSE;
  scard_set_config(p_priv->p_smc, &smc_cfg);
}


static BOOL itn_check_card(scard_atr_desc_t *p_atr)
{
  scard_pro_cfg_t pro_cfg;
  OS_PRINTF("itn_check_card\n");
  if(scard_pro_atr_parse(p_atr, &pro_cfg) != SUCCESS)
  {
    OS_PRINTF("itn_check_card::ATR parse fail\n");
    return FALSE;
  }
  return TRUE;
}


RET_CODE scard_pro_atr_parse(scard_atr_desc_t *p_atr, scard_pro_cfg_t *p_cfg)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  u8 td = 0;
  u8 t = 0; //index of T in TDi.  t = 0 is for T0; t = 1 for TD1 ...
  u8 i = 1; //T0
  u8 tck = 0; //present of tck.
  u8 protocol = 0;

  /*use default Fi = 372 5M Di = 1*/
  u16 clk_rate_conv = 1; //Fi
  u16 baud_rate_adj = 1; //Di

  u32 extra_gardtime = 255; //N
  u8 *p_u8 = p_atr->p_buf;
  u8 size = p_atr->atr_len;

  if(p_u8 == NULL || size == 0)
  {
    SMC_PRO_PRINTF("scard_pro_atr_parse error return \n");
    return ERR_PARAM;
  }
//  MT_ASSERT((NULL != p) && (1 < size));
  MT_ASSERT(NULL != p_cfg);

  /*
	 * set the default value of cwi and  bwi
	 */
  p_cfg->cwi = 13;
  p_cfg->bwi = 4;

  while (i < size)
  {
    td = p_u8[i]; //td0 = T0;
    SMC_PRO_PRINTF("scard_pro_atr_parse::Data[0x%x] idx[%d] t[%d]\n",td,i,t);
    if (t >= 1)
    {
//      MT_ASSERT((protocol + 1) == (td & 0xF));
      protocol = td & 0xF;
      SMC_PRO_PRINTF("scard_pro_atr_parse::Protocol[%d] TD%d: data[0x%x] \n",
                        protocol, t + 1, p_u8[i]);
      tck = 1;
    }

    if (td & 0x10) //TAi
    {
      i++;
      if (0 == t) //TA1
      {
        SMC_PRO_PRINTF("scard_pro_atr_parse::TA1: data[0x%x] \n",p_u8[i]);
        baud_rate_adj = p_u8[i] & 0xF;
        clk_rate_conv = (p_u8[i] >>4) & 0xF;
        SMC_PRO_PRINTF("scard_pro_atr_parse::TA1: br[0x%x] clk[0x%x]\n",
                        baud_rate_adj,clk_rate_conv);
      }
      else if(1 == t)
      {
        //TA2, negotiable/specific mode, F/D apply, and T
        //TA2, TAi
        SMC_PRO_PRINTF("scard_pro_atr_parse::TA2 is exist\n");
        if(p_u8[i]&0x10)
        {
          clk_rate_conv = 1;
          baud_rate_adj = 1;
        }
      }
    }
    else/*TA1 is not exist*/
    {
      clk_rate_conv = 1;
      baud_rate_adj = 1;
      SMC_PRO_PRINTF("scard_pro_atr_parse::TA1 is not exist!\n");
    }

    if (td & 0x20) //TBi
    {
      i++;
      if (0 == t) //TB1
      {
        SMC_PRO_PRINTF("scard_pro_atr_parse::TB1 is exist!\n");
      }
      else
      {
        SMC_PRO_PRINTF("scard_pro_atr_parse::TB%d: data[0x%x] \n",t + 1, p_u8[i]);
        p_cfg->cwi = p_u8[i] & 0xF;
        p_cfg->bwi = (p_u8[i] >>4) & 0xF;
        SMC_PRO_PRINTF("scard_pro_atr_parse::TB%d: cwi[0x%x] bwi[0x%x]\n",t + 1,
                        p_cfg->cwi,p_cfg->bwi);
      }
    }

    if (td & 0x40) //TCi
    {
      i++;
      if (0 == t) //TC1
      {
        SMC_PRO_PRINTF("scard_pro_atr_parse::TC1: data[0x%x] \n",p_u8[i]);
        extra_gardtime = p_u8[i];
      }
    }

    if (td & 0x80) //TDi
    {
      SMC_PRO_PRINTF("scard_pro_atr_parse::TD%d: data[0x%x] \n",t,p_u8[i]);
      i++;
      t++;
    }
    else
    {
      break;
    }
  }


  /*!
  Check TCK.
     */
  if (1 == tck)
  {
    u8 val = p_u8[1] ^ p_u8[2];
    SMC_PRO_PRINTF("scard_pro_atr_parse::TCK Check\n");
    for (i = 3; i < size; i++)
    {
      val ^= p_u8[i];
    }

    if (0 != val)
    {
      SMC_PRO_PRINTF("scard_pro_atr_parse::TCK val[0x%x]\n", val);
      return ERR_FAILURE;
    }
  }

  /*!
  set CLK_DIV
     */
  /*!
  set FD
     */
  p_cfg->clk_fi = clk_rate_conv;
  p_cfg->baud_di = baud_rate_adj;
  p_cfg->extra_gardtime = extra_gardtime;
  SMC_PRO_PRINTF("scard_pro_atr_parse::atr: FI[%d] DI[%d] gt[%d]\n",
                 p_cfg->clk_fi,p_cfg->baud_di,p_cfg->extra_gardtime);
  switch (protocol)
  {
  case 1:
    p_cfg->protocol = SMC_PROTOCOL_T1;
    OS_PRINTF("scard_pro_atr_parse::T1 SMC_PROTOCOL_T1\n");
    break;
  case 14:
    p_cfg->protocol = SMC_PROTOCOL_T14;
    OS_PRINTF("scard_pro_atr_parse::T14 SMC_PROTOCOL_T14\n");
    break;
  default:
    p_cfg->protocol = SMC_PROTOCOL_T0;
    OS_PRINTF("scard_pro_atr_parse::T0 SMC_PROTOCOL_T0\n");
    break;
  }
  p_priv->m_protocol = p_cfg->protocol;

  scard_pro_set_protocol(p_priv->m_protocol);
  return SUCCESS;
}


RET_CODE scard_pro_interface_cfg(scard_pro_cfg_t *p_cfg)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;
  scard_config_t smc_cfg;

  scard_get_config(p_priv->p_smc, &smc_cfg);
  OS_PRINTF("%s %d\n",__FUNCTION__,__LINE__);
  /*!
  Protocol selection.
     */
  if (p_priv->m_protocol != p_cfg->protocol)
  {
    OS_PRINTF("%s %d\n",__FUNCTION__,__LINE__);
    return ERR_NOFEATURE;
  }
  else
  {
    //smc_proll_pts(dev);
  }
  OS_PRINTF("%s %d\n",__FUNCTION__,__LINE__);
  /*!
  set FD
     */
  if(SMC_CARD_CONAX != p_priv->card_type &&
      SMC_CARD_VIACCESS != p_priv->card_type)
  {
    smc_cfg.bit_d_rate = baud_rate_table[p_cfg->baud_di];
    smc_cfg.clk_f_rate = clock_rate_table[p_cfg->clk_fi].fi;
    if(SMC_CARD_IRDETO_162 == p_priv->card_type)
    {
      smc_cfg.bit_d_rate = 1;
      smc_cfg.clk_f_rate = 625;
    }
  }

  if (SMC_PROTOCOL_T1 == p_priv->m_protocol ||
      SMC_PROTOCOL_T14 == p_priv->m_protocol)
  {
    smc_cfg.parity_check = FALSE;
    smc_cfg.ch_time = SMC_GT_T1 + (1 << p_cfg->cwi);//CWT
    smc_cfg.blk_time = SMC_GT_T1 +
    (1 << p_cfg->bwi) * 960 * 372 * baud_rate_table[p_cfg->baud_di]
        / clock_rate_table[p_cfg->clk_fi].fi;//BWT

    if(255 != p_cfg->extra_gardtime)
    {
      smc_cfg.ch_guard = SMC_GT_T0 + p_cfg->extra_gardtime + 32;
    }
    else
    {
      smc_cfg.ch_guard = SMC_GT_T1;
    }
    smc_cfg.blk_guard = 22;//BGT
  }
  else
  {
    smc_cfg.parity_check = TRUE;
    smc_cfg.ch_time = 19200;
    smc_cfg.blk_time = 19200;
    if(255 != p_cfg->extra_gardtime)
    {
      smc_cfg.ch_guard = SMC_GT_T0 + p_cfg->extra_gardtime;
    }
    else
    {
      smc_cfg.ch_guard = SMC_GT_T0;
    }
    smc_cfg.blk_guard = 0xff;
  }
  
  OS_PRINTF("%s %d\n",__FUNCTION__,__LINE__);
  scard_set_config(p_priv->p_smc, &smc_cfg);
  return SUCCESS;
}


RET_CODE scard_pro_deactive(void)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  return scard_deactive(p_priv->p_smc);
}

RET_CODE scard_pro_rw_transaction(scard_opt_desc_t *p_rwopt)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  //SMC_PRO_PRINTF("%s line%d\n",__FUNCTION__,__LINE__);
  return scard_rw_transaction(p_priv->p_smc, p_rwopt);
}

/*********************************************************************************
T=0 Protocol Process
**********************************************************************************/
#define T0_CMD_LENGTH   5
#define T0_CLA_OFFSET   0
#define T0_INS_OFFSET   1
#define T0_P1_OFFSET    2
#define T0_P2_OFFSET    3
#define T0_P3_OFFSET    4
#define T0_RETRIES_DEFAULT  3
#define T0_MAX_LENGTH   256

static s32 smc_t0_procedure_bytes(scard_device_t *dev, u8 INS, s16 num_to_transfer, u8 status[2])
{
  s32 r = 0;
  u32 act_size = 0;
  u8 buff = 0;

  SMC_PRO_PRINTF("t0: PROC: ");

  do
  {
    do
    {
      if (scard_read(dev, &buff, 1, &act_size) != SUCCESS)
        return ERR_TIMEOUT;
      SMC_PRO_PRINTF("%02x ", buff);
    } while(buff == 0x60);  /*NULL, send by the card to reset WWT*/

    if((buff&0xF0) == 0x60 || (buff&0xF0) == 0x90) //SW1
    {
      status[0] = buff;
      if(scard_read(dev, &buff, 1, &act_size) != SUCCESS)
        return ERR_TIMEOUT;
      SMC_PRO_PRINTF("%02x\n", buff);
      status[1] = buff;
      return 0;
    }
    else //ACK
    {
      if((buff ^ INS) == 0)
      {
        /* ACK == INS*/
        /*Vpp is idle. All remaining bytes are transfered subsequently.*/
        r = num_to_transfer;
      }
      else if((buff ^ INS) == 0xFF)
      {
        /* ACK == ~INS*/
        /*Vpp is idle. Next byte is transfered subsequently.*/
        r = 1;
      }
      else if((buff ^ INS) == 0x01)
      {
        /* ACK == INS+1*/
        /*Vpp is active. All remaining bytes are transfered subsequently.*/
        r = num_to_transfer;
      }
      else if((buff ^ INS) == 0xFE)
      {
        /* ACK == ~INS+1*/
        /*Vpp is active. Next bytes is transfered subsequently.*/
        r = 1;
      }
      else
      {
        SMC_PRO_PRINTF("iso: cannot handle procedure %02x (INS=%02x)\n", buff, INS);
        return -1;
      }
      if(r > num_to_transfer)
      {
        SMC_PRO_PRINTF("iso: data overrun r=%d num_to_transfer=%d", r, num_to_transfer);
        return -1;
      }
    }
  } while(0 == r);

  SMC_PRO_PRINTF("\n");
  return r;
}

RET_CODE scard_pro_t0_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  s32 error = SUCCESS;
  u8 write_to_smartcard = 0;
  u32 size = 0;
  s16 num_to_transfer = 0;
  s16 length = 0;
  s16 temp_length = 0;
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;
  u32 num_to_write = p_rwopt->bytes_out;
  s32 ret = 0;

  p_rwopt->bytes_in_actual = 0;

  if(num_to_write > T0_CMD_LENGTH)
  {
    write_to_smartcard = 1;
    num_to_transfer = p_command[T0_P3_OFFSET];
    SMC_PRO_PRINTF("iso: wr, size[%d]\n", num_to_transfer);
  }
  else if(num_to_write == T0_CMD_LENGTH)
  {
    write_to_smartcard = 0;
    if(p_response == NULL)
    {
      /* write data to smart card*/
      num_to_transfer = 0;
    }
    else
    {
      /*read data from smart card*/
      num_to_transfer = (p_command[T0_P3_OFFSET] == 0) ? T0_MAX_LENGTH : p_command[T0_P3_OFFSET];
    }
    SMC_PRO_PRINTF("iso: rd, size[%d]\n", num_to_transfer);
  }
  else
  {
    SMC_PRO_PRINTF("iso: error p_command length!\n");
    return ERR_FAILURE;
  }

  /* Check the CLA and INS bytes are valid */
  if (p_command[T0_CLA_OFFSET] != 0xFF)
  {
    if ((p_command[T0_INS_OFFSET] & 0x01) == 0 &&
    (p_command[T0_INS_OFFSET] & 0xF0) != 0x60 &&
    (p_command[T0_INS_OFFSET] & 0xF0) != 0x90)
    {
      SMC_PRO_PRINTF("iso: scard_write call!\n");
      if(scard_write(g_smc_pro_priv.p_smc, p_command, T0_CMD_LENGTH, &size) != SUCCESS)
      {
        SMC_PRO_PRINTF("iso: write cmd error!\n");
        return ERR_FAILURE;
      }
      length = 0;
      while(1)
      {
        temp_length = smc_t0_procedure_bytes(g_smc_pro_priv.p_smc,
          p_command[T0_INS_OFFSET], num_to_transfer - length, status);
        if(temp_length == 0)
        {
          return SUCCESS;
        }
        else if(temp_length < 0)
        {
          return ERR_FAILURE;
        }
        if (write_to_smartcard == 1)
        {
          if(scard_write(g_smc_pro_priv.p_smc,
            p_command + T0_CMD_LENGTH + length, temp_length, &size) != SUCCESS)
          {
            SMC_PRO_PRINTF("iso: write data error!\n");
            return ERR_FAILURE;
          }
          SMC_PRO_PRINTF("iso: -> WDATA: ");
          SMC_PRO_DUMP(p_command + T0_CMD_LENGTH + length, temp_length);
        }
        else
        {
          ret = scard_read(g_smc_pro_priv.p_smc,
            p_response + length, temp_length, &size);
          if (ret != SUCCESS)
          {
            SMC_PRO_PRINTF("iso: read data error!\n");
            return ERR_FAILURE;
          }
          p_rwopt->bytes_in_actual += size;
          SMC_PRO_PRINTF("iso: <- DATA: ");
          SMC_PRO_DUMP(p_response + length, temp_length);
        }
        length += temp_length;
      }
    }
    else
    {
      SMC_PRO_PRINTF("iso: INS is invalid!\n");
      /* INS is invalid */
      error = ERR_FAILURE;
    }
  }
  else
  {
    SMC_PRO_PRINTF("iso: CLA is invalid!\n");
    /* CLA is invalid */
    error = ERR_FAILURE;
  }
  return error;
}

/*********************************************************************************
T=1 Protocol Process
**********************************************************************************/
/* Generally useful things */
#define MAXBLOCKLEN             3 + 254 + 2

/* Our internal state markers */
#define SMC_T1_OURTX          0x0001
#define SMC_T1_CHAINING_US    0x0002
#define SMC_T1_CHAINING_THEM  0x0004
#define SMC_T1_TXWAITING      0x0008
#define SMC_T1_SRESPEXPECT    0x0010
#define SMC_T1_VPPHIGHTILNAD  0x0020
#define SMC_T1_GOTNAK         0x0040
#define SMC_T1_WTX            0x0080

/* Block types */
#define BLOCK_TYPE_BIT          0xC0
#define S_REQUEST_BLOCK         0xC0
#define R_BLOCK                 0x80
#define I_BLOCK_1               0x00
#define I_BLOCK_2               0x40

/* In S-block */
#define S_RESYNCH_REQUEST       0x00
#define S_IFS_REQUEST           0x01
#define S_ABORT_REQUEST         0x02
#define S_WTX_REQUEST           0x03
#define S_VPP_ERROR_P_RESPONSE    0x24
#define S_P_RESPONSE_BIT          0x20
#define S_RESYNCH_P_RESPONSE      0xE0

/* I-block */
#define I_CHAINING_BIT          0x20
#define I_SEQUENCE_BIT          0x40

/* R-block */
#define R_EDC_ERROR             0x01
#define R_OTHER_ERROR           0x02

/* NAD byte */
#define VPP_NOTHING             0x00
#define VPP_TILNAD              0x08
#define VPP_TILPCB              0x80
#define VPP_INVALID             0x88

/*!
Block type
   */
typedef enum
{
  T1_R_BLOCK = 0,
  T1_S_REQUEST,
  T1_S_P_RESPONSE,
  T1_I_BLOCK,
  T1_CORRUPT_BLOCK
} smc_block_type_t;

/*!
smc_t1_header
   */
typedef struct {
  u8 NAD;
  u8 PCB;
  u8 LEN;
} smc_t1_header_t;

/*!
smc_t1_block
   */
typedef struct {
  smc_t1_header_t header;
  u8 *p_buffer;
  union
  {
    u8 LRC;
    u8 CRC;
  } EDC;
} smc_t1_block_t;

#define NEXT_SEQ(i)      ((i == 1) ? 0 : 1)

static u8 smc_gen_lrc(u8 *mem, int len)
{
  u8 cs = 0;

  while(len > 0)
  {
    cs ^= *mem++;
    len--;
  }
  return cs;
}

RET_CODE scard_pro_t1_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  u32 actlen = 0;
  u8 lrc = 0;
  u8 buffer[MAXBLOCKLEN];
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;

  p_rwopt->bytes_in_actual = 0;

  /* assume that user has prepared the block and EDC */
  SMC_PRO_PRINTF("scard_pro_t1_transactionT1::CMD:");
  SMC_PRO_PRINTF(p_command, p_command[2] + 4);

  if (scard_write(g_smc_pro_priv.p_smc,
    p_command, p_command[2] + 4, &actlen) != SUCCESS)
    return ERR_FAILURE;
  if (scard_read(g_smc_pro_priv.p_smc,
    buffer, 3, &actlen) != SUCCESS)
    return ERR_FAILURE;
  if (scard_read(g_smc_pro_priv.p_smc,
    buffer + 3, buffer[2] + 1, &actlen) != SUCCESS)
    return ERR_FAILURE;

  SMC_PRO_PRINTF("scard_pro_t1_transactionT1::T1 ASR:");
  SMC_PRO_PRINTF(buffer, buffer[2] + 4);
  SMC_PRO_PRINTF("\n");

  lrc = smc_gen_lrc(buffer, buffer[2]+3);
  if (lrc != buffer[buffer[2] + 3])
  {
    SMC_PRO_PRINTF("scard_pro_t1_transactionT1::EDR Failure\n");
    return ERR_FAILURE;
  }

  memcpy(p_response, buffer, buffer[2] + 4);
  p_rwopt->bytes_in_actual = buffer[2] + 4;

  return SUCCESS;
}


/*********************************************************************************
T=14 Protocol Process
**********************************************************************************/
#define T14_XOR_START   0x3F  /* Start value for xor checksumm */
#define T14_CMD_LEN_ADDR  5   /* T14 p_command length byte */
#define T14_CMD_HEAD_LEN  6   /* T14 p_command header length, do not include checksum */

RET_CODE scard_pro_t14_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  u16 length = 0;
  u8 xorsum = 0;
  u32 actlen = 0;
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;
  u32 num_to_write = p_rwopt->bytes_out;

  p_rwopt->bytes_in_actual = 0;

  length = p_command[T14_CMD_LEN_ADDR] + T14_CMD_HEAD_LEN;
  if (num_to_write != length + 1)
  {
    SMC_PRO_PRINTF("iso: error p_command length \n");
    return ERR_FAILURE;
  }

  xorsum = smc_gen_lrc(p_command, length) ^ T14_XOR_START;
  if (p_command[length] != xorsum)
  {
    SMC_PRO_PRINTF("iso: update T14 check sum \n");
    //    p_command[length] = xorsum;
  }

  SMC_PRO_PRINTF("T14 CMD: ");
  SMC_PRO_DUMP(p_command, num_to_write);
  if (scard_write(g_smc_pro_priv.p_smc,
    p_command, num_to_write, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }

  if (scard_read(g_smc_pro_priv.p_smc,
    p_response, 4, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }
  length = 4;
  if (p_response[0] == p_command[0] && p_response[1] == p_command[1])
  {
    status[0] = p_response[2];
    status[1] = p_response[3];
    if (scard_read(g_smc_pro_priv.p_smc,
        p_response + length, 5, &actlen) != SUCCESS)
    {
      return ERR_FAILURE;
    }
    length += 5;
    if (p_response[7])
    {
      if (scard_read(g_smc_pro_priv.p_smc,
          p_response + length, p_response[7], &actlen) != SUCCESS)
      {
        return ERR_FAILURE;
      }
      length += p_response[7];
      if (smc_gen_lrc(p_response, length) != T14_XOR_START)
      {
        SMC_PRO_PRINTF("iso: T14 checksum failed\n");
        return ERR_FAILURE;
      }
    }
  }
  else
  {
    status[0] = p_response[1];
    status[1] = p_response[2];
  }

  p_rwopt->bytes_in_actual = length;

  SMC_PRO_PRINTF("T14 ASR: ");
  SMC_PRO_DUMP(p_response, length);
  return SUCCESS;
}

static void data_dump(u8 *p_addr, u32 size)
{
  u32 i = 0;

  OS_PRINTF("\r\n size is %d: \r ",size);

  for(i = 0; i < size; i++)
  {
    OS_PRINTF("%2x ", p_addr[i]);

    if ((0 == (i % 4)) && (i != 0))
    {
      OS_PRINTF("    ");
    }
    if ((0 == (i % 32)) && (i != 0))
    {
      OS_PRINTF("\r\n");
    }
  }

  OS_PRINTF("\r\n");
}

RET_CODE scard_pro_active(scard_atr_desc_t *p_atr)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;
  scard_config_t smc_cfg;
  RET_CODE ret = SUCCESS;
  BOOL check = TRUE;
  MT_ASSERT((NULL != p_atr) && (NULL != p_atr->p_buf));


  scard_reset(p_priv->p_smc);
  itn_proll_config_def();
  OS_PRINTF("******scard_pro_active::ACTIVE TRY 1*****\n");
  ret = scard_active(p_priv->p_smc, p_atr);
  check = itn_check_card(p_atr);
  OS_PRINTF("===scard_pro_active::ATR Data_dump====\n");
  data_dump(p_atr->p_buf, p_atr->atr_len);
  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = TRUE;
    scard_set_config(p_priv->p_smc, &smc_cfg);
  	OS_PRINTF("******scard_pro_active::ACTIVE TRY 2*****\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }

  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = FALSE;
    smc_cfg.clk_ref = 11;
    smc_cfg.clk_f_rate = 625;
    scard_set_config(p_priv->p_smc, &smc_cfg);
  	OS_PRINTF("******scard_pro_active::ACTIVE TRY 3*****\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }

  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = FALSE;
    smc_cfg.clk_ref = 18;
    smc_cfg.clk_f_rate = 607;
    scard_set_config(p_priv->p_smc, &smc_cfg);
  	OS_PRINTF("*****scard_pro_active::ACTIVE TRY 4******\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }
  OS_PRINTF("active ret[%d] \n",ret);

  if(check)
  {
    return SUCCESS;
  }
  return ERR_FAILURE;
}

RET_CODE scard_pro_register_op(scard_device_t *p_op_device)
{
  g_smc_pro_priv.p_smc = p_op_device;

  return SUCCESS;
}

RET_CODE scard_pro_set_protocol(scard_protocol_t protocol)
{
  g_smc_pro_priv.m_protocol = protocol;

  return SUCCESS;
}

RET_CODE scard_pro_get_card_type(u32 *card_type)
{
  *card_type = g_smc_pro_priv.card_type;
  return SUCCESS;
}
#ifdef PPS
#define PPS_MAX_LENGTH  6

#define PPS_HAS_PPS1(block) ((block[1] & 0x10) == 0x10)
#define PPS_HAS_PPS2(block) ((block[1] & 0x20) == 0x20)
#define PPS_HAS_PPS3(block) ((block[1] & 0x40) == 0x40)

static int scard_pps_length(u8 *block)
{
  u32 length = 3;
  if(PPS_HAS_PPS1(block)) {
    length++;
  }

  if(PPS_HAS_PPS2(block)) {
    length++;
  }

  if(PPS_HAS_PPS3(block)) {
    length++;
  }
  return length;
}
static u32 data_cmp(u8 *src,u8 *dst, u32 len)
{
  while (len--)
  {
    if(*src++ != *dst++)
    {
      return 1;
    }
  }
  return 0;
}
RET_CODE smc7816_pps_req(scard_pro_cfg_t *p_cfg)
{
  u8 request[PPS_MAX_LENGTH] = {0};
  u8 response[PPS_MAX_LENGTH] = {0};
  //u32 baudrate = 0;
  u32 len_request = 0,len_response = 0,len_confirm = 0;

  /* build PPS command */
  /* PPSS: initial byte-always 0xFF */
  request[0] = 0xFF;
  /* PPS0: format byte-encodes T and PPS1 to follow */
  request[1] = (0x10 | p_cfg->protocol);
  /* PPS1: parameter byte-encodes F and D */
  request[2] = (p_cfg->baud_di | (p_cfg->clk_fi << 4));
  /* PPS2 and PPS3 are RFU, so ignore */
  /* PCK : check byte-bitwise XOR of PPSS,PPS0,and PPS1 */
  request[3] = (request[0] ^ request[1] ^ request[2]);

  OS_PRINTF("PPS: request\n");
  data_dump(request, 4);
  if (scard_write(g_smc_pro_priv.p_smc, request, 4, &len_request) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_write error\n");
    return ERR_FAILURE;
  }
  if (4 != len_request)
  {
    OS_PRINTF("PPS: len error\n");
    return ERR_FAILURE;
  }
  /*Get PPSS and PPSO response first*/
  if (scard_read(g_smc_pro_priv.p_smc, response, 2, &len_response) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_read error\n");
    return ERR_FAILURE;
  }
  /*Get the next 4 bytes PPS1 PPS2 PPS3 and PCK*/
  len_confirm = scard_pps_length(response);
  if (scard_read(g_smc_pro_priv.p_smc, (response + 2), len_confirm - 2 , &len_response) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_read2 error\n");
    return ERR_FAILURE;
  }
  OS_PRINTF("PPS: response\n");
  data_dump(response, len_confirm);
  if ((len_request != len_confirm)|| (data_cmp(request, response, len_confirm) == 1))
  {
    OS_PRINTF("PPS: fail\n");
    return ERR_FAILURE;
  }
  /*PPS1 response is not exist,so use Fd and Dd*/
  if(!PPS_HAS_PPS1(response))
  {
    p_cfg->clk_fi = 0;
    p_cfg->baud_di = 1;
  }
  scard_pro_interface_cfg(p_cfg);
  return SUCCESS;
}

#endif

