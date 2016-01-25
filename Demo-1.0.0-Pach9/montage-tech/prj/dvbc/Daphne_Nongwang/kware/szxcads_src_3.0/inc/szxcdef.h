/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#ifndef __SZXC_DEF_H__
#define __SZXC_DEF_H__

/*!
  id screen.
  */  
#define SZXC_TABLE_ID_LOGO 0x90
/*!
  id float.
  */  
#define SZXC_TABLE_ID_FLOAT 0x91
/*!
  id hangpic.
  */  
#define SZXC_TABLE_ID_HANGPIC 0x92
/*!
  id menupic.
  */  
#define SZXC_TABLE_ID_MENUPIC 0x93
/*!
  screen pic maxsize.
  */  
#define SZXC_LOGO_MAXSIZE (64*1024)
/*!
  float maxsize.
  */  
#define SZXC_FLOAT_MAXSIZE (64*1024)
/*!
  float max section num
  */  
#define SZXC_FLOAT_MAXSECTNUM (256)
/*!
  menu pic maxsize.
  */  
#define SZXC_MENUPIC_MAXSIZE (64*1024)
/*!
  menu pic maxnum.
  */  
#define SZXC_MENUPIC_MAXNUM (3)
/*!
  hang pic maxsize.
  */  
#define SZXC_HANGPIC_MAXSIZE (32*1024)
/*!
  hang pic maxnum.
  */
#define SZXC_HANGPIC_MAXNUM (5)
/*!
  hang pic serviceid.
  */
#define SZXC_HANGPIC_MAXSERVICEID 10
/*!
  hang pic caption size.
  */
#define SZXC_HANGPIC_MAXCAPTIONSIZE (512)
/*!
  hang pic maxpicsize.
  */
#define SZXC_HANGPIC_MAXPICSIZE (32*1024)

/*!
  hangpic_content
*/
typedef struct szxc_hangpic_content{
/*!
  hang pic service size.
  */
  u32 service_size;
/*!
  hang pic service id.
  */
  u16 service_id[SZXC_HANGPIC_MAXSERVICEID];
/*!
  hang pic captionlen.
  */  
  u32 caption_length;
/*!
  hang pic caption.
  */
  u8 caption_content[SZXC_HANGPIC_MAXCAPTIONSIZE];
 /*!
  hang pic picsize.
  */ 
  u32 pic_size;
 /*!
  hang pic.
  */
  u8 pic_content[SZXC_HANGPIC_MAXPICSIZE];
}szxc_hangpic_content_t;

/*!
  SZXC AD TYPE
*/ 
typedef enum
{
/*!
  logo.
  */
  SZXC_LOGO_IFRAME = 1,
/*!
  float.
  */
  SZXC_FLOAT,
/*!
  menu pic.
  */
  SZXC_MENU_PIC,
/*!
  hang pic.
  */
  SZXC_HANG_PIC,
/*!
  undefine.
  */
  SZXC_UNDEFINED,
}szxc_drv_adtype_t;

/*!
  SZXC AD TYPE
*/ 
typedef enum AD_SZXC_AD_TYPE_E{
/*!
  open.
  */
AD_SZXC_AD_START = 0,
/*!
  message.
  */
AD_SZXC_AD_MES,
/*!
  honm pic.
  */
AD_SZXC_AD_HOME,
/*!
  volum pic.
  */
AD_SZXC_AD_VOLBAR,
/*!
  broadcast pic.
  */
AD_SZXC_AD_BROADCAST,
/*!
  txt pic.
  */
AD_SZXC_AD_TXT,
/*!
  hang pic.
  */
AD_SZXC_AD_CON,
/*!
  epg pic.
  */
AD_SZXC_AD_EPG,
/*!
  channel pic.
  */
AD_SZXC_AD_CHANNEL_LIST,
/*!
  close pic.
  */
AD_SZXC_AD_POWER_OFF,
}AD_SZXC_AD_TYPE_T;


/*!
  tuner getlock.
  */
u8 szxc_drv_tuner_getlk(void);
/*!
  tuner setparams.
  */
s8 szxc_drv_tuner_setparams (u32 _chanFrequency,
                  u32 _chanSymbolRate,u8 _qammode);

/*!
 ads init.
  */
void szxc_ads_client_init();
/*!
  search ads pat.
  */
void szxc_drv_start_search_bat();
/*!
  search ads pat end.
  */
s32 szxc_drv_search_bat_end();
/*!
  pat get check.
  */
s32 szxc_drv_check_pic_pid();
/*!
  pat get check.
  */
u8 szxc_drv_check_pic_finish();
/*!
  search ads pic.
  */
void szxc_drv_start_search_pic();
/*!
  search ads hang.
  */
void szxc_drv_start_search_hang();
/*!
  search ads txt.
  */
void szxc_drv_start_search_txt();
/*!
  exit search ads.
  */
void szxc_drv_exit_searchad();
/*!
  write pic data to flasd.
  */
RET_CODE szxc_picdata_to_flash();
/*!
  show logo.
  */
RET_CODE szxc_ad_show_logo(AD_SZXC_AD_TYPE_T e_adtype_t);
/*!
  show pic.
  */
RET_CODE szxc_ad_show_pic(AD_SZXC_AD_TYPE_T e_adtype_t, u16 service_id);
/*!
  show float.
  */
void szxc_ad_showfloat(void);
/*!
  show menu pic.
  */
void szxc_ad_showmenupic(u32 picindex);
/*!
  show menu pic.
  */
void szxc_ad_showhangpic(u32 service_id);
/*!
  clear txt flag.
  */
void szxc_clear_txt_flag(u8 pos_txt);

#endif
