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

#include "trdef.h"
#include "sys_dbg.h"

#define CHANGE_CHANNEL_PIC_MAXNUM    6
#define AD_DEBUG
/*!
  Declaration for ADS module's operation table
  */
extern ads_adapter_priv_t g_ads_priv;
ads_module_priv_t *ads_priv = NULL;

static u32 parse_ad_type(u32 ad_type)
{
  u32 tmp_type = UNDEFINED;
  switch(ad_type)
  {
    case ADS_AD_TYPE_WELCOME:
      tmp_type = LOGO_IFRAME;
      break;

    case ADS_AD_TYPE_RADIO:
      tmp_type = RADIO_PIC;
      break;

    case ADS_AD_TYPE_VOD:
      tmp_type =  NVOD_GIF;
      break;

    case ADS_AD_TYPE_EPG:
      tmp_type =  EPG_GIF;//4
      break;

    case ADS_AD_TYPE_MENU_UP:
      tmp_type =  MIANMENU_UP;
      break;

    case ADS_AD_TYPE_MENU_DOWN:
      tmp_type =  MIANMENU_DOWN;
      break;

    case ADS_AD_TYPE_BANNER:
      tmp_type =  BANNER_GIF;
      break;

    case ADS_AD_TYPE_CHANGE_CHANNEL:
      tmp_type =  CHANGE_CHANNEL;
      break;

    case ADS_AD_TYPE_OTHER_GIF:
      tmp_type = OTHERMENU_GIF;
      break;

    case ADS_AD_TYPE_URGENCY_NOTICE:
      tmp_type =  URGENCY_NOTICE;//15
      break;

    case ADS_AD_TYPE_VOLBAR:
      tmp_type =  VOLUME_GIF;//10
      break;

    case ADS_AD_TYPE_MANAGE_CHANNEL:
      tmp_type =  MANAGE_CHANNEL;//11
      break;

    case ADS_AD_TYPE_BROADCAST_LIST:
      tmp_type =  BROADCAST_LIST;//12
      break;

    case ADS_AD_TYPE_MAIN_MENU:
      tmp_type =  MAINMENU_MIDDLE;//16
      break;

    case ADS_AD_TYPE_CHLIST:
      tmp_type =  SORTCHANNEL_LIST;//14
      break;

    case ADS_AD_TYPE_CA:
      tmp_type =  CA_INFORMATION;//13
      break;

    case ADS_AD_TYPE_PIC:
      tmp_type =  HANGAD_GIF;//17
      break;

    case ADS_AD_MENU_ONE:
      tmp_type =  MENUAD_GIFONE;//18
      break;

    case ADS_AD_MENU_TWO:
      tmp_type =  MENUAD_GIFTWO;//19
      break;

    case ADS_AD_MENU_THREE:
      tmp_type =  MENUAD_GIFTHREE;//20
      break;

    case ADS_AD_MENU_FOUR:
      tmp_type =  MENUAD_GIFFOUR;//21
      break;

    default:
      break;
  }

  return tmp_type;
}

static RET_CODE ads_adt_tr_init(void *param)
{
  /* init Topreal ADS module */
  Init_AD_Task();
  /* init ad data */
  Init_MTR_AD_Data();

#ifdef AD_DEBUG
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif

  return SUCCESS;
}

static RET_CODE ads_adt_tr_deinit()
{
  return SUCCESS;
}

static RET_CODE ads_adt_tr_open()
{
  /* init ad data */
  //Init_MTR_AD_Data();

  return SUCCESS;
}

static RET_CODE ads_adt_tr_close()
{
  return SUCCESS;
}

static RET_CODE ads_adt_tr_display(void *param)
{
  MTRDRV_BOOL ret = 0;
  ads_module_cfg_t *p_cfg = (ads_module_cfg_t *)param;
  Lib_GIF_Position gif = {0};
  MTR_DRV_AdType type = parse_ad_type(p_cfg->ad_type);
  gif.X_x = p_cfg->pic_pos.x;
  gif.Y_y = p_cfg->pic_pos.y;
  gif.Type = type;
  switch(type)
  {
    case LOGO_IFRAME:
	MTR_AD_ShowLogo(LOGO_IFRAME);
	break;
    case RADIO_PIC:
	MTR_AD_ShowLogo(RADIO_PIC);
	break;
    case BANNER_GIF:
	ret = MTR_Show_Gif_AD(gif,p_cfg->service_info.service_id,p_cfg->pic_in);
	break;
    case CHANGE_CHANNEL:
	ret = MTR_Show_Gif_AD(gif,p_cfg->service_info.ts_id % CHANGE_CHANNEL_PIC_MAXNUM,p_cfg->pic_in);
	break;
    case NVOD_GIF:
    case EPG_GIF:
    case MIANMENU_UP:
    case MIANMENU_DOWN:
    case OTHERMENU_GIF:
    case VOLUME_GIF:               //音量条
    case MANAGE_CHANNEL:   //频道管理
    case BROADCAST_LIST:        //数据广播列表
    case CA_INFORMATION:      //Ca提示信息
    case SORTCHANNEL_LIST:   //喜爱/电视/广播节目列表
    case URGENCY_NOTICE:      //紧急通知
    case MAINMENU_MIDDLE: //主菜单广告二
    case HANGAD_GIF:               //挂角广告
    case MENUAD_GIFONE:      //菜单广告一
    case MENUAD_GIFTWO:      //菜单广告二
    case MENUAD_GIFTHREE:   //菜单广告三
    case MENUAD_GIFFOUR:     //菜单广告四
	      ret = MTR_Show_Gif_AD(gif,0,p_cfg->pic_in);
       break;
    default:
	break;
  }
#ifdef AD_DEBUG
  OS_PRINTF("%s %d ret:%d\n",__FUNCTION__, __LINE__,ret);
#endif

  return (ret == MTRDRV_TRUE)?SUCCESS : ERR_FAILURE;
}

static RET_CODE ads_adt_tr_hide(void *param)
{
  Clean_TRAD();
#ifdef AD_DEBUG
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#endif
  return SUCCESS;
}

static RET_CODE ads_adt_tr_io_ctrl(u32 cmd, void *param)
{
  MTRDRV_BOOL ret = 0;
  ads_module_cfg_t *p_cfg = (ads_module_cfg_t *)param;
  Lib_GIF_Position gif = {0};
  MTR_DRV_AdType type = parse_ad_type(p_cfg->ad_type);
  switch(cmd)
  {
    case ADS_IOCMD_AD_PIC_INFO_GET:
      gif.Type = type;
      gif.X_x = p_cfg->pic_pos.x;
      gif.Y_y = p_cfg->pic_pos.y;
      if(type == BANNER_GIF)
      {
      	  //ret = MTR_AD_have_this_gif_picture(gif,p_cfg->service_info.service_id,p_cfg->pic_in);
      	}
	else if(type == CHANGE_CHANNEL)
	{
	  //ret = MTR_AD_have_this_gif_picture(gif,p_cfg->service_info.ts_id % CHANGE_CHANNEL_PIC_MAXNUM,p_cfg->pic_in);
	}
	else
	{
	  //ret = MTR_AD_have_this_gif_picture(gif,0,p_cfg->pic_in);
	}
      break;
    case ADS_IOCMD_AD_SEARCH_SET:
       MTR_Start_SearchAD();
	break;
    default:
      break;
  }
#ifdef AD_DEBUG
  OS_PRINTF("%s %d cmd:%d,ret:%d\n",__FUNCTION__, __LINE__,cmd,ret);
#endif
  return ret;
}

RET_CODE ads_adt_tr_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
  g_ads_priv.adm_op[ADS_ID_ADT_TR].attached = 1;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].inited = 0;

  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.init
    = ads_adt_tr_init;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.deinit
    = ads_adt_tr_deinit;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.open
    = ads_adt_tr_open;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.close
    = ads_adt_tr_close;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.display
    = ads_adt_tr_display;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.hide
    = ads_adt_tr_hide;
  g_ads_priv.adm_op[ADS_ID_ADT_TR].func.io_ctrl
    = ads_adt_tr_io_ctrl;

  ads_priv = g_ads_priv.adm_op[ADS_ID_ADT_TR].p_priv
    = mtos_malloc(sizeof(ads_module_priv_t));
  MT_ASSERT(NULL != ads_priv);
  memset(ads_priv, 0x00, sizeof(ads_module_priv_t));

  ads_priv->id = ADS_ID_ADT_TR;
  ads_priv->p_dmx_dev = p_cfg->p_dmx_dev;
  ads_priv->flash_size = p_cfg->flash_size;
  ads_priv->flash_start_adr = p_cfg->flash_start_adr;
  ads_priv->nv_read = p_cfg->nvram_read;
  ads_priv->nv_write = p_cfg->nvram_write;
  ads_priv->nv_erase= p_cfg->nvram_erase;
  ads_priv->read_gif = p_cfg->read_gif;
  ads_priv->read_rec = p_cfg->read_rec;
  ads_priv->write_rec = p_cfg->write_rec;
  ads_priv->channel_frequency = p_cfg->channel_frequency;
  ads_priv->channel_symbolrate = p_cfg->channel_symbolrate;
  ads_priv->channel_qam = p_cfg->channel_qam;
  ads_priv->task_prio_start = p_cfg->task_prio_start;
  ads_priv->task_prio_end = p_cfg->task_prio_end;

  *p_adm_id = ADS_ID_ADT_TR;
#ifdef AD_DEBUG
  DEBUG(TRAD, INFO, "end.p_adm_id:%d\n", *p_adm_id);
#endif
  return SUCCESS;
}

