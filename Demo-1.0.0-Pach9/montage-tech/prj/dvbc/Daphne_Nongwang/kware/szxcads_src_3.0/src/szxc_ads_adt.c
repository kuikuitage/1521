/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "pti.h"
#include "ads_ware.h"
#include "ads_adapter.h"

#include "szxcdef.h"

#define ADS_SZXC_ADT_PRINTF

#ifdef ADS_SZXC_ADT_PRINTF
    #define ADS_SZXC_PRINTF OS_PRINTF
#else
    #define ADS_SZXC_PRINTF DUMMY_PRINTF
#endif


#define CHANGE_CHANNEL_PIC_MAXNUM    6
#define GET_ADPID_MAX_TIMES 0x2FF
#define GET_ADV_MAX_TIMES 0x2FF
#define AD_DEBUG
/*!
  Declaration for ADS module's operation table
  */
extern ads_adapter_priv_t g_ads_priv;
ads_module_priv_t *p_ads_mod_priv = NULL;

extern u32 mtos_ticks_get(void);

 RET_CODE ads_adt_szxc_init(void *param)
{
   ADS_SZXC_PRINTF("[szxc] %s,%d \n",__FUNCTION__,__LINE__);
    u32 isLock = 0;
    u32 index = 0;
    s32 ret = ERR_FAILURE;
    ads_module_cfg_t *p_cfg = (ads_module_cfg_t *)param;
    u32 chanFrequency = p_cfg->channel_frequency;
    u32 chanSymbolRate = p_cfg->channel_symbolrate;
    u16 qammode = p_cfg->channel_qam;
    u8 pic_flag = 0;
    ADS_SZXC_PRINTF("[szxc] %s,%d \n",__FUNCTION__,__LINE__);

    isLock = szxc_drv_tuner_setparams (chanFrequency,
                      chanSymbolRate,qammode);
    if(!isLock)
    {
        ADS_SZXC_PRINTF("[szxc]%s %d ,nim unlock\n", __FUNCTION__, __LINE__);
        return ERR_TIMEOUT;
    }

    szxc_ads_client_init();

  OS_PRINTF("ticket = %d\n",mtos_ticks_get());
	szxc_drv_start_search_bat();

    for(index = 0; index < GET_ADPID_MAX_TIMES; index++)
    {
        ret = szxc_drv_check_pic_pid();
            if(SUCCESS == ret)
                break;
            else
                mtos_task_sleep(20);
      }

	if(SUCCESS == ret)
		szxc_drv_search_bat_end();
	else
	{
	 	szxc_drv_search_bat_end();
		ADS_SZXC_PRINTF("[szxc]%s %d ,not find ad pid\n", __FUNCTION__, __LINE__);
		return ERR_TIMEOUT;
	}
    OS_PRINTF("ticket = %d\n",mtos_ticks_get());
    OS_PRINTF("CALL szxc_drv_start_search_pic\n");
    OS_PRINTF("#####ticket = %d\n",mtos_ticks_get());
    szxc_drv_start_search_pic();

    for(index = 0; index < GET_ADV_MAX_TIMES; index++)
    {
        pic_flag = szxc_drv_check_pic_finish();
        if(pic_flag)
            break;
        else
            mtos_task_sleep(200);
      }
    OS_PRINTF("#####ticket = %d\n",mtos_ticks_get());
    OS_PRINTF("#####search pic ok\n");
    if(!pic_flag)
    {
        ADS_SZXC_PRINTF("[szxc]%s %d ,search pic wrong \n", __FUNCTION__, __LINE__);
        return ERR_TIMEOUT;
    }
   
   ret = szxc_picdata_to_flash();
   if(ret != SUCCESS)
        return ERR_TIMEOUT;
   ADS_SZXC_PRINTF("[szxc] write date to flash ok!\n");
   szxc_drv_start_search_hang();
   szxc_drv_start_search_txt();  
    return SUCCESS;
}

static RET_CODE ads_adt_szxc_deinit()
{
    ADS_SZXC_PRINTF("[szxc] %s,%d \n",__FUNCTION__,__LINE__);
    return SUCCESS;
}

static RET_CODE ads_adt_szxc_open()
{ 
    ADS_SZXC_PRINTF("[szxc] %s,%d \n",__FUNCTION__,__LINE__);
    return SUCCESS;
}

static RET_CODE ads_adt_szxc_close()
{
    ADS_SZXC_PRINTF("[szxc]%s %d\n",__FUNCTION__, __LINE__);
    return SUCCESS;
}


static RET_CODE ads_adt_szxc_io_ctrl(u32 cmd, void *param)
{
    BOOL ret = SUCCESS;
    u16 service_id = 0;
    u8 *pos_txt  = NULL;
    ads_module_cfg_t *p_cfg = (ads_module_cfg_t *)param;
    service_id = p_cfg->service_info.service_id;
    OS_PRINTF("\n ads_adt_szxc_io_ctrl in \n");
     ADS_SZXC_PRINTF("[szxc]%s %d CMD= %dn",__FUNCTION__, __LINE__,cmd);
    switch(cmd)
    {
        case ADS_IOCMD_AD_OPEN_LOGO:
            OS_PRINTF("ADS_IOCMD_AD_OPEN_LOGO\n");
            szxc_ad_show_logo(AD_SZXC_AD_START);   
            break;
        case ADS_IOCMD_AD_CLOSE_LOGO:
            OS_PRINTF("ADS_IOCMD_AD_CLOSE_LOGO\n");
            szxc_ad_show_logo(AD_SZXC_AD_POWER_OFF);
            break;
        case ADS_IOCMD_AD_MES_PIC:
            OS_PRINTF("ADS_IOCMD_AD_MES_PIC\n");
            szxc_ad_show_pic(AD_SZXC_AD_MES, service_id);
            break;
        case ADS_IOCMD_AD_HOME_PIC:
           OS_PRINTF("ADS_IOCMD_AD_HOME_PIC\n");
           szxc_ad_show_pic(AD_SZXC_AD_HOME, service_id);
            break;
        case ADS_IOCMD_AD_VOLUM_PIC:
           OS_PRINTF("ADS_IOCMD_AD_VOLUM_PIC\n");
           szxc_ad_show_pic(AD_SZXC_AD_VOLBAR, service_id);
            break;
        case ADS_IOCMD_AD_BROAD_PIC:
            OS_PRINTF("ADS_IOCMD_AD_BROAD_PIC\n");
            szxc_ad_show_pic(AD_SZXC_AD_BROADCAST, service_id);
            break;
        case ADS_IOCMD_AD_EPG_PIC:
            OS_PRINTF("ADS_IOCMD_AD_EPG_PIC\n");
            szxc_ad_show_pic(AD_SZXC_AD_EPG, service_id);
            break;
        case ADS_IOCMD_AD_CHANNEL_PIC:
            OS_PRINTF("ADS_IOCMD_AD_CHANNEL_PIC\n");
            szxc_ad_show_pic(AD_SZXC_AD_CHANNEL_LIST, service_id);
            break;
        case ADS_IOCMD_AD_CLEAR_FLAG:
            OS_PRINTF("ADS_IOCMD_AD_CLEAR_FLAG\n");
            pos_txt = (u8 *)param;
            szxc_clear_txt_flag(*pos_txt);
            break;
        default:
            break;
    }
    ADS_SZXC_PRINTF("[szxc] %s %d cmd:%d,ret:%d\n",__FUNCTION__, __LINE__,cmd,ret);
    OS_PRINTF("\nads_adt_szxc_io_ctrl out\n");
    return ret;
}

RET_CODE ads_adt_szxc_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
       ADS_SZXC_PRINTF("[szxc] %s %d\n",__FUNCTION__, __LINE__);
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].attached = 1;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].inited = 0;

    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.init
            = ads_adt_szxc_init;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.deinit
        = ads_adt_szxc_deinit;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.open
            = ads_adt_szxc_open;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.close
            = ads_adt_szxc_close;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.display
            = NULL;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.hide
            = NULL;
    g_ads_priv.adm_op[ADS_ID_ADT_SZXC].func.io_ctrl
            = ads_adt_szxc_io_ctrl;

    p_ads_mod_priv = g_ads_priv.adm_op[ADS_ID_ADT_SZXC].p_priv
            = mtos_malloc(sizeof(ads_module_priv_t));
    MT_ASSERT(NULL != p_ads_mod_priv);
    memset(p_ads_mod_priv, 0x00, sizeof(ads_module_priv_t));

    p_ads_mod_priv->id = ADS_ID_ADT_SZXC;
    p_ads_mod_priv->p_dmx_dev = p_cfg->p_dmx_dev;
    p_ads_mod_priv->flash_size = p_cfg->flash_size;
    p_ads_mod_priv->flash_start_adr = p_cfg->flash_start_adr;
    p_ads_mod_priv->nv_read = p_cfg->nvram_read;
    p_ads_mod_priv->nv_write = p_cfg->nvram_write;
    p_ads_mod_priv->nv_erase = p_cfg->nvram_erase;
    p_ads_mod_priv->display_logo = p_cfg->display_logo;
    p_ads_mod_priv->display_ad = p_cfg->display_ad;
    p_ads_mod_priv->get_screen_statu = p_cfg->get_screen_statu;
    p_ads_mod_priv->hide_ad = p_cfg->hide_ad;
    p_ads_mod_priv->channel_frequency = p_cfg->channel_frequency;
    p_ads_mod_priv->channel_symbolrate = p_cfg->channel_symbolrate;
    p_ads_mod_priv->channel_qam = p_cfg->channel_qam;
    p_ads_mod_priv->task_prio_start = p_cfg->task_prio_start;
    p_ads_mod_priv->task_prio_end = p_cfg->task_prio_end;

    *p_adm_id = ADS_ID_ADT_SZXC;
  
    ADS_SZXC_PRINTF("[szxc] %s %d\n",__FUNCTION__, __LINE__);
    return SUCCESS;
}

