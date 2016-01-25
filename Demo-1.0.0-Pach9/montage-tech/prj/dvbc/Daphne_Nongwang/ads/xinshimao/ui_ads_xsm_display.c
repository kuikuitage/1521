/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "ui_common.h"

#include "ui_ads_xsm_display.h"
#include "ui_ad_logo_api.h"
#include "ui_config.h"
#include "pdec.h"

static BOOL is_ads_logo_stored = FALSE;
static BOOL is_full_screen_ad_enable = FALSE;
static BOOL is_ads_osd_roll = FALSE;

static u8 uc_current_osd_index = 0;
//static ST_ADS_OSD_PROGRAM_SHOW_INFO osd_prom_show_info;

static ST_ADS_OSD_PROGRAM_SHOW_INFO osd_prom_show_info;

static BOOL is_full_screen_ad_on_showing = FALSE;
static BOOL is_full_screen_ad_need_display = FALSE;

//static ST_AD_XSM_PICTURE_SHOW_INFO * pic_full_scr_info = NULL;

extern void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos);
extern RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);
static RET_CODE nvram_ad_logo_write(u32 offset, u8 *p_buf, u32 size)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  RET_CODE ret = FALSE;
  void *p_dm_handle = NULL;
  u32 block_size;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_LOGO_BLOCK_ID);
  OS_PRINTF("ad write addr[0x%x], size[0x%x], block_size[ox%x]\n", offset, size, block_size);
  if((size!=0) && (size <= block_size))
  {
    ret=dm_direct_write(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID,offset,size,p_buf);
    MT_ASSERT(ret != DM_FAIL);
  }
  OS_PRINTF("AD nvram_write end\n");

  return TRUE;
}

//read the ad data from flash
RET_CODE nvram_ad_logo_read(u32 offset, u8 *p_buf, u32 *p_size)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  void *p_dm_handle = NULL;
  u32 nvm_read =0;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  OS_PRINTF("ad read dm_direct_read addr[0x%x], size[0x%x]\n", offset,*p_size);

  nvm_read=dm_direct_read(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID,offset,*p_size,p_buf);
  OS_PRINTF("ad nvm_read[0x%x]\n", nvm_read);
  
  OS_PRINTF("AD nvram_read success!\n");

  return TRUE;
}

static RET_CODE nvram_ad_logo_erase(u32 offset, u32 size)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  OS_PRINTF("ADS:Erase size_pre =%d\n", size);

  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID,offset, size);
  MT_ASSERT(ret != DM_FAIL);

  return TRUE;
}

void ui_adv_get_osd_msg(u16 pg_id)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  ads_module_cfg_t module_config = {0};
  dvbs_prog_node_t pg = {0};
  RET_CODE ret = SUCCESS;

  if(pg_id != INVALIDID)
  {
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      OS_PRINTF("ADV ui_adv_set_pic_by_type SID[%d] TS_ID[%d] NET_ID[%d]\n",pg.s_id, pg.ts_id,pg.orig_net_id);
      module_config.service_info.service_id = (u16)pg.s_id;
      module_config.service_info.ts_id = (u16)pg.ts_id;
      module_config.service_info.network_id= (u16)pg.orig_net_id;
    }  
  }
  
  ret = ads_io_ctrl(ADS_ID_ADT_XSM,ADS_IOCMD_AD_OSD,&module_config);
  UI_PRINTF("ui_adv_get ads osd osd msg ret = %d\n", ret);
  return;
}

//desai ads display func
void ads_xsm_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg)
{
  OS_PRINTF("%s:LINE%d, get in, type=%d\n", __func__, __LINE__, type); 
  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
    case ADS_AD_TYPE_MENU_DOWN:
    case ADS_AD_TYPE_MAIN_MENU:
    case ADS_AD_TYPE_SUB_MENU:
    case ADS_AD_TYPE_OTHER_GIF:
    case ADS_AD_TYPE_SEARCHING:
    case ADS_AD_TYPE_SIGNAL:
      p_cfg->ad_type = ADS_AD_TYPE_MAIN_MENU;
      *cmd = ADS_IOCMD_AD_NO_PROGRAM;
      break;
    case ADS_AD_TYPE_CHLIST:
    case ADS_AD_TYPE_CHBAR:
      p_cfg->ad_type =  ADS_AD_TYPE_CHLIST;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
    case ADS_AD_TYPE_VOLBAR:
      p_cfg->ad_type =  ADS_AD_TYPE_VOLBAR;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
    default:
      break;           
  }
}


RET_CODE ui_check_fullscr_ad_play(void)
{
OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 

  RET_CODE ret = ERR_FAILURE;
#if 0
  if(FALSE == get_full_scr_ad_status())
  {   
    return ret;
  }
  if((pic_full_scr_info->uiDataLen == 0) ||
     (pic_full_scr_info->pucPicData == NULL) ||
     (XSMAD_PIC_GIF != pic_full_scr_info->enPicType))
  {
    return ret;
  }
  //fullscreen ad
  UI_PRINTF("[UI_ADS_DIS] fw_get_focus_id()= %d \n",fw_get_focus_id());
  UI_PRINTF("[show full screen ad] pic_full_scr_info = 0x%x, pic_show_info->uiDataLen = %d\n",pic_full_scr_info,
                          pic_full_scr_info->uiDataLen);
  if((is_full_screen_ad_need_display == TRUE) && (ROOT_ID_BACKGROUND == fw_get_focus_id()))
  {
    ui_pic_stop();
    //ui_adv_set_fullscr_rect(pic_full_scr_info->usStartX, pic_full_scr_info->usStartY, 
                                            pic_full_scr_info->usWidth, pic_full_scr_info->usHeigth);     
    ret = ui_adv_set_pic(pic_full_scr_info->uiDataLen, pic_full_scr_info->pucPicData);

    ui_set_pic_show(TRUE);
    is_full_screen_ad_on_showing = TRUE;
  }
#endif
  return ret;
}

void ui_set_full_scr_ad_status(BOOL status)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  is_full_screen_ad_on_showing = status;
}

#if 0
BOOL ui_set_ads_osd_status(BOOL status)
{
  is_ads_osd_roll = status;
}
#endif

BOOL ui_get_ads_osd_status(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  return is_ads_osd_roll;
}

void ui_ads_osd_roll(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  #if 1
  S_YTDY_OSD_DATA *osd_show_info = NULL;
  u16 uni_str[256] = {0};

  osd_show_info = (S_YTDY_OSD_DATA *)&(osd_prom_show_info.stTextShow);
  
  if((osd_show_info->m_chAdContent == NULL))
  {
    OS_PRINTF("%s:LINE%d, content=NULL\n", __func__, __LINE__); 
    uc_current_osd_index = 0;
    ui_ads_osd_roll_stop();
    return;
  }
  gb2312_to_unicode((osd_show_info->m_chAdContent), osd_show_info->m_bigOsdLen, uni_str, MAX_OSD_TEXT_SIZE);
  
  #if 0
  UI_PRINTF("[AD_OSD]osd_prom_show_info.ucOsdNum = %d \n",osd_prom_show_info.ucOsdNum);
  for(i=0;i<osd_prom_show_info.ucOsdNum;i++)
  UI_PRINTF("[AD_OSD] **** osd_show_info[%d]->pucDisplayBuffer = %s \n",i,osd_prom_show_info.stTextShow[i].pucDisplayBuffer);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer = %s \n",osd_show_info->pucDisplayBuffer);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->enDisplayPos = %d \n",osd_show_info->enDisplayPos);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer[0] = %d \n",osd_show_info->pucDisplayBuffer[0]);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->usGetBufferSize = %d \n",osd_show_info->usGetBufferSize);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->usContentSize = %d \n",osd_show_info->usContentSize);
  //UI_PRINTF("[AD_OSD] **** osd_show_info->ucShowTimes = %d \n",osd_show_info->ucShowTimes);
  #endif
  
  /******************************************************************************
  //不判断CA滚动字幕状态，直接显示广告滚动，
  //在显示完所有广告滚动字幕螅送一条滚动结束的消息给CA
  //目前只滚动一次，且仅在屏幕上方显示
  ******************************************************************************/
  //if(TRUE == get_ca_msg_rolling_status())    

  UI_PRINTF("[AD_OSD] no ca msg on rolling, roll ad msg now \n");
  on_desktop_start_roll(uni_str, 0, 1, OSD_SHOW_TOP);
  set_ca_msg_rolling_status(FALSE);
  
  #endif
}

void ui_ads_osd_roll_stop(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  is_ads_osd_roll = FALSE;
  
  on_ca_rolling_over(fw_find_root_by_id(ROOT_ID_BACKGROUND), 0, 0, 0);
}

RET_CODE ui_show_xsm_osd_ad(u8 *p_data)
{
    OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
    //RET_CODE ret = ERR_FAILURE;
    //u8 uc_ads_osd_num = 0;
    ST_ADS_OSD_PROGRAM_SHOW_INFO * pst_osd_ads_roll = NULL;

    pst_osd_ads_roll = (ST_ADS_OSD_PROGRAM_SHOW_INFO *)p_data;
    if(pst_osd_ads_roll == NULL)
    {
        UI_PRINTF("[AD_OSD] ads osd null!! \n");
        return ERR_FAILURE;
    }  
    memset((void *)&osd_prom_show_info, 0, sizeof(ST_ADS_OSD_PROGRAM_SHOW_INFO));
    memcpy((void *)&osd_prom_show_info, (void *)pst_osd_ads_roll, sizeof(ST_ADS_OSD_PROGRAM_SHOW_INFO));

    //uc_ads_osd_num = osd_prom_show_info.ucOsdNum;
    /*
    if(uc_ads_osd_num == 0)
    {
        UI_PRINTF("[ad_osd] ads osd num is 0, return \n");
        //return ERR_FAILURE;
    }
    */
    if(pst_osd_ads_roll->stTextShow.m_chAdContent != NULL)
    {
        osd_prom_show_info.b_SaveFlag = TRUE;
    }
    else
        osd_prom_show_info.b_SaveFlag = FALSE;
    
    //set current roll index to 1st osd msg
    uc_current_osd_index = 0;

    OS_PRINTF("%s:LINE%d, saveflag=[%d]\n", 
                        __func__, __LINE__, osd_prom_show_info.b_SaveFlag); 
    
    //ads osd display
    if(FALSE == get_ca_msg_rolling_status())   
    {
        OS_PRINTF("%s:LINE%d, rolling status is false!\n", __func__, __LINE__); 
        ui_ads_osd_roll_stop();
        return ERR_FAILURE;
    }
    ui_ads_osd_roll();

    is_ads_osd_roll = TRUE;

    return SUCCESS;
}

static RET_CODE ui_show_xsm_pic_ad(u8 *p_data)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  RET_CODE ret = ERR_FAILURE;
#if 0
  ST_AD_XSM_PICTURE_SHOW_INFO * pic_show_info = NULL;
  pic_show_info = (ST_AD_XSM_PICTURE_SHOW_INFO*)p_data;
  
  UI_PRINTF("[UI]: pic_show_info-->enPicType = %d \n",pic_show_info->enPicType);
  
  if(pic_show_info == NULL || pic_show_info->pucPicData == NULL || pic_show_info->uiDataLen == 0)
    return ERR_FAILURE;
  #if 0
    //set dec mode
    if(DSAD_PIC_JPG == pic_show_info->enPicType)
    {
      set_dec_mode(DEC_ALL_MODE);
      return SUCCESS;
    }
    else
    {
      set_dec_mode(DEC_FRAME_MODE);
    }
  #endif

  // fullscreen ad interface
  if(XSMAD_STB_FULLSCREEN == pic_show_info->enPosType)
  {
    UI_PRINTF("[show full screen ad] get_full_scr_ad_status= %d \n",get_full_scr_ad_status());
    UI_PRINTF("[show full screen ad] p_data = 0x%x, pic_show_info->uiDataLen = %d\n",
                          p_data, pic_show_info->uiDataLen);
    UI_PRINTF("[full screen ad rect] x= %d, y=%d, w= %d, h = %d \n",
                        pic_show_info->usStartX, pic_show_info->usStartY,
                       pic_show_info->usWidth, pic_show_info->usHeigth);

      pic_full_scr_info = (ST_AD_XSM_PICTURE_SHOW_INFO*)p_data;
      is_full_screen_ad_need_display = TRUE;
      
      ret = ui_check_fullscr_ad_play();
      return ret;
  }
  else      // other ads osd display
  {
    if(XSMAD_PIC_GIF == pic_show_info->enPicType)
    {
      ret = ui_adv_set_pic(pic_show_info->uiDataLen, pic_show_info->pucPicData);
    }
    is_full_screen_ad_on_showing = FALSE;

  }
#endif
  return ret;
}
static RET_CODE ui_save_logo_data_to_nvm(ST_AD_XSM_AV_SHOW_INFO * av_show_info)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  RET_CODE ret = ERR_FAILURE;
  u32 offset = 0;
  ST_AD_XSM_LOG_SAVE_INFO logo_info;

  memset(&logo_info, 0, sizeof(logo_info));
  logo_info.ucLogType = av_show_info->enAvType;
  logo_info.uiLogoSize = av_show_info->uiDataLen;
  
  //save logo info:type + len
  nvram_ad_logo_erase(offset, sizeof(logo_info));
  nvram_ad_logo_write(offset, (u8 *)&logo_info, sizeof(logo_info));

  //save logo data
  offset += sizeof(logo_info);
  nvram_ad_logo_erase(offset, av_show_info->uiDataLen);
  ret = nvram_ad_logo_write(offset, av_show_info->pucAvData,av_show_info->uiDataLen);

  if(ret == TRUE)
    is_ads_logo_stored = TRUE;
  return ret;
}
static RET_CODE ui_show_xsm_vid_ad(u8 *p_data)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  RET_CODE ret = SUCCESS;
  ST_AD_XSM_AV_SHOW_INFO * av_show_info = NULL;
  av_show_info = (ST_AD_XSM_AV_SHOW_INFO*)p_data;

  //ads osd display
  switch(av_show_info->enPosType)
  {
    case XSMAD_STB_LOG:
    {
      UI_PRINTF("[ADS_DIS] Desai ads show logo:av_show_info.enAvType = %d \n",av_show_info->enAvType);
      UI_PRINTF("[ADS_DIS] Desai ads show:av_show_info->uiDataLen = %d  \n",av_show_info->uiDataLen);
      
#if 0
      ui_show_logo_by_data(av_show_info->uiDataLen, av_show_info->pucAvData);
#endif

      //save logo info
      ui_save_logo_data_to_nvm(av_show_info);
      break;
    }
    case XSMAD_STB_FULLSCREEN:
    case XSMAD_STB_EPG:
    case XSMAD_STB_MENU:
    case XSMAD_STB_VOLUME:
    case XSMAD_STB_POS_UNKNOWN:
      break;
    default:
      break;
  }
  
  return ret;
}

BOOL get_ads_logo_status(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  return  is_ads_logo_stored;
 }

BOOL get_full_scr_ad_status(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  return  is_full_screen_ad_enable;
 }

void set_full_scr_ad_status(BOOL is_show)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  is_full_screen_ad_enable = is_show;
 }

RET_CODE ui_adv_xsm_display(ads_ad_type_t ad_type, u8 *p_data)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  RET_CODE ret = ERR_FAILURE;

  if(p_data == NULL)
  {
    UI_PRINTF("ADS: ads desai p_data null \n");
    return ret;
  }
  switch(ad_type)
  {
    case ADS_AD_TYPE_OSD:
    {
      UI_PRINTF("ADS: Desai ad type [OSD_TYPE] \n");
      ret = ui_show_xsm_osd_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_PIC:
    {
      UI_PRINTF("ADS: Desai ad type [PIC_TYPE] \n");
      ret = ui_show_xsm_pic_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_AV:
    {
      UI_PRINTF("ADS: Desai ad type [AV_TYPE] \n");
      ret = ui_show_xsm_vid_ad(p_data);
      break;
    }
    default:
    break;
    }
  return ret;
}

RET_CODE ui_adv_xsm_hide(ads_ad_type_t ad_type, u8 *p_data)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
//hide fullscreen ad
  if(is_full_screen_ad_on_showing && ui_is_pic_showing())
  {
    UI_PRINTF("[UI_ADS_DIS] hide fullscreen ad \n");
    ui_pic_stop();
    is_full_screen_ad_need_display = FALSE;
    is_full_screen_ad_on_showing = FALSE;
  }
  return SUCCESS;
}


