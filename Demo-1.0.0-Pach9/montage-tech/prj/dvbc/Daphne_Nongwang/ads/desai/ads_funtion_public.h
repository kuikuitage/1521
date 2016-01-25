#ifndef __ADS_FUNTION_PUBLIC_H__
#define __ADS_FUNTION_PUBLIC_H__

#define CONFIG_ADS_ID  CONFIG_ADS_ID_DESAI

void ads_mem_init(u32 mem_start);
void ads_ap_init(void);
u8* ads_get_ad_version(void);
#endif