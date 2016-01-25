
#include "ui_common.h"
#include "ui_email_mess.h"
#include "ui_email_mess_content.h"

enum contrl_id
{
  IDC_INVALID = 0,
  IDC_EMAIL_CONT,
  IDC_EMAIL_CONTENT_TITLE_HEAD,
  IDC_EMAIL_CONTENT_TIME_HEAD,
  IDC_EMAIL_CONTENT_BG,
  IDC_EMAIL_CONTENT,
  IDC_EMAIL_SBAR,
  IDC_EMAIL_BAR,
  IDC_EMAIL_LEFT_ARROW,
  IDC_EMAIL_RIGHT_ARROW,
  IDC_EMAILL_HEAD,
  IDC_EMAIL_TITLE_HEAD,
  IDC_EMAIL_TIME_HEAD,
  IDC_EMAIL_SENDER,
  IDC_EMAIL_SENDER_HEAD,
  IDC_EMAIL_BMP,
};

enum status_return_id
{
  SET_MAIL_SUCCESS = 0,
};

static u16 mail_content_str[512] = {0};
static u16 email_body_text_keymap(u16 key);
RET_CODE email_body_text_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

RET_CODE open_email_mess_content(u32 para1, u32 para2)
{  
#define SN_SCREEN_WIDTH			640
#define SN_SCREEN_HEIGHT		480
#define SN_EMAIL_MENU_W        	SN_SCREEN_WIDTH
#define SN_EMAIL_MENU_H			SN_SCREEN_HEIGHT

#define SN_EMAIL_HEAD_W			98
#define SN_EMAIL_CONTENT_HEAD_TITLE_X        10
#define SN_EMAIL_CONTENT_HEAD_TITLE_Y        10
#define SN_EMAIL_CONTENT_HEAD_TITLE_H        31

#define SN_EMAIL_CONTENT_HEAD_TIME_X        (SN_EMAIL_CONTENT_HEAD_TITLE_X + EMAIL_CONTENT_HEAD_TITLE_W)
#define SN_EMAIL_CONTENT_HEAD_TIME_Y        SN_EMAIL_CONTENT_HEAD_TITLE_Y
#define SN_EMAIL_CONTENT_HEAD_TIME_H        31

#define SN_EMAIL_CONTENT_BG_X        SN_EMAIL_CONTENT_HEAD_TITLE_X

#define SN_EMAIL_CONTENT_BG_Y        (SN_EMAIL_CONTENT_HEAD_TITLE_Y+SN_EMAIL_CONTENT_HEAD_TITLE_H+5)
#define SN_EMAIL_CONTENT_BG_W        (EMAIL_CONTENT_HEAD_TITLE_W + EMAIL_CONTENT_HEAD_TIME_W )
#define SN_EMAIL_CONTENT_BG_H        360

	//email bar
#define SN_EMAIL_BAR_X				  10
#define SN_EMAIL_BAR_Y				  360
#define SN_EMAIL_BAR_W				  600
#define SN_EMAIL_BAR_H				  31
  
	//email arrow
#define SN_EMAIL_LEFT_ARROW_X			  295
#define SN_EMAIL_RIGHT_ARROW_X			  SN_EMAIL_LEFT_ARROW_X+SN_EMAIL_ARROW_W+6
#define SN_EMAIL_ARROW_Y			  10
#define SN_EMAIL_ARROW_W			  12
#define SN_EMAIL_ARROW_H			  11
#define HELP_RSC_CNT					9


  control_t  *p_ctrl, *p_content_bg, *p_content, *p_menu;//;
  control_t  *p_head_bg, *p_bmp;
  cas_mail_body_t *mail_body = NULL;
  cas_mail_header_t *p_mail_header = NULL;
  u16 i;
  u16 uni_str[64];
 // u8 asc_str[32];
  static sn_comm_help_data_t email_help_data; //help bar data
  help_rsc help_item[HELP_RSC_CNT]=
  { 
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_UP},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_ARROW_DOWN},
	 {SN_IDC_HELP_BMAP, 25 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 160 , IDS_READ},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_LEFT},
	 {SN_IDC_HELP_TEXT, 40 , IDS_BACK},
	 {SN_IDC_HELP_BMAP, 30 , IM_SN_ICON_BRACKET_RIGHT},
	 {SN_IDC_HELP_TEXT, 60 , IDS_EXIT}
  };
  p_mail_header = (cas_mail_header_t *)para1;
  mail_body = (cas_mail_body_t *)para2;
  
  OS_PRINTF("open_email_mess_content!!!!!!\n");
  OS_PRINTF("m_id %d\n",p_mail_header->m_id);
  OS_PRINTF("subject %d\n",p_mail_header->subject[p_mail_header->m_id]);
  OS_PRINTF("total_email_num %d\n",mail_body->data_len);
  OS_PRINTF("mail boy data start!!!!!!\n");
  for(i = 0;i < mail_body->data_len;i ++)
  {
    OS_PRINTF("%d",mail_body->data[i]);
    if(i % 16 == 0)
      OS_PRINTF("\n");
  }
  OS_PRINTF("mail boy data end!!!!!!\n"); 
   p_menu = ui_background_create(ROOT_ID_EMAIL_MESS_CONTENT,
                           0, 0,
                           SN_SCREEN_WIDTH, SN_SCREEN_HEIGHT,
                           IDS_MAIL_CONTENT,TRUE);
  ctrl_set_rstyle(p_menu, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);

  //bmp
  DEBUG(DVBCAS_PORTING,INFO,"1---*************EMAIL CONTERNT***************\n");
  p_bmp = ctrl_create_ctrl((u8*)CTRL_BMAP, IDC_EMAIL_BMP,
  							SN_EMAIL_CONTENT_HEAD_TITLE_X+20,SN_EMAIL_CONTENT_HEAD_TITLE_Y+10,
  							21,16,p_menu,0);
  bmap_set_content_by_id(p_bmp, IM_SN_ICON_MAIL_NEW);
  
  //head bg
  DEBUG(DVBCAS_PORTING,INFO,"2---*************EMAIL CONTERNT***************\n");
  p_head_bg = ctrl_create_ctrl((u8*)CTRL_CONT, IDC_EMAIL_SENDER,
  						   SN_EMAIL_CONTENT_HEAD_TITLE_X+50, SN_EMAIL_CONTENT_HEAD_TITLE_Y,
                           EMAIL_CONTENT_HEAD_TITLE_W*2-70,3*SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_menu, 0);
  ctrl_set_rstyle(p_head_bg, RSI_EMAIL_HEAD, RSI_EMAIL_HEAD,RSI_EMAIL_HEAD);
  DEBUG(DVBCAS_PORTING,INFO,"3---*************EMAIL CONTERNT***************\n");
  //sender
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_SENDER,
                           0, 13,
                           SN_EMAIL_HEAD_W,SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl,IDS_CA_SENDER);
  DEBUG(DVBCAS_PORTING,INFO,"4---*************EMAIL CONTERNT***************\n");
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_SENDER_HEAD,
                           SN_EMAIL_HEAD_W+8, 13,
                           EMAIL_CONTENT_HEAD_TITLE_W*2-70-(SN_EMAIL_HEAD_W+8),SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  #ifndef WIN32
  gb2312_to_unicode(p_mail_header->sender, 80, uni_str,40);
  text_set_content_by_unistr(p_ctrl, uni_str); 
  #else
  text_set_content_by_ascstr(p_ctrl, "XXXX");
  #endif
  
  //title
  DEBUG(DVBCAS_PORTING,INFO,"5---*************EMAIL CONTERNT***************\n");
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_TITLE_HEAD,
                           0, SN_EMAIL_CONTENT_HEAD_TITLE_H+15,
                           SN_EMAIL_HEAD_W,SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_TITLE);
  DEBUG(DVBCAS_PORTING,INFO,"6---*************EMAIL CONTERNT***************\n");
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_CONTENT_TITLE_HEAD,
                           SN_EMAIL_HEAD_W+8, SN_EMAIL_CONTENT_HEAD_TITLE_H+15,
                           EMAIL_CONTENT_HEAD_TITLE_W*2-70-(SN_EMAIL_HEAD_W+8),SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  #ifndef WIN32
  gb2312_to_unicode(p_mail_header->subject, 80, uni_str,40);
  text_set_content_by_unistr(p_ctrl, uni_str); 
  #else
  text_set_content_by_ascstr(p_ctrl, "XXXX");
  #endif
/* 
  //time
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_TIME_HEAD,
                           0, SN_EMAIL_CONTENT_HEAD_TITLE_H*2,
                           SN_EMAIL_HEAD_W+8,SN_EMAIL_CONTENT_HEAD_TIME_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_TIME);
  
  p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_CONTENT_TIME_HEAD,
                           SN_EMAIL_HEAD_W+8, SN_EMAIL_CONTENT_HEAD_TITLE_H*2,
                           EMAIL_CONTENT_HEAD_TITLE_W*2-70-(SN_EMAIL_HEAD_W+8),SN_EMAIL_CONTENT_HEAD_TIME_H,
                           p_head_bg, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_offset(p_ctrl,20,0);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  #ifndef WIN32
  sprintf((char *)asc_str, "%.2x%.2x-%.2x-%.2x %.2x:%.2x:%.2x",
          p_mail_header->creat_date[0],
          p_mail_header->creat_date[1],
          p_mail_header->creat_date[2],
          p_mail_header->creat_date[3],
          p_mail_header->creat_date[4],
          p_mail_header->creat_date[5],
          p_mail_header->creat_date[6]);
  text_set_content_by_ascstr(p_ctrl, asc_str); 
  #else
  text_set_content_by_ascstr(p_ctrl, "2013-09-05");
  #endif
*/DEBUG(DVBCAS_PORTING,INFO,"7---*************EMAIL CONTERNT***************\n");
  //mail content bg
  p_content_bg = ctrl_create_ctrl((u8*)CTRL_CONT, IDC_EMAIL_CONTENT_BG,
                           SN_EMAIL_CONTENT_BG_X, SN_EMAIL_CONTENT_BG_Y+2*SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           SN_EMAIL_CONTENT_BG_W,310-2*SN_EMAIL_CONTENT_HEAD_TITLE_H,
                           p_menu, 0);
  ctrl_set_rstyle(p_content_bg, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  DEBUG(DVBCAS_PORTING,INFO,"8---*************EMAIL CONTERNT***************\n");

  //mail content
  p_content = ctrl_create_ctrl((u8*)CTRL_TEXT, IDC_EMAIL_CONTENT,
                           EMAIL_CONTENT_X+36, EMAIL_CONTENT_Y,
                           EMAIL_CONTENT_W-17,SN_EMAIL_CONTENT_BG_H-EMAIL_CONTENT_Y-40-2*SN_EMAIL_CONTENT_HEAD_TITLE_H-20,
                           p_content_bg, 0);
  ctrl_set_keymap(p_content, email_body_text_keymap);
  ctrl_set_proc(p_content, email_body_text_proc);
  ctrl_set_rstyle(p_content, RSI_SN_BG, RSI_SN_BG, RSI_SN_BG);
  text_set_align_type(p_content, STL_LEFT |STL_TOP);
  text_set_font_style(p_content, FSI_COMM_BTN,FSI_COMM_BTN,FSI_COMM_BTN);
  text_set_content_type(p_content, TEXT_STRTYPE_EXTSTR);
  text_enable_page(p_content, TRUE);
  memset(mail_content_str, 0, 512 * sizeof(u16));
  #ifndef WIN32
  gb2312_to_unicode(mail_body->data, 512, mail_content_str,512);  
  #else
  sprintf((u8 *)(mail_body->data),"上海三洲迅驰数字技术有限责任公司成立于2001年，\n"
                      "是专业从事数字电视条件接收系统、\n"
                      "用户管理系统、\n"
                      "运营支撑系统、\n"
                      "呼叫中心以及增值业务的研发、生产、系统集成的高科技企。aphysssss\n"
                      "2004年获国家广电总局数字电视条件接收系统和用户管理系统首批入网测评A级证书，\n"
                      "并在 2007年和2010年都通过了国家广电总局入网测评。\n"
                      "被上海市评为高新技术企业、软件企业、最具活力科技企业。\n"
                      "三洲迅驰szxc yaphysssss szxc yaphysssss szxc yaphysssss szxc yaphysssss szxc yaphysssss szxc yaphysssss szxc yaphysssss szxc yaphysssss\n"
                      "位。",512);
  gb2312_to_unicode(mail_body->data, 512, mail_content_str,512);
  #endif
  text_set_content_by_extstr(p_content, mail_content_str);
  DEBUG(DVBCAS_PORTING,INFO,"9---*************EMAIL CONTERNT***************\n");

  text_reset_param(p_content);
  memset((void*)&email_help_data, 0, sizeof(sn_comm_help_data_t));
  email_help_data.x=0;
  email_help_data.item_cnt=HELP_RSC_CNT;
  email_help_data.y=ctrl_get_height(p_menu)-SN_HELP_H-20;
  email_help_data.offset=110;
  email_help_data.rsc =help_item;
  
  sn_ui_comm_help_create(&email_help_data, p_menu);

  ctrl_default_proc(p_content, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
  OS_PRINTF("id of the mail to change status %d\n",p_mail_header->m_id);
#ifndef WIN32 
  ui_ca_get_info(CAS_CMD_MAIL_CHANGE_STATUS, p_mail_header->m_id, 0);
  #endif
  return SUCCESS;
}

static RET_CODE on_ca_email_content_exit(control_t *p_menu, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_EMAIL_MESS_CONTENT, 0, 0);
 
    #ifndef WIN32 
  ui_ca_get_info(CAS_CMD_MAIL_HEADER_GET, 0, 0);
	  #endif

  return SUCCESS;
}

static RET_CODE on_ca_mail_status_set(control_t *p_menu, u16 msg, u32 para1, u32 para2)
{
	u32 ret;
	ret = para1;
	OS_PRINTF("(change email)ret = %d \n",ret);
	if(ret == SET_MAIL_SUCCESS)
		OS_PRINTF("mail status set success!!\n");
	else
		OS_PRINTF("mail status set failure!!\n");
  return SUCCESS;
}

BEGIN_KEYMAP(email_body_text_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(email_body_text_keymap, NULL)


BEGIN_MSGPROC(email_body_text_proc, text_class_proc)
  ON_COMMAND(MSG_EXIT, on_ca_email_content_exit)
  ON_COMMAND(MSG_CA_MAIL_CHANGE_STATUS,on_ca_mail_status_set)
END_MSGPROC(email_body_text_proc, text_class_proc)

