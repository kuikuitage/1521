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
#ifdef PATULOUS_FUNCTION_CA
#include "cas_ware.h"
#endif
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

#include "ads_ware.h"

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
#ifdef PATULOUS_FUNCTION_CA
#include "ap_cas.h"
#endif
#include "ap_pic_play.h"

#include "customer_def.h"
#include "config_customer.h"
#include "config_prj.h"
#include "ads_funtion_public.h"

void ads_mem_init(u32 mem_start)
{
}

void ads_ap_init(void)
{

}
BOOL ui_set_pic_show (BOOL show)
{
   return TRUE;
}
void ui_pic_stop(void)
{
}

BOOL ui_pic_play(ads_ad_type_t type, u8 root_id)
{
  return TRUE;
}
BOOL ui_check_pic_by_type(ads_ad_type_t type)
{
    return FALSE;
}
RET_CODE ui_check_fullscr_ad_play(void)
{
  return SUCCESS;
}

BOOL ui_is_pic_showing (void)
{
    return TRUE;
}



void pic_player_init(void)
{
}

void ui_create_adv(void)
{
}

void set_full_scr_ad_status(BOOL is_show)
{
  
 }

