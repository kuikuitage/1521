#include "ui_common.h"
#include "ui_config.h"
#include "region.h"
#include "pdec.h"
#include "ui_ad_logo_api.h"
#include "ap_pic_play.h"
#include "ui_mainmenu.h"
#include "ads_ware.h"

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif

#if(CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC)
#include "ui_ads_szxc_display.h"
#endif

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
void* p_sub_rgn;
static ap_pic_data_t g_picture_data[2];
static u8 pic_index = 0;
static u8 pic_count = 1;
point_t g_sub_pos;  
BOOL is_pic_stop = TRUE;
u8 g_category = 0;
rect_t g_rect = {0};
u8 g_root_id = 0;
u8 *p_mem = NULL;
u32 g_mem_size = 0;
dec_mode_t dec_mode = 0;

typedef struct
{
  u8* p_pic_add;
  u32 pic_size;
  u32 has_read;
}showing_pic_info_t;

static showing_pic_info_t g_show_pic[2] = {{0}};

void pic_set_init_ret(BOOL ret)
{
  init_ret = ret;
}

void pic_player_init(void)
{
  cmd_t cmd = {0};

  OS_PRINTF("ADV pic_player_init\n");
  #if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)  
  g_mem_size = 2048 * KBYTES;   //for desai 720 * 576 fullscreen ad
#else
  g_mem_size = 512 * KBYTES + PIC_MEM_OFFSET;
#endif
  //g_mem_size = 512 * KBYTES + PIC_MEM_OFFSET;
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
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_EPG);
}

void pic_player_start(ap_pic_data_t *p_pic_data)
{
  cmd_t cmd = {0};

  OS_PRINTF("ADV pic_player_start is_pic_stop[%d]\n",is_pic_stop);
  
  cmd.id = PIC_CMD_START;
  cmd.data1 = pic_count;
  cmd.data2 = (u32)(p_pic_data);

  ap_frm_do_command(APP_PICTURE, &cmd);

  is_pic_stop = FALSE;
}

void pic_player_stop(void)
{
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
  }
}

BOOL ui_is_pic_showing (void)
{
  OS_PRINTF("ADV ui_is_pic_showing is_pic_show[%d]\n",is_pic_show);
  return is_pic_show;
}

BOOL ui_set_pic_show (BOOL show)
{
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
  (*p_len) = g_show_pic[0].pic_size;
  return g_show_pic[0].p_pic_add;
}

 u8 * buffer_read1(u32 *p_len)
{
  (*p_len) = g_show_pic[1].pic_size;
  return g_show_pic[1].p_pic_add;
}
 
void ui_create_adv()
{
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
  UI_PRINTF("[AD_LOGO] g_show_pic = 0x%x \n",g_show_pic);
  if(pic_index > 1)
    pic_index = 1;
  g_show_pic[pic_index].p_pic_add = pic_add;
  g_show_pic[pic_index].pic_size = size;
  g_show_pic[pic_index].has_read = readed;
}

void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h)
{
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
  //u32 i=0;
  OS_PRINTF("ADV ui_adv_set_pic length[0x%x]\n",size);
  if(pic_add == NULL || size == 0)
  {
    OS_PRINTF("ADV p_data = NULL");
    return ERR_FAILURE;
  }
  OS_PRINTF("ui_adv_set_pic size=0x%x\n",size);
  #if 0
  OS_PRINTF("==========================\n");

  for(i=0; i<128; i++)
  {
    OS_PRINTF("0x%02x,",pic_add[i]);
  }
  OS_PRINTF("\n");

  for(i=0; i<128; i++)
  {
    OS_PRINTF("0x%02x,",pic_add[size-i-1]);
  }
  
  OS_PRINTF("\n==========================\n");
  #endif

  if(!is_pic_stop)
  {
    pic_player_stop();
  }

  if(is_pic_show_logo)
  {
    ui_show_logo_by_data(size, pic_add);
    return SUCCESS;
  }

  set_showing_pic_info(pic_add, size, 0);
  OS_PRINTF("\n##debug: ui_adv_set_pic [%d][%d][%d][%d][%d][%d]\n", 
  	pic_index, pic_count, g_rect.left, g_rect.top, g_rect.right, g_rect.bottom);
  g_picture_data[pic_index].file_size = size;
  if (pic_index == 0)
  {
    g_picture_data[pic_index].buf_get = (u32)buffer_read0;
  }
  else
  {
    g_picture_data[pic_index].buf_get = (u32)buffer_read1;
  }
  OS_PRINTF("g_picture_data[pic_index].size = %x",g_picture_data[pic_index].file_size);
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
  OS_PRINTF("***************dec_mode === %d  \n",dec_mode);
  g_picture_data[pic_index].dec_mode = dec_mode;
  pic_index ++;
  OS_PRINTF("***************pic index = %d, pic_count = %d \n",pic_index, pic_count);
  if (pic_index == pic_count)
  { 
    UI_PRINTF("[ui_ads_pic_start] pic start !!!!!!   \n");
    pic_player_start(g_picture_data);
  }
  return SUCCESS;
}

RET_CODE ui_adv_set_float(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data)
{
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
  g_category = index;
}

BOOL ui_check_pic_by_type(ads_ad_type_t type)
{
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

  if(ads_io_ctrl(ADS_ID_ADT_QZ, ADS_IOCMD_AD_PIC_INFO_GET, &module_config) == 1)
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
    dec_mode = mode;
}
  
BOOL ui_adv_set_pic_by_type(ads_ad_type_t type)
{
  ads_module_cfg_t module_config = {0};
  u16 pg_id = 0;
#if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) || (CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
  u32 cmd = 0;
#endif
  dvbs_prog_node_t pg = {0};
  RET_CODE ret = SUCCESS;

  if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
  	OS_PRINTF("progId = %d\n",pg_id);
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      module_config.service_info.service_id = (u16)pg.s_id;
      module_config.service_info.ts_id = (u16)pg.ts_id;
      module_config.service_info.network_id= (u16)pg.orig_net_id;
    }  
  }
  module_config.ad_type = type;
  module_config.pic_in = ADS_PIC_IN_SDRAM;

  OS_PRINTF("service id = %d\n",module_config.service_info.service_id);
  OS_PRINTF("ts id = %d\n", module_config.service_info.ts_id);
  OS_PRINTF("network id = %d\n",module_config.service_info.network_id);
  OS_PRINTF("ad_type = %d\n",module_config.ad_type);

  OS_PRINTF("ADV pic_type[%d}, pic_x[%d], pic_y[%d]\n",module_config.pic_pos.type, module_config.pic_pos.x, module_config.pic_pos.y);

  is_pic_show_logo = FALSE;

  dec_mode = DEC_FRAME_MODE;

  pic_count = 1;
  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
      pic_count = 1;

      g_rect.left = 235;
      g_rect.right = g_rect.left + 250;
      g_rect.top = 358;
      g_rect.bottom = g_rect.top + 112;
      break;

      case ADS_AD_TYPE_CHLIST:
      g_rect.left = 369;//410
      g_rect.right = g_rect.left+280;
      g_rect.top = 338;//385
      g_rect.bottom = g_rect.top+140;
      break;

      case ADS_AD_TYPE_EPG:
      g_rect.left = 369;
      g_rect.right = g_rect.left+280;
      g_rect.top = 338;
      g_rect.bottom = g_rect.top+140;
      break;
      
      case ADS_AD_TYPE_CHBAR:
      g_rect.left = 430;//407
      g_rect.right = g_rect.left+280;//280
      g_rect.top = 378;//335
      g_rect.bottom = g_rect.top+140;//140
      break;
      
      case ADS_AD_TYPE_MENU_DOWN:
      g_rect.left = 500;//407
      g_rect.right = g_rect.left+280;
      g_rect.top = 0;//333
      g_rect.bottom = g_rect.top+140;
      break;

      case ADS_AD_TYPE_SUB_MENU:
      g_rect.left = 500;//60
      g_rect.right = g_rect.left+245;
      g_rect.top = 400;//135
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
      
    case ADS_AD_TYPE_MAIN_MENU:
      g_rect.left = 210;//MAINMENU_AD_X+12
      g_rect.right = g_rect.left+300;//MAINMENU_AD_X+12 +265
      g_rect.top = 303;//MAINMENU_AD_Y-15
      g_rect.bottom = g_rect.top+110;//MAINMENU_AD_Y-15+220
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
      dec_mode = DEC_LINE_MODE;
      break;

    case ADS_AD_TYPE_VOLBAR:
      g_rect.left = 429;
      g_rect.right = g_rect.left+280;
      g_rect.top = 378;
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
#ifndef WIN32
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI)
  ads_desai_set_type(type, &cmd, &module_config);
  ret = ads_io_ctrl(ADS_ID_ADT_DESAI, cmd, &module_config);
  ret = (ret == DSAD_FUN_OK)?SUCCESS : ERR_FAILURE;
#elif(CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC)
  ads_szxc_set_type(type, &cmd, &module_config);
  ret = ads_io_ctrl(ADS_ID_ADT_SZXC, cmd, &module_config);
#else
  ret = ads_display(ADS_ID_ADT_QZ, &module_config);
#endif
#endif
 
  OS_PRINTF("return value =%d\n", ret);

  return (ret==SUCCESS)?TRUE : FALSE;
}

void ui_pic_stop(void)
{
  if(init_ret == TRUE)
  {
    OS_PRINTF("ADV ui_pic_stop\n");
    pic_player_stop();
    
    #if((CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) || (CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC))
      ui_set_full_scr_ad_status(FALSE);
      ui_clear_adv();
    #endif
  }
}

void ui_adv_set_root(u8 root_id)
{
  g_root_id = root_id;
}

u8 ui_adv_get_root(void)
{
  return g_root_id;
}

BOOL ui_pic_play(ads_ad_type_t type, u8 root_id)
{
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
  is_pic_show_logo = is_show;
}

void ads_show_logo(void)
{
  RET_CODE ret = SUCCESS;
  ads_module_cfg_t module_config = {0};
  
  module_config.ad_type = ADS_AD_TYPE_WELCOME;
  module_config.pic_in = ADS_PIC_IN_SDRAM;
  is_pic_show_logo = TRUE;
#ifndef WIN32
  ret = ads_display(ADS_ID_ADT_QZ, &module_config);
#endif
  if(ret == SUCCESS)
  {
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

BEGIN_AP_EVTMAP(ui_picture_evtmap)
  CONVERT_EVENT(PIC_PLAYER_EVT_DRAW_END, MSG_PIC_EVT_DRAW_END)
  CONVERT_EVENT(PIC_PLAYER_UPDATE_FRAME, MSG_PIC_EVT_DRAW_END)
END_AP_EVTMAP(ui_picture_evtmap)

