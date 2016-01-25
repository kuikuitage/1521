/******************************************************************************/
/******************************************************************************/
#include  "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_sem.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "drv_dev.h"
#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "eit.h"
#include "tdt.h"
#include "mt_time.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"

#include "lib_rect.h"
#include "common.h"

#include "region.h"
#include "display.h"
#include "pdec.h"
#include "eva_filter_factory.h"
#include "file_source_filter.h"
#include "ap_framework.h"
#include "ap_pic_play.h"
#include "pic_render_filter.h"
#include "pic_filter.h"
#include "sys_dbg.h"



#ifdef PRINT_ON
#define PLAYER_DEBUG
#endif

#ifdef PLAYER_DEBUG
#define PLAYER_DBG(x)  (OS_PRINTF x)
#else
#define PLAYER_DBG(x)  do {} while(0);
#endif

/*!
  pic player private information
  */
typedef struct
{
  /*!
    controller
    */
  controller_t g_controller;
  /*!
    chain
    */
  void *p_f_chain;
  /*!
    source filter
    */
  void *p_source_filter;
 /*!
   play filter
   */
   void *p_pic_play_filter;
 /*!
   render filter
   */
  void *p_pic_render_filter;
  /*!
    jpeg parameter
    */
  ap_pic_data_t pic_data;
}pic_one_t;

/*!
  pic player private information
  */
typedef struct
{
  /*!
    player instance
    */
  app_t pic_instance;
  /*!
    max picture count same time
    */
  u32 max_pic_cnt;
  /*!
    current picture count
    */
  u32 cur_pic_cnt;
  /*!
    max picture count same time
    */
  u32 dec_pic_index;
  /*!
    reserved
    */
  //u8 reserved;
  /*!
    controller
    */
  pic_one_t *p_pic;
}pic_player_priv_t;

static void send_msg_to_ui(u32 id, u32 param1, u32 param2)
{
  event_t evt = {0};
  
  evt.id = id;
  evt.data1 = param1;
  evt.data2 = param2;
  ap_frm_send_evt_to_ui(APP_PICTURE, &evt);
}

static RET_CODE ap_pic_play_start(pic_player_priv_t *p_priv, u32 index)
{
  chain_t *p_chain = (chain_t *)p_priv->p_pic[index].p_f_chain;
  ifilter_t *p_fsrc_filter = p_priv->p_pic[index].p_source_filter;
  ifilter_t *p_pic_filter = p_priv->p_pic[index].p_pic_play_filter;
  ifilter_t *p_render_filter = p_priv->p_pic[index].p_pic_render_filter;
  icmd_t icmd = {0};
  src_pin_attr_t sp_attr = {0};
  u32 input_size = 0; 
  u32 output_size = 0;
  rect_size_t rect_size = {0};
  u8 *p_input = NULL;
  u8 *p_output = NULL;
  pic_filter_cfg_t pic_cfg = {0};

  rect_size.w = RECTWP(&(p_priv->p_pic[index].pic_data.rect));
  rect_size.h = RECTHP(&(p_priv->p_pic[index].pic_data.rect));
  if(p_priv->p_pic[index].pic_data.file_or_mem)
  {
    input_size = ((p_priv->p_pic[index].pic_data.file_size - 1) / 8 + 1) * 8;
    MT_ASSERT(p_priv->p_pic[index].pic_data.p_mem != NULL);
    MT_ASSERT(p_priv->p_pic[index].pic_data.mem_size >= input_size);
    output_size = p_priv->p_pic[index].pic_data.mem_size - input_size;
   // memset(p_priv->p_pic[index].pic_data.p_mem, 0, input_size + output_size);
    p_input = p_priv->p_pic[index].pic_data.p_mem;
    p_output = p_priv->p_pic[index].pic_data.p_mem + input_size;
  }
  else
  {
    output_size = rect_size.w * rect_size.h * 4 + 512;
    MT_ASSERT(p_priv->p_pic[index].pic_data.p_mem != NULL);
    MT_ASSERT(p_priv->p_pic[index].pic_data.mem_size >= output_size);
    output_size = p_priv->p_pic[index].pic_data.mem_size;
//    memset(p_priv->p_pic[index].pic_data.p_mem, 0, output_size);
    p_output = p_priv->p_pic[index].pic_data.p_mem;
  }

  //config source pin
  if(p_priv->p_pic[index].pic_data.file_or_mem)
  {
    icmd.cmd = FSRC_CFG_FILE_NAME;
    icmd.p_para = p_priv->p_pic[index].pic_data.file_name;
    //sp_attr.src = SRC_FROM_FILE;
  }
  else
  {
    OS_PRINTF("\n##debug: pic_data.buf_get [0x%x]\n", p_priv->p_pic[index].pic_data.buf_get);
    icmd.cmd = FSRC_CFG_FILE_NAME;
    icmd.lpara = p_priv->p_pic[index].pic_data.buf_get;
    icmd.p_para = NULL;
    //sp_attr.src = SRC_FROM_BUFFER;
  }
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);

  //config pin
  sp_attr.buffers = 2;
  sp_attr.buffer_size = 100 * KBYTES;
  sp_attr.is_circular = FALSE;
  sp_attr.is_share = FALSE;
  icmd.cmd = FSRC_CFG_SOURCE_PIN;
  icmd.p_para = &sp_attr;
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);
#if 0
  if(p_priv->p_pic[index].pic_data.file_or_mem)
  {
    //config jpeg filter input pin
    icmd.cmd = PIC_CFG_INPUT_BUFFER;
    icmd.lpara = input_size;
    icmd.p_para = p_input;
    p_pic_filter->do_command(p_pic_filter, &icmd);
  }

  //config jpeg filter output pin
  icmd.cmd = PIC_CFG_OUTPUT_BUFFER;
  icmd.lpara = output_size;
  icmd.p_para = p_output;
  p_pic_filter->do_command(p_pic_filter, &icmd);

  //config render pin
  icmd.cmd = PIC_RENDER_CFG_RECT;
  icmd.p_para = &p_priv->p_pic[index].pic_data.rect;
  p_render_filter->do_command(p_render_filter, &icmd);

  icmd.cmd = PIC_RENDER_CFG_RGN;
  icmd.p_para = p_priv->p_pic[index].pic_data.p_rgn;
  p_render_filter->do_command(p_render_filter, &icmd);

  icmd.cmd = PIC_CFG_SIZE;
  icmd.lpara = 0;
  icmd.p_para = (void *)&rect_size;
  p_pic_filter->do_command(p_pic_filter, &icmd);

  icmd.cmd = PIC_CFG_FLIP;
  icmd.lpara = PIC_FLIP_0;
  icmd.p_para = NULL;
  p_pic_filter->do_command(p_pic_filter, &icmd);

  icmd.cmd = PIC_CFG_OUT_FMT;
  icmd.lpara = JPEG_OUTFMT_444;//PIC_OUTFMT_422;
  icmd.p_para = NULL;
  p_pic_filter->do_command(p_pic_filter, &icmd);

  icmd.cmd = PIC_CFG_DEC_MODE;
  icmd.lpara = p_priv->p_pic[index].pic_data.dec_mode;
  icmd.p_para = NULL;
  p_pic_filter->do_command(p_pic_filter, &icmd);
#else
  pic_cfg.input_buf_size = input_size;
  pic_cfg.p_input_buf = p_input;
  pic_cfg.out_buf_size = output_size;
  pic_cfg.p_out_buf = p_output;
  pic_cfg.w = rect_size.w;
  pic_cfg.h = rect_size.h;
  pic_cfg.flip = PIC_FLIP_0;
  pic_cfg.out_fmt = JPEG_OUTFMT_444;
  pic_cfg.dec_mode = p_priv->p_pic[index].pic_data.dec_mode;
  
  icmd.cmd = PIC_FILTER_CFG;
  icmd.lpara = 0;
  icmd.p_para = &pic_cfg;
  p_pic_filter->do_command(p_pic_filter, &icmd);

#endif

  //config render pin
  icmd.cmd = PIC_RENDER_CFG_RECT;
  icmd.p_para = &p_priv->p_pic[index].pic_data.rect;
  p_render_filter->do_command(p_render_filter, &icmd);

  icmd.cmd = PIC_RENDER_CFG_RGN;
  icmd.p_para = p_priv->p_pic[index].pic_data.p_rgn;
  p_render_filter->do_command(p_render_filter, &icmd);


  p_chain->open(p_chain);
  p_chain->start(p_chain);

  return SUCCESS;
}

static RET_CODE ap_pic_play_stop(pic_player_priv_t *p_priv)
{
  chain_t *p_chain = NULL;
  u32 loopi = 0;
  
  for (loopi = 0; loopi < p_priv->dec_pic_index; loopi ++)
  {
    OS_PRINTF("\n##debug: ap_pic_play_stop [%d]\n", loopi);
    p_chain = (chain_t *)p_priv->p_pic[loopi].p_f_chain;
    p_chain->stop(p_chain);
    p_chain->close(p_chain);
  }
  return SUCCESS;
}

static void pic_process_msg(void *p_handle, os_msg_t *p_msg)
{
  pic_player_priv_t *p_priv = (pic_player_priv_t *)p_handle;
  RET_CODE ret = SUCCESS;
  u32 loopi = 0;

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_msg != NULL);
  
  switch (p_msg->content)
  {
  case PIC_CMD_START:
  case PIC_CMD_SYNC_START:
    OS_PRINTF("\n##debug: pic_process_msg [%d]\n", p_msg->para1);
    p_priv->cur_pic_cnt = p_msg->para1;
    for (loopi = 0; loopi < p_priv->cur_pic_cnt; loopi ++)
    {
      memcpy(&(p_priv->p_pic[loopi].pic_data),
        (void *)(p_msg->para2 + loopi * sizeof(ap_pic_data_t)), sizeof(ap_pic_data_t));
    }
    p_priv->dec_pic_index = 0;
    //ret = ap_pic_play_start(p_priv, p_priv->dec_pic_index ++);
    if(p_priv->dec_pic_index < p_priv->cur_pic_cnt)
    {
      ret = ap_pic_play_start(p_priv, p_priv->dec_pic_index++);
    }

    if(p_priv->dec_pic_index < p_priv->cur_pic_cnt)
    {
       ret = ap_pic_play_start(p_priv, p_priv->dec_pic_index++);
    }
    
    if (p_msg->content == PIC_CMD_SYNC_START)
    {
      send_msg_to_ui(PIC_PLAYER_EVT_START, 0, 0);
    }
    break;

  case PIC_CMD_STOP:
  case PIC_CMD_SYNC_STOP:
    ret = ap_pic_play_stop(p_priv);
    if (p_msg->content == PIC_CMD_SYNC_STOP)
    {
      send_msg_to_ui(PIC_PLAYER_EVT_STOPED, 0, 0);
    }
    break;

  default:
    break;
  }
}

static void pic_state_machine(void *p_handle)
{

}

/*!
   The init operation will be invoked once when system power up
  */
static void init(void *p_handle)
{
  pic_player_priv_t *p_priv_data = (pic_player_priv_t *)p_handle;

  MT_ASSERT(p_priv_data != NULL);
  
}

static RET_CODE on_pic_draw_end(handle_t _this, os_msg_t *p_msg)
{
  controller_t *p_contrl = (controller_t *)_this;
  pic_player_priv_t *p_priv  = p_contrl->get_part_handle(p_contrl);

  MT_ASSERT(p_priv != NULL);
  
  switch (p_msg->content)
  {
  case PIC_DRAW_END:
    OS_PRINTF("\n##debug: PIC_PLAYER_EVT_DRAW_END!!\n");
    send_msg_to_ui(PIC_PLAYER_EVT_DRAW_END, 0, 0);
    break;
  case PIC_DRAW_DYNAMIC_END:
    OS_PRINTF("\n##debug: PIC_DRAW_DYNAMIC_END!!\n");
    //if (p_priv->dec_pic_index < p_priv->cur_pic_cnt)
    //{
    //  ap_pic_play_start(p_priv, p_priv->dec_pic_index ++);
    //   return SUCCESS;
    //}
    send_msg_to_ui(PIC_PLAYER_UPDATE_FRAME, 0, 0);
    break;
  default:
    break;
  }
  return SUCCESS;
}

BOOL create_pic_chain(pic_player_priv_t *p_priv, u32 priority, u32 index)
{
  chain_t   *p_f_chain = NULL;
  ifilter_t *p_fsrc_filter = NULL;
  ifilter_t *p_pic_filter = NULL;
  ifilter_t *p_render_filter = NULL;
  ipin_t *p_output_pin = NULL;
  ipin_t *p_input_pin = NULL;
  chain_para_t chain_para = {0};
  RET_CODE ret = SUCCESS;
  media_format_t format = {0};
  controller_t *p_contrl = &(p_priv->p_pic[index].g_controller);
  ctrl_para_t ctrl_para = {(void *)p_priv};

  controller_create(p_contrl, &ctrl_para);
  p_contrl->on_process_evt = on_pic_draw_end;
  //create chain
  chain_para.p_owner = (interface_t *)&(p_priv->p_pic[index].g_controller);
  if (index)
  {
    chain_para.p_name = "pic_chain1";
  }
  else
  {
    chain_para.p_name = "pic_chain";
  }
  chain_para.priority = priority;
  chain_para.stk_size = 16 * KBYTES;
  p_f_chain = chain_create(&chain_para);

  p_priv->p_pic[index].p_f_chain = p_f_chain;

  //create filter
  ret = eva_add_filter_by_id(FILE_SOURCE_FILTER, &p_fsrc_filter);
  MT_ASSERT(ret == SUCCESS);

  ret = eva_add_filter_by_id(JPEG_FILTER, &p_pic_filter);
  MT_ASSERT(ret == SUCCESS);

  ret = eva_add_filter_by_id(PIC_RENDER_FILTER, &p_render_filter);
  MT_ASSERT(ret == SUCCESS);

  p_priv->p_pic[index].p_source_filter = p_fsrc_filter;
  p_priv->p_pic[index].p_pic_play_filter = p_pic_filter;
  p_priv->p_pic[index].p_pic_render_filter = p_render_filter;

  //add to chain
  ret = p_f_chain->add_filter(p_f_chain, p_fsrc_filter, "source filter");
  MT_ASSERT(ret == SUCCESS);
  ret = p_f_chain->add_filter(p_f_chain, p_pic_filter, "pic filter");
  MT_ASSERT(ret == SUCCESS);
  ret = p_f_chain->add_filter(p_f_chain, p_render_filter, "render filter");
  MT_ASSERT(ret == SUCCESS);

  //1. >>-------------link source filter and jpeg filter ------------------------>>>>
  //get source pin
  ret = p_fsrc_filter->get_unconnect_pin(p_fsrc_filter, OUTPUT_PIN, &p_output_pin);
  MT_ASSERT(ret == SUCCESS);
  MT_ASSERT(p_output_pin != NULL);

  //get jpeg filter input pin
  ret = p_pic_filter->get_unconnect_pin(p_pic_filter, INPUT_PIN, &p_input_pin);
  MT_ASSERT(ret == SUCCESS);
  MT_ASSERT(p_input_pin != NULL);

  ret = p_f_chain->connect(p_f_chain, p_output_pin, p_input_pin, NULL);
  MT_ASSERT(ret == SUCCESS);

  //2. >>-------------link jpeg filter and render filter ------------------------>>>>
  //get jpeg filter output pin
  ret = p_pic_filter->get_unconnect_pin(p_pic_filter, OUTPUT_PIN, &p_output_pin);
  MT_ASSERT(ret == SUCCESS);
  MT_ASSERT(p_output_pin != NULL);

  //get render pin
  ret = p_render_filter->get_unconnect_pin(p_render_filter, INPUT_PIN, &p_input_pin);
  MT_ASSERT(ret == SUCCESS);
  MT_ASSERT(p_input_pin != NULL);

  ret = p_f_chain->connect(p_f_chain, p_output_pin, p_input_pin, &format);
  MT_ASSERT(ret == SUCCESS);

  return TRUE;
}
 

app_t *construct_ap_pic_play(ap_pic_policy_t *p_policy)
{
  pic_player_priv_t *p_priv = NULL;
  u32 pic_cnt = 1;
  u32 loopi = 0;
  MT_ASSERT(p_policy != NULL);

  if (p_policy->max_pic_cnt_one_time)
  {
    pic_cnt = p_policy->max_pic_cnt_one_time;
  }
  p_priv = mtos_malloc(sizeof(pic_player_priv_t) + pic_cnt * sizeof(pic_one_t));
  MT_ASSERT(p_priv != NULL);

  memset((void *)p_priv, 0, sizeof(pic_player_priv_t) + pic_cnt * sizeof(pic_one_t));
  p_priv->max_pic_cnt = pic_cnt;
  p_priv->p_pic = (pic_one_t *)((u32)p_priv + sizeof(pic_player_priv_t));
  memset(&(p_priv->pic_instance), 0, sizeof(app_t));
  
  //Attach record instance
  p_priv->pic_instance.get_msgq_timeout = NULL;

  p_priv->pic_instance.init = init;
  p_priv->pic_instance.process_msg = pic_process_msg;
  p_priv->pic_instance.state_machine = pic_state_machine;
  p_priv->pic_instance.p_data = (void *)p_priv;

  for (loopi = 0; loopi < pic_cnt; loopi ++)
  {
    create_pic_chain(p_priv, p_policy->pic_chain_priority[loopi], loopi);
  }

  return &(p_priv->pic_instance);
}

//end of file
