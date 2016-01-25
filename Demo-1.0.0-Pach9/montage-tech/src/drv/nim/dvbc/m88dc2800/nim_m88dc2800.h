#ifndef __M88DC2800_H__
#define __M88DC2800_H__

#define MAX_BS_TIMES 3
#define MAX_TP_ONE_SCAN 50

#define X_TAL 28800
#define DC2800_SLEEP(n) (mtos_task_delay_ms (n))
#define DEMOD_I2C_ADDR 0x38
/* tuner default i2c address setting */ 
#define TUNER_TDCC_G051F_I2C_ADDR 	0xC0
#define  TUNER_I2C_ADDR_TC2800		0xc2
typedef enum
{
	TC2K_b2wAddress_C0 = 0xC0,
	TC2K_b2wAddress_C2 = 0xC2,
	TC2K_b2wAddress_C4 = 0xC4,
	TC2K_b2wAddress_C6 = 0xC6
} TC2K_b2wAddress;

typedef enum 
{
  TN_TDCC_G051F = 1,
  TN_TC2000,
  TN_TC2800,
  TN_XDCT6A,
}dc2800_tn_id_t;

typedef struct
 {
	dc2800_tn_id_t tn_id;
	s32 (*init)(void);
	u32 (*get_signal_strength)(void);
	s32 (*set_freq) (u32 freq_KHz, nim_modulation_t qam);
}dc2800_tn_drv_t;

typedef struct
{
  u32 x_crystal;
  nim_channel_info_t cur_channel;
  u8 dmd_addr;
  u8 chip_mode; // 0: new DC2800		1: new Jazz			2: old DC2800
  u8 tuner_loopthrough;
  u8 tuner_mode; //tuner mode if needed, 0: DVB-C, 2: MMDS
  u8 tuner_bandwidth;
}nim_m88dc280_priv_t;

extern i2c_bus_t *g_dc2800_i2c;
extern os_sem_t reg_rw_mutex;
extern dc2800_tn_drv_t g_dc2800_tn_drv;
extern nim_m88dc280_priv_t *g_p_dc2800_priv;

void dc2800_dmd_reg_write_unlock(u8 addr, u8 reg, u8 data);
u8 dc2800_dmd_reg_read_unlock(u8 addr, u8 reg);
void dc2800_dmd_reg_write(u8 addr, u8 reg, u8 data);
u8 dc2800_dmd_reg_read(u8 addr, u8 reg);
void dc2800_tn_reg_write(u8 addr, u8 reg, u8 data);
u8 dc2800_tn_reg_read(u8 addr, u8 reg);
s32 dc2800_tn_std_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_tn_seq_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_dmd_std_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_dmd_seq_read(u8 addr, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_tn_seq_write(u8 addr, u8 *p_buf, u32 len);
s32 dc2800_dmd_seq_write(u8 addr, u8 *p_buf, u32 len);

extern u16 parse_qam(nim_modulation_t qam_id);

#endif
