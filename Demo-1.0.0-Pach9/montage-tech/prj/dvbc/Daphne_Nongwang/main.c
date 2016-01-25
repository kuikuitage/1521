#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "sys_regs_jazz.h"

#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_msg.h"
#include "mem_manager.h"

#include "hal_base.h"
#include "hal_uart.h"
#include "hal_irq.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal.h"
#include "ads_ware.h"
#include "mem_cfg.h"
#include "ads_funtion_public.h"
#include "sys_dbg.h"
#include "chipId.h"
#include "config_customer.h"
#define REG32(addr)                 (*((volatile u32 *)(addr)))

//#define MEM_OLD_PORTING

BOOL is_test_version = FALSE;
    
u32 get_mem_addr(void)
{
  return 0;
}
    
u32 get_flash_addr(void)
{
  return FLASH_BASE_ADDR;
}

#ifdef MEM_OLD_PORTING
static void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  //MT_ASSERT(p_addr != NULL);
 // OS_PRINTF("alloc mem addr 0x%x\n",p_addr);
  memset((u8 *)p_addr,0,size);
  return p_addr;
}
  
static void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  //param.p_addr = (u32)p_addr;
  param.p_addr = p_addr;
 // OS_PRINTF("free mem addr 0x%x\n",p_addr);
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}
#else
extern void dlmem_init(void * base, int size);
static void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  p_addr = malloc(size);
  memset((u8 *)p_addr,0,size);
  return p_addr;
}
  
static void mt_mem_free(void* p_addr)
{
   free(p_addr);
}
#endif
  
extern void Task_SysTimer(void *p_data);
extern void ap_init(void);

extern unsigned long *__bss_start;
extern unsigned long *__bss_end;

void task_sys_init(void* p_data)
{
  BOOL ret = FALSE;
  u32 mem_start;
  u32 mem_sys_size = 0;
  u32* pstack_pnt = NULL;
  int check_chipId_flag;

  //volatile unsigned long * dst	= (volatile unsigned long *)&__bss_start;
  void * bss_end 	= (void*)&__bss_end;

  DEBUG_ENABLE_MODE(DSCA4, INFO);  

  #ifdef MEM_OLD_PORTING
  mem_mgr_partition_param_t partition_param = { 0 };
  
  #if(CONFIG_ADS_ID == CONFIG_ADS_ID_BOYUAN)
  ads_mem_init(bss_end);
  bss_end += ADS_PARTITION_SIZE;
  #endif

  #endif
  //actural mem start is bss_end + initT task stack size
  mem_start = ROUNDUP((u32)bss_end,4);
  mem_sys_size = OSD0_16BIT_BUFFER_ADDR - mem_start;
  OS_PRINTF("\n  mem_start[0x%x]\n",mem_start);
  OS_PRINTF("\n  defined MEMORY_END[0x%x]\n", SYS_MEMORY_END);
  OS_PRINTF("\n  heap size[%d]\n",SYS_MEMORY_END-mem_start);
  OS_PRINTF("\n  system free size[%d]\n",mem_sys_size);
    
  //init memory manager, to the memory end
  mem_mgr_init((u8 *)mem_start, mem_sys_size);
#ifdef MEM_OLD_PORTING
  //create system partition
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = (mem_sys_size -100 * KBYTES);//SYS_PARTITION_SIZE - ADS_PARTITION_SIZE;
  partition_param.p_addr = (u8 *)mem_start;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = mem_mgr_create_partition(&partition_param);
  MT_ASSERT(FALSE != ret);

  //register malloc/free function, using system partition
  mtos_mem_init(mt_mem_malloc, mt_mem_free);
#else

   dlmem_init((u8 *)mem_start,mem_sys_size);
   mtos_mem_init(mt_mem_malloc,mt_mem_free);
   //memset((u8 *)mem_start,0,mem_sys_size);
#endif
   
  OS_PRINTF("init mem ok!\n");

	mtos_ticks_init(SYS_CPU_CLOCK);

	//init message queue
	ret = mtos_message_init();
	MT_ASSERT(FALSE != ret);

	//create timer task
	pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
	MT_ASSERT(pstack_pnt != NULL);

	ret = mtos_task_create((u8 *)"Timer",
		Task_SysTimer,
		(void *)0,
		SYS_TIMER_TASK_PRIORITY,
		pstack_pnt,
		SYS_TIMER_TASK_STKSIZE);


	MT_ASSERT(FALSE != ret);
	OS_PRINTF("create timer task ok!\n");

	mtos_task_sleep(25);
	OS_PRINTF("start ap init!\n");
	//check chipId
	check_chipId_flag=SeeNewQueryAuthorization(THINEW_AUTH_STR, THINED_KEY_STR);
	DEBUG(MAIN, NOTICE, "check_chipId_flag:%d\n", check_chipId_flag);
	//if no chipId function, this software is test version
	if(0 != check_chipId_flag)
	{
		is_test_version = TRUE;
	}
	else
	{
		is_test_version = FALSE;
	}
	//while(0!=check_chipId_flag);

	ap_init();
	OS_PRINTF("ap init ok!\n");

	mtos_task_exit( );
}
   
extern void hal_jazz_attach(void);
extern void unaligned_access_enable();
int main(void)
{
  mtos_cfg_t os_cfg = {0};
DEBUG_ENABLE_MODE(MAIN,INFO);
DEBUG(MAIN,INFO,"main start now\n");

  //u8* stk_ptr = (u8*)SYS_MEMORY_END - 1024*2;
  u8* stk_ptr = (u8*)SYS_MEMORY_END - SYS_INIT_TASK_STKSIZE;
  //unaligned_access_enable();
  hal_jazz_attach();
  //init os
  os_cfg.enable_bhr = FALSE;

  mtos_os_init(&os_cfg);

  //init irq, and start uart
  mtos_irq_init();
  uart_init(0);
  uart_set_param(0,115200, 8, 1, 0);
  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);
  //create the first task to conitinue
  mtos_task_create((u8 *)"initT",
                 task_sys_init,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)stk_ptr,
                 SYS_INIT_TASK_STKSIZE);
  /* begin to run multi-task */
  mtos_start();
  return (0);
}
  
