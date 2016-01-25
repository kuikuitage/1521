

#ifndef __XINSHIMAO_CA_H_
#define __XINSHIMAO_CA_H_

#ifdef __cplusplus
extern "C" {
#endif


#define SUPPORT_MATHER_SON_CARD
#define SUPPORT_CARD_STB_PAIR
#define SUPPORT_EMAIL_OSD
#define SUPPORT_REGION_LIMIT

typedef signed char INT8;
typedef signed short INT16;
typedef signed long  INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef unsigned char  CAS_BOOL;

#define   CA_TASK_PRIORITY   9// 低于gui task 以免 ECM parse 线程抢占CA线程
#define   SC_TASK_PRIORITY   7// 要高于CA_TASK_PRIORITY，因为可能会有 card resum 调用




#define CAS_SUCCESS 0
#define CAS_FAILTURE -1
#define CAS_TRUE 1
#define CAS_FALSE 0
#define CAS_NULL				(0)


#define CA_INFO_CLEAR 0//清除上一次 设置的ECM，EMM PID
#define CA_INFO_ADD    1 //增加设置 ECM，EMM PID

#define CA_OSD_ON_TOP   0
#define CA_OSD_ON_BOTTOM 1

//------------------------------------------------------------------------------------
#define CA_STB_FILTER_1 1 //emm 授权
#define CA_STB_FILTER_2 2 //emm osd email
#define CA_STB_FILTER_3 3// emm osd email
#define CA_STB_FILTER_4 4 //ecm 

#define ECM_FITLER_TIMEOUT (100)//()//单位:毫秒 传到 U8 结构类型中 要小于250
#define EMM_FITLER_TIMEOUT (250)//()//单位:毫秒 传到 U8 结构类型中 要小于250


#define     SC_NORMAL                     0  /*正常状态*/
#define     SC_NO_CARD   			1  /*未插卡*/
#define     SC_NO_PPV_RIGHT  		2  /*没有PPV授权*/
#define     SC_NO_PPC_RIGHT  		3  /*没有PPC授权*/
#define     SC_PARENT_CTRL  		4 /*家长控制*/
#define     SC_NO_CONDITION  		5 /*条件限播*/
#define     SC_INVALID_CARD          6  /*无效卡*/
#define	  SC_TYPEERROR			7  /*子母卡喂养失败,插入智能卡类型错误*/
#define	  SC_NEEDFEED			8  /*子卡需要与母卡对应,请喂养子卡*/
#define     SC_ZONEERROR               9  /*区域错误*/
#define     SC_FEEDTIMEERROR        10 /*喂养时间错误*/
#define     SC_FEEDDATEERROR        11 /*喂养日期错误*/
#define     SC_FEEDSYSTEMTIMEERROR  12 /*系统时间没有正确获取,喂养失败*/
#define     SC_ACSEVICEIDEERROR  13 /*加扰机AC 错误*/
#define     SC_CARDSTBNOTPAIR  14 /*加扰机AC 错误*/
#define     SC_CARDENTITLEREMAINTDAY  15 /*授权剩余天数*/
#define     SC_UPDATAINT  16 /*智能卡数据更新，请勿断电!*/




typedef struct {
        UINT32 q1stWordOfMsg;
	 UINT32 q2ndWordOfMsg;
	 UINT32 q3rdWordOfMsg;
	 UINT32 q4thWordOfMsg;
}XinShiMao_Queue_message;

/* -------------------------------------service info-------------------------------*/
#define CA_MAX_SERVICE_PER_ECM 1
typedef struct _CAServiceInfo {
	UINT16 m_wEcmPid;
	UINT8 m_bServiceCount;//默认填1
	UINT16 m_wServiceId[CA_MAX_SERVICE_PER_ECM];
}XinShiMao_CAServiceInfo;

//------------------------------------------------------------------------------------
typedef struct _CAOSDInfo{
	UINT8 m_bOSD_Position;//显示位置 0:top,1:bottom
	UINT16 m_wOSD_Show_Circle;//是否循环显示
	UINT8 m_bOSD_Text_length;
	UINT8 m_bOSD_Text[150];
}XinShiMao_CAOSD_Info;

typedef struct _TXinShiMao_DATETIME
{
	UINT16 Y;   //年
	UINT16 M;  //月
	UINT8   D;  //日
	UINT8   H;  //时
	UINT8   MI; //分
	UINT8   S;  //秒
}TXinShiMao_DATETIME;


#define   MAX_EMAIL_NUM            10
#define   EMAIL_TITLE_LEN          36
#define   EMAIL_CONTENT_LEN      128

typedef struct _TXinShiMao_EmailHead{
	TXinShiMao_DATETIME	m_tCreateTime;    
	UINT8    		m_bEmail_Level;    /*重要程度*/
	UINT8		m_bNewEmail;         /*0:旧邮件 1:新邮件*/
	UINT16		m_bEmailID;          
	UINT8   		m_szEmailTitle[EMAIL_TITLE_LEN];
}TXinShiMao_EmailHead;

typedef struct _TXinShiMao_EmailContent{
	UINT8		m_szEmail[EMAIL_CONTENT_LEN];
}TXinShiMao_EmailContent;


typedef enum{
	XINSHIMAO_ERR = -100,
	XINSHIMAO_OK = 0
}XINSHIMAO_RESULT;

/* -------------------------------------init ca core-------------------------------*/
extern INT32 XinShiMao_Initialize(UINT8 mailManagerType);

/* --------------------------------------ca info-------------------------------*/
extern INT32 XinShiMao_GetSMCNO(UINT8 * pbCardno);
//智能卡是否有授权标志
extern INT32 XinShiMao_GetSMCEntitle(UINT8 * isEntitle);
//子母卡，0:母卡，1:子卡
extern INT32 XinShiMao_GetSMCType(UINT8 * isType);
//0:自由配对，1:不配对，2:机卡已配对
extern INT32 XinShiMao_GetSMCStbPaired(UINT8 * isCardStbPaired);
//获取智能卡的网络ID号
extern UINT16 XinShiMao_GetCardRegionID( void );
//设置NIT网络ID号
extern void XinShiMao_SetNetWorkRegionID(UINT16 chRegionID);
//设置机顶盒的ID号，2字节，用于区域控制，开机初始化好库后调用
extern void XinShiMao_SetStbRegionID(UINT16 chRegionID);

//获取授权的剩余天数，如果小于15天，机顶盒每隔半小时在大画面上提醒一次
extern UINT8 XinShiMao_GetEntitleRemainDay(void);

/* --------------------------------------ca event-------------------------------*/
/*提 供 者： 机顶盒 

输入参数： event 为消息类型1: 为OSD消息；4为邮件消息
				2,3 大画面的消息提示:param1 为消息提示的内容 */
extern void  XinShiMao_EventHandle(UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3);


/* --------------------------------------ca smart -------------------------------*/
extern INT32 XinShiMao_SC_DRV_Initialize(void);
//bCardNumber 默认0
extern void  XinShiMao_SC_DRV_ResetCard(UINT8 bCardNumber);
/*提 供 者： 机顶盒。 
输入参数： 
		bCardNumber： 暂时保留不用； 
		bLength： pabMessage的长度； 
		pabMessage： 发送命令的消息指针； 
		pabResponse： 接收响应结果的数据块的指针； 
		bRLength： 响应结果的数据块长度指针。 
输出参数： 
		pabResponse： 响应结果的数据块； 
		bRLength： 响应结果的数据块长度。 
		pbSW1 智能卡状态字节1 
		pbSW2 智能卡状态字节2 
返 回 值： 
		CAS_TRUE：成功
		CAS_FALSE：失败 */
extern INT32 XinShiMao_SC_DRV_SendDataEx(UINT8 bCardNumber,UINT8 bLength,
	                                                      UINT8 * pabMessage,
	                                                      UINT8 * pabResponse,
	                                                      UINT8 * bRLength,
	                                                      UINT8 * pbSW1,
	                                                      UINT8 * pbSW2);


/* --------------------------------------ca demux -------------------------------*/
/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前节目的ServiceID 
输入参数： 无
输出参数： 无。 
返 回 值： 当前节目的ServiceID*/
extern UINT16 XinShiMao_GetCurr_ServiceID(void);
/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Emm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Emm Pid*/
extern UINT16 XinShiMao_GetCurr_EmmID(void);
/*提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Ecm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Ecm Pid*/
extern UINT16 XinShiMao_GetCurr_EcmID(void);
//插拔卡时通知CA库，
extern void  XinShiMao_SCStatusChange(UINT8 status);
/*提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 收到CA描述符的内容后。 
功能描述： 判断CA系统ID是否是XinShiMao。 
输入参数： 
		wCASystemID，CA系统ID。 
输出参数： 无。 
返 回 值： 
		CAS_TRUE：是本CA；
		CAS_FALSE：不是本CA。*/
extern CAS_BOOL  XinShiMao_IsMatchCAID(UINT16 wCASystemID);
/*提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 当有新ECM PID时(如切换频道)调用该接口把相关信息传递给CA模块。 
功能描述： 将ECM的PID等信息传递给CA模块。 
输入参数： 
		bType： 设置的类型，清空、增加、修改当前ECMPID列表， 使机顶盒同时可以处理多路节目。为以下几个值之一:
		CAS_INFO_CLEAR：用户告诉CA清空ECMPID, pEcmInfo 须是空；
		CAS_INFO_ADD： 用户告诉CA增加一个ECMPID； 
 */
extern void  XinShiMao_SetEcmPID(UINT8 bType,XinShiMao_CAServiceInfo * pEcmInfo);
extern void  XinShiMao_SetEmmPID(UINT8 bType,UINT16 wEmmPid);
/*提 供 者： CA模块。 
使 用 者： 机顶盒。 
功能描述： 当有CA的私有数据的时候，将私有数据接收下来，并将该数据传递给 CA模块，使得CA模块可以处理这些数据。 
输入参数： 
		bOK： 收取数据有没有成功；TRUE：成功，FALSE：失败。 
		wPid： 接收的流的PID。 
		pbReceiveData： 收取私有数据的指针，CA模块不负责其空间的释放。 
		wLen： 收取到的私有数据的长度。 

pbReceiveData[0]:0x80,0x81 ecm;0x82,0x83,0x84 emm */
extern void  XinShiMao_TableReceived(UINT8 bRequestID,UINT8 bOK,UINT16 wPid,const UINT8 * pbReceiveData,UINT16 wLen);

/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 启动过滤器去接收CA的数据。 
输入参数： 
		iRequestID     ,     为如下几个值之一
						CA_STB_FILTER_1,
						CA_STB_FILTER_2
						CA_STB_FILTER_3
						CA_STB_FILTER_4
		pbFilterMatch1， 如果是过滤Ecm数据，则该参数为Ecm filter的数据；
						 如果是过滤Emm数据，则该参数为Emm filter1的数据；
		pbFilterMask1，  如果是过滤Ecm数据，则该参数为Ecm filter的mask；
						 如果是过滤Emm数据，则该参数为Emm filter1的mask；
		bLen， filter的长度。 
		wPid， 通道的PID值。 
		bWaitTime， 通道过期时间，单位毫秒，超时时间见宏定义 ECM_FITLER_TIMEOUT, EMM_FITLER_TIMEOUT
	       if ( wPid <0 && wPid >0x1fff ) 提示机顶盒DEMMUX需要释放过滤器空间，EMM可以不释放，ECM必须释放
	       ECM过滤数据TABILE_ID:0X80,0X81;emm 授权过滤TABLE id:0X82;osd,email table id:0x83
返 回 值： 
		SUCCESS：成功， 
		FAILURE： 失败。 */
extern INT32 XinShiMao_TableStart(UINT8 iRequestID,
						  const UINT8 * pbFilterMatch1,
	                                     const UINT8 * pbFilterMask1,
	                                     UINT8 bLen,
	                                     UINT16 wPid,
	                                     UINT8 bWaitTime);
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 设置解扰通道的控制字。 
输入参数： 
		wEcmPid， 控制字的所在的ecm包的PID值。 
		szOddKey， 奇控制字数据的指针。 
		szEvenKey， 偶控制字数据的指针。 
		bKeyLen， 控制字长度。 
		bReservedFlag, 保留。 
*/
extern void XinShiMao_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag);


/* --------------------------------------ca flash -------------------------------*/
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 获得机顶盒提供给CA模块保存信息的起始地址和空间大小 
输入参数： 
输出参数： 
		lStartAddr 机顶盒分配空间的起始地址 
		lSize 机顶盒分配空间的大小 
返 回 值： 
		TRUE 成功 
		FALSE 失败 */
extern CAS_BOOL XinShiMao_GetBuffer(UINT32 *lStartAddr,UINT32 * lSize);
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 从机顶盒分配空间的指定起始地址读指定长度的数据。 
输入参数： 
		lStartAddr： 要读取数据的存储空间的地址。 
		pbData： 被读取数据的存放地址指针。 
		nLen： 要读的数据的长度 
输出参数： 
		pbData： 被读出的数据。 
返 回 值： 
		返回实际读到的字节数*/
extern UINT32 XinShiMao_ReadBuffer(const UINT8 *lStartAddr,UINT8 *pData,INT32 nLen);

/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 向机顶盒的存储空间写信息 
输入参数： 
		lStartAddr： 要写的存储空间的目标地址。 
		pData： 要写的数据 
		nLen： 要写的数据的长度 
输出参数： 无。 
返 回 值： 
		TRUE：成功
		FALSE： 失败。*/
extern CAS_BOOL XinShiMao_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen);

/* --------------------------------------ca os -------------------------------*/

/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建任务 
输入参数： 
		name[]， 4个字节的任务名称。 
		stackSize， 任务所使用的堆栈的大小。 
		entryPoint， 任务的入口地址。 
		priority， 任务的优先级。 
		arg1， 传递给任务的第一个参数。 
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。*/
extern INT32 XinShiMao_OSPTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void*),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
extern INT32 XinShiMao_OSPTaskTemporarySleep(UINT32 milliSecsToWait);
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建一个消息队列。 
输入参数： 
		name[]， 4个字节的队列名称。 
		maxQueueLength，消息队列中可以存放的消息的数量。当消息队列中该数量达到 该数量时，再往该消息队列发消息将会失败。 
		taskWaitMode:可以不管
		queueId， 消息队列的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误*/
extern INT32 XinShiMao_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId);
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 
输入参数： 
		queueId， 所要取得的消息的ID。 
		messsage， 为消息的格式。参照XINSHIMAO_QUEUE_MESSAGE。 
		waitMode:目前只用到，等待直到得到消息 

		SUCCESS：成功；
		FAILURE： 发生错误。*/
extern INT32 XinShiMao_OSPQueueGetMessage(UINT32 queueId,XinShiMao_Queue_message * message,UINT32 waitMode,UINT32 milliSecsToWait);
/*提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 发送消息到消息队列。 
输入参数： 
		queueId， 消息队列ID。 
		message， 要发送的消息。其格式见XINSHIMAO_QUEUE_MESSAGE结构。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。*/
extern INT32 XinShiMao_OSPQueueSendMessage(UINT32 queueId,XinShiMao_Queue_message * message);
extern UINT32 XinShiMao_Printf(const char * fmt,...);
extern void XinShiMao_memcpy(void* pDestBuf,const void* pSrcBuf,UINT32  wSize);
extern void XinShiMao_memset(void* pDestBuf,UINT8 c,UINT32 wSize);
extern INT32  XinShiMao_memcmp(const void *buf1, const void *buf2, UINT32 count);

UINT32   XinShiMao_OS_CreateSemaphore(UINT32 ulInitialCount);  /*创建信号量*/
void  XinShiMao_OS_WaitSemaphore(UINT32 ulSemaphore);       /*等待信号量*/
void  XinShiMao_OS_SignalSemaphore(UINT32 ulSemaphore);       /*释放信号量*/


//设置机顶盒时间，机顶盒无需过滤TDT表，由CA来提供时间基准
extern void	XinShiMao_SetStbTime(TXinShiMao_DATETIME* ca_time);


/* --------------------------------------mother son card  该功能可以暂时不移植-------------------------------*/
//ret: 0:read mother card data right 1:invailed card 2:mother card and son card is not paired
extern UINT8 XinShiMao_ReadFeedDataFromParent(UINT8 * chSpbyFeedData, UINT8 *chLen);
extern UINT8 XinShiMao_WriteFeedDataToChild(UINT8 * chSpbyFeedData, UINT8 chLen);
/* --------------------------------------email 该功能可以暂时不移植-------------------------------*/
extern UINT16 XinShiMao_GetEmailHeads(TXinShiMao_EmailHead* pEmailHeads, UINT8 nTitleNum);
extern UINT8 XinShiMao_GetNewMailCount(void);
CAS_BOOL XinShiMao_GetEmailHead(UINT8 bEmailID,TXinShiMao_EmailHead* pEmailHead);
CAS_BOOL XinShiMao_GetEmailContent(UINT8 bEmailID,TXinShiMao_EmailContent* pEmailContent);
CAS_BOOL XinShiMao_DelEmail(UINT8 bEmailID);
void	XinShiMao_UpStbId(char* p_stbid);


#ifdef __cplusplus
}
#endif


#endif


