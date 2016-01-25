/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "lib_rect.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "pti.h"
#include "nim.h"
#include "dmx.h"
//#include "ui_util_api.h"

#include "ads_ware.h"
#include "../inc/ads_adapter.h"
#include "../inc/ads_api_xsm.h"

#define ADS_XSM_ADT_PRINTF

#ifdef ADS_XSM_ADT_PRINTF
    #define ADS_ADT_PRINTF OS_PRINTF
#else
    #define ADS_ADT_PRINTF DUMMY_PRINTF
#endif

#define GET_AD_PID_MAX_TIMES    0x2FF

extern ads_adapter_priv_t g_ads_priv;

ads_module_priv_t *p_ads_xsm_priv = NULL;



extern void xsm_ads_client_init(void);


RET_CODE ads_adt_xsm_init(void *param)
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

    
  //start monitor task
  xsm_ads_client_init();
  
  //Init
  YTDY_AD_init();

  
  YTDY_AD_Open();



  return SUCCESS;
}

RET_CODE ads_adt_xsm_deinit()
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  return SUCCESS;
}


RET_CODE ads_adt_xsm_open()
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  return SUCCESS; 
}

RET_CODE ads_adt_xsm_close()
{
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
    return SUCCESS;
}

static E_YTDY_AD_SHOW_TYPE xsm_ad_type_get(u32 ads_type)
{
    E_YTDY_AD_SHOW_TYPE xsm_ad_type = 0x00;

    switch(ads_type)
    {
        case ADS_AD_TYPE_CHLIST:
            xsm_ad_type = AD_EPG;
            break;

        case ADS_AD_TYPE_WELCOME:
            xsm_ad_type = AD_OPEN_LOGO;
            break;

        case ADS_AD_TYPE_VOLBAR:
            xsm_ad_type = AD_VOLUME;
            break;
        case ADS_AD_TYPE_OTHER_GIF:
            xsm_ad_type = AD_FS_RIGHT;
            break;
        case ADS_AD_TYPE_MAIN_MENU:
        case ADS_AD_TYPE_MENU_UP:
        case ADS_AD_TYPE_MENU_DOWN:
            xsm_ad_type = AD_MAIN_MENU;
            break;
        case ADS_AD_TYPE_CHBAR:
        case ADS_AD_TYPE_BANNER:
            xsm_ad_type = AD_PF_INFO;
            break;
    }

    return xsm_ad_type;
}

RET_CODE ads_adt_xsm_io_ctrl(u32 cmd, void *param)
{
  RET_CODE ret = SUCCESS;
  return ret;
}

extern s32 ads_adt_ad_show(E_YTDY_AD_SHOW_TYPE show_type,u16 service_id, u16 ts_id);

static RET_CODE ads_adt_xsm_display(void *param)
{
  RET_CODE err = -1;
  ads_module_cfg_t *p_cfg = (ads_module_cfg_t *)param;
  //Lib_GIF_Position gif = {0};
  OS_PRINTF("ads_adt_xsm_display ad_type = %d\n",p_cfg->ad_type);
  
  E_YTDY_AD_SHOW_TYPE type = xsm_ad_type_get(p_cfg->ad_type);
  
  OS_PRINTF("service_id = %d, ts_id = %d\n",
        p_cfg->service_info.service_id,p_cfg->service_info.ts_id);
  
  err = ads_adt_ad_show(type,p_cfg->service_info.service_id,p_cfg->service_info.ts_id);


  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);


  return err;
}

static RET_CODE ads_adt_xsm_hide(void *param)
{
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
  return SUCCESS;
}

RET_CODE ads_adt_xsm_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

  g_ads_priv.adm_op[ADS_ID_ADT_XSM].attached = 1;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].inited = 0;

  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.init
    = ads_adt_xsm_init;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.deinit
    = ads_adt_xsm_deinit;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.open
    = ads_adt_xsm_open;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.close
    = ads_adt_xsm_close;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.display
    = ads_adt_xsm_display;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.hide
    = ads_adt_xsm_hide;
  g_ads_priv.adm_op[ADS_ID_ADT_XSM].func.io_ctrl
    = ads_adt_xsm_io_ctrl;

  p_ads_xsm_priv = g_ads_priv.adm_op[ADS_ID_ADT_XSM].p_priv
  = mtos_malloc(sizeof(ads_module_priv_t));
  MT_ASSERT(NULL != p_ads_xsm_priv);
  memset(p_ads_xsm_priv, 0x00, sizeof(ads_module_priv_t));

  p_ads_xsm_priv->id = ADS_ID_ADT_XSM;
  p_ads_xsm_priv->p_dmx_dev = p_cfg->p_dmx_dev;
  p_ads_xsm_priv->flash_size = p_cfg->flash_size;
  p_ads_xsm_priv->flash_start_adr = p_cfg->flash_start_adr;
  p_ads_xsm_priv->nv_read = p_cfg->nvram_read;
  p_ads_xsm_priv->nv_write = p_cfg->nvram_write;
  p_ads_xsm_priv->nv_erase2 = p_cfg->nvram_erase2;
  p_ads_xsm_priv->read_gif = p_cfg->read_gif;
  
  p_ads_xsm_priv->read_rec = p_cfg->read_rec;
  p_ads_xsm_priv->write_rec = p_cfg->write_rec;
  p_ads_xsm_priv->display_ad = p_cfg->display_ad;
  p_ads_xsm_priv->hide_ad = p_cfg->hide_ad;
  p_ads_xsm_priv->channel_frequency = p_cfg->channel_frequency;
  p_ads_xsm_priv->channel_symbolrate = p_cfg->channel_symbolrate;
  p_ads_xsm_priv->channel_qam = p_cfg->channel_qam;
  p_ads_xsm_priv->task_prio_start = p_cfg->task_prio_start;
  p_ads_xsm_priv->task_prio_end = p_cfg->task_prio_end;  

  *p_adm_id = ADS_ID_ADT_XSM;

   return SUCCESS;
}


