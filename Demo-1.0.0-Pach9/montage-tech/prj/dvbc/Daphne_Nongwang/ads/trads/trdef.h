/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#ifndef __TF_DEF_H__
#define __TF_DEF_H__


typedef signed int		MTRDRV_SINT;
typedef unsigned int	MTRDRV_UINT;
typedef signed long		MTRDRV_S32;
typedef unsigned long	MTRDRV_U32;
typedef signed short	MTRDRV_S16;
typedef unsigned short	MTRDRV_U16;
typedef float			MTRDRV_FLOAT;
typedef double 			MTRDRV_DOUBLE;
typedef long double		MTRDRV_LONGDOUBLE;
typedef char			MTRDRV_CHAR;
typedef signed char		MTRDRV_S8;
typedef unsigned char	MTRDRV_U8;
typedef void			MTRDRV_VOID;
typedef unsigned long	MTRDRV_HANDLE;
typedef unsigned long   MTRDRV_FERR;


typedef MTRDRV_U32	MTR_DRV_OS_THREADID;
typedef MTRDRV_U32  MTR_DRV_OS_QUEUEID;
typedef MTRDRV_U32	MTR_DRV_OS_SEMPHORE;
typedef MTRDRV_VOID (*MTR_OD_ThreadEntryFunc)(MTRDRV_VOID* arg);
typedef MTRDRV_VOID* PMTR_DRV_OS_VOID;
typedef MTRDRV_VOID MTR_OS_VOID;
typedef MTRDRV_U32 MTR_DRV_OS_PRIORITY;
typedef MTRDRV_U32 MTR_DRV_OS_U32;
typedef MTRDRV_U16 MTR_OS_U16;
typedef MTRDRV_U8 MTR_OS_U8;
typedef MTRDRV_S32 MTR_DRV_OS_STATUS;
typedef struct
{
	MTRDRV_U16	 Type;
    	MTRDRV_U8        Length;
	MTRDRV_U8       DataBuffer[10];
}MTR_OS_Msg;

typedef enum
{
	MTRDRV_FALSE = 0,
	MTRDRV_TRUE
}MTRDRV_BOOL;


typedef enum
{
/*01*/LOGO_IFRAME = 1,       //��������
/*02*/MIANMENU_UP,           //���˵���
/*03*/MIANMENU_DOWN,   //���˵���
/*04*/EPG_GIF,                          //EPG��Ŀָ���б�
/*05*/NVOD_GIF,                      //nvod��Ƶ�㲥�б�
/*06*/OTHERMENU_GIF,        //�����˵�
/*07*/BANNER_GIF,                 //���Ĺ�棬���ж��
/*08*/RADIO_PIC,           //�㲥���ֲ���棬���ж��
/*09*/CHANGE_CHANNEL,    //��̨��ʾһ���Ź�棬һ��Ƶ���Ӧһ��
/*10*/VOLUME_GIF,                //������
/*11*/MANAGE_CHANNEL,   //Ƶ������
/*12*/BROADCAST_LIST,        //���ݹ㲥�б�
/*13*/CA_INFORMATION,      //Ca��ʾ��Ϣ
/*14*/SORTCHANNEL_LIST,   //ϲ��/����/�㲥��Ŀ�б�
/*15*/URGENCY_NOTICE,      //����֪ͨ
/*16*/MAINMENU_MIDDLE, //���˵�����
/*17*/HANGAD_GIF,                //�ҽǹ��
/*18*/MENUAD_GIFONE,       //�˵����һ
/*19*/MENUAD_GIFTWO,      //�˵�����
/*20*/MENUAD_GIFTHREE,   //�˵������
/*21*/MENUAD_GIFFOUR,     //�˵������
	    UNDEFINED,
}MTR_DRV_AdType;


typedef struct
{
	MTRDRV_U8  Type;//������ͼ��������š�2��3....
	MTRDRV_S16 X_x;//��ͼ��ʼ������
	MTRDRV_S16 Y_y;//��ͼ��ʼ������
}Lib_GIF_Position;


typedef  struct
{
	MTRDRV_S16 X_x;
	MTRDRV_S16 Y_y;

}MTR_DRV_OSD_DOTCOORDINATE;			//һ���������


typedef enum{
	MTR_DRV_OS_NOT_WAIT,
	MTR_DRV_OS_WAIT,
}MTR_DRV_OS_WAITMODE;

typedef enum
{
	MTR_DRV_TIMER_OK,
	MTR_DRV_TIMER_ERROR,
	MTR_DRV_TIMER_CREATE_ERROR,
	MTR_DRV_TIMER_FREE_ERROR,
	MTR_DRV_TIMER_TIME_VALUE_ERROR,
	MTR_DRV_TIMER_TIMER_MODE_ERROR,
}MTR_DRV_TIMER_STATUS;//status of return value

typedef enum
{
	MTR_DRV_TIMER_ONE_SHOT,		//һ���Զ�ʱ��
	MTR_DRV_TIMER_REPEAT,		//�ظ��Զ�ʱ��
}MTR_DRV_TIMER_MODE;//Operation mode of timer

//���Ҫ����״ֵ̬��һ��ҪΪ��������Ϊ�ڳ�������ʱ�������������ʾĳЩ��ȷ��ֵ
#define MTR_DRV_OS_OK		0
#define MTR_DRV_OS_ERR		-1
#define MTR_DRV_OS_TIMEOUT	-2
/***********************************����������״̬����********************/
typedef enum
{
	MTR_DRV_DMX_STOP_CHANNEL,
	MTR_DRV_DMX_START_CHANNEL,
	MTR_DRV_DMX_RESET_CHANNEL,
} MTR_DRV_DMX_ChannelAction_t;


extern MTRDRV_VOID Init_AD_Task(MTRDRV_VOID);
extern MTRDRV_VOID Init_MTR_AD_Data(MTRDRV_VOID);
extern MTRDRV_VOID MTR_Start_SearchAD(MTRDRV_VOID);
extern MTRDRV_VOID MTR_AD_ShowLogo(MTRDRV_U8 logo);
extern MTRDRV_BOOL MTR_Show_Gif_AD(Lib_GIF_Position adpos,MTRDRV_U32 Ad_Banner_Index,MTRDRV_U8 GIF_dataIN);
//extern MTRDRV_BOOL MTR_AD_have_this_gif_picture(Lib_GIF_Position adpos,MTRDRV_U32 Ad_Banner_Index,MTRDRV_U8 GIF_dataIN);
extern MTRDRV_VOID Clean_TRAD(MTRDRV_VOID);


/******************************************************************************

��������	MTR_AD_Creat_Drv_Task

����������	����һ���������ݵ�����
(�������Բ���д����ӿں�����д�����Ŀ������������ʱ���и����������˳�򣬼��ɷ�Ҳ����ֱ�Ӱ��������д�գ����Լ��������д����������)

��ڲ�����	��

����ֵ��	�ޣ�

******************************************************************************/
MTRDRV_VOID	MTR_AD_Creat_Drv_Task(MTRDRV_VOID);


/******************************************************************************

��������	MTR_DRV_TIMER_CreateTimer

����������	����һ����ʱ���������Ч��handleֵ

��ڲ�����	handle		--	��������ÿһ����ʱ��

����ֵ��	0���ɹ���

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_CreateTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

��������	MTR_DRV_TIMER_FreeTimer

����������	�ͷ�һ����ʱ��

��ڲ�����	handle		--	��������ÿһ����ʱ��

����ֵ��	0���ɹ���

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_FreeTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

��������	MTR_DRV_TIMER_SetTimer

����������	����һ����ʱ��

��ڲ�����	handle		--	��������ÿһ����ʱ��
			timeValue	--	��ʱ���趨��ʱ��ֵ����λ�루s����
							��0��������ʱ����0���ͷŶ�ʱ����
			TimerMode	--	��ʱ��������ģʽ��TIMER_REPEAT���ظ��ͣ�TIMER_ONE_SHOT��һ���Եģ�
			CallBackFunc--	��ʱ���������õĻص�����

����ֵ��	0���ɹ���

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_SetTimer(MTRDRV_HANDLE* handle, MTRDRV_U32 TimeValue, MTRDRV_U32 TimerMode, void (*CallBackFunc)());

/******************************************************************************

��������	MTR_DRV_TIMER_StartTimer

����������	����һ����ʱ��

��ڲ�����	handle		--	��������ÿһ����ʱ��

����ֵ��	0���ɹ���

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_StartTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

��������	MTR_DRV_TIMER_StopTimer

����������	ֹͣһ����ʱ���ļ�ʱ������û���ͷŶ�ʱ��

��ڲ�����	handle		--	��������ÿһ����ʱ��

����ֵ��	0���ɹ���

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_StopTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

��������	MTR_DRV_API_printf

����������	�����ӡ���

��ڲ�����	*format	-- ����ִ�

����ֵ��	MTRDRV_OK	-- ��ӡ�ɹ�
			MTRDRV_ERROR	-- ��ӡʧ��

******************************************************************************/
MTRDRV_SINT MTR_DRV_API_printf(MTRDRV_CHAR *format, ...);

/******************************************************************************

��������	MTR_DRV_TUNER_GetLK

����������	��ȡ��ǰTuner����״̬�����Ĵ���

��ڲ�����	��

����ֵ��	1			-- ����
			0			-- ʧ��

******************************************************************************/
MTRDRV_U8 MTR_DRV_TUNER_GetLK (void);

/******************************************************************************

��������	MTR_DRV_TUNER_SetParams

����������	Tuner��Ҫ������

��ڲ�����	MTRDRV_U32 _chanFrequency		-- ����Ƶ��(10KHz)
			MTRDRV_U32 _chanSymbolRate	-- ���������(Ks/s)
			MTRDRV_U8 _Qammode		-- QAM��ʽ�򼫻������DVBT����(0:16QAM,1:32QAM,2:64QAM,3:128QAM,4:256QAM)

����ֵ��	1	-- �����˳�
			0	-- δ�����˳�

******************************************************************************/
MTRDRV_S8 MTR_DRV_TUNER_SetParams (MTRDRV_U32 _chanFrequency,MTRDRV_U32 _chanSymbolRate,MTRDRV_U8 _Qammode);

/******************************************************************************

��������	MTR_DRV_TUNER_GetMainFreqParams

����������	�����Ƶ�����

��ڲ�����	MTRDRV_U32 _chanFrequency		-- ����Ƶ��(10KHz)
			MTRDRV_U32 _chanSymbolRate	-- ���������(Ks/s)
			MTRDRV_U8 _Qammode		-- QAM��ʽ�򼫻������DVBT����(0:16QAM,1:32QAM,2:64QAM,3:128QAM,4:256QAM)

����ֵ��	��

******************************************************************************/
MTRDRV_VOID MTR_DRV_TUNER_GetMainFreqParams(MTRDRV_U32* _chanFrequency,MTRDRV_U32* _chanSymbolRate,MTRDRV_U8* _Qammode);

/******************************************************************************

��������	MTR_DRV_OS_TaskCreate

����������	��������������������

��ڲ�����	func		-- ������ں���ָ��
			arg			-- ���ݸ�������ں����Ĳ���
			dPriority	-- ��������ȼ�
			stack		-- ����Ķ�ջ��ʼ��ַ
			stack_size	-- �����ջ�Ĵ�С

����ֵ��	�����ID��0Ϊ��������ʧ��

******************************************************************************/
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskCreate(MTR_OD_ThreadEntryFunc func, PMTR_DRV_OS_VOID arg,MTR_DRV_OS_PRIORITY priority,
								  PMTR_DRV_OS_VOID stack, MTR_DRV_OS_U32 stack_size);

/******************************************************************************

��������	MTR_DRV_OS_TaskDelete(Ŀǰδ�õ�,Ԥ��)

����������	ɾ��һ������

��ڲ�����	taskid	-- ����ID

����ֵ��	MTR_DRV_OS_OK	-- ɾ������ɹ�
			MTR_DRV_OS_ERR	-- ɾ������ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskDelete(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

��������	MTR_DRV_OS_TaskGetCurTaskPriority

����������	��ȡ��ǰ�����ID

��ڲ�����	��

����ֵ��	���ô˺����������ID��0Ϊ��ȡ����IDʧ��

******************************************************************************/
MTR_DRV_OS_PRIORITY MTR_DRV_OS_TaskGetCurTaskPriority(MTRDRV_VOID);

/******************************************************************************

��������	MTR_DRV_OS_TaskSetPriority

����������	������������ȼ�

��ڲ�����	taskid		-- ����ID
			priority	-- Ҫ���õ����ȼ�

����ֵ��	MTR_DRV_OS_OK	-- ������������ȼ��ɹ�
			MTR_DRV_OS_ERR	-- ������������ȼ�ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid, MTR_DRV_OS_PRIORITY priority);

/******************************************************************************

��������	MTR_DRV_OS_TaskGetCurTaskId(Ŀǰδ�õ�,Ԥ��)

����������	��ȡ��ǰ�����ID

��ڲ�����	��

����ֵ��	���ô˺����������ID��0Ϊ��ȡ����IDʧ��

******************************************************************************/
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskGetCurTaskId(MTRDRV_VOID);

/******************************************************************************

��������	MTR_DRV_OS_TaskSuspend(Ŀǰδ�õ�,Ԥ��)

����������	���������(��ͣ���������),�ú�����Ҫ��MTR_OS_TaskResume�����ɶ�ִ��

��ڲ�����	taskid	-- ����ID,��Ϊ0ʱ�������ñ���������������.

����ֵ��	MTR_DRV_OS_OK	-- ��������ɹ�

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

��������	MTR_DRV_OS_TaskResume(Ŀǰδ�õ�,Ԥ��)

����������	�ָ�һ������������������,�ú�����Ҫ��MTR_OS_TaskSuspend�����ɶ�ִ��

��ڲ�����	taskid	-- ����ID

����ֵ��	MTR_DRV_OS_OK	-- ��������ɹ�
			MTR_DRV_OS_ERR	-- ��������ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskResume(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

��������	MTR_DRV_OS_TaskSleep

����������	�õ�ǰ����˯��һ��ʱ��

��ڲ�����	dMilliSeconds	-- ����˯�ߵ�ʱ�䣨ms��

����ֵ��	MTR_DRV_OS_OK	-- ��������ɹ�
			MTR_DRV_OS_ERR	-- ��������ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds);

/******************************************************************************

��������	MTR_DRV_OS_SemaphoreCreate

����������	����һ���ź���

��ڲ�����	semphore_init_no	-- ָ�����ź����ĳ�ʼ���źŸ���,����ø��ź���������ĳ��Դ,�ø��������
						�����ж��ٸ�����ͬʱ���ʸ���Դ

����ֵ��	semphore	-- �������ź�����ʶ��0��ʾ����ʧ��

******************************************************************************/
MTR_DRV_OS_SEMPHORE MTR_DRV_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no);

/******************************************************************************

��������	MTR_DRV_OS_SemaphoreDelete(Ŀǰδ�õ�,Ԥ��)

����������	ɾ��һ���ź���

��ڲ�����	semphoreid	-- �ź���ID

����ֵ��	MTR_DRV_OS_OK	-- ɾ��һ���ź����ɹ�
			MTR_DRV_OS_ERR	-- ɾ��һ���ź���ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid);

/******************************************************************************

��������	MTR_DRV_OS_SemaphoreGet

����������	��ָ������Ϣ���л�ȡһ����Ϣ

��ڲ�����	semphoreid	-- �ź�����ID
			wait_ornot	-- ָ���ڻ�ȡ�ź���ʧ��ʱ�Ƿ�ȴ�
			wait_time	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�ȴ���ʱ��(ms),�ò�����wait_ornot����
						MTR_OS_WAITʱ��������,��Ϊ0,���ʾһֱ�ȴ�

����ֵ��	MTR_DRV_OS_ERR		-- ��ȡ�ź���ʧ��
			MTR_DRV_OS_TIMEOUT	-- ��ȡ�ź�����ʱ
			MTR_DRV_OS_OK(>=0)	-- ��ȡ�ź�������ź�����ʣ����ź�������

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/******************************************************************************

��������	MTR_DRV_OS_SemaphoreRelease

����������	�ͷ�һ���ź���

��ڲ�����	semphoreid	-- �ź�����ID

����ֵ��	MTR_DRV_OS_OK	-- �ͷ��ź����ɹ�
			MTR_DRV_OS_ERR	-- �ͷ��ź���ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreRelease(MTR_DRV_OS_SEMPHORE semphoreid);


/******************************************************************************

��������	MTR_DRV_OS_QueueCreate

����������	����һ����Ϣ����

��ڲ�����	queue_element--�ö��п������ɵ������Ϣ����

����ֵ��	���е�ID�ţ�0Ϊ��������ʧ�ܡ�

******************************************************************************/
MTR_DRV_OS_QUEUEID MTR_DRV_OS_QueueCreate(MTR_DRV_OS_U32 queue_element);

/******************************************************************************

��������	MTR_DRV_OS_QueueDelete(Ŀǰδ�õ�,Ԥ��)

����������	ɾ��һ����Ϣ����

��ڲ�����	queueid	-- ��Ϣ���е�ID

����ֵ��	MTR_DRV_OS_OK	-- ɾ�����гɹ�
			MTR_DRV_OS_ERR	-- ɾ������ʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid);

/******************************************************************************

��������	MTR_DRV_OS_QueueSendMessage

����������	��ָ������Ϣ���з���һ����Ϣ

��ڲ�����	queueid		-- ��Ϣ���е�ID
			pmessage	-- ָ��Ҫ���͵���Ϣ��ָ��

����ֵ��	MTR_DRV_OS_OK	-- �������﷢����Ϣ�ɹ�
			MTR_DRV_OS_ERR	-- �������﷢����Ϣʧ��

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueSendMessage(MTR_DRV_OS_QUEUEID queueid,MTR_OS_Msg *pmessage);

/******************************************************************************

Name:        MTR_DRV_OS_QueueReceiveMessage

Description: ��ָ������Ϣ���л�ȡһ����Ϣ

Parameters:  queueid	-- ��Ϣ���е�ID
			 wait_ornot	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�Ƿ�ȴ�
			 wait_time	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�ȴ���ʱ��(ms),�ò�����wait_ornot����
						MTR_OS_WAITʱ��������,��Ϊ0,���ʾһֱ�ȴ�

Returns:     pmessage	-- ��ȡ����Ϣָ��
			 0			-- ��ȡ��Ϣʧ��

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_QueueReceiveMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/******************************************************************************

��������	MTR_DRV_OS_MemoryAllocate

����������	�����ڴ�

��ڲ�����	size	-- Ҫ������ڴ�Ĵ�С(BYTE).�ú���Ҫ��MTRAPI_OS_MemoryFree�ɶ�ʹ��

����ֵ��	������ڴ��ָ��,NULLΪ����ʧ��

******************************************************************************/
MTR_OS_VOID *MTR_DRV_OS_MemoryAllocate(MTR_DRV_OS_U32 size);

/******************************************************************************

��������	MTR_DRV_OS_MemoryFree

����������	�ͷ��ڴ�

��ڲ�����	paddress	-- Ҫ�ͷŵ��ڴ��ָ��.�ú���Ҫ��MTRAPI_OS_MemoryAllocate�ɶ�ʹ��

����ֵ��	MTR_DRV_OS_OK	-- �ͷ��ڴ�ɹ�

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_MemoryFree(MTR_OS_VOID *paddress);

/******************************************************************************

��������	MTR_DRV_OS_MemorySet

����������	����ָ���ĵ�ַ������

��ڲ�����	mem		-- ��ַָ��
			value	-- ���õ�ֵ
			bytes	-- ���õ����ݳ���

����ֵ��	���õĵ�ַָ��

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_MemorySet( PMTR_DRV_OS_VOID mem, MTR_OS_U8 value, MTR_DRV_OS_U32 bytes );

/******************************************************************************

��������	MTR_DRV_OS_MemoryCopy

����������	��Դ��ַ�����ݿ�����Ŀ���ַ��

��ڲ�����	pdst	-- Ŀ���ַָ��
			psrc	-- Դ��ַָ��
			bytes	-- ���������ݳ���

����ֵ��	Ŀ���ַ�ĵ�ַָ��

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_MemoryCopy( PMTR_DRV_OS_VOID pdst, PMTR_DRV_OS_VOID psrc, MTR_DRV_OS_U32 bytes );

/***************************************************************************
��������	MTR_DRV_DMX_AllocateSectionChannel
����������	����channel
��ڲ�����	UINT32 nFilterNumber -- ��channel���ɷ���filter����
			UINT32 nChannelBufferSize -channel�Ļ�������С
����ֵ��	���䵽��channel ID
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_AllocateSectionChannel(MTRDRV_U32 nFilterNumber,MTRDRV_U32 nChannelBufferSize);

/***************************************************************************
��������	MTR_DRV_DMX_FreeChannel
����������	�ͷ�channel
��ڲ�����	UINT32 channelID -- ��channel ID;
����ֵ��	��
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_FreeChannel(MTRDRV_U32 nChannelId);

/***************************************************************************
��������	MTR_DRV_DMX_AllocateFilter
����������	����filter
��ڲ�����	UINT32 channelID -- channel ID
			MTRDRV_U32 nFilterSize -- filter�Ĵ�С
����ֵ��	���䵽��filter ID
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_AllocateFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterSize);

/***************************************************************************
��������	MTR_DRV_DMX_FreeFilter
����������	�ͷŹ�����
��ڲ�����	UINT32 channelID -- ��ͨ��ID;
			UINT32 filterID -- ������ID;
����ֵ��	��
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_FreeFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId);

/***************************************************************************
��������	MTR_DRV_DMX_SetChannelPid
����������	����ͨ�������˵�PID
��ڲ�����	UINT32 channelID -- ��ͨ��ID;
			UINT32 uiPid -- ���˵�PID
����ֵ��	��
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_SetChannelPid(MTRDRV_U32 nChannelId, MTRDRV_U32 nPid);

/***************************************************************************
��������	MTR_DRV_DMX_ControlChannel
����������	����ͨ������״̬
��ڲ�����	UINT32 channelID -- ��ͨ��ID;
			UINT32 channelAction -- ͨ������״̬
����ֵ��	��
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_ControlChannel(MTRDRV_U32 nChannelId,MTR_DRV_DMX_ChannelAction_t channelAction);

/***************************************************************************
��������	MTR_DRV_DMX_SetFilter
����������	���ù������Ĺ�������
��ڲ�����	UINT32 channelID -- ��ͨ��ID;
			UINT32 filterID -- ������ID;
			MTRDRV_U8 Table_id -- table_id
			MTRDRV_U8 Table_idMask -- table_id����
			PINT8U puiMask -- ��������������
			PINT8U puiMatch -- ����������ֵ
����ֵ��	0���ɹ���
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_SetFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId,MTRDRV_U8 Table_id,MTRDRV_U8 Table_idMask,
								  MTRDRV_U8* puiMatch ,MTRDRV_U8 *puiMask, MTRDRV_U8 DataLen);

/***************************************************************************
��������	MTR_DRV_DMX_RegisterChannelCallback
����������	ע��ص�����
��ڲ�����	UINT32 channelID -- ��ͨ��ID;
			MTRDRV_VOID (* pfnCallBack) -- �ص�����ָ��
			(�ص������ĵڶ������������ݵ�ַ�����������������ݳ���)
����ֵ��	��
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_RegisterChannelCallback(MTRDRV_U32 nChannelId, MTRDRV_VOID(* pfnCallBack)(MTRDRV_U32 nChannelId,MTRDRV_U8 *data,MTRDRV_U32 len));

/***************************************************************************
��������	MTR_DRV_NVRAM_GetFlashAllocateSize
����������	��ù�����ݴ洢����С
��ڲ�����	��
����ֵ��	������ݴ洢����С
******************************************************************************/
MTRDRV_U32 MTR_DRV_NVRAM_GetFlashAllocateSize(MTRDRV_VOID);

/***************************************************************************
��������	MTR_DRV_NVRAM_ReadData
����������	��ȡflash����
��ڲ�����	MTRDRV_U32 nvrOffset -- Nvram��ƫ����word��
			MTRDRV_U16 *extBuffer -- �ⲿ�洢�������ڴ洢��ȡ������
			MTRDRV_U32 size -- ��ȡ���ݵĳ���
����ֵ��	��
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_ReadData(MTRDRV_U32 nvrOffset, MTRDRV_U8 *extBuffer, MTRDRV_U32 size);

/***************************************************************************
��������	MTR_DRV_NVRAM_WriteData
����������	�洢���ݵ�flash
��ڲ�����	MTRDRV_U32 nvrOffset -- Nvram��ƫ����word��
			MTRDRV_U16 *extBuffer -- �ⲿ�洢�����洢������
			 MTRDRV_U32 size -- �洢���ݵĳ���
����ֵ��	��
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_WriteData(MTRDRV_U32 nvrOffset, MTRDRV_U8 * extBuffer, MTRDRV_U32 size);

/***************************************************************************
��������	MTR_DRV_NVRAM_Erase
����������	�����洢������ݵ�flash��
��ڲ�����	���ͼƬ�ĳ���
����ֵ��	��
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_Erase(MTRDRV_U32 AD_length);

/***************************************************************************
��������	MTR_AD_SearchOver_Process
����������	������������ĺ����������Իָ������Ŀ�������
��ڲ�����	��
����ֵ��	��
******************************************************************************/
MTRDRV_VOID MTR_AD_SearchOver_Process(MTRDRV_VOID);

/******************************************************************************

��������	TRADReadRectangle

����������	���øú�������Ļ��ָ����������ݶ���ָ���Ļ�����

��ڲ�����	x	-- ָ������ĺ����꣬y	--ָ������������� width	--ָ������Ŀ��
			height	--ָ������ĸ߶� destination��Ŷ������ݵĻ���

����ֵ��	��

******************************************************************************/
MTRDRV_VOID TRADReadRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *destination);

/******************************************************************************

��������	TRADWriteRectangle

����������	���øú���������д����Ļָ������

��ڲ�����	x	-- ָ������ĺ����꣬y	--ָ������������� width	--ָ������Ŀ��
			height	--ָ������ĸ߶� sourceҪд��ָ�����������

����ֵ��	��

******************************************************************************/
MTRDRV_VOID TRADWriteRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *source);

/******************************************************************************

��������	TRAD_Display_Logo

����������	���øú�����ʾ����ͼƬ

��ڲ�����	source	--�����������ݵ�ָ��(���ڵĿ������治������I֡�����ˣ����п�����gif���ݻ���bm����)��length--���ݵĳ���

����ֵ��	��

******************************************************************************/
MTRDRV_VOID TRAD_Display_Logo(MTRDRV_U8 *source,MTRDRV_U32 length);

/******************************************************************************

��������	MTR_AD_Show_Gif

����������	���øú�����ʾgifͼƬ

��ڲ�����	GifCoordinate	--���ͼƬ������
			GifData-----------gifͼƬ������
			gif_length-----------gifͼƬ�ĳ���
			gif_idx��refresh����ֱ�Ӵ�0

����ֵ��	��

******************************************************************************/
MTRDRV_BOOL MTR_AD_Show_Gif(MTR_DRV_OSD_DOTCOORDINATE GifCoordinate, MTRDRV_U8* GifData , MTRDRV_HANDLE gif_length, MTRDRV_U8 gif_idx,MTRDRV_U8 refresh);
#endif
