/******************************************************************************/
/******************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "mtos_fifo.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "hal_irq.h"
#include "drv_dev.h"
#include "hal_base.h"
#include "uio.h"
#include "../uio_priv.h"

#define IRDA_SW_NEC 1

#define IRDA_DEBUG
#ifdef IRDA_DEBUG
 #define IRDA_PRINT OS_PRINTF
#else
 #define IRDA_PRINT(x)     do {} while(0)
#endif

/*****************************************************************
* IRDA Internal variables
*****************************************************************/
#define IRDA_RPT_TIME_MS         300

typedef struct
{
  BOOL print_user_code;
  u16 sw_user_code[IRDA_MAX_USER];
  u8 user_code_set;
  u8 irda_protocol;
  u16 user_code;
  u32 irda_repeat_time;
  u8 irda_wfilt_channel;
  u32 irda_flag;
  irda_wfilt_cfg_t irda_wfilt_channel_cfg[IRDA_MAX_CHANNEL];
  
}irda_priv_t;


/*table of pairs of sample freq and sample timeout*/
static u8 sample_frq2timeout[4] = {
0x9d, /*3Mhz*/
0x4e, /*750Khz*/
0x7e,/*0xd3, 187.5Khz*/
0x1a /*46.875Khz*/};

extern BOOL uio_check_rpt_key(uio_type_t uio, u8 code, u8 ir_index);


/*****************************************************************
* IRDA Internal functions
*****************************************************************/
/* IRDA Soft Decoder Common */
#define PULSE_FIFO_SIZE 256
typedef struct pulsedata
{
    u16 ontime;
    u16 period;
} st_pulsedata;

typedef struct pulse_fifo
{
    struct pulsedata pulse[PULSE_FIFO_SIZE];
    int wp;
    int rp;
    int count;
} st_pulse_fifo;

static os_sem_t recvpulse_sem = 0;
static struct pulse_fifo pulsefifo;
static int put_pulsefifo(struct pulsedata *p_pulse)
{

    if((pulsefifo.wp == pulsefifo.rp) && pulsefifo.count > 0)
    {
        return pulsefifo.count;
    }
    else
    {
        pulsefifo.pulse[pulsefifo.wp].ontime = p_pulse->ontime;
        pulsefifo.pulse[pulsefifo.wp].period = p_pulse->period;
        
        pulsefifo.count ++;
        pulsefifo.wp ++;
        pulsefifo.wp %= PULSE_FIFO_SIZE;
    }

    return pulsefifo.count;
}

static int get_pulsefifo(struct pulsedata *p_pulses, int pulsenum)
{
    u32 sr;
    int read_count = 0;
    mtos_critical_enter(&sr);
    while(pulsefifo.count && pulsenum)
    {
        p_pulses[read_count].ontime = pulsefifo.pulse[pulsefifo.rp].ontime;
        p_pulses[read_count].period = pulsefifo.pulse[pulsefifo.rp].period;
        read_count ++;
        pulsefifo.rp ++;
        pulsefifo.rp %= PULSE_FIFO_SIZE;
        pulsefifo.count  --;

        pulsenum --;
    }
    mtos_critical_exit(sr);
    return read_count;
}

static void flush_pulse_fifo(void)
{
    u32 sr;
    mtos_critical_enter(&sr);
    memset(pulsefifo.pulse, 0, sizeof(struct pulsedata) * PULSE_FIFO_SIZE);
    pulsefifo.count = 0;
    pulsefifo.rp = 0;
    pulsefifo.wp = 0;
    mtos_critical_exit(sr);
}

#define clock_to_us(clock) (((u32)clock * 10000) / 1875)
static void get_ontime_and_period(u16 *p_ontime, u16 *p_period)
{
    u8 ontime_l = 0;
    u8 ontime_h = 0;
    u8 period_l = 0;
    u8 period_h = 0;

    hal_put_u8((volatile u8 *) R_IR_BUF_POP, 1);
    ontime_l = hal_get_u8((volatile u8 *) R_IR_COM_ONTIME_L);
    ontime_h = hal_get_u8((volatile u8 *) R_IR_COM_ONTIME_H);
    period_l = hal_get_u8((volatile u8 *) R_IR_COM_PERIOD_L);
    period_h = hal_get_u8((volatile u8 *) R_IR_COM_PERIOD_H);
    *p_ontime = ontime_h << 8 | ontime_l;
    *p_period = period_h << 8 | period_l;
}

static u8 get_pulse_num(void)
{
    return hal_get_u8((volatile u8 *) R_IR_RECEIVE_NUM);
}

/* IRDA Soft Decoder Common END*/

/* IRDA NEC Decoder */
//#define NEC_DEBUG
#ifdef NEC_DEBUG
#define NEC_DEBUG_PRINT OS_PRINTF
#else
#define NEC_DEBUG_PRINT(...) do{}while(0)
#endif

#define PULSE_TIMEOUT   150
#define START_ONTIME_DOWNLIMIT  6292
#define START_ONTIME_UPPERLIMIT  10000
#define DATA_ONTIME_DOWNLIMIT  298
#define DATA_ONTIME_UPPERLIMIT  980

#define LOGIC_0_DOWNLIMIT   980
#define LOGIC_0_UPPERLIMIT  1450
#define LOGIC_1_DOWNLIMIT   1578 
#define LOGIC_1_UPPERLIMIT  2922
#define LOGIC_START_DOWNLIMIT   12180
#define LOGIC_START_UPPERLIMIT  17534
#define LOGIC_REPEAT_DOWNLIMIT  10000
#define LOGIC_REPEAT_UPPERLIMIT  12159

typedef enum nec_pulse_type
{
    NEC_PULSE_START = 0,
    NEC_PULSE_0,
    NEC_PULSE_1,
    NEC_PULSE_REPEAT,
    NEC_PULSE_UNKNOWN
} e_nec_pulse_type;

static int nec_decoder_is_01_noise(st_pulsedata *p_pulse)
{
    if(clock_to_us(p_pulse->period) <= LOGIC_0_DOWNLIMIT)
        return 1;
    return 0;
}

static int nec_decoder_is_start_noise(st_pulsedata *p_pulse)
{
    if(clock_to_us(p_pulse->period) <= LOGIC_REPEAT_DOWNLIMIT)
        return 1;
    return 0;
}

static int nec_decoder_is_validkey(e_nec_pulse_type *p_pulseslogics, u32 num)
{
    u32 i = 0;
    for(i = 0; i < num; i ++)
    {
        if(p_pulseslogics[i] != NEC_PULSE_0 && p_pulseslogics[i] != NEC_PULSE_1)
            return 0;
    }
    return 1;
}

static e_nec_pulse_type nec_decoder_get_pulse_logic(st_pulsedata *p_pulse)
{
    if(clock_to_us(p_pulse->period) >= LOGIC_0_DOWNLIMIT &&
                clock_to_us(p_pulse->period) <= LOGIC_0_UPPERLIMIT)
    {
        return NEC_PULSE_0;
    }
    else if(clock_to_us(p_pulse->period) >= LOGIC_1_DOWNLIMIT && 
                            clock_to_us(p_pulse->period) <= LOGIC_1_UPPERLIMIT)
    {
        return NEC_PULSE_1;
    }
    else if(clock_to_us(p_pulse->period) >= LOGIC_REPEAT_DOWNLIMIT && 
                            clock_to_us(p_pulse->period) <= LOGIC_REPEAT_UPPERLIMIT)
    {
        return NEC_PULSE_REPEAT;
    }
    else if(clock_to_us(p_pulse->period) >= LOGIC_START_DOWNLIMIT && 
                            clock_to_us(p_pulse->period) <= LOGIC_START_UPPERLIMIT)
    {
        return NEC_PULSE_START;
    }
    else
        return NEC_PULSE_UNKNOWN;
}

#ifdef NEC_DEBUG
static char *p_tostr[5] = {
"Start","0", "1", "Repeat", "Unknown"
};
#endif
static int nec_decoder_get_01_pulse(e_nec_pulse_type *p_pulses, u32 num)
{
    st_pulsedata one_pulse;
    u32 validpulse_count = 0; 
    u32 pulse_patch = 0;
    while(num)
    {
        if(get_pulsefifo(&one_pulse, 1) == 0)
        {
            if(mtos_sem_take(&recvpulse_sem, PULSE_TIMEOUT) == FALSE)
                return -1;
            continue;
        }
        NEC_DEBUG_PRINT("ND(RAW)-> ontime=%d us, period=%d us\n", 
        clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));
        one_pulse.ontime += pulse_patch;
        one_pulse.period += pulse_patch;
        NEC_DEBUG_PRINT("ND(Rm Noise)-> ontime=%d us, period=%d us\n", 
        clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));

        if(nec_decoder_is_01_noise(&one_pulse))
        {
            NEC_DEBUG_PRINT("ND(Have Noise)-> ontime=%d us, period=%d us\n", 
            clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));
            pulse_patch = one_pulse.period;
            continue;
        }

        pulse_patch = 0;
        p_pulses[validpulse_count] = nec_decoder_get_pulse_logic(&one_pulse);
        NEC_DEBUG_PRINT("ND(Logic)-> %s\n", p_tostr[p_pulses[validpulse_count]]);
        if(p_pulses[validpulse_count] == NEC_PULSE_UNKNOWN)
            return -1;
        num --;
        validpulse_count ++;
    }
    return 0;
}

static int nec_decoder_get_start_pulse(e_nec_pulse_type *p_pulses, u32 num)
{
    st_pulsedata one_pulse;
    u32 validpulse_count = 0; 
    u32 pulse_patch = 0;
    while(num)
    {
        if(get_pulsefifo(&one_pulse, 1) == 0)
        {
            if(mtos_sem_take(&recvpulse_sem, PULSE_TIMEOUT) == FALSE)
                return -1;
            continue;
        }
        NEC_DEBUG_PRINT("ND(RAW)@-> ontime=%d us, period=%d us\n", 
        clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));
        one_pulse.ontime += pulse_patch;
        one_pulse.period += pulse_patch;
        NEC_DEBUG_PRINT("ND(Rm Noise)@-> ontime=%d us, period=%d us\n", 
        clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));

        if(nec_decoder_is_start_noise(&one_pulse))
        {
            NEC_DEBUG_PRINT("ND(Have Noise)@-> ontime=%d us, period=%d us\n", 
            clock_to_us(one_pulse.ontime), clock_to_us(one_pulse.period));
            pulse_patch = one_pulse.period;
            continue;
        }

        pulse_patch = 0;
        p_pulses[validpulse_count] = nec_decoder_get_pulse_logic(&one_pulse);
        NEC_DEBUG_PRINT("ND(Logic)@-> %s\n", p_tostr[p_pulses[validpulse_count]]);
        if(p_pulses[validpulse_count] == NEC_PULSE_UNKNOWN)
            return -1;

        num --;
        validpulse_count ++;
    }
    return 0;
}
static void nec_decoder_sync_start(void)
{
    e_nec_pulse_type pulselogic = NEC_PULSE_UNKNOWN;
    NEC_DEBUG_PRINT("ND(Enter)-> nec_decoder_sync_start\n");

    while(1)
    {
        nec_decoder_get_start_pulse(&pulselogic, 1);
        if(pulselogic == NEC_PULSE_START)
            break;
    }
    NEC_DEBUG_PRINT("ND(Exit)-> nec_decoder_sync_start\n");

}

static u32 nec_decoder_bits2key(e_nec_pulse_type *p_pulses, u32 num)
{
    u32 key = 0;
    u32 i = 0;

    for(i = 0; i < num; i ++)
    {
        if(p_pulses[i] == NEC_PULSE_1)
        {
            key |= 1 << i;
        }
    }
    key = (key & 0xffff) << 16 | ((key >> 16) & 0xff) << 8 | ((key >> 24) & 0xff);
    return key;
}


static void nec_decoder_monitor(void *param)
{
    e_nec_pulse_type pulselogics[32];
    u32 key = 0;
    u8 need_sync = 1;
    u32 repeat_count = 0;
    u32 i = 0;
	u32 key_prefix = 0;
	u32 final_key = 0;

    uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                    DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
    uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
    irda_priv_t *p_irda = (irda_priv_t *)p_lld->p_irda;
		
    while(1)
    {
        if(need_sync)
        {
            nec_decoder_sync_start();
        }
        if(nec_decoder_get_01_pulse(pulselogics, 32) < 0)
        {
            need_sync = 1;
            continue;
        }
        if(nec_decoder_is_validkey(pulselogics,32))
        {
            key = nec_decoder_bits2key(pulselogics,32);
            NEC_DEBUG_PRINT("NEC Decoder:%X\n", key);
            if(TRUE == p_irda->user_code_set)
            {
                for(i = 0; i < IRDA_MAX_USER; i++)
                {
                    if(p_irda->sw_user_code[i] == ((key >> 16) & 0xffff))
                    {
                        break;
                    }
                }
                if(i >= IRDA_MAX_USER)
                {
                    continue;
                }
			   key_prefix = i << UIO_USR_CODE_SHIFT;
            }
		   final_key =  ((key >> 8) & 0xff) | (key_prefix << 8);
            NEC_DEBUG_PRINT("PUT KEY=%X\n", final_key);
            mtos_fifo_put(&p_priv->fifo, final_key);
            mtos_task_sleep(1);
        }
        else
        {
            need_sync = 1;
            continue;
        }

        repeat_count = 0;
        while(1)
        {
            if(nec_decoder_get_start_pulse(pulselogics, 1) < 0)
            {
                need_sync = 1;
                break;
            }
            if(pulselogics[0] == NEC_PULSE_REPEAT)
            {
                need_sync = 1;
                if(FALSE == uio_check_rpt_key(UIO_IRDA, final_key, i))
                {
                    break;
                }
			   if(repeat_count > 0)
                {
                    repeat_count = 0;
                    NEC_DEBUG_PRINT("Repeat PUT Key=%X\n", final_key | (1 << 15));
                    mtos_fifo_put(&p_priv->fifo, final_key | (1 << 15));
                    mtos_task_sleep(1);
                }
                else
                    repeat_count ++;
            }
            else if(pulselogics[0] == NEC_PULSE_START)
            {
                need_sync = 0;
                break;
            }
            else 
            {
                need_sync = 1;
                break;
            }
        }
    }
}

/*IRDA NEC Decoder END*/


inline static void irda_jazz_soft_reset(void)
{
  u8 rtmp = 0;
  u8 loop = 0xff;

  rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CTRL);
  rtmp &= ~0x1;
  hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, rtmp);
  while(loop --);

  rtmp |= 0x1;
  hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, rtmp);
}

inline static void irda_jazz_wave_channel_set(u8 channel)
{
  hal_put_u8((volatile u8 *) R_IR_WAVEFILT_EN, channel);
}

inline static void irda_jazz_wave_mask_set(u8 n, u8 mask)
{
    u32 ptmp = 0;
    ptmp = R_IR_WAVEFILT_MASK(n);
    hal_put_u8((volatile u8 *) ptmp, mask);
}

inline static void irda_jazz_wave_mask_bit_set(u8 bit, u8 value)
{
    u8 position = 0;
    u8 field = 0, dtmp = 0;
    u32 ptmp = 0;

    position = bit / 8;
    field = bit % 8;

    ptmp = R_IR_WAVEFILT_MASK(position+1);
    dtmp = hal_get_u8((volatile u8 *) ptmp);
    dtmp &= ~(1 << field);
    dtmp |=(value << field);
    hal_put_u8((volatile u8 *) ptmp, dtmp);
}

inline static void irda_jazz_wave_len_set(u8 channel, u8 len)
{
   switch(channel)
    {
      case 1:
        hal_put_u8((volatile u8 *) R_IR_WAVEFILT_LEN0, len);
    break;
  case 2:
     hal_put_u8((volatile u8 *) R_IR_WAVEFILT_LEN1, len);
    break;
  case 3:
     hal_put_u8((volatile u8 *) R_IR_WAVEFILT_LEN2, len);
    break;
  case 4:
     hal_put_u8((volatile u8 *) R_IR_WAVEFILT_LEN3, len);
    break;
  default:
    break;
    }  
}

inline static void irda_jazz_wave_add_set(u8 channel, u8 len)
{
  switch(channel)
    {
    case 1:
        hal_put_u8((volatile u8 *) R_IR_WAVEFILT_STADR0, len);
    break;
  case 2:
     hal_put_u8((volatile u8 *) R_IR_WAVEFILT_STADR1, len);
    break;
  case 3:
     hal_put_u8((volatile u8 *)R_IR_WAVEFILT_STADR2, len);
    break;
  case 4:
     hal_put_u8((volatile u8 *) R_IR_WAVEFILT_STADR3, len);
    break;
  default:
    break;
    } 
}

 static void irda_jazz_wave_wfn_set(u8 n, u32 ontime_l, u32 ontime_h, u32 period_l, u32 period_h)
{
    u32 ptmp = 0;
    u8 dtmp = 0;

    ptmp = R_IR_WFN_ONTIME_LL(n); 
    dtmp = ontime_l & 0xff;
    hal_put_u8((volatile u8 *) ptmp, dtmp);

    ptmp = R_IR_WFN_ONTIME_LH(n); 
    dtmp = (ontime_l >> 8) & 0x0f;
    hal_put_u8((volatile u8 *) ptmp, dtmp);
    
    ptmp = R_IR_WFN_ONTIME_HL(n); 
    dtmp = ontime_h & 0xff;
    hal_put_u8((volatile u8 *) ptmp, dtmp);
    
    ptmp = R_IR_WFN_ONTIME_HH(n); 
    dtmp = (ontime_h >> 8) & 0x0f;
    hal_put_u8((volatile u8 *) ptmp, dtmp);

    ptmp = R_IR_WFN_PERIOD_LL(n); 
    dtmp = period_l & 0xff;
    hal_put_u8((volatile u8 *) ptmp, dtmp);

    ptmp = R_IR_WFN_PERIOD_LH(n); 
    dtmp = (period_l >> 8) & 0x0f;
    hal_put_u8((volatile u8 *) ptmp, dtmp);


    ptmp = R_IR_WFN_PERIOD_HL(n); 
    dtmp = period_h & 0xff;
    hal_put_u8((volatile u8 *) ptmp, dtmp);
    
    ptmp = R_IR_WFN_PERIOD_HH(n); 
    dtmp = (period_h >> 8) & 0x0f;
    hal_put_u8((volatile u8 *) ptmp, dtmp);
}

inline static void irda_jazz_switch_init(void)
{
    u32 dtmp = 0;
  dtmp = hal_get_u32((volatile u32 *)0x70000030);
  dtmp = dtmp & (~(0x3<<16));
  hal_put_u32((volatile u32 *)0x70000030,dtmp);

  dtmp = hal_get_u32((volatile u32 *)0x70000030);
  dtmp = dtmp & (~(0x1<<19));
  hal_put_u32((volatile u32 *)0x70000030,dtmp);

  dtmp = hal_get_u32((volatile u32 *)0x70000030);
  dtmp = dtmp | (0x1<<19);
  hal_put_u32((volatile u32 *)0x70000030,dtmp);
}

 static void irda_jazz_idle_set(u8 idle)
  {
      u8 dtmp = 0;
      
      if(idle)
      {    
          dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CTRL);
          dtmp |= 0x4;
          hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, dtmp);
          
      }else
      {  
          dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CTRL);
          dtmp &= 0xfb; 
          hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, dtmp);
      }          
  }

 static void irda_jazz_sample_clk_set(u8 sample)
   {
       u8  dtmp = 0;
       
       switch(sample)
       {
           case 0:
             dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
             dtmp &= 0x3f; 
             hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, dtmp);

            hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, 0x9d);      
           break;
         
       case 1: 
           dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
           dtmp &= 0x3f; 
           dtmp |= 0x40; 
           hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, dtmp);

           hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, 0x4e);      
           break;
         
       case 2:
           dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
           dtmp &= 0x3f; 
           dtmp |= 0x80; 
           hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, dtmp);

           hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, 0xd3);      
           break;
         
       case 3:
           dtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
           dtmp &= 0x3f; 
           dtmp |= 0xc0; 
           hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, dtmp);

           hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, 0x1a);  
         
       default:
           break;
       }
       
   }


static void irda_jazz_int_handler(void)
{
  u8 nec_st = 0;
  u8 wav_st = 0;
  u8 code = 0;
  u16 irda_code = 0;
  u16 usr_code = 0;
  u8 ucl = 0, uch = 0, i = 0, irh = 0;
  
  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
  irda_priv_t *p_irda = (irda_priv_t *)p_lld->p_irda;
  nec_st = hal_get_u8((volatile u8 *) R_IR_NEC_STA);
  wav_st = hal_get_u8((volatile u8 *) R_IR_WAVEFILT_STA);

  p_irda->irda_flag = 1;
  
  if((nec_st >> 7) & 0x1) /*buffer overflow*/
  {
    irda_jazz_soft_reset();
    return;
  }

  if(wav_st& 0xf) /*waveform pass*/
  {
    hal_put_u8((volatile u8 *) R_IR_WAVEFILT_STACLR, 0x1); //clear state of wave filter
   mtos_printk("fly:wflit interrupt: 0x%x\n",wav_st);
  }

  if(p_irda->irda_protocol == IRDA_NEC)  /*decoding by hardware*/
  {
      if((nec_st) >> 1 & 0x1) /*key down*/
      {
          code = hal_get_u8((volatile u8 *)R_IR_NEC_KEY);
          ucl = hal_get_u8((volatile u8 *) R_IR_NEC_USERL);
          uch = hal_get_u8((volatile u8 *) R_IR_NEC_USERH);
          usr_code = ucl | (uch << 8);
      }
      else if((nec_st) >> 2 & 0x1) /*key up*/
      {
        return;
      }
      else /*unknown*/
      {
        return;
      }
  }
  else
  {
        u16 ontime = 0, period = 0;
        u8 pulse_num = 0;
        struct pulsedata one_pulse;
        pulse_num = get_pulse_num();
        while(pulse_num --)
        {
            get_ontime_and_period(&ontime, &period);
            one_pulse.ontime = ontime;
            one_pulse.period = period;
            put_pulsefifo(&one_pulse);
            mtos_sem_give(&recvpulse_sem);

            //OS_PRINTF("ontime=%d(%d us), period=%d(%d us)\n", 
               // ontime, clock_to_us(ontime), period, clock_to_us(period));
        }
        return;
        
  }

  if(TRUE == p_irda->user_code_set)
  {

    if (p_irda->print_user_code)
    {
      OS_PRINTF("\nReceive irda data code %d", code);
      OS_PRINTF("   user code high: %d, user code low: %d\n",
                  ((usr_code & 0xFF00) >> 8), (usr_code & 0xFF));
    }

    for(i = 0; i < IRDA_MAX_USER; i++)
    {
      if(p_irda->sw_user_code[i] == usr_code)
      {
        break;
      }
    }
    if(i >= IRDA_MAX_USER)
    {
      return;
    }
  }
  if(nec_st & 0x1)
  {
    if(FALSE == uio_check_rpt_key(UIO_IRDA, code, i))
    {
      return;
    }
  }
  irh = UIO_IRDA | ((i) << UIO_USR_CODE_SHIFT);
  irda_code = code | (irh << 8);
  if(nec_st & 0x1)
  {
     irda_code |= (1 << 15);
  }
  OS_PRINTF("   user code high: %08x, user code low: %08x\n",
              ((usr_code & 0xFF00) >> 8), (usr_code & 0xFF));
  OS_PRINTF("   irda code: %08x, %x",code, irda_code);
  mtos_fifo_put(&p_priv->fifo, irda_code);
  
}


static void irda_jazz_set_rpt_speed(u32 speed_ms)
{
  u8 rtmp = 0;
  u32 dtmp = 0;
  /*
   * Interval approximately is 110ms between repeat and next repeat.
   * Jazz's irda controller supports a maximum of do once sampling while 15 repeat codes have passed.
   * */
  dtmp = (u32)((15 * speed_ms) / 1650);
  dtmp = dtmp > 15 ? 15 : (u8)dtmp;

  rtmp = hal_get_u8((volatile u8 *) R_IR_NEC_CONFIG);
  rtmp |= (u8)dtmp;
  hal_put_u8((volatile u8 *) R_IR_NEC_CONFIG, rtmp);
}

static RET_CODE irda_jazz_set_hw_usercode(u32 en, u32 nuserCode)
{
  u8 rtmp = 0;

  if(FALSE == en)
  {
    //disable usercode mask, all IR signal will be received
   rtmp = hal_get_u8((volatile u8 *) R_IR_NEC_CONFIG);
   rtmp &= ~(1 << 7);
    hal_put_u8((volatile u8 *)R_IR_NEC_CONFIG, rtmp);
    return SUCCESS;
  }

  //filter the specific user code
  rtmp = nuserCode & 0xff;
  hal_put_u8((volatile u8 *)R_IR_NEC_USERL_SET, rtmp);
  rtmp = (nuserCode >> 8) & 0xff;
  hal_put_u8((volatile u8 *)R_IR_NEC_USERH_SET, rtmp);
  rtmp = hal_get_u8((volatile u8 *)R_IR_NEC_CONFIG);
  rtmp |= (1 << 7);
  hal_put_u8((volatile u8 *)R_IR_NEC_CONFIG, rtmp);
  return SUCCESS;
}


static RET_CODE irda_jazz_get_usercode(lld_uio_t *p_lld, u16 *p_usercode)
{
  u8 dh = 0, dl = 0;
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;

  if(p_usercode == NULL)
  {
    return ERR_PARAM;
  }

  if(p_ir->irda_protocol != IRDA_NEC)
  {
    return ERR_NOFEATURE;
  }
  dh = hal_get_u8((volatile u8 *)R_IR_NEC_USERH);
  dl = hal_get_u8((volatile u8 *)R_IR_NEC_USERL);

  *p_usercode = (dh << 8) | dl;
  return SUCCESS;
}

static RET_CODE irda_jazz_set_wfilt(irda_priv_t *p_ir)
{
  u8 i = 0;
  u8 j = 0;
  u8 channel_num = 0;
  u8 wfilt_add_len = 0;
  u8 channel_len = 0;
  u32 code = 0;
  u32 code_temp = 0;
  u32 dtmp = 0;
  u32 mask_code = 0;
  u32 mask_ctmp = 0;
  u32 mask_dtmp = 0;
  
  OS_PRINTF("fly:enter wflit\n");
  channel_num = p_ir->irda_wfilt_channel;
  irda_jazz_switch_init();  
  irda_jazz_soft_reset();
  irda_jazz_idle_set(1); 
  irda_jazz_sample_clk_set(2);

  switch(channel_num)
  {
      case 1:
           irda_jazz_wave_channel_set(0x01);
           break;
      case 2:
           irda_jazz_wave_channel_set(0x03);
           break;
      case 3:
           irda_jazz_wave_channel_set(0x07);
           break;
      case 4:
           irda_jazz_wave_channel_set(0x0f);
           break;
      default : 
           return ERR_PARAM;
  }

//   for(i = 1;i < 17; i++) 
//     irda_jazz_wave_mask_set(i, 0xff);

  for(i = 0; i < channel_num; i++)
  {
       channel_len = p_ir->irda_wfilt_channel_cfg[i].addr_len;
       irda_jazz_wave_len_set(i + 1, channel_len - 1);
       irda_jazz_wave_add_set(i + 1, wfilt_add_len);
       if(p_ir->irda_wfilt_channel_cfg[i].protocol == IRDA_NEC)
       {
/*      irda_jazz_wave_wfn_set(wfilt_add_len, 1434, 1940, 2151, 2910);
          irda_jazz_wave_mask_bit_set(wfilt_add_len, 0);  
          wfilt_add_len ++;*/
          code_temp = p_ir->irda_wfilt_channel_cfg[i].wfilt_code;
          mask_ctmp = p_ir->irda_wfilt_channel_cfg[i].wfilt_mask;
          dtmp = code_temp >> 16;
          mask_dtmp = mask_ctmp >> 16;
          code = 0;
          mask_code = 0;
          for(j = 0; j < 16; j++)
          {
            code |= dtmp & 0x01;
            code <<= 1;
            dtmp >>= 1;
            mask_code |= mask_dtmp & 0x01;
            mask_code <<= 1;
            mask_dtmp >>= 1;
          }
          dtmp = code_temp >> 8;
          mask_dtmp = mask_ctmp >> 8;
          for(j = 0; j < 8; j++)
          {
            code |= dtmp & 0x01;
            code <<= 1;
            dtmp >>= 1;
            mask_code |= mask_dtmp & 0x01;
            mask_code <<= 1;
            mask_dtmp >>= 1;
          }
          for(j = 0; j < 7; j++)
          {
            code |= code_temp & 0x01;
            code <<= 1;
            code_temp >>= 1;
            mask_code |= mask_ctmp & 0x01;
            mask_code <<= 1;
            mask_ctmp >>= 1;
          }
          code |= code_temp & 0x01;
          mask_code |= mask_ctmp & 0x01;
          for(j = 0; j < channel_len; j++)
          {
            if(mask_code == 0)
                irda_jazz_wave_mask_bit_set(j + wfilt_add_len, 1);
            else
            {
                if((mask_code << j) & 0x80000000)
                    irda_jazz_wave_mask_bit_set(j + wfilt_add_len, 1);
                else
                    irda_jazz_wave_mask_bit_set(j + wfilt_add_len, 0);
            }
            if((code << j) & 0x80000000)
            {
                irda_jazz_wave_wfn_set(j + wfilt_add_len, 89, 120, 358, 485);
            }
            else
            {
                irda_jazz_wave_wfn_set(j + wfilt_add_len, 89, 120, 179, 242);
            }
          }
       wfilt_add_len += channel_len;  
       }
      OS_PRINTF("fly:channel len: %d, wflit_add_len: %d\n", channel_len, wfilt_add_len); 
      OS_PRINTF("fly:wfilt code:0x%x\n", code); 
  }
  return SUCCESS;
}



/*****************************************************************
* IRDA APIs
*****************************************************************/
static RET_CODE irda_jazz_io_ctrl(lld_uio_t *p_lld,u32 cmd, u32 param)
{
  RET_CODE ret = SUCCESS;
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;
  int i = 0;

  switch(cmd)
  {
    case UIO_IR_SET_HW_USERCODE:
      ret = irda_jazz_set_hw_usercode(param >> 16, param & 0xffff);
      break;
    case UIO_IR_GET_USERCODE:
      ret = irda_jazz_get_usercode(p_lld, (u16 *)param);
      break;
    case UIO_IR_DISABLE_SW_USERCODE:
      {
        p_ir->user_code_set = FALSE;
        for(i = 0; i < IRDA_MAX_USER; i ++)
        {
          p_ir->sw_user_code[i] = 0x0;
        }
      }
      break;
    case UIO_IR_PRINT_SW_USERCODE:
      if (param)
      {
        p_ir->print_user_code = TRUE;
      }
      else
      {
        p_ir->print_user_code = FALSE;
      }
      break;
    case UIO_IR_SET_WAVEFILT:
       ret = irda_jazz_set_wfilt(p_ir);
      break;
    case UIO_IR_CHECK_KEY:
    if(p_ir->irda_flag)
    { 
      ret = SUCCESS;//irda key pressed
      *((u32 *)param) = p_ir->irda_flag;
    }
    else
    { 
      ret = ERR_FAILURE;//no key pressed
      *((u32 *)param) = p_ir->irda_flag;
    }
    break;
    case UIO_IR_CLEAR_KEY_FLAG:
    p_ir->irda_flag = 0;//set the key flag 0
    break;
    default:
      ret = ERR_NOFEATURE;
      break;
  }

  return ret;
}


static RET_CODE irda_jazz_set_sw_usercode(lld_uio_t *p_lld,u8 num, u16 *p_array)
{
  u8 i = 0;
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;

  if(num > IRDA_MAX_USER)
  {
    return ERR_PARAM;
  }
  for(i = 0; i < num; i++)
  {
    p_ir->sw_user_code[i] = p_array[i];
  }
  p_ir->user_code_set = TRUE;
  return SUCCESS;
}
/*
Irda soft decoding initialization
*/
#if 1
static void irda_jazz_swdec_init(lld_uio_t *p_lld)
{
  //irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;
  u8 rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CTRL);
  rtmp |= (1 << 5); //interrupt of key down enable in soft protocol mode
  rtmp |= (1 << 2);//high level/low level while irda is idle
  rtmp |= (1 << 1);////decoding by software
  hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, rtmp);

  rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
    rtmp |= (0x2 << 6); // sample clock, default=187.5KHz
    rtmp &= ~0x1f; // interrupt freq
    hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, rtmp);
//    irda_jazz_set_rpt_speed(IRDA_RPT_TIME_MS);// repeat sample mode

    rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_OTSET);
    rtmp = sample_frq2timeout[2]; //time of sample timeout
    hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, rtmp);

}
#endif

/*
Irda hardware decoding initialization
Irda controller only supports NEC protocol
*/
static void irda_jazz_hwdec_init(lld_uio_t *p_lld)
{
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;
  u8 rtmp = hal_get_u8((volatile u8 *) R_IR_NEC_CONFIG);
    rtmp &= ~(1 << 7); //usercode filter enable
    rtmp &= ~(1 << 6); //keycode filter enable
    hal_put_u8((volatile u8 *) R_IR_NEC_CONFIG, rtmp);
    if(p_ir->irda_repeat_time == 0)
      irda_jazz_set_rpt_speed(IRDA_RPT_TIME_MS);// repeat sample mode
    else
      irda_jazz_set_rpt_speed(p_ir->irda_repeat_time);// repeat sample mode

    rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CTRL);
    rtmp &= ~(1 << 7); //handling for repeat code timeout
    rtmp |= (1 << 6);//check keycode by hardware
    rtmp &= ~(1 << 4); //interrupt of key up enable
    rtmp |= (1 << 3); //interrupt of key down enable
    rtmp |= (1 << 2); //high level/low level while irda is idle
    rtmp &= ~(1 << 1); //decoding by hardware
    hal_put_u8((volatile u8 *) R_IR_GLOBAL_CTRL, rtmp);

    rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_CFG);
    rtmp |= (0x2 << 6); // sample clock, default=187.5KHz
    hal_put_u8((volatile u8 *) R_IR_GLOBAL_CFG, rtmp);

    rtmp = hal_get_u8((volatile u8 *) R_IR_GLOBAL_OTSET);
    rtmp = sample_frq2timeout[2]; //time of sample timeout
    hal_put_u8((volatile u8 *) R_IR_GLOBAL_OTSET, rtmp);

}

static RET_CODE irda_jazz_open(lld_uio_t *p_lld, irda_cfg_t *p_cfg)
{

  irda_priv_t *p_ir = NULL;
  u32 *p_stack = NULL;

  if(NULL != p_lld->p_irda)
  {
    return SUCCESS;
  }

  p_lld->p_irda = (irda_priv_t *)mtos_malloc(sizeof(irda_priv_t));
  MT_ASSERT(NULL != p_lld->p_irda);
  memset(p_lld->p_irda, 0, sizeof(irda_priv_t));
  p_ir = p_lld->p_irda;
  p_ir->irda_protocol = p_cfg->protocol;
  p_ir->irda_repeat_time = p_cfg->irda_repeat_time;
  p_ir->irda_wfilt_channel = p_cfg->irda_wfilt_channel;
  p_ir->irda_wfilt_channel_cfg[0] = p_cfg->irda_wfilt_channel_cfg[0];
  p_ir->irda_wfilt_channel_cfg[1] = p_cfg->irda_wfilt_channel_cfg[1];
  p_ir->irda_wfilt_channel_cfg[2] = p_cfg->irda_wfilt_channel_cfg[2];
  p_ir->irda_wfilt_channel_cfg[3] = p_cfg->irda_wfilt_channel_cfg[3];

  irda_jazz_soft_reset();
#if 1
  if(p_ir->irda_protocol != IRDA_NEC)
  {
    p_stack = (u32 *)mtos_malloc(p_cfg->stack_size);
    if(p_stack == NULL)
        return ERR_FAILURE;
    flush_pulse_fifo();
    mtos_sem_create(&recvpulse_sem, FALSE);
    OS_PRINTF("task prio:%d, stack size:%d\n", p_cfg->task_prio, p_cfg->stack_size);
    mtos_task_create((u8 *)"nec decocer", nec_decoder_monitor, NULL, 
                            p_cfg->task_prio, p_stack, p_cfg->stack_size);
    irda_jazz_swdec_init(p_lld);
  }
  else
  {
    irda_jazz_hwdec_init(p_lld);
  }
#else
  if(p_ir->irda_protocol == IRDA_NEC)
  {
    irda_jazz_hwdec_init(p_lld);
  }
  else
  {
    OS_PRINTF("%s %d %s\n", __FUNCTION__,__LINE__,__FILE__);
    return ERR_FAILURE;
  }
#endif
  OS_PRINTF("irda open\n");
  mtos_irq_request(IRQ_IRDA_ID, irda_jazz_int_handler,IRQ_INT_TYPE_NORMAL);
//  irda_jazz_io_ctrl(p_lld, UIO_IR_SET_WAVEFILT, 0);
  return SUCCESS;
}


static RET_CODE irda_jazz_stop(lld_uio_t *p_lld)
{
#if 1
  mtos_irq_release(IRQ_IRDA_ID, irda_jazz_int_handler);
  if(NULL != p_lld->p_irda)
  {
    mtos_free(p_lld->p_irda);
    p_lld->p_irda = NULL;
  }
  #endif
  return SUCCESS;
}

void irda_attach(lld_uio_t *p_lld)
{
  p_lld->irda_open = irda_jazz_open;
  p_lld->irda_stop = irda_jazz_stop;
  p_lld->irda_io_ctrl = irda_jazz_io_ctrl;
  p_lld->set_sw_usrcode = irda_jazz_set_sw_usercode;
}
