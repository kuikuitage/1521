/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_SIGNAL_H__
#define __UI_SIGNAL_H__

/* coordinate */
#define SIGNAL_CONT_FULL_X  ((SCREEN_WIDTH-SIGNAL_CONT_FULL_W)/2)
#define SIGNAL_CONT_FULL_Y  ((SCREEN_HEIGHT-SIGNAL_CONT_FULL_H)/2 -20)
#define SIGNAL_CONT_FULL_W  300
#define SIGNAL_CONT_FULL_H  100

#define SIGNAL_ICON_FULL_X  10
#define SIGNAL_ICON_FULL_Y  18
#define SIGNAL_ICON_FULL_W  45
#define SIGNAL_ICON_FULL_H  45

#define SIGNAL_TITLE_FULL_X  0
#define SIGNAL_TITLE_FULL_Y  0
#define SIGNAL_TITLE_FULL_W  SIGNAL_CONT_FULL_W
#define SIGNAL_TITLE_FULL_H  40

#define SIGNAL_CONTENT_FULL_X  10
#define SIGNAL_CONTENT_FULL_Y  (SIGNAL_TITLE_FULL_Y+SIGNAL_TITLE_FULL_H+5)
#define SIGNAL_CONTENT_FULL_W  (SIGNAL_CONT_FULL_W - 2*SIGNAL_CONTENT_FULL_X)
#define SIGNAL_CONTENT_FULL_H  50

#define SIGNAL_CONT_PREVIEW_W 230
#define SIGNAL_CONT_PREVIEW_H 40
#define SIGNAL_CONT_PRE_VGAP 8

#define SIGNAL_TXT_PREVIEW_X  0
#define SIGNAL_TXT_PREVIEW_Y  0
#define SIGNAL_TXT_PREVIEW_W  SIGNAL_CONT_PREVIEW_W
#define SIGNAL_TXT_PREVIEW_H  SIGNAL_CONT_PREVIEW_H

/* font style */
#define FSI_SIGNAL_TXT     FSI_COMM_TXT_N

void update_ca_message(u16 strid);

void update_signal (void);

void close_signal(void);

u16 get_singal_strid(void);

void set_signal_strid(u16 *str, u16 id);

void clear_signal_strid(u16 *str);

void clean_ca_prompt(void);
#endif


