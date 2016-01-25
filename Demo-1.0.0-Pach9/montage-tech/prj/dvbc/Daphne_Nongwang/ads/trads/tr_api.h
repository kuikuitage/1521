/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

	//ָ��Ĭ��ֵ
#ifndef MTR_DRV_NULL
    #define MTR_DRV_NULL 0
#endif

//
#define MTR_DRV_OS_OK		 0
#define MTR_DRV_OS_ERR		-1
#define MTR_DRV_OS_TIMEOUT	-2


//�������Ͷ���
typedef signed int		MTRDRV_SINT;
typedef unsigned int	MTRDRV_UINT;
typedef signed long 	MTRDRV_S32;
typedef unsigned long	MTRDRV_U32;
typedef signed short	MTRDRV_S16;
typedef unsigned short	MTRDRV_U16;
typedef signed char 	MTRDRV_S8;
typedef unsigned char	MTRDRV_U8;
typedef char			MTRDRV_CHAR;
typedef void			MTRDRV_VOID;
typedef unsigned long	MTRDRV_HANDLE;
typedef float			MTRDRV_FLOAT;
typedef double			MTRDRV_DOUBLE;
typedef long double 	MTRDRV_LONGDOUBLE;
typedef void			MTRDRV_FUN;
typedef int 			MTRDRV_INT;
typedef void*			MTRDRV_PHANDLE;

typedef MTRDRV_U32	MTR_DRV_OS_THREADID;
typedef MTRDRV_U32  MTR_DRV_OS_QUEUEID;
typedef MTRDRV_U32	MTR_DRV_OS_SEMPHORE;
typedef MTRDRV_VOID (*MTR_DRV_OD_ThreadEntryFunc)(MTRDRV_VOID* arg);
typedef MTRDRV_VOID* PMTR_DRV_OS_VOID;
typedef MTRDRV_VOID MTR_DRV_OS_VOID;
typedef MTRDRV_U32 MTR_DRV_OS_PRIORITY;
typedef MTRDRV_U32 MTR_DRV_OS_U32;
typedef MTRDRV_U16 MTR_DRV_OS_U16;
typedef MTRDRV_U8 MTR_DRV_OS_U8;
typedef MTRDRV_S32 MTR_DRV_OS_STATUS;
typedef struct
{
	MTRDRV_U16		 Type;
    MTRDRV_U8        Length;
	MTRDRV_U8        DataBuffer[16];
}MTR_DRV_OS_Msg;

typedef enum{
	MTR_DRV_OS_NOT_WAIT,
	MTR_DRV_OS_WAIT

}MTR_DRV_OS_WAITMODE;


typedef enum
{
	MTRDRV_FALSE=0,
	MTRDRV_TRUE=1
}MTRDRV_BOOL;

//״ֵ̬
typedef enum
{
	MTRDRV_OK = 0,
	MTRDRV_ERROR,
	MTRDRV_BAD_PARAMETER,
	MTRDRV_OUTOF_MEMORY,
	MTRDRV_TIMEOUT,
	MTRDRV_INVALID_HANDLE
}MTRDRV_STATUS;

enum
{
	MTRDRV_SUCCESS,
	MTRDRV_FAILURE
};

typedef  struct
{
	MTRDRV_S16 X_x;		//x����
	MTRDRV_S16 Y_y;		//y����
	MTRDRV_S16 W_w;		//wide
	MTRDRV_S16 H_h;		//height
}MTRDRV_OSD_RECTCOORDINATE;	//�续����ʱ�õ����Ͻǵĵ�����꼰x�ϵĿ��,y�ϵĸ߶�

enum
{
	MTR_DRV_TIMER_ONE_SHOT, 	//һ���Զ�ʱ��
	MTR_DRV_TIMER_REPEAT		//�ظ��Զ�ʱ��
};//��ʱ��������ģʽ

typedef enum
{
	MTR_DRV_DMX_STOP_CHANNEL,
	MTR_DRV_DMX_START_CHANNEL,
	MTR_DRV_DMX_RESET_CHANNEL
} MTR_DRV_DMX_ChannelAction_t;

/*!
  ������     : MTR_TITLE_Creat_ReceiveData_Task
  ��������: ����������Ļ���ݵ�����
  ��ڲ���: ��
  ����ֵ      : ��
  ˵��            : ��
  */
MTRDRV_VOID MTR_TITLE_Creat_ReceiveData_Task(MTRDRV_VOID);

/*!
  ������     : MTR_DRV_TITLE_DMX_AllocateSectionChannel
  ��������: ����channel
  ��ڲ���: nFilterNumber: ��channel�����Է���filter����
                           nChannelBufferSize : channel�Ļ����С
  ����ֵ      : ���䵽��channel_id
  ˵��            : ����ֵ����Ϊ0
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateSectionChannel(MTRDRV_U8 nFilterNumber,MTRDRV_U32 nChannelBufferSize);

/*!
  ������     : MTR_DRV_TITLE_DMX_FreeChannel
  ��������: �ͷ�channel
  ��ڲ���: channelID
  ����ֵ      : 0 :�ɹ�
  ˵��            : ��
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeChannel(MTRDRV_U32 channelID);

/*!
  ������     : MTR_DRV_TITLE_DMX_AllocateFilter
  ��������: ����filter
  ��ڲ���: channelID
                          nFilterSize: filter data size
  ����ֵ      : ���䵽��channel_id
  ˵��            : channel_id ����Ϊ0
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateFilter(MTRDRV_U32 channelID,MTRDRV_U32 nFilterSize);

/*!
  ������     : MTR_DRV_TITLE_DMX_FreeFilter
  ��������: �ͷ�filter
  ��ڲ���: channelID
                          filterID
  ����ֵ      : 0 :�ɹ�
  ˵��            : ��
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeFilter(MTRDRV_U32 channelID,MTRDRV_U32 filterID);

/*!
  ������     : MTR_DRV_TITLE_DMX_SetChannelPid
  ��������: ����channel ���˵�pid
  ��ڲ���: channelID
                          nPid :���˵�pid
  ����ֵ      : 0 :�ɹ�
  ˵��            : ��
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_SetChannelPid(MTRDRV_U32 channelID, MTRDRV_U16 nPid);

/*!
  ������     : MTR_DRV_TITLE_DMX_ControlChannel
  ��������: ����channel ����״̬
  ��ڲ���: channelID
                          channelAction :channel ����״̬
  ����ֵ      : 0 :�ɹ�
  ˵��            : ��
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_ControlChannel(MTRDRV_U32 channelID,MTR_DRV_DMX_ChannelAction_t channelAction);

/*!
  ������     : MTR_DRV_TITLE_DMX_SetFilter
  ��������: ���ù�������
  ��ڲ���: channelID
                          nFilterId 
                          Table_id
                          Table_idMask
                          puiMatch
                          puiMask
                          DataLen:���˵����ݳ��ȼ�puiMatch����
  ����ֵ      : 0 :�ɹ�
  ˵��            : Table_id + puiMatch���������Ĺ�������
  */
MTRDRV_SINT MTR_DRV_TITLE_DMX_SetFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId,MTRDRV_U8 Table_id,MTRDRV_U8 Table_idMask,
								  MTRDRV_U8* puiMatch ,MTRDRV_U8 *puiMask, MTRDRV_U8 DataLen);
/*!
  ������     : MTR_DRV_TITLE_DMX_RegistChannelCallback
  ��������: ע��ص�����
  ��ڲ���: channelID
                          pfnCallBack :�ص�����ָ��,��filter�����յ����ݺ����
                          data :�յ����ݵ�ָ��
                          len   :�յ����ݵĳ���
  ����ֵ      : 0 :�ɹ�
  ˵��            : ��
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_RegistChannelCallback(MTRDRV_U32 channelID, MTRDRV_VOID(* pfnCallBack)(MTRDRV_U32 channelID,MTRDRV_U8* data,MTRDRV_U32 len));

/*!
  ������     : MTR_DRV_TITLE_DMX_GetCurrentChannelInfo
  ��������: ��ȡ��ǰ��Ŀ��Ƶ����Ϣ
  ��ڲ���: net_id [out]
                          ts_id   [out]
                          service_id [out]
  ����ֵ      : ��
  ˵��            : ��
  */
MTRDRV_VOID MTR_DRV_TITLE_DMX_GetCurrentChannelInfo(MTRDRV_U16 *net_id, MTRDRV_U16 *ts_id,MTRDRV_U16 *service_id);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskCreate
  ��������: ��������������������
  ��ڲ���: func : ������ں���ָ��
                          arg :���ݸ�������ں����Ĳ���
                          priority: �������ȼ�
                          stack:    �����ջ��ʼ��ַ
                          stack_size: �����ջ�Ĵ�С
  ����ֵ      : ����ID, 0:��������ʧ��
  ˵��            : ��
  */
MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskCreate(MTR_DRV_OD_ThreadEntryFunc func, PMTR_DRV_OS_VOID arg,MTR_DRV_OS_PRIORITY priority,
								  PMTR_DRV_OS_VOID stack, MTR_DRV_OS_U32 stack_size);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskDelete
  ��������: ɾ��һ������
  ��ڲ���: taskid
  ����ֵ      : MTR_DRV_OS_OK	-- ɾ������ɹ�
                           MTR_DRV_OS_ERR	-- ɾ������ʧ��
  ˵��            : Ŀǰδʹ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskDelete(MTR_DRV_OS_THREADID taskid);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskGetPriority
  ��������: ��ȡ�������ȼ�
  ��ڲ���: ��
  ����ֵ      : ���ô˺�������������ȼ���0Ϊ��ȡ�������ȼ�ʧ��
  ˵��            : Ŀǰδʹ��
  */
MTR_DRV_OS_PRIORITY MTR_DRV_TITLE_OS_TaskGetPriority(MTRDRV_VOID);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskSetPriority
  ��������: �����������ȼ�
  ��ڲ���: taskid
                          priority
  ����ֵ      : MTR_DRV_OS_OK	-- ������������ȼ��ɹ�
                           MTR_DRV_OS_ERR	-- ������������ȼ�ʧ��
  ˵��            : Ŀǰδʹ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid, MTR_DRV_OS_PRIORITY priority);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskGetCurTaskId
  ��������: ��ȡ��ǰ�����ID
  ��ڲ���: ��
  ����ֵ      : ���ô˺����������ID��0Ϊ��ȡ����IDʧ��
  ˵��            : Ŀǰδʹ��
  */
MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskGetCurTaskId(MTRDRV_VOID);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskSuspend
  ��������: ���������(��ͣ���������),�ú�����Ҫ��MTR_OS_TaskResume�����ɶ�ִ��
  ��ڲ���: taskid
  ����ֵ      : MTR_DRV_OS_OK	-- ��������ɹ�
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskResume
  ��������: �ָ�һ������������������,�ú�����Ҫ��MTR_OS_TaskSuspend�����ɶ�ִ��
  ��ڲ���: taskid
  ����ֵ      : MTR_DRV_OS_OK	-- ��������ɹ�
                           MTR_DRV_OS_ERR	-- ��������ʧ��
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskResume(MTR_DRV_OS_THREADID taskid);

/*!
  ������     : MTR_DRV_TITLE_OS_TaskSleep
  ��������:�õ�ǰ����˯��һ��ʱ��
  ��ڲ���: dMilliSeconds	-- ����˯�ߵ�ʱ�䣨ms��
  ����ֵ      : MTR_DRV_OS_OK	-- ��������ɹ�
                           MTR_DRV_OS_ERR	-- ��������ʧ��
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds);

/*!
  ������     : MTR_DRV_TITLE_OS_SemaphoreCreate
  ��������: ����һ���ź���
  ��ڲ���: semphore_init_no	-- ָ�����ź����ĳ�ʼ���źŸ���,����ø��ź���������ĳ��Դ,
                          �ø�������������ж��ٸ�����ͬʱ���ʸ���Դ
  ����ֵ      : semphore	-- �������ź�����ʶ��0��ʾ����ʧ��
  ˵��            : ����ֵ����Ϊ0
  */
MTR_DRV_OS_SEMPHORE MTR_DRV_TITLE_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no);

/*!
  ������     : MTR_DRV_TITLE_OS_SemaphoreDelete
  ��������: ɾ��һ���ź���
  ��ڲ���: semphoreid	-- �ź���ID
  ����ֵ      : MTR_DRV_OS_OK	-- ɾ��һ���ź����ɹ�
                           MTR_DRV_OS_ERR	-- ɾ��һ���ź���ʧ��
  ˵��            : Ŀǰδʹ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid);

/*!
  ������     : MTR_DRV_TITLE_OS_SemaphoreGet
  ��������: ��ָ������Ϣ���л�ȡһ����Ϣ
  ��ڲ���: semphoreid	-- �ź�����ID
                          wait_ornot	-- ָ���ڻ�ȡ�ź���ʧ��ʱ�Ƿ�ȴ�
                          wait_time	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�ȴ���ʱ��(ms),
                                              �ò�����wait_ornot����MTR_OS_WAITʱ��������,
                                              ��Ϊ0,���ʾһֱ�ȴ�
  ����ֵ      : MTR_DRV_OS_ERR		-- ��ȡ�ź���ʧ��
                           MTR_OS_TIMEOUT	-- ��ȡ�ź�����ʱ
                           MTR_DRV_OS_OK(>=0)	-- ��ȡ�ź�������ź�����ʣ����ź�������
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/*!
  ������     : MTR_DRV_TITLE_OS_SemaphoreRelease
  ��������: �ͷ�һ���ź���
  ��ڲ���: semphoreid	-- �ź���ID
  ����ֵ      : MTR_DRV_OS_OK	-- �ͷ��ź����ɹ�
                           MTR_DRV_OS_ERR	-- �ͷ��ź���ʧ��
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreRelease(MTR_DRV_OS_SEMPHORE semphoreid);

/*!
  ������     : MTR_DRV_TITLE_OS_QueueCreate
  ��������: ����һ����Ϣ����
  ��ڲ���: queue_element--�ö��п������ɵ������Ϣ����
  ����ֵ      : ���е�ID�ţ�0Ϊ��������ʧ�ܡ�
  ˵��            : ��
  */
MTR_DRV_OS_QUEUEID MTR_DRV_TITLE_OS_QueueCreate(MTR_DRV_OS_U32 queue_element);

/*!
  ������     : MTR_DRV_TITLE_OS_QueueDelete
  ��������: ����һ����Ϣ����
  ��ڲ���: queue_element--�ö��п������ɵ������Ϣ����
  ����ֵ      : ���е�ID�ţ�0Ϊ��������ʧ�ܡ�
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid);

/*!
  ������     : MTR_DRV_TITLE_OS_QueueSendMessage
  ��������: ��ָ������Ϣ���з���һ����Ϣ
  ��ڲ���: queueid		-- ��Ϣ���е�ID
                          pmessage	-- ָ��Ҫ���͵���Ϣ��ָ��
  ����ֵ      : MTR_DRV_OS_OK	-- �������﷢����Ϣ�ɹ�
                           MTR_DRV_OS_ERR	-- �������﷢����Ϣʧ��
  ˵��            : ��
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueSendMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_VOID * pmessage);

/*!
  ������     : MTR_DRV_TITLE_OS_QueueReceiveMessage
  ��������: ��ָ������Ϣ���л�ȡһ����Ϣ
  ��ڲ���: queueid	-- ��Ϣ���е�ID
                            wait_ornot	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�Ƿ�ȴ�
                            wait_time	-- ָ���ڻ�ȡ��Ϣʧ��ʱ�ȴ���ʱ��(ms),
                            �ò�����wait_ornot����MTR_OS_WAITʱ��������,
                            ��Ϊ0,���ʾһֱ�ȴ�
  ����ֵ      : pmessage	-- ��ȡ����Ϣָ��
                           0	       -- ��ȡ��Ϣʧ��
  ˵��            : ��
  */
PMTR_DRV_OS_VOID MTR_DRV_TITLE_OS_QueueReceiveMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/*!
  ������     : MTR_DRV_TITLE_OS_MemoryAllocate
  ��������: �����ڴ�
  ��ڲ���: size	-- Ҫ������ڴ�Ĵ�С(BYTE).�ú���Ҫ��MTR_DRV_TITLE_OS_MemoryFree�ɶ�ʹ��
  ����ֵ      : ������ڴ��ָ��,MTR_DRV_NULLΪ����ʧ��
  ˵��            : ��
  */
MTR_DRV_OS_VOID *MTR_DRV_TITLE_OS_MemoryAllocate(MTR_DRV_OS_U32 size);

/*!
  ������     : MTR_DRV_TITLE_OS_MemoryFree
  ��������: �ͷ��ڴ�
  ��ڲ���: semphoreid	-- �ź���ID
  ����ֵ      : paddress	-- Ҫ�ͷŵ��ڴ��ָ��.�ú���Ҫ��MTR_DRV_TITLE_OS_MemoryAllocate�ɶ�ʹ��
  ˵��            : MTR_DRV_OS_OK	-- �ͷ��ڴ�ɹ�
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_MemoryFree(MTR_DRV_OS_VOID *paddress);

/*!
  ������     : MTR_DRV_TITLE_OS_MemoryCopy
  ��������: �ڴ濽��
  ��ڲ���: semphoreid	-- �ź���ID
                           pdst	-- Ŀ���ַָ��
                           psrc	-- Դ��ַָ��
                           bytes	-- ���������ݳ���
����ֵ      : ��
˵��            : ��
  */
MTRDRV_VOID* MTR_DRV_TITLE_OS_MemoryCopy( MTRDRV_VOID* pdst, MTRDRV_VOID* psrc, MTRDRV_U32 bytes );

/*!
  ������     : MTR_DRV_TITLE_OS_SystemHalt
  ��������: ϵͳ����һ�������������صĴ���ϵͳ��Ҫ��������
  ��ڲ���: ��
  ����ֵ      : ��
  ˵��            : ��
  */
MTRDRV_VOID MTR_DRV_TITLE_OS_SystemHalt(MTRDRV_VOID);

/*!
  ������     : MTR_DRV_TITLE_OS_MemorySet
  ��������:����ָ���ĵ�ַ������
  ��ڲ���: mem		-- ��ַָ��
                           value	-- ���õ�ֵ
                           bytes	-- ���õ����ݳ���
  ����ֵ      : ���õĵ�ַָ��
  ˵��            : ��
  */
MTRDRV_VOID* MTR_DRV_TITLE_OS_MemorySet( MTRDRV_VOID* mem, MTRDRV_U8 value, MTRDRV_U32 bytes );

/*!
  ������     : MTR_DRV_TITLE_TIMER_Create
  ��������: ����һ����ʱ���������Ч��handleֵ
  ��ڲ���: handle		--	��������ÿһ����ʱ��
  ����ֵ      : 0���ɹ���
  ˵��            : ��
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Create(MTRDRV_HANDLE* handle);

/*!
  ������     : MTR_DRV_TITLE_TIMER_Free
  ��������: �ͷ�һ����ʱ��
  ��ڲ���: handle		--	��������ÿһ����ʱ��
  ����ֵ      : 0���ɹ���
  ˵��            : ��
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Free(MTRDRV_HANDLE* handle);

/*!
  ������     : MTR_DRV_TITLE_TIMER_SetParams
  ��������: ����һ����ʱ��
  ��ڲ���: handle	--	��������ÿһ����ʱ��
                          timeValue	--	��ʱ���趨��ʱ��ֵ����λ���루ms����
                                                ��0��������ʱ����0���ͷŶ�ʱ����
                          TimerMode	--	��ʱ��������ģʽ��TIMER_REPEAT���ظ��ͣ�TIMER_ONE_SHOT��һ���Եģ�
                          CallBackFunc--	��ʱ���������õĻص�����
  ����ֵ      : 0���ɹ���
  ˵��            : ��
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_SetParams(MTRDRV_HANDLE* handle, MTRDRV_U32 TimeValue, MTRDRV_U32 TimerMode, MTRDRV_VOID (*CallBackFunc)());

/*!
  ������     : MTR_DRV_TITLE_TIMER_Start
  ��������: ����һ����ʱ��
  ��ڲ���: handle		--	��������ÿһ����ʱ��
  ����ֵ      : 0���ɹ���
  ˵��            : ��
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Start(MTRDRV_HANDLE* handle);

/*!
  ������     : MTR_DRV_TITLE_TIMER_Stop
  ��������: ֹͣһ����ʱ���ļ�ʱ������û���ͷŶ�ʱ��
  ��ڲ���: handle		--	��������ÿһ����ʱ��
  ����ֵ      : 0���ɹ���
  ˵��            : ��
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Stop(MTRDRV_HANDLE* handle);

/*!
  ������     : MTR_DRV_TITLE_Print
  ��������: �����ӡ���
  ��ڲ���: *format	-- ����ִ�
  ����ֵ      : MTRDRV_OK	-- ��ӡ�ɹ�
                           MTR_ERROR	-- ��ӡʧ��
  ˵��            : ��
  */
MTRDRV_SINT MTR_DRV_TITLE_Print (MTRDRV_CHAR *format, ...);

/*!
  ������     : OnKeyPress_Is_Normal_or_Not
  ��������: �ж��ǲ�����Ҫ��Ļ�����ĺ���
  ��ڲ���: flg -- ��ʱûʲô�ã�����ֱ�Ӵ�0
  ����ֵ      : 1--��ʾ��ǰ״̬��Ҫ����,������ʾ����Ҫ������״̬
  ˵��            : ��
  */
MTRDRV_U8 OnKeyPress_Is_Normal_or_Not(MTRDRV_U8 flg);

/*!
  ������     : MTR_DRV_TITLE_OSD_DRAW_Rectangle
  ��������: �ú�����Ŀ���ǲ���ָ��λ�õĲ˵�
  ��ڲ���: RectCoordinate	-- ���������Լ���ȸ߶�
                          color -- ʵ�ľ��ε����ɫ(���color == 255���ǲ����˵�����ɫ)
  ����ֵ      : �ɹ�ʱ����MTRDRV_OK
  ˵��            : ��
  */
MTRDRV_VOID MTR_DRV_TITLE_OSD_DRAW_Rectangle(MTRDRV_OSD_RECTCOORDINATE RectCoordinate,MTRDRV_U32 color);

/*!
  ������     : TT_GFMFixupRectangleDepth
  ��������: 
  ��ڲ���:   1) x:		�����ĺ�����
                            2) y: 		������������
                            3) width: 		�����Ŀ��
                            4) height:	�����ĸ߶�
                            5) Bmpwidth: 	ͼƬ��������Ч���ؿ�
                            6) source:	����ͼƬһ�����ʱ�ĵ�һ��ͼƬ�����ݵ�ַ
                            7) source1:  	����ͼƬһ�����ʱ�ĵڶ���ͼƬ�����ݵ�ַ
                            8) Scoll_time: �����ж���һ��ͼƬ��������ͼƬ
                                (Scoll_time == 0)//��һ��ͼƬ�տ�ʼ��������
                                (Scoll_time == 1)//���һ��ͼƬ��ʼ������ʧ
                                (Scoll_time == 2)//����ͼƬ��������һ�������
  ˵��            : ��
  */
MTRDRV_VOID TT_GFMFixupRectangleDepth(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U32 Bmpwidth,MTRDRV_U8 *source,MTRDRV_U8 *source1,MTRDRV_U8 Scoll_time);

/*!
  ������     : TT_GIFdataFixupRectangle
  ��������: 
  ��ڲ���:   1) x:��ͼ�ĺ�����
                            2) y:��ͼ��������
                            3) width:	��ͼ�Ŀ��
                            4) height:��ͼ�ĸ߶�
                            5) source:ARGB16λͼƬ������
  ˵��            : ��
  */
MTRDRV_VOID TT_GIFdataFixupRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *source);

/*!
  ������     : MTR_DRV_TITLE_OSD_GetOpenState
  ��������: 
  ��ڲ���:   
  ����ֵ      : MTRDRV_FALSE =0,��
                           MTRDRV_TRUE	=1,��
  ˵��            : ��
  */
MTRDRV_BOOL	MTR_DRV_TITLE_OSD_GetOpenState(MTRDRV_VOID);

/*!
  ������     : MTR_DRV_TITLE_Get_OSD_Image
  ��������: ��ȡ��Ļ��������
  ��ڲ���:  X_x,Y_y 	--- ����
                            W_w,H_h --- ���ݿ�Ⱥ͸߶�
                            imageAdd --- ���ݴ洢�ĵ�ַ
  ����ֵ      : ��
  ˵��            : �˺�������д��
  */
MTRDRV_VOID MTR_DRV_TITLE_Get_OSD_Image( MTRDRV_S16 X_x, MTRDRV_S16 Y_y, MTRDRV_U16 W_w, MTRDRV_U16 H_h,MTRDRV_U32 imageAdd );

/*!
  ������     : MTR_TT_GFX_CalcOsdMemoryAddr
  ��������: Calculate the pointer(x,y) Osd memory start address,offset of bit
                           and bytes pre line.
  ��ڲ���: 1) x position.
                          2) y position.
                          3) return value of bit offset
                          4) return value of bytes pre line.
                          5) color depth
  ����ֵ      : Osd memory address
  ˵��            : �˺�������д��
  */
MTRDRV_U8* MTR_TT_GFX_CalcOsdMemoryAddr(MTRDRV_U16 x,MTRDRV_U16 y,MTRDRV_U8* bitOffset,MTRDRV_U16* bytePreLine,MTRDRV_U8 depth);

/*!
  ������     : Get_current_TV_format
  ��������: ��ȡ��ǰ�ĵ�����ʽ
  ��ڲ���: 
  ����ֵ      : N�Ʒ���1
                           P�Ʒ���0
  ˵��            : Ŀǰδʹ��
  */
MTRDRV_U8 Get_current_TV_format(MTRDRV_VOID);

/*!
  ������     : MTR_TITLE_Get_pixel
  ��������: ��ȡ����������
  ��ڲ���: ������Ļÿ���ƶ������ظ�����ȡֵ��Χ1-4�����������ΧĬ��1
  ����ֵ      : ��
  ˵��            : ��
  */
MTRDRV_VOID MTR_TITLE_Get_pixel(MTRDRV_U8 *g_pixel);
