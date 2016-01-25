#include "ui_common.h"
//#include "sowell.h"
#include "ui_dvbca_sp_id.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CA_INFO_HEAD,  
  IDC_CA_INFO_LIST_ENTITLE,
  SN_IDC_PRODUCE_ID,
  SN_IDC_OVER_TIME,
};



static cas_operators_info_t *p_sp_id = NULL;

RET_CODE ca_sp_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_sp_list_keymap(u16 key);
RET_CODE ca_sp_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern BOOL ui_is_smart_card_insert(void);

static RET_CODE ca_info_list_sp_id_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u8 asc_str[64] = {0};
  u16 sp_id = 0;
  DEBUG(DVBCAS_PORTING,INFO,"****CALL IN*****\n"); 
  DEBUG(DVBCAS_PORTING,INFO,"***size = %d",size);
  DEBUG(DVBCAS_PORTING,INFO,"***start = %d",start);
  DEBUG(DVBCAS_PORTING,INFO,"***cnt = %d",list_get_count(p_list));
  DEBUG(DVBCAS_PORTING,INFO,"plist4 = %x\n",p_list);
  if(ui_is_smart_card_insert() == FALSE)
  	return SUCCESS;
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sp_id = p_sp_id->p_operator_info[i+start].operator_id;
      list_set_field_content_by_dec(p_list, 
        (u16)(start + i), 0, sp_id); 
      DEBUG(DVBCAS_PORTING,INFO,"product_id = %d\n",sp_id);
      sprintf((char *)asc_str,"%s",p_sp_id->p_operator_info[i+start].operator_name);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str); 
	  DEBUG(DVBCAS_PORTING,INFO,"**************asc_str = %s************\n",asc_str);
    }
  }
  return SUCCESS;
}



RET_CODE open_dvb_ca_sp_id_list(u32 para1, u32 para2)
{
  //mbox item title
  #define SN_DVB_CA_MBOX_X                   (640 - (SN_DVB_CA_MBOX_W))/2
  #define SN_DVB_CA_MBOX_Y                   10
  #define SN_DVB_CA_MBOX_W                   605
  #define SN_DVB_CA_MBOX_H                   31

  #define SN_DVB_CA_MBOX_MID_X               20
  #define SN_DVB_CA_MBOX_MID_Y               0
  #define SN_DVB_CA_MBOX_MID_W               505
  #define SN_DVB_CA_MBOX_MID_H               31

  #define SN_DVB_CA_PLIST_BG_X               SN_DVB_CA_MBOX_X
  #define SN_DVB_CA_PLIST_BG_Y               ((SN_DVB_CA_MBOX_Y) + (SN_DVB_CA_MBOX_H) + 10)
  #define SN_DVB_CA_PLIST_BG_W               SN_DVB_CA_MBOX_W
  #define SN_DVB_CA_PLIST_BG_H               300

  #define SN_DVB_CA_LIST_BG_MIDL             4
  #define SN_DVB_CA_LIST_BG_MIDT             4
  #define SN_DVB_CA_LIST_BG_MIDW             ((SN_DVB_CA_PLIST_BG_W) - 2*(SN_DVB_CA_LIST_BG_MIDL))
  #define SN_DVB_CA_LIST_BG_MIDH             ((SN_DVB_CA_PLIST_BG_H) - 2*(SN_DVB_CA_LIST_BG_MIDT))

  #define SN_DVB_CA_LIST_BG_VGAP             6
  
  #define SN_SUB_MENU_HELP_RSC_CNT       13

#ifdef WIN32
//LXD ADD PRODUCT ID     
#define  SN_DVB_PRODUCT_ID_X   30
#define  SN_DVB_PRODUCT_ID_Y   50
#define  SN_DVB_PRODUCT_ID_W   100
#define  SN_DVB_PRODUCT_ID_H   35

//LXD ADD  OVER TIME
#define  SN_DVB_OVER_TIME_X    ((SN_DVB_PRODUCT_ID_X) +245)
#define  SN_DVB_OVER_TIME_Y    SN_DVB_PRODUCT_ID_Y
#define  SN_DVB_OVER_TIME_W    100
#define  SN_DVB_OVER_TIME_H    35
#endif
  control_t *p_cont, *p_list, *p_mbox;//,*p_btn, *p_ctrl
  u8 i;
  u16 itemmenu_btn_str[DVB_CA_ITME_COUNT] =
  {
    IDS_SP_ID, IDS_SP_NAME
  };

  static sn_comm_help_data_t sn_submenu_help_data; //help bar data
  help_rsc help_item[SN_SUB_MENU_HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
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
  
  static list_xstyle_t ca_info_item_rstyle =
  {

    RSI_SN_BG,
    RSI_SN_BG,
    RSI_SN_BAR_YELLOW_HL,
    RSI_SN_BG,
    RSI_SN_BG,
  };
  static list_xstyle_t ca_info_field_fstyle =
  {
    FSI_LIST_TXT_G,
    FSI_LIST_TXT_N,
    FSI_LIST_BTN_HL,
    FSI_LIST_TXT_SEL,
    FSI_LIST_BTN_HL,
  };
  static list_xstyle_t ca_info_field_rstyle =
  {
    RSI_IGNORE,//RSI_COMM_LIST_N,
    RSI_IGNORE,//RSI_COMM_LIST_N,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  static list_field_attr_t ca_info_list_entitle_attr[DVB_CA_INFO_ENTITLE_FIELD] =
  {
    { LISTFIELD_TYPE_DEC| STL_LEFT | STL_VCENTER,
      80, 25, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

    { LISTFIELD_TYPE_UNISTR| STL_LEFT | STL_VCENTER,
      270, 270, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
  };
  // create container
  p_cont = ui_background_create(ROOT_ID_SP_LIST,
                             0, 0,
                             640,480,//SN_SUBMENU_W, SN_SUBMENU_H,
                             IDS_SP, TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_sp_info_proc);
 
  //mbox item title
  p_mbox = ctrl_create_ctrl((u8 *)CTRL_MBOX, (u8)IDC_CA_INFO_HEAD,
                            SN_DVB_CA_MBOX_X,SN_DVB_CA_MBOX_Y,//CA_MBOX_X, CA_MBOX_Y, 
                            SN_DVB_CA_MBOX_W, SN_DVB_CA_MBOX_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL, RSI_SN_BAR_BLUE_NORMAL);//RSI_TIP_BOX,RSI_COMMON_BTN_HL,RSI_TIP_BOX
  ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox, 1);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox, DVB_CA_ITME_COUNT, DVB_CA_ITME_COUNT, 1);
  mbox_set_item_rect(p_mbox, SN_DVB_CA_MBOX_MID_X, SN_DVB_CA_MBOX_MID_Y, SN_DVB_CA_MBOX_MID_W, SN_DVB_CA_MBOX_MID_H, 0, 0);
  mbox_set_string_fstyle(p_mbox, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE, FSI_INFOBOX_TITLE);//FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY
  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  for (i = 0; i < DVB_CA_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
  }
#ifdef WIN32
  //LXD ADD PRODUCT ID   
  p_mbox= ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_PRODUCE_ID),
					SN_DVB_PRODUCT_ID_X, SN_DVB_PRODUCT_ID_Y,
					SN_DVB_PRODUCT_ID_W, SN_DVB_PRODUCT_ID_H,p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_mbox, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
  text_set_align_type(p_mbox, STL_LEFT | STL_TOP);
  text_set_content_type(p_mbox, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_mbox, IDS_QAM256);

  //LXD ADD  OVER TIME
  p_mbox= ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(SN_IDC_OVER_TIME),
					SN_DVB_OVER_TIME_X, SN_DVB_OVER_TIME_Y,
					SN_DVB_OVER_TIME_W, SN_DVB_OVER_TIME_H,p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_mbox, FSI_COMM_BTN, FSI_COMM_BTN, FSI_GRAY);
  text_set_align_type(p_mbox, STL_LEFT | STL_TOP);
  text_set_content_type(p_mbox, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_mbox, IDS_QPSK_VP);
#endif
  p_list = ctrl_create_ctrl((u8 *)CTRL_LIST, IDC_CA_INFO_LIST_ENTITLE,
                           SN_DVB_CA_PLIST_BG_X,SN_DVB_CA_PLIST_BG_Y,//CA_PLIST_BG_X, CA_PLIST_BG_Y, 
                           SN_DVB_CA_PLIST_BG_W,SN_DVB_CA_PLIST_BG_H, p_cont, 0);
  ctrl_set_keymap(p_list, ca_sp_list_keymap);
  ctrl_set_proc(p_list, ca_sp_list_proc);
  ctrl_set_rstyle(p_list, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);//RSI_TIP_BOX
  list_set_mid_rect(p_list, SN_DVB_CA_LIST_BG_MIDL, SN_DVB_CA_LIST_BG_MIDT,
           SN_DVB_CA_LIST_BG_MIDW, SN_DVB_CA_LIST_BG_MIDH,SN_DVB_CA_LIST_BG_VGAP);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_count(p_list, 0, DVB_CA_INFO_LIST_PAGE);
  list_set_field_count(p_list, DVB_CA_INFO_ENTITLE_FIELD, DVB_CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 1);
  list_set_update(p_list, ca_info_list_sp_id_update, 0);
 
  for (i = 0; i < DVB_CA_INFO_ENTITLE_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,
      (u32)(ca_info_list_entitle_attr[i].attr),
      (u16)(ca_info_list_entitle_attr[i].width),
      (u16)(ca_info_list_entitle_attr[i].left),
      (u8)(ca_info_list_entitle_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,
                                ca_info_list_entitle_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,
                                ca_info_list_entitle_attr[i].fstyle);
  }  

  memset((void*)&sn_submenu_help_data, 0, sizeof(sn_comm_help_data_t));
  sn_submenu_help_data.x=0;
  sn_submenu_help_data.item_cnt=SN_SUB_MENU_HELP_RSC_CNT;
  sn_submenu_help_data.y=ctrl_get_height(p_cont)-SN_HELP_H;
  sn_submenu_help_data.offset=58;
  sn_submenu_help_data.rsc =help_item;
  DEBUG(DVBCAS_PORTING,INFO,"##################CNT = %d################\n",list_get_count(p_list));
  sn_ui_comm_help_create(&sn_submenu_help_data, p_cont);
  DEBUG(DVBCAS_PORTING,INFO,"4---****************open_dvb_ca_entitle_info***************\n");

  #ifndef WIN32
  ui_ca_get_info(CAS_CMD_OPERATOR_INFO_GET, 0, 0);
  DEBUG(DVBCAS_PORTING,INFO,"5---****************open_dvb_ca_entitle_info***************\n");
  #endif
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  DEBUG(DVBCAS_PORTING,INFO,"[dvbcas] %s>>>>>>>>>>>>> %d ! \n",__func__,__LINE__);
  return SUCCESS;
}

static RET_CODE on_ca_info_sp_id(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_ENTITLE);

  p_sp_id= (cas_operators_info_t *)para2;
  list_set_count(p_list, p_sp_id->max_num, DVB_CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  ca_info_list_sp_id_update(p_list, list_get_valid_pos(p_list), DVB_CA_INFO_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_sp_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus;

  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  my_sp_id = p_sp_id->p_operator_info[old_focus].operator_id;
  DEBUG(DVBCAS_PORTING,INFO,"p_sp_id select = %d\n",my_sp_id);
  manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, 0, 0);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_sp_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_OPE_INFO, on_ca_info_sp_id)
//  ON_COMMAND(MSG_SAVE, on_ca_info_save)
END_MSGPROC(ca_sp_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_sp_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(ca_sp_list_keymap, NULL)

BEGIN_MSGPROC(ca_sp_list_proc, list_class_proc)
  ON_COMMAND(MSG_YES, on_sp_list_select)
END_MSGPROC(ca_sp_list_proc, list_class_proc)

