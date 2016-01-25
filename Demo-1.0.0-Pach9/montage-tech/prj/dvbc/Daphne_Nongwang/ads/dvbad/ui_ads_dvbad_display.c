/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "ui_common.h"
#include "ads_xml_parser.h"
#include "ui_ads_display.h"
#include "ui_ad_logo_api.h"
#include "ui_config.h"
#include "ads_ware.h"
#include "pdec.h"
#include "fcrc.h"
#include "ui_signal.h"

static BOOL is_ads_logo_stored = FALSE;
static BOOL is_full_screen_ad_enable = FALSE;
static BOOL is_unauth_ad_enable = FALSE;

static BOOL is_ads_osd_roll = FALSE;

static u8 uc_current_osd_index = 0;
static dvbad_osd_program_show_info osd_prom_show_info;
static ST_ADS_OSD_INFO osd_display_info[MAX_ROLL_OSD_NUM];

static BOOL is_full_screen_ad_on_showing = FALSE;
static BOOL is_full_screen_ad_need_display = FALSE;

static dvbad_show_pic_info * pic_full_scr_info = NULL;
//static dvbad_show_pic_info * pic_unauth_ad_info = NULL;
//static BOOL is_unauth_ad_on_showing = FALSE;

extern void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos);
extern RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);
static void nvm_ad_block_crc_init(void)
{
  RET_CODE ret;
  
   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret); 
}

static u32 nvm_ad_block_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
  
}
static RET_CODE nvram_ad_logo_write(dvbad_av_show_info * av_show_info)
{
  RET_CODE ret = FALSE;
  void *p_dm_handle = NULL;
  u32 offset = 0;
  u8 *p_buf = NULL;
  u32 size = 0;
  u32 block_size;
  u32 write_length = 0;
  u8 *p_block_buffer = NULL;
  u32 crc_32_read = 0;
  u32 crc_32_write = 0;
  u32 crc_32_check = 0;
  u8 *p_read_crc_buffer = NULL;
  u8  p_write_crc_buffer[4] ;
  dvbad_log_save_info *r_logo_info = NULL;
  dvbad_log_save_info w_logo_info;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_LOGO_BLOCK_ID);

  p_buf = av_show_info->pucAvData;
  size = av_show_info->uiDataLen;
  
  UI_PRINTF("ad write addr[0x%x], size[0x%x], block_size[ox%x]\n", offset, size, block_size);
  
  if((size!=0) && (size <= block_size - sizeof(dvbad_log_save_info) - 4))
  {
    p_block_buffer = mtos_malloc(block_size);
    if(p_block_buffer == NULL)
    {
       return ERR_FAILURE; 
    }
    
    memset(p_block_buffer, 0, block_size);
    
    dm_direct_read(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID, 0, block_size, (u8*)p_block_buffer);

    r_logo_info = (dvbad_log_save_info*)p_block_buffer;
    if(r_logo_info->uiLogoSize == size)
    {
      //check crc
      //read orig data and crc info 
      p_read_crc_buffer = (u8*)(p_block_buffer + sizeof(dvbad_log_save_info) + r_logo_info->uiLogoSize);
      UI_PRINTF("### %s read crc[0] =0x%x,crc[1] =0x%x,crc[2] =0x%x,crc[3] =0x%x, \n",
                          __FUNCTION__,*p_read_crc_buffer,*(p_read_crc_buffer + 1),*(p_read_crc_buffer + 2),*(p_read_crc_buffer + 3));
      crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                         +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));
      
      //calc new data crc info
      nvm_ad_block_crc_init();
      crc_32_check = nvm_ad_block_generate(p_buf, size);
      
      UI_PRINTF("### %s read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32_read,crc_32_check);
      //same crc,don't wirte, return;
      if(crc_32_read == crc_32_check)
      {
        UI_PRINTF("adv_logo:write data is same with data stored in flash, don't to write flash\n",__FUNCTION__);
        mtos_free(p_block_buffer);
        return SUCCESS;
      }   

    }
    
    memset(p_block_buffer,0,block_size);

    memset(&w_logo_info, 0, sizeof(w_logo_info));
    w_logo_info.ucLogType = av_show_info->enAvType;
    w_logo_info.uiLogoSize = av_show_info->uiDataLen;
    
    //logo info write
    memcpy(p_block_buffer,&w_logo_info,sizeof(dvbad_log_save_info));
    UI_PRINTF("### sizeof(dvbad_log_save_info) = 0x%x\n",sizeof(dvbad_log_save_info));
    /***rewrite data***/
    memcpy(p_block_buffer+sizeof(dvbad_log_save_info), p_buf, size);
    
    /***reback crc data***/
    nvm_ad_block_crc_init();
    crc_32_write = nvm_ad_block_generate(p_buf, size);
    
    p_write_crc_buffer[0] = (crc_32_write >>24)&0xff;
    p_write_crc_buffer[1] = (crc_32_write >>16)&0xff;
    p_write_crc_buffer[2] = (crc_32_write >>8)&0xff;
    p_write_crc_buffer[3] = (crc_32_write >>0)&0xff;
    memcpy(p_block_buffer+sizeof(dvbad_log_save_info)+size, p_write_crc_buffer, 4);

    write_length = sizeof(dvbad_log_save_info) + size + 4;
    if(write_length > block_size)
    {
      mtos_free(p_block_buffer);
      return ERR_FAILURE; 
    }
    ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID,0, block_size);
    MT_ASSERT(ret != DM_FAIL);

    UI_PRINTF("AD nvram_write write_length = 0x%x !! \n",write_length);
    ret=dm_direct_write(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID, offset, write_length, p_block_buffer);
    MT_ASSERT(ret != DM_FAIL);
    
    UI_PRINTF("AD nvram_write success !! \n");
    
    mtos_free(p_block_buffer);
    return SUCCESS;
  }
  else
  {
    UI_PRINTF("AD nvram_write fail, data lenth too long !! \n");
    return ERR_FAILURE; 
  }

}

//read the ad data from flash
RET_CODE nvram_ad_logo_read(u32 offset, u8 *p_buf, u32 size)
{
  void *p_dm_handle = NULL;
  u32 nvm_read =0;
  u32 crc_32_read = 0;
  u32 crc_32_check = 0;
  u8 *p_read_crc_buffer = NULL;
  dvbad_log_save_info *r_logo_info = NULL;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  if(p_buf == NULL)
  {
     return ERR_FAILURE; 
  }
  memset(p_buf,0,size);
  
#if 0
  p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
  if(p_block_buffer == NULL)
  {
     return ERR_FAILURE; 
  }
#endif
   
  nvm_read = dm_direct_read(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID, 0, size, p_buf);
  UI_PRINTF("ad nvm_read[0x%x]\n", nvm_read);
  
  r_logo_info= (dvbad_log_save_info *)p_buf;
  if(r_logo_info->uiLogoSize > 100 * KBYTES || r_logo_info->ucLogType != DVBAD_AV_VIDEO_M2V)
  {
    UI_PRINTF("adv_logo:read logo data length too long or data type not right \n",__FUNCTION__);
    //mtos_free(p_block_buffer);
    return ERR_FAILURE;
  }
  p_read_crc_buffer = (u8*)(p_buf + sizeof(dvbad_log_save_info) + r_logo_info->uiLogoSize);
  crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                         +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));

  /****check crc and read back block*****/
 nvm_ad_block_crc_init();
 crc_32_check = nvm_ad_block_generate(p_buf + sizeof(dvbad_log_save_info), r_logo_info->uiLogoSize);

 UI_PRINTF("### %s read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32_read,crc_32_check);
 
 if(crc_32_read != crc_32_check)
  {
    //mtos_free(p_block_buffer);
    return ERR_FAILURE;
  }
  //memcpy(p_buf,p_block_buffer,size);
  //mtos_free(p_block_buffer);
  return SUCCESS;
  
  UI_PRINTF("AD nvram_read ok, return success!\n");
}
#if 0
static RET_CODE nvram_ad_logo_erase(u32 offset, u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;

  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  OS_PRINTF("ADS:Erase size_pre =%d\n", size);

  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_LOGO_BLOCK_ID,offset, size);
  MT_ASSERT(ret != DM_FAIL);

  return TRUE;
}
#endif
void ui_adv_get_osd_msg(u16 pg_id)
{
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
  
  ret = ads_io_ctrl(ADS_ID_ADT_DVBAD,ADS_IOCMD_AD_OSD,&module_config);
  UI_PRINTF("ui_adv_get ads osd osd msg ret = %d\n", ret);
  return;
}

//desai ads display func
void ads_dvbad_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg)
{
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
	  p_cfg->ad_type =  ADS_AD_TYPE_CHLIST;
	  *cmd = ADS_IOCMD_AD_PROGRAM;
	  break;
	  
    case ADS_AD_TYPE_CHBAR:
      p_cfg->ad_type =  ADS_AD_TYPE_CHBAR;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
	case ADS_AD_TYPE_EPG:
  	  p_cfg->ad_type =  ADS_AD_TYPE_EPG;
  	  *cmd = ADS_IOCMD_AD_PROGRAM;
  	  break;
    case ADS_AD_TYPE_VOLBAR:
      p_cfg->ad_type =  ADS_AD_TYPE_VOLBAR;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
    case ADS_AD_TYPE_UNAUTH_PG:
      p_cfg->ad_type =  ADS_AD_TYPE_UNAUTH_PG;
      *cmd = ADS_IOCMD_AD_NO_PROGRAM;
      break;
    default:
      break;           
  }
}

/*fullscreen advertisement display*//*
static RET_CODE ui_pic_fullscr_play(void)
{
  RET_CODE ret = 0;
  
  if(FALSE == get_full_scr_ad_status())
  {   
    return ret;
  }
  if((pic_full_scr_info->data_len== 0) ||
     (pic_full_scr_info->pic_data== NULL) ||
     (ADS_PIC_FORMAT_GIF != pic_full_scr_info->pic_type))
  {
    return ret;
  }
  //fullscreen ad
  UI_PRINTF("[UI_ADS_DIS] fw_get_focus_id()= %d \n",fw_get_focus_id());
  UI_PRINTF("[show full screen ad] pic_full_scr_info = 0x%x, pic_show_info->data_len = %d\n",pic_full_scr_info,
                         pic_full_scr_info->data_len);
  if((is_full_screen_ad_need_display == TRUE) && (ROOT_ID_BACKGROUND == fw_get_focus_id()))
  {
    ui_pic_stop();
    UI_PRINTF("[ui_pic_fullscr_play] show fullscreen ad!!!!\n");
    ui_adv_set_fullscr_rect(pic_full_scr_info->x, pic_full_scr_info->y, 
                                            pic_full_scr_info->w, pic_full_scr_info->h);     
    ret = ui_adv_set_pic(pic_full_scr_info->data_len, pic_full_scr_info->pic_data);

    //ui_set_pic_show(TRUE);
    is_full_screen_ad_on_showing = TRUE;
  }
  return ret;
}
*/
/*unauth adv display*//*
static RET_CODE ui_unauth_ad_play(void)
{
  RET_CODE ret = 0;
  control_t *p_ctrl = NULL;
  
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  
  if((pic_unauth_ad_info->data_len == 0) ||
      (pic_unauth_ad_info->pic_data == NULL) ||
      (ADS_PIC_FORMAT_GIF != pic_unauth_ad_info->pic_type))
  {
    UI_PRINTF("pic_unauth_ad_info is null, will show full screen ad \n");
    ret =ui_pic_fullscr_play();
    return ret;
  }

  if(ROOT_ID_BACKGROUND == fw_get_focus_id())
  {
    ui_pic_stop();
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, MSG_CA_DS_CLEAR_UNAUTH);
    UI_PRINTF("ui_unauth_ad_play show pic_unauth_ad_info!! \n");
    ui_adv_set_fullscr_rect(pic_unauth_ad_info->x, pic_unauth_ad_info->y, 
                                          pic_unauth_ad_info->w, pic_unauth_ad_info->h);     
    ret = ui_adv_set_pic(pic_unauth_ad_info->data_len, pic_unauth_ad_info->pic_data);
    is_unauth_ad_on_showing = TRUE;
  }
  return ret;
}*/

RET_CODE ui_check_fullscr_ad_play(void)
{
return SUCCESS;
#if 0
  RET_CODE ret = FALSE;
  
  is_unauth_ad_on_showing = FALSE;
  
  if(TRUE == is_unauth_ad_enable)
  {
    if(ROOT_ID_BACKGROUND == fw_get_focus_id())
    {
      ret = ui_pic_play(ADS_AD_TYPE_UNAUTH_PG, ROOT_ID_BACKGROUND);  
    }
    UI_PRINTF("get ADS_AD_TYPE_UNAUTH ad ret value = %d \n",ret);
  }  
  if(!ret)
  {
    UI_PRINTF("[ui_check_fullscr_ad_play] show full screen ad \n",ret);
    ret = ui_pic_fullscr_play();
  }
  return ret;
#endif
}
void ui_set_full_scr_ad_status(BOOL status)
{
  is_full_screen_ad_on_showing = status;
}

BOOL ui_get_full_scr_ad_status(void)
{
  return is_full_screen_ad_on_showing;
}
#if 0
BOOL ui_set_ads_osd_status(BOOL status)
{
  is_ads_osd_roll = status;
}
#endif

BOOL ui_get_ads_osd_status(void)
{
  return is_ads_osd_roll;
}

void ui_ads_osd_roll(void)
{
  dvbad_osd_show_info * osd_show_info = NULL;
  u16 uni_str[260] = {0};
  u16 index = 0;
  
  osd_show_info = (dvbad_osd_show_info *)(osd_prom_show_info.stTextShow + uc_current_osd_index);
  
  OS_PRINTF("[AD_OSD] **** osd_show_info->usContentSize = %d \n",osd_show_info->usContentSize);
  OS_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer = %s \n",osd_show_info->pucDisplayBuffer);
  OS_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer[%d] = %s \n",uc_current_osd_index, osd_display_info[uc_current_osd_index].osd_display_buffer);
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

  if(osd_display_info[uc_current_osd_index].b_SaveFlag == TRUE)
  {
  
    /******************************************************************************
    //不判断CA滚动字幕状态，直接显示广告滚动，
    ******************************************************************************/
  
    gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
                                                                osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);
    
    on_desktop_start_roll(uni_str, 0, osd_show_info->ucShowTimes, osd_show_info->enDisplayPos);
    set_ca_msg_rolling_status(TRUE);
    
    uc_current_osd_index++;
  }
  else
  { 
    for(index = uc_current_osd_index + 1; index <= osd_prom_show_info.ucOsdNum; index++)
    {
      if(index == osd_prom_show_info.ucOsdNum)
      {
        uc_current_osd_index = 0;
        break;
      }
      else
      {
        if(osd_display_info[index].b_SaveFlag == TRUE)
         { 
           osd_show_info = (dvbad_osd_show_info *)(osd_prom_show_info.stTextShow + index);
           gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
                                                                osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);
    
            on_desktop_start_roll(uni_str, 0, osd_show_info->ucShowTimes, osd_show_info->enDisplayPos);
            set_ca_msg_rolling_status(TRUE);
            uc_current_osd_index = ++index;
           break;
          }
        }
    }
  }
  
  if(uc_current_osd_index == osd_prom_show_info.ucOsdNum)
  {
    uc_current_osd_index = 0;
  }
}

void ui_ads_osd_roll_stop(void)
{
  RET_CODE ret = 0;
  is_ads_osd_roll = FALSE;
  set_ca_msg_rolling_status(FALSE);
  ret = on_ca_rolling_over(fw_find_root_by_id(ROOT_ID_BACKGROUND), 0, 0, 0);
}

static RET_CODE ui_show_dvbad_osd_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  u8 uc_ads_osd_num = 0;
  u32 index = 0;
  u16 content_size = 0;
  dvbad_osd_program_show_info * pst_osd_ads_roll = NULL;
  
  pst_osd_ads_roll = (dvbad_osd_program_show_info*)p_data;
  if(pst_osd_ads_roll == NULL)
  {
    UI_PRINTF("[AD_OSD] ads osd null!! \n");
    return ERR_FAILURE;
  }  
  memset((void *)&osd_prom_show_info, 0, sizeof(dvbad_osd_program_show_info));
  memcpy((void *)&osd_prom_show_info, (void *)pst_osd_ads_roll, sizeof(dvbad_osd_program_show_info));

  uc_ads_osd_num = osd_prom_show_info.ucOsdNum;
  
  if(uc_ads_osd_num == 0)
  {
    UI_PRINTF("[ad_osd] ads osd num is 0, return \n");
    return ERR_FAILURE;
  }
  
  memset((void *)osd_display_info, 0, MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
  UI_PRINTF("[ad_osd] osd_display_info mem clr num = %d \n", MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
  
   
  for(index = 0; index < uc_ads_osd_num; index++)
  {
    content_size = pst_osd_ads_roll->stTextShow[index].usContentSize;
    
    if(content_size> MAX_ROLL_OSD_LENGTH)
    {
      content_size = MAX_ROLL_OSD_LENGTH;
    }
    
    if(pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer != NULL)
    {
      memcpy(osd_display_info[index].osd_display_buffer, pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer, content_size);
      *(osd_display_info[index].osd_display_buffer + content_size) = '\0';
      osd_display_info[index].b_SaveFlag = TRUE;
    }
    else
    {
      osd_display_info[index].b_SaveFlag = FALSE;
    }
  }
  
  //set current roll index to 1st osd msg
  uc_current_osd_index = 0;
  
  //ads osd display
  if(TRUE == get_ca_msg_rolling_status())   
  {
    ui_ads_osd_roll_stop();
  }
  ui_ads_osd_roll();
  
  is_ads_osd_roll = TRUE;

  return ret;
}

static RET_CODE ui_show_dvbad_pic_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  dvbad_show_pic_info * pic_show_info = NULL;
  pic_show_info = (dvbad_show_pic_info*)p_data;
  
  DEBUG(DVBAD, INFO, "pos_type:%d, pic_type:%d, pic_id:%d\n", 
  				pic_show_info->pos_type, pic_show_info->pic_type, 
  				pic_show_info->pic_id);  
  if(pic_show_info == NULL || pic_show_info->pic_data == NULL || pic_show_info->data_len == 0)
    return ERR_FAILURE;
  
  switch(pic_show_info->pos_type)
  {
/*    case DVBAD_STB_UNAUTHORIZED:
    {
      UI_PRINTF("[DVBAD_STB_UNAUTHORIZED] p_data = 0x%x, pic_show_info->data_len = %d\n",
                    p_data, pic_show_info->data_len);
      UI_PRINTF("[DVBAD_STB_UNAUTHORIZED] x= %d, y=%d, w= %d, h = %d \n",
                  pic_show_info->x, pic_show_info->y,
                 pic_show_info->w, pic_show_info->h);

      pic_unauth_ad_info = (dvbad_show_pic_info*)p_data;
    
      ret = ui_unauth_ad_play();
      break;
    }
    */
    case DVBAD_STB_POS_UNKNOWN:
      {
        UI_PRINTF("[show full screen ad] get_full_scr_ad_status= %d \n",get_full_scr_ad_status());
        UI_PRINTF("[show full screen ad] p_data = 0x%x, pic_show_info->data_len = %d\n",
                        p_data, pic_show_info->data_len);
        UI_PRINTF("[full screen ad rect] x= %d, y=%d, w= %d, h = %d \n",
                      pic_show_info->x, pic_show_info->y,
                     pic_show_info->w, pic_show_info->h);

        pic_full_scr_info = (dvbad_show_pic_info*)p_data;
        is_full_screen_ad_need_display = TRUE;
    
        ret = ui_check_fullscr_ad_play();
        break;
      }

	case DVBAD_STB_LOG:
		break;

	case DVBAD_STB_CHLIST:
    case DVBAD_STB_MAINMENU:
    case DVBAD_STB_VOLUME:
    case DVBAD_STB_EPG:
	case DVBAD_STB_CURRENT_FOLLOW:
	case DVBAD_STB_CHANNEL_INFO:
	    if(ADS_PIC_FORMAT_GIF == pic_show_info->pic_type)
	    {
	      ret = ui_adv_set_pic(pic_show_info->data_len, pic_show_info->pic_data);
	    }
		else
			DEBUG(DVBAD, ERR, "This is not thse GIF format pic\n");
	    is_full_screen_ad_on_showing = FALSE;
	    break;
    
    default:
		break;
  }
  return ret;
}

static RET_CODE ui_save_logo_data_to_nvm(dvbad_av_show_info * av_show_info)
{
  RET_CODE ret = ERR_FAILURE;
  
  //desai max ads element: 100 kb
  if(av_show_info->enAvType != DVBAD_AV_VIDEO_M2V || av_show_info->uiDataLen > 100 * KBYTES)
  {
    UI_PRINTF("[ADS_DIS] Desai ads show:av_show_info->uiDataLen = %d  \n",av_show_info->uiDataLen);
    return ret;
  }
  ret = nvram_ad_logo_write(av_show_info);
  
  if(ret == SUCCESS)
    is_ads_logo_stored = TRUE;
  return ret;
}
static RET_CODE ui_show_dvbad_vid_ad(u8 *p_data)
{
  RET_CODE ret = SUCCESS;
  dvbad_av_show_info * av_show_info = NULL;
  av_show_info = (dvbad_av_show_info*)p_data;

  //ads osd display
  switch(av_show_info->enPosType)
  {
    case DVBAD_STB_LOG:
    {
      //save logo info
      ui_save_logo_data_to_nvm(av_show_info);
      break;
    }
    case DVBAD_STB_EPG:
    case DVBAD_STB_MAINMENU:
    case DVBAD_STB_VOLUME:
    case DVBAD_STB_POS_UNKNOWN:
      break;
    default:
      break;
  }
  
  return ret;
}

BOOL get_ads_logo_status(void)
{
  return  is_ads_logo_stored;
 }

BOOL get_full_scr_ad_status(void)
{
  return  is_full_screen_ad_enable;
 }

void set_full_scr_ad_status(BOOL is_show)
{
  is_full_screen_ad_enable = is_show;
 }
void set_unauth_ad_status(BOOL status)
{
  is_unauth_ad_enable = status;
  ui_check_fullscr_ad_play();
}
RET_CODE ui_adv_dvbad_display(ads_ad_type_t ad_type, u8 *p_data)
{
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
      UI_PRINTF("ADS: DVB ad type [OSD_TYPE] \n");
      ret = ui_show_dvbad_osd_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_PIC:
    {
      UI_PRINTF("ADS: DVB ad type [PIC_TYPE] \n");
      ret = ui_show_dvbad_pic_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_AV:
    {
      UI_PRINTF("ADS: DVB ad type [AV_TYPE] \n");
      ret = ui_show_dvbad_vid_ad(p_data);
      break;
    }
    default:
    break;
    }
  return ret;
}

RET_CODE ui_adv_dvbad_hide(ads_ad_type_t ad_type, u8 *p_data)
{

//hide fullscreen ad
  if(is_full_screen_ad_on_showing)
  {
    UI_PRINTF("[UI_ADS_DIS] hide fullscreen ad \n");
    ui_pic_stop();
    is_full_screen_ad_on_showing = FALSE;
  }
  is_full_screen_ad_need_display = FALSE;
  return SUCCESS;
}


