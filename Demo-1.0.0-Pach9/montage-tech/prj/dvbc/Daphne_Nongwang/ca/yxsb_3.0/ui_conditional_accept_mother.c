/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_conditional_accept_mother.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_CA,
};

enum ctrl_id
{
  IDC_TITLE1 = 1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
   IDC_TITLE5,
  IDC_CONDITIONAL_ACCEPT_MOTHER_LIST,
  IDC_CONDITIONAL_ACCEPT_MOTHER_LIST_SBAR,
};

static list_xstyle_t mother_list_item_rstyle =
{
/*QQ
    RSI_COMM_LIST_N,
    RSI_COMM_LIST_N,
    RSI_COMM_LIST_HL,
    RSI_COMM_LIST_SEL,
    RSI_COMM_LIST_N,
   */
    RSI_SN_BG,
    RSI_SN_BG,
    RSI_SN_BAR_YELLOW_HL,
    RSI_SN_BAR_YELLOW_HL,
    RSI_SN_BG,


};

static list_xstyle_t mother_list_field_fstyle =
{
/*	FSI_LIST_TXT_G,
	FSI_LIST_TXT_N,
	FSI_LIST_TXT_HL,
	FSI_LIST_TXT_SEL,
	FSI_LIST_TXT_HL,
*/

    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_TXT_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_TXT_HL,
};

static list_xstyle_t mother_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t mother_list_attr[CONDITIONAL_ACCEPT_MOTHER_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_DEC | STL_LEFT | STL_VCENTER,
    115, 30, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    60, 125, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle },
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    80, 185, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
  { LISTFIELD_TYPE_DEC | STL_CENTER | STL_VCENTER,
    60, 265, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
   { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    100, 325, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
};

static cas_operators_info_t *p_operator = NULL;
static chlid_card_status_info p_child_info[4] = {{0},};
static u8 child_info_count = 0;

comm_dlg_data_t dlg_data = 
{
  ROOT_ID_INVALID,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  DLG_STR_MODE_EXTSTR,
  (SCREEN_WIDTH - COMM_DLG_W)/2, (SCREEN_HEIGHT- COMM_DLG_H)/2 - 40,
  COMM_DLG_W, COMM_DLG_H + 40,
  IDS_CARD_MANAGE,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  0,
};

u16 conditional_accept_mother_cont_keymap(u16 key);
RET_CODE conditional_accept_mother_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 mother_list_keymap(u16 key);
RET_CODE mother_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE mon_child_info_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 uni_str[32];
  u8 asc_str[64];
  u16 cnt = list_get_count(p_list);

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      // id 
      list_set_field_content_by_dec(p_list, (u16)(start + i), 0, p_child_info[i].operator_id); 

      // state 
      switch(p_child_info[i].is_child)
      {
        case 0:
          str_asc2uni((u8 *)"monther", uni_str);
          break;
        case 1:
          str_asc2uni((u8 *)"child", uni_str);
          break;
        default:
          str_asc2uni((u8 *)"common", uni_str);
          break;
      }
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str);

      UI_PRINTF("@@@@@ parent_card_sn == %s\n", p_child_info[i].parent_card_sn);
      //mother card sn;
      memset(asc_str, 0, sizeof(asc_str));
      if (p_child_info[i].is_child == 1)
      {
        sprintf((char*)asc_str,"%s", p_child_info[i].parent_card_sn);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str);
      }
      memset(asc_str, 0, sizeof(asc_str));
        
      UI_PRINTF("@@@@@ is_child = %d, delay_time = %d, is_can_feed = %d, last_feed_time:%d-%d-%d %d-%d-%d\n",
                                          p_child_info[i].is_child,
                                          p_child_info[i].delay_time,
                                          p_child_info[i].is_can_feed,
                                          p_child_info[i].last_feed_time[0] * 100 + p_child_info[i].last_feed_time[1],
                                          p_child_info[i].last_feed_time[2],
                                          p_child_info[i].last_feed_time[3],
                                          p_child_info[i].last_feed_time[4],
                                          p_child_info[i].last_feed_time[5],
                                          p_child_info[i].last_feed_time[6]);
      //interval time
      if (p_child_info[i].is_child == 1)
      {
        list_set_field_content_by_dec(p_list, (u16)(start + i), 3, p_child_info[i].delay_time); 
        UI_PRINTF("delay time:%d\n", p_child_info[i].delay_time);
        
        //last time
        memset(asc_str, 0, sizeof(asc_str));
        sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",
                    p_child_info[i].last_feed_time[0] * 100 + p_child_info[i].last_feed_time[1],
                    p_child_info[i].last_feed_time[2],
                    p_child_info[i].last_feed_time[3],
                    p_child_info[i].last_feed_time[4],
                    p_child_info[i].last_feed_time[5],
                    p_child_info[i].last_feed_time[6]);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 4, asc_str);
      }      
    }
  }
  return SUCCESS;
}

static RET_CODE on_operate_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u32 i =0;
  u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  UI_PRINTF("@@@@@ on_operate_info curn_root_id:%d,p_operator->max_num == %d\n", curn_root_id, p_operator->max_num);
  if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
  {
    p_operator = (cas_operators_info_t *)para2;
    if(p_operator->max_num > 0)
    {    
     for(i = 0; i < p_operator->max_num; i++)
      {
        ui_ca_get_info(CAS_CMD_MON_CHILD_STATUS_GET, p_operator->p_operator_info[i].operator_id, 0);
      }
    }
  }

  return SUCCESS;
}
/*
static RET_CODE on_conditional_accept_feed_btn_f1(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 cur_pos = 0;
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u16 str_text[64] = {0,};
  cur_pos = list_get_focus_pos(p_ctrl);
  if(p_child_info[cur_pos].is_child == 0)
  {
    OS_PRINTF("card type is_monther!\n");
    ui_ca_get_info(CAS_CMD_READ_FEED_DATA, p_child_info[cur_pos].operator_id,0);
  }
  else if(p_child_info[cur_pos].is_child == 1)
  {
    OS_PRINTF("card type is_child!\n");
    if(p_child_info[cur_pos].is_can_feed)
    {
      gui_get_string(IDS_CA_ADDRES_INFO, str_text, 64); //QQ  IDS_CA_INSERT_PARENT_CARD
    }
    else
    {
      gui_get_string(IDS_CA_ADDRES_INFO, str_text, 64);//QQ  IDS_CA_NOT_TIME_FEED
    }
    dlg_data.text_content = (u32)str_text;
    dlg_ret = ui_comm_dlg_open(&dlg_data);
    if(dlg_ret == DLG_RET_YES)
    {
      if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
      {
        ui_comm_dlg_close();
      }
      UI_PRINTF("@@@@@ on_conditional_accept_feed_btn_select get feed data, cur_pos == %d, p_child_info[cur_pos].operator_id == %d,is_can_feed == %d!!!\n",
                                            cur_pos,p_child_info[cur_pos].operator_id, p_child_info[cur_pos].is_can_feed);

      if(p_child_info[cur_pos].is_can_feed)
      {
        ui_ca_get_info(CAS_CMD_READ_FEED_DATA, p_child_info[cur_pos].operator_id,0);
      }
    }
  }
  else
  {
    OS_PRINTF("ERROR card type!\n");
  }
  return SUCCESS;
}
*/
static RET_CODE on_mon_child_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  //RET_CODE ret = para1;
  //u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  UI_PRINTF("@@@@@child_info_count ==%d, p_operator->max_num == %d\n", child_info_count, p_operator->max_num);
  //if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
  //p_child_info = (chlid_card_status_info *)para2;
  memcpy(&p_child_info[child_info_count], (chlid_card_status_info *)para2, sizeof(chlid_card_status_info));
  child_info_count++;

  if(child_info_count == p_operator->max_num)
  {
    list_set_count(p_ctrl, child_info_count, CONDITIONAL_ACCEPT_MOTHER_LIST_PAGE);
    list_set_focus_pos(p_ctrl, 0);
    mon_child_info_update(p_ctrl, 0, child_info_count, 0);
    ctrl_paint_ctrl(ctrl_get_parent(p_ctrl), TRUE);
  }
  
  return SUCCESS;
}

static RET_CODE on_mon_child_feed_data(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = para1;
  u16 cur_pos = 0;
  u16 str_text[64] = {0,};
  chlid_card_status_info *mother_card = (chlid_card_status_info *)para2;
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  
  cur_pos = list_get_focus_pos(p_ctrl);
  UI_PRINTF("@@@@@ on_mon_child_feed_data curn_root_id:%d, cur_pos : %d\n", curn_root_id, cur_pos);
  if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
  {
    if(ret != SUCCESS)
    {
      gui_get_string(IDS_CA_ADDRES_INFO, str_text, 64);//QQ  IDS_CA_INVALID_PARENT_CARD
      dlg_data.text_content = (u32)str_text;
      dlg_ret = ui_comm_dlg_open(&dlg_data);
      if(dlg_ret == DLG_RET_YES)
      {
      /*QQ 
        if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
        {
          ui_comm_dlg_close();
        }
        */
        if(p_child_info[cur_pos].is_child == 1)
        {
          ui_ca_get_info(CAS_CMD_READ_FEED_DATA, p_child_info[cur_pos].operator_id,0);
        }
      }
      return ret;
    }
    else
    {
      gui_get_string(IDS_CA_ADDRES_INFO, str_text, 64); //QQ   IDS_CA_INSERT_CHILD_CARD
      dlg_data.text_content = (u32)str_text;
      dlg_ret = ui_comm_dlg_open(&dlg_data);
      if(dlg_ret == DLG_RET_YES)
      {
        ui_ca_get_info(CAS_CMD_MON_CHILD_FEED, (u32)mother_card, 0);
        /*QQ
		if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
            {
                ui_comm_dlg_close();
            }
              */
      }
    }
  }
  return SUCCESS;
}

static RET_CODE on_mon_child_feed_result(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = para1;
  dlg_ret_t dlg_ret = 0;
  u16 str_text[64] = {0,};
  u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  UI_PRINTF("@@@@@ on_mon_child_feed_result curn_root_id:%d\n", curn_root_id);
  if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
  {
    if(ret != SUCCESS)
    { 
      gui_get_string(IDS_BACK, str_text, 64);//QQ  IDS_FEED_FAILURE
      dlg_data.text_content = (u32)str_text;
      if(ret == CAS_E_FEEDTIME_NOT_ARRIVE)
      {
        gui_get_string(IDS_CA_ADDRES_INFO, str_text, 64);//QQ  IDS_CA_NOT_TIME_FEED
        dlg_data.text_content = (u32)str_text;
      }
    }
    else
    {
      gui_get_string(IDS_FEED_SUCCESS, str_text, 64);
      dlg_data.text_content = (u32)str_text;
    }
    dlg_ret = ui_comm_dlg_open(&dlg_data);
    if(dlg_ret == DLG_RET_YES)
    {
   /*QQ    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
    		  {
        		ui_comm_dlg_close();
      		}*/
    }
  }
  return ret;
}

static RET_CODE on_mon_child_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  child_info_count = 0;
  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, 0, 0);
  return SUCCESS;
}

static RET_CODE on_mon_child_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  child_info_count = 0;
  ui_close_all_mennus();
  return SUCCESS;
}

RET_CODE open_conditional_accept_mother(u32 para1, u32 para2)
{
#define SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT 15

	control_t *p_cont,*p_list, *p_ctrl;//, *p_sbar
	control_t *p_ca_frm;
	u8 i=0;

	u16 x, title_width[CONDITIONAL_ACCEPT_MOTHER_TITLE_CNT] = {110, 120, 110, 130,130};
	u16 stxt_title[CONDITIONAL_ACCEPT_MOTHER_TITLE_CNT] =
	{ 
		///QQ   IDS_SP_ID, IDS_CARD_TYPE_NO_SIGN, IDS_CA_MASTER_CARD_ID_NO_SIGN,
		//   IDS_SP_ID, IDS_BACK, IDS_CA_ADDRES_INFO,
		IDS_SP_ID, IDS_CARD_TYPE_NO_SIGN, IDS_CA_MASTER_CARD_ID_NO_SIGN,
		IDS_FEED_DURATION, IDS_LAST_FEED_TIME,
	};
	static sn_comm_help_data_t operate_help_data; //help bar data  
	help_rsc help_item[SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT]=
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


//	full_screen_title_t title_data = {IM_SN_BG,IDS_CARD_MANAGE, RSI_ORANGE, IDS_CARD_MANAGE};
	child_info_count = 0;
/*	p_cont = ui_comm_prev_root_create(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER,
							   RSI_ORANGE,
							   0, 0,
							   SCREEN_WIDTH, SCREEN_HEIGHT,
							   &title_data);
*/DEBUG(MAIN,INFO,"1------------------------------------");
  p_cont = ui_background_create(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER,
                        0,0, //CA_CONT_X, CA_CONT_Y,
                        CONDITIONAL_ACCEPT_MOTHER_CONT_W,CONDITIONAL_ACCEPT_MOTHER_CONT_H,//CONDITIONAL_ACCEPT_MOTHER_CONT_H,//CONDITIONAL_ACCEPT_MOTHER_CONT_H,//CA_CONT_W, CA_CONT_H,
                         IDS_CARD_MANAGE,TRUE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_keymap(p_cont, conditional_accept_mother_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_mother_cont_proc);
  DEBUG(MAIN,INFO,"2------------------------------------");
  //CA frm
  p_ca_frm = ctrl_create_ctrl((u8 *)CTRL_CONT, (u8)IDC_FRM_CA,
                              CONDITIONAL_ACCEPT_MOTHER_CA_FRM_X, CONDITIONAL_ACCEPT_MOTHER_CA_FRM_Y-60,
                              CONDITIONAL_ACCEPT_MOTHER_CA_FRM_W, CONDITIONAL_ACCEPT_MOTHER_CA_FRM_H-60,
                              p_cont, 0);
 //QQ  ctrl_set_rstyle(p_ca_frm, RSI_TIP_BOX, RSI_TIP_BOX, RSI_TIP_BOX);
 ctrl_set_rstyle(p_ca_frm, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  //ctrl_set_keymap(p_ca_frm, conditional_accept_mother_ca_frm_btn_keymap);
  //ctrl_set_proc(p_ca_frm, conditional_accept_mother_ca_frm_btn_proc);
  DEBUG(MAIN,INFO,"3------------------------------------");

  //titles
  
  x = CONDITIONAL_ACCEPT_MOTHER_TITLE_X;
  for(i=0; i<CONDITIONAL_ACCEPT_MOTHER_TITLE_CNT; i++)
  {
    p_ctrl = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_TITLE1 + i),
                              x, CONDITIONAL_ACCEPT_MOTHER_TITLE_Y,
                              title_width[i],
                              CONDITIONAL_ACCEPT_MOTHER_TITLE_H,
                              p_ca_frm, 0);
    
   //QQ   ctrl_set_rstyle(p_ctrl, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);
    ctrl_set_rstyle(p_ctrl, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
    text_set_font_style(p_ctrl, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, stxt_title[i]);
    DEBUG(MAIN,INFO,"4------------------------------------");
    x += (title_width[i]+CONDITIONAL_ACCEPT_MOTHER_TITLE_H_GAP);
  }
  DEBUG(MAIN,INFO,"5------------------------------------");

  //create program list  
  p_list =  ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CONDITIONAL_ACCEPT_MOTHER_LIST,
                     CONDITIONAL_ACCEPT_MOTHER_LIST_X, CONDITIONAL_ACCEPT_MOTHER_LIST_Y ,
                     CONDITIONAL_ACCEPT_MOTHER_LIST_W, CONDITIONAL_ACCEPT_MOTHER_LIST_H-70,
                     p_ca_frm, 0);
  DEBUG(MAIN,INFO,"5---1--------------------------------");
  ctrl_set_rstyle(p_list, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  ctrl_set_keymap(p_list, mother_list_keymap);
  ctrl_set_proc(p_list, mother_list_proc);
  DEBUG(MAIN,INFO,"5---2--------------------------------");

  list_set_mid_rect(p_list, CONDITIONAL_ACCEPT_MOTHER_LIST_MID_L, CONDITIONAL_ACCEPT_MOTHER_LIST_MID_T,
                    CONDITIONAL_ACCEPT_MOTHER_LIST_MID_W, CONDITIONAL_ACCEPT_MOTHER_LIST_MID_H-70,
                    CONDITIONAL_ACCEPT_MOTHER_LIST_ITEM_V_GAP);
  DEBUG(MAIN,INFO,"5---3--------------------------------");
  list_set_item_rstyle(p_list, &mother_list_item_rstyle);
  DEBUG(MAIN,INFO,"5---4--------------------------------");
  list_set_count(p_list, 0, CONDITIONAL_ACCEPT_MOTHER_LIST_PAGE);
  list_set_field_count(p_list, CONDITIONAL_ACCEPT_MOTHER_LIST_FIELD_NUM, CONDITIONAL_ACCEPT_MOTHER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  DEBUG(MAIN,INFO,"5---5--------------------------------");
  list_set_update(p_list, mon_child_info_update, 0);
 DEBUG(MAIN,INFO,"6-----------------------------------");
  for (i = 0; i < CONDITIONAL_ACCEPT_MOTHER_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(mother_list_attr[i].attr), (u16)(mother_list_attr[i].width),
                        (u16)(mother_list_attr[i].left), (u8)(mother_list_attr[i].top));
	DEBUG(MAIN,INFO,"6-----1------------------------------i=%d\n",i);
    list_set_field_rect_style(p_list, (u8)i, mother_list_attr[i].rstyle);
	DEBUG(MAIN,INFO,"6-----2-----------------------------i=%d\n",i);
    list_set_field_font_style(p_list, (u8)i, mother_list_attr[i].fstyle);
	DEBUG(MAIN,INFO,"6-----3-----------------------------i=%d\n",i);
  }
  
  // list_update(p_list, list_get_valid_pos(p_list), CONDITIONAL_ACCEPT_MOTHER_LIST_PAGE, 0);

  //create scroll bar
 
  /* set focus according to current info */
/*QQ  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
*/
 //QQ   ui_comm_help_create(&ca_mother_help_data, p_cont);
  memset((void*)&operate_help_data, 0, sizeof(sn_comm_help_data_t));
  operate_help_data.x=0;
  operate_help_data.item_cnt=SN_CONDITIONAL_ACCEPT_OPERATE_HELP_RSC_CNT;
  operate_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  operate_help_data.offset=37;
  operate_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&operate_help_data, p_cont);
  
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_get_info(CAS_CMD_OPERATOR_INFO_GET, 0, 0);
  DEBUG(MAIN,INFO,"7------------------------------------");

  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_mother_cont_keymap, ui_comm_root_keymap)
  //ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_mother_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_mother_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_mother_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_mother_cont_focus_change)  
END_MSGPROC(conditional_accept_mother_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(mother_list_keymap, NULL)
//QQ   ON_EVENT(V_KEY_RED, MSG_CA_FEED_START)  
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)  
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)   
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(mother_list_keymap, NULL)

BEGIN_MSGPROC(mother_list_proc, list_class_proc)
  //ON_COMMAND(MSG_SELECT, on_slist_select)
  //ON_COMMAND(MSG_FOCUS_UP, on_slist_change_focus)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_mon_child_info)
  ON_COMMAND(MSG_CA_OPE_INFO, on_operate_info)
  ON_COMMAND(MSG_EXIT, on_mon_child_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_mon_child_exit_all)
  ON_COMMAND(MSG_CA_READ_FEED_DATA, on_mon_child_feed_data)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_mon_child_feed_result)
//QQ   ON_COMMAND(MSG_CA_FEED_START, on_conditional_accept_feed_btn_f1)
END_MSGPROC(mother_list_proc, list_class_proc)

