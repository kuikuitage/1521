#include "ui_common.h"
#include "ui_config.h"
#include "region.h"
#include "pdec.h"
#include "ui_ad_logo_api.h"
#include "ap_pic_play.h"
#include "ui_mainmenu.h"
#include "ads_ware.h"
#include "ads_api_xsm.h"
#include "ui_ads_xsm_display.h"
#include "mt_time.h"


#define PIC_MEM_OFFSET 520
static BOOL is_pic_show = FALSE;
static BOOL is_pic_show_logo = FALSE;
static BOOL init_ret = FALSE;

extern void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate);
extern RET_CODE ads_module_init(void);
extern void ads_show_logo(void);
extern void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos);

u16 ui_picture_evtmap(u32 event);
void pic_player_stop(void);
void ui_clear_adv();
void ui_ads_attach(void);
void* p_sub_rgn;
static ap_pic_data_t g_picture_data[2];

static u8 pic_count = 1;
//static u8 pic_count_roll = 1;

point_t g_sub_pos;  
BOOL is_pic_stop = TRUE;
u8 g_category = 0;
rect_t g_rect = {0};
u8 g_root_id = 0;
u8 *p_mem = NULL;
u32 g_mem_size = 0;
dec_mode_t dec_mode = 0;
u16 cur_ts_id = 0;
u16 cur_service_id = 0;

typedef struct
{
  u8* p_pic_add;
  u32 pic_size;
  u32 has_read;
}showing_pic_info_t;

typedef struct _OSD_TIME_T
{
    u32 year    : 12;
    u32 month   : 4;
    u32 day     : 5;
    u32 hour    : 5;
    u32 minute  : 6;
    u32 second;
} OSD_TIME_T;

static showing_pic_info_t g_show_pic[2] = {{0}};

/*****************************************!
    xinshimao ad info
******************************************/
/*!
    current show ui
*/
typedef enum
{
  NO_PIC_SHOW = 0,
  ROLL_PIC_SHOW,
  FS_PIC_SHOW,//角标广告跟其他广告互斥
  OTHER_PIC_SHOW,
}ads_status_t;

/*!
    global param
*/
extern ads_module_priv_t *p_ads_xsm_priv;

S_YTDY_AD_SHOW_DATA_T s_ad_data; //param of picture showing
S_YTDY_OSD_DATA s_osd_data; //param of osd showing
ads_status_t cur_show_ui = NO_PIC_SHOW;
BOOL fs_on_show = FALSE;
u8  pic_data_buffer[MAX_PIC_AD_SIZE];
static u8 pic_index = 0;

u8 s_rolling_start = 0;
//static u16 s_rolling_x = 0;
static u16 s_roll_y = 500;

//u8  g_roll_rev = 0;
//u8  g_show_fs_ads = 0;

BOOL is_roll_pic_stop = TRUE;
BOOL is_fs_pic_stop = TRUE;
//BOOL is_fs_ads_jpeg = TRUE;//JPEG/GIF


void pic_set_init_ret(BOOL ret)
{
  OS_PRINTF("%s:LINE%d, get in,ret=%d\n", __func__, __LINE__, ret); 
  init_ret = ret;
}

void pic_player_init(void)
{
  OS_PRINTF("%s:LINE%d, get in!\n", __func__, __LINE__); 
  cmd_t cmd = {0};

  OS_PRINTF("ADV pic_player_init\n");
  g_mem_size = 512 * KBYTES + PIC_MEM_OFFSET;
  p_mem = mtos_align_malloc(g_mem_size, 8);
  MT_ASSERT(p_mem != NULL);
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_PICTURE;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_PICTURE, ui_picture_evtmap);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_BAR);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_VOLUME);
  fw_register_ap_msghost(APP_PICTURE, SN_ROOT_ID_MAINMENU);
  fw_register_ap_msghost(APP_PICTURE, SN_ROOT_ID_SUBMENU);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_LIST);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PRO_CLASSIFY);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_GAME_INN);
  //fw_register_ap_msghost(APP_PICTURE, ROOT_ID_SMALL_LIST);

  ui_ads_attach();
}

void pic_player_start(ap_pic_data_t *p_pic_data)
{
  OS_PRINTF("%s:LINE%d, get in, pic_count=[%d], p_pic_data=[0x%x]\n", \
                    __func__, __LINE__, pic_count, (u32)(p_pic_data)); 
  cmd_t cmd = {0};

  OS_PRINTF("%s:LINE%d, is pic stop? yes=[%d]\n", __func__, __LINE__, is_pic_stop);
  
  cmd.id = PIC_CMD_START;
  cmd.data1 = pic_count;
  cmd.data2 = (u32)(p_pic_data);

  ap_frm_do_command(APP_PICTURE, &cmd);
  OS_PRINTF("%s:LINE%d, picture stop!\n", __func__, __LINE__); 
  is_pic_stop = FALSE;
  OS_PRINTF("%s:LINE%d, picture open!\n", __func__, __LINE__); 
}

void pic_player_stop(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  cmd_t cmd = {0};
  if (!is_pic_stop)
  {
    OS_PRINTF("ADV pic_player_stop\n");
    pic_index = 0;
    pic_count = 0;
    cmd.id = PIC_CMD_SYNC_STOP;
    cmd.data1 = APP_PICTURE;
    ap_frm_do_command(APP_PICTURE, &cmd);

    is_pic_stop = TRUE;
    fs_on_show = FALSE;
  }
}

BOOL ui_is_pic_showing (void)
{
  OS_PRINTF("ADV ui_is_pic_showing is_pic_show[%d]\n",is_pic_show);
  return is_pic_show;
}

BOOL ui_set_pic_show (BOOL show)
{
  OS_PRINTF("%s:LINE%d, get in, show=[%d]\n", __func__, __LINE__, show); 
  void* p_dev;

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_dev);

  if(disp_layer_is_show(p_dev, DISP_LAYER_ID_SUBTITL) != show)
  {
    disp_layer_show(p_dev, DISP_LAYER_ID_SUBTITL, show);
    OS_PRINTF("ADV ui_set_pic_show show[%d]\n",show);
    is_pic_show = show; 
  }
  
  if (!show)
  {
     ui_clear_adv();
  }
  return TRUE;
}

u8 * buffer_read0(u32 *p_len)
{
  OS_PRINTF("%s:LINE%d, get in, g_show_pic[0].pic_size=[0x%x], 222222addr=[0x%x]\n", \
                __func__, __LINE__, g_show_pic[0].pic_size, g_show_pic[0].p_pic_add); 
  (*p_len) = g_show_pic[0].pic_size;
  return g_show_pic[0].p_pic_add;
}

 u8 * buffer_read1(u32 *p_len)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  (*p_len) = g_show_pic[1].pic_size;
  return g_show_pic[1].p_pic_add;
}
 
void ui_create_adv()
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  void* p_disp;
  s32 ret;
  rect_size_t rect_size = {0};

  OS_PRINTF("ADV ui_create_adv \n");
  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  rect_size.w = AD_SCREEN_WIDTH; 
  rect_size.h = AD_SCREEN_HEIGHT;
  p_sub_rgn = region_create(&rect_size,PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != p_sub_rgn); 

  g_sub_pos.x = 0;
  g_sub_pos.y = 0;

  ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL, p_sub_rgn, &g_sub_pos, NULL);
  MT_ASSERT(SUCCESS == ret); 

  region_show(p_sub_rgn, TRUE);

  disp_layer_alpha_onoff(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
  region_alpha_onoff(p_sub_rgn, FALSE);

  disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);

  OS_PRINTF("ADV ui_create_adv end\n");
}

void ui_clear_adv()
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
#ifndef WIN32
  void* p_gpd_dev;
  rect_t rc = {0};

  OS_PRINTF("ADV ui_clear_adv \n");
  p_gpd_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpd_dev);

  rc.left = 0;
  rc.top = 0;
  rc.right = AD_SCREEN_WIDTH; 
  rc.bottom = AD_SCREEN_HEIGHT;

  gpe_draw_rectangle_vsb(p_gpd_dev, p_sub_rgn, &rc, 0x00000000);
#endif
}


void set_showing_pic_info(u8* pic_add, u32 size, u32 readed)
{
    OS_PRINTF("%s:LINE%d, get in, pic_add=[0x%x],index=[%d],size=[0x%x]\n", \
                    __func__, __LINE__, (u32)pic_add, pic_index, size); 
    UI_PRINTF("[AD_LOGO] g_show_pic=[0x%x]\n",g_show_pic);
    if(pic_index > 1)
        pic_index = 1;
    
    g_show_pic[pic_index].p_pic_add = pic_add;
    g_show_pic[pic_index].pic_size = size;
    g_show_pic[pic_index].has_read = readed;
}


BOOL osd_time_check(OSD_TIME_T *osd_time)
{
    s8 ret = -1;
    utc_time_t osd_endtime = { 0 };
    utc_time_t current_time = { 0 };
    
    if(NULL == osd_time)
        return FALSE;
    
    osd_endtime.year = osd_time->year;
    osd_endtime.month = osd_time->month;
    osd_endtime.day = osd_time->day;
    osd_endtime.hour = osd_time->hour;
    osd_endtime.minute = osd_time->minute;
    osd_endtime.second = (u8)osd_time->second;
    OS_PRINTF("%s:LINE%d, \nyear=[%d],\nmonth=[%d],\nday=[%d],\nhour=[%d],\nminute=[%d],\nsecond=[%d]\n", 
                                                    __func__, __LINE__, osd_endtime.year, osd_endtime.month, osd_endtime.day, \
                                                    osd_endtime.hour, osd_endtime.minute, osd_endtime.second); 

    if(time_get(&current_time, FALSE))
    {
        OS_PRINTF("%s:LINE%d, get curr time OK!\nyear=[%d],\nmonth=[%d],\nday=[%d],\nhour=[%d],\nminute=[%d],\nsecond=[%d]\n", 
                                                    __func__, __LINE__, current_time.year, current_time.month, current_time.day, \
                                                    current_time.hour, current_time.minute, current_time.second); 
        ret = time_cmp(&current_time, &osd_endtime, FALSE);
        OS_PRINTF("%s:LINE:%d, ret=[%d]!\n", __func__, __LINE__, ret);
        if(ret == -1)
            return 1;
        else
            return 0;
    }
    else
    {
        OS_PRINTF("%s:LINE:%d, get current time ERROR!\n", __func__, __LINE__);
        return 0;
    }
}


RET_CODE ads_adt_ad_show(E_YTDY_AD_SHOW_TYPE show_type,u16 service_id, u16 ts_id)
{
    OS_PRINTF("%s:LINE%d, get in, show_type=[0x%x],service_id=[%d],ts_id=[%d]\n", 
                                                    __func__, __LINE__, show_type, service_id, ts_id); 
    RET_CODE ret = -1;
    YTDY_AD_ERR_E nRet = 0;
	OS_PRINTF("ATTENTION ad_type!!!!\n");
    if(AD_ROLL_PIC == show_type)
    {
        ST_ADS_OSD_PROGRAM_SHOW_INFO p_osd_info = {{{0,},},};
        S_YTDY_OSD_DATA *p_osd_data = NULL;
        
        cur_show_ui = ROLL_PIC_SHOW;
		
/*************************tmp*************************/
//        p_osd_data = YTDY_AD_Get_osd_data(service_id, ts_id);
/*************************tmp*************************/

        if(NULL == p_osd_data)
        {
            OS_PRINTF("%s:LINE:%d, there are no osd data!\n", __func__, __LINE__);
            return ERR_FAILURE;
        }
        else
        {
            if(osd_time_check((OSD_TIME_T *)&p_osd_data->font_end_time))
            {
                OS_PRINTF("%s:LINE:%d, begin to show osd data!\n", __func__, __LINE__);
                memcpy(&p_osd_info.stTextShow, p_osd_data, sizeof(S_YTDY_OSD_DATA));
                ret = ui_show_xsm_osd_ad((u8 *)&p_osd_info);
                
                return ret;
            }
            else
            {
                OS_PRINTF("%s:LINE:%d, the osd is overtime!\n", __func__, __LINE__);
                return ERR_FAILURE;
            }
        }
    }
    else
    {
        memset(&s_ad_data, 0 ,sizeof(S_YTDY_AD_SHOW_DATA_T));
        s_ad_data.p_pic_data = pic_data_buffer;
        OS_PRINTF("%s:LINE:%d, p_pic_data=[0x%x]\n", __func__, __LINE__, (u32)s_ad_data.p_pic_data);

        nRet = YTDY_AD_GetPicture(show_type, service_id, ts_id, &s_ad_data);
        if(nRet == YTDY_AD_SUCCESS )
        {
            OS_PRINTF("%s:LINE:%d, type=[%x],start_x=[%d],start_Y=[%d],width=[%d], height=[%d],size=[0x%x],format=[%d]\n", \
                                                        __func__, __LINE__, show_type, s_ad_data.start_x, s_ad_data.start_y, \
                                                        s_ad_data.width, s_ad_data.height, \
                                                        s_ad_data.size, s_ad_data.pic_format);

            s_ad_data.type = show_type;
    
            if(show_type==AD_FS_RIGHT)
            {
                cur_show_ui = FS_PIC_SHOW;
                fs_on_show = TRUE;
                //ui_adv_set_category_index(fs_index);           
                g_rect.left = s_ad_data.start_x;
                g_rect.right = s_ad_data.start_x + s_ad_data.width;
                g_rect.top = s_ad_data.start_y;
                g_rect.bottom = s_ad_data.start_y + s_ad_data.height;

                OS_PRINTF("fs ads left = %d, right = %d, top = %d, bottom = %d\n",
                                            g_rect.left,g_rect.right,g_rect.top,g_rect.bottom);
                
                if(g_rect.bottom > s_roll_y)
                {
                    g_rect.top -= g_rect.bottom - s_roll_y;
                    g_rect.bottom = s_roll_y;

                    OS_PRINTF("adjust top:%d and bottom:%d\n",g_rect.top,g_rect.bottom);
                }
            }
            else if(show_type==AD_OPEN_LOGO)
            {
                OS_PRINTF("%s:LINE:%d, ad open logo!\n", __func__, __LINE__);
                cur_show_ui = OTHER_PIC_SHOW;
                //ui_adv_set_category_index(pic_index);
                g_rect.left = s_ad_data.start_x;
                g_rect.right = s_ad_data.start_x + s_ad_data.width;
                g_rect.top = s_ad_data.start_y;
                g_rect.bottom = s_ad_data.start_y + s_ad_data.height;
            }
            else
            {
                cur_show_ui = OTHER_PIC_SHOW;
                //ui_adv_set_category_index(pic_index);
            }

            ret  = p_ads_xsm_priv->read_gif(s_ad_data.size, s_ad_data.p_pic_data);
            OS_PRINTF("ads_adt_ad_show read_gif   ret = %d\n",nRet);

            return ret;
        }
        else
            OS_PRINTF("ads_adt_ad_show  get ad data != YTDY_AD_SUCCESS ret = %d\n",nRet);
    }

    return ret;
}

void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  if(x > (AD_SCREEN_WIDTH - w))
    x = AD_SCREEN_WIDTH - w;
  
  if(y > (AD_SCREEN_HEIGHT- h))
    y = AD_SCREEN_HEIGHT - h;
  
  if(x < AD_SCREEN_WIDTH/2)
    x += 20;
  else
    x -= 23;
  
  if(y < AD_SCREEN_HEIGHT/2)
    y += 10;
  else
    y -= 10;
  
  g_rect.left = x;
  g_rect.right = g_rect.left + w;
  g_rect.top = y;
  g_rect.bottom = g_rect.top + h;
  pic_count = 1;
}

RET_CODE ui_adv_set_pic(u32 size, u8 *pic_add)
{
    OS_PRINTF("%s:LINE%d, get in, length=[0x%x]\n", __func__, __LINE__, size); 

    if(pic_add == NULL || size == 0)
    {
        OS_PRINTF("ADV p_data = NULL");
        return ERR_FAILURE;
    }

    OS_PRINTF("%s:LINE%d, is pic stop? yes=[%d]\n", __func__, __LINE__, is_pic_stop);
    if(!is_pic_stop)
    {
        pic_player_stop();
    }

    if(is_pic_show_logo)
    {
        OS_PRINTF("%s:LINE%d, show logo!\n", __func__, __LINE__);
        ui_show_logo_by_data(size, pic_add);
        //YTDY_AD_Show_open_logo_over();
        return SUCCESS;
    }

    set_showing_pic_info(pic_add, size, 0);
    
    OS_PRINTF("\n##debug: ui_adv_set_pic [%d][%d][%d][%d][%d][%d]\n", \
                        pic_index, pic_count, g_rect.left, g_rect.top, g_rect.right, g_rect.bottom);
    
    g_picture_data[pic_index].file_size = size;
    if (pic_index == 0)
    {
        OS_PRINTF("%s:LINE:%d; 111111buf_get=[0x%x]\n", __func__, __LINE__, g_picture_data[pic_index].buf_get);
        g_picture_data[pic_index].buf_get = (u32)buffer_read0; //(u32)buffer_read0;
    }
    else
    {
        g_picture_data[pic_index].buf_get = (u32)buffer_read1;
    }
    OS_PRINTF("%s:LINE:%d;\nleft=[%d]\ntop=[%d]\nright=[%d]\nbottom=[%d]\np_mem=[0x%x]\ng_memsize=[0x%x]\n", \
                                        __func__, __LINE__, g_rect.left, g_rect.top, g_rect.right, g_rect.bottom, p_mem, g_mem_size);
    
    g_picture_data[pic_index].p_rgn = p_sub_rgn;
    g_picture_data[pic_index].rect.left = g_rect.left;
    g_picture_data[pic_index].rect.top = g_rect.top;
    g_picture_data[pic_index].rect.right= g_rect.right;
    g_picture_data[pic_index].rect.bottom = g_rect.bottom;
    g_picture_data[pic_index].file_or_mem = FALSE;

    if (pic_index == 0)
    {
        g_picture_data[pic_index].p_mem = p_mem;
        g_picture_data[pic_index].mem_size = g_mem_size;
        if ((4 * (g_rect.right - g_rect.left) * (g_rect.bottom - g_rect.top) + PIC_MEM_OFFSET) > g_mem_size)
        {
            OS_PRINTF("%s:LINE:%d; size overflow\n");
            return ERR_FAILURE;
        }
    }
    else
    {
        g_picture_data[pic_index].p_mem = p_mem + g_mem_size / 2;
        g_picture_data[pic_index].mem_size = g_mem_size / 2;
        if ((4 * (g_rect.right - g_rect.left) * (g_rect.bottom - g_rect.top) + PIC_MEM_OFFSET) > g_mem_size / 2)
        {
            return ERR_FAILURE;
        }
    }
    OS_PRINTF("***************dec_mode= [%d],index=[%d]\n",dec_mode, pic_index);
    
    g_picture_data[pic_index].dec_mode = dec_mode;
    if((++pic_index) == 2)
        pic_index =1;
    
    if ((pic_index==pic_count))
    { 
        UI_PRINTF("[ui_ads_pic_start] pic start !!!!!!   \n");
        pic_player_start(g_picture_data);
    }
    return SUCCESS;
}

RET_CODE ui_adv_set_float(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  u8 *p_content[16];
  u8 i = 0;
  u16 uni_str[512] = {0};
  if(p_data == NULL)
  {
    OS_PRINTF("ADV p_data = NULL");
    return ERR_FAILURE;
  }
  p_content[0] = p_data + p_playsize[0] * 5;
  for(i = 1;i < floatnum;i++)
  {
    p_content[i] = p_content[i - 1] + p_contentlen[i - 1] + p_playsize[i] * 5;
  }
  if(p_playsize[0] != 0 || p_contentlen[0] != 0)
  {
    gb2312_to_unicode(p_content[0], (s32)p_contentlen[0], uni_str, (s32)p_contentlen[0]);
    on_desktop_start_roll(uni_str, 0, 0, 0);
  }
  return SUCCESS;
}

void ui_adv_set_category_index(u8 index)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  g_category = index;
}

BOOL ui_check_pic_by_type(ads_ad_type_t type)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  ads_module_cfg_t module_config = {0};
  u16 pg_id = 0;
  dvbs_prog_node_t pg = {0};
  BOOL ret = FALSE;

  if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      OS_PRINTF("ADV_CHECK ui_adv_is_pic_exist_by_type SID[%d] TS_ID[%d]\n",pg.s_id, pg.ts_id);
      module_config.service_info.service_id = (u16)pg.s_id;
      module_config.service_info.ts_id = (u16)pg.ts_id;
    }  
  }

  module_config.ad_type = type;
  module_config.pic_in = ADS_PIC_IN_SDRAM;

  if(ADS_AD_TYPE_BANNER == type)
  {
    module_config.service_info.service_id = (u16)pg.s_id;
    OS_PRINTF("ADV_CHECK pg.s_id[%d]\n", pg.s_id);
  }

  if(ads_io_ctrl(ADS_ID_ADT_XSM, ADS_IOCMD_AD_PIC_INFO_GET, &module_config) == 1)
  {
    OS_PRINTF("ADV_CHECK pic_type[%d}, pic_x[%d], pic_y[%d]\n",module_config.pic_pos.type, module_config.pic_pos.x, module_config.pic_pos.y);
    ret = TRUE;
  }
  else
  {
    OS_PRINTF("ADV_CHECK no such pic\n");
  }

  return ret;
}

void set_dec_mode(u32 mode)
{
    OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
    dec_mode = mode;
}
  
BOOL ui_adv_set_pic_by_type(ads_ad_type_t type)
{
  ads_module_cfg_t module_config = {0};
  u16 pg_id = 0;
  //u32 cmd = 0;
  OS_PRINTF("%s:LINE%d, get in, type=[%d]\n", __func__, __LINE__, type); 
  dvbs_prog_node_t pg = {0};
  RET_CODE ret = SUCCESS;

  if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
    OS_PRINTF("%s:LINE%d, program_id=[%d]\n", __func__, __LINE__, pg_id); 
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      OS_PRINTF("ADV ui_adv_set_pic_by_type SID[%d] TS_ID[%d] NET_ID[%d]\n",pg.s_id, pg.ts_id,pg.orig_net_id);
      cur_service_id = module_config.service_info.service_id = (u16)pg.s_id;
      cur_ts_id = module_config.service_info.ts_id = (u16)pg.ts_id;
      module_config.service_info.network_id= (u16)pg.orig_net_id;
    }  
  }

  module_config.ad_type = type;
  module_config.pic_in = ADS_PIC_IN_SDRAM;

  //if(ADS_AD_TYPE_BANNER == type)
  //{
  //  module_config.service_info.service_id = g_category+1;
  //  OS_PRINTF("ADV category+1[%d]\n",g_category+1);
  //}

  OS_PRINTF("ADV pic_type[%d}, pic_x[%d], pic_y[%d]\n",module_config.pic_pos.type, module_config.pic_pos.x, module_config.pic_pos.y);

  is_pic_show_logo = FALSE;

  //dec_mode = DEC_ALL_MODE;
  dec_mode = DEC_FRAME_MODE;
  pic_count = 1;
  
  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
      g_rect.left = 226;
      g_rect.right = g_rect.left + 250;
      g_rect.top = 344;
      g_rect.bottom = g_rect.top + 112;
      break;

      case ADS_AD_TYPE_MENU_DOWN:
      g_rect.left = 407;
      g_rect.right = g_rect.left+280;
      g_rect.top = 333;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_MAIN_MENU:
      g_rect.left = MAINMENU_AD_X+12;
      g_rect.right = MAINMENU_AD_X+12 +265;
      g_rect.top = MAINMENU_AD_Y-15;
      g_rect.bottom = MAINMENU_AD_Y-15+220;
      break;

    case ADS_AD_TYPE_CHBAR:
      g_rect.left = 407;
      g_rect.right = g_rect.left+280;
      g_rect.top = 335;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_SUB_MENU:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 135;
      g_rect.bottom = g_rect.top+110;
      break;
      
    case ADS_AD_TYPE_SEARCHING:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 90;
      g_rect.bottom = g_rect.top+110;
      break;
      
    case ADS_AD_TYPE_SIGNAL:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 160;
      g_rect.bottom = g_rect.top+110;
      break;

    case ADS_AD_TYPE_BANNER:
      g_rect.left = 417;
      g_rect.right = g_rect.left+280;
      g_rect.top = 350;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_OTHER_GIF:
      g_rect.left = 42;
      g_rect.right = g_rect.left+280;
      g_rect.top = 375;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_CHANGE_CHANNEL:
      g_rect.left = 0;
      g_rect.right = AD_SCREEN_WIDTH - g_rect.left;
      g_rect.top = 0;
      g_rect.bottom = AD_SCREEN_HEIGHT - g_rect.top;
      break;

    case ADS_AD_TYPE_VOLBAR:
      g_rect.left = 428;
      g_rect.right = g_rect.left+280;
      g_rect.top = 335;
      g_rect.bottom = g_rect.top+140;
      break;
      
    case ADS_AD_TYPE_WELCOME:
      g_rect.left = 0;
      g_rect.right = AD_SCREEN_WIDTH ;
      g_rect.top = 0;
      g_rect.bottom = AD_SCREEN_HEIGHT ;
      break;
    case ADS_AD_TYPE_CHLIST:
      g_rect.left = 410;
      g_rect.right = g_rect.left+280;
      g_rect.top = 385;
      g_rect.bottom = g_rect.top+140;
      break;
    default:
      g_rect.left = 0;
      g_rect.right = AD_SCREEN_WIDTH - g_rect.left;
      g_rect.top = 0;
      g_rect.bottom = AD_SCREEN_HEIGHT - g_rect.top;
      break;
  }
    
  OS_PRINTF("start show picture tickets2=%d\n", mtos_ticks_get());
#if 0
  ads_xsm_set_type(type, &cmd, &module_config);
  ret = ads_io_ctrl(ADS_ID_ADT_XSM, cmd, &module_config);
  ret = (ret == DSAD_FUN_OK)?SUCCESS : ERR_FAILURE;
#else
  ret = ads_display(ADS_ID_ADT_XSM, &module_config);
#endif

 
  OS_PRINTF("ui_adv_set_pic_by_type ret = %d\n", ret); 

  return (ret==SUCCESS)?TRUE : FALSE;
}

void ui_pic_stop(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  if(init_ret == TRUE)
  {
    OS_PRINTF("ADV ui_pic_stop\n");
    pic_player_stop();
    
    #if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
      ui_set_full_scr_ad_status(FALSE);
      ui_clear_adv();
    #endif
    //ui_set_pic_show(FALSE);
  }
}

void ui_adv_set_root(u8 root_id)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  g_root_id = root_id;
}

u8 ui_adv_get_root(void)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  return g_root_id;
}

BOOL ui_pic_play(ads_ad_type_t type, u8 root_id)
{
  OS_PRINTF("%s:LINE%d, get in, type=[%d],root_id=[%d],init_ret=[%d]\n", \
                    __func__, __LINE__, type, root_id, init_ret); 
  BOOL ret = FALSE;
  if(init_ret == TRUE)
  {
    OS_PRINTF("ADV ui_pic_play type[%d], root_id[%d]\n",type,root_id);
    pic_player_stop();
    if(ui_adv_set_pic_by_type(type))
    {
      ui_adv_set_root(root_id);
      ret = TRUE;
    }
  }
  return ret;
}

BOOL ui_pic_float()
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  ads_module_cfg_t module_config = {0};
  u16 pg_id = 0;
  dvbs_prog_node_t pg = {0};
  BOOL ret = FALSE;
  if(init_ret == TRUE)
  {
    if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
    {
      if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
      {
        OS_PRINTF("ADV ui_adv_set_pic_by_type SID[%d] TS_ID[%d]\n",pg.s_id, pg.ts_id);
        module_config.service_info.service_id = (u16)pg.s_id;
        module_config.service_info.ts_id = (u16)pg.ts_id;
      }  
    }

    module_config.ad_type = ADS_AD_TYPE_OSD;
    module_config.pic_in = ADS_PIC_IN_SDRAM;
    ret = ads_display(ADS_ID_ADT_QZ, &module_config);

    OS_PRINTF("ui_pic_float ret = %d\n", ret); 

    return (ret==SUCCESS)?TRUE : FALSE;
  }
  return ret;
}

void ui_ads_set_show_logo(BOOL is_show)
{
  OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
  is_pic_show_logo = is_show;
}

void ads_show_logo(void)
{
    OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
    RET_CODE ret = SUCCESS;
    ads_module_cfg_t module_config = {0};

    module_config.ad_type = ADS_AD_TYPE_WELCOME;
    module_config.pic_in = ADS_PIC_IN_SDRAM;
    is_pic_show_logo = TRUE;
    
#ifndef WIN32
    ret = ads_display(ADS_ID_ADT_XSM, &module_config);
#endif
    if(ret == SUCCESS)
    {
        OS_PRINTF("%s:LINE%d, ret == SUCCESS\n", __func__, __LINE__); 
        //mtos_task_sleep(3000);// added by mengshuai , the open logo show time = 3s
        //void* p_dev;

        //p_dev  = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);
        //MT_ASSERT(NULL != p_dev);

        //disp_layer_show(p_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

        //ui_set_pic_show(TRUE);
        //is_pic_show_logo = TRUE;
        //mtos_task_sleep(1000);
    }
    else
    {
#ifdef OTA_DM_ON_STATIC_FLASH
        if(get_sys_static_boot_flag() != 1)
#else
        if(get_boot_logo_flag() != 1)
#endif  
            ui_show_logo(LOGO_BLOCK_ID_M0);
    }
}

#if 0
RET_CODE ui_adv_get_event(u16 show_type, u8 state)
{
    OS_PRINTF("%s:LINE:%d, get in, show_type=[0x%x],state=[%d]\n", __func__, __LINE__, show_type, state);
    #if 1
    if(show_type == AD_ROLL_TEXT)
    {
        g_roll_rev = state; //show or hide
        OS_PRINTF("%s:LINE:%d, g_roll_rev=[%d]\n", __func__, __LINE__, state);
    }
    else if(show_type == AD_FS_RIGHT)
    {
      g_show_fs_ads = state; //show or hide
      OS_PRINTF("%s:LINE:%d, g_show_fs_ads=[%d]\n", __func__, __LINE__, state);
    }
    #endif
    return 0;
}
#endif

void roll_ads_monitor(void *p_param)
{
    OS_PRINTF("%s:LINE%d, get in\n", __func__, __LINE__); 
    while(1)
    {
        mtos_task_sleep(5000);

        OS_PRINTF("%s:LINE%d, focus_id=[%d], cur_show_ui=[%d], ca_rolling_status=[%d]\n", \
                        __func__, __LINE__, fw_get_focus_id(), cur_show_ui, get_ca_msg_rolling_status()); 

        if((TRUE == get_ca_msg_rolling_status()) && (ROOT_ID_BACKGROUND == fw_get_focus_id()))
        {
            OS_PRINTF("rollrollrollrollrollrollroll  roll pic display start!\n");

            if( SUCCESS == ads_adt_ad_show(AD_ROLL_PIC,cur_service_id, cur_ts_id))//AD_ROLL_TEXT
            {
                //ui_set_pic_show(TRUE);
                OS_PRINTF("FS RIGHT ADS DISPLAY SUCCESS\n");
            }
            else
                OS_PRINTF("fs right ads diplay faild!\n");
        }
        else
            OS_PRINTF("%s:LINE%d, do not need roll!\n", __func__, __LINE__);
    }
}


void fs_ads_monitor(void *p_param)
{
  OS_PRINTF("%s:LINE%d, get in!\n", __func__, __LINE__); 
  while(1)
  {
    mtos_task_sleep(5000);
    OS_PRINTF("fs_ads_monitor start!\n");

    OS_PRINTF("%s:LINE%d, focus_id=[%d], cur_show_ui=[%d],fs_on_show=[%d]\n", \
                    __func__, __LINE__, fw_get_focus_id(), cur_show_ui, fs_on_show); 
    
    if((ROOT_ID_BACKGROUND == fw_get_focus_id()) && (cur_show_ui != FS_PIC_SHOW) && (fs_on_show != TRUE))//全屏状态(如何判断)，显示角标
    {
      OS_PRINTF("AAAAAAAAAAAAAAA  FS RIGHT ADS DISPLAY start\n");
      pic_count = 1;
      if( SUCCESS == ads_adt_ad_show(AD_FS_RIGHT,cur_service_id, cur_ts_id))
      {
         //ui_set_pic_show(TRUE);
         OS_PRINTF("FS RIGHT ADS DISPLAY SUCCESS\n");
      }
    }
    else if(cur_show_ui == FS_PIC_SHOW)//当前为角标状态，库提示不需要显示
    {
      OS_PRINTF("BBBBBBBBBBBBBBBB FS RIGHT ADS DISPLAY start\n");
      //ui_set_pic_show(FALSE);
    }
    else if(is_fs_pic_stop == FALSE)//通过此标记判断，角标数据解析完成后，关闭对应解码器
    {
      OS_PRINTF("CCCCCCCCCCCCCCCC FS RIGHT ADS DISPLAY STOP filter\n");
      is_fs_pic_stop = TRUE;
      //pic_player_stop(fs_index);
      pic_player_stop();
    }
    OS_PRINTF("cur_show_ui:%d, is_fs_pic_stop:%d\n", cur_show_ui, is_fs_pic_stop);
  }
}

void ui_ads_attach(void)
{
    s16 ret = FALSE;
    u32 stack_size  = 32 * KBYTES;
    u32* p_stack_roll = NULL;
    u32 *p_stack_fs = NULL;
    void* p_dev = NULL;
    OS_PRINTF("ui_ads_attach\n");

    p_stack_fs = (u32 *)mtos_malloc(stack_size);
    MT_ASSERT(p_stack_fs != NULL);

    ret = mtos_task_create((u8 *)"fs_monitor",
                                         (void *)fs_ads_monitor,//挂角广告 jpeg/gif
                                         NULL,
                                         AD_TASK_PRIO_END - 1, //59
                                         p_stack_fs,
                                         stack_size);
    if(!ret)
    {
        OS_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    p_stack_roll = (u32 *)mtos_malloc(stack_size);
    MT_ASSERT(p_stack_roll != NULL);

    ret = mtos_task_create((u8 *)"roll_monitor",
                                         (void *)roll_ads_monitor,//滚动字幕广告 jpeg
                                         NULL,
                                         AD_TASK_PRIO_END - 2, //58
                                         p_stack_roll,
                                         stack_size);
    if(!ret)
    {
        OS_PRINTF("%s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    if(is_pic_show_logo)
    {
        //ui_set_pic_show(FALSE);
        OS_PRINTF("%s %d is_pic_show_logo=[%d] \n",__func__,__LINE__, is_pic_show_logo);
        p_dev  = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);
        MT_ASSERT(NULL != p_dev);
        disp_layer_show(p_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
    }
    OS_PRINTF("%s %d  show logo over! \n",__func__,__LINE__);
    is_pic_show_logo = FALSE;
}

BEGIN_AP_EVTMAP(ui_picture_evtmap)
  CONVERT_EVENT(PIC_PLAYER_EVT_DRAW_END, MSG_PIC_EVT_DRAW_END)
  //CONVERT_EVENT(PIC_PLAYER_EVT_DRAW_FAIL, MSG_PIC_EVT_DRAW_FAIL)
  CONVERT_EVENT(PIC_PLAYER_UPDATE_FRAME, MSG_PIC_EVT_DRAW_END)
  //CONVERT_EVENT(PIC_DRAW_DYNAMIC_END, MSG_PIC_EVT_DRAW_FAIL)
END_AP_EVTMAP(ui_picture_evtmap)

