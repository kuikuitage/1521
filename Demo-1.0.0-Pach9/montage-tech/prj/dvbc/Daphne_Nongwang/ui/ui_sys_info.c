/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "string.h"
#include "stdio.h"
#include "ui_common.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_notify.h"

#include "ui_mainmenu.h"
#include "ui_sys_info.h"
#include "lib_unicode.h"
#include "sys_dbg.h"
#include "ui_comm_root.h"
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
#include "ads_funtion_public.h"
#endif

enum osd_set_control_id
{
  IDC_INVALID = 0,
  SN_IDC_HW_VERSION_NAME,
  SN_IDC_SW_VERSION_NAME,
  SN_IDC_RELEASE_TIME_NAME,
  SN_IDC_OUI_ID_NAME,
  SN_IDC_LOADER_VERSION_NAME,

  SN_IDC_HW_VERSION_DETAIL,
  SN_IDC_SW_VERSION_DETAIL,
  SN_IDC_RELEASE_TIME_DETAIL,
  SN_IDC_OUI_ID_DETAIL,
  SN_IDC_LOADER_VERSION_DETAIL,
  
  IDC_TV_WINDOW,
  IDC_AD_WINDOW,
  IDC_SECOND_TITLE,
  IDC_SMALL_BG,

  IDC_STBID_NAME,
  IDC_LOADER_VERSION_NAME,
  IDC_HW_VERSION_NAME,
  IDC_SW_VERSION_NAME,
  IDC_SW_DEMOD_NAME,
  IDC_SW_TUNER_NAME,

  
  IDC_STBID_DETAIL,
  IDC_LOADER_VERSION_DETAIL,
  IDC_HW_VERSION_DETAIL,
  IDC_SW_VERSION_DETAIL,
  IDC_STBID_DEMOD,
  IDC_STBID_TUNER,

};
u16 sys_info_cont_keymap(u16 key);

RET_CODE sys_info_cont_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);



RET_CODE open_sys_info(u32 para1, u32 para2)
{

  #define SN_RELEASE_TIME	       __DATE__
  #define SN_SYS_INFO_W            640
  #define SN_SYS_INFO_H            480               
  #define SN_SYS_INFO_ITEM_H	    30
  #define SN_SYS_INFO_ITEM_X	    80//48        //lxd change to "80"
  #define SN_SYS_INFO_ITEM_Y	    40//40       //lxd change to "40"
  #define SN_SYS_INFO_ITEM_NAME_W	160
  control_t *p_cont,*p_ctrl;
  u16 i,x,y;
  u16 unistr[64];
  u8 ansstr[64] = {0};
  
  u16 str[]={IDS_HARD_VER2, IDS_SOFT_VER2, IDS_SYS_INFO_ISSUE_TIME,IDS_OUI_VER,IDS_LOADER_VER};

  misc_options_t misc;
  u32 read_len = 0;
  sw_info_t sw_info;
  struct nim_device_t* p_nim = NULL ;
  u32 version_dm = 0;
  u32  version_tn = 0;
  u8 changeset[MAX_CHANGESET_LEN]; 
  
  p_nim = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_NIM);
  dev_io_ctrl(p_nim, NIM_IOCTRL_GET_TN_VERSION, (u32)&version_tn);
  dev_io_ctrl(p_nim, NIM_IOCTRL_GET_DMD_VERSION, (u32)&version_dm);

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0, 
                               sizeof(misc_options_t), (u8 *)&misc);

  sys_status_get_sw_info(&sw_info);
  memset(&changeset,0,sizeof(u8) * MAX_CHANGESET_LEN);
  sys_status_get_sw_changeset(changeset);
  OS_PRINTF("----------------------changeset=%s----------------------",changeset);
  // create container 
  p_cont = ui_background_create(ROOT_ID_SYS_INFO,0, 0,
                             SN_SYS_INFO_W, SN_SYS_INFO_H,
                             IDS_SYSTEM_INFO,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);
  //ctrl_set_style(p_cont, STL_EX_WHOLE_HL);


  //ctrl
  x = SN_SYS_INFO_ITEM_X;
  y = SN_SYS_INFO_ITEM_Y;

  for(i=0; i<5; i++)
  {

    //title
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_HW_VERSION_NAME+i),
                              x,
                              y + i*(SYS_INFO_ITEM_H + SYS_INFO_ITEM_VGAP),
                              SYS_INFO_ITEM_NAME_W+40, SYS_INFO_ITEM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, str[i]);

    //data
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_HW_VERSION_NAME+i+5),
                              x +SN_SYS_INFO_ITEM_NAME_W+50+40, //LXD ADD "+50"
                              y + i*(SYS_INFO_ITEM_H + SYS_INFO_ITEM_VGAP),
                              SYS_INFO_ITEM_DETAIL_W+90, SYS_INFO_ITEM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_ctrl, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    switch(i)
    {
     case 0:
        memset(ansstr,0,sizeof(ansstr));
        sys_status_get_all_hw_version(ansstr,sizeof(ansstr));
        str_asc2uni(ansstr, unistr);
        break;
      case 1:
        memset(ansstr,0,sizeof(ansstr));
        sys_status_get_all_sw_version(ansstr,sizeof(ansstr));
		OS_PRINTF("----------------------ansstr=%s----------------------",ansstr);
		DEBUG(MAIN,INFO,"-----------------------ansstr=%s------------------------\n",ansstr);
        str_asc2uni(ansstr, unistr);
        break;
      case 2:
        str_asc2uni((u8 *)SN_RELEASE_TIME, unistr);
    	OS_PRINTF("----------------------ansstr----------------------");
        break;
	  case 3:
		memset(ansstr,0,sizeof(ansstr));
	  	sys_status_get_all_oui_version(ansstr,sizeof(ansstr));
		OS_PRINTF("----------------------ansstr=%s----------------------",ansstr);
        str_asc2uni(ansstr, unistr);
        break;
	  case 4:
        memset(ansstr,0,sizeof(ansstr));
	  	sys_status_get_all_boot_version_info(ansstr,sizeof(ansstr));
		OS_PRINTF("----------------------ansstr=%s----------------------",ansstr);
        str_asc2uni(ansstr, unistr);
        break;
          
      default:
        str_asc2uni((u8 *)"unknown", unistr);
        break;
    }
    text_set_content_by_unistr(p_ctrl, unistr);
  }


#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

//  ui_comm_help_create(&sys_info_help_data, p_cont);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(sys_info_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(sys_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(sys_info_cont_proc, ui_comm_root_proc)
END_MSGPROC(sys_info_cont_proc, ui_comm_root_proc)


