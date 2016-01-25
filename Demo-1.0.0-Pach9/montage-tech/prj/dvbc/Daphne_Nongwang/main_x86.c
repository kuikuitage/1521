#include <malloc.h>
#include <assert.h>

#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_msg.h"
//andytest_automerge_1
#include "mem_manager.h"
#include "sys_dbg.h"

#include "mem_cfg.h"

extern void Task_SysTimer(void *p_data);
extern void ap_init(void);

static u8 g_sys_mem[SYS_MEMORY_END] = {0};

BOOL is_test_version = FALSE;

u32 get_mem_addr(void)
{
  return (u32)g_sys_mem;
}

extern u8 *p_flash_buf;
u32 get_flash_addr(void)
{
  return (u32)p_flash_buf;
}


void* my_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  MT_ASSERT(p_addr != NULL);
  return p_addr;
}

void my_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}

void task_sys_init(void* p_data)
{
  BOOL ret = FALSE;
  u32* pstack_pnt = NULL;
  mem_mgr_partition_param_t partition_param = { 0 };
  
  //init memory manager, to the memory end
  mem_mgr_init(g_sys_mem, SYS_MEMORY_END);
  
  /* create SYS partition */
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = SYS_PARTITION_SIZE;
  partition_param.p_addr = g_sys_mem;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = MEM_CREATE_PARTITION(&partition_param);
  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create patition ok!\n");
  
  //register malloc/free function, using system partition
  mtos_mem_init(my_malloc, my_free);
  OS_PRINTF("init mem ok!\n");

  mtos_ticks_init(SYS_CPU_CLOCK);

  //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);

#ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
#endif

  //create timer task
  pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);
  
  ret = mtos_task_create("Timer",
         Task_SysTimer,
         (void *)0,
         SYS_TIMER_TASK_PRIORITY,
         pstack_pnt,
         SYS_TIMER_TASK_STKSIZE);

  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create timer task ok!\n");

  mtos_task_sleep(25);
  OS_PRINTF("start ap init!\n");
  ap_init();
  OS_PRINTF("ap init ok!\n");
  
  mtos_task_exit( );
}

int main(void)
{
  u8* p_stack = malloc(SYS_INIT_TASK_STKSIZE);
  mtos_cfg_t os_cfg = {0};
  
  os_cfg.enable_bhr = FALSE;
  DEBUG_ENABLE_MODE(MSG, INFO);
  mtos_os_init(&os_cfg);
  mtos_task_create("initT",
    task_sys_init,
    NULL, 
    1,
    (u32*)p_stack,
    SYS_INIT_TASK_STKSIZE);

  /* begin to run multi-task */
  mtos_start();
  return 0;
}

int main_entry(void)
{
	return main();
}
