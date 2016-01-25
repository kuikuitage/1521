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
#include "mtos_timer.h"
#include "mtos_printk.h"
#include "charsto.h"

#include "dmx.h"

#include "subt_pic_ware.h"
#include "subt_pic_adapter.h"
#include "tr_api.h"
#include "tr_subt.h"

//#define SUBT_TR_ADT_DEBUG

#ifdef SUBT_TR_ADT_DEBUG
#define    SUBT_TR_ADT_PRINTF OS_PRINTF
#else
#define    SUBT_TR_ADT_PRINTF DUMMY_PRINTF
#endif

extern subt_pic_adapter_priv_t g_subt_pic_priv;
subt_pic_module_priv_t *subt_pic_priv = NULL;
static u8 dmx_monitor_prio = 0;

static RET_CODE subt_pic_tr_init(void *param)
{
    return SUCCESS;
}

static RET_CODE subt_pic_tr_deinit()
{
    return SUCCESS;
}

static RET_CODE subt_pic_tr_open()
{
    SUBT_TR_ADT_PRINTF("[sub],subt_pic_tr_open \n");
    MTR_TITLE_Start();
    mtos_task_resume(dmx_monitor_prio);
    return SUCCESS;
}

static RET_CODE subt_pic_tr_close()
{
    SUBT_TR_ADT_PRINTF("[sub],subt_pic_tr_close \n");
    mtos_task_suspend(dmx_monitor_prio);
    MTR_TITLE_Message_Stop();
    return SUCCESS;
}

static RET_CODE subt_pic_tr_stop()
{
    SUBT_TR_ADT_PRINTF("[sub],subt_pic_tr_stop \n");
    mtos_task_suspend(dmx_monitor_prio);
    MTR_TT_StopScroll();
    return SUCCESS;
}

static RET_CODE subt_pic_tr_io_ctrl(u32 cmd, void *param)
{
    return SUCCESS;
}

MTRDRV_U8 OnKeyPress_Is_Normal_or_Not(MTRDRV_U8 flg)
{
    BOOL flag = 1;
    SUBT_TR_ADT_PRINTF("\n [sub],OnKeyPress_Is_Normal_or_Not \n");

    if(NULL != subt_pic_priv->subt_is_scroll)
    {
        subt_pic_priv->subt_is_scroll(&flag);
    }
    SUBT_TR_ADT_PRINTF("[sub],flag=%d \n",flag);
    
    return flag;
}

MTRDRV_VOID MTR_DRV_TITLE_OSD_DRAW_Rectangle(
                                                    MTRDRV_OSD_RECTCOORDINATE RectCoordinate,
                                                    MTRDRV_U32 color)
{
    SUBT_TR_ADT_PRINTF("\n [sub],MTR_DRV_TITLE_OSD_DRAW_Rectangle, color=%d \n",color);
    SUBT_TR_ADT_PRINTF("[sub],Rect(%d,%d,%d,%d) \n",RectCoordinate.X_x,RectCoordinate.Y_y,
                                                                                 RectCoordinate.W_w,RectCoordinate.H_h);

    if(NULL != subt_pic_priv->draw_rectangle)
    {
        subt_pic_priv->draw_rectangle(RectCoordinate.X_x, RectCoordinate.Y_y,
                                                         RectCoordinate.W_w,RectCoordinate.H_h,
                                                         color);
    }

}

MTRDRV_VOID TT_GFMFixupRectangleDepth(MTRDRV_U32 x,MTRDRV_U32 y,
                                                                                  MTRDRV_U32 width,MTRDRV_U32 height,
                                                                                  MTRDRV_U32 Bmpwidth,MTRDRV_U8 *source,
                                                                                  MTRDRV_U8 *source1,MTRDRV_U8 Scoll_time)
{
    SUBT_TR_ADT_PRINTF("\n [sub],TT_GFMFixupRectangleDepth \n");
    SUBT_TR_ADT_PRINTF("x=%d,y=%d,w=%d,h=%d \n",x,y,width,height);
    SUBT_TR_ADT_PRINTF("Bmpwidth=%d, Scoll_time=%d \n",Bmpwidth,Scoll_time);
    SUBT_TR_ADT_PRINTF("source=0x%X, source=0x%X \n",(MTRDRV_U32)source,(MTRDRV_U32)source1);

    if(NULL != subt_pic_priv->draw_picture)
    {
        subt_pic_priv->draw_picture(x, y,width,height,source,source1,Bmpwidth,Scoll_time);
    }
}

//the function don't used
MTRDRV_VOID TT_GIFdataFixupRectangle(MTRDRV_U32 x,MTRDRV_U32 y,
                                                                            MTRDRV_U32 width,MTRDRV_U32 height,
                                                                            MTRDRV_U8 *source)
{
        SUBT_TR_ADT_PRINTF("\n [sub],TT_GIFdataFixupRectangle \n");
        SUBT_TR_ADT_PRINTF("x=%d,y=%d,w=%d,h=%d \n",x,y,width,height);
        SUBT_TR_ADT_PRINTF("source=0x%X \n",(MTRDRV_U32)source);

}

MTRDRV_BOOL	MTR_DRV_TITLE_OSD_GetOpenState(MTRDRV_VOID)
{
    BOOL display_state = 0;
    SUBT_TR_ADT_PRINTF("\n [sub],MTR_DRV_TITLE_OSD_GetOpenState \n");

    if(NULL != subt_pic_priv->osd_display_is_ok)
    {
        subt_pic_priv->osd_display_is_ok(&display_state);
    }
    
    return display_state;
}

//don't used
MTRDRV_VOID MTR_DRV_TITLE_Get_OSD_Image( MTRDRV_S16 X_x, MTRDRV_S16 Y_y,
                                                                                            MTRDRV_U16 W_w, MTRDRV_U16 H_h,
                                                                                            MTRDRV_U32 imageAdd )
{
        SUBT_TR_ADT_PRINTF("\n [sub],MTR_DRV_TITLE_Get_OSD_Image \n");
        SUBT_TR_ADT_PRINTF("x=%d,y=%d,w=%d,h=%d \n",X_x,Y_y,W_w,H_h);
        SUBT_TR_ADT_PRINTF("imageAdd=0x%X \n",imageAdd);
}

//don't used
MTRDRV_U8* MTR_TT_GFX_CalcOsdMemoryAddr(MTRDRV_U16 x,
                                                                                           MTRDRV_U16 y,
                                                                                           MTRDRV_U8* bitOffset,
                                                                                           MTRDRV_U16* bytePreLine,
                                                                                           MTRDRV_U8 depth)
{
        MTRDRV_U8 * p_addr = NULL;

        SUBT_TR_ADT_PRINTF("\n [sub],MTR_TT_GFX_CalcOsdMemoryAddr \n");
        SUBT_TR_ADT_PRINTF("x=%d,y=%d \n",x,y);

        return p_addr;
}

MTRDRV_VOID MTR_TITLE_Get_pixel(MTRDRV_U8 *g_pixel)
{
    MTRDRV_U8 step_pixel = 4;
    
    SUBT_TR_ADT_PRINTF("\n [sub],MTR_TITLE_Get_pixel \n");

    if(NULL == g_pixel)
    {
        SUBT_TR_ADT_PRINTF("paramter is error ! \n");
        return ;
    }

    if(NULL != subt_pic_priv->scroll_step_pixel_set)
    {
         subt_pic_priv->scroll_step_pixel_set(&step_pixel);
         *g_pixel = step_pixel;
    }

}

MTRDRV_VOID MTR_DRV_TITLE_DMX_GetCurrentChannelInfo(MTRDRV_U16 *net_id,
                                                                                                                   MTRDRV_U16 *ts_id,
                                                                                                                   MTRDRV_U16 *service_id)
{
    SUBT_TR_ADT_PRINTF("\n [sub],MTR_DRV_TITLE_DMX_GetCurrentChannelInfo \n");

    #if 0
    *net_id = 0;
    *ts_id = 0x4;
    *service_id = 0xe7;
    #else
    if(NULL != subt_pic_priv->channel_info_set)
    {
        subt_pic_priv->channel_info_set(net_id, ts_id, service_id);
    }
    #endif
    
    SUBT_TR_ADT_PRINTF("[sub],network_id:0x%x, ts_id:0x%x, service_id:0x%x \n",*net_id,*ts_id,*service_id);
}

RET_CODE subt_pic_adt_tr_attach(subt_pic_module_cfg_t * p_cfg,
                                                                    u32 *p_adm_id)
{
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].attached = 1;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].inited = 0;

    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.init 
        = subt_pic_tr_init;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.deinit 
        = subt_pic_tr_deinit;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.open
        = subt_pic_tr_open;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.close
        = subt_pic_tr_close;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.stop
        = subt_pic_tr_stop;
    g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].func.io_ctrl 
        = subt_pic_tr_io_ctrl;

    subt_pic_priv = g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR].p_priv 
        = mtos_malloc(sizeof(subt_pic_module_priv_t));
    MT_ASSERT(NULL != subt_pic_priv);
    memset(subt_pic_priv, 0x00, sizeof(subt_pic_module_priv_t));
    
    subt_pic_priv->id = SUBT_PIC_ID_ADT_TR;
    subt_pic_priv->task_prio_start = p_cfg->task_prio_start;
    subt_pic_priv->task_prio_end = p_cfg->task_prio_end;
    subt_pic_priv->subt_is_scroll = p_cfg->subt_is_scroll;
    subt_pic_priv->osd_display_is_ok= p_cfg->osd_display_is_ok;
    subt_pic_priv->channel_info_set= p_cfg->channel_info_set;
    subt_pic_priv->scroll_step_pixel_set= p_cfg->scroll_step_pixel_set;
    subt_pic_priv->draw_rectangle= p_cfg->draw_rectangle;
    subt_pic_priv->draw_picture= p_cfg->draw_picture;
    
    SUBT_TR_ADT_PRINTF("ID:%d \n");
    SUBT_TR_ADT_PRINTF("start prio:%d \n",subt_pic_priv->task_prio_start);
    SUBT_TR_ADT_PRINTF("end prio:%d \n",subt_pic_priv->task_prio_end);
    
    *p_adm_id = (u32)(&g_subt_pic_priv.adm_op[SUBT_PIC_ID_ADT_TR]);

    SUBT_TR_ADT_PRINTF("[sub],subtitle_pic_adt_tr_attach finished \n");

    dmx_monitor_prio = subt_pic_priv->task_prio_end;
    receive_data_task_prio_set(dmx_monitor_prio);
    tr_roll_title_init_set();
    MTR_TITLE_Creat_Task(subt_pic_priv->task_prio_start,subt_pic_priv->task_prio_end-1);

    return SUCCESS;
}


