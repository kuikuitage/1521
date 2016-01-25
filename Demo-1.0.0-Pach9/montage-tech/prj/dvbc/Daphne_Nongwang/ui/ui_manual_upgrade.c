/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_channel_warn.h"
#include "ui_manual_upgrade.h"
#ifdef OTA_DM_ON_STATIC_FLASH
#include "sys_data_staic.h"
#endif
enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,
  IDC_MANUAL_UPG_BRIEF,

  IDC_MANUAL_UPG_TP_FREQ,
  IDC_MANUAL_UPG_TP_SYM,
  IDC_MANUAL_UPG_TP_DEMOD,
  IDC_MANUAL_UPG_PID,
  IDC_MANUAL_UPG_START,
};



//static dvbc_lock_t upgrade_tp = {0};

#if 0
rsc
static comm_help_data_t manual_upg_help_data = //help bar data
{
  2,
  2,
  {
    IDS_BACK,
    IDS_QUIT,
  },
  {
    IM_ICON_BACK,
    IM_ICON_EXIT,
  },
};
#endif


static BOOL g_ota_tp_save_disable = FALSE;
void ui_set_disable_ota_tp_save(void)
{
    g_ota_tp_save_disable = TRUE;
}

extern u32 ui_get_dvbc_min_freq(void);
extern u32 ui_get_dvbc_max_freq(void);
u16 manual_upgrade_cont_keymap(u16 key);
RET_CODE manual_upgrade_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE manual_upgrade_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);


u16 manual_upgrade_select_keymap(u16 key);
RET_CODE manual_upgrade_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static RET_CODE on_ota_tp_set_save(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_freq, *p_sym, *p_demod, *p_pid;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  u32 freq;
  u32 sym;
  u8 demod;
  u32 pid;
  dvbc_lock_t set_upgrade_tp = {0};
  u32 max_freq, min_freq;
  channel_warn_data_t warn_data_t =
    {
      ROOT_ID_MANUAL_UPGRADE,
      PARAM_OTA_SAVE_X,
      PARAM_OTA_SAVE_Y,
      IDS_SAVE_DATA,
    };
    if(g_ota_tp_save_disable == TRUE)
    {
      return SUCCESS;
    }
  
  p_freq = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_FREQ);
  freq = ui_comm_numedit_get_num(p_freq);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_SYM);
  sym = ui_comm_numedit_get_num(p_sym);
  p_demod = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_demod);
  p_pid = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_PID);
  pid = ui_comm_numedit_get_num(p_pid);
  max_freq = ui_get_dvbc_max_freq();
  min_freq = ui_get_dvbc_min_freq();

  if(freq < min_freq || freq > max_freq || (pid > UPGRADE_DATA_PID_MAX))
  {
    if(freq < min_freq)
    {
      ui_comm_numedit_set_num(p_freq, min_freq);
    }
     else if(freq > max_freq)
    {
      ui_comm_numedit_set_num(p_freq, max_freq);
    }
     if(pid > UPGRADE_DATA_PID_MAX)
      { 
         ui_comm_numedit_set_num(p_pid, UPGRADE_DATA_PID_MAX);
      }

    ctrl_paint_ctrl(p_cont, TRUE);
    ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, OUTRANGE_TMOUT);
    return SUCCESS;
  }
  
  
  switch(demod)
  {
    case 0:
      nim_modul = NIM_MODULA_QAM16;
      break;

    case 1:
      nim_modul = NIM_MODULA_QAM32;
      break;

    case 2:
      nim_modul = NIM_MODULA_QAM64;
      break;

    case 3:
      nim_modul = NIM_MODULA_QAM128;
      break;

    case 4:
      nim_modul = NIM_MODULA_QAM256;
      break;

    default:
      nim_modul = NIM_MODULA_QAM64;
      break;
  }

  set_upgrade_tp.tp_freq = freq;
  set_upgrade_tp.tp_sym = sym;
  set_upgrade_tp.nim_modulate = nim_modul;
  set_upgrade_tp.reserve1 = (u8)((u16)pid >> 8);
  set_upgrade_tp.reserve2 = (u8)((u16)pid & 0xff);
  sys_status_set_upgrade_tp(&set_upgrade_tp);
  sys_status_save();
  #ifdef OTA_DM_ON_STATIC_FLASH
  sys_static_set_status_ota_upg_tp();
  #else
  sys_status_set_boot_status();
  #endif
  ui_channel_warn_open(&warn_data_t);
  mtos_task_delay_ms(1000);
  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
  
  return SUCCESS;
}
static RET_CODE on_manual_upgrade_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

  return ERR_NOFEATURE;
}


static RET_CODE on_manual_upgrade_start(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_freq, *p_sym, *p_demod, *p_pid;
  control_t* p_cont;
  ota_info_t upgrade = {0};
  u32 max_freq, min_freq;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  u32 freq;
  u32 sym;
  u8 demod;
  u32 pid;
  dvbc_lock_t set_upgrade_tp = {0};
  ss_public_t *ss_public;

   p_cont = ctrl_get_parent(p_ctrl);

  p_freq = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_FREQ);
  freq = ui_comm_numedit_get_num(p_freq);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_SYM);
  sym = ui_comm_numedit_get_num(p_sym);
  p_demod = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_TP_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_demod);
  p_pid = ctrl_get_child_by_id(p_cont, IDC_MANUAL_UPG_PID);
  pid = ui_comm_numedit_get_num(p_pid);

  max_freq = ui_get_dvbc_max_freq();
  min_freq = ui_get_dvbc_min_freq();
   if(freq < min_freq || freq > max_freq || (pid > UPGRADE_DATA_PID_MAX))
  {
    if(freq < min_freq)
    {
      ui_comm_numedit_set_num(p_freq, min_freq);
    }
    else if(freq > max_freq)
    {
      ui_comm_numedit_set_num(p_freq, max_freq);
    }
   if(pid > UPGRADE_DATA_PID_MAX)
    { 
       ui_comm_numedit_set_num(p_pid, UPGRADE_DATA_PID_MAX);
    }

    ctrl_paint_ctrl(p_cont, TRUE);
    ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, OUTRANGE_TMOUT);
    return SUCCESS;
  }

  switch(demod)
  {
    case 0:
      nim_modul = NIM_MODULA_QAM16;
      break;

    case 1:
      nim_modul = NIM_MODULA_QAM32;
      break;

    case 2:
      nim_modul = NIM_MODULA_QAM64;
      break;

    case 3:
      nim_modul = NIM_MODULA_QAM128;
      break;

    case 4:
      nim_modul = NIM_MODULA_QAM256;
      break;

    default:
      nim_modul = NIM_MODULA_QAM64;
      break;
  }

  if(g_ota_tp_save_disable == TRUE)
    {
  set_upgrade_tp.tp_freq = freq;
  set_upgrade_tp.tp_sym = sym;
  set_upgrade_tp.nim_modulate = nim_modul;
  set_upgrade_tp.reserve1 = (u8)((u16)pid >> 8);
  set_upgrade_tp.reserve2 = (u8)((u16)pid & 0xff);
  sys_status_set_upgrade_tp(&set_upgrade_tp);
  sys_status_save();
  #ifdef OTA_DM_ON_STATIC_FLASH
   sys_static_set_status_ota_upg_tp();
  #else
  sys_status_set_boot_status();
  #endif
    }
  upgrade.download_data_pid = (u16)pid;
  upgrade.ota_tri = OTA_TRI_NONE;
  upgrade.sys_mode = SYS_DVBC;
  upgrade.lockc.tp_freq = freq;
  upgrade.lockc.tp_sym = sym;
  upgrade.lockc.nim_modulate = nim_modul;
  //upgrade.lockc.nim_modulate = NIM_MODULA_QAM64;

  ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));      
  upgrade.orig_software_version = ss_public->otai.orig_software_version;
  OS_PRINTF("local version = %d\n", upgrade.orig_software_version); 

  {
    dvbs_tp_node_t tp = {0};

    tp.freq = freq;
    tp.sym = sym;
    tp.nim_modulate = upgrade.lockc.nim_modulate;

    ui_set_transpond(&tp);
  }

#ifdef OTA_SUPPORT
  ui_ota_init();
#endif

  manage_open_menu(ROOT_ID_OTA_SEARCH, 0, (u32)&upgrade);
  return SUCCESS;
}

RET_CODE open_manual_upgrade(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_brief, *p_ctrl_item[MANUAL_UPG_ITEM_CNT];
  control_t *p_small_bg1, *p_small_bg2;
  u8 i;

  u16 stxt [MANUAL_UPG_ITEM_CNT] =
  {
    IDS_MANUL_UPGRADE_FREQ, IDS_MANUL_UPGRADE_SYMBOL, IDS_MANUL_UPGRADE_MODUL,
    IDS_PID,IDS_MANUAL_UPGRADE
  };
  u16 y;
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_SYSTEM_SET,RSI_TITLE_BG, IDS_MANUAL_UPGRADE};

  p_cont = ui_comm_prev_root_create(ROOT_ID_MANUAL_UPGRADE, RSI_FULL_SCREEN_BG,
                          MANUAL_UPG_MENU_X, MANUAL_UPG_MENU_Y,
                          MANUAL_UPG_MENU_W, MANUAL_UPG_MENU_H,
                          &title_data);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, manual_upgrade_cont_keymap);
  ctrl_set_proc(p_cont, manual_upgrade_cont_proc);

  //small bg 1
  p_small_bg1 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG1,
                              MANUAL_UPG_CONT1_X, MANUAL_UPG_CONT1_Y,
                              MANUAL_UPG_CONT1_W, MANUAL_UPG_CONT1_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_small_bg1, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);

  //brief
  p_brief = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_MANUAL_UPG_BRIEF,
                              MANUAL_UPG_BRIEF_X, MANUAL_UPG_BRIEF_Y,
                              MANUAL_UPG_BRIEF_W, MANUAL_UPG_BRIEF_H,
                              p_small_bg1, 0);
  ctrl_set_rstyle(p_brief, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_brief, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_offset(p_brief, 50, 0);
  text_set_align_type(p_brief, STL_LEFT);
  text_set_content_type(p_brief, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_brief, IDS_MANUAL_OTA_TIP);

  //small bg 2
  p_small_bg2 = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_SMALL_BG2,
                              MANUAL_UPG_CONT2_X, MANUAL_UPG_CONT2_Y,
                              MANUAL_UPG_CONT2_W, MANUAL_UPG_CONT2_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_small_bg2, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);

  //create menu item
  y = MANUAL_UPG_ITEM_Y;

  for (i = 0; i < MANUAL_UPG_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
        p_ctrl_item[i] = ui_comm_numedit_create_withposix(p_cont, (u8)(IDC_MANUAL_UPG_TP_FREQ + i),
                                             MANUAL_UPG_ITEM_X, y, MANUAL_UPG_ITEM_LW,
                                             MANUAL_UPG_ITEM_MW, MANUAL_UPG_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_numedit_set_posix_txt(p_ctrl_item[i],
                                      ((i==0)||(i==3)) ? IDS_UNIT_KHZ:IDS_UNIT_KBAUD );
        ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
        break;
      case 3:
        p_ctrl_item[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_MANUAL_UPG_TP_FREQ + i),
                                             MANUAL_UPG_ITEM_X, y,
                                             MANUAL_UPG_ITEM_LW,MANUAL_UPG_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
        ui_comm_ctrl_set_ctrl_font_style(COMM_NUMEDIT, p_ctrl_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
        
        break;
      case 2:
        p_ctrl_item[i] = ui_comm_select_create(p_cont, (u8)(IDC_MANUAL_UPG_TP_FREQ + i),
                                            MANUAL_UPG_ITEM_X, y, MANUAL_UPG_ITEM_LW,
                                            MANUAL_UPG_ITEM_MW);
        ui_comm_ctrl_set_ctrl_rstyle(p_ctrl_item[i], RSI_NEW_PSW, RSI_OLD_PSW, RSI_NEW_PSW);
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
        ui_comm_ctrl_set_ctrl_font_style(COMM_SELECT, p_ctrl_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_N, FSI_GRAY);
        break;
      case 4:
        p_ctrl_item[i] = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_MANUAL_UPG_TP_FREQ + i),
                           MANUAL_UPG_ITEM_X+120, y,
                           120, 35,
                           p_cont, 0);
        ctrl_set_keymap(p_ctrl_item[i], manual_upgrade_select_keymap);
        ctrl_set_proc(p_ctrl_item[i], manual_upgrade_select_proc);
        ctrl_set_rstyle(p_ctrl_item[i], RSI_COMMON_BTN_HL, RSI_COMMON_BTN_ORANGE, RSI_COMMON_BTN_HL);
        text_set_content_type(p_ctrl_item[i], TEXT_STRTYPE_STRID);
        text_set_font_style(p_ctrl_item[i], FSI_COMM_BTN, FSI_COMM_BTN, FSI_COMM_BTN);
        text_set_align_type(p_ctrl_item[i], STL_CENTER | STL_VCENTER);
        text_set_content_by_strid(p_ctrl_item[i], stxt[i]);
        break;
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    //ui_comm_ctrl_set_cont_rstyle(p_ctrl_item[i], RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH, RSI_SUB_BUTTON_SH);

    y += MANUAL_UPG_ITEM_H + MANUAL_UPG_ITEM_V_GAP;

    ctrl_set_related_id(p_ctrl_item[i],
                      0,                                     /* left */
                      (u8)((i - 1 +
                              MANUAL_UPG_ITEM_CNT) %
                             MANUAL_UPG_ITEM_CNT + IDC_MANUAL_UPG_TP_FREQ),           /* up */
                      0,                                     /* right */
                      (u8)((i + 1) % MANUAL_UPG_ITEM_CNT + IDC_MANUAL_UPG_TP_FREQ));/* down */
  }

  sys_status_get_upgrade_tp(&upgrade_tp);
  ui_comm_numedit_set_param(p_ctrl_item[0], NBOX_NUMTYPE_DEC, ui_get_dvbc_min_freq(), ui_get_dvbc_max_freq(), 6, 0);
  ui_comm_numedit_set_num(p_ctrl_item[0], upgrade_tp.tp_freq);

  ui_comm_numedit_set_param(p_ctrl_item[1], NBOX_NUMTYPE_DEC, 0, 9999, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[1], upgrade_tp.tp_sym);

  ui_comm_select_set_focus(p_ctrl_item[2], upgrade_tp.nim_modulate - 4);

  ui_comm_numedit_set_param(p_ctrl_item[3], NBOX_NUMTYPE_DEC, 0000, UPGRADE_DATA_PID_MAX, 4, 0);
  ui_comm_numedit_set_num(p_ctrl_item[3], ((upgrade_tp.reserve1<<8)|upgrade_tp.reserve2));

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_comm_help_create(&manual_upg_help_data, p_cont);

  ctrl_default_proc(p_ctrl_item[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

#ifdef OTA_SUPPORT
  //ui_ota_init();
#endif
#endif
  return SUCCESS;
}

static RET_CODE on_manual_upgrade_exit_all(control_t *cont, u16 msg, u32 para1,
                             u32 para2)
{
#ifdef OTA_SUPPORT   
#endif
  return ERR_NOFEATURE;
}

static RET_CODE on_manual_upgrade_exit(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
#ifdef OTA_SUPPORT   
#endif
  if(g_ota_tp_save_disable == FALSE)
  {
    on_ota_tp_set_save(cont, msg, para1, para2);
  }
  return ERR_NOFEATURE;
}


BEGIN_KEYMAP(manual_upgrade_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SAVE)
END_KEYMAP(manual_upgrade_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_upgrade_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_manual_upgrade_exit_all)
  ON_COMMAND(MSG_EXIT, on_manual_upgrade_exit)	  
  ON_COMMAND(MSG_FOCUS_UP, on_manual_upgrade_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_manual_upgrade_cont_focus_change)
  ON_COMMAND(MSG_SAVE, on_ota_tp_set_save)
END_MSGPROC(manual_upgrade_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(manual_upgrade_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(manual_upgrade_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_upgrade_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_manual_upgrade_start)
END_MSGPROC(manual_upgrade_select_proc, text_class_proc)

