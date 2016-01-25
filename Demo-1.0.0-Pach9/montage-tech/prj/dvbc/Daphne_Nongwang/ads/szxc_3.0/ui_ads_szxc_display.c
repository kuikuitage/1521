/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "ui_common.h"

#include "ui_ads_szxc_display.h"
#include "ui_ad_logo_api.h"
#include "ui_config.h"
#include "ads_ware.h"
#include "pdec.h"


static s_szxc_adv_osd_show_t osd_show_info;
static u8 ui_finish_flag = 0;
static u8 hang_pic_statu[5] = {0};
static s_szxc_adv_hangpic_show_t *hang_pic_data[5] = {NULL};

static BOOL is_full_screen_ad_on_showing = FALSE;
/*
extern void on_desktop_start_roll_rect(
  u16 *p_uni_str, u8 roll_speed, u8 font_size, u32 dis_pos,
  rect_t *pDisplayRect, u32 repeat_times, u8 back_color, u8 font_color);*/
extern void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos);
extern RET_CODE ui_adv_set_pic(u32 size, u8 * pic_add);

RET_CODE ui_adv_szxc_statu(u8 adv_stype)
{
    RET_CODE ret = ERR_FAILURE;
    u8 param = 0;
    switch(adv_stype)
    {
        case 1:
               if(ui_finish_flag)
                {
                    if(ROOT_ID_BACKGROUND == fw_get_focus_id())
                        ret = SUCCESS;
                }
            break;
        case 2:
             UI_PRINTF("[AD_STATU] ui adv szxc statu set: %d\n", adv_stype);
             ui_finish_flag = 1;
             break;
         case 3:
             UI_PRINTF("[AD_STATU] ui adv szxc statu set: %d\n", adv_stype);
             param = 1;
             ads_io_ctrl(ADS_ID_ADT_SZXC, ADS_IOCMD_AD_CLEAR_FLAG, &param);
             break;
        default:
            break;
    }
    return ret;
}

void ui_ads_osd_roll_stop(void)
{
  fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_EXIT_AD_ROLLING, 0, 0);
}

void ui_ads_osd_roll(void)
{
  u16 uni_str[512] = {0};

  if((osd_show_info.pucDisplayBuffer == NULL))
  {
    ui_ads_osd_roll_stop();
    return;
  }

  gb2312_to_unicode(
    (osd_show_info.pucDisplayBuffer), osd_show_info.usGetBufferSize, uni_str, osd_show_info.usGetBufferSize);

  #if 1
  UI_PRINTF("[AD_OSD] **** osd_show_info.usGetBufferSize = %d\n", osd_show_info.usGetBufferSize);
  UI_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer = %s\n", osd_show_info.pucDisplayBuffer);
  #endif

  // 通知底层显示完成
  /*on_desktop_start_roll_rect(uni_str,
    osd_show_info.enSpeed,
    osd_show_info.enFontSize,
    osd_show_info.enDisplayPos,
    &osd_show_info.stDisplayRect,
    osd_show_info.ucShowTimes,
    osd_show_info.enBackgroundColor,
    osd_show_info.enFontColor);
*/}

static RET_CODE ui_show_szxc_osd_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  s_szxc_adv_osd_show_t * pst_osd_ads_roll = NULL;

  pst_osd_ads_roll = (s_szxc_adv_osd_show_t*)p_data;
  if(pst_osd_ads_roll == NULL)
  {
    UI_PRINTF("[AD_OSD] ads osd null!! \n");
    return ERR_FAILURE;
  }
  memset((void *)&osd_show_info, 0, sizeof(s_szxc_adv_osd_show_t));
  memcpy((void *)&osd_show_info, (void *)pst_osd_ads_roll, sizeof(s_szxc_adv_osd_show_t));

  //ads osd display
  ui_ads_osd_roll();

  return ret;
}

static RET_CODE ui_show_szxc_pic_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  u8 *pic_data = NULL;
  s_szxc_adv_pic_show_t * pic_show_info = NULL;
  pic_show_info = (s_szxc_adv_pic_show_t*)p_data;

  UI_PRINTF("[UI]: pic_show_info-->enPicType = %d \n",pic_show_info->enPicType);

  if(pic_show_info == NULL || pic_show_info->pucPicData == NULL || pic_show_info->uiDataLen == 0)
    return ERR_FAILURE;

  UI_PRINTF("[show full screen ad] pic_full_scr_info = 0x%x, pic_show_info->uiDataLen = %d\n",pic_show_info,
                          pic_show_info->uiDataLen);
  
  pic_data = mtos_malloc(pic_show_info->uiDataLen);
  memset(pic_data, 0, pic_show_info->uiDataLen);
  memcpy(pic_data, pic_show_info->pucPicData, pic_show_info->uiDataLen);
  if(pic_show_info->enPicType == SZAD_PIC_GIF)
    ret = ui_adv_set_pic(pic_show_info->uiDataLen, pic_data);
  else
    return ERR_FAILURE;

  return ret;
}

#if 0
static void ui_hang_pic_coord(s_szxc_adv_hangpic_show_t *pic_show_info, u16 *x, u16 *y)
{
    switch(pic_show_info->enDisplayPos)
  {
      case SHOW_RIGHT_BOTTOM:
          *x = AD_SCREEN_WIDTH -pic_show_info->usWidth -20 ;
          *y = AD_SCREEN_HEIGHT -pic_show_info->usHeigth  -20;
          break;
      case SHOW_LEFT_BOTTOM:
          *x = pic_show_info->usWidth + 20 ;
          *y = AD_SCREEN_HEIGHT -pic_show_info->usHeigth  -20;
          break;
      case SHOW_RIGHT_TOP:
          *x = AD_SCREEN_WIDTH -pic_show_info->usWidth -20 ;
          *y = pic_show_info->usHeigth + 20;
          break;
      case SHOW_LEFT_TOP:
          *x = pic_show_info->usWidth + 20 ;
          *y = pic_show_info->usHeigth + 20;
          break;
      default:
        break;
  }
}
#endif

void ui_hang_pic_orientation(u16 x, u16 y, s_szxc_adv_hangpic_show_t *pic_show_info)
{
  if ((x < (AD_SCREEN_WIDTH / 2)) && (y < (AD_SCREEN_HEIGHT / 2)))
  {
    pic_show_info->enDisplayPos = SHOW_LEFT_TOP;
  }
  else if ((x > (AD_SCREEN_WIDTH / 2)) && (y < (AD_SCREEN_HEIGHT / 2)))
  {
    pic_show_info->enDisplayPos = SHOW_RIGHT_TOP;
  }
  else if ((x < (AD_SCREEN_WIDTH / 2)) && (y > (AD_SCREEN_HEIGHT / 2)))
  {
    pic_show_info->enDisplayPos = SHOW_LEFT_BOTTOM;
  }
  else if ((x > (AD_SCREEN_WIDTH / 2)) && (y > (AD_SCREEN_HEIGHT / 2)))
  {
    pic_show_info->enDisplayPos = SHOW_RIGHT_BOTTOM;
  }
  else
  {
    pic_show_info->enDisplayPos = SHOW_UNKNOWN_TYPE;
  }
}

RET_CODE ui_check_fullscr_ad_play(void)
{
   RET_CODE ret = ERR_FAILURE;
   //u16 startx = 0;
   //u16 starty = 0;
   u8 index = 0;
   s_szxc_adv_hangpic_show_t *pic_show_info = NULL;
   for(index = 1; index < 5; index++)
  {
      if(hang_pic_statu[index] == 1)
      {
          pic_show_info = hang_pic_data[index];
          //ui_hang_pic_coord(pic_show_info, &startx, &starty);
          ui_adv_set_fullscr_rect(pic_show_info->usStartX, pic_show_info->usStartY,
                                            pic_show_info->usWidth, pic_show_info->usHeigth);
          if(pic_show_info->enPicType == SZAD_PIC_GIF)
            ret = ui_adv_set_pic(pic_show_info->uiDataLen, pic_show_info->pucPicData);
          else
            return ERR_FAILURE;
      }
  }

  return ret;
}

static RET_CODE ui_show_szxc_hangpic_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  //u16 startx = 0;
  //u16 starty = 0;
  s_szxc_adv_hangpic_show_t * pic_show_info = NULL;
  pic_show_info = (s_szxc_adv_hangpic_show_t*)p_data;

  UI_PRINTF("[UI]: pic_show_info-->enPicType = %d \n",pic_show_info->enPicType);

  if(pic_show_info == NULL || pic_show_info->pucPicData == NULL || pic_show_info->uiDataLen == 0)
    return ERR_FAILURE;

//  if(SZAD_PIC_GIF == pic_show_info->enPicType)
  //ui_hang_pic_coord(pic_show_info, &startx, &starty);
  UI_PRINTF("[UI]: hang_pic_show_info: \n");
  UI_PRINTF("[UI]: x = %d, y = %d, w = %d ,h =%d\n", pic_show_info->usStartX,
    pic_show_info->usStartY, pic_show_info->usWidth, pic_show_info->usHeigth);
  hang_pic_data[pic_show_info ->enDisplayPos] = pic_show_info;
  hang_pic_statu[pic_show_info ->enDisplayPos] = 1;
  ui_adv_set_fullscr_rect(pic_show_info->usStartX, pic_show_info->usStartY,
                                            pic_show_info->usWidth, pic_show_info->usHeigth);
  if(pic_show_info->enPicType == SZAD_PIC_GIF)
    ret = ui_adv_set_pic(pic_show_info->uiDataLen, pic_show_info->pucPicData);
  else
    return ERR_FAILURE;
  ui_set_pic_show(TRUE);
  UI_PRINTF("[UI]: hang_pic_show_info   %d: \n", ret);
  return ret;
}

void ads_szxc_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg)
{

  OS_PRINTF("DEBUG : ads_szxc_set_type   type : [%d]",type);
  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
    case ADS_AD_TYPE_MENU_DOWN:
    case ADS_AD_TYPE_MAIN_MENU:
    case ADS_AD_TYPE_SUB_MENU:
    case ADS_AD_TYPE_OTHER_GIF:
    case ADS_AD_TYPE_SEARCHING:
    case ADS_AD_TYPE_SIGNAL:
      *cmd = ADS_IOCMD_AD_HOME_PIC;
      break;
    case ADS_AD_TYPE_CHLIST:
      *cmd = ADS_IOCMD_AD_CHANNEL_PIC;
      break;
    case ADS_AD_TYPE_CHBAR:
      *cmd = ADS_IOCMD_AD_EPG_PIC;
      break;
    case ADS_AD_TYPE_VOLBAR:
      *cmd = ADS_IOCMD_AD_VOLUM_PIC;
      break;

    default:
      break;
  }
}

RET_CODE ui_adv_szxc_display(ads_ad_type_t ad_type, u8 *p_data)
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
      UI_PRINTF("ADS: szxc ad type [OSD_TYPE] \n");
      ret = ui_show_szxc_osd_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_PIC:
    {
      UI_PRINTF("ADS: szxc ad type [PIC_TYPE] \n");
      ret = ui_show_szxc_pic_ad(p_data);
      break;
    }
    case ADS_AD_TYPE_SUBTITLE:
    {
      UI_PRINTF("ADS: szxc ad type [HANGPIC_TYPE] \n");
      ret = ui_show_szxc_hangpic_ad(p_data);
      break;
    }
    default:
      break;
  }
  return ret;
}

RET_CODE ui_adv_szxc_hide(ads_ad_type_t ad_type, u8 *p_data)
{
  s_szxc_adv_hangpic_show_t * pic_show_info = NULL;
  pic_show_info = (s_szxc_adv_hangpic_show_t*)p_data;
  //u16 startx = 0;
  //u16 starty = 0;
  hang_pic_statu[pic_show_info ->enDisplayPos] = 0;
  //ui_hang_pic_coord(pic_show_info, &startx, &starty);
  //清空相应位置的广告，需AP提供接口
  ui_pic_stop();
  return SUCCESS;
}

void ui_set_full_scr_ad_status(BOOL status)
{
  is_full_screen_ad_on_showing = status;
}
