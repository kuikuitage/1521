/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_channel_edit.h"
#include "ui_channel_numb.h"
#include "ui_channel_save.h"
#include "ui_comm_root.h"

enum control_id
{
  IDC_CHAN_ID = 1,
  IDC_CHAN_HEAD,
  IDC_CHAN_NUM,
  IDC_CHAN_NAME,
  IDC_CHAN_HIDE,
  IDC_CHAN_F3,
  IDC_CHAN_FREQ,
  IDC_CHAN_F2,
  IDC_CHAN_FAV,
  IDC_CHAN_F1,
  IDC_CHAN_ORDER,
  IDC_CHAN_F4,
  IDC_CHAN_BAR,
  IDC_CHAN_PRO,

  IDC_CHAN_LINE,
  IDC_CHAN_LITTLE,
  IDC_CHAN_BAR_LITTLE_ARROWU,
  IDC_CHAN_BAR_LITTLE_ARROWD,
  IDC_CHAN_BAR_LITTLE_ARROWL,
  IDC_CHAN_BAR_LITTLE_ARROWR,

};

enum prv_msg_t
{
  MSG_FAV_SET = MSG_USER_BEGIN + 200,
  MSG_LOCK_SET,
  MSG_HIDE_SET,
  MSG_EXCH_SET,
  MSG_UPDATE_LIST,
};
enum menu_state_t
{
  MS_NOR = 1,
  MS_MOVE,
};

static u16 total = 0;
static u16 g_usProgPos = INVALIDPOS;
static u16 g_usProgid = INVALIDID;
static dvbs_view_t g_usViewtype = DB_DVBS_INVALID_VIEW;
static BOOL g_bProgLock = FALSE;
static BOOL is_tv = FALSE;
static BOOL is_modify = FALSE;
static BOOL is_give_up = FALSE;
static BOOL is_exit_flag = FALSE;
static BOOL is_switch_pg = FALSE;
static BOOL is_ok_flag = FALSE;

static RET_CODE chl_edit_list_update(control_t* ctrl, u16 start, u16 size, u32 context);
extern void ui_restore_view(void);
extern void ui_cache_view(void);
u16 channel_edit_cont_keymap(u16 key);
RET_CODE channel_edit_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 channel_edit_list_keymap(u16 key);
RET_CODE channel_edit_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 channel_edit_pwd_keymap(u16 key);
RET_CODE channel_edit_pwd_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE exchan_channel_num(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  channel_numb_data_t numb_data_t =
  {
    ROOT_ID_CHANNEL_EDIT,
    SN_NUMB_EXCHAN_X,
    SN_NUMB_EXCHAN_Y,
    IDS_INPUT_NEW_CHANNEL,
  };
  ui_channel_numb_open(&numb_data_t,total);
  return SUCCESS;
}


static BOOL channel_edit_get_next_pgid(u8 v_id,u16 pos,u16 *des_pgid,u8 dir)
{
   u16 cnt,pg_id,start_pos;
   db_dvbs_ret_t db_ret = DB_DVBS_OK;
   dvbs_prog_node_t  sProgNode = {0};
   BOOL ret = FALSE;
   cnt  = db_dvbs_get_count(v_id);
   if(dir)
   {
       start_pos = pos+1;//ignore current pg
       while(start_pos < cnt)
      {
          pg_id  = db_dvbs_get_id_by_view_pos(v_id,start_pos);
          db_ret  = db_dvbs_get_pg_by_id(pg_id,&sProgNode);
          if(DB_DVBS_OK != db_ret)
          {
            break;
          }
         if(sProgNode.skp_flag != 1)
         {
            *des_pgid =  pg_id;
            ret = TRUE;
            break;
          }
         start_pos++;
        }
    }
   else
   {
      start_pos = 0;
      while(start_pos < pos)
      {
          pg_id  = db_dvbs_get_id_by_view_pos(v_id,start_pos);
          db_ret  = db_dvbs_get_pg_by_id(pg_id,&sProgNode);
          if(DB_DVBS_OK != db_ret)
          {
            break;
          }
         if(sProgNode.skp_flag != 1)
         {
            *des_pgid =  pg_id;
            ret = TRUE;
            break;
          }
         start_pos ++;
      }
  }
 return ret ;
}
static u16 chaannel_edit_get_new_pgid(dvbs_view_t type,u16 pg_id)
{
  u16 cnt,pos,des_pgid;
  u8 view_id;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t  sProgNode = {0};
  BOOL ret ;
  if((type != DB_DVBS_ALL_TV)&&(type != DB_DVBS_TV_RADIO))
  {
    return sys_status_get_curn_group_curn_prog_id();   
  }
  view_id = db_dvbs_create_view(type, 0, NULL);
  cnt =db_dvbs_get_count(view_id);
  if(cnt == 0)
  {
    return sys_status_get_curn_group_curn_prog_id();  
  }
  if(type == DB_DVBS_ALL_TV)
  {
    view_id  = db_dvbs_create_view(DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG,0,NULL);
   }
  else
  {
    view_id  = db_dvbs_create_view(DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG,0,NULL);
   }
   db_ret  = db_dvbs_get_pg_by_id(pg_id,&sProgNode);
   if((DB_DVBS_OK != db_ret) || (sProgNode.skp_flag == 0))
   {
      return sys_status_get_curn_group_curn_prog_id();
   }
  pos = db_dvbs_get_view_pos_by_id(view_id,pg_id);
 
  if(pos == 0)
  {
    ret =  channel_edit_get_next_pgid(view_id,pos,&des_pgid,1);
    if(ret == TRUE)
    {
        return des_pgid;
    }
    else
    {
        return sys_status_get_curn_group_curn_prog_id();
     }
  }
  else
  {
    ret =  channel_edit_get_next_pgid(view_id,pos,&des_pgid,1);
    if(ret == TRUE)
    {
       return des_pgid;  
    }
    ret =  channel_edit_get_next_pgid(view_id,pos,&des_pgid,0);
     if(ret == TRUE)
    {
       return des_pgid;  
    }
    else
    {
      return sys_status_get_curn_group_curn_prog_id();
    }
  }
}

/****************************************************
NAME:chan_edit_play_unhide_prog
FUNCTION: if the current prog is hide,play the unhide prog
                 and update the prog lock status
RETURN: success/failure
*****************************************************/
static RET_CODE chan_edit_play_unhide_prog(void)
{
  RET_CODE ret = SUCCESS;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  u16 usNewProgId = INVALIDID;
  u16 usProgPos = INVALIDPOS;
  u8 ucProgMode = CURN_MODE_NONE;
  dvbs_prog_node_t  sProgNode = {0};
  customer_cfg_t cfg = {0};
  
  get_customer_config(&cfg);
  sys_status_check_group_view(TRUE);

  ucProgMode = sys_status_get_curn_prog_mode();

  switch(ucProgMode)
  {
    case CURN_MODE_TV:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;
    case CURN_MODE_RADIO:
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
      break;
    case CURN_MODE_NONE:
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
      break;
    default:
      ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
      break;
  }
  if(CURN_MODE_NONE == ucProgMode)
  {
      return SUCCESS;
  }

  if(CUSTOMER_BOYUAN == cfg.customer || cfg.customer == CUSTOMER_ZHILING ||
    cfg.customer == CUSTOMER_ZHILING_KF || cfg.customer == CUSTOMER_ZHILING_LQ||
    CUSTOMER_SHENZHOU_QN == cfg.customer)  //fix 21464
  {
    usNewProgId = chaannel_edit_get_new_pgid(g_usViewtype,g_usProgid); 
  }
 else
 {
   usNewProgId = sys_status_get_curn_group_curn_prog_id(); 
  }
  db_ret = db_dvbs_get_pg_by_id(g_usProgid, &sProgNode);
  if(DB_DVBS_OK != db_ret)
  {
      ui_play_prog(usNewProgId, FALSE);
      return SUCCESS;
  }

  if(g_usProgid != usNewProgId) /*prog changed,play new prog*/
  {
    if(sProgNode.skp_flag) 
    {
      ui_play_prog(usNewProgId, FALSE);
      return SUCCESS;
    }
    else  //set back to orignal prog
    {
      usNewProgId = g_usProgid;  
      usProgPos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), usNewProgId);
      sys_status_set_curn_group_info(usNewProgId, usProgPos);
      sys_status_save();
    }
  }
  
  if(g_bProgLock != sProgNode.lck_flag) //lock status changed,update lock status 
  {
      ui_set_chk_pwd(usNewProgId, !sProgNode.lck_flag);
      if(sProgNode.lck_flag)
      {
        ui_stop_play(STOP_PLAY_BLACK, TRUE);
      }
      else
      {
        ui_play_prog(usNewProgId, TRUE);
      }
  }

  return ret;
}

static void chl_edit_undo_save_all(void)
{
  db_dvbs_undo(ui_dbase_get_pg_view_id());
}

static void channel_edit_set_save(void)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y + 100,
    IDS_INPUT_PASSWORD,
    1,
    channel_edit_pwd_keymap,
    channel_edit_pwd_proc,
  };
  ui_comm_pwdlg_open(&pwdlg_data);
}
  
static void channel_set_save(void)
{
  channel_edit_set_save();
  is_give_up = FALSE;
  return;
}

static void channel_set_no_save(void)
{
  chl_edit_undo_save_all();
  is_exit_flag = FALSE;
  is_modify = FALSE;
  is_give_up = TRUE;
  is_switch_pg = FALSE;
  return;
}

static RET_CODE on_channe_list_change_group(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 curn_mode = 0;
  u16 curn_group = 0;
  u16 pg_id, pg_pos = 0xFFFF;
  u32 group_context = 0;
  u8 view_id = 0xFF;
  u16 view_count = 0;
  
  if(is_modify)
  {
    is_switch_pg = TRUE;
    ui_comm_ask_for_dodlg_open(NULL, IDS_SAVING_OR_NOT, 
                           channel_set_save, channel_set_no_save, 0);
    if(is_give_up)
    {
      is_give_up = FALSE;
      chl_edit_list_update(p_list, list_get_valid_pos(p_list), CHAN_LIST_PAGE, 1);
      ctrl_paint_ctrl(p_list, TRUE);
    }
  }
  else
  {
    if(is_tv)
    {
      ui_dvbc_change_view(DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG, FALSE);
      is_tv = FALSE;
    }
    else
    {
      ui_dvbc_change_view(DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG, FALSE);
      is_tv = TRUE;
    }

    curn_mode = sys_status_get_curn_prog_mode();
    curn_group = sys_status_get_curn_group();
    sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &pg_pos, &group_context);
    view_id = ui_dbase_get_pg_view_id();
    view_count = db_dvbs_get_count(view_id);
    total = view_count;
    list_set_count(p_list, view_count, CHAN_LIST_PAGE);
    list_set_focus_pos(p_list, pg_pos);
    chl_edit_list_update(p_list, list_get_valid_pos(p_list), CHAN_LIST_PAGE, 1);
    ctrl_paint_ctrl(p_list, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_save_list_date(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  if(FALSE == is_modify)
  {
      return ERR_FAILURE;
  }
  is_ok_flag = TRUE;
  channel_edit_set_save();
  return SUCCESS;
}

static RET_CODE on_plist_numb_update(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u8 view_id;
  u16 pos = (u16)para1-1;
  u16 focus;
  
  is_modify = TRUE;
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
  
  db_dvbs_move_item_in_view(view_id,focus,pos);
 #if 0
  pg_id = db_dvbs_get_id_by_view_pos(view_id,pos);
  db_dvbs_get_pg_by_id(pg_id, &pg);
  db_dvbs_edit_program(&pg);
  list_set_focus_pos(p_list, pos);
//  db_dvbs_save(view_id);
#endif
  chl_edit_list_update(p_list, list_get_valid_pos(p_list), CHAN_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_channel_set_attr(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  u8 view_id;
  u16 focus;
  u8 field_idx = 0;
  u16 usMarkValue = 0;
  u16 usImvalue = 0;
  u16 mark[] = {0, 0, DB_DVBS_FAV_GRP, DB_DVBS_MARK_LCK, DB_DVBS_MARK_SKP};
  u16 icon[] =  {0, 0, IM_SN_ICON_LOVE, IM_SN_ICON_LOCK, IM_SN_ICON_DELETE};

  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
  switch(msg)
  {
      case MSG_FAV_SET:
        field_idx = 2;
        break;
      case MSG_LOCK_SET:
        field_idx = 3;
        break;
      case MSG_HIDE_SET:
        field_idx = 4;
        break;
      default:
        return ERR_FAILURE;
  }
  if(db_dvbs_get_mark_status(view_id, focus, mark[field_idx], 0))
  {
      usMarkValue = 0;
      usImvalue = 0;
  }
  else
  {
      usMarkValue = DB_DVBS_PARAM_ACTIVE_FLAG;
      usImvalue = icon[field_idx];
  }
  db_dvbs_change_mark_status(view_id, focus, mark[field_idx], usMarkValue);
  list_set_field_content_by_icon(p_list, focus, field_idx, usImvalue);
  list_draw_field_ext(p_list, focus, field_idx, TRUE);
  
  is_modify = TRUE;
  sys_status_set_status(BS_PROG_LOCK, TRUE);
  return SUCCESS;
}

static void channel_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, im_value[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_SN_ICON_LOVE : 0;//IM_ICON_FAVOURITE 
  im_value[1] =
   db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0)? IM_SN_ICON_LOCK : 0;//IM_ICON_LOCK 
  im_value[2] =
   db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, 0)? IM_SN_ICON_DELETE : 0;//IM_ICON_HIDE
  for (i = 0; i < 3; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), im_value[i]);
  }
}

static RET_CODE chl_edit_list_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 pg_id, cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[8];
  u16 uni_str[32];

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &pg);

      /* NO. */
      #ifdef LCN_SWITCH
      sprintf((char *)asc_str, "%4d ", pg.logic_ch_num);
      #else
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name

      /* MARKS */
      channel_set_list_item_mark(ctrl, (u16)(start + i), pg_id);
      if(pg_id == sys_status_get_curn_group_curn_prog_id())
      {
        list_select_item(ctrl, start + i);
      }
    }
  }

  return SUCCESS;
}
       
RET_CODE open_channel_edit(u32 para1, u32 para2)
{
  //head title cont
  #define SN_CHAN_HEAD_X              15//26
  #define SN_CHAN_HEAD_Y              10
  #define SN_CHAN_HEAD_W              605//605
  #define SN_CHAN_HEAD_H              31

  //LIST
  #define SN_CHAN_LIST_X               SN_CHAN_HEAD_X
  #define SN_CHAN_LIST_Y               ((SN_CHAN_HEAD_Y) + (SN_CHAN_HEAD_H) +10)//125
  #define SN_CHAN_LIST_W               SN_CHAN_HEAD_W
  #define SN_CHAN_LIST_H               280//340

  //list mid rect
  #define SN_CHAN_LIST_MIDL            4
  #define SN_CHAN_LIST_MIDT            5
  #define SN_CHAN_LIST_MIDW            (SN_CHAN_LIST_W - 8)//597//581
  #define SN_CHAN_LIST_MIDH            (SN_CHAN_LIST_H - 10)//300//330


  //bar little
  #define SN_CHAN_LITTLE_X                SN_CHAN_LIST_X
  #define SN_CHAN_LITTLE_Y                ((SN_CHAN_LIST_Y) + (SN_CHAN_LIST_H) + 10)
  #define SN_CHAN_LITTLE_W                SN_CHAN_LIST_W
  #define SN_CHAN_LITTLE_H                31

  //bar little arrow
  #define SN_CHAN_BAR_ARROW_UP_X         ((SN_CHAN_LIST_W)/2 - 30)
  #define SN_CHAN_BAR_ARROW_DOWN_X		 ((SN_CHAN_LIST_W)/2 - 15)
  #define SN_CHAN_BAR_ARROW_LEFT_X		 ((SN_CHAN_LIST_W)/2 + 15)
  #define SN_CHAN_BAR_ARROW_RIGHT_X        ((SN_CHAN_LIST_W)/2 + 30)
  #define SN_CHAN_BAR_ARROW_Y              (((SN_CHAN_LITTLE_H) - (SN_CHAN_BAR_ARROW_H))/2)
  #define SN_CHAN_BAR_ARROW_W              15
  #define SN_CHAN_BAR_ARROW_H              18

  //help bar
  #define SN_SUB_MENU_HELP_RSC_CNT         19
  

  control_t *p_cont, *p_title_cont,*p_ctrl, *p_list;//,*p_sbar, *p_line_H
  control_t *p_barctrl;
  u16 curn_group, curn_mode;
  u32 group_context;
  u16 i;
  u8 vid;
  customer_cfg_t cfg = {0};
  
  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 50 , IM_SN_F1},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_LOVE},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 50 , IM_SN_F2},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_LOCK},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 50 , IM_SN_F3},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 50 , IDS_HIDE},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 40 , IM_SN_F4},
	 {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_REORDER}  //LXD CHANGE 50 TO 60
  };
  
  list_xstyle_t chan_item_rstyle =
  {
      RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_N,
      RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_N,
      RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_HL,
      RSI_SN_BG,//RSI_SN_BAR_BLUE_NORMAL,//RSI_COMM_LIST_SEL,
      RSI_SN_BAR_YELLOW_HL,//RSI_COMM_LIST_N,
  };

  list_xstyle_t chan_field_fstyle =
  {
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_INDEX4,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
  };

  list_xstyle_t chan_field_rstyle =
  {
    RSI_SN_BAR_YELLOW_MIDDLE,//RSI_IGNORE,//RSI_WHITE_YELLOW,
    RSI_IGNORE,//RSI_SUB_BUTTON_SH,
    RSI_SN_BAR_YELLOW_MIDDLE,//RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };

  list_field_attr_t list_attr[CHAN_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR,
    50, 10/*0*/, 0, &chan_field_rstyle,  &chan_field_fstyle },

    { LISTFIELD_TYPE_UNISTR| STL_LEFT | STL_VCENTER,
    300, 70+30, 0, &chan_field_rstyle,  &chan_field_fstyle},

    { LISTFIELD_TYPE_ICON,
    50, 315, 0, &chan_field_rstyle,  &chan_field_fstyle},

    { LISTFIELD_TYPE_ICON,
    50, 385, 0, &chan_field_rstyle,  &chan_field_fstyle},

    { LISTFIELD_TYPE_ICON,
    CHAN_LIST_MIDW - 515, 455, 0, &chan_field_rstyle,  &chan_field_fstyle},
  };

  get_customer_config(&cfg);
  //global variable initialize
  is_give_up = FALSE;
  
  ui_cache_view();

  vid = ui_dbase_get_pg_view_id();
  g_usViewtype = db_dvbs_get_view_type(vid);
  switch(db_dvbs_get_view_type(vid))
  {
   case DB_DVBS_ALL_TV:
    ui_dvbc_change_view(DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG, FALSE);
    is_tv = TRUE;
    break;

   case DB_DVBS_ALL_RADIO:
    ui_dvbc_change_view(DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG, FALSE);
    is_tv = FALSE;
    break;
   default:
    ui_dvbc_change_view(DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG, FALSE);
    is_tv = TRUE;
    break;
  }
  vid = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(vid);

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &g_usProgid, &g_usProgPos, &group_context);
  g_bProgLock = db_dvbs_get_mark_status(vid, g_usProgPos, DB_DVBS_MARK_LCK, 0);


  p_cont = ui_background_create(ROOT_ID_CHANNEL_EDIT,
                             0, 0,
                             640,480,//SCREEN_WIDTH, SCREEN_HEIGHT,
                             IDS_PROGRAM_MANAGE,TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, channel_edit_cont_keymap);
  ctrl_set_proc(p_cont, channel_edit_cont_proc);
  
  //head title cont
  p_title_cont = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_HEAD,
                           SN_CHAN_HEAD_X, SN_CHAN_HEAD_Y, SN_CHAN_HEAD_W,SN_CHAN_HEAD_H, p_cont, 0);
  ctrl_set_rstyle(p_title_cont, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_SECOND_TITLE

  //head title:numberm
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_NUM,
                           CHAN_NUMB_X+10, CHAN_NUMB_Y, CHAN_NUMB_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_NUMB_H
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CHANNEL);//IDS_ID

  //head title:name
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_NAME,
                         CHAN_NAME_X+20, CHAN_NAME_Y, CHAN_NAME_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_NAME_H
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_PRO_WF);//IDS_CHANNEL_NAME
  
  //head title:order
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_ORDER,
                           CHAN_ORDER_X, CHAN_ORDER_Y, CHAN_ORDER_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_ORDER_H
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_REORDER);
  
  //head title:f4
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_F4,
                            CHAN_FORDER_X, CHAN_FORDER_Y,
                            CHAN_FORDER_W, SN_CHAN_HEAD_H,//CHAN_FORDER_H
                            p_title_cont, 0);
  switch(cfg.customer)
  {
    case CUSTOMER_ZHONGDA:
    case CUSTOMER_HAIHUIDA:
    case CUSTOMER_FANTONG:
    case CUSTOMER_FANTONG_KF:
    case CUSTOMER_FANTONG_KFAJX:
    case CUSTOMER_FANTONG_KF_SZXC312:
    case CUSTOMER_FANTONG_BYAJX:
    case CUSTOMER_FANTONG_XSMAJX:
      break;
    default:
      break;
  }

  //head title:hid
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_HIDE,
                           CHAN_HIDE_X, CHAN_HIDE_Y, CHAN_HIDE_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_HIDE_H
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_HIDE);
  //head title:f3
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_F3,
                            CHAN_FHIDE_X, CHAN_FHIDE_Y,
                            CHAN_FHIDE_W, SN_CHAN_HEAD_H,//CHAN_FHIDE_H
                            p_title_cont, 0);
  switch(cfg.customer)
  {
    case CUSTOMER_XINSHIDA:
    case CUSTOMER_XINSIDA_LQ:
    case CUSTOMER_CHANGJIANG:
    case CUSTOMER_CHANGJIANG_LQ:
    case CUSTOMER_CHANGJIANG_NY:
    case CUSTOMER_CHANGJIANG_QY:
    case CUSTOMER_CHANGJIANG_JS:
    case CUSTOMER_XINSIDA_SPAISH:
    case CUSTOMER_HEBI:
      break;
    case CUSTOMER_ZHONGDA:
    case CUSTOMER_HAIHUIDA: 
    case CUSTOMER_FANTONG:
    case CUSTOMER_FANTONG_KF:
    case CUSTOMER_FANTONG_KFAJX:
    case CUSTOMER_FANTONG_KF_SZXC312:
    case CUSTOMER_FANTONG_BYAJX:
    case CUSTOMER_FANTONG_XSMAJX:
      break;
    default:
      break;
  }

  //head title:lock
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_FREQ,
                           CHAN_FREQ_X, CHAN_FREQ_Y, CHAN_FREQ_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_FREQ_H

  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
   text_set_content_by_strid(p_ctrl, IDS_LOCK);

  //head title:f2
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_F2,
                          CHAN_FFREQ_X, CHAN_FFREQ_Y,
                          CHAN_FFREQ_W, SN_CHAN_HEAD_H,//CHAN_FFREQ_H
                          p_title_cont, 0);
  if((CUSTOMER_XINSHIDA == cfg.customer) 
      || (CUSTOMER_XINSIDA_LQ == cfg.customer) 
      || (CUSTOMER_CHANGJIANG == cfg.customer)
      || (CUSTOMER_CHANGJIANG_LQ == cfg.customer)
      || (CUSTOMER_CHANGJIANG_NY== cfg.customer)
      || (CUSTOMER_CHANGJIANG_QY== cfg.customer)
      || (CUSTOMER_CHANGJIANG_JS == cfg.customer)
      || (CUSTOMER_XINSIDA_SPAISH == cfg.customer)
      || (CUSTOMER_HEBI== cfg.customer))
  {
    //bmap_set_content_by_id(p_ctrl, IM_F2_2);
  }
  else
  {
    //bmap_set_content_by_id(p_ctrl, IM_F2);
  }
  //head title:fav
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_CHAN_FAV,
                         CHAN_FAV_X, CHAN_FAV_Y, CHAN_FAV_W,SN_CHAN_HEAD_H, p_title_cont, 0);//CHAN_FAV_H
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_CHAN_NUMB,FSI_CHAN_NUMB,FSI_CHAN_NUMB);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_LOVE);

  //head title:f1
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_F1,
                          CHAN_FFAV_X, CHAN_FFAV_Y,
                          CHAN_FFAV_W, SN_CHAN_HEAD_H,//CHAN_FFAV_H
                          p_title_cont, 0);
 // bmap_set_content_by_id(p_ctrl, IM_F1);
  
  
  //LIST
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CHAN_PRO,
                           SN_CHAN_LIST_X, SN_CHAN_LIST_Y, SN_CHAN_LIST_W,SN_CHAN_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);//RSI_TIP_BOX
  ctrl_set_keymap(p_list, channel_edit_list_keymap);
  ctrl_set_proc(p_list, channel_edit_list_proc);

  list_set_mid_rect(p_list, SN_CHAN_LIST_MIDL, SN_CHAN_LIST_MIDT,
           SN_CHAN_LIST_MIDW, SN_CHAN_LIST_MIDH,2);//CHAN_LIST_VGAP
  list_set_item_rstyle(p_list, &chan_item_rstyle);
  list_set_count(p_list, total, CHAN_LIST_PAGE);//CHAN_LIST_PAGE

  list_set_field_count(p_list, CHAN_LIST_FIELD, CHAN_LIST_PAGE);//CHAN_LIST_PAGE
  list_set_focus_pos(p_list, g_usProgPos);
  list_set_update(p_list, chl_edit_list_update, 0);
  for (i = 0; i < CHAN_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(list_attr[i].attr), (u16)(list_attr[i].width),
                        (u16)(list_attr[i].left), (u8)(list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, list_attr[i].fstyle);
  }


  //bar little
  p_ctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_LITTLE,
                               SN_CHAN_LITTLE_X, SN_CHAN_LITTLE_Y,
                               SN_CHAN_LITTLE_W, SN_CHAN_LITTLE_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);  

  //bar little arrow
  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_BAR_LITTLE_ARROWU,
                            SN_CHAN_BAR_ARROW_UP_X, SN_CHAN_BAR_ARROW_Y,
                            SN_CHAN_BAR_ARROW_W, SN_CHAN_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_UP);

  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_BAR_LITTLE_ARROWD,
                            SN_CHAN_BAR_ARROW_DOWN_X, SN_CHAN_BAR_ARROW_Y,
                            SN_CHAN_BAR_ARROW_W, SN_CHAN_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_DOWN);
  
  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_BAR_LITTLE_ARROWL,
                            SN_CHAN_BAR_ARROW_LEFT_X, SN_CHAN_BAR_ARROW_Y,
                            SN_CHAN_BAR_ARROW_W, SN_CHAN_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_LEFT);

  p_barctrl = ctrl_create_ctrl((u8 *)CTRL_BMAP, IDC_CHAN_BAR_LITTLE_ARROWR,
                            SN_CHAN_BAR_ARROW_RIGHT_X, SN_CHAN_BAR_ARROW_Y,
                            SN_CHAN_BAR_ARROW_W, SN_CHAN_BAR_ARROW_H,
                            p_ctrl, 0);
  bmap_set_content_by_id(p_barctrl, IM_SN_ICON_ARROW_RIGHT);
  
  chl_edit_list_update(p_list, list_get_valid_pos(p_list), CHAN_LIST_PAGE, 0);
  


  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=47;
  sn_submenu_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);


  
  return SUCCESS;
}

static RET_CODE on_channel_edit_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  dlg_ret_t ret=FALSE;
  comm_dialog_data_t dialog;
    dialog.grade = DIALOG_INFO;
	dialog.x      = 180;
	dialog.y      = 180;
	dialog.w     = 300;
	dialog.h     = 150;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID   = IDS_SAVING_OR_NOT;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;

   if(is_modify)
   {
     ret=ui_comm_dialog_open(&dialog);
     if(DLG_RET_YES==ret)
     {
       manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
      
       if(is_give_up)
       {
         channel_set_save();
         is_give_up = FALSE;
	  
       }
	   channel_edit_set_save();
  }
  else
  {
    channel_set_no_save();
    ui_restore_view();
    chan_edit_play_unhide_prog();
    manage_close_menu(ROOT_ID_CHANNEL_EDIT,0,0);
  }
    
    ret = FALSE;
  }
  else
  {
    ui_restore_view();
    chan_edit_play_unhide_prog();
    
    manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
  }
	
  return SUCCESS;
}

static RET_CODE on_channel_edit_exit_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  dlg_ret_t ret=FALSE;
  comm_dialog_data_t dialog;

    dialog.grade = DIALOG_INFO;
	dialog.x      = 180;
	dialog.y      = 180;
	dialog.w     = 300;
	dialog.h     = 150;
	dialog.parent_root = 0;
	dialog.icon_id = 0;
	dialog.string_mode = STR_MODE_STATIC;
	dialog.text_strID   = IDS_SAVING_OR_NOT;
	dialog.text_content = 0;
	dialog.type   = DLG_FOR_ASK;
	dialog.timeout = 100000;
  is_exit_flag = TRUE; //EXIT KEY
  if(is_modify)
  {
    ret=ui_comm_dialog_open(&dialog);
    if(DLG_RET_YES==ret)
    {
	  manage_close_menu(ROOT_ID_CHANNEL_WARN, 0 ,0);
      if(is_give_up)
      {
        channel_set_save();
        is_give_up = FALSE;
      }
	  channel_edit_set_save();
	 
    }
    else
    {
 	  channel_set_no_save();
      ui_restore_view();
      chan_edit_play_unhide_prog();
      manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0 ,0);
    }

    ret = FALSE;
  }
  else
  {
    ui_restore_view();
    chan_edit_play_unhide_prog();
	manage_close_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE on_channel_edit_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  chl_edit_undo_save_all();
  //ui_restore_view();//fix bug 22179
  ui_comm_pwdlg_close();
  is_modify = FALSE;
  is_exit_flag = FALSE;
  is_ok_flag = FALSE;
  return SUCCESS;
}

static RET_CODE on_channel_edit_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  u8 view_id;
  
  is_modify = FALSE;
   if(is_switch_pg)
  {
    fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_MSG, FALSE,
                      MSG_UPDATE_LIST, 0, 0);
  }
  ui_comm_pwdlg_close();

  view_id = ui_dbase_get_pg_view_id();
  db_dvbs_save(view_id);
  if(!is_switch_pg)
  {
    if(is_ok_flag)
    {
      is_ok_flag = FALSE;
      is_switch_pg = FALSE;
      return SUCCESS;
    }
    ui_restore_view();
    chan_edit_play_unhide_prog();
    if(is_exit_flag)
    {
      ui_close_all_mennus();
      is_exit_flag = FALSE;
    }
    else
    manage_close_menu(ROOT_ID_CHANNEL_EDIT,0,0);
  }
  is_switch_pg = FALSE;
  return SUCCESS;
}


BEGIN_KEYMAP(channel_edit_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(channel_edit_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(channel_edit_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_channel_edit_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_channel_edit_exit_all)
END_MSGPROC(channel_edit_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(channel_edit_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)    
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_F1, MSG_FAV_SET)
  ON_EVENT(V_KEY_RED, MSG_FAV_SET)
#ifdef CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_F2, MSG_LOCK_SET)
  ON_EVENT(V_KEY_GREEN, MSG_LOCK_SET)
  ON_EVENT(V_KEY_F3, MSG_HIDE_SET)
  ON_EVENT(V_KEY_YELLOW, MSG_HIDE_SET)
  ON_EVENT(V_KEY_F4, MSG_EXCH_SET)
  ON_EVENT(V_KEY_BLUE, MSG_EXCH_SET)
 #else
  ON_EVENT(V_KEY_F2, MSG_LOCK_SET)
  ON_EVENT(V_KEY_YELLOW, MSG_LOCK_SET)
  ON_EVENT(V_KEY_F3, MSG_HIDE_SET)
  ON_EVENT(V_KEY_GREEN, MSG_HIDE_SET)
  ON_EVENT(V_KEY_F4, MSG_EXCH_SET)
  ON_EVENT(V_KEY_BLUE, MSG_EXCH_SET)
#endif 

  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(channel_edit_list_keymap, NULL)

BEGIN_MSGPROC(channel_edit_list_proc, list_class_proc)
  ON_COMMAND(MSG_FAV_SET, on_channel_set_attr)
  ON_COMMAND(MSG_LOCK_SET, on_channel_set_attr)
  ON_COMMAND(MSG_HIDE_SET, on_channel_set_attr)
  ON_COMMAND(MSG_EXCH_SET, exchan_channel_num)
  ON_COMMAND(MSG_FOCUS_LEFT, on_channe_list_change_group)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_channe_list_change_group)
  ON_COMMAND(MSG_UPDATE_LIST, on_channe_list_change_group)
  ON_COMMAND(MSG_NUM_SELECT, on_plist_numb_update)
  ON_COMMAND(MSG_SELECT, on_save_list_date)
END_MSGPROC(channel_edit_list_proc, list_class_proc)

BEGIN_KEYMAP(channel_edit_pwd_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(channel_edit_pwd_keymap, NULL)

BEGIN_MSGPROC(channel_edit_pwd_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_channel_edit_pwdlg_correct)
  ON_COMMAND(MSG_PRESET_PROG, on_channel_edit_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_channel_edit_pwdlg_exit)
END_MSGPROC(channel_edit_pwd_proc, cont_class_proc)

