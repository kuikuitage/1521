/******************************************************************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "ap_framework.h"

//#ifdef TEST_FRAMEWORK
#include "testfm_define.h"
#include "testfm_inc.h"
//#endif

#define printk OS_PRINTK

/******************************************************************************
 * Parse Hex Data
 * param:  *s
 *         *number
 * return: bits
 ******************************************************************************/
static u8 ParseHexUL(const u8 *s, u32 *number)
{

   u8 bits  = 0;
   u32 val = 0;

   while (*s != '\0')
   {
    if ((*s >= '0') && (*s <= '9'))
    {
     val = val * 16 + *s++ - '0';
 }else if ((*s >= 'a') && (*s <= 'f'))
 {
     val = val * 16 + *s++ - 'a' + 10;
 }else
 {
  break;
 }

 if (++bits >= 8)
 {
  break;     // 8 Hex bits Max: u32 Para
 }
   }

   *number = val;

   return (bits);
}

/******************************************************************************
 * consvert string to int
 * param:  *str
 * return: number
 ******************************************************************************/
u32 str2int(const u8 *str)
{
 if (str == NULL)
 {
  return 0;
 }

    u8 * p = (u8*)str;
    u32 number;

    if ((p[0] == '0') && (p[1] == 'x'))
    {
        p += 2;
        ParseHexUL(p, &number);
    }else
    {
        number = (u32)atoi((char *)p);      //@@: Call Lib Function: atoi
    }

    return number;
}


/******************************************************************************
 * Parse Option Param: Format 0
 * param:  *s
 *         *mode
 *         *num
 * return: no
 ******************************************************************************/
static void ParseOption(const u8 *s, u32 *mode, u32 *num)
{
    *num  = 1;

    switch (s[0])
    {
        case 'b':
        *mode = 1;
       break;

        case 'h':
       *mode = 2;
      break;

        case 'w':
        *mode = 4;
       break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          break;

        default:
         *mode = 0;
     *num  = 0;
   break;
    }
}

/******************************************************************************
 * Parse Option Param: Format 1
 * param:  *s
 *         *mode
 *         *num
 * return: no
 ******************************************************************************/
static void ParseOption1(const u8 *str, u32 *mode, u32 *num)
{
    u8 step;

    *num  = 1;

    step = 0;

    while (*str)
    {
        switch (str[0])
        {
            case 'b':
                  if(step == 0)
                  {
                      *mode = 1;
                      step++;
                  }
           break;

            case 'h':
                  if (step == 0)
                  {
                      *mode = 2;
                      step++;
                  }
           break;

            case 'w':
                  if (step == 0)
                  {
                  *mode = 4;
                      step++;
                  }
           break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                  if (step >= 1)
                  {
                      *num = str2int( str );
                      return;
                  }
              break;

            default:
              break;
        }
        str++;
    }
}

/*****************************************************************************
 * Internal Shell Command: Clear Screen
 *****************************************************************************/
#if 0
static void clearscreen( s32 argc, u8 ** argv)
{
    s32 iloop = 0;
    while (iloop++ < 50)
    {
     printk("\r\n");
    }
}
#endif

/*****************************************************************************
 * Internal Shell Optional Command: read memory
 *****************************************************************************/
static void readmem( u32 argc, u8 **argv)
{
    u32   *ptr4;
    u16   *ptr2;
    u8    *ptr1;
    u32    mode = 0;
    u32    num;
    u32    address;

    if((argc == 1) || (argc > 3))
    {
     goto RMEXIT;
    }else if (argc == 2)
    {
     mode = 4;
    }else if (argc == 3)
    {
        if (argv[3][0] == '/')
        {
            ParseOption(&argv[3][1], &mode, &num);
        }
 else
            mode = str2int(argv[2]);
    }

    address = str2int(argv[1]);

    switch(mode)
    {
        case 1:
            ptr1 = (u8*)address;
            printk("\r\n\t[0x%08x] = 0x%02x\r\n", (u32)ptr1, *ptr1);
          return;

        case 2:
            ptr2 = (u16 *) (address&0xfffffffe);
            printk("\r\n\t[0x%08x] = 0x%04x\r\n", (u32)ptr2, *ptr2);
          return;

        case 4:
            ptr4 = (u32 *) (address&0xfffffffc);
            printk("\r\n\t[0x%08x] = 0x%08x\r\n", (u32)ptr4, *ptr4);
          return;

        default:
   break;
    }

RMEXIT:
    printk("\r\n Usage:   rm  addr  [size]");
    printk("\r\n          size -- 1 byte; 2 half word; 4 word\r\n");
}

/*****************************************************************************
 * Internal Shell Optional Command: read memory block
 *****************************************************************************/
static void readmemblock( u32 argc, u8 **argv)
{
    u32   *ptr4;
    u16   *ptr2;
    u8    *ptr1;
    u32    mode = 0;
    u32    num = 0;
    u32    address;
    u32    iloop;

    if (argc != 3)
    {
        goto RMBEXIT;
    }

    if (argv[2][0] == '/')
    {
        ParseOption1(&argv[2][1], &mode, &num);
        if (num > 10000)
        {
             printk("\r\n memory block is too big!!!, %d \r\n", num);
             num = 10000;
        }
    }else
    {
        mode = str2int(argv[2]);
    }

    address = str2int(argv[1]);
    iloop = 0;
    switch(mode)
    {
        case 1:
           ptr1 = (u8 *)address;
              ///printk("\r\n [0x%08x]: \r\n", (u32)ptr1);

       while(num--)
              {
                  if ((iloop%16) == 0)
                  {
                      printk("\r\n ");
                  }

                  printk("0x%02x ", *ptr1);
                  ptr1++;
                  iloop++;
              }
          return;

        case 2:
              ptr2 = (u16 *)(address&0xfffffffe);
              //printk("\r\n [0x%08x]: \r\n", (u16)ptr2);
              while(num--)
              {
                  if ((iloop%8) == 0)
                  {
                      printk("\r\n ");
                  }

                  printk("0x%04x  ", *ptr2);
                  ptr2++;
                  iloop++;
              }
          return;

        case 4:
              ptr4 = (u32 *)(address&0xfffffffc);
              printk("\r\n [0x%08x]: \r\n", (u32)ptr4);

              while(num--)
              {
                  if ((iloop%8) == 0)
                  {
                      printk("\r\n ");
                  }

                  printk("0x%08x   ", *ptr4);
                  ptr4++;
                  iloop++;
              }
          return;

        default:
          break;
    }

RMBEXIT:
    printk("\r\n Usage:   rmb  addr  [/mode]");
    printk("\r\n          mode      -- /bxxx byte");
    printk("\r\n                    -- /hxxx half word");
    printk("\r\n                    -- /wxxx 4 word\r\n");
}

/*****************************************************************************
 * Internal Shell Optional Command: write memory
 *****************************************************************************/
static void writemem( s32 argc, u8 **argv)
{
    u32   *ptr4;
    u16   *ptr2;
    u8    *ptr1;
    u32    mode = 0;
    u32    address;
    u32    val;

    if((argc <= 2) || (argc > 4))
    {
        goto WMEXIT;
    }else if (argc == 3)
    {
        mode = 4;
    }else if (argc == 4)
    {
        mode = str2int(argv[3]);
    }


    address = str2int(argv[1]);
    if (address <= 0x400000)
    {
       // printk("\r\n this address is risky, please certify it!\r\n");
       // return ;
    }

    val = str2int(argv[2]);

    switch(mode)
    {
        case 1:
            ptr1  = (u8*)address;
     *ptr1 = val;
            printk("\r\n\t[0x%08x] = 0x%02x\r\n", (u32)ptr1, *ptr1);
          return;

        case 2:
            ptr2  = (u16 *) (address&0xfffffffe);
     *ptr2 = val;
            printk("\r\n\t[0x%08x] = 0x%04x\r\n", (u32)ptr2, *ptr2);
          return;

        case 4:
            ptr4  = (u32 *) (address&0xfffffffc);
     *ptr4 = val;
            printk("\r\n\t[0x%08x] = 0x%08x\r\n", (u32)ptr4, *ptr4);
          return;

        default:
   break;
    }

WMEXIT:
    printk("\r\n Usage:   wm  addr  val [size]");
    printk("\r\n          val  --    0 ~~ 0xffffffff");
    printk("\r\n          size --    1 byte; 2 half word; 4 word\r\n");
}

typedef void (*FUNCP)(int argc, unsigned char ** argv);

void exec_cmd(void * pCmd)
{
  char tmp[32];
  int argc=1;
  char   *argv[64] = {NULL};
  char line[256];
  int len=0;
  FUNCP cmd=pCmd;
  int i;
  memset(line, 0,256);

  do
  {
    memset(tmp, 0x0, 32);
    testfm_console_get_parameter(tmp, ' ');
    i=strlen(tmp);
    if(i== 0)
    {
      break;
    }

  if( len+i+1>=256) break;

  strcpy(line+len,tmp);
  argv[argc++]=line+len;
  len+=strlen(tmp)+1;

  }while(1);

  cmd(argc,(unsigned char **)argv);

}

static s32 cmd_mem_init(void)
{
  return 0;
}

static s32 cmd_mem_deinit(void)
{
  return 0;
}

static void cmd_rm(void)
{
  exec_cmd(readmem);
  return;
}
//extern void nim_m88dc2800_test_chip_id(u32 para);
static void chip_id( u32 argc, u8 **argv)
{
    u32    num;

    if((argc == 1) || (argc > 3))
    {
     goto RMEXIT;
    }

    num = str2int(argv[1]);
    if (num >= 0 && num < 3)
    {
   //nim_m88dc2800_test_chip_id(num);
   return;
    }

RMEXIT:
 printk("\r\n Function: change chip_id");
 printk("\r\n Usage:  chip_id  val");
 printk("\r\n val --- 0: new DC2800  1: new Jazz   2: old DC2800\r\n");
}



static void cmd_chipid(void)
{
  exec_cmd(chip_id);
  return;
}

static void cmd_chipid_help(void)
{
    printk("\r\n Function: change chip_id");
    printk("\r\n Usage:   chip_id  val");
    printk("\r\n val -- 0: new DC2800  1: new Jazz   2: old DC2800\r\n");
}

extern void dc2800_dmd_reg_write(u8 addr, u8 reg, u8 data);
extern u8 dc2800_dmd_reg_read(u8 addr, u8 reg);
extern void dc2800_tn_reg_write(u8 addr, u8 reg, u8 data);
extern u8 dc2800_tn_reg_read(u8 addr, u8 reg);
extern s32 dc2800_tn_std_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
extern s32 dc2800_tn_seq_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
extern s32 dc2800_dmd_std_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
extern s32 dc2800_dmd_seq_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
extern s32 dc2800_tn_seq_write(u8 addr, u8 *p_buf, u32 len);
extern s32 dc2800_dmd_seq_write(u8 addr, u8 *p_buf, u32 len);



static void td_debug( u32 argc, u8 **argv)
{
    u32 wlen = 0;
    u32 rlen = 0;
    u8 address = 0;
    u8 value = 0;
    u8 data = 0;
    u8 reg = 0;
    u8 mode = 0;
    u8 g_buf[256] = {0};
    u8 i =0;
    RET_CODE ret = ERR_FAILURE;

    mode = str2int(argv[2]);
    address = str2int(argv[4]);


    if(argc == 1)
    {
     goto RMEXIT;
    }

    if(argv[1][0] == 'r')
    {
      switch(mode)
       {
        case 0:
          reg = str2int(argv[5]);
          break;
        case 1:
        case 2:
          rlen = str2int(argv[5]);
          wlen = str2int(argv[6]);
          for(i = 0; i < wlen; i++)
         {
           g_buf[i] = str2int(argv[7+i]);
         }
         break;
        default:
          printk("\r\n wrong write mode, should be 0 or 1 or 2");
          return;
       }

      switch(argv[3][0])
      {
        case 't':
                     if(mode == 0)
                     {
                        data = dc2800_tn_reg_read(address, reg);
                        printk("\r\n read data from tuner reg: 0x%x = 0x%x,",reg,data);
                     }
                     else  if(mode == 1)
                     {
                      ret = dc2800_tn_seq_read(address, g_buf, wlen, rlen);
                      if( ret == SUCCESS)
                        printk("\n sequence read tuner success");
                       else
                        printk("\n sequence read tuner wrong");
                     }
                     else
                     {
                      ret = dc2800_tn_std_read(address, g_buf, wlen, rlen);
                      if( ret == SUCCESS)
                        printk("\n stantard read tuner success");
                       else
                        printk("\n stantard read tuner wrong");
                     }

                     if(ret == SUCCESS)
                     {
                      printk("\n read data:");
                      printk("\n ");
                      for(i = 0; i < rlen; i ++)
                        printk("\r data[%d] = 0x%x",i,g_buf[i]);
                     }
                     return;
        case 'd':
                     if(mode == 0)
                     {
                        data = dc2800_dmd_reg_read(address, reg);
                        printk("\r\n read data from demod reg: 0x%x = 0x%x,",reg,data);
                     }
                     else  if(mode == 1)
                     {
                      ret = dc2800_dmd_seq_read(address, g_buf, wlen, rlen);
                      if( ret == SUCCESS)
                        printk("\n sequence read demod success");
                       else
                        printk("\n sequence read demod wrong");
                     }
                     else
                     {
                      ret = dc2800_dmd_std_read(address, g_buf, wlen, rlen);
                      if( ret == SUCCESS)
                        printk("\n stantard read demod success");
                       else
                        printk("\n stantard read demod wrong");
                     }

                     if(ret == SUCCESS)
                     {
                      printk("\n read data:");
                      printk("\n ");
                      for(i = 0; i < rlen; i ++)
                        printk("\r data[%d] = 0x%x",i,g_buf[i]);
                     }
                     return;
        default:
                  printk("\n wrong param, should be t(uner) or d(emod)");
                  goto RMEXIT;
      }
    }
    else if(argv[1][0] == 'w')
    {
       switch(mode)
       {
        case 0:
          reg = str2int(argv[5]);
          value = str2int(argv[6]);
          break;
        case 1:
          wlen = str2int(argv[5]);
          for(i = 0; i < wlen; i++)
         {
           g_buf[i] = str2int(argv[6+i]);
         }
         break;
        default:
          printk("\r\n wrong write mode, should be 0 or 1");
          return;
       }

       switch(argv[3][0])
      {
        case 't':
                     if(mode == 0)
                     {
                       dc2800_tn_reg_write(address, reg, value);
                       printk("\n write data to tuner reg: 0x%x = 0x%x,",reg,value);
                     }
                     else
                     {
                       ret = dc2800_tn_seq_write(address, g_buf, wlen);
                       if( ret == SUCCESS)
                        printk("\n sequence write tuner success");
                       else
                        printk("\n sequence write tuner wrong");
                     }
                     return;
        case 'd':
                     if(mode == 0)
                     {
                       dc2800_dmd_reg_write(address, reg, value);
                       printk("\n write data to demod reg: 0x%x = 0x%x,",reg,value);
                     }
                     else
                     {
                       ret = dc2800_dmd_seq_write(address, g_buf, wlen);
                       if( ret == SUCCESS)
                        printk("\n sequence write demod success");
                       else
                        printk("\n sequence write demod wrong");
                     }
                     return;
        default:
                   printk("\n wrong param, should be t(uner) or d(emod)");
                  goto RMEXIT;
      }
    }
    else
      goto RMEXIT;


RMEXIT:
 printk("\r\n Function: tuner demod dubug");
 printk("\r\n Usage:  td  param1 param2 param3  addr [rlen] [wlen] [reg] [data]");
 printk("\r\n param1 ----- r:  read ");
 printk("\r\n             ----- w: write");
 printk("\r\n param2 ------ : read/write mode");
 printk("\r\n             ----- 0: read/write register");
 printk("\r\n             ----- 1: sequence read/write mode");
 printk("\r\n             ----- 2: stantard write mode");
 printk("\r\n param3 ----- t:  tuner ");
 printk("\r\n             ----- d: demod");
 printk("\r\n addr -------- :  addr of i2c client ");
 printk("\r\n rlen --- ----- :  the length to read with mode 1 or 2 ");
 printk("\r\n wlen --- -----:  the length to write with mode ");
 printk("\r\n reg --- ------:  the register of tuner or demod ");
 printk("\r\n data --- -----:  data needed to be writen");
 printk("\n example:");
 printk("\n 1. td r 0 t 0xc2 0x00");
 printk("\n ---- read the data from the tuner reg 0x00 ");
 printk("\n 2. td r 1 t 0xc2 0x05 0x01 0x00");
 printk("\n ---- sequence read 5 data from the tuner begin with reg 0x00 ");
 printk("\n 1. td w 0 t 0xc2 0x00 0x01");
 printk("\n ---- data the data 0x01 to the tuner reg 0x00 ");
 printk("\n 2. td w 1 t 0xc2 0x05 0x00 0x01 0x02 0x03 0x04");
 printk("\n ---- sequence write 5 data to the tuner begin with reg 0x00 ");
 printk("\n ");

}

static void cmd_td(void)
{
  exec_cmd(td_debug);
  return;
}

static void cmd_td_help(void)
{
 printk("\r\n Function: tuner demod dubug");
 printk("\r\n Usage:  td  param1 param2 param3  addr [rlen] [wlen] [reg] [data]");
 printk("\r\n param1 ----- r:  read ");
 printk("\r\n             ----- w: write");
 printk("\r\n param2 ------ : read/write mode");
 printk("\r\n             ----- 0: read/write register");
 printk("\r\n             ----- 1: sequence read/write mode");
 printk("\r\n             ----- 2: stantard write mode");
 printk("\r\n param3 ----- t:  tuner ");
 printk("\r\n             ----- d: demod");
 printk("\r\n addr -------- :  addr of i2c client ");
 printk("\r\n rlen --- ----- :  the length to read with mode 1 or 2 ");
 printk("\r\n wlen --- -----:  the length to write with mode ");
 printk("\r\n reg --- ------:  the register of tuner or demod ");
 printk("\r\n data --- -----:  data needed to be writen \n");
 printk("\n example:");
 printk("\n 1. td r 0 t 0xc2 0x00");
 printk("\n ---- read the data from the tuner reg 0x00 ");
 printk("\n 2. td r 1 t 0xc2 0x05 0x01 0x00");
 printk("\n ---- sequence read 5 data from the tuner begin with reg 0x00 ");
 printk("\n 1. td w 0 t 0xc2 0x00 0x01");
 printk("\n ---- data the data 0x01 to the tuner reg 0x00 ");
 printk("\n 2. td w 1 t 0xc2 0x05 0x00 0x01 0x02 0x03 0x04");
 printk("\n ---- sequence write 5 data to the tuner begin with reg 0x00 ");
 printk("\n ");
}


static void cmd_rm_help(void)
{
    printk("\r\n Function: read  memory");
    printk("\r\n Usage:   rm  addr  [size]");
    printk("\r\n          size -- 1 byte; 2 half word; 4 word\r\n");
}

static void cmd_rmb(void)
{
  exec_cmd(readmemblock);
  return;
}

static void cmd_rmb_help(void)
{
    printk("\r\n Function: read  memory block");
    printk("\r\n Usage:   rmb  addr  [/mode]");
    printk("\r\n          mode      -- /bxxx byte");
    printk("\r\n                    -- /hxxx half word");
    printk("\r\n                    -- /wxxx 4 word\r\n");
}

static void cmd_wm(void)
{
  exec_cmd(writemem);
  return;
}

static void cmd_wm_help(void)
{
    printk("\r\n Function: write memory");
    printk("\r\n Usage:   wm  addr  val [size]");
    printk("\r\n          val  --    0 ~~ 0xffffffff");
    printk("\r\n          size --    1 byte; 2 half word; 4 word\r\n");
}

void shell_init_cmd(void)
{
  testfm_p_suite_t p_suite = NULL;

  p_suite = testfm_add_suite("mem", cmd_mem_init, cmd_mem_deinit);

  testfm_add_cmd(p_suite, "rm", cmd_rm, cmd_rm_help,
                            PLATFORM_MAGIC |PLATFORM_WIZARD | PLATFORM_WARRIORS |PLATFORM_JAZZ, FALSE);

  testfm_add_cmd(p_suite, "rmb", cmd_rmb, cmd_rmb_help,
                            PLATFORM_MAGIC |PLATFORM_WIZARD | PLATFORM_WARRIORS |PLATFORM_JAZZ, FALSE);

  testfm_add_cmd(p_suite, "wm", cmd_wm, cmd_wm_help,
                            PLATFORM_MAGIC |PLATFORM_WIZARD | PLATFORM_WARRIORS |PLATFORM_JAZZ, FALSE);

  testfm_add_cmd(p_suite, "chip_id", cmd_chipid, cmd_chipid_help,
                            PLATFORM_MAGIC |PLATFORM_WIZARD | PLATFORM_WARRIORS |PLATFORM_JAZZ, FALSE);

   testfm_add_cmd(p_suite, "td", cmd_td, cmd_td_help,
                            PLATFORM_MAGIC |PLATFORM_WIZARD | PLATFORM_WARRIORS |PLATFORM_JAZZ, FALSE);

}

