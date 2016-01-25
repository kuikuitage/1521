/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "hal_misc.h"
#include "mtos_printk.h"
#include "mtos_misc.h"

#include "testfm_define.h"
#include "testfm_inc.h"
#include "testfm_db.h"
#include "testfm_console.h"
#include "testfm_op.h"

#ifndef WIN32
#ifndef ORCHID
#ifdef JAZZ
//#include "../../src/os/ucos/inc/includes.h"
#else
#include "includes.h"
#include "mips_cpu.h"
#endif
#endif
#include "mtos_int.h"
#endif
#include <string.h>
static console_mode_t consolemode = DOWNLOAD_MODE;

static int is_downloaded = 0;

extern void Console_PutChar(u8 c);
extern u8 Console_GetChar(void);

void restore_enviroment(void);



static u8 char2dec(char c)
{
  if((c >= '0') && (c <= '9'))
    return (c-'0');

  if((c >= 'a') && (c <= 'f'))
    return (10 + (c-'a'));

  if((c >= 'A') && (c <= 'F'))
    return (10 + (c-'A'));

  return 0xff;
}

char dec2char(u8 num)
{
  if(/*(num >= 0) && */(num <= 9))
    return ('0' + num);

  if((num >= 0xa) && (num <= 0xf))
    return ('a' + (num-10));

  return ' ';
}

u32 testfm_get_num(char *str)
{
  int len = 0;
  int i = 0;
  u32 num = 0;
  int indicate = 0;
  u8 dec = 0;
  char *p_str = str;

  MT_ASSERT(str != NULL);

  if((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
  {
    indicate = 1;
    p_str = &str[2];
  }

  len = strlen(p_str);

  if(len == 0)
    return -1;

  for(i = 0; i < len; i++)
  {
    dec = char2dec(p_str[i]);
    if(dec == 0xff)
      continue;

    if(indicate == 1)
      num = num<<4;
    else
      num = num *10;
    num += dec;
  }

  return num;
}

void read_test(u32 add, u32 len)
{
  u32 size = 0;
  unsigned char *address = 0;
  int index = 0;

  address = (unsigned char *)add;
  size = len;

  OS_PRINTF("from 0x%p to 0x%p(total size = %d):\n", address, address+size-1, size);

  for(index = 0; index < size; index++)
  {
    OS_PRINTF("%c%c ", dec2char((address[index]&0xf0)>>4), dec2char(address[index]&0xf));
  }
  OS_PRINTF("\n");
}

#if !(defined(WIN32)|defined(JAZZ))
static u32 *p_sp = NULL;
static u32 cpsr;
static u32 restore_dest = 0;
static u32 restore_sp = 0;

void testfm_save_env(u32 pointer, u32 stack)
{
  restore_dest = pointer;
  restore_sp = stack;
	OS_PRINTF("restore function pointer 0x%x\n", restore_dest);
  OS_PRINTF("stack pointer 0x%x\n", restore_sp);
  //read_test(restore_dest, 16);
	//read_test(restore_sp, 16);
  consolemode = TEST_MODE;
}

#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest)); }
void testfm_restroe_to_cmd()
{
#ifndef ORCHID
  u32 p_sr;
  u32 dest;
  if((restore_dest > 0x600000 && restore_dest < 0x610000) &&
      (restore_sp > (0x800000 - 8*1024 - 4) && restore_sp <0x800000))
  {
    OS_PRINTF("restore function pointer 0x%x\n", restore_dest);
    OS_PRINTF("stack pointer 0x%x\n", restore_sp);
    //read_test(restore_dest, 16);
    //read_test(restore_sp, 16);
    dest = (u32)restore_dest;
    mtos_critical_enter(&p_sr);
    $SP = (OS_REGS *)restore_sp;
    JUMP(dest);
  }
  else
  {
    OS_PRINTF("restore dest: %p, restore sp: %p\n", restore_dest, restore_sp);
  }
  return;
#endif
}

void __save_enviroment_jump()
{
#ifndef ORCHID
  register u32 $RA  __asm__ ("$31");
  //move back the sp subbed by function call
  //be careful to make sure adjust correctly $SP to rollback $SP placed by compiler
   __asm__ ("addiu $sp,$sp,8");


  // Allocate space on the stack and save all the registers
  // We only need to save the non volatile registers because
  // we come here through a normal function call.
  //SAVE_NON_VOLATILE($RA);
  SAVE_ALL_NEW($RA);

  // Save the current stack pointer
  p_sp = (u32 *)$SP;

  __asm__ ("li $s0, 0x0");
  __asm__ ("la $a0, restore_enviroment");
  __asm__ ("move $a1, $sp");
  __asm__ ("nop");
  __asm__ ("jr	$s0");
  __asm__ ("nop");
#endif
}


void save_enviroment_jump()
{
#ifndef ORCHID
	GET_SR(cpsr);
	mtos_printk(" Save environment\n");
  __save_enviroment_jump();
	mtos_printk(" Restore environment\n");
#endif
}

void __restore_enviroment()
{
#ifndef ORCHID
  register INT32U $KT0 __asm__ ("$26");
  $SP = (OS_REGS *)p_sp;
  RESTORE_ALL($KT0);
  RET($KT0);
#endif
}

void restore_enviroment()
{
#ifndef ORCHID
  __restore_enviroment();
  SET_SR(cpsr);
  //mtos_irq_enable(TRUE);
#endif
}
#else
void testfm_save_env(u32 pointer, u32 stack)
{
}

void testfm_restroe_to_cmd(void)
{
  return;
}

void save_enviroment_jump(void)
{
}

void restore_enviroment(void)
{
}
#endif

void set_console_mode(console_mode_t mode)
{
  consolemode = mode;
}

console_mode_t get_console_mode(void)
{
  return consolemode;
}

void cmd_help()
{
  if(get_console_mode() == DOWNLOAD_MODE)
  {
    OS_PRINTF("%-20s  %s\n", "download", "download the binary to memory(download size)");
    OS_PRINTF("%-20s  %s\n", "go", "jump to 0x0 to run the binary");
    OS_PRINTF("%-20s  %s\n", "read", "read the content of specified address(read address, size)");
    OS_PRINTF("%-20s  %s\n", "reboot", "reboot the platform");
  }
  else
  {
    testfm_p_registry_t p_registry = NULL;
    testfm_p_suite_t p_curSuite = NULL;
    testfm_p_cmd_t p_curCmd = NULL;

    p_registry = testfm_get_registry();
    MT_ASSERT(NULL != p_registry);

    OS_PRINTF("%-20s  %s\n", "COMMAND", "DESCRIPTION");
    p_curSuite = p_registry->pSuite;
    while((p_curSuite != NULL) )
    {
      p_curCmd = p_curSuite->pCmd;

      while(p_curCmd != NULL &&(p_curSuite->spec_cmd == PLATFORM_MAX))//not specific
      {
        OS_PRINTF("%-20s  %s\n", p_curCmd->cmdName, "It's a test command. No description currently.");
        p_curCmd = p_curCmd->pNext;
      }

      p_curSuite = p_curSuite->pNext;
    }
    OS_PRINTF("%-20s  %s\n", "help", "List all the commands and description");
    OS_PRINTF("%-20s  %s\n", "exit", "exit from test mode");
    OS_PRINTF("For more information on a specific command, type 'CMD -h'\n");
  }
}


void cmd_help_specific(supported_platform_t plat)
{
  if(get_console_mode() == DOWNLOAD_MODE)
  {
    return;
  }
  else
  {
    testfm_p_registry_t p_registry = NULL;
    testfm_p_suite_t p_curSuite = NULL;
    testfm_p_cmd_t p_curCmd = NULL;

    p_registry = testfm_get_registry();
    MT_ASSERT(NULL != p_registry);

    OS_PRINTF("%-20s  %s\n", "COMMAND", "DESCRIPTION");
    p_curSuite = p_registry->pSuite;
    while(p_curSuite != NULL)
    {
      p_curCmd = p_curSuite->pCmd;

      while(p_curCmd != NULL&& (p_curSuite->spec_cmd & plat)) //specific command
      {
        OS_PRINTF("%-20s  %s\n", p_curCmd->cmdName, "It's a test command. No description currently.");
        p_curCmd = p_curCmd->pNext;
      }

      p_curSuite = p_curSuite->pNext;
    }
    OS_PRINTF("%-20s  %s\n", "help", "List all the commands and description");
    OS_PRINTF("%-20s  %s\n", "exit", "exit from test mode");
    OS_PRINTF("For more information on a specific command, type 'CMD -h'\n");
  }
}

void cmd_read(void)
{
  u32 size = 0;
  unsigned char *address = 0;
  char tmp[32];
  int index = 0;

  testfm_console_get_parameter( tmp, ',');
  address = (unsigned char *)testfm_get_num(tmp);
  testfm_console_get_parameter(tmp, ',');
  size = testfm_get_num(tmp);

  OS_PRINTF("from 0x%p to 0x%p(total size = %d):\n", address, address+size-1, size);

  for(index = 0; index < size; index++)
  {
    OS_PRINTF("%c%c ", dec2char((address[index]&0xf0)>>4), dec2char(address[index]&0xf));
  }
  OS_PRINTF("\n");
  //a fix of unused funtion dec2char
  dec2char(5);
}

void cmd_download(void)
{
  u32 size = 0;
  char tmp[32];
  char tmp1 = 0;
  volatile unsigned char *binary = (unsigned char*)0x0;
  int index = 0;
  int percent = 0, tmpper = 0;
  int is_cancelled = 0;
  testfm_console_get_parameter_str(tmp);
  size = testfm_get_num(tmp);

  if(size <= 0)
  {
    OS_PRINTF("Binary size should greater than zero!\n");
    return;
  }
  if(size >= 2*1024*1024) //the binary should less than 2M
  {
    OS_PRINTF("Binary size should less than 2M!\n");
    return;
  }

  OS_PRINTF("Start downloading %d bytes to 0x%p. Progress: 0%", size, binary);

  while(index < size)
  {
    // receive the binary from uart
    tmp1 = Console_GetChar();
    if((tmp1 == 'q') || (tmp1 == 'Q'))
    {
      is_cancelled = 1;
      break;
    }
    binary[index] = char2dec(tmp1)<<4;

    tmp1 = Console_GetChar();
     if((tmp1 == 'q') || (tmp1 == 'Q'))
    {
      is_cancelled = 1;
      break;
    }
    binary[index++] |= char2dec(tmp1);

    // caculate the percentage
    tmpper = index * 100 / size;
    if(tmpper != percent)
    {
      percent = tmpper;
      Console_PutChar(8);
      Console_PutChar(8);

      if(percent >= 10)
      {
        Console_PutChar(8);
      }
      OS_PRINTF("%d%", percent);
    }

  }

  if(is_cancelled == 1)
  {
    is_downloaded = 0;
    is_cancelled = 0;
    OS_PRINTF("\ndownload was cancelled\n");
  }
  else
  {
    is_downloaded = 1;
    if(percent < 100)
    {
      Console_PutChar(8);
      Console_PutChar(8);
      Console_PutChar(8);
      OS_PRINTF("100%\n");
    }
    else
    {
      OS_PRINTF("\n");
    }
  }
}

void cmd_go(void)
{
  if(is_downloaded == 1)
  {
    is_downloaded = 0;
    save_enviroment_jump();
  }
  else
  {
    OS_PRINTF("No valid binary!\n");
  }
}

void cmd_reboot(void)
{
#ifndef WIN32
#ifndef ORCHID
  hal_pm_reset();
#endif
#endif
}

void cmd_exit(void)
{
  testfm_restroe_to_cmd();
}

void cmd_runall(void)
{
}

void cmd_statistic(void)
{
  // do nothing currently
}
