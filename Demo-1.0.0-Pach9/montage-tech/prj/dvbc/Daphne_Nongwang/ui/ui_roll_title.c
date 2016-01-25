/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2012).
*              All rights reserved.
* Owner:       Lei Chen <lei.chen@montage-tech.com>
****************************************************************************/

#include "ui_common.h"

#include "ui_roll_title.h"

static u32 p_bmp[LENGTH_OF_ROLL_BMP];
extern void* p_sub_rgn;;


/***************************************************************************
函数名：	OnKeyPress_Is_Normal_or_Not
函数描述：	判断是不是需要字幕滚动的函数
入口参数：	flg -- 暂时没什么用，可以直接传0

返回值：1--表示当前状态需要滚动,其他表示不需要滚动的状态
******************************************************************************/
RET_CODE subt_is_scroll(BOOL *flag)
{
  RET_CODE ret = SUCCESS;
  if(ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    *flag = 1;
  }
  else
  {
    *flag = 0;
  }
  return ret;
}

RET_CODE osd_display_is_ok(BOOL *flag)
{
  RET_CODE ret = SUCCESS;
  *flag = 1;
  return ret;
}

RET_CODE scroll_step_pixel_set(u8 *p_pixel_num)
{
  RET_CODE ret = SUCCESS;
  customer_cfg_t cus_cfg = {0};

  get_customer_config(&cus_cfg);

  *p_pixel_num = 2;
  
  return ret;
}

RET_CODE channel_info_set(u16 *network_id, u16 *ts_id, u16 *service_id)
{
  u16 prog_id;
  dvbs_prog_node_t prog_node;
  RET_CODE ret = SUCCESS;

  prog_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(prog_id, &prog_node);
  
  *network_id = prog_node.orig_net_id;
  *ts_id = prog_node.ts_id;
  *service_id = prog_node.s_id;
  return ret;
}


RET_CODE draw_rectangle(u32 x, u32 y, u32 width, u32 height, u32 color)
{
  RET_CODE ret = SUCCESS;
  UI_PRINTF("draw_rectangle x=%d,y=%d,width=%d,heigh=%dt,color=%d\n",x,y,width,height,color);
  #if 0
  control_t *p_cont, *p_ctrl;
  u32 i, j;
  u32 ctrl_height, ctrl_width;
  u16 *p_bit_data = NULL;
  u16 *temp = NULL;
  bitmap_t *bit_map = NULL;
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_ctrl = ctrl_get_child_by_id(p_cont,1);
  bmap_set_content_type(p_ctrl, TRUE);
  bit_map = (bitmap_t *)bmap_get_content(p_ctrl);
  p_bit_data = (u16 *)(bit_map->p_bits);
  temp = p_bit_data;
  ctrl_height = p_ctrl->frame.bottom - p_ctrl->frame.top;
  ctrl_width = p_ctrl->frame.right - p_ctrl->frame.left;
  x = 0;
  y = SCREEN_HEIGHT - 100;
  width = SCREEN_WIDTH;
  height = SCREEN_HEIGHT;
  UI_PRINTF("@@@@@ enter draw_rectangle\n");
  //if(color == 255)
  //{
    for(i = 0; i < ctrl_height; i++)
    {
      if(i < y || i > (height + y) )
      {
        for(j = 0; j < ctrl_width; j++)
        {
          temp++;
        }
      }
      else
      {
        for(j = 0; j < x; j++)
        {
          temp++;
        }
        for(j = 0; j < width; j++)
        {
          *temp = C_TRANS;
          temp++;
        }
        for(j = 0; j < PROG_BAR_subt_pic_W; j++)
        {
          temp++;
        }
      }
        
    }
  //}

  bmap_set_content_by_data(p_ctrl, bit_map);
  #endif
  return ret;
}

extern void ui_clear_adv();
BOOL ui_set_roll_title_show (BOOL show)
{
  u32 i=0, k=0, j=0;
  void* p_gpe_dev = NULL;
  rect_t rgn_rect = {0};
  rect_t fill_rect = {0};
  gpe_param_vsb_t gpe_param = {0};
  void* p_disp;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  if (!show)
  {
  #if 1
    p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_gpe_dev);

    for(i = 0; i < ROLL_TITLE_ERASE_H; i++)
    {
      for(j = 0; j < ROLL_TITLE_RGN_W; j++)
      {
        p_bmp[k] = 0x00000000;
        k++;
      }
    }

    UI_PRINTF("ui_set_roll_title_show clear\n");

    rgn_rect.left = 0;
    rgn_rect.top = 0;
    rgn_rect.right = ROLL_TITLE_RGN_W;
    rgn_rect.bottom = ROLL_TITLE_ERASE_H;

    gpe_draw_image_vsb(p_gpe_dev, p_sub_rgn, &rgn_rect,
        p_bmp, NULL, 0, ROLL_TITLE_W * 4,
        ROLL_TITLE_RGN_W*ROLL_TITLE_ERASE_H, PIX_FMT_ARGB8888, &gpe_param, &fill_rect);
  #endif
  }

  if(disp_layer_is_show(p_disp, DISP_LAYER_ID_SUBTITL) != show)
  {
    disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, show);
    UI_PRINTF("ui_set_roll_title_show[%d]\n",show);
  }
  
  return TRUE;
}

/******************************************************************************

函数名：	FixupRectangleDepth

Parameters:  1) x:			滚屏的横坐标
             2) y: 			滚屏的纵坐标
             3) width: 		滚屏的宽度
             4) height:		滚屏的高度
             5) Bmpwidth: 	图片的真正有效像素宽
             6) source:		两张图片一起滚动时的第一张图片的数据地址
			 7) source1:  	两张图片一起滚动时的第二张图片的数据地址
			 8) Scoll_time: 用于判断是一张图片还是两张图片
			(Scoll_time == 0)//第一张图片刚开始慢慢出来
			(Scoll_time == 1)//最后一张图片开始慢慢消失
			(Scoll_time == 2)//两张图片链接起来一起滚动的
******************************************************************************/
extern BOOL is_tr_title_started();

static u32 inl(u32 port)
{
    return *((volatile u32*)(port));
}

RET_CODE draw_picture(u32 x, u32 y,
                               u32 width,u32 height,
                               u8 *source,u8 *source1,
                               u32 Bmpwidth,u8 Scoll_time)
{
  u16 *temp = NULL;
  static u32 temp_test1 = 0;
  static u32 hw_ticks1 = 0;
  u32 hw_ticks2 = 0;
  u32 temp_test2 = 0;
  u16 *temp1 = NULL;
  u32 i=0, k=0, j=0;
  u8 alpha = 0xFF;
  s32 modify_width;
  RET_CODE ret = ERR_FAILURE;
  void* p_gpe_dev = NULL;
  void *p_disp = NULL;
  gpe_param_vsb_t gpe_param = {0};
  rect_t rgn_rect = {0};
  rect_t fill_rect = {0};

  p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe_dev);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

  //UI_PRINTF("draw_picture 11\n");

  if(temp_test1 == 0)
  {
    temp_test1 = inl(0x6f910000) & 0xffff0000;
    temp_test2 = temp_test1;

    while(temp_test1 == temp_test2)
    {
      //mtos_task_sleep(1);
      temp_test2 = inl(0x6f910000) & 0xffff0000;
    }
    hw_ticks1 = mtos_hw_ticks_get();
  }
  else // temp_test1 == 1
  {
   hw_ticks2 = mtos_hw_ticks_get();
   while(((hw_ticks2 - hw_ticks1)/(165*1000)) <= 19)
   {
      //mtos_task_sleep(1);
      hw_ticks2 = mtos_hw_ticks_get();
   }
    temp_test1 = 0;
  }

  if(!is_tr_title_started())
  {
    #if 1
    UI_PRINTF("draw_picture has stoped\n");
    for(i = 0; i < ROLL_TITLE_ERASE_H; i++)
    {
      for(j = 0; j < ROLL_TITLE_RGN_W; j++)
      {
        p_bmp[k] = 0x00000000;
        k++;
      }
    }
    #endif
  }
  else if(Bmpwidth > ROLL_TITLE_W)
  {
    //第一张图片刚开始慢慢出来
    if(Scoll_time == 0)
    {
      k = 0;
      temp = (u16 *)source;
      if(width <= ROLL_TITLE_W)
      {
        for(i = 0; i < height; i++)
        {
          for(j = 0; j < ROLL_TITLE_W - width; j++)
          {
            p_bmp[k] = 0x00000000;
            k++;
          }
          for(j = 0; j < width; j++)
          {        
            if(((((*temp)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            } 
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp++;
          }
          temp = temp + (Bmpwidth - width);
        }
      }
      else
      {
        for(i = 0; i < height; i++)
        {
          for(j = 0; j < width - ROLL_TITLE_W; j++)
          {
            temp++;
          }
          for(j = 0; j < ROLL_TITLE_W; j++)
          {      
            if(((((*temp)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }         
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp++;
          }
          temp = temp + (Bmpwidth - width);
      }
      }
    }
    else if(Scoll_time == 1)  //最后一张图片开始慢慢消失
    {
    
      modify_width = width;
      modify_width -= 8;
      if(modify_width <= 0)
      {
        return ret;
      }
      
      k = 0;
      temp = (u16 *)source;

      if(modify_width > ROLL_TITLE_W)
      {
        for(i = 0; i < height; i++)
        {
          temp = temp + (Bmpwidth - modify_width);
          
          for(j = 0; j < ROLL_TITLE_W; j++)
          {       
            if(((((*temp)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }         
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp++;
         }

          temp = temp + (modify_width - ROLL_TITLE_W);
       
        }
      }
      else
      {
        for(i = 0; i < height; i++)
        {
          temp = temp + (Bmpwidth - modify_width);
          
          for(j = 0; j < modify_width; j ++)
          {
            if(((((*temp)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }     
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp++;
          }
          for(j = 0; j < ROLL_TITLE_W - modify_width; j++)
          {
            p_bmp[k] = 0x00000000;
            k++;
          }
        }
      }
    }
    else if(Scoll_time == 2)   //两张图片链接起来一起滚动的
    {
      k = 0;
      temp = (u16 *)source;
      temp1 = (u16 *)source1;
      if(width <= ROLL_TITLE_W)
      {
        for(i = 0; i < height; i++)
        {
          temp = temp + ((Bmpwidth - ROLL_TITLE_W) + width);
          
          for(j = 0; j < ROLL_TITLE_W - width; j++)
          {
            if(((((*temp)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }         
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp++;
          }
          for(j = 0; j < width; j++)
          {
            if(((((*temp1)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp1++;
          }
          temp1 = temp1 + (Bmpwidth - width);
        }
      }
      else
      {
        for(i = 0; i < height; i++)
        {
          temp1 = temp1 + (width - ROLL_TITLE_W);
          
          for(j = 0; j < ROLL_TITLE_W; j++)
          {
            if(((((*temp1)&0x8000))>>15)==1)
            {
              alpha = 0xff;
            }
            else
            {
              alpha = 0x00;
            }          
            p_bmp[k] = (alpha==0)? 0x00:0xffffffff;
            k++;
            temp1++;
          }
          temp1 = temp1 +(Bmpwidth - width);
        }
      }
    }
    else
    {
      return ret;
    }
  }
  else
  {
    return ret;
  }
    
  rgn_rect.left = 0;
  rgn_rect.top = 20;
  rgn_rect.right = ROLL_TITLE_W+rgn_rect.left;
  rgn_rect.bottom = height+rgn_rect.top;
  
  gpe_draw_image_vsb(p_gpe_dev, p_sub_rgn, &rgn_rect,
      p_bmp, NULL, 0, ROLL_TITLE_W * 4,
      ROLL_TITLE_W*height, PIX_FMT_ARGB8888, &gpe_param, &fill_rect);

  ret = SUCCESS;
  
  return ret;

}




RET_CODE draw_roll_title_to_trans()
{
  control_t *p_cont, *p_ctrl;
  u16 p_bmp[60480];
  u32 i, j, k;
  RET_CODE ret = ERR_FAILURE;
  bitmap_t bit_map = {0};
  
  k = 0;
  for(i = 0; i < ROLL_TITLE_H; i++)
  {
    for(j = 0; j < ROLL_TITLE_W; j++)
    {
      p_bmp[k] = C_TRANS;
      k++;
    }
  }
  bit_map.format = 11;
  bit_map.enable_ckey = 0;
  bit_map.bpp = 16;
  bit_map.width = ROLL_TITLE_W;
  bit_map.height = ROLL_TITLE_H; 
  bit_map.pitch = ROLL_TITLE_W * 2;
  bit_map.colorkey = 16384255;
  bit_map.is_font = 0;
  bit_map.p_bits = (u8 *)p_bmp;

  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_ROLL_TITLE);

  //ctrl_move_ctrl(control_t * p_ctrl,s16 x,s16 y)
  bmap_set_content_type(p_ctrl, TRUE);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
  bmap_set_content_by_data(p_ctrl,&bit_map);
  ctrl_paint_ctrl(p_cont, TRUE);
  
  return ret;
}


