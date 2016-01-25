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
#include "szxcdef.h"
#include "dmx.h"
#include "ads_ware.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"
#include "common.h"

#include "vdec.h"
#include "display.h"
#include "ui_ads_szxc_display.h"


#define ADS_SZXC_DRV_PRINTF
#define ADS_SZXC_DRV_ERROR_PRINTF

#ifdef ADS_SZXC_DRV_PRINTF
    #define ADS_PRINTF OS_PRINTF
#else
    #define ADS_PRINTF DUMMY_PRINTF
#endif

#ifdef ADS_SZXC_DRV_ERROR_PRINTF
    #define ADS_ERROR OS_PRINTF
#else
    #define ADS_ERROR DUMMY_PRINTF
#endif

#define SZXC_ADS_STACK_SIZE ((32) * (KBYTES))   // 32 BIG SMALL?
#define SZXC_AD_FILTER_NUM 31
#define SZXC_AD_MAX_SEM_NUM 16
#define SZXC_AD_MAX_CHANNEL_NUM 4

#define SZXC_AD_INVALID_CHANNEL_ID 0xffff
#define SZXC_AD_DMX_BUF_SIZE ((32) * (KBYTES) + 188)

#define BAT_ADV_DESC		0xfe
#define AD_TYPE_NUM 3

#define AD_HANG_MAXNUM 5
#define AD_TXT_MAXNUM 200

#define SZXC_AD_FLASH_SIZE ((1024) * (KBYTES))

/*!
   ADS channel state
  */
typedef enum szxc_ad_channel_state
{
    SZXC_AD_CHAN_DISABLED = 0,
    SZXC_AD_CHAN_ENABLED,
}szxc_ad_channel_state_t;

/*!
   ADS filter state
  */
typedef enum szxc_ad_filter_state
{
    SZXC_AD_FILTER_DISABLED = 0,
    SZXC_AD_FILTER_ENABLED,
}szxc_ad_filter_state_t;

/*!
   ADS rev state
  */
typedef enum AD_REV_STATUS_E
{
  AD_NO_REV,
  AD_FINISH_REV,
  AD_START_REV,
}AD_REV_STATUS_T;

/*!
   ADS type
  */
typedef enum ADV_TYPE_E
{
  ADV_PIC = 0,
  ADV_CON,
  ADV_TXT,
}ADV_TYPE_T;

typedef enum AD_SZXC_PIC_TYPE_E{
	AD_SZXC_PIC_BMP = 0,
	AD_SZXC_PIC_JPG,
	AD_SZXC_PIC_GIF,
	AD_SZXC_PIC_I_FRAME,
	AD_SZXC_PIC_TXT,
}AD_SZXC_PIC_TYPE_T;

/*!
   ADS filter info
  */
typedef struct{
    dmx_filter_setting_t  alloc_param;
    dmx_chanid_t handle;
    u8 *p_data_buf;
    u8 *p_data_buf_for_free;
    szxc_ad_filter_state_t state;
}szxc_ad_filter_info_t;

/*!
   for topreal channel information store
  */
typedef struct{
    szxc_ad_channel_state_t state;  /* 1: start  0: stop */
    szxc_ad_filter_info_t *filter_info[SZXC_AD_FILTER_NUM];
    dmx_slot_setting_t slot_t;
    u16 max_filter_num;
    u16 max_filter_size;
    u16 actual_filter_num;
    u32 buffer_size;
    u16 chan_pid;
    void (*upcall_func)(u16 nSlotId, u32 nFilterId, u8 *p_data, u32 nlength);
}szxc_ad_channel_info_t;


typedef struct ad_szxc_adv_info_s
{
  /*PIC TYPE ONLY*/
	u32	adv_ori_size;
	u32	adv_crc32;
	u16 adv_seg_len;
	u16 adv_seg_num;
	u8 	adv_com_flag;//:1
	u8  reserved;//:7
	u32	adv_com_size;
	u16 adv_hight;
	u16 adv_width;
	u8  adv_type;
	u16 adv_x;
	u16 adv_y;
	u8  adv_speed;
	u8  adv_font_size;
	u8  adv_location;			//显示位置
	u8  adv_duration_times; //持续时间/滚动次数
	u8  adv_back_color;		//just for txt adver
	u8  adv_font_color;		//just for txt adver
#if 0
  union
  {
    struct
    {
      u32 reserved1;
      u32 reserved2;
      u8  reserved3;
    }adv_pic;

    struct
    {
      u32 reserved1;
      u8  reserved2;
      u8  adv_location;
      u8  adv_duration;
      u8  reserved3;
      u8  reserved4;
    }adv_corner;

    struct
    {
      u32 reserved1;
      u8  adv_roll_speed;
      u8  adv_location;
      u8  adv_times;
      u8  adv_back_color;		//just for txt adver
	    u8  adv_font_color;		//just for txt adver
    }adv_txt;
  }adv_diff;
  u32 adv_revese;
#endif
}ad_szxc_adv_info_t;

#if 0
typedef struct ad_szxc_ctrl_s
{
	u8					adv_id;
	u8  				adv_type;
	u16         service_id;
  struct ad_szxc_ctrl_s *next;
}ad_szxc_ctrl_t;
#endif

typedef struct ad_szxc_service_s
{
  u16 service_id;
  struct ad_szxc_service_s *next;
}ad_szxc_service_t;

typedef struct ad_szxc_ad_s
{
    u8					adv_id;
    u8  				       adv_type;
    ad_szxc_service_t          *adv_service;
    u16                                 adv_received;
    u8                                 *adv_section_status;
    ad_szxc_adv_info_t 	       adv_info;
    u8  				      *start;				//malloc区域的起始地址
    u32					 flash_addr;		//ad[0]的为初始地址，ad[1]的为ad[0]数据长度/4*4+4。类推。
    struct ad_szxc_ad_s       *next;
}ad_szxc_content_t;

typedef struct ad_szxc_rev_flag_s
{
   //控制表是否收全
   u8 adv_ctrl_total_num;
   u8 adv_ctrl_rev_num;
   u8 *adv_ctrl_rev_status;

   u8 *adv_info_rev_status;

   //当前表的所有广告是否搜全
   u8 adv_total_num;
   u8 adv_rev_num;
   u8 adv_finish_flag;
}ad_szxc_rev_flag_t;

typedef struct ad_szxc_s  	//总的信息，version及广告数
{
	ad_szxc_content_t		*ad;
	u8					download_version;
}ad_szxc_t;


os_sem_t g_ads_szxc_lock = 0;
os_sem_t g_ads_txtflag_lock = 0;

extern ads_module_priv_t *p_ads_mod_priv;

szxc_ad_channel_info_t *p_szxc_ad_channel[SZXC_AD_MAX_CHANNEL_NUM];
dmx_device_t *p_szxc_ad_dmx_dev = NULL;

static u8 pic_version = 0xff;
static u8 con_version = 0xff;
static u8 txt_version = 0xff;
static u16 PicAdvPID = 0xffff;
static u16 TxtAdvPID = 0xffff;
static u16 ConAdvPID = 0xffff;
static u16 bat_pid   = 0x11;

ad_szxc_t szxc_ad[AD_TYPE_NUM];//0:pic 1:con 2:txt
ad_szxc_rev_flag_t szxc_rev_flag[AD_TYPE_NUM];

static u8 flash_head_info[4096] = {0};
static u8 flash_head_flag = 0;
static u8 flash_data_change_flag = 0;

static s_szxc_adv_hangpic_show_t szxc_rev_hang[AD_HANG_MAXNUM];
static s_szxc_adv_osd_show_t szxc_rev_txt[AD_TXT_MAXNUM];
static u8 txt_show_pos[4] = {0};
static u8 txt_show_flag= 0;

//extern BOOL is_txt_rolling(void);

#if 1
static void data_dump(u8 *p_addr, u32 size)
{
  u32 i = 0;

  for(i = 0; i < size; i++)
  {
    OS_PRINTF("%02x", p_addr[i]);
    #if 0
    if ((0 == ((i + 1) % 20)) && (i != 0))
    {
        OS_PRINTF("\n");
    }
    #endif
  }
  OS_PRINTF("\n");
}
#endif

static void szxc_ads_lock(void)
{
   s32 ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)&g_ads_szxc_lock, 0);

    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void szxc_ads_unlock(void)
{
    s32 ret = FALSE;

    ret = mtos_sem_give((os_sem_t *)&g_ads_szxc_lock);
    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static u8 *dmx_memory_malloc(u32 buf_size)
{
   u32 buf = 0;
#ifdef  WARRIORS
   buf = (u32)mtos_malloc(buf_size + 7);    //memory align
   hal_dcache_flush_all();
   buf |= 0xa0000000;
#elif ORCHID
   buf = (u32)mtos_malloc(buf_size + 7);    //memory align
#else
   buf = (u32)mtos_align_malloc(buf_size, 16);
#endif

   return (u8 *)buf;
}

static void dmx_memory_free(u8 *addr)
{
#ifdef WARRIORS
    u8 *buf = NULL;
    buf = (u8 *)(((u32)addr) & 0xdfffffff);
    mtos_free(buf);
#elif ORCHID
   mtos_free(addr);
#else
    mtos_align_free(addr);
#endif
}

/*---------------------flash 接口--------------------*/
static RET_CODE szxc_drv_readflash(u32 flashOffset, u8 *extBuffer, u32 size)
{
  RET_CODE ret = ERR_FAILURE;
  u32 size_back = size;
  u32 offset = flashOffset;

  ADS_PRINTF("\n%s %d nvrOffset=%x,size=%x\n",__FUNCTION__, __LINE__,flashOffset,size);

  MT_ASSERT(extBuffer != NULL);
  if(NULL != p_ads_mod_priv->nv_read && size_back != 0)
  {
    ret = p_ads_mod_priv->nv_read(offset, extBuffer, (u32 *)&size_back);
    if(SUCCESS != ret)
    {
        ADS_ERROR("***Read flash data failed.\n");
        return ERR_FAILURE;
     }
  }
//  data_dump(extBuffer,size);
  return  SUCCESS;
}

static RET_CODE szxc_drv_writeflash(u32 flashOffset, u8 * extBuffer, u32 size)
{
    RET_CODE ret = ERR_FAILURE;
    u32 size_back = size;
    u32 offset = flashOffset;

    ADS_PRINTF("%s %d nvrOffset=%x,extBuffer=%x,size=%x\n",
           __FUNCTION__, __LINE__,flashOffset,extBuffer,size);

    MT_ASSERT(extBuffer != NULL);
   // data_dump(extBuffer,size);
    if(NULL != p_ads_mod_priv->nv_write && size_back != 0)
    {
        ret = p_ads_mod_priv->nv_write(offset, extBuffer, size_back);
        if(SUCCESS != ret)
        {
            ADS_ERROR("***Write flash data failed.\n");
            return ERR_FAILURE;
        }
    }
    return SUCCESS;
}

static RET_CODE szxc_drv_erase(u32 AD_length)
{
  RET_CODE ret = ERR_FAILURE;
  u32 elength = AD_length;

  ADS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
  MT_ASSERT(elength != 0);
  if(NULL != p_ads_mod_priv->nv_erase)
  {
    ret = p_ads_mod_priv->nv_erase(elength);
    if(SUCCESS != ret)
    {
        ADS_ERROR("***Erase flash data failed.\n");
        return ERR_FAILURE;
    }
  }
  return SUCCESS;
}

RET_CODE szxc_picdata_to_flash()
{
    RET_CODE ret = ERR_FAILURE;
    ad_szxc_content_t *p_ad = NULL;
    ad_szxc_service_t  *p_service = NULL;
    u8 *flash_data = NULL;
    u8 *dst_data = NULL;
    u8 *src_data = NULL;
    u8 adv_mum = 0;
    u16 flash_info_addr = 256;
    u32 flash_datd_addr = 4*1024;

    if(flash_data_change_flag)
    {
        ADS_PRINTF("adv data not need change\n");
        return SUCCESS;
    }
    p_ad = szxc_ad[ADV_PIC].ad;
    if(p_ad == NULL)
    {
        ADS_ERROR("***no adv data\n");
        return ERR_FAILURE;
    }

    ret = szxc_drv_erase(p_ads_mod_priv->flash_size);
    if(ret != SUCCESS)
    {
        ADS_ERROR("***erase flash wrong\n");
        return ERR_FAILURE;
    }

    flash_data = mtos_malloc(SZXC_AD_FLASH_SIZE);
    if(flash_data == NULL)
    {
        ADS_ERROR("**memory malloc wrong\n");
        return ERR_FAILURE;
    }
    memset(flash_data, 0, SZXC_AD_FLASH_SIZE);

    flash_data[0] = szxc_ad[ADV_PIC].download_version;

    while(p_ad != NULL)
    {
        flash_data[2+adv_mum] = p_ad->adv_type;
        // service id
        flash_data[flash_info_addr+(adv_mum*16) + 0] =
                          ((p_ad->adv_service->service_id >> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 1] =
                          ((p_ad->adv_service->service_id >> 0) & 0xff);
        //hight
        flash_data[flash_info_addr+(adv_mum*16) + 2] =
                          ((p_ad->adv_info.adv_hight >> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 3] =
                          ((p_ad->adv_info.adv_hight >> 0) & 0xff);
        //width
        flash_data[flash_info_addr+(adv_mum*16) + 4] =
                          ((p_ad->adv_info.adv_width >> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 5] =
                          ((p_ad->adv_info.adv_width >> 0) & 0xff);

        //crc32
        flash_data[flash_info_addr+(adv_mum*16) + 6] =
                          ((p_ad->adv_info.adv_crc32 >> 24) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 7] =
                          ((p_ad->adv_info.adv_crc32 >> 16) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 8] =
                          ((p_ad->adv_info.adv_crc32 >> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 9] =
                          ((p_ad->adv_info.adv_crc32 >> 0) & 0xff);

        //lenth
        flash_data[flash_info_addr+(adv_mum*16) + 10] =
                          ((p_ad->adv_info.adv_ori_size >> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 11] =
                          ((p_ad->adv_info.adv_ori_size>> 0) & 0xff);

        // adv_style
        flash_data[flash_info_addr+(adv_mum*16) + 12] = p_ad->adv_info.adv_type;

        //flash_addr
        flash_data[flash_info_addr+(adv_mum*16) + 13] =
                          ((flash_datd_addr>> 16) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 14] =
                          ((flash_datd_addr>> 8) & 0xff);
        flash_data[flash_info_addr+(adv_mum*16) + 15] =
                          (flash_datd_addr & 0xff);

        if(p_ad->adv_type == AD_SZXC_AD_MES)
        {
            p_service = p_ad->adv_service->next;
            while(p_service != NULL)
            {
                 adv_mum = adv_mum + 1;
                 flash_data[2+adv_mum] = p_ad->adv_type;

                 flash_data[flash_info_addr + (adv_mum*16) + 0] =
                                      ((p_service->service_id >> 24) & 0xff);
                 flash_data[flash_info_addr + (adv_mum*16) + 1] =
                                      ((p_service->service_id >> 16) & 0xff);
                 dst_data = &flash_data[flash_info_addr+(adv_mum*16) + 2];
                 src_data = &flash_data[flash_info_addr+(adv_mum*16) - 14 ];
                 memcpy(dst_data, src_data, 14);
                 p_service = p_service ->next;
            }
        }

        if ((SZXC_AD_FLASH_SIZE - flash_datd_addr) >= p_ad->adv_info.adv_ori_size)
       	{
          memcpy(&flash_data[flash_datd_addr], p_ad->start, p_ad->adv_info.adv_ori_size);
        }
	else
	{
            ADS_ERROR("***SZXC_AD_FLASH_SIZE isn't enough: %d, %d",
	      SZXC_AD_FLASH_SIZE - flash_datd_addr, p_ad->adv_info.adv_ori_size);

            mtos_free(flash_data);
            return ERR_FAILURE;
	}

        flash_datd_addr = flash_datd_addr + p_ad->adv_info.adv_ori_size;
        if(flash_datd_addr > p_ads_mod_priv->flash_size)
        {
            ADS_ERROR("***flash is small: 0x%x, 0x%x",
	      flash_datd_addr, p_ads_mod_priv->flash_size);

            mtos_free(flash_data);
            return ERR_FAILURE;
        }

        adv_mum = adv_mum + 1;
        p_ad = p_ad->next;
    }

    flash_data[1] =  adv_mum;

//    data_dump(flash_data,flash_datd_addr);
    ret = szxc_drv_writeflash(0, flash_data, flash_datd_addr);
    if(ret != SUCCESS)
    {
        ADS_ERROR("***write flash wrong");
        mtos_free(flash_data);
        return ERR_FAILURE;
    }

    mtos_free(flash_data);
    return SUCCESS;

}


static u32 szxc_drv_dmx_freefilter(u32 nChannelId,u32 nFilterId)
{
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;

    ADS_PRINTF("%s %d nChannelId=%x,nFilterId=%x\n",__FUNCTION__, __LINE__,nChannelId,nFilterId);

    MT_ASSERT(NULL != p_szxc_ad_dmx_dev);
    MT_ASSERT(NULL != p_szxc_ad_dmx_dev->p_base);

    MT_ASSERT(SZXC_AD_MAX_CHANNEL_NUM > (nChannelId) && SZXC_AD_FILTER_NUM > (nFilterId));
    p_chan = p_szxc_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);

    if(0 >= p_chan->actual_filter_num)
    {
        ADS_ERROR("***[szxc]Filter %d does not exist.\n",nFilterId);
        return 0;
    }
    p_filter = p_chan->filter_info[nFilterId];
    if(NULL == p_filter)
    {
        ADS_PRINTF("Filter [%d] of channel [%d] has been freed.\n", nFilterId, nChannelId);
        return 0;
    }
    if(SZXC_AD_INVALID_CHANNEL_ID != p_filter->handle)
    {
        dmx_chan_close(p_szxc_ad_dmx_dev, p_filter->handle);
        p_filter->handle = SZXC_AD_INVALID_CHANNEL_ID;
    }
    if(NULL != p_filter->p_data_buf)
    {
        dmx_memory_free(p_filter->p_data_buf_for_free);
        p_filter->p_data_buf = NULL;
    }

    mtos_free(p_filter);
    p_chan->filter_info[nFilterId]= NULL;
    p_chan->actual_filter_num--;

    return 0;
}

// slot_pid:  表头 pid
u32 szxc_drv_search_channel_id(u16 Slot_pid)
{
	u16 chan_idx = 0;
	szxc_ad_channel_info_t *p_chan = NULL;

	for(chan_idx = 0; chan_idx < SZXC_AD_MAX_CHANNEL_NUM; chan_idx++)
       {
                p_chan = p_szxc_ad_channel[chan_idx];
		  if(p_chan->chan_pid == Slot_pid)
		  	break;
	}

	if(SZXC_AD_MAX_CHANNEL_NUM <= chan_idx)
    	{
            ADS_ERROR("***can't find pic channel\n");
        	return SZXC_AD_MAX_CHANNEL_NUM;
    	}

	return chan_idx;
}

/*!
 *  This function will allocate a section channel to filter data from mpeg TS.
 *
 *  \param[in] nFilterNumber - Maximum number of the section filters on this channel.
 *  \param[in] nChannelBufferSize - The buffer size of the channel.
 *
 *  Return
 *     The channel number will be returned. It is on identifier of this channel.
 *     The value is from 1 to ADS_DEMUX_INVALID_CHANNEL_ID
 */
u32 szxc_drv_dmx_allocatesectionchannel(u32 nFilterNumber,
                                                     u32 nChannelBufferSize)
{
    u32 chan_idx = 0;
    szxc_ad_channel_info_t *p_chan = NULL;

    for(chan_idx = 0; chan_idx < SZXC_AD_MAX_CHANNEL_NUM; chan_idx++)
    {
        if(NULL == p_szxc_ad_channel[chan_idx])
        {
            ADS_PRINTF("Allocated channel ID is %d.\n Max filter number: %d, Buffer size: %d\n",
                chan_idx, nFilterNumber, nChannelBufferSize);
            break;
         }
    }
    if(SZXC_AD_MAX_CHANNEL_NUM <= chan_idx)
    {
        ADS_ERROR("***All channel are busy, allocate section channel failed.\n");
        return 0;
    }

    p_chan = mtos_malloc(sizeof(szxc_ad_channel_info_t));
    MT_ASSERT(NULL != p_chan);
    memset(p_chan, 0, sizeof(szxc_ad_channel_info_t));

    p_chan->buffer_size = nChannelBufferSize;
    p_chan->max_filter_num = nFilterNumber;
    p_chan->max_filter_size = 0; //Set in MTR_DRV_DMX_AllocateFilter
    p_chan->actual_filter_num = 0;
    p_chan->state = SZXC_AD_CHAN_DISABLED;
    p_chan->upcall_func = NULL;
    p_chan->chan_pid = 0;
    p_szxc_ad_channel[chan_idx] = p_chan;

    ADS_PRINTF("%s %d chan_idx:%x\n", __FUNCTION__, __LINE__, chan_idx);

    return chan_idx;
}

/*!
 * This function will free one section channel. It will be called after calling MTR_DRV_DMX_FreeFilter.
 *
 * Param[in] nChannelId - The idetifier of the channel to be released.
 */
u32 szxc_drv_dmx_freechannel(u16 nSlotId)
{
    u32 ret = 1;
    szxc_ad_channel_info_t *p_chan = NULL;
    u16 filter_idx = 0;
    u32 nChannelId = 0;
    u8 filter_num = 0;

    nChannelId = szxc_drv_search_channel_id(nSlotId);

    ADS_PRINTF("%s %d nChannelId=%x\n",__FUNCTION__, __LINE__,nChannelId);
    MT_ASSERT(SZXC_AD_MAX_CHANNEL_NUM >= nChannelId);
    p_chan = p_szxc_ad_channel[nChannelId];
    szxc_ads_lock();

    if(NULL != p_chan)
    {
        filter_num = p_chan->actual_filter_num;
        for(filter_idx = 0; filter_idx < filter_num; filter_idx++)
        {
            ret = szxc_drv_dmx_freefilter(nChannelId, filter_idx);
        }
        mtos_free(p_chan);
        p_szxc_ad_channel[nChannelId] = NULL;
    }

    szxc_ads_unlock();
    return 0;
}

/*!
 * This function will allocate a section filter on one given channel.
 *
 * \param[in] nChannelId - The channel identifier to be allocate filter.
 * \param[in] nFilterSize - The size of filter.
 *
 * Return
 *    Return the identifier of the filter if it is successfully allocated.
 *    Multiple section filters can be allocated on one channel.
 */
u32 szxc_drv_dmx_allocatefilter(u32 nChannelId,u32 nFilterSize)
{
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;
    u16 filter_idx = 0;

    ADS_PRINTF("%s %d nChannelId=%x,nFilterSize=%x\n",__FUNCTION__, __LINE__,nChannelId,nFilterSize);


    MT_ASSERT(SZXC_AD_MAX_CHANNEL_NUM > nChannelId);
    MT_ASSERT(NULL != p_szxc_ad_channel[nChannelId]);
    p_szxc_ad_channel[nChannelId]->max_filter_size = nFilterSize;
    p_chan = p_szxc_ad_channel[nChannelId];

    while(NULL != p_chan->filter_info[filter_idx])
    {
        filter_idx++;
    }
    MT_ASSERT(SZXC_AD_FILTER_NUM > filter_idx);

    p_filter = mtos_malloc(sizeof(szxc_ad_filter_info_t));
    MT_ASSERT(NULL != p_filter);
    memset(p_filter, 0, sizeof(szxc_ad_filter_info_t));
    p_filter->p_data_buf = NULL;
    p_filter->handle = SZXC_AD_INVALID_CHANNEL_ID;

    p_chan->filter_info[filter_idx] = p_filter;
    p_chan->actual_filter_num++;
    ADS_PRINTF("%s %d filter_idx=%x\n",__FUNCTION__, __LINE__,filter_idx);
    return filter_idx;
}



/*!
 * This function will set PID to a section channel which has been allocated.
 * The channel should be disabled before calling the MTR_DRV_DMX_ControlChannel.
 *
 * Param[in] nChannelId - The identifier of the channel to allocate PID.
 * Param[in] nPid - The PID value.
 *
 */
u32 szxc_drv_dmx_setchannelpid(u32 nChannelId, u32 nPid)
{
    szxc_ad_channel_info_t *p_chan = NULL;
    ADS_PRINTF("%s %d nChannelId=%x,nPid=%x\n",__FUNCTION__, __LINE__,nChannelId,nPid);
    MT_ASSERT(SZXC_AD_MAX_CHANNEL_NUM > nChannelId);
    p_chan = p_szxc_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);

    p_chan->chan_pid = nPid;
    return 0;
}

static RET_CODE szxc_start_filter(u32 nChannelId,u32 nFilterId)
{
    RET_CODE ret = ERR_FAILURE;
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;
    if(SZXC_AD_MAX_CHANNEL_NUM > nChannelId)
        p_chan = p_szxc_ad_channel[nChannelId];
    else
        p_chan = NULL;
    if(p_chan == NULL)
    {
        ADS_ERROR("***channel is null: %d\n", nChannelId);
        return ERR_FAILURE;
    }
    p_filter = p_chan->filter_info[nFilterId];
    if(p_chan == NULL)
    {
        ADS_ERROR("***filter is null: %d\n", nFilterId);
        return ERR_FAILURE;
    }
    if(SZXC_AD_INVALID_CHANNEL_ID != p_filter->handle)
    {
        ret = dmx_si_chan_set_filter(p_szxc_ad_dmx_dev, p_filter->handle, &(p_filter->alloc_param));
              MT_ASSERT(SUCCESS == ret);
        ret = dmx_chan_start(p_szxc_ad_dmx_dev, p_filter->handle);
        if(SUCCESS != ret)
        {
            ADS_ERROR("***stat filter error: %d\n", ret);
            return ERR_FAILURE;
        }
        ADS_PRINTF("start filter %d success:.\n",nFilterId);
    }
    p_filter->state = SZXC_AD_FILTER_ENABLED;
//    p_chan->state = SZXC_AD_CHAN_ENABLED;
    return SUCCESS;
}

static RET_CODE szxc_stop_filter(u32 nChannelId,u32 nFilterId)
{
    RET_CODE ret = ERR_FAILURE;
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;
    if(SZXC_AD_MAX_CHANNEL_NUM > nChannelId)
        p_chan = p_szxc_ad_channel[nChannelId];
    else
        p_chan = NULL;
    if(p_chan == NULL)
    {
        ADS_ERROR("***channel is null: %d\n", nChannelId);
        return ERR_FAILURE;
    }
        p_filter = p_chan->filter_info[nFilterId];
    if(p_chan == NULL)
    {
        ADS_ERROR("***filter is null: %d\n", nFilterId);
        return ERR_FAILURE;
    }
    if(SZXC_AD_INVALID_CHANNEL_ID != p_filter->handle)
    {
        ret = dmx_chan_stop(p_szxc_ad_dmx_dev, p_filter->handle);
        if(SUCCESS != ret)
        {
            ADS_ERROR("***stop filter error: %d\n", ret);
            return ERR_FAILURE;
        }
        ADS_PRINTF("stop filter %d success\n", nFilterId);
    }
    p_filter->state = SZXC_AD_FILTER_DISABLED;
//    p_chan->state =  SZXC_AD_CHAN_DISABLED;
    return SUCCESS;
}

static RET_CODE szxc_set_filter(szxc_ad_filter_info_t *p_filter, u16 pid)
{
    RET_CODE ret = ERR_FAILURE;
    dmx_chanid_t si_channel_id = SZXC_AD_INVALID_CHANNEL_ID;
    dmx_slot_setting_t slot;
    u8 *p_data = NULL;
    u8 *p_buf_align = NULL;

    MT_ASSERT(NULL != p_szxc_ad_dmx_dev);
    MT_ASSERT(NULL != p_szxc_ad_dmx_dev->p_base);
    //ADS_PRINTF("Mask: \n");
    //data_dump(p_filter->alloc_param.mask, DMX_SECTION_FILTER_SIZE);
    //ADS_PRINTF("Data: \n");
    //data_dump(p_filter->alloc_param.value, DMX_SECTION_FILTER_SIZE);
     memset(&slot, 0, sizeof(dmx_slot_setting_t));
     slot.pid   = pid;
     slot.type = DMX_CH_TYPE_SECTION;

     if(SZXC_AD_INVALID_CHANNEL_ID == p_filter->handle)
     {
         ret = dmx_si_chan_open(p_szxc_ad_dmx_dev, &slot, &si_channel_id);
         MT_ASSERT(SUCCESS == ret);
         p_filter->handle = si_channel_id;
     }

     if(NULL == p_filter->p_data_buf)
     {
         p_data = (u8 *)dmx_memory_malloc(SZXC_AD_DMX_BUF_SIZE);
         MT_ASSERT(NULL != p_data);
         p_buf_align = (u8 *)(((u32)p_data + 7) & 0xfffffff8);
         p_filter->p_data_buf = p_buf_align;
         p_filter->p_data_buf_for_free = p_data;
         memset(p_filter->p_data_buf,0,SZXC_AD_DMX_BUF_SIZE);
     }
     ret = dmx_si_chan_set_buffer(p_szxc_ad_dmx_dev, p_filter->handle,
            p_filter->p_data_buf, SZXC_AD_DMX_BUF_SIZE);
     MT_ASSERT(SUCCESS == ret);

     p_filter->alloc_param.continuous = TRUE;
     p_filter->alloc_param.en_crc = TRUE;
     /*for ads it must be CONTINUOUS_SECTION*/
     p_filter->alloc_param.req_mode = DMX_REQ_MODE_SECTION_DOUBLE;

     p_filter->alloc_param.ts_packet_mode = DMX_ONE_MODE;
     ret = dmx_si_chan_set_filter(p_szxc_ad_dmx_dev, p_filter->handle, &(p_filter->alloc_param));
     MT_ASSERT(SUCCESS == ret);
     ADS_PRINTF("%s PID:%x handle:%x\n", __FUNCTION__, pid, p_filter->handle);
     return SUCCESS;
}

static RET_CODE szxc_enable_channel(u32 channelID)
{
     szxc_ad_channel_info_t *p_chan = NULL;
     szxc_ad_filter_info_t *p_filter = NULL;
     u16 filter_idx = 0;
     RET_CODE ret = ERR_FAILURE;

     MT_ASSERT(NULL != p_szxc_ad_dmx_dev);
     MT_ASSERT(NULL != p_szxc_ad_dmx_dev->p_base);
     p_chan = p_szxc_ad_channel[channelID];
     MT_ASSERT(NULL != p_chan);

     for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
     {
         p_filter = p_chan->filter_info[filter_idx];
         szxc_set_filter(p_filter, p_chan->chan_pid);

         MT_ASSERT(NULL != p_filter);
         if(SZXC_AD_INVALID_CHANNEL_ID != p_filter->handle)
         {
             ret = dmx_chan_start(p_szxc_ad_dmx_dev, p_filter->handle);
             MT_ASSERT(SUCCESS == ret);
         }
         p_filter->state = SZXC_AD_FILTER_ENABLED;
     }
    p_chan->state = SZXC_AD_CHAN_ENABLED;
    return SUCCESS;
}




/*!
 * This function will be set filter mask and match bytes of one given filter. The number of
 * mask and match bytes depends on the wMaxFilterSize parameter TRDRV_DEMUX_AllocateSectionChannel.
 *
 * \param[in] nChannelId - The identifier of the channel associated with this filter.
 * \param[in] nFilterId - The identifier of the filter to set mask and match bytes.
 * \param[in] Table_id - The table id.
 * \param[in] Table_idMask - The mask bytes of table id.
 * \param[in] puiMatch - The pointer of the match value bytes.
 * \param[in] puiMask - The pointer of the match mask bytes.
 * \param[in] DataLen - The data length.
 *
 * Return
 *    0 - Successful
 *    1 - Failed
 */
u32 szxc_drv_dmx_setfilter(u32 nChannelId,
                          u32 nFilterId,u8 Table_id,u8 Table_idMask,
                          u8 *puiMatch ,u8 *puiMask, u8 DataLen)
{
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;
    u16 idx = 0;

    MT_ASSERT(SZXC_AD_MAX_CHANNEL_NUM > (nChannelId) && SZXC_AD_FILTER_NUM > (nFilterId));
    MT_ASSERT(NULL != puiMatch && NULL != puiMask);

    p_chan = p_szxc_ad_channel[nChannelId];
    MT_ASSERT(NULL != p_chan);

    if(0 == p_chan->actual_filter_num)
    {
        ADS_PRINTF("Actual allocated filter number is 0.\n");
        return 1;
    }
    p_filter = p_chan->filter_info[nFilterId];
    MT_ASSERT(NULL != p_filter);

    p_filter->alloc_param.mask[0] = Table_idMask;
    p_filter->alloc_param.value[0] = Table_id;

    for(idx = 0; idx < p_chan->max_filter_size; idx++)
    {
        ADS_PRINTF("idx=%d,puiMask=%x,puiMatch=%x\n",idx,puiMask[idx],puiMatch[idx]);
    }

    /* puiMatch[0] --> table[3] means >> 3 */
    for(idx = 0; idx < p_chan->max_filter_size; idx++)
    {
       p_filter->alloc_param.mask[idx + 1] = puiMask[idx];
       p_filter->alloc_param.value[idx + 1] = puiMatch[idx];
    }

    return 0;
}

static RET_CODE szxc_drv_dmx_filter_change(u16 nPid,
                          u32 nFilterId,u8 Table_id,u8 Table_idMask,
                          u8 *puiMatch ,u8 *puiMask, u8 DataLen)
{
    u32 nChannelId = 0;
    RET_CODE ret = ERR_FAILURE;

    nChannelId = szxc_drv_search_channel_id(nPid);
    if(nChannelId == SZXC_AD_MAX_CHANNEL_NUM)
        return ERR_FAILURE;

    ret = szxc_stop_filter(nChannelId, nFilterId);
    if(ret != SUCCESS)
        return ret;

    szxc_drv_dmx_setfilter(nChannelId,
                          nFilterId, Table_id, Table_idMask,
                          puiMatch, puiMask, DataLen);

    ret = szxc_start_filter(nChannelId, nFilterId);
    if(ret != SUCCESS)
        return ret;

    return SUCCESS;
}


/*!
 * This fucntion will associate a given channel with one callback function,
 * which will be called when the channel gets section data. The given channel
 * identifier will be a parameter of the up-call function.
 *
 * Param[in] nChannelId - The channel identifier of the allocated channel that
 *           must register the up-call function.
 * Param[in] pfnCallBack - The function to be called when the channel gets data.
 *
 */
u32 szxc_drv_dmx_registerchannelcallback(u32 nChannelId,
           void (*pfnCallBack)(u16 nSlotId, u32 nFilterId, u8 *data,u32 len))
{
    szxc_ad_channel_info_t *p_chan = NULL;

    ADS_PRINTF("%s %d nChannelId=%x\n",__FUNCTION__, __LINE__,nChannelId);
    if(NULL != pfnCallBack && SZXC_AD_MAX_CHANNEL_NUM > nChannelId)
    {
       p_chan = p_szxc_ad_channel[nChannelId];
       MT_ASSERT(NULL != p_chan);
       p_chan->upcall_func = pfnCallBack;
    }
    return 0;
}

static u8 szxc_parse_qam(u16 value)
{
   u8 n_qam = NIM_MODULA_QAM32;
   switch(value)
   {
        case 16:
           n_qam = NIM_MODULA_QAM16;
           break;
        case 32:
           n_qam = NIM_MODULA_QAM32;
           break;
        case 64:
           n_qam = NIM_MODULA_QAM64;
           break;
        case 128:
           n_qam = NIM_MODULA_QAM128;
           break;
        case 256:
           n_qam = NIM_MODULA_QAM256;
           break;
        default:
           break;
   }
   return n_qam;
}

// 锁住主频点
s8 szxc_drv_tuner_setparams (u32 chanFrequency,
                  u32 chanSymbolRate,u8 qammode)
{
  RET_CODE ret = 0;
  nim_channel_info_t channel_info = {0};
  nim_device_t *p_nim_dev = NULL;

  p_nim_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_nim_dev);

  channel_info.frequency = chanFrequency;
  channel_info.param.dvbc.symbol_rate = chanSymbolRate;
  channel_info.param.dvbc.modulation = szxc_parse_qam(qammode);

  ret = nim_channel_connect(p_nim_dev, &channel_info, FALSE);

  if(SUCCESS != ret);
         ADS_ERROR("***%s %d is_locked:%d\n",__FUNCTION__, __LINE__,channel_info.lock);

  return channel_info.lock;
}

void bat_descriptor_parse( u8 * buf,u16 len,
                            u16 ts_id,u16 bouquet_id)
{
	u8*		data = buf;
	u8		descriptor_tag = 0;
	u8		descriptor_length = 0;
	u16		length = len;


	if( data!=NULL)
  {
    while(length>0)
    {
      descriptor_tag = data[0];
      descriptor_length = data[1];

		  if(descriptor_length == 0&&descriptor_tag == 0)
			  break;

      data += 2;

    	switch( descriptor_tag )
      {
				case BAT_ADV_DESC://0xfe
					PicAdvPID = ((u16)data[3]<<8)|data[4];
					TxtAdvPID = ((u16)data[7]<<8)|data[8];
					ConAdvPID = ((u16)data[11]<<8)|data[12];
					OS_PRINTF("[BAT] PicAdvPID = 0x%x,TxtAdvPID = 0x%x,ConAdvPID = 0x%x\n",
		            			PicAdvPID,TxtAdvPID,ConAdvPID);
					break;
        default:
            break;
      }

      data+=descriptor_length;
      length  -= (2+descriptor_length);
    }
  }
}


//bat表分析，过滤出三种广告的PID
static void szxc_drv_adparser_bat(u16 nSlotId, u32 nFilterId,u8 *section, u32 len)
{
	u8  table_id = 0;
	u8  version_number = 0;
	u8  section_number = 0;
	u8  last_section_number = 0;

	s16 section_length = 0;
	u16 transport_stream_id = 0;
	u16 original_network_id = 0;
	u16 bouquet_id = 0;
	u16 bouquet_descriptors_length = 0;
	u16 transport_stream_loop_length = 0;
	u16 transport_descriptors_length = 0;
	u8  *data = NULL;

	if( section!=NULL )
	{
		data = section;

		table_id = data[0];
		section_length = (((u16)data[1]<<8)|data[2])&0xFFF;
		section_number = data[6];
		last_section_number = data[7];

		OS_PRINTF("[BAT] table_id = 0x%x,section_length = %d,section_num = %d,last_section_number = %d\n",
		            table_id,section_length,section_number,last_section_number);

		#if 0
		{
			u32 i = 0;
			OS_PRINTF("[BAT]section_data = 0x:");
			for( i=0; i<section_length; i++ )
			{
				OS_PRINTF("%x,",data[3+i]);
			}
		}
		OS_PRINTF("\n");
		#endif

		if( section_length>0 )
		{
			bouquet_id = ((u16)data[3]<<8)|data[4];
			version_number = (data[5]&0x3E)>>1;

			bouquet_descriptors_length = ((u16)(data[8]&0x0F)<<8)|data[9];
			section_length -= (bouquet_descriptors_length+8);
			data += 10;
			if(BAT_ADV_DESC == data[0])
			{
				transport_descriptors_length = data[1];
				if (transport_descriptors_length != 0)
				{
			    data += 2;
					PicAdvPID = ((u16)data[2]<<8)|data[3];
					TxtAdvPID = ((u16)data[6]<<8)|data[7];
					ConAdvPID = ((u16)data[10]<<8)|data[11];
					OS_PRINTF("[BAT] PicAdvPID = 0x%x,TxtAdvPID = 0x%x,ConAdvPID = 0x%x\n",
		            			PicAdvPID,TxtAdvPID,ConAdvPID);
				}
			}
			return;
			data += transport_descriptors_length;
			transport_stream_loop_length = (u16)((data[0]&0x0F)<<8 | data[1]);

			OS_PRINTF("[BAT] bouquet_id = 0x%x,version_number =0x%x,bouquet_des_len = 0x%x,transport_stream_loop_length=0x%x\n",
			             bouquet_id,version_number,bouquet_descriptors_length,transport_stream_loop_length);

			data+=2;

			while(transport_stream_loop_length>0)
			{
				transport_stream_id = ((u16)data[0]<<8)|data[1];
				original_network_id = ((u16)data[2]<<8)|data[3];
				transport_descriptors_length = (u16)((data[4]&0x0F)<<8 | data[5]);
				OS_PRINTF("[BAT] tsid = 0x%x, transport_descriptors_length = %d\n",
				transport_stream_id ,transport_descriptors_length);

				data = data + 6;
				if(transport_descriptors_length>0)
				{
					bat_descriptor_parse(	data,
					        				transport_descriptors_length,
					        				transport_stream_id,bouquet_id);
				}

		   	data += transport_descriptors_length;

				transport_stream_loop_length -= (transport_descriptors_length + 6 );
				OS_PRINTF("[BAT]transport_stream_loop_length = 0x%x\n",transport_stream_loop_length);

			}
		}
	}//if( section!=NULL&&data!=NULL )
}


s8 szxc_get_type_bypid(u16 pid)
{
  s8 adv_type = -1;

  if(pid == PicAdvPID)
  {
    adv_type = 0;
  }
  else if(pid == ConAdvPID)
  {
    adv_type = 1;
  }
  else if(pid == TxtAdvPID)
  {
    adv_type = 2;
  }

  return adv_type;
}


u8 szxc_get_not_rev_adv(u8 adv_type)
{
  u8 adv_num =  szxc_rev_flag[adv_type].adv_total_num;
  u8 index = 0;

  OS_PRINTF("szxc_get_not_rev_adv adv_type=%d, adv_num = %d\n",adv_type,adv_num);

  for(index = 1; index <= adv_num; index++)
  {
    OS_PRINTF("index = %d, status = %d\n",index,szxc_rev_flag[adv_type].adv_info_rev_status[index]);\

    if( AD_NO_REV == szxc_rev_flag[adv_type].adv_info_rev_status[index])
    {
      return index;
    }
  }

  return 0;
}

void szxc_data_filter_setup(u16 pid, u32 nFilterId, u8 adv_id)
{
  u8 puiMatch[8] = {0};
  u8 puiMask[8] = {0};

  puiMatch[0] = 0x00;//table_ext_id
	puiMask[0] = 0x00;

	puiMatch[1] = 0x00;
	puiMask[1] = 0x00;

	puiMatch[3] = adv_id;//section_num
	puiMask[3] = 0xff;

  szxc_drv_dmx_filter_change(pid,nFilterId,0xfe,0xff,
                        puiMatch ,puiMask, 7);//过滤系统信息表
}

void szxc_info_filter_setup(u8 adv_type , u32 nFilterId,u16 pid)
{
  //ad_szxc_ctrl_t *adv_ctl = NULL;
  u8 puiMatch[8] = {0};
  u8 puiMask[8] = {0};
  u8 adv_id = 0;

  adv_id = szxc_get_not_rev_adv(adv_type);
  if(adv_id != 0)
  {
    puiMatch[0] = 0x00;//table_ext_id
		puiMask[0] = 0xff;
		puiMatch[1] = 0x00;
		puiMask[1] = 0xff;

		puiMatch[3] = adv_id;//section_num
		puiMask[3] = 0xff;

    szxc_rev_flag[adv_type].adv_info_rev_status[adv_id] = AD_START_REV;
    OS_PRINTF("szxc_info_filter_setup PID = %d, adv_id = %d\n",pid,adv_id);
#if 1
    szxc_drv_dmx_filter_change(pid,nFilterId,0xfe,0xff,
                          puiMatch ,puiMask, 7);//过滤系统信息表
#endif
  }
  else
  {
    OS_PRINTF("szxc no adv need rev\n ");
  }

}

void szxc_add_ctrl(u8 adv_type, u8 adv_ctrl_type, u8 adv_id, u16 service_id)
{
  ad_szxc_content_t		*szxc_adv = NULL;
  ad_szxc_service_t   *szxc_service = NULL;

  if(NULL == szxc_ad[adv_type].ad)
  {
    szxc_ad[adv_type].ad = (ad_szxc_content_t *)calloc(sizeof(ad_szxc_content_t),1);
    szxc_ad[adv_type].ad->adv_id = adv_id;
    szxc_ad[adv_type].ad->adv_type = adv_ctrl_type;
    szxc_ad[adv_type].ad->adv_service = (ad_szxc_service_t *)calloc(sizeof(ad_szxc_service_t),1);
    szxc_ad[adv_type].ad->adv_service->service_id = service_id;
  }
  else
  {
    szxc_adv = szxc_ad[adv_type].ad;
    while( (NULL != szxc_adv->next) && (szxc_adv->next->adv_id != adv_id) )
    {
      szxc_adv = szxc_adv->next;
    }

    if(NULL == szxc_adv->next)//no find cur adv_id adver
    {
      szxc_adv->next = (ad_szxc_content_t *)calloc(sizeof(ad_szxc_content_t),1);
      szxc_adv->next->adv_id = adv_id;
      szxc_adv->next->adv_type = adv_ctrl_type;
      szxc_adv->next->adv_service = (ad_szxc_service_t *)calloc(sizeof(ad_szxc_service_t),1);
      szxc_adv->next->adv_service->service_id = service_id;
    }
    else//find adv_id
    {
      szxc_service = szxc_adv->next->adv_service;

      while( (NULL != szxc_service->next) && (szxc_service->next->service_id != service_id) )
      {
        szxc_service = szxc_service->next;
      }

      if(NULL == szxc_service->next)//no find service_id then add it
      {
        szxc_service->next = (ad_szxc_service_t *)calloc(sizeof(ad_szxc_service_t),1);

        szxc_service->next->service_id = service_id;
      }
    }
  }
}

//图片广告过滤通道释放，因为图片广告是一次性下发的，可以释放
//当广告图片被存入flash后可以被调用
s32 szxc_drv_search_pic_end()
{
	s32 ret = ERR_FAILURE;
	ret = szxc_drv_dmx_freechannel(PicAdvPID);
	if(ret == SUCCESS)
		return SUCCESS;
	else
	{
		ADS_ERROR("pic chnl end error %d \n", PicAdvPID);
		return ret;
	}
}


static void sys_control_section(u8 adv_type,u16 pid, u8 *section)
{
  u8  *data 		= NULL;
  u8	version 	= 0xff;
  u16 table_extension_id 	= 0;
  u32 nFilterIndex = 0;
  u8 last_section_num  = 0;
  u8 adv_total_num = 0;
  u16 desc_len = 0;

  u8  table_id = 0;
  u16 section_length = 0;
  u32 filter_num = 8;

  u8  adv_id = 0;
  u8  adv_ctrl_type = 0;
  u16 service_id = 0;
  u32 index = 0;
  RET_CODE ret = ERR_FAILURE;

  data = (u8 *)section;


  table_id = data[0];
  section_length = (((u16)data[1]<<8)|data[2])&0xFFF;
  table_extension_id = ((u16)data[3] << 8) | data[4]; //segment num
  version = data[5];//用于版本判断，判断广告是否需要更新
  last_section_num = data[7];//total segment num
  adv_total_num = data[8];//adv total num



  OS_PRINTF("[sys_control_section] pid = 0x%x,adv_type = %d,cur segment= 0x%x,section_total_num = %d,adv_total_num = %d\n",
		            pid,adv_type,table_extension_id,last_section_num,adv_total_num);


	#if 0
	{
		u32 i = 0;
		OS_PRINTF("[sys_control_section]section_data = 0x:");
		for( i=0; i<section_length; i++ )
		{
			OS_PRINTF("%x,",data[3+i]);
		}
	}
	OS_PRINTF("\n");
	#endif
  if(adv_type == ADV_PIC)
 {
     ret = szxc_drv_readflash(0, flash_head_info, 4096);
     szxc_ad[adv_type].download_version = flash_head_info[0];
     if(version == szxc_ad[adv_type].download_version)
     {
         flash_head_flag = 1;
         flash_data_change_flag = 1;
         szxc_rev_flag[adv_type].adv_finish_flag = 1;
         szxc_drv_search_pic_end();
         ADS_PRINTF("pic data has saved, don't need receive");
         return;
     }
  }
  else if(version == szxc_ad[adv_type].download_version)
  {
    //不需要更新//stop slot
    return;
  }

  if(0 == szxc_rev_flag[adv_type].adv_ctrl_total_num)
  {
    szxc_rev_flag[adv_type].adv_ctrl_total_num = last_section_num;
    szxc_rev_flag[adv_type].adv_ctrl_rev_status = (u8 *)calloc(sizeof(u8),last_section_num);
  }

  //代码待添加，暂先不考虑保存问题
  if(AD_FINISH_REV == szxc_rev_flag[adv_type].adv_ctrl_rev_status[table_extension_id])
  {
    OS_PRINTF("[sys_control_section]Segment num = %d had rev\n",table_extension_id);
    return;
  }
  else
  {
    szxc_rev_flag[adv_type].adv_ctrl_rev_status[table_extension_id] = AD_FINISH_REV;

    data += 11;

  	desc_len = ((u16)data[1] << 8) | data[2];
  	data += 3;

    OS_PRINTF("[sys_control_section]desc_len = %d\n",desc_len);

    while(desc_len > 3)
  	{

      service_id = ((u16)data[0] << 8) | data[1];
      adv_ctrl_type = data[2];
      adv_id  = data[3];

      szxc_add_ctrl(adv_type, adv_ctrl_type, adv_id, service_id);

      OS_PRINTF("[sys_control_section]service_id = %d, adv_type = %d, adv_id = %d\n",
        ((u16)data[0] << 8) | data[1],data[2],data[3]);

  		desc_len -= 4;
  		data += 4;
  	}
  }

  szxc_rev_flag[adv_type].adv_ctrl_rev_num++;	//adv_link中的总计数目
  szxc_rev_flag[adv_type].adv_total_num = adv_total_num;

  //该类型广告的控制表分段接收完成
  if(szxc_rev_flag[adv_type].adv_ctrl_rev_num == szxc_rev_flag[adv_type].adv_ctrl_total_num)
  {
    //停止过滤当前slot/filter
    //开始过滤接下去的系统信息表和广告数据表
    //广告数量跟系统信息表数量相同
    //szxc_rev_flag[adv_type].adv_info_rev_num = szxc_rev_flag[adv_type].adv_total_num;
    szxc_rev_flag[adv_type].adv_info_rev_status = (u8 *)calloc(sizeof(u8),szxc_rev_flag[adv_type].adv_total_num+1);

    memset(szxc_rev_flag[adv_type].adv_info_rev_status,0,sizeof(u8)*(szxc_rev_flag[adv_type].adv_total_num+1));

    OS_PRINTF("[sys_control_section] rev finish and szxc_info_filter_setup start adv_type = %d,adv_total_num = %d\n",
            adv_type,szxc_rev_flag[adv_type].adv_total_num);

   for(index = 1; index <= szxc_rev_flag[adv_type].adv_total_num; index++)
  {
    OS_PRINTF("index = %d, status = %d\n",index,szxc_rev_flag[adv_type].adv_info_rev_status[index]);

  }

    if(ADV_PIC == adv_type)
    {
      pic_version = version;

      OS_PRINTF("ticket = %d\n",mtos_ticks_get());

      if(szxc_rev_flag[adv_type].adv_total_num < 8)
      {
        filter_num = szxc_rev_flag[adv_type].adv_total_num;
      }

      for(nFilterIndex=0; nFilterIndex<filter_num; nFilterIndex++)
          szxc_info_filter_setup(adv_type,nFilterIndex, pid);//filterid 1234
    }
    else
    {
      if(ADV_CON == adv_type)
      {
        con_version = version;
      }
      else//ADV_TXT
      {
        txt_version = version;
      }

      szxc_info_filter_setup(adv_type,nFilterIndex, pid);
    }
  }
}

void ad_set_sysInfo(ad_szxc_adv_info_t *ad_info, u8  *data)
{
	ad_info->adv_ori_size 	= ((u32)data[0]<<24)|((u32)data[1]<<16)|((u32)data[2]<<8)|((u32)data[3]);
	OS_PRINTF("[ad_set_sysInfo] original size = %d\n", ad_info->adv_ori_size);
	data += 4;
	ad_info->adv_crc32	= ((u32)data[0]<<24)|((u32)data[1]<<16)|((u32)data[2]<<8)|((u32)data[3]);
	OS_PRINTF("[ad_set_sysInfo] crc32 = %d\n", ad_info->adv_crc32);
	data += 4;
	ad_info->adv_seg_len	= ((u16)data[0]<<8)|((u16)data[1]);
	OS_PRINTF("[ad_set_sysInfo] segment len = %d\n",ad_info->adv_seg_len);
	data += 2;
	ad_info->adv_seg_num	= ((u16)data[0]<<8)|((u16)data[1]);
	OS_PRINTF("[ad_set_sysInfo] adv_seg_num = %d\n",ad_info->adv_seg_num);
	data += 2;
	ad_info->adv_com_flag	= data[0]>>7;
	OS_PRINTF("[ad_set_sysInfo] adv_com_flag = %d\n",ad_info->adv_com_flag);
	data += 1;
	ad_info->adv_com_size	= ((u32)data[0]<<24)|((u32)data[1]<<16)|((u32)data[2]<<8)|((u32)data[3]);
	OS_PRINTF("[ad_set_sysInfo] adv_com_size = %d\n",ad_info->adv_com_size);
	data += 4;
	ad_info->adv_hight		= ((u16)data[0]<<8)|((u16)data[1]);
	OS_PRINTF("[ad_set_sysInfo] adv_hight = %d\n",ad_info->adv_hight);
	data += 2;
	ad_info->adv_width	= ((u16)data[0]<<8)|((u16)data[1]);
  	OS_PRINTF("[ad_set_sysInfo] adv_width = %d\n",ad_info->adv_width);
	data += 2;
	ad_info->adv_type		= data[0];
	OS_PRINTF("[ad_set_sysInfo] adv_type = %d\n",ad_info->adv_type);
	data += 1;
	ad_info->adv_x = ((u16)data[0]<<8)|((u16)data[1]);
	OS_PRINTF("[ad_set_sysInfo] adv_x = %d\n",ad_info->adv_x);
	data += 2;
	ad_info->adv_y = ((u16)data[0]<<8)|((u16)data[1]);
	OS_PRINTF("[ad_set_sysInfo] adv_y = %d\n",ad_info->adv_y);
	data += 2;
        ad_info->adv_speed = data[0] & 0x3;
        OS_PRINTF("[ad_set_sysInfo] adv_speed = %d\n",ad_info->adv_speed);
        ad_info->adv_font_size = (data[0] >> 6) & 0x3;
        OS_PRINTF("[ad_set_sysInfo] adv_font_size = %d\n",ad_info->adv_font_size);
	data += 1;
	ad_info->adv_location = data[0];
        OS_PRINTF("[ad_set_sysInfo] adv_location = %d\n",ad_info->adv_location);
	data += 1;
	ad_info->adv_duration_times = data[0];
        OS_PRINTF("[ad_set_sysInfo] adv_duration_time = %d\n",ad_info->adv_duration_times);
	data += 1;
	ad_info->adv_back_color = data[0];
	data += 1;
	ad_info->adv_font_color	= data[0];
}

void print_section(ad_szxc_content_t *p_ad_info)
{
  OS_PRINTF("[sys_information_section] display info\n");
  OS_PRINTF("adv_id = %d\n",p_ad_info->adv_id);
  OS_PRINTF("adv_type = %d\n",p_ad_info->adv_type);
  OS_PRINTF("adv_data_rev_segment = %d\n",p_ad_info->adv_received);
  OS_PRINTF("adv_data_total_segment = %d\n",p_ad_info->adv_info.adv_seg_num);
  OS_PRINTF("adv_dram_addr = 0x%x\n",p_ad_info->start);
}

static void sys_information_section(u8 adv_type, u16 pid, u32 nFilterId, u8 *section)
{

  u8  *data 		= NULL;
  u8 section_num = 0;
  u8 last_section_num = 0;
  ad_szxc_content_t *p_ad = NULL;

  data = (u8 *)section;

  section_num = data[6]; // adv id
  last_section_num = data[7];

  OS_PRINTF("[sys_information_section] adv_type = %d,adv_id = %d nFilterId = %d status = %d rev\n",
            adv_type,section_num,nFilterId,szxc_rev_flag[adv_type].adv_info_rev_status[section_num]);

  if(AD_FINISH_REV == szxc_rev_flag[adv_type].adv_info_rev_status[section_num])
  {
    OS_PRINTF("[sys_control_section]Segment num = %d had rev\n",section_num);
    return;
  }
  else
  {
    szxc_rev_flag[adv_type].adv_info_rev_status[section_num] = AD_FINISH_REV;

    p_ad = szxc_ad[adv_type].ad;
    while(NULL != p_ad)
    {
      if(p_ad->adv_id == section_num)
      {
        break;
      }

      p_ad = p_ad->next;
    }
    if(NULL == p_ad)
        ADS_ERROR("****ERROE, P_AD IS NULL\n");

    ad_set_sysInfo(&p_ad->adv_info, &data[8]);
    p_ad->start = (u8 *)malloc(p_ad->adv_info.adv_com_size);
    p_ad->adv_section_status = (u8 *)calloc(sizeof(u8),(p_ad->adv_info.adv_seg_num + 1));
    memset( p_ad->adv_section_status,0,sizeof(u8)*(p_ad->adv_info.adv_seg_num + 1));
    print_section(p_ad);

    //AD_FINISH_REV
    //接收完当前广告图片的系统信息，重新设置过滤条件，过滤数据信息
    OS_PRINTF("ticket = %d\n",mtos_ticks_get());

    szxc_data_filter_setup(pid,nFilterId,section_num);
  }
}

#if 0
static void szxc_ad_display_I(u8 *source,u32 length)
{

  OS_PRINTF("%s %d length=%x\n",__FUNCTION__, __LINE__,length);

  RET_CODE ret = SUCCESS;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);

  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_DISPLAY);


  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);
  MT_ASSERT(SUCCESS == ret);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)source, length);
  MT_ASSERT(SUCCESS == ret);
  ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  MT_ASSERT(SUCCESS == ret);
}
#endif

static void sys_segment_data_section(u8 adv_type, u16 pid, u32 nFilterIndex, u8 *section)
{
    u8  *data 		= NULL;
    u16 table_extension_id 	= 0;
    u8	section_num 			= 0;
    u8  last_section_num = 0;
    ad_szxc_content_t *p_ad = NULL;
    u32 data_len	= 0;
    u32 nChannelId = 0;

    data = (u8 *)section;

    table_extension_id = ((u16)data[3] << 8) | data[4];//data segment
    section_num 	= data[6];//adv id
    last_section_num = data[7];//
    data += 8;
    data_len = ((u16)data[0]<<8) | data[1];
    data += 2;

    OS_PRINTF("[sys_segment_data_section]adv_id=%d segment = %d\n",section_num,table_extension_id);


    p_ad = szxc_ad[adv_type].ad;

    while(p_ad != NULL)
    {
      if(section_num == p_ad->adv_id)
      {
        break;
      }

      p_ad = p_ad->next;
    }

    if(NULL == p_ad || AD_FINISH_REV == p_ad->adv_section_status[table_extension_id])
    {
        OS_PRINTF("[sys_segment_data_section]adv_id=%d segment = %d data had rev\n",section_num,table_extension_id);
        OS_PRINTF("[sys_segment_data_section]adv_section_status=%d \n",p_ad->adv_section_status[table_extension_id]);
        return;
     }
    else
    {
        p_ad->adv_section_status[table_extension_id] = AD_FINISH_REV;
        memcpy(p_ad->start + 2048*(table_extension_id-1), data, data_len);
        p_ad->adv_received++;

        OS_PRINTF("[sys_segment_data_section] rev_num = %d, total_num = %d\n",
              p_ad->adv_received,p_ad->adv_info.adv_seg_num);
    }


    if(p_ad->adv_received == p_ad->adv_info.adv_seg_num)
    {

        //当前类型广告图片的其中一张接收完毕
        OS_PRINTF("-----------[sys_segment_data_section]adv_id = %d pic rev finish--------\n",section_num);
        OS_PRINTF("ticket = %d\n",mtos_ticks_get());
        nChannelId = szxc_drv_search_channel_id(pid);
        szxc_stop_filter(nChannelId, nFilterIndex);

#if 0
    if(p_ad->adv_info.adv_type == 1)
    {
      OS_PRINTF("---------display jpg------\n");
      ui_adv_szxc_display(p_ad->adv_info.adv_type ,p_ad->start);
      while(1);
    }
#endif

        szxc_rev_flag[adv_type].adv_rev_num++;

        if(szxc_rev_flag[adv_type].adv_rev_num == szxc_rev_flag[adv_type].adv_total_num)
        {
            OS_PRINTF("++++++[sys_segment_data_section]cur pid=%d pic rev finish+++++++\n",pid);
            OS_PRINTF("ticket = %d\n",mtos_ticks_get());

            //当前PID下的所有广告接收完毕
            //接着过滤其余类型PID的广告
            szxc_rev_flag[adv_type].adv_finish_flag = 1;//

            if(ADV_PIC == adv_type)
            {
               szxc_ad[adv_type].download_version = pic_version;
               szxc_drv_search_pic_end();

               /*p_ads_mod_priv->nv_write(p_ads_mod_priv->flash_start_adr,
                          (UINT8 *)pData, nLen);*/


            }
            else if(ADV_CON == adv_type)
            {
               szxc_ad[adv_type].download_version = con_version;
            }
            else//ADV_TXT
            {
               szxc_ad[adv_type].download_version = txt_version;
            }
          }
          else
          {
              szxc_info_filter_setup(adv_type, nFilterIndex, pid);
          }
      }

}



static void szxc_drv_adparser(u16 pid, u32 nFilterId, u8 *data, u32 len)
{
  u8 *data_buffer = NULL;
  u8 table_id = data[0];
  u16 table_extension_id 	= ((u16)data[3] << 8) | data[4];	//table_extension_id
  u8	section_num			= data[6];
  s8 adv_type = -1;

  MT_ASSERT(len >  8);

  adv_type = szxc_get_type_bypid(pid);

  data_buffer = mtos_malloc(len);
  MT_ASSERT(data_buffer != NULL);
  memset(data_buffer, 0, len);
  memcpy(data_buffer, data, len);

  if(adv_type >= 0)
  {
    switch(table_id)
    {
      case 0xfe:
        {
          if(0 == section_num)
          {
          	sys_control_section(adv_type,pid,data_buffer);
          }
          else if(0 == table_extension_id)
          {
          	sys_information_section(adv_type,pid,nFilterId,data_buffer);
          }
          else
          {
            sys_segment_data_section(adv_type,pid,nFilterId,data_buffer);
          }
        }
        break;
       default:
         break;
     }
  }

  mtos_free(data_buffer);
}

//判断是否过滤到了图片广告的PID
s32 szxc_drv_check_pic_pid()
{
	if(PicAdvPID != 0xffff || TxtAdvPID != 0xffff || ConAdvPID != 0xffff)
		return SUCCESS;
	else
		 return ERR_FAILURE;
}


u8 szxc_drv_check_pic_finish()
{
  if(szxc_rev_flag[ADV_PIC].adv_finish_flag )
  {
    OS_PRINTF("adv pic rev finish\n");
    return 1;
  }
  else
  {
    return 0;
  }
}

// 分配过滤通道
void szxc_drv_start_search(u32 PID )
{
  u32 chnidx = 0;
  u32 filteridx = 0;
  u8 Table_id = 0xfe;
  u8 Table_idMask = 0xff;
  u8 puiMatch[8] = {0};
  u8 puiMask[8] = {0};
  u8 index = 4;

   ADS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);

  puiMatch[0] = 0x00;
  puiMask[0] = 0x00;
  puiMatch[1] = 0x00;
  puiMask[1] = 0x00;//table_ext_id != 0

  puiMatch[3] = 0x00;
  puiMask[3] = 0xff;


  if(PID == PicAdvPID)
  {
      chnidx = szxc_drv_dmx_allocatesectionchannel(8,4096);
      if(chnidx >= SZXC_AD_MAX_CHANNEL_NUM)
      {
            ADS_PRINTF("chnl allocate error %d \n", chnidx);
            return ;
      }

      for(index = 0; index < 8; index++)
      {
           filteridx = szxc_drv_dmx_allocatefilter(chnidx,8);
          if(filteridx >= SZXC_AD_FILTER_NUM)
          {
                ADS_ERROR("***filteridx allocate error %d \n", filteridx);
                return ;
          }
          szxc_drv_dmx_setfilter(chnidx,filteridx,Table_id,Table_idMask,
                                        puiMatch ,puiMask, 7);
      }
    }
  else
  {
      chnidx = szxc_drv_dmx_allocatesectionchannel(1,4096);
      if(chnidx >= SZXC_AD_MAX_CHANNEL_NUM)
      {
            ADS_PRINTF("chnl allocate error %d \n", chnidx);
            return ;
      }

      filteridx = szxc_drv_dmx_allocatefilter(chnidx,4);
      if(filteridx >= SZXC_AD_FILTER_NUM)
      {
            ADS_ERROR("***filteridx allocate error %d \n", filteridx);
            return ;
      }
      szxc_drv_dmx_setfilter(chnidx,filteridx,Table_id,Table_idMask,
                                        puiMatch ,puiMask, 7);
  }

  szxc_drv_dmx_registerchannelcallback(chnidx , szxc_drv_adparser);

  szxc_drv_dmx_setchannelpid(chnidx,PID);

  szxc_enable_channel(chnidx);
  OS_PRINTF("ticket = %d\n",mtos_ticks_get());

  return;
}



RET_CODE szxc_drv_search_bat_end()
{
    RET_CODE ret = ERR_FAILURE;

    OS_PRINTF("Call szxc_drv_search_bat_end\n");

    ret = szxc_drv_dmx_freechannel(bat_pid);
    if(ret == SUCCESS)
        return SUCCESS;
    else
    {
        ADS_ERROR("***pat chnl end error %d \n", bat_pid);
        return ret;
    }
}

// 分配挂角广告过滤通道
void szxc_drv_start_search_txt()
{
	szxc_drv_start_search(TxtAdvPID);
}

// 分配挂角广告过滤通道
void szxc_drv_start_search_hang()
{
	szxc_drv_start_search(ConAdvPID);
}

// 分配图片广告过滤通道
void szxc_drv_start_search_pic()
{
  memset(szxc_ad,0,sizeof(ad_szxc_t)*AD_TYPE_NUM);
  memset(szxc_rev_flag,0,sizeof(ad_szxc_rev_flag_t)*AD_TYPE_NUM);
  szxc_drv_start_search(PicAdvPID);

}




// 分配通道，过滤bat表
void szxc_drv_start_search_bat()
{

  u32 chnidx = 0;
  u32 filteridx = 0;
  u8 Table_id = 0x4a;
  u8 Table_idMask = 0xff;
  u8 puiMatch[8] = {0};
  u8 puiMask[8] = {0};

   ADS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);

  chnidx = szxc_drv_dmx_allocatesectionchannel(1,8196);              //是否太大，需注意
  if(chnidx >= SZXC_AD_MAX_CHANNEL_NUM)
  {
    ADS_ERROR("chnl allocate error %d \n", chnidx);
    return ;
  }

  filteridx = szxc_drv_dmx_allocatefilter(chnidx,4);
  if(filteridx >= SZXC_AD_FILTER_NUM)
  {
    ADS_ERROR("filteridx allocate error %d \n", filteridx);
    return ;
  }

  szxc_drv_dmx_setfilter(chnidx,filteridx,Table_id,Table_idMask,
                                    puiMatch ,puiMask, 8);

  szxc_drv_dmx_registerchannelcallback(chnidx , szxc_drv_adparser_bat);

  szxc_drv_dmx_setchannelpid(chnidx,bat_pid);

  szxc_enable_channel(chnidx);

  return;
}

RET_CODE szxc_drv_search_pat_end()
{
    RET_CODE ret = ERR_FAILURE;

    ret = szxc_drv_dmx_freechannel(bat_pid);
    if(ret == SUCCESS)
        return SUCCESS;
    else
    {
        ADS_ERROR("***pat chnl end error %d \n", bat_pid);
        return ret;
    }
}

extern int dmx_jazz_wait_for_data(int timeout);
// 任务监控，实时监测数据
static void szxc_ads_data_monitor(void *p_param)
{
    szxc_ad_channel_info_t *p_chan = NULL;
    szxc_ad_filter_info_t *p_filter = NULL;
    u8 *p_data = NULL;
    u32 data_len = 0;
    u16 chan_idx = 0;
    u16 filter_idx = 0;
    s16 ret = 0;

    while(1)
    {
    //  dmx_jazz_wait_for_data(20);
      mtos_task_sleep(20);
      szxc_ads_lock();

      for(chan_idx = 0; chan_idx < SZXC_AD_MAX_CHANNEL_NUM; chan_idx++)
      {
        p_chan = p_szxc_ad_channel[chan_idx];
        if(NULL == p_chan || SZXC_AD_CHAN_DISABLED == p_chan->state)
        	continue;

        for(filter_idx = 0; filter_idx < p_chan->actual_filter_num; filter_idx++)
        {
          p_data = NULL;
          p_filter = p_chan->filter_info[filter_idx];
          //MT_ASSERT(NULL != p_filter);
          if(NULL == p_filter)
            continue;

          if(p_filter->state == SZXC_AD_FILTER_DISABLED)
              continue;

          ret = dmx_si_chan_get_data(p_szxc_ad_dmx_dev, p_filter->handle, &p_data, &data_len);

          if(SUCCESS == ret)
          {
            if(NULL != p_chan->upcall_func && data_len > 0 && p_data != NULL)
            {
               ADS_PRINTF("%s %d, get data \n",__FUNCTION__, __LINE__);
               szxc_ads_unlock();
               p_chan->upcall_func(p_chan->chan_pid, filter_idx, p_data, data_len);
               szxc_ads_lock();
            }
          }
        }
      }

      szxc_ads_unlock();
    }
}

/*----------------------------hang pic --------------------------*/
static void szxc_free_hang_data()
{
    u8 index = 0;

    for(index = 0; index < AD_HANG_MAXNUM; index++)
    {
        if(szxc_rev_hang[index].pucPicData != NULL)
            mtos_free(szxc_rev_hang[index].pucPicData);
    }
}

extern void ui_hang_pic_orientation(u16 x, u16 y, s_szxc_adv_hangpic_show_t *pic_show_info);
static void szxc_copy_hang_data()
{
    u8 index = 0;
    ad_szxc_content_t *p_ad = NULL;

    p_ad = szxc_ad[ADV_CON].ad;
    if(p_ad == NULL)
    {
        ADS_ERROR("***no adv data\n");
        return ;
    }

    for(index = 0; index < AD_HANG_MAXNUM; index++)
    {
        if (p_ad->adv_info.adv_ori_size)
        {
          szxc_rev_hang[index].usShowFlag = 0;
          szxc_rev_hang[index].usShowOver = 0;
          szxc_rev_hang[index].enPicType = p_ad->adv_info.adv_type;
          szxc_rev_hang[index].usHeigth = p_ad->adv_info.adv_hight;
          szxc_rev_hang[index].usWidth = p_ad->adv_info.adv_width;
          szxc_rev_hang[index].usStartX = p_ad->adv_info.adv_x;
          szxc_rev_hang[index].usStartY = p_ad->adv_info.adv_y;
    	//szxc_rev_hang[index].enDisplayPos = p_ad->adv_info.adv_location;
          ui_hang_pic_orientation(p_ad->adv_info.adv_x, p_ad->adv_info.adv_y, &szxc_rev_hang[index]);
          szxc_rev_hang[index].usShowMinute = p_ad->adv_info.adv_duration_times;
          szxc_rev_hang[index].uiDataLen = p_ad->adv_info.adv_ori_size;
          szxc_rev_hang[index].pucPicData = (u8*)mtos_malloc(szxc_rev_hang[index].uiDataLen);
          MT_ASSERT(NULL != szxc_rev_hang[index].pucPicData);
          memset(szxc_rev_hang[index].pucPicData, 0, szxc_rev_hang[index].uiDataLen);
          memcpy(szxc_rev_hang[index].pucPicData, p_ad->start, szxc_rev_hang[index].uiDataLen);

          ADS_PRINTF("GET HANG PIC IFO: %d\n", index);
          ADS_PRINTF("adv type: %d\n", szxc_rev_hang[index].enPicType);
          ADS_PRINTF("adv hight: %d\n", szxc_rev_hang[index].usHeigth);
          ADS_PRINTF("adv width: %d\n", szxc_rev_hang[index].usWidth);
          ADS_PRINTF("adv startX: %d\n", szxc_rev_hang[index].usStartX);
          ADS_PRINTF("adv startY: %d\n", szxc_rev_hang[index].usStartY);
          ADS_PRINTF("adv pos: %d\n", szxc_rev_hang[index].enDisplayPos);
          ADS_PRINTF("adv time: %d\n", szxc_rev_hang[index].usShowMinute);
          ADS_PRINTF("adv len: %d\n", szxc_rev_hang[index].uiDataLen);
        }
        p_ad = p_ad->next;
        if(p_ad == NULL)
            break;
    }

    if(index < AD_HANG_MAXNUM)
        ADS_PRINTF("GET HANG PIC TOTLE: %d\n", index);
    else
        ADS_ERROR("***hang pic buffer is too few\n");

}

static void szxc_ads_hangpic_monitor()
{
    static u8 receive_hang_flag = 0;
    static u8 receive_hang_version = 0;
    static u8 receive_hang[4] = {0};
    static u32 show_hang_time[4] = {0};
    u8 index = 0;
    u8 hang_pos = 0;
    u32 time_tmp = 0;
    RET_CODE ret = ERR_FAILURE;

    OS_PRINTF("hangpic 11111\n");

    while(1)
    {
         if(receive_hang_flag)
         {
             if(receive_hang_version != szxc_ad[ADV_CON].download_version)
             {
                  ADS_PRINTF("GET HANG PIC DATA FIRST AGAIN\n");
                  szxc_free_hang_data();
                  szxc_copy_hang_data();
                  for(index = 0; index < 4; index++)
                  {
                      if(receive_hang[index])
                      {
                          //关闭图片
                          receive_hang[index] = 0;
                          show_hang_time[index] = 0;
                      }
                  }
             }
         }
         else
         {
             if(szxc_rev_flag[ADV_CON].adv_finish_flag)
             {
                 ADS_PRINTF("GET HANG PIC DATA FIRST\n");
                 receive_hang_flag = 1;
                 szxc_copy_hang_data();
                 receive_hang_version = szxc_ad[ADV_CON].download_version;
             }
         }

        if(receive_hang_flag)
        {
            for(index = 0; index < AD_HANG_MAXNUM; index++)
            {
                if(szxc_rev_hang[index].usShowOver == 0 && szxc_rev_hang[index].pucPicData != NULL)
                {
                    hang_pos = szxc_rev_hang[index].enDisplayPos;
                    if(szxc_rev_hang[index].usShowFlag) // showing
                    {
                        time_tmp = (mtos_ticks_get()) -show_hang_time[hang_pos-1];
                        if(time_tmp >= (szxc_rev_hang[index].usShowMinute * 60 *100))  //show over
                        {
                            receive_hang[hang_pos -1] = 0;
                            szxc_rev_hang[index].usShowOver = 1;
                            //调用显示关闭接口
                             ADS_PRINTF("close hang pic pos: %d\n", hang_pos);
                            p_ads_mod_priv->hide_ad(ADS_AD_TYPE_SUBTITLE, (u8 *)&szxc_rev_hang[index]);
                        }
                    }
                    else  //  have not show
                    {
                        ret = p_ads_mod_priv->get_screen_statu(1);
                        if(receive_hang[hang_pos-1] == 0 && ret == SUCCESS)
                        {
                            //调用显示图片接口
                            ADS_PRINTF("start show hang pic pos: %d\n", hang_pos);
                            show_hang_time[hang_pos-1] = mtos_ticks_get();
                            p_ads_mod_priv->display_ad(ADS_AD_TYPE_SUBTITLE, (u8 *)&szxc_rev_hang[index]);
                            szxc_rev_hang[index].usShowFlag = 1;
                            receive_hang[hang_pos-1] = 1;
                        }
                    }
                }
            }
         }
        mtos_task_sleep(20);
     }
}

/*------------------------------txt pic--------------------------*/
static void szxc_txt_lock(void)
{
   s32 ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)&g_ads_szxc_lock, 0);

    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void szxc_txt_unlock(void)
{
    s32 ret = FALSE;

    ret = mtos_sem_give((os_sem_t *)&g_ads_szxc_lock);
    if (!ret)
    {
        MT_ASSERT(0);
    }
}

void szxc_clear_txt_flag(u8 pos_txt)
{
    szxc_txt_lock();
    txt_show_pos[pos_txt - 1] = 0;
    txt_show_flag = 0;
    szxc_txt_unlock();
}

static void szxc_free_txt_data()
{
    u8 index = 0;

    for(index = 0; index < AD_TXT_MAXNUM; index++)
    {
        if(szxc_rev_txt[index].pucDisplayBuffer!= NULL)
            mtos_free(szxc_rev_txt[index].pucDisplayBuffer);
    }
}

static void szxc_copy_txt_data()
{
    u8 index = 0;
    ad_szxc_content_t *p_ad = NULL;

    p_ad = szxc_ad[ADV_TXT].ad;
    if(p_ad == NULL)
    {
        ADS_ERROR("***no txt data\n");
        return ;
    }

    szxc_txt_lock();
    for(index = 0; index < AD_TXT_MAXNUM; index++)
    {
        if (p_ad->adv_info.adv_ori_size)
        {
          szxc_rev_txt[index].ucShowFlag = 0;
          szxc_rev_txt[index].ucShowTimes= p_ad->adv_info.adv_duration_times;
          szxc_rev_txt[index].enDisplayPos= p_ad->adv_info.adv_location;
          szxc_rev_txt[index].stDisplayRect.right = p_ad->adv_info.adv_hight;
          szxc_rev_txt[index].stDisplayRect.top = p_ad->adv_info.adv_width;
          szxc_rev_txt[index].stDisplayRect.left = p_ad->adv_info.adv_x;
          szxc_rev_txt[index].stDisplayRect.bottom = p_ad->adv_info.adv_y;
          szxc_rev_txt[index].enSpeed = p_ad->adv_info.adv_speed;
          szxc_rev_txt[index].enFontSize = p_ad->adv_info.adv_font_size;
          szxc_rev_txt[index].enBackgroundColor = p_ad->adv_info.adv_back_color;
          szxc_rev_txt[index].enFontColor = p_ad->adv_info.adv_font_color;
          szxc_rev_txt[index].usGetBufferSize = p_ad->adv_info.adv_ori_size;
          szxc_rev_txt[index].pucDisplayBuffer = (u8*)mtos_malloc(szxc_rev_txt[index].usGetBufferSize);
          MT_ASSERT(NULL != szxc_rev_txt[index].pucDisplayBuffer);
          memset(szxc_rev_txt[index].pucDisplayBuffer, 0, szxc_rev_txt[index].usGetBufferSize);
          memcpy(szxc_rev_txt[index].pucDisplayBuffer, p_ad->start, szxc_rev_txt[index].usGetBufferSize);

          ADS_PRINTF("GET TXT PIC IFO: %d\n", index);
          ADS_PRINTF("txt times: %d\n", szxc_rev_txt[index].ucShowTimes);
          ADS_PRINTF("txt Pos: %d\n", szxc_rev_txt[index].enDisplayPos);
          ADS_PRINTF("txt Left: %d\n", szxc_rev_txt[index].stDisplayRect.left);
          ADS_PRINTF("txt Top: %d\n", szxc_rev_txt[index].stDisplayRect.top);
          ADS_PRINTF("txt Right: %d\n", szxc_rev_txt[index].stDisplayRect.right);
          ADS_PRINTF("txt Bottom: %d\n", szxc_rev_txt[index].stDisplayRect.bottom);
          ADS_PRINTF("txt speed: %d\n", szxc_rev_txt[index].enSpeed);
          ADS_PRINTF("txt font size: %d\n", szxc_rev_txt[index].enFontSize);
          ADS_PRINTF("txt BColor: %d\n", szxc_rev_txt[index].enBackgroundColor);
          ADS_PRINTF("txt FColor: %d\n", szxc_rev_txt[index].enFontColor);
          ADS_PRINTF("txt len: %d\n", szxc_rev_txt[index].usGetBufferSize);
          ADS_PRINTF("txt content: %s\n", szxc_rev_txt[index].pucDisplayBuffer);
        }
        p_ad = p_ad->next;
        if(p_ad == NULL)
            break;
    }
    szxc_txt_unlock();

    if(index < AD_TXT_MAXNUM)
        ADS_PRINTF("GET HANG PIC TOTLE: %d\n", index);
    else
        ADS_ERROR("***txt pic buffer is too few: %d\n", index);

}

static void szxc_ads_txtpic_monitor()
{
    static u8 receive_txt_flag = 0;
    static u8 receive_txt_version = 0;
    u8 index = 0;
    u8 rolling_index = 0;
    u8 txt_pos = 0;

    MT_ASSERT(TRUE == mtos_sem_create((os_sem_t *)&g_ads_txtflag_lock,1));
    OS_PRINTF("txt 11111\n");

    while(1)
    {
         if(receive_txt_flag)
         {
             if(receive_txt_version != szxc_ad[ADV_TXT].download_version)
             {
                  szxc_free_txt_data();
                  szxc_copy_txt_data();
                  szxc_txt_lock();
                  for(index = 0; index < 4; index++)
                  {
                      if(txt_show_pos[index])
                          txt_show_pos[index] = 0;
                  }
                  txt_show_flag = 0;
                  szxc_txt_unlock();
             }
         }
         else
         {
             if(szxc_rev_flag[ADV_TXT].adv_finish_flag)
             {
                 OS_PRINTF("txt 2222\n");
                 receive_txt_flag = 1;
                 szxc_copy_txt_data();
                 receive_txt_version = szxc_ad[ADV_TXT].download_version;
             }
         }

        if(receive_txt_flag)
        {
            for(index = 0; index < AD_TXT_MAXNUM; index++)
            {
                if(szxc_rev_txt[index].ucShowFlag == 0 && szxc_rev_txt[index].pucDisplayBuffer!= NULL)
                {
                    txt_pos = szxc_rev_txt[index].enDisplayPos;
                    szxc_txt_lock();
                    //每次仅显示一条
                    if(txt_show_flag == 0 && (szxc_rev_txt[index].ucShowTimes != 0))
                    {
                        //调用显示滚动字幕接口
                        ADS_PRINTF("显示滚动字幕%d\n", index);
                        p_ads_mod_priv->display_ad(ADS_AD_TYPE_OSD, (u8 *)&szxc_rev_txt[index]);
                        szxc_rev_txt[index].ucShowFlag = 1;
                        txt_show_flag = 1;
			rolling_index = index;
                    }

                    /*
                    if(txt_show_pos[txt_pos - 1] == 0)
                    {
                        //调用显示滚动字幕接口
                        szxc_rev_txt[index].ucShowFlag = 1;
                        txt_show_pos[txt_pos - 1] = 1;
                    }*/
                    szxc_txt_unlock();
                }
		else	//if (!is_txt_rolling())
	        {
                  szxc_clear_txt_flag(rolling_index + 1);
                }
            }
        }
        mtos_task_sleep(100);
     }
}


//初始化，创建实时监测任务，任务优先级由上层传下来
void szxc_ads_client_init()
{
	s16 ret = FALSE;
	u32 *p_stack = NULL;
 	ret = mtos_sem_create((os_sem_t *)&g_ads_szxc_lock,1);
	if(!ret)
	{
		ADS_ERROR("[drv] %s %d ERR! \n",__func__,__LINE__);
        	MT_ASSERT(0);
    	}

	if(p_szxc_ad_dmx_dev == NULL)
          p_szxc_ad_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL,
            DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_szxc_ad_dmx_dev);
       MT_ASSERT(NULL != p_szxc_ad_dmx_dev->p_base);

    	p_stack = (u32 *)mtos_malloc(SZXC_ADS_STACK_SIZE);
    	MT_ASSERT(p_stack != NULL);

    	ret = mtos_task_create((u8 *)"ads_monitor",
                                           (void *)szxc_ads_data_monitor,
                                           NULL,
                                           p_ads_mod_priv->task_prio_start,
                                           p_stack,
                                           SZXC_ADS_STACK_SIZE);
    	if(!ret)
    	{
        	ADS_ERROR("[drv] %s %d ERR! \n",__func__,__LINE__);
        	MT_ASSERT(0);
    	}

       p_stack = (u32 *)mtos_malloc(4*1024);
    	MT_ASSERT(p_stack != NULL);

    	ret = mtos_task_create((u8 *)"adv_hang",
                                           (void *)szxc_ads_hangpic_monitor,
                                           NULL,
                                          // p_ads_mod_priv->task_prio_start + 1,
                                          19,
                                           p_stack,
                                           (4*1024));
    	if(!ret)
    	{
        	ADS_ERROR("[drv] %s %d ERR! \n",__func__,__LINE__);
        	MT_ASSERT(0);
    	}

        p_stack = (u32 *)mtos_malloc(4*1024);
    	MT_ASSERT(p_stack != NULL);

    	ret = mtos_task_create((u8 *)"adv_txt",
                                           (void *)szxc_ads_txtpic_monitor,
                                           NULL,
                                           p_ads_mod_priv->task_prio_start + 2,
                                           p_stack,
                                           (4*1024));
    	if(!ret)
    	{
        	ADS_ERROR("[drv] %s %d ERR! \n",__func__,__LINE__);
        	MT_ASSERT(0);
    	}
}

/*---------------send data to display-------------------*/

static RET_CODE szxc_read_flashhead()
{
    RET_CODE ret = ERR_FAILURE;

    if(flash_head_flag)
        return SUCCESS;
    else
        ret = szxc_drv_readflash(0, flash_head_info, 4096);

    if(ret == SUCCESS)
    {
        flash_head_flag = 1;
        data_dump(flash_head_info,512);
        return ret;
    }

    return ret;
}

static u8 szxc_find_picinfo_index(AD_SZXC_AD_TYPE_T e_adtype_t, u8 ad_num, u16 severiceid )
{
    u8 index = 0;
    u16 s_id = 0;

    ad_num = flash_head_info[1];
    if(e_adtype_t == AD_SZXC_AD_MES)
    {
        for(index = 0; index < ad_num; index ++)
        {
            s_id = (flash_head_info[256 +index*16] << 8) | flash_head_info[256 + 1 + index*16];
            if(flash_head_info[2+index] == e_adtype_t && severiceid == s_id)
                 break;
        }
        if(index >= ad_num)
        {
            for(index = 0; index < ad_num; index ++)
            {
                s_id = (flash_head_info[256 +index*16] << 8) | flash_head_info[256 + 1 + index*16];
                if(flash_head_info[2+index] == e_adtype_t && 0 == s_id)
                     break;
            }
         }
    }
    else
    {
         for(index = 0; index < ad_num; index ++)
        {
            if(flash_head_info[2+index] == e_adtype_t)
            {
              s_id = (flash_head_info[256 +index*16] << 8) | flash_head_info[256 + 1 + index*16];
              OS_PRINTF("^^^^^^^^^^^^^^^^^^^INDEX: %x, s_id : %x, e_adtype_t : %x\n", index, s_id, e_adtype_t);
              break;
            }
        }
    }

   if(index >= ad_num)
        return ad_num;
   else
        return index;
}


RET_CODE szxc_ad_show_logo(AD_SZXC_AD_TYPE_T e_adtype_t)
{
    u8 *logo_data = NULL;
    RET_CODE ret = ERR_FAILURE;
    u8 ad_num = 0;
    u8 index = 0;
    u8 pic_type = 0;
    u16 pic_len = 0;
    u32 pic_addr = 0;

    ret = szxc_read_flashhead();
    if(ret == SUCCESS)
    {
         ad_num = flash_head_info[1];
         index = szxc_find_picinfo_index(e_adtype_t, ad_num, 0);

         if(index >= ad_num)
            return ERR_FAILURE;

        pic_len = (flash_head_info[256 + 10 +index*16] << 8) | flash_head_info[256 + 11 +index*16];
        pic_addr = (flash_head_info[256 + 13 +index*16] << 16) |(flash_head_info[256 + 14 +index*16] << 8) | flash_head_info[256 + 15 +index*16];
        pic_type = flash_head_info[256 + 12 +index*16];

        logo_data = (u8*)mtos_malloc(pic_len);
        MT_ASSERT(NULL != logo_data);
        memset(logo_data, 0, pic_len);
        ret = szxc_drv_readflash(pic_addr, logo_data, pic_len);
        if(ret == SUCCESS)
        {
             ADS_PRINTF("#####adv_type : %d \n", pic_type);
             p_ads_mod_priv->display_logo(pic_len, logo_data);
            // szxc_ad_display_I(logo_data,pic_len);
             mtos_free(logo_data);
             return ret;
        }
        else
        {
            mtos_free(logo_data);
            ADS_ERROR("***%s %d read close pic error\n", __func__,__LINE__);
            return ret;
        }
    }
    else
    {
        ADS_ERROR("*** %s %d read flash head info error\n", __func__,__LINE__);
        return ret;
    }
}

RET_CODE szxc_ad_show_pic(AD_SZXC_AD_TYPE_T e_adtype_t, u16 service_id)
{
    u8 *pic_data = NULL;
    RET_CODE ret = ERR_FAILURE;
    s_szxc_adv_pic_show_t adv_pic = {0};
    u8 ad_num = 0;
    u8 index = 0;
    u8 pic_type = 0;
    u32 pic_len = 0;
    u32 pic_addr = 0;
    u16 pic_hight = 0;
    u16 pic_width = 0;

    ADS_PRINTF("[szxc]%s %d\n",__FUNCTION__, __LINE__);
    ret = szxc_read_flashhead();
    if(ret == SUCCESS)
    {
         ad_num = flash_head_info[1];
         if(e_adtype_t == AD_SZXC_AD_MES)
             index = szxc_find_picinfo_index(e_adtype_t, ad_num, service_id);
         else
             index = szxc_find_picinfo_index(e_adtype_t, ad_num, 0);

         if(index >= ad_num)
            return ERR_FAILURE;
        OS_PRINTF("***************************INDEX : %d\n",index);
        pic_hight = (flash_head_info[256 + 2 +index*16] << 8) | flash_head_info[256 + 3 +index*16];
        pic_width = (flash_head_info[256 + 4 +index*16] << 8) | flash_head_info[256 + 5 +index*16];
        pic_len = (flash_head_info[256 + 10 +index*16] << 8) | flash_head_info[256 + 11 +index*16];
        pic_addr = (flash_head_info[256 + 13 +index*16] << 16) |(flash_head_info[256 + 14 +index*16] << 8) | flash_head_info[256 + 15 +index*16];
        pic_type = flash_head_info[256 + 12 +index*16];

        OS_PRINTF("***************************pic_hight : %x\n",pic_hight);
        OS_PRINTF("***************************pic_width : %x\n",pic_width);
        OS_PRINTF("***************************pic_len : %x\n",pic_len);
        OS_PRINTF("***************************pic_addr : %x\n",pic_addr);
        OS_PRINTF("***************************pic_type : %x\n",pic_type);
        pic_data = mtos_malloc(pic_len);
        MT_ASSERT(NULL != pic_data);
        memset(pic_data, 0, pic_len);
        ret = szxc_drv_readflash(pic_addr, pic_data, pic_len);
        if(ret == SUCCESS)
        {
             adv_pic.pucPicData = pic_data;
             adv_pic.enPicType = pic_type;
             adv_pic.usHeigth = pic_hight;
             adv_pic.usWidth = pic_width;
             adv_pic.uiDataLen= pic_len;
             ADS_PRINTF("#####adv_type : %d \n", pic_type);
             p_ads_mod_priv->display_ad(ADS_AD_TYPE_PIC, (u8 *)&adv_pic);
             //data_dump(adv_pic.pucPicData, adv_pic.uiDataLen);
             mtos_free(pic_data);
             return ret;
        }
        else
        {
            mtos_free(pic_data);
            ADS_ERROR("***%s %d read close pic error\n", __func__,__LINE__);
            return ret;
        }
    }
    else
    {
        ADS_ERROR("*** %s %d read flash head info error\n", __func__,__LINE__);
        return ret;
    }
}





