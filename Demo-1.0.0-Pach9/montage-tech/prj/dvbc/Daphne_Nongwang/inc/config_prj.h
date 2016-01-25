#ifndef __CONFIG_PRJ_H__
#define __CONFIG_PRJ_H__
void on_cas_init(void);
#ifndef WIN32
void set_volume_mute_by_hardware(u8 onoff);
void nim_init_config(nim_config_t *nim_cfg);
void smc_init_config(scard_open_t *smc_op_p);
void power_on_led_display(void);
void power_off_led_display(void);
void signal_led_display(u8 is_locked);
void irda_led_display(u8 flag);
void email_led_display(u8 flag);
void uio_init_config(uio_cfg_t *uiocfg);
void standby_config(lpower_cfg_t *lpower_cfg);
void set_ota_config(void);
#endif
#endif //__SYS_DEVS_H__

