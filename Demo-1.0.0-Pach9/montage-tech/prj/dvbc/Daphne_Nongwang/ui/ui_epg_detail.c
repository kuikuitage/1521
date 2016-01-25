
#include "ui_common.h"

#include "ui_epg_detail.h"

#include "epg_util.h"

#include "ui_signal.h"

static evt_node_t *p_evt_node;
static u16 *p_ext_detail = NULL;
static u16 pgid;

enum local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 225,
  MSG_GREEN,
};

enum epg_detail_ctrl_id
{
  IDC_DETAIL_TTL = 1,
  IDC_DETAIL_DTL,
  IDC_DETAIL_SBAR,
  IDC_DETAIL_HELP,
};

enum epg_detail_title_ctrl_id
{
  IDC_DETAIL_SNAME = 1,
  IDC_DETAIL_ENAME,
  IDC_DETAIL_ETIME,
  IDC_DETAIL_ELANG,
  IDC_DETAIL_ETYPE,
};

u16 epg_detail_cont_keymap(u16 key);
RET_CODE epg_detail_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 epg_detail_keymap(u16 key);

static RET_CODE on_epg_detail_close(control_t *p_cont, u16 msg, u32 para1,
                              u32 para2)
{
  mtos_free(p_ext_detail);
  
  return manage_close_menu(ROOT_ID_EPG_DETAIL, 0, 0);
}

RET_CODE open_epg_detail(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[DETAIL_ITEM_NUM];
  control_t *p_title, *p_detail, *p_help, *p_sbar;
  u8 i, j, asc_str[32];
  u16 x, y, w, view_pos;
  u16 uni_str1[32], uni_str2[32];
  utc_time_t startime;
  utc_time_t endtime;
  dvbs_prog_node_t pg;
  ext_evt_desc_uni_t *p_evt_desc;

  //special code, to close signal first.
  close_signal();

  p_evt_node = (evt_node_t *)para1;
  pgid = (u16)para2;
  
  p_ext_detail = (u16 *)mtos_malloc(MAX_EXT_TXT_BUF_LEN);
  MT_ASSERT(p_ext_detail != NULL);

  memset(p_ext_detail, 0, MAX_EXT_TXT_BUF_LEN);

  p_cont = fw_create_mainwin(ROOT_ID_EPG_DETAIL,
    DETAIL_CONT_X, DETAIL_CONT_Y, DETAIL_CONT_W, DETAIL_CONT_H,
    ROOT_ID_EPG, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    OS_PRINTF("ui: open_epg_detail %d\n", __LINE__);
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_DETAIL_FRM, RSI_DETAIL_FRM, RSI_DETAIL_FRM);
  ctrl_set_keymap(p_cont, epg_detail_cont_keymap);
  ctrl_set_proc(p_cont, epg_detail_cont_proc);

  p_title = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_DETAIL_TTL,
    DETAIL_TTL_X, DETAIL_TTL_Y, DETAIL_TTL_W, DETAIL_TTL_H, p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_DETAIL_TTL, RSI_DETAIL_TTL, RSI_DETAIL_TTL);
  
  x = DETAIL_ITEM_X;
  y = DETAIL_ITEM_Y;
  w = DETAIL_ITEM_W;
  OS_PRINTF("ui: open_epg_detail %d\n", __LINE__);
  for (i = 0; i < DETAIL_ITEM_NUM; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_DETAIL_SNAME + i), 
      x, y, w, DETAIL_ITEM_H, p_title, 0);
    text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
        
    switch (i)
    {
      case 0://sevice name
        db_dvbs_get_pg_by_id(pgid, &pg);
        view_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), pgid);
        sprintf((char *)asc_str, "%.4d  ", view_pos + 1);            
        str_nasc2uni(asc_str, uni_str1, 31);
        ui_dbase_get_full_prog_name(&pg, uni_str2, 31);
        uni_strcat(uni_str1, uni_str2, 31);
        text_set_content_by_unistr(p_ctrl[i], uni_str1);
        
        y += DETAIL_ITEM_H + DETAIL_V_GAP;        
        break;
      case 1://event name
        text_set_content_by_unistr(p_ctrl[i], p_evt_node->event_name); 
        
        y += DETAIL_ITEM_H + DETAIL_V_GAP;
        w = DETAIL_ITEM_LW;
        break;
      case 2://event time
        memcpy(&startime, &p_evt_node->start_time, sizeof(utc_time_t));
        memcpy(&endtime, &startime, sizeof(utc_time_t));
        time_add(&endtime, &(p_evt_node->drt_time));

        sprintf((char *)asc_str, "%.2d:%.2d-%.2d:%.2d", 
          startime.hour, startime.minute, endtime.hour, endtime.minute);      
        text_set_content_by_ascstr(p_ctrl[i], asc_str); 
        
        x = DETAIL_ITEM_X + DETAIL_ITEM_LW;
        w = DETAIL_ITEM_RW;
        break;
      case 3://event language code
        text_set_content_by_unistr(p_ctrl[i], p_evt_node->lang_code);

        x = DETAIL_ITEM_X;
        w = DETAIL_ITEM_W; 
        y += DETAIL_ITEM_H + DETAIL_V_GAP;  
        break;
      case 4://event type
        //tbd
        text_set_content_by_ascstr(p_ctrl[i], epg_get_type(p_evt_node->cont_level));
        y += DETAIL_ITEM_H + DETAIL_V_GAP;
        break;
      default:
        MT_ASSERT(0);
        break;
    }

  }
  OS_PRINTF("ui: open_epg_detail %d\n", __LINE__);
  p_detail = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_DETAIL_DTL,
    DETAIL_DETAIL_X, DETAIL_DETAIL_Y, DETAIL_DETAIL_W, DETAIL_DETAIL_H,
    p_cont, 0);
  ctrl_set_keymap(p_detail, epg_detail_keymap);
  ctrl_set_rstyle(p_detail, RSI_DETAIL_DTL, RSI_DETAIL_DTL, RSI_DETAIL_DTL);
  text_set_content_type(p_detail, TEXT_STRTYPE_EXTSTR);
  text_set_align_type(p_detail, STL_LEFT | STL_TOP);
  text_set_offset(p_detail, 0, 0);
  text_set_mid_rect(p_detail, DETAIL_DETAIL_MIDX, DETAIL_DETAIL_MIDY,
    DETAIL_DETAIL_MIDW, DETAIL_DETAIL_MIDH);
  text_enable_page(p_detail, TRUE);

  //short txt
  str_asc2uni((u8 *)"\n", uni_str1);
  str_asc2uni((u8 *)"...", uni_str2);
  if(p_evt_node->p_sht_txt != NULL)
  {
    OS_PRINTF("short txt\n");
    uni_strncpy(p_ext_detail, p_evt_node->p_sht_txt, DETAIL_MAX_LENTH);
    uni_strcat(p_ext_detail, uni_str1, DETAIL_MAX_LENTH);
  }

  //extent txt
  p_evt_desc = p_evt_node->p_ext_desc;
  OS_PRINTF("ext desc %d\n", p_evt_node->ext_desc_num);
  for(i = 0; i < p_evt_node->ext_desc_num; i++)
  {
    OS_PRINTF("item_num [%d]\n", p_evt_desc->item_num);
    for(j = 0; j < p_evt_desc->item_num; j++)
    {
      uni_strcat(p_ext_detail, p_evt_desc->item[j].item_name, DETAIL_MAX_LENTH);
      uni_strcat(p_ext_detail, uni_str2, DETAIL_MAX_LENTH);        
      uni_strcat(p_ext_detail, p_evt_desc->item[j].item_cont, DETAIL_MAX_LENTH);
      uni_strcat(p_ext_detail, uni_str1, DETAIL_MAX_LENTH);
    }
	  p_evt_desc = p_evt_desc->p_nxt_ext;
  }
  
  OS_PRINTF("ui: open_epg_detail %d\n", __LINE__);
  //extent txt
  if(p_evt_node->p_ext_txt != NULL)
  {
    OS_PRINTF("extent txt\n");
    uni_strcat(p_ext_detail, p_evt_node->p_ext_txt, DETAIL_MAX_LENTH);
  }

  OS_PRINTF("length %d\n", uni_strlen(p_ext_detail));
  text_set_content_by_extstr(p_detail, p_ext_detail);

  p_sbar = ctrl_create_ctrl((u8 *)CTRL_SBAR, IDC_DETAIL_SBAR, 
    DETAIL_BAR_X, DETAIL_BAR_Y, DETAIL_BAR_W, DETAIL_BAR_H,
    p_cont, 0);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rect(p_sbar, 0, 12, DETAIL_BAR_W, DETAIL_BAR_H - 12);
  text_set_scrollbar(p_detail, p_sbar);
  
  text_reset_param(p_detail);

  p_help = ctrl_create_ctrl((u8 *)CTRL_MBOX, IDC_DETAIL_HELP, 
    DETAIL_HELP_X, DETAIL_HELP_Y, DETAIL_HELP_W, DETAIL_HELP_H, p_cont, 0);
  ctrl_set_rstyle(p_help, RSI_DETAIL_HELP, RSI_DETAIL_HELP, RSI_DETAIL_HELP);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, 1, 1, 1);
  mbox_set_item_rect(p_help, 0, 0, DETAIL_HELP_W, DETAIL_HELP_H, 20, 0);
  mbox_set_string_offset(p_help, 200, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_icon_align_type(p_help, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);
  mbox_set_content_by_strid(p_help, 0, IDS_CANCEL);
  mbox_set_content_by_icon(p_help, 0, IM_SN_F1, IM_SN_F1);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_detail, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(epg_detail_cont_keymap, NULL)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_EXIT, MSG_RED)
  ON_EVENT(V_KEY_MENU, MSG_RED)
END_KEYMAP(epg_detail_cont_keymap, NULL)

BEGIN_MSGPROC(epg_detail_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_RED, on_epg_detail_close)
END_MSGPROC(epg_detail_cont_proc, cont_class_proc)

BEGIN_KEYMAP(epg_detail_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
END_KEYMAP(epg_detail_keymap, NULL)
