
#ifndef _UI_ADS_DISPLAY_H
#define _UI_ADS_DISPLAY_H


#define MAX_ROLL_OSD_LENGTH 256
#define MAX_ROLL_OSD_NUM 15
//2֧�ֵĹ������
typedef enum 
{
	DSAD_STB_LOG=1,       //�������
	DSAD_STB_EPG=2,          
	DSAD_STB_FULLSCREEN=3,//���͹��
	DSAD_STB_VOLUME = 4,
	DSAD_STB_MENU = 5,
	DSAD_STB_UNAUTHORIZED = 6,/*δ��Ȩ��Ŀ���*/
	DSAD_STB_POS_UNKNOWN,
}ST_AD_DS_POS_TYPE;

typedef enum 
{
	DSAD_PIC_BMP=1,
	DSAD_PIC_JPG=2,
	DSAD_PIC_GIF=3,
	DSAD_PIC_UNKNOWN,
}ST_AD_DS_PIC_TYPE;

//4��֧�ֵ�����Ƶ��ʽ
typedef enum 
{
	DSAD_AV_VIDEO_AVI=1,
	DSAD_AV_AUDIO_MP3=2,  //�ݲ�֧��
	DSAD_AV_VIDEO_TS=3,
	DSAD_AV_VIDEO_M2V=4,
	DSAD_AV_VIDEO_MPG=5,
	DSAD_AV_UNKNOWN,
}ST_AD_DS_AV_TYPE;

//5�����ù��ϵͳ��������ֵ���Ͷ���
typedef enum 
{
	DSAD_FUN_ERR_PARA,
	DSAD_FUN_NO_ELEMENT,
	DSAD_FUN_NOT_SUPPORT,
	DSAD_FUN_FLASH_NOT_READY,
	DSAD_FUN_OK,
}ST_AD_DS_FUNC_TYPE;

//6��ʱ���ʽ
typedef struct 
{
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_DS_TIME_TYPE;

//7������ʱ���ʽ
typedef struct 
{
	u16	usYear;
	u8	ucMonth;
	u8	ucDay;
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_DS_DATE_TIME_INFO;

//8��OSD��ʾλ��
typedef enum 
{
	SHOW_TOP = 0,					//�ڶ�����ʾ
	SHOW_BOTTOM,					//�ڵײ���ʾ
	SHOW_TOP_BOTTOM,				//�ڶ����͵ײ�ͬʱ��ʾ
	SHOW_LEFT,						//�������ʾ
	SHOW_RIGHT,					//���ұ���ʾ	
	SHOW_LEFT_RIGHT,				//����ߺ��ұ�ͬʱ��ʾ
	SHOW_UNKNOWN_TYPE,
}ST_AD_DS_OSD_SHOW_POS;

//9��OSD��ʾ��Ϣ
typedef struct 
{
	ST_AD_DS_OSD_SHOW_POS enDisplayPos;
	u8				aucBackgroundColor[4];
	u8				aucDisplayColor[4];
	u16				usContentSize;
	u16				usGetBufferSize;
	u8* 				pucDisplayBuffer;
	u8				ucFontType;
	u8				ucFontSize;
	u8				ucShowTimes;
}ST_AD_DS_OSD_SHOW_INFO;

//10��ͼƬ��ʾ��Ϣ
typedef struct 
{
	ST_AD_DS_POS_TYPE	enPosType;
	ST_AD_DS_PIC_TYPE	enPicType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8				aucElementId[4];
	u8* 				pucPicData;
}ST_AD_DS_PICTURE_SHOW_INFO;

//11������Ƶ��ʾ��Ϣ
typedef struct 
{
	ST_AD_DS_POS_TYPE	enPosType;
	ST_AD_DS_AV_TYPE	enAvType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8				aucElementId[4];
	u8* 				pucAvData;
}ST_AD_DS_AV_SHOW_INFO;

//12�������ʾ��Ϣ
typedef struct 
{
	ST_AD_DS_PICTURE_SHOW_INFO stPictureShow;
	ST_AD_DS_AV_SHOW_INFO stAvPlay;
	u8 aucFlag[2];
}ST_AD_DS_PROGRAM_SHOW_INFO;

//13���������Ϣ
typedef struct 
{
	u8 ucLogType;//1ͼƬ��2��Ƶ
	union
	{
		ST_AD_DS_AV_SHOW_INFO stAvType;
		ST_AD_DS_PICTURE_SHOW_INFO stPictureType;
	}Element;
}ST_AD_DS_LOG_INFO;

typedef struct 
{
  u8 ucLogType;
  u32 uiLogoSize;
}ST_AD_DS_LOG_SAVE_INFO;

typedef struct 
{
	ST_AD_DS_OSD_SHOW_INFO stTextShow[15];	
	u8 aucFlag[15];
	u8 ucOsdNum;
}ST_ADS_OSD_PROGRAM_SHOW_INFO;

typedef struct 
{
  BOOL b_SaveFlag;
  u8 osd_display_buffer[MAX_ROLL_OSD_LENGTH + 1];	
}ST_ADS_OSD_INFO;

BOOL get_ads_logo_status(void);
BOOL get_full_scr_ad_status(void);
BOOL ui_get_ads_osd_status(void);
BOOL ui_get_full_scr_ad_status(void);

void set_full_scr_ad_status(BOOL is_show);
void set_unauth_ad_status(BOOL status);
void ui_set_full_scr_ad_status(BOOL status);

void ui_ads_osd_roll_stop(void);
void ui_ads_osd_roll(void);

RET_CODE ui_adv_desai_display(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_adv_desai_hide(ads_ad_type_t ad_type, u8 *p_data);

void ui_adv_get_osd_msg(u16 pg_id);
void ads_desai_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg);
void ui_get_unauth_program_ad(void);

#endif
