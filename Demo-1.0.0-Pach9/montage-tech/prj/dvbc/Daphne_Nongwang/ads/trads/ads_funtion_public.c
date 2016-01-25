// lib
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
//#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_irq_jazz.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "pdec.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "spi.h"
#include "cas_ware.h"
#include "sub.h"
#include "driver.h"
#include "lpower.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager32.h"
#include "data_manager_v3.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
//#include "ca_svc.h"

#include "monitor_service.h"

#include "dsmcc.h"
#include "epg_data4.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"
#include "vbi_vsb.h"
#include "surface.h"

#include "db_dvbs.h"
#include "sys_status.h"
#include "mem_cfg.h"

//eva
#include "interface.h"
#include "eva.h"

// ap

#include "ap_framework.h"

#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_epg4.h"
#include "ap_signal_monitor.h"

#include "ap_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#include "ap_pic_play.h"

#include "config_customer.h"
#include "config_prj.h"
#include "ads_funtion_public.h"

#ifdef ROLL_TITLE
#include "subt_pic_ware.h"
#include "ui_roll_title.h"
#endif

u8 *p_ads_mem = NULL;

#ifdef ROLL_TITLE
u8 *p_subt_pic_mem = NULL;
#endif

extern void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate);
extern void ads_module_init(void);
extern void ads_show_logo(void);

#ifdef ROLL_TITLE
extern void subt_pic_module_init(subt_pic_module_cfg_t cfg);
#endif

//extern void FixupRectangleDepth(u32 x, u32 y,
 //                              u32 width,u32 height,
 //                              u32 Bmpwidth,u8 *source,
 //                              u8 *source1,u8 Scoll_time);


void ads_mem_init(u32 mem_start)
{
#if 0
  BOOL ret = FALSE;
  mem_mgr_partition_param_t partition_param = { 0 };
    //create ads partition
  partition_param.id   = MEM_ADS_PARTITION;
  partition_param.size = 6*KBYTES*KBYTES;
  partition_param.p_addr = (u8 *)mem_start + 200*KBYTES;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_APP_TEST;
  partition_param.method_id = MEM_METHOD_NORMAL;
  
  //memset(partition_param.p_addr,0,partition_param.size);
  ret = mem_mgr_create_partition(&partition_param);
  MT_ASSERT(FALSE != ret);
#endif
}

void ads_ap_init(void)
{
  dvbc_lock_t tmp_maintp = {0};
  sys_status_get_main_tp1(&tmp_maintp);
  set_adver_maintp(tmp_maintp.tp_freq,tmp_maintp.tp_sym,tmp_maintp.nim_modulate);

  OS_PRINTF("0000000\n");

  mem_show();

  ads_module_init();
  mem_show();
  OS_PRINTF("11111111\n");
  p_ads_mem = mtos_malloc(10);
  OS_PRINTF("22222222\n");
  mem_show();
  MT_ASSERT(p_ads_mem != NULL);
  OS_PRINTF("33333333\n");
  memset(p_ads_mem, 0x8a, 10);
  
  OS_PRINTF("4444444\n");
  //ads_show_logo();
  
  OS_PRINTF("55555555\n");
}


void subt_pic_ap_init(void)
{
#ifdef ROLL_TITLE
  subt_pic_module_cfg_t cfg;
  cfg.task_prio_start = subt_pic_TASK_PRIO_START;
  cfg.task_prio_end = subt_pic_TASK_PRIO_END;
  cfg.subt_is_scroll = subt_is_scroll;
  cfg.osd_display_is_ok = osd_display_is_ok;
  cfg.channel_info_set = channel_info_set;
  cfg.scroll_step_pixel_set = scroll_step_pixel_set;
  cfg.draw_rectangle = draw_rectangle;
  cfg.draw_picture = draw_picture;

  subt_pic_module_init(cfg);

  p_subt_pic_mem = mtos_malloc(10);
  MT_ASSERT(p_subt_pic_mem != NULL);
  memset(p_subt_pic_mem, 0x8a, 10);
#endif
}

