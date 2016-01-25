/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "hal_dma.h"
#include "hal_misc.h"

#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "lib_rect.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"

#include "drv_misc.h"
#include "mdl.h"
#include "vbi_api.h"
#include "ttx_lang_vsb.h"
#include "ttx_dec_vsb.h"
#include "ttx_format_vsb.h"
#include "ttx_render_vsb.h"

#ifdef __LINUX__
void flush_dcache_all()
{
}
#endif
/*!
 cp the whole charator to a buffer, then do gpe ops, not every pixel do gpe ops.
  */
#define TTX_RENDER_OPTIMIZE

#ifndef WARRIORS
/*!
  ABC
  */
extern const u8 wstfont2_bits_pal_vsb[];

/*!
  ABC
  */
extern const u8 wstfont2_bits_ntsc_vsb[];

/*!
  Small size font
  */
extern const u8 wstfont2_bits_small_vsb[];
#endif

/*!
  position calc, the unicode mapped to xbm
  example, unicode 0x20 mapped to 0x20-0x20 = 0 position in xbm font
  example, unicode 0x21 mapped to 0x21-0x20 = 1 position in xbm font
  */
static u32 unicode_wstfont2_vsb(u32 c, s32 italic)
{
    static const unsigned short specials[] =
    {
        0x01B5, 0x2016, 0x01CD, 0x01CE, 0x0229, 0x0251, 0x02DD, 0x02C6,
        0x02C7, 0x02C9, 0x02CA, 0x02CB, 0x02CD, 0x02CF, 0x02D8, 0x02D9,
        0x02DA, 0x02DB, 0x02DC, 0x2014, 0x2018, 0x2019, 0x201C, 0x201D,
        0x20A0, 0x2030, 0x20AA, 0x2122, 0x2126, 0x215B, 0x215C, 0x215D,
        0x215E, 0x2190, 0x2191, 0x2192, 0x2193, 0x25A0, 0x266A, 0xE800,
        0xE75F
    };
    const u32   invalid = 357;
    u32         i = 0;

    if(c < 0x0180)
    {
        if(c < 0x0080)
        {
            if(c < 0x0020)
                return invalid;
            /* A workround way to fix bug 10433, because the font in font library 0x5f is
              *  not correct, this method can point to right charactor.
            */
            else if(c == 0x7f)
                c = 357;
            else /* %3 Basic Latin (ASCII) 0x0020 ... 0x007F */
                c = c - 0x0020 + 0 * 32;
        }
        else if(c < 0x00A0)
            return invalid;
        else /* %3 Latin-1 Supplement, Latin Extended-A 0x00A0 ... 0x017F */
            c = c - 0x00A0 + 3 * 32;
    }
    else if(c < 0xEE00)
    {
        if(c < 0x0460)
        {
            if(c < 0x03D0)
            {
                if(c < 0x0370)
                {
                    for(i = 0; i < sizeof(specials) / sizeof(specials[0]); i ++)
                    {
                        if(specials[i] == c)
                        {
                            if(italic != 0)
                                return i + 41 * 32;
                            else
                                return i + 10 * 32;
                        }
                    }
                }
                else /* %5 Greek 0x0370 ... 0x03CF */
                {
                    c = c - 0x0370 + 12 * 32;
                }
            }
            else if(c < 0x0400)
                return invalid;
            else /* %5 Cyrillic 0x0400 ... 0x045F */
                c = c - 0x0400 + 15 * 32;
        }
        else if(c < 0x0620)
        {
            if(c < 0x05F0)
            {
                if(c < 0x05D0)
                    return invalid;
                else /* %6 Hebrew 0x05D0 ... 0x05EF */
                    return c - 0x05D0 + 18 * 32;
            }
            else if(c < 0x0600)
                return invalid;
            else /* %6 Arabic 0x0600 ... 0x061F */
                return c - 0x0600 + 19 * 32;
        }
        else if(c >= 0xE600 && c < 0xE740)
        {
            return c - 0xE600 + 19 * 32; /* %6 Arabic (TTX) */
        }
        else
        {
            for(i = 0; i < sizeof(specials) / sizeof(specials[0]); i ++)
            {
                if(specials[i] == c)
                {
                    if(italic != 0)
                        return i + 41 * 32;
                    else
                        return i + 10 * 32;
                }
            }
        }
    }
    else if(c < 0xEF00)
    { /* %3 G1 Graphics */
        return (c ^ 0x20) - 0xEE00 + 23 * 32;
    }
    else if(c < 0xF000)
    { /* %4 G3 Graphics */
        return c - 0xEF20 + 27 * 32;
    }
    else /* 0xF000 ... 0xF7FF reserved for DRCS */
        return invalid;

    if(italic != 0)
        return c + 31 * 32;
    else
        return c;

    return invalid;
}

#if TTX_SUPPORT_DRCS
static void draw_drcs_vsb(ttx_render_vsb_t *p_render
    , u8 *p_pen, u8 color_offset, s32 top, s32 left
    , u8 *p_font, s32 glyph, ttx_size_t size)
{
    u8  *p_src = NULL;
    u32 col = 0;
    s32 x = 0, y = 0;
    u8  *p_pix_buf = NULL;
    u32 stride = 0;
    s32 char_h = 0;

#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_HD)
        char_h = TTX_CHAR_H_HD;
    else if(p_render->video_std == VID_STD_PAL)
#else
    if(p_render->video_std == VID_STD_PAL)
#endif
        char_h = TTX_CHAR_H_PAL;
    else
        char_h = TTX_CHAR_H_NTSC;

    p_src = p_font + glyph * 60;
    p_pen = p_pen + color_offset;

    stride     = p_render->region_w;
    p_pix_buf  = (u8 *)((top & 1)
        ? p_render->p_even_addr : p_render->p_odd_addr);
    p_pix_buf += left + (top / 2) * p_render->region_w;

    switch(size)
    {
        case TTX_NORMAL_SIZE:
            for(y = 0; y < char_h; top ++, y ++)
            {
                p_pix_buf  = (u8 *)((top & 1)
                    ? p_render->p_even_addr : p_render->p_odd_addr);
                p_pix_buf += left + (top / 2) * stride;

                for(x = 0; x < 12; p_src ++, x += 2)
                {
                    p_pix_buf[x + 0] = p_pen[*p_src & 0xF];
                    p_pix_buf[x + 1] = p_pen[*p_src >> 4];
                }
            }
            break;

        case TTX_DOUBLE_HEIGHT2:
            p_src += 30;

        case TTX_DOUBLE_HEIGHT:
            for(y = 0; y < char_h / 2; top += 2, y ++)
            {
                p_pix_buf  = (u8 *)((top & 1)
                    ? p_render->p_even_addr : p_render->p_odd_addr);
                p_pix_buf += left + (top / 2) * stride;

                for(x = 0; x < 12; p_src ++, x += 2)
                {
                    col = p_pen[*p_src & 15];
                    p_pix_buf[x + 0]          = col;
                    p_pix_buf[x + stride + 0] = col;

                    col = p_pen[*p_src >> 4];
                    p_pix_buf[x + 1]          = col;
                    p_pix_buf[x + stride + 1] = col;

                }
            }
            break;

        case TTX_DOUBLE_WIDTH:
            for(y = 0; y < char_h; top ++, y ++)
            {
                p_pix_buf  = (u8 *)((top & 1)
                    ? p_render->p_even_addr : p_render->p_odd_addr);
                p_pix_buf += left + (top / 2) * stride;

                for(x = 0; x < 12 * 2; p_src ++, x += 4)
                {
                    col = p_pen[*p_src & 15];
                    p_pix_buf[x + 0] = col;
                    p_pix_buf[x + 1] = col;

                    col = p_pen[*p_src >> 4];
                    p_pix_buf[x + 2] = col;
                    p_pix_buf[x + 3] = col;
                }
            }
            break;

        case TTX_DOUBLE_SIZE2:
            p_src += 30;

        case TTX_DOUBLE_SIZE:
            for(y = 0; y < char_h / 2; top += 2, y ++)
            {
                p_pix_buf  = (u8 *)((top & 1)
                    ? p_render->p_even_addr : p_render->p_odd_addr);
                p_pix_buf += left + (top / 2) * stride;

                for(x = 0; x < 12 * 2; p_src ++, x += 4)
                {
                    col = p_pen[*p_src & 15];
                    p_pix_buf[x + 0]          = col;
                    p_pix_buf[x + 1]          = col;
                    p_pix_buf[x + stride + 0] = col;
                    p_pix_buf[x + stride + 1] = col;


                    col = p_pen[*p_src >> 4];
                    p_pix_buf[x + 2]          = col;
                    p_pix_buf[x + 3]          = col;
                    p_pix_buf[x + stride + 2] = col;
                    p_pix_buf[x + stride + 3] = col;
                }
            }
            break;

        default:
            break;
    }
}
#endif

#if 0
static void draw_wstfont(ttx_render_vsb_t *p_render)
{
    u8 *p_src, p_src;
    s32 x, y, z;
    u8  *p_pix_buf;
    u32 stride = p_render->w;
    u32 top = 0;;

    for(y = 0; y < p_render->h; y ++)
    {
        if((y & 1) != 0)
        {
            p_pix_buf = ((u8 *)p_render->p_odd_addr) + (y / 2) * stride;
        }
        else
        {
            p_pix_buf = ((u8 *)p_render->p_even_addr) + (y / 2) * stride;
        }

        p_src = wstfont2_bits + y * wstfont2_width / 8;

        for(x = 0; x < wstfont2_width / 8; x ++)
        {
            p_src = *(p_src + x);
            for(z = 0; z < 8; p_src >>= 1, z ++)
                p_pix_buf[x * 8 + z] = (p_src & 1) ? TTX_BLACK : TTX_WHITE;
        }
    }
}
#endif

/*
 * Draw blank character.
 */
static void draw_blank_vsb(ttx_render_vsb_t *p_render
    , u32 top, u32 left, u8 color, s32 cw, s32 ch)
{
    RET_CODE ret = 0;
    rect_t rect;

    if(TRUE == p_render->is_sub)
    {
      MT_ASSERT(NULL != p_render->p_sub_hdl);
    }
    else
    {
      MT_ASSERT(NULL != p_render->p_osd_hdl);
    }

    rect.left = left;
    rect.top = top;
    rect.right = left + cw;
    rect.bottom = top + ch;
    ret = gpe_draw_rectangle_vsb(p_render->p_gpe,
                            (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                            &rect,
                            color);
    MT_ASSERT(SUCCESS == ret);
}

/*
 * Draw a character.
  \param[in] cpl characters per line
  \param[in] cw character width
  \param[in] ch character height
  \param[in] glyph character index in font xbm
 */
static void draw_char_vsb(ttx_render_vsb_t *p_render, u8 *p_pen, u32 top, u32 left
    , const u8 *p_font, s32 cpl, s32 cw, s32 ch, s32 glyph
    , s32 bold, u32 underline, ttx_size_t size)
{
    RET_CODE ret = 0;
    point_t pos;
    const u8  *p_src = NULL;
    s32 shift = 0, x = 0, y = 0, z = 0;
    u32 bits = 0;
    u8  col = 0;
    rect_t rect;
    u32 cp_size = 0;
#ifdef WARRIORS
    u32 ttx_char_max_w = 0;
#endif
    u8 *p_char_buf = p_render->p_char_buf;
#ifdef TTX_RENDER_OPTIMIZE
    gpe_param_vsb_t gpe_param = {0, 0, 0, 0, 0, 0};
#endif
    rect_t fill_rect = {0};
#ifdef WARRIORS
    static u8 *p_buf_convert = NULL;
    u8 *p_tmp_dst = NULL;
    u8 *p_tmp_src = NULL;
    int i = 0;
    region_t *p_region = NULL;
    u32 pitch_tmp = 0;
#endif
    MT_ASSERT(NULL != p_char_buf);

    bold = !!bold;

    y = (glyph / cpl) * ch;
    x = (glyph % cpl) * cw;
#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_HD)
    {
        z = WSTFONT2_WIDTH_HD * y + x;
        ttx_char_max_w = TTX_CHAR_MAX_HD_W;
    }
    else
    {
#endif
        if(p_render->video_std == VID_STD_PAL)
            z = WSTFONT2_WIDTH_PAL * y + x;
        else
            z = WSTFONT2_WIDTH_NTSC * y + x;
#ifdef WARRIORS
        ttx_char_max_w = TTX_CHAR_MAX_W;
    }
#endif

    shift = z & 7;
    p_src = p_font + z / 8;

    pos.x = 0;
    pos.y = 0;
    rect.left = left;
    rect.top = top;
    rect.right = rect.left + cw;
    rect.bottom = rect.top + ch;

    switch(size)
    {
        case TTX_NORMAL_SIZE:
        {
            for(y = 0; y < ch; underline >>= 1,  y ++, p_src += cpl * cw / 8)
            {
                bits = 0xffffffff;
                if((underline & 1) == 0)
                {
#ifdef WARRIORS
                    bits  = ((p_src[3]<< 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                    bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                        >> shift;
                    bits |= bits << bold;
                }

                for(x = 0; x < cw; bits >>= 1, x ++)
                {
#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * ttx_char_max_w)] = p_pen[bits & 1];
#else
                    p_char_buf[x + (y * TTX_CHAR_MAX_W)] = p_pen[bits & 1];
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          p_pen[bits & 1]);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top ++;
            }

            cp_size = (u32)(cw * ch);

            break;
        }

        case TTX_DOUBLE_WIDTH:
        {
            for(y = 0; y < ch; underline >>= 1, y ++, p_src += cpl * cw / 8)
            {
                bits = 0xffffffff;
                if((underline & 1) == 0)
                {
#ifdef WARRIORS
                    bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                    bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                        >> shift;
                    bits |= bits << bold;
                }

                for(x = 0; x < cw * 2; bits >>= 1, x += 2)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + (y * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + (y * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top ++;
            }

            cp_size = (u32)(cw * ch * 2);
            rect.right = rect.left + (cw * 2);

            break;
        }
        case TTX_DOUBLE_HEIGHT:
        {
            for(y = 0; y < ch / 2; y ++, p_src += cpl * cw / 8)
            {
#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw; bits >>= 1, x ++)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + ((y * 2 + 1) * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top += 2;
            }

            if((ch % 2) != 0)
            {
#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw; bits >>= 1, x ++)
                {
#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = p_pen[bits & 1];
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = p_pen[bits & 1];
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          p_pen[bits & 1]);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }
            }

            cp_size = (u32)(cw * ch * 2);
            rect.bottom = rect.top + (ch * 2);

            break;
        }
        case TTX_DOUBLE_SIZE:
        {
            for(y = 0; y < ch / 2; y ++, p_src += cpl * cw / 8)
            {
#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw * 2; bits >>= 1, x += 2)
                {
                    col = p_pen[bits & 1];
#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + ((y * 2 + 1) * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + ((y * 2 + 1) * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top += 2;
            }

            if((ch % 2) != 0)
            {
#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw * 2; bits >>= 1, x += 2)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + (y * 2 * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + (y * 2 * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }
            }

            cp_size = (u32)(cw * ch * 4);
            rect.right = rect.left + (cw * 2);
            rect.bottom = rect.top + (ch * 2);

            break;
        }
        case TTX_DOUBLE_HEIGHT2:
        {
            p_src += cpl * cw / 8 * (ch / 2);
            underline >>= ch / 2;

            if((ch % 2) != 0)
            {
#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw; bits >>= 1, x ++)
                {
#ifdef TTX_RENDER_OPTIMIZE
                    y = 0;
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = p_pen[bits & 1];
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = p_pen[bits & 1];
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          p_pen[bits & 1]);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top ++;
                p_src += cpl * cw / 8;
            }

            for(y = 0; y < ch / 2; underline >>= 1, y ++, p_src += cpl * cw / 8)
            {
                bits = 0xffffffff;

                if((underline & 1) == 0)
                {
#ifdef WARRIORS
                    bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                    bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                        >> shift;
                    bits |= bits << bold;
                }

                for(x = 0; x < cw; bits >>= 1, x ++)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + ((y * 2 + 1) * ttx_char_max_w)] = col;
                    p_char_buf[x + ((y * 2 + 2) * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + ((y * 2 + 2) * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top += 2;
            }

            cp_size = (u32)(cw * ch * 2);
            rect.bottom = rect.top + (ch * 2);

            break;
        }
        case TTX_DOUBLE_SIZE2:
        {
            p_src += cpl * cw / 8 * (ch / 2);
            underline >>= ch / 2;

            if((ch % 2) != 0)
            {
                bits = 0xffffffff;

#ifdef WARRIORS
                bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                    >> shift;
                bits |= bits << bold;

                for(x = 0; x < cw * 2; bits >>= 1, x += 2)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + (y * 2 * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + (y * 2 * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top ++;
                p_src += cpl * cw / 8;
            }

            for(y = 0; y < ch / 2; underline >>= 1, y ++, p_src += cpl * cw / 8)
            {
                bits = 0xffffffff;

                if((underline & 1) == 0)
                {
#ifdef WARRIORS
                    bits  = ((p_src[3] << 24) + (p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#else
                    bits  = ((p_src[2] << 16) + (p_src[1] << 8) + p_src[0])
#endif
                        >> shift;
                    bits |= bits << bold;
                }

                for(x = 0; x < cw * 2; bits >>= 1, x += 2)
                {
                    col = p_pen[bits & 1];

#ifdef TTX_RENDER_OPTIMIZE
#ifdef WARRIORS
                    p_char_buf[x + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + (y * 2 * ttx_char_max_w)] = col;
                    p_char_buf[x + ((y * 2 + 1) * ttx_char_max_w)] = col;
                    p_char_buf[x + 1 + ((y * 2 + 1) * ttx_char_max_w)] = col;
#else
                    p_char_buf[x + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + (y * 2 * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
                    p_char_buf[x + 1 + ((y * 2 + 1) * TTX_CHAR_MAX_W)] = col;
#endif
#else
                    pos.x = x + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);

                    pos.x = x + 1 + left;
                    pos.y = top + 1;
                    ret = gpe_draw_pixel_vsb(p_render->p_gpe,
                          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                          &pos,
                          col);
                    MT_ASSERT(SUCCESS == ret);
#endif
                }

                top += 2;
            }

            cp_size = (u32)(cw * ch * 4);
            rect.right = rect.left + (cw * 2);
            rect.bottom = rect.top + (ch * 2);

            break;
        }
        default:
            break;
    }
#ifdef WARRIORS
    p_region = (TRUE != p_render->is_sub) ? (region_t *)p_render->p_osd_hdl :
               (region_t *)p_render->p_sub_hdl;
          ///////////////////////////////////////////
#ifdef WARRIORS
    pitch_tmp = (ttx_char_max_w + 7) &~ 7;
#else
    pitch_tmp = (TTX_CHAR_MAX_W + 7) &~ 7;
#endif
    cp_size = ch * 2 * pitch_tmp;
    if(!p_buf_convert)
    {    
        p_buf_convert = mtos_align_malloc(cp_size + 8, 16);
        p_buf_convert = (u8 *)((u32)p_buf_convert | 0xa0000000); 
        p_buf_convert = (u8 *)ROUNDUP((u32)p_buf_convert, 8);
    }

    p_tmp_dst = p_buf_convert;
    p_tmp_src = p_char_buf;

    for(i = 0; i < ch * 2; i ++)
    {
#ifdef WARRIORS
        memcpy(p_tmp_dst, p_tmp_src, ttx_char_max_w);
#else
        memcpy(p_tmp_dst, p_tmp_src, TTX_CHAR_MAX_W);
#endif
        p_tmp_dst += pitch_tmp;
        p_tmp_src += cw * 2;
    }
//rect.right = rect.left + TTX_CHAR_MAX_W;
//rect.top = rect.left + TTX_CHAR_MAX_H;

    ret = gpe_draw_image_vsb(p_render->p_gpe, 
              p_region,
              &rect,
              p_buf_convert,
#ifdef __LINUX__
              NULL,        
#else
              p_region->p_palette,
#endif
              256,
              pitch_tmp,
              cp_size,
              PIX_FMT_RGBPALETTE8,
              &gpe_param,
              &fill_rect);
    MT_ASSERT(SUCCESS == ret);
#else
#ifdef TTX_RENDER_OPTIMIZE
    ret = gpe_draw_image_vsb(p_render->p_gpe,
          (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
          &rect,
          p_char_buf,
          NULL,
          0,
#ifdef WARRIORS
          ttx_char_max_w,
#else
          TTX_CHAR_MAX_W,
#endif
          cp_size,
          PIX_FMT_MAX,
          &gpe_param,
          &fill_rect);
    MT_ASSERT(SUCCESS == ret);
#endif
#endif
}

/*
 * clear first character line is also the header line.
 */
void ttx_render_clear_header_vsb(ttx_render_vsb_t *p_render, u8 color)
{
    RET_CODE ret = 0;
    rect_t rect;
    s16 char_h = 0;

#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_HD)
    {
        char_h = TTX_CHAR_H_HD;
    }
    else if(p_render->video_std == VID_STD_PAL)
#else
    if(p_render->video_std == VID_STD_PAL)
#endif
    {
        char_h = TTX_CHAR_H_PAL;
    }
    else
    {
        char_h = TTX_CHAR_H_NTSC;
    }

    if(TRUE == p_render->is_sub)
    {
      MT_ASSERT(NULL != p_render->p_sub_hdl);
    }
    else
    {
      MT_ASSERT(NULL != p_render->p_osd_hdl);
    }

    rect.left = p_render->page_x;
    rect.top = p_render->page_y;
    rect.right = p_render->page_x + p_render->page_w;
    rect.bottom = p_render->page_y + char_h;

    ret = gpe_draw_rectangle_vsb(p_render->p_gpe,
                            (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                            &rect,
                            color);
    MT_ASSERT(SUCCESS == ret);

#ifndef WIN32
#ifdef CACHE_ON
    extern void flush_dcache_all();
    flush_dcache_all();
#endif
#endif

    return;
}

/*
 * clear whole page.
 */
void ttx_render_clear_page_vsb(ttx_render_vsb_t *p_render, u8 color)
{
    RET_CODE ret = 0;
    rect_t rect;

    if(TRUE == p_render->is_sub)
    {
      MT_ASSERT(NULL != p_render->p_sub_hdl);
    }
    else
    {
      MT_ASSERT(NULL != p_render->p_osd_hdl);
    }

    rect.left = p_render->page_x;
    rect.top = p_render->page_y;
    rect.right = p_render->page_x + p_render->page_w;
#ifdef WARRIORS
    rect.bottom = p_render->page_y + p_render->page_h;
#else
    rect.bottom = p_render->page_y + p_render->page_h + p_render->page_h % 2;
#endif

    ret = gpe_draw_rectangle_vsb(p_render->p_gpe,
                            (TRUE != p_render->is_sub) ? p_render->p_osd_hdl : p_render->p_sub_hdl,
                            &rect,
                            color);
    MT_ASSERT(SUCCESS == ret);

#ifndef WIN32
#ifdef CACHE_ON
    extern void flush_dcache_all();
    flush_dcache_all();
#endif
#endif

    return;
}

/*
 * Draw page number.
 */
void ttx_render_draw_page_no_vsb(ttx_render_vsb_t *p_render, u16 page_no, u16 sub_no, BOOL b_dec)
{
    s32 i = 0;
    u16 unicode = 0;
    u8  pen[2];
    u32 cnt = 0;
    u32 char_w = 0, char_h = 0;
    const u8 *p_font = NULL;
    void *p_disp_dev = NULL;

#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_NORMAL)
    {
        if(p_render->video_std == VID_STD_PAL)
       {
           char_h = TTX_CHAR_H_PAL;
           char_w = TTX_CHAR_W_PAL;
           p_font = vbi_get_font(TTX_FONT_SRC_PAL);
       }
       else
       {
           char_h = TTX_CHAR_H_NTSC;
           char_w = TTX_CHAR_W_NTSC;
           p_font = vbi_get_font(TTX_FONT_SRC_NTSL);
       }
    }
    else if(p_render->font_size == TTX_FONT_SMALL)
    {
         char_h = TTX_CHAR_H_SMALL;
         char_w = TTX_CHAR_W_SMALL;
         p_font = vbi_get_font(TTX_FONT_SRC_SMALL);
    }
    else
    {
         char_h = TTX_CHAR_H_HD;
         char_w = TTX_CHAR_W_HD;
         p_font = vbi_get_font(TTX_FONT_SRC_HD);
    }
    MT_ASSERT(p_font != NULL);
#else
    if(p_render->video_std == VID_STD_PAL)
    {
        char_h = TTX_CHAR_H_PAL;
        char_w = TTX_CHAR_W_PAL;
        p_font = wstfont2_bits_pal_vsb;
    }
    else
    {
        char_h = TTX_CHAR_H_NTSC;
        char_w = TTX_CHAR_W_NTSC;
        p_font = wstfont2_bits_ntsc_vsb;
    }
#endif

    pen[0] = 40 + TTX_BLACK;
    pen[1] = TTX_WHITE;

    cnt = (page_no &0xf000) >> 12;
    cnt = (cnt <= 3) ? 3 - cnt : 0;

    if(TRUE == b_dec)//when is true, update the render area of the incoming page_no
    {
        if((page_no >= 0x100) && (page_no <= 0x8ff))
        {
            for(i = 1; i < 4; i ++)
            {
                if(cnt-- > 0)
                    unicode = ((page_no >> ((3 - i) * 4)) & 0xf) + 0x30;
                draw_char_vsb(p_render
                    , pen
                    , p_render->page_y
                    , p_render->page_x + char_w * i + 270
                    , p_font
                    , TTX_CHAR_NUM_PER_LINE, char_w, char_h
                    , unicode_wstfont2_vsb (unicode, 0)
                    , 0
                    , 0
                    , TTX_NORMAL_SIZE);
            }
        }
    }
    else
    {
        for(i = 0; i < 8; i ++)
        {
            if(sub_no == 0
                || sub_no == TTX_NULL_SUBPAGE
                || sub_no == TTX_ANY_SUBPAGE
                || sub_no == TTX_FIRST_SUBPAGE)
            {
                if(i == 0)
                {
                    unicode = ' ';
                }
                else if(i < 4)
                {
                    if(cnt-- > 0)
                        unicode = ((page_no >> ((3 - i) * 4)) & 0xf) + 0x30;
                    else
                        unicode = '_';
                }
                else
                {
                    unicode = ' ';
                }
            }
            else
            {
                if(i == 0)
                {
                    unicode = ' ';
                }
                else if(i < 4)
                {
                    if(cnt-- > 0)
                        unicode = ((page_no >> ((3 - i) * 4)) & 0xf) + 0x30;
                    else
                        unicode = '_';
                }
                else if(i == 4)
                {
                    if(sub_no != 0)
                        unicode = '/';
                }
                else
                {
                    unicode = ((sub_no >> ((6 - i) * 4)) & 0xf) + 0x30;
                }
            }

            draw_char_vsb(p_render
                , pen
                , p_render->page_y
                , p_render->page_x + char_w * i
                , p_font
                , TTX_CHAR_NUM_PER_LINE, char_w, char_h
                , unicode_wstfont2_vsb (unicode, 0)
                , 0
                , 0
                , TTX_NORMAL_SIZE);
        }
    }

    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                        SYS_DEV_TYPE_DISPLAY);
#ifndef WIN32
#ifdef WARRIORS
    if(FALSE == b_dec)
#endif
      disp_layer_update_region(p_disp_dev, p_render->p_osd_hdl, NULL);
#endif
    #ifndef WIN32
    #ifdef CACHE_ON
    extern void flush_dcache_all();
    flush_dcache_all();
    #endif
    #endif
}

/*
 * Draw whole page.
 */
void ttx_render_draw_page_vsb(ttx_render_vsb_t *p_render, BOOL subtile
    , ttx_osd_page_t *p_osd_page
    , u8 first_row, u8 last_row
    , u8 first_column, u8 last_column
    , BOOL conceal, BOOL flash_on)
{
    RET_CODE    ret = 0;
    u32         i = 0;
    u32         *p_c = NULL;
    u8          pen[42];
    u16         unicode = 0;
    ttx_char_t  *p_ac = NULL, ac;
    u8          row = 0, column = 0;
    s32         top = 0, left = 0;
    s32         char_w = 0, char_h = 0;
    const u8    *p_font = NULL;
    void *p_disp_dev = NULL;

#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_NORMAL)
    {
        if(p_render->video_std == VID_STD_PAL)
       {
           char_h = TTX_CHAR_H_PAL;
           char_w = TTX_CHAR_W_PAL;
           p_font = vbi_get_font(TTX_FONT_SRC_PAL);
       }
       else
       {
           char_h = TTX_CHAR_H_NTSC;
           char_w = TTX_CHAR_W_NTSC;
           p_font = vbi_get_font(TTX_FONT_SRC_NTSL);
       }
    }
    else if(p_render->font_size == TTX_FONT_SMALL)
    {
         char_h = TTX_CHAR_H_SMALL;
         char_w = TTX_CHAR_W_SMALL;
         p_font = vbi_get_font(TTX_FONT_SRC_SMALL);
    }
    else
    {
         char_h = TTX_CHAR_H_HD;
         char_w = TTX_CHAR_W_HD;
         p_font = vbi_get_font(TTX_FONT_SRC_HD);
    }
    MT_ASSERT(p_font != NULL);
#else
    if(p_render->font_size == TTX_FONT_NORMAL)
    {
        if(p_render->video_std == VID_STD_PAL)
       {
           char_h = TTX_CHAR_H_PAL;
           char_w = TTX_CHAR_W_PAL;
           p_font = wstfont2_bits_pal_vsb;
       }
       else
       {
           char_h = TTX_CHAR_H_NTSC;
           char_w = TTX_CHAR_W_NTSC;
           p_font = wstfont2_bits_ntsc_vsb;
       }
    }
    else
    {
         char_h = TTX_CHAR_W_SMALL;
         char_w = TTX_CHAR_W_SMALL;
         p_font = wstfont2_bits_small_vsb;
    }
#endif

    top  = p_render->page_y + char_h * first_row;
    left = p_render->page_x + char_w * first_column;

    memcpy(p_render->clut
        , p_osd_page->color_map
        , sizeof(p_osd_page->color_map));

    p_c = p_render->clut + sizeof(p_osd_page->color_map) / 4;
    for(i = 0; i < (sizeof(p_osd_page->color_map) / 4); i ++, p_c ++)
        *p_c = (p_render->clut[i] & 0xffffff)
            | (0xff * p_osd_page->user_screen_opacity / 100) << 24;

    if(subtile)
    {
      ret = region_set_palette(p_render->p_sub_hdl, (u32 *)p_render->clut, 256);
      MT_ASSERT(SUCCESS == ret);
    }
    else
    {
      ret = region_set_palette(p_render->p_osd_hdl, (u32 *)p_render->clut, 256);
      MT_ASSERT(SUCCESS == ret);
    }

    if(p_osd_page->p_drcs_clut != NULL)
        memcpy(pen + 2, p_osd_page->p_drcs_clut, 40);

    ac = p_osd_page->text[0];
    for(row = first_row; row < last_row; row ++)
    {
        p_ac = &p_osd_page->text[row * p_osd_page->columns + first_column];

        for(column = first_column; column < last_column; column ++, p_ac ++)
        {
            if((p_ac->conceal != 0 && conceal == TRUE)
                || (p_ac->flash != 0 && flash_on == TRUE))
                unicode = 0x0020;
            else
                unicode = p_ac->unicode;

            pen[0] = p_ac->background + sizeof(p_osd_page->color_map) / 4;
            pen[1] = p_ac->foreground;

            if(p_ac->opacity == TTX_TRANSPARENT_SPACE
                || p_ac->opacity == TTX_TRANSPARENT_FULL)
            {
                draw_blank_vsb(p_render
                    , top
                    , left
                    , SCREEM_COLOR_INDEX
                    , char_w, char_h);

                left += char_w;

                continue;
            }

            switch(p_ac->size)
            {
                case TTX_OVER_TOP:
                case TTX_OVER_BOTTOM:
                    break;

                default:
                    if(vbi_is_drcs_vsb(unicode) == TRUE)
                    {
                        #if TTX_SUPPORT_DRCS
                        p_font = p_osd_page->p_drcs[(unicode >> 6) & 0x1F];

                        if(p_font != NULL)
                            draw_drcs_vsb(p_render
                                , pen
                                , p_ac->drcs_clut_offs
                                , top
                                , left
                                , p_font
                                , unicode & 0x3F
                                , p_ac->size);
                        else /* shouldn't happen */
                            MT_ASSERT(0);
                        #else
                        draw_blank_vsb(p_render
                            , top
                            , left
                            , pen[0]
                            , char_w, char_h);
                        #endif
                    }
                    else
                    {
                        draw_char_vsb(p_render
                            , pen
                            , top
                            , left
                            , p_font
                            , TTX_CHAR_NUM_PER_LINE, char_w, char_h
                            , unicode_wstfont2_vsb (unicode, p_ac->italic)
                            , p_ac->bold
                            , p_ac->underline << (char_h - 1) /* cell row 9 */
                            , p_ac->size);
                    }
            }

            left += char_w;
        }

        if(column == TTX_COLUMNS)
        {
            ttx_char_t  *p_tmp = p_ac - 1;

            if(p_tmp->opacity == TTX_TRANSPARENT_SPACE
                || p_tmp->opacity == TTX_TRANSPARENT_FULL)
            {
                draw_blank_vsb(p_render
                    , top
                    , left
                    , SCREEM_COLOR_INDEX
                    , char_w, char_h);
            }
            else
            {
                draw_blank_vsb(p_render
                    , top
                    , left
                    , 40 + TTX_BLACK
                    , char_w, char_h);
            }
        }
        else
        {
            draw_blank_vsb(p_render
                , top
                , left
                , SCREEM_COLOR_INDEX
                , char_w, char_h);
        }

        left = p_render->page_x + char_w * first_column;
        top += char_h;
    }

   p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                            SYS_DEV_TYPE_DISPLAY);
#ifndef WIN32
#ifdef WARRIORS
   if(subtile)    
      disp_layer_update_region(p_disp_dev, p_render->p_sub_hdl, NULL);
   else
#endif
   disp_layer_update_region(p_disp_dev, p_render->p_osd_hdl, NULL);
#endif

    #ifndef WIN32
    #ifdef CACHE_ON
    extern void flush_dcache_all();
    flush_dcache_all();
    #endif
    #endif
}

void ttx_render_set_bg_transparent_vsb(ttx_render_vsb_t *p_render
    , ttx_osd_page_t *p_osd_page, u8 percent)
{
    RET_CODE ret = 0;
    u32 i = 0;
    u32 *p_c = p_render->clut + sizeof(p_osd_page->color_map) / 4;


    for(i = 0; i < (sizeof(p_osd_page->color_map) / 4); i ++, p_c ++)
    {
        *p_c = (*p_c & 0xffffff) | (0xff * percent / 100) << 24;
    }

    ret = region_set_palette(p_render->p_osd_hdl, (u32 *)p_render->clut, 256);
    MT_ASSERT(SUCCESS == ret);
}

/*!
  The region in osd0 layer is created by upper user, we only configure and show.

  */

vbi_rc_t ttx_render_create_region_vsb(ttx_render_vsb_t *p_render, video_std_t std)
{
    RET_CODE            ret = 0;
    rect_t              rect;
    rect_size_t         rect_size;
#if 1 //ndef WARRIORS   used for sdk project
    point_t             pos;
    u32 align = 0;
    u32 rgn_size = 0;
    void *p_buf_align = NULL;
#endif

    p_render->p_disp = (void *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_render->p_disp);

    p_render->p_gpe = (void *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_render->p_gpe);

    p_render->video_std = std;

    if(vbi_get_region_pos() == TTX_REGION_OUT_DECODER)
    {
        ret = region_show(p_render->p_osd_hdl, FALSE);
        MT_ASSERT(SUCCESS == ret);

        ret = region_get_rectsize(p_render->p_osd_hdl, &rect_size);
        if(ret != SUCCESS)
        {
            OS_PRINTF("TTX!:    get region rectsize failed\n");
            MT_ASSERT(0);
            return ret;
        }
    }
    
    if(p_render->font_size == TTX_FONT_HD)
    {
        p_render->page_x = (rect_size.w - TTX_PAGE_W_HD) / 2;
        p_render->page_y = (rect_size.h - TTX_PAGE_H_HD) / 2;
        p_render->page_w = TTX_PAGE_W_HD;
        p_render->page_h = TTX_PAGE_H_HD;
    }
    else if(p_render->video_std == VID_STD_PAL)
    {
      p_render->page_x = (rect_size.w - TTX_PAGE_W_PAL) / 2;
      p_render->page_y = (rect_size.h - TTX_PAGE_H_PAL) / 2;
      p_render->page_w = TTX_PAGE_W_PAL;
      p_render->page_h = TTX_PAGE_H_PAL;
    }
    else
    {
        p_render->page_x = (rect_size.w - TTX_PAGE_W_NTSC) / 2;
        p_render->page_y = (rect_size.h - TTX_PAGE_H_NTSC) / 2;
        p_render->page_w = TTX_PAGE_W_NTSC;
        p_render->page_h = TTX_PAGE_H_NTSC;
    }    
    
    p_render->region_w    = rect_size.w;
    p_render->region_h    = rect_size.h;
    p_render->clut[SCREEM_COLOR_INDEX] = 0x00000000;

    if(vbi_get_region_pos() == TTX_REGION_IN_DECODER)
    {
        p_render->p_osd_hdl = region_create(&rect_size, PIX_FMT_RGBPALETTE8);
        MT_ASSERT(NULL != p_render->p_osd_hdl);

        ret = disp_calc_region_size(p_render->p_disp, p_render->sub_layer_id,
            p_render->p_osd_hdl, &align, &rgn_size);
        MT_ASSERT(SUCCESS == ret);

        p_render->p_buf_actual = mtos_align_malloc((rgn_size + align), 8);
        MT_ASSERT(NULL != p_render->p_buf_actual);
        p_buf_align = p_render->p_buf_actual;
        memset(p_buf_align, 0, (rgn_size + align));
        p_buf_align = (void *)((((u32) p_buf_align + align - 1) & (~(align - 1))));
        ret = disp_layer_add_region(p_render->p_disp, p_render->sub_layer_id,
            p_render->p_osd_hdl, &pos, p_buf_align);
    }

    rect.left = 0;
    rect.top = 0;
    rect.right = rect_size.w;
    rect.bottom = rect_size.h;
    ret = gpe_draw_rectangle_vsb(p_render->p_gpe,
                                 p_render->p_osd_hdl,
                                 &rect,
                                 SCREEM_COLOR_INDEX - 1);

    MT_ASSERT(SUCCESS == ret);

    if(vbi_get_region_pos() == TTX_REGION_IN_DECODER)
    {
      ret = region_set_palette(p_render->p_osd_hdl, (u32 *)p_render->clut, 256);
      MT_ASSERT(SUCCESS == ret);
    }
    
    ret = region_show(p_render->p_osd_hdl, TRUE);
    MT_ASSERT(SUCCESS == ret);

    return VBI_RC_SUCCESS;
}

/*!
  The region in osd0 layer is created by upper user, we only hide and configure.

  */
vbi_rc_t ttx_render_delete_region_vsb(ttx_render_vsb_t *p_render)
{

    RET_CODE            ret = 0;
#if 1 //def WARRIORS
    rect_t              rect;
#endif

    MT_ASSERT(NULL != p_render->p_disp);
    MT_ASSERT(NULL != p_render->p_gpe);

    ret = region_show(p_render->p_osd_hdl, FALSE);
    MT_ASSERT(SUCCESS == ret);

    if(vbi_get_region_pos() == TTX_REGION_OUT_DECODER)
    {
        p_render->clut[SCREEM_COLOR_INDEX] = 0x00000000;

        rect.left = 0;
        rect.top = 0;
        rect.right = p_render->region_w;
        rect.bottom = p_render->region_h;
        ret = gpe_draw_rectangle_vsb(p_render->p_gpe, 
                                     p_render->p_osd_hdl, 
                                     &rect, 
                                     SCREEM_COLOR_INDEX - 1);
        MT_ASSERT(SUCCESS == ret);
    }
    else
    {
        ret = disp_layer_remove_region(p_render->p_disp,
                                       p_render->sub_layer_id,
                                       p_render->p_osd_hdl);
        MT_ASSERT(SUCCESS == ret);

        ret = region_delete(p_render->p_osd_hdl);
        MT_ASSERT(SUCCESS == ret);

        if (NULL != p_render->p_buf_actual)
        {
            mtos_align_free(p_render->p_buf_actual);
            p_render->p_buf_actual = NULL;
        }
    }

    return VBI_RC_SUCCESS;
}

/*!
  The region in SUB layer is created, we will malloc buffer or use inner buffer.
  */
vbi_rc_t ttx_render_create_sub_region_vsb(ttx_render_vsb_t *p_render, video_std_t std)
{
    RET_CODE            ret = 0;
    rect_size_t         rect_size;
    point_t             pos;
    u32 align = 0;
    u32 rgn_size = 0;
#ifdef WARRIORS
#ifndef WIN32
    chip_rev_t chip_rev = hal_get_chip_rev();
#else
    chip_rev_t chip_rev = IC_MAGIC;
#endif
#else
    void *p_buf_align = NULL;
#endif

    p_render->p_disp = (void *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_render->p_disp);

    p_render->p_gpe = (void *)dev_find_identifier(NULL
        , DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_render->p_gpe);

    p_render->video_std = std;
#ifdef WARRIORS
    if(p_render->font_size == TTX_FONT_HD)
    {
        pos.x = TTX_PAGE_X_HD;
        pos.y = TTX_PAGE_Y_HD;
        rect_size.w = TTX_PAGE_W_HD;
        rect_size.h = TTX_PAGE_H_HD + TTX_PAGE_H_HD % 2;

        p_render->page_x = 0;
        p_render->page_y = 0;
        p_render->page_w = rect_size.w;
        p_render->page_h = rect_size.h;
    }
    else if(p_render->video_std == VID_STD_PAL)
#else
    if(p_render->video_std == VID_STD_PAL)
#endif
    {
        pos.x = TTX_PAGE_X_PAL;
        pos.y = TTX_PAGE_Y_PAL;
        rect_size.w = TTX_PAGE_W_PAL;
        rect_size.h = TTX_PAGE_H_PAL + TTX_PAGE_H_PAL % 2;

        p_render->page_x = 0;
        p_render->page_y = 0;
#ifdef WARRIORS
        p_render->page_w = rect_size.w;
        p_render->page_h = rect_size.h;
#else
        p_render->page_w = TTX_PAGE_W_PAL;
        p_render->page_h = TTX_PAGE_H_PAL;
#endif
    }
    else
    {
        pos.x = TTX_PAGE_X_NTSC;
        pos.y = TTX_PAGE_Y_NTSC;
        rect_size.w = TTX_PAGE_W_NTSC;
        rect_size.h = TTX_PAGE_H_NTSC + TTX_PAGE_H_NTSC % 2;

        p_render->page_x = 0;
        p_render->page_y = 0;
#ifdef WARRIORS
        p_render->page_w = rect_size.w;
        p_render->page_h = rect_size.h;
#else
        p_render->page_w = TTX_PAGE_W_NTSC;
        p_render->page_h = TTX_PAGE_H_NTSC;
#endif
    }

    p_render->clut[SCREEM_COLOR_INDEX] = 0x00000000;

    p_render->p_sub_hdl = region_create(&rect_size, PIX_FMT_RGBPALETTE8);
    MT_ASSERT(NULL != p_render->p_sub_hdl);

    p_render->region_w    = rect_size.w;
    p_render->region_h    = rect_size.h;

#ifdef WARRIORS
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WIZARDS)
    {
      ret = disp_calc_region_size(p_render->p_disp, DISP_LAYER_ID_OSD0,
                                p_render->p_sub_hdl, &align, &rgn_size);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_MAGIC)
    {
      ret = disp_calc_region_size(p_render->p_disp, DISP_LAYER_ID_SUBTITL,
                                p_render->p_sub_hdl, &align, &rgn_size);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WARRIORS)
    {
      ret = disp_calc_region_size(p_render->p_disp, DISP_LAYER_ID_SUBTITL,
                                p_render->p_sub_hdl, &align, &rgn_size);
      MT_ASSERT(SUCCESS == ret);
    }
    p_render->p_buf = NULL;
    
    //the buffer NULL means to use the configured buffer 
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WIZARDS)
    {
      ret = disp_layer_add_region(p_render->p_disp, DISP_LAYER_ID_OSD0,
                                  p_render->p_sub_hdl, &pos, p_render->p_buf);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_MAGIC)
    {
      ret = disp_layer_add_region(p_render->p_disp, DISP_LAYER_ID_SUBTITL,
                                  p_render->p_sub_hdl, &pos, p_render->p_buf);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WARRIORS)
    {
      ret = disp_layer_add_region(p_render->p_disp, DISP_LAYER_ID_SUBTITL,
                                  p_render->p_sub_hdl, &pos, p_render->p_buf);
      MT_ASSERT(SUCCESS == ret);
    }
#else
    ret = disp_calc_region_size(p_render->p_disp, p_render->sub_layer_id,
                              p_render->p_sub_hdl, &align, &rgn_size);
    MT_ASSERT(SUCCESS == ret);

    p_render->p_buf_actual = mtos_align_malloc((rgn_size + align), 8);
    MT_ASSERT(NULL != p_render->p_buf_actual);
    p_buf_align = p_render->p_buf_actual;
    memset(p_buf_align, 0xff, (rgn_size + align));
    p_buf_align = (void *)((((u32) p_buf_align + align - 1) & (~(align - 1))));

    p_render->p_buf = p_buf_align;

    //the buffer NULL means to use the configured buffer
    ret = disp_layer_add_region(p_render->p_disp, p_render->sub_layer_id,
                                p_render->p_sub_hdl, &pos, p_render->p_buf);
    MT_ASSERT(SUCCESS == ret);
#endif
    ret = region_set_palette(p_render->p_sub_hdl, (u32 *)p_render->clut, 256);
    MT_ASSERT(SUCCESS == ret);

    ret = region_show(p_render->p_sub_hdl, TRUE);
    MT_ASSERT(SUCCESS == ret);

#ifdef WARRIORS
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WIZARDS)
    {
      ret = disp_layer_show(p_render->p_disp, DISP_LAYER_ID_OSD0, TRUE);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_MAGIC)
    {
      ret = disp_layer_show(p_render->p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WARRIORS)
    {
      ret = disp_layer_show(p_render->p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
      MT_ASSERT(SUCCESS == ret);
    }
#else
    ret = disp_layer_show(p_render->p_disp, p_render->sub_layer_id, TRUE);
    MT_ASSERT(SUCCESS == ret);
#endif

    //render in subtitle plane
    p_render->is_sub = TRUE;

    return VBI_RC_SUCCESS;
}

/*!
  The region in SUB layer is removed and deleted, we will free buffer or move back inner buffer.
  */
vbi_rc_t ttx_render_delete_sub_region_vsb(ttx_render_vsb_t *p_render)
{
    RET_CODE            ret = 0;
#ifdef WARRIORS
#ifndef WIN32
    chip_rev_t chip_rev = hal_get_chip_rev();
#else
    chip_rev_t chip_rev = IC_MAGIC;
#endif
#endif

    ret = region_show(p_render->p_sub_hdl, FALSE);
    MT_ASSERT(SUCCESS == ret);
#ifdef WARRIORS
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WIZARDS)
    {
      ret = disp_layer_remove_region(p_render->p_disp, 
                                     DISP_LAYER_ID_OSD0,
                                     p_render->p_sub_hdl);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_MAGIC)
    {
      ret = disp_layer_remove_region(p_render->p_disp, 
                                     DISP_LAYER_ID_SUBTITL,
                                     p_render->p_sub_hdl);
      MT_ASSERT(SUCCESS == ret);
    }
    if((CHIP_IC_ID_MASK & chip_rev) == IC_WARRIORS)
    {
      ret = disp_layer_remove_region(p_render->p_disp, 
                                     DISP_LAYER_ID_SUBTITL,
                                     p_render->p_sub_hdl);
      MT_ASSERT(SUCCESS == ret);
    }
#else
    ret = disp_layer_remove_region(p_render->p_disp,
                                   p_render->sub_layer_id,
                                   p_render->p_sub_hdl);
    MT_ASSERT(SUCCESS == ret);
#endif

    ret = region_delete(p_render->p_sub_hdl);
    MT_ASSERT(SUCCESS == ret);
    p_render->p_sub_hdl = NULL;

#ifdef WARRIORS
    if(NULL != p_render->p_buf)
    {
      mtos_free(p_render->p_buf);
      p_render->p_buf = NULL;
    }
#else
    if (NULL != p_render->p_buf_actual)
    {
        mtos_align_free(p_render->p_buf_actual);
        p_render->p_buf_actual = NULL;
    }
#endif
    //clear flag
    p_render->is_sub = FALSE;

    return VBI_RC_SUCCESS;
}
