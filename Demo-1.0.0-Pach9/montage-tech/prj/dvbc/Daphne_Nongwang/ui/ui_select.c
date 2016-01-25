/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_num_play.h"
#include "ui_select.h"

static help_item_fc item_fc = {IM_SN_F1,IM_SN_F1,IM_SN_F1,IM_SN_F1};
#if(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)
static BOOL g_disable_display_time = FALSE;
#endif
static u8 g_input_cont = NUM_PLAY_CNT;
static BOOL g_pos_on_right = FALSE;
static BOOL g_num_big_font = FALSE;

/*
  set item bmp on help meue from F1~F3 key or color key;default color;
  */
void ui_set_help_item_img_f(void)
{
    item_fc.help_item_fc1 = IM_SN_F1;
    item_fc.help_item_fc2 = IM_SN_F2;
   item_fc.help_item_fc3 = IM_SN_F3;
   item_fc.help_item_fc4 = IM_SN_F4;
}
/*
  get item bmp on help meue from F1~F3 key or color key;default color;
  */
void ui_get_help_item_img_fc1(help_item_fc * p_item_fc)
{
  memcpy(p_item_fc,&item_fc,sizeof(help_item_fc));
}

/*
  set time no display on info bar;default display time on bar;
  */
 #if(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)
void ui_set_disable_display_time_status(void)
{
   g_disable_display_time = TRUE;
}
BOOL ui_get_disable_display_time_status(void)
{
  return g_disable_display_time;
}
/*
  set input program number max cont,default 4;
  */
void ui_set_input_number_cont(u8 cont)
{
    g_input_cont = cont;
}
#endif

u8 ui_get_input_number_cont(void)
{
    return g_input_cont ;
}

/*
  set input program number position on right,default on right;
  */
#if(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)
void ui_set_input_number_on_right(void)
{
    g_pos_on_right = TRUE;
}
#endif

BOOL ui_get_input_number_on_right(void)
{
    return g_pos_on_right;
}

/*
  set input program number display font big,default in small;
  */
#if(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)
void ui_set_num_big_font(void)
{
  g_num_big_font = TRUE;
}
#endif

BOOL ui_get_num_big_font(void)
{
    return g_num_big_font;
}
