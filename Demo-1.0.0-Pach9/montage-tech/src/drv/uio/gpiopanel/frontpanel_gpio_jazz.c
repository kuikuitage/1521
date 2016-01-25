/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "mtos_misc.h"
#include "mtos_event.h"
#include "mtos_fifo.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "drv_dev.h"
#include "bus/spi.h"
#include "hal_timer.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "uio.h"
#include "../uio_priv.h"
#include "../../drvbase/drv_svc.h"
#include "../../drvbase/drv_dev_priv.h"

#define FP_INT_MASK             0x3FFF
#define FP_SYNC_INT_TIME_START  0x01

typedef struct fp_info_s
{
    os_sem_t sem;
    u32 bitmap_size;
    os_fifo_t *p_fifo;
    const led_bitmap_t *p_bitmap;
    u8 led_buff[8];
    pan_hw_info_t *p_fp_hw_info;
    s32 timer_id;
    drvsvc_handle_t *p_fp_svc;
    u32 scan_intv;
    u32 rept_intv;
    u32 rept_cnt;
    u32 last_key;
    u8 led_len;
    u8 fp_key_num;
    u32 led_lum;
    u8 mp_dot;
    pan_gpio_info_t led_lock_buff[4];
    u8 led_lock_flag[4];
    u8 led_display_mode;
    u8 fp_type;
    u8 led_lock;

    u32 *p_stack; 
    mtos_evt_t m_sync_evt;
    drvsvc_handle_t *p_svc;
} fp_info_t;

static fp_info_t fpinfo;
s32 timer_id = 0;


static const led_bitmap_t fp_bitmap[] =
{
  {'.', 0x80},  
  {'0', 0x3F},  {'1', 0x06},  {'2', 0x5B},  {'3', 0x4F},  
  {'4', 0x66},  {'5', 0x6D},  {'6', 0x7D},  {'7', 0x07},  
  {'8', 0x7F},  {'9', 0x6F},  {'a', 0x77},  {'A', 0x77},  
  {'b', 0x7C},  {'B', 0x7C},  {'c', 0x39},  {'C', 0x39},  
  {'d', 0x5E},  {'D', 0x5E},  {'e', 0x79},  {'E', 0x79},  
  {'f', 0x71},   {'F', 0x71},  {'g', 0x6F},  {'G', 0x3D},  
  {'h', 0x76},  {'H', 0x76},  {'i', 0x04},   {'I', 0x30},  
  {'j', 0x0E},   {'J', 0x0E},  {'l', 0x38},    {'L', 0x38},  
  {'n', 0x54},  {'N', 0x37},  {'o', 0x5C},  {'O', 0x3F},  
  {'p', 0x73},  {'P', 0x73},  {'q', 0x67},  {'Q', 0x67},  
  {'r', 0x50},   {'R', 0x77},  {'s', 0x6D},  {'S', 0x6D},  
  {'t', 0x78},   {'T', 0x31},  {'u', 0x3E},  {'U', 0x3E},  
  {'y', 0x6E},  {'Y', 0x6E},  {'z', 0x5B},   {'Z', 0x5B},  
  {':', 0x80},   {'-', 0x40},  {'_', 0x08},  {' ', 0x00},    
};

#define FP_TABLE_SIZE sizeof(fp_bitmap) / sizeof(led_bitmap_t)
#define DELAY_US  10

extern BOOL uio_check_rpt_key(uio_type_t uio, u8 code, u8 ir_index);

static u8 index_bitmap(u8 ch,  const led_bitmap_t * p_fp_bitmap, u16 bitmap_tbl_size)
{
  u32 i = 0;
  for(;i < bitmap_tbl_size; i ++)
  {
    if(p_fp_bitmap[i].ch == ch)
    {
      return p_fp_bitmap[i].bitmap;
    }
  }
  return 0;
}

static void display(u8 ch, u8 led_sel)
{
  u32 cmd = 0;
  u32 i = 0;
  u8 bit = 0;
  cmd = ~(0xff << 10);
  cmd |= ch << 10;
  cmd &= ~(1 << led_sel);

  for(;i < 18; i ++)
  {
    bit = (cmd >> i) & 0x01;
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, bit);
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 1);
  }
//latch
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);

//output
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);

}

static u32 key_scan(u8 key_sel)
{
  u32 cmd = 0;
  u32 i = 0;

  u8 bit = 0;
  cmd = ~(0xff << 10);
  cmd |= (1 << key_sel) << 10;
//  OS_PRINTF("ledkb cmd=0x%04x\n", cmd);
  for(;i < 18; i ++)
  {
    bit = (cmd >> i) & 0x01;
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, bit);
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 1);
  }
  //latch
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);
//output
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);
    gpio_get_value(fpinfo.p_fp_hw_info->scan[0].pos, &bit);
    if(bit == 1)
    {
        return key_sel;
    }
    else
    {
       return 0xff;
    }
  
}

static u32 key_scan2(u8 key_sel)
{
  u32 cmd = 0;
  u32 i = 0;

  u8 bit = 0;
  u8 bit2 = 0;
  u8 key = 0;
  cmd = ~(0xff << 10);
  cmd = 0xff;
  cmd &= ~(1 << key_sel);
//  OS_PRINTF("ledkb cmd=0x%04x\n", cmd);
  for(;i < 18; i ++)
  {
    bit = (cmd >> i) & 0x01;
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, bit);
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 1);
  }
  //latch
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);
//output
    gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 0);
    gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos, 1);
    gpio_get_value(fpinfo.p_fp_hw_info->scan[0].pos, &bit);
    gpio_get_value(fpinfo.p_fp_hw_info->scan[1].pos, &bit2);

    if(bit == 0 || bit2 == 0)
    {
        if(bit == 0)
        {
            key |= 1 << key_sel;
        }
        if(bit2 == 0)
        {
            key |=  1 << (key_sel + 4);
        }
        return key;
    }
    else
    {
       return 0xff;
    }
}

static u8 key_scan_gpiokey(void)
{
    u8 val = 1;
    u8 i = 0;
    
    for(i = 0; i < fpinfo.fp_key_num; i++)
    {
        gpio_get_value(fpinfo.p_fp_hw_info->com[i].pos, &val);
        if(!val)
            return i;
    }
    return 0xff;
}

void fp_time_isr_jazz(void)
{
   fp_info_t *p_priv = &fpinfo;
  u32 int_sta = FP_SYNC_INT_TIME_START;
  /* trigger driver service to handle this event */
  drvsvc_event_set(p_priv->p_svc, int_sta);
}

static void scan_callback(void)
{
    u8 i = 0;
    u8 j = 0;
    u8 ch = 0;
    u32 key = 0;
    u8 key_sel = 0;

    key = 0;

    if(fpinfo.led_display_mode == LED_DISPLAY_REVERSE)
    {
      for(i = fpinfo.led_len; i > 0; i --)
      {
       // ch = index_bitmap(fpinfo.led_buff[i], fpinfo.p_bitmap, fpinfo.bitmap_size);
        ch = fpinfo.led_buff[i - 1];
        display(ch, (fpinfo.led_len - i));
        mtos_task_delay_us(fpinfo.led_lum);
      }
    }
    else
    {
      if(fpinfo.fp_type == FP_TYPE_TS3189)
      {
        if(fpinfo.led_lock == 1)
        {
          display(0, 0);
          mtos_task_delay_us(fpinfo.led_lum);
        }
        for(i = 0; i < fpinfo.led_len; i ++)
        {
           ch = fpinfo.led_buff[i];
           display(ch, i + 1);
           mtos_task_delay_us(fpinfo.led_lum);
         }
      }
      else
      {
        for(i = 0; i < fpinfo.led_len; i ++)
        {
            // ch = index_bitmap(fpinfo.led_buff[i], fpinfo.p_bitmap, fpinfo.bitmap_size);
            ch = fpinfo.led_buff[i];
            display(ch, i);
            mtos_task_delay_us(fpinfo.led_lum);
         }
      }
    }

    for(j = 0; j < fpinfo.fp_key_num; j ++)
    {
        if(fpinfo.fp_type == FP_TYPE_CT1642_2)
        {
            key_sel = key_scan2(j);
            if(key_sel != 0xff)
            {
              key |= key_sel;
            }
        }
        else
        {
            key_sel = key_scan(j);
            if(key_sel != 0xff)
            {
              key |= (1 << key_sel);
            }
        }
    }

    if(key != 0)
    {
      if(fpinfo.last_key == key)
      {
        if((! fpinfo.rept_cnt)&& (TRUE == uio_check_rpt_key(UIO_FRONTPANEL, (u8)key, 0)))
        {
          fpinfo.rept_cnt = fpinfo.rept_intv;
          OS_PRINTF("keycode1 = 0x%02x\n", key);
          mtos_fifo_put(fpinfo.p_fifo , key | (UIO_FRONTPANEL << 8));
        }
        fpinfo.rept_cnt --;
      }
      else
      {
        OS_PRINTF("keycode2 = 0x%02x\n", key);
        mtos_fifo_put(fpinfo.p_fifo, key | (UIO_FRONTPANEL << 8));
        fpinfo.last_key = key;
        fpinfo.rept_cnt = fpinfo.rept_intv;
      }  
    }
    else
    {
      fpinfo.last_key = 0xffffff00;
    }
 }

static void scan_callback_gpiokey()
{
    u16 key = 0;
    u8 key_sel = 0;

    key_sel = key_scan_gpiokey();
    if(key_sel != 0xff)
    {
        key |= (1 << key_sel);
    }

    if(key != 0)
    {
      if(fpinfo.last_key == key)
      {
        if((! fpinfo.rept_cnt)&& (TRUE == uio_check_rpt_key(UIO_FRONTPANEL, (u8)key, 0)))
        {
          fpinfo.rept_cnt = fpinfo.rept_intv;
          OS_PRINTF("keycode1 = 0x%02x\n", key);
          mtos_fifo_put(fpinfo.p_fifo , key | (UIO_FRONTPANEL << 8));
        }
        fpinfo.rept_cnt --;
      }
      else
      {
        OS_PRINTF("keycode2 = 0x%02x\n", key);
        mtos_fifo_put(fpinfo.p_fifo, key | (UIO_FRONTPANEL << 8));
        fpinfo.last_key = key;
        fpinfo.rept_cnt = fpinfo.rept_intv;
      }  
    }
    else
    {
      fpinfo.last_key = 0xffffff00;
    }
 }

static RET_CODE scan_fp_service(void *p_param, u32 event)
{
    u8 i = 0;
    u8 j = 0;
    u8 ch = 0;
    u32 key = 0;
    u8 key_sel = 0;

    key = 0;

    if(fpinfo.led_display_mode == LED_DISPLAY_REVERSE)
    {
      for(i = fpinfo.led_len; i > 0; i --)
      {
       // ch = index_bitmap(fpinfo.led_buff[i], fpinfo.p_bitmap, fpinfo.bitmap_size);
        ch = fpinfo.led_buff[i - 1];
        display(ch, (fpinfo.led_len - i));
        mtos_task_delay_us(fpinfo.led_lum);
      }
    }
    else
    {
      for(i = 0; i < fpinfo.led_len; i ++)
      {
     //   ch = index_bitmap(fpinfo.led_buff[i], fpinfo.p_bitmap, fpinfo.bitmap_size);
        ch = fpinfo.led_buff[i];
        display(ch, i);
        mtos_task_delay_us(fpinfo.led_lum);
      }
    }
  
  
    for(j = 0; j < fpinfo.fp_key_num; j ++)
    {
        key_sel = key_scan(j);
        if(key_sel != 0xff)
        {
          key |= (1 << key_sel);
        }
    }

    if(key != 0)
    {
      if(fpinfo.last_key == key)
      {
        if((! fpinfo.rept_cnt)&& (TRUE == uio_check_rpt_key(UIO_FRONTPANEL, (u8)key, 0)))
        {
          fpinfo.rept_cnt = fpinfo.rept_intv;
          OS_PRINTF("keycode1 = 0x%02x\n", key);
          mtos_fifo_put(fpinfo.p_fifo , key | (UIO_FRONTPANEL << 8));
        }
        fpinfo.rept_cnt --;
      }
      else
      {
        OS_PRINTF("keycode2 = 0x%02x\n", key);
        mtos_fifo_put(fpinfo.p_fifo, key | (UIO_FRONTPANEL << 8));
        fpinfo.last_key = key;
        fpinfo.rept_cnt = fpinfo.rept_intv;
      }  
    }
    else
    {
      fpinfo.last_key = 0xffffff00;
    }
    return SUCCESS;
 }


/******************************************************
 *   External APIs
 ******************************************************/
static RET_CODE gpio_panel_jazz_display(lld_uio_t *p_lld,u8 *p_data, u32 len)
{

  u8 index = 0;
  u8 i = 0;
  MT_ASSERT(p_lld->p_fp != NULL);
  for(i = 0; i < len; i ++)
  {
    fpinfo.led_buff[i] = p_data[i];
    fpinfo.led_buff[i] = index_bitmap(fpinfo.led_buff[i], fpinfo.p_bitmap, fpinfo.bitmap_size);
    if(fpinfo.led_lock_flag[i] == 1)
    {
      fpinfo.led_buff[i] |= fpinfo.mp_dot;
    }
    
  }

  
  if(fpinfo.fp_type == FP_TYPE_TS3189)    
  {
   if(len == 1)
      {
        fpinfo.led_buff[2] = fpinfo.led_buff[0];
        fpinfo.led_buff[0] =  fpinfo.led_buff[1] = 0;
      }
      else if(len == 2)
      {
        index = fpinfo.led_buff[0];
        fpinfo.led_buff[2] = fpinfo.led_buff[1];
        fpinfo.led_buff[1] = index;
        fpinfo.led_buff[0]  = 0;
      }
      
      index = fpinfo.led_buff[2];
      fpinfo.led_buff[2] = fpinfo.led_buff[0];
      fpinfo.led_buff[0] = index;
      fpinfo.led_len = 3;
      
      return SUCCESS; 
    }

  fpinfo.led_len = len; 
  return SUCCESS;
}

#if 0
static void fp_timer_isr(viod)
{
  if(mtos_sem_give(&(fpinfo.sem)) != TRUE)
  {
      OS_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
  }
}
#endif

RET_CODE gpio_panel_jazz_open(lld_uio_t *p_lld, fp_cfg_t *p_cfg)
{
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
  u32 *p_stack = NULL;
  drvsvc_handle_t *p_fp_svc = NULL;
  u8 i = 0;

  MT_ASSERT(p_cfg != NULL);
  MT_ASSERT(p_cfg->p_info != NULL);

  if(p_lld->p_fp != NULL)
  {
      mtos_timer_delete(fpinfo.timer_id);
      mtos_sem_destroy(&(fpinfo.sem), 0);
  }
  memset(&fpinfo, 0, sizeof(fp_info_t));

  fpinfo.p_fp_hw_info = p_cfg->p_info;
  
  p_lld->p_fp = &fpinfo;

  fpinfo.scan_intv = p_cfg->p_info->com_scan_intv;
  fpinfo.rept_intv = p_cfg->p_info->repeat_intv;
  fpinfo.last_key = 0xffffff00;
  fpinfo.rept_cnt =  fpinfo.rept_intv;
  fpinfo.p_fifo = &(p_priv->fifo);
  fpinfo.led_display_mode = p_cfg->led_display_mode; 
  fpinfo.fp_type = p_cfg->fp_type;

  if(p_cfg->fp_led_lum == 0)
    fpinfo.led_lum = 500;
  else
    fpinfo.led_lum = p_cfg->fp_led_lum;

  if(p_cfg->fp_type == FP_TYPE_DEFAULT || p_cfg->fp_type == FP_TYPE_TS3189
    || p_cfg->fp_type == FP_TYPE_CT1642_2)
  {
  if(p_cfg->p_map == NULL)
  {
      fpinfo.p_bitmap = fp_bitmap;
      fpinfo.bitmap_size = FP_TABLE_SIZE;
      fpinfo.mp_dot = 0x80;
  }
  else
  {
      fpinfo.p_bitmap = p_cfg->p_map;
      fpinfo.bitmap_size = p_cfg->map_size;
      for(i = 0; i < fpinfo.bitmap_size; i++)
      {
        if('.' == fpinfo.p_bitmap[i].ch)
          fpinfo.mp_dot = fpinfo.p_bitmap[i].bitmap;
      }
  }
      
  gpio_ioctl(GPIO_CMD_IO_ENABLE, fpinfo.p_fp_hw_info->fdata.pos, TRUE);
  gpio_set_dir(fpinfo.p_fp_hw_info->fdata.pos, GPIO_DIR_OUTPUT);
  gpio_set_value(fpinfo.p_fp_hw_info->fdata.pos,0);
  gpio_ioctl(GPIO_CMD_IO_ENABLE,fpinfo.p_fp_hw_info->fclock.pos, TRUE);
  gpio_set_dir(fpinfo.p_fp_hw_info->fclock.pos, GPIO_DIR_OUTPUT);
  gpio_set_value(fpinfo.p_fp_hw_info->fclock.pos, 0);
  
  if(fpinfo.p_fp_hw_info->num_scan > 1)
  {
      gpio_ioctl(GPIO_CMD_IO_ENABLE,fpinfo.p_fp_hw_info->scan[0].pos, TRUE);
      gpio_ioctl(GPIO_CMD_IO_ENABLE,fpinfo.p_fp_hw_info->scan[1].pos, TRUE);
      gpio_set_dir(fpinfo.p_fp_hw_info->scan[0].pos, GPIO_DIR_INPUT);
      gpio_set_dir(fpinfo.p_fp_hw_info->scan[1].pos, GPIO_DIR_INPUT);
  }
  else
  {
      gpio_ioctl(GPIO_CMD_IO_ENABLE,fpinfo.p_fp_hw_info->scan[0].pos, TRUE);
      gpio_set_dir(fpinfo.p_fp_hw_info->scan[0].pos, GPIO_DIR_INPUT);
  }
  
  fpinfo.fp_key_num = 8;
  memset(fpinfo.led_buff, ' ', 4);
  fpinfo.led_len = 4;


   fpinfo.scan_intv = p_cfg->p_info->com_scan_intv;
   fpinfo.rept_intv = p_cfg->p_info->repeat_intv;
   fpinfo.last_key = 0xffffff00;
   fpinfo.rept_cnt =  fpinfo.rept_intv;
  fpinfo.p_fifo = &(p_priv->fifo);

  if(p_cfg->fp_led_lum == 0)
    fpinfo.led_lum = 500;
  else
    fpinfo.led_lum = p_cfg->fp_led_lum;

  /*
  timer_id = hal_timer_request(fpinfo.scan_intv ,
                                    scan_callback, TRUE);
*/
  if(p_cfg->fp_scan_mode ==  FP_SCAN_TASK)
{
   /* create driver service to handle the ISR */
  MT_ASSERT(0!= p_cfg->fp_op_pri);
  MT_ASSERT(0!= p_cfg->fp_op_stksize);
    p_stack = (u32 *)mtos_malloc(p_cfg->fp_op_stksize);
    MT_ASSERT(p_stack != NULL);
    fpinfo.p_stack = p_stack;
    p_fp_svc = drvsvc_create(p_cfg->fp_op_pri, p_stack, p_cfg->fp_op_stksize, 4);
    MT_ASSERT(NULL != p_fp_svc);
    fpinfo.p_svc = p_fp_svc;
    drvsvc_nod_insert(p_fp_svc, scan_fp_service,
      (void *)&fpinfo, 
      FP_INT_MASK | FP_SYNC_INT_TIME_START, 
      MTOS_WAIT_FOREVER);

    /* create a event flag to sync the operation */
    MT_ASSERT(TRUE == mtos_event_create(&fpinfo.m_sync_evt));
   timer_id = hal_timer_request(fpinfo.scan_intv,
                                     fp_time_isr_jazz, TRUE);

}

else
{
 timer_id = hal_timer_request(fpinfo.scan_intv,
                                     scan_callback, TRUE);
}
hal_timer_start(timer_id);
  }
  else if(p_cfg->fp_type == FP_TYPE_GPIOKEY)
  {
    fpinfo.fp_key_num = p_cfg->p_info->num_com;
    
    for(i = 0; i < fpinfo.fp_key_num; i++)
    {
        gpio_ioctl(GPIO_CMD_IO_ENABLE, fpinfo.p_fp_hw_info->com[i].pos, TRUE);
        gpio_set_dir(fpinfo.p_fp_hw_info->com[i].pos, fpinfo.p_fp_hw_info->com[i].io);
     //   gpio_set_value(fpinfo.p_fp_hw_info->com[i].pos, fpinfo.p_fp_hw_info->com[i].polar);
    }
    timer_id = hal_timer_request(fpinfo.scan_intv,
                                         scan_callback_gpiokey, TRUE);
    hal_timer_start(timer_id);
    
  }
  return SUCCESS;
}

     
static RET_CODE gpio_panel_jazz_stop(lld_uio_t *p_lld)
{
   hal_timer_stop(timer_id);
   return SUCCESS;
}


static RET_CODE gpio_panel_jazz_io_ctrl(lld_uio_t *p_lld,u32 cmd, u32 param)
  {
    RET_CODE ret = SUCCESS;
    u8 pos = 0;
    pan_gpio_info_t *p_info = fpinfo.p_fp_hw_info->lbd;

    switch(cmd)
    {
      case UIO_FP_SET_POWER_LBD:
        pos = p_info[FP_POWER_INDEX].pos;
        if (param == 1)
        {
           fpinfo.led_buff[pos]  |= fpinfo.mp_dot;
           fpinfo.led_lock_flag[pos]  = 1;
        }
        else
        {
          fpinfo.led_buff[pos] &= (~fpinfo.mp_dot);
          fpinfo.led_lock_flag[pos]  = 0;
        }
        break;
      case UIO_FP_SET_EMAIL_LBD:
        pos = p_info[FP_EMAIL_INDEX].pos;
        if (param == 1)
        {
           fpinfo.led_buff[pos]  |= fpinfo.mp_dot;
           fpinfo.led_lock_flag[pos]  = 1;
        }
        else
        {
          fpinfo.led_buff[pos] &= (~fpinfo.mp_dot);
          fpinfo.led_lock_flag[pos]  = 0;
        }
        break;
       case UIO_FP_SET_LOCK_LBD:
        if(fpinfo.fp_type == FP_TYPE_TS3189)
        {
            if (param == 1)
              fpinfo.led_lock = 1;
            else
              fpinfo.led_lock = 0;
        }
        else
        {
            pos = p_info[FP_LOCK_INDEX].pos;
            if (param == 1)
            {
                fpinfo.led_buff[pos]  |= fpinfo.mp_dot;
                fpinfo.led_lock_flag[pos]  = 1;                
            }
           else
           {
               fpinfo.led_buff[pos] &= (~fpinfo.mp_dot);
               fpinfo.led_lock_flag[pos]  = 0;               
           }
        }
        break;
      case UIO_FP_SET_IR_LBD:
        pos = p_info[FP_IR_INDEX].pos;
        if (param == 1)
        {
           fpinfo.led_buff[pos]  |= fpinfo.mp_dot;
           fpinfo.led_lock_flag[pos]  = 1;
        }
        else
        {
          fpinfo.led_buff[pos] &= (~fpinfo.mp_dot);
          fpinfo.led_lock_flag[pos]  = 0;
        }
        break;
     default:
       break;
    }
  
    return ret;
  }


void gpio_panel_jazz_attach(lld_uio_t *p_lld)
{
  p_lld->fp_open = gpio_panel_jazz_open;
  p_lld->fp_stop = gpio_panel_jazz_stop;
  p_lld->fp_io_ctrl = gpio_panel_jazz_io_ctrl;
  p_lld->display = gpio_panel_jazz_display;
}

