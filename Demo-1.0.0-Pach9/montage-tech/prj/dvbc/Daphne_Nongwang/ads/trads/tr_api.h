/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

	//指针默认值
#ifndef MTR_DRV_NULL
    #define MTR_DRV_NULL 0
#endif

//
#define MTR_DRV_OS_OK		 0
#define MTR_DRV_OS_ERR		-1
#define MTR_DRV_OS_TIMEOUT	-2


//基本类型定义
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

//状态值
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
	MTRDRV_S16 X_x;		//x坐标
	MTRDRV_S16 Y_y;		//y坐标
	MTRDRV_S16 W_w;		//wide
	MTRDRV_S16 H_h;		//height
}MTRDRV_OSD_RECTCOORDINATE;	//如画矩形时用的左上角的点的坐标及x上的宽度,y上的高度

enum
{
	MTR_DRV_TIMER_ONE_SHOT, 	//一次性定时器
	MTR_DRV_TIMER_REPEAT		//重复性定时器
};//定时器的运行模式

typedef enum
{
	MTR_DRV_DMX_STOP_CHANNEL,
	MTR_DRV_DMX_START_CHANNEL,
	MTR_DRV_DMX_RESET_CHANNEL
} MTR_DRV_DMX_ChannelAction_t;

/*!
  函数名     : MTR_TITLE_Creat_ReceiveData_Task
  函数描述: 创建搜索字幕数据的任务
  入口参数: 无
  返回值      : 无
  说明            : 无
  */
MTRDRV_VOID MTR_TITLE_Creat_ReceiveData_Task(MTRDRV_VOID);

/*!
  函数名     : MTR_DRV_TITLE_DMX_AllocateSectionChannel
  函数描述: 分配channel
  入口参数: nFilterNumber: 改channel最多可以分配filter数量
                           nChannelBufferSize : channel的缓冲大小
  返回值      : 分配到的channel_id
  说明            : 返回值不能为0
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateSectionChannel(MTRDRV_U8 nFilterNumber,MTRDRV_U32 nChannelBufferSize);

/*!
  函数名     : MTR_DRV_TITLE_DMX_FreeChannel
  函数描述: 释放channel
  入口参数: channelID
  返回值      : 0 :成功
  说明            : 无
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeChannel(MTRDRV_U32 channelID);

/*!
  函数名     : MTR_DRV_TITLE_DMX_AllocateFilter
  函数描述: 分配filter
  入口参数: channelID
                          nFilterSize: filter data size
  返回值      : 分配到的channel_id
  说明            : channel_id 不能为0
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_AllocateFilter(MTRDRV_U32 channelID,MTRDRV_U32 nFilterSize);

/*!
  函数名     : MTR_DRV_TITLE_DMX_FreeFilter
  函数描述: 释放filter
  入口参数: channelID
                          filterID
  返回值      : 0 :成功
  说明            : 无
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_FreeFilter(MTRDRV_U32 channelID,MTRDRV_U32 filterID);

/*!
  函数名     : MTR_DRV_TITLE_DMX_SetChannelPid
  函数描述: 设置channel 过滤的pid
  入口参数: channelID
                          nPid :过滤的pid
  返回值      : 0 :成功
  说明            : 无
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_SetChannelPid(MTRDRV_U32 channelID, MTRDRV_U16 nPid);

/*!
  函数名     : MTR_DRV_TITLE_DMX_ControlChannel
  函数描述: 设置channel 工作状态
  入口参数: channelID
                          channelAction :channel 工作状态
  返回值      : 0 :成功
  说明            : 无
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_ControlChannel(MTRDRV_U32 channelID,MTR_DRV_DMX_ChannelAction_t channelAction);

/*!
  函数名     : MTR_DRV_TITLE_DMX_SetFilter
  函数描述: 设置过滤条件
  入口参数: channelID
                          nFilterId 
                          Table_id
                          Table_idMask
                          puiMatch
                          puiMask
                          DataLen:过滤的数据长度即puiMatch长度
  返回值      : 0 :成功
  说明            : Table_id + puiMatch构成完整的过滤数据
  */
MTRDRV_SINT MTR_DRV_TITLE_DMX_SetFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId,MTRDRV_U8 Table_id,MTRDRV_U8 Table_idMask,
								  MTRDRV_U8* puiMatch ,MTRDRV_U8 *puiMask, MTRDRV_U8 DataLen);
/*!
  函数名     : MTR_DRV_TITLE_DMX_RegistChannelCallback
  函数描述: 注册回调函数
  入口参数: channelID
                          pfnCallBack :回调函数指针,在filter过滤收到数据后调用
                          data :收到数据的指针
                          len   :收到数据的长度
  返回值      : 0 :成功
  说明            : 无
  */
MTRDRV_U32 MTR_DRV_TITLE_DMX_RegistChannelCallback(MTRDRV_U32 channelID, MTRDRV_VOID(* pfnCallBack)(MTRDRV_U32 channelID,MTRDRV_U8* data,MTRDRV_U32 len));

/*!
  函数名     : MTR_DRV_TITLE_DMX_GetCurrentChannelInfo
  函数描述: 获取当前节目的频点信息
  入口参数: net_id [out]
                          ts_id   [out]
                          service_id [out]
  返回值      : 无
  说明            : 无
  */
MTRDRV_VOID MTR_DRV_TITLE_DMX_GetCurrentChannelInfo(MTRDRV_U16 *net_id, MTRDRV_U16 *ts_id,MTRDRV_U16 *service_id);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskCreate
  函数描述: 创建任务并启动任务运行
  入口参数: func : 任务入口函数指针
                          arg :传递给任务入口函数的参数
                          priority: 任务优先级
                          stack:    任务堆栈起始地址
                          stack_size: 任务堆栈的大小
  返回值      : 任务ID, 0:创建任务失败
  说明            : 无
  */
MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskCreate(MTR_DRV_OD_ThreadEntryFunc func, PMTR_DRV_OS_VOID arg,MTR_DRV_OS_PRIORITY priority,
								  PMTR_DRV_OS_VOID stack, MTR_DRV_OS_U32 stack_size);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskDelete
  函数描述: 删除一个任务
  入口参数: taskid
  返回值      : MTR_DRV_OS_OK	-- 删除任务成功
                           MTR_DRV_OS_ERR	-- 删除任务失败
  说明            : 目前未使用
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskDelete(MTR_DRV_OS_THREADID taskid);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskGetPriority
  函数描述: 获取任务优先级
  入口参数: 无
  返回值      : 调用此函数的任务的优先级，0为获取任务优先级失败
  说明            : 目前未使用
  */
MTR_DRV_OS_PRIORITY MTR_DRV_TITLE_OS_TaskGetPriority(MTRDRV_VOID);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskSetPriority
  函数描述: 设置任务优先级
  入口参数: taskid
                          priority
  返回值      : MTR_DRV_OS_OK	-- 设置任务的优先级成功
                           MTR_DRV_OS_ERR	-- 设置任务的优先级失败
  说明            : 目前未使用
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid, MTR_DRV_OS_PRIORITY priority);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskGetCurTaskId
  函数描述: 获取当前任务的ID
  入口参数: 无
  返回值      : 调用此函数的任务的ID，0为获取任务ID失败
  说明            : 目前未使用
  */
MTR_DRV_OS_THREADID MTR_DRV_TITLE_OS_TaskGetCurTaskId(MTRDRV_VOID);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskSuspend
  函数描述: 将任务挂起(暂停任务的运行),该函数需要和MTR_OS_TaskResume函数成对执行
  入口参数: taskid
  返回值      : MTR_DRV_OS_OK	-- 挂起任务成功
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskResume
  函数描述: 恢复一个被挂起的任务的运行,该函数需要和MTR_OS_TaskSuspend函数成对执行
  入口参数: taskid
  返回值      : MTR_DRV_OS_OK	-- 唤醒任务成功
                           MTR_DRV_OS_ERR	-- 唤醒任务失败
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskResume(MTR_DRV_OS_THREADID taskid);

/*!
  函数名     : MTR_DRV_TITLE_OS_TaskSleep
  函数描述:让当前任务睡眠一段时间
  入口参数: dMilliSeconds	-- 任务睡眠的时间（ms）
  返回值      : MTR_DRV_OS_OK	-- 休眠任务成功
                           MTR_DRV_OS_ERR	-- 休眠任务失败
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds);

/*!
  函数名     : MTR_DRV_TITLE_OS_SemaphoreCreate
  函数描述: 创建一个信号量
  入口参数: semphore_init_no	-- 指定该信号量的初始化信号个数,如果用该信号量来保护某资源,
                          该个数则表明可以有多少个任务同时访问该资源
  返回值      : semphore	-- 创建的信号量标识，0表示创建失败
  说明            : 返回值不能为0
  */
MTR_DRV_OS_SEMPHORE MTR_DRV_TITLE_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no);

/*!
  函数名     : MTR_DRV_TITLE_OS_SemaphoreDelete
  函数描述: 删除一个信号量
  入口参数: semphoreid	-- 信号量ID
  返回值      : MTR_DRV_OS_OK	-- 删除一个信号量成功
                           MTR_DRV_OS_ERR	-- 删除一个信号量失败
  说明            : 目前未使用
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid);

/*!
  函数名     : MTR_DRV_TITLE_OS_SemaphoreGet
  函数描述: 从指定的消息队列获取一个消息
  入口参数: semphoreid	-- 信号量的ID
                          wait_ornot	-- 指定在获取信号量失败时是否等待
                          wait_time	-- 指定在获取消息失败时等待的时间(ms),
                                              该参数在wait_ornot等于MTR_OS_WAIT时采用意义,
                                              若为0,则表示一直等待
  返回值      : MTR_DRV_OS_ERR		-- 获取信号量失败
                           MTR_OS_TIMEOUT	-- 获取信号量超时
                           MTR_DRV_OS_OK(>=0)	-- 获取信号量后该信号量还剩余的信号量个数
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/*!
  函数名     : MTR_DRV_TITLE_OS_SemaphoreRelease
  函数描述: 释放一个信号量
  入口参数: semphoreid	-- 信号量ID
  返回值      : MTR_DRV_OS_OK	-- 释放信号量成功
                           MTR_DRV_OS_ERR	-- 释放信号量失败
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_SemaphoreRelease(MTR_DRV_OS_SEMPHORE semphoreid);

/*!
  函数名     : MTR_DRV_TITLE_OS_QueueCreate
  函数描述: 创建一个消息队列
  入口参数: queue_element--该队列可以容纳的最大消息个数
  返回值      : 队列的ID号，0为创建队列失败。
  说明            : 无
  */
MTR_DRV_OS_QUEUEID MTR_DRV_TITLE_OS_QueueCreate(MTR_DRV_OS_U32 queue_element);

/*!
  函数名     : MTR_DRV_TITLE_OS_QueueDelete
  函数描述: 创建一个消息队列
  入口参数: queue_element--该队列可以容纳的最大消息个数
  返回值      : 队列的ID号，0为创建队列失败。
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid);

/*!
  函数名     : MTR_DRV_TITLE_OS_QueueSendMessage
  函数描述: 往指定的消息队列发送一个消息
  入口参数: queueid		-- 消息队列的ID
                          pmessage	-- 指向要发送的消息的指针
  返回值      : MTR_DRV_OS_OK	-- 往队列里发送消息成功
                           MTR_DRV_OS_ERR	-- 往队列里发送消息失败
  说明            : 无
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_QueueSendMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_VOID * pmessage);

/*!
  函数名     : MTR_DRV_TITLE_OS_QueueReceiveMessage
  函数描述: 从指定的消息队列获取一个消息
  入口参数: queueid	-- 消息队列的ID
                            wait_ornot	-- 指定在获取消息失败时是否等待
                            wait_time	-- 指定在获取消息失败时等待的时间(ms),
                            该参数在wait_ornot等于MTR_OS_WAIT时采用意义,
                            若为0,则表示一直等待
  返回值      : pmessage	-- 获取的消息指针
                           0	       -- 获取消息失败
  说明            : 无
  */
PMTR_DRV_OS_VOID MTR_DRV_TITLE_OS_QueueReceiveMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/*!
  函数名     : MTR_DRV_TITLE_OS_MemoryAllocate
  函数描述: 分配内存
  入口参数: size	-- 要分配的内存的大小(BYTE).该函数要和MTR_DRV_TITLE_OS_MemoryFree成对使用
  返回值      : 分配的内存的指针,MTR_DRV_NULL为分配失败
  说明            : 无
  */
MTR_DRV_OS_VOID *MTR_DRV_TITLE_OS_MemoryAllocate(MTR_DRV_OS_U32 size);

/*!
  函数名     : MTR_DRV_TITLE_OS_MemoryFree
  函数描述: 释放内存
  入口参数: semphoreid	-- 信号量ID
  返回值      : paddress	-- 要释放的内存的指针.该函数要和MTR_DRV_TITLE_OS_MemoryAllocate成对使用
  说明            : MTR_DRV_OS_OK	-- 释放内存成功
  */
MTR_DRV_OS_STATUS MTR_DRV_TITLE_OS_MemoryFree(MTR_DRV_OS_VOID *paddress);

/*!
  函数名     : MTR_DRV_TITLE_OS_MemoryCopy
  函数描述: 内存拷贝
  入口参数: semphoreid	-- 信号量ID
                           pdst	-- 目标地址指针
                           psrc	-- 源地址指针
                           bytes	-- 拷贝的数据长度
返回值      : 无
说明            : 无
  */
MTRDRV_VOID* MTR_DRV_TITLE_OS_MemoryCopy( MTRDRV_VOID* pdst, MTRDRV_VOID* psrc, MTRDRV_U32 bytes );

/*!
  函数名     : MTR_DRV_TITLE_OS_SystemHalt
  函数描述: 系统挂起，一般是遇到了严重的错误，系统需要重新启动
  入口参数: 无
  返回值      : 无
  说明            : 无
  */
MTRDRV_VOID MTR_DRV_TITLE_OS_SystemHalt(MTRDRV_VOID);

/*!
  函数名     : MTR_DRV_TITLE_OS_MemorySet
  函数描述:设置指定的地址的数据
  入口参数: mem		-- 地址指针
                           value	-- 设置的值
                           bytes	-- 设置的数据长度
  返回值      : 设置的地址指针
  说明            : 无
  */
MTRDRV_VOID* MTR_DRV_TITLE_OS_MemorySet( MTRDRV_VOID* mem, MTRDRV_U8 value, MTRDRV_U32 bytes );

/*!
  函数名     : MTR_DRV_TITLE_TIMER_Create
  函数描述: 申请一个定时器，获得有效的handle值
  入口参数: handle		--	用来区别每一个软定时器
  返回值      : 0：成功；
  说明            : 无
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Create(MTRDRV_HANDLE* handle);

/*!
  函数名     : MTR_DRV_TITLE_TIMER_Free
  函数描述: 释放一个定时器
  入口参数: handle		--	用来区别每一个软定时器
  返回值      : 0：成功；
  说明            : 无
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Free(MTRDRV_HANDLE* handle);

/*!
  函数名     : MTR_DRV_TITLE_TIMER_SetParams
  函数描述: 设置一个软定时器
  入口参数: handle	--	用来区别每一个软定时器
                          timeValue	--	定时器设定的时间值，单位毫秒（ms）；
                                                非0：启动定时器；0：释放定时器；
                          TimerMode	--	定时器的运行模式：TIMER_REPEAT：重复型；TIMER_ONE_SHOT：一次性的；
                          CallBackFunc--	定时器到点后调用的回调函数
  返回值      : 0：成功；
  说明            : 无
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_SetParams(MTRDRV_HANDLE* handle, MTRDRV_U32 TimeValue, MTRDRV_U32 TimerMode, MTRDRV_VOID (*CallBackFunc)());

/*!
  函数名     : MTR_DRV_TITLE_TIMER_Start
  函数描述: 启动一个软定时器
  入口参数: handle		--	用来区别每一个软定时器
  返回值      : 0：成功；
  说明            : 无
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Start(MTRDRV_HANDLE* handle);

/*!
  函数名     : MTR_DRV_TITLE_TIMER_Stop
  函数描述: 停止一个定时器的计时，但并没有释放定时器
  入口参数: handle		--	用来区别每一个软定时器
  返回值      : 0：成功；
  说明            : 无
  */
MTRDRV_S32	MTR_DRV_TITLE_TIMER_Stop(MTRDRV_HANDLE* handle);

/*!
  函数名     : MTR_DRV_TITLE_Print
  函数描述: 常规打印语句
  入口参数: *format	-- 输出字串
  返回值      : MTRDRV_OK	-- 打印成功
                           MTR_ERROR	-- 打印失败
  说明            : 无
  */
MTRDRV_SINT MTR_DRV_TITLE_Print (MTRDRV_CHAR *format, ...);

/*!
  函数名     : OnKeyPress_Is_Normal_or_Not
  函数描述: 判断是不是需要字幕滚动的函数
  入口参数: flg -- 暂时没什么用，可以直接传0
  返回值      : 1--表示当前状态需要滚动,其他表示不需要滚动的状态
  说明            : 无
  */
MTRDRV_U8 OnKeyPress_Is_Normal_or_Not(MTRDRV_U8 flg);

/*!
  函数名     : MTR_DRV_TITLE_OSD_DRAW_Rectangle
  函数描述: 该函数的目的是擦除指定位置的菜单
  入口参数: RectCoordinate	-- 矩形坐标以及宽度高度
                          color -- 实心矩形的填充色(如果color == 255就是擦除菜单的颜色)
  返回值      : 成功时返回MTRDRV_OK
  说明            : 无
  */
MTRDRV_VOID MTR_DRV_TITLE_OSD_DRAW_Rectangle(MTRDRV_OSD_RECTCOORDINATE RectCoordinate,MTRDRV_U32 color);

/*!
  函数名     : TT_GFMFixupRectangleDepth
  函数描述: 
  入口参数:   1) x:		滚屏的横坐标
                            2) y: 		滚屏的纵坐标
                            3) width: 		滚屏的宽度
                            4) height:	滚屏的高度
                            5) Bmpwidth: 	图片的真正有效像素宽
                            6) source:	两张图片一起滚动时的第一张图片的数据地址
                            7) source1:  	两张图片一起滚动时的第二张图片的数据地址
                            8) Scoll_time: 用于判断是一张图片还是两张图片
                                (Scoll_time == 0)//第一张图片刚开始慢慢出来
                                (Scoll_time == 1)//最后一张图片开始慢慢消失
                                (Scoll_time == 2)//两张图片链接起来一起滚动的
  说明            : 无
  */
MTRDRV_VOID TT_GFMFixupRectangleDepth(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U32 Bmpwidth,MTRDRV_U8 *source,MTRDRV_U8 *source1,MTRDRV_U8 Scoll_time);

/*!
  函数名     : TT_GIFdataFixupRectangle
  函数描述: 
  入口参数:   1) x:贴图的横坐标
                            2) y:贴图的纵坐标
                            3) width:	贴图的宽度
                            4) height:贴图的高度
                            5) source:ARGB16位图片的数据
  说明            : 无
  */
MTRDRV_VOID TT_GIFdataFixupRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *source);

/*!
  函数名     : MTR_DRV_TITLE_OSD_GetOpenState
  函数描述: 
  入口参数:   
  返回值      : MTRDRV_FALSE =0,否
                           MTRDRV_TRUE	=1,是
  说明            : 无
  */
MTRDRV_BOOL	MTR_DRV_TITLE_OSD_GetOpenState(MTRDRV_VOID);

/*!
  函数名     : MTR_DRV_TITLE_Get_OSD_Image
  函数描述: 获取字幕背景数据
  入口参数:  X_x,Y_y 	--- 坐标
                            W_w,H_h --- 数据宽度和高度
                            imageAdd --- 数据存储的地址
  返回值      : 无
  说明            : 此函数可以写空
  */
MTRDRV_VOID MTR_DRV_TITLE_Get_OSD_Image( MTRDRV_S16 X_x, MTRDRV_S16 Y_y, MTRDRV_U16 W_w, MTRDRV_U16 H_h,MTRDRV_U32 imageAdd );

/*!
  函数名     : MTR_TT_GFX_CalcOsdMemoryAddr
  函数描述: Calculate the pointer(x,y) Osd memory start address,offset of bit
                           and bytes pre line.
  入口参数: 1) x position.
                          2) y position.
                          3) return value of bit offset
                          4) return value of bytes pre line.
                          5) color depth
  返回值      : Osd memory address
  说明            : 此函数可以写空
  */
MTRDRV_U8* MTR_TT_GFX_CalcOsdMemoryAddr(MTRDRV_U16 x,MTRDRV_U16 y,MTRDRV_U8* bitOffset,MTRDRV_U16* bytePreLine,MTRDRV_U8 depth);

/*!
  函数名     : Get_current_TV_format
  函数描述: 获取当前的电视制式
  入口参数: 
  返回值      : N制返回1
                           P制返回0
  说明            : 目前未使用
  */
MTRDRV_U8 Get_current_TV_format(MTRDRV_VOID);

/*!
  函数名     : MTR_TITLE_Get_pixel
  函数描述: 获取滚动的像素
  入口参数: 滚动字幕每次移动的像素个数，取值范围1-4，超过这个范围默认1
  返回值      : 无
  说明            : 无
  */
MTRDRV_VOID MTR_TITLE_Get_pixel(MTRDRV_U8 *g_pixel);
