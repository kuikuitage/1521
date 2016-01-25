/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __AP_COMMON_H__
#define __AP_COMMON_H__

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "sys_devs.h"
//#include "ap_kingvon_ota.h"
#include "sys_cfg.h"

// c lib
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

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
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "iso_639_2.h"


// driver
#include "common.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"
//#include "cas_ware.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "vdec.h"
#include "display.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "dmx.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager32.h"
#include "data_manager_v3.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "monitor_service.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "ts_packet.h"
#include "eit.h"
#include "epg_data4.h"
#include "nvod_data.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"

#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "dvbs_util.h"
#include "ss_ctrl.h"
#include "smc_ctrl.h"
#include "mt_time.h"
#include "vbi_vsb.h"
#include "surface.h"

#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_base.h"
#include "ctrl_container.h"
#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"
#include "ctrl_progressbar.h"
#include "ctrl_matrixbox.h"
#include "ctrl_list.h"
#include "ctrl_combobox.h"
#include "ctrl_bitmap.h"
#include "ctrl_editbox.h"
#include "ctrl_common.h"
#include "ctrl_string.h"
#include "ctrl_numbox.h"
#include "ctrl_timebox.h"
#include "ctrl_setbox.h"

#include "framework.h"
#include "gui_script.h"

// prj misc
#include "sys_status.h"
#include "mem_cfg.h"

// ap
#include "ap_framework.h"
#include "ap_kingvon_ota.h"


#include "ap_uio.h"

// ui common
#include "ui_config.h"
#include "ui_desktop.h"
#include "ui_menu_manager.h"

#include "ui_ota_api.h"

// resource
#include "str_id.h"
#include "language_id.h"
#include "font_id.h"

#include "ui_gui_defines.h"


#endif

