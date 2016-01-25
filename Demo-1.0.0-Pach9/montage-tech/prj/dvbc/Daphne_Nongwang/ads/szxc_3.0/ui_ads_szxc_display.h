
#ifndef _UI_ADS_SZXC_DISPLAY_H
#define _UI_ADS_SZXC_DISPLAY_H

//支持的广告类型
typedef enum 
{
	SZAD_OPEN_LOG = 0,                        //开机广告
	SZAD_MES_PIC = 1,                          //信息广告
	SZAD_MAIN_PIC = 2,                        //主页广告
	SZAD_VOLUME_PIC = 3,                    //音量调背景广告
	SZAD_AUDIO_PIC = 4,                      //广播节目背景广告
	SZAD_TXT = 5,                                  //TXT文字滚屏广告
	SZAD_HANG_PIC = 6,                       //挂角广告
	SZAD_PROGRAM_PIC = 7,                 //节目指南广告
	SZAD_CHANNEL_PIC = 8,                 //频道列表广告
	SZAD_CLOSE_LOG = 9,                     //关机广告
	SZAD_STB_POS_UNKNOWN,
}e_szxc_adv_type_t;

// 广告的格式
typedef enum 
{
	SZAD_PIC_BMP = 0,
	SZAD_PIC_JPG = 1,
	SZAD_PIC_GIF = 2,
	SZAD_PIC_I = 3,
	SZAD_PIC_TXT = 4,
	SZAD_PIC_UNKNOWN,
}e_szxc_adv_pic_type_t;


//TXT显示位置
typedef enum 
{
	SHOW_TOP = 1,					//在顶部显示
	SHOW_BOTTOM,					//在底部显示
	SHOW_LEFT,						//在左边显示
	SHOW_RIGHT,					//在右边显示	
	SHOW_UNKNOWN_POS_TYPE,
}e_szxc_adv_osd_pos_t;

//TXT显示速度
typedef enum 
{
	SHOW_SLOW = 1,				//慢
	SHOW_MID,				        //中
	SHOW_QUICK,				//快
	SHOW_UNKNOWN_SPEED_TYPE,
}e_szxc_adv_osd_speed_t;

typedef enum 
{
	SHOW_FONT_SIZE_LIT = 1,
	SHOW_FONT_SIZE_MID,
	SHOW_FONT_SIZE_BIG,
	SHOW_UNKNOWN_FONT_SIZE,
}e_szxc_adv_osd_font_size_t;

//TXT显示背景颜色
typedef enum 
{
	SHOW_COLBACK_BLUE = 1,				   //蓝色
	SHOW_COLBACK_YELLOW,				         //黄色
	SHOW_COLBACK_NOCOLOUR,			         //透色
	SHOW_COLBACK_RED,				         //红色
	SHOW_COLUNKNOWN_COLBACK_TYPE,
}e_szxc_adv_osd_colback_t;

//TXT显示字体颜色
typedef enum 
{
	SHOW_COLFONT_WHITE = 1,				  //白色
	SHOW_COLFONT_BLACK,				         //黑色
	SHOW_COLFONT_YELLOW,			               //黄色
	SHOW_COLFONT_RED,				        //红色
	SHOW_UNKNOWN_COLFONT_TYPE,
}e_szxc_adv_osd_colfont_t;



//挂角广告显示位置
typedef enum 
{
	SHOW_RIGHT_BOTTOM = 1,		//右下
	SHOW_LEFT_BOTTOM,			//左下
	SHOW_RIGHT_TOP,				//右上
	SHOW_LEFT_TOP,				//左上	
	SHOW_UNKNOWN_TYPE,
}e_szxc_adv_hangpic_pos_t;

//TXT显示信息
typedef struct 
{
	e_szxc_adv_osd_pos_t           enDisplayPos;
	rect_t					stDisplayRect;
	e_szxc_adv_osd_colback_t	  enBackgroundColor;
	e_szxc_adv_osd_colfont_t       enFontColor;
	e_szxc_adv_osd_speed_t        enSpeed;
	e_szxc_adv_osd_font_size_t        enFontSize;
	u8				ucShowFlag;
        u8				ucShowTimes;
	u16				usGetBufferSize;
	u8* 				pucDisplayBuffer;
}s_szxc_adv_osd_show_t;

//图片显示信息
typedef struct 
{
	e_szxc_adv_type_t 	enPosType;
	e_szxc_adv_pic_type_t	enPicType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8* 				pucPicData;
}s_szxc_adv_pic_show_t;


//挂角图片显示信息
typedef struct 
{
	e_szxc_adv_type_t 	      enPosType;
	e_szxc_adv_pic_type_t	      enPicType;
       e_szxc_adv_hangpic_pos_t  enDisplayPos;
       u8                        usShowFlag;
       u8                        usShowOver; 
       u8				usShowMinute;
       u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8* 				pucPicData;
}s_szxc_adv_hangpic_show_t;





RET_CODE ui_adv_szxc_display(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_adv_szxc_hide(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_adv_szxc_statu(u8 adv_stype);
void ui_set_full_scr_ad_status(BOOL status);
void ads_szxc_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg);

#endif
