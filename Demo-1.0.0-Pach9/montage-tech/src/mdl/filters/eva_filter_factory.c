/******************************************************************************/
/******************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "eva.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "sink_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "src_filter.h"
#include "transf_filter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"

#include "ap_framework.h"

//filters
BOOL fsrc_filter_check_parameter(void);
ifilter_t * fsrc_filter_create(void);

BOOL fsink_filter_check_parameter(void);
ifilter_t * fsink_filter_create(void);

BOOL mp3_decode_filter_check_parameter(void);
ifilter_t * mp3_decode_filter_create(void);

BOOL mp3_transfer_filter_check_parameter(void);
ifilter_t * mp3_transfer_filter_create(void);

BOOL ts_player_filter_check_parameter(void);
ifilter_t * ts_player_filter_create(void);

BOOL av_render_filter_check_parameter(void);
ifilter_t * av_render_filter_create(void);

BOOL record_filter_check_parameter(void);
ifilter_t * record_filter_create(void);

BOOL pic_filter_check_parameter(void);
ifilter_t * pic_filter_create(void);

BOOL pic_render_filter_check_parameter(void);
ifilter_t * pic_render_filter_create(void);

BOOL dmx_filter_check_parameter(void);
ifilter_t * dmx_filter_create(void);

/*!
  filter factory
  */
typedef struct tag_filter_factory
{
  /*!
    this filter is enabled in current version
    */
  BOOL enabled;
  /*!
    this filter's id
    */
  filter_id_t id;
  /*!
    this filter's create function
    */
  ifilter_t * (* filter_create)(void);
  /*!
    this filter is ready
    */
  BOOL (* check)(void);
}filter_factory_t;

static filter_factory_t g_filter_list[] =
{
  {TRUE, FILE_SOURCE_FILTER, fsrc_filter_create, fsrc_filter_check_parameter},
#ifndef JAZZ    
  {TRUE, FILE_SINK_FILTER, fsink_filter_create, fsink_filter_check_parameter},
  {TRUE, MP3_DECODE_FILTER, mp3_decode_filter_create, mp3_decode_filter_check_parameter},
  {TRUE, MP3_TRANSFER_FILTER, mp3_transfer_filter_create, mp3_transfer_filter_check_parameter},
  {TRUE, TS_PLAYER_FILTER, ts_player_filter_create, ts_player_filter_check_parameter},
  {TRUE, AV_RENDER_FILTER, av_render_filter_create, av_render_filter_check_parameter},
#endif  
  {TRUE, JPEG_FILTER, pic_filter_create, pic_filter_check_parameter},
  {TRUE, PIC_RENDER_FILTER, pic_render_filter_create, pic_render_filter_check_parameter},    
#ifndef JAZZ    
  {TRUE, RECORD_FILTER, record_filter_create, record_filter_check_parameter},
  {TRUE, DEMUX_FILTER, dmx_filter_create, dmx_filter_check_parameter},
#endif  
};

RET_CODE eva_add_filter_by_id(filter_id_t id, ifilter_t **pp_filter)
{
  u32 filter_cnt = sizeof(g_filter_list) / sizeof(filter_factory_t);
  u32 i = 0;

  for(i = 0; i < filter_cnt; i++)
  {
    if(g_filter_list[i].id == id && g_filter_list[i].enabled)
    {
      if((NULL == g_filter_list[i].check) || g_filter_list[i].check())
      {
        *pp_filter = g_filter_list[i].filter_create();
        return SUCCESS;
      }
      else
      {
        EVA_ERROR("can't load the filter, the filter's attribute is unsafe.\n");
        return ERR_FAILURE;
      }
    }
  }

  if(i == filter_cnt)
  {
    EVA_ERROR("can't found the filter, please make sure the id is valid.\n");
  }
  
  return ERR_FAILURE;
}

RET_CODE eva_filter_factory_init(void)
{
  MT_ASSERT((filter_id_t)APP_LAST < UNKNOWN_FILTER_ID);
  //read config file and disable some filters
  //g_filter_list[i].enable = FALSE;
  return SUCCESS;
}


