/******************************************************************************/
/******************************************************************************/
/*!
 \file gui_resource.c
   this file  implement the functions defined in  gui_resource.h, also it implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar control.
 */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"
#include "charsto.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

#include "osd.h"
#include "gpe.h"
#include "surface.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "gui_resource.h"
#include "gui_vfont.h"
#include "class_factory.h"

//#define RSC_DEBUG
#ifdef RSC_DEBUG
#define RSC_PRINTF    OS_PRINTF
#else
#define RSC_PRINTF DUMMY_PRINTF
#endif

/*!
  resource type.
  */
typedef enum 
{
  /*!
    bitmap
    */
  RSC_COMM_BMP = 1,
  /*!
    palette
    */
  RSC_COMM_PAL,
  /*!
    common font
    */  
  RSC_COMM_CFONT,
  /*!
    local font
    */
  RSC_COMM_LFONT,
  /*!
    string table
    */
  RSC_COMM_STRTAB,
  /*!
    script
    */
  RSC_COMM_SCRIPT
}rsc_comm_type_t;

/*! 
  Structure of all resource information.
  */
typedef struct
{
  /*!
    Resource start address
    */
  u8 *p_rsc_data_addr;
  /*!
    Memory heap for resource module
    */
  lib_memp_t heap;
  /*!
    Memory heap for resource module
    */
  void *p_heap_addr;
  /*!
    Max common data buffer size, for read data
    */
  u32 comm_buf_size;
  /*!
    Common data buffer
    */
  u8 *p_comm_buf_addr;
  /*!
    One cache buffer size
    */
  u32 char_cache_size;
  /*!
    Cache buffer length
    */
  u8 char_cache_cnt;
  /*!
    A pointer point to cache, to record the information of the data in cache
    */
  char_cache_t *p_char_cache_array;
  /*!
    Cache buffer address
    */
  u8 *p_char_cache_buf_addr;

  /*!
    Mutext.
    */
  //OS_MUTEX_T 		char_lock;

  /*!
    Current bitmap info(If the bitmap id equals to current bitmap id,
    then we can read data from bitmap buffer directly for saving time!)
    */
  rsc_bmp_info_t bmp_info;
  /*!
    Current palette info(If the palette id equals to current palette id,
    then we can read data from palette buffer directly for saving time!)
    */
  rsc_pal_info_t palette_info;
  /*!
    Current string table info(If the string table id equals to current string table id,
    then we can read data from string table buffer directly for saving time!)
    */
  rsc_strtab_info_t strtab_info;
  /*!
    Current common font info(If the font id equals to current local font id,
    then we can read data from local font buffer directly for saving time!)
    */
  rsc_font_info_t cfont_info;
  /*!
    Current local font info(If the font id equals to current local font id,
    then we can read data from local font buffer directly for saving time!)
    */
  rsc_font_info_t lfont_info;
  /*!
    Current script info(If the script id equals to current script id,
    then we can read data from script buffer directly for saving time!)
    */
  rsc_script_info_t script_info;

  /*!
    Language and font info
    */
  u16 curn_language_id;

  /*!
    Palette & style 
    */
  u16 curn_palette_id;

  /*!
    Actual rectangle style number
    */
  u16 rstyle_cnt;
  /*!
    Rectangle style table
    */
  rsc_rstyle_t *p_rstyle_tab;
  /*!
    Actual font style number
    */
  u8 fstyle_cnt;
  /*!
    Font style table
    */
  rsc_fstyle_t *p_fstyle_tab;
  /*!
    Font map number
    */
  u8 fmap_cnt;
  /*!
    Font map table
    */
  font_map_t *p_fmap_tab;
  /*!
    Flash base.
    */
  u32 flash_base;
  /*!
    charsto device.
    */
  charsto_device_t *p_charsto_dev;
  /*!
    is memory uncached & 8 byte align
    */
  BOOL is_uncached;    
}rsc_main_t;

/*!
  global variable for saving information about resource memory.
  */
//-static rsc_main_t g_rsc_info;

/*!
  Internal used API, to decompressed resource in rle mode.

  \param[in] rsc_handle : resource handle.
  \param[in] p_input : input data
  \param[in] data_size : data size before compress.
  \param[out] p_output : output data.
  
  \return : data size afer compress.
  */
static u32 rsc_rle_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output);

/*!
  get decompressed resource data.

  \param[in] rsc_handle : resource handle.
  \param[in] hdr_addr : Resource header.
  
  \return : decompressed data.
  */
static u8 *rsc_decompress_data(handle_t rsc_handle, rsc_comm_type_t type);


/*!
  Internal used API, to transform char data by ite's font color and screen pixel type.

  \param[in] rsc_handle : resource handle.
  \param[in] p_input : input data buffer.
  \param[in] width : char width.
  \param[in] height : char height.
  \param[out] p_output : output data buffer.
  \param[in] font_color : font color.
  
  \return : color key.
  */
static u32 rsc_transform_char(handle_t rsc_handle, u8 *p_input, u16 width, u16 height, 
  u8 *p_output, u32 font_color, u32 bpp);

static rsc_comm_info_t *_get_rsc_comm_info(handle_t rsc_handle, rsc_comm_type_t type)
{
  rsc_comm_info_t *p_comm_info = NULL;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  switch(type)
  {
    case RSC_COMM_BMP:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->bmp_info;
      break;
      
    case RSC_COMM_PAL:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->palette_info;
      break;
      
    case RSC_COMM_CFONT:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->cfont_info;
      break;

    case RSC_COMM_LFONT:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->lfont_info;
      break;

    case RSC_COMM_SCRIPT:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->script_info;
      break;

    case RSC_COMM_STRTAB:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->strtab_info;
      break;

    default:
      p_comm_info = NULL;
  }

  return p_comm_info;
}

static inline BOOL rsc_is_data_on_flash(u32 addr)
{
  charsto_device_t *p_charsto = (charsto_device_t *)dev_find_identifier(
    NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  return (dev_io_ctrl(
    p_charsto, CHARSTO_IOCTRL_IS_VALID_ADDR, addr) == SUCCESS) ? TRUE : FALSE;
}


static rsc_head_t *_get_rsc_comm_head(rsc_comm_info_t *p_comm_info)
{
  u32 addr = 0;
  MT_ASSERT(p_comm_info != NULL);

  addr = (u32)p_comm_info;
  addr += sizeof(rsc_comm_info_t);

  return (rsc_head_t *)(addr);
}


#ifdef RSC_DEBUG

static void rsc_dump_info(rsc_info_t *p_info)
{
  RSC_PRINTF("dump rsc info......\n");
  RSC_PRINTF("\t type     = %d\n", p_info->type);
  RSC_PRINTF("\t count    = %d\n", p_info->count);
  RSC_PRINTF("\t offset   = 0x%x\n", p_info->offset);
}


static void rsc_dump_idx(rsc_idx_t *p_idx)
{
//  RSC_PRINTF("dump rsc idx......\n");
//  RSC_PRINTF("\t id       = %d\n", idx->id);
//  RSC_PRINTF("\t offset   = 0x%x\n", idx->offset);
}

/*
static void rsc_dump_head(rsc_head_t *head)
{
  RSC_PRINTF("dump rsc head......\n");
  RSC_PRINTF("\t type     = %d\n", head->type);
  RSC_PRINTF("\t id       = %d\n", head->id);
  RSC_PRINTF("\t c_mode   = %d\n", head->c_mode);
  RSC_PRINTF("\t cmp_size = %d\n", head->cmp_size);
  RSC_PRINTF("\t org_size = %d\n", head->org_size);
}
*/

static void rsc_dump_palette(color_t *p_entrys, u16 cnt)
{
  u16 i;
  color_t *p_item = p_entrys;
  RSC_PRINTF("dump rsc palette(%d)......\n", cnt);
  for(i = 0; i < cnt; i++)
  {
    RSC_PRINTF("\t RGBA(%d, %d, %d, %d)\n",
               p_item->r, p_item->g, p_item->b, p_item->a);
    p_item++;
  }
}


static void rsc_dump_global_info(rsc_main_t *p_rsc_info)
{
  RSC_PRINTF("dump rsc global info......\n");
  RSC_PRINTF("\t rsc addr        = 0x%x\n", p_rsc_info->p_rsc_data_addr);
  RSC_PRINTF("\t heap addr       = 0x%x\n", p_rsc_info->heap_addr);
  RSC_PRINTF("\t bitmap  buf size = %d\n", p_rsc_info->bmp_buf_size);
  RSC_PRINTF("\t bitmap  buf addr = 0x%x\n", p_rsc_info->bmp_buf_addr);
  RSC_PRINTF("\t string  buf size = %d\n", p_rsc_info->str_buf_size);
  RSC_PRINTF("\t string  buf addr = 0x%x\n", p_rsc_info->str_buf_addr);
  RSC_PRINTF("\t lang    buf size = %d\n", p_rsc_info->strtab_buf_size);
  RSC_PRINTF("\t lang    buf addr = 0x%x\n", p_rsc_info->strtab_buf_addr);
  RSC_PRINTF("\t palete  buf size = %d\n", p_rsc_info->pal_buf_size);
  RSC_PRINTF("\t palete  buf addr = 0x%x\n", p_rsc_info->pal_buf_addr);
  RSC_PRINTF("\t comfont buf size = %d\n", p_rsc_info->comm_font_buf_size);
  RSC_PRINTF("\t comfont buf addr = 0x%x\n", p_rsc_info->comm_font_buf_addr);
  RSC_PRINTF("\t locfont buf size = %d\n", p_rsc_info->local_font_buf_size);
  RSC_PRINTF("\t locfont buf addr = 0x%x\n", p_rsc_info->local_font_buf_addr);
  RSC_PRINTF("\t f cache buf size = %d\n", p_rsc_info->char_cache_size);
  RSC_PRINTF("\t f cache buf addr = 0x%x\n", p_rsc_info->char_cache_buf_addr);
  RSC_PRINTF("\t f cache buf cnt  = %d\n", p_rsc_info->char_cache_cnt);

  RSC_PRINTF("\t curn pal  id    = %d\n", p_rsc_info->last_pal_id);
  RSC_PRINTF("\t curn lang id    = %d\n", p_rsc_info->curn_language);
}


#endif

#if 1

/*!
   RLE decode for some resource data.

 \param[in] p_input : point to input data
 \param[in] data_size : input data size
 \param[out] p_output : point to output buffer
 \return : data size after decode
 */
static u32 rsc_rle_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output)
{
#if 0  
  u32 output_pos = 0;    //offset in output buffer
  u32 input_pos = 0;      //offset in input buffer
  u8 i = 0;                //position in temporary buffer
  u8 data;
  u8 size = 0;

  u8 *buf_addr;
  u32 unread_size, read_size, buf_size;

  buf_addr = rsc_get_comm_buf_addr();
  buf_size = rsc_get_comm_buf_size();
  
  unread_size = data_size;

  read_size = unread_size > buf_size ? buf_size : unread_size;
  unread_size -= read_size;
  rsc_read_data((u32)(input + input_pos), buf_addr, read_size);

  do
  {
    if(i < read_size)//read data from buffer
    {
      size = buf_addr[i];
    }
    else                        //read another nbytes from flash
    {
      read_size = unread_size > buf_size ? buf_size : unread_size;
      unread_size -= read_size;
      rsc_read_data((u32)(input + input_pos), buf_addr, read_size);
      i = 0;      
      size = buf_addr[i];   
    }
    input_pos++;
    i++;
    
    if(size <128)
    {
      if(i < read_size)//read data from buffer
      {
        data = buf_addr[i];
      }
      else                        //data is in another nbyters.
      {
        read_size = unread_size > buf_size ? buf_size : unread_size;
        unread_size -= read_size;
        rsc_read_data((u32)(input + input_pos), buf_addr, read_size);
        i = 0;
        data = buf_addr[i];
      }
      memset((output + output_pos), data, size);
      i ++;
      input_pos ++;
    }
    else
    {
      size -=128;
      if((i + size) < read_size)//can be read from memory completely
      {
        rsc_read_data((u32)(buf_addr + i), (output + output_pos), size);
      }
      else                                    //read from flash
      {
        rsc_read_data((u32)(input + input_pos), (output + output_pos), size);
      }
      i += size;
      input_pos += size;
    }
    output_pos += size;
  }while(input_pos < data_size);

  return output_pos;
#else
  u32 output_pos = 0;    //offset in output buffer
  u32 input_pos = 0;      //offset in input buffer
  u8 buffer[DATA_BUF_SIZE];
  u8 i = 0;                //position in temporary buffer
  u8 data = 0;
  u8 size = 0;

  rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);

  do
  {
    if(i < DATA_BUF_SIZE)//read data from buffer
    {
      size = buffer[i];
    }
    else                        //read another nbytes from flash
    {
      rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);
      i = 0;      
      size = buffer[i];   
    }
    input_pos++;
    i++;
    
    if(size <128)
    {
      if(i < DATA_BUF_SIZE)//read data from buffer
      {
        data = buffer[i];
      }
      else                        //data is in another nbyters.
      {
        rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);
        i = 0;
        data = buffer[i];
      }
      memset((p_output + output_pos), data, size);
      i ++;
      input_pos ++;
    }
    else
    {
      size -=128;
      if((i + size) < DATA_BUF_SIZE)//can be read from memory completely
      {
        rsc_read_data(rsc_handle, (u32)(buffer + i), (p_output + output_pos), size);
      }
      else                                    //read from flash
      {
        rsc_read_data(rsc_handle, (u32)(p_input + input_pos),
          (p_output + output_pos), size);
      }
      i += size;
      input_pos += size;
    }
    output_pos += size;
  }while(input_pos != data_size);

  return output_pos;
#endif

}

#else
static u32 rle_decode(u8 *rle, u32 *len, u8 *buf)
{
  u32 i, last, out_size;
  u8 size;

  out_size = 0;
  i = 0, last = *len - 1;
  do
  {
    size = *(rle + i);
    if(size < 128)
    {
      if(i > last)
      {
        break;
      }
      memset(buf + out_size, *(rle + i + 1), size);
      i += 2;
    }
    else
    {
      size -= 128;
      if(size + i > last)
      {
        break;
      }
      memcpy(buf + out_size, rle + i + 1, size);
      i += size + 1;
    }
    out_size += size;
  } while(i < *len);

  *len = i;
  return out_size;
}

#endif

/* external API */
/*!
    Get the resource head by it's resource type and id.

 \param[in] type 		: resource type
 \param[in] id		 	: resource id
 \return 				: if failed, return NULL, otherwise return the head of the resource.
 */
u32 rsc_get_hdr_addr(handle_t rsc_handle, u8 type, u16 id)
{
  rsc_info_t info;
  rsc_idx_t idx = {0};
  BOOL find = FALSE;
  u32 offset = 0;
  u16 i = 0;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(RSC_INVALID_ID == id)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr rsc_invalid_id \n\n");
    return 0;
  }
  offset = (u32)(p_rsc_info->p_rsc_data_addr + sizeof(rsc_info_t) * type);
  rsc_read_data(rsc_handle, offset, &info, sizeof(rsc_info_t));
#ifdef RSC_DEBUG
  rsc_dump_info(&info);
#endif

  if(info.type != type)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr offset:0x%x, info.type:%d, type:%d \n\n",
               offset,info.type,type);
    return 0;
  }

  offset = (u32)(p_rsc_info->p_rsc_data_addr + info.offset);
  if(type != RSC_TYPE_SCRIPT)
  {
    /* Notice: the index of resource must be continuous for this solution */
    offset += (id - 1/* id is base on 1*/) * sizeof(rsc_idx_t);
    rsc_read_data(rsc_handle, offset, &idx, sizeof(rsc_idx_t));
    find = (BOOL)(id == idx.id);
  }
  else
  {
    find = FALSE;
    for(i = 0; i < info.count; i++)
    {
      rsc_read_data(rsc_handle, offset, &idx, sizeof(rsc_idx_t));
#ifdef RSC_DEBUG
      rsc_dump_idx(&idx);
#endif

      if(id == idx.id)
      {
        find = TRUE;
        break;
      }
      offset += sizeof(rsc_idx_t);
    }
  }
  if(!find)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr rsc not find \n\n");
    return 0;
  }

  return (u32)(p_rsc_info->p_rsc_data_addr + info.offset + idx.offset);

}


/*!
  Get a decompressed resource data by it's head.

  \param[in] rsc_head	: resource head
  \return : if failed, return NULL, otherwise return the buffer address of the data
  */
static u8 *rsc_decompress_data(handle_t rsc_handle, rsc_comm_type_t type)
{
  u32 *p_buf_addr = NULL;
  u32 data_addr = 0, org_size = 0, cmp_size = 0, buf_size = 0;

  rsc_head_t *p_hdr_rsc = NULL;
  rsc_comm_info_t *p_comm_info = NULL;

  p_comm_info = _get_rsc_comm_info(rsc_handle, type);
  if(p_comm_info == NULL)
  {
    OS_PRINTF("rsc_decompress_data: can NOT get comm_info(type = %d), ERROR\n",
              type);
    return NULL;
  }
  p_hdr_rsc = _get_rsc_comm_head(p_comm_info);

  p_buf_addr = &p_comm_info->unzip_buf_addr;
  buf_size = p_comm_info->unzip_buf_size;

  switch(type)
  {
    case RSC_COMM_CFONT:
    case RSC_COMM_LFONT:      
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_font_t);
      break;
    case RSC_COMM_PAL:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_palette_t);
      break;
    case RSC_COMM_BMP:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_bitmap_t);
      break;
    case RSC_COMM_STRTAB:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_string_t);
      break;
    case RSC_COMM_SCRIPT:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_script_t);
      break;
    default:
      MT_ASSERT(0);
      return NULL;
  }

  org_size = p_hdr_rsc->org_size;
  cmp_size = p_hdr_rsc->cmp_size;
  
  switch(p_hdr_rsc->c_mode)
  {
    case  CMP_MODE_NONE:
      if(buf_size > 0)
      {
        /* buffer is ok, copy to buf */
        if(org_size > buf_size)
        {
          RSC_PRINTF(
            "RSC: buf_size is non-zero(%d), but NOT enough(%d), ERROR!\n",
            org_size, buf_size);
          MT_ASSERT(0);
        }
        else
        {
          rsc_read_data(rsc_handle, data_addr, (void *)(*p_buf_addr), org_size);
        }
      }
      else
      {
        /* no buffer, return address on FLASH */
        *p_buf_addr = data_addr;
      }
      break;

    case CMP_MODE_RLE:
      //os_mutex_lock(lock);
      if(org_size > buf_size)
      {
        RSC_PRINTF(
          "RSC: buf_size is non-zero(%d), but NOT enough(%d), ERROR!\n",
          org_size, buf_size);
        MT_ASSERT(0);
      }
      else
      {
        rsc_rle_decode(rsc_handle, (u8 *)data_addr, cmp_size, (u8 *)(*p_buf_addr));
      }
      break;

#if 0
    case CMP_MODE_ZIP:
      rsc_unzip(data, output);
      break;

    case CMP_MODE_JPG: /*only for bitmap*/
      if(RSC_TYPE_BITMAP != rsc_head->type)
      {
        return NULL;
      }
      rsc_jpg_to_bmp(data, output);
      break;
#endif
    default:
      return NULL;
  }

  return (u8 *)(*p_buf_addr);
}

/*!
    Get a rectangle style by style id

 \param[in] idx		: rectangle style id
 \return 			: a pointer point to the specified rstyle
 */
rsc_rstyle_t *rsc_get_rstyle(handle_t rsc_handle, u32 idx)
{
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(idx < p_rsc_info->rstyle_cnt)
  {
    return(p_rsc_info->p_rstyle_tab + idx);
  }
  else
  {
    return p_rsc_info->p_rstyle_tab;
  }
}

/*!
    Determine a rectangle style has a background or not

 \param[in] p_rstyle		: a pointer point to the specified rstyle
 \return 			: has background or not.
 */
BOOL rsc_is_rstyle_has_bg(rsc_rstyle_t *p_rstyle)
{
  // Using simple rules to determine now
  return (!RSTYLE_IS_R_IGNORE(p_rstyle->bg)
          && !RSTYLE_IS_R_COPY_BG(p_rstyle->bg));
}


/*!
    Determine a rectangle style need copy its background or not

 \param[in] p_rstyle		: a pointer point to the specified rstyle
 \return 			: need or not.
 */
BOOL rsc_is_rstyle_cpy_bg(rsc_rstyle_t *p_rstyle)
{
  // Using simple rules to determine now
  return RSTYLE_IS_R_COPY_BG(p_rstyle->bg);
}


/*!
   Get font style by style id.

 \param[in] idx		:font style id
 \return 			: a pointer point to the specified fstyle
 */
rsc_fstyle_t *rsc_get_fstyle(handle_t rsc_handle, u32 idx)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  MT_ASSERT(idx < p_rsc_info->fstyle_cnt);
  
  return (p_rsc_info->p_fstyle_tab + idx);
}


static BOOL rsc_check_char(rsc_font_t *p_hdr_font, u16 font_lib_id,
  font_map_t *p_font_map, u16 char_code, void *p_data, rsc_fstyle_t *p_fstyle)
{
  BOOL ret = FALSE;

  switch(p_hdr_font->type)
  {
    case RSC_FONT_TYPE_ARRAY:
      if(p_font_map->check == NULL)
      {
        if((char_code >= p_hdr_font->start_code)
            && (char_code <= p_hdr_font->end_code))
        {
          ret = TRUE;
        }
      }
      else
      {
        ret = p_font_map->check(font_lib_id, char_code);
      }      
      break;

    case RSC_FONT_TYPE_VECTOR:
      //to check vector font.
      ret = gui_vfont_check_char(char_code, p_data, p_hdr_font->head.org_size, p_fstyle, font_lib_id);
      break;

    default:
      MT_ASSERT(0);
  }



  return ret;
}


/*!
  find a font that can show the char_code in the font style id

  \param[out] p_hdr_font : font head
  \param[in] char_code : unicode
  \param[in] font_id : font style id, every id include several font file
  \return : font resource
  */
BOOL rsc_get_font_by_char(handle_t rsc_handle, rsc_fstyle_t *p_fstyle, u16 char_code, 
  rsc_font_t *p_hdr_font, u8 **p_data)
{
  u8 i = 0;
  u32 addr = 0;
  u16 font_lib_id = 0, last_cfont_id = 0, last_lfont_id = 0;
  rsc_font_info_t *p_cfont_info = NULL, *p_lfont_info = NULL;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  MT_ASSERT(p_fstyle != NULL);
  
  if(p_fstyle->font_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_cfont_info = (rsc_font_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_CFONT);
  p_lfont_info = (rsc_font_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_LFONT);
  
  MT_ASSERT(p_cfont_info != NULL && p_lfont_info != NULL);

  last_cfont_id = p_cfont_info->comm.id;
  last_lfont_id = p_lfont_info->comm.id;

  //check current font , we can save time of load resource
  for(i = 0; i < p_rsc_info->fmap_cnt; i++)
  {
    font_lib_id = p_rsc_info->p_fmap_tab[i].fontlib_id;
    if(p_rsc_info->p_fmap_tab[i].font_id == p_fstyle->font_id)
    {
      if((p_rsc_info->p_fmap_tab[i].font_buf_type == FONT_BUFFER_COMMON)
          && (font_lib_id == last_cfont_id))
      {
        //os_mutex_lock(g_rsc_info.font_buf_lock);
        *p_data = (u8 *)p_cfont_info->comm.unzip_buf_addr;
        
        if(rsc_check_char(&p_cfont_info->hdr_font, font_lib_id,
          p_rsc_info->p_fmap_tab + i, char_code, *p_data, p_fstyle))
        {
          memcpy(p_hdr_font, &p_cfont_info->hdr_font, sizeof(rsc_font_t));

          return TRUE;
        }
        //else
        //	rsc_unlock_data(RSC_TYPE_FONT);
      }

      if((p_rsc_info->p_fmap_tab[i].font_buf_type !=
           FONT_BUFFER_COMMON) && (font_lib_id == last_lfont_id))
      {
        //os_mutex_lock(g_rsc_info.font_buf_lock);
        *p_data = (u8 *)p_lfont_info->comm.unzip_buf_addr;

        if(rsc_check_char(&p_lfont_info->hdr_font, font_lib_id,
           p_rsc_info->p_fmap_tab + i, char_code, *p_data, p_fstyle))
        {
          memcpy(p_hdr_font, &p_lfont_info->hdr_font, sizeof(rsc_font_t));

          return TRUE;
        }
        //else
        //	rsc_unlock_data(RSC_TYPE_FONT);
      }
    }
  }

  //check other common font : only check it when font id is changed
  if(last_cfont_id != p_fstyle->font_id)
  {
    for(i = 0; i < p_rsc_info->fmap_cnt; i++)
    {
      font_lib_id = p_rsc_info->p_fmap_tab[i].fontlib_id;
      if((p_rsc_info->p_fmap_tab[i].font_id == p_fstyle->font_id)
          && (font_lib_id != last_cfont_id))
      {
        if(p_rsc_info->p_fmap_tab[i].font_buf_type == FONT_BUFFER_COMMON)
        {
          if((addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_FONT, font_lib_id)) != 0)
          {
            /* read from flash*/
            rsc_read_data(rsc_handle, addr, p_hdr_font, sizeof(rsc_font_t));

            p_cfont_info->comm.id = font_lib_id;
            p_cfont_info->comm.rsc_hdr_addr = addr;
            memcpy(&p_cfont_info->hdr_font, p_hdr_font, sizeof(rsc_font_t));

            *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_CFONT);


            if(rsc_check_char(p_hdr_font, font_lib_id, 
              p_rsc_info->p_fmap_tab + i, char_code, *p_data, p_fstyle))
            {
              // update font info
              // decompress font buffer

              return TRUE;
            }
            //else
            //{
            //	rsc_unlock_data(RSC_TYPE_FONT);
            //	break;
            //}
          }
        }
      }
    }
  }

  //check all local font
  for(i = 0; i < p_rsc_info->fmap_cnt; i++)
  {
    font_lib_id = p_rsc_info->p_fmap_tab[i].fontlib_id;
    if((p_rsc_info->p_fmap_tab[i].font_id == p_fstyle->font_id)
        && (font_lib_id != last_lfont_id))
    {
      if(p_rsc_info->p_fmap_tab[i].font_buf_type != FONT_BUFFER_COMMON)
      {
        if((addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_FONT, font_lib_id)) != 0)
        {
          /* read from flash*/
          rsc_read_data(rsc_handle, addr, p_hdr_font, sizeof(rsc_font_t));

          p_lfont_info->comm.id = font_lib_id;
          p_lfont_info->comm.rsc_hdr_addr = addr;
          memcpy(&p_lfont_info->hdr_font, p_hdr_font, sizeof(rsc_font_t));

          *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_LFONT);

          if(rsc_check_char(p_hdr_font, font_lib_id, 
            p_rsc_info->p_fmap_tab + i, char_code, *p_data, p_fstyle))
          {
            // update font info

            // decompress font buffer

            return TRUE;
          }
          //else
          //{
          //	rsc_unlock_data(RSC_TYPE_FONT);
          //}
        }
      }
    }
  }      

  return FALSE;
}

BOOL rsc_get_font(handle_t rsc_handle, u16 font_lib_id, rsc_font_t *p_hdr_font, u8 **p_data)
{
  u32 addr = 0;
  rsc_font_info_t *p_font_info = NULL;

  if(font_lib_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_font_info = (rsc_font_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_CFONT);
  MT_ASSERT(p_font_info != NULL);
 
  if(p_font_info->comm.id == font_lib_id)
  {
    *p_data = (u8 *)p_font_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_FONT, font_lib_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_font_info->comm.id = font_lib_id;
    p_font_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_font_info->hdr_font, sizeof(rsc_font_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_BMP);
  }

  memcpy(p_hdr_font, &p_font_info->hdr_font, sizeof(rsc_font_t));
  return TRUE;
}

//need mutex protect
/*!
  Get decompressed bitmap data.

  \param[in] bmp_id			: bitmap id
  \param[out] hdr_bmp			: a point pointer to the bitmap header
  \param[out] data			: a point pointer to the bitmap data buffer
  \return 					: bitmap resource head got by it's id
  */
BOOL rsc_get_bmp(handle_t rsc_handle, u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 **p_data)
{
  u32 addr = 0;
  rsc_bmp_info_t *p_bmp_info = NULL;

  if(bmp_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_bmp_info = (rsc_bmp_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_BMP);
  MT_ASSERT(p_bmp_info != NULL);
 
  if(p_bmp_info->comm.id == bmp_id)
  {
    *p_data = (u8 *)p_bmp_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_BITMAP, bmp_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_bmp_info->comm.id = bmp_id;
    p_bmp_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_bmp_info->hdr_bmp, sizeof(rsc_bitmap_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_BMP);
  }

  memcpy(p_hdr_bmp, &p_bmp_info->hdr_bmp, sizeof(rsc_bitmap_t));
  return TRUE;
}


/*!
    Get a string data by language id and string id.

 \param[in] id				: language id
 \param[in] string_idx		: string id
 \return					: string buffer
 */
BOOL rsc_get_string(handle_t rsc_handle, u16 strtab_id, u16 string_id, u8 **p_data)
{
  u16 offset = 0;
  u32 addr = 0;
  rsc_strtab_info_t *p_strtab_info = NULL;

  if(string_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_strtab_info = (rsc_strtab_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_STRTAB);
  MT_ASSERT(p_strtab_info != NULL);

  // check string table 
  if(p_strtab_info->comm.id == strtab_id)
  {
    addr = p_strtab_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_STRING, strtab_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }
    
    p_strtab_info->comm.id = strtab_id;
    p_strtab_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_strtab_info->hdr_strtab, sizeof(rsc_string_t));
    addr = (u32)rsc_decompress_data(rsc_handle, RSC_COMM_STRTAB);
  }

  // read the offset of string
  rsc_read_data(rsc_handle,
    addr + ((string_id - 1/* base on 1*/) << 1), &offset, sizeof(u16));
  
  *p_data = (u8 *)(addr + offset);

  return TRUE;
}

/*!
    Get palette resource.

 \param[in] palette_id		: palette id
 \param[out] entrys			: a point pointer to the palette buffer
 \return 					: palette resource head got by it's id
 */
BOOL rsc_get_palette(handle_t rsc_handle, u16 palette_id, rsc_palette_t *p_hdr_pal, u8 **p_data)
{
  u32 addr = 0;
  rsc_pal_info_t *p_pal_info = NULL;

  if(palette_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_pal_info = (rsc_pal_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_PAL);
  MT_ASSERT(p_pal_info != NULL);

  if(p_pal_info->comm.id == palette_id)
  {
    *p_data = (u8 *)p_pal_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_PALETTE, palette_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_pal_info->comm.id = palette_id;
    p_pal_info->comm.rsc_hdr_addr = addr;

    rsc_read_data(rsc_handle, addr, &p_pal_info->hdr_pal, sizeof(rsc_palette_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_PAL);
  }
  
  memcpy(p_hdr_pal, &p_pal_info->hdr_pal, sizeof(rsc_palette_t));
  return TRUE;
}

BOOL rsc_get_script(handle_t rsc_handle, u16 spt_id, rsc_script_t *p_hdr_spt, u8 **p_data)
{
  u32 addr = 0;
  rsc_script_info_t *p_spt_info = NULL;

  if(spt_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_spt_info = (rsc_script_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_SCRIPT);
  MT_ASSERT(p_spt_info != NULL);

  if(p_spt_info->comm.id == spt_id)
  {
    *p_data = (u8 *)p_spt_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_SCRIPT, spt_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_spt_info->comm.id = spt_id;
    p_spt_info->comm.rsc_hdr_addr = addr;

    rsc_read_data(rsc_handle, addr, &p_spt_info->hdr_spt, sizeof(rsc_script_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_SCRIPT);
  }
  
  memcpy(p_hdr_spt, &p_spt_info->hdr_spt, sizeof(rsc_script_t));
  return TRUE;
}

/*!
    search the character in character table.

 \param[in] tab			: character table
 \param[in] count			: total count
 \param[in] target			: target character code
 \return					: offset in character table
 */
u16 rsc_search(handle_t rsc_handle, u8 *p_tab, u16 count, u16 target)
{
  u16 value = 0, temp = 0;
  u16 start = 0, end = 0, mid = 0;
  BOOL is_in_buf = FALSE;
  u16 buf_start_at = 0;
  u16 *p_buf_addr = (u16 *)rsc_get_comm_buf_addr(rsc_handle);
  u32 buf_size = rsc_get_comm_buf_size(rsc_handle) / sizeof(u16);

  temp = target;

  start = 0; end = count;
  mid = (start + end) / 2;

  if(count < buf_size)
  {
    rsc_read_data(rsc_handle, (u32)p_tab, p_buf_addr, count * sizeof(u16));
    p_tab = (u8 *)p_buf_addr;
    buf_start_at = 0;
    is_in_buf = TRUE;
  }

  rsc_read_data(rsc_handle, (u32)(p_tab + (mid - buf_start_at) * 2), &value, sizeof(u16));

  while(temp != value)
  {
    if(temp < value)
    {
      end = mid - 1;
    }
    else  if(temp > value)
    {
      start = mid + 1;
    }
    if(start > end)
    {
      return count;
    }

    /* try to load into buf */
    if(!is_in_buf)
    {
      if((end - start + 1) < buf_size)
      {
        rsc_read_data(rsc_handle, (u32)(p_tab + start * 2), p_buf_addr,
                      (end - start + 1) * sizeof(u16));
        p_tab = (u8 *)p_buf_addr;

        buf_start_at = start;
        is_in_buf = TRUE;
      }
    }

    mid = (start + end) / 2;
    rsc_read_data(rsc_handle, (u32)(p_tab + (mid - buf_start_at) * 2), 
      &value, sizeof(u16));
  }
  return mid;
}


#define SHIFT1(x)    (7 - (x & 7))
static const u8 pix1mask [] = { 0x80, 0x40, 0x20, 0x10, 
                                0x08, 0x04, 0x02, 0x01 };

static inline u8 get_1bit_in_byte(u32 x, const u8 *p_byte)
{
  u8 tmp = *p_byte;
  tmp = tmp & pix1mask[x & 7];
  return(tmp >> SHIFT1(x));
}

#define SHIFT2(x)    (6 - (x & 6))
static const u8 pix2mask [] = { 0xC0, 0x00, 0x30, 0x00, 
                                0x0C, 0x00, 0x03, 0x00};
static inline u8 get_2bit_in_byte(u32 x, const u8 *p_byte)
{
  u8 tmp = *p_byte;
  tmp = tmp & pix2mask[x & 6];
  return (tmp >> SHIFT2(x));
}

//#ifdef GPE_4BIT_SUPPORT
static void init_pixel4_model(u32 fc, u32 bc, u8 *p_model)
{
  p_model[0] = (bc << 4) | bc; /*00*/
  p_model[1] = (bc << 4) | fc; /*01*/
  p_model[2] = (fc << 4) | bc; /*10*/
  p_model[3] = (fc << 4) | fc; /*11*/
}

u32 entend_char_to_u4buf(u8 *p_data, u16 width, u16 height, 
  u8 *p_buf, u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u32 s = 0, i = 0;
  u8 *p_cursor = NULL, *p_line = NULL;
  u16 aligned_width = 0; /* byte align */
  u8 pix4model[4];

  // init model
  init_pixel4_model(front_color, back_color, pix4model);

  p_cursor = p_line = (u8 *)p_buf;
  s = 0;

  if(width & 1)
  {
    aligned_width = 1;
    width -= aligned_width;
  }

  while(0 != height)
  {
    (height--);
    
    i = 0;
    while(i < width)
    {

      if(s & 1)
      {
        pixel = get_1bit_in_byte(s, p_data + (s >> 3));
        s++;
        pixel = (pixel << 1) | get_1bit_in_byte(s, p_data + (s >> 3));
        s++;
      }
      else
      {
      pixel = get_2bit_in_byte(s, p_data + (s >> 3));
      s += 2;
      }

      *p_cursor++ = pix4model[pixel];
      i += 2;
    }

    if(aligned_width > 0)
    {
      pixel = get_1bit_in_byte(s, p_data + (s >> 3)) << 1;
      *p_cursor = pix4model[pixel];
      s += aligned_width;
    }

    p_line += pitch;
    p_cursor = p_line;
  }

  return (u32)(p_cursor - p_buf);
}
//#endif

//#ifdef GPE_8BIT_SUPPORT
u32 entend_char_to_u8buf(u8 *p_data, u16 width, u16 height, 
  u8 *p_buf, u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u8 color[2];
  u32 i = 0;
  u32 j = 0;
  u8 *p_cursor = (u8 *)p_buf;
  //u32 cnt = width * height;
  
  color[0] = (u8)back_color;
  color[1] = (u8)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = p_buf + i * pitch;
    
    for(j = 0; j < width; j++)
    {
        pixel = get_1bit_in_byte((i * width + j), p_data + ((i * width + j) >> 3));

        *p_cursor++ = color[pixel];
    }
    
  }

  if((p_cursor - p_buf) != (height - 1) * pitch + width)
  {
    MT_ASSERT(0);
  }

  return (u32)(height * pitch);
  
  //return (u32)(p_cursor - p_buf);
}


//#endif

//#ifdef GPE_16BIT_SUPPORT
u32 entend_char_to_u16buf(u8 *p_data, u16 width, u16 height, u8 *p_buf,
  u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u16 color[2];
  u32 i = 0, j = 0;
  u16 *p_cursor = (u16 *)p_buf;
  //u32 cnt = width * height;

  color[0] = (u16)back_color;
  color[1] = (u16)front_color;
#if 0
  for(i = 0; i < cnt; i++)
  {
    pixel = get_1bit_in_byte(i, p_data + (i >> 3));
    *p_cursor++ = color[pixel];
  }

  return (u32)((u8 *)p_cursor - p_buf);
#else
  for(i = 0; i < height; i++)
  {
    p_cursor = (u16 *)(p_buf + i * pitch);

    for(j = 0; j < width; j++)
    {
      pixel = get_1bit_in_byte(i * width + j, p_data + ((i * width + j) >> 3));
     
      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_buf % 8 == 0);
  MT_ASSERT(pitch % 8 == 0);
  
  return (u32)(pitch * height);
#endif
}


//#endif

//#ifdef GPE_32BIT_SUPPORT
u32 entend_char_to_u32buf(u8 *p_data, u16 width, u16 height, u8 *p_buf,
  u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u32 color[2];
  u32 i = 0;
  u32 j = 0;
  u32 *p_cursor = (u32 *)p_buf;
  
  color[0] = (u32)back_color;
  color[1] = (u32)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = (u32 *)(p_buf + i * pitch);

    for(j = 0; j < width; j++)
    {
      pixel = get_1bit_in_byte(i * width + j, p_data + ((i * width + j) >> 3));
     
      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_buf % 8 == 0);
  MT_ASSERT(pitch % 8 == 0);
  
  return (u32)(pitch * height);
}


//#endif

/*!
    transform character data by it's font color.

 \param[in] p_input			: input font data
 \param[in] width			: character width
 \param[in] height			: character height
 \param[in] font_color		: font color
 \param[out] p_output			: output data buffer
 \return 					: color key
 */
static u32 rsc_transform_char(handle_t rsc_handle, u8 *p_input, u16 width, 
  u16 height, u8 *p_output, u32 font_color, u32 bpp)
{
  u32 color_key = 0, size = 0;
  u8 *p_char_buf = NULL;
  u32 pitch = 0;

  color_key = (0 == font_color) ? 1 : 0;
  size = ((width * height)+ 7) >> 3;
  
  if(bpp >= 16)
  {
    pitch = (width * (bpp / 8) + 7)  /  8 * 8;
  }
  else
  {
    pitch = (width * bpp + 7) >> 3;
  }

  p_char_buf = (u8 *)rsc_get_comm_buf_addr(rsc_handle);
  MT_ASSERT(size <= rsc_get_comm_buf_size(rsc_handle));
  
  if(size > rsc_get_comm_buf_size(rsc_handle))
  {
    return 0;
  }
  
  rsc_read_data(rsc_handle, (u32)p_input, p_char_buf, size);

  switch(bpp)
  {
//#ifdef GPE_4BIT_SUPPORT
    case 4:
      entend_char_to_u4buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;
//#endif

//#ifdef GPE_8BIT_SUPPORT
    case 8:
      entend_char_to_u8buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;
//#endif

//#ifdef GPE_16BIT_SUPPORT
    case 16:
      entend_char_to_u16buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;
//#endif

//#ifdef GPE_32BIT_SUPPORT
    case 32:
      entend_char_to_u32buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;
//#endif

    default:
      MT_ASSERT(0);
  }

  return color_key;
}


/*!
    Get font buffer id.

 \param[in] font_lib_id	: font lib id
 \return 				: if found it, return the buffer type, else return FONT_BUFFER_COMMON.
 */
font_buf_type_t rsc_get_font_buf_type(handle_t rsc_handle, u16 font_lib_id)
{
  u8 i = 0;
  font_map_t *p_maps = NULL;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  p_maps = p_rsc_info->p_fmap_tab;
  
  for(i = 0; i < p_rsc_info->fmap_cnt; i++)
  {
    if(p_maps->fontlib_id == font_lib_id)
    {
      return p_maps->font_buf_type;
    }
    p_maps++;
  }
  return FONT_BUFFER_COMMON;
}


/*!
    Get font interval.

 \param[in] head		: font resource head
 \return 				: if found the font, return interval, else return 0
 */
u8 rsc_get_font_interval(handle_t rsc_handle, u16 font_lib_id)
{
  u8 i = 0;
  font_map_t *p_maps = NULL;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  p_maps = p_rsc_info->p_fmap_tab;
  
  for(i = 0; i < p_rsc_info->fmap_cnt; i++)
  {
    if(p_maps->fontlib_id == font_lib_id)
    {
      return p_maps->interval;
    }
    p_maps++;
  }
  return 0;
}

/*!
    Get string attrib.

 \param[in] char_code	: unicode of the character
 \param[in] style		: font style
 \param[out] p_width		: width of the character
 \param[out] p_height		: height of the character
 \return 				: NULL
 */
void rsc_get_char_attr(handle_t rsc_handle, u16 char_code,
  rsc_fstyle_t *p_style, u16 *p_width, u16 *p_height)
{
  rsc_font_t hdr_font;
  u8 *p_font_data = NULL;
  u8 interval = 0;
  u16 array_count = 0;
  u16 char_offset = 0;
  u8 *p_index_tab = NULL, *p_width_tab = NULL;
  u8 char_width = 0;

  if(!rsc_get_font_by_char(rsc_handle, p_style, char_code, &hdr_font, &p_font_data))
  {
    *p_width = *p_height = 0;
    return;
  }

  interval = rsc_get_font_interval(rsc_handle, hdr_font.head.id);
  
  switch(hdr_font.type)
  {
    case RSC_FONT_TYPE_ARRAY:
      if(hdr_font.width > 0) // monospaced font
      {
        *p_width = hdr_font.width;
      }
      else
      {
        array_count = hdr_font.count;
        p_index_tab = p_font_data;
        p_width_tab = p_font_data + array_count * sizeof(u16);

        /*find char*/
        char_offset = rsc_search(rsc_handle, p_index_tab, array_count, char_code);

        rsc_read_data(rsc_handle, (u32)(p_width_tab + char_offset), &char_width, sizeof(u8));
        *p_width = char_width + interval;
      }

      *p_height = hdr_font.height;      
      break;

    case RSC_FONT_TYPE_VECTOR:
      gui_vfont_get_char_attr(char_code,
        p_font_data, hdr_font.head.org_size, p_style, p_width, p_height);      
      break;

    default:
      MT_ASSERT(0);
  }
}

/*!
    Get string attrib.

 \param[in] str			: head of the font resource
 \param[in] style		: font style
 \param[out] width		: width of the character
 \param[out] height		: height of the character
 \return 				: SUCCESS
 */
s32 rsc_get_string_attr(handle_t rsc_handle, u16 *p_str, rsc_fstyle_t *p_style, u16 *p_width,
                            u16 *p_height)
{
#if 0
  u16 char_width, char_height;
  u16 code;

  *width = 0;
  *height = 0;

  rsc_read_data((u32)str, &code, sizeof(u16));
  while(0 != code)
  {
    rsc_get_char_attr(code, style, &char_width, &char_height);
    *width += char_width;
    *height = MAX(*height, char_height);
    str++;
    rsc_read_data((u32)str, &code, sizeof(u16));
  }

  return SUCCESS;
#else
  u16 char_width = 0, char_height = 0;
  u16 i = 0, blank = 0;
  u16 buf[DATA_BUF_SIZE];
  u16 *p_buf_addr = NULL;
  BOOL is_use_buf = rsc_is_data_on_flash((u32)p_str);

  *p_width = 0;
  *p_height = 0;

  p_buf_addr = is_use_buf ? buf : p_str;

  if(is_use_buf)
  {
    rsc_read_data(rsc_handle, (u32)p_str, p_buf_addr, DATA_BUF_SIZE * sizeof(u16));
  }
  
  while(0 != p_buf_addr[i])
  {
    rsc_get_char_attr(rsc_handle, p_buf_addr[i], p_style, &char_width, &char_height);
    *p_width += char_width;
    *p_height = MAX(*p_height, char_height);
    i++;

    if(is_use_buf)
    {
      if(i == DATA_BUF_SIZE)
      {
        p_str += DATA_BUF_SIZE;
        i = 0;
        rsc_read_data(rsc_handle, (u32)p_str, p_buf_addr, DATA_BUF_SIZE * sizeof(u16));
      }
    }
  }


  if(p_buf_addr[0] == 0)//actually it's "\n", temp solution
  {
    blank = char_asc2uni(' ');
    rsc_get_char_attr(rsc_handle, blank, p_style, &char_width, &char_height);

    *p_height = MAX(*p_height, char_height);

  }
  return SUCCESS;  
#endif
}

static u8 *hit_font_cache(handle_t rsc_handle, u16 code, rsc_fstyle_t *p_style, 
  u16 *p_width, u16 *p_height, u16 *p_xstep)
{
  char_cache_t ccache;
  u8 i = 0, j = 0;
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  for(i = 0; i < p_rsc_info->char_cache_cnt; i++) //search in cache
  {
    if((p_rsc_info->p_char_cache_array[i].char_code == code)
        && (memcmp(&p_rsc_info->p_char_cache_array[i].char_style, 
        p_style, sizeof(rsc_fstyle_t))== 0))//found it in cache
    {
      ccache = p_rsc_info->p_char_cache_array[i];

      for(j = i; j > 0; (j--))
      {
        p_rsc_info->p_char_cache_array[j] =
          p_rsc_info->p_char_cache_array[j - 1];
      }

      p_rsc_info->p_char_cache_array[0] = ccache;

      *p_width = ccache.width;
      *p_height = ccache.height;
      *p_xstep = ccache.x_step;
      
      return (u8 *)ccache.p_data_buffer;
    }
  }
  
  return NULL;
}

static u8 *insert_font_cache(handle_t rsc_handle, u16 code, rsc_fstyle_t *p_style, 
  u16 width, u16 height, u16 x_step)
{
  u8 i = 0, *p_data_buf = NULL;
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;  
  MT_ASSERT(p_rsc_info != NULL);

  p_data_buf = 
    p_rsc_info->p_char_cache_array[p_rsc_info->char_cache_cnt -1].p_data_buffer;
  
  for(i = (p_rsc_info->char_cache_cnt - 1); i > 0; (i--))
  {
    p_rsc_info->p_char_cache_array[i] = p_rsc_info->p_char_cache_array[i - 1];
  }

  p_rsc_info->p_char_cache_array[0].p_data_buffer = p_data_buf;
  p_rsc_info->p_char_cache_array[0].char_code = code;
  p_rsc_info->p_char_cache_array[0].char_style = *p_style;
  p_rsc_info->p_char_cache_array[0].width = width;
  p_rsc_info->p_char_cache_array[0].height = height;
  p_rsc_info->p_char_cache_array[0].x_step = x_step;

  return p_data_buf;
}
/*!
    Get a character resource data.

 \param[in] head		: head of the font resource
 \param[in] style		: font style
 \param[in] char_code	: unicode of the character
 \param[out] width		: width of the character
 \param[out] height		: height of the character
 \param[out] color_key	: color key of the character
 \return 				: if success, return the data buffer, else return null.
 */
BOOL rsc_get_char(handle_t rsc_handle, rsc_font_t *p_hdr_font, u8 *p_font_data,
  rsc_fstyle_t *p_style, u16 char_code, rsc_char_info_t *p_info, u32 bpp)
{
  u16 char_offset = 0, char_count = 0;
  u32 data_offset = 0;
  u8 *p_index_tab = NULL, *p_width_tab = NULL;
  u8 *p_offset_tab = NULL, *p_font = NULL;
  u8 char_width = 0, interval = 0;
  u32 size = 0;
  u8 *p_cache = NULL;
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  switch(p_hdr_font->type)
  {
    case RSC_FONT_TYPE_ARRAY:
      p_info->p_alpha = NULL;

      p_info->alpha_pitch = 0;

      if((p_info->p_char = hit_font_cache(rsc_handle, char_code,
        p_style, &p_info->width, &p_info->height, &p_info->x_step)) != NULL)
      {
        p_info->ckey = p_style->color ? 0 : 1;

        return TRUE;
      }

      interval = rsc_get_font_interval(rsc_handle, p_hdr_font->head.id);
      char_count = p_hdr_font->count;
      p_index_tab = p_font_data;
      p_width_tab = p_font_data + char_count * sizeof(u16);
      p_offset_tab = p_width_tab + char_count * sizeof(u8);
      p_font = p_offset_tab + char_count * sizeof(u32);

      /*find char*/
      char_offset = rsc_search(rsc_handle, p_index_tab, char_count, char_code);
      rsc_read_data(rsc_handle, (u32)(p_offset_tab + sizeof(u32) * char_offset),
                    &data_offset, sizeof(u32));

      p_font += data_offset;
      p_info->height = p_hdr_font->height;

      rsc_read_data(rsc_handle, (u32)(p_width_tab + char_offset), &char_width, sizeof(u8));
      p_info->width = char_width;
      p_info->x_step = char_width + interval;

      size = (u32)(p_info->height * ((p_info->width * bpp + 7) >> 3));
      if(size > p_rsc_info->char_cache_size)
      {
        RSC_PRINTF("w[%d], h[%d], bpp[%d], char_cache_size[%d], need_size[%d]\n",
          p_info->width, p_info->height, bpp, p_rsc_info->char_cache_size, size);
        //rsc_unlock_data(hdr_font->head.type);
        return FALSE;
      }

      //os_mutex_lock(g_rsc_info.char_lock);
      p_info->p_char = insert_font_cache(rsc_handle, char_code,
        p_style, p_info->width, p_info->height, p_info->x_step);

      p_info->ckey = rsc_transform_char(rsc_handle, p_font, p_info->width,
                      p_info->height, p_info->p_char, p_style->color, bpp);
      
      p_info->xoffset = 0;
      p_info->yoffset = 0;
      break;

    case RSC_FONT_TYPE_VECTOR:
      if(gui_vfont_hit_cache(char_code, p_style, p_info))
      {
        p_info->ckey = p_style->color ? 0 : 1;
        
        return TRUE;
      }

      p_cache = gui_vfont_insert_cache(char_code, p_style);

      p_info->ckey = gui_vfont_get_char(char_code, p_font_data, p_hdr_font->head.org_size,
        p_style, p_cache, p_info);

      break;

    default:
      MT_ASSERT(0);
  }
  

  //rsc_unlock_data(hdr_font->head.type);

  return TRUE;
}


/*!
    Set current language type.

 \param[in] language		: language id.
 \return 					: NULL
 */
void rsc_set_curn_language(handle_t rsc_handle, u16 language)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_rsc_info->curn_language_id = language;
}


/*!
    Get language id

 \return 			: language id
 */
u16 rsc_get_curn_language(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->curn_language_id;
}


/*!
   set current language.

 \param[in]language			: palette id
 \return					: NULL
 */
void rsc_set_curn_palette(handle_t rsc_handle, u16 palette)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_rsc_info->curn_palette_id = palette;
}

/*!
   get current language.

 \return					: palette id
 */
u16 rsc_get_curn_palette(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->curn_palette_id;
}


static void *rsc_alloc(handle_t rsc_handle, u32 size)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return (void *)lib_memp_alloc(&p_rsc_info->heap, size);
}

static void *rsc_align_alloc(handle_t rsc_handle, u32 size, u32 alignment)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return (void *)lib_memp_align_alloc(&p_rsc_info->heap, size, alignment);
}

static void *rsc_align_free(handle_t rsc_handle, void *p_buf)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return (void *)lib_memp_align_free(&p_rsc_info->heap, p_buf);
}


static void rsc_free(handle_t rsc_handle, void *p_buf)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  lib_memp_free(&p_rsc_info->heap, p_buf);
}


u8 * rsc_get_comm_buf_addr(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(p_rsc_info->comm_buf_size == 0)
  {
    return NULL;
  }

  return p_rsc_info->p_comm_buf_addr;
}

u32 rsc_get_comm_buf_size(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->comm_buf_size;
}

static void init_rsc_comm_info(handle_t rsc_handle, u8 type, u32 buf_size)
{
  rsc_comm_info_t *p_rsc_info = NULL;

  p_rsc_info = _get_rsc_comm_info(rsc_handle, type);
  MT_ASSERT(p_rsc_info != NULL);
  
  if(buf_size > 0)
  {
    p_rsc_info->unzip_buf_addr = (u32)rsc_alloc(rsc_handle, buf_size);
  }
#ifdef MUTI_THREAD_SUPPORT
  if(SUCCESS != os_mutex_create(&p_rsc_info->lock))
  {
    return ERR_FAILURE;
  }
#endif
  p_rsc_info->unzip_buf_size = buf_size;
  p_rsc_info->id = RSC_INVALID_ID;
}
/*!
    Resource initialize, allocate memory for all resource buffer.

 \param[in] init_info		: it specified buffer size of the resource buffer
 \return 				: return ERR_FAILURE or SUCESS
 */
#define ALIGN_TO_4BYTES(x)    (((u32)(x) + 3) & (~0x3))

#define ALIGN_TO_8BYTES(x)    (((u32)(x) + 8))

handle_t rsc_init(rsc_config_t *p_config)
{
  u32 i = 0, size = 0;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)mmi_alloc_buf(sizeof(rsc_main_t));
  memset((void *)p_rsc_info, 0, sizeof(rsc_main_t));
  
  p_rsc_info->p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_rsc_info->p_charsto_dev != NULL);

  p_rsc_info->is_uncached = p_config->is_uncached;
    
  p_rsc_info->p_rsc_data_addr = (u8 *)p_config->rsc_data_addr;

  // init data heap
  MT_ASSERT(p_config->comm_buf_size > 0);

  size += ALIGN_TO_4BYTES(p_config->comm_buf_size);
  size += ALIGN_TO_4BYTES(p_config->strtab_buf_size);
  size += ALIGN_TO_4BYTES(p_config->comm_font_buf_size);
  size += ALIGN_TO_4BYTES(p_config->local_font_buf_size);
  size += ALIGN_TO_4BYTES(p_config->pal_buf_size);
  size += ALIGN_TO_4BYTES(sizeof(char_cache_t) * p_config->char_cache_cnt); 

  if(FALSE == p_config->is_uncached)  
  {
    size += ALIGN_TO_4BYTES(p_config->char_cache_size * p_config->char_cache_cnt);
    size += ALIGN_TO_4BYTES(p_config->bmp_buf_size);
    size += MEMP_PIECE_HEADER_SIZE * 9;
    
  }
  else
  {
    size += ALIGN_TO_8BYTES(p_config->char_cache_size) * p_config->char_cache_cnt;
    size += ALIGN_TO_8BYTES(p_config->bmp_buf_size);
    size += MEMP_PIECE_HEADER_SIZE * (8 + p_config->char_cache_cnt);
  }
  

  p_rsc_info->p_heap_addr = mmi_create_memp(&p_rsc_info->heap, size);
  MT_ASSERT(p_rsc_info->p_heap_addr != NULL);

  /* common data buffer */
  if(p_config->comm_buf_size > 0)
  {
    p_rsc_info->p_comm_buf_addr = rsc_alloc(p_rsc_info, p_config->comm_buf_size);
  }
  p_rsc_info->comm_buf_size = p_config->comm_buf_size;

  /* bmp buffer */
  if(TRUE == p_config->is_uncached)
  {
    if(p_config->bmp_buf_size > 0)
    {
      p_rsc_info->bmp_info.comm.unzip_buf_addr = 
               (u32)rsc_align_alloc((handle_t)p_rsc_info, p_config->bmp_buf_size, 8);
      
      MT_ASSERT(p_rsc_info->bmp_info.comm.unzip_buf_addr != 0);
    }
    p_rsc_info->bmp_info.comm.unzip_buf_size = p_config->bmp_buf_size;
    p_rsc_info->bmp_info.comm.id = RSC_INVALID_ID;

  }
  else
  {
    init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_BMP, p_config->bmp_buf_size);
  }
  
  /* string table buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_STRTAB, p_config->strtab_buf_size);

  /* palette buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_PAL, p_config->pal_buf_size);

  /* font buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_CFONT, p_config->comm_font_buf_size);
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_LFONT, p_config->local_font_buf_size);
  
  /* script buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_SCRIPT, p_config->script_buf_size);

  /* char cache */
  if((p_config->char_cache_size > 0) && (p_config->char_cache_cnt > 0))
  {
    p_rsc_info->p_char_cache_array = rsc_alloc((handle_t)p_rsc_info,
      p_config->char_cache_cnt * sizeof(char_cache_t));
      
    if(TRUE == p_config->is_uncached)
    {
      p_rsc_info->p_char_cache_buf_addr = NULL;
      for(i = 0; i < p_config->char_cache_cnt; i++)
      {
        p_rsc_info->p_char_cache_array[i].p_data_buffer =
          rsc_align_alloc((handle_t)p_rsc_info, p_config->char_cache_size, 8);

        MT_ASSERT(p_rsc_info->p_char_cache_array[i].p_data_buffer != NULL);
      }
    }
    else
    {
      p_rsc_info->p_char_cache_buf_addr =
        rsc_alloc((handle_t)p_rsc_info, p_config->char_cache_cnt * p_config->char_cache_size);
      for(i = 0; i < p_config->char_cache_cnt; i++)
      {
        p_rsc_info->p_char_cache_array[i].p_data_buffer =
          p_rsc_info->p_char_cache_buf_addr + i * p_config->char_cache_size;
      }
    }
  }
  p_rsc_info->char_cache_size = p_config->char_cache_size;
  p_rsc_info->char_cache_cnt = p_config->char_cache_cnt;

  /*others*/
  p_rsc_info->curn_language_id = RSC_INVALID_ID;
  p_rsc_info->curn_palette_id = RSC_INVALID_ID;

  p_rsc_info->p_rstyle_tab = p_config->p_rstyle_tab;
  p_rsc_info->rstyle_cnt = p_config->rstyle_cnt;
  p_rsc_info->p_fstyle_tab = p_config->p_fstyle_tab;
  p_rsc_info->fstyle_cnt = p_config->fstyle_cnt;
  p_rsc_info->fmap_cnt = p_config->fmap_cnt;
  p_rsc_info->p_fmap_tab = p_config->p_fmap_tab;
  p_rsc_info->flash_base = p_config->flash_base;

#ifdef RSC_DEBUG
  rsc_dump_global_info(p_rsc_info);
#endif

  return (handle_t)p_rsc_info;
}


/*!
    Release memory.

 \return			: NULL
 */
void rsc_release(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  u32 i = 0;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(TRUE == p_rsc_info->is_uncached)
  {
    if(p_rsc_info->bmp_info.comm.unzip_buf_addr != 0)
    {
      rsc_align_free(rsc_handle, (void *)p_rsc_info->bmp_info.comm.unzip_buf_addr);
    }

    for(i = 0; i < p_rsc_info->char_cache_cnt; i++)
    {
      if(p_rsc_info->p_char_cache_array[i].p_data_buffer != NULL)
      {
        rsc_align_free(rsc_handle, p_rsc_info->p_char_cache_array[i].p_data_buffer);
      }
    }

    if(p_rsc_info->p_char_cache_array != NULL)
    {
      rsc_free(rsc_handle, p_rsc_info->p_char_cache_array);
    }
  }

  mmi_destroy_memp(&p_rsc_info->heap, p_rsc_info->p_heap_addr);

  //release global variable.
  mmi_free_buf((void *)p_rsc_info);
}

/*!
    Get  unistring width.

 \param[in] f_style_idx		: font style id
 \param[in] str				: string data buffer
 \return 					: string width
 */
u16 rsc_get_unistr_width(handle_t rsc_handle, u16 *p_str, u32 f_style_idx)
{
  rsc_fstyle_t *p_fstyle = NULL;
  u16 str_height = 0, str_width = 0;

  p_fstyle = rsc_get_fstyle(rsc_handle, f_style_idx);

  if(rsc_get_string_attr(rsc_handle, p_str, p_fstyle, &str_width, &str_height) != SUCCESS)
  {
    return 0;
  }

  return str_width;
}


/*!
    Get a string width by style id and string id.

 \param[in] f_style_idx		: font style id
 \param[in] string_id		: string id
 \return 					: string width
 */
u16 rsc_get_strid_width(handle_t rsc_handle, u16 string_id, u32 f_style_idx)
{
  u8 *p_bits = NULL;
  u16 str_width = 0;

//  rsc_lock_data(RSC_TYPE_STRING);
  if(!rsc_get_string(rsc_handle, rsc_get_curn_language(rsc_handle), string_id, (u8 **)&p_bits))
  {
    return 0;
  }
  str_width = rsc_get_unistr_width(rsc_handle, (u16 *)p_bits, f_style_idx);
//  rsc_unlock_data(RSC_TYPE_STRING);
  return str_width;
}


void rsc_read_data(handle_t rsc_handle, u32 offset, void *p_buf, u32 len)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
//  static u32 times = 0;

  if(rsc_is_data_on_flash(offset)) //read data from flash
  {
#ifdef __LINUX__
if(offset > 0x90000000)
{
charsto_read(p_rsc_info->p_charsto_dev,
             (offset - p_rsc_info->flash_base), (u8 *)p_buf, len);
}
else
{
    memcpy(p_buf, (void *)offset, len);
}
#else
        charsto_read(p_rsc_info->p_charsto_dev,
             (offset - p_rsc_info->flash_base), (u8 *)p_buf, len);

#endif
//    times++;
//    OS_PRINTF("Oh....rsc read data %d tims\n", times);
  }
  else //read data from flash
  {
    memcpy(p_buf, (void *)offset, len);
  }
}


