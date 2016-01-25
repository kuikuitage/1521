#include "ui_common.h"
#include "ui_ca_ppt_dlg.h"
#include "ui_ca_finger.h"

enum control_id
{
  IDC_INVALID = 0,
    
  IDC_TIPS,
  IDC_TITLE,

  IDC_IPP_CONT,
  IDC_PRICE_CONT,

};

enum control_tip_id
{
  IDC_TIP_TXT = 1,
  IDC_TIP_TXT2,
};

enum control_ipp_detail
{
  IDC_PG_ID = 1,
  IDC_OPE_ID,
  IDC_OVERDUE_TIME,
  IDC_NORECORDED_PRICE,
  IDC_RECORDED_PRICE,
  IDC_SLOT_ID,
  IDC_PIN_INPUT,
};


enum local_msg
{
  MSG_PPV_ORDE = MSG_LOCAL_BEGIN + 720,

};


static ipp_buy_info_t *r_time_info = NULL;
static u8 pin_s[7];
static u8 temp_ctrl_id = 0;

static u16 pin_modify_pwdedit_keymap(u16 key);
static u16 ca_order_unsubscribe_cont_keymap(u16 key);
static u16 ca_order_ipp_info_keymap(u16 key);

static RET_CODE ca_order_unsubscribe_cont_proc(control_t *ctrl, u16 msg,u32 para1, u32 para2);
static RET_CODE ca_order_ipp_info_proc(control_t *ctrl, u16 msg,u32 para1, u32 para2);
static RET_CODE pin_modify_pwdedit_proc(control_t *ctrl, u16 msg,u32 para1, u32 para2);
extern RET_CODE svstb_convert_time(u32 u32Time,utc_time_t *pUTCTime);
static void ca_ipp_refresh_real_time(control_t *p_parent)
{
  control_t *p_temp,*p_temp_cont;
  u16 temp2[128],temp[20];
  u8 uIndex = 0;
  u8 asc_buf[32];
  
  OS_PRINTF("UI_CA_OU ipp debug!!!!/n");
  OS_PRINTF("p_r_time_info->message_type=%u!\n", r_time_info->message_type);
  OS_PRINTF("p_r_time_info->burse_id=%u!\n", r_time_info->burse_id);
  OS_PRINTF("p_r_time_info->price_num=%u!\n", r_time_info->price_num);
  OS_PRINTF("p_r_time_info->product_id=%u!\n", r_time_info->product_id);
  OS_PRINTF("p_r_time_info->ecm_pid=%u!\n", r_time_info->ecm_pid);
  for(;uIndex < r_time_info->price_num; uIndex++)
  {
    OS_PRINTF("p_r_time_info->ipp_price[%d].price_type=%u!\n", uIndex, r_time_info->ipp_price[uIndex].price_type);
    OS_PRINTF("p_r_time_info->ipp_price[%d].price=%u!\n", uIndex, r_time_info->ipp_price[uIndex].price);
  }
  if(r_time_info->message_type == 2)
  {
    OS_PRINTF(" IPPT  interval(min) =%u!\n", ((r_time_info->expired_time[5] << 8) | r_time_info->expired_time[4]));
  }
  else
  {
    OS_PRINTF("IPPV overdue year = %d, month = %d, day = %d\n",
                                            (r_time_info->expired_time[0] * 100 + r_time_info->expired_time[1]),
                                            r_time_info->expired_time[2],
                                            r_time_info->expired_time[3]);
  }
  
  p_temp_cont = ctrl_get_child_by_id(p_parent,IDC_IPP_CONT);

  
  //for pg id
  p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_PG_ID);
  sprintf((char *)asc_buf,"%lu",r_time_info->product_id);
  str_asc2uni(asc_buf, temp);
  text_set_content_by_unistr(p_temp, temp);
  
  //for opertor id
  p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_OPE_ID);
  sprintf((char *)asc_buf,"%d",r_time_info->tvs_id);
  str_asc2uni(asc_buf, temp);
  text_set_content_by_unistr(p_temp, temp);
  
  //for overdue time
  p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_OVERDUE_TIME);
  if(r_time_info->message_type == 2)
  {
    sprintf((char *)asc_buf,"%d", ((r_time_info->expired_time[5] << 8) | r_time_info->expired_time[4]));
    str_asc2uni(asc_buf, temp2);
    gui_get_string(IDS_MINUTE2, temp, 20);
    uni_strcat(temp2, temp, uni_strlen(temp) + uni_strlen(temp2));
  }
  else
  {
    sprintf((char *)asc_buf,"%04d/%02d/%02d %02d:%02d", (r_time_info->expired_time[0] * 100 + r_time_info->expired_time[1]),
                                            r_time_info->expired_time[2],
                                            r_time_info->expired_time[3],
                                            r_time_info->expired_time[4],
                                            r_time_info->expired_time[5]);
    str_asc2uni(asc_buf, temp2);
  }
  text_set_content_by_unistr(p_temp, temp2);

  //for recorded and no-recorded price
  for(uIndex = 0;uIndex < r_time_info->price_num; uIndex++)
  {
    if(r_time_info->ipp_price[uIndex].price_type == 1)/* 不回传，不录像类型 */
    {
      p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_NORECORDED_PRICE);
    }
    else
    {
      p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_RECORDED_PRICE);
    }
    
    ctrl_set_attr(p_temp, OBJ_ATTR_ACTIVE);
    sprintf((char *)asc_buf,"%d",r_time_info->ipp_price[uIndex].price);
    str_asc2uni(asc_buf, temp2);
    gui_get_string(IDS_CA_POINT,temp,20);
    uni_strcat(temp2, temp, uni_strlen(temp) + uni_strlen(temp2));
    text_set_content_by_unistr(p_temp, temp2);
  }
  
  //for slot id
  p_temp = ctrl_get_child_by_id(p_temp_cont,IDC_SLOT_ID);
  sprintf((char *)asc_buf,"%d",r_time_info->burse_id);
  str_asc2uni(asc_buf, temp);
  text_set_content_by_unistr(p_temp, temp);
}


RET_CODE open_real_time_ipp(u32 para1, u32 para2)
{
#define SN_SUB_MENU_HELP_RSC_CNT  8

  control_t *p_mark, *p_cont, *p_tmp ,*p_tmp_cnt, *p_title, *p_child_cont, *p_ipp_info;
  u8 i;
  u16 ipp_str2[6] = {IDS_CA_PROG_ID, IDS_CA_SP_ID, IDS_CA_OVERDUE_TIME, IDS_CA_NO_RECORD_PRICE,
                     IDS_CA_RECORD_PRICE,IDS_SLOT_ID2};
  static sn_comm_help_data_t help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
     {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
     {SN_IDC_HELP_TEXT, 40 , IDS_BACK},
     {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
     {SN_IDC_HELP_TEXT, 50 , IDS_EXIT},
     {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_LEFT},
     {SN_IDC_HELP_TEXT, 40 , IDS_OK_WF},
     {SN_IDC_HELP_BMAP, 20 , IM_SN_ICON_BRACKET_RIGHT},
     {SN_IDC_HELP_TEXT, 50 , IDS_CAS_IPPV_ORDER}
  };

  r_time_info = (ipp_buy_info_t *)para2;

  if(NULL != fw_find_root_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME))
  {
    return SUCCESS;
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_PROG_BAR))
  {
	  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }

  // create window
  p_mark = fw_create_mainwin(ROOT_ID_CA_BOOK_IPP_REAL_TIME,
                             IPP_RTIME_IPP_CONT_X, IPP_RTIME_IPP_CONT_Y,
                             IPP_RTIME_IPP_CONT_W, IPP_RTIME_IPP_CONT_H,
                             0, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if (p_mark == NULL)
  {
    return FALSE;
  }
  ctrl_set_rstyle(p_mark,
                  RSI_SN_BG,
                  RSI_SN_BG,
                  RSI_SN_BG);
  p_cont = ctrl_create_ctrl((u8 *)CTRL_CONT,1,
                   IPP_RTIME_IPP_X,IPP_RTIME_IPP_Y,
                   IPP_RTIME_IPP_W, IPP_RTIME_IPP_H,p_mark,0);
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_cont, ca_order_unsubscribe_cont_keymap);
  ctrl_set_proc(p_cont, ca_order_unsubscribe_cont_proc);
  //for creat title
  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TITLE,
                           IPP_RTIME_STRIP_X,0,IPP_RTIME_STRIP_W,IPP_RTIME_STRIP_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_SN_TITLE_BG, RSI_SN_TITLE_BG, RSI_SN_TITLE_BG);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  
  if(r_time_info->message_type == 0)   /* IPPV免费预览阶段，是否购买 */
    text_set_content_by_strid(p_title,IDS_CA_PPV_FREE); // QQ  IDS_CA_PPV_FREE
  else if(r_time_info->message_type == 1)   /* IPPV收费阶段，是否购买 */
    text_set_content_by_strid(p_title,IDS_CA_PPV_CHARGES); //QQ IDS_CA_PPV_CHARGES
  else if(r_time_info->message_type == 2)   /* IPPT 收费阶段，是否购买 */
    text_set_content_by_strid(p_title,IDS_CA_PPT_CHARGES);  //QQ  IDS_CA_PPT_CHARGES
  else
    OS_PRINTF("ipp type error !!!!\n");
    
  //ipp info
  p_ipp_info = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_IPP_CONT,
                               IPP_PROG_INFO_CONT_X, IPP_PROG_INFO_CONT_Y,
                               IPP_PROG_INFO_CONT_W,IPP_PROG_INFO_CONT_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_ipp_info,RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  ctrl_set_keymap(p_ipp_info, ca_order_ipp_info_keymap);
  ctrl_set_proc(p_ipp_info, ca_order_ipp_info_proc);
  for(i=0; i<6 ;i++)
  {
    p_tmp = ctrl_create_ctrl((u8 *)CTRL_TEXT, 0 ,
                             IPP_RTIME_ITM_X + 4, IPP_RTIME_ITM_Y+i*IPP_RTIME_ITM_H, IPP_PROG_ITM_W, IPP_RTIME_ITM_H,
                             p_ipp_info, 0);
    text_set_align_type(p_tmp, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_tmp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_tmp, TEXT_STRTYPE_STRID);
    if(i == 2)
    {
      if(r_time_info->message_type == 2)
        text_set_content_by_strid(p_tmp, IDS_CA_ADDRES_INFO);//QQ   IDS_CA_INTERVAL
      else
        text_set_content_by_strid(p_tmp, ipp_str2[i]);
    }
    else
    {
      text_set_content_by_strid(p_tmp, ipp_str2[i]);
    }

    p_tmp = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_PG_ID+i) ,
                             IPP_PROG_ITM_W,IPP_RTIME_ITM_Y+i*IPP_RTIME_ITM_H,
                             IPP_RTIME_ITM_W, IPP_RTIME_ITM_H,
                             p_ipp_info, 0);
    text_set_content_type(p_tmp, TEXT_STRTYPE_UNICODE);
    if((i == 3) || (i == 4))
    {
      text_set_align_type(p_tmp, STL_CENTER| STL_VCENTER);
	  ctrl_set_rstyle(p_tmp, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      text_set_font_style(p_tmp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    }
    else
    {
      text_set_align_type(p_tmp, STL_LEFT | STL_VCENTER);
	  ctrl_set_rstyle(p_tmp, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
      text_set_font_style(p_tmp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    }
  }
  
  ctrl_set_related_id(ctrl_get_child_by_id(p_ipp_info, IDC_NORECORDED_PRICE), 0, IDC_RECORDED_PRICE, 0, IDC_RECORDED_PRICE);
  ctrl_set_related_id(ctrl_get_child_by_id(p_ipp_info, IDC_RECORDED_PRICE), 0, IDC_NORECORDED_PRICE, 0, IDC_NORECORDED_PRICE);
  
  //for PIN input
  p_child_cont = ui_comm_pwdedit_create(p_ipp_info, (u8)IDC_PIN_INPUT,
                                 4, IPP_RTIME_ITM_Y+i*IPP_RTIME_ITM_H,
                                 IPP_RTIME_ITM_PIN_W_L,
                                 IPP_RTIME_ITM_PIN_W_R);
  ui_comm_static_set_align_type(p_child_cont,0,STL_LEFT | STL_VCENTER,0,STL_CENTER | STL_VCENTER);
  ctrl_set_rstyle(p_child_cont,
                  RSI_IGNORE,
                  RSI_IGNORE,
                  RSI_IGNORE);
  ui_comm_pwdedit_set_static_txt(p_child_cont,IDS_INPUT_PASSWORD);
  ui_comm_pwdedit_set_param(p_child_cont, CAS_MAX_PIN_LEN);
  ui_comm_ctrl_set_keymap(p_child_cont, pin_modify_pwdedit_keymap);
  ui_comm_ctrl_set_proc(p_child_cont, pin_modify_pwdedit_proc);
  ui_comm_ctrl_set_txt_font_style(p_child_cont,FSI_WHITE,FSI_WHITE,FSI_WHITE);
  ctrl_set_attr(p_child_cont, OBJ_ATTR_HIDDEN);
  
  //for tips
  p_tmp_cnt = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_TIPS,
                                       FOR_RTIME_TIPS_CONT_X, FOR_RTIME_TIPS_CONT_Y,
                                       FOR_RTIME_TIPS_CONT_W,FOR_RTIME_TIPS_CONT_H,
                                       p_cont, 0);
  ctrl_set_rstyle(p_tmp_cnt,RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  p_tmp = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TIP_TXT,
                     FOR_RTIME_TIPS_CONT_X,0,
                     FOR_RTIME_TIPS_CONT_W / 2 - FOR_RTIME_TIPS_CONT_X,FOR_RTIME_TIPS_CONT_H,
                     p_tmp_cnt, 0);
  text_set_align_type(p_tmp, STL_LEFT);
  ctrl_set_rstyle(p_tmp,RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  text_set_font_style(p_tmp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_tmp, TEXT_STRTYPE_STRID);

  p_tmp = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_TIP_TXT2,
                     FOR_RTIME_TIPS_CONT_W / 2 - FOR_RTIME_TIPS_CONT_X,0,
                     FOR_RTIME_TIPS_CONT_W / 2 - FOR_RTIME_TIPS_CONT_X,FOR_RTIME_TIPS_CONT_H,
                     p_tmp_cnt, 0);
  text_set_align_type(p_tmp, STL_LEFT);
  ctrl_set_rstyle(p_tmp,RSI_SN_BG,RSI_SN_BG,RSI_SN_BG);
  text_set_font_style(p_tmp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_tmp, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_tmp, IDS_BACK_DEDUCTIONS); //QQ IDS_BACK_DEDUCTIONS
  if(r_time_info->message_type == 2)
  {
    ctrl_set_attr(p_tmp, OBJ_ATTR_ACTIVE);
  }
  else
  {
    ctrl_set_attr(p_tmp, OBJ_ATTR_HIDDEN);
  }
  
  memset((void*)&help_data, 0, sizeof(sn_comm_help_data_t));
  help_data.x=0;
  help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  help_data.offset=180;
  help_data.rsc =help_item;
  sn_ui_comm_help_create(&help_data, p_cont);
  ca_ipp_refresh_real_time(p_cont);
  ctrl_default_proc(ctrl_get_child_by_id(p_ipp_info, IDC_NORECORDED_PRICE), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mark, TRUE);

  return SUCCESS;
}


static RET_CODE on_ca_select_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_psw =NULL, *p_price1 =NULL, *p_price2 =NULL;
  
  p_price1 = ctrl_get_child_by_id(p_ctrl,IDC_NORECORDED_PRICE);
  p_price2 = ctrl_get_child_by_id(p_ctrl,IDC_RECORDED_PRICE);
  
  temp_ctrl_id = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl));
  p_psw = ctrl_get_child_by_id(p_ctrl,IDC_PIN_INPUT);

  ctrl_default_proc(p_price1, MSG_LOSTFOCUS, 0, 0);
  ctrl_set_attr(p_psw, OBJ_ATTR_ACTIVE);  
  ctrl_default_proc(p_psw, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_psw, TRUE);
  return SUCCESS;
} 

static RET_CODE on_ipp_pin_select_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_temp;
  u32 pin_code;
  BOOL pin_code_is_full;
  s8 i;
  s8 pin; 
  u8 uIndex = 0;
  p_temp = ctrl_get_parent(p_ctrl);
  pin_code = ui_comm_pwdedit_get_value_sv(p_temp);
  pin_code_is_full = ui_comm_pwdedit_is_full(p_temp);

  if(!ui_is_smart_card_insert())
  {
    ui_comm_cfmdlg_open2(NULL,IDS_INSERT_SMC,NULL,0);
    ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_temp), MSG_EMPTY, 0, 0);
    return SUCCESS;
  }
  
  for(uIndex = 0;uIndex < r_time_info->price_num; uIndex++)
  {
      if(temp_ctrl_id == IDC_NORECORDED_PRICE)
      {
        if(r_time_info->ipp_price[uIndex].price_type == 1)//不可录像
        {
          break;
        }
      }
      else
      {
        if(r_time_info->ipp_price[uIndex].price_type != 1)//可录像
        {
          break;
        }
      }
  }

  r_time_info->index = uIndex;
    
  for (i = CAS_MAX_PIN_LEN -1; i >= 0; i--)
  {
    pin = pin_code%10;
    pin_s[i] = pin;
    pin_code = pin_code/10;
  }
  if(pin_code_is_full == FALSE)
  {
    ui_comm_cfmdlg_open2(NULL,IDS_CAS_E_PIN_INVALID,NULL,0);//QQ  IDS_DIGIT_NOT_ENOUGH
  }
  else
  {
    pin_s[6] = '\0';
    OS_PRINTF("PIN发出去new_pin的值是%s\n",pin_s);
    memcpy(r_time_info->pin,pin_s,7);
    r_time_info->state_flag = 1;
    ui_ca_get_info(CAS_CMD_IPP_BUY_SET, (u32)r_time_info ,0);
  }
  ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_temp), MSG_EMPTY, 0, 0);
  ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_temp), TRUE);
  return SUCCESS;
}
static RET_CODE on_buy_ipp_result(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;
  if(fw_find_root_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME) == NULL)
    return SUCCESS;
  r_time_info = NULL;
  p_child = ctrl_get_child_by_id(p_cont,IDC_TIPS);
  p_child = ctrl_get_child_by_id(p_child,IDC_TIP_TXT);
  OS_PRINTF("on_buy_ipp_result ret == %d\n",para1);
  switch(para1)
  {
    case CAS_EVT_NONE:
      text_set_content_by_strid(p_child,IDS_CA_PURCHASED_SUCCESS); //QQ  IDS_CA_PURCHASED_SUCCESS
      //ctrl_process_msg(p_cont,MSG_CA_IPP_EXIT,0,0);
      break;

    case CAS_E_ILLEGAL_CARD:
      text_set_content_by_strid(p_child,IDS_CA_NO_CARD_INVALID);//QQ IDS_CA_NO_CARD_INVALID
      break;

    case CAS_E_INVALID_POINTER:
      text_set_content_by_strid(p_child,IDS_CA_POINT_NULL); //QQ  IDS_CA_POINT_NULL
      break;

    case CAS_E_CARD_NO_ROOM:
      text_set_content_by_strid(p_child,IDS_CA_CARD_NO_ROOM); //QQ IDS_CA_CARD_NO_ROOM
      break;

    case CAS_E_PROG_STATUS_INVALID:
      text_set_content_by_strid(p_child,IDS_CA_PRICE_INVALID);//QQ  IDS_CA_PRICE_INVALID
      break;

    case CAS_E_PIN_INVALID:
      text_set_content_by_strid(p_child,IDS_CA_PIN_INVALID);//QQ IDS_CA_PIN_INVALID
      break;

    case CAS_E_DATA_NOT_FIND:
      text_set_content_by_strid(p_child,IDS_CA_NO_OPE_PURSE);//QQ  IDS_CA_NO_OPE_PURSE
      break;
    default:
      text_set_content_by_strid(p_child,IDS_CAS_E_UNKNOWN_ERROR);
      break;
  }

  fw_tmr_create(ROOT_ID_CA_BOOK_IPP_REAL_TIME, MSG_CA_HIDE_IPPV_PPT, 5000, FALSE);
  
  ctrl_paint_ctrl(p_child,TRUE);

  return SUCCESS;
}

static RET_CODE on_ipp_channel_up_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 prog_id;
  control_t *p_cont = NULL;
  
  p_cont = fw_find_root_by_id(ROOT_ID_PROG_BAR);
  if(p_cont != NULL)
  {
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }
  switch(msg)
  {
    case MSG_FOCUS_UP:
      ui_shift_prog(1, TRUE, &prog_id);
      break;
    case MSG_FOCUS_DOWN:
      ui_shift_prog(-1, TRUE, &prog_id);
      break;
    default:
      OS_PRINTF("error message in on_ipp_channel_up_down\n");
  }
  
  manage_close_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME, 0, 0);
  ui_play_prog(prog_id,FALSE);
  return SUCCESS;
}

RET_CODE on_ca_real_time_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("before close ipp real time menu !!!!\n");
  r_time_info->state_flag = 0;
  ui_ca_get_info(CAS_CMD_IPP_BUY_SET, (u32)r_time_info,0);
//QQ  fw_tmr_destroy(ROOT_ID_CA_BOOK_IPP_REAL_TIME, MSG_CA_IPP_EXIT);
  fw_destroy_mainwin_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME);

  return SUCCESS;
}

RET_CODE on_ca_real_time_cancel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("on_ca_real_time_cancel !!!!\n");
  
  fw_tmr_destroy(ROOT_ID_CA_BOOK_IPP_REAL_TIME, MSG_CA_HIDE_IPPV_PPT);
  fw_destroy_mainwin_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME);

  if(r_time_info != NULL)
  {
    manage_open_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME, 0, (u32)r_time_info);
  }

  return SUCCESS;
}

BEGIN_KEYMAP(ca_order_unsubscribe_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_CH_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CH_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_order_unsubscribe_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_order_unsubscribe_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_IPP_BUY_OK, on_buy_ipp_result)
  ON_COMMAND(MSG_EXIT, on_ca_real_time_exit)
  ON_COMMAND(MSG_CA_HIDE_IPPV_PPT, on_ca_real_time_cancel)
  ON_COMMAND(MSG_FOCUS_UP, on_ipp_channel_up_down)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ipp_channel_up_down)
  //ON_COMMAND(MSG_FOCUS_UP, on_ipp_channel_up_down)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_ipp_channel_up_down)
END_MSGPROC(ca_order_unsubscribe_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_order_ipp_info_keymap,NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ca_order_ipp_info_keymap, NULL)

BEGIN_MSGPROC(ca_order_ipp_info_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_ca_select_ok)
END_MSGPROC(ca_order_ipp_info_proc, cont_class_proc)

BEGIN_KEYMAP(pin_modify_pwdedit_keymap, ui_comm_edit_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
  ON_EVENT(V_KEY_OK, MSG_PPV_ORDE)
END_KEYMAP(pin_modify_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(pin_modify_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_PPV_ORDE, on_ipp_pin_select_ok)
END_MSGPROC(pin_modify_pwdedit_proc, ui_comm_edit_proc)
