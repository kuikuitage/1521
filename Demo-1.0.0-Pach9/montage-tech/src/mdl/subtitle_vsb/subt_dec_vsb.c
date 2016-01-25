/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_int.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mem_manager.h"

#include "lib_rect.h"
#include "common.h"

#include "dmx.h"
#include "display.h"
#include "region.h"
#include "drv_misc.h"
#include "drv_dev.h"

#include "subt_priv_vsb.h"

#ifdef WIN32
#define SUBT_ENABLE_ASSER
#define SUBT_ENABLE_DATA_ERR_ASSER
#else
#define SUBT_ENABLE_ASSER
//#define SUBT_ENABLE_DATA_ERR_ASSER
#endif

#ifdef SUBT_ENABLE_ASSER
    #define SUBT_ASSERT         MT_ASSERT
#else
    #define SUBT_ASSERT(x)      do{} while(0)
#endif

#ifdef SUBT_ENABLE_DATA_ERR_ASSER
#define SUBT_DATA_ERR(condition)        \
  do                                    \
  {                                     \
    if((condition) != TRUE)             \
      SUBT_ASSERT(0);                   \
  } while(0)
#else
#define SUBT_DATA_ERR(x)        do{} while(0)
#endif

typedef struct
{
    u8  *p_start;
    u8  *p_p;
    u8  *p_end;
    s32 left;
} bs_sub_t;

/*
 *    define in vbi_inserter.c
 */
extern u32 vbi_get_stc(void);
extern s32 vbi_get_pts_offset(u32 pts, u32 stc);

static const u32 default_clut256_vsb[256] =
{
    0xffbf8080, 0x7f7d8080, 0xff20ca73, 0x7f85a579,
    0xff654e41, 0x7fa76760, 0xff769935, 0x7fb08c5a,
    0xff3b66ca, 0x7f9273a5, 0xff4cb1be, 0x7f9b989f,
    0xff91358c, 0x7fbd5a86, 0xffa18080, 0x7fc58080,
    0x7f108080, 0xff7d8080, 0x7f20ca73, 0xff85a579,
    0x7f654e41, 0xffa76760, 0x7f769935, 0xffb08c5a,
    0x7f3b66ca, 0xff9273a5, 0x7f4cb1be, 0xff9b989f,
    0x7f91358c, 0xffbd5a86, 0x7fa18080, 0xffc58080,
    0xbf28f06d, 0x7f81927c, 0xff28f06d, 0x7f89b876,
    0xff6d733b, 0x7fac7a5d, 0xff7ebe2f, 0x7fb49f57,
    0xff438bc4, 0x7f9786a2, 0xff54d6b8, 0x7f9fab9c,
    0xff995a86, 0x7fc16d82, 0xffaaa579, 0x7fca927c,
    0x7f18a47a, 0xff81927c, 0x7f28f06d, 0xff89b876,
    0x7f6d733b, 0xffac7a5d, 0x7f7ebe2f, 0xffb49f57,
    0x7f438bc4, 0xff9786a2, 0x7f54d6b8, 0xff9fab9c,
    0x7f995a86, 0xffc16d82, 0x7faaa579, 0xffca927c,
    0xbf903522, 0x7f927370, 0xff4ab254, 0x7f9a986a,
    0xff903522, 0x7fbd5a50, 0xffa18015, 0x7fc5804a,
    0xff654eab, 0x7fa86695, 0xff76999f, 0x7fb08c8f,
    0xffbc1c6c, 0x7fd34e76, 0xffcc6760, 0x7fdb7370,
    0x7f3a6761, 0xff927370, 0x7f4ab254, 0xff9a986a,
    0x7f903522, 0xffbd5a50, 0x7fa18015, 0xffc5804a,
    0x7f654eab, 0xffa86695, 0x7f76999f, 0xffb08c8f,
    0x7fbc1c6c, 0xffd34e76, 0x7fcc6760, 0xffdb7370,
    0xbfa9a510, 0x7f96866d, 0xff53d74e, 0x7f9fab67,
    0xff985a1c, 0x7fc16d4d, 0xffa9a510, 0x7fca9347,
    0xff6e73a5, 0x7fac7992, 0xff7ebe99, 0x7fb59f8c,
    0xffc44166, 0x7fd76173, 0xffd58c5a, 0x7fdf866d,
    0x7f428c5b, 0xff96866d, 0x7f53d74e, 0xff9fab67,
    0x7f985a1c, 0xffc16d4d, 0x7fa9a510, 0xffca9347,
    0x7f6e73a5, 0xffac7992, 0x7f7ebe99, 0xffb59f8c,
    0x7fc44166, 0xffd76173, 0x7fd58c5a, 0xffdf866d,
    0xbf515af0, 0x7f887992, 0xff36be98, 0x7f909e8c,
    0xff7b4266, 0x7fb26073, 0xff8b8c5a, 0x7fbb866d,
    0xff515af0, 0x7f9d6db8, 0xff62a4e3, 0x7fa692b2,
    0xffa728b1, 0x7fc85498, 0xffb773a5, 0x7fd17992,
    0x7f2573a4, 0xff887992, 0x7f36be98, 0xff909e8c,
    0x7f7b4266, 0xffb26073, 0x7f8b8c5a, 0xffbb866d,
    0x7f515af0, 0xff9d6db8, 0x7f62a4e3, 0xffa692b2,
    0x7fa728b1, 0xffc85498, 0x7fb773a5, 0xffd17992,
    0xbf6acadd, 0x7f8c8c8f, 0xff3ee392, 0x7f94b189,
    0xff836660, 0x7fb77370, 0xff94b254, 0x7fbf996a,
    0xff597fea, 0x7fa27fb5, 0xff6acadd, 0x7faaa5af,
    0xffaf4dab, 0x7fcc6795, 0xffc0989f, 0x7fd58c8f,
    0x7f2d989e, 0xff8c8c8f, 0x7f3ee392, 0xff94b189,
    0x7f836660, 0xffb77370, 0x7f94b254, 0xffbf996a,
    0x7f597fea, 0xffa27fb5, 0x7f6acadd, 0xffaaa5af,
    0x7faf4dab, 0xffcc6795, 0x7fc0989f, 0xffd58c8f,
    0xbfd11092, 0x7f9d6d83, 0xff60a579, 0x7fa6927d,
    0xffa62947, 0x7fc85463, 0xffb6743a, 0x7fd0795d,
    0xff7b41d1, 0x7fb360a8, 0xff8c8cc5, 0x7fbb85a2,
    0xffd11092, 0x7fde4789, 0xffe25a86, 0x7fe66d83,
    0x7f4f5b85, 0xff9d6d83, 0x7f60a579, 0xffa6927d,
    0x7fa62947, 0xffc85463, 0x7fb6743a, 0xffd0795d,
    0x7f7b41d1, 0xffb360a8, 0x7f8c8cc5, 0xffbb85a2,
    0x7fd11092, 0xffde4789, 0x7fe25a86, 0xffe66d83,
    0xbfea8080, 0x7fa18080, 0xff68cb73, 0x7faaa579,
    0xffae4e41, 0x7fcc6760, 0xffbe9934, 0x7fd58c5a,
    0xff8466cb, 0x7fb773a5, 0xff94b1be, 0x7fc0989f,
    0xffda348c, 0x7fe25a86, 0xffea8080, 0x7fea8080,
    0x7f588080, 0xffa18080, 0x7f68cb73, 0xffaaa579,
    0x7fae4e41, 0xffcc6760, 0x7fbe9934, 0xffd58c5a,
    0x7f8466cb, 0xffb773a5, 0x7f94b1be, 0xffc0989f,
    0x7fda348c, 0xffe25a86, 0x7fea8080, 0xffea8080
};

static const u32 default_clut16_vsb[16] =
{
    0xffea8080, 0xff108080, 0xff68cb73, 0xff1cb776,
    0xffae4e41, 0xff4f5b51, 0xffbe9934, 0xff5c9248,
    0xff8466cb, 0xff306db7, 0xff94b1be, 0xff3da4ae,
    0xffda348c, 0xff704889, 0xffea8080, 0xff7d8080
};

static const u32 default_clut4_vsb[4] =
{
    0xff7d8080, 0xff108080, 0xffea8080, 0xff7d8080
};

/*!
  subtitle bitstream init  
  */
static inline void bs_sub_init(bs_sub_t *p_s, void *p_data, u32 size)
{
    p_s->p_start = p_data;
    p_s->p_p     = p_data;
    p_s->p_end   = p_s->p_p + size;
    p_s->left    = 8;
}

/*!
  subtitle bitstream end flag  
  */
static inline BOOL bs_sub_eof(bs_sub_t *p_s)
{
    //fix bug 6300
    return p_s->p_p > p_s->p_end ? TRUE : FALSE;
}

/*!
  subtitle bitstream read 
  */
static u32 bs_sub_read(bs_sub_t *p_s, s32 count)
{
    static const u32 mask[33] =
    {
        0x00,
        0x01, 0x03, 0x07, 0x0f,
        0x1f, 0x3f, 0x7f, 0xff,
        0x1ff, 0x3ff, 0x7ff, 0xfff,
        0x1fff, 0x3fff, 0x7fff, 0xffff,
        0x1ffff, 0x3ffff, 0x7ffff, 0xfffff,
        0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
        0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
        0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
    };
    s32 shr    = 0;
    u32 result = 0;

    while(count > 0)
    {
        if(p_s->p_p > p_s->p_end)//fix bug 6300
        {
            //MT_ASSERT(0);
            OS_PRINTF("SUBT!:bs read error \n");  //for bug 11328
            break;
        }

        if((shr = p_s->left - count) >= 0)
        {
            /* more in the buffer than requested */
            result  |= (*p_s->p_p >> shr) & mask[count];
            p_s->left -= count;
            if(p_s->left == 0)
            {
                p_s->p_p ++;
                p_s->left = 8;
            }
            return result;
        }
        else
        {
            /* less in the buffer than requested */
            result |= (*p_s->p_p & mask[p_s->left]) << (- shr);
            count -= p_s->left;
            p_s->p_p ++;
            p_s->left = 8;
        }
    }

    return result;
}

/*!
  subtitle bitstream skip  
  */
static void bs_sub_skip(bs_sub_t *p_s, int i_count)
{
    p_s->left -= i_count;

    while(p_s->left <= 0)
    {
        p_s->p_p ++;
        p_s->left += 8;
    }
}

/*!
  subtitle bitstream align  
  */
static inline void bs_sub_align(bs_sub_t *p_s)
{
    if(p_s->left != 8)
    {
        p_s->left = 8;
        p_s->p_p ++;
    }
}

static void dec_pixel_data_2bpp_vsb(bs_sub_t *p_s
    , u8 *p_pix_buf, u16 width, u16 *p_offset)
{
    BOOL    is_stop = FALSE;
    u32     count   = 0;
    u8      color   = 0;

    while(!is_stop && !bs_sub_eof(p_s))
    {
        color = bs_sub_read(p_s, 2);

        if(color != 0x00)
        {
            count = 1;
        }
        else
        {
            if(bs_sub_read(p_s, 1) == 0x01)           // Switch1
            {
                count = 3 + bs_sub_read(p_s, 3);
                color = bs_sub_read(p_s, 2);
            }
            else
            {
                if(bs_sub_read(p_s, 1) == 0x00)       //Switch2
                {
                    switch(bs_sub_read(p_s, 2))       //Switch3
                    {
                        case 0x00:
                            is_stop = TRUE;
                            count = 0;
                            break;
                        case 0x01:
                            count = 2;
                            break;
                        case 0x02:
                            count = 12 + bs_sub_read(p_s, 4);
                            color = bs_sub_read(p_s, 2);
                            break;
                        case 0x03:
                            count = 29 + bs_sub_read(p_s, 8);
                            color = bs_sub_read(p_s, 2);
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    count = 1;
                }
            }
        }

        if(!count)
        {
            continue;
        }

        if((count + *p_offset) > width)
        {
            break;
        }

        if(count == 1)
        {
            p_pix_buf[*p_offset] = color;
        }
        else
        {
            memset((p_pix_buf + *p_offset), color, count);
        }

        (*p_offset) += count;
    }

    bs_sub_align(p_s);
}

static void dec_pixel_data_4bpp_vsb(bs_sub_t *p_s
    , u8 *p_pix_buf, u16 width, u16 *p_offset)
{
    BOOL    is_stop = FALSE;
    u32     count   = 0;
    u8      color   = 0;

    while(!is_stop && !bs_sub_eof(p_s))
    {
        //next 4bits
        color = bs_sub_read(p_s, 4);
        if(color != 0x00)
        {
            //not 0x00 then regarded as 4bit pixel code
            count = 1;
        }
        else
        {
            if(bs_sub_read(p_s, 1) == 0x00)             // Switch1
            {
                //next 3bit
                count = bs_sub_read(p_s, 3);

                if(count != 0x00)
                {
                    //run length 3-9
                    //3bit value max 1-7, so run length 3-9
                    count += 2;
                    color = 0x00;
                }
                else
                {
                    //end of the string
                    is_stop = TRUE;
                }
            }
            else
            {
                if(bs_sub_read(p_s, 1) == 0x00)         //Switch2
                {
                    //run length 4-7
                    count = 4 + bs_sub_read(p_s, 2);
                    color = bs_sub_read(p_s, 4);
                }
                else
                {
                    switch(bs_sub_read(p_s, 2))         //Switch3
                    {
                        case 0x0:
                            // 1 pixel set to pseudo color 0x00 
                            count = 1;
                            color = 0x00;
                            break;
                        case 0x1:
                            // 2 pixels set to pseudo color 0x00 
                            count = 2;
                            color = 0x00;
                            break;
                        case 0x2:
                            //run length 9-24
                            count = 9 + bs_sub_read(p_s, 4);
                            color = bs_sub_read(p_s, 4);
                            break;
                        case 0x3:
                            //run length 25-280
                            count = 25 + bs_sub_read(p_s, 8);
                            color = bs_sub_read(p_s, 4);
                            break;
                    }
                }
            }
        }

        if(!count)
        {
            //no copy then reach the end of string.
            continue;
        }

        if((count + *p_offset) > width)
        {
            //error outof boundary            
            //MT_ASSERT(0);
            //we don't assert here for bug 11328 because when add error injection as 1/1 bit errors
            //will down here
            OS_PRINTF("SUBT!:error out of boundary  \n"); 
            break;
        }

        if(count == 1)
        {
            p_pix_buf[*p_offset] = color;
        }
        else
        {
            memset((p_pix_buf + *p_offset), color, count);
        }

        (*p_offset) += count;
    }

    bs_sub_align(p_s);
}

static void dec_pixel_data_8bpp_vsb(bs_sub_t *p_s
    , u8 *p_pix_buf, u16 width, u16 *p_offset)
{
    BOOL    is_stop = FALSE;
    u8      count   = 0;
    u8      color   = 0;

    while((is_stop != TRUE) && (bs_sub_eof(p_s) != TRUE))
    {
        color = bs_sub_read(p_s, 8);
        if(color != 0x00)
        {
            count = 1;
        }
        else
        {
            if(bs_sub_read(p_s, 1) == 0x00)            // Switch1
            {
                count = bs_sub_read(p_s, 7);
                if(count == 0x00)
                {
                    is_stop = TRUE;
                }
            }
            else
            {
                count = bs_sub_read(p_s, 7);
                color = bs_sub_read(p_s, 8);
            }
        }

        if(count > 0)
        {
            continue;
        }

        if((count + *p_offset) > width)
        {
            break;
        }

        if(count == 1)
        {
            p_pix_buf[*p_offset] = color;
        }
        else
        {
            memset((p_pix_buf + *p_offset), color, count);
        }

        (*p_offset) += count;
    }

    bs_sub_align(p_s);
}

#define PITCH_ALIGN_SIZE 8
#define BASE_BUF_ADDRESS_ALIGN_SIZE 8
static void render_pixel_data_vsb(dvb_subtitle_vsb_t *p_subt
    , subt_region_t *p_region, u16 x, u16 y, u8 *p_field, u16 field_length)
{
    u16  offset = 0;
    bs_sub_t bs;
    #ifdef WARRIORS
    static u8  *pix_buf = NULL;
    void *p_disp_dev = NULL;
    #else
    u8   pix_buf[SUBT_MAX_DISPALY_WIDTH];
    #endif
    u8   pix_buf_cov[SUBT_MAX_DISPALY_WIDTH];
    u8   pixels_in_byte = 0;
    u8   type = 0, last_type = 0;
    rect_t rect;
    #ifdef WARRIORS
    if(!pix_buf)
      {
        pix_buf = mtos_align_malloc(SUBT_MAX_DISPALY_WIDTH +
                                            BASE_BUF_ADDRESS_ALIGN_SIZE, 16);
        pix_buf = (u8 *)((u32)pix_buf | 0xa0000000);
        pix_buf = (u8 *)(((u32)pix_buf + (BASE_BUF_ADDRESS_ALIGN_SIZE -1))
                                  &~ (BASE_BUF_ADDRESS_ALIGN_SIZE -1));
      }
    #endif
    if(y >= p_region->height || x >= p_region->width)
    {
        OS_PRINTF("SUBT!: invalid offset (%d,%d)", x, y);
        return;
    }

    //use bitstream operations for conveniece
    bs_sub_init(&bs, p_field, field_length);

    type      = bs_sub_read(&bs, 8);
    last_type = type;

    while(bs_sub_eof(&bs) != TRUE)
    {
        if(y > p_region->height)
        {
            OS_PRINTF("SUBT!: invalid height (%d,%d)", y, p_region->height);
            return;
        }

        switch(type)
        {
            case 0x10:
                dec_pixel_data_2bpp_vsb(&bs, pix_buf, p_region->width, &offset);
                break;

            case 0x11:
                dec_pixel_data_4bpp_vsb(&bs, pix_buf, p_region->width, &offset);
                break;

            case 0x12:
                dec_pixel_data_8bpp_vsb(&bs, pix_buf, p_region->width, &offset);
                break;

            /* don't use map tables */
            case 0x20:
                //OS_PRINTF(("SUBT: 2 to 4 bit map table\n"));
                bs_sub_skip(&bs, 16);
                break;

            case 0x21:
                //OS_PRINTF(("SUBT: 2 to 8 bit map table\n"));
                bs_sub_skip(&bs, 32);
                break;

            case 0x22:
                //OS_PRINTF(("SUBT: 4 to 8 bit map table\n"));
                bs_sub_skip(&bs, 128);
                break;

            case 0xf0:       /* End of line code */
                if(last_type == 0x10)
                {
                    pixels_in_byte = 4;
                }
                else if(last_type == 0x11)
                {
                    pixels_in_byte = 2;
                    //subt_osd_copy_h_line_4bpp(p_subt, &p_region->osd_region
                      //  , x, y, p_region->width, pix_buf, offset);
                }
                else if(last_type == 0x12)
                {
                    pixels_in_byte = 1;
                    //subt_osd_copy_h_line_8bpp(p_subt, &p_region->osd_region
                      //  , x, y, p_region->width, pix_buf, offset);
                }
                else
                {
                    OS_PRINTF("SUBT!: unkouwn last object type 0x%x"
                        , last_type);
                    break;
                }
                pixels_in_byte = 1;
                rect.left = x;
                rect.top = y;
            #ifdef WARRIORS
                rect.right = x + (offset / pixels_in_byte);
            #else
                rect.right = x + p_region->width;
            #endif
                rect.bottom = y + 1;//one line only
                /*need to convert the buffer
                              2/4bpp, 
                            */
                ///TODO:minnan workaround              
 //OS_PRINTF("!!!!!pix in byte:%d left:%d right:%d top:%d bottom:%d\n", 
 //pixels_in_byte, rect.left, rect.right, rect.top, rect.bottom);
 //subt_osd_buf_convert(p_subt, pix_buf, pix_buf_cov, pixels_in_byte, offset);           
                subt_osd_copy_lines_vsb(p_subt,
                                      &p_region->osd_region,
                                      &rect,
                                      //2/4bpp use converted buffer, 8bpp use default buffer
                                      (1 == pixels_in_byte) ? pix_buf : pix_buf_cov,
            #ifdef WARRIORS
                                      (p_region->width / pixels_in_byte) & (~7),
                                      (((p_region->width / pixels_in_byte) & (~7))
                                        * (rect.bottom - rect.top)));
             #else
                                      (p_region->width / pixels_in_byte),
             (offset / pixels_in_byte));
             #endif
                offset = 0;
                y     += 2;
                break;

                default:
                 //MT_ASSERT(0);
                OS_PRINTF("SUBT!: unkouwn object type 0x%x\n", type);
                //here won't assert for bug 11328
                break;
        }

        if(offset > p_region->width)
        {
          MT_ASSERT(0);
        }

        last_type = type;
        type      = bs_sub_read(&bs, 8);
    }
#ifdef WARRIORS
    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
                    SYS_DEV_TYPE_DISPLAY);
    if(p_region->osd_region.p_rgn_hdl != NULL)
    {
      disp_layer_update_region(p_disp_dev, p_region->osd_region.p_rgn_hdl,
                                                                    NULL);
    }
#endif

    #ifndef WIN32
    #ifdef CACHE_ON
    extern void flush_dcache_all();
    flush_dcache_all();
    #endif
    #endif
}

/*!
   parse Display Definition Segment

   \param[in] p_subt      This instance
   \param[in] p_segment   Point to the start of Display Definition Segment

   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
static subt_rc_t dec_display_definition_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_segment)
{
    p_subt->window.display_width = SUBT_MAKE_U16(p_segment[7], p_segment[8]);
    p_subt->window.display_heigth = SUBT_MAKE_U16(p_segment[9], p_segment[10]);

    if((p_segment[6] & 0x08) != 0)
    {
        p_subt->window.display_left
            = SUBT_MAKE_U16(p_segment[11], p_segment[12]);
        p_subt->window.display_right
            = SUBT_MAKE_U16(p_segment[13], p_segment[14]);
        p_subt->window.display_top
            = SUBT_MAKE_U16(p_segment[15], p_segment[16]);
        p_subt->window.display_bottom
            = SUBT_MAKE_U16(p_segment[17], p_segment[18]);
    }
    else
    {
        p_subt->window.display_left = 0;
        p_subt->window.display_right = p_subt->window.display_width;
        p_subt->window.display_top = 0;
        p_subt->window.display_bottom = p_subt->window.display_heigth;
    }

    if(((p_subt->window.display_bottom - p_subt->window.display_top + 1)
            > SUBT_MAX_DISPALY_HEIGTH)
        || ((p_subt->window.display_right - p_subt->window.display_left + 1)
            > SUBT_MAX_DISPALY_WIDTH))
    {
        p_subt->window.is_supported = FALSE;
    }
    else
    {
        p_subt->window.is_supported = TRUE;
    }

    return SUBT_RC_SUCCESS;
}

#define MINNAN_NEW_REGION_REORDER
#ifdef MINNAN_NEW_REGION_REORDER
/*!
  region reorder  
  */
static void region_reorder(subt_region_info_t *p_dst, subt_region_info_t *p_src, u16 num)
{
  u8 i = 0;
  u8 j = 0;
  subt_region_info_t temp;

  if(num == 0)
  {
    OS_PRINTF("SUBTITLE No need to reorder\n");
    return;
  }
  OS_PRINTF("SUBTITLE reorder no. %d\n", num);
  MT_ASSERT(NULL != p_dst);
  MT_ASSERT(NULL != p_src);
  MT_ASSERT(0 < num);

  for(i = 0; i < num; i++)
  {
    p_dst[i] = p_src[i];
  }

  for(i = 0; i < num; i++)
  {
    for(j = 0; j <= i; j++)
    {
      //smallest in the front
      if(p_dst[j].top > p_dst[i].top)
      {
        temp = p_dst[j] ;
        p_dst[j] = p_dst[i];
        p_dst[i] = temp;
      }
    }
  }

  for(i = 0; i < num; i++)
  {
    //the highest minus most, the lowest minus least, in 2pixels interval
    p_dst[i].top = p_dst[i].top - (num * 2) + (i * 2);
  }
  
  for(i = 0; i < num; i++)
  {
    for(j = 0; j < num; j++)
    {
      if(p_dst[i].id == p_src[i].id)
      {
        //modify src region info
        p_src[i].top = p_dst[i].top;
      }
    }    
  }
}
#endif

/*!
   parse Page Composition Segment

   \param[in] p_subt      This instance
   \param[in] p_segment   Point to the start of Page Composition Segment


   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
static subt_rc_t dec_page_composition_vsb(dvb_subtitle_vsb_t *p_subt
    , u8 *p_segment, BOOL *p_is_update)
{
    u16 page_id        = 0;
    u16 segment_length = 0;
    u8  page_state     = 0;
    u16 region_num     = 0;
    u8  version_number = 0;
    u8  *p_buf         = 0;

    subt_page_t         *p_page        = NULL;
    subt_region_info_t  *p_region_info = NULL;
#ifdef MINNAN_NEW_REGION_REORDER
    subt_region_info_t  temp_info[20] = {{0, 0, 0}};
    u16 num = 0;
#endif

    *p_is_update = FALSE;

    page_id        = SUBT_MAKE_U16(p_segment[2], p_segment[3]);
    segment_length = SUBT_MAKE_U16(p_segment[4], p_segment[5]);
    version_number = (p_segment[7] & 0xf0) >> 4;
    page_state     = (p_segment[7] & 0x0c) >> 2;
    region_num     = (segment_length - 2) / 6;

    if(p_subt->composition_page.page_id == page_id)
    {
        p_page = &(p_subt->composition_page);
    }
    else if(p_subt->ancillary_page.page_id == page_id)
    {
        ///TODO:how to handle ancillary page
        OS_PRINTF("SUBT!: invalid ancillary subtitle packet\n");
        return SUBT_RC_PAGE_ID_ERROR;
    }
    else
    {
        OS_PRINTF("SUBT!: invalid ancillary page id 0x%x\n", page_id);
        return SUBT_RC_PAGE_ID_ERROR;
    }
    /*
      page state: 2bits
      normal case: 00;acquistion point: 01;mode change:10, reserve:11
      start situation:normal case is based on the previous page, should not regard as the first.
      */
    if(p_subt->dec_stats != SUBT_DEC_STATS_ACQUISITION
        && (page_state != 0x01)
        && (page_state != 0x02))
    {
        return SUBT_RC_SUCCESS;
    }
    ///TODO:0x1 the same as 0x2???0x0 passed ,set dec_state normal??
    

    if(page_state == 0x02)//mode change
    {
        if(p_subt->is_display)
        {
            //hide all first
            subt_dec_hide_page_vsb(p_subt);
        }
        subt_db_reset_vsb(p_subt);
    }
    else if(p_page->page_version_number == version_number)
    {
        //no need to update ??
        return SUBT_RC_SUCCESS;
    }

    *p_is_update = TRUE;

    p_subt->dec_stats                     = SUBT_DEC_STATS_ACQUISITION;
    p_subt->composition_page.page_timeout = p_segment[6];
    p_page->page_version_number           = version_number;

    if(region_num != p_page->region_number)
    {
        u16 created_num = 0;

        if(p_page->p_region_info != NULL)
        {
            subt_db_delete_region_info_vsb(p_subt, p_page->p_region_info);
            p_page->p_region_info = NULL;
        }


        if(region_num != 0)
        {
            ///TODO: clear all the region  first
            subt_db_clear_regionlist(p_subt);
            
            subt_db_creat_region_info_vsb(p_subt
                , region_num, &p_region_info, &created_num);
            if(created_num != region_num)
            {
                OS_PRINTF("SUBT!: create region info error, %d - %d\n"
                    , region_num, created_num);
                region_num = created_num;
                SUBT_ASSERT(0);
                return SUBT_RC_FAILED;
            }
        }
        else
        {
            p_region_info = NULL;
        }

        p_page->p_region_info = p_region_info;
        p_page->region_number = region_num;
    }

    p_region_info = p_page->p_region_info;
    p_buf         = p_segment + 8;
#ifdef MINNAN_NEW_REGION_REORDER
    num = region_num;
#endif
    //fill the region info parsed from pcs
    while(region_num > 0)
    {
        p_region_info->id   = p_buf[0];
        p_region_info->left = SUBT_MAKE_U16(p_buf[2], p_buf[3]);
        p_region_info->top  = SUBT_MAKE_U16(p_buf[4], p_buf[5]); 
        OS_PRINTF(">>>left %d top %d\n",p_region_info->left,p_region_info->top);

        if (p_region_info->left > 720 || p_region_info->top > 576)
        {
            if(p_subt->is_display)
            {
                subt_dec_hide_page_vsb(p_subt);
            }
            subt_db_reset_vsb(p_subt);
            OS_PRINTF(">>>SUBT: receive bad page_composition, break\n");
            return SUBT_RC_INVALID_DATA;
        }
#ifdef WARRIORS
        if(VID_STD_NTSC == p_subt->vid_std)//fix bug4766 
        {            
          p_region_info->left = (p_region_info->left * 480)/576;
          if(p_region_info->top > 240)
          {
            p_region_info->top  = p_region_info->top - 96;
          }
        }

         p_region_info->left = (1280 - 720) / 2;
         if(p_region_info->top > 240)
         {
           p_region_info->top = (p_region_info->top * 720)/576;
         }
#endif
        p_buf         += 6;
        p_region_info ++;
        region_num    --;
    }
#ifdef MINNAN_NEW_REGION_REORDER
    region_reorder(temp_info, p_page->p_region_info, num);
#endif

#ifdef WARRIORS
    p_subt->page_timeout = p_subt->composition_page.page_timeout * 1000 + 5000;
#else
    p_subt->page_timeout = p_subt->composition_page.page_timeout * 1000 + 7000;
#endif

    if(p_subt->p_notify_func != NULL)
    {
        p_subt->p_notify_func(SUBT_MSG_PAGE_UPDATE, p_subt->page_timeout, 0);
    }

  return SUBT_RC_SUCCESS;
}

/*!
   parse Region Composition Segment

   \param[in] p_subt      This instance
   \param[in] p_segment   Point to the start of Region Composition Segment

   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
static subt_rc_t dec_region_composition_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_segment)
{
    subt_rc_t rc;
    u16       segment_length             = 0;
    u16       page_id                    = 0;
    u8        region_id                  = 0;
    u8        version_number             = 0;
    u8        depth                      = 0;
    u16       width                      = 0;
    u16       height                     = 0;
    u16       object_num                 = 0;
    u8        object_type                = 0;
    u16       processed_length           = 0;
    subt_region_t      *p_region      = NULL;
    subt_object_info_t *p_object_info = NULL;
    u8                 *p_buf         = NULL;

    page_id        = SUBT_MAKE_U16(p_segment[2], p_segment[3]);
    region_id      = p_segment[6];
    version_number = (p_segment[7] & 0xf0) >> 4;
    width          = SUBT_MAKE_U16(p_segment[8], p_segment[9]);
    height         = SUBT_MAKE_U16(p_segment[10], p_segment[11]);
    depth          = (p_segment[12] & 0x1c) >> 2;

    if(width > SUBT_MAX_DISPALY_WIDTH || height > SUBT_MAX_DISPALY_HEIGTH)
    {
        OS_PRINTF("SUBT!: region too big! %d x %d\n", width, height);
        return SUBT_RC_INVALID_DATA;
    }

    ///TODO: region no 0 then do not create
    if(0 == p_subt->composition_page.region_number)
    {
      OS_PRINTF("SUBT: region number 0\n");
      return SUBT_RC_INVALID_DATA;
    }

    //get exsisting region in database
    rc = subt_db_get_region_vsb(p_subt, page_id, region_id, &p_region);
    if(rc == SUBT_RC_SUCCESS)
    {
        if(p_region->version_number == version_number)
        {
            return SUBT_RC_SUCCESS;
        }
    }
    else
    {
        const u32           *p_clut         = NULL;
        pix_fmt_t           color_mode      = PIX_FMT_MAX;
        subt_region_info_t  *p_region_info  = NULL;
        u8                  region_info_num = 0;
        u32                 clut_entries    = 0;
        rect_t              rect;

        //create in database
        rc = subt_db_create_region_vsb(p_subt, &p_region);
        if(rc != SUBT_RC_SUCCESS || p_region == NULL)
        {
            OS_PRINTF("SUBT!: create region failed !\n");
            SUBT_ASSERT(0);
            return SUBT_RC_FAILED;
        }

        if(depth == 1)
        {
            p_clut = default_clut4_vsb;
            color_mode = PIX_FMT_YUVPALETTE2;
            clut_entries = 4;
        }
        else if(depth == 2)
        {
            p_clut = default_clut16_vsb;
            color_mode = PIX_FMT_YUVPALETTE4;
            clut_entries = 16;
        }
        else if(depth == 3)
        {
            p_clut = default_clut256_vsb;
            color_mode = PIX_FMT_YUVPALETTE8;
            clut_entries = 256;
        }
        else
        {
            subt_db_delete_region_vsb(p_subt, page_id, region_id);

            OS_PRINTF("SUBT!: invalid region depth %d!\n", depth);
            return SUBT_RC_INVALID_DATA;
        }

        ///TODO:now only region number valid in composition page

        region_info_num = p_subt->composition_page.region_number;
        p_region_info   = p_subt->composition_page.p_region_info;
        for(; region_info_num > 0; region_info_num --, p_region_info ++)
        {
            if(p_region_info->id == region_id)
                break;
        }
        if(region_info_num == 0)
        {
            region_info_num = p_subt->ancillary_page.region_number;
            p_region_info   = p_subt->ancillary_page.p_region_info;

            for(; region_info_num > 0; region_info_num --, p_region_info ++)
            {
                if(p_region_info->id == region_id)
                    break;
            }
        }

        if(p_subt->is_display)
        {
            
            if(region_info_num != 0)
            {
              rect.left = p_region_info->left; 
              rect.top = p_region_info->top; 
              rect.right = rect.left + width - 1; 
              rect.bottom = rect.top + height - 1; 
              //create display buffer, inner buffer
              rc = subt_osd_create_region_vsb(p_subt->p_render,
                &rect, color_mode, p_clut, clut_entries, &p_region->osd_region);
              if(rc != SUBT_RC_SUCCESS)
              {
                  subt_db_delete_region_vsb(p_subt, page_id, region_id);
                  OS_PRINTF("SUBT!: create OSD region failed !\n");
                  //SUBT_ASSERT(0);
                  return SUBT_RC_FAILED;
              }

            }
            else
            {
                subt_osd_hide_region_vsb(p_subt->p_render, p_region);
            }
        }
        else
        {
            p_region->osd_region.p_rgn_hdl = NULL;
        }

        p_region->width         = width;
        p_region->height        = height;
        p_region->compatibility = (p_segment[12] & 0xe0) >> 4;
        p_region->depth         = depth;
        p_region->clut_id       = p_segment[13];
        p_region->object_number = 0;
        p_region->p_object_info = NULL;
    }

    SUBT_ASSERT(p_region != NULL);
    p_region->is_updated       = TRUE;
    p_region->page_id          = page_id;
    p_region->id               = region_id;
    p_region->version_number   = version_number;
    p_region->is_region_fill   = (p_segment[7] & 0x08) ? TRUE : FALSE;
    p_region->pixel_code_8_bit = p_segment[14];
    p_region->pixel_code_4_bit = (p_segment[15] & 0xf0) >> 4;
    p_region->pixel_code_2_bit = (p_segment[16] & 0x0c) >> 2;

    segment_length   = SUBT_MAKE_U16(p_segment[4], p_segment[5]);
    object_num       = 0;
    processed_length = 10;
    p_buf            = p_segment + 6 + 10;
    while(processed_length < segment_length)
    {
        object_type = (p_buf[2] & 0xc0) >> 6;
        if(object_type == 0x01 || object_type == 0x02)
        {
            processed_length += 8;
            p_buf += 8;
        }
        else
        {
            processed_length += 6;
            p_buf += 6;
        }

        object_num ++;
    }

    if(p_region->object_number != object_num)
    {
        u16 created_num = 0;

        if(p_region->p_object_info != NULL)
        {
            subt_db_delete_object_info_vsb(p_subt, p_region->p_object_info);
            p_region->p_object_info = NULL;
        }

        if(object_num != 0)
        {
            subt_db_creat_object_info_vsb(p_subt
                , object_num, &p_object_info, &created_num);
            if(created_num != object_num)
            {
                OS_PRINTF("SUBT!: create object info error, %d - %d\n"
                    , object_num, created_num);

                object_num = created_num;
                SUBT_ASSERT(0);
            }
        }
        else
        {
            p_object_info = NULL;
        }

        p_region->p_object_info = p_object_info;
        p_region->object_number = object_num;
    }

    p_object_info = p_region->p_object_info;
    p_buf         = p_segment + 16;
    while(object_num > 0)
    {
        p_object_info->id            = SUBT_MAKE_U16(p_buf[0], p_buf[1]);
        p_object_info->type          = (p_buf[2] & 0xc0) >> 6;
        p_object_info->provider_flag = (p_buf[2] & 0x30) >> 4;
        p_object_info->x_position    = SUBT_MAKE_U16(p_buf[2] & 0x0f, p_buf[3]);
        p_object_info->y_position    = SUBT_MAKE_U16(p_buf[4] & 0x0f, p_buf[5]);

        if(p_object_info->type == 0x01 || p_object_info->type == 0x02)
        {
            p_object_info->fg_pixel_code = p_buf[6];
            p_object_info->bg_pixel_code = p_buf[7];

            p_buf += 8;
        }
        else
        {
            p_buf += 6;
        }

        p_object_info ++;
        object_num --;
    }

    if(p_subt->is_display && p_region->is_region_fill)
    {
        subt_osd_fill_region_vsb(p_subt, p_region);
        p_region->is_region_fill = FALSE;
    }
  return SUBT_RC_SUCCESS;
}

/*!
   parse CLUT Definition Segment

   \param[in] p_subt      This instance
   \param[in] p_segment   Point to the start of CLUT Definition Segment

   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
static subt_rc_t dec_clut_definition_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_segment)
{
    subt_rc_t   rc;
    u16         segment_length = 0;
    u16         page_id = 0;
    u8          clut_id = 0, version_number = 0, entry_id = 0;
    u8          full_range = 0, depth = 0;
    u8          y = 0, cr = 0, cb = 0, alpha = 0;
    u32         yuv = 0;

    u16         processed_length = 0;
    subt_clut_t *p_clut = NULL;
    u8          *p_buf  = NULL;

    //one page has one clut, not each region has its own clut.
    page_id        = SUBT_MAKE_U16(p_segment[2], p_segment[3]);
    segment_length = SUBT_MAKE_U16(p_segment[4], p_segment[5]);
    clut_id        = p_segment[6];
    version_number = (p_segment[7] & 0xf0) >> 4;

    rc = subt_db_get_clut_vsb(p_subt, page_id, clut_id, &p_clut);
    if(rc == SUBT_RC_SUCCESS)
    {
        if(p_clut->version_number == version_number)
            return SUBT_RC_SUCCESS;
    }
    else
    {
        rc = subt_db_create_clut_vsb(p_subt, &p_clut);
        if(rc != SUBT_RC_SUCCESS || p_clut == NULL)
        {
            OS_PRINTF("SUBT!: create CLUT failed !\n");
            SUBT_ASSERT(0);
            return SUBT_RC_FAILED;
        }

        memcpy(p_clut->clut4, &default_clut4_vsb, sizeof(default_clut4_vsb));
        memcpy(p_clut->clut16, &default_clut16_vsb, sizeof(default_clut16_vsb));
        memcpy(p_clut->clut256, &default_clut256_vsb, sizeof(default_clut256_vsb));
    }

    SUBT_ASSERT(p_clut != NULL);

    p_clut->page_id        = page_id;
    p_clut->clut_id        = clut_id;
    p_clut->version_number = version_number;

    processed_length = 2;
    p_buf            = p_segment + 6 + 2;

    while(processed_length + 4 < segment_length)
    {
        entry_id   = *p_buf++;
        depth      = (*p_buf) & 0xe0;
        full_range = (*p_buf++) & 1;

        if(full_range != 0)
        {
            y     = *p_buf++;
            cr    = *p_buf++;
            cb    = *p_buf++;
            alpha = 0xff - *p_buf++;

            processed_length += 6;
        }
        else
        {
            y     = p_buf[0] & 0xfc;
            cr    = (((p_buf[0] & 3) << 2) | ((p_buf[1] >> 6) & 3)) << 4;
            cb    = (p_buf[1] << 2) & 0xf0;
            alpha = 0xff - ((p_buf[1] << 6) & 0xc0);

            p_buf            += 2;
            processed_length += 4;
        }
        if(y == 0)
        {
            alpha = 0;
        }


        yuv =
        (((u32)alpha << 24) | ((u32)y << 16) | ((u32)cb << 8) | ((u32)cr) << 0);


        if((depth & 0x80) != 0)
        {
            p_clut->clut4[entry_id] = yuv;
        }
        else if((depth & 0x40) != 0)
        {
            p_clut->clut16[entry_id] = yuv;
        }
        else if((depth & 0x20) != 0)
        {
            p_clut->clut256[entry_id] = yuv;
        }
        else
        {
            OS_PRINTF("SUBT!: Invalid clut depth 0x%x!\n", depth);
            return SUBT_RC_INVALID_DATA;
        }
    }

    return SUBT_RC_SUCCESS;
}

/*!
   parse Object Data Segment

   \param[in] p_subt      This instance
   \param[in] p_segment   Point to the start of Object Data Segment


   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
static subt_rc_t dec_object_data_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_segment)
{
    u32           i = 0;
    u16           segment_length = 0;
    u16           object_id = 0;
    u8            coding_method = 0;
    ///TODO:
    u8            non_modify_colour = 0;
    subt_region_t *p_region = NULL;

    if(!p_subt->is_display)
    {
        return SUBT_RC_SUCCESS;
    }

    segment_length    = SUBT_MAKE_U16(p_segment[4], p_segment[5]);
    object_id         = SUBT_MAKE_U16(p_segment[6], p_segment[7]);
    coding_method     = (p_segment[8] & 0x0c) >> 2;
    non_modify_colour = (p_segment[8] & 0x02) >> 1;

    //only two valid coding method
    if(coding_method > 1)
    {
        OS_PRINTF("SUBT!: unknown coding method %d !", coding_method);
        return SUBT_RC_INVALID_DATA;
    }

    /*  Check if the object needs to be rendered in at least one
        of the regions
    */
    for(p_region = p_subt->p_region_list
        ; p_region != NULL
        ; p_region = p_region->p_next)
    {
        for(i = 0; i < p_region->object_number; i ++)
        {
            if(p_region->p_object_info[i].id == object_id)
                break;
        }

        if(i != p_region->object_number)
            break;
    }
    if(p_region == NULL)
    {
        SUBT_PRINTF("SUBT:  no region include news objects\n");
        return SUBT_RC_SUCCESS;
    }

    if(coding_method == 0x00)
    {
        u16 top_field_length = 0, bottom_field_length = 0;
        u8  *p_top_field = NULL, *p_bottom_field = NULL;

        top_field_length    = SUBT_MAKE_U16(p_segment[9], p_segment[10]);
        bottom_field_length = SUBT_MAKE_U16(p_segment[11], p_segment[12]);


        if((top_field_length + bottom_field_length + 3) > segment_length)
        {
            OS_PRINTF("SUBT!: invalid field length ! top %d, bottom %d\n"
                , top_field_length, bottom_field_length);
            return SUBT_RC_INVALID_DATA;
        }


        p_top_field    = p_segment + 13;
        p_bottom_field = p_top_field + top_field_length;

        if(segment_length < (top_field_length + bottom_field_length + 7))
        {
            OS_PRINTF("SUBT!: corrupted object data !");
            return SUBT_RC_INVALID_DATA;
        }

        for(p_region = p_subt->p_region_list
            ; p_region != NULL
            ; p_region = p_region->p_next)
        {
            for(i = 0; i < p_region->object_number; i ++)
            {
                if(p_region->p_object_info[i].id != object_id)
                {
                    continue;
                }

                render_pixel_data_vsb(p_subt, p_region
                    , p_region->p_object_info[i].x_position
                    , p_region->p_object_info[i].y_position
                    , p_top_field
                    , top_field_length);

                if(bottom_field_length > 0)
                {
                    render_pixel_data_vsb(p_subt
                        , p_region
                        , p_region->p_object_info[i].x_position
                        , p_region->p_object_info[i].y_position + 1
                        , p_bottom_field
                        , bottom_field_length);
                }
                else
                {
                    render_pixel_data_vsb(p_subt
                        , p_region
                        , p_region->p_object_info[i].x_position
                        , p_region->p_object_info[i].y_position + 1
                        , p_top_field
                        , top_field_length);
                }
            }
        }
    }
    else
    {
        /* DVB subtitling as characters */
    }

    return SUBT_RC_SUCCESS;
}

#if 1
void subt_dec_init_vsb(void)
{
}
#else
void subt_dec_init(void)
{
#if 0
#define RGB_TO_Y(r, g, b) ((u16)77 * (r) + 150 * (g) + 29 * (b)) / 256;
#define RGB_TO_U(r, g, b) ((u16) - 44 * (r) - 87 * (g) + 131 * (b)) / 256;
#define RGB_TO_V(r, g, b) ((u16)131 * (r) - 110 * (g) - 21 * (b)) / 256;
#else
#define RGB_TO_Y(r, g, b)   \
        ((u32)(0.257 * (r) + 0.504 * (g) + 0.098 * (b) + 16));
#define RGB_TO_U(r, g, b)   \
        ((u32)(-0.148 * (r) - 0.291 * (g) + 0.439 * (b) + 128));
#define RGB_TO_V(r, g, b)   \
        ((u32)(0.439 * (r) - 0.368 * (g) - 0.071 * (b) + 128));
#endif

#define SUBT_ARGB(a, r, g, b)       \
  ((((a) & 0xFF) << 24)             \
   | (((r) & 0xFF) << 16)           \
   | (((g) & 0xFF) << 8)            \
   | (((b) & 0xFF) << 0))

#define SUBT_AYUV(a, y, u, v)        \
  ((((a) & 0xFF) << 24)             \
   | (((y) & 0xFF) << 16)           \
   | (((u) & 0xFF) << 8)            \
   | (((v) & 0xFF) << 0))

    u8  i = 0;
    u32 Y = 0, Cb = 0, Cr = 0;

    /* 4 entries CLUT */
    for(i = 0; i < 4; i ++)
    {
        u8 R = 0, G = 0, B = 0, T = 0xFF;

        if((i & 0x3) == 0)
        {
            T = 0x0;
        }
        else if((i & 0x3) == 0x1)
        {
            R = G = B = 0xFF;
        }
        else if((i & 0x3) != 0x2)
        {
            R = G = B = 0;
        }
        else
        {
            R = G = B = 0x7F;
        }

        Y  = RGB_TO_Y(R, G, B);
        Cb = RGB_TO_V(R, G, B);
        Cr = RGB_TO_U(R, G, B);
        T  = T;

        default_clut4[i] = SUBT_AYUV(T, Y, Cb, Cr);
    }

    /* 16 entries CLUT */
    for(i = 0; i < 16; i ++)
    {
        u8 R = 0, G = 0, B = 0, T = 0xFF;

        if((i & 0x1) == 0)
        {
            if((i & 0xe) == 0)
            {
                T = 0;
            }
            else
            {
                R = (i & 0x8) ? 0xFF : 0;
                G = (i & 0x4) ? 0xFF : 0;
                B = (i & 0x2) ? 0xFF : 0;
            }
        }
        else
        {
            R = (i & 0x8) ? 0x7F : 0;
            G = (i & 0x4) ? 0x7F : 0;
            B = (i & 0x2) ? 0x7F : 0;
        }

        Y = RGB_TO_Y(R, G, B);
        Cb = RGB_TO_V(R, G, B);
        Cr = RGB_TO_U(R, G, B);
        T = T;

        default_clut16[i] = SUBT_AYUV(T, Y, Cb, Cr);
        //default_clut16[i] = SUBT_ARGB(T,R,G,B);
    }

  /* 256 entries CLUT */
  //memset(default_clut16, 0xFF, 256 * sizeof(dvbsub_color_t));
}
#endif

subt_rc_t subt_dec_set_page_id_vsb(dvb_subtitle_vsb_t *p_subt,
                               u16 composition_page_id,
                               u16 ancillary_page_id)
{
    if(p_subt->ancillary_page.page_id != ancillary_page_id
        || p_subt->composition_page.page_id != composition_page_id)
    {
        p_subt->ancillary_page.page_id = ancillary_page_id;
        p_subt->composition_page.page_id = composition_page_id;
    }

    return SUBT_RC_SUCCESS;
}

/*!
   Hide page

   \param[in] p_subt      This instance
   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
subt_rc_t subt_dec_display_page_vsb(dvb_subtitle_vsb_t *p_subt)
{
    subt_rc_t         ret = SUBT_RC_SUCCESS;
    subt_region_t     *p_region = NULL;
    subt_color_mode_t color_mode;
    u32               *p_clut = NULL;
    rect_t            rect;
    u32               clut_entries = 0;
    u8                region_info_num = 0;
    subt_region_info_t  *p_region_info  = NULL;

    p_region = p_subt->p_region_list;
    while(p_region != NULL)
    {
        p_region_info   = p_subt->composition_page.p_region_info;
        for(; region_info_num > 0; region_info_num --, p_region_info ++)
        {
            //valid in composition page
            if(p_region_info->id == p_region->id)
                break;
        }
        if(0 == region_info_num)
        {
            region_info_num = p_subt->ancillary_page.region_number;
            p_region_info   = p_subt->ancillary_page.p_region_info;

            for(; region_info_num > 0; region_info_num --, p_region_info ++)
            {
                //valid in ancillary page
                if(p_region_info->id == p_region->id)
                    break;
            }
        }
        if(0 == region_info_num)
        {
            //no valid region, find next
            continue;
        }
        if(p_region->depth == 1)
        {
            p_clut     = (u32 *)default_clut4_vsb;
            color_mode = PIX_FMT_YUVPALETTE2;
            clut_entries = 4;
        }
        else if(p_region->depth == 2)
        {
            p_clut     = (u32 *)default_clut16_vsb;
            color_mode = PIX_FMT_YUVPALETTE4;
            clut_entries = 16;
        }
        else if(p_region->depth == 3)
        {
            p_clut     = (u32 *)default_clut256_vsb;
            color_mode = PIX_FMT_YUVPALETTE8;
            clut_entries = 256;
        }
        else
        {
            OS_PRINTF("SUBT!: invalid region depth %d!\n", p_region->depth);
            SUBT_DATA_ERR(0);
            continue;
        }

        rect.left = p_region_info->left; 
        rect.top = p_region_info->top; 
        rect.right = rect.left + p_region->width - 1; 
        rect.bottom = rect.top + p_region->height - 1; 
        ret = subt_osd_create_region_vsb(p_subt->p_render,
          &rect, color_mode, p_clut, clut_entries, &p_region->osd_region);
        if (ret != SUBT_RC_SUCCESS)
        {
            OS_PRINTF("SUBT!: create OSD region failed in display!\n");
            MT_ASSERT(0);
            return SUBT_RC_FAILED;
        }
        SUBT_PRINTF("SUBT:  create region %d\n", p_region->id);
        p_region = p_region->p_next;
    }

    return SUBT_RC_SUCCESS;
}

/*!
   Display page

   \param[in] p_subt      This instance
   \return    Return SUBT_RC_SUCCESS for success and others for failure.
*/
subt_rc_t subt_dec_hide_page_vsb(dvb_subtitle_vsb_t *p_subt)
{
    subt_region_t *p_region = NULL;

    p_region = p_subt->p_region_list;
    while(p_region != NULL)
    {
        subt_osd_hide_region_vsb(p_subt->p_render, p_region);
        subt_osd_delete_region_vsb(p_subt->p_render, &p_region->osd_region);
        p_region = p_region->p_next;
    }

    return SUBT_RC_SUCCESS;
}

/*!
   Check PES header

   \param[in] p_dev       This instance
   \param[in] p_pes_pkt    Point to the start of one PES packet

   \return            Return SUBT_RC_SUCCESS for success and others for failure.
*/
subt_rc_t subt_dec_check_pes_header_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_pes_pkt
    , u32 *p_pts, u16 *p_length, u8 **p_segment)
{
    u8  *p_pes_data_filed = NULL;
    u32 pts = 0;

    //packet_start_code_prefix: 0x000001
    //streamid 0xbd private stream
    if((p_pes_pkt[0] != 0x00)
        || (p_pes_pkt[1] != 0x00)
        || (p_pes_pkt[2] != 0x01)
        || (p_pes_pkt[3] != 0xbd))
    {
        OS_PRINTF("SUBT!:    Bad boundary.  0x%2.2x%2.2x%2.2x%2.2x\n"
        , p_pes_pkt[0], p_pes_pkt[1], p_pes_pkt[2], p_pes_pkt[3]);

        return SUBT_RC_PES_BAD_BOUNDARY;
    }

    if(p_length != NULL)
    {
        *p_length = SUBT_MAKE_U16(p_pes_pkt[4], p_pes_pkt[5]);
    }

    p_pes_data_filed = p_pes_pkt + 9 + p_pes_pkt[8];

    /*pes data field
          data_identifier(data for subtitle) 0x20
          subtitle stream id 0x00
       */
    if(p_pes_data_filed[0] != 0x20 || p_pes_data_filed[1] != 0x00)
    {
        OS_PRINTF("SUBT!:  unkown PES.  data_id= 0x%2.2x, stream_id= 0x%2.2x\n"
            , p_pes_data_filed[0], p_pes_data_filed[1]);

        return SUBT_RC_INVALID_DATA;
    }

      pts = 0;
      if((p_pes_pkt[7] & 0x80) != 0)
    {
        pts   = p_pes_pkt[9 + 0] & 0x0e;
        pts <<= 7;
        pts  += p_pes_pkt[9 + 1];
        pts <<= 8;
        pts  += p_pes_pkt[9 + 2] & 0xfe;
        pts <<= 7;
        pts  += p_pes_pkt[9 + 3];
        pts <<= 6;
        pts  += ((p_pes_pkt[9 + 3] & 0xfe) >> 2);
    }
    else
    {
        pts = vbi_get_stc();
    }

    if(p_pts != NULL)
    {
        *p_pts = pts;
    }

    if(p_segment != NULL)
    {
        //subtitle segment start
        *p_segment = p_pes_data_filed + 2;
    }

    #ifdef WIN32
    *p_pts = 0;
    #endif

    return SUBT_RC_SUCCESS;
}

/*!
   parse Subtitle PES packet

   \param[in] p_dev   This instance
   \param[in] p_pes   Point to the start of one PES packet


   \return            Return SUBT_RC_SUCCESS for success and others for failure.
*/
subt_rc_t subt_dec_process_vsb(dvb_subtitle_vsb_t *p_subt, u8 *p_pes_pkt)
{
    subt_rc_t   rc;
    u16         pes_packet_length = 0;
    u16         processed_bytes   = 0;    /*  for this PES packet */
    u8          *p_segment = NULL, *p_s = NULL;
    u8          segment_type   = 0;
    u16         page_id        = 0;
    u16         segment_length = 0;
    u32         pts            = 0;
    BOOL        is_dds_present = FALSE;
    BOOL        is_pcs_update  = FALSE;

    rc = subt_dec_check_pes_header_vsb(p_subt, p_pes_pkt, &pts
        , &pes_packet_length, &p_segment);
    if(rc != SUBT_RC_SUCCESS)
    {
        return rc;
    }

    #ifndef WIN32
    if(p_subt->pts == pts || pts == 0)
    {
        return SUBT_RC_SUCCESS;
    }
    #endif

    p_subt->pts = pts;

    /***********
       ETS300743
       subtitling_segment()
       {
          sync_byte            8
          segment type       8
          page_id               16
          segment_length    16
          segment_data_field()
       }       
       segment types:
       DDS,PCS,RCS,CDS,ODS
       ************/

    /*
         DDS 
      */
    //PES packet:
    //packet_start_code_prefix + streamid + packetlength = 3+1+2 = 6
    processed_bytes = p_segment - (p_pes_pkt + 6);
    p_s             = p_segment;
    //if nextbits equals 0x0f subtitle segment loop 
    while(*p_s == 0x0f)
    {
        segment_type   = p_s[1];
        page_id        = SUBT_MAKE_U16(p_s[2], p_s[3]);
        segment_length = SUBT_MAKE_U16(p_s[4], p_s[5]);

        processed_bytes += segment_length + 6;
        if(processed_bytes >= pes_packet_length)
        {
            OS_PRINTF("SUBT!:    PES length error.  pes_packet_length = %d\n"
                , pes_packet_length);
            return SUBT_RC_INVALID_DATA;
        }

        if(p_subt->composition_page.page_id != page_id
            && p_subt->ancillary_page.page_id != page_id)
        {
            p_s += segment_length + 6;
            OS_PRINTF("SUBT!:    unkown page ID 0x%x\n", page_id);
            continue;
        }

        if(segment_type == SUBT_SEGMENT_DDS)
        {
              is_dds_present = TRUE;
              dec_display_definition_vsb(p_subt, p_s);
              break;
        }

        p_s += segment_length + 6;
    }

     /*  if DDS is not present, set display window default.
         else if window size is not support, skip this PES, and return;
    */
    if(is_dds_present != TRUE)
    {
        p_subt->window.display_left = 0;
        p_subt->window.display_right = SUBT_MAX_DISPALY_WIDTH;
        p_subt->window.display_top = 0;
        p_subt->window.display_bottom = SUBT_MAX_DISPALY_HEIGTH;
        p_subt->window.is_supported = TRUE;
    }
    else if(p_subt->window.is_supported != TRUE)
    {
        return SUBT_RC_SUCCESS;
    }

    /*
    *    PCS
    */
    processed_bytes = p_segment - (p_pes_pkt + 6);
    p_s = p_segment;
    while(*p_s == 0x0f)//sync byte
    {
        segment_type   = p_s[1];
        page_id        = SUBT_MAKE_U16(p_s[2], p_s[3]);
        segment_length = SUBT_MAKE_U16(p_s[4], p_s[5]);

        /*subtitling_segment before segment_data_field :
                 8+8+16+18 = 6bytes
             */
        processed_bytes += segment_length + 6; 
        if(processed_bytes >= pes_packet_length)
        {
            OS_PRINTF("SUBT!:    PES length error.  pes_packet_length = %d\n"
                , pes_packet_length);
            return SUBT_RC_INVALID_DATA;
        }

        if(p_subt->composition_page.page_id != page_id
            && p_subt->ancillary_page.page_id != page_id)
        {
            p_s += segment_length + 6;
            OS_PRINTF("SUBT!:    unkown page ID 0x%x\n", page_id);
            continue;
        }

        if(segment_type == SUBT_SEGMENT_PCS)
        {
            rc = dec_page_composition_vsb(p_subt, p_s, &is_pcs_update);
            if (rc == SUBT_RC_INVALID_DATA)
                return rc;
        }

        p_s += segment_length + 6;
    }

    if(p_subt->dec_stats != SUBT_DEC_STATS_ACQUISITION)
    {
        return SUBT_RC_SUCCESS;
    }

    /*
     *    RCS
     */
    processed_bytes = p_segment - (p_pes_pkt + 6);
    p_s = p_segment;
    while(*p_s == 0x0f)
    {
        segment_type   = p_s[1];
        page_id        = SUBT_MAKE_U16(p_s[2], p_s[3]);
        segment_length = SUBT_MAKE_U16(p_s[4], p_s[5]);

        processed_bytes += segment_length + 6;
        if(processed_bytes >= pes_packet_length)
        {
            OS_PRINTF("SUBT!:    PES length error.  pes_packet_length = %d\n"
                , pes_packet_length);
            return SUBT_RC_INVALID_DATA;
        }

        if(p_subt->composition_page.page_id != page_id
            && p_subt->ancillary_page.page_id != page_id)
        {
            p_s += segment_length + 6;
            OS_PRINTF("SUBT!:    unkown page ID 0x%x\n", page_id);
            continue;
        }

        if(segment_type == SUBT_SEGMENT_RCS)
        {
            dec_region_composition_vsb(p_subt, p_s);
        }

        p_s += segment_length + 6;
    }

    /*
    *    CDS
    */
    processed_bytes = p_segment - (p_pes_pkt + 6);
    p_s             = p_segment;
    while(*p_s == 0x0f)
    {
        segment_type   = p_s[1];
        page_id        = SUBT_MAKE_U16(p_s[2], p_s[3]);
        segment_length = SUBT_MAKE_U16(p_s[4], p_s[5]);

        processed_bytes += segment_length + 6;
        if(processed_bytes >= pes_packet_length)
        {
            OS_PRINTF("SUBT!:    PES length error.  pes_packet_length = %d\n"
                , pes_packet_length);
            return SUBT_RC_INVALID_DATA;
        }

        if(p_subt->composition_page.page_id != page_id
            && p_subt->ancillary_page.page_id != page_id)
        {
            p_s += segment_length + 6;
            OS_PRINTF("SUBT!:    unkown page ID 0x%x\n", page_id);
            continue;
        }

        if(segment_type == SUBT_SEGMENT_CDS)
        {
            dec_clut_definition_vsb(p_subt, p_s);
        }

        p_s += segment_length + 6;
    }

    /*
     *    ODS
     */
    processed_bytes = p_segment - (p_pes_pkt + 6);
    p_s = p_segment;
    while(*p_s == 0x0f)
    {
        segment_type   = p_s[1];
        page_id        = SUBT_MAKE_U16(p_s[2], p_s[3]);
        segment_length = SUBT_MAKE_U16(p_s[4], p_s[5]);

        processed_bytes += segment_length + 6;
        if(processed_bytes >= pes_packet_length)
        {
            OS_PRINTF("SUBT!:    PES length error.  pes_packet_length = %d\n"
                , pes_packet_length);
            return SUBT_RC_INVALID_DATA;
        }

        if(p_subt->composition_page.page_id != page_id
            && p_subt->ancillary_page.page_id != page_id)
        {
            p_s += segment_length + 6;
            OS_PRINTF("SUBT!:    unkown page ID 0x%x\n", page_id);
            continue;
        }

        if(segment_type == SUBT_SEGMENT_ODS)
        {
            dec_object_data_vsb(p_subt, p_s);
        }

        p_s += segment_length + 6;
    }

    if(is_pcs_update && p_subt->is_display)
    {
        u8                  region_num     = 0;
        subt_page_t         *p_page        = NULL;
        subt_region_t       *p_region      = NULL;
        subt_region_info_t  *p_region_info = NULL;

        p_page = &(p_subt->composition_page);
        p_region = p_subt->p_region_list;

        while(p_region != NULL)
        {
            region_num = p_page->region_number;
            for(; region_num > 0; region_num --)
            {
                p_region_info = p_page->p_region_info + region_num - 1;
                if(p_region_info->id == p_region->id)
                {
                    subt_rc_t   rc;
                    subt_clut_t *p_clut = NULL;
                    point_t     pos;

                    pos.x = p_region_info->left;
                    pos.y = p_region_info->top;
                    if(0)
                    {
                    subt_osd_move_region_vsb(p_subt->p_render
                        , p_region
                        , &pos);
                    SUBT_PRINTF(("SUBT:    move region %d [%d,%d]\n"
                        , p_region->id
                        , p_region_info->left
                        , p_region_info->top));
                    }
                    rc = subt_db_get_clut_vsb(p_subt
                        , p_subt->composition_page.page_id
                        , p_region->clut_id
                        , &p_clut);
                    if(rc != SUBT_RC_SUCCESS)
                    {
                        return rc;
                    }

                    subt_osd_dispaly_region_vsb(p_subt->p_render, p_region, p_clut);
                    break;
                }
            }

            if(region_num == 0)
            {
                subt_osd_hide_region_vsb(p_subt->p_render, p_region);
            }

            p_region = p_region->p_next;
        }
    }

    #ifndef WIN32
    if(p_subt->is_display == TRUE)
    {
        #ifdef CACHE_ON
        extern void flush_dcache_all();
        flush_dcache_all();
        #endif
    }
    #endif

    return SUBT_RC_SUCCESS;
}
