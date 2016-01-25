/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "hal_base.h"
#include "hal_dma.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mem_manager.h"
#include "class_factory.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "common.h"
#include "lib_rect.h"
#include "dmx.h"
#include "vbi_inserter.h"
#include "display.h"
#include "mdl.h"
#include "vbi_api.h"

#include "ttx_lang_vsb.h"
#include "ttx_bcd_vsb.h"
#include "ttx_hamm_vsb.h"
#include "ttx_dec_vsb.h"
#include "ttx_format_vsb.h"
#include "ttx_db_vsb.h"
#include "ttx_render_vsb.h"

#define VBI_MSG_DEPTH               128
#define VBI_PES_BUF_SIZE            (64/2 * (KBYTES))
#define VBI_FILTER_BUF_SIZE         (512 * 188)
#define VBI_TASK_TIMEOUT            50
#ifdef WARRIORS
//#define DMX_INTERFACE
#else
#define DMX_INTERFACE
#endif

#define VBI_MAKE_U16(high, low)     ((((u16)(high) << 8) & 0xff00) + (low))

//#define DRV_VBI_DEBUG
#ifdef DRV_VBI_DEBUG
    #define VBI_PRINTF    OS_PRINTF
#else
    #ifdef WIN32
        #define VBI_PRINTF
    #else
        #define VBI_PRINTF  DUMMY_PRINTF
    #endif
#endif

#ifndef WARRIORS
static u16 hDmxLock = 0;
static u16 hAvLock = 0;
void OSSemPend(u16 *pevent, u16 timeout, u8 *err);
void OSSemPost(u16 *pevent);
#endif

extern const u8 _vbi_bit_reverse_vsb[256];
extern const u8 _vbi_hamm8_fwd_vsb[16];
extern const s8 _vbi_hamm8_inv_vsb[256];
extern const u8 _vbi_hamm24_fwd_0_vsb[256];
extern const u8 _vbi_hamm24_fwd_1_vsb[256];
extern const u8 _vbi_hamm24_fwd_2_vsb[4];
extern const s8 _vbi_hamm24_inv_par_vsb[3][256];
extern const u8 _vbi_hamm24_inv_d1_d4_vsb[64];
extern const s32 _vbi_hamm24_inv_err_vsb[64];

static u32 pre_font_size = 0;

/*!
   TODO: this function is odd parity decode

   \param[in] c
  */
static inline s32 vbi_unpar8_vsb(u32 c)
{
  if((_vbi_hamm24_inv_par_vsb[0][(u8)c] & 32) != 0)
  {
    return c & 127;
  }
  else
  {
    /*!
       The idea is to OR results together to find a parity
       error in a sequence, rather than a test and branch on
       each byte.
      */
    return -1;
  }
}

typedef struct dvb_vbi
{
    s32             callback_msg_id;
    s16             msg_cnt;
    s32             msg_hadle;

    vbi_inserter_device_t   *p_inserter;
    ttx_decoder_t           *p_ttx_dec;
    ttx_render_vsb_t            *p_ttx_render;
    ttx_osd_page_t          *p_osd_page;
    BOOL                    is_ttx_pause;

    video_std_t         vid_std;
    u16                 pid;
    dmx_chanid_t        dmx_channel;
    u8                  *p_pes_buf;
    u8                  *p_dmx_buf;
    void                *p_rgn;//ui create the osd region handle
#ifdef WARRIORS
    u8 *p_pal_font;
    u8 *p_ntsl_font;
    u8 *p_small_font;
    u8 *p_hd_font;
    BOOL  is_big_mem;
#else
    disp_layer_id_t sub_layer_id;
#endif
    ttx_region_pos_t region_pos;
} dvb_vbi_vsb_t;

/*!
  command for VBI
  */
typedef enum
{
    VBI_CMD_START_TTX,
    VBI_CMD_STOP_TTX,
    VBI_CMD_TTX_SHOW,
    VBI_CMD_TTX_HIDE,
    VBI_CMD_TTX_PAUSE,
    VBI_CMD_TTX_RESUME,
    VBI_CMD_TTX_KEY,

    VBI_CMD_START_INSER,
    VBI_CMD_STOP_INSER,

    VBI_CMD_SET_PID,
    VBI_CMD_SET_VIDEO_STD,
    VBI_CMD_SET_FONT_SIZE,
} vbi_cmd_t;

static const u8 bit_swap_vsb[256] =
{
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

static BOOL ttx_is_same_clock_vsb(u8 *p_new_header, u8 *p_old_header)
{
    s32 i = 0;

    for(i = 0; i < 8; p_new_header ++, p_old_header ++, i ++)
    {
        if(*p_new_header != *p_old_header
            && (vbi_unpar8_vsb(*p_new_header) | vbi_unpar8_vsb(*p_old_header)) >= 0)
            return FALSE;
    }

    return TRUE;
}

static void ttx_set_display_page_vsb(dvb_vbi_vsb_t *p_dvb_vbi
    , ttx_raw_t *p_in_page, u16 page_no, u16 sub_no)
{
    vbi_rc_t      rc;
    ttx_decoder_t *p_ttx_dec = p_dvb_vbi->p_ttx_dec;
    ttx_raw_t *p_page = NULL;


  //OS_PRINTF("ttx_set_display_page_vsb 0x%x  0x%x \n",page_no, sub_no);
    if(p_ttx_dec == NULL || !p_ttx_dec->is_display)
        return;

    p_ttx_dec->waiting_page.page_no = page_no;
    p_ttx_dec->waiting_page.sub_no  = sub_no;

    if(p_in_page == NULL)
    {
        rc = ttx_get_raw_page_vsb(p_ttx_dec, page_no, sub_no, 0xffff, &p_page);

        if(rc != VBI_RC_SUCCESS)
        {
            if(p_ttx_dec->is_subtitle_mode != TRUE)
            {
                p_ttx_dec->is_display_subtitle = FALSE;
                ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                    , page_no, sub_no, FALSE);
            }
            return;
        }
    }
    else
    {
      p_page = p_in_page;
    }

    if((p_page->control_bits & C6_SUBTITLE) != 0)
    {
        if(p_ttx_dec->is_display_subtitle != TRUE)
            ttx_render_clear_page_vsb(p_dvb_vbi->p_ttx_render, SCREEM_COLOR_INDEX);
        else if(p_ttx_dec->input_page_no == 0xffff)
            ttx_render_clear_header_vsb(p_dvb_vbi->p_ttx_render
                , SCREEM_COLOR_INDEX);

        ttx_osd_page_format_vsb(p_ttx_dec
            , p_dvb_vbi->p_osd_page, p_page, FALSE, FALSE);
        ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, p_dvb_vbi->p_ttx_dec->is_subtitle_mode
            , p_dvb_vbi->p_osd_page
            , 1, p_dvb_vbi->p_osd_page->rows
            , 0, p_dvb_vbi->p_osd_page->columns
            , FALSE, FALSE);
        p_ttx_dec->is_display_subtitle = TRUE;
    }
    else
    {
        if(p_ttx_dec->is_display_subtitle == TRUE)
            ttx_render_clear_page_vsb(p_dvb_vbi->p_ttx_render, SCREEM_COLOR_INDEX);

        ttx_osd_page_format_vsb(p_ttx_dec
            , p_dvb_vbi->p_osd_page, p_page, FALSE, TRUE);

        if(p_ttx_dec->input_page_no == 0xffff
            || (p_ttx_dec->input_page_no & 0xfff) == p_page->page_no)
        {
            ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                , p_dvb_vbi->p_osd_page
                , 0, p_dvb_vbi->p_osd_page->rows
                , 0, p_dvb_vbi->p_osd_page->columns
                , FALSE, FALSE);
        }
        else
        {
            ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                , p_dvb_vbi->p_osd_page
                , 0, 1, 8, p_dvb_vbi->p_osd_page->columns
                , FALSE, FALSE);

            ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                , p_dvb_vbi->p_osd_page
                , 1, p_dvb_vbi->p_osd_page->rows
                , 0, p_dvb_vbi->p_osd_page->columns
                , FALSE, FALSE);
        }

        p_ttx_dec->is_display_subtitle = FALSE;
    }

    p_ttx_dec->display_page.page_no = p_page->page_no;
    p_ttx_dec->display_page.sub_no  = p_page->sub_no;
    p_ttx_dec->waiting_page.page_no = p_page->page_no;
    p_ttx_dec->waiting_page.sub_no  = p_page->sub_no;
    //p_page->priority = TTX_PAGE_PRI_1;
    return;
}

static u32 ttx_dec_notify_vsb(ttx_notify_msg_t msg
    , u32 p1, u32 p2, void *p_context)
{
    static u32  old_ticks = 0;

    s32         i   = 0;
    s32         raw = 0;
    vbi_rc_t    rc;

    dvb_vbi_vsb_t       *p_dvb_vbi      = (dvb_vbi_vsb_t *)p_context;
    ttx_decoder_t   *p_ttx_dec      = p_dvb_vbi->p_ttx_dec;
    ttx_page_link_t *p_waiting_page = &p_ttx_dec->waiting_page;
    ttx_page_link_t *p_display_page = &p_ttx_dec->display_page;

    if(msg == TTX_NOTIFY_TIME_UPDATE)
    {
        ttx_page_link_t *p_link = (ttx_page_link_t *)p1;
        u8              *p_data = (u8 *)p2;

        if(p_ttx_dec->is_display != TRUE)
            return 0;

        if(p_display_page->page_no == p_waiting_page->page_no
            && p_display_page->sub_no == p_waiting_page->sub_no
            && (p_link->page_no & 0xf00) == (p_display_page->page_no & 0xf00))
        {
            s32         ticks_delta     = 0;
            ttx_raw_t   *p_current_page = NULL;

            ticks_delta = mtos_ticks_get() - old_ticks;
            if(ticks_delta < 50)
                return 0;

            old_ticks += ticks_delta;
            rc = ttx_get_raw_page_vsb(p_ttx_dec
                , p_ttx_dec->display_page.page_no
                , p_ttx_dec->display_page.sub_no
                , 0xffff
                , &p_current_page);

            if(rc == VBI_RC_SUCCESS
                && (ttx_is_same_clock_vsb(&p_data[32]
                    , &p_current_page->data.lop.raw[0][32])) != TRUE)
            {
                for(i = 32; i < p_dvb_vbi->p_osd_page->columns; i ++)
                {
                    p_current_page->data.lop.raw[0][i]
                        = p_data[i];

                    if((raw = vbi_unpar8_vsb(p_data[i])) > 0x1F)
                        p_dvb_vbi->p_osd_page->text[i].unicode = raw;
                }

                ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render,
                              p_dvb_vbi->p_ttx_dec->is_subtitle_mode
                              , p_dvb_vbi->p_osd_page
                              , 0, 1, 32, p_dvb_vbi->p_osd_page->columns
                              , FALSE, FALSE);
            }
        }

        return 0;
    }

    if(msg == TTX_NOTIFY_RECEIVED_PAGE)
    {
        ttx_raw_t *p_page = (ttx_raw_t *)p1;
        u32 i = 0;


        if(p_ttx_dec->is_display != TRUE)
            return 0;

        if(p_waiting_page->page_no == p_page->page_no
            && (p_waiting_page->sub_no == p_page->sub_no
                || p_waiting_page->sub_no == TTX_NULL_SUBPAGE
                || p_waiting_page->sub_no == TTX_ANY_SUBPAGE
                || p_waiting_page->sub_no == TTX_FIRST_SUBPAGE))
        {
            //fix bug 6242, the first display subpage should be index1.
            //Index0 stands for no subpage
            //use ttx_get_buf to store the subpage in order
            if(((p_page->control_bits & (C8_UPDATE | C4_ERASE_PAGE)) != 0)
                || ((p_display_page->page_no != p_waiting_page->page_no))
                || ((p_display_page->page_no == p_waiting_page->page_no)
                && (p_display_page->sub_no != p_waiting_page->sub_no)))
            {
           //   OS_PRINTF("p_page->control_bits 0x%x, pg no %x %x  sub no %x %x\n",
           //     p_page->control_bits, p_display_page->page_no, p_waiting_page->page_no,
           //    p_display_page->sub_no , p_waiting_page->sub_no);
                ttx_set_display_page_vsb(p_dvb_vbi, p_page,
                      p_waiting_page->page_no, p_waiting_page->sub_no);
            }

            return 0;
        }
        else if(((p_page->control_bits & (C5_NEWSFLASH | C6_SUBTITLE
                | C7_SUPPRESS_HEADER | C9_INTERRUPTED | C10_INHIBIT_DISPLAY))
                == 0)
            && !p_ttx_dec->is_display_subtitle)
        {
            if(p_display_page->page_no != p_waiting_page->page_no
                || p_display_page->sub_no != p_waiting_page->sub_no)
            {/*    roll header    */
                ttx_osd_page_format_vsb(p_ttx_dec
                    , p_dvb_vbi->p_osd_page, p_page, TRUE, FALSE);
                ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                    , p_dvb_vbi->p_osd_page
                    , 0, 1, 8, p_dvb_vbi->p_osd_page->columns
                    , FALSE, FALSE);
            }
            else if(1 &&(((p_page->control_bits & C13_PARTIAL_PAGE) == 0)
                || ((p_ttx_dec->display_page.page_no & 0xf00)
                    == (p_page->page_no & 0xf00))))
            {/*    update time    */
                s32         ticks_delta     = 0;
                ttx_raw_t   *p_current_page = NULL;

                ticks_delta = mtos_ticks_get() - old_ticks;
                if(ticks_delta < 50)
                    return 0;

                old_ticks += ticks_delta;


                rc = ttx_get_raw_page_vsb(p_ttx_dec
                    , p_ttx_dec->display_page.page_no
                    , p_ttx_dec->display_page.sub_no
                    , 0xffff
                    , &p_current_page);
                if(rc == VBI_RC_SUCCESS
                    && (ttx_is_same_clock_vsb(&p_page->data.lop.raw[0][32]
                        , &p_current_page->data.lop.raw[0][32])) != TRUE)
                {
                    for(i = 32; i < p_dvb_vbi->p_osd_page->columns; i ++)
                    {
                        p_current_page->data.lop.raw[0][i]
                            = p_page->data.lop.raw[0][i];

                        if((raw = vbi_unpar8_vsb(p_page->data.lop.raw[0][i]))
                                > 0x1F)
                            p_dvb_vbi->p_osd_page->text[i].unicode = raw;
                    }
                    ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                        , p_dvb_vbi->p_osd_page
                        , 0, 1, 32, p_dvb_vbi->p_osd_page->columns
                        , FALSE, FALSE);
                }
            }

            return 0;
        }
    }

    return 0;
}

static void ttx_key_proc_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u32 key)
{
#ifdef WARRIORS
    BOOL is_big_mem = vbi_is_big_mem();
#endif
    ttx_decoder_t *p_ttx_dec = p_dvb_vbi->p_ttx_dec;

    switch(key)
    {
        case TTX_KEY_PAGE_UP:
        case TTX_KEY_PAGE_DOWN:
        {
            u16         new_page_no = 0;
            u16         cur_page_no = 0;
            BOOL        b_reset = FALSE;

            cur_page_no = p_ttx_dec->waiting_page.page_no;

            if(key == TTX_KEY_PAGE_UP)
            {
                ttx_get_next_page_up_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                new_page_no = p_ttx_dec->input_page_no;
            }
            else
            {
                ttx_get_prev_page_down_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                new_page_no = p_ttx_dec->input_page_no;
            }

            b_reset = ttx_buf_flush_vsb(p_ttx_dec, cur_page_no, new_page_no);
            if(TRUE == b_reset)
            {
                //different page section
                ttx_dec_reset_vsb(p_ttx_dec);
            }
            p_ttx_dec->is_buf_update = TRUE;
            p_ttx_dec->input_page_no = new_page_no;
            ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                , new_page_no, 0, FALSE);

            ttx_set_display_page_vsb(p_dvb_vbi
                , NULL, new_page_no, TTX_FIRST_SUBPAGE);
            break;
        }
        case TTX_KEY_UP:
        case TTX_KEY_DOWN:
        {
            vbi_rc_t    rc;
            u16         new_page_no = 0;
            u16         cur_page_no = 0;
            u16         cur_page_section = 0;
            BOOL        b_reset = FALSE;

            cur_page_no = p_ttx_dec->waiting_page.page_no;
            rc = ttx_get_page_section_vsb(p_ttx_dec, cur_page_no, &cur_page_section);

            if(key == TTX_KEY_UP)
            {
#ifdef WARRIORS
                if(is_big_mem)
                  ttx_get_next_raw_page_no_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                else
#endif
                  ttx_get_next_page_no_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                new_page_no = p_ttx_dec->input_page_no;
            }
            else
            {
#ifdef WARRIORS
                if(is_big_mem)
                  ttx_get_prev_raw_page_no_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                else
#endif 
                  ttx_get_prev_page_no_vsb(p_ttx_dec, cur_page_no, &(p_ttx_dec->input_page_no));
                new_page_no = p_ttx_dec->input_page_no;
            }

            b_reset = ttx_buf_flush_vsb(p_ttx_dec, cur_page_no, new_page_no);
            if(TRUE == b_reset)
            {
                //different page section
                ttx_dec_reset_vsb(p_ttx_dec);
            }
            p_ttx_dec->is_buf_update = TRUE;
            p_ttx_dec->input_page_no = new_page_no;
            ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                , new_page_no, 0, FALSE);

            ttx_set_display_page_vsb(p_dvb_vbi
                , NULL, new_page_no, TTX_FIRST_SUBPAGE);
            break;
        }
        case TTX_KEY_RIGHT:
        case TTX_KEY_LEFT:
        {
            vbi_rc_t  rc;
            ttx_raw_t *p_page = NULL;
            //fix bug6242 subpage increase in order
            u8  cur_in_decimal = 0;
            u8  next_in_decimal = 0;
            u8  prev_in_decimal = 0;
            s16 sub_no = 0;

            sub_no = p_ttx_dec->waiting_page.sub_no;
            cur_in_decimal = ((sub_no & 0xF0) >> 4) * 10
                            + ((sub_no & 0xF));

            if(key == TTX_KEY_LEFT)
            {
                p_ttx_dec->is_buf_update = TRUE;
                p_ttx_dec->input_page_no = p_ttx_dec->waiting_page.page_no;
                rc = ttx_get_prev_raw_subpage_vsb(p_ttx_dec
                    , p_ttx_dec->waiting_page.page_no
                    , p_ttx_dec->waiting_page.sub_no
                    , 0xffff
                    , &p_page);
                if(NULL == p_page)
                {
                    return;
                }
                sub_no = p_page->sub_no;
                prev_in_decimal = ((sub_no & 0xF0) >> 4) * 10
                              + ((sub_no & 0xF));
                {//not support over roll now
                    if(sub_no != p_ttx_dec->waiting_page.sub_no)
                    ttx_set_display_page_vsb(p_dvb_vbi,
                                        NULL,
                                        p_ttx_dec->waiting_page.page_no,
                                        prev_in_decimal);
                }
            }
            else
            {
                p_ttx_dec->is_buf_update = TRUE;
                p_ttx_dec->input_page_no = p_ttx_dec->waiting_page.page_no;
                rc = ttx_get_next_raw_subpage_vsb(p_ttx_dec
                    , p_ttx_dec->waiting_page.page_no
                    , p_ttx_dec->waiting_page.sub_no
                    , 0xffff
                    , &p_page);
                if(NULL == p_page)
                {
                    return;
                }
                sub_no = p_page->sub_no;
                next_in_decimal = ((sub_no & 0xF0) >> 4) * 10
                              + ((sub_no & 0xF));
                {
                    //we do not know which is the last subpage,set the first subpage if reaches last
                    //this may occurs when last subpage reaches.
                    //Or the next subpage packet not decoded
                    if(sub_no != p_ttx_dec->waiting_page.sub_no)
                    ttx_set_display_page_vsb(p_dvb_vbi
                        , NULL, p_ttx_dec->waiting_page.page_no, next_in_decimal);
                }
            }

            if((rc != VBI_RC_SUCCESS) || (NULL == p_page))
                return;

            break;
        }
        case TTX_KEY_RED:
        case TTX_KEY_GREEN:
        case TTX_KEY_YELLOW:
        case TTX_KEY_CYAN:
        case TTX_KEY_INDEX:
        {
            u8              index = 0;
            ttx_osd_page_t  *p_osd_page = p_dvb_vbi->p_osd_page;


            index = (key == TTX_KEY_RED)
                ? 0 : (key == TTX_KEY_GREEN)
                ? 1 : (key == TTX_KEY_YELLOW)
                ? 2 : (key == TTX_KEY_CYAN)
                ? 3 : 5;
            if(p_osd_page->nav_link[index].page_no > 0
                && p_osd_page->nav_link[index].page_no < TTX_NULL_PAGE_NO)//fix bug5978
            {
                ttx_set_display_page_vsb(p_dvb_vbi
                    , NULL
                    , p_osd_page->nav_link[index].page_no
                    , p_osd_page->nav_link[index].sub_no);

                p_ttx_dec->input_page_no = 0xffff;
            }
            break;
        }
        case TTX_KEY_TRANSPARENT:
        {
            p_dvb_vbi->p_osd_page->user_screen_opacity -= 25;
            if(p_dvb_vbi->p_osd_page->user_screen_opacity < 0)
                p_dvb_vbi->p_osd_page->user_screen_opacity = 100;

            ttx_render_set_bg_transparent_vsb(p_dvb_vbi->p_ttx_render
                , p_dvb_vbi->p_osd_page
                , p_dvb_vbi->p_osd_page->user_screen_opacity);
        }
        default:
            if(key >= TTX_KEY_0 && key <= TTX_KEY_9)
            {
                u16     page_no = 0;
                u32     cnt     = 0;
                BOOL    b_flush = FALSE;

                if(0 == (p_ttx_dec->input_page_no & 0xf000))
                    p_ttx_dec->input_page_no = p_ttx_dec->input_page_no | 0xf000;

                page_no = p_ttx_dec->input_page_no & 0xfff;
                cnt     = (p_ttx_dec->input_page_no & 0xf000) >> 12;

                if(cnt > 3)
                    cnt = 3;

                if(cnt != 3
                    || (key != TTX_KEY_0 && key != TTX_KEY_9))
                {
                    cnt --;
                    page_no = (u16)((page_no & ~(0xf << (cnt * 4)))
                        | ((key - TTX_KEY_0) << (cnt * 4)));
                }

                page_no += (u16)(cnt << 12);
                p_ttx_dec->input_page_no = page_no;

                if(cnt == 0)
                {
                    ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                        , page_no, 0, FALSE);
                    b_flush = ttx_buf_flush_vsb(p_ttx_dec,
                                p_ttx_dec->display_page.page_no, page_no);
                    if(b_flush)
                    {
                        ttx_dec_reset_vsb(p_ttx_dec);
                        p_ttx_dec->input_page_no = page_no;
                    }
                    p_ttx_dec->is_buf_update = TRUE;
                    ttx_set_display_page_vsb(p_dvb_vbi
                        , NULL, page_no, TTX_FIRST_SUBPAGE);
                }
                else
                {
                    ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                        , page_no, 0, FALSE);
                }
            }
            break;
    }
}

static vbi_rc_t vbi_pes_parse_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u8 *p_pes_pkt)
{
#define PACKET_LENGTH_OFFSET        4
#define PTS_FLAG_OFFSET             7
#define PTS_FLAG_MASK               0x80
#define PTS_OFFSET                  9
#define HEADER_DATA_LENGTH_OFFSET   8

    u8              *p_pes_data_filed = NULL;
    u8              *p_vbi_data_filed = NULL;
    u8              *p_pes_pkt_last_byte = NULL;
    u16             pes_packet_length = 0;
    u8              pts_flag = 0;
    u32             pts = 0;
    u8              vbi_data_id = 0;
    u8              data_unit_id = 0;
    u8              data_unit_length = 0;
    ttx_decoder_t   *p_ttx_dec  = p_dvb_vbi->p_ttx_dec;
    u32             i = 0;

    if(p_pes_pkt[0] != 0x00 || p_pes_pkt[1] != 0x00
        || p_pes_pkt[2] != 0x01 || p_pes_pkt[3] != 0xbd)
    {/*    'stream_id' can only occur at transport packet content boundary. */
        VBI_PRINTF("VBI!: Bad boundary #1.\n");

        return VBI_RC_BAD_BAOUNDARY;
    }

    if(p_pes_pkt[8] != 0x24)
    {/*    'PES_header_data_length' must be 0x24    */
        VBI_PRINTF("VBI!: bad PES_header_data_length: 0x%2x\n", p_pes_pkt[8]);

        return VBI_RC_INVALID_DATA;
    }

    p_pes_data_filed    = p_pes_pkt + 9 + 0x24;    /* point to pes_data_filed */
    p_vbi_data_filed    = p_pes_data_filed + 1;    /* point to data_unit_id   */
    pes_packet_length   = VBI_MAKE_U16(p_pes_pkt[4], p_pes_pkt[5]);
    p_pes_pkt_last_byte = p_pes_pkt + 6 + pes_packet_length;


    VBI_PRINTF("p_pes_packet = %d\n", p_pes_pkt);
    VBI_PRINTF("p_pes_data_filed = %d\n", p_pes_data_filed);
    VBI_PRINTF("p_vbi_data_filed = %d\n", p_vbi_data_filed);
    VBI_PRINTF("pes_packet_length = %d\n", pes_packet_length);
    VBI_PRINTF("p_pes_packet_last_byte = %d\n", p_pes_pkt_last_byte);

    vbi_data_id = p_pes_data_filed[0];
    if((vbi_data_id < 0x10)
        || (vbi_data_id > 0x1F && vbi_data_id < 0x99)
        || (vbi_data_id > 0x9B))
    {/* 'data_identifier' was NOT for VBI. */
        VBI_PRINTF("VBI!:    bad data_identifier: 0x%2x\n", vbi_data_id);
        return VBI_RC_INVALID_DATA;
    }

    pts_flag = p_pes_pkt[7] & PTS_FLAG_MASK;
    if(pts_flag != 0)
    {
        pts   = p_pes_pkt[PTS_OFFSET + 0] & 0x0e;
        pts <<= 7;
        pts  += p_pes_pkt[PTS_OFFSET + 1];
        pts <<= 8;
        pts  += p_pes_pkt[PTS_OFFSET + 2] & 0xfe;
        pts <<= 7;
        pts  += p_pes_pkt[PTS_OFFSET + 3];
        pts <<= 6;
        pts  += ((p_pes_pkt[PTS_OFFSET + 3] & 0xfe) >> 2);
    }
    else
    {
        pts = vbi_get_stc();
    }

#ifdef WARRIORS
    if(p_dvb_vbi->p_inserter != NULL)
    {
        RET_CODE r = SUCCESS;
        r = vbi_inserter_dev_inserte_data(p_dvb_vbi->p_inserter
            , pts, p_pes_data_filed, p_pes_pkt_last_byte);
        if(r != SUCCESS)
            VBI_PRINTF("VBI!:    inserter parse data error\n");
    }
#endif

    if(p_ttx_dec == NULL || p_dvb_vbi->is_ttx_pause)
        return VBI_RC_SUCCESS;

    while(1)
    {
        /*
            VBI Packet Data
            ---------------
            in:        p_vbi_data_filed
                    pts
        */

        data_unit_id      = p_vbi_data_filed[0];
        data_unit_length  = p_vbi_data_filed[1];

         /*    step over the data_unit_length, point to vbi_data_filed    */
        p_vbi_data_filed += 2;
        VBI_PRINTF("VBI: data_unit_id = 0x%x, data_unit_length = %d\n"
            , data_unit_id, data_unit_length);

        if((p_vbi_data_filed + data_unit_length) > p_pes_pkt_last_byte)
        {
            VBI_PRINTF("VBI!: Bad boundary #2.\n");
            break;
        }

        /* 0x02:    EBU Teletext non-subtitle data, transcode as EBU Teletext */
        /* 0x03:    EBU Teletext subtitle data, transcode as EBU Teletext     */
        /* 0xc0:    Inverted Teletext, transcode as EBU Teletext with an]
                    inverted framing code                                     */
        /* 0xC3:    VPS, transcode as VPS                                     */
        /* 0xC4:    WSS, transcode as WSS                                     */
        /* 0xC5:    Closed Captioning, transcode as Closed Captioning         */
        /* 0xC6:    monochrome 4:2:2 samples, transcode as raw VBI data       */
        if(1)
        {
            //whole length exclude clock-run-in 2bytes, framing code 1bytes
            u8 ttx_data[TTX_PACKET_LENGTH - 3];

            if(((data_unit_id == 0x02 || data_unit_id == 0x03)
                 && p_vbi_data_filed[1] == 0xe4)
                || (data_unit_id == 0xc0 && p_vbi_data_filed[1] == 0x1b))
            {
                for(i = 0; i < (TTX_PACKET_LENGTH - 3); i ++)
                    ttx_data[i] = bit_swap_vsb[p_vbi_data_filed[2 + i]];

                ttx_dec_data_process_vsb(p_ttx_dec, ttx_data, data_unit_id);

                if((p_ttx_dec->display_page.page_no != p_ttx_dec->input_page_no) &&
                   (p_ttx_dec->incoming_page_no != 0xffff) &&
                   (NULL != p_dvb_vbi->p_ttx_render) &&
                   (TRUE == p_ttx_dec->is_buf_update) &&//init state
                   (TRUE != p_ttx_dec->is_subtitle_mode))//no teletext subtitle
                {
                    ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                        , p_ttx_dec->incoming_page_no, 0, TRUE);
                }
            }
            else if(((data_unit_id == 0x02 || data_unit_id == 0x03)
                    && p_vbi_data_filed[1] == 0x27)
                || (data_unit_id == 0xc0 && p_vbi_data_filed[1] == 0xd8))
            {
                for(i = 0; i < (TTX_PACKET_LENGTH - 3); i ++)
                    ttx_data[i] = p_vbi_data_filed[2 + i];

                ttx_dec_data_process_vsb(p_ttx_dec, ttx_data, data_unit_id);

                if((p_ttx_dec->display_page.page_no != p_ttx_dec->input_page_no) &&
                   (p_ttx_dec->incoming_page_no != 0xffff) &&
                   (NULL != p_dvb_vbi->p_ttx_render))
                {
                    ttx_render_draw_page_no_vsb(p_dvb_vbi->p_ttx_render
                        , p_ttx_dec->incoming_page_no, 0, TRUE);
                }
            }
        }

        p_vbi_data_filed += data_unit_length;

        if(p_vbi_data_filed >= p_pes_pkt_last_byte)
          break;
    }

    return VBI_RC_SUCCESS;
}

#ifdef DMX_INTERFACE
#ifndef WARRIORS
static u8 s_p_pkt_txt[188+20] = {0};
#endif
static u8 *dmx_next_packet_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    RET_CODE     ret    = ERR_FAILURE;
    dmx_device_t *p_dev    = NULL;
    u32 pkt_size = 0;
    u8 *p_pkt = NULL;
#ifndef WARRIORS
    u8 err = 0;
#endif

    if(p_dvb_vbi->dmx_channel == DMX_INVALID_CHANNEL_ID)
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
      ret = dmx_si_chan_get_data(p_dev, p_dvb_vbi->dmx_channel, &p_pkt, &pkt_size);
      if(SUCCESS != ret)
      {
        ///TODO: maybe the tasksleep should be changed
        VBI_PRINTF("VBI:no input data\n");
        mtos_task_sleep(1);
        
        OSSemPost(&hDmxLock);

        return NULL;
      }
    }
#ifdef WARRIORS
    return p_pkt;
#else
    memcpy(s_p_pkt_txt, p_pkt, pkt_size); 

    OSSemPost(&hDmxLock);

    return s_p_pkt_txt;
#endif
}
#endif

#ifdef WARRIORS
static u8 *next_pkt_vsb(u8 *p_pkt, u8 *p_buf)
{
#ifdef WIN32

    if(p_pkt == (p_buf + (VBI_FILTER_BUF_SIZE - 188)))
        p_pkt = p_buf;
    else
        p_pkt += 188;

    return p_pkt;
#else
    p_buf = (u8 *)((u32)p_buf);
    p_pkt = (u8 *)((u32)p_pkt);
    if(p_pkt == (p_buf + (VBI_FILTER_BUF_SIZE - 188)))
        p_pkt = p_buf;
    else
        p_pkt += 188;

    return p_pkt;
#endif
}
#endif

static void start_data_filter_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u16 pid)
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
        || (pid == p_dvb_vbi->pid && p_dvb_vbi->dmx_channel != DMX_INVALID_CHANNEL_ID))
    {
        return;
    }

    for(i = 0; i < 512; i ++)
    {
        p_dvb_vbi->p_dmx_buf[i * 188] = 0;
    }

    p_dvb_vbi->pid = pid;

#ifndef WARRIORS
    OSSemPend(&hDmxLock, 0, &err);
#endif

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    slot_t.in     = DMX_INPUT_EXTERN0;
    slot_t.pid   = pid;
    slot_t.type = DMX_CH_TYPE_TSPKT;
    ret = dmx_si_chan_open(p_dev, &slot_t, &(p_dvb_vbi->dmx_channel));
    if(ret != SUCCESS)
    {
      OS_PRINTF("VBI:open vbi channel failed!\n");
      MT_ASSERT(0);
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif

      return;
    }

    ret = dmx_si_chan_set_buffer(p_dev,
          p_dvb_vbi->dmx_channel, p_dvb_vbi->p_dmx_buf, VBI_FILTER_BUF_SIZE);
    if(ret != SUCCESS)
    {
      OS_PRINTF("VBI:set filter buffer failed!\n");
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
    filter_param_t.ts_packet_mode = DMX_EIGHT_MODE;
    ret = dmx_si_chan_set_filter(p_dev, p_dvb_vbi->dmx_channel, &filter_param_t);
    if(ret != SUCCESS)
    {
      OS_PRINTF("VBI:set filter failed!\n");
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

    ret = dmx_chan_start(p_dev, p_dvb_vbi->dmx_channel);
    if(ret != SUCCESS)
    {
      OS_PRINTF("VBI:start channel  failed!\n");
#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
      return;
    }

#ifndef WARRIORS
      OSSemPost(&hDmxLock);
#endif
}

static void stop_data_filter_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    RET_CODE     ret    = ERR_FAILURE;
    dmx_device_t *p_dev    = NULL;
#ifndef WARRIORS
    u8 err = 0;
#endif

#ifndef WARRIORS
    OSSemPend(&hDmxLock, 0, &err);
    OSSemPend(&hAvLock, 0, &err);
#endif
    if(p_dvb_vbi->dmx_channel == DMX_INVALID_CHANNEL_ID)
    {
#ifndef WARRIORS
        OSSemPost(&hAvLock);
        OSSemPost(&hDmxLock);
#endif
        return;
    }

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, p_dvb_vbi->dmx_channel);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(p_dev, p_dvb_vbi->dmx_channel);
    MT_ASSERT(ret == SUCCESS);
    //should set pid invalid, othewise can not show ttx when switch channel
    p_dvb_vbi->pid = 0x1fff;
    p_dvb_vbi->dmx_channel = DMX_INVALID_CHANNEL_ID;

#ifndef WARRIORS
    OSSemPost(&hAvLock);
    OSSemPost(&hDmxLock);
#endif
}

#ifdef WARRIORS
static vbi_rc_t ttx_show_vsb(dvb_vbi_vsb_t *p_dvb_vbi, BOOL is_subtitle, u32 page_no)
#else
static vbi_rc_t ttx_show_vsb(dvb_vbi_vsb_t *p_dvb_vbi, BOOL  is_subtitle, u32 para2)
#endif
{
    vbi_rc_t        rc = VBI_RC_SUCCESS;
    ttx_decoder_t   *p_ttx_dec = p_dvb_vbi->p_ttx_dec;
#ifdef WARRIORS
    u32             sub_no     = TTX_FIRST_SUBPAGE;
#else
    u32             sub_no     = para2 & 0xffff;
    u32 page_no = (para2 >> 16) & 0xffff;
#endif

    if(p_ttx_dec == NULL)
        return VBI_RC_FAILED;

    if(p_dvb_vbi->p_osd_page == NULL)
    {
        p_dvb_vbi->p_osd_page
            = mtos_malloc(sizeof(ttx_osd_page_t));
        MT_ASSERT(p_dvb_vbi->p_osd_page != NULL);
        ttx_osd_page_init_vsb(p_dvb_vbi->p_ttx_dec
            , p_dvb_vbi->p_osd_page);
    }

    if(p_dvb_vbi->p_ttx_render == NULL)
    {
        p_dvb_vbi->p_ttx_render
            = mtos_malloc(sizeof(ttx_render_vsb_t));
        MT_ASSERT(p_dvb_vbi->p_ttx_render != NULL);
        memset(p_dvb_vbi->p_ttx_render,0,sizeof(ttx_render_vsb_t));
        //max double size character
        if(!p_dvb_vbi->p_ttx_render->p_char_buf)
        p_dvb_vbi->p_ttx_render->p_char_buf =
#ifdef WARRIORS
          (u8 *)mtos_malloc(TTX_CHAR_MAX_H * TTX_CHAR_MAX_W);
#else
          (u8 *)mtos_malloc(TTX_CHAR_MAX_H * TTX_CHAR_MAX_W * 10);
#endif
        MT_ASSERT(p_dvb_vbi->p_ttx_render->p_char_buf != NULL);

        memcpy(p_dvb_vbi->p_ttx_render->clut
            , p_ttx_dec->def_mag.extension.color_map
            , sizeof(p_ttx_dec->def_mag.extension.color_map));

        memcpy(p_dvb_vbi->p_ttx_render->clut
                + sizeof(p_ttx_dec->def_mag.extension.color_map) / 4
            , p_ttx_dec->def_mag.extension.color_map
            , sizeof(p_ttx_dec->def_mag.extension.color_map));
        p_dvb_vbi->p_ttx_render->font_size = pre_font_size;
#ifndef WARRIORS
        p_dvb_vbi->p_ttx_render->sub_layer_id = p_dvb_vbi->sub_layer_id;
#endif
    }

    //fix bug6319
    if(vbi_get_region_pos() == TTX_REGION_OUT_DECODER)
      p_dvb_vbi->p_ttx_render->p_osd_hdl = p_dvb_vbi->p_rgn;

    if(p_ttx_dec->is_display)
    {
        if(p_ttx_dec->is_subtitle_mode != is_subtitle)
        {
            if(p_ttx_dec->is_subtitle_mode)
            {
                ttx_render_delete_sub_region_vsb(p_dvb_vbi->p_ttx_render);
            }
            else
            {
                ttx_render_delete_region_vsb(p_dvb_vbi->p_ttx_render);
            }
        }
    }

    if(!p_ttx_dec->is_display || p_ttx_dec->is_subtitle_mode != is_subtitle)
    {
        if(is_subtitle)
        {
            rc = ttx_render_create_sub_region_vsb(p_dvb_vbi->p_ttx_render
                , p_dvb_vbi->vid_std);
        }
        else
        {
            rc = ttx_render_create_region_vsb(p_dvb_vbi->p_ttx_render
                , p_dvb_vbi->vid_std);
        }

        if (rc != VBI_RC_SUCCESS)
        {
            mtos_free(p_dvb_vbi->p_osd_page);
            p_dvb_vbi->p_osd_page = NULL;
            mtos_free(p_dvb_vbi->p_ttx_render);
            p_dvb_vbi->p_ttx_render = NULL;
            return rc;
        }
    }

    if(is_subtitle == TRUE)
    {
        // for bug 11417
        //p_dvb_vbi->p_ttx_render->page_h = p_dvb_vbi->p_ttx_render->page_h + 10;
        ttx_render_clear_page_vsb(p_dvb_vbi->p_ttx_render, SCREEM_COLOR_INDEX);
        //p_dvb_vbi->p_ttx_render->page_h = p_dvb_vbi->p_ttx_render->page_h -10;

        p_ttx_dec->is_display_subtitle = TRUE;
        sub_no                         = TTX_ANY_SUBPAGE;
        if(page_no >= 0x900)
            MT_ASSERT(0);
    }
    else
    {
        //ttx_render_clear_page_vsb(p_dvb_vbi->p_ttx_render, SCREEM_COLOR_INDEX);
        ttx_render_clear_header_vsb(p_dvb_vbi->p_ttx_render, 40 + TTX_BLACK);

        p_ttx_dec->is_display_subtitle = FALSE;
        sub_no                         = TTX_FIRST_SUBPAGE;
        if(page_no >= 0x900)
            page_no = p_ttx_dec->initial_page.page_no;
    }

    p_ttx_dec->is_display       = TRUE;
    p_ttx_dec->is_subtitle_mode = is_subtitle;

    if(is_subtitle == TRUE)
    {
      ttx_dec_reset_vsb(p_ttx_dec);
      p_ttx_dec->is_buf_update = TRUE;
      p_ttx_dec->input_page_no = page_no;
    }

    ttx_set_display_page_vsb(p_dvb_vbi, NULL, (u16)page_no, (u16)sub_no);

    return VBI_RC_SUCCESS;
}

static void ttx_hide_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    ttx_decoder_t *p_ttx_dec = p_dvb_vbi->p_ttx_dec;


    if(!p_ttx_dec->is_display)
        return;

    p_ttx_dec->is_display = FALSE;

    if(p_ttx_dec->is_subtitle_mode)
    {
        ttx_render_delete_sub_region_vsb(p_dvb_vbi->p_ttx_render);
    }
    else
    {
        ttx_render_delete_region_vsb(p_dvb_vbi->p_ttx_render);
    }

    if(p_dvb_vbi->p_osd_page != NULL)
    {
        mtos_free(p_dvb_vbi->p_osd_page);
        p_dvb_vbi->p_osd_page = NULL;
    }

    if(p_dvb_vbi->p_ttx_render != NULL)
    {
        if(NULL != p_dvb_vbi->p_ttx_render->p_char_buf)
        {
            mtos_free(p_dvb_vbi->p_ttx_render->p_char_buf);
            p_dvb_vbi->p_ttx_render->p_char_buf = NULL;
        }
        mtos_free(p_dvb_vbi->p_ttx_render);
        p_dvb_vbi->p_ttx_render = NULL;
    }

    p_ttx_dec->display_page.page_no = TTX_NULL_PAGE_NO;
    p_ttx_dec->display_page.sub_no  = TTX_NULL_SUBPAGE;
    p_ttx_dec->waiting_page.page_no = 0x100;
    p_ttx_dec->waiting_page.sub_no  = TTX_FIRST_SUBPAGE;
    //bug 15405
    //ttx_dec_reset_vsb(p_ttx_dec);
}

static void ttx_start_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u32 max_page_num, u32 max_sub_page_num)
{
    u16 mag_max_page_no = 0;
    vbi_rc_t ret;

    if(p_dvb_vbi->p_ttx_dec != NULL)
        return;

    p_dvb_vbi->p_ttx_dec = mtos_malloc(sizeof(ttx_decoder_t));
    MT_ASSERT(p_dvb_vbi->p_ttx_dec != NULL);
    memset(p_dvb_vbi->p_ttx_dec, 0, sizeof(ttx_decoder_t));
    p_dvb_vbi->p_ttx_dec->p_raw = mtos_malloc(sizeof(ttx_raw_t) * max_page_num);
    MT_ASSERT(p_dvb_vbi->p_ttx_dec->p_raw != NULL);
    memset(p_dvb_vbi->p_ttx_dec->p_raw, 0, sizeof(ttx_raw_t) * max_page_num);
    //sub page buffer
    p_dvb_vbi->p_ttx_dec->p_raw_sub = mtos_malloc(sizeof(ttx_raw_t) * max_sub_page_num);
    MT_ASSERT(p_dvb_vbi->p_ttx_dec->p_raw_sub != NULL);
    memset(p_dvb_vbi->p_ttx_dec->p_raw_sub, 0 , sizeof(ttx_raw_t) * max_sub_page_num);

    p_dvb_vbi->p_ttx_dec->max_page_num = max_page_num;
    p_dvb_vbi->p_ttx_dec->max_sub_page_num = max_sub_page_num;
    //one magzine have max 100 pages
    mag_max_page_no = (max_page_num > TTX_PAGE_STEPS)
                        ? TTX_PAGE_STEPS : max_page_num;
    //change into 0x mode, for example, 32 change into 0x32
    mag_max_page_no = ((mag_max_page_no / 10) << 4)|(mag_max_page_no % 10);
    p_dvb_vbi->p_ttx_dec->mag_max_page_no = mag_max_page_no;
    ret = ttx_get_page_section_vsb(p_dvb_vbi->p_ttx_dec,
            0x899, &(p_dvb_vbi->p_ttx_dec->mag_max_page_section));

    ttx_dec_start_vsb(p_dvb_vbi->p_ttx_dec, ttx_dec_notify_vsb, p_dvb_vbi);
}

static void ttx_stop_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    ttx_decoder_t *p_ttx_dec = p_dvb_vbi->p_ttx_dec;


    if(p_dvb_vbi->p_ttx_dec == NULL)
        return;

    if(p_dvb_vbi->p_osd_page != NULL)
    {
        mtos_free(p_dvb_vbi->p_osd_page);
        p_dvb_vbi->p_osd_page = NULL;
    }

    if(p_dvb_vbi->p_ttx_render != NULL)
    {
        if(p_ttx_dec->is_display)
        {
            if(p_ttx_dec->is_subtitle_mode)
                ttx_render_delete_sub_region_vsb(p_dvb_vbi->p_ttx_render);
            else
                ttx_render_delete_region_vsb(p_dvb_vbi->p_ttx_render);
        }

        mtos_free(p_dvb_vbi->p_ttx_render);
        p_dvb_vbi->p_ttx_render = NULL;
    }

    ttx_dec_stop_vsb(p_dvb_vbi->p_ttx_dec);
    mtos_free(p_dvb_vbi->p_ttx_dec->p_raw);
    p_dvb_vbi->p_ttx_dec->p_raw = NULL;
    mtos_free(p_dvb_vbi->p_ttx_dec->p_raw_sub);
    p_dvb_vbi->p_ttx_dec->p_raw_sub = NULL;
    mtos_free(p_dvb_vbi->p_ttx_dec);
    p_dvb_vbi->p_ttx_dec = NULL;
}

static void ttx_pause_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    if(p_dvb_vbi->p_ttx_dec == NULL || p_dvb_vbi->is_ttx_pause)
        return;

    p_dvb_vbi->is_ttx_pause = TRUE;
}

static void ttx_resume_vsb(dvb_vbi_vsb_t *p_dvb_vbi)
{
    if(p_dvb_vbi->p_ttx_dec == NULL || !p_dvb_vbi->is_ttx_pause)
        return;

    p_dvb_vbi->is_ttx_pause = FALSE;
}

#ifdef WARRIORS
static void inserter_start_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u32 param)
{
    if(p_dvb_vbi->p_inserter != NULL)
        return;

    p_dvb_vbi->p_inserter = dev_find_identifier(NULL, DEV_IDT_TYPE
        , SYS_DEV_TYPE_VBI_INSERTER);

    vbi_inserter_dev_start(p_dvb_vbi->p_inserter, param);

    vbi_inserter_dev_set_vid_std(p_dvb_vbi->p_inserter, p_dvb_vbi->vid_std);
}

static void inserter_stop_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u32 param)
{
    if(p_dvb_vbi->p_inserter == NULL)
        return;

    vbi_inserter_dev_stop(p_dvb_vbi->p_inserter, param);
    p_dvb_vbi->p_inserter = NULL;
}
#endif

static void set_pid_vsb(dvb_vbi_vsb_t *p_dvb_vbi, u16 pid)
{
#ifdef WARRIORS
    if(p_dvb_vbi->p_ttx_dec == NULL && p_dvb_vbi->p_inserter == NULL)
#else
    if(p_dvb_vbi->p_ttx_dec == NULL)
#endif

        return;

    if(p_dvb_vbi->pid == pid)
        return;

    stop_data_filter_vsb(p_dvb_vbi);

    if(p_dvb_vbi->p_ttx_dec)
        ttx_dec_reset_vsb(p_dvb_vbi->p_ttx_dec);

    start_data_filter_vsb(p_dvb_vbi, pid);
}

static void vbi_task_vsb(void *p_data)
{
    os_msg_t    msg;
    BOOL        rc = FALSE;
    u8          *p_pes_buf  = NULL;
    u8          *p_tspkt    = NULL;
    dvb_vbi_vsb_t   *p_dvb_vbi   = (dvb_vbi_vsb_t *)p_data;
    u32         task_timeout = 0;
    u32         pes_copy_size = 0;
    u8          ts_cont_cnt   = 0xff;

    p_dvb_vbi->callback_msg_id = -1;
    p_dvb_vbi->msg_cnt      = 0;
#ifdef WARRIORS
    p_dvb_vbi->p_inserter   = NULL;
#endif
    p_dvb_vbi->p_ttx_dec    = NULL;
    p_dvb_vbi->p_ttx_render = NULL;
    p_dvb_vbi->p_osd_page   = NULL;
    p_dvb_vbi->is_ttx_pause = TRUE;

    p_dvb_vbi->vid_std      = VID_STD_PAL;
    p_dvb_vbi->pid          = (u16)INVALID;
    p_dvb_vbi->dmx_channel  = DMX_INVALID_CHANNEL_ID;
    p_dvb_vbi->p_pes_buf    = NULL;
    p_dvb_vbi->p_dmx_buf = NULL;

    while(1)
    {
        rc = mtos_messageq_receive(p_dvb_vbi->msg_hadle, &msg, task_timeout);
        if(rc != TRUE)
        {
#ifdef WARRIORS
            if(p_dvb_vbi->p_inserter != NULL
                ||(p_dvb_vbi->p_ttx_dec != NULL && !p_dvb_vbi->is_ttx_pause))
#else
            if(p_dvb_vbi->p_ttx_dec != NULL && !p_dvb_vbi->is_ttx_pause)
#endif
            {
                u8  af_ctrl = 0;            /*    adaption_field_ctrl   */
                u8  af_len  = 0;            /*    adaption_field_len    */
                u8  tmp     = 0;
#ifdef WARRIORS
                u8  *p_next_pkt = NULL;
#endif

                ///TODO:  page flash
 #ifndef WARRIORS
               if(p_dvb_vbi->p_ttx_dec
                    && !p_dvb_vbi->p_ttx_dec->is_subtitle_mode
                    && p_dvb_vbi->p_ttx_dec->is_display == TRUE
                    && p_dvb_vbi->p_ttx_dec->is_display_subtitle != TRUE)
                {
                    static u32        old_ticks = 0;
                    s32                ticks_delta;

                    ticks_delta = mtos_ticks_get() - old_ticks;
                    if(ticks_delta >= 100)
                    {
                        old_ticks += ticks_delta;

                        if(p_dvb_vbi->p_ttx_dec->waiting_page.page_no
                           == p_dvb_vbi->p_osd_page->page_no
                                && p_dvb_vbi->p_ttx_dec->waiting_page.sub_no
                                //|| p_dvb_vbi->p_ttx_dec->waiting_page.sub_no
                                   == p_dvb_vbi->p_osd_page->sub_no)
                        {
                            //fix bug6139, no subtitle
                            ttx_render_draw_page_vsb(p_dvb_vbi->p_ttx_render, FALSE
                                , p_dvb_vbi->p_osd_page
                                , 1, p_dvb_vbi->p_osd_page->rows
                                , 0, p_dvb_vbi->p_osd_page->columns
                                , FALSE, (old_ticks / 100 & 1));
                        }
                    }
                }
#endif

                while(p_dvb_vbi->msg_cnt <= 0)
                {
                    if(p_tspkt[0] != 0x47)
                    {
#ifndef WARRIORS
                        p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                        break;
                    }

#ifdef WARRIORS
                    if(task_timeout != VBI_TASK_TIMEOUT / 2)
                    {
                        p_next_pkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
                        if(p_next_pkt[0] != 0x47)
                        {
                            task_timeout = VBI_TASK_TIMEOUT / 2;
                            break;
                        }
                    }
                    else
#endif
                    {
#ifdef WARRIORS
                        task_timeout = VBI_TASK_TIMEOUT;
#else
                        task_timeout = VBI_TASK_TIMEOUT >> 1;
#endif
                    }

                    //transport_error_indicator
                    if((p_tspkt[1] & 0x80) != 0)
                    {
#ifndef DMX_INTERFACE
                        p_tspkt[0]  = 0;
                        p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                        p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                        ts_cont_cnt = 0xff;
                        VBI_PRINTF("VBI:    error packet, skip\n");
                        continue;
                    }

                    //payload_unit_start_indicator, 1 stands for first valid packet
                    if(pes_copy_size == 0 && !(p_tspkt[1] & 0x40))
                    {
#ifndef DMX_INTERFACE
                        p_tspkt[0]  = 0;
                        p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                        p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                        ts_cont_cnt = 0xff;
                        continue;
                    }

                    ///TODO:
                    //transport_scrambling_control 00 stands for not scramble
                    if(0 && (p_tspkt[3] & 0xc0) != 0)
                    {
#ifndef DMX_INTERFACE
                        p_tspkt[0]  = 0;
                        p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                        p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                        ts_cont_cnt   = 0xff;
                        pes_copy_size = 0;
                        VBI_PRINTF("VBI:    scrambling, skip \n");
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
#ifndef DMX_INTERFACE
                        p_tspkt[0] = 0;
                        p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                        p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
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
                            ts_cont_cnt = 0;
                        else
                            ts_cont_cnt ++;

                        if(ts_cont_cnt != tmp)
                        {
                            VBI_PRINTF("VBI!:    continuity counter error.\n");
                            ts_cont_cnt   = 0xff;
                            pes_copy_size = 0;

#ifndef DMX_INTERFACE
                            p_tspkt[0] = 0;
                            p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                            p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                            continue;
                        }
                    }

                    //next first start begin , check whether old pes packet is ready
                    if(p_tspkt[1] & 0x40 && pes_copy_size != 0)
                    {
                        if(pes_copy_size > 6)
                        {
                            u32 pes_data_length = 0;

                            pes_data_length = (((u16)p_pes_buf[4] << 8)
                                & 0xff00) + p_pes_buf[5] + 6;

                            if(pes_copy_size >= pes_data_length)
                            {
                                VBI_PRINTF("VBI:    new PES, length = %d\n"
                                    , pes_copy_size);
                                vbi_pes_parse_vsb(p_dvb_vbi, p_pes_buf);
                                pes_copy_size = 0;
                                ts_cont_cnt   = 0xff;
                            }
                            else
                            {
                                VBI_PRINTF("VBI!:    data length error #1"
                                    " [%d - %d]\n"
                                    , pes_data_length, pes_copy_size);
                                pes_copy_size = 0;
                                ts_cont_cnt   = 0xff;
                            }
                        }
                        else
                        {
                            VBI_PRINTF("VBI!:    data length error #2 [%d]\n"
                                , pes_copy_size);
                            pes_copy_size = 0;
                            ts_cont_cnt   = 0xff;
                        }
                    }

                    //184 comes from 188 - 4(ts packe header)
                    if((pes_copy_size + 184 - af_len) > VBI_PES_BUF_SIZE)
                    {
                        VBI_PRINTF("VBI!:    PES buffer overflow\n");
                        pes_copy_size = 0;
                        continue;
                    }

                    if(af_len < 184)
                    {
                        memcpy(p_pes_buf + pes_copy_size
                            , (void *)((u32)p_tspkt + 4 + af_len)
                            , 184 - af_len);

                        pes_copy_size += 184 - af_len;
                    }
                    else
                    {
                        VBI_PRINTF("VBI!:    TS Packet Error #4\n");
                    }

                    if(pes_copy_size > 6)
                    {
                        u32 pes_data_length = (((u16)p_pes_buf[4] << 8)
                            & 0xff00) + p_pes_buf[5] + 6;

                        if(pes_copy_size >= pes_data_length)
                        {
                            vbi_pes_parse_vsb(p_dvb_vbi, p_pes_buf);
                            pes_copy_size = 0;
                            ts_cont_cnt   = 0xff;
                        }
                    }
#ifndef DMX_INTERFACE
                    p_tspkt[0] = 0;
                    p_tspkt = next_pkt_vsb(p_tspkt
                                            , p_dvb_vbi->p_dmx_buf);
#else
                    p_tspkt = dmx_next_packet_vsb(p_dvb_vbi);
#endif
                }
            }
                
            continue;
        }
        else
        {
            if(p_dvb_vbi->msg_cnt > 0)
                p_dvb_vbi->msg_cnt --;
        }

        switch(msg.content)
        {
            case VBI_CMD_TTX_KEY:
                if(p_dvb_vbi->p_ttx_dec != NULL
                    && !p_dvb_vbi->p_ttx_dec->is_subtitle_mode)
                    ttx_key_proc_vsb(p_dvb_vbi, msg.para1);
                break;

            case VBI_CMD_TTX_SHOW:
                if (p_dvb_vbi->p_inserter != NULL)
                {
                    //inserter_stop_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                }
                //fix bug6448 bug6571
                if(0 == msg.para1)
                {
                    p_dvb_vbi->p_ttx_dec->is_buf_update = FALSE;

                    p_dvb_vbi->p_ttx_dec->initial_page.function = TTX_PAGE_FUNC_LOP;
                    p_dvb_vbi->p_ttx_dec->initial_page.page_no  = 0x100;
                    p_dvb_vbi->p_ttx_dec->initial_page.sub_no   = TTX_FIRST_SUBPAGE;

                    p_dvb_vbi->p_ttx_dec->display_page.function = TTX_PAGE_FUNC_LOP;
                    p_dvb_vbi->p_ttx_dec->display_page.page_no  = TTX_NULL_PAGE_NO;
                    p_dvb_vbi->p_ttx_dec->display_page.sub_no   = TTX_NULL_SUBPAGE;

                    p_dvb_vbi->p_ttx_dec->waiting_page.function = TTX_PAGE_FUNC_LOP;
                    p_dvb_vbi->p_ttx_dec->waiting_page.page_no  = 0x100;
                    p_dvb_vbi->p_ttx_dec->waiting_page.sub_no   = TTX_FIRST_SUBPAGE;

                    p_dvb_vbi->p_ttx_dec->input_page_no         = 0xffff;
                    p_dvb_vbi->p_ttx_dec->incoming_page_no      = 0xffff;
                    //ttx_dec_reset_vsb(p_dvb_vbi->p_ttx_dec);
                }

                ttx_show_vsb(p_dvb_vbi, msg.para1, msg.para2);

                if (p_dvb_vbi->p_inserter != NULL)
                {
                    //inserter_start_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                }
                break;

            case VBI_CMD_TTX_HIDE:
                if (p_dvb_vbi->p_inserter != NULL)
                {
                    //inserter_stop_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                }

                ttx_hide_vsb(p_dvb_vbi);
                if(msg.para1 != 0||p_dvb_vbi->p_ttx_dec->is_display)
                {
                    os_msg_t  m = {0};

                    m.content = VBI_TTX_HIDED;
                    mdl_broadcast_msg(&m);
                }
                if (p_dvb_vbi->p_inserter != NULL)
                {
                    //inserter_start_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                }
                break;

            case VBI_CMD_START_TTX://malloc dmx,pes buffer and start ttx task
                OS_PRINTF(">>>VBI_CMD_START_TTX!!!\n");
                if(p_dvb_vbi->p_dmx_buf == NULL)
                {
#ifdef WARRIORS
                    p_dvb_vbi->p_dmx_buf = mtos_malloc(VBI_FILTER_BUF_SIZE);
                    p_dvb_vbi->p_dmx_buf = (u8 *)((u32)p_dvb_vbi->p_dmx_buf | 0xa0000000);
#else
                    p_dvb_vbi->p_dmx_buf = mtos_malloc(VBI_FILTER_BUF_SIZE);
#endif
                    MT_ASSERT(p_dvb_vbi->p_dmx_buf != NULL);
                    memset(p_dvb_vbi->p_dmx_buf, 0x0, VBI_FILTER_BUF_SIZE);
                }

                if(p_dvb_vbi->p_pes_buf == NULL)
                {
                    p_dvb_vbi->p_pes_buf = mtos_malloc(VBI_PES_BUF_SIZE);
                    MT_ASSERT(p_dvb_vbi->p_pes_buf != NULL);
                    memset(p_dvb_vbi->p_pes_buf, 0x0, VBI_PES_BUF_SIZE);
                }

#ifdef WARRIORS
                if(msg.para1 >= 800)
                  p_dvb_vbi->is_big_mem = TRUE;
                else
                  p_dvb_vbi->is_big_mem = FALSE;
#endif

                ttx_start_vsb(p_dvb_vbi, msg.para1, msg.para2);

                p_dvb_vbi->is_ttx_pause = FALSE;
                break;

            case VBI_CMD_STOP_TTX:
                OS_PRINTF(">>>>>>VBI_CMD_STOP_TTX!!!\n");
                ttx_stop_vsb(p_dvb_vbi);

                //if(p_dvb_vbi->p_inserter == NULL)
                {
                    task_timeout = 0;
                    stop_data_filter_vsb(p_dvb_vbi);
                }

                if(p_dvb_vbi->p_dmx_buf != NULL)
                {
                #ifdef WARRIORS
                    p_dvb_vbi->p_dmx_buf = (u8 *)((u32)p_dvb_vbi->p_dmx_buf - 0x20000000);
                    mtos_free(p_dvb_vbi->p_dmx_buf);
                #else
                    mtos_free(p_dvb_vbi->p_dmx_buf);
                #endif                
                    p_dvb_vbi->p_dmx_buf = NULL;
                }

                if(p_dvb_vbi->p_pes_buf != NULL)
                {
                    mtos_free(p_dvb_vbi->p_pes_buf);
                    p_dvb_vbi->p_pes_buf = NULL;
                }
                {
                    os_msg_t  m = {0};
                    m.content = VBI_TTX_STOPPED;
                    mdl_broadcast_msg(&m);
                }
                break;

            case VBI_CMD_TTX_PAUSE:
                ttx_pause_vsb(p_dvb_vbi);
#ifdef WARRIORS
                if(p_dvb_vbi->p_inserter == NULL)
                    task_timeout = 0;
#endif
                break;

            case VBI_CMD_TTX_RESUME:
                ttx_resume_vsb(p_dvb_vbi);

                task_timeout  = VBI_TASK_TIMEOUT;
                pes_copy_size = 0;
                ts_cont_cnt   = 0xff;

                break;

#ifdef WARRIORS
            case VBI_CMD_START_INSER:
                inserter_start_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                pes_copy_size = 0;
                ts_cont_cnt   = 0xff;

                break;

            case VBI_CMD_STOP_INSER:
                inserter_stop_vsb(p_dvb_vbi, VBI_ALL_INSERTION);
                if(p_dvb_vbi->p_ttx_dec == NULL
                    || p_dvb_vbi->is_ttx_pause)
                {
                    task_timeout = 0;
                    stop_data_filter_vsb(p_dvb_vbi);
                }

                break;
#endif

            case VBI_CMD_SET_PID://set pid should be after start
                OS_PRINTF(">>>>>>VBI_CMD_SET_PID!!!\n");
                if(msg.para1 == 0)
                    break;
                set_pid_vsb(p_dvb_vbi, (u16)msg.para1);

#ifdef WARRIORS
                if(p_dvb_vbi->p_dmx_buf == NULL)
                {
                #ifdef WARRIORS  
                    p_dvb_vbi->p_dmx_buf = mtos_malloc(VBI_FILTER_BUF_SIZE);
                    p_dvb_vbi->p_dmx_buf = (u8 *)((u32)p_dvb_vbi->p_dmx_buf | 0xa0000000);
                #else
                    p_dvb_vbi->p_dmx_buf = mtos_malloc(VBI_FILTER_BUF_SIZE);
                #endif
                    MT_ASSERT(p_dvb_vbi->p_dmx_buf != NULL);
                    memset(p_dvb_vbi->p_dmx_buf, 0x0, VBI_FILTER_BUF_SIZE);
                }
 
                if(p_dvb_vbi->p_pes_buf == NULL)
                {
                    p_dvb_vbi->p_pes_buf = mtos_malloc(VBI_PES_BUF_SIZE);
                    MT_ASSERT(p_dvb_vbi->p_pes_buf != NULL);
                    memset(p_dvb_vbi->p_pes_buf, 0x0, VBI_PES_BUF_SIZE);
                }
#else
                MT_ASSERT(NULL != p_dvb_vbi->p_pes_buf);
                MT_ASSERT(NULL != p_dvb_vbi->p_dmx_buf);
#endif

                p_pes_buf     = p_dvb_vbi->p_pes_buf;
                p_tspkt       = p_dvb_vbi->p_dmx_buf;

                task_timeout  = VBI_TASK_TIMEOUT;
                pes_copy_size = 0;
                ts_cont_cnt   = 0xff;
                break;

            case VBI_CMD_SET_VIDEO_STD:
                p_dvb_vbi->vid_std = msg.para1;
#ifdef WARRIORS
                if(p_dvb_vbi->p_inserter != NULL)
                {
                    vbi_inserter_dev_set_vid_std(p_dvb_vbi->p_inserter
                        , p_dvb_vbi->vid_std);
                }
#endif
                break;
            case VBI_CMD_SET_FONT_SIZE:
              if(p_dvb_vbi->p_ttx_render != NULL)
              {
                  p_dvb_vbi->p_ttx_render->font_size = msg.para1;
              }
              else
              {
                    pre_font_size = msg.para1;
              }
               break;

            default:
                MT_ASSERT(0);
                break;
        }
    }
}

#ifdef WARRIORS
RET_CODE vbi_init_vsb(u32 prio, u32 stack_size)
#else
RET_CODE vbi_init_vsb(u32 prio, u32 stack_size, disp_layer_id_t layer)
#endif
{
    dvb_vbi_vsb_t *p_dvb_vbi = NULL;
    u8        *p_stack   = NULL;


    p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);
    if(p_dvb_vbi != NULL)
        return ERR_FAILURE;


    p_dvb_vbi = mtos_malloc(sizeof(dvb_vbi_vsb_t));
    MT_ASSERT(p_dvb_vbi != NULL);
    class_register(VBI_CLASS_ID, p_dvb_vbi);
#ifndef WARRIORS
    p_dvb_vbi->sub_layer_id = layer;
#endif

    p_dvb_vbi->region_pos = TTX_REGION_OUT_DECODER;
    p_dvb_vbi->msg_hadle = mtos_messageq_create(VBI_MSG_DEPTH, (u8 *)"VBI");

    p_stack = (u8 *)mtos_malloc(stack_size);
    memset(p_stack, 0, stack_size);

    if(mtos_task_create((u8 *)"VBI_task" , vbi_task_vsb
        , (void *)p_dvb_vbi
        , prio
        , (u32 *)p_stack
        , stack_size) != TRUE)
    {
        MT_ASSERT(0);
    }

    return SUCCESS;
}

RET_CODE vbi_set_msg_id_vsb(s32 msg_id)
{
    u32       sr         = 0;
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    mtos_critical_enter(&sr);
    p_dvb_vbi->callback_msg_id = msg_id;
    mtos_critical_exit(sr);

    return SUCCESS;
}

RET_CODE vbi_set_region_handle_vsb(void *p_rgn)
{
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    p_dvb_vbi->p_rgn = p_rgn;

    return SUCCESS;
}

RET_CODE vbi_set_pid_vsb(u16 pid)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;


    p_dvb_vbi->msg_cnt ++;
    msg.content  = VBI_CMD_SET_PID;
    msg.para1    = pid;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_ttx_start_vsb(u32 max_page_num, u32 max_sub_page_num)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content  = VBI_CMD_START_TTX;
    msg.para1    = max_page_num;
    msg.para2    = max_sub_page_num;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_ttx_stop_vsb(void)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content  = VBI_CMD_STOP_TTX;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_ttx_pause_vsb(void)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_TTX_PAUSE;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_ttx_resume_vsb(void)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_TTX_RESUME;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

#ifdef WARRIORS
RET_CODE vbi_ttx_show_vsb(BOOL is_subtitle, u32 page_no)
#else
RET_CODE vbi_ttx_show_vsb(BOOL is_subtitle, u32 page_no, u32 sub_no)
#endif
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;
    //some ttx_subtitle do not have magzine number, but in fact it is in mag8
    if((page_no & 0xf00) == 0)
        page_no = (page_no & 0x0ff) + 0x800;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_TTX_SHOW;
    msg.para1   = is_subtitle;
#ifdef WARRIORS
    msg.para2   = page_no;
#else
    msg.para2   = ((page_no << 16) & 0xffff0000) | (sub_no & 0xffff);
#endif
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_ttx_hide_vsb(BOOL sync)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;


    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_TTX_HIDE;
    msg.para1 = (u32)sync;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_post_ttx_key_vsb(ttx_key_t key)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);


    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    if(p_dvb_vbi->msg_cnt < VBI_MSG_DEPTH / 2)
    {
        p_dvb_vbi->msg_cnt ++;
        msg.content  = VBI_CMD_TTX_KEY;
        msg.para1    = key;
        mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);
    }

    return SUCCESS;
}

RET_CODE vbi_inserter_start_vsb(void)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content  = VBI_CMD_START_INSER;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_inserter_stop_vsb(void)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_STOP_INSER;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_set_video_std_vsb(video_std_t std)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_SET_VIDEO_STD;
    msg.para1   = std;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_set_font_size_vsb(ttx_font_size_t font_size)
{
    os_msg_t  msg        = {0};
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    if(p_dvb_vbi == NULL)
        return ERR_FAILURE;

    p_dvb_vbi->msg_cnt ++;
    msg.content = VBI_CMD_SET_FONT_SIZE;
    msg.para1   = font_size;
    mtos_messageq_send(p_dvb_vbi->msg_hadle, &msg);

    return SUCCESS;
}

RET_CODE vbi_set_region_pos(ttx_region_pos_t pos)
{
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);
    
    p_dvb_vbi->region_pos = pos;
    
    return SUCCESS;
}

ttx_region_pos_t vbi_get_region_pos(void)
{
    
    dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);
    
    return  p_dvb_vbi->region_pos;
}


#ifdef WARRIORS
u8 *vbi_get_font(ttx_font_type_t src)
{
  dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);
  
  switch(src)
  {
    case TTX_FONT_SRC_PAL:
      return p_dvb_vbi->p_pal_font;
    case TTX_FONT_SRC_NTSL:
      return p_dvb_vbi->p_ntsl_font;
    case TTX_FONT_SRC_SMALL:
      return p_dvb_vbi->p_small_font;
    case TTX_FONT_SRC_HD:
      return p_dvb_vbi->p_hd_font;
  }
  return NULL;

}

void vbi_set_font_src(ttx_font_src_t *p_font)
{
  dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

  p_dvb_vbi->p_pal_font = p_font->p_pal_font;
  p_dvb_vbi->p_ntsl_font = p_font->p_ntsl_font;
  p_dvb_vbi->p_small_font = p_font->p_small_font;
  p_dvb_vbi->p_hd_font = p_font->p_hd_font;
}  

void vbi_reset_font_src(void)
{
  dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

  p_dvb_vbi->p_pal_font = NULL;
  p_dvb_vbi->p_ntsl_font = NULL;
  p_dvb_vbi->p_small_font = NULL;
  p_dvb_vbi->p_hd_font = NULL;
}

BOOL vbi_is_big_mem(void)
{
  dvb_vbi_vsb_t *p_dvb_vbi = class_get_handle_by_id(VBI_CLASS_ID);

    return p_dvb_vbi->is_big_mem;
}
#endif
