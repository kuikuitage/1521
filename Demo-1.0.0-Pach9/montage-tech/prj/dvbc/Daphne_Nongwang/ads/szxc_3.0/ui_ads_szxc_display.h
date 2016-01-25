
#ifndef _UI_ADS_SZXC_DISPLAY_H
#define _UI_ADS_SZXC_DISPLAY_H

//֧�ֵĹ������
typedef enum 
{
	SZAD_OPEN_LOG = 0,                        //�������
	SZAD_MES_PIC = 1,                          //��Ϣ���
	SZAD_MAIN_PIC = 2,                        //��ҳ���
	SZAD_VOLUME_PIC = 3,                    //�������������
	SZAD_AUDIO_PIC = 4,                      //�㲥��Ŀ�������
	SZAD_TXT = 5,                                  //TXT���ֹ������
	SZAD_HANG_PIC = 6,                       //�ҽǹ��
	SZAD_PROGRAM_PIC = 7,                 //��Ŀָ�Ϲ��
	SZAD_CHANNEL_PIC = 8,                 //Ƶ���б���
	SZAD_CLOSE_LOG = 9,                     //�ػ����
	SZAD_STB_POS_UNKNOWN,
}e_szxc_adv_type_t;

// ���ĸ�ʽ
typedef enum 
{
	SZAD_PIC_BMP = 0,
	SZAD_PIC_JPG = 1,
	SZAD_PIC_GIF = 2,
	SZAD_PIC_I = 3,
	SZAD_PIC_TXT = 4,
	SZAD_PIC_UNKNOWN,
}e_szxc_adv_pic_type_t;


//TXT��ʾλ��
typedef enum 
{
	SHOW_TOP = 1,					//�ڶ�����ʾ
	SHOW_BOTTOM,					//�ڵײ���ʾ
	SHOW_LEFT,						//�������ʾ
	SHOW_RIGHT,					//���ұ���ʾ	
	SHOW_UNKNOWN_POS_TYPE,
}e_szxc_adv_osd_pos_t;

//TXT��ʾ�ٶ�
typedef enum 
{
	SHOW_SLOW = 1,				//��
	SHOW_MID,				        //��
	SHOW_QUICK,				//��
	SHOW_UNKNOWN_SPEED_TYPE,
}e_szxc_adv_osd_speed_t;

typedef enum 
{
	SHOW_FONT_SIZE_LIT = 1,
	SHOW_FONT_SIZE_MID,
	SHOW_FONT_SIZE_BIG,
	SHOW_UNKNOWN_FONT_SIZE,
}e_szxc_adv_osd_font_size_t;

//TXT��ʾ������ɫ
typedef enum 
{
	SHOW_COLBACK_BLUE = 1,				   //��ɫ
	SHOW_COLBACK_YELLOW,				         //��ɫ
	SHOW_COLBACK_NOCOLOUR,			         //͸ɫ
	SHOW_COLBACK_RED,				         //��ɫ
	SHOW_COLUNKNOWN_COLBACK_TYPE,
}e_szxc_adv_osd_colback_t;

//TXT��ʾ������ɫ
typedef enum 
{
	SHOW_COLFONT_WHITE = 1,				  //��ɫ
	SHOW_COLFONT_BLACK,				         //��ɫ
	SHOW_COLFONT_YELLOW,			               //��ɫ
	SHOW_COLFONT_RED,				        //��ɫ
	SHOW_UNKNOWN_COLFONT_TYPE,
}e_szxc_adv_osd_colfont_t;



//�ҽǹ����ʾλ��
typedef enum 
{
	SHOW_RIGHT_BOTTOM = 1,		//����
	SHOW_LEFT_BOTTOM,			//����
	SHOW_RIGHT_TOP,				//����
	SHOW_LEFT_TOP,				//����	
	SHOW_UNKNOWN_TYPE,
}e_szxc_adv_hangpic_pos_t;

//TXT��ʾ��Ϣ
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

//ͼƬ��ʾ��Ϣ
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


//�ҽ�ͼƬ��ʾ��Ϣ
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
