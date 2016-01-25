#ifndef __UI_AD_LOG_H__
#define __UI_AD_LOG_H__


/* coordinate */
#define ADLOGO_X_GAP			(60)
#define ADLOGO_Y_GAP			(20)

void ui_create_ad_logo(u8 block_id);

void ui_show_ad_logo();
void ui_hide_ad_logo(void);
BOOL ui_adlogo_ison(void);

#endif

