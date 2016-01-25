#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "sys_cfg.h"

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

#include "spi.h"


#include "mem_cfg.h"



#include "class_factory.h"
#include "mdl.h"


#include "data_manager.h"
#include "data_manager32.h"
#include "data_manager_v3.h"

#include "service.h"
#include "mosaic.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"

#include "charsto.h"
#include "uio.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"

#include "dvb_svc.h"
#include "ap_framework.h"
#include "ap_kingvon_ota.h"
#include "sys_cfg.h"
#include "driver.h"

#include "lib_util.h"
#include "customer_def.h"
#include "sys_data_staic.h"

#define REG32(addr)                 (*((volatile u32 *)(addr)))

#define  MEM_OLD_PORTING

u32 get_mem_addr(void)
{
  return 0;
}

u32 get_flash_addr(void)
{
  return FLASH_BASE_ADDR;
}

#if 0
int mtos_printk(const char *p_fmt, ...)
{
    return 0;
 }
#endif

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
  return p_addr;
}

static void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
 // OS_PRINTF("free mem addr 0x%x\n",p_addr);
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}
#else
extern void dlmem_init(void* base, int size);
#endif
extern void Task_SysTimer(void *p_data);
extern void ap_init(void);

extern unsigned long *__bss_start;
extern unsigned long *__bss_end;

extern RET_CODE spi_attach_jazz(char *p_name);
extern void ota_read_otai(ota_info_t *p_otai);
extern BOOL is_burn_finish(void);
extern void ota_write_otai(ota_info_t *p_otai);
extern void ota_uio_init(void);
extern void set_ota_config(void);
extern u8 get_ota_force_irkey(void);
extern u8 get_ota_force_fpkey(void);
extern void ap_uio_init_kep_map(void);
BOOL is_force_key_press()
{
  void *p_dev = NULL;
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  RET_CODE ret = SUCCESS;
  
  p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);
  MT_ASSERT(p_dev != NULL);
  ap_uio_init_kep_map();
  
  ret = uio_get_code(p_dev, &code);
  if((code & 0x0100) != 0)/**fp_key***/
  {
    hkey = get_ota_force_fpkey();
  }
  else
  {
    hkey = get_ota_force_irkey();
  }
  

  OS_PRINTF("hkey 0x%x, code 0x%x, ret=%d\n", hkey, code, ret);

  if((ret == SUCCESS) && (hkey == (0xff&code)) )
  {
    OS_PRINTF("it will force up ota!!!!\n");
    return TRUE;
  }

  return FALSE;
}
extern void ota_init_info(ota_info_t *otai);
static void ota_init_spi_charsto(void)
{
    void  *p_dev = NULL;
    RET_CODE ret = ERR_FAILURE;
    /* char storage */
    void *p_spi_0 = NULL;
    spi_cfg_t spi_cfg = {0};

    /* init spi bus driver */
    ret = spi_attach_jazz("spi_jazz_0");
    MT_ASSERT(SUCCESS == ret);
    p_spi_0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"spi_jazz_0");
    /* config spi bus for flash */
    {
      u32 dtmp = 0;
      dtmp = hal_get_u32((u32*)0x70000000);
      //dtmp &= ~(3 << 8);
      dtmp &= 0xfffffcff;
      hal_put_u32((u32*)0x70000000, dtmp);
    }
    spi_cfg.bus_clk_mhz = 50;
    spi_cfg.bus_clk_delay = 1;
    spi_cfg.io_num = 1;
    spi_cfg.lock_mode = OS_MUTEX_LOCK;
    spi_cfg.op_mode = 0;
    spi_cfg.pins_cfg[0].miso1_src = 0;
    spi_cfg.pins_cfg[0].miso0_src = 1;
    spi_cfg.pins_cfg[0].spiio1_src = 0;
    spi_cfg.pins_cfg[0].spiio0_src = 0;
    spi_cfg.pins_dir[0].spiio1_dir = 0;
    spi_cfg.pins_dir[0].spiio0_dir = 1;
    spi_cfg.spi_id = 0;
    ret = dev_open(p_spi_0, &spi_cfg);
    MT_ASSERT(SUCCESS == ret);

    /* char storage */
    {
      charsto_cfg_t charsto_cfg = {0};
      ret = ATTACH_DRIVER(CHARSTO, jazz, default, default);
      MT_ASSERT(ret == SUCCESS);

      p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
      MT_ASSERT(NULL != p_dev);

      charsto_cfg.rd_mode = SPI_FR_MODE;
      #ifndef WIN32
      charsto_cfg.p_bus_handle = p_spi_0;
      #endif
      charsto_cfg.size = CHARSTO_SIZE;
      ret = dev_open(p_dev, &charsto_cfg);
      MT_ASSERT(ret == SUCCESS);
    }
}

static void ota_init_dm(void)
{
     dm_v3_init_para_t dm_para = { 0 };
    /* init data manager */
    dm_para.ver = DM_VER_3;
    dm_para.flash_base_addr = get_flash_addr();
    OS_PRINTF("flash base addr --0x%x\n",dm_para.flash_base_addr);
    dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
    dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
    dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
    dm_para.open_monitor = TRUE;
    dm_para.use_mutex = TRUE;
    dm_para.mutex_prio = OS_MUTEX_PRIORITY;
    dm_para.piece_size = 0;
    dm_para.ghost_addr = 0;
    dm_para.using_max_node_len = DM_MAX_DATA_LEN;
    dm_para.test_mode = FALSE;
    OS_PRINTF("init v3 --4\n");
    dm_init_v3(&dm_para);
    OS_PRINTF("init v3 end --4\n");

    OS_PRINTF("set header \n");

    dm_set_header(class_get_handle_by_id(DM_CLASS_ID), FLASH_BOOT_ADRR_DM);
    //dm_set_header(class_get_handle_by_id(DM_CLASS_ID), FLASH_LOGIC_ADRR_DM);
  }
ota_trigger_t ota_check(void)
{
  ota_info_t otai = {0};

  /****chenguangfu move here from drv_init****/
  OS_PRINTF("drv --1\n");
  gpio_init();
  OS_PRINTF("drv --2\n");
  hal_dma_init();
  OS_PRINTF("drv --3\n");
  ota_init_spi_charsto();
  OS_PRINTF("drv --3\n");
  ota_init_dm();
   /* open uio */
  ota_uio_init();   
  ota_read_otai(&otai);          
  ota_init_info(&otai);
  /* flash burning is not finished, force ota*/
  if(!is_burn_finish())
  {
    OS_PRINTF("###ota stop  no burn finsh  it will fore ota\n");
    sys_static_check_and_repair_ota_info();
    ota_read_otai(&otai);
    otai.ota_tri = OTA_TRI_FORC;
    ota_write_otai(&otai);
    return OTA_TRI_FORC;
  }
  /* check if download flag has been set */
  if(otai.ota_tri == OTA_TRI_AUTO)
  {      
    return OTA_TRI_AUTO;
  }
   /*********/     
  if(otai.ota_tri == OTA_TRI_FORC)
  {
    return OTA_TRI_FORC;
  }    
 
  mtos_task_delay_ms(200);
  if(is_force_key_press())
  {
    sys_static_check_and_repair_ota_info();
    ota_read_otai(&otai);
    otai.ota_tri = OTA_TRI_AUTO;  /***make sure exist ota for 3 times***/
    ota_write_otai(&otai);    
    return OTA_TRI_AUTO;
  }

  return OTA_TRI_NONE;
}

u32 restore_dest = 0;
 u32 restore_sp = 0;
u32 boot_addr;

extern void __restore_enviroment_b(u32);
void __testfm_save_env_b(u32 pointer, u32 stack, u32 addr)
{
  restore_dest = pointer;
  restore_sp = stack;
  boot_addr = addr;
}



void ota_to_app()
{
    OS_PRINTF("ota t0 app test !!! \n");
     __restore_enviroment_b(restore_sp);
}


void task_sys_init(void* p_data)
{
  BOOL ret = FALSE;
  customer_cfg_t g_cus_cfg;
  u32 mem_start;
  u32 mem_ota_start = 0;
  u32 mem_ota_end = 0;
  u32 mem_ota_size = 0;
  u32* pstack_pnt = NULL;
  ota_trigger_t ota_trigger = 0;
  //volatile unsigned long * dst	= (volatile unsigned long *)&__bss_start;
  volatile unsigned long * bss_end 	= (volatile unsigned long *)&__bss_end;
#ifdef MEM_OLD_PORTING
  mem_mgr_partition_param_t partition_param = { 0 };
#endif
  //actural mem start is bss_end + initT task stack size
  mem_start = ROUNDUP((u32)bss_end,4);
  OS_PRINTF("\n 2. mem_start[0x%x]\n",mem_start);
  OS_PRINTF("\n 3. defined MEMORY_END[0x%x]\n", SYS_MEMORY_END);
  OS_PRINTF("\n 4. heap size[%d]\n",SYS_MEMORY_END-mem_start);
  /***the size is define in boot load***/
  /***0x900000~0xafffff boot self  0xb00000~mem_start is ota img self***/
 OS_PRINTF("\n 5.1 mem total size:[0x%x]\n",SYS_MEMORY_TOTALT_SIZE);  
  OS_PRINTF("\n 5.2 flalsh total size:[0x%x]\n",SYS_FLASH_TOTALT_SIZE);  
  #if(SYS_MEMORY_TOTALT_SIZE >= SYS_MEMORY_TOTALT_SIZE_32M)
  mem_ota_start = mem_start;
  mem_ota_end =  OSD0_8BIT_BUFFER_ADDR;
  mem_ota_size = mem_ota_end - mem_ota_start;
  /***0x400~0x900000 will user save data buff,other area don't user it****/
  #elif(SYS_MEMORY_TOTALT_SIZE == SYS_MEMORY_TOTALT_SIZE_16M)
  mem_ota_start = (1 * KBYTES); /**don't set  addr = o**/
  mem_ota_end =  0x900000;
  mem_ota_size = mem_ota_end - mem_ota_start;
  #endif
  OS_PRINTF("\n 5.3 ota free start:[0x%x] end:[0x%x]\n",mem_ota_start ,mem_ota_end);
  OS_PRINTF("\n 5.5 OTA  sysmemsize[%d]\n",mem_ota_size);
  //init memory manager, to the memory end
  #ifdef MEM_OLD_PORTING
  mem_mgr_init((u8 *)mem_ota_start, mem_ota_size);

  //create system partition
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = (mem_ota_size - 100 * KBYTES);
  partition_param.p_addr = (u8 *)mem_ota_start;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = mem_mgr_create_partition(&partition_param);
  MT_ASSERT(FALSE != ret);

  //register malloc/free function, using system partition
  mtos_mem_init(mt_mem_malloc, mt_mem_free);
  #else
   dlmem_init((u8 *)mem_ota_start,mem_ota_size);
   mtos_mem_init((pmalloc)malloc,free);
   memset((u8 *)mem_ota_start,0,mem_ota_size);
  #endif

  OS_PRINTF("init mem ok!\n");

	mtos_ticks_init(SYS_CPU_CLOCK);

	//init message queue
	ret = mtos_message_init();
	MT_ASSERT(FALSE != ret);

	//create timer task
	pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
	MT_ASSERT(pstack_pnt != NULL);

	ret = mtos_task_create((u8*)"Timer",
		Task_SysTimer,
		(void *)0,
		SYS_TIMER_TASK_PRIORITY,
		pstack_pnt,
		SYS_TIMER_TASK_STKSIZE);

	MT_ASSERT(FALSE != ret);
	OS_PRINTF("create timer task ok!\n");

	mtos_task_sleep(25);
	OS_PRINTF("start ap init!\n");
    set_customer_config();
    get_customer_config(&g_cus_cfg);
    set_ota_config();
    
   ota_trigger = ota_check();
   OS_PRINTF("###ota check triger:%d\n",ota_trigger);
  if(ota_trigger == OTA_TRI_NONE)
  {
    OS_PRINTF("no ota !!! Go Back to main code\n");
    u8 content[4] = {' ', 'O', 'N', 0};


    uio_device_t* p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

    #ifdef ZhongDA
     uio_display(p_dev, content, 4);
    #else
    uio_display(p_dev, content, 3);
    #endif

    ota_to_app();
  }
  else
  {
    u8 content[5] = {' ', 'U', 'P', 'G', 0};
    if(get_uio_led_number_type() == (u8)UIO_LED_TYPE_3D)
    {
      memcpy(content, "UPG", 4);
    }
    uio_device_t* p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    uio_display(p_dev, content, 4);
    ap_init();
  }

  mtos_task_exit( );
}

extern void hal_jazz_attach(void);

int main(int p1, char **p2, char **p3)
{
  mtos_cfg_t os_cfg = {0};
  __testfm_save_env_b((u32)p1, (u32)p2, (u32)p3);

  //u8* stk_ptr = (u8*)SYS_MEMORY_END - 1024*2;
  u8* stk_ptr = (u8*)SYS_MEMORY_END - SYS_INIT_TASK_STKSIZE;

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
  mtos_task_create((u8*)"initT",
                 task_sys_init,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)stk_ptr,
                 SYS_INIT_TASK_STKSIZE);
  /* begin to run multi-task */
  mtos_start();
  return (0);
}

