#ifndef __YTDY_AD_DEF_H
#define __YTDY_AD_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*-------------------------����ڲ�����---------------------------------*/

#define SUPPORT_YTDY_AD



#ifdef SUPPORT_YTDY_AD

#define SUPPORT_YTDY_PF_AD
#define SUPPORT_YTDY_MENU_AD
#define SUPPORT_YTDY_EPG_AD

#if 0
#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif
#endif

#ifndef NULL
#define NULL				(0)
#endif

#ifndef DEFINED_YTDY_U8
#define DEFINED_YTDY_U8
typedef     unsigned char   YTDY_U8;         //!< 8 bit unsigned integer.
#endif  //DEFINED_YTDY_U8

#ifndef DEFINED_YTDY_U16
#define DEFINED_YTDY_U16
typedef     unsigned short YTDY_U16;        //!< 16 bit unsigned integer.
#endif  //DEFINED_YTDY_U16

#ifndef DEFINED_YTDY_U32
#define DEFINED_YTDY_U32
typedef     unsigned long   YTDY_U32;        //!< 32 bit unsigned integer.
#endif  //DEFINED_YTDY_U32

#ifndef DEFINED_YTDY_S8
#define DEFINED_YTDY_S8
typedef     signed char    YTDY_S8;         //!< 8 bit signed integer.
#endif  //DEFINED_ YTDY_S8

#ifndef DEFINED_YTDY_S16
#define DEFINED_YTDY_S16
typedef     signed short    YTDY_S16;        //!< 16 bit signed integer.
#endif  //DEFINED_YTDY_S16

#ifndef DEFINED_YTDY_S32
#define DEFINED_YTDY_S32
typedef     signed long     YTDY_S32;        //!< 32 bit signed integer.
#endif  //DEFINED_YTDY_S32

typedef unsigned int YTDY_AD_Semaphore;


#define MAX_PIC_AD_NUM (180)
#define MAX_OSD_AD_NUM (200)


#define YTDY_AD_MAX_MASK_LEN ( 8 )

#define MAX_OSD_TEXT_SIZE (256)//128����//YTDY_U8 ����
#define MAX_PIC_AD_SIZE	  (1024*128) //ͼƬ��С������120K + 8Kͷ����Ϣ (S_YTDY_AD_SHOW_DATA_T) 

#define MAX_AD_SECTION_LEN (4096)  

#define AD_PER_PIC_MAX_SEC_NUM (32*4)// һ��ͼƬ���32*4���ӱ�32*4?1024 = 128k //ͼƬ��С+head des < 120k +8k head des

#define AD_PIC_CONFIG_PIC_ID (0xfffd)
#define AD_OSD_CONFIG_PIC_ID (0xfffe)

#define MAX_OPEN_LOGO_NUM (3)
#define MAX_LOGO_SIZE (700*80)

#define SUPPORT_OSD_ROLL_AD


#define _YTDY_AD_DEBUG

#ifdef _YTDY_AD_DEBUG

#define YTDY_AD_ASSERT(e)    	do{\
                            	if(!(e))\
                                YTDY_AD_Printf("\n[YTDY_AD] ASSERT!! %s:%d\n", __FILE__,__LINE__);\
                        	}while(0)

#else

#define YTDY_AD_ASSERT(e)    	do{}while(0)

#endif

#define YTDY_AD_CREAT_SLOT_FAIL (-1)
typedef enum YTDY_AD_ERR
{
	YTDY_AD_SUCCESS=0,
	YTDY_AD_ERR_PIC_TIMEOUT,
	YTDY_AD_ERR_UNKNOWN,
	YTDY_AD_ERR_MALLOC,
	YTDY_AD_ERR_CREATE_TASK,
	YTDY_AD_ERR_CREATE_SEM,
	YTDY_AD_ERR_OBTAIN_SEM,
	YTDY_AD_ERR_RELEASE_SEM,
	YTDY_AD_ERR_CREATE_QUE,
	YTDY_AD_ERR_DELETE_QUE,
	YTDY_AD_ERR_SEND_QUE,
	YTDY_AD_ERR_RECV_QUE,
	YTDY_AD_ERR_TIMEOUT_QUE,
	YTDY_AD_ERR_CREATE_TIMER,
	YTDY_AD_ERR_CONTROL_TIMER,
	YTDY_AD_ERR_RESET_TIMER,
	YTDY_AD_ERR_RESOURCEID_NO_FOUND,
	YTDY_AD_ERR_CUSTOMER_ID,
}YTDY_AD_ERR_E;


typedef struct YTDY_COM_TIME_T
{
    YTDY_U32 year    : 12;
    YTDY_U32 month   : 4;
    YTDY_U32 day     : 5;
    YTDY_U32 hour    : 5;
    YTDY_U32 minute  : 6;
    YTDY_U32 second;
} YTDY_AD_TIME_T;

typedef enum YTDY_AD_SHOW_TYPE
{
	AD_NONE = 0X0000,
	AD_ROLL_PIC = 0x1000,
	AD_OPEN_LOGO = 0x2000,
	AD_PF_INFO = 0x3000,
	AD_FS_RIGHT = 0x4000,
	AD_EPG = 0x5000,
	AD_CH_LIST = 0x6000,
	AD_MAIN_MENU = 0x7000,
	AD_RADIO_LOGO = 0x8000,
	AD_FAV_LIST = 0x9000,
	AD_NVOD = 0xA000,
	AD_VOLUME = 0xB000,
	AD_ROLLING_PIC = 0xC000
}E_YTDY_AD_SHOW_TYPE;


typedef enum YTDY_AD_PIC_FOR
{
	AD_JPG = 'J',
	AD_GIF = 'G',
	AD_BMP = 'B',
	AD_I   = 'I'
}E_YTDY_AD_PIC_FOR;


typedef struct YTDY_AD_SHOW_DATA
{	
	YTDY_U16	type;//E_YTDY_AD_SHOW_TYPE
	YTDY_U16 	showing_time;
	YTDY_U16	delaying_time;
	YTDY_U16	start_x;
	YTDY_U16	start_y;
	YTDY_U16	width;
	YTDY_U16	height;
	YTDY_U32	size;//<120k	
	YTDY_U8   pic_format;//E_YTDY_AD_PIC_FOR  'G':gif, 'B':bmp , 'J':jpeg
	YTDY_U8   res;//
	YTDY_U8   *p_pic_data;
}S_YTDY_AD_SHOW_DATA_T;

typedef struct YTDY_ROLL_Text_AD_DATA
{
	YTDY_U8       m_chAdContent[MAX_OSD_TEXT_SIZE];
	YTDY_U8       m_bigOsdLen;
	YTDY_AD_TIME_T    font_end_time;
	YTDY_U16    start_x;
	YTDY_U16  start_y;
	YTDY_U16  width;
	YTDY_U16	bk_width;
	YTDY_U16	bk_height;
} S_YTDY_OSD_DATA;


/* os define */
#define YTDY_AD_ENABLE_TIMER			1
#define YTDY_AD_DISABLE_TIMER			2 
#define 	YTDY_AD_INFINITE_WAIT			0xFFFFFFFF

typedef YTDY_U32  YTDY_AD_Queue;
typedef YTDY_U32  YTDY_AD_Timer;



/*���º�����Ӧ��ʵ�֣��ṩAD �����*/

extern void  YTDY_AD_Upcall_Back(YTDY_U16  ad_id, YTDY_U8 show_or_hide, YTDY_U32 res2);
/*Filter Mask for res*/
extern YTDY_S32  YTDY_AD_Section_req(YTDY_U16 PID, YTDY_U8 TableId,YTDY_U16  ProgNo);
extern YTDY_U32 YTDY_AD_section_cancel(YTDY_U32 LogicSlot);


extern void* YTDY_AD_Malloc(YTDY_U32 byBufSize);
extern void YTDY_AD_Free(void* pBuf);
extern void YTDY_AD_Memset(void* pDestBuf,YTDY_U8 c,YTDY_U32 wSize);
extern void YTDY_AD_Memcpy(void* pDestBuf,const void* pSrcBuf,YTDY_U32 wSize);
extern void YTDY_AD_Get_local_time( YTDY_AD_TIME_T *  ad_time);
extern void YTDY_AD_Printf(const char *fmt, ...);
YTDY_AD_ERR_E YTDY_AD_RegisterTask(YTDY_S8* szName, YTDY_U8 byPriority,
	    void* pTaskFun,void* pParam, YTDY_U16 nStackSize);
void YTDY_AD_TaskSleep( unsigned long ms );

void YTDY_AD_InitBuffer(YTDY_U32 *Start_adr, YTDY_U32 *Size);
void YTDY_AD_ReadBuffer(YTDY_U8* pAdvData,YTDY_U32 *DataLen,YTDY_U32 *DataCRC);
void YTDY_AD_WriteBuffer(YTDY_U8* pAdvData,YTDY_U32 DataLen,YTDY_U32 DataCRC);


/*���º���AD��ʵ�֣��ṩӦ�õ���*/
extern void YTDY_AD_init( void );
/*����2������������̨ǰ����ã�������ʼ�����õ���OPEN*/
extern void YTDY_AD_Open( void );
extern void YTDY_AD_Close( void );
extern YTDY_U32 YTDY_AD_PrivateDataGot(YTDY_U16 pid,YTDY_U8 *p_data, YTDY_U16 length);
extern YTDY_U32 YTDY_AD_GetPicture(E_YTDY_AD_SHOW_TYPE  show_type,YTDY_U16 ser_id,YTDY_U16 ts_id,S_YTDY_AD_SHOW_DATA_T *s_ad_data);
YTDY_U32 YTDY_AD_Check_Roll_Ad_Show(YTDY_U16 ser_id,YTDY_U16 ts_id);


#endif




/* C++ support */
#ifdef __cplusplus
}
#endif




#endif
