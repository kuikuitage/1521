/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "class_factory.h"
#include "mem_manager.h"

#include "dmx.h"
#include "lib_rect.h"
#include "common.h"
#include "display.h"
#include "vdec.h"

#include "drv_misc.h"

#include "mdl.h"

#include "subtitle_api.h"
#include "subt_priv_vsb.h"

#define SUBT_DYNAMIC_ALLOC_BUFFER   1
#ifdef WARRIORS
#define SUBT_TASK_TIMEOUT           200
#else
#define SUBT_TASK_TIMEOUT           100
#endif
#define SUBT_PTS_100MS              (90000 / 10)

#ifdef WARRIORS
//#define DMX_NEW_INTERFACE
#else
#define DMX_NEW_INTERFACE
static u16 hDmxLock = 0;
void OSSemPend(u16 *pevent, u16 timeout, u8 *err);
void OSSemPost(u16 *pevent);
#endif

/*!
  command
  */
typedef enum
{
    SUBT_CMD_START,
    SUBT_CMD_STOP,
    SUBT_CMD_SET_DISPLAY,
    SUBT_CMD_SET_VIDEO_STD,
    SUBT_CMD_SET_PAUSE,
    SUBT_CMD_SET_PAGE
} vbi_cmd_t;

/*!
  state of PES
  */
typedef enum
{
    PES_STATE_TIMEOUT_VSB,
    PES_STATE_CURRENTLY_VSB,
    PES_STATE_AFTERTIME_VSB,
    PES_STATE_BEFORE_VSB
} pes_state_vsb_t;

extern u32 vbi_get_stc(void);
extern s32 vbi_get_pts_offset(u32 pts, u32 stc);
#ifdef JAZZ
extern int dmx_jazz_wait_for_data(int timeout);
#endif
#ifndef WARRIORS
static u8 s_p_pkt[188] = { 0 };
#endif

static pes_state_vsb_t check_PES_state_vsb(u32 pts)
{
    s32 pts_offset = 0;
    u32 stc        = 0;

    stc        = vbi_get_stc();
    pts_offset = vbi_get_pts_offset(pts, stc);

    if(pts_offset < ((- SUBT_PTS_100MS) * 10))
    {
        OS_PRINTF("SUBT!:    [#1]PTS timeout, pts %d, stc %d, offset %d\n"
            , pts, stc, pts_offset);
        return PES_STATE_TIMEOUT_VSB;
    }
    else if(pts_offset < SUBT_PTS_100MS * 1)
    {
        return PES_STATE_CURRENTLY_VSB;
    }
    else if(pts_offset < SUBT_PTS_100MS * 22)
    {
        OS_PRINTF("SUBT!:    [#1]PTS Error, pts %d, stc %d, offset %d\n"
            , pts, stc, pts_offset);
        return PES_STATE_AFTERTIME_VSB;
    }

    ///TODO:If the pts is very large with stc, we must drop the frame,and get next.
    OS_PRINTF("SUBT!:    [#2]PTS Error, pts %d, stc %d, offset %d\n"
        , pts, stc, pts_offset);
    return PES_STATE_BEFORE_VSB;
}


static void start_filter_vsb(dvb_subtitle_vsb_t *p_subt, u16 pid)
{
    RET_CODE     ret    = ERR_FAILURE;
    u32          i      = 0;
    dmx_device_t *p_dev    = NULL;
    dmx_slot_setting_t slot_t;
    dmx_filter_setting_t  filter_param_t;
#ifndef WARRIORS
    u8 err = 0;
#endif

    if(pid == (u16)INVALID
        || (pid == p_subt->pid && p_subt->dmx_channel != DMX_INVALID_CHANNEL_ID))
    {
        return;
    }

    for(i = 0; i < 512; i ++)
    {
        p_subt->p_dmx_buf[i * 188] = 0;
    }

    p_subt->pid = pid;

#ifndef WARRIORS
    OSSemPend(&hDmxLock, 0, &err);
#endif

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
#ifdef WARRIORS
    MT_ASSERT(NULL != p_dev->p_base);
#else
    MT_ASSERT(NULL != p_dev);
#endif

    slot_t.in     = DMX_INPUT_EXTERN0;
    slot_t.pid   = pid;
    slot_t.type = DMX_CH_TYPE_TSPKT;
    ret = dmx_si_chan_open(p_dev, &slot_t, &(p_subt->dmx_channel));
    if(ret != SUCCESS)
    {
      OS_PRINTF("open si channel failed!\n");
      MT_ASSERT(0);
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

    ret = dmx_si_chan_set_buffer(p_dev,
              p_subt->dmx_channel, p_subt->p_dmx_buf, SUBT_FILTER_BUF_SIZE);

    OS_PRINTF(" set p_filter_buf 0x%x\n",p_subt->p_dmx_buf);
    if(ret != SUCCESS)
    {
      OS_PRINTF("set filter buffer failed!\n");
      MT_ASSERT(0);
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

#ifndef WARRIORS
    filter_param_t.req_mode = DMX_REQ_MODE_CONTINUOUS_TS;
    filter_param_t.en_crc = FALSE;
#endif
    filter_param_t.continuous = TRUE;
    filter_param_t.ts_packet_mode = DMX_FIVE_MODE;
    ret = dmx_si_chan_set_filter(p_dev, p_subt->dmx_channel, &filter_param_t);
    if(ret != SUCCESS)
    {
      OS_PRINTF("set filter failed!\n");
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

    ret = dmx_chan_start(p_dev, p_subt->dmx_channel);
    if(ret != SUCCESS)
    {
      OS_PRINTF("start channel  failed!\n");
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
}


static void stop_filter_vsb(dvb_subtitle_vsb_t *p_subt)
{
    RET_CODE     ret    = ERR_FAILURE;
    dmx_device_t *p_dev    = NULL;
#ifndef WARRIORS
    u8 err = 0;
#endif

    if(p_subt->dmx_channel == DMX_INVALID_CHANNEL_ID)
    {
        return;
    }

#ifndef WARRIORS
    OSSemPend(&hDmxLock, 0, &err);
#endif

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, p_subt->dmx_channel);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(p_dev, p_subt->dmx_channel);
    MT_ASSERT(ret == SUCCESS);

    p_subt->dmx_channel = DMX_INVALID_CHANNEL_ID;
#ifndef WARRIORS
    OSSemPost(&hDmxLock);
#endif
}

static u8 *dmx_next_ts_packet_vsb(dvb_subtitle_vsb_t *p_subt)
{
    RET_CODE     ret    = ERR_FAILURE;
    dmx_device_t *p_dev    = NULL;
    u32 pkt_size = 0;
    u8 *p_pkt = NULL;
#ifndef WARRIORS
    u8 err = 0;
#endif

    if(p_subt->dmx_channel == DMX_INVALID_CHANNEL_ID)
    {
        MT_ASSERT(0);
    }

#ifndef WARRIORS
    OSSemPend(&hDmxLock, 0, &err);
#endif

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);
    while(SUCCESS != ret)
    {
#ifdef JAZZ
      dmx_jazz_wait_for_data(100);
#endif
      ret = dmx_si_chan_get_data(p_dev, p_subt->dmx_channel, &p_pkt, &pkt_size);
      if(SUCCESS != ret)
      {
        ///TODO: maybe the tasksleep should be changed
#ifndef WARRIORS
        OS_PRINTF("SUBT:no input data\n");
        OSSemPost(&hDmxLock);
#endif
        return NULL;
      }
    }

#ifdef WARRIORS
    OS_PRINTF("return  addr  0x%x\n",p_pkt);
    return p_pkt;
#else
    memcpy(s_p_pkt, p_pkt, pkt_size);
    OSSemPost(&hDmxLock);

    return s_p_pkt;
#endif
}

static u8 *next_ts_packet_vsb(u8 *p_pkt, u8 *p_buf)
{
    #ifdef WIN32
    if(p_pkt == (p_buf + (SUBT_FILTER_BUF_SIZE - 188)))
    {
        p_pkt = p_buf;
    }
    else
    {
        p_pkt += 188;
    }

    return p_pkt;

    #else
      #ifndef WARRIORS
          p_buf = (u8 *)(((u32)p_buf) | 0x10000000);
          p_pkt = (u8 *)(((u32)p_pkt) | 0x10000000);
       #else
          p_buf = (u8 *)((u32)p_buf);
          p_pkt = (u8 *)((u32)p_pkt);
       #endif
    if(p_pkt == (p_buf + (SUBT_FILTER_BUF_SIZE - 188)))
    {
        p_pkt = p_buf;
    }
    else
    {
        p_pkt += 188;
    }

    return p_pkt;
    #endif
}

static void check_subt_vsb(void *p_data)
{
    vdec_info_t v_info;
    BOOL clear_screen = FALSE;
    static u8 cnt = 0;
    dvb_subtitle_vsb_t  *p_subt = (dvb_subtitle_vsb_t *)p_data;

    void *p_video_dev = dev_find_identifier(NULL,
      DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    vdec_get_info(p_video_dev,&v_info);
    if(v_info.err == VDEC_ERROR_THIRSTY)     //decode isn't working
    {
        cnt++;

        if(cnt > 3)
        {
            clear_screen = TRUE;
        }
        else
        {
            cnt++;

          if(cnt > 5)
          {
              clear_screen = TRUE;
          }
        }
        if(clear_screen)
        {
            if(p_subt->is_display)
          {
                subt_dec_hide_page_vsb(p_subt);
                cnt = 0;
          }
            subt_db_reset_vsb(p_subt);
        }
    }
}

static void subt_task_vsb(void *p_data)
{
    dvb_subtitle_vsb_t  *p_subt = (dvb_subtitle_vsb_t *)p_data;
    os_msg_t        msg = {0,};
    BOOL            rc  = FALSE;
    u32             pts = 0;
    u32             task_timeout  = 0;
    u8              *p_tspkt      = NULL;
    u8              *p_pes_buf    = NULL;
    u32             pes_copy_size = 0;
    u8              ts_cont_cnt   = 0xff;
    os_msg_t        send_msg = {0,};

    MT_ASSERT(p_subt != NULL);

    p_subt->callback_msg_id = -1;

    p_subt->msg_cnt       = 0;
    p_subt->pid           = (u16)INVALID;
    p_subt->stats         = SUBT_STATS_IDLE;
    p_subt->is_display    = FALSE;
    p_subt->is_PES_ready  = FALSE;

    p_subt->dmx_channel   = DMX_INVALID_CHANNEL_ID;
    p_subt->p_dmx_buf  = NULL;
    p_subt->p_pes_buf     = NULL;
    p_subt->p_notify_func = NULL;
    p_subt->p_render      = NULL;
    p_subt->vid_std       = VID_STD_PAL;

    subt_dec_init_vsb();

    while(1)
    {
        rc = mtos_messageq_receive(p_subt->msg_hadle, &msg, task_timeout);
        if(!rc)
        {
            //p_subt->msg_cnt = 0;

            if(p_subt->stats != SUBT_STATS_RUNNING)
            {
                continue;
            }


            if(p_subt->is_PES_ready)
            {
                pes_state_vsb_t state = check_PES_state_vsb(pts);
                check_subt_vsb(p_subt);  //check if the decode is working,if not,hide the subtitle

                if(state == PES_STATE_CURRENTLY_VSB
                    || state == PES_STATE_TIMEOUT_VSB)
                {
                    subt_dec_process_vsb(p_subt, p_pes_buf);
                    p_subt->is_PES_ready = FALSE;
                    pes_copy_size        = 0;
                    ts_cont_cnt          = 0xff;
                }
                else if(state == PES_STATE_BEFORE_VSB)
                {
                    //if lack behind, drop the frame,read next
                    p_subt->is_PES_ready = FALSE;
                    pes_copy_size        = 0;
                    ts_cont_cnt          = 0xff;
                }
                else if(state == PES_STATE_AFTERTIME_VSB)
                {
                    continue;  //data is faster than stc
                    //if lack behind, drop the frame,read next
                    p_subt->is_PES_ready = FALSE;
                    pes_copy_size        = 0;
                    ts_cont_cnt          = 0xff;
                }
            }
            else if(p_subt->page_timeout > 0)
            {
                p_subt->page_timeout -= task_timeout;

                if(p_subt->page_timeout <= 0)
                {
                    if(p_subt->is_display)
                    {
                        subt_dec_hide_page_vsb(p_subt);
                    }

                    subt_db_reset_vsb(p_subt);
                }
            }

            if(!p_subt->is_PES_ready)
            {
                u8 af_ctrl = 0;     /*    adaption_field_ctrl */
                u8 af_len  = 0;     /*    adaption_field_len    */
                u8 tmp     = 0;

#ifndef WIN32
#ifndef WARRIORS
#ifndef JAZZ
                p_tspkt = (u8 *)((u32)p_tspkt | 0x10000000);
#endif
#endif
#endif

                while(p_subt->msg_cnt <= 0)
                {
                    //sync_byte
                    if(p_tspkt[0] != 0x47)
                    {
                        //OS_PRINTF("SUBT:sync byte error 0x%x  addr 0x%x!!!\n",
                         //         p_tspkt[0], (u32)p_tspkt);
#ifndef DMX_NEW_INTERFACE
                        //p_tspkt     = next_ts_packet_vsb(p_tspkt
                        //                    , p_subt->p_dmx_buf);
#else
                        p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
#ifdef WARRIORS
                        if(NULL == p_tspkt)
                        {
                            break;
                        }
#endif
#endif
                        break;
                    }

                    if(task_timeout != SUBT_TASK_TIMEOUT / 2)
                    {
                        u8 *p_next_pkt = next_ts_packet_vsb(p_tspkt
                                                , p_subt->p_dmx_buf);
                        if(p_next_pkt[0] != 0x47)
                        {
                            task_timeout = SUBT_TASK_TIMEOUT / 2;
                            break;
                        }
                    }
                    else
                    {
#ifdef WARRIORS
                        task_timeout = SUBT_TASK_TIMEOUT;
#else
                        task_timeout = SUBT_TASK_TIMEOUT >> 1;
#endif
                    }


                    /*
                                  skip until payload_unit_start_indicator is set
                                  */
                    //transport_error_indicator
                    if(p_tspkt[1] & 0x80)
                    {
#ifndef DMX_NEW_INTERFACE
                        p_tspkt[0]  = 0;
                        p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                        p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
                        if(NULL == p_tspkt)
                        {
                            break;
                        }
#endif
                        ts_cont_cnt = 0xff;
                        OS_PRINTF("SUBT:    error packet, skip\n");
                        continue;
                    }

                    //payload_unit_start_indicator, 1 stands for first valid packet
                    if(pes_copy_size == 0 && !(p_tspkt[1] & 0x40))
                    {
#ifndef DMX_NEW_INTERFACE
                        p_tspkt[0]  = 0;
                        p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                        p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
                        if(NULL == p_tspkt)
                        {
                            break;
                        }
#endif
                        ts_cont_cnt = 0xff;
                        continue;
                    }

                    ///TODO:
                    //transport_scrambling_control 00 stands for not scramble
                    if(0 && (p_tspkt[3] & 0xc0) != 0)
                    {
#ifndef DMX_NEW_INTERFACE
                       p_tspkt[0]    = 0;
                       p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                        p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
                        if(NULL == p_tspkt)
                        {
                            break;
                        }
#endif
                        ts_cont_cnt   = 0xff;
                        pes_copy_size = 0;
                        OS_PRINTF("SUBT:    scrambling, skip \n");
                        continue;
                    }

                    /*
                                  adaption_field_ctrl
                                  00 reserve
                                  01 no adaption field ,payload only
                                  10 adaption field only, no payload
                                  11 adaption filed followed by payload
                                  */
                    af_ctrl = (p_tspkt[3] & 0x30) >> 4;
                    //no payload skip
                    if(af_ctrl == 0x02 || af_ctrl == 0)
                    {
                        OS_PRINTF("SUBT:    af #1, skip [0x%x]\n", af_ctrl);
#ifndef DMX_NEW_INTERFACE
                        p_tspkt[0] = 0;
                        p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                        p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
                        if(NULL == p_tspkt)
                        {
                            break;
                        }
#endif
                        continue;
                    }
                    else if(af_ctrl == 0x03)
                    {
                        af_len = p_tspkt[4] + 1;
                    }
                    else if(af_ctrl == 0x01)
                    {
                        af_len = 0;
                    }

                    /*
                                   continuity_counter
                                   */
                    tmp = p_tspkt[3] & 0x0f;
                    if((ts_cont_cnt == 0xff) || (p_tspkt[1] & 0x40))
                    {
                        ts_cont_cnt = tmp;
                    }
                    else
                    {
                        if(ts_cont_cnt == 0x0f)
                        {
                            ts_cont_cnt = 0;
                        }
                        else
                        {
                            ts_cont_cnt ++;
                        }

                        if(ts_cont_cnt != tmp)
                        {
                            OS_PRINTF("SUBT!:    TS Packet Error #1\n");
                            ts_cont_cnt   = 0xff;
                            pes_copy_size = 0;
                            //first start should start indicator as 1
                            if((p_tspkt[1] & 0x40) == 0)
                            {
#ifndef DMX_NEW_INTERFACE
                                p_tspkt[0] = 0;
                                p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                                p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
                                if(NULL == p_tspkt)
                                {
                                    break;
                                }
#endif
                                continue;
                            }
                        }
                    }

                    //next first start begin , check whether old pes packet is ready
                    if((p_tspkt[1] & 0x40) && (pes_copy_size != 0))
                    {
                        if(pes_copy_size > 6)
                        {
                            u32 pes_data_length
                                = (((u16)p_pes_buf[4] << 8) & 0xff00)
                                + p_pes_buf[5] + 6;

                            if(pes_copy_size >= pes_data_length)
                            {
                                p_subt->is_PES_ready = TRUE;
                            }
                            else
                            {
                                OS_PRINTF("SUBT!:TS Packet Error #2\n");
                                pes_copy_size = 0;
                                ts_cont_cnt   = 0xff;
                            }
                        }
                        else
                        {
                            OS_PRINTF("SUBT!:    TS Packet Error #3\n");
                            pes_copy_size = 0;
                            ts_cont_cnt   = 0xff;
                        }
                    }

                    //184 comes from 188 - 4(ts packe header)
                    if((pes_copy_size + 184 - af_len) > SUBT_PES_BUF_SIZE)
                    {
                        OS_PRINTF("SUBT!:    PES buffer overflow\n");
                        pes_copy_size = 0;
                        continue;
                    }

                    if(af_len < 184)
                    {
                        memcpy(p_pes_buf + pes_copy_size
                            ,(void *)((u32)p_tspkt + 4 + af_len)
                            , 184 - af_len);

                        pes_copy_size += 184 - af_len;
                    }
                    else
                    {
                        OS_PRINTF("VBI!:    TS Packet Error #4\n");
                    }

                    if(pes_copy_size > 6)
                    {
                        u32 pes_data_length
                            = (((u16)p_pes_buf[4] << 8) & 0xff00)
                            + p_pes_buf[5] + 6;

                        if(pes_copy_size >= pes_data_length)
                        {
                            p_subt->is_PES_ready = TRUE;
                        }
                    }

#ifndef DMX_NEW_INTERFACE
                    p_tspkt[0] = 0;
                    p_tspkt     = next_ts_packet_vsb(p_tspkt
                                            , p_subt->p_dmx_buf);
#else
                    p_tspkt    = dmx_next_ts_packet_vsb(p_subt);
#ifdef WARRIORS
                    if(NULL == p_tspkt)
                    {
                        break;
                    }
#endif
#endif

                    if(p_subt->is_PES_ready == TRUE)
                    {
                        pes_state_vsb_t state;

                        SUBT_PRINTF("SUBT:new PES, length = 0x%x\n", pes_copy_size);

                        subt_dec_check_pes_header_vsb(p_subt, p_pes_buf, &pts
                                                                , NULL, NULL);

                        state = check_PES_state_vsb(pts);

                        if(state == PES_STATE_CURRENTLY_VSB
                            || state == PES_STATE_TIMEOUT_VSB)
                        {
                            subt_dec_process_vsb(p_subt, p_pes_buf);
                            pes_copy_size        = 0;
                            ts_cont_cnt          = 0xff;
                            p_subt->is_PES_ready = FALSE;
                        }
                        else if(state == PES_STATE_BEFORE_VSB)
                        {
                            //if lack behind, drop the frame,read next
                            p_subt->is_PES_ready = FALSE;
                            pes_copy_size        = 0;
                            ts_cont_cnt          = 0xff;
                        }
                        else if(state == PES_STATE_AFTERTIME_VSB)
                        {
                            break;   //data is faster than stc
                            //if lack behind, drop the frame,read next
                            p_subt->is_PES_ready = FALSE;
                            pes_copy_size        = 0;
                            ts_cont_cnt          = 0xff;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                if(p_subt->msg_cnt > 0)
                {
                    p_subt->msg_cnt = 0;
                }
            }

          continue;
        }


        if(p_subt->msg_cnt > 0)
        {
            p_subt->msg_cnt = 0;
        }


        switch(msg.content)
        {
            case SUBT_CMD_SET_DISPLAY:
            {
                if(p_subt->stats == SUBT_STATS_IDLE
                    || p_subt->is_display == msg.para1)
                {
                    break;
                }

                if(p_subt->is_display != TRUE)
                {
                    if(p_subt->p_render == NULL)
                    {
                        p_subt->p_render = mtos_malloc(sizeof(subt_render_t));
                        MT_ASSERT(p_subt->p_render != NULL);
                    }

                    subt_layer_select_osd(p_subt->p_render,p_subt->lay_id_t);
                    subt_osd_init_vsb(p_subt->p_render);
                    subt_dec_display_page_vsb(p_subt);
                }
                else
                {
                    subt_dec_hide_page_vsb(p_subt);
                    subt_osd_deinit_vsb(p_subt->p_render);

                    if(SUBT_DYNAMIC_ALLOC_BUFFER && p_subt->p_render != NULL)
                    {
                        mtos_free(p_subt->p_render);
                        p_subt->p_render = NULL;
                    }
                }

                p_subt->is_display = msg.para1;
                break;
            }
            case SUBT_CMD_START:
            {
                if(p_subt->stats == SUBT_STATS_RUNNING)
                {
                    break;
                }

                p_subt->stats     = SUBT_STATS_PAUSE;
                p_subt->is_display = FALSE;
                break;
            }
            case SUBT_CMD_STOP:
            {
                send_msg.content = SUBT_EVT_STOPPED;
                if(p_subt->stats == SUBT_STATS_IDLE)
                {
                    mdl_broadcast_msg(&send_msg);
                    break;
                }
                stop_filter_vsb(p_subt);
                task_timeout = 0;

                if(p_subt->is_display == TRUE)
                {
                    subt_dec_hide_page_vsb(p_subt);
                }

                subt_db_reset_vsb(p_subt);

                if(SUBT_DYNAMIC_ALLOC_BUFFER && p_subt->p_dmx_buf != NULL)
                {
                #ifdef WARRIORS
                    p_subt->p_dmx_buf = (u8 *)((u32)p_subt->p_dmx_buf - 0x20000000);
                    mtos_free(p_subt->p_dmx_buf);
                #else
                    mtos_free(p_subt->p_dmx_buf);
                #endif
                    p_subt->p_dmx_buf = NULL;
                }

                if(SUBT_DYNAMIC_ALLOC_BUFFER && p_subt->p_pes_buf != NULL)
                {
                #ifdef WARRIORS
                    p_subt->p_pes_buf = (u8 *)((u32)p_subt->p_pes_buf - 0x20000000);
                    mtos_free(p_subt->p_pes_buf);
                #else
                    mtos_free(p_subt->p_pes_buf);
                #endif
                    p_subt->p_pes_buf = NULL;
                }

                if(SUBT_DYNAMIC_ALLOC_BUFFER && p_subt->p_render != NULL)
                {
                    subt_osd_deinit_vsb(p_subt->p_render);
                    mtos_free(p_subt->p_render);
                    p_subt->p_render = NULL;
                }

                p_subt->stats = SUBT_STATS_IDLE;
                mdl_broadcast_msg(&send_msg);
                break;
            }
            case SUBT_CMD_SET_PAUSE:
            {
                if(p_subt->stats == SUBT_STATS_IDLE
                    || (msg.para1 && p_subt->stats == SUBT_STATS_PAUSE)
                    || (!msg.para1 && p_subt->stats == SUBT_STATS_RUNNING))
                {
                    break;
                }


                if(((BOOL)msg.para1) == TRUE)
                {
                    stop_filter_vsb(p_subt);
                    task_timeout = 0;
                    p_subt->stats = SUBT_STATS_PAUSE;

                    if(SUBT_DYNAMIC_ALLOC_BUFFER
                        && p_subt->p_dmx_buf != NULL)
                    {
                      #ifdef WARRIORS
                        p_subt->p_dmx_buf = (u8 *)((u32)p_subt->p_dmx_buf - 0x20000000);
                        mtos_free(p_subt->p_dmx_buf);
                      #else
                        mtos_free(p_subt->p_dmx_buf);
                      #endif
                        p_subt->p_dmx_buf = NULL;
                        p_tspkt              = NULL;
                    }

                    if(SUBT_DYNAMIC_ALLOC_BUFFER && p_subt->p_pes_buf != NULL)
                    {
                    #ifdef WARRIORS
                        p_subt->p_pes_buf = (u8 *)((u32)p_subt->p_pes_buf - 0x20000000);
                        mtos_free(p_subt->p_pes_buf);
                    #else
                        mtos_free(p_subt->p_pes_buf);
                      #endif
                        p_subt->p_pes_buf = NULL;
                        p_pes_buf         = NULL;
                    }
                }
                else
                {
                    if(p_subt->p_dmx_buf == NULL)
                    {
                      #ifdef WARRIORS
                        p_subt->p_dmx_buf = mtos_malloc(SUBT_FILTER_BUF_SIZE);
                        p_subt->p_dmx_buf = (u8 *)((u32)p_subt->p_dmx_buf | 0xa0000000);
                      #else
                        p_subt->p_dmx_buf = mtos_malloc(SUBT_FILTER_BUF_SIZE);
                      #endif
                        MT_ASSERT(p_subt->p_dmx_buf != NULL);

                        OS_PRINTF("p_filter_buf 0x%x\n",p_subt->p_dmx_buf);
                    }

                    if(p_subt->p_pes_buf == NULL)
                    {
                      #ifdef WARRIORS
                        p_subt->p_pes_buf = mtos_malloc(SUBT_FILTER_BUF_SIZE);
                        p_subt->p_pes_buf = (u8 *)((u32)p_subt->p_pes_buf | 0xa0000000);
                      #else
                      p_subt->p_pes_buf = mtos_malloc(SUBT_PES_BUF_SIZE);
                      #endif
                        MT_ASSERT(p_subt->p_pes_buf != NULL);
                        memset(p_subt->p_pes_buf, 0x0, SUBT_PES_BUF_SIZE);
                    }
                    p_pes_buf = p_subt->p_pes_buf;

                    start_filter_vsb(p_subt, p_subt->pid);
#ifdef DMX_NEW_INTERFACE
                    if(NULL != p_subt->p_dmx_buf)
                    {
                      p_tspkt = dmx_next_ts_packet_vsb(p_subt);
                    }
                    else
                    {
                      MT_ASSERT(0);
                    }
#else
                    p_tspkt = p_subt->p_dmx_buf;
#endif
                    pes_copy_size = 0;
                    ts_cont_cnt   = 0xff;
                    task_timeout  = SUBT_TASK_TIMEOUT;
                    p_subt->stats = SUBT_STATS_RUNNING;
#ifdef DMX_NEW_INTERFACE
                    if(NULL == p_tspkt)
                    {
                        continue;
                    }
#endif
                }

                break;
            }
            case SUBT_CMD_SET_PAGE:
            {
                u16 composition_page = (msg.para2 >> 16) & 0xffff;
                u16 ancillary_page = msg.para2 & 0xffff;

                if(p_subt->stats == SUBT_STATS_IDLE)
                {
                    break;
                }

                stop_filter_vsb(p_subt);
                task_timeout = 0;

                if(p_subt->is_display == TRUE)
                {
                    subt_dec_hide_page_vsb(p_subt);
                }

                subt_db_reset_vsb(p_subt);
                subt_dec_set_page_id_vsb(p_subt, composition_page, ancillary_page);
                p_subt->pid = msg.para1;

                if(p_subt->stats == SUBT_STATS_RUNNING)
                {
                    start_filter_vsb(p_subt, p_subt->pid);
                    pes_copy_size = 0;
                    ts_cont_cnt   = 0xff;
                    task_timeout  = SUBT_TASK_TIMEOUT;
                }
                break;
            }
            case SUBT_CMD_SET_VIDEO_STD:
            {
                p_subt->vid_std = msg.para1;
                break;
            }

            default:
                MT_ASSERT(0);
                break;
        }
    }
}


RET_CODE subt_init_vsb(u32 prio, u32 stack_size, disp_layer_id_t layer)
{
    dvb_subtitle_vsb_t *p_subt  = NULL;
    u8             *p_stack = NULL;

//    p_subt = class_get_handle_by_id(SUBT_CLASS_ID);
//    if(p_subt != NULL)
//        return ERR_FAILURE;

    p_subt = mtos_malloc(sizeof(dvb_subtitle_vsb_t));
    memset(p_subt,0,sizeof(dvb_subtitle_vsb_t));


    class_register(SUBT_CLASS_ID, p_subt);


    p_subt->msg_hadle = mtos_messageq_create(SUBT_MSG_DEPTH, (u8 *)"SUBT");
    p_subt->lay_id_t = layer;

    p_stack = (u8 *)mtos_malloc(stack_size);
    memset(p_stack, 0, stack_size);

    if(mtos_task_create((u8 *)"Subt_task", subt_task_vsb, (void *)p_subt
        , prio, (u32 *)p_stack, stack_size) != TRUE)
    {
        MT_ASSERT(0);
    }

    return SUCCESS;
}


RET_CODE subt_set_msg_id_vsb(s32 msg_id)
{
    u32             sr = 0;
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    mtos_critical_enter(&sr);
    p_subt->callback_msg_id = msg_id;
    mtos_critical_exit(sr);

    return SUCCESS;
}

RET_CODE subt_set_page_vsb(u16 pid, u16 composition_page, u16 ancillary_page)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    msg.content = SUBT_CMD_SET_PAGE;
    msg.para1   = pid;
    msg.para2   = ((composition_page << 16) & 0xffff0000)
        + (ancillary_page & 0xffff);
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}

RET_CODE subt_start_vsb(void)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;


    msg.content = SUBT_CMD_START;
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}

RET_CODE subt_stop_vsb(void)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    msg.content = SUBT_CMD_STOP;
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}

RET_CODE subt_set_pause_vsb(BOOL is_pause)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    msg.content = SUBT_CMD_SET_PAUSE;
    msg.para1   = is_pause;
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}

RET_CODE subt_set_display_vsb(BOOL is_display)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    msg.content = SUBT_CMD_SET_DISPLAY;
    msg.para1   = is_display;
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}

RET_CODE subt_set_video_std_vsb(video_std_t std)
{
    os_msg_t        msg     = {0};
    dvb_subtitle_vsb_t  *p_subt = class_get_handle_by_id(SUBT_CLASS_ID);

    if(p_subt == NULL)
        return ERR_FAILURE;

    msg.content = SUBT_CMD_SET_VIDEO_STD;
    msg.para1   = std;
    mtos_messageq_send(p_subt->msg_hadle, &msg);
    p_subt->msg_cnt ++;

    return SUCCESS;
}
