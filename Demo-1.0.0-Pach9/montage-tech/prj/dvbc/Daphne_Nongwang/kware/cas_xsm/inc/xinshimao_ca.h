

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

#define   CA_TASK_PRIORITY   9// ����gui task ���� ECM parse �߳���ռCA�߳�
#define   SC_TASK_PRIORITY   7// Ҫ����CA_TASK_PRIORITY����Ϊ���ܻ��� card resum ����




#define CAS_SUCCESS 0
#define CAS_FAILTURE -1
#define CAS_TRUE 1
#define CAS_FALSE 0
#define CAS_NULL				(0)


#define CA_INFO_CLEAR 0//�����һ�� ���õ�ECM��EMM PID
#define CA_INFO_ADD    1 //�������� ECM��EMM PID

#define CA_OSD_ON_TOP   0
#define CA_OSD_ON_BOTTOM 1

//------------------------------------------------------------------------------------
#define CA_STB_FILTER_1 1 //emm ��Ȩ
#define CA_STB_FILTER_2 2 //emm osd email
#define CA_STB_FILTER_3 3// emm osd email
#define CA_STB_FILTER_4 4 //ecm 

#define ECM_FITLER_TIMEOUT (100)//()//��λ:���� ���� U8 �ṹ������ ҪС��250
#define EMM_FITLER_TIMEOUT (250)//()//��λ:���� ���� U8 �ṹ������ ҪС��250


#define     SC_NORMAL                     0  /*����״̬*/
#define     SC_NO_CARD   			1  /*δ�忨*/
#define     SC_NO_PPV_RIGHT  		2  /*û��PPV��Ȩ*/
#define     SC_NO_PPC_RIGHT  		3  /*û��PPC��Ȩ*/
#define     SC_PARENT_CTRL  		4 /*�ҳ�����*/
#define     SC_NO_CONDITION  		5 /*�����޲�*/
#define     SC_INVALID_CARD          6  /*��Ч��*/
#define	  SC_TYPEERROR			7  /*��ĸ��ι��ʧ��,�������ܿ����ʹ���*/
#define	  SC_NEEDFEED			8  /*�ӿ���Ҫ��ĸ����Ӧ,��ι���ӿ�*/
#define     SC_ZONEERROR               9  /*�������*/
#define     SC_FEEDTIMEERROR        10 /*ι��ʱ�����*/
#define     SC_FEEDDATEERROR        11 /*ι�����ڴ���*/
#define     SC_FEEDSYSTEMTIMEERROR  12 /*ϵͳʱ��û����ȷ��ȡ,ι��ʧ��*/
#define     SC_ACSEVICEIDEERROR  13 /*���Ż�AC ����*/
#define     SC_CARDSTBNOTPAIR  14 /*���Ż�AC ����*/
#define     SC_CARDENTITLEREMAINTDAY  15 /*��Ȩʣ������*/
#define     SC_UPDATAINT  16 /*���ܿ����ݸ��£�����ϵ�!*/




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
	UINT8 m_bServiceCount;//Ĭ����1
	UINT16 m_wServiceId[CA_MAX_SERVICE_PER_ECM];
}XinShiMao_CAServiceInfo;

//------------------------------------------------------------------------------------
typedef struct _CAOSDInfo{
	UINT8 m_bOSD_Position;//��ʾλ�� 0:top,1:bottom
	UINT16 m_wOSD_Show_Circle;//�Ƿ�ѭ����ʾ
	UINT8 m_bOSD_Text_length;
	UINT8 m_bOSD_Text[150];
}XinShiMao_CAOSD_Info;

typedef struct _TXinShiMao_DATETIME
{
	UINT16 Y;   //��
	UINT16 M;  //��
	UINT8   D;  //��
	UINT8   H;  //ʱ
	UINT8   MI; //��
	UINT8   S;  //��
}TXinShiMao_DATETIME;


#define   MAX_EMAIL_NUM            10
#define   EMAIL_TITLE_LEN          36
#define   EMAIL_CONTENT_LEN      128

typedef struct _TXinShiMao_EmailHead{
	TXinShiMao_DATETIME	m_tCreateTime;    
	UINT8    		m_bEmail_Level;    /*��Ҫ�̶�*/
	UINT8		m_bNewEmail;         /*0:���ʼ� 1:���ʼ�*/
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
//���ܿ��Ƿ�����Ȩ��־
extern INT32 XinShiMao_GetSMCEntitle(UINT8 * isEntitle);
//��ĸ����0:ĸ����1:�ӿ�
extern INT32 XinShiMao_GetSMCType(UINT8 * isType);
//0:������ԣ�1:����ԣ�2:���������
extern INT32 XinShiMao_GetSMCStbPaired(UINT8 * isCardStbPaired);
//��ȡ���ܿ�������ID��
extern UINT16 XinShiMao_GetCardRegionID( void );
//����NIT����ID��
extern void XinShiMao_SetNetWorkRegionID(UINT16 chRegionID);
//���û����е�ID�ţ�2�ֽڣ�����������ƣ�������ʼ���ÿ�����
extern void XinShiMao_SetStbRegionID(UINT16 chRegionID);

//��ȡ��Ȩ��ʣ�����������С��15�죬������ÿ����Сʱ�ڴ���������һ��
extern UINT8 XinShiMao_GetEntitleRemainDay(void);

/* --------------------------------------ca event-------------------------------*/
/*�� �� �ߣ� ������ 

��������� event Ϊ��Ϣ����1: ΪOSD��Ϣ��4Ϊ�ʼ���Ϣ
				2,3 �������Ϣ��ʾ:param1 Ϊ��Ϣ��ʾ������ */
extern void  XinShiMao_EventHandle(UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3);


/* --------------------------------------ca smart -------------------------------*/
extern INT32 XinShiMao_SC_DRV_Initialize(void);
//bCardNumber Ĭ��0
extern void  XinShiMao_SC_DRV_ResetCard(UINT8 bCardNumber);
/*�� �� �ߣ� �����С� 
��������� 
		bCardNumber�� ��ʱ�������ã� 
		bLength�� pabMessage�ĳ��ȣ� 
		pabMessage�� �����������Ϣָ�룻 
		pabResponse�� ������Ӧ��������ݿ��ָ�룻 
		bRLength�� ��Ӧ��������ݿ鳤��ָ�롣 
��������� 
		pabResponse�� ��Ӧ��������ݿ飻 
		bRLength�� ��Ӧ��������ݿ鳤�ȡ� 
		pbSW1 ���ܿ�״̬�ֽ�1 
		pbSW2 ���ܿ�״̬�ֽ�2 
�� �� ֵ�� 
		CAS_TRUE���ɹ�
		CAS_FALSE��ʧ�� */
extern INT32 XinShiMao_SC_DRV_SendDataEx(UINT8 bCardNumber,UINT8 bLength,
	                                                      UINT8 * pabMessage,
	                                                      UINT8 * pabResponse,
	                                                      UINT8 * bRLength,
	                                                      UINT8 * pbSW1,
	                                                      UINT8 * pbSW2);


/* --------------------------------------ca demux -------------------------------*/
/*�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Ŀ��ServiceID 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Ŀ��ServiceID*/
extern UINT16 XinShiMao_GetCurr_ServiceID(void);
/*�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Emm Pid 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Emm Pid*/
extern UINT16 XinShiMao_GetCurr_EmmID(void);
/*�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Ecm Pid 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Ecm Pid*/
extern UINT16 XinShiMao_GetCurr_EcmID(void);
//��ο�ʱ֪ͨCA�⣬
extern void  XinShiMao_SCStatusChange(UINT8 status);
/*�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� �յ�CA�����������ݺ� 
���������� �ж�CAϵͳID�Ƿ���XinShiMao�� 
��������� 
		wCASystemID��CAϵͳID�� 
��������� �ޡ� 
�� �� ֵ�� 
		CAS_TRUE���Ǳ�CA��
		CAS_FALSE�����Ǳ�CA��*/
extern CAS_BOOL  XinShiMao_IsMatchCAID(UINT16 wCASystemID);
/*�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� ������ECM PIDʱ(���л�Ƶ��)���øýӿڰ������Ϣ���ݸ�CAģ�顣 
���������� ��ECM��PID����Ϣ���ݸ�CAģ�顣 
��������� 
		bType�� ���õ����ͣ���ա����ӡ��޸ĵ�ǰECMPID�б� ʹ������ͬʱ���Դ����·��Ŀ��Ϊ���¼���ֵ֮һ:
		CAS_INFO_CLEAR���û�����CA���ECMPID, pEcmInfo ���ǿգ�
		CAS_INFO_ADD�� �û�����CA����һ��ECMPID�� 
 */
extern void  XinShiMao_SetEcmPID(UINT8 bType,XinShiMao_CAServiceInfo * pEcmInfo);
extern void  XinShiMao_SetEmmPID(UINT8 bType,UINT16 wEmmPid);
/*�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
���������� ����CA��˽�����ݵ�ʱ�򣬽�˽�����ݽ������������������ݴ��ݸ� CAģ�飬ʹ��CAģ����Դ�����Щ���ݡ� 
��������� 
		bOK�� ��ȡ������û�гɹ���TRUE���ɹ���FALSE��ʧ�ܡ� 
		wPid�� ���յ�����PID�� 
		pbReceiveData�� ��ȡ˽�����ݵ�ָ�룬CAģ�鲻������ռ���ͷš� 
		wLen�� ��ȡ����˽�����ݵĳ��ȡ� 

pbReceiveData[0]:0x80,0x81 ecm;0x82,0x83,0x84 emm */
extern void  XinShiMao_TableReceived(UINT8 bRequestID,UINT8 bOK,UINT16 wPid,const UINT8 * pbReceiveData,UINT16 wLen);

/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����������ȥ����CA�����ݡ� 
��������� 
		iRequestID     ,     Ϊ���¼���ֵ֮һ
						CA_STB_FILTER_1,
						CA_STB_FILTER_2
						CA_STB_FILTER_3
						CA_STB_FILTER_4
		pbFilterMatch1�� ����ǹ���Ecm���ݣ���ò���ΪEcm filter�����ݣ�
						 ����ǹ���Emm���ݣ���ò���ΪEmm filter1�����ݣ�
		pbFilterMask1��  ����ǹ���Ecm���ݣ���ò���ΪEcm filter��mask��
						 ����ǹ���Emm���ݣ���ò���ΪEmm filter1��mask��
		bLen�� filter�ĳ��ȡ� 
		wPid�� ͨ����PIDֵ�� 
		bWaitTime�� ͨ������ʱ�䣬��λ���룬��ʱʱ����궨�� ECM_FITLER_TIMEOUT, EMM_FITLER_TIMEOUT
	       if ( wPid <0 && wPid >0x1fff ) ��ʾ������DEMMUX��Ҫ�ͷŹ������ռ䣬EMM���Բ��ͷţ�ECM�����ͷ�
	       ECM��������TABILE_ID:0X80,0X81;emm ��Ȩ����TABLE id:0X82;osd,email table id:0x83
�� �� ֵ�� 
		SUCCESS���ɹ��� 
		FAILURE�� ʧ�ܡ� */
extern INT32 XinShiMao_TableStart(UINT8 iRequestID,
						  const UINT8 * pbFilterMatch1,
	                                     const UINT8 * pbFilterMask1,
	                                     UINT8 bLen,
	                                     UINT16 wPid,
	                                     UINT8 bWaitTime);
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ���ý���ͨ���Ŀ����֡� 
��������� 
		wEcmPid�� �����ֵ����ڵ�ecm����PIDֵ�� 
		szOddKey�� ����������ݵ�ָ�롣 
		szEvenKey�� ż���������ݵ�ָ�롣 
		bKeyLen�� �����ֳ��ȡ� 
		bReservedFlag, ������ 
*/
extern void XinShiMao_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag);


/* --------------------------------------ca flash -------------------------------*/
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ��û������ṩ��CAģ�鱣����Ϣ����ʼ��ַ�Ϳռ��С 
��������� 
��������� 
		lStartAddr �����з���ռ����ʼ��ַ 
		lSize �����з���ռ�Ĵ�С 
�� �� ֵ�� 
		TRUE �ɹ� 
		FALSE ʧ�� */
extern CAS_BOOL XinShiMao_GetBuffer(UINT32 *lStartAddr,UINT32 * lSize);
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� �ӻ����з���ռ��ָ����ʼ��ַ��ָ�����ȵ����ݡ� 
��������� 
		lStartAddr�� Ҫ��ȡ���ݵĴ洢�ռ�ĵ�ַ�� 
		pbData�� ����ȡ���ݵĴ�ŵ�ַָ�롣 
		nLen�� Ҫ�������ݵĳ��� 
��������� 
		pbData�� �����������ݡ� 
�� �� ֵ�� 
		����ʵ�ʶ������ֽ���*/
extern UINT32 XinShiMao_ReadBuffer(const UINT8 *lStartAddr,UINT8 *pData,INT32 nLen);

/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ������еĴ洢�ռ�д��Ϣ 
��������� 
		lStartAddr�� Ҫд�Ĵ洢�ռ��Ŀ���ַ�� 
		pData�� Ҫд������ 
		nLen�� Ҫд�����ݵĳ��� 
��������� �ޡ� 
�� �� ֵ�� 
		TRUE���ɹ�
		FALSE�� ʧ�ܡ�*/
extern CAS_BOOL XinShiMao_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen);

/* --------------------------------------ca os -------------------------------*/

/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� �������� 
��������� 
		name[]�� 4���ֽڵ��������ơ� 
		stackSize�� ������ʹ�õĶ�ջ�Ĵ�С�� 
		entryPoint�� �������ڵ�ַ�� 
		priority�� ��������ȼ��� 
		arg1�� ���ݸ�����ĵ�һ�������� 
		arg2�� ���ݸ�����ĵڶ��������� taskId�� �����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������*/
extern INT32 XinShiMao_OSPTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void*),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
extern INT32 XinShiMao_OSPTaskTemporarySleep(UINT32 milliSecsToWait);
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����һ����Ϣ���С� 
��������� 
		name[]�� 4���ֽڵĶ������ơ� 
		maxQueueLength����Ϣ�����п��Դ�ŵ���Ϣ������������Ϣ�����и������ﵽ ������ʱ����������Ϣ���з���Ϣ����ʧ�ܡ� 
		taskWaitMode:���Բ���
		queueId�� ��Ϣ���е�ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������*/
extern INT32 XinShiMao_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId);
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� 
��������� 
		queueId�� ��Ҫȡ�õ���Ϣ��ID�� 
		messsage�� Ϊ��Ϣ�ĸ�ʽ������XINSHIMAO_QUEUE_MESSAGE�� 
		waitMode:Ŀǰֻ�õ����ȴ�ֱ���õ���Ϣ 

		SUCCESS���ɹ���
		FAILURE�� ��������*/
extern INT32 XinShiMao_OSPQueueGetMessage(UINT32 queueId,XinShiMao_Queue_message * message,UINT32 waitMode,UINT32 milliSecsToWait);
/*�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ������Ϣ����Ϣ���С� 
��������� 
		queueId�� ��Ϣ����ID�� 
		message�� Ҫ���͵���Ϣ�����ʽ��XINSHIMAO_QUEUE_MESSAGE�ṹ�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������*/
extern INT32 XinShiMao_OSPQueueSendMessage(UINT32 queueId,XinShiMao_Queue_message * message);
extern UINT32 XinShiMao_Printf(const char * fmt,...);
extern void XinShiMao_memcpy(void* pDestBuf,const void* pSrcBuf,UINT32  wSize);
extern void XinShiMao_memset(void* pDestBuf,UINT8 c,UINT32 wSize);
extern INT32  XinShiMao_memcmp(const void *buf1, const void *buf2, UINT32 count);

UINT32   XinShiMao_OS_CreateSemaphore(UINT32 ulInitialCount);  /*�����ź���*/
void  XinShiMao_OS_WaitSemaphore(UINT32 ulSemaphore);       /*�ȴ��ź���*/
void  XinShiMao_OS_SignalSemaphore(UINT32 ulSemaphore);       /*�ͷ��ź���*/


//���û�����ʱ�䣬�������������TDT����CA���ṩʱ���׼
extern void	XinShiMao_SetStbTime(TXinShiMao_DATETIME* ca_time);


/* --------------------------------------mother son card  �ù��ܿ�����ʱ����ֲ-------------------------------*/
//ret: 0:read mother card data right 1:invailed card 2:mother card and son card is not paired
extern UINT8 XinShiMao_ReadFeedDataFromParent(UINT8 * chSpbyFeedData, UINT8 *chLen);
extern UINT8 XinShiMao_WriteFeedDataToChild(UINT8 * chSpbyFeedData, UINT8 chLen);
/* --------------------------------------email �ù��ܿ�����ʱ����ֲ-------------------------------*/
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


