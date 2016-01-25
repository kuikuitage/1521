/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/

#include "ui_common.h"
#include "ui_prog_bar.h"
#include "ui_mute.h"
#include "ui_email_picture.h"


#define EN_TITLE_HELP
#define SN_SCREEN_WIDTH		640
#define SN_SCREEN_HEIGHT	480

/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
enum comm_title_idc
{
  IDC_COMM_TITLE_ICON = 1,
  IDC_COMM_TITLE_TXT,
};

enum comm_timedate_idc
{
  IDC_COMM_TIMEDATE_TXT = 1,
};

void ui_comm_timedate_create(control_t *parent, u16* p_unistr, u8 rstyle)
{
  control_t *p_cont, *p_txt;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_TIMEDATE_CONT,
                            TIMEDATE_X, TIMEDATE_Y, TIMEDATE_W, TIMEDATE_H,
                            parent, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);

  p_txt = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_TIMEDATE_TXT,
                           TIMEDATE_TXT_X, TIMEDATE_TXT_Y, TIMEDATE_TXT_W, TIMEDATE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_RIGHT | STL_VCENTER);
  text_set_font_style(p_txt, FSI_TIMEDATE_TXT, FSI_TIMEDATE_TXT, FSI_TIMEDATE_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  text_set_content_by_unistr(p_txt, (u16*)p_unistr);

}

void ui_comm_timedate_set_content_unistr(control_t *parent, u16* p_unistr)
{
  control_t *p_cont, *p_txt;
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TIMEDATE_CONT)) == NULL)
  {
    return;
  }

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TIMEDATE_TXT);
  text_set_content_by_unistr(p_txt, p_unistr);
}

#define ___TITLE__
enum bg_title_idc
{
	IDC_TITLE_ICON = 1,
	IDC_TITLE_TXT,
};
#define BG_TITLE_W 			640
#define BG_TITLE_H			42
#define BG_TITLE_OFFSET_X 	0
#define BG_TITLE_OFFSET_Y	2

#define BG_TITLE_ICON_W 	48
void ui_background_title_create(control_t *parent, u16 icon_id, u16 strid)
{
	control_t *p_cont;
	control_t *p_ctrl = NULL;
	u16 w = 0,text_start_x = 0;

	MT_ASSERT(parent != NULL);

	w = (u16)ctrl_get_width(parent);
	w= w > BG_TITLE_W?BG_TITLE_W:w;

	p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_TITLE_CONT,
	                        BG_TITLE_OFFSET_X,
	                        BG_TITLE_OFFSET_Y, 
	                        w, 
	                        BG_TITLE_H,
	                        parent, 0);
	ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

	if(icon_id != RSC_INVALID_ID)
	{
		p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_TITLE_ICON,
		                         0, 
		                         0, 
		                         BG_TITLE_ICON_W, 
		                         BG_TITLE_H,
		                         p_cont, 0);
		bmap_set_content_by_id(p_ctrl, icon_id);
		bmap_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
		text_start_x += BG_TITLE_ICON_W;
	}
	p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE_TXT,
	                       text_start_x,
	                       0,
	                       w - BG_TITLE_OFFSET_X,
	                       BG_TITLE_H,
	                       p_cont, 0);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_TOP_TITLE_TXT, FSI_TOP_TITLE_TXT, FSI_TOP_TITLE_TXT);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, strid);

}

void ui_comm_timedate_update(control_t *parent, BOOL is_draw)
{
  control_t *p_cont, *p_txt;
  rect_t rc;
  u8  weekday;
  u16 dst_unistr[128];
  u16 tmp_unistr[128];
  u8  ansstr[256];
  u16 weekstr[] = 
    {
      IDS_MONDAY,
      IDS_TUESDAY,
      IDS_WEDNESDAY,
      IDS_THURSDAY,
      IDS_FRIDAY,
      IDS_SATDAY,
      IDS_SUNDAY,
    };
  utc_time_t time;
  
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TIMEDATE_CONT)) == NULL)
  {
    return;
  }

  time_get(&time, FALSE);
  sprintf((char *)ansstr, "%d.%02d.%02d ",time.year, time.month, time.day);
  str_asc2uni(ansstr, dst_unistr);

  weekday = date_to_weekday(&time);
  gui_get_string(weekstr[weekday], tmp_unistr, 64);
  
  uni_strcat(dst_unistr, tmp_unistr, uni_strlen(dst_unistr) + uni_strlen(tmp_unistr));

  sprintf((char *)ansstr, " %02d:%02d", time.hour, time.minute);
  str_asc2uni(ansstr, tmp_unistr);  
  uni_strcat(dst_unistr, tmp_unistr, uni_strlen(dst_unistr) + uni_strlen(tmp_unistr));
  
  ui_comm_timedate_set_content_unistr(parent, (u16*)&dst_unistr);

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TIMEDATE_TXT);
  ctrl_get_frame(p_txt, &rc);
  ctrl_add_rect_to_invrgn(p_cont, &rc);
  if(is_draw)
  {
    ctrl_paint_ctrl(p_cont, is_draw);
  }
}

void ui_comm_title_create(control_t *parent, u16 icon_id, u16 strid, u8 rstyle)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_first;
  control_t *p_ctrl = NULL;
  u16 w = 0;
  
  MT_ASSERT(parent != NULL);

  w = (u16)ctrl_get_width(parent);

  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_TITLE_CONT,
                            TITLE_CONT_X, TITLE_CONT_Y, w, TITLE_CONT_H,
                            NULL, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);

  if(icon_id != RSC_INVALID_ID)
  {
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_COMM_TITLE_ICON,
                             TITLE_ICON_X, TITLE_ICON_Y, TITLE_ICON_W, TITLE_ICON_H,
                             p_cont, 0);
    bmap_set_content_by_id(p_ctrl, icon_id);
    bmap_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
  }
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_COMM_TITLE_TXT,
                           TITLE_TXT_X, TITLE_TXT_Y, TITLE_TXT_W, TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_TOP_TITLE_TXT, FSI_TOP_TITLE_TXT, FSI_TOP_TITLE_TXT);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, strid);

  // insert to parent
  p_first = parent->p_child;

  if(p_first != NULL)
  {
    p_first->p_prev = p_cont;
    p_cont->p_next = p_first;
  }
  parent->p_child = p_cont;
  p_cont->p_parent = parent;
#endif
}


void ui_comm_title_set_content(control_t *parent, u16 strid)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt;
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TITLE_CONT)) == NULL)
  {
    return;
  }

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_TXT);
  text_set_content_by_strid(p_txt, strid);
#endif
}


void ui_comm_title_update(control_t *parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt;
  rect_t rc;
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TITLE_CONT)) == NULL)
  {
    return;
  }

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_TXT);
  ctrl_get_frame(p_txt, &rc);
  ctrl_add_rect_to_invrgn(p_cont, &rc);
  ctrl_paint_ctrl(p_cont, FALSE);
#endif
}


/***********************************************************************
 *   COMMON  HELP BAR
 ***********************************************************************/
enum comm_help_idc
{
  IDC_HELP_ICON = 1,
  IDC_COMM_HELP_MBOX,
};

void sn_ui_comm_help_create(sn_comm_help_data_t *p_data, control_t *p_parent)
{
  
  #define SN_HELP_H		43
  control_t *p_cont, *p_txt,*p_bmp;
  u16 x = 0,w, h,y = 0;
  u8 i, ctrl_id=1;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);
  DEBUG_ENABLE_MODE(HELP,INFO);
  DEBUG(HELP,INFO,"sn_ui_comm_help_create now\n");
  
  if((p_cont = ctrl_get_child_by_id(p_parent, SN_IDC_COMM_HELP_CONT)) != NULL)
  {
    ctrl_destroy_child_by_id(p_parent, SN_IDC_COMM_HELP_CONT);
  }
  
  y = p_data->y;
  w = ctrl_get_width(p_parent);
  h = SN_HELP_H;

  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, SN_IDC_COMM_HELP_CONT,
                            0,y,
                            w, h,
                            p_parent, 0);
  if(NULL == p_cont)
  {
  	DEBUG(HELP,ERR,"sn_ui_comm_help_create open failure\n");
    return;
  }
  ctrl_set_rstyle(p_cont, RSI_HELP_BG, RSI_HELP_BG, RSI_HELP_BG);
  
  x = p_data->offset;
  for(i = 0; i < p_data->item_cnt; i++)
  {
    w = p_data->rsc->rsc_width;
    switch(p_data->rsc->type)
    {
      case SN_IDC_HELP_BMAP:
        p_bmp = ctrl_create_ctrl((u8*)CTRL_BMAP, ctrl_id, x, 0, w, h, p_cont, 0);
		bmap_set_align_type(p_bmp, STL_CENTER | STL_VCENTER);
        bmap_set_content_by_id(p_bmp, (u16)(p_data->rsc->rsc_id));
        break;

      case SN_IDC_HELP_TEXT:
        p_txt = ctrl_create_ctrl((u8*)CTRL_TEXT, ctrl_id, x, 0, w, h, p_cont, 0);
        text_set_font_style(p_txt, FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
        text_set_align_type(p_txt, STL_LEFT | STL_VCENTER);
        text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_txt, (u16)(p_data->rsc->rsc_id));
        break;
    }
	p_data->rsc++;
    ctrl_id++;
    x += w;
  }
}
/*********************************************************************
 * COMMON ROOT
 *********************************************************************/
//static BOOL g_is_locked = FALSE;

u16 comm_mask_keymap(u16 key);

RET_CODE comm_mask_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


// ON COMM ROOT
static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  ui_pic_stop();
  ui_close_all_mennus();
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) == NULL && fw_find_root_by_id(ROOT_ID_EPG) == NULL) 
 {
  	open_prog_bar(0,0);
 }
 if(p_mute != NULL)
 {
   	open_mute(0,0);
 }
 if(ui_is_email())
 {
  	update_email_picture(EMAIL_PIC_SHOW);
 }
  return SUCCESS;
}

static RET_CODE on_exit_to_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_pic_stop();
  ui_return_to_mainmenu();

  return SUCCESS;
}

// ON COMM ROOT
static RET_CODE on_time_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_timedate_update(p_ctrl, TRUE);

  return SUCCESS;
}

static RET_CODE on_exit_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 root_id;

  MT_ASSERT(p_ctrl != NULL);

  root_id = ctrl_get_ctrl_id(p_ctrl);
  if(root_id == BACKGROUND_IDC_CLIENT_CONT)
  {
    root_id = ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl));
  }
  if(root_id != ROOT_ID_NUM_PLAY)
    ui_pic_stop();
  manage_close_menu(root_id, 0, 0);

  return SUCCESS;
}


// ON COMM MASK
static RET_CODE on_save(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *cont;

  cont = ctrl_get_child_by_id(p_ctrl, BACKGROUND_IDC_CLIENT_CONT);
  if(cont != NULL)
  {
    return ctrl_process_msg(cont, msg, para1, para2);
  }

  return SUCCESS;
}


static RET_CODE on_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *cont;

  cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_SIGN_CONT);
  if(cont != NULL)
  {
    ui_enable_signal_monitor(FALSE);
    fw_tmr_destroy(ctrl_get_ctrl_id(p_ctrl), MSG_BEEPER);
  }

  // bottom half process by default msgmap
  return ERR_NOFEATURE;
}


static RET_CODE on_signal_update(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *cont;

  cont = ctrl_get_child_by_id(p_ctrl, BACKGROUND_IDC_CLIENT_CONT);
  cont->p_proc(cont, msg, para1, para2);
#if 0
  cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_SIGN_CONT);

  if(cont != NULL)
  {
    struct signal_data *data = (struct signal_data *)(para1);
    ui_comm_root_update_signbar(p_ctrl, data->intensity,
                                data->quality, data->lock);
    if(data->lock != g_is_locked)
    {
      g_is_locked = data->lock;
      fw_tmr_reset(ctrl_get_ctrl_id(p_ctrl), MSG_BEEPER,
                   g_is_locked ? BEEPER_LOCKD_TMOUT :
                   BEEPER_UNLKD_TMOUT);
    }
  }
#endif
  return SUCCESS;
}


static RET_CODE on_beeper(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL beeper;

  sys_status_get_status(BS_BEEPER, &beeper);
  if(beeper)
  {
    /*dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    OS_PRINTF("Beeper on, %d\n", mtos_ticks_get());
    cfg.is_pcm = FALSE;
    cfg.p_data = (u8 *)dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                         LOGO_BLOCK_ID_M2);
    cfg.size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),
                                         LOGO_BLOCK_ID_M2);*/
    //todo:AUD_VSB
    //dev_io_ctrl(dev, AUDIO_CMD_PLAY_MEDIA, (u32) & cfg);
  }
  return SUCCESS;
}


control_t *ui_comm_root_create(u8 root_id, u8 rsi_root,
                               u16 x, u16 y, u16 w, u16 h,
                               u16 title_strid, u8 rsi_title)
{
#if 0
rsc

  control_t *p_mask, *p_cont;

  p_mask = fw_create_mainwin(root_id,
                             16, 38, SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);
  ctrl_set_rstyle(p_mask, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_proc(p_mask, comm_mask_proc);

  // create container
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_ROOT_CONT,
                            x, y, w, h, p_mask, 0);
  ctrl_set_rstyle(p_cont, rsi_root, rsi_root, rsi_root);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create title
  ui_comm_title_create(p_cont, IM_COMMON_BANNER_SETTLE, title_strid, rsi_title);

  return p_cont;
#endif 
return 0;
}

control_t *ui_comm_prev_root_create(u8 root_id,  u8 rsi_root,
                              u16 x, u16 y, u16 w, u16 h,
                              full_screen_title_t *p_title_data)
{
#if 0
rsc

  control_t *p_mask, *p_cont;
  control_t *p_bg, *p_tv_win, *p_ad_win, *p_second_title;
  u16 bg_x,bg_y,bg_w,bg_h;

  bg_x = x;
  bg_y = y + TITLE_CONT_Y + TITLE_CONT_H;
  bg_w = w;
  bg_h = h - TITLE_CONT_Y - TITLE_CONT_H - HELP_ITEM_H;

  p_mask = fw_create_mainwin(root_id,
                             16, 38, SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);
  ctrl_set_rstyle(p_mask, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_proc(p_mask, comm_mask_proc);

  // create container
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_ROOT_CONT,
                            x, y, w, h, p_mask, 0);
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);
  
  //background
  p_bg = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_COMM_RIGHT_BG,
                              bg_x, bg_y,
                              bg_w, bg_h,
                              p_cont, 0);
  ctrl_set_rstyle(p_bg, rsi_root, rsi_root, rsi_root);

  //TV preview window
  p_tv_win = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_COMM_TV,
                              SUB_PREV_X, SUB_PREV_Y,
                              SUB_PREV_W, SUB_PREV_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_PREVIEW_TV_WIN, RSI_PREVIEW_TV_WIN, RSI_PREVIEW_TV_WIN);

  //advertisement window
  p_ad_win = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_COMM_AD,
                              SUB_AD_X, SUB_AD_Y,
                              SUB_AD_W, SUB_AD_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ad_win, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN, RSI_ADVERTISEMENT_WIN);
  bmp_pic_draw(p_ad_win);

  if(p_title_data == NULL)
  {
      return p_cont;
  }
  //second title
  p_second_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_COMM_SECOND_TITLE,
                              SECOND_TITLE_X, SECOND_TITLE_Y,
                              SECOND_TITLE_W, SECOND_TITLE_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_second_title, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
  text_set_font_style(p_second_title, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_second_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_second_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_second_title, p_title_data->second_title_strid);


  // create title
  ui_comm_title_create(p_cont, p_title_data->bmp_id, p_title_data->str_id, p_title_data->bg_rsi);
  return p_cont;
#endif 
return 0;
}

enum comm_signbar_idc
{
  IDC_COMM_SIGN_INTENSITY = 1,
  IDC_COMM_SIGN_QUALITY,
  IDC_COMM_SIGN_LOCK,
};

control_t *ui_comm_root_create_with_signbar(u8 root_id,
                                            u8 rsi_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_strid,
                                            u8 rsi_title,
                                            BOOL is_beeper)
{
#if 0
rsc

  control_t *p_mask, *p_cont, *p_frame, *p_ctrl;
  u16 i, stxt_y, bar_y, percent_y;
  u16 stxt_ids[] =
  {IDS_STRENGTH, IDS_STRENGTH, 0};

  p_cont = ui_comm_root_create(root_id, rsi_root,
                               x, y, w, h, title_strid, rsi_title);
  p_mask = ctrl_get_parent(p_cont);

  p_frame = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_COMM_SIGN_CONT,
                             x, y + (s16)(h + ROOT_SIGN_CONT_GAP),
                             (s16)w, ROOT_SIGN_CONT_H,
                             p_mask, 0);
  ctrl_set_rstyle(p_frame, RSI_ROOT_SIGN_CONT,
                  RSI_ROOT_SIGN_CONT, RSI_ROOT_SIGN_CONT);

  stxt_y = ROOT_SIGN_BAR_STXT_Y;
  bar_y = ROOT_SIGN_BAR_Y;
  percent_y = ROOT_SIGN_BAR_PERCENT_Y;
  for(i = 0; i < 2; i++)
  {
    p_ctrl = ui_comm_bar_create(p_frame, (u8)(IDC_COMM_SIGN_INTENSITY + i),
                                ROOT_SIGN_BAR_X, bar_y,
                                ROOT_SIGN_BAR_W,
                                ROOT_SIGN_BAR_H,
                                ROOT_SIGN_BAR_STXT_X, stxt_y,
                                ROOT_SIGN_BAR_STXT_W,
                                ROOT_SIGN_BAR_STXT_H,
                                ROOT_SIGN_BAR_PERCENT_X, percent_y,
                                ROOT_SIGN_BAR_PERCENT_W,
                                ROOT_SIGN_BAR_PERCENT_H);
    ui_comm_bar_set_style(p_ctrl,
                          RSI_ROOT_SIGN_BAR,
                          RSI_ROOT_SIGN_BAR_MID_1,
                          RSI_ROOT_SIGN_BAR_STXT,
                          FSI_ROOT_SIGN_BAR_STXT,
                          RSI_ROOT_SIGN_BAR_PERCENT,
                          FSI_ROOT_SIGN_BAR_PERCENT);
    ui_comm_bar_set_param(p_ctrl, stxt_ids[i], 0, 100, 100);
    ui_comm_bar_update(p_ctrl, 0, TRUE);

    stxt_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
    bar_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
    percent_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
  }

  /* TODO */
  ui_enable_signal_monitor(TRUE);
  if (is_beeper)
  {
    fw_tmr_create(root_id, MSG_BEEPER, BEEPER_UNLKD_TMOUT, TRUE);
  }
  g_is_locked = FALSE;

  return p_cont;
#endif 
return NULL;
}


static void update_comm_signbar(control_t *p_ctrl, u8 value, BOOL is_lock)
{
  control_t *p_bar;

  if(ui_comm_bar_update(p_ctrl, value, TRUE))
  {
    p_bar = ctrl_get_child_by_id(p_ctrl, IDC_COMM_BAR);

    pbar_set_rstyle(
      p_bar,
      is_lock ? RSI_SN_BAR_YELLOW_HL :
      RSI_SN_BAR_YELLOW_HL,
      RSI_IGNORE,
      INVALID_RSTYLE_IDX);

    ui_comm_bar_paint(p_ctrl, TRUE);
  }
}


void ui_comm_root_update_signbar(control_t *root,
                                 u8 intensity,
                                 u8 quality,
                                 BOOL is_lock)
{
  control_t *p_cont, *p_ctrl;

  p_cont = ctrl_get_child_by_id(root, IDC_COMM_SIGN_CONT);
  if(p_cont == NULL)
  {
    UI_PRINTF("ROOT: p_cont == NULL \n");
    return;
  }

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_COMM_SIGN_INTENSITY);
  update_comm_signbar(p_ctrl, intensity, is_lock);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_COMM_SIGN_QUALITY);
  update_comm_signbar(p_ctrl, quality, is_lock);

  // set front panel by quality
  ui_set_front_panel_by_num(quality);
}

static RET_CODE on_comm_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  control_t * p_comm_root = NULL;
  
  sys_status_get_lang_set(&lang_set);
  if(lang_set.osd_text == 0)
  {
    rsc_set_curn_language(gui_get_rsc_handle(),2);
    lang_set.osd_text = 1;
  }
  else
  {
    rsc_set_curn_language(gui_get_rsc_handle(),1);
    lang_set.osd_text = 0;
  }
  sys_status_set_lang_set(&lang_set);
  sys_status_save();
  p_ctrl = ctrl_get_root(p_ctrl);
  p_comm_root = ctrl_get_child_by_id(p_ctrl,IDC_COMM_ROOT_CONT);
  if(NULL == p_comm_root)
  {
    p_comm_root = p_ctrl; //no comm root
  }
  ui_comm_timedate_update(p_comm_root, FALSE);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}


#define BG_CLIENT_OFFSET_X      		0  
#define BG_CLIENT_OFFSET_Y        	58//54
#define BG_CLIENT_OFFSET_B      		10



control_t *ui_background_create(u8 root_id,
                               u16 x, u16 y, u16 w, u16 h,
                               u16 title_strid,u32 flags)
{

    //create background title
    #define SN_BG_TITLE_X      		0
    #define SN_BG_TITLE_Y      		0
    #define SN_BG_TITLE_W      		640
    #define SN_BG_TITLE_H      		57

	control_t *p_mask, *p_cont, *p_bg, *p_bg_title;

	p_mask = fw_create_mainwin(root_id,
	                         16, 38, w, h,
	                         0, 0, OBJ_ATTR_ACTIVE, 0);
	MT_ASSERT(p_mask != NULL);
	ctrl_set_rstyle(p_mask, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	ctrl_set_proc(p_mask, comm_mask_proc);

	// create background
	p_bg = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_BG_CONT,
	                        0, 0, w, h, p_mask, 0);
	ctrl_set_rstyle(p_bg, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

	//create background title    
	p_bg_title = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_BG_TITLE,
	                        SN_BG_TITLE_X, SN_BG_TITLE_Y, 
	                        SN_BG_TITLE_W, SN_BG_TITLE_H, p_bg, 0);
	ctrl_set_rstyle(p_bg_title, RSI_SN_BG_TITLE, RSI_SN_BG_TITLE, RSI_SN_BG_TITLE);

	// create container
	p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, BACKGROUND_IDC_CLIENT_CONT,
	                        BG_CLIENT_OFFSET_X, 
	                        BG_CLIENT_OFFSET_Y, 
	                        w,h - (BG_CLIENT_OFFSET_Y + BG_CLIENT_OFFSET_B), 
	                        p_mask, 0);
	ctrl_set_keymap(p_cont, ui_comm_root_keymap);
	ctrl_set_proc(p_cont, ui_comm_root_proc);

	// create title
	ui_background_title_create(p_bg, 0, title_strid);
	if (flags & BACKGROUND_FLAGS_HAVE_TIME) 
	{
		//ui_comm_timedate_create(p_bg, 0, RSI_IGNORE);
		//ui_comm_timedate_update(p_bg, 0);
	}
	return p_cont;
}

control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id)
{
  control_t *root = fw_find_root_by_id(root_id);

  if(root == NULL)
  {
    return NULL;
  }
  if(ctrl_get_ctrl_id(root->p_child) == IDC_COMM_ROOT_CONT)
  {
    root = root->p_child;
  }
  return ctrl_get_child_by_id(root, ctrl_id);
}


/*********************************************************************
 * COMMON MENU
 *********************************************************************/
enum comm_menu_idc
{
  IDC_COMM_MENU_MBOX = 1,
};

BEGIN_KEYMAP(ui_comm_root_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
END_KEYMAP(ui_comm_root_keymap, NULL)

BEGIN_MSGPROC(comm_mask_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SAVE, on_save)
  ON_COMMAND(MSG_DESTROY, on_destory)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_update)
  ON_COMMAND(MSG_BEEPER, on_beeper)
END_MSGPROC(comm_mask_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ui_comm_root_proc, cont_class_proc)
  ON_COMMAND(MSG_TO_MAINMENU, on_exit_to_menu)
  ON_COMMAND(MSG_EXIT, on_exit_menu)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_SET_LANG, on_comm_switch_lang)
END_MSGPROC(ui_comm_root_proc, cont_class_proc)

