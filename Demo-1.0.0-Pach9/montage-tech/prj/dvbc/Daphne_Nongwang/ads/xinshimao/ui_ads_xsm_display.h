
#ifndef _UI_ADS_DISPLAY_H
#define _UI_ADS_DISPLAY_H

#include "ads_ware.h"
#include "ads_api_xsm.h"
//2支持的广告类型
typedef enum 
{
	XSMAD_STB_LOG=1,       //开机广告
	XSMAD_STB_EPG=2,          
	XSMAD_STB_FULLSCREEN=3,//推送广告
	XSMAD_STB_VOLUME = 4,
	XSMAD_STB_MENU = 5,
	XSMAD_STB_POS_UNKNOWN,
}ST_AD_XSM_POS_TYPE;

typedef enum 
{
	XSMAD_PIC_BMP=1,
	XSMAD_PIC_JPG=2,
	XSMAD_PIC_GIF=3,
	XSMAD_PIC_UNKNOWN,
}ST_AD_XSM_PIC_TYPE;

//4、支持的音视频格式
typedef enum 
{
	XSMAD_AV_VIDEO_AVI=1,
	XSMAD_AV_AUDIO_MP3=2,  //暂不支持
	XSMAD_AV_VIDEO_TS=3,
    XSMAD_AV_VIDEO_M2V=4,
	XSMAD_AV_UNKNOWN,
}ST_AD_XSM_AV_TYPE;

//5、调用广告系统函数返回值类型定义
typedef enum 
{
	XSMAD_FUN_ERR_PARA,
	XSMAD_FUN_NO_ELEMENT,
	XSMAD_FUN_NOT_SUPPORT,
	XSMAD_FUN_FLASH_NOT_READY,
	XSMAD_FUN_OK,
}ST_AD_XSM_FUNC_TYPE;

//6、时间格式
typedef struct 
{
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_XSM_TIME_TYPE;

//7、日期时间格式
typedef struct 
{
	u16	usYear;
	u8	ucMonth;
	u8	ucDay;
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_XSM_DATE_TIME_INFO;

//8、OSD显示位置
typedef enum 
{
	SHOW_TOP = 0,					//在顶部显示
	SHOW_BOTTOM,					//在底部显示
	SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
	SHOW_LEFT,						//在左边显示
	SHOW_RIGHT,					//在右边显示	
	SHOW_LEFT_RIGHT,				//在左边和右边同时显示
	SHOW_UNKNOWN_TYPE,
}ST_AD_XSM_OSD_SHOW_POS;

//9、OSD显示信息
typedef struct 
{
	ST_AD_XSM_OSD_SHOW_POS enDisplayPos;
	u8				aucBackgroundColor[4];
	u8				aucDisplayColor[4];
	u16				usContentSize;
	u16				usGetBufferSize;
	u8* 				pucDisplayBuffer;
	u8				ucFontType;
	u8				ucFontSize;
	u8				ucShowTimes;
}ST_AD_XSM_OSD_SHOW_INFO;

//10、图片显示信息
typedef struct 
{
	ST_AD_XSM_POS_TYPE	enPosType;
	ST_AD_XSM_PIC_TYPE	enPicType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8				aucElementId[4];
	u8* 				pucPicData;
}ST_AD_XSM_PICTURE_SHOW_INFO;

//11、音视频显示信息
typedef struct 
{
	ST_AD_XSM_POS_TYPE	enPosType;
	ST_AD_XSM_AV_TYPE	enAvType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8				aucElementId[4];
	u8* 				pucAvData;
}ST_AD_XSM_AV_SHOW_INFO;

//12、广告显示信息
typedef struct 
{
	ST_AD_XSM_PICTURE_SHOW_INFO stPictureShow;
	ST_AD_XSM_AV_SHOW_INFO stAvPlay;
	u8 aucFlag[2];
}ST_AD_XSM_PROGRAM_SHOW_INFO;

//13开机广告信息
typedef struct 
{
	u8 ucLogType;//1图片；2视频
	union
	{
		ST_AD_XSM_AV_SHOW_INFO stAvType;
		ST_AD_XSM_PICTURE_SHOW_INFO stPictureType;
	}Element;
}ST_AD_XSM_LOG_INFO;

typedef struct 
{
  u8 ucLogType;
  u32 uiLogoSize;
}ST_AD_XSM_LOG_SAVE_INFO;

typedef struct 
{
	S_YTDY_OSD_DATA stTextShow;	
	u8 aucFlag[15];
	u8 ucOsdNum;
        BOOL b_SaveFlag;
}ST_ADS_OSD_PROGRAM_SHOW_INFO;

BOOL get_ads_logo_status(void);
BOOL get_full_scr_ad_status(void);
BOOL ui_get_ads_osd_status(void);
void set_full_scr_ad_status(BOOL is_show);
void ui_ads_osd_roll_stop(void);
void ui_ads_osd_roll(void);

RET_CODE ui_adv_xsm_display(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_adv_xsm_hide(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_show_xsm_osd_ad(u8 *p_data);

void ui_adv_get_osd_msg(u16 pg_id);
void ads_xsm_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg);

#endif
