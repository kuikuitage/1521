/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_NUM_PLAY_H__
#define __UI_NUM_PLAY_H__

/* coordinate */
#define NUM_PLAY_CONT_X       0
#define NUM_PLAY_CONT_Y       0
#define NUM_PLAY_CONT_W       SCREEN_WIDTH
#define NUM_PLAY_CONT_H       60

#define NUM_PLAY_FRM_X       0
#define NUM_PLAY_FRM_Y       0
#define NUM_PLAY_FRM_W      160
#define NUM_PLAY_FRM_H       NUM_PLAY_CONT_H

#define NUM_PLAY_TXT_X       4
#define NUM_PLAY_TXT_Y       4
#define NUM_PLAY_TXT_W      (NUM_PLAY_FRM_W-2*NUM_PLAY_TXT_X)
#define NUM_PLAY_TXT_H       (NUM_PLAY_FRM_H-2*NUM_PLAY_TXT_Y)

#ifdef LCN_SWITCH
#define NUM_PLAY_CNT        4
#else
#define NUM_PLAY_CNT        3
#endif
/* rect style */
#define RSI_NUM_PLAY_CONT     RSI_TRANSPARENT
#define RSI_NUM_PLAY_TXT     RSI_TRANSPARENT

/* font style */
#define FSI_NUM_PLAY_TXT     FSI_NUMBER
#define FSI_NUM_PLAY_TXT_BIG     FSI_NUMBER_BIG

/* others */

RET_CODE open_num_play(u32 para1, u32 para2);

#ifdef LCN_SWITCH
u16 get_pos_by_logic_num(u16 logic_num);
#endif
void ui_set_input_number_cont(u8 cont);
void ui_set_input_number_on_right(void);
void ui_set_num_big_font(void);

#endif

