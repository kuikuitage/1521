#ifndef __UI_AD_LOG_API_H__
#define __UI_AD_LOG_API_H__

#define AD_SCREEN_WIDTH  720
#define AD_SCREEN_HEIGHT 576

#ifdef MINGXIN_GIF_TEST
void ui_adv_play_static(void);
#endif

BOOL ui_set_pic_show (BOOL show);
#ifdef PATULOUS_FUNCTION_ADS
BOOL get_pic_stop_status(void);
BOOL ui_pic_play(ads_ad_type_t type, u8 root_id);
BOOL ui_check_pic_by_type(ads_ad_type_t type);
#endif
void ui_pic_stop(void);

void pic_set_init_ret(BOOL ret);

void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h);

void ui_adv_set_category_index(u8 index);



BOOL ui_is_pic_showing (void);


#endif

