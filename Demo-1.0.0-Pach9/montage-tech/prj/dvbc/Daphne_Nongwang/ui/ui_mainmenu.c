/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_mainmenu.h"
#include "ui_comm_dlg.h"
#include "Z_XUXIN.h"
#include "ui_mute.h"
#include "ui_signal.h"



enum btn_control_id
{
  IDC_INVALID = 0,
  IDC_MAIN_PREV,
  IDC_MAIN_AD_BG,
  
  IDC_CHANNEL_LIST,
  IDC_PROGRAM_GUIDE, 
  IDC_EMAIL,
  IDC_CHANNEL_EDIT,
  IDC_GAME_INN,
  IDC_SYSTEM_SET,
  IDC_PROGRAM_CLASSIFY,  
  IDC_PROGRAM_NAVIGATE,  
  IDC_NVOD, 
  IDC_INFORMATION_PLAZA, 
  IDC_STOCK_ANALYZE, 
  
  IDC_TEST_VERSION,
};

enum local_msg
{
  MSG_CA_HIDE_MENU = MSG_LOCAL_BEGIN + 460,
};
static u8 g_curn_index = 0;
#ifndef WIN32
extern BOOL is_test_version;
#endif
u16 mainmenu_menu_cont_keymap(u16 key);
RET_CODE main_menu_cont_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 mainmenu_btn_keymap(u16 key);

RET_CODE mainmenu_btn_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);



RET_CODE open_main_menu(u32 para1, u32 para2)
{
  #define SN_SUB_MENU_HELP_RSC_CNT	15
  #define SN_MAINMENU_AD_X			(MAINMENU_AD_X + 25)
  #define SN_MAINMENU_AD_Y			(MAINMENU_AD_Y - 2)
  #define SN_MAINMENU_AD_W			(MAINMENU_AD_W - 51)
  #define SN_MAINMENU_AD_H			(MAINMENU_AD_H + 3)
  control_t *p_cont, *p_tv_win, *p_ctrl_test_version, *p_btn[MAINMENU_CNT], *p_icon[MAINMENU_CNT];//,*p_txt[MAINMENU_CNT];
  control_t *p_ctrl_ad_bg = NULL;//,*p_title;
  u16 i,x,y;
  u16 menu_item[MAINMENU_CNT][3] = 
  {
    {RSI_SN_CHLIST_SH,   RSI_SN_CHLIST_HL, IDS_PRO_LIST},//IDS_TV_PROGRAM
    {RSI_SN_EPG_SH,       RSI_SN_EPG_HL,      IDS_CHANNEL_GUIDE}, 
    {RSI_SN_CHEDIT_SH,  RSI_SN_CHEDIT_HL, IDS_MEMAIL}, 
    {RSI_SN_SYSSET_SH,  RSI_SN_SYSSET_HL,  IDS_PROGRAM_MANAGE},
    {RSI_SN_GAME_SH,     RSI_SN_GAME_HL,    IDS_PROGRAM_BOOK}, 
    {RSI_SN_MSG_SH,       RSI_SN_MSG_HL,      IDS_SYSTEM_SET}, 
  };
  u16 menu_icon[MAINMENU_CNT] = {RSI_ICON_CHANNEL,RSI_MENU_ICON,RSI_MSG_ICON,RSI_MANAGER_ICON,
  								RSI_BOOK_ICON,RSI_SYS_ICON};
  u16 icon_num[MAINMENU_CNT][4] =
  {
	{29,24,53,43},
	{29,24,54,52},
	{31,24,47,42},
	{29,24,52,50},
	{36,24,37,50},
	{36,24,38,41},
  };

  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_RIGHT},
	 {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_MOVE},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_SELECT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
  };
  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  // create container
  DEBUG_ENABLE_MODE(MAINMENU,INFO);
  DEBUG(MAINMENU,INFO,"open_main_menu now\n");
  p_cont = ui_background_create(SN_ROOT_ID_MAINMENU,
                           MAINMENU_CONT_X, MAINMENU_CONT_Y,
                           MAINMENU_CONT_W, MAINMENU_CONT_H,
                           IDS_MAINMENU,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, mainmenu_menu_cont_keymap);
  ctrl_set_proc(p_cont, main_menu_cont_proc);

  //TV preview window
  p_tv_win = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_MAIN_PREV,
                              MAINMENU_PREV_X, MAINMENU_PREV_Y,
                              MAINMENU_PREV_W, MAINMENU_PREV_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_SN_PREVIEW_TV_WIN, RSI_SN_PREVIEW_TV_WIN, RSI_SN_PREVIEW_TV_WIN);
  //advertisement window
  #if CONFIG_CAS_ID == CONFIG_CAS_ID_DS
  p_ctrl_ad_bg = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_MAIN_AD_BG,
                              SN_MAINMENU_AD_X, SN_MAINMENU_AD_Y - 2,
                              SN_MAINMENU_AD_W, SN_MAINMENU_AD_H + 2,
                              p_cont, 0);
  
  #else
  p_ctrl_ad_bg = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)IDC_MAIN_AD_BG,
                              MAINMENU_AD_X, MAINMENU_AD_Y,
                              MAINMENU_AD_W, MAINMENU_AD_H,
                              p_cont, 0);
  #endif
  ctrl_set_rstyle(p_ctrl_ad_bg, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN, RSI_SN_ADVERTISEMENT_WIN);

  //only for test version information(if no chipId function,show test version information on TV preview window)
  p_ctrl_test_version = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TEST_VERSION, 
                                        MAINMENU_PREV_X, MAINMENU_PREV_H-10, 
                                        MAINMENU_PREV_W, 50, 
                                        p_cont, 0);
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

  // create 
  for(i=0; i<MAINMENU_CNT; i++)
  {
    if(i>=(MAINMENU_CNT/2))
    {
      x = MAINMENU_BTN_X_COL2;
      y = MAINMENU_BTN_Y + (i-MAINMENU_CNT/2)*(MAINMENU_BTN_H + MAINMENU_BTN_V_GAP);
    }
    else
    {
      x = MAINMENU_BTN_X_COL1;
      y = MAINMENU_BTN_Y + i*(MAINMENU_BTN_H + MAINMENU_BTN_V_GAP);
    }
    
    //string button
    p_btn[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_CHANNEL_LIST + i),
                              x, y, MAINMENU_BTN_W,MAINMENU_BTN_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_btn[i], (u8)menu_item[i][0], (u8)menu_item[i][1], (u8)menu_item[i][0]);
    ctrl_set_keymap(p_btn[i], mainmenu_btn_keymap);
    ctrl_set_proc(p_btn[i], mainmenu_btn_proc);
    text_set_font_style(p_btn[i], FSI_INDEX3, FSI_COMM_TXT_HL, FSI_INDEX3);//FSI_MAIN_MENU_BTN, FSI_MAIN_MENU_BTN, FSI_GRAY
    text_set_align_type(p_btn[i], STL_BOTTOM| STL_CENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], menu_item[i][2]);
    text_set_offset(p_btn[i], 10, 0);
	
  //create ICON CHANNEL
  	p_icon[i] = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)(IDC_CHANNEL_LIST + i),
                              icon_num[i][0], icon_num[i][1],
                              icon_num[i][2], icon_num[i][3],
                              p_btn[i], 0);
	ctrl_set_rstyle(p_icon[i],(u8)menu_icon[i], (u8)menu_icon[i], (u8)menu_icon[i]);

  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ui_pic_stop();
  ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU);
    memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_btn[g_curn_index], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
 
  return SUCCESS;
}


static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define SN_MAINMENU_TITLE_CONT_H	53
	comm_prompt_data_t warn_data_t =
	  {
		SN_ROOT_ID_MAINMENU,
		STR_MODE_STATIC,
		MAINMENU_PREV_X+16,
		(MAINMENU_CONT_H-100+SN_MAINMENU_TITLE_CONT_H)/2,
		MAINMENU_PREV_W,100,
		50,
		IDS_NO_PROG,0,
		0,DIALOG_DEADLY,
		3000
	  };
     control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);

  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    if(fw_find_root_by_id(ROOT_ID_DIALOG) == NULL)
    {
      ui_comm_prompt_open(&warn_data_t);
	  update_signal();
	  if(p_mute != NULL)
  	  {
    	open_mute(0,0);
 	  }
	  if(ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU) == TRUE)
	  {
	  	ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU);
	  }
	  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
     }
   return SUCCESS;
  }
  ui_pic_stop();
  return ERR_NOFEATURE;
}


static RET_CODE on_exit_mainmenu(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
#define SN_MAINMENU_TITLE_CONT_H	53
	comm_prompt_data_t warn_data_t =
	  {
		ROOT_ID_INVALID,
		STR_MODE_STATIC,
		MAINMENU_PREV_X+16,
		(MAINMENU_CONT_H-100+SN_MAINMENU_TITLE_CONT_H)/2,
		MAINMENU_PREV_W,100,
		50,
		IDS_NO_PROG,0,
		0,DIALOG_DEADLY,
		3000
	  };
  control_t *p_mute = fw_find_root_by_id(ROOT_ID_MUTE);
  fw_destroy_mainwin_by_id(SN_ROOT_ID_MAINMENU);
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    if(fw_find_root_by_id(ROOT_ID_DIALOG) == NULL)
    {
      ui_comm_prompt_open(&warn_data_t);
	  update_signal();
	  if(p_mute != NULL)
  	  {
    	open_mute(0,0);
 	  }  
	  if(ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU) == TRUE)
	  {
	  	ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU);
	  }
	  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
    }
    return SUCCESS;
  }
  ui_pic_stop();
  
  return SUCCESS;
}


static RET_CODE on_save_mainmenu(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    // update view at first
    fw_paint_all_mainwin();

    return ERR_FAILURE;
  }

  return SUCCESS;
}

static RET_CODE on_mainmenu_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  RET_CODE ret = ERR_FAILURE;
  //comm_dialog_data_t dialog;
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl->p_parent));
  
  switch(ctrlID)
  {
    case IDC_CHANNEL_LIST:
      ret = manage_open_menu(ROOT_ID_PROG_LIST, 0, 0);
    #if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) || (CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
      ret = ERR_FAILURE;
    #endif
      break;

    case IDC_PROGRAM_GUIDE:
      ret = manage_open_menu(ROOT_ID_EPG, 0, 0);	  
    #if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) || (CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
      ret = ERR_FAILURE;
    #endif
      break;
      
    case IDC_PROGRAM_CLASSIFY:
     ret = manage_open_menu(ROOT_ID_PRO_CLASSIFY, 0, 0);
      break;
      
    case IDC_CHANNEL_EDIT://IDC_FAV_LIST:
      ret = manage_open_menu(ROOT_ID_CHANNEL_EDIT,0,0);//ROOT_ID_FAV_LIST, 0, 0);
      //when open sub menu stop play
	  ui_stop_play(STOP_PLAY_BLACK,TRUE);
      break;
    case IDC_PROGRAM_NAVIGATE:
      ret = manage_open_menu(ROOT_ID_MOSAIC, 0, 0);
      break;
	  
	case IDC_NVOD:
      ret = manage_open_menu(ROOT_ID_NVOD, 0, 0);
      break;
      
    case IDC_INFORMATION_PLAZA:
      ret = manage_open_menu(ROOT_ID_BROWSER, 0, 0);
      break;
      
    case IDC_EMAIL:
      ret = manage_open_menu(ROOT_ID_EMAIL_MESS, 0, 0);
	  //when open sub menu stop play
	  ui_stop_play(STOP_PLAY_BLACK,TRUE);
      break;
      
    case IDC_GAME_INN:
      ret = manage_open_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);//ROOT_ID_GAME_INN,ROOT_ID_PREBOOK_MANAGE
      //when open sub menu stop play
	  ui_stop_play(STOP_PLAY_BLACK,TRUE);
      break;
      
    case IDC_STOCK_ANALYZE:
      break;
      
    case IDC_SYSTEM_SET:
      ret = manage_open_menu(SN_ROOT_ID_SUBMENU, 0, 0);
	  //when open sub menu stop play
	  ui_stop_play(STOP_PLAY_BLACK,TRUE);
      break;

    default:
      break;
  }
  
  if(ret == SUCCESS)
  {
    ui_pic_stop();
  }
  return SUCCESS;
}

static RET_CODE on_mainmenu_btn_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl;
  
  p_cont = p_ctrl->p_parent;
  
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
    case MSG_FOCUS_LEFT:
      g_curn_index = (g_curn_index + MAINMENU_CNT/2) % MAINMENU_CNT;
      if(ctrl_get_attr(ctrl_get_child_by_id(p_cont, IDC_CHANNEL_LIST+g_curn_index)) == OBJ_ATTR_INACTIVE)
      {
        g_curn_index = (g_curn_index + MAINMENU_CNT/2) % MAINMENU_CNT;
        return SUCCESS;
      }
      break;

    case MSG_FOCUS_UP:
      g_curn_index = (g_curn_index + MAINMENU_CNT - 1) % MAINMENU_CNT;
      while(ctrl_get_attr(ctrl_get_child_by_id(p_cont, IDC_CHANNEL_LIST+g_curn_index)) == OBJ_ATTR_INACTIVE)
      {
        g_curn_index = (g_curn_index + MAINMENU_CNT - 1) % MAINMENU_CNT;
      }
      break;

    case MSG_FOCUS_DOWN:
      g_curn_index = (g_curn_index + 1) % MAINMENU_CNT;
      while(ctrl_get_attr(ctrl_get_child_by_id(p_cont, IDC_CHANNEL_LIST+g_curn_index)) == OBJ_ATTR_INACTIVE)
      {
        g_curn_index = (g_curn_index + 1) % MAINMENU_CNT;
      }
      break;
  }

  p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_CHANNEL_LIST+g_curn_index);
  
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, FALSE);
  ctrl_paint_ctrl(p_next_ctrl, FALSE);
  
  return SUCCESS;
}

static RET_CODE on_maimenu_list_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 cur_mode = sys_status_get_curn_prog_mode();
    
  switch(cur_mode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;

    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
  }
  ctrl_process_msg(p_list->p_parent, MSG_EXIT_ALL, para1, para2);
  return SUCCESS;
}

static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_pic_play(ADS_AD_TYPE_MENU_UP, SN_ROOT_ID_MAINMENU);

  return SUCCESS;
}

static RET_CODE on_mainmenu_ad_getdata_end(control_t *p_cont, u16 msg, 
                               u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_mainmenu_destroy(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_pic_stop();
  return ERR_NOFEATURE;
}


static RET_CODE on_ca_hide_menu(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(fw_get_focus_id() == SN_ROOT_ID_MAINMENU)
  {
    manage_open_menu(ROOT_ID_HIDE_CA_MENU, 0, 0);
  }
  
  return SUCCESS;
}

BEGIN_KEYMAP(mainmenu_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)
END_KEYMAP(mainmenu_menu_cont_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(mainmenu_btn_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_BACK, MSG_BACKSPACE)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CA_HIDE_MENU, MSG_CA_HIDE_MENU)
END_KEYMAP(mainmenu_btn_keymap, NULL)

BEGIN_MSGPROC(mainmenu_btn_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_mainmenu_btn_select)
  ON_COMMAND(MSG_FOCUS_UP, on_mainmenu_btn_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_mainmenu_btn_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_mainmenu_btn_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_mainmenu_btn_change_focus)
  ON_COMMAND(MSG_BACKSPACE, on_maimenu_list_exit_all)
  ON_COMMAND(MSG_CA_HIDE_MENU, on_ca_hide_menu)
END_MSGPROC(mainmenu_btn_proc, text_class_proc)


BEGIN_MSGPROC(main_menu_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
  ON_COMMAND(MSG_EXIT, on_exit_mainmenu)
  ON_COMMAND(MSG_SAVE, on_save_mainmenu)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_mainmenu_ad_getdata_end)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
  ON_COMMAND(MSG_DESTROY, on_mainmenu_destroy)
END_MSGPROC(main_menu_cont_proc, ui_comm_root_proc)



