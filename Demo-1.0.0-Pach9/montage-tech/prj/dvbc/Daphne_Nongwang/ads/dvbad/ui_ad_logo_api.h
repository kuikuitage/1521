#ifndef __UI_AD_LOG_API_H__
#define __UI_AD_LOG_API_H__

#define AD_SCREEN_WIDTH  720
#define AD_SCREEN_HEIGHT 576

void pic_set_init_ret(BOOL ret);

void ads_ap_init(void);

BOOL ui_set_pic_show (BOOL show);

BOOL ui_pic_play(ads_ad_type_t type, u8 root_id);

BOOL ui_pic_float();

void ui_pic_stop(void);

void ui_adv_set_category_index(u8 index);

BOOL ui_check_pic_by_type(ads_ad_type_t type);

BOOL ui_is_pic_showing (void);

RET_CODE ui_adv_set_pic(u32 size, u8 *pic_add);

RET_CODE ui_adv_set_float(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data);

void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h);

void ui_ads_set_show_logo(BOOL is_show);

void set_dec_mode(u32 mode);

#endif

