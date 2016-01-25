/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_EMAIL_PICTURE_H__
#define __UI_EMAIL_PICTURE_H__

/* coordinate */
#define EMAIL_CONT_FULL_X         15
#define EMAIL_CONT_FULL_Y         50
#define EMAIL_CONT_FULL_W        EMAIL_ICON_FULL_W
#define EMAIL_CONT_FULL_H         EMAIL_ICON_FULL_H

#define EMAIL_ICON_FULL_X          0
#define EMAIL_ICON_FULL_Y          0
#define EMAIL_ICON_FULL_W         36
#define EMAIL_ICON_FULL_H          36

#define EMAIL_CONT_PREV_W       EMAIL_ICON_PREV_W
#define EMAIL_CONT_PREV_H       EMAIL_ICON_PREV_H
#define EMAIL_CONT_PREV_GAP     10


#define EMAIL_ICON_PREV_X         0
#define EMAIL_ICON_PREV_Y         0
#define EMAIL_ICON_PREV_W        31
#define EMAIL_ICON_PREV_H         31



typedef enum
{
  EMAIL_PIC_SHOW = 0,
  EMAIL_PIC_HIDE,
  EMAIL_PIC_FLASH
}email_pic_show_mode_t;

BOOL ui_is_email(void);
void update_email_picture(email_pic_show_mode_t e_show_mode);

 
#endif


