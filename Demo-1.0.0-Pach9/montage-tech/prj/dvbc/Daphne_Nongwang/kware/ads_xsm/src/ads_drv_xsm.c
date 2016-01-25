/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_timer.h"
#include "charsto.h"

#include "mem_manager.h"
#include "nim.h"
#include "dmx.h"
#include "ads_ware.h"
#include "../inc/ads_api_xsm.h"
#include "sys_cfg.h"
#define ADS_XSM_DRV_PRINTF
//#define ADS_XSM_DRV_DEBUG

#ifdef ADS_XSM_DRV_PRINTF
    #define ADS_DRV_PRINTF OS_PRINTF
#else
    #define ADS_DRV_PRINTF DUMMY_PRINTF
#endif

#ifdef ADS_XSM_DRV_DEBUG
    #define ADS_DRV_DEBUG OS_PRINTF
#else
    #define ADS_DRV_DEBUG DUMMY_PRINTF
#endif

//#define XSM_ADS_TIMEOUT 50000

#define XSM_ADS_TASK_STKSIZE    (32 * KBYTES)
#define XSM_ADS_FLASH_MAP_SIZE  (128 * KBYTES)
#define XSM_ADS_INVALID_CHANNEL_ID   (0xffff)
#define XSM_ADS_DMX_BUF_SIZE  (128 * KBYTES)//(32 * KBYTES + 188)
#define XSM_ADS_FILTER_MAX_NUM    (5)
#define XSM_ADS_QUEUE_MAX_DEPTH (32)
#define XSM_ADS_TASK_NUM  (5)
#define NIT_PID (0x10)
#define PAT_PID (0x0)
#define NIT_TABLE_ID  (0x40)
#define PAT_TABLE_ID  (0x0)
#define PMT_TABLE_ID  (0x02)

#define MAX_YTDY_AD_SEC_NUM (120*2+10)//理论上个数应该是 MAX_PIC_AD_NUM*AD_PER_PIC_MAX_SEC_NUM 考虑到空间限制,+OSD PIC CONFIG
#define YTDY_AD_SEC_NUM     (256)

typedef enum
{
    XSM_ADS_FREE = 0,
    XSM_ADS_USED,
    XSM_ADS_FINISH
}xsm_ads_status_t;

typedef struct _s_xsm_task_struct
{
    u32 task_prio;
    xsm_ads_status_t task_status;
}s_xsm_ads_task_struct_t;

static s_xsm_ads_task_struct_t xsm_ads_task_record[XSM_ADS_TASK_NUM] =
{
    {0, XSM_ADS_FREE},
    {0, XSM_ADS_FREE},
    {0, XSM_ADS_FREE},
    {0, XSM_ADS_FREE},
    {0, XSM_ADS_FREE},
};

#define AD_PIC_CONFIG_PIC_ID (0xfffd)
#define AD_OSD_CONFIG_PIC_ID (0xfffe)
#define AD_OSD_CONTENT_PIC_ID (0xfffc)

typedef struct
{
    dmx_device_t        *p_dev;
    xsm_ads_status_t    filter_status;
    dmx_chanid_t        channel_filter_id;
    u16                 channel_pid;
    u8                  table_id;
    u16                 table_ext_id;
    u8                  adv_received;
    u8                  adv_total_num;
    u8                  adv_section_status[YTDY_AD_SEC_NUM];
    //u8                  adv_version;
    s16                 adv_version;
    u8                  *p_buf;
    u32                 start_time;
    u32                 timeout;
}xsm_ads_filter_struct_t;

static xsm_ads_filter_struct_t xsm_ads_filter_info[XSM_ADS_FILTER_MAX_NUM] = {{0,},};
static u8 xsm_ads_filter_buf[XSM_ADS_FILTER_MAX_NUM][XSM_ADS_DMX_BUF_SIZE] = {{0,},};
//static u8 xsm_ads_section_buf[MAX_YTDY_AD_SEC_NUM][MAX_AD_SECTION_LEN] = {{0,},};
u8 xsm_ads_section_buf[MAX_AD_SECTION_LEN] = {0,};

u16 g_ads_pid = 0xffff;

//extern u8  g_roll_rev;
//extern u8  g_show_fs_ads;


os_sem_t g_ads_xsm_lock = 0;

extern ads_module_priv_t *p_ads_xsm_priv;
extern int dmx_jazz_wait_for_data(int timeout);

#define AD_FINISH_REV 0xff
#define AD_NO_REV     0x00


u8 chAdFlashMap[XSM_ADS_FLASH_MAP_SIZE] = {0,};

static void xsm_ads_lock(void)
{
    u32 ret = 0;

    ret = mtos_sem_take((os_sem_t *)&g_ads_xsm_lock, 0);

    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void xsm_ads_unlock(void)
{
    u32 ret = 0;

    ret = mtos_sem_give((os_sem_t *)&g_ads_xsm_lock);
    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void xsm_ads_filter_stop(u8 index)
{
    ADS_DRV_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    ADS_DRV_PRINTF("Dev = 0x%x  FilterHandle= 0x%x\n",
              xsm_ads_filter_info[index].p_dev,
              xsm_ads_filter_info[index].channel_filter_id);

    u32 ret = 0;

    ret = dmx_chan_stop(xsm_ads_filter_info[index].p_dev,
                        xsm_ads_filter_info[index].channel_filter_id);
    MT_ASSERT(ret == SUCCESS);

    //DRV_PTI_ShowInfo (0);

}

static void xsm_ads_filter_free(u32 index)
{
    ADS_DRV_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    ADS_DRV_PRINTF("Dev = 0x%x  FilterHandle= 0x%x\n",
          xsm_ads_filter_info[index].p_dev,
          xsm_ads_filter_info[index].channel_filter_id);

    u32 ret = 0;

    ret = dmx_chan_stop(xsm_ads_filter_info[index].p_dev,
                        xsm_ads_filter_info[index].channel_filter_id);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(xsm_ads_filter_info[index].p_dev,
                         xsm_ads_filter_info[index].channel_filter_id);
    MT_ASSERT(ret == SUCCESS);
}


static void xsm_ads_filter_start(u32 index)
{
    ADS_DRV_PRINTF("[XSM]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    ADS_DRV_PRINTF("Dev = 0x%x  FilterHandle= 0x%x\n",
          xsm_ads_filter_info[index].p_dev,
          xsm_ads_filter_info[index].channel_filter_id);

    u32 ret = 0;

    ret = dmx_chan_start(xsm_ads_filter_info[index].p_dev,
                         xsm_ads_filter_info[index].channel_filter_id);
    MT_ASSERT(ret == SUCCESS);
}

#if 0
static u32 ads_table_crc[256];


static u32 cal_crc32(u8* buffer, u32 size)
{
    u32 Result = 0xFFFFFFFF;

    while (size--)
    {
        Result = (Result << 8) ^ ads_table_crc[(Result >> 24) ^ *buffer++];
    }

    return Result;
}

static void init_crc_table(void )
{
	u32 i = 0;
	u32 j;
	u32 nData;
	u32 nAccum;

	for ( i = 0; i < 256; i++ )
	{
		nData = ( u32 )( i << 24 );
		nAccum = 0;
		for ( j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x80000000 )
				nAccum = ( nAccum << 1 ) ^ 0x04C11DB7;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		ads_table_crc[i] = nAccum;
	}
}
#endif
 /*!
    ABC
  */
void xsm_ads_add_data(u32 index,  u16 pid, u8 *p_data)
{
  if(index >= XSM_ADS_FILTER_MAX_NUM)
  {
    ADS_DRV_PRINTF("[xsm],xsm_add_data: Too much data, uIndex = %d\n", index);
    return;
  }

  if(NULL == p_data)
  {
    ADS_DRV_PRINTF("[xsm],input param err\n");
    return;
  }

  u8 table_id = p_data[0];
  u16 length = (u16)((p_data[1] & 0x0F)<<8)|p_data[2];
	u8 version = p_data[5] ;
	u16 prog_no = ( p_data[3]  << 8 )| p_data[4];
	u8 cur_sec = p_data[6];
	u8 last_sec = p_data[7];


  /*ADS_DRV_PRINTF("rev index = %d, pid = 0x%x,table_id = 0x%x, prog_no = 0x%x,cur_section = %d,
          last_section = %d, version = 0x%x, length = %d\n",
          index,pid,table_id,prog_no,cur_sec,last_sec,version,length);*/


  if(0 == xsm_ads_filter_info[index].adv_received)
  {

      if(xsm_ads_filter_info[index].table_ext_id == AD_PIC_CONFIG_PIC_ID ||
         xsm_ads_filter_info[index].table_ext_id == AD_OSD_CONFIG_PIC_ID )
      {
          if(xsm_ads_filter_info[index].adv_version == version)
          {
            ADS_DRV_PRINTF("packet version equal = %d\n",version);
            return;
          }
          else
          {
            ADS_DRV_PRINTF("00000000000000table_ext_id = 0x%x, version = %d\n",
                            xsm_ads_filter_info[index].table_ext_id,version);
          }
      }

      xsm_ads_filter_info[index].adv_total_num = last_sec+1;
      xsm_ads_filter_info[index].adv_version = version;

      ADS_DRV_PRINTF("111111111111first section index : %d adv_version : 0x%x,total_num = %d\n",
          index,xsm_ads_filter_info[index].adv_version,xsm_ads_filter_info[index].adv_total_num);

 }

  if(xsm_ads_filter_info[index].adv_section_status[cur_sec] == AD_FINISH_REV)
  {
    ADS_DRV_PRINTF("section had rev\n");
    ADS_DRV_PRINTF("cur_section = %d, last_section = %d, index = %d, pid = 0x%x,table_id = 0x%x, prog_no = 0x%x\n",
                    cur_sec,last_sec,index,pid,table_id,prog_no);
    return;
  }

  if(xsm_ads_filter_info[index].table_id != table_id ||
     xsm_ads_filter_info[index].table_ext_id != prog_no)
  {
    ADS_DRV_PRINTF("index : %d err param rev\n",index);
    return;
  }

  if(xsm_ads_filter_info[index].table_ext_id == AD_PIC_CONFIG_PIC_ID ||
         xsm_ads_filter_info[index].table_ext_id == AD_OSD_CONFIG_PIC_ID )
  {
    if(xsm_ads_filter_info[index].adv_version != version)
    {
      ADS_DRV_PRINTF("index : %d version is not match\n",index);
      ADS_DRV_PRINTF("cur version = %d, other version = %d\n",
                xsm_ads_filter_info[index].adv_version,version);
      return;
    }
  }

#if 0
    u32 n_Sec_crc = cal_crc32(p_data, length - 4);
		u32 n_crc_32;

		n_crc_32 = ((p_data[length - 4]<<24)&0xff000000)
					| ((p_data[length - 4 + 1]<<16)&0x00ff0000)
	    				 | ((p_data[length - 4 + 2]<<8)&0x0000ff00)
	    			 	| ((p_data[length - 4 + 3]<<0)&0x000000ff);

    if(n_Sec_crc != n_crc_32)
    {
      	mtos_printk(" [%d] %x %x crc err\n", cur_sec, n_Sec_crc, n_crc_32);

    }
#endif
    ADS_DRV_PRINTF("rev new section index = %d, pid = 0x%x,table_id = 0x%x, prog_no = 0x%x,cur_section = %d, last_section = %d, version = 0x%x, length = %d\n",
          index,pid,table_id,prog_no,cur_sec,last_sec,version,length);

   //ADS_DRV_PRINTF("index : %d new section = %d rev\n",index,cur_sec);

   xsm_ads_filter_info[index].adv_section_status[cur_sec] = AD_FINISH_REV;

   xsm_ads_filter_info[index].adv_received++;

   if(xsm_ads_filter_info[index].adv_received == xsm_ads_filter_info[index].adv_total_num)
   {

      xsm_ads_filter_info[index].filter_status = XSM_ADS_FINISH;
      ADS_DRV_PRINTF("Cur index:%d,table_pid:0x%x,table_ext_pid = 0x%x rev finish\n",
            index,xsm_ads_filter_info[index].table_id,xsm_ads_filter_info[index].table_ext_id);

      if(xsm_ads_filter_info[index].table_ext_id == AD_PIC_CONFIG_PIC_ID ||
         xsm_ads_filter_info[index].table_ext_id == AD_OSD_CONFIG_PIC_ID )
      {
          ADS_DRV_PRINTF("ads:0x%x finish rev\n",xsm_ads_filter_info[index].table_ext_id);
          xsm_ads_filter_stop(index);
          //memset(xsm_ads_filter_info[index].p_buf,0,XSM_ADS_DMX_BUF_SIZE);
      }
   }


   xsm_ads_unlock();
   YTDY_AD_PrivateDataGot(pid,p_data,length);
   xsm_ads_lock();

}


void YTDY_AD_InitBuffer(YTDY_U32 *Start_adr, YTDY_U32 *Size)
{
    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == Start_adr || NULL == Size )
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    *Start_adr = p_ads_xsm_priv->flash_start_adr;
    *Size = p_ads_xsm_priv->flash_size;


    ADS_DRV_PRINTF("flash addr = 0x%x, size=0x%x\n",
        p_ads_xsm_priv->flash_start_adr,p_ads_xsm_priv->flash_size);


}


void YTDY_AD_ReadBuffer(YTDY_U8* pAdvData,YTDY_U32 *DataLen,YTDY_U32 *DataCRC)
{
    s16 ret = FALSE;
    u32 offset = 0;
    u8 *p_data = chAdFlashMap;
    u32 size = XSM_ADS_FLASH_MAP_SIZE;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pAdvData || NULL == DataLen || NULL == DataCRC)
    {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      MT_ASSERT(0);
    }

    if(NULL != p_ads_xsm_priv->nv_read && size != 0)
    {
      ret = p_ads_xsm_priv->nv_read(offset, p_data, &size);
      if(TRUE != ret)
      {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
      }


      memcpy(DataLen, p_data, 4);

    	if(*DataLen !=0xFFFFFFFF && *DataLen < XSM_ADS_FLASH_MAP_SIZE
          && *DataLen>0)
    	{
    		memcpy(DataCRC ,p_data+4, 4);
    		memcpy(pAdvData ,p_data+8, *DataLen);
    	}
    	else
    	{
    		pAdvData = NULL;
    		*DataLen = 0;
    		*DataCRC = 0;
    	}
    }
    //return ret;
}

//extern void show_logo(void);


void YTDY_AD_WriteBuffer(YTDY_U8* pAdvData,YTDY_U32 DataLen,YTDY_U32 DataCRC)
{

    s32 ret = FALSE;
    u32 offset = 0;
    u8 *p_data = chAdFlashMap;
    //u32 size = XSM_ADS_FLASH_MAP_SIZE;//crc 4bytes

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pAdvData)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    ADS_DRV_PRINTF("[drv], write addr=0x%x, len=%d, crc = 0x%x \n",
            pAdvData,DataLen,DataCRC);

    if(NULL != p_ads_xsm_priv->nv_write && DataLen != 0)
    {
        memcpy(p_data, &DataLen, 4);
        OS_PRINTF("cpy datalen end\n");
        memcpy(p_data+4, &DataCRC, 4);
        OS_PRINTF("cpy crc end\n");
        memcpy(p_data+8, pAdvData, DataLen);
        OS_PRINTF("cpy data end\n");

        ret = p_ads_xsm_priv->nv_erase2(offset,DataLen);
        OS_PRINTF("erase flase end\n");
        ret = p_ads_xsm_priv->nv_write(offset, p_data, DataLen+8);
        OS_PRINTF("write flase end\n");

        if(TRUE != ret)
        {
            ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
            MT_ASSERT(0);
        }
    }


    //show_logo();

    //return ret;
}


void* YTDY_AD_Malloc(YTDY_U32 byBufSize)
{
  void *p_buf = 0;
  u32 size = byBufSize;

  //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
  p_buf = (void *)mtos_malloc(size);

  if(NULL == p_buf)
  {
    MT_ASSERT(0);
  }

  memset(p_buf, 0, size);

  return p_buf;
}

void YTDY_AD_Free(void* pBuf)
{
    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pBuf)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
    mtos_free(pBuf);
    pBuf = NULL;
}

void YTDY_AD_Memset(void* pDestBuf,YTDY_U8 c,YTDY_U32 wSize)
{
  //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
  u8 *p = NULL;
  u8 *q = NULL;
  u8  b_value = 0;
  u8  a_value = 0;

  //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
  if(NULL == pDestBuf)
  {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      MT_ASSERT(0);
  }

  p = (u8 *)(pDestBuf + wSize);
  b_value = *p;
  //ADS_DRV_PRINTF("memset, beofre = 0x%x \n",b_value);
  memset(pDestBuf, c, wSize);
  q = (u8 *)(pDestBuf + wSize);
  a_value = *q;
  //ADS_DRV_PRINTF("memset, after = 0x%x \n",a_value);
  if(b_value != a_value)
  {
    ADS_DRV_PRINTF("pucBuffer = 0x%x \n",pDestBuf);
    MT_ASSERT(0);
  }

}

void YTDY_AD_Memcpy(void* pDestBuf,const void* pSrcBuf,YTDY_U32 wSize)
{
  u8 *p = NULL;
  u8 *q = NULL;
  u8  b_value = 0;
  u8  a_value = 0;

  //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
  if(NULL == pDestBuf || NULL == pSrcBuf)
  {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      MT_ASSERT(0);
  }

  p = (u8 *)(pDestBuf + wSize);
  //ADS_DRV_PRINTF("memcpy, pucDestBuffer[0x%x] , p= 0x%x \n",pucDestBuffer,p);
  //ADS_DRV_PRINTF("memcpy1, 0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x \n",*(u8 *)pucDestBuffer,*(u8 *)(pucDestBuffer+1),
  //*(u8 *)(pucDestBuffer+2),*(u8 *)(pucDestBuffer+3),*(u8 *)(pucDestBuffer+4),*(u8 *)(pucDestBuffer+5),*(u8 *)(pucDestBuffer+6),*(u8 *)(pucDestBuffer+7));
  b_value = *p;
  //ADS_DRV_PRINTF("memcpy, before = 0x%x \n",b_value);
  memcpy(pDestBuf, pSrcBuf, wSize);
  q = (u8 *)(pDestBuf + wSize);
  a_value = *q;
  //ADS_DRV_PRINTF("memcpy, after = 0x%x \n",a_value);
  //ADS_DRV_PRINTF("memcpy2, 0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x \n",*(u8 *)pucDestBuffer,*(u8 *)(pucDestBuffer+1),
  //*(u8 *)(pucDestBuffer+2),*(u8 *)(pucDestBuffer+3),*(u8 *)(pucDestBuffer+4),*(u8 *)(pucDestBuffer+5),*(u8 *)(pucDestBuffer+6),*(u8 *)(pucDestBuffer+7));

  if(b_value != a_value)
  {
    ADS_DRV_PRINTF("pucDestBuffer = 0x%x, pucSourceBuffer = 0x%x \n",pDestBuf,pSrcBuf);
    MT_ASSERT(0);
  }
}

/*semaphore*/
YTDY_AD_ERR_E YTDY_AD_SemaphoreInit(YTDY_AD_Semaphore* pSemaphore ,YTDY_U32 bInitVal)
{
  ADS_DRV_PRINTF("[YTDY]%s:LINE:%d, get in, bInitVal =%d!\n", __FUNCTION__, __LINE__, bInitVal);
  ADS_DRV_PRINTF("pSemaphore addr = 0x%x, value = 0x%x\n",pSemaphore,*pSemaphore);

  if(NULL == pSemaphore)
  {
    MT_ASSERT(0);
  }

  //bInitVal = bInitVal ? 1 : 0;
  if(!mtos_sem_create((os_sem_t *)pSemaphore, bInitVal))
  {
    ADS_DRV_PRINTF("[YTDY]YTDY_AD_SemaphoreInit Failed!\n");
    //MT_ASSERT(0);
    return YTDY_AD_ERR_CREATE_SEM;
  }
  ADS_DRV_PRINTF("pSemaphore addr = 0x%x, value = 0x%x\n",pSemaphore,*pSemaphore);

   return YTDY_AD_SUCCESS;
}

#if 0
void CDSTBCA_SemaphoreInit( CDCA_Semaphore* pSemaphore, CDCA_BOOL bInitVal )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in, bInitVal =%d!\n", __FUNCTION__, __LINE__, bInitVal);

    MT_ASSERT(pSemaphore != NULL);
    bInitVal = bInitVal ? 1 : 0;
    if(!mtos_sem_create((os_sem_t *)pSemaphore, bInitVal))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreInit Failed!\n");
        //MT_ASSERT(0);
    }
}

void CDSTBCA_SemaphoreSignal( CDCA_Semaphore* pSemaphore )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    MT_ASSERT(pSemaphore != NULL);
    if(!mtos_sem_give((os_sem_t *)pSemaphore))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreSignal Failed!\n");
        //MT_ASSERT(0);
    }
}

void CDSTBCA_SemaphoreWait( CDCA_Semaphore* pSemaphore )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    MT_ASSERT(pSemaphore != NULL);
    if(!mtos_sem_take((os_sem_t *)pSemaphore, 0))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreWait Failed!\n");
        //MT_ASSERT(0);
    }
}
#endif

YTDY_AD_ERR_E YTDY_AD_SemaphoreSignal(YTDY_AD_Semaphore pSemaphore )
{
  ADS_DRV_PRINTF("[YTDY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
  ADS_DRV_PRINTF("pSemaphore addr = 0x%x, value = 0x%x\n",&pSemaphore,pSemaphore);
  
  if(!mtos_sem_give((os_sem_t *)&pSemaphore))
  {
      ADS_DRV_PRINTF("[YTDY]YTDY_AD_SemaphoreSignal Failed!\n");
      //MT_ASSERT(0);
      return YTDY_AD_ERR_RELEASE_SEM;
      
  }

  return YTDY_AD_SUCCESS;
}

YTDY_AD_ERR_E YTDY_AD_SemaphoreWait(YTDY_AD_Semaphore pSemaphore )
{
    ADS_DRV_PRINTF("[YTDY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    ADS_DRV_PRINTF("pSemaphore addr = 0x%x, value = 0x%x\n",&pSemaphore,pSemaphore);

    if(!mtos_sem_take((os_sem_t *)&pSemaphore, 0))
    {
        ADS_DRV_PRINTF("[YTDY]YTDY_AD_SemaphoreWait Failed!\n");
        //MT_ASSERT(0);
        return YTDY_AD_ERR_OBTAIN_SEM;
    }
      return YTDY_AD_SUCCESS;

}

void YTDY_AD_TaskSleep( unsigned long ms )
{
  //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
  //ADS_DRV_PRINTF("ms = %d\n",ms);
  mtos_task_sleep(ms);
}


YTDY_AD_ERR_E YTDY_AD_RegisterTask(YTDY_S8* szName, YTDY_U8 byPriority,
	    void* pTaskFun,void* pParam, YTDY_U16 nStackSize)
{
  u32 ret = 0;
  u32 *p_stack = NULL;
  u8 uIndex = 0;

  ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

  if(NULL == szName || NULL == pTaskFun)
  {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      return YTDY_AD_ERR_CREATE_TASK;
  }

  for(uIndex = 0; uIndex < XSM_ADS_TASK_NUM; uIndex++)
  {
    if(XSM_ADS_FREE == xsm_ads_task_record[uIndex].task_status)
    {
      xsm_ads_task_record[uIndex].task_status = XSM_ADS_USED;
      break;
    }
  }

  if(uIndex >= XSM_ADS_TASK_NUM)
  {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      return YTDY_AD_ERR_CREATE_TASK;
   }

  xsm_ads_task_record[uIndex].task_prio =
            p_ads_xsm_priv->task_prio_start + uIndex;

  ADS_DRV_PRINTF("[drv] %s,%d \n",szName,
                               xsm_ads_task_record[uIndex].task_prio);

  p_stack = (u32 *)mtos_malloc(nStackSize);
  if(NULL == p_stack)
  {
    MT_ASSERT(p_stack != NULL);
    return YTDY_AD_ERR_MALLOC;
  }

  ret = mtos_task_create((u8 *)szName,
                         (void *)pTaskFun,
                         NULL,
                         xsm_ads_task_record[uIndex].task_prio,
                         p_stack,
                         nStackSize);
  if(!ret)
  {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      xsm_ads_task_record[uIndex].task_status = XSM_ADS_FREE;
      return YTDY_AD_ERR_CREATE_TASK;
  }

  return YTDY_AD_SUCCESS;
}

void  YTDY_AD_Upcall_Back(YTDY_U16  ad_id, YTDY_U8 show_or_hide, YTDY_U32 res2)
{

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    ADS_DRV_PRINTF("[YTDY_AD_Upcall_Back] ad_id = 0x%x, show_or_hide = %d, res2 = %d\n",
                    ad_id,show_or_hide,res2);

    //u32 ret = 0;
    //ret = p_ads_xsm_priv->ad_state_event(ad_id,show_or_hide);

}

/*Filter Mask for res*/
YTDY_S32  YTDY_AD_Section_req(YTDY_U16 PID, YTDY_U8 TableId,YTDY_U16  ProgNo)
{
    s32 ret = -1;
    dmx_device_t *p_dev = NULL;
    dmx_slot_setting_t slot = {0,};
    dmx_filter_setting_t  filter_param = {0,};
    u32 index = 0;

    ADS_DRV_PRINTF("\n [drv] %s,%d \n",__func__,__LINE__);


    ADS_DRV_PRINTF("PID                 :0x%x \n",PID);
    ADS_DRV_PRINTF("TableId             :0x%x \n",TableId);
    ADS_DRV_PRINTF("ProgNo              :0x%x \n",ProgNo);



    for(index = 0; index <XSM_ADS_FILTER_MAX_NUM; index++)
    {
      if(xsm_ads_filter_info[index].filter_status != XSM_ADS_FREE &&
         xsm_ads_filter_info[index].channel_pid == PID &&
         xsm_ads_filter_info[index].table_id == TableId &&
         xsm_ads_filter_info[index].table_ext_id == ProgNo
        )
      {
          ADS_DRV_PRINTF("[drv] %s %d unlock ,the same filter data ! \n",__func__,__LINE__);
          //xsm_ads_unlock();
          //return -1;
          YTDY_AD_section_cancel(index);
          break;
      }
      else if(xsm_ads_filter_info[index].filter_status == XSM_ADS_FREE)
      {
        ADS_DRV_PRINTF("[drv] get free filter index:%d\n",index);
        break;
      }
    }



    if(index >= XSM_ADS_FILTER_MAX_NUM)
    {
      ADS_DRV_PRINTF("[drv] filter array is full and free all used filter except 0/1\n");

      for(index = 2; index <XSM_ADS_FILTER_MAX_NUM; index++)
      {
        if(xsm_ads_filter_info[index].filter_status != XSM_ADS_FREE)
        {
           YTDY_AD_section_cancel(index);
        }
      }
      //xsm_ads_unlock();
      return -1;
    }

    xsm_ads_lock();
    //set filter
    p_dev = (dmx_device_t *)dev_find_identifier(NULL
          , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);


    MT_ASSERT(NULL != p_dev->p_base);

    xsm_ads_filter_info[index].p_buf = xsm_ads_filter_buf[index];
    memset(xsm_ads_filter_info[index].p_buf,0,XSM_ADS_DMX_BUF_SIZE);

    memset(&slot, 0, sizeof(dmx_slot_setting_t));
    slot.in = DMX_INPUT_EXTERN0;
    slot.pid   = PID;
    slot.type = DMX_CH_TYPE_SECTION;

    ret = dmx_si_chan_open(p_dev, &slot, &xsm_ads_filter_info[index].channel_filter_id);
    if(0 != ret)
    {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      xsm_ads_unlock();
      MT_ASSERT(0);
    }

    ADS_DRV_PRINTF("ret = %d, channel_filter_id = 0x%x\n",
        ret,xsm_ads_filter_info[index].channel_filter_id);


    ret = dmx_si_chan_set_buffer(p_dev,
              xsm_ads_filter_info[index].channel_filter_id,
              xsm_ads_filter_info[index].p_buf,
              XSM_ADS_DMX_BUF_SIZE);
    if(0 != ret)
    {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      xsm_ads_unlock();
      MT_ASSERT(0);
    }

    ADS_DRV_PRINTF("\n data        mask: \n");


    filter_param.value[0] = TableId;
    filter_param.value[1] = ( u8 ) ( ( ProgNo & 0xFF00 ) >> 8 );
    filter_param.value[2] = ( u8 ) ( ProgNo & 0x00FF );

    filter_param.mask[0] = 0xff;
    filter_param.mask[1] = 0xff;
    filter_param.mask[2] = 0xff;

    ADS_DRV_PRINTF("filter param value:0x%x, 0x%x, 0x%x\n",
              filter_param.value[0],filter_param.value[1],filter_param.value[2]);

    filter_param.continuous = TRUE;
    filter_param.en_crc = TRUE;
    filter_param.req_mode = DMX_REQ_MODE_FILTER_LOOPBUFF;
    filter_param.ts_packet_mode = DMX_ONE_MODE;

    ret = dmx_si_chan_set_filter(p_dev,
                          xsm_ads_filter_info[index].channel_filter_id,
                          &filter_param);
    if(0 != ret)
    {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      xsm_ads_unlock();
      MT_ASSERT(0);
    }

    xsm_ads_filter_info[index].p_dev = p_dev;
    xsm_ads_filter_info[index].channel_pid = PID;//pstFilterInfo->usChannelPid;
    xsm_ads_filter_info[index].table_id = TableId;
    xsm_ads_filter_info[index].table_ext_id = ProgNo;
    xsm_ads_filter_info[index].start_time = mtos_ticks_get();
    xsm_ads_filter_info[index].timeout = 1000;//XSM_ADS_TIMEOUT;
    xsm_ads_filter_info[index].filter_status = XSM_ADS_USED;
    xsm_ads_filter_info[index].adv_version = -1;
    
    ret = dmx_chan_start(p_dev,xsm_ads_filter_info[index].channel_filter_id);
    if(0 != ret)
    {
      ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
      xsm_ads_unlock();
      MT_ASSERT(0);
    }


    xsm_ads_unlock();

    ADS_DRV_PRINTF("YTDY_AD_Section_req return index = %d,p_dev = %d,filter handle = 0x%x\n",
            index,xsm_ads_filter_info[index].p_dev,xsm_ads_filter_info[index].channel_filter_id);

    return index;
}


YTDY_U32 YTDY_AD_section_cancel(YTDY_U32 LogicSlot)
{


    ADS_DRV_PRINTF("\n [drv] %s,%d LogicSlot = %d\n",__func__,__LINE__,LogicSlot);


    ADS_DRV_PRINTF("\n [drv] start filter_status = %d\n",xsm_ads_filter_info[LogicSlot].filter_status);


    if( LogicSlot < 2 || xsm_ads_filter_info[LogicSlot].filter_status == XSM_ADS_FREE )
    {
        ADS_DRV_PRINTF("LogicSlot :%d err\n",LogicSlot);
        return 1;
    }

    ADS_DRV_PRINTF("\n [drv] free pid = 0x%x, table_id = 0x%x, table_ext_id = 0x%x\n",
      xsm_ads_filter_info[LogicSlot].channel_pid,xsm_ads_filter_info[LogicSlot].table_id,
      xsm_ads_filter_info[LogicSlot].table_ext_id);

    xsm_ads_lock();

    xsm_ads_filter_free(LogicSlot);


    xsm_ads_filter_info[LogicSlot].filter_status = XSM_ADS_FREE;
    xsm_ads_filter_info[LogicSlot].p_buf = NULL;//static buff,so no need free

    xsm_ads_filter_info[LogicSlot].adv_received = 0;
    xsm_ads_filter_info[LogicSlot].adv_total_num = 0;
    memset(xsm_ads_filter_info[LogicSlot].adv_section_status,0,sizeof(u8)*YTDY_AD_SEC_NUM);

    xsm_ads_unlock();

    return 0;
}


extern s32 dmx_si_chan_get_data_safe(void *p_dev, dmx_chanid_t channel, u32 maxBufLen,
                                       u8 **pp_data, u32 *p_size);

static void xsm_ads_data_monitor(void *p_param)
{
    s32 ret = FALSE;
    u8 index = 0;
    u8 *p_data = xsm_ads_section_buf;
    u32 data_len = 0;
    u32 filter_time = 0;
    //jazz_dmx_config_market_type(1);
    u32 maxBufLen = MAX_AD_SECTION_LEN;
      while(1)
      {

            //dmx_jazz_wait_for_data(20);
            mtos_task_sleep(20);

            xsm_ads_lock();
            for(index = 0; index < XSM_ADS_FILTER_MAX_NUM; index++)
            {
                if(xsm_ads_filter_info[index].filter_status == XSM_ADS_FREE)
                {
                    continue;
                }


#if 1
                ret=dmx_si_chan_get_data_safe(xsm_ads_filter_info[index].p_dev,
                                            xsm_ads_filter_info[index].channel_filter_id,
                                            maxBufLen,&p_data,
                                            &data_len);
#else
                ret=dmx_si_chan_get_data(xsm_ads_filter_info[index].p_dev,
                                            xsm_ads_filter_info[index].channel_filter_id,
                                            &p_data,
                                            &data_len);
#endif
                if(0 != ret)
                {
                    filter_time = mtos_ticks_get() - xsm_ads_filter_info[index].start_time;

                    if(filter_time > xsm_ads_filter_info[index].timeout )
                    {

                      xsm_ads_filter_info[index].start_time = mtos_ticks_get();

                      if(xsm_ads_filter_info[index].table_ext_id == AD_PIC_CONFIG_PIC_ID ||
                          xsm_ads_filter_info[index].table_ext_id == AD_OSD_CONFIG_PIC_ID )
                      {
                        if(xsm_ads_filter_info[index].filter_status == XSM_ADS_FINISH)
                        {
                            ADS_DRV_PRINTF("index:%d table_ext_id:0x%x restart again\n",
                                  index,xsm_ads_filter_info[index].table_ext_id);

                            xsm_ads_filter_info[index].adv_received = 0;
                            xsm_ads_filter_info[index].adv_total_num = 0;
                            memset(xsm_ads_filter_info[index].adv_section_status,0,sizeof(u8)*YTDY_AD_SEC_NUM);
                            xsm_ads_filter_start(index);
                        }
                      }
                    }
                }
                else
                {
                    //memcpy(xsm_ads_section_buf,p_data,data_len);
                    #if 0
                    ADS_DRV_PRINTF("[drv] get data[%d], %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
                        data_len,p_data[0],p_data[1],p_data[2],p_data[3],p_data[4],p_data[5],p_data[6],p_data[7],
                        p_data[8],p_data[9],p_data[10],p_data[11],p_data[12],p_data[13],p_data[14],p_data[15]);
                    #endif

                    xsm_ads_add_data(index,xsm_ads_filter_info[index].channel_pid,p_data);

                    //xsm_ads_filter_info[index].start_time = mtos_ticks_get();
                }
            }
            xsm_ads_unlock();
    }
}


void xsm_ads_client_init(void)
{
    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    s16 ret = FALSE;
    u32 *p_stack = NULL;
    ret = mtos_sem_create((os_sem_t *)&g_ads_xsm_lock,1);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }


    OS_PRINTF("xsm pri:%d %d\n",
               p_ads_xsm_priv->task_prio_start,p_ads_xsm_priv->task_prio_end);

    p_stack = (u32 *)mtos_malloc(XSM_ADS_TASK_STKSIZE);
    MT_ASSERT(p_stack != NULL);

    ret = mtos_task_create((u8 *)"ads_monitor",
                                           (void *)xsm_ads_data_monitor,
                                           NULL,
                                           p_ads_xsm_priv->task_prio_end,//subt_pic_TASK_PRIO_END,//
                                           p_stack,
                                           XSM_ADS_TASK_STKSIZE);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
}


void YTDY_AD_Get_local_time( YTDY_AD_TIME_T *  ad_time)
{

      ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

}


 typedef char    *ck_va_list;
 typedef unsigned int  CK_NATIVE_INT;
 typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
 extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);


void YTDY_AD_Printf(const char *fmt,...)
{
#ifdef ADS_XSM_DRV_DEBUG

    ADS_DRV_PRINTF("#######xsm debug ######\n");

    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, fmt);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)fmt, p_args);

    CK_VA_END(p_args);
    ADS_DRV_PRINTF(printk_buf);
    ADS_DRV_PRINTF("\n");
#endif
}

