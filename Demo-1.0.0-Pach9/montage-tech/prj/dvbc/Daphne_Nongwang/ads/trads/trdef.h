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
/*01*/LOGO_IFRAME = 1,       //开机画面
/*02*/MIANMENU_UP,           //主菜单上
/*03*/MIANMENU_DOWN,   //主菜单下
/*04*/EPG_GIF,                          //EPG节目指南列表
/*05*/NVOD_GIF,                      //nvod视频点播列表
/*06*/OTHERMENU_GIF,        //其他菜单
/*07*/BANNER_GIF,                 //旗帜广告，现有多个
/*08*/RADIO_PIC,           //广播下轮播广告，现有多个
/*09*/CHANGE_CHANNEL,    //换台显示一整张广告，一个频点对应一个
/*10*/VOLUME_GIF,                //音量条
/*11*/MANAGE_CHANNEL,   //频道管理
/*12*/BROADCAST_LIST,        //数据广播列表
/*13*/CA_INFORMATION,      //Ca提示信息
/*14*/SORTCHANNEL_LIST,   //喜爱/电视/广播节目列表
/*15*/URGENCY_NOTICE,      //紧急通知
/*16*/MAINMENU_MIDDLE, //主菜单广告二
/*17*/HANGAD_GIF,                //挂角广告
/*18*/MENUAD_GIFONE,       //菜单广告一
/*19*/MENUAD_GIFTWO,      //菜单广告二
/*20*/MENUAD_GIFTHREE,   //菜单广告三
/*21*/MENUAD_GIFFOUR,     //菜单广告四
	    UNDEFINED,
}MTR_DRV_AdType;


typedef struct
{
	MTRDRV_U8  Type;//贴的是图的类型序号。2，3....
	MTRDRV_S16 X_x;//贴图起始横坐标
	MTRDRV_S16 Y_y;//贴图起始纵坐标
}Lib_GIF_Position;


typedef  struct
{
	MTRDRV_S16 X_x;
	MTRDRV_S16 Y_y;

}MTR_DRV_OSD_DOTCOORDINATE;			//一个点的坐标


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
	MTR_DRV_TIMER_ONE_SHOT,		//一次性定时器
	MTR_DRV_TIMER_REPEAT,		//重复性定时器
}MTR_DRV_TIMER_MODE;//Operation mode of timer

//如果要增加状态值，一定要为负数，因为在程序中有时候会用整数来表示某些正确的值
#define MTR_DRV_OS_OK		0
#define MTR_DRV_OS_ERR		-1
#define MTR_DRV_OS_TIMEOUT	-2
/***********************************过滤器工作状态类型********************/
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

函数名：	MTR_AD_Creat_Drv_Task

函数描述：	创建一个接收数据的任务
(本来可以不用写这个接口函数，写这个的目的是在启动的时候有个创建任务的顺序，集成方也可以直接把这个函数写空，在自己的流程中创建这个任务)

入口参数：	无

返回值：	无；

******************************************************************************/
MTRDRV_VOID	MTR_AD_Creat_Drv_Task(MTRDRV_VOID);


/******************************************************************************

函数名：	MTR_DRV_TIMER_CreateTimer

函数描述：	申请一个定时器，获得有效的handle值

入口参数：	handle		--	用来区别每一个软定时器

返回值：	0：成功；

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_CreateTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

函数名：	MTR_DRV_TIMER_FreeTimer

函数描述：	释放一个定时器

入口参数：	handle		--	用来区别每一个软定时器

返回值：	0：成功；

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_FreeTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

函数名：	MTR_DRV_TIMER_SetTimer

函数描述：	设置一个软定时器

入口参数：	handle		--	用来区别每一个软定时器
			timeValue	--	定时器设定的时间值，单位秒（s）；
							非0：启动定时器；0：释放定时器；
			TimerMode	--	定时器的运行模式：TIMER_REPEAT：重复型；TIMER_ONE_SHOT：一次性的；
			CallBackFunc--	定时器到点后调用的回调函数

返回值：	0：成功；

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_SetTimer(MTRDRV_HANDLE* handle, MTRDRV_U32 TimeValue, MTRDRV_U32 TimerMode, void (*CallBackFunc)());

/******************************************************************************

函数名：	MTR_DRV_TIMER_StartTimer

函数描述：	启动一个软定时器

入口参数：	handle		--	用来区别每一个软定时器

返回值：	0：成功；

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_StartTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

函数名：	MTR_DRV_TIMER_StopTimer

函数描述：	停止一个定时器的计时，但并没有释放定时器

入口参数：	handle		--	用来区别每一个软定时器

返回值：	0：成功；

******************************************************************************/
MTRDRV_S32	MTR_DRV_TIMER_StopTimer(MTRDRV_HANDLE* handle);

/******************************************************************************

函数名：	MTR_DRV_API_printf

函数描述：	常规打印语句

入口参数：	*format	-- 输出字串

返回值：	MTRDRV_OK	-- 打印成功
			MTRDRV_ERROR	-- 打印失败

******************************************************************************/
MTRDRV_SINT MTR_DRV_API_printf(MTRDRV_CHAR *format, ...);

/******************************************************************************

函数名：	MTR_DRV_TUNER_GetLK

函数描述：	获取当前Tuner锁定状态，读寄存器

入口参数：	空

返回值：	1			-- 锁定
			0			-- 失锁

******************************************************************************/
MTRDRV_U8 MTR_DRV_TUNER_GetLK (void);

/******************************************************************************

函数名：	MTR_DRV_TUNER_SetParams

函数描述：	Tuner主要处理函数

入口参数：	MTRDRV_U32 _chanFrequency		-- 输入频率(10KHz)
			MTRDRV_U32 _chanSymbolRate	-- 输入符号率(Ks/s)
			MTRDRV_U8 _Qammode		-- QAM格式或极化方向或DVBT带宽(0:16QAM,1:32QAM,2:64QAM,3:128QAM,4:256QAM)

返回值：	1	-- 锁定退出
			0	-- 未锁定退出

******************************************************************************/
MTRDRV_S8 MTR_DRV_TUNER_SetParams (MTRDRV_U32 _chanFrequency,MTRDRV_U32 _chanSymbolRate,MTRDRV_U8 _Qammode);

/******************************************************************************

函数名：	MTR_DRV_TUNER_GetMainFreqParams

函数描述：	获得主频点参数

入口参数：	MTRDRV_U32 _chanFrequency		-- 输入频率(10KHz)
			MTRDRV_U32 _chanSymbolRate	-- 输入符号率(Ks/s)
			MTRDRV_U8 _Qammode		-- QAM格式或极化方向或DVBT带宽(0:16QAM,1:32QAM,2:64QAM,3:128QAM,4:256QAM)

返回值：	无

******************************************************************************/
MTRDRV_VOID MTR_DRV_TUNER_GetMainFreqParams(MTRDRV_U32* _chanFrequency,MTRDRV_U32* _chanSymbolRate,MTRDRV_U8* _Qammode);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskCreate

函数描述：	创建任务并启动任务运行

入口参数：	func		-- 任务入口函数指针
			arg			-- 传递给任务入口函数的参数
			dPriority	-- 任务的优先级
			stack		-- 任务的堆栈起始地址
			stack_size	-- 任务堆栈的大小

返回值：	任务的ID，0为创建任务失败

******************************************************************************/
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskCreate(MTR_OD_ThreadEntryFunc func, PMTR_DRV_OS_VOID arg,MTR_DRV_OS_PRIORITY priority,
								  PMTR_DRV_OS_VOID stack, MTR_DRV_OS_U32 stack_size);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskDelete(目前未用到,预留)

函数描述：	删除一个任务

入口参数：	taskid	-- 任务ID

返回值：	MTR_DRV_OS_OK	-- 删除任务成功
			MTR_DRV_OS_ERR	-- 删除任务失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskDelete(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskGetCurTaskPriority

函数描述：	获取当前任务的ID

入口参数：	无

返回值：	调用此函数的任务的ID，0为获取任务ID失败

******************************************************************************/
MTR_DRV_OS_PRIORITY MTR_DRV_OS_TaskGetCurTaskPriority(MTRDRV_VOID);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskSetPriority

函数描述：	设置任务的优先级

入口参数：	taskid		-- 任务ID
			priority	-- 要设置的优先级

返回值：	MTR_DRV_OS_OK	-- 设置任务的优先级成功
			MTR_DRV_OS_ERR	-- 设置任务的优先级失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSetPriority(MTR_DRV_OS_THREADID taskid, MTR_DRV_OS_PRIORITY priority);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskGetCurTaskId(目前未用到,预留)

函数描述：	获取当前任务的ID

入口参数：	无

返回值：	调用此函数的任务的ID，0为获取任务ID失败

******************************************************************************/
MTR_DRV_OS_THREADID MTR_DRV_OS_TaskGetCurTaskId(MTRDRV_VOID);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskSuspend(目前未用到,预留)

函数描述：	将任务挂起(暂停任务的运行),该函数需要和MTR_OS_TaskResume函数成对执行

入口参数：	taskid	-- 任务ID,当为0时会挂起调用本函数的任务自身.

返回值：	MTR_DRV_OS_OK	-- 挂起任务成功

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSuspend(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskResume(目前未用到,预留)

函数描述：	恢复一个被挂起的任务的运行,该函数需要和MTR_OS_TaskSuspend函数成对执行

入口参数：	taskid	-- 任务ID

返回值：	MTR_DRV_OS_OK	-- 唤醒任务成功
			MTR_DRV_OS_ERR	-- 唤醒任务失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskResume(MTR_DRV_OS_THREADID taskid);

/******************************************************************************

函数名：	MTR_DRV_OS_TaskSleep

函数描述：	让当前任务睡眠一段时间

入口参数：	dMilliSeconds	-- 任务睡眠的时间（ms）

返回值：	MTR_DRV_OS_OK	-- 休眠任务成功
			MTR_DRV_OS_ERR	-- 休眠任务失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_TaskSleep(MTR_DRV_OS_U32 dMilliSeconds);

/******************************************************************************

函数名：	MTR_DRV_OS_SemaphoreCreate

函数描述：	创建一个信号量

入口参数：	semphore_init_no	-- 指定该信号量的初始化信号个数,如果用该信号量来保护某资源,该个数则表明
						可以有多少个任务同时访问该资源

返回值：	semphore	-- 创建的信号量标识，0表示创建失败

******************************************************************************/
MTR_DRV_OS_SEMPHORE MTR_DRV_OS_SemaphoreCreate(MTR_DRV_OS_U32 semphore_init_no);

/******************************************************************************

函数名：	MTR_DRV_OS_SemaphoreDelete(目前未用到,预留)

函数描述：	删除一个信号量

入口参数：	semphoreid	-- 信号量ID

返回值：	MTR_DRV_OS_OK	-- 删除一个信号量成功
			MTR_DRV_OS_ERR	-- 删除一个信号量失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreDelete(MTR_DRV_OS_SEMPHORE semphoreid);

/******************************************************************************

函数名：	MTR_DRV_OS_SemaphoreGet

函数描述：	从指定的消息队列获取一个消息

入口参数：	semphoreid	-- 信号量的ID
			wait_ornot	-- 指定在获取信号量失败时是否等待
			wait_time	-- 指定在获取消息失败时等待的时间(ms),该参数在wait_ornot等于
						MTR_OS_WAIT时采用意义,若为0,则表示一直等待

返回值：	MTR_DRV_OS_ERR		-- 获取信号量失败
			MTR_DRV_OS_TIMEOUT	-- 获取信号量超时
			MTR_DRV_OS_OK(>=0)	-- 获取信号量后该信号量还剩余的信号量个数

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreGet(MTR_DRV_OS_SEMPHORE semphoreid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/******************************************************************************

函数名：	MTR_DRV_OS_SemaphoreRelease

函数描述：	释放一个信号量

入口参数：	semphoreid	-- 信号量的ID

返回值：	MTR_DRV_OS_OK	-- 释放信号量成功
			MTR_DRV_OS_ERR	-- 释放信号量失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_SemaphoreRelease(MTR_DRV_OS_SEMPHORE semphoreid);


/******************************************************************************

函数名：	MTR_DRV_OS_QueueCreate

函数描述：	创建一个消息队列

入口参数：	queue_element--该队列可以容纳的最大消息个数

返回值：	队列的ID号，0为创建队列失败。

******************************************************************************/
MTR_DRV_OS_QUEUEID MTR_DRV_OS_QueueCreate(MTR_DRV_OS_U32 queue_element);

/******************************************************************************

函数名：	MTR_DRV_OS_QueueDelete(目前未用到,预留)

函数描述：	删除一个消息队列

入口参数：	queueid	-- 消息队列的ID

返回值：	MTR_DRV_OS_OK	-- 删除队列成功
			MTR_DRV_OS_ERR	-- 删除队列失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueDelete(MTR_DRV_OS_QUEUEID queueid);

/******************************************************************************

函数名：	MTR_DRV_OS_QueueSendMessage

函数描述：	往指定的消息队列发送一个消息

入口参数：	queueid		-- 消息队列的ID
			pmessage	-- 指向要发送的消息的指针

返回值：	MTR_DRV_OS_OK	-- 往队列里发送消息成功
			MTR_DRV_OS_ERR	-- 往队列里发送消息失败

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_QueueSendMessage(MTR_DRV_OS_QUEUEID queueid,MTR_OS_Msg *pmessage);

/******************************************************************************

Name:        MTR_DRV_OS_QueueReceiveMessage

Description: 从指定的消息队列获取一个消息

Parameters:  queueid	-- 消息队列的ID
			 wait_ornot	-- 指定在获取消息失败时是否等待
			 wait_time	-- 指定在获取消息失败时等待的时间(ms),该参数在wait_ornot等于
						MTR_OS_WAIT时采用意义,若为0,则表示一直等待

Returns:     pmessage	-- 获取的消息指针
			 0			-- 获取消息失败

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_QueueReceiveMessage(MTR_DRV_OS_QUEUEID queueid,MTR_DRV_OS_WAITMODE wait_ornot,MTR_DRV_OS_U32 wait_time);

/******************************************************************************

函数名：	MTR_DRV_OS_MemoryAllocate

函数描述：	分配内存

入口参数：	size	-- 要分配的内存的大小(BYTE).该函数要和MTRAPI_OS_MemoryFree成对使用

返回值：	分配的内存的指针,NULL为分配失败

******************************************************************************/
MTR_OS_VOID *MTR_DRV_OS_MemoryAllocate(MTR_DRV_OS_U32 size);

/******************************************************************************

函数名：	MTR_DRV_OS_MemoryFree

函数描述：	释放内存

入口参数：	paddress	-- 要释放的内存的指针.该函数要和MTRAPI_OS_MemoryAllocate成对使用

返回值：	MTR_DRV_OS_OK	-- 释放内存成功

******************************************************************************/
MTR_DRV_OS_STATUS MTR_DRV_OS_MemoryFree(MTR_OS_VOID *paddress);

/******************************************************************************

函数名：	MTR_DRV_OS_MemorySet

函数描述：	设置指定的地址的数据

入口参数：	mem		-- 地址指针
			value	-- 设置的值
			bytes	-- 设置的数据长度

返回值：	设置的地址指针

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_MemorySet( PMTR_DRV_OS_VOID mem, MTR_OS_U8 value, MTR_DRV_OS_U32 bytes );

/******************************************************************************

函数名：	MTR_DRV_OS_MemoryCopy

函数描述：	把源地址的数据拷贝到目标地址中

入口参数：	pdst	-- 目标地址指针
			psrc	-- 源地址指针
			bytes	-- 拷贝的数据长度

返回值：	目标地址的地址指针

******************************************************************************/
PMTR_DRV_OS_VOID MTR_DRV_OS_MemoryCopy( PMTR_DRV_OS_VOID pdst, PMTR_DRV_OS_VOID psrc, MTR_DRV_OS_U32 bytes );

/***************************************************************************
函数名：	MTR_DRV_DMX_AllocateSectionChannel
函数描述：	分配channel
入口参数：	UINT32 nFilterNumber -- 该channel最多可分配filter数量
			UINT32 nChannelBufferSize -channel的缓冲区大小
返回值：	分配到的channel ID
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_AllocateSectionChannel(MTRDRV_U32 nFilterNumber,MTRDRV_U32 nChannelBufferSize);

/***************************************************************************
函数名：	MTR_DRV_DMX_FreeChannel
函数描述：	释放channel
入口参数：	UINT32 channelID -- 此channel ID;
返回值：	无
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_FreeChannel(MTRDRV_U32 nChannelId);

/***************************************************************************
函数名：	MTR_DRV_DMX_AllocateFilter
函数描述：	分配filter
入口参数：	UINT32 channelID -- channel ID
			MTRDRV_U32 nFilterSize -- filter的大小
返回值：	分配到的filter ID
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_AllocateFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterSize);

/***************************************************************************
函数名：	MTR_DRV_DMX_FreeFilter
函数描述：	释放过滤器
入口参数：	UINT32 channelID -- 此通道ID;
			UINT32 filterID -- 过滤器ID;
返回值：	无
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_FreeFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId);

/***************************************************************************
函数名：	MTR_DRV_DMX_SetChannelPid
函数描述：	设置通道所过滤的PID
入口参数：	UINT32 channelID -- 此通道ID;
			UINT32 uiPid -- 过滤的PID
返回值：	无
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_SetChannelPid(MTRDRV_U32 nChannelId, MTRDRV_U32 nPid);

/***************************************************************************
函数名：	MTR_DRV_DMX_ControlChannel
函数描述：	设置通道工作状态
入口参数：	UINT32 channelID -- 此通道ID;
			UINT32 channelAction -- 通道工作状态
返回值：	无
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_ControlChannel(MTRDRV_U32 nChannelId,MTR_DRV_DMX_ChannelAction_t channelAction);

/***************************************************************************
函数名：	MTR_DRV_DMX_SetFilter
函数描述：	设置过滤器的过滤条件
入口参数：	UINT32 channelID -- 此通道ID;
			UINT32 filterID -- 过滤器ID;
			MTRDRV_U8 Table_id -- table_id
			MTRDRV_U8 Table_idMask -- table_id掩码
			PINT8U puiMask -- 过滤条件的掩码
			PINT8U puiMatch -- 过滤条件的值
返回值：	0：成功；
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_SetFilter(MTRDRV_U32 nChannelId,MTRDRV_U32 nFilterId,MTRDRV_U8 Table_id,MTRDRV_U8 Table_idMask,
								  MTRDRV_U8* puiMatch ,MTRDRV_U8 *puiMask, MTRDRV_U8 DataLen);

/***************************************************************************
函数名：	MTR_DRV_DMX_RegisterChannelCallback
函数描述：	注册回调函数
入口参数：	UINT32 channelID -- 此通道ID;
			MTRDRV_VOID (* pfnCallBack) -- 回调函数指针
			(回调函数的第二个参数是数据地址，第三个参数是数据长度)
返回值：	无
******************************************************************************/
MTRDRV_U32 MTR_DRV_DMX_RegisterChannelCallback(MTRDRV_U32 nChannelId, MTRDRV_VOID(* pfnCallBack)(MTRDRV_U32 nChannelId,MTRDRV_U8 *data,MTRDRV_U32 len));

/***************************************************************************
函数名：	MTR_DRV_NVRAM_GetFlashAllocateSize
函数描述：	获得广告数据存储区大小
入口参数：	无
返回值：	广告数据存储区大小
******************************************************************************/
MTRDRV_U32 MTR_DRV_NVRAM_GetFlashAllocateSize(MTRDRV_VOID);

/***************************************************************************
函数名：	MTR_DRV_NVRAM_ReadData
函数描述：	读取flash数据
入口参数：	MTRDRV_U32 nvrOffset -- Nvram的偏移量word型
			MTRDRV_U16 *extBuffer -- 外部存储区，用于存储读取的数据
			MTRDRV_U32 size -- 读取数据的长度
返回值：	无
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_ReadData(MTRDRV_U32 nvrOffset, MTRDRV_U8 *extBuffer, MTRDRV_U32 size);

/***************************************************************************
函数名：	MTR_DRV_NVRAM_WriteData
函数描述：	存储数据到flash
入口参数：	MTRDRV_U32 nvrOffset -- Nvram的偏移量word型
			MTRDRV_U16 *extBuffer -- 外部存储区，存储的数据
			 MTRDRV_U32 size -- 存储数据的长度
返回值：	无
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_WriteData(MTRDRV_U32 nvrOffset, MTRDRV_U8 * extBuffer, MTRDRV_U32 size);

/***************************************************************************
函数名：	MTR_DRV_NVRAM_Erase
函数描述：	擦除存储广告数据的flash区
入口参数：	广告图片的长度
返回值：	无
******************************************************************************/
MTRDRV_VOID MTR_DRV_NVRAM_Erase(MTRDRV_U32 AD_length);

/***************************************************************************
函数名：	MTR_AD_SearchOver_Process
函数描述：	广告搜索结束的后续处理，可以恢复正常的开机流程
入口参数：	无
返回值：	无
******************************************************************************/
MTRDRV_VOID MTR_AD_SearchOver_Process(MTRDRV_VOID);

/******************************************************************************

函数名：	TRADReadRectangle

函数描述：	调用该函数将屏幕上指定区域的数据读入指定的缓存中

入口参数：	x	-- 指定区域的横坐标，y	--指定区域的纵坐标 width	--指定区域的宽度
			height	--指定区域的高度 destination存放读出数据的缓存

返回值：	无

******************************************************************************/
MTRDRV_VOID TRADReadRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *destination);

/******************************************************************************

函数名：	TRADWriteRectangle

函数描述：	调用该函数将数据写入屏幕指定区域

入口参数：	x	-- 指定区域的横坐标，y	--指定区域的纵坐标 width	--指定区域的宽度
			height	--指定区域的高度 source要写入指定区域的数据

返回值：	无

******************************************************************************/
MTRDRV_VOID TRADWriteRectangle(MTRDRV_U32 x,MTRDRV_U32 y,MTRDRV_U32 width,MTRDRV_U32 height,MTRDRV_U8 *source);

/******************************************************************************

函数名：	TRAD_Display_Logo

函数描述：	调用该函数显示开机图片

入口参数：	source	--开机画面数据的指针(现在的开机画面不局限于I帧数据了，还有可能是gif数据或者bm数据)，length--数据的长度

返回值：	无

******************************************************************************/
MTRDRV_VOID TRAD_Display_Logo(MTRDRV_U8 *source,MTRDRV_U32 length);

/******************************************************************************

函数名：	MTR_AD_Show_Gif

函数描述：	调用该函数显示gif图片

入口参数：	GifCoordinate	--广告图片的坐标
			GifData-----------gif图片的数据
			gif_length-----------gif图片的长度
			gif_idx和refresh可以直接传0

返回值：	无

******************************************************************************/
MTRDRV_BOOL MTR_AD_Show_Gif(MTR_DRV_OSD_DOTCOORDINATE GifCoordinate, MTRDRV_U8* GifData , MTRDRV_HANDLE gif_length, MTRDRV_U8 gif_idx,MTRDRV_U8 refresh);
#endif
