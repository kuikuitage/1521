#ifndef _DS_AD_API
#define _DS_AD_API

//��ʾ��Ϣ
#define DSAD_MESSAGE_HIDEOSD_TYPE			(0X01)
#define DSAD_MESSAGE_SHOWOSD_TYPE			(0X02)
#define DSAD_MESSAGE_UPDATE_LOG_TYPE			(0X03)
#define DSAD_MSEEAGE_SHOWFULLSRCEEN_TYPE	(0X04)
#define DSAD_MSEEAGE_HIDEFULLSRCEEN_TYPE	(0X05)


#define	DSAD_FILTER_MAX_LEN		(15)
#define  IN                         /*��������(��������)*/
#define	 OUT                      /*��������(�ش�����)*/

#ifndef		NULL
#define		NULL		0
#endif

#ifndef TRUE
#define TRUE				(1)
#endif

#ifndef FALSE
#define FALSE			(0)
#endif

#ifndef BOOL
#define BOOL			int
#endif

#ifndef INT8
#define INT8			char
#endif

#ifndef UINT8
#define UINT8			unsigned char
#endif

#ifndef INT16
#define INT16			short
#endif

#ifndef UINT16
#define UINT16			unsigned short
#endif

#ifndef INT32
#define INT32			int
#endif

#ifndef UINT32
#define UINT32			unsigned int
#endif

#ifndef DSAD_Semaphore
#define DSAD_Semaphore	unsigned int
#endif

#ifndef DSAD_MsgQueue
#define DSAD_MsgQueue	unsigned int
#endif



//1��������ƽ̨
typedef enum _EN_DSAD_STB_DECODE_TYPE
{
	DSAD_STB_SD=1,
	DSAD_STB_HD=2,
	DSAD_STB_UNKNOWN,
}EN_DSAD_STB_DECODE_TYPE;

//2֧�ֵĹ������
typedef enum _EN_DSAD_POS_TYPE
{
	DSAD_STB_LOG=1,       //�������
	DSAD_STB_EPG=2,          
	DSAD_STB_FULLSCREEN=3,//���͹��
	DSAD_STB_VOLUME = 4,
	DSAD_STB_MENU = 5,
	DSAD_STB_UNAUTHORIZED = 6,/*δ��Ȩ��Ŀ���*/
	DSAD_STB_POS_UNKNOWN,
}EN_DSAD_POS_TYPE;

//3��֧�ֵ�ͼƬ��ʽ
typedef enum _EN_DSAD_PIC_TYPE
{
	DSAD_PIC_BMP=1,
	DSAD_PIC_JPG=2,
	DSAD_PIC_GIF=3,
	DSAD_PIC_UNKNOWN,
}EN_DSAD_PIC_TYPE;

//4��֧�ֵ�����Ƶ��ʽ
typedef enum _EN_DSAD_AV_TYPE
{
	DSAD_AV_VIDEO_AVI=1,
	DSAD_AV_AUDIO_MP3=2,  
	DSAD_AV_VIDEO_TS=3,
	DSAD_AV_VIDEO_M2V=4,
	DSAD_AV_VIDEO_MPG=5,
	DSAD_AV_UNKNOWN,
}EN_DSAD_AV_TYPE;

//5�����ù��ϵͳ��������ֵ���Ͷ���
typedef enum _EN_DSAD_FUNCTION_TYPE
{
	DSAD_FUN_ERR_PARA,
	DSAD_FUN_NO_ELEMENT,
	DSAD_FUN_NOT_SUPPORT,
	DSAD_FUN_FLASH_NOT_READY,
	DSAD_FUN_OK,
}EN_DSAD_FUNCTION_TYPE;

//6��ʱ���ʽ
typedef struct _ST_DSAD_TIME_INFO
{
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DSAD_TIME_INFO;

//7������ʱ���ʽ
typedef struct _ST_DSAD_DATE_TIME_INFO
{
	UINT16	usYear;
	UINT8	ucMonth;
	UINT8	ucDay;
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DSAD_DATE_TIME_INFO;

//8��OSD��ʾλ��
typedef enum _EN_DSAD_OSD_SHOW_POS
{
	SHOW_TOP = 0,					//�ڶ�����ʾ
	SHOW_BOTTOM,					//�ڵײ���ʾ
	SHOW_TOP_BOTTOM,				//�ڶ����͵ײ�ͬʱ��ʾ
	SHOW_LEFT,						//�������ʾ
	SHOW_RIGHT,					//���ұ���ʾ	
	SHOW_LEFT_RIGHT,				//����ߺ��ұ�ͬʱ��ʾ
	SHOW_UNKNOWN_TYPE,
}EN_DSAD_OSD_SHOW_POS;

//9��OSD��ʾ��Ϣ
typedef struct _ST_DSAD_OSD_SHOW_INFO
{
	EN_DSAD_OSD_SHOW_POS		enDisplayPos;
	UINT8				aucBackgroundColor[4];
	UINT8				aucDisplayColor[4];
	UINT16				usContentSize;
	UINT16				usGetBufferSize;
	UINT8* 				pucDisplayBuffer;
	UINT8				ucFontType;
	UINT8				ucFontSize;
	UINT8				ucShowTimes;
}ST_DSAD_OSD_SHOW_INFO;

//10��ͼƬ��ʾ��Ϣ
typedef struct _ST_DSAD_PICTURE_SHOW_INFO
{
	EN_DSAD_POS_TYPE	enPosType;
	EN_DSAD_PIC_TYPE	enPicType;
	UINT16				usStartX;
	UINT16				usStartY;
	UINT16				usWidth;
	UINT16				usHeigth;
	UINT32				uiDataLen;
	UINT8				aucElementId[4];
	UINT8* 				pucPicData;
}ST_DSAD_PICTURE_SHOW_INFO;

//11������Ƶ��ʾ��Ϣ
typedef struct _ST_DSAD_AV_SHOW_INFO
{
	EN_DSAD_POS_TYPE	enPosType;
	EN_DSAD_AV_TYPE		enAvType;
	UINT16				usStartX;
	UINT16				usStartY;
	UINT16				usWidth;
	UINT16				usHeigth;
	UINT32				uiDataLen;
	UINT8				aucElementId[4];
	UINT8* 				pucAvData;
}ST_DSAD_AV_SHOW_INFO;

//12��ͼƬ��Ƶ�����ʾ��Ϣ
typedef struct _ST_DSAD_PROGRAM_SHOW_INFO
{
	ST_DSAD_PICTURE_SHOW_INFO stPictureShow;
	ST_DSAD_AV_SHOW_INFO stAvPlay;
	UINT8 aucFlag[2];
}ST_DSAD_PROGRAM_SHOW_INFO;

//13��OSD��ʾ��Ϣ//
typedef struct _ST_DSAD_OSD_PROGRAM_SHOW_INFO
{
	ST_DSAD_OSD_SHOW_INFO stTextShow[15];	
	UINT8 aucFlag[15];
	UINT8 ucOsdNum;
}ST_DSAD_OSD_PROGRAM_SHOW_INFO;

//14���������Ϣ
typedef struct _ST_DSAD_LOG_INFO
{
	UINT8 ucLogType;//1ͼƬ��2��Ƶ
	union
	{
		ST_DSAD_AV_SHOW_INFO stAvType;
		ST_DSAD_PICTURE_SHOW_INFO stPictureType;
	}Element;
}ST_DSAD_LOG_INFO;

//14�������ݵ�����
typedef enum _EN_DSAD_DATA_ID
{
	DSAD_ELEMENT_DATA = 1,
	DSAD_DELECMD_DATA = 2,
	DSAD_CONFIG_DATA = 3,
    DSAD_ELEMENT2_DATA = 4,
    DSAD_ELEMENT3_DATA = 5, 
    DSAD_ELEMENT4_DATA = 6,
    DSAD_ELEMENT5_DATA = 7,
    DSAD_ELEMENT6_DATA = 8,
	DSAD_TOTAL_TYPE,
}EN_DSAD_DATA_ID;

//15���������ش�����
typedef void (* FilterNotifyFunction)(UINT16 usPid, IN UINT8* pucDataBuffer, UINT16 usDataLen);

//16����������������
typedef struct _ST_DSAD_FILTER_INFO
{
	EN_DSAD_DATA_ID enDataId;//8
	UINT16 usChannelPid;//16
	UINT8 ucFilterLen;//8
	UINT8 aucFilter[15];//15*8
	UINT8 aucMask[15];//15*8
	UINT8 ucWaitSeconds;
	FilterNotifyFunction pfDataNotifyFunction;
}ST_DSAD_FILTER_INFO;

//17����Ŀ����
typedef struct _ST_DSAD_PROGRAM_PARA
{
	UINT16 usNetworkId;
	UINT16 usTsId;
	UINT16 usServiceId;
}ST_DSAD_PROGRAM_PARA;

//18
typedef struct _ST_DSAD_MSG_QUEUE
{
	UINT32 uiFirstPara;
	UINT32 uiSecondPara;
	UINT32 uiThirdPara;
	UINT32 uiFourthPara;
}ST_DSAD_MSG_QUEUE;

//19
typedef enum _EN_DSAD_MSG_QUEUE_MODE
{
	DSAD_MSG_QUEUE_WAIT,
	DSAD_MSG_QUEUE_NOWAIT,
	DSAD_MSG_QUEUE_UNKNOW,
}EN_DSAD_MSG_QUEUE_MODE;

/***************************************stb�ṩ�ĺ���******************************************/
extern void DSAD_Printf(OUT const INT8 *string,...);

extern BOOL DS_AD_Flash_Read(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen);
extern BOOL DS_AD_Flash_Write(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen);
extern BOOL DS_AD_Flash_Erase(UINT32 puiStartAddr,  UINT32 uiLen);

extern void* DSAD_Malloc(UINT32 uiBufferSize);
extern void DSAD_Free(IN void* pucBuffer);
extern void DSAD_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize);
extern void DSAD_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSAD_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSAD_StrLen(IN const UINT8* pucFormatBuffer);
extern INT32 DSAD_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...);

extern void DSAD_Sleep(UINT16 usMilliSeconds);	
extern BOOL DSAD_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun );
extern void DSAD_SemaphoreInit(IN DSAD_Semaphore* puiSemaphore , UINT8 ucInitVal);
extern void DSAD_ReleaseSemaphore(IN DSAD_Semaphore* puiSemaphore);
extern void DSAD_WaitSemaphore(IN DSAD_Semaphore* puiSemaphore);
extern BOOL DSAD_MsgQueueInit(IN INT8* pucName, UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_DSAD_MSG_QUEUE_MODE enMode);
extern BOOL DSAD_MsgQueueGetMsg(UINT32 uiMsgHandle, ST_DSAD_MSG_QUEUE* pstMsg, EN_DSAD_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond);
extern BOOL DSAD_MsgQueueSendMsg(UINT32 uiMsgHandle, ST_DSAD_MSG_QUEUE* pstMsg);

extern EN_DSAD_FUNCTION_TYPE DS_AD_SetFilter(ST_DSAD_FILTER_INFO *pstFilterInfo);
extern EN_DSAD_FUNCTION_TYPE DS_AD_StopFilter(EN_DSAD_DATA_ID enDataID, UINT16 usPID);

extern void DSAD_ShowMessage(UINT8 ucMessageType, IN UINT8 * pucMessage);
/**********************************************************************************************/

/******************************************���ģ���ṩ��stb���õĺ���****************************************************/
void DSAD_SetDataPid(UINT16 usADPid);
BOOL DSAD_Init(UINT8 ucPriority, UINT32 uiFlashStartAddress, EN_DSAD_STB_DECODE_TYPE enStbDecodeType);
EN_DSAD_FUNCTION_TYPE DSAD_ProgramGetElement(ST_DSAD_PROGRAM_PARA stProgramPara, ST_DSAD_PROGRAM_SHOW_INFO *pstProgramShow,EN_DSAD_POS_TYPE enAdPos);
EN_DSAD_FUNCTION_TYPE DSAD_NoProgramGetElement(ST_DSAD_PROGRAM_SHOW_INFO *pstProgramShow,EN_DSAD_POS_TYPE enAdPos);
EN_DSAD_FUNCTION_TYPE DSAD_ProgramGetOSD(ST_DSAD_PROGRAM_PARA stProgramPara,ST_DSAD_OSD_PROGRAM_SHOW_INFO *pstOsdProgramShow);//2013-4-7 wsq ��
EN_DSAD_FUNCTION_TYPE DSAD_GetADVersion(UINT8 * pucADVersion);
/**********************************************************************************************/

/*********************************DRV API*************************************/
BOOL ds_ad_nim_lock(u32 freq, u32 symbolrate, u16 qammode);
BOOL ds_ad_set_filter_nit_table();
void ds_ads_client_init();
void ds_ad_info_parser(ST_DSAD_PROGRAM_SHOW_INFO *p_show_info);
void ds_ad_osd_info_parser(ST_DSAD_OSD_PROGRAM_SHOW_INFO *p_osd_show);
u16 ds_ad_get_pid();
/*****************************************************************************/
#endif
