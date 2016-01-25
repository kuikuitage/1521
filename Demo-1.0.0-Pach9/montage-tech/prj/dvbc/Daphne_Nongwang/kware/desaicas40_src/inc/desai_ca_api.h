#ifndef __DS_SC_H__
#define __DS_SC_H__

/****************************************************************/
/****************************************************************/
/*提示信息*/
#define		MESSAGE_CANCEL_TYPE		0x00           
#define		MESSAGE_BADCARD_TYPE		0x01		/*无法识别卡，不能使用*/
#define		MESSAGE_EXPICARD_TYPE		0x02		/*智能卡已经过期，请更换新卡*/
#define		MESSAGE_CARD_INIT_TYPE		0x03		/*智能卡正在初始化*/
#define		MESSAGE_INSERTCARD_TYPE		0x04		/*加密节目，请插入智能卡*/
#define		MESSAGE_CARD_INIT_ERR_TYPE	0x05		/*智能卡初始化失败*/
#define		MESSAGE_NOOPER_TYPE		0x10		/*卡中不存在节目运营商*/
#define		MESSAGE_CARDSTOP_TYPE		0x11		/*智能卡已停用*/
#define		MESSAGE_OUTWORKTIME_TYPE	0x12		/*不在工作时段内*/
#define		MESSAGE_WATCHLEVEL_TYPE		0x13		/*节目级别高于设定观看级别*/
#define		MESSAGE_PAIRING_TYPE		0x14		/*机卡没有对应*/
#define		MESSAGE_NOENTITLE_TYPE		0x20		/*您没有购买该节目*/
#define		MESSAGE_DECRYPTFAIL_TYPE	0x21		/*节目解密失败*/
#define		MESSAGE_NOMONEY_TYPE		0x22		/*卡内金额不足*/
#define		MESSAGE_ERRREGION_TYPE		0x23		/*区域不正确*/
#define		MESSAGE_FINGER_SHOW		0x24		/*指纹显示*/
#define		MESSAGE_FINGER_DISAPPEAR	0x30		/*指纹隐藏*/
#define		MESSAGE_CARDNOTACTIVE_TYPE	0x31		/*智能卡未激活*/
#define		MESSAGE_CARDLOGOUT_TYPE		0x32		/*智能卡已注销*/

#define    	MESSAGE_ZBCardPAIRING_OK	0x40		/* 子母卡配对成功 */
#define    	MESSAGE_ZBCardPAIRING_Err	0x41		/* 子母卡配对失败,请重新插入正确的母卡 */
#define    	MESSAGE_ZCardPAIRING_REQ	0x42		/* 子卡需重新和母卡配对 */
#define		MESSAGE_CARD_MATCH_OK		0x43		/*机卡配对成功*/
#define		MESSAGE_REMATCH_TYPE		0x44		/*重新机卡配对*/
#define		MESSAGE_CANCEL_MATCH_TYPE	0x45		/*取消机卡配对*/

#define		MESSAGE_SHOWNEWEMAIL		0x50		/* 显示新邮件图标 */  
#define		MESSAGE_SHOWOVEREMAIL		0x51		/* 显示邮件存满 */
#define		MESSAGE_HIDENEWEMAIL		0x52		/* 隐藏新邮件图标*/
#define		MESSAGE_EXIGENT_PROGRAM		0x53		/*紧急广播*/
#define		MESSAGE_STOP_EXIGENT_PROGRAM	0x54		/*停止紧急广播*/
#define		MESSAGE_CARD_UNLOCK_OK		0x55		/*智能卡密码解锁成功*/
#define		MESSAGE_CARD_PURSER_UPDATE_OK	0x56		/*电子钱包更新*/

/*OSD消息*/
#define		MESSAGE_SHOWOSD			0x60		/*显示OSD消息*/
#define		MESSAGE_HideOSD			0x61		/*隐藏OSD消息*/


/*提示对话框*/
#define		MESSAGE_CARDISDUE_TYPE		0x70		/*智能卡处于欠费催缴*/
#define		MESSAGE_IPPVBUY			0x71		/*Ippv购买提示框 */
#define		MESSAGE_IPPV_OUT_TIME		0x72		/*Ippv节目过期*/
#define		MESSAGE_HIDE_IPPVBUY		0x73		/*隐藏 Ippv购买提示框 */
#define		MESSAGE_ReqPAIRING_TYPE		0x74		/*请求节目配对入网测试用*/
/*内部测试卡显示信息*/
#define		MESSAGE_URGE_OPEN_TYPE			0x80
#define		MESSAGE_URGE_CLOSE_TYPE			0x81
#define		MESSAGE_URGE_DAYS_TYPE			0x82
#define		MESSAGE_URGE_OSD_SHOWTIME_TYPE		0x83
#define		MESSAGE_ALLCARD_ADDR_CTL_OPEN_TYPE	0x84
#define		MESSAGE_ALLCARD_ADDR_CTL_CLOSE_TYPE	0x85
#define		MESSAGE_CARD_PAIR_DAYS_TYPE		0x86
#define		MESSAGE_WT_OPEN_TYPE			0x87
#define		MESSAGE_WT_CLOSE_TYPE			0x88
#define		MESSAGE_WL_OPEN_TYPE			0x89
#define		MESSAGE_WL_CLOSE_TYPE			0x8A
#define		MESSAGE_ALLCARD_FINGER_OPEN_TYPE	0x8B
#define		MESSAGE_ALLCARD_FINGER_CLOSE_TYPE	0x8C
#define		MESSAGE_FINGER_SHOW_TIME_TYPE		0x8D
#define		MESSAGE_FINGER_STOP_TIME_TYPE		0x8E
#define		MESSAGE_DUE_OPEN_TYPE			0x8F
#define		MESSAGE_DUE_CLOSE_TYPE			0x90
#define		MESSAGE_DUE_DAYS_TYPE			0x91
#define		MESSAGE_CARD_OVERDUE_TIME_TYPE		0x92
#define		MESSAGE_URGE_OSD_UPDATE_TYPE		0x93
#define		MESSAGE_EXIGENT_OSD_UPDATE_TYPE		0x94
#define		MESSAGE_CARD_TYPE_TYPE			0x95
#define		MESSAGE_MOTHER_CARD_UPDATE_TYPE		0x96
#define		MESSAGE_CARD_FINGER_OPEN_TYPE		0x97
#define		MESSAGE_CARD_FINGER_CLOSE_TYPE		0x98
#define		MESSAGE_CARD_ADDR_CTL_OPEN_TYPE		0x99
#define		MESSAGE_CARD_ADDR_CTL_CLOSE_TYPE	0x9A
#define		MESSAGE_CARD_ADDR_UPDATE_TYPE		0x9B
#define		MESSAGE_ENTITLE_TYPE_UPDATE_TYPE	0x9C
#define		MESSAGE_ALL_CARD_ENTITLE_OPEN_TYPE	0x9D
#define		MESSAGE_ALL_CARD_ENTITLE_CLOSE_TYPE	0x9E
#define		MESSAGE_ALL_CARD_ENTITLE_UPDATE_TYPE	0x9F
#define		MESSAGE_EXIGENT_PROG_OPEN_TYPE		0xA0
#define		MESSAGE_EXIGENT_PROG_CLOSE_TYPE		0xA1
/****************************************************************/
/****************************************************************/

//#define NET_TEST               /*入网测试*/

#define    IN                         /*函数参数(外部传入数据)*/
#define	   OUT                      /*函数参数(函数内部回传数据)*/
#define    DSCA_CARD_REGION_DESC   0XA5/*NIT表中智能卡区域ID描述符*/

#ifndef		NULL
#define		NULL		0
#endif

typedef char				INT8;
typedef unsigned char		UINT8;
typedef short				INT16;
typedef unsigned short		UINT16;
typedef int				INT32;
typedef unsigned int		UINT32;
typedef UINT32			DSCA_Semaphore;
typedef UINT32			DSCA_MsgQueue; 

//时间格式
typedef struct _ST_TIME_INFO
{
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_TIME_INFO;

//日期时间格式
typedef struct _ST_DATE_TIME_INFO
{
	UINT16	usYear;
	UINT8	ucMonth;
	UINT8	ucDay;
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DATE_TIME_INFO;

//邮件信息
typedef struct _ST_MAIL_INFO
{
	UINT8				ucMailID;				// Mail ID 
	UINT8				ucNewFlag;				// 0 不是新邮件 1是新邮件 
	UINT8				aucMailTitle[32 + 1];		// 邮件标题，最长为30
	UINT8				aucMailSender[32 + 1];	//邮件发送人
	ST_DATE_TIME_INFO	stSendTime;				//邮件发送时间(接收时间)
}ST_MAIL_INFO;

//产品授权信息
typedef struct  _ST_PRODUCT_ENTITLE
{
	UINT8			ucEnableType;		/*授权模式*/
	UINT8          		ucRecoadFlag;		/* 暂没有用 ，0：不可以录像 1：可以录像*/
	UINT16        		usProductID;		/* 产品ID */
	ST_DATE_TIME_INFO 	stBeginTime;		/* 产品起始时间*/
	ST_DATE_TIME_INFO 	stEndTime;		/* 产品结束时间*/
}ST_PRODUCT_ENTITLE;

//邮件列表信息
typedef struct _ST_MAILLIST_INFO
{
	ST_MAIL_INFO	stMailInfo[50];
	UINT8 		ucMailNum;
	
}ST_MAILLIST_INFO;

//运营商信息
typedef struct _ST_OPERATOR_INFO
{
	UINT8 aucOperatorName[40];
	UINT8 aucOperatorPhone[40];
	UINT8 aucOperatorAddress[80];
	
}ST_OPERATOR_INFO;

//IPPV信息
typedef struct _ST_IPPV_INFO
{
	UINT8			aucIppvName[33];
	UINT8			aucEventID[4];
	UINT8			ucProviderID;     
	UINT16			usPrice;  
	ST_DATE_TIME_INFO	stBeginTime;
	ST_DATE_TIME_INFO	stEndTime;

}ST_IPPV_INFO;

//指纹显示位置
typedef enum _EN_FINGER_SHOW_POS
{
	SHOW_LEFT_TOP = 0,
	SHOW_RIGHT_TOP,
	SHOW_LEFT_BOTTOM,
	SHOW_RIGHT_BOTTOM,
	
}EN_FINGER_SHOW_POS;

//指纹显示信息
typedef struct _ST_FINGER_SHOW_INFO
{
	EN_FINGER_SHOW_POS enShowPostion;		/*显示位置*/
	UINT8 ucShowColor[4];					/*显示颜色*/
	UINT8 ucShowFont;						/*显示字体*/
	UINT8 ucShowFontSize;
	UINT8 pucShowContent[20];				/*显示内容*/
	
}ST_FINGER_SHOW_INFO;

//OSD显示位置
typedef enum _EN_OSD_SHOW_POS
{
	SHOW_TOP = 0,					//在顶部显示
	SHOW_BOTTOM,					//在底部显示
	SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
	SHOW_LEFT,						//在左边显示
	SHOW_RIGHT,					//在右边显示	
	SHOW_LEFT_RIGHT,				//在左边和右边同时显示
}EN_OSD_SHOW_POS;

//OSD显示信息
typedef struct _ST_OSD_SHOW_INFO
{
	EN_OSD_SHOW_POS	ucDisplayPos;
	UINT8	ucFontType;
	UINT8	ucFontSize;
	UINT8	aucBackgroundColor[4];
	UINT8	aucDisplayColor[4];
	UINT16	usContentSize;
	UINT16	usGetBufferSize;
	UINT8* 	pucDisplayBuffer;
	
}ST_OSD_SHOW_INFO;

//工作时段信息
typedef struct _ST_WORKTIME_INFO
{
	ST_TIME_INFO stBeginTime;
	ST_TIME_INFO stEndTime;
}ST_WORKTIME_INFO;

typedef enum _EN_CA_LANGUAGE
{
	CA_LANGUAGE_CHINESE,
	CA_LANGUAGE_ENGLISH,
	CA_LANGUAGE_TOTAL,
}EN_CA_LANGUAGE;

typedef struct _ST_CA_EXIGENT_PROGRAM
{
	UINT16 usNetWorkID;
	UINT16 usTransportStreamID;
	UINT16 usServiceID;
}ST_CA_EXIGENT_PROGRAM;

typedef struct _ST_CA_MSG_QUEUE
{
	UINT32 uiFirstPara;
	UINT32 uiSecondPara;
	UINT32 uiThirdPara;
	UINT32 uiFourthPara;
}ST_CA_MSG_QUEUE;

/* 智能卡通讯结果定义*/
typedef enum _EN_CAS_SMC
{
	CAS_SMC_NORESP = -4,				/* 智能卡无响应*/
	CAS_SMC_LEN_MISMATCH,			/* 输入长度不匹配*/
	CAS_SMC_INVALIDCARDNO,			/* 输入的卡号无效*/
	CAS_SMC_REMOVED,					/* 智能卡不存在*/
	CAS_SMC_OK = 0					       /* 智能卡发送命令成功*/
}EN_CAS_SMC;

typedef enum _EN_CAS_SHOW_MSG_TYPE
{
	CAS_MSG_TYPE_PICTURE,//显示图片
	CAS_MSG_TYPE_FINGER_SHOW,//显示指纹
	CAS_MSG_TYPE_OSD,//显示osd
	CAS_MSG_TYPE_NOTE_LONG_TIME,//长期显示的消息
	CAS_MSG_TYPE_NOTE_SHORT_TIME,//显示时间很短的消息
	CAS_MSG_TYPE_DIALOG,//对话框
	CAS_MSG_TYPE_NOT_SHOW,//只是进行某种操作，不涉及到显示
}EN_CAS_SHOW_MSG_TYPE;

//函数返回值类型
typedef enum _EN_ERR_CAS
{
	/*错误类型定义*/
	CA_ERROR_OK = 0x00,
	CA_CARD_FAILURE,                           /*卡通讯失败*/
	CA_CARD_SUCCESS,                             /*卡通讯成功*/
	CA_CARD_NOMONEY,                            /*卡内金额不足*/
	CA_CARD_WRITEPURSER_ERROR,          /*写钱包失败*/
	CA_CARD_ERROR_REGION,		     /*区域不正确*/
	CA_CARD_CHECK_PIN_OK,                   /*验证PIN码成功*/
	CA_CARD_CHECK_PIN_ERROR,		     /*验证PIN码失败*/
	CA_CARD_CHANGE_PIN_FAILED,        /*修改PIN失败*/
	CA_CARD_CHECK_PIN_LOCK,               /*验证PIN码次数已达最大，文件被锁*/
	CA_CARD_UNLOCK_PIN_FAILED,	     /*解锁PIN失败*/
	CA_ERROR_IC_INVALID ,                     /* 机顶盒内无卡或无效卡         */ 
	CA_ERROR_CARD_PAIROTHER,            /* 智能卡已经对应别的机顶盒     */
	CA_ERROR_CARD_STOP,        			/* 卡已停用    */
	CA_ERROR_CARD_CANCELED,        			/* 卡已注销  */
	CA_ERROR_CARD_OUTTIME,			/* 卡已过期 */
	CA_ERROR_CARD_PENDING,        			/* 卡未激活    */
	CA_ERROR_CARD_NOPAIR,                  /* 不要求机卡配对*/
	CA_ERROR_PARAMETER,                      /*参数错误*/   
	CA_ERROR_FLASHADDR,                      /*FLASH地址错误（为NULL）*/
	CA_ERROR_UNKOWNDATA,                  /*未知数据，不是CA所需要的数据*/
	CA_ERROR_REPEATINGDATA,             /*重复的数据*/
	CA_ERROR_PAIRED_OK,                      /*字母卡配对成功*/
	CA_ERROR_WORKTIME,                         /*不在工作时段*/
	CA_ERROR_WATCH_LEVLE,                 /*当前节目观看级别大于设置级别*/
	CA_ERROR_PAIRED_FAILED,              /*字母卡配对失败*/
	CA_ERROR_PAIRED_REPAIRED,          /*字母卡需要重新配对*/
	CA_ERROR_NOENTITLE,                      /*没有授权*/
	CA_ERROR_IPPV_NOBUY,                    /*IPPV未购买*/
	CA_ERROR_NETTEST,                         /*入网测试*/
	CA_ERROR_CRC,                                 /*CRC错误*/
	CA_ERROR_WORKTIME_FORMAT,			/*工作时段错误*/
	CA_ERROR_MALLOC_ERR,			/*内存分配失败*/
	CA_ERROR_UNKNOWN,                       /* 未知错误*/
}EN_ERR_CAS;

typedef enum _EN_CA_MSG_QUEUE_MODE
{
	CA_MSG_QUEUE_WAIT,
	CA_MSG_QUEUE_NOWAIT,
	CA_MSG_QUEUE_UNKNOW,
}EN_CA_MSG_QUEUE_MODE;

typedef enum _EN_CA_DATA_ID
{
	ECM_DATA_ID = 1,
	EMM_DATA_ID = 2,
	OSD_DATA_ID = 3,
	MAIL_DATA_ID = 4,
	DELE_OSD_DATA_ID = 5,
	CONFIG_DATA_ID = 6,
	SK_DATA_ID = 7,
	TOTAL_DATA_TYPE = 8,
}EN_CA_DATA_ID;

/***********************************机顶盒提供给CA的函数**************************************/
extern void DSCA_printf(OUT const INT8 *string,...);
extern void DSCA_GetBuffer(IN UINT32 * puiStartAddr,  IN UINT32 * puiSize);
extern void DSCA_FlashRead(UINT32 uiStartAddr,  IN UINT8 * pucData,  OUT UINT32 * puiLen);
extern void DSCA_FlashWrite(UINT32 uiStartAddr, OUT UINT8* pucData, UINT32 uiLen);
extern void DSCA_FlashErase(void);
extern void* DSCA_Malloc(UINT16 usBufferSize);
extern void DSCA_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize);
extern void DSCA_Memcpy(IN void* pucDestBuffer, OUT void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...);
extern INT32 DSCA_StrLen(IN const UINT8* pucFormatBuffer);
extern void DSCA_Free(IN void* pucBuffer);
extern void DSCA_Sleep(UINT16 usMilliSeconds);				       
extern void DSCA_SemaphoreInit(IN DSCA_Semaphore* puiSemaphore , UINT8 ucInitVal);
extern void DSCA_ReleaseSemaphore(OUT DSCA_Semaphore* puiSemaphore);
extern void DSCA_WaitSemaphore(OUT DSCA_Semaphore* puiSemaphore);
extern void DSCA_ShowMessage(UINT8 ucMessageType, EN_CAS_SHOW_MSG_TYPE  enShowType, OUT UINT8 * pucMessage);
extern void DSCA_SetCW(UINT16 usEcmPID, OUT UINT8* pucOddKey, OUT UINT8* pucEvenKey, OUT UINT8* pucKeyLen);
extern void DSCA_SendDataToSmartCard(OUT UINT8* pucSend, OUT UINT8 *pucSendLen, IN UINT8* pucReceive,
					IN UINT8 *pucReceiveLen, UINT32 uiTimeout, IN UINT8* pucStatusWord );
extern UINT8 DSCA_RegisterTask (OUT INT8* pucName, UINT8 ucPriority, OUT void* pTaskFun );
extern UINT8 DSCA_SCReset(UINT8 ucCardNumber);
extern UINT8 DSCA_SetFilter(EN_CA_DATA_ID enDataID, OUT UINT8* pucFilter, OUT UINT8* pucMask,
				UINT8 ucFilterLen, UINT16 usPID, UINT8 ucWaitSeconds);
extern UINT8 DSCA_StopFilter(EN_CA_DATA_ID enDataID, UINT16 usPID);
extern UINT8 DSCA_QueryProgram(UINT16 usNetWorkID, UINT16 usTransportStreamID, UINT16 usServiceID);
extern UINT8 DSCA_MsgQueueInit(OUT INT8* pucName, IN UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_CA_MSG_QUEUE_MODE enMode);
extern UINT8 DSCA_MsgQueueGetMsg(UINT32 uiMsgHandle, IN ST_CA_MSG_QUEUE* pstMsg, EN_CA_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond);
extern UINT8 DSCA_MsgQueueSendMsg(UINT32 uiMsgHandle, IN ST_CA_MSG_QUEUE* pstMsg);
/*****************************************************************************************************/

/*******************************CA 提供给机顶盒调用的函数********************************/
void DSCA_SetEcmPid(UINT16 usPid);
void DSCA_SetEmmPid(UINT16 usPid);
void DSCA_SetCardState(UINT8  ucInOrOut);
void DSCA_SetSysLanguage(EN_CA_LANGUAGE enLanguage);
void DSCA_SetCardAddressCode(IN UINT8* pucCardCode);
void DSCA_OsdOneTimeOk(void);
void DSCA_GetPrivateData(UINT16 usPid, EN_CA_DATA_ID enDataID, IN UINT8* pucReceiveData, UINT16 usDataLen);
UINT8 DSCA_IsDSCa(UINT16 usCaSystemID);
EN_ERR_CAS DSCA_SC_UnlockPin(void);
EN_ERR_CAS DSCA_DelEmail(UINT8 ucMailID);
EN_ERR_CAS DSCA_DelAllEmail(void);
EN_ERR_CAS DSCA_GetCardRegion(OUT UINT8 * pucCardRegion);
EN_ERR_CAS DSCA_GetOperatorID(OUT UINT8 * pucOperator);
EN_ERR_CAS DSCA_GetOperatorInfo(OUT ST_OPERATOR_INFO* pstOperatorInfo );
EN_ERR_CAS DSCA_SetWatchLevelState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_SetWorkTimeState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_IppvBuy(UINT8 ucBuy,UINT8 ucProvideID) ;
EN_ERR_CAS DSCA_Init(UINT8 ucPriority, EN_CA_LANGUAGE enLanguage);
EN_ERR_CAS DSCA_Pairing(UINT8 ucYesOrNo);
EN_ERR_CAS DSCA_GetProductNumber(UINT32 uiOperatorID, OUT UINT32 *puiNumber);
EN_ERR_CAS DSCA_GetProductInfo (UINT32 uiOperatorID,  UINT32 uiStartNo,  UINT32 uiReadNum, OUT ST_PRODUCT_ENTITLE *pstProductEntitles);
EN_ERR_CAS DSCA_GetCardNo(OUT UINT8 * pucCardNo, OUT UINT8* pucCardType);
EN_ERR_CAS DSCA_GetCAVer(OUT UINT8 * pucCAVersion);
EN_ERR_CAS DSCA_GetPurserInfo(UINT8 ucNo, OUT UINT8 * pucPurser);
EN_ERR_CAS DSCA_GetEmailNumber(OUT UINT8 *pstEmailNo);
EN_ERR_CAS DSCA_GetEmailInfoList(OUT ST_MAILLIST_INFO *pstEmailInfoList);
EN_ERR_CAS DSCA_GetEmailContent(UINT32 uiEmailID, OUT UINT8* pucContentBuf, OUT UINT16 *pusContentLen);
EN_ERR_CAS DSCA_GetCardState(void);
EN_ERR_CAS DSCA_ChangePin(IN UINT8* pucOldPin, IN UINT8* pucNewPin, OUT UINT8* ucOddTimes);
EN_ERR_CAS DSCA_PurseCheckPin(UINT8* pucPin, UINT8* pucOddTimes);
EN_ERR_CAS DSCA_SetWorkTime(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_SetWatchLevel(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN UINT8* pucWorkLevel);
EN_ERR_CAS DSCA_GetWorkTime(OUT ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_GetWorkLevel(OUT UINT8* pucWorkLevel);
/****************************************************************************************************/
#endif

