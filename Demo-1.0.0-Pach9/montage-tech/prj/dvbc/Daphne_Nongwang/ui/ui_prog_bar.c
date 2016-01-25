/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_prog_bar.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_mute.h"

#ifdef FAST_SWITCH_PROGRAN_ON_UP_DOWN
#define FAST_SWITCH_PROGRAN_TIMEOUT 300
#endif

//   title bg   
#define  PROG_BITLE_BG_X 10
#define  PROG_BITLE_BG_Y 10
#define  PROG_BITLE_BG_W 173//205
#define  PROG_BITLE_BG_H 22

//   info cont
#define SN_PROG_INFO_INFO_CONT_X   0
#define SN_PROG_INFO_INFO_CONT_Y   0//173
#define SN_PROG_INFO_INFO_CONT_W   400//407
#define SN_PROG_INFO_INFO_CONT_H   SUB_AD_H

//   boot bg
#define SN_PROG_INFO_BOOT_BG_X   10
#define SN_PROG_INFO_BOOT_BG_Y   ((PROG_BITLE_BG_Y + PROG_BITLE_BG_H) + 5)//29
#define SN_PROG_INFO_BOOT_BG_W   (SN_PROG_INFO_INFO_CONT_W -20)
#define SN_PROG_INFO_BOOT_BG_H   64

//	prog number       
#define SN_PROG_BAR_PG_NUM_X  (PROG_BITLE_BG_X + 2)
#define SN_PROG_BAR_PG_NUN_Y  9

//   prog name
#define SN_PROG_BAR_PG_NAME_X (SN_PROG_BAR_PG_NUM_X + PROG_BAR_PG_NUN_W)
#define SN_PROG_BAR_PG_NAME_Y (SN_PROG_BAR_PG_NUN_Y - 1)

//   advertisement
#define SN_PROG_BAR_AD_X      (SN_PROG_INFO_INFO_CONT_X + SN_PROG_INFO_INFO_CONT_W + 17)
#define SN_PROG_BAR_AD_Y      SN_PROG_INFO_INFO_CONT_Y

//   date & time
#define SN_PROG_BAR_DATATIME_X   (PROG_BITLE_BG_X+PROG_BITLE_BG_W + 33)
#define SN_PROG_BAR_DATATIME_Y   PROG_BITLE_BG_Y
#define SN_PROG_BAR_DATATIME_W   175
#define SN_PROG_BAR_DATATIME_H   (PROG_BITLE_BG_H + 5) 

//   epg pf 
#define SN_PROG_BAR_PF_INFO_X  (PROG_BAR_PF_INFO_X + 5)

//test

enum progbar_local_msg
{
  MSG_INTRODUCE_P = MSG_USER_BEGIN + 500,
  MSG_INTRODUCE_F,
  MSG_SWITCH_AUDIO,
  MSG_TVRADIO_TV,
  MSG_TVRADIO_RADIO,
};

enum control_id
{
  IDC_NICON = 1,
  IDC_INTRO_CONT,
  IDC_FRAME,
  IDC_AD_WIN,
};

enum introduce_control_id
{
  IDC_TITLE = 1,
  IDC_DETAIL,
  IDC_DETAIL_TEST,
  IDC_BAR_TEST_VERSION,
};

enum icon_sub_control_id
{
  IDC_ICON_NUMBER = 1,
};

enum frame_sub_control_id
{
  IDC_TITLE_BG = 1,
  IDC_NAME ,
  IDC_TIME,
  IDC_EPG_P,
  IDC_EPG_F,
  IDC_ICON_AUDIO,
  IDC_ICON_MONEY,
  IDC_ICON_FAV,
  IDC_ICON_LOCK,
  IDC_ICON_SKIP,
  IDC_INFO_SUBTT,
  IDC_INFO_TELTEXT,
  IDC_INFO_EPG,
  IDC_CA_INFO,
  IDC_TP_INFO,
  IDC_SIGNAL_QUALITY,
  IDC_SIGNAL_INTENSITY,
  IDC_ICON_NUMBER1,
  SN_IDC_TIME
};

enum local_msg
{
  MSG_RECALL = MSG_LOCAL_BEGIN + 475,
};

static BOOL g_is_curn_pg_intro = FALSE;
#ifndef WIN32
extern BOOL is_test_version;
#endif

#if 0
rsc

static comm_help_data_t prog_bar_help_data = //help bar data
{
  3,
  3,
  {
    IDS_EXIT,
    IDS_CUR_PROG_INTRO,
    IDS_NEXT_PROG_INTRO,
  },
  {
    IM_ICON_EXIT,
    IM_F1,
    IM_F2,
  },
};
#endif
static comm_prompt_data_t warn_data_t =
				  {
					  0,
					  STR_MODE_STATIC,
					  COMM_DLG_X,
					  COMM_DLG_Y,
					  COMM_DLG_W,
					  COMM_DLG_H,
					  50,
					  IDS_NO_NVOD,0,
					  0,DIALOG_DEADLY,
					  1500
				  };

u16 prog_bar_cont_keymap(u16 key);
RET_CODE prog_bar_cont_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);

static void progbar_get_time_area(utc_time_t *p_start, utc_time_t *p_end)
{
  utc_time_t g_time = {0};
  //utc_time_t t_time = {0};

  time_get(&g_time, TRUE);

  memcpy(p_start, &g_time, sizeof(utc_time_t));
  memcpy(p_end, &g_time, sizeof(utc_time_t));
  p_end->hour = 23;
  p_end->minute = 59;
  p_end->second = 59;
}

void fill_prog_info(control_t *cont, BOOL is_redraw, u16 prog_id)
{
  control_t *p_frm, *p_subctrl;
  dvbs_prog_node_t pg;
  u8 asc_str[30] = {0};
  epg_filter_t filter;
  u32 evt_cnt = 0;
  u16 uni_char[64];
  evt_node_t *p_evt_node = NULL;
  u8 asc_buf[8];
  u16 uni_buf[40];
  BOOL is_epg_get = FALSE;
  u8 view_id;
  u16 view_pos;
  utc_time_t s_time = {0};
  utc_time_t e_time = {0};
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  u16 progname[18]={0};  //   LXD DEFINE

  view_id = ui_dbase_get_pg_view_id();
  view_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  if(NULL == cont)
  {
    return;
  }
  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  if(NULL == p_frm)
  {
    return;
  }
  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 64);
    #endif

    p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_P);
    text_set_content_by_unistr(p_subctrl, uni_char);

    p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_F);
    text_set_content_by_unistr(p_subctrl, uni_char);

    if(is_redraw)
    {
      ctrl_paint_ctrl(cont, TRUE);
    }
    return;
  }

  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_ICON_NUMBER1);
  #ifdef LCN_SWITCH
    if(pg.is_scrambled)
      sprintf((char*)asc_buf, "%.4d$", pg.logic_ch_num);
    else
      sprintf((char*)asc_buf, "%.4d ", pg.logic_ch_num);
  #else
    if(pg.is_scrambled)
      sprintf((char *)asc_buf, "%.3d$", view_pos + 1);
    else
      sprintf((char *)asc_buf, "%.3d ", view_pos + 1);
  #endif
  text_set_content_by_ascstr(p_subctrl, asc_buf);

  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_NAME);

  // LXD change(zhang)
  
  //memset(a,0,sizeof(a)); 
  if (pg.name[0] <= 0xff)
  {
	 memcpy(progname,pg.name,16);
	 progname[16]='\0';
  }

  else 
  {
	 memcpy(progname,pg.name,12);
	 progname[12] = '\0';
  }
  OS_PRINTF("progname = %s\n",(char*)progname);
  OS_PRINTF("program len = %d\n",strlen((char*)progname));

  text_set_content_by_unistr(p_subctrl,progname); // LXD ADD 
  progbar_get_time_area(&s_time, &e_time);
  memset(&filter, 0, sizeof(epg_filter_t));
  if(pg.orig_net_id == 0)                                        //for asia 3
  {
    filter.stream_id = 0xffff;
    filter.orig_network_id = 0xffff;
  }
  else
  {
    filter.stream_id = (u16)pg.ts_id;
    filter.orig_network_id = (u16)pg.orig_net_id;
  }
  filter.service_id = (u16)pg.s_id;
  filter.cont_level = 0;

  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &evt_cnt);

  if (p_evt_node != NULL)
  {
    time_to_local(&(p_evt_node->start_time), &start_time);
    memcpy(&end_time, &start_time, sizeof(utc_time_t));
    time_add(&end_time, &(p_evt_node->drt_time));

    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour, start_time.minute,
                  end_time.hour, end_time.minute);
    str_asc2uni(asc_str,uni_char);
    uni_strncpy(uni_buf, p_evt_node->event_name, 40);
    uni_strcat(uni_char, uni_buf,(uni_strlen(uni_char)+uni_strlen(uni_buf)));
    is_epg_get = TRUE;
  }
  else
  {
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 64);
    #endif
   
    is_epg_get = FALSE;
  }

  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_P);
  text_set_content_by_unistr(p_subctrl, uni_char);

  if (is_epg_get == TRUE && p_evt_node->p_next_evt_node != NULL)
  {
    p_evt_node = p_evt_node->p_next_evt_node;
    time_to_local(&(p_evt_node->start_time), &start_time);
    memcpy(&end_time, &start_time, sizeof(utc_time_t));
    time_add(&end_time, &(p_evt_node->drt_time));

    memset(uni_buf, 0, sizeof(u16)*32); 
    sprintf((char *)asc_str, " %.2d:%.2d~%.2d:%.2d   ", start_time.hour,
              start_time.minute,end_time.hour, end_time.minute);
    str_asc2uni(asc_str,uni_char);
    uni_strncpy(uni_buf, p_evt_node->event_name, 40);
    uni_strcat(uni_char, uni_buf,(uni_strlen(uni_char)+uni_strlen(uni_buf)));

    is_epg_get = TRUE;
  }
  else
  {
    #ifdef changjiang_js
      uni_char[0] = '\0';
    #else
      gui_get_string(IDS_NO_PROG_EVENT_LIST, uni_char, 64);
    #endif
    is_epg_get = FALSE;
  }
  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_F);
  text_set_content_by_unistr(p_subctrl, uni_char);
  if(is_redraw)
  {
    ctrl_paint_ctrl(cont, TRUE);
  }
}


void fill_time_info(control_t *cont, BOOL is_redraw)
{
  utc_time_t time;
  control_t *p_frm = NULL, *p_ctrl = NULL;
  u8 time_str[32];
  if(ui_get_disable_display_time_status() == TRUE)
  {
    return;
  }

  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  MT_ASSERT(p_frm != NULL);
  time_get(&time, FALSE);
  sprintf((char *)time_str, "%.2d/%.2d/%.2d  %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
  p_ctrl = ctrl_get_child_by_id(p_frm,SN_IDC_TIME ); // LXD CHANGE "IDC_TIME" TO "SN_IDC_TIME" 
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

static RET_CODE on_pbar_show_introduce(control_t *p_cont, u16 msg, u32 para1,u32 para2);

RET_CODE open_prog_bar(u32 para1, u32 para2)
{
  control_t *p_cont, *p_intro_cont;
  control_t *p_ctrl, *p_subctrl, *p_ctrl_test_version;
  u16 i, y;
  u8 org_mode = 0;
  u16 ret = RSC_INVALID_ID;
  customer_cfg_t cfg = {0};
  u32 title_bar_size = 0;
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
#if 0
rsc
  get_customer_config(&cfg);
  if(CUSTOMER_XINSHIDA == cfg.customer || CUSTOMER_XINSIDA_LQ == cfg.customer ||
    CUSTOMER_CHANGJIANG == cfg.customer ||CUSTOMER_CHANGJIANG_LQ == cfg.customer ||CUSTOMER_CHANGJIANG_JS == cfg.customer || 
    CUSTOMER_CHANGJIANG_NY== cfg.customer ||CUSTOMER_CHANGJIANG_QY== cfg.customer ||
    CUSTOMER_XINSIDA_SPAISH== cfg.customer ||CUSTOMER_HEBI == cfg.customer)
  {
    prog_bar_help_data.bmp_id[2] = IM_F2_2;
  }
#endif
  
  org_mode = sys_status_get_curn_prog_mode();

  switch (para1)
  {
    case V_KEY_UP:
      ui_shift_prog(1, FALSE, &prog_id);
      break;
    case V_KEY_DOWN:
      ui_shift_prog(-1, FALSE, &prog_id);
      break;
    case V_KEY_CH_UP:
      ui_shift_prog(1, FALSE, &prog_id);
      break;
    case V_KEY_CH_DOWN:
      ui_shift_prog(-1, FALSE, &prog_id);
      break;
    case V_KEY_PAGE_UP:
      ui_shift_prog(2, FALSE, &prog_id);
      break;
    case V_KEY_PAGE_DOWN:
      ui_shift_prog(-2, FALSE, &prog_id);
      break;
    case V_KEY_BACK:
    case V_KEY_RECALL:
      ui_recall(FALSE, &prog_id);
      break;
    case V_KEY_TVRADIO:
      ret = ui_tvradio_switch(FALSE, &prog_id);
      if(RSC_INVALID_ID != ret)
      {
		warn_data_t.text_strID = ret;
		ui_comm_prompt_open(&warn_data_t);
		if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
		  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		update_signal();
		if(p_mute != NULL)
  	  	{
    		open_mute(0,0);
 	  	}
        return ERR_FAILURE;
      }
      break;
    case V_KEY_TV:
      if(CURN_MODE_TV == org_mode)
      {
        return SUCCESS;
      }
      else
      {
        ret = ui_tvradio_switch(FALSE, &prog_id);
        if(RSC_INVALID_ID != ret)
        {
			warn_data_t.text_strID = ret;
			
			ui_comm_prompt_open(&warn_data_t);
			if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
			  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
			update_signal();
			if(p_mute != NULL)
  	  		{
    			open_mute(0,0);
 	 	 	}
          return ERR_FAILURE;
        }
      }
      break;
    case V_KEY_RADIO:
      if(CURN_MODE_RADIO == org_mode)
      {
        return SUCCESS;
      }
      else
      {
        ret = ui_tvradio_switch(FALSE, &prog_id);
        if(RSC_INVALID_ID != ret)
        {
			warn_data_t.text_strID = ret;
			
			ui_comm_prompt_open(&warn_data_t);
			if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
			  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
			update_signal();
			if(p_mute != NULL)
  	  		{
    			open_mute(0,0);
 	  		}
          return ERR_FAILURE;
        }
      }
      break;
    case V_KEY_9:   //means number key change channel
      prog_id = (u16)para2;
      break;
    default:
      /* do nothing */;
  }

  // check, if no prog
  if(prog_id == INVALIDID)
  {
    UI_PRINTF("PROGBAR: ERROR, no prog in view!\n");
    return ERR_FAILURE;
  }
  // reset
  is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  
  /* create */
  p_cont = fw_create_mainwin(ROOT_ID_PROG_BAR,
                           PROG_BAR_MENU_X,
                           PROG_BAR_MENU_Y , // LXD CHANGE "+ 50"
                           PROG_BAR_MENU_W+10,
                           PROG_BAR_MENU_H-6,
                           ROOT_ID_INVALID,
                           0,
                           OBJ_ATTR_ACTIVE,
                           0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, prog_bar_cont_keymap);
  ctrl_set_proc(p_cont, prog_bar_cont_proc);

  //introduce container
  p_intro_cont = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_INTRO_CONT,
                            PROG_BAR_INTRO_CONT_X, PROG_BAR_INTRO_CONT_Y + 20,//CHANG 20 HAVE NO EFFECT
                            PROG_BAR_INTRO_CONT_W-300,PROG_BAR_INTRO_CONT_H -100, //CHANGE"60" HAVE NO EFFECT                         
                             p_cont, 0);
   ctrl_set_attr(p_intro_cont, OBJ_ATTR_HIDDEN);
	
  //prog info cont   LXD_CREAT BOOT1 
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_FRAME,
                             SN_PROG_INFO_INFO_CONT_X+2, SN_PROG_INFO_INFO_CONT_Y,
                             SN_PROG_INFO_INFO_CONT_W , SN_PROG_INFO_INFO_CONT_H+6,
                             p_cont, 0);
   ctrl_set_rstyle(p_ctrl, RSI_SN_BG,  RSI_SN_BG, RSI_SN_BG); // change the boot to be green


  //prog info: title bg    LXDCREAT BOOT2_LEFTUP
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE_BG,
                             PROG_BITLE_BG_X ,PROG_BITLE_BG_Y+2, PROG_BITLE_BG_W, PROG_BITLE_BG_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_SN_PROGRAM_YELLOW_BAR, RSI_SN_PROGRAM_YELLOW_BAR, RSI_SN_PROGRAM_YELLOW_BAR);


  //prog info: boot bg     LXD_CREAT BOOT2_LEFTDOWN
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_DETAIL_TEST,
                             SN_PROG_INFO_BOOT_BG_X, SN_PROG_INFO_BOOT_BG_Y+5, SN_PROG_INFO_BOOT_BG_W, SN_PROG_INFO_BOOT_BG_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_SN_PROGRAM_BG_BG, RSI_SN_PROGRAM_BG_BG, RSI_SN_PROGRAM_BG_BG);

  //only for test version information(if no chipId function,show test version information on program bar)
  p_ctrl_test_version = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_BAR_TEST_VERSION, 
                                        0, 0, 
                                        SN_PROG_INFO_BOOT_BG_W, SN_PROG_INFO_BOOT_BG_H, 
                                        p_subctrl, 0);
  ctrl_set_rstyle(p_ctrl_test_version, RSI_ORANGE, RSI_ORANGE, RSI_ORANGE);
#ifndef WIN32
  if(TRUE == is_test_version)
  {
  	ctrl_set_attr(p_ctrl_test_version, OBJ_ATTR_ACTIVE);
  }
  else
#endif
  {
  	ctrl_set_attr(p_ctrl_test_version, OBJ_ATTR_HIDDEN);
  }
  text_set_font_style(p_ctrl_test_version, FSI_CHINESE_BIG, FSI_CHINESE_BIG, FSI_CHINESE_BIG);
  text_set_align_type(p_ctrl_test_version, STL_CENTER|STL_VCENTER);
  text_set_content_type(p_ctrl_test_version, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl_test_version, IDS_TEST_VERSION);


  //prog number   LXD_FRONTNUMBER
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_ICON_NUMBER1,
                            SN_PROG_BAR_PG_NUM_X, SN_PROG_BAR_PG_NUN_Y+2,
                            PROG_BAR_PG_NUN_W, PROG_BAR_PG_NUN_H,
                             p_ctrl, 0);


  text_set_align_type(p_subctrl,STL_LEFT|STL_TOP);        // set number in ctrl begin left & top  
  text_set_font_style(p_subctrl, FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N); // 001 black to white
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);  


  //prog name     LXD_FRONTNAME
  title_bar_size = PROG_BAR_PG_NAME_W;
  if(ui_get_disable_display_time_status() == TRUE)
  {
    title_bar_size += PROG_BAR_TIME_W;
  }
  p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_NAME,
                            SN_PROG_BAR_PG_NAME_X, SN_PROG_BAR_PG_NAME_Y+2,
                            title_bar_size, PROG_BAR_PG_NAME_H,
                            p_ctrl, 0);

  text_set_align_type(p_subctrl, STL_LEFT|STL_TOP);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);


  // date & time
  if(ui_get_disable_display_time_status() == FALSE)
  {
      p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, SN_IDC_TIME,                   //LXD CHANGE IDC_TIME TO SN_IDC_TIME
                                SN_PROG_BAR_DATATIME_X, SN_PROG_BAR_DATATIME_Y,    //PROG_BITLE_BG_X+PROG_BITLE_BG_W+
                                SN_PROG_BAR_DATATIME_W, SN_PROG_BAR_DATATIME_H,
                                p_ctrl, 0);
     ctrl_set_rstyle(p_subctrl, RSI_SN_PROGRAM_BG, RSI_SN_PROGRAM_BG, RSI_SN_PROGRAM_BG);
	 text_set_align_type(p_subctrl, STL_LEFT|STL_TOP);//STL_LEFT|STL_VCENTER
     text_set_font_style(p_subctrl, FSI_SECOND_TITLE,
   					  FSI_SECOND_TITLE,FSI_SECOND_TITLE);
	 text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);     
    }
 

  // epg pf
  y = PROG_BAR_PF_INFO_Y ;        
  for (i = 0; i < PROG_BAR_PF_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_EPG_P+ i),
                       SN_PROG_BAR_PF_INFO_X, y+5,         // y add --> two strings down
                       PROG_BAR_PF_INFO_W, PROG_BAR_PF_INFO_H,
                       p_ctrl, 0);

	
    text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_SECOND_TITLE,     //   the two stings in the big ctrl
                     FSI_SECOND_TITLE,FSI_SECOND_TITLE);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_breakword(p_subctrl, TRUE);

    y+= PROG_BAR_PF_INFO_H + PROG_BAR_PF_INFO_V_GAP;
  }

  // advertisement
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_AD_WIN,
                             SN_PROG_BAR_AD_X+2, SN_PROG_BAR_AD_Y,
                             PROG_BAR_AD_W+7,PROG_BAR_AD_H+6,
                             p_cont, 0);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
	ctrl_set_rstyle(p_ctrl, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);
  #else
	ctrl_set_rstyle(p_ctrl, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);
  #endif
  //bmp_pic_draw(p_ctrl);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  fill_time_info(p_cont, FALSE);
  fill_prog_info(p_cont, FALSE, prog_id);

  ui_enable_signal_monitor(TRUE);

  if (CUSTOMER_CHANGJIANG_JS != cfg.customer)

  ui_pic_stop();
  ctrl_paint_ctrl(p_cont, FALSE);

  if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL)
  {
	  ui_play_prog(prog_id, FALSE);
  }

#if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) ||(CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
    ui_pic_play(ADS_AD_TYPE_CHBAR, ROOT_ID_PROG_BAR);
#else
    ui_pic_play(ADS_AD_TYPE_MENU_DOWN, ROOT_ID_PROG_BAR);
#endif


  ui_set_epg_dy();
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

#ifdef CUS_TONGGUANG_HEBI
 if(para1 == V_KEY_RED)
 {
   on_pbar_show_introduce(p_cont, MSG_INTRODUCE_P, 0, 0);
 }
 else if(para1 == V_KEY_BLUE)
 {
   on_pbar_show_introduce(p_cont, MSG_INTRODUCE_F, 0, 0);
 }
#endif

 return SUCCESS;
}

static RET_CODE on_pbar_show_introduce(control_t *p_cont, u16 msg, u32 para1,u32 para2)
{
  control_t *p_detail, *p_title, *p_intro_cont;
  dvbs_prog_node_t pg;
  epg_filter_t filter;
  u32 evt_cnt = 0;
  evt_node_t *p_evt_node = NULL;
  u16 uni_buf[256];
  u16 prog_pos;
  u16 ids_str = 0;
  u8 view_id;
  u16 curn_group, curn_mode, pg_id, view_pos;
  u32 group_context= 0;
  memset(uni_buf, 0, sizeof(uni_buf)/sizeof(u16));
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &
  prog_pos, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  view_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return ERR_FAILURE;
  }
  p_intro_cont = ctrl_get_child_by_id(p_cont, IDC_INTRO_CONT);
  p_title = ctrl_get_child_by_id(p_intro_cont, IDC_TITLE);
  p_detail = ctrl_get_child_by_id(p_intro_cont, IDC_DETAIL);
  memset(&filter, 0, sizeof(epg_filter_t));
  filter.service_id = (u16)pg.s_id;
    if(pg.orig_net_id == 0)                                        //for asia 3
  {
    filter.stream_id = 0xffff;
    filter.orig_network_id = 0xffff;
  }
  else
  {
    filter.stream_id = (u16)pg.ts_id;
    filter.orig_network_id = (u16)pg.orig_net_id;
  }
  filter.cont_level = 0;
  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &filter, &evt_cnt);

  switch(msg)
  {
    case MSG_INTRODUCE_P:
      g_is_curn_pg_intro = TRUE;
      ids_str = IDS_CUR_PROG_INTRO;
      if (p_evt_node != NULL)
      {
        uni_strncpy(uni_buf, p_evt_node->p_sht_txt, p_evt_node->sht_txt_len);
      }
      break;

    case MSG_INTRODUCE_F:
      g_is_curn_pg_intro = FALSE;
      ids_str = IDS_NEXT_PROG_INTRO;
      if((p_evt_node != NULL) && (p_evt_node->p_next_evt_node!= NULL))
      {
        uni_strncpy(uni_buf, p_evt_node->p_next_evt_node->p_sht_txt, p_evt_node->p_next_evt_node->sht_txt_len);
      }
      break;

    default:
      break;
  }

  text_set_content_by_strid(p_title, ids_str);
  text_set_content_by_unistr(p_detail, uni_buf);

  ctrl_set_attr(p_intro_cont, OBJ_ATTR_ACTIVE);
  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

#ifndef FAST_SWITCH_PROGRAN_ON_UP_DOWN
static RET_CODE shift_prog_in_bar(control_t *p_ctrl, s16 offset)
{
  // change prog
  control_t *p_intro_cont;
  u16 prog_id, curn_msg;
  ui_shift_prog(offset, TRUE, &prog_id);
  // update view
  fill_prog_info(p_ctrl, TRUE,prog_id);
  p_intro_cont = ctrl_get_child_by_id(p_ctrl, IDC_INTRO_CONT);
  if(ctrl_get_attr(p_intro_cont) == OBJ_ATTR_ACTIVE)
  {
    curn_msg = (g_is_curn_pg_intro ? MSG_INTRODUCE_P : MSG_INTRODUCE_F);
    on_pbar_show_introduce(p_ctrl, curn_msg , 0, 0);
  }
  OS_PRINTF("send MSG_REFRESH_ADS_PIC to mainmenu\n");
  fw_notify_root(p_ctrl, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  
  return SUCCESS;
}


static RET_CODE on_focus_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, 1);
}


static RET_CODE on_focus_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -1);
}
#endif

#if 0
static RET_CODE on_page_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, 10);
}

static RET_CODE on_page_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -10);
}
#endif
static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  fill_time_info(p_ctrl, TRUE);
  fill_prog_info(p_ctrl, TRUE, prog_id);
  return SUCCESS;
}

static RET_CODE on_pbar_update_epg_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{  
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  DEBUG(MAIN,INFO,"on_pbar_update_epg_event.....\n");
  
  fill_prog_info(p_cont, TRUE, prog_id);
  
  return SUCCESS;
}

static RET_CODE on_recall(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id,curn_msg;
  control_t *p_intro_cont;
  /* do recall */
  if (ui_recall(TRUE, &prog_id))
  {
    // update view
    fill_prog_info(p_ctrl, TRUE, prog_id);
    p_intro_cont = ctrl_get_child_by_id(p_ctrl, IDC_INTRO_CONT);
    if(ctrl_get_attr(p_intro_cont) == OBJ_ATTR_ACTIVE)
    {
      curn_msg = (g_is_curn_pg_intro ? MSG_INTRODUCE_P : MSG_INTRODUCE_F);
      on_pbar_show_introduce(p_ctrl, curn_msg , 0, 0);
    }
  }
  else
  {
    UI_PRINTF("PROGBAR: recall is failed\n");
  }
  return SUCCESS;
}

static RET_CODE on_tvradio(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id, ret;
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  /* do tvradio switch */
  ret = ui_tvradio_switch(TRUE, &prog_id);
  if(ret == RSC_INVALID_ID)
  {
    // update view
    fill_prog_info(p_ctrl, TRUE, prog_id);
  }
  else
  {
	  warn_data_t.text_strID = ret;
	  ui_comm_prompt_open(&warn_data_t);
	  if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
	  	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	  update_signal();
	  if(p_mute != NULL)
  	  {
    	open_mute(0,0);
 	  }
  }
  return SUCCESS;
}

//for plh remote control tv_key
static RET_CODE on_tvradio_tv(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id, ret;
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  if(is_tv)
  {
    return SUCCESS;
  }
  /* do tvradio switch */
  ret = ui_tvradio_switch(TRUE, &prog_id);
  if(ret == RSC_INVALID_ID)
  {
    // update view
    fill_prog_info(p_ctrl, TRUE, prog_id);
  }
  else
  {
	  warn_data_t.text_strID = ret;
	  
	  ui_comm_prompt_open(&warn_data_t);
	  if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
	  	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	  update_signal();
	  if(p_mute != NULL)
  	  {
    	open_mute(0,0);
 	  }
  }
  return SUCCESS;
}

//for plh remote control radio_key
static RET_CODE on_tvradio_radio(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id, ret;
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  if(!is_tv)
  {
    return SUCCESS;
  }
  /* do tvradio switch */
  ret = ui_tvradio_switch(TRUE, &prog_id);
  if(ret == RSC_INVALID_ID)
  {
    // update view
    fill_prog_info(p_ctrl, TRUE, prog_id);
  }
  else
  {
	  warn_data_t.text_strID = ret;
	  ui_comm_prompt_open(&warn_data_t);
	  if(fw_find_root_by_id(ROOT_ID_PROG_BAR)!=NULL)
	  	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	  update_signal();
	  if(p_mute != NULL)
  	  {
    	open_mute(0,0);
 	  }
  }
  return SUCCESS;
}
static RET_CODE on_subt_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  return SUCCESS;
}
static RET_CODE on_ttx_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  return SUCCESS;
}

extern RET_CODE ui_check_fullscr_ad_play(void);

static RET_CODE on_pbar_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  ui_pic_stop();
  ui_check_fullscr_ad_play();
  return ERR_NOFEATURE;
}
static RET_CODE on_bar_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

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
  fill_prog_info(ctrl_get_root(p_ctrl), FALSE, prog_id); //add for bug 13492
  ctrl_paint_ctrl(ctrl_get_root(p_ctrl),TRUE);
  return SUCCESS;
}




static RET_CODE pass_key_to_parent(u16 msg,
                            u32 para1, u32 para2)
{

  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_SWITCH_AUDIO:
      key = V_KEY_AUDIO;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PROG_BAR, NOTIFY_T_KEY, FALSE, key, para1, para2);

  return SUCCESS;
}

static RET_CODE on_pbar_switch_audio(control_t * p_ctrl, u16 msg, u32 para1, u32 para2)
{

  pass_key_to_parent(msg,para1,para2);
  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  return SUCCESS;
}


/****fast switch program for up and down******/
#ifdef FAST_SWITCH_PROGRAN_ON_UP_DOWN

static u16 fast_prog_id = 0;
static void set_fast_switch_prog_id(u16 id)
{
  fast_prog_id = id;
}
static u16 get_fast_switch_prog_id(void)
{
  return fast_prog_id;
}
static RET_CODE fast_switch_prog_tmr_create(void)
{
    return fw_tmr_create(ROOT_ID_PROG_BAR, MSG_PROG_FAST_SWITCH, 
                                                           FAST_SWITCH_PROGRAN_TIMEOUT, FALSE);
}


static RET_CODE  fast_switch_prog_tmr_destroy(void)
{
   return fw_tmr_destroy(ROOT_ID_PROG_BAR, MSG_PROG_FAST_SWITCH);
}

static RET_CODE fast_switch_prog_tmr_reset(void)
{
  if(fw_tmr_reset(ROOT_ID_PROG_BAR, MSG_PROG_FAST_SWITCH,
                                       FAST_SWITCH_PROGRAN_TIMEOUT) != SUCCESS)
  {
    return fast_switch_prog_tmr_create();
  }
  return SUCCESS;
}
static RET_CODE fill_prog_fast_switch_info(control_t *cont,u16 prog_id)
{
  control_t *p_frm, *p_subctrl;
  dvbs_prog_node_t pg;
  u8 asc_buf[8];
  u8 view_id;
  u16 view_pos;
  u16 uni_char[64];
  
  view_id = ui_dbase_get_pg_view_id();
  view_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  if(NULL == cont)
  {
    return ERR_FAILURE;
  }
  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  if(NULL == p_frm)
  {
    return ERR_FAILURE;
  }
  
  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return ERR_FAILURE;
  }
  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_ICON_NUMBER1);
  #ifdef LCN_SWITCH
    if(pg.is_scrambled)
      sprintf((char*)asc_buf, "%.4d$", pg.logic_ch_num);
    else
      sprintf((char*)asc_buf, "%.4d ", pg.logic_ch_num);
  #else
    if(pg.is_scrambled)
      sprintf((char *)asc_buf, "%.3d$", view_pos + 1);
    else
      sprintf((char *)asc_buf, "%.3d ", view_pos + 1);
  #endif
  text_set_content_by_ascstr(p_subctrl, asc_buf);

  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_NAME);
  text_set_content_by_unistr(p_subctrl, pg.name);

  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_P);
  str_asc2uni((u8 *)"",uni_char);
  text_set_content_by_unistr(p_subctrl, uni_char);
  p_subctrl = ctrl_get_child_by_id(p_frm, IDC_EPG_F);
  str_asc2uni((u8 *)"",uni_char);
  text_set_content_by_unistr(p_subctrl, uni_char);
  
  ctrl_paint_ctrl(cont, TRUE);
  return SUCCESS;
}


static RET_CODE shift_fast_switch_prog_in_bar(control_t *p_ctrl, s16 offset)
{
  u16 prog_id = 0;
  u32 tick = 0;
  u16 prog_pos;
  tick = mtos_ticks_get();
  ui_shift_prog(offset, FALSE, &prog_id);
  fill_prog_fast_switch_info(p_ctrl,prog_id);
  set_fast_switch_prog_id(prog_id);

   prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), 
                                          prog_id);
  sys_status_set_curn_group_info(prog_id, prog_pos);
  sys_status_save();

  return SUCCESS;
}
static RET_CODE on_focus_fast_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  fast_switch_prog_tmr_reset();
  shift_fast_switch_prog_in_bar(p_ctrl, 1);
  return SUCCESS;
}

static RET_CODE on_focus_fast_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  fast_switch_prog_tmr_reset();
  shift_fast_switch_prog_in_bar(p_ctrl, -1);
  return SUCCESS;
}
static RET_CODE shift_fast_switch_play_prog_in_bar(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_intro_cont;
  u16 prog_id, curn_msg;
  
  fast_switch_prog_tmr_destroy();

  prog_id = get_fast_switch_prog_id();
  ui_play_prog(prog_id, FALSE);
  // update view
  fill_prog_info(p_ctrl, TRUE,prog_id);
  p_intro_cont = ctrl_get_child_by_id(p_ctrl, IDC_INTRO_CONT);
  if(ctrl_get_attr(p_intro_cont) == OBJ_ATTR_ACTIVE)
  {
    curn_msg = (g_is_curn_pg_intro ? MSG_INTRODUCE_P : MSG_INTRODUCE_F);
    on_pbar_show_introduce(p_ctrl, curn_msg , 0, 0);
  }
  return SUCCESS;
}

#endif
/****fast switch program for up and down end******/

static RET_CODE on_prog_bar_draw_pic_end(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}
static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("on_item_refresh_ads\n");

#if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) || (CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
    ui_pic_play(ADS_AD_TYPE_CHBAR, ROOT_ID_PROG_BAR);
#else
    ui_pic_play(ADS_AD_TYPE_MENU_DOWN, ROOT_ID_PROG_BAR);
#endif
  return SUCCESS;
}

BEGIN_KEYMAP(prog_bar_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RECALL, MSG_RECALL)
  ON_EVENT(V_KEY_BACK, MSG_RECALL)
  ON_EVENT(V_KEY_TVRADIO, MSG_TVRADIO)
  ON_EVENT(V_KEY_TV,MSG_TVRADIO_TV)
  ON_EVENT(V_KEY_RADIO,MSG_TVRADIO_RADIO)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_F1, MSG_INTRODUCE_P)
  ON_EVENT(V_KEY_RED, MSG_INTRODUCE_P)
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_F2, MSG_INTRODUCE_F)
  ON_EVENT(V_KEY_GREEN, MSG_INTRODUCE_F)
#else
  ON_EVENT(V_KEY_F2, MSG_INTRODUCE_F)
  ON_EVENT(V_KEY_YELLOW, MSG_INTRODUCE_F)
#endif
  ON_EVENT(V_KEY_INFO, MSG_EPG_DEL) 
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
  ON_EVENT(V_KEY_AUDIO, MSG_SWITCH_AUDIO)
END_KEYMAP(prog_bar_cont_keymap, NULL)

BEGIN_MSGPROC(prog_bar_cont_proc, ui_comm_root_proc)

  #ifdef FAST_SWITCH_PROGRAN_ON_UP_DOWN
  ON_COMMAND(MSG_FOCUS_UP, on_focus_fast_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_fast_down)
  ON_COMMAND(MSG_PROG_FAST_SWITCH, shift_fast_switch_play_prog_in_bar)
  #else
  ON_COMMAND(MSG_FOCUS_UP, on_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_down)
  #endif
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_EPG_READY, on_pbar_update_epg_event)
  ON_COMMAND(MSG_RECALL, on_recall)
  ON_COMMAND(MSG_TVRADIO, on_tvradio)
  ON_COMMAND(MSG_TVRADIO_TV, on_tvradio_tv)
  ON_COMMAND(MSG_TVRADIO_RADIO, on_tvradio_radio)
  ON_COMMAND(MSG_TTX_READY, on_ttx_ready)
  ON_COMMAND(MSG_SUBT_READY, on_subt_ready)
  ON_COMMAND(MSG_DESTROY, on_pbar_destory)

  ON_COMMAND(MSG_SET_LANG, on_bar_switch_lang)
  ON_COMMAND(MSG_SWITCH_AUDIO, on_pbar_switch_audio)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_prog_bar_draw_pic_end)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
END_MSGPROC(prog_bar_cont_proc, ui_comm_root_proc)

