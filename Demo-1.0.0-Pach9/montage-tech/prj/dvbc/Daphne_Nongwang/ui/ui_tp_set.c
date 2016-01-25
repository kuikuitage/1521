/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_channel_warn.h"
#include "ui_tp_set.h"
#include "ui_comm_root.h"
#include "sys_dbg.h"

enum tp_set_local_msg
{
  MSG_OK = MSG_LOCAL_BEGIN + 525,
};

enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,
  IDC_TV,
  IDC_NVOD,

  IDC_TV_MAIN_TP_FREQ,
  IDC_TV_MAIN_TP_SYM,
  IDC_TV_MAIN_TP_DEMOD,
  IDC_NVOD_TP_FREQ,
  IDC_NVOD_TP_SYM,
  IDC_NVOD_TP_DEMOD,
  IDC_PREV,
};

comm_dlg_data_t tp_set_exit_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  DLG_STR_MODE_NULL,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_SAVE_MODIFY,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  0,
};

static dvbc_lock_t main_tp = {0};
static dvbc_lock_t nvod_tp = {0};
static BOOL g_is_search_flag = FALSE;

extern u32 ui_get_dvbc_min_freq(void);
extern u32 ui_get_dvbc_max_freq(void);
u16 tp_set_cont_keymap(u16 key);
RET_CODE tp_set_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE tp_set_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static RET_CODE on_tp_set_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);
  ctrl_id = ctrl_get_ctrl_id(p_child);

  return ERR_NOFEATURE;
}

void tp_set_search_flag(BOOL flag)
{
  g_is_search_flag = flag;
}

static void on_tp_set_get_tp_data(control_t *p_cont, dvbc_lock_t* tp1, dvbc_lock_t* tp2)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TV_MAIN_TP_FREQ);
  tp1->tp_freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TV_MAIN_TP_SYM);
  tp1->tp_sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TV_MAIN_TP_DEMOD);
  tp1->nim_modulate = ui_comm_select_get_focus(p_ctrl) + 4;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NVOD_TP_FREQ);
  tp2->tp_freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NVOD_TP_SYM);
  tp2->tp_sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NVOD_TP_DEMOD);
  tp2->nim_modulate = ui_comm_select_get_focus(p_ctrl) + 4;

}

static RET_CODE on_tp_set_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  dvbc_lock_t tp1 = {0};
  dvbc_lock_t tp2 = {0};
  control_t  *p_main, *p_nvod;
  u32 main_freq, nvod_freq, max_freq, min_freq;
  customer_cfg_t cfg = {0};
  comm_prompt_data_t warn_data_t =
    {
      ROOT_ID_TP_SET,
	  STR_MODE_STATIC,
      200,165,250,150,15,
      IDS_SAVE_DATA,0,
      0,DIALOG_DEADLY,
      3000
    };
  get_customer_config(&cfg);

  p_main = ctrl_get_child_by_id(p_cont, IDC_TV_MAIN_TP_FREQ);
  p_nvod = ctrl_get_child_by_id(p_cont, IDC_NVOD_TP_FREQ);
  max_freq = ui_get_dvbc_max_freq();
  min_freq = ui_get_dvbc_min_freq();
  main_freq = ui_comm_numedit_get_num(p_main);
  nvod_freq = ui_comm_numedit_get_num(p_nvod);
  if(main_freq < min_freq || main_freq > max_freq
    ||nvod_freq < min_freq || nvod_freq > max_freq)
  {
    if(main_freq < min_freq)
    {
      ui_comm_numedit_set_num(p_main, min_freq);
    }
     else if(main_freq > max_freq)
    {
      ui_comm_numedit_set_num(p_main, max_freq);
    }
    else if(nvod_freq < min_freq)
    {
      ui_comm_numedit_set_num(p_nvod, min_freq);
    }
    else if(nvod_freq > max_freq)
    {
      ui_comm_numedit_set_num(p_nvod, max_freq);
    }
    ctrl_paint_ctrl(p_cont, TRUE);
    ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, OUTRANGE_TMOUT);
    return SUCCESS;
  }
  on_tp_set_get_tp_data(p_cont, &tp1, &tp2);

  memcpy(&main_tp, &tp1, sizeof(dvbc_lock_t));
  memcpy(&nvod_tp, &tp2, sizeof(dvbc_lock_t));

  sys_status_set_main_tp1(&main_tp);
  sys_status_set_nvod_tp(&nvod_tp);

  sys_status_save();
  #ifndef OTA_DM_ON_STATIC_FLASH
  sys_status_set_boot_status();
  #endif
  ui_comm_prompt_open(&warn_data_t);
  mtos_task_delay_ms(1000);
  
  if((g_is_search_flag == TRUE) &&
    ((cfg.customer == CUSTOMER_BOYUAN) ||
    (cfg.customer == CUSTOMER_PANLONGHU) || 
    (cfg.customer == CUSTOMER_ZHILING) ||
     (cfg.customer == CUSTOMER_ZHILING_KF) ||
      (cfg.customer == CUSTOMER_ZHILING_LQ) ||
    (cfg.customer == CUSTOMER_SHENZHOU_QN)))
  {
    g_is_search_flag = FALSE;
    ui_cas_factory_set(CAS_CMD_FACTORY_SET, 0, 0);
    manage_open_menu(SN_ROOT_ID_SEARCH, SCAN_TYPE_AUTO, 0);
  }
  else
  {
      manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
  }
  
  return SUCCESS;
}

static RET_CODE on_tp_set_select_save(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont;

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));

  on_tp_set_save(p_cont, msg, para1, para2);
  
  return SUCCESS;
}

static RET_CODE on_tp_exit(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{  
    dlg_ret_t ret=FALSE;
	comm_dialog_data_t dialog;
  dvbc_lock_t tp1 = {0};
  dvbc_lock_t tp2 = {0};
  
  on_tp_set_get_tp_data(p_ctrl, &tp1, &tp2);
  g_is_search_flag = FALSE;
	dialog.grade = DIALOG_INFO;
	dialog.x      = 170;
	dialog.y      = 160;
	dialog.w     = 300;
	dialog.h     = 150;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID   = IDS_SAVING_OR_NOT;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;

  if(memcmp(&main_tp, &tp1, sizeof(dvbc_lock_t)) != 0
 		 || memcmp(&nvod_tp, &tp2, sizeof(dvbc_lock_t)))
  {
	ret=ui_comm_dialog_open(&dialog);
	if(DLG_RET_YES==ret)
	{
      memcpy(&main_tp, &tp1, sizeof(dvbc_lock_t));
      memcpy(&nvod_tp, &tp2, sizeof(dvbc_lock_t));
      on_tp_set_save(p_ctrl,0,0,0);
      sys_status_set_main_tp1(&main_tp);
      sys_status_set_nvod_tp(&nvod_tp);
      sys_status_save();
    
    #ifndef OTA_DM_ON_STATIC_FLASH
      sys_status_set_boot_status();
    #endif
      ui_comm_pwdlg_close();
      ret = FALSE;
    }
  }

  return ERR_NOFEATURE;
}


RET_CODE open_tp_set(u32 para1, u32 para2)
{
  #define SN_TP_SET_HELP_RSC_CNT				15
  #define SN_SCREEN_WIDTH						640
  #define SN_SCREEN_HEIGHT						480
  #define SN_TP_SET_CONT1_X                 96
  #define SN_TP_SET_CONT1_Y                 55
  #define SN_TP_SET_CONT1_W                 445
  #define SN_TP_SET_CONT1_H                 165

  //create menu item
  #define SN_TP_SET_ITEM_X                  SN_TP_SET_CONT1_X + 10//306
  #define SN_TP_SET_ITEM_Y                  SN_TP_SET_CONT1_Y + 40//165
  #define SN_TP_SET_ITEM_LW                 320
  #define SN_TP_SET_ITEM_MW                 SN_TP_SET_CONT1_W - SN_TP_SET_ITEM_LW
  #define SN_TP_SET_ITEM_RW                 0

  #define SN_TP_SET_ITEM_V_GAP              20


  control_t *p_cont, *p_nvod, *p_ctrl_item[TP_SET_ITEM_CNT];//, *p_tv
  control_t *p_small_bg1, *p_small_bg2;
  u8 i;
  u16 stxt [TP_SET_ITEM_CNT] =
  { 
    IDS_FREQ_SET_MAIN_FREQ, IDS_FREQ_SET_SYMBOL, IDS_FREQ_SET_DEMODUL,
    IDS_FREQ_SET_MAIN_FREQ, IDS_FREQ_SET_SYMBOL, IDS_FREQ_SET_DEMODUL,
  };
  u16 y;

  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_TP_SET_HELP_RSC_CNT]=
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

  p_cont = ui_background_create(ROOT_ID_TP_SET,
                             0, 0,
                             SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                             IDS_CA_MAIN_FREQ_SET,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, tp_set_cont_keymap);
  ctrl_set_proc(p_cont, tp_set_cont_proc);

  //TV main tp set cont
  p_small_bg1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG1,
                              SN_TP_SET_CONT1_X, SN_TP_SET_CONT1_Y,
                              SN_TP_SET_CONT1_W, SN_TP_SET_CONT1_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_small_bg1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_TIP_BOX

  //NVOD main tp set cont
  p_small_bg2 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG2,
                              0, 0,
                              TP_SET_CONT2_W, TP_SET_CONT2_H,
                              p_cont, 0);
//  ctrl_set_rstyle(p_small_bg2, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
  
  ctrl_set_attr(p_small_bg2, OBJ_ATTR_HIDDEN);

  //nvod name
  p_nvod = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_NVOD,
                              0, 0,
                              TP_SET_NVOD_W, TP_SET_NVOD_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_nvod, RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECT, RSI_SN_SET_BUTTON_SELECT);
  text_set_font_style(p_nvod, FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);//FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_N);
  text_set_align_type(p_nvod, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_nvod, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_nvod, IDS_FREQ_SET_NVOD);
  ctrl_set_attr(p_nvod, OBJ_ATTR_HIDDEN);

  //create menu item
  y = SN_TP_SET_ITEM_Y;

  for (i = 0; i < TP_SET_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
      case 3:
      case 4:
		DEBUG_ENABLE_MODE(TP_SET, INFO);
		DEBUG(TP_SET, INFO, "open_tp_set......\n");
        p_ctrl_item[i] = ui_comm_numedit_create_withposix(p_cont, (u8)(IDC_TV_MAIN_TP_FREQ + i),
                                             SN_TP_SET_ITEM_X-30, y, SN_TP_SET_ITEM_LW+60,
                                             SN_TP_SET_ITEM_MW+10, SN_TP_SET_ITEM_RW);
		DEBUG_ENABLE_MODE(TP_SET, INFO);
		DEBUG(TP_SET, INFO, "open_tp_set......\n");
        ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_numedit_set_posix_txt(p_ctrl_item[i],
                                      ((i==0)||(i==3)) ? IDS_UNIT_KHZ:IDS_UNIT_KBAUD );
		ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_LEFT , RSI_SN_ALL_BG);
		ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
   		 ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
		ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i],FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);//FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
        ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_item[i], FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);//FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);//FSI_GRAY
        if((i == 3) || (i == 4))
        {
          ctrl_set_attr(p_ctrl_item[i], OBJ_ATTR_HIDDEN);
        }
        break;
      case 2:
      case 5:
        p_ctrl_item[i] = ui_comm_select_create(p_cont, (u8)(IDC_TV_MAIN_TP_FREQ + i),
                                            SN_TP_SET_ITEM_X-30, y, SN_TP_SET_ITEM_LW+60,
                                            SN_TP_SET_ITEM_MW+10);
        ui_comm_ctrl_set_proc(p_ctrl_item[i], tp_set_select_proc);
        ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_RIGHT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_txt_rstyle(p_ctrl_item[i], RSI_SN_ALL_BG, RSI_SN_SEARCH_SELECT_ARROW_LEFT, RSI_SN_ALL_BG);
		ui_comm_ctrl_set_ctrl_align_tyle(COMM_SELECT, p_ctrl_item[i], STL_LEFT|STL_VCENTER);
   		 ui_comm_ctrl_set_txt_align_tyle(p_ctrl_item[i], STL_LEFT|STL_VCENTER);
		ui_comm_ctrl_set_txt_font_style(p_ctrl_item[i],FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);//FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
        ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);//FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_COMM_TXT_N);
        ui_comm_select_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl_item[i], TRUE,
                                   CBOX_WORKMODE_STATIC, 5,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);
        ui_comm_select_set_content(p_ctrl_item[i], 0, IDS_QAM16);
        ui_comm_select_set_content(p_ctrl_item[i], 1, IDS_QAM32);
        ui_comm_select_set_content(p_ctrl_item[i], 2, IDS_QAM64);
        ui_comm_select_set_content(p_ctrl_item[i], 3, IDS_QAM128);
        ui_comm_select_set_content(p_ctrl_item[i], 4, IDS_QAM256);
        if(i == 5)
        {
          ctrl_set_attr(p_ctrl_item[i], OBJ_ATTR_HIDDEN);
        }
        break;
      default:
        MT_ASSERT(0);
        break;
    }
    ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SUB_BUTTON_SH

    y += TP_SET_ITEM_H + SN_TP_SET_ITEM_V_GAP;//TP_SET_ITEM_V_GAP

    ctrl_set_related_id(p_ctrl_item[i],
                      0,                                     /* left */
                      (u8)((i - 1 +
                              TP_SET_ITEM_CNT) %
                             TP_SET_ITEM_CNT + IDC_TV_MAIN_TP_FREQ),           /* up */
                      0,                                     /* right */
                      (u8)((i + 1) % TP_SET_ITEM_CNT + IDC_TV_MAIN_TP_FREQ));/* down */
  }

  sys_status_get_main_tp1(&main_tp);
  sys_status_get_nvod_tp(&nvod_tp);
  
  ui_comm_numedit_set_param(p_ctrl_item[0], NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[0], main_tp.tp_freq);
  ui_comm_numedit_set_param(p_ctrl_item[1], NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[1], main_tp.tp_sym);
  ui_comm_select_set_focus(p_ctrl_item[2], (main_tp.nim_modulate - 4));

  ui_comm_numedit_set_param(p_ctrl_item[3], NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[3], nvod_tp.tp_freq);
  ui_comm_numedit_set_param(p_ctrl_item[4], NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[4], nvod_tp.tp_sym);
  ui_comm_select_set_focus(p_ctrl_item[5], (nvod_tp.nim_modulate - 4));

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

    memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_TP_SET_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=37;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);

  ctrl_default_proc(p_ctrl_item[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(tp_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_OK)
  ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(tp_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tp_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_tp_set_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_tp_set_cont_focus_change)
  ON_COMMAND(MSG_OK, on_tp_set_save)
  ON_COMMAND(MSG_EXIT, on_tp_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_tp_exit)
END_MSGPROC(tp_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(tp_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_tp_set_select_save)
END_MSGPROC(tp_set_select_proc, cbox_class_proc)



