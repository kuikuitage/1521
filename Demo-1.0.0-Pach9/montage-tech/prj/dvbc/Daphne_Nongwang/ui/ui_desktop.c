/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_prog_bar.h"
#include "ui_channel_warn.h"
#include "ui_nvod_api.h"
#include "ui_browser_api.h"
#include "ui_email_picture.h"
#if((CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_XSM)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_QL))
#include "ui_ca_finger.h"
#endif
#include "ui_ca_public.h"
#include "ui_manual_search.h"
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
#include "ui_finger_print.h"
#endif
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
#include "ui_ads_ds_display.h"
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
#include "ui_conditional_accept_feed_passive.h"
#include "ui_ca_urgency_broadcast.h"
#include "ui_ca_force_msg.h"
#endif
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC)
#include "ui_ads_szxc_display.h"
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
#include "ui_ca_urgent.h"
#include "dvbca_cas_adapter.h"
#include "ui_ca_public.h"
#endif
#include "sys_dbg.h"
enum local_msg
{
  MSG_START_TTX = MSG_LOCAL_BEGIN + 100,
  MSG_STOP_TTX,
  MSG_STOP_BROWSER,
  MSG_SWITCH_SCART,
  MSG_EDIT_UCAS,
  MSG_SWITCH_PN,
  MSG_SWITCH_AUDIO,
  MSG_EXIT_CA_ROLLING,
  MSG_HOTKEY_GREEN,
  MSG_HOTKEY_BLUE,
  MSG_HOTKEY_RED,
  MSG_HOTKEY_YELLOW,
  MSG_HOTKEY_SEARCH,
  #ifdef WIN32
  MSG_CA_TEST1,
  MSG_CA_TEST2,
  MSG_CA_TEST3,
  MSG_CA_TEST4,
  MSG_TEST3,
  #endif
//  MSG_DURATION_TIMEOUT,
};

enum background_menu_ctrl_id
{
  IDC_BG_MENU_CA_ROLL_TOP = 1,
  IDC_BG_MENU_CA_ROLL_BOTTOM,
  IDC_OVERDUE,
  IDC_OVERDUE_ID,
};
enum ca_rolling_mode
{
  CA_ROLL_MODE_TIMES = 1,
};

#if(CONFIG_CAS_ID==CONFIG_CAS_ID_TR)
BOOL g_son_card_need_feed_msg = FALSE;
u8 g_ca_pin[UI_MAX_PIN_LEN];
extern void ui_ca_set_sid(cas_sid_t *p_ca_sid);
#endif

extern void pic_player_init(void);
extern void ui_restore_view(void);
extern void ui_cache_view(void);

u16 ui_desktop_keymap_on_normal(u16 key);

u16 ui_desktop_keymap_on_vcr(u16 key);

u16 ui_desktop_keymap_on_standby(u16 key);


RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);

typedef struct bmpfile_header {
  u32 filesz;
  u16 reserve1;
  u16 reserve2;
  u32 bmp_offset;
}BMP_HEADER;

typedef struct dib_header{
  u32 header_sz;
  s32 width;
  s32 height;
  u16 nplanes;
  u16 bitspp;
  u32 compress_type;
  u32 bmp_bytesz;
  s32 hres;
  s32 vres;
  u32 ncolors;
  u32 nimpcolors;
}DIB_HEADER;

static bitmap_t bit_map;

#define AD_PROGBAR_BLOCK_ID 0xc1
#define DES_COLOR_DEPTH     16

static void FIT_ENDIAN_U16(unsigned short *value)
{
#ifndef WIN32
  unsigned short orig = *value;
  char * buf = (char *)value;

  buf[0] = (orig) & 0xFF;
  buf[1] = (orig) >> 8L;
#endif
}

static void FIT_ENDIAN_U32(unsigned long *value)
{
#ifndef WIN32
  unsigned long orig = *value;
  char * buf = (char *)value;

  buf[0] = (char)(orig & 0xFF);
  buf[1] = (char)(orig >> 8L);
  buf[2] = (char)(orig >> 16);
  buf[3] = (char)(orig >> 24);
#endif
}

#if(CONFIG_CAS_ID==CONFIG_CAS_ID_TR)
static RET_CODE on_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(para1 == V_KEY_UP || para1 == V_KEY_DOWN)
  {
    if(fw_find_root_by_id(ROOT_ID_PROG_LIST) == NULL && fw_find_root_by_id(ROOT_ID_FAV_LIST) == NULL)
    {
      ui_comm_pwdlg_close();
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
      fw_notify_root(fw_find_root_by_id(ROOT_ID_PROG_BAR), NOTIFY_T_KEY, FALSE, (u16)para1, para2, 0);
    }
    else if(fw_find_root_by_id(ROOT_ID_PROG_LIST))
    {
      ui_comm_pwdlg_close();
      fw_notify_root(fw_find_root_by_id(ROOT_ID_PROG_LIST), NOTIFY_T_KEY, FALSE, (u16)para1, para2, 0);
    }
    else if(fw_find_root_by_id(ROOT_ID_FAV_LIST))
    {
      ui_comm_pwdlg_close();
      fw_notify_root(fw_find_root_by_id(ROOT_ID_FAV_LIST), NOTIFY_T_KEY, FALSE, (u16)para1, para2, 0);
    }
    else
    {
      fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, (u16)para1, para2, 0);
      ui_comm_pwdlg_close();
    }
  }
  else
  {
    ui_comm_pwdlg_close();
  }
#if 0 //bug 18051
  if(para1 != V_KEY_UP && para1 != V_KEY_DOWN)
  {
    if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
     manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
  }
  #endif
  return SUCCESS;
}

static RET_CODE on_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  #ifdef PATULOUS_FUNCTION_CA
  s8 i =0;
  for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
  {
    g_ca_pin[i] = para1%10;
    para1 = para1/10;
  }
  #if 0
  UI_PRINTF("on_pwdlg_correct para1=%d\n",para1);
  UI_PRINTF("============\n");
  for(i=0; i<UI_MAX_PIN_LEN; i++)
  {
    UI_PRINTF("%d\n",g_ca_pin[i]);
  }
  UI_PRINTF("============\n");
  #endif
//  ui_ca_do_cmd(CAS_CMD_UNLOCK_PIN_SET, (u32)g_ca_pin, 0);
  #endif
  return SUCCESS;
}
#endif

static BOOL BmpReader(const u8* bmpaddr, const u32 size, BMP_HEADER* p_bmp_header, DIB_HEADER* p_dib_header)
{
  const u8* p = bmpaddr;

  if(*p != 'B' || *(p+1) != 'M')
  {
    return FALSE;
  }

  p += 2; //skip magic
  memcpy(p_bmp_header, p, sizeof(BMP_HEADER));
 
  FIT_ENDIAN_U32(&p_bmp_header->filesz);
  FIT_ENDIAN_U16(&p_bmp_header->reserve1);
  FIT_ENDIAN_U16(&p_bmp_header->reserve2);
  FIT_ENDIAN_U32(&p_bmp_header->bmp_offset);

  if(p_bmp_header->filesz != size)
  {
    return FALSE;
  }

  p += sizeof(BMP_HEADER);
  memcpy(p_dib_header, p, sizeof(DIB_HEADER));
  
  FIT_ENDIAN_U32(&p_dib_header->header_sz);
  FIT_ENDIAN_U32((u32*)&p_dib_header->width);
  FIT_ENDIAN_U32((u32*)&p_dib_header->height);
  FIT_ENDIAN_U16(&p_dib_header->nplanes);
  FIT_ENDIAN_U16(&p_dib_header->bitspp);
  FIT_ENDIAN_U32(&p_dib_header->compress_type);
  FIT_ENDIAN_U32(&p_dib_header->bmp_bytesz);
  FIT_ENDIAN_U32((u32*)&p_dib_header->hres);
  FIT_ENDIAN_U32((u32*)&p_dib_header->vres);
  FIT_ENDIAN_U32(&p_dib_header->ncolors);
  FIT_ENDIAN_U32(&p_dib_header->nimpcolors);
  
  return TRUE;
}

static void common_bmp_pic_read(void)
{
  static BOOL bReaded = FALSE;
  u32 block_size = 0, i=0, j=0, k=0;
  BOOL bRet = FALSE;
  u16 c = 0;
  BMP_HEADER bmp_header;
  DIB_HEADER dib_header;
  u8 *p_ad_pic_add = NULL;
  u8 *p_color_data = NULL;
  u8 *lpDst = NULL;
  u8 *lpSrc = NULL;
  u32 des_pitch = 0;
  u32 SrcLineByteCnt = 0;
  

  if(!bReaded)
  {    
    memset(&bit_map, 0, sizeof(bitmap_t));
    
    block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), AD_PROGBAR_BLOCK_ID);

    if(block_size>0)
    {
      p_ad_pic_add = mtos_malloc(block_size);
      MT_ASSERT(p_ad_pic_add != NULL);
      dm_read(class_get_handle_by_id(DM_CLASS_ID), AD_PROGBAR_BLOCK_ID, 0, 0, block_size, p_ad_pic_add);

      bRet = BmpReader((unsigned char*)p_ad_pic_add, block_size, &bmp_header, &dib_header);
      if(!bRet)
      {
        mtos_free(p_ad_pic_add);
        p_ad_pic_add = NULL;
        return;
      }

      SrcLineByteCnt = (dib_header.width * dib_header.bitspp + 7)/8;
      SrcLineByteCnt = ((SrcLineByteCnt + 1)/2)*2;

      des_pitch = ((((dib_header.width * DES_COLOR_DEPTH) + 7)/8)*8)/8;

      bit_map.pitch = des_pitch;
      bit_map.format = COLORFORMAT_ARGB1555;
      bit_map.enable_ckey = 0;
      bit_map.bpp = DES_COLOR_DEPTH;
      bit_map.width = dib_header.width;
      bit_map.height = dib_header.height; 
      bit_map.colorkey = 0xFA00FF;
      bit_map.is_font = 0;
      bit_map.p_bits = mtos_malloc(des_pitch*dib_header.height);

      lpDst = bit_map.p_bits;

      //point to the end line of the img becuase the img data is reversed
      p_color_data = (u8*)p_ad_pic_add + bmp_header.bmp_offset;
      lpSrc = p_color_data + (dib_header.height-1) * SrcLineByteCnt;


      switch(dib_header.bitspp)
      {
        case 16:
          #if 0
          switch(dib_header.compress_type)
          {
            case 0://BI_RGB
              for(i=0; i<dib_header.height; i++)
              {
                for(j=0, k=0; j<dib_header.width; j++, k+=2)
                {
                  #ifndef WIN32
                  c = *(u16 *)(lpSrc+k);
                  c = (1<<15) | (c & 0x7C00) |  (c & 0x03E0) | (c & 0x001F);
                  #else
                  c = *(u16 *)(lpSrc+k);
                  c = ((c & 0x7C00)>>10<<11) |  ((c & 0x03E0)>>5<<6 ) | (c & 0x001F);
                  #endif

                  *(u16 *)(lpDst+j*2) = c;
                }
                lpDst += des_pitch;
                lpSrc -= SrcLineByteCnt;
              }

              bReaded = TRUE;
              break;

            default:
              break;
          }
          #endif
          break;
          
        case 24:
          for(i=0; i<dib_header.height; i++)
          {
            for(j=0, k=0; j<dib_header.width; j++, k+=3)
            {
              #ifndef WIN32
              c = (1<<15) | (((lpSrc[k+2]&0XF8)>>3)<<10) | (((lpSrc[k+1]&0XF8)>>3)<<5) | (((lpSrc[k]&0xF8)>>3)<<0);
              #else
              c = (((lpSrc[k+2]&0XF8)>>3)<<11) | (((lpSrc[k+1]&0XFC)>>2)<<5) | ((lpSrc[k]&0xF8)>>3);
              #endif

              *(u16 *)(lpDst+j*2) = c;
            }
            lpDst += des_pitch;
            lpSrc -= SrcLineByteCnt;
          }
          bReaded = TRUE;
          break;
        
        default:
          break;
      }

      mtos_free(p_ad_pic_add);
      p_ad_pic_add = NULL;
    }
  }
}

BOOL bmp_pic_draw(control_t *p_ctrl)
{
  BOOL bRet = FALSE;

  if(bit_map.p_bits != NULL)
  {
    bmap_set_content_type(p_ctrl, TRUE);
    bmap_set_content_by_data(p_ctrl,&bit_map);

    bRet = TRUE;
  }
  else
  {
    bmap_set_content_by_id(p_ctrl, IM_SN_AD_PIC);
  }
  
  return bRet;
}

RET_CODE ui_desktop_proc_on_normal(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
static BOOL g_vcr_input_is_tv = FALSE;

//is the sleep timer has one minute left.
static BOOL g_is_one_min_left = FALSE;
static BOOL g_is_uiokey_disable = FALSE;
static BOOL g_is_force_channel = FALSE;

#ifdef WIN32
  static finger_msg_t finger_info_ft = {{0x95, 0x7f, 0x5e, 0x7f, 0x1f, 0xc2, 0x13, 0xb2, 0xfa, 0x1c},21,10,100,1,{0,10,0,0,0x00882244,0xff000000,90}};
#endif


#define OSD_ROLL_BAK_COUNT     5
#define OSD_ROLL_BAK_CNT_LEN   512
#define OSD_ROOL_MAX_LEN     (OSD_ROLL_BAK_CNT_LEN - 2)
typedef struct osd_roll_info_s {
	u16 valid;
	u16 msg[OSD_ROLL_BAK_CNT_LEN];
	u16 repeat;     //repeat 优先于duration
	u16 duration;
}osd_roll_info_t;
typedef struct osd_roll_state_s {
	int is_busy;
	osd_roll_info_t info[OSD_ROLL_BAK_COUNT];
}osd_roll_state_t;

// just top and buttom
static osd_roll_state_t osd_rolling[2];

static BOOL g_is_finger_show = FALSE;
static BOOL g_is_osd_stop_msg_unsend = FALSE;
#if((CONFIG_CAS_ID == CONFIG_CAS_ID_DS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG))
static finger_msg_t g_ca_finger_msg = {{0,},0,};
#endif
static update_t update_info = {0};
static BOOL is_already_prompt_ota = FALSE;

static BOOL g_disable_display_time = FALSE;

BOOL ui_get_disable_display_time_status(void)
{
  UI_PRINTF("&g_disable_display_time[0x%x] = %d \n,",&g_disable_display_time, g_disable_display_time);
  return g_disable_display_time;
}
void ui_set_disable_display_time_status(void)
{
   g_disable_display_time = TRUE;
}

void ui_clean_all_menus_do_nothing(void)
{
    /* close all menu */
    manage_enable_autoswitch(FALSE);
    fw_destroy_all_mainwin(TRUE);
    manage_enable_autoswitch(TRUE);
}
void ui_sleep_timer_create(void)
{
  utc_time_t sleep_time;
  u32 tm_out;
  BOOL is_sleep_on;

  sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);

  if(is_sleep_on)
  {
    sys_status_get_sleep_time(&sleep_time);

    printf_time(&sleep_time, "sleep timer create");

    tm_out = (((sleep_time.hour * 60) + sleep_time.minute - 1) * 60000);

    OS_PRINTF("tm out %d\n", tm_out);

    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out, FALSE);
  }
}

void ui_sleep_timer_destory(void)
{
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT);
}

void ui_sleep_timer_reset(void)
{
  utc_time_t sleep_time;
  u32 tm_out;
  BOOL is_sleep_on;

  sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);

  if(is_sleep_on)
  {
    sys_status_get_sleep_time(&sleep_time);

    printf_time(&sleep_time, "sleep timer create");

    tm_out = (((sleep_time.hour * 60) + sleep_time.minute - 1) * 60000);

    fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out);
  }
}

void uio_reset_sleep_timer(void)
{
  if(g_is_one_min_left)
  {
    //destory one minute timer.
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_POWER_OFF);

    //create sleep timer by sleep time.
    ui_sleep_timer_create();
  }
  else
  {
    ui_sleep_timer_reset();
  }

  g_is_one_min_left = FALSE;
}

static void enter_standby(u32 tm_out)
{
  cmd_t cmd;

  cmd.id = AP_FRM_CMD_STANDBY;
  cmd.data1 = tm_out;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}


static void reset_screen(BOOL is_ntsc)
{
#if 0
  rect_t orc;
  s16 x, y;
  BOOL is_9pic = FALSE;
  u8 prv_root_id = ui_get_preview_menu();

  gdi_set_enable(FALSE);

  if (prv_root_id != ROOT_ID_INVALID)
  {
    if(manage_find_preview(prv_root_id, &orc) != INVALID_IDX)
    {
      avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
        orc.left, orc.top, orc.right, orc.bottom);
    }
  }

  x = is_ntsc ? SCREEN_POS_NTSC_L : SCREEN_POS_PAL_L;
  y = is_ntsc ? SCREEN_POS_NTSC_T : SCREEN_POS_PAL_T;

  gdi_move_screen(x, y);
  gdi_set_enable(TRUE);
#endif
}


static BOOL menu_open_conflict(control_t *p_curn, u8 new_root)
{
  if (fw_find_root_by_id(new_root) != NULL)  //this menu has been opened
  {
    return FALSE;
  }

  if(ctrl_get_ctrl_id(p_curn) == ROOT_ID_BACKGROUND)
  {
    return TRUE;
  }

  if (ui_is_fullscreen_menu(ctrl_get_ctrl_id(p_curn)))
  {
    if (ctrl_process_msg(p_curn, MSG_EXIT, 0, 0) != SUCCESS)
    {
      fw_destroy_mainwin_by_root(p_curn);
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static RET_CODE on_sleep_tmrout(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  utc_time_t curn_time, one_minute = {0};
  utc_time_t tmp_s, tmp_e;
  rect_t sleep_notify =
  {
    420, 10, 620, 70,
  };
  book_pg_t tmp_book;
  BOOL ret = FALSE;
  u8 i;

  OS_PRINTF("sleep time out\n");


  //Destory sleep timer.
  ui_sleep_timer_destory();

  //To Check if any booked pg is playing.
  time_get(&curn_time, FALSE);
  one_minute.minute = 1;
  time_add(&curn_time, &one_minute);

  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_book);

    memcpy(&tmp_s, &(tmp_book.start_time), sizeof(utc_time_t));
    memcpy(&tmp_e, &(tmp_book.start_time), sizeof(utc_time_t));
    time_add(&tmp_e, &(tmp_book.drt_time));

    if((time_cmp(&curn_time, &tmp_s, FALSE) >= 0)
    && (time_cmp(&curn_time, &tmp_e, FALSE) < 0)
    && tmp_book.pgid != INVALIDID)
    {
      printf_time(&tmp_s, "tmp start");
      printf_time(&tmp_e, "tmp end");
      OS_PRINTF("pgid %d, i %d\n", tmp_book.pgid, i);
      ret = TRUE;
    }
  }

  OS_PRINTF("ret %d\n", ret);

  //No booked pg is playing.
  if(!ret)
  {
    g_is_one_min_left = TRUE;

    //Set notify for on minute left.
    ui_set_notify(&sleep_notify, NOTIFY_TYPE_STRID, IDS_ONE_MIN_LEFT);

    //Create a one minute timer for power off.
    OS_PRINTF("create one min timer\n");
    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 60000, FALSE);
  }

  return SUCCESS;
}


static RET_CODE on_power_off(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u8 index = MAX_BOOK_PG;
  u32 tm_out = 0;
  book_pg_t node;
  utc_time_t loc_time = {0};

  switch(fw_get_focus_id())
  {
#if 0
    case ROOT_ID_UPGRADE:
      /* don't respond POWER key */
      return SUCCESS;

    case ROOT_ID_SATCODX:
      /* don't respond POWER key */
      return SUCCESS;

    case ROOT_ID_OTA_SEARCH:
      /* don't respond POWER key */
      return SUCCESS;
#endif
    case SN_ROOT_ID_SEARCH_RESULT:
      /* ask for enter standby on prog scan */
      if (!do_search_is_finish())
      {
        comm_dlg_data_t dlg_data = {0};
        dlg_ret_t ret;

        // pause at first
        ui_pause_scan();

        // opend dlg
        dlg_data.x = COMM_DLG_X, dlg_data.y = COMM_DLG_Y;
        dlg_data.w = COMM_DLG_W, dlg_data.h = COMM_DLG_H;
        dlg_data.title_style = DLG_FOR_ASK | DLG_STR_MODE_STATIC;
        dlg_data.title_strID = IDS_ASK_FOR_STANDBY;

        ret = ui_comm_dlg_open(&dlg_data);

        if (ret != DLG_RET_YES)
        {
          ui_resume_scan();
          return SUCCESS;
        }
      }
      break;
    //case ROOT_ID_UPGRADE:
      //if(ui_is_upgrading())
      //{
      //  return SUCCESS;
      //}
      //break;
    default:
      ;
  }

#ifdef WIN32
  /* pre standby */
  gdi_set_enable(FALSE);

  /* change keymap */
  fw_set_keymap(ui_desktop_keymap_on_standby);

  /* close all menu */
  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);


  /* kill autosleep tmr */

  /* release apps */
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  ui_release_signal();

  /* clear history */
  ui_clear_play_history();

  //to get wake up time.
  index = book_get_latest_index();
  if(index != MAX_BOOK_PG)
  {
    book_get_book_node(index, &node);

    time_get(&loc_time, FALSE);
    if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
    {
      book_overdue_node_handler(index);
      index = book_get_latest_index();
      if(index != MAX_BOOK_PG)
      {
        book_get_book_node(index, &node);

        time_get(&loc_time, FALSE);
        loc_time.second = 0;
        OS_PRINTF("index %d\n", index);
        printf_time(&loc_time, "loc time");
        printf_time(&(node.start_time), "start time");
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
      }
    }
    else
    {
      time_get(&loc_time, FALSE);
      loc_time.second = 0;
      OS_PRINTF("index %d\n", index);
      printf_time(&loc_time, "loc time");
      printf_time(&(node.start_time), "start time");
      tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
    }
  }
#else

  /* do nothing but send cmd to ap to enter standby */

  //to get wake up time.
  index = book_get_latest_index();
  if(index != MAX_BOOK_PG)
  {
    book_get_book_node(index, &node);

    time_get(&loc_time, FALSE);
    if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
    {
      book_overdue_node_handler(index);
      index = book_get_latest_index();
      if(index != MAX_BOOK_PG)
      {
        book_get_book_node(index, &node);

        time_get(&loc_time, FALSE);
        loc_time.second = 0;
        OS_PRINTF("index %d\n", index);
        printf_time(&loc_time, "loc time");
        printf_time(&(node.start_time), "start time");
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
      }
    }
    else
    {
      time_get(&loc_time, FALSE);
      loc_time.second = 0;
      OS_PRINTF("index %d\n", index);
      printf_time(&loc_time, "loc time");
      printf_time(&(node.start_time), "start time");
      tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
    }
  }

  /* enter standby */
  OS_PRINTF("on power off time out : %d\n", tm_out);
  enter_standby(tm_out);
#endif
  return SUCCESS;
}


static RET_CODE on_power_on(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
#ifndef WIN32 /* jump to start address */
  hal_pm_reset();
#else
  fw_set_keymap(ui_desktop_keymap_on_normal);
  manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }

  ui_play_curn_pg();
  ui_init_signal();
  update_signal();
  mtos_task_delay_ms(1 * SECOND);
  
  gdi_set_enable(TRUE);
#endif
  /* set front panel */

  return SUCCESS;
}

static void update_fp_on_scart_switch(BOOL input_is_tv, BOOL is_detected)
{
  if (input_is_tv)
  {
    ui_set_front_panel_by_str("TV");
  }
  else
  {
    if (is_detected)
    {
      ui_set_front_panel_by_str("VCR");
    }
    else
    {
      // set frontpanel
      ui_set_frontpanel_by_curn_pg();
    }
  }
}

static RET_CODE on_scart_switch(control_t *p_ctrl, u16 msg,
                                 u32 para1, u32 para2)
{
  BOOL is_detected = avc_detect_scart_vcr_1(class_get_handle_by_id(AVC_CLASS_ID));

  g_vcr_input_is_tv = !g_vcr_input_is_tv;

  if (!is_detected)
  {
    avc_cfg_scart_select_tv_master_1(class_get_handle_by_id(AVC_CLASS_ID),
      g_vcr_input_is_tv ? SCART_TERM_VCR : SCART_TERM_STB);
  }

  avc_cfg_scart_vcr_input_1(class_get_handle_by_id(AVC_CLASS_ID),
    g_vcr_input_is_tv ? SCART_TERM_TV : SCART_TERM_STB);
  update_fp_on_scart_switch(g_vcr_input_is_tv, is_detected);

  return SUCCESS;
}


static RET_CODE on_scart_vcr_detected(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  BOOL is_detected = (BOOL)(para1);
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  /* video adc */
  void *display_dev = NULL;
  display_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

  disp_cvbs_onoff(display_dev, 1,  !is_detected);
  disp_component_onoff(display_dev, 1, !is_detected);

  if (is_detected)
  {
    avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_VCR);
    avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_TV);
    // force to CVBS mode
    avc_cfg_scart_format_jazz(avc_handle, SCART_VID_CVBS);
    fw_set_keymap(ui_desktop_keymap_on_vcr);
  }
  else
  {
    av_set_t av_set;
    sys_status_get_av_set(&av_set);
    avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_STB);
    avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_STB);

    // restore by setting
    avc_cfg_video_out_format_jazz(avc_handle,
      sys_status_get_video_out_cfg(av_set.video_output));
    fw_set_keymap(ui_desktop_keymap_on_normal);
  }

  update_fp_on_scart_switch(FALSE, is_detected);
  return SUCCESS;
}

static void do_lnb_recheck(void)
{
  u16 cur_pgid;
  nim_device_t *p_nim_dev= (nim_device_t *)dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_NIM);
  ui_comm_dlg_close();

  dev_io_ctrl(p_nim_dev, NIM_IOCTRL_LNB_SC_PROT_RESTORE, 0);
  dev_io_ctrl(p_nim_dev, NIM_IOCTRL_SET_LNB_ONOFF, NIM_LNB_POWER_ON);

  cur_pgid = sys_status_get_curn_group_curn_prog_id();
  if(cur_pgid != INVALIDID)
  {
    do_lock_pg(cur_pgid);
  }

  ui_time_lnb_check_enable(TRUE);

}

static RET_CODE on_lnb_short(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{

  ui_time_lnb_check_enable(FALSE);
  ui_comm_cfmdlg_open(NULL, IDS_LNB_SHORT, do_lnb_recheck, 0);

  return SUCCESS;
}

static RET_CODE on_update_descramble(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  ui_set_playpg_scrambled(msg == MSG_DESCRAMBLE_FAILED);
  update_signal();
  return SUCCESS;
}
#ifndef WIN32
extern void ota_write_otai(ota_info_t *p_otai);
#endif
static void do_update_software(void)
{
  ota_info_t otai = {0};
  ss_public_t *ss_public;

  ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));

  otai.orig_software_version = ss_public->otai.orig_software_version;
  otai.new_software_version = (u16)update_info.swVersion;

  otai.download_data_pid = update_info.data_pid;

  otai.lockc.tp_freq = update_info.freq;
  otai.lockc.tp_sym = update_info.symbol;
  otai.lockc.nim_modulate = update_info.qam_mode+4;

  otai.sys_mode = SYS_DVBC;

  otai.ota_tri = OTA_TRI_AUTO;

  #if 1
  OS_PRINTF("up_info.freq=%d\n", update_info.freq);
  OS_PRINTF("up_info.symbol=%d\n", update_info.symbol);
  OS_PRINTF("up_info.qam_mode=%d\n", update_info.qam_mode);
  OS_PRINTF("up_info.data_pid=%d\n", update_info.data_pid);
  OS_PRINTF("up_info.ota_type=%d\n", update_info.ota_type);
  OS_PRINTF("up_info.Serial_number_start=%s\n", update_info.Serial_number_start);
  OS_PRINTF("up_info.Serial_number_end=%s\n", update_info.Serial_number_end);
  OS_PRINTF("up_info.swVersion=%d\n", update_info.swVersion);
  OS_PRINTF("up_info.hwVersion=%d\n", update_info.hwVersion);
  OS_PRINTF("==============================\n");
  OS_PRINTF("otai.orig_software_version=%d\n",otai.orig_software_version);
  OS_PRINTF("otai.new_software_version=%d\n",otai.new_software_version);
  OS_PRINTF("otai.download_data_pid=%d\n",otai.download_data_pid);
  OS_PRINTF("otai.lockc.tp_freq=%d\n",otai.lockc.tp_freq);
  OS_PRINTF("otai.lockc.tp_sym=%d\n",otai.lockc.tp_sym);
  OS_PRINTF("otai.lockc.nim_modulate=%d\n",otai.lockc.nim_modulate);
  OS_PRINTF("otai.sys_mode=%d\n",otai.sys_mode);
  OS_PRINTF("otai.ota_tri=%d\n",otai.ota_tri);
  #endif

#ifndef WIN32
  ota_write_otai(&otai);
  hal_pm_reset();
#endif
}

static RET_CODE on_software_update(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{

  OS_PRINTF("on_software_update update is_already_prompt:%d\n", is_already_prompt_ota);

  if(fw_get_focus_id() == ROOT_ID_OTA_SEARCH)
  {
    OS_PRINTF("on_software_update ota searching\n");
    return SUCCESS;
  }

  memcpy((void *)&update_info, (void *)para1, sizeof(update_t));

  if(!is_already_prompt_ota)
  {
    is_already_prompt_ota = TRUE;

    ui_comm_ask_for_dodlg_open(NULL, IDS_UPGRADE_TO_NEW_VER,
                                   do_update_software, NULL, 0);
  }
  return SUCCESS;
}
static RET_CODE on_mute(control_t *p_ctrl, u16 msg,
                        u32 para1, u32 para2)
{
  u8 index;
  index = fw_get_focus_id();
  if (index != SN_ROOT_ID_SEARCH_RESULT
       && do_search_is_finish()
       /*&& !ui_is_upgrading()*/)
  {
    ui_set_mute(!ui_is_mute());
  }

  return SUCCESS;
}


static RET_CODE on_pause(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u8 index;

  index = fw_get_focus_id();
  if ((index != ROOT_ID_NUM_PLAY && ui_is_fullscreen_menu(index))
      || (index == ROOT_ID_PROG_LIST)
      || (index == ROOT_ID_EPG))
  {
    // only respond the key on tv mode
    if (sys_status_get_curn_prog_mode() == CURN_MODE_TV)
    {
      ui_set_pause(!ui_is_pause());
    }
    else
    {
      return ERR_NOFEATURE;
    }
  }

  return SUCCESS;
}


static RET_CODE on_open_in_tab(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u32 vkey = para1;
  u8 new_root = (u8)(msg & MSG_DATA_MASK);;
  if((fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL) && (vkey != V_KEY_0) && (vkey != V_KEY_1)
  	&& (vkey != V_KEY_2) && (vkey != V_KEY_3) && (vkey != V_KEY_4) && (vkey != V_KEY_5) && (vkey != V_KEY_6)
  	&& (vkey != V_KEY_7) && (vkey != V_KEY_8) && (vkey != V_KEY_9))// shield all KEYs if pwd open
  {
	return SUCCESS;
  }
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  if(!vkey_is_need(vkey))
  {
    return SUCCESS;
  }
  #endif
  if (menu_open_conflict(fw_get_focus(), new_root))
  {
    return manage_open_menu(new_root, vkey, 0);
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_close_menu(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  manage_autoclose();
  return SUCCESS;
}
extern BOOL fav_list_save_data(void);
static RET_CODE on_close_all_menus(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u8 focus_id = 0;
  customer_cfg_t cfg = {0};
  focus_id = fw_get_focus_id();
  get_customer_config(&cfg);

  if((cfg.customer != CUSTOMER_TAIHUI) &&
           (cfg.customer != CUSTOMER_JINGHUICHENG_QZ))
    {
      return SUCCESS;
    }

  if((focus_id == SN_ROOT_ID_MAINMENU) || (focus_id == ROOT_ID_PROG_LIST)
    || (focus_id == ROOT_ID_FAV_LIST) || (focus_id == SN_ROOT_ID_SUBMENU))
    {
      if(sys_status_get_curn_prog_mode() != CURN_MODE_NONE)
      {
          if(focus_id == ROOT_ID_FAV_LIST)
          {
            fav_list_save_data();
          }
          ui_close_all_mennus();
      }
    }
  return SUCCESS;
}

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  utc_time_t curn_time = {0};
  book_pg_t node;
  u32 tm_out;
  u8 node_state= LATEST_NODE_WAIT;
  u8 latest_index = MAX_BOOK_PG;
  control_t *p_root = NULL;
  u8 ucSkip = 0;
  comm_dlg_data_t dlg_data = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_EXTSTR,
    COMM_DLG_X-40, COMM_DLG_Y-85,
    (COMM_DLG_W + 120),(COMM_DLG_WITHTEXT_H + 30),
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    60000,
  };
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u16 content[256];
  u16 conttext[64];

  rect_t rect =
  {
    217, 60, 417, 120,
  };

  time_get(&curn_time, TRUE);

  //printf_time(&curn_time, "desktop time update");

  //save time.
  sys_status_set_utc_time(&curn_time);

  //time update, delete overdue node & reset latest index.
  book_delete_overdue_node();

  latest_index = book_get_latest_index();
  node_state = book_check_latest_node(latest_index);
  #if 0
  #ifdef WIN32
  node_state = LATEST_NODE_START;
  #endif
  #endif
  if(ui_get_book_flag())
  {
    if(node_state == LATEST_NODE_START)
    {
      if((SUCCESS  == book_check_is_skip(latest_index, &ucSkip))
          &&(1 == ucSkip))
      {
            book_delete_node(latest_index);
            return SUCCESS;
      }
      if(fw_find_root_by_id(ROOT_ID_DIALOG) != NULL) // already opened
      {
        UI_PRINTF("UI: already open a dialog, force close it! \n");
        ctrl_process_msg(fw_find_root_by_id(ROOT_ID_DIALOG), MSG_SELECT, 0, 0);
        fw_notify_root(p_ctrl,NOTIFY_T_MSG,FALSE,msg, para1,para2);
        return SUCCESS;
      }
      book_get_book_node(latest_index, &node);

      gui_get_string(IDS_BOOK_REMIND_CONT1, content, 64);
      uni_strcat(content, node.event_name, 256);
      gui_get_string(IDS_BOOK_REMIND_CONT2, conttext, 64);
      uni_strcat(content, conttext, 256);
      dlg_data.text_content = (u32)content;
      dlg_data.title_strID = IDS_BOOK_REMIND;

      dlg_ret = ui_comm_dlg_open(&dlg_data);
      if(dlg_ret == DLG_RET_YES)
      {

        //try to close all menus and play in fullscreen.
        ui_close_all_mennus();

        if(node.svc_type != SVC_TYPE_NVOD_REFRENCE)
        {
          p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);

          if(p_root == NULL)
          {
            manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
          }

          ui_book_play(node.pgid);

          fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, node.pgid);

          if(ui_is_mute() && (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
          {
            open_mute(0, 0);
          }
        }


        tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;

        if(tm_out == 0)
        {
          book_overdue_node_handler(latest_index);
        }
      }

      OS_PRINTF("====latest node start, drt time %d min======\n",
        (node.drt_time.hour * 60 + node.drt_time.minute));
    }
    else if(node_state == LATEST_NODE_END)
    {
      book_overdue_node_handler(latest_index);

      //to check if there is a book pg start at the same time.
      latest_index = book_get_latest_index();
      node_state = book_check_latest_node(latest_index);
      if(node_state == LATEST_NODE_START)
      {
        OS_PRINTF("====latest node end, next latest start at the same time.======\n");
        if((SUCCESS  == book_check_is_skip(latest_index, &ucSkip))
            &&(1 == ucSkip))
        {
              book_delete_node(latest_index);
              return SUCCESS;
        }

        book_get_book_node(latest_index, &node);

        //try to close all menus and play in fullscreen.
        ui_close_all_mennus();

        p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);

        if(p_root == NULL)
        {
          manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
        }

        ui_book_play(node.pgid);

        fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, node.pgid);

        if(ui_is_mute() && (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
        {
          open_mute(0, 0);
        }

        //book remind.
        ui_set_notify(&rect, NOTIFY_TYPE_STRID, IDS_EVT_NOTIFIED);

        tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;

        if(tm_out == 0)
        {
          book_overdue_node_handler(latest_index);
        }

        OS_PRINTF("====latest node start, drt time %d min======\n",
          (node.drt_time.hour * 60 + node.drt_time.minute));

      }
      else
      {
        OS_PRINTF("====latest node end======\n");
        ui_set_notify(&rect, NOTIFY_TYPE_STRID, IDS_EVT_FINISHED);

      }
    }
  }

  return SUCCESS;
}

static RET_CODE on_check_signal(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  ui_signal_check(para1, para2);
  return SUCCESS;
}

static RET_CODE on_motor_out_range(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  if (fw_find_root_by_id(ROOT_ID_DIALOG) == NULL)
  {
    ui_comm_cfmdlg_open(NULL, IDS_OUT_OF_RANGE, NULL, OUTRANGE_DLG_TMOUT);
  }
  return SUCCESS;
}

static RET_CODE on_heart_beat(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  gui_rolling();

  return SUCCESS;
}

static RET_CODE on_orde_ippv_info(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
  ipps_info_t *p_ippv_info = (ipps_info_t *) para2;
  u8 focus = fw_get_focus_id();
	#ifdef WIN32
		if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
	#else
  	if(p_ippv_info->p_ipp_info[0].type == 1 && (focus == ROOT_ID_BACKGROUND || focus == ROOT_ID_PROG_BAR))
	#endif
#endif
#if(CONFIG_CAS_ID == CAS_ID_LX)
  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
#endif
    {
      update_ca_message(RSC_INVALID_ID);
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
	  manage_open_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME,1,(u32)para2);
#elif(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
	  manage_open_menu(ROOT_ID_CA_IPP,1,(u32)para2);
#else
	  DEBUG(MAIN, INFO, "\n");
	  manage_open_menu(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
#endif
    }
  return SUCCESS;
}

static RET_CODE on_hide_ippv_info(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_YXSB)
	manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
#else
	manage_close_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME,1,(u32)para2);
#endif
  return SUCCESS;
}

static RET_CODE on_update_search(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  comm_dlg_data_t dlg_ack_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, 170,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    10000,
  };

  comm_dlg_data_t dlg_force_data = //popup dialog data
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_NULL,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W,COMM_DLG_H,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    3000,
  };
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u16 content[64];
  u8 index = 0;
  u32 pre_nit_ver = 0;
  u32 new_nit_ver = 0;
  comm_dlg_data_t dlg_data;
  customer_cfg_t cfg = {0};
  
  get_customer_config(&cfg);
  if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
  	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
  	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
    memcpy(&dlg_data,&dlg_force_data,sizeof(comm_dlg_data_t));
  }
  else
  {
     memcpy(&dlg_data,&dlg_ack_data,sizeof(comm_dlg_data_t));
  }
  
  new_nit_ver = para2;
  sys_status_get_nit_version(&pre_nit_ver);
  OS_PRINTF("\r\n*** %s:new_nit_ver[0x%x], pre_nit_ver [0x%x]  ", __FUNCTION__, new_nit_ver, pre_nit_ver );
  if(new_nit_ver == pre_nit_ver)
  {
    #if (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
    sys_status_t *p_sys;
    p_sys = sys_status_get();
    if((p_sys->local_set.longitude == 0) || (p_sys->local_set.longitude == 0xffff))
      sv_nit_area_lock(para1, 1);
    #endif
    return SUCCESS;
  }
  else if(pre_nit_ver == 0xFFFFFFFF)
  {
     sys_status_set_nit_version(new_nit_ver);
    return SUCCESS;
  }

  index = fw_get_focus_id();
  if((SN_ROOT_ID_SEARCH_RESULT == index)
          ||(ROOT_ID_PROGRAM_SEARCH == index)
#if (FRONT_BOARD == FRONT_BOARD_YINGJI)
          ||(ROOT_ID_FULL_SEARCH == index)
#endif
          ||(! do_search_is_finish()))
  {
        return ERR_FAILURE;
  }
  if(cfg.customer == CUSTOMER_YINGJI)
  {
    if(pre_nit_ver != 0xFE && new_nit_ver == 0xFE)
    {
      gui_get_string(IDS_PROG_UPDATE_TO_SEARCH, content, 64);
      dlg_data.text_content = (u32)content;
      dlg_data.title_strID = IDS_PROG_UPDATE;

      dlg_ret = ui_comm_dlg_open(&dlg_data);
      if(dlg_ret != DLG_RET_NO)
      {
        //try to close all menus and play in fullscreen.
        ui_clean_all_menus_do_nothing();
         manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
      }
    }
  }
  else
  {
    gui_get_string(IDS_PROG_UPDATE_TO_SEARCH, content, 64);
    dlg_data.text_content = (u32)content;
    dlg_data.title_strID = IDS_PROG_UPDATE;

    dlg_ret = ui_comm_dlg_open(&dlg_data);
    if(dlg_ret != DLG_RET_NO)
    {
      //try to close all menus and play in fullscreen.
    #if (CONFIG_ADS_ID == CONFIG_ADS_ID_DESAI) 
        if(TRUE == ui_get_full_scr_ad_status())
          ui_pic_stop();
     #endif
      index = fw_get_focus_id();
      if(index != SN_ROOT_ID_SEARCH_RESULT)
      {
        ui_clean_all_menus_do_nothing();
         manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
      }
    }
  }
#if (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  sv_nit_area_lock(para1, 1);
#endif
  return SUCCESS;
}
static RET_CODE on_desai_conditional_search(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
#ifdef CAS_CONFIG_DS_5_0
      u8 index = 0;
      index = fw_get_focus_id();
      

      if((SN_ROOT_ID_SEARCH_RESULT == index) ||
          (SN_ROOT_ID_SEARCH == index)  ||
          (!do_search_is_finish()))
      {
        return ERR_FAILURE;
      }
       //ui_close_all_mennus();
       ui_clean_all_menus_do_nothing();
        manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
#endif
   return SUCCESS;
}

#ifndef CAS_CONFIG_DS_5_0
static void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  p_info->ecm_num = (pg.ecm_num < 8 ? (u8)pg.ecm_num : 8);
  p_info->emm_num = (tp.emm_num < 8 ? (u8)tp.emm_num : 8);

  memcpy(&p_info->ecm_info, &pg.cas_ecm, p_info->ecm_num * sizeof(cas_desc_t));
#if(AP_CAS_ID == CAS_ID_DS)
  if(p_info->ecm_num == 0)
  {
    p_info->ecm_num = 1;
    p_info->ecm_info[0].ecm_pid= 0x1FFF;
  }
#endif
  memcpy(&p_info->emm_info, &tp.emm_info, p_info->emm_num * sizeof(ca_desc_t));
}
#endif

extern void ui_set_smart_card_insert(BOOL p_card_status);
extern cas_operators_info_t *g_get_operator_info();
extern void g_set_operator_info(cas_operators_info_t *p);

static RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
#ifndef CAS_CONFIG_DS_5_0
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  u16 sid;
  u16 curn_id = sys_status_get_curn_group_curn_prog_id();
  cas_sid_t ca_sid;
  
  /*for ds ca 5.1 : desai lib will do play pg when ca initial finished*/

  if (db_dvbs_get_pg_by_id(curn_id, &pg) != DB_DVBS_OK)
  {
    return SUCCESS;
  }

  if (db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    return SUCCESS;
  }

  OS_PRINTF("CA: on ca init ok, set sid : %d\n", pg.s_id);
  sid = (u16)pg.s_id;

  memset(&ca_sid, 0, sizeof(cas_sid_t));

  ca_sid.num = 1;
  ca_sid.pgid = (u16)pg.id;
  ca_sid.pg_sid_list[0] = sid;
  ca_sid.tp_freq = tp.freq;
  ca_sid.v_pid = (u16)pg.video_pid;
  ca_sid.a_pid = pg.audio[pg.audio_channel].p_id;
  ca_sid.pmt_pid = (u16)pg.pmt_pid;
  ca_sid.ca_sys_id = 0x1FFF;
  load_desc_paramter_by_pgid(&ca_sid, ca_sid.pgid);
  ui_ca_set_sid(&ca_sid);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
    ui_set_smart_card_insert(TRUE);
    if(NULL != g_get_operator_info)
      ui_ca_get_info(CAS_CMD_OPERATOR_INFO_GET,0,0);
  #endif
#endif
  return SUCCESS;
}

static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
   ui_set_playpg_scrambled(FALSE);
   update_signal();
   return SUCCESS;
}

void ui_set_finger_show(BOOL is_show)
{
  g_is_finger_show = is_show;
}

BOOL ui_is_finger_show(void)
{
  return g_is_finger_show;
}

RET_CODE open_finger(u32 para1, u32 para2)
{
  UI_PRINTF("redraw finger msg **** \n");
#if((CONFIG_CAS_ID == CONFIG_CAS_ID_DS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG))
  open_ca_finger_menu(&g_ca_finger_msg);
#endif
  return SUCCESS;
}

#ifdef WIN32
static RET_CODE pre_open_finger(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  finger_msg_t p_ca_finger_msg_temp;
  u8 *p_color_data = NULL;
  p_ca_finger_msg_temp.finger_property.m_byLocationFromLeft = -1;
  p_ca_finger_msg_temp.finger_property.m_byLocationFromTop = -1;
  p_ca_finger_msg_temp.during = 30;
  strcpy(p_ca_finger_msg_temp.data, "YAPHYSSSSCA");
  p_ca_finger_msg_temp.data_len = 18;
  on_ca_finger_update(p_ctrl, MSG_CA_SHOW_FINGER, para1, (u32)(&p_ca_finger_msg_temp));
  
  return SUCCESS;
}
static msg_info_t p_ca_msg_info = {0,{0,1,0},{0x31,0x30},2};

static RET_CODE pre_open_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 p_str[512] = {0,};
  p_ca_msg_info.osd_display.roll_mode = 1;
  p_ca_msg_info.osd_display.roll_value = 1;
  p_ca_msg_info.display_pos = 1;
  #ifndef WIN32
  p_ca_msg_info.display_status = 1;
  #endif
  p_ca_msg_info.display_time= 60;
  //memset(p_ca_msg_info.data,'d',512);
  sprintf((char *)p_ca_msg_info.data, "数码");
  p_ca_msg_info.data_len = 513;
  on_ca_rolling_msg_update(p_ctrl, msg, para1, &p_ca_msg_info);
  
  return SUCCESS;
}
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
static RET_CODE on_finger_print(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #ifdef PATULOUS_FUNCTION_CA
  u8 focus_id = 0;
  focus_id = fw_get_focus_id();

  if((focus_id == ROOT_ID_BACKGROUND || focus_id == ROOT_ID_PROG_BAR) && (focus_id != ROOT_ID_FINGER_PRINT ))
  {
    //manage_open_menu(ROOT_ID_FINGER_PRINT, para1, para2);
    open_finger_print(para1, para2);
  }
#endif
  return SUCCESS;
 }
#endif

static RET_CODE on_ca_finger_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  finger_msg_t *p_ca_finger_msg = NULL;
  customer_cfg_t p_customer = {0};
  //DEBUG(MAIN,INFO,"%s------\n",__function__);
  //u8 asc_str[256] = {'1','2','3','4','5','1','2','3','4','5','1','2','3','4','5','1','2','3','4','5'};
  get_customer_config(&p_customer);
  OS_PRINTF("\r\n*** on_ca_finger_update msg[0x%x],  para2 addr[0x%x] ***\r\n",
                  msg, para2);
  switch(msg)
  {
      case MSG_CA_SHOW_FINGER:
        p_ca_finger_msg = (finger_msg_t *)para2;
        #ifndef WIN32
        if(NULL == p_ca_finger_msg)
        {
            return ERR_FAILURE;
        }
        
        OS_PRINTF("\r\n *** finger_msg:data = %s***",p_ca_finger_msg->data);
        OS_PRINTF("\r\n *** finger_msg:postion =%d  :x = %d,y =                 %d***",
                                           p_ca_finger_msg->show_postion,
                                           p_ca_finger_msg->show_postion_x,
                                           p_ca_finger_msg->show_postion_y);
        #endif
        #ifndef WIN32
		#if((CONFIG_CAS_ID!=CONFIG_CAS_ID_XSM)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_GS)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_QL))
        	open_ca_finger_menu(p_ca_finger_msg);
		#endif
        #elif(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
        {
          finger_msg_t *finger_info;
          finger_info = &finger_info_ft;
          open_ca_finger_menu((u32)&finger_info_ft);
        }
        #endif
        
#if((CONFIG_CAS_ID == CONFIG_CAS_ID_DS) || (CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG))
        memcpy(&g_ca_finger_msg, p_ca_finger_msg, sizeof(g_ca_finger_msg));  //don't remove!!
#endif
        
        g_is_finger_show = TRUE;
        break;
      case MSG_CA_HIDE_FINGER:
#if((CONFIG_CAS_ID!=CONFIG_CAS_ID_XSM)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_TR)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_GS)&&(CONFIG_CAS_ID!=CONFIG_CAS_ID_QL))
        close_ca_finger_menu();
#endif
        g_is_finger_show = FALSE;
        break;
      default:
        break;
  }
  DEBUG(MAIN,INFO,"222-------------------------------------------------\n");

   return SUCCESS;
}
#if(CONFIG_ADS_ID == CONFIG_CAS_ID_DVBCA)
static dvbcas_urgent p_urgent;
static RET_CODE on_ca_urgent_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
	dvbcas_urgent *urgent= NULL;
	customer_cfg_t p_customer = {0};
	get_customer_config(&p_customer);
	
	OS_PRINTF("\r\n*** on_ca_urgent_update msg[0x%x],  para2 addr[0x%x] ***\r\n",
	      msg, para2);
	switch(msg)
	{
		case MSG_CA_SHOW_URGENT:
			urgent = (dvbcas_urgent *)para2;
			if(NULL == urgent)
			{
				return ERR_FAILURE;
			}

			OS_PRINTF("\r\n *** urgent_msg:title = %s***",urgent->Title);
			OS_PRINTF("\r\n *** urgent_msg:content =%s  ***",urgent->Content);
			open_ca_urgent(urgent);
			memcpy(&p_urgent, urgent, sizeof(p_urgent));
			break;
		case MSG_CA_HIDE_URGENT:
			close_ca_urgent();
			break;
		default:
			break;
	}

	return SUCCESS;
}
#endif
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_YXSB)
static u16 on_find_pgid_by_sid_in_currview(u32 service_id, u32 ts_id, u16 network_id)
{
  u16 view_count = 0, pg_id = INVALID_PG_ID;
  u8 view_id;
  u8 i = 0;
  dvbs_prog_node_t pg = {0};

  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);

  for(i=0; i<view_count; i++)
  {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
     {
        OS_PRINTF("\r\n***%s, s_id[0x%x], ts_id[0x%x], i[%d]***",
                                    __FUNCTION__, pg.s_id, pg.ts_id, i);
		#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
        	if(pg.s_id == service_id)
        #else
			if((pg.s_id == service_id) && (pg.ts_id == ts_id))
		#endif
	        {
	          #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
	            if(pg.orig_net_id == network_id)
	              return pg_id;
	          #else
	              return pg_id;
	          #endif
	        }
     }
      if(0 == (i%50))
      {
        mtos_task_sleep(10);
      }
  }

  return INVALID_PG_ID;

}

static RET_CODE on_force_channel_check_play(u32 service_id, u32 ts_id, u16 network_id)
{
  u16 pg_id = INVALID_PG_ID;
  u16 cur_pg_id = INVALID_PG_ID;
  u8 view_id;
  u8 focus_id = 0;
  dvbs_view_t e_orignal_view_type = DB_DVBS_INVALID_VIEW;

  view_id = ui_dbase_get_pg_view_id();
  cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  e_orignal_view_type = db_dvbs_get_view_type(view_id);

  pg_id = on_find_pgid_by_sid_in_currview(service_id, ts_id, network_id);
  if(INVALID_PG_ID == pg_id)
  {
      if(DB_DVBS_ALL_TV == e_orignal_view_type)
      {
          (void)ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
          pg_id = on_find_pgid_by_sid_in_currview(service_id, ts_id, network_id);
      }
      else if(DB_DVBS_ALL_RADIO == e_orignal_view_type)
      {
          (void)ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
          pg_id = on_find_pgid_by_sid_in_currview(service_id, ts_id, network_id);
      }
      else
      {
          OS_PRINTF("\r\n*** get view type error!***");
          return ERR_FAILURE;
      }
  }

  if (INVALID_PG_ID != pg_id)
  {
    focus_id = fw_get_focus_id();
    if(!ui_is_fullscreen_menu(focus_id))
    {
      ui_close_all_mennus();
    }
    if(ui_is_mute())
    {
      ui_set_mute(FALSE);
    }
    g_is_force_channel = TRUE;
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)    
    if(pg_id == cur_pg_id)
    {
      if(TRUE == ui_get_ads_osd_status())
      {
        ui_ads_osd_roll_stop();
      }
    }
#endif
    ui_play_prog(pg_id, FALSE);

    if(pg_id != cur_pg_id)
    {
      manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
    }
    return SUCCESS;
  }
  else
  {
      OS_PRINTF("\r\n *** not find the match service id[0x%x]***", service_id);
      //change back to orignal view
      (void)ui_dvbc_change_view(e_orignal_view_type, FALSE);
  }
  return ERR_FAILURE;

}
#endif

static RET_CODE on_ca_force_channel_play(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  cas_force_channel_t *p_ca_force_channel = NULL;
  RET_CODE ret = ERR_FAILURE;
  static u16 pre_pg_id = INVALID_PG_ID;
  customer_cfg_t customer_cfg = {0};
  u8 focus = 0;
  ui_close_all_mennus();
 
  get_customer_config(&customer_cfg);
  OS_PRINTF("\r\n*** %s, msg[0x%x],  para2 addr[0x%x] ***\r\n", __FUNCTION__, msg, para2);

  p_ca_force_channel = (cas_force_channel_t *)para2;

  switch(msg)
  {
      case MSG_CA_FORCE_CHANNEL_INFO:
        if(NULL == p_ca_force_channel)
        {
            return ERR_FAILURE;
        }
        OS_PRINTF("\r\n*** %s, serv_id[0x%x],  ts_id[0x%x] ***\r\n", __FUNCTION__,
                                            p_ca_force_channel->serv_id, p_ca_force_channel->ts_id);
        focus = fw_get_focus_id();
        if(focus == ROOT_ID_CA_IPPV_PPT_DLG)
        {
          manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
          manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
        }
        if(fw_find_root_by_id(ROOT_ID_CA_BROADCAST) == NULL)
        {
          pre_pg_id = sys_status_get_curn_group_curn_prog_id();
        }
		#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
          ret = on_ca_force_channel(p_ctrl, msg, para1, para2);
        #else
        	ret = on_force_channel_check_play(p_ca_force_channel->serv_id, p_ca_force_channel->ts_id, p_ca_force_channel->netwrok_id);
        	if(SUCCESS == ret)
	        {
			  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG) || (CONFIG_CAS_ID == CONFIG_CAS_ID_TR))
	          {
	            rect_t ca_notify_rect =
	            {
	              250, 125,
	              260 + NOTIFY_CONT_W,
	              125 + NOTIFY_CONT_H,
	            };
	             ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_FORCE_PROG);
	          }
	          #elif(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
	          {
	            ui_creat_urgency_broadcast(p_ca_force_channel->duration_time);
	            OS_PRINTF("urgency_broadcast   duration_time : %d\n",p_ca_force_channel->duration_time);
	            #ifdef WIN32
	              fw_tmr_create(ROOT_ID_BACKGROUND,MSG_CA_TEST1,80000,0);
	            #endif
	            g_is_uiokey_disable = TRUE;
	          }
	          #else
	              g_is_uiokey_disable = TRUE;
	          #endif
	        }
			else
			{
				pre_pg_id = INVALID_PG_ID;
			}
		#endif
        break;
      case MSG_CA_UNFORCE_CHANNEL_INFO:
        #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
          if(fw_find_root_by_id(ROOT_ID_CA_BROADCAST)!= NULL)
          {
            OS_PRINTF("desktop close broadcast\n");

            close_broadcast();
          }
        #endif
        g_is_force_channel = FALSE;
        g_is_uiokey_disable = FALSE;
        if(pre_pg_id != INVALID_PG_ID)
        {
          ui_play_prog(pre_pg_id, FALSE);
          pre_pg_id = INVALID_PG_ID;
         }
        break;
      default:
        break;
  }
  update_signal();
  ctrl_paint_ctrl(ctrl_get_root(p_ctrl),FALSE); 
   return SUCCESS;
}

static void on_desktop_start_position_roll(control_t *p_ctrl, u16 *p_uni_str, u32 value)
{
  roll_param_t p_param = {0};
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  gui_stop_roll(p_ctrl);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
  text_set_content_by_unistr(p_ctrl,p_uni_str);
  p_param.pace = ROLL_SINGLE;
  if (cfg.customer == CUSTOMER_ZHILING_KF)
  {
  	p_param.style = ROLL_RL;
  }
  else
  {
  	p_param.style = ROLL_LR;
  }
  
  if(0 == value)
  {
    p_param.repeats= 2; //value;
  }
  else //dure time
  {
    p_param.repeats= value;
  }
  OS_PRINTF("repeat times value : %d, repeats %d\n",value, p_param.repeats);
  gui_start_roll(p_ctrl, &p_param);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

}

#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
  extern u32 time_spent_for_osd_roll(u8 flag);
  void on_desktop_set_uio(BOOL uio_flag)
  {
    g_is_uiokey_disable = uio_flag;
  }
#endif

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
void clear_osd_rolling_buffer(void)
{
	int i=0;
	for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
	{
		osd_rolling[OSD_SHOW_BOTTOM].info[i].valid = 0;
		osd_rolling[OSD_SHOW_TOP].info[i].valid = 0;
	}
	
}
#endif

void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_top_roll = NULL;
  control_t *p_ctrl_bottom_roll = NULL;
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_ctrl_top_roll = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
  p_ctrl_bottom_roll = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
  
  if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
  {
    if(0 == value)
      value = 1; //value;
      DEBUG(MAIN, INFO, "dis_pos:%d\n", dis_pos);
    if((OSD_SHOW_TOP == dis_pos) ||  (OSD_SHOW_TOP_BOTTOM== dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_top_roll, p_uni_str, value);
    }

    if((OSD_SHOW_BOTTOM == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_bottom_roll, p_uni_str, value);
    }
  }    
  else if (CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
  {
	if (value == 0)
		value =1;
	if (OSD_SHOW_TOP== dis_pos)
		p_cont = p_ctrl_top_roll;
	else
		p_cont = p_ctrl_bottom_roll;

	on_desktop_start_position_roll(p_cont, p_uni_str, value);
  }
  else if (CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
  {
	if (value == 0)
		value =1;
	if((OSD_SHOW_TOP == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_top_roll, p_uni_str, value);
    }

    if((OSD_SHOW_BOTTOM == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_bottom_roll, p_uni_str, value);
    }
  }
  else if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
  {
	if (value == 0)
		value =1;
	if((OSD_SHOW_TOP == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_top_roll, p_uni_str, value);
    }

    if((OSD_SHOW_BOTTOM == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_bottom_roll, p_uni_str, value);
    }
  }
  else
  {
	if (value == 0)
		value =1;
	if((OSD_SHOW_TOP == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_top_roll, p_uni_str, value);
    }

    if((OSD_SHOW_BOTTOM == dis_pos))
    {
      on_desktop_start_position_roll(p_ctrl_bottom_roll, p_uni_str, value);
    }
  }
}

static RET_CODE on_ca_rolling_msg_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_YXSB && CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)//-------- ! CONFIG_CAS_ID_YXSB
	msg_info_t *p_ca_msg_info_t = NULL;
	u16 uni_str[512] = {0};//0x673a;
	customer_cfg_t cus_cfg = {0};
	u8 location=0;
	p_ca_msg_info_t = (msg_info_t *)para2;
#endif
	DEBUG(TRCA, INFO, "start\n");
#if(CONFIG_CAS_ID==CONFIG_CAS_ID_TR)
{
	static u8 msg_time[64][CAS_MAIL_DATE_MAX_LEN];
	static u8 msg_new_index = 0;
	u8 i=0;
	msg_info_t cas_msg;
	u8 focus_id = 0;
	BOOL update = TRUE;

	memcpy(&cas_msg, (msg_info_t *)para2, sizeof(msg_info_t));

	UI_PRINTF("on_ca_show_osd =========\n");
	UI_PRINTF("on_ca_show_osd cas_msg.type=%d, cas_msg=%s\n",cas_msg.type, cas_msg.title);
	UI_PRINTF("on_ca_show_osd %04d.%02d.%02d-%02d:%02d:%02d\n",cas_msg.create_time[0]*100 + cas_msg.create_time[1], cas_msg.create_time[2], cas_msg.create_time[3], cas_msg.create_time[4], cas_msg.create_time[5], cas_msg.create_time[6]);
	UI_PRINTF("on_ca_show_osd =========\n");

	if(cas_msg.type == 2/*CAS_MSG_NOTIFICATION*/)
	{
		if(msg_new_index <= 0)
		{
			for(i=0; i<64; i++)
			{
				memset(msg_time[i], 0, CAS_MAIL_DATE_MAX_LEN);
			}
		}

		for(i=0; i<msg_new_index; i++)
		{
			if(memcmp(msg_time[i], cas_msg.create_time, CAS_MAIL_DATE_MAX_LEN) == 0)
			{
				update = FALSE;
				break;
			}
		}

		if(update)
		{
			memcpy(msg_time[msg_new_index], cas_msg.create_time, CAS_MAIL_DATE_MAX_LEN);
			msg_new_index++;
		}

		for(i=0; i<msg_new_index; i++)
		{
			UI_PRINTF("on_ca_show_osd %04d.%02d.%02d-%02d:%02d:%02d\n",msg_time[i][0]*100 + msg_time[i][1], msg_time[i][2], msg_time[i][3], msg_time[i][4], msg_time[i][5], msg_time[i][6]);
		}

		if(msg_new_index >= 64)
		{
			msg_new_index = 0;
		}

		if(!update)
		{
			UI_PRINTF("on_ca_show_osd alread showed\n");
			return SUCCESS;
		}
	}

	focus_id = fw_get_focus_id();
	DEBUG(TRCA, INFO, "focus_id[%d]\n",focus_id);
	if((focus_id == ROOT_ID_BACKGROUND || focus_id == ROOT_ID_PROG_BAR) &&
	(focus_id != ROOT_ID_CA_NOTIFY))
	{
		//fix bug 18556 18553
		if(fw_find_root_by_id(ROOT_ID_PROG_BAR))
		manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		manage_open_menu(ROOT_ID_CA_NOTIFY, 0, (u32)&cas_msg);
	}
	else if((focus_id == ROOT_ID_CA_NOTIFY) && update)
	{
		//fix bug 18556 18553
		if(fw_find_root_by_id(ROOT_ID_PROG_BAR))
		manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		manage_open_menu(ROOT_ID_CA_NOTIFY, 0, (u32)&cas_msg);
	}
	return SUCCESS;
}
#endif

#if(CONFIG_CAS_ID != CONFIG_CAS_ID_YXSB && CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)//-------- ! CONFIG_CAS_ID_YXSB
	get_customer_config(&cus_cfg);
	
	OS_PRINTF("\r\n*** rolling_msg_update  p_ca_msg_info addr[0x%x] ***\r\n", para2);
	if(NULL == p_ca_msg_info_t)
	{
		return ERR_FAILURE;
	}
	OS_PRINTF("\r\n *** rolling_msg:%s***",p_ca_msg_info_t->data);	
	OS_PRINTF("\r\n *** roll_mode[0x%x], roll_value[0x%x],display_pos [0x%x],display_time[%d],display_location[%d]\n",
	              p_ca_msg_info_t->osd_display.roll_mode,  
	              p_ca_msg_info_t->osd_display.roll_value,
	              p_ca_msg_info_t->display_pos,
	              p_ca_msg_info_t->display_time,
	              p_ca_msg_info_t->osd_display.location);
	{
		u32 len = strlen((char*)p_ca_msg_info_t->data);
		int i;
		for (i=0;i<len;i++)
		{
		//	OS_PRINTF("%02x,",p_ca_msg_info->data[i]);
			if (p_ca_msg_info_t->data[i] == 0x0d && p_ca_msg_info_t->data[i+1] == 0x0a)
			{
				p_ca_msg_info_t->data[i] = ' ';
				p_ca_msg_info_t->data[i+1] = ' ';
			}
		}
	}
	DEBUG(MAIN,INFO, "1 location:%d\n", location);
	gb2312_to_unicode(p_ca_msg_info_t->data, 511, uni_str, 511);

	location = p_ca_msg_info_t->osd_display.location;
	MT_ASSERT(location < 2);
	OS_PRINTF("*** location:%d***\n",location);
	OS_PRINTF("*** is_busy:%d***\n",osd_rolling[location].is_busy);
 
	if (osd_rolling[OSD_SHOW_TOP].is_busy == 0
	&& osd_rolling[OSD_SHOW_BOTTOM].is_busy == 0)
	{
	
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)
		on_desktop_start_roll(uni_str,
		                            p_ca_msg_info_t->osd_display.roll_mode,
		                            p_ca_msg_info_t->osd_display.roll_value,
									p_ca_msg_info_t->osd_display.location);
		osd_rolling[location].is_busy = 1; 
#endif
		
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
		u8 i=0;
		for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
		{
			if (osd_rolling[location].info[i].valid == 0)
				break;
			DEBUG(MAIN,INFO, "33----^^^^^^^^^^^^^^^^^^^osd_rolling[i = %d]^^^^^^^^^^^^^^^^^^^^^\n",i);
		}
		if (i != OSD_ROLL_BAK_COUNT) 
		{
			DEBUG(MAIN,INFO, "44----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			memcpy(osd_rolling[location].info[i].msg,uni_str,OSD_ROOL_MAX_LEN);
			osd_rolling[location].info[i].repeat = p_ca_msg_info_t->osd_display.roll_value; //显示次数
		  	osd_rolling[location].info[i].valid = 1;  //是否已显示1:未显示
		}
		on_desktop_start_roll(osd_rolling[location].info[i].msg,
		                            0,
		                            osd_rolling[location].info[i].repeat,
									location);
		DEBUG(MAIN,INFO, "22----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		osd_rolling[location].is_busy = 1; 
		
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			DEBUG(MAIN,INFO, "OSD_ROLL_BAK_COUNT----^^^^^^^^^^^^^^^^^^i= %d,	msg= %s,  repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
									  osd_rolling[location].info[i].msg,
									  osd_rolling[location].info[i].repeat,
									  osd_rolling[location].info[i].valid);
		}

#endif
	}

	else
	{
		u8 i;
		OS_PRINTF("*** busy bak:***\n");
		for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
		{
			if (osd_rolling[location].info[i].valid == 0)
				break;
		}
		if (i != OSD_ROLL_BAK_COUNT) 
		{
			memcpy(osd_rolling[location].info[i].msg,uni_str,OSD_ROOL_MAX_LEN);
			osd_rolling[location].info[i].repeat = p_ca_msg_info_t->osd_display.roll_value;
		  	osd_rolling[location].info[i].valid = 1;
		}
		else 
		{
			OS_PRINTF("No bak buffer to store roll msg!!\n");
		}
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			DEBUG(MAIN,INFO, "OSD_ROLL_BAK_COUNT----^^^^^^^^^^^^^^^^^^i= %d,	msg= %s,  repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
									  osd_rolling[location].info[i].msg,
									  osd_rolling[location].info[i].repeat,
									  osd_rolling[location].info[i].valid);
		}
	}

#endif


DEBUG(MAIN,INFO,"2-------------------------------------------------\n");

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB  || CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)  //--------CONFIG_CAS_ID_YXSB

	msg_info_t *p_ca_msg_info_t = NULL;
	u16 uni_str[512] = {0};//0x673a;
	customer_cfg_t cus_cfg = {0};
	u8 location=0;
	p_ca_msg_info_t = (msg_info_t *)para2;
	get_customer_config(&cus_cfg);
	DEBUG(MAIN,INFO, "1----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	OS_PRINTF("\r\n*** rolling_msg_update  p_ca_msg_info addr[0x%x] ***\r\n", para2);
	if(NULL == p_ca_msg_info_t)
	{
		return ERR_FAILURE;
	}
	OS_PRINTF("\r\n *** rolling_msg:%s***",p_ca_msg_info_t->data);	
	OS_PRINTF("\r\n *** roll_mode[0x%x], roll_value[0x%x],display_pos [0x%x],display_time[%d],display_location[%d]\n",
	              p_ca_msg_info_t->osd_display.roll_mode,  
	              p_ca_msg_info_t->osd_display.roll_value,
	              p_ca_msg_info_t->display_pos,
	              p_ca_msg_info_t->display_time,
	              p_ca_msg_info_t->osd_display.location);
	DEBUG(MAIN,INFO, "2----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	{
		u32 len = strlen((char*)p_ca_msg_info_t->data);
		int i;
		for (i=0;i<len;i++)
		{
		//	OS_PRINTF("%02x,",p_ca_msg_info->data[i]);
			if (p_ca_msg_info_t->data[i] == 0x0d && p_ca_msg_info_t->data[i+1] == 0x0a)
			{
				p_ca_msg_info_t->data[i] = ' ';
				p_ca_msg_info_t->data[i+1] = ' ';
			}
		}
	}
	DEBUG(MAIN,INFO, "3----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	gb2312_to_unicode(p_ca_msg_info_t->data, 511, uni_str, 511);

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
	location = p_ca_msg_info_t->display_pos;
#else
	location = p_ca_msg_info_t->osd_display.location;
#endif
	DEBUG(MAIN,INFO, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^p_ca_msg_info_t->data = %s\n",(char *)p_ca_msg_info_t->data);
	MT_ASSERT(location < 2);
	OS_PRINTF("*** location:%d***\n",location);
	OS_PRINTF("*** is_busy:%d***\n",osd_rolling[location].is_busy);


    DEBUG(MAIN,INFO, "4----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	if(location == OSD_SHOW_TOP)
	{
		if(osd_rolling[OSD_SHOW_TOP].is_busy == 0)
		{
			on_desktop_start_roll(uni_str,
			                        p_ca_msg_info_t->osd_display.roll_mode,
			                        p_ca_msg_info_t->osd_display.roll_value,
									p_ca_msg_info_t->display_pos);
			DEBUG(MAIN,INFO, "5----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			DEBUG(MAIN,INFO, "22---->>>>p_ca_msg_info_t->display_pos=%d,  p_ca_msg_info_t->osd_display.location = %d\n",p_ca_msg_info_t->display_pos);
			osd_rolling[location].is_busy = 1; 	
			DEBUG(MAIN,INFO, "6----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		}
		else
		{
			u8 i;
			OS_PRINTF("*** busy bak:***\n");
			for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
			{
				if (osd_rolling[location].info[i].valid == 0)
					break;
			}
			if (i != OSD_ROLL_BAK_COUNT) 
			{
				memcpy(osd_rolling[location].info[i].msg,uni_str,OSD_ROOL_MAX_LEN);
				osd_rolling[location].info[i].repeat = p_ca_msg_info_t->osd_display.roll_value;//显示次数
			  	osd_rolling[location].info[i].valid = 1;  //是否已显示1:未显
			  	DEBUG(MAIN,INFO, "7----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			}
			else 
			{
				OS_PRINTF("No bak buffer to store roll msg!!\n");
			}
			
			for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
			{			
				DEBUG(MAIN,INFO, "OSD_SHOW_TOP----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,   repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
										  osd_rolling[OSD_SHOW_TOP].info[i].msg,
										  osd_rolling[OSD_SHOW_TOP].info[i].repeat,
										  osd_rolling[OSD_SHOW_TOP].info[i].valid);
			}
		}
	}


	if(location == OSD_SHOW_BOTTOM)
	{
	    DEBUG(MAIN,INFO, "8----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		if(osd_rolling[OSD_SHOW_BOTTOM].is_busy == 0)
		{
			on_desktop_start_roll(uni_str,
			                        p_ca_msg_info_t->osd_display.roll_mode,
			                        p_ca_msg_info_t->osd_display.roll_value,
									p_ca_msg_info_t->display_pos);
		DEBUG(MAIN,INFO, "9----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			DEBUG(MAIN,INFO, "22---->>>>p_ca_msg_info_t->display_pos=%d,  p_ca_msg_info_t->osd_display.location = %d\n",p_ca_msg_info_t->display_pos);
			osd_rolling[location].is_busy = 1; 	
			DEBUG(MAIN,INFO, "10----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		}
		else
		{
			u8 i;
			OS_PRINTF("*** busy bak:***\n");
			for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
			{
				if (osd_rolling[location].info[i].valid == 0)
					break;
			}
			if (i != OSD_ROLL_BAK_COUNT) 
			{
				memcpy(osd_rolling[location].info[i].msg,uni_str,OSD_ROOL_MAX_LEN);
				osd_rolling[location].info[i].repeat = p_ca_msg_info_t->osd_display.roll_value;//显示次数
			  	osd_rolling[location].info[i].valid = 1;  //是否已显示1:未显
			  	DEBUG(MAIN,INFO, "11----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			}
			else 
			{
				OS_PRINTF("No bak buffer to store roll msg!!\n");
			}
			
			for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
			{			
				DEBUG(MAIN,INFO, "OSD_SHOW_TOP----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,   repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
										  osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
										  osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
										  osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
			}
		}
	}


/*
	if (osd_rolling[OSD_SHOW_TOP].is_busy == 0
	&& osd_rolling[OSD_SHOW_BOTTOM].is_busy == 0)
	{

		on_desktop_start_roll(uni_str,
		                            p_ca_msg_info_t->osd_display.roll_mode,
		                            p_ca_msg_info_t->osd_display.roll_value,
									p_ca_msg_info_t->display_pos);
		DEBUG(MAIN,INFO, "22---->>>>p_ca_msg_info_t->display_pos=%d,  p_ca_msg_info_t->osd_display.location = %d\n",p_ca_msg_info_t->display_pos);
		osd_rolling[location].is_busy = 1; 
	}
	else
	{
		u8 i;
		OS_PRINTF("*** busy bak:***\n");
		for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
		{
			if (osd_rolling[location].info[i].valid == 0)
				break;
		}
		if (i != OSD_ROLL_BAK_COUNT) 
		{
			memcpy(osd_rolling[location].info[i].msg,uni_str,OSD_ROOL_MAX_LEN);
			osd_rolling[location].info[i].repeat = p_ca_msg_info_t->osd_display.roll_value;//显示次数
		  	osd_rolling[location].info[i].valid = 1;  //是否已显示1:未显
		}
		else 
		{
			OS_PRINTF("No bak buffer to store roll msg!!\n");
		}
		
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{			
			DEBUG(MAIN,INFO, "OSD_SHOW_TOP----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,   repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
									  osd_rolling[OSD_SHOW_TOP].info[i].msg,
									  osd_rolling[OSD_SHOW_TOP].info[i].repeat,
									  osd_rolling[OSD_SHOW_TOP].info[i].valid);
		}
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			DEBUG(MAIN,INFO, "OSD_SHOW_BOTTOM----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,  repeat = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
									  osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
									  osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
									  osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
		}
	}
     */
	 	
#endif

	return SUCCESS;
}


#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
static RET_CODE on_ca_rolling_resume_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
	int i=0;
	for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
	{
		DEBUG(DVBCAS_PORTING,INFO, "5----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,  repeat = %d,  valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
		
		if (osd_rolling[OSD_SHOW_BOTTOM].info[i].valid == 1)
		{
			DEBUG(DVBCAS_PORTING,INFO, "Z----^^^^^^^^^^^^^^^^^^^i = %d^^^^^^^^^^^^^^^^^^^^^\n",osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
			//if(osd_rolling[OSD_SHOW_BOTTOM].is_busy == 0)
			//{
				on_desktop_start_roll(osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
									  0,
									  osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
									  OSD_SHOW_BOTTOM);
				//osd_rolling[OSD_SHOW_BOTTOM].info[i].valid = 0;
				DEBUG(DVBCAS_PORTING,INFO, "3----00000000000000000000000000000000000000\n");
				osd_rolling[OSD_SHOW_BOTTOM].is_busy = 1;
			//}
		}

		if (osd_rolling[OSD_SHOW_TOP].info[i].valid == 1)
		{
			DEBUG(DVBCAS_PORTING,INFO, "Z----^^^^^^^^^^^^^^^^^^^i = %d^^^^^^^^^^^^^^^^^^^^^\n",osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
			//if(osd_rolling[OSD_SHOW_BOTTOM].is_busy == 0)
			//{
				DEBUG(DVBCAS_PORTING,INFO, "*************osd_rolling[OSD_SHOW_BOTTOM].is_busy = %d**************\n",osd_rolling[OSD_SHOW_BOTTOM].is_busy);
				on_desktop_start_roll(osd_rolling[OSD_SHOW_TOP].info[i].msg,
									  0,
									  osd_rolling[OSD_SHOW_TOP].info[i].repeat,
									  OSD_SHOW_TOP);
				//osd_rolling[OSD_SHOW_TOP].info[i].valid = 0;
				DEBUG(DVBCAS_PORTING,INFO, "3----00000000000000000000000000000000000000\n");
				osd_rolling[OSD_SHOW_TOP].is_busy = 1;
			//}
		}
	}
	
	for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
	{
		DEBUG(DVBCAS_PORTING,INFO, "on_ca_rolling_resume_update----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,  repeat = %d,  valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
	                              osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
	}

	return SUCCESS;
}
#endif



static RET_CODE on_switch_pn(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  av_set_t av_set;
  u8 org_mode;
  u8 mode_str[5][8] = {"Auto", "NTSC", "PAL", "PAL-M", "PAL-N"};
  rect_t pn_notify =
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    sys_status_get_av_set(&av_set);
    org_mode = av_set.tv_mode;

    av_set.tv_mode++;
    if(av_set.tv_mode > AVC_VIDEO_MODE_PAL_N_1)
    {
      av_set.tv_mode = AVC_VIDEO_MODE_AUTO_1;
    }

    if(av_set.tv_mode != org_mode)
    {
      ui_reset_tvmode(av_set.tv_mode);
    }

    ui_set_notify(&pn_notify, NOTIFY_TYPE_ASC, (u32)mode_str[av_set.tv_mode]);

    sys_status_set_av_set(&av_set);
    sys_status_save();
  }
  return SUCCESS;
}

static RET_CODE on_pro_classify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  
  if(cfg.customer == CUSTOMER_TAIHUI)
  {
      if(ui_is_fullscreen_menu(fw_get_focus_id()))
      {
          return manage_open_menu(ROOT_ID_PRO_CLASSIFY, 0, 0);
      }
  }
  return SUCCESS;
}
static RET_CODE on_switch_lang(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  language_set_t lang_set;
  sys_status_get_lang_set(&lang_set);
  if(lang_set.osd_text == 0)
  {
    rsc_set_curn_language(gui_get_rsc_handle(),2);
    lang_set.osd_text = 1;
  }
  else
  {
    rsc_set_curn_language(gui_get_rsc_handle(),1);
    lang_set.osd_text = 0;
  }
  sys_status_set_lang_set(&lang_set);
  sys_status_save();
  return SUCCESS;
}

static RET_CODE on_switch_preview(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t orc = {0};

  u8 idx = manage_find_preview((u8)para1, &orc);

  if(idx != INVALID_IDX)
  {
    avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                      orc.left, orc.top, orc.right, orc.bottom);
    ui_play_curn_pg();
  }

  return SUCCESS;
}

void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2)
{
#if(AP_CAS_ID == CAS_ID_DS) 
  cmd_t cmd = {0};
  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | AP_CAS_ID);
  ap_frm_do_command(APP_CA, &cmd);
#endif
  return;
}

void ui_ca_rolling_post_stop(control_t *p_ctrl,BOOL osd_flag)
{
	control_t *tmp = 0, *proot = 0;
	u8 which,i;
#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
	u32 dur_time = 0;
#endif

	//confirm which rolling test has stoped
	proot = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	tmp = ctrl_get_child_by_id(proot, IDC_BG_MENU_CA_ROLL_TOP);
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	if(ui_is_smart_card_insert() == FALSE)
    {
		DEBUG(DVBCAS_PORTING,INFO, "ui_ca_rolling_post_stop<<<<<<<<<<<<<<<<   card is out   on_ca_rolling_stop(return)  >>>>>>>>>>>\n");
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			DEBUG(DVBCAS_PORTING,INFO, "ui_ca_rolling_post_stop----^^^^^^^^^^^^^^^^^^i= %d,   msg= %s,  repeat = %d,  valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
		                              osd_rolling[OSD_SHOW_BOTTOM].info[i].msg,
		                              osd_rolling[OSD_SHOW_BOTTOM].info[i].repeat,
		                              osd_rolling[OSD_SHOW_BOTTOM].info[i].valid);
		}
		return;
    }
#endif
	if (p_ctrl == tmp)
	{
		which = OSD_SHOW_TOP;
	}
	else
	{
		tmp = ctrl_get_child_by_id(proot, IDC_BG_MENU_CA_ROLL_BOTTOM);
		if (p_ctrl == tmp)
			which = OSD_SHOW_BOTTOM;
		else 
			which = OSD_SHOW_TOP;
	}

	// find need to show osd info on the same ops
	for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
	{
		if (osd_rolling[which].info[i].valid == 1)
		{
			break;
		}
	}
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_DVBCA)
	if(i < OSD_ROLL_BAK_COUNT 
		&& (osd_rolling[which].info[i].valid == 1)
		&& (g_is_force_channel == FALSE))
	{
		on_desktop_start_roll(osd_rolling[which].info[i].msg,
						          0,
						          osd_rolling[which].info[i].repeat,
						          which);
		osd_rolling[which].info[i].valid = 0;
		osd_rolling[which].is_busy = 1;
	}
#endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
	if(i < (OSD_ROLL_BAK_COUNT-1) 
		&& (osd_rolling[which].info[i].valid == 1)
		&& (g_is_force_channel == FALSE))
	{
		on_desktop_start_roll(osd_rolling[which].info[i+1].msg,
						          0,
						          osd_rolling[which].info[i+1].repeat,
						          which);
		if(ui_is_smart_card_insert() == TRUE)
		{
			osd_rolling[which].info[i].valid = 0;
		}
		DEBUG(DVBCAS_PORTING,INFO, "4-----00000000000000000000000000000000000000\n");
		osd_rolling[which].is_busy = 1;
		DEBUG(DVBCAS_PORTING,INFO, "555----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	}	
#endif

	else  // if not 
	{
		DEBUG(MAIN, INFO, "which:%d, is_busy:%d\n", which, osd_rolling[which].is_busy);
		osd_rolling[which].is_busy = 0;
		// swap
		if (which == OSD_SHOW_TOP)
			which = OSD_SHOW_BOTTOM;
		else
			which = OSD_SHOW_TOP;
		// find the against pos store msg
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			if (osd_rolling[which].info[i].valid == 1)
			{
				break;
			}
		}
		if(i < OSD_ROLL_BAK_COUNT 
		&& (osd_rolling[which].info[i].valid == 1)
		&& (g_is_force_channel == FALSE))
		{
			on_desktop_start_roll(osd_rolling[which].info[i].msg,
			              0,
			              osd_rolling[which].info[i].repeat,
			              which);
			osd_rolling[which].info[i].valid = 0;
			osd_rolling[which].is_busy = 1;
		}	
		else
		{
			if(do_search_is_finish() && manual_search_is_finish())
			{
#if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
				if(osd_flag)
					dur_time = -1;
				else
					dur_time = time_spent_for_osd_roll(1);
				OS_PRINTF("osd 显示时间 = %d\n",dur_time);
				ui_ca_get_info(CAS_CMD_CA_OSD_SHOW_TIME, dur_time, 0);
				redraw_finger_again();
#else
				ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
#endif
				g_is_osd_stop_msg_unsend = FALSE;
			}
			else
			{
				g_is_osd_stop_msg_unsend = TRUE;
			}
		}
	}
}

static RET_CODE on_ca_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t frame = {0};
  control_t *p_parent = NULL;
  BOOL osd_exit_flag = FALSE;

  MT_ASSERT(p_ctrl != NULL);
  ctrl_set_attr(p_ctrl ,OBJ_ATTR_HIDDEN);
  ctrl_get_frame(p_ctrl, &frame);
//#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)
//	clear_osd_rolling_buffer();
//#endif
  
  gui_stop_roll(p_ctrl);
  p_parent = ctrl_get_parent(p_ctrl);
  MT_ASSERT(p_parent != NULL);

  ctrl_add_rect_to_invrgn(p_parent, &frame);
  ctrl_paint_ctrl(p_parent, TRUE);
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  osd_exit_flag = sv_osd_get_exit_key_enable();
  #endif

  
#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
  if(TRUE == ui_get_ads_osd_status())
  {    
    UI_PRINTF("[ui_desktop] roll title continue roll!!!\n");
    ui_ads_osd_roll();
  }
  else
#endif
  {
    ui_ca_rolling_post_stop(p_ctrl,osd_exit_flag);
  }
    return SUCCESS;
}

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
RET_CODE on_ca_sv_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  sv_osd_set_exit_key_enable(TRUE);
  on_ca_rolling_stop(p_ctrl, msg, para1, para2);
  return SUCCESS;
}
#endif

BOOL osd_roll_stop_msg_unsend(void)
{
  return g_is_osd_stop_msg_unsend;
}

BOOL get_ca_msg_rolling_status(void)
{
  return osd_rolling[OSD_SHOW_TOP].is_busy;
}

void set_ca_msg_rolling_status(BOOL is_busy)
{
  osd_rolling[OSD_SHOW_TOP].is_busy = is_busy;
}

RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  RET_CODE ret = 0;
  control_t *p_roll_top_ctrl = NULL;
  control_t *p_roll_bottom_ctrl = NULL;

#if(CONFIG_ADS_ID  == CONFIG_ADS_ID_DESAI)
  if(TRUE == ui_get_ads_osd_status() && (msg == MSG_CA_HIDE_ROLL_MSG))
    return ret;
#endif
  OS_PRINTF("%d \n",__LINE__);

  p_roll_top_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_TOP);
  p_roll_bottom_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_BOTTOM);
  if((NULL != p_roll_top_ctrl) && (ctrl_is_rolling(p_roll_top_ctrl)))
  {
    ret = on_ca_rolling_stop(p_roll_top_ctrl, msg, para1, para2);
	DEBUG(MAIN,INFO,"OSD rolling top\n");
  }
  if((NULL != p_roll_bottom_ctrl) && (ctrl_is_rolling(p_roll_bottom_ctrl)))
  {
    ret = on_ca_rolling_stop(p_roll_bottom_ctrl, msg, para1, para2);  
	DEBUG(MAIN,INFO,"OSD rolling bottom\n");
  }
  return ret;
}


#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
RET_CODE on_ca_rolling_over_stop_osd_top(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  RET_CODE ret = 0;
  control_t *p_roll_top_ctrl = NULL;
  OS_PRINTF("%d \n",__LINE__);

  p_roll_top_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_TOP);
  if((NULL != p_roll_top_ctrl) && (ctrl_is_rolling(p_roll_top_ctrl)))
  {
    ret = on_ca_rolling_stop(p_roll_top_ctrl, msg, para1, para2);
	DEBUG(MAIN,INFO,"1----------------------------------------\n");
  }
  return ret;
}


RET_CODE on_ca_rolling_over_stop_osd_bottom(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  RET_CODE ret = 0;
  control_t *p_roll_bottom_ctrl = NULL;
  OS_PRINTF("%d \n",__LINE__);

  p_roll_bottom_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_BOTTOM);

  if((NULL != p_roll_bottom_ctrl) && (ctrl_is_rolling(p_roll_bottom_ctrl)))
  {
    ret = on_ca_rolling_stop(p_roll_bottom_ctrl, msg, para1, para2);  
	DEBUG(MAIN,INFO,"2----------------------------------------\n");
  }
  return ret;
}

#endif


static RET_CODE on_ca_rolling_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t frame = {0};
  control_t *p_roll_top_ctrl = NULL;
  control_t *p_roll_bottom_ctrl = NULL;

  p_roll_top_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_TOP);
  #if 1
  p_roll_bottom_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BG_MENU_CA_ROLL_BOTTOM);
  #endif
  if(NULL != p_roll_top_ctrl)
  {
      ctrl_set_attr(p_roll_top_ctrl ,OBJ_ATTR_HIDDEN);
      ctrl_get_frame(p_roll_top_ctrl, &frame);
      gui_stop_roll(p_roll_top_ctrl);

      ctrl_add_rect_to_invrgn(p_ctrl, &frame);
      ctrl_paint_ctrl(p_ctrl, FALSE);
      return SUCCESS;
  }
  #if 1
  if(NULL != p_roll_bottom_ctrl)
  {
      ctrl_set_attr(p_roll_bottom_ctrl ,OBJ_ATTR_HIDDEN);
      ctrl_get_frame(p_roll_bottom_ctrl, &frame);
      gui_stop_roll(p_roll_bottom_ctrl);

      ctrl_add_rect_to_invrgn(p_ctrl, &frame);
      ctrl_paint_ctrl(p_ctrl, FALSE);
      return SUCCESS;
  }
  #endif
  return ERR_FAILURE;
}

static RET_CODE on_switch_audio(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  audio_set_t audio_set;
  u16 mode_str[3] = { IDS_STERO, IDS_LEFT_TRACK, IDS_RIGHT_TRACK,};
  u8 str_index = 0;
  rect_t pn_notify = 
  {
	  NOTIFY_CONT_X, NOTIFY_CONT_Y-5,	// LXD ADD TO BE WIDELESS
	  NOTIFY_CONT_X + NOTIFY_CONT_W-80, // LXD SHIFT TO RIGHT
	  NOTIFY_CONT_Y + NOTIFY_CONT_H-10,
  };

  u16 prog_id;
  dvbs_prog_node_t curn_prog;

  if ((prog_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
  {
    return ERR_FAILURE;
  }

  if (db_dvbs_get_pg_by_id(prog_id, &curn_prog) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND || fw_get_focus_id() == ROOT_ID_VOLUME)
  {
    sys_status_get_audio_set(&audio_set);

    if(audio_set.is_global_track)
    {
      audio_set.global_track = (audio_set.global_track+1)%3;
      sys_status_set_audio_set(&audio_set);
      avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), audio_set.global_track);

      str_index = audio_set.global_track;
    }
    else
    {
      curn_prog.audio_track = (curn_prog.audio_track+1)%3;
      db_dvbs_edit_program(&curn_prog);
      db_dvbs_save_pg_edit(&curn_prog);
      avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), curn_prog.audio_track);

      str_index = (u8)curn_prog.audio_track;
    }
    if(ui_is_mute())
    {
      ui_set_mute(FALSE);
    }
    ui_set_notify(&pn_notify, NOTIFY_TYPE_STRID, (u32)mode_str[str_index]);

    sys_status_save();
  }
  
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  redraw_finger_again();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  return SUCCESS;
}

void ui_roll_big_top_window()
{
  control_t *p_cont = NULL;
  control_t *p_txt_top = NULL;
  DEBUG(DVBCAS_PORTING,INFO,"*****OSD TOP\n");
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_txt_top = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
                           190, 12,
                           SCREEN_WIDTH - 190,40,
                           p_cont, 0);
  ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);

  ctrl_set_attr(p_txt_top,OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_txt_top,
                        RSI_TRANSPARENT,
                        RSI_TRANSPARENT,
                        RSI_TRANSPARENT);
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_DS)
    text_set_font_style(p_txt_top, FSI_CHINESE_BIG, FSI_CHINESE_BIG, FSI_CHINESE_BIG);
  #else
    text_set_font_style(p_txt_top, FSI_CHINESE_BIG, FSI_CHINESE_BIG, FSI_CHINESE_BIG);
  #endif
  text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
}

void ui_roll_big_bottom_window()
{
  control_t *p_cont = NULL;
  control_t *p_txt_bottom = NULL;
  DEBUG(DVBCAS_PORTING,INFO,"*****OSD bottom\n");
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_txt_bottom = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
                           0, SCREEN_HEIGHT - 45,
                           SCREEN_WIDTH,40,
                           p_cont, 0);
  ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);

  ctrl_set_attr(p_txt_bottom,OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_txt_bottom,
                        RSI_TRANSPARENT,
                        RSI_TRANSPARENT,
                        RSI_TRANSPARENT);
  text_set_font_style(p_txt_bottom, FSI_CHINESE_BIG, FSI_CHINESE_BIG, FSI_CHINESE_BIG);
  text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);
}

void ui_roll_top_window()
{
  control_t *p_cont = NULL;
  control_t *p_txt_top = NULL;
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_txt_top = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
                           0, 10,
                           SCREEN_WIDTH,32,
                           p_cont, 0);
  ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);

  ctrl_set_attr(p_txt_top,OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_txt_top,
                        RSI_IGNORE,
                        RSI_IGNORE,
                        RSI_IGNORE);
  text_set_font_style(p_txt_top, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
}

void ui_roll_bottom_window()
{
  control_t *p_cont = NULL;
  control_t *p_txt_bottom = NULL;
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_txt_bottom = ctrl_create_ctrl((u8 *)CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
                           0, SCREEN_HEIGHT - 42,
                           SCREEN_WIDTH,32,
                           p_cont, 0);
  ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);

  ctrl_set_attr(p_txt_bottom,OBJ_ATTR_HIDDEN);
  ctrl_set_rstyle(p_txt_bottom,
                        RSI_IGNORE,
                        RSI_IGNORE,
                        RSI_IGNORE);
  text_set_font_style(p_txt_bottom, FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
  text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);
}

void ui_desktop_init(void)
{
  fw_config_t config =
  {
    { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },                /* background */

    DST_IDLE_TMOUT,                                       /* timeout of idle */
    RECEIVE_MSG_TMOUT,                                    /* timeout of recieving msg */
    POST_MSG_TMOUT,                                       /* timeout of sending msg */

    ROOT_ID_BACKGROUND,                                   /* background root_id */
    MAX_ROOT_CONT_CNT,
    MAX_MESSAGE_CNT,
    MAX_HOST_CNT,
    MAX_TMR_CNT,

    RSI_TRANSPARENT,
  };

  customer_cfg_t cus_cfg = {0};
  get_customer_config(&cus_cfg);
  
  fw_init(&config,
               ui_desktop_keymap_on_normal,
               ui_desktop_proc_on_normal,
               ui_menu_manage);
  DEBUG(DVBCAS_PORTING,INFO,"desktop init>>>>\n");
#ifdef ROLL_TITLE_BIG_FONT
{
  DEBUG(DVBCAS_PORTING,INFO,"desktop init SHOW OSD>>>>\n");
  ui_roll_big_top_window();
  ui_roll_big_bottom_window();
}
#else
{
  DEBUG(DVBCAS_PORTING,INFO,"desktop init SHOW OSD>>>>\n");
  ui_roll_top_window();
  ui_roll_bottom_window();
}
#endif
  manage_init();

  pic_player_init();

  common_bmp_pic_read();

  // set callback func
  avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID),
    reset_screen);
}

extern  u8 get_channel_init_nit_ver(void);
extern  u16 get_channel_init_nit_network_id(void);
extern BOOL get_channel_init_channel_status(void);

void ui_desktop_start(void)
{
  u8 curn_mode = CURN_MODE_NONE;
  u8 vid = 0;
  u16 type;
  u32 context;
  BOOL auto_search_bit = FALSE;
  BOOL full_tp_search_bit = FALSE;
  u32 old_version = 0xff;
  u32 new_version = 0xff;
  dlg_ret_t dlg_ret = DLG_RET_NO;
  u16 content[64] = {0};
  comm_dlg_data_t dlg_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, 170,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    10000,
  };
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  ui_cache_view();
  ui_dvbc_change_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, FALSE);
  curn_mode = sys_status_get_curn_prog_mode();
  if(curn_mode != CURN_MODE_NONE)
  {
    sys_status_get_curn_view_info(&type, &context);
    vid = ui_dbase_create_view(type, context, NULL);
    ui_dbase_set_pg_view_id(vid);
  }

  sys_status_get_nit_version(&old_version);
  new_version = get_channel_init_nit_ver();
  ui_restore_view();

  OS_PRINTF("###debug save nit version = 0x%x,new nit version = 0x%x",old_version,new_version);
  if(get_channel_init_channel_status() == FALSE)
  {
    if((old_version != (u32)new_version) && (new_version != 0xff)) /**if no nitversion it is 0xff***/
    {
      auto_search_bit = TRUE;
    }
  }
  if(curn_mode == CURN_MODE_NONE)
  {
    auto_search_bit = TRUE;
    if(new_version == 0xff)
    {
      full_tp_search_bit = TRUE;
    }   
  }


  if(auto_search_bit == TRUE)  /***need to auto scan****/
  {
      if(cfg.customer == CUSTOMER_XINSIDA_SPAISH)
      {
        gui_get_string(IDS_PROG_UPDATE_TO_SEARCH, content, 64);
        dlg_data.text_content = (u32)content;
        dlg_data.title_strID = IDS_PROG_UPDATE;

        dlg_ret = ui_comm_dlg_open(&dlg_data);
        if(dlg_ret != DLG_RET_NO)
        {
          //try to close all menus and play in fullscreen.
          ui_clean_all_menus_do_nothing();
          if(full_tp_search_bit)
          {
            manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_FULL, 0);
          }
          else
          {
             manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
          }

        }
      }
      else
      {
         manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
      }
  }
  else
  {
    vid = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
    if(db_dvbs_get_count(vid)<=0)
    {
      manage_open_menu(SN_ROOT_ID_MAINMENU, 0, 0);
    }
    else if(LATEST_NODE_START != book_check_latest_node(book_get_latest_index()))
    {
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
    }
  }
#if (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  sv_nit_area_lock(0, 0);
#endif
#if(CONFIG_ADS_ID == CONFIG_ADS_ID_SZXC)
  {
    ui_adv_szxc_statu(2);
  }
#endif
}

void ui_desktop_release(void)
{
  fw_release();
  // set callback func
  avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID), NULL);
}

void ui_desktop_main(void)
{
  fw_default_mainwin_loop(ROOT_ID_BACKGROUND);
}

BOOL ui_desktop_key_disable(u16 vkey)
{

  if(!g_is_uiokey_disable)
  {
      return FALSE;
  }
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_SV)
  if(V_KEY_POWER == vkey)
  {
      return FALSE;
  }
#else
  if(fw_find_root_by_id(ROOT_ID_FORCE_EMAIL_CONTENT) != NULL)
  {
    if((V_KEY_UP== vkey) || (V_KEY_DOWN== vkey))
    {
        return FALSE;
    }
  }
#endif
  OS_PRINTF("\r\n ***ui_desktop_key_disable vkey value[%d]*** ", vkey);
  return TRUE;
}

#ifdef OTA_CHECK_DEFAULT
static void do_ota(void)
{
  #ifndef WIN32
  hal_pm_reset();
  #endif
}
static RET_CODE on_force_ota(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  do_ota();
  return SUCCESS;
}


static RET_CODE on_main_freq_force_ota(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  fw_set_keymap(ui_desktop_keymap_on_normal);
  do_ota();
  return SUCCESS;
}
static RET_CODE on_main_freq_check_tmout(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
   ss_public_t *ss_public;

  ui_enable_uio(TRUE);
  ui_enable_playback(TRUE);
  #if(CONFIG_CAS_ID  != CONFIG_CAS_ID_DS)
  ui_init_ca();
  #endif
  ui_time_init();
  /* activate EPG */
  ui_epg_init();
  ui_epg_start();
  // open signal monitor
  ui_init_signal();

   // restore the setting
  sys_status_reload_environment_setting();
  //set sw version.
  ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));
  OS_PRINTF("get ver %d\n", sys_status_get_sw_version());
  ss_public->otai.orig_software_version = (u16)sys_status_get_sw_version();
  ss_ctrl_update_public(ss_public);
  // try to play
  epg_data_init_dy_policy(class_get_handle_by_id(EPG_CLASS_ID),
                          DY_SVC_NUM);
  sys_status_check_group_view(TRUE);

  //create sleep timer.
  ui_sleep_timer_create();

  #ifndef WIN32
  on_ca_zone_check_start(CAS_CMD_ZONE_CHECK,0,0);
  #endif
    on_ca_zone_check_stop();

  return SUCCESS;
}
#endif
RET_CODE pre_open_do_search(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
   u16 content[64];
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u8 focus_id = 0;
   comm_dlg_data_t dlg_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, 170,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    0,
  };

  /***/

  focus_id = fw_get_focus_id();
  if((focus_id != SN_ROOT_ID_MAINMENU) && (focus_id != ROOT_ID_BACKGROUND))
  {
    return SUCCESS;
  }

  gui_get_string(IDS_PROGRAM_SEARCH, content, 64);
  dlg_data.text_content = (u32)content;
  dlg_data.title_strID = RSC_INVALID_ID;

  dlg_ret = ui_comm_dlg_open(&dlg_data);
  if(dlg_ret != DLG_RET_NO)
  {
   // ui_close_all_mennus();
    ui_clean_all_menus_do_nothing();
     manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
  }
 return SUCCESS;
}


RET_CODE pre_open_set_freq(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u16 content[64];
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  u8 focus_id = 0;
   comm_dlg_data_t dlg_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, 170,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    0,
  };

  /***/

  focus_id = fw_get_focus_id();
  if((focus_id != SN_ROOT_ID_MAINMENU) && (!ui_is_fullscreen_menu(focus_id)))
  {
    return SUCCESS;
  }

  gui_get_string(IDS_PROGRAM_SEARCH, content, 64);
  dlg_data.text_content = (u32)content;
  dlg_data.title_strID = RSC_INVALID_ID;

  dlg_ret = ui_comm_dlg_open(&dlg_data);
  if(dlg_ret != DLG_RET_NO)
  {
    extern void tp_set_search_flag(BOOL flag);
    tp_set_search_flag(TRUE);
    //ui_close_all_mennus();
    ui_clean_all_menus_do_nothing();
    manage_open_menu(ROOT_ID_PROGRAM_SEARCH, 0, 0);
  }
 return SUCCESS;
}

RET_CODE on_ca_get_info(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
      {
        ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0, 0);
      }
  return SUCCESS;
}

RET_CODE on_ca_open_card_info(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};

  get_customer_config(&cfg);
  if((cfg.customer == CUSTOMER_TAIHUI) || (cfg.customer == CUSTOMER_JINGHUICHENG_QZ))
  {
     if(ui_is_fullscreen_menu(fw_get_focus_id()))
     {
         ui_stop_play(STOP_PLAY_BLACK,TRUE);
         manage_open_menu(ROOT_ID_CA_CARD_INFO, para1, 0);
     }
  }
  return SUCCESS;
}

RET_CODE on_ca_authen_message_update(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
   if(ui_is_fullscreen_menu(fw_get_focus_id()))
   {
       manage_open_menu(ROOT_ID_CA_ENTITLE_EXP_DLG, 0, para2);
   }
  return SUCCESS;
}

RET_CODE on_ca_show_new_email(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
  u8 index;
  cas_mail_headers_t *p_mail = NULL;
 u16 i = 0;
  index = fw_get_focus_id();
  if(index != SN_ROOT_ID_SEARCH_RESULT
    && do_search_is_finish() 
    && index != ROOT_ID_FORCE_EMAIL_CONTENT)
  {
    p_mail = (cas_mail_headers_t *)para2;

    for(i = 0; i < p_mail->max_num; i++)
    {
      if(p_mail->p_mail_head[i].new_email)
      {
        update_email_picture(EMAIL_PIC_SHOW);
        return SUCCESS;
      }
    }
    update_email_picture(EMAIL_PIC_HIDE);
  }
  update_signal();
  return SUCCESS;
}

RET_CODE on_ca_important_email(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{ 
  cas_module_id_t ca_module;
  u8 vid = 0;
  #ifndef WIN32
  cas_mail_body_t *im_mail_info = NULL;
  im_mail_info = (cas_mail_body_t *)para2;
  ca_module = para1;
  OS_PRINTF("get in here\n");
  if(ca_module != CAS_ID_SV)
  {
 //   return SUCCESS;
  }
  #else
  cas_mail_body_t im_mail_info;
  sprintf(im_mail_info.data,"0x31,0x32,0x33,0x34,0x33,0x32,0x33,0x32");
  im_mail_info.data_len = 8;
  im_mail_info.mail_id = 11;
  #endif
  //for test
  #ifndef WIN32
  #if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
  OS_PRINTF("g_mail_version : %d\n",g_mail_version);
  OS_PRINTF("强制消息内容:%s\n消息版本%d\n",im_mail_info->data,im_mail_info->mail_version);
  OS_PRINTF("im_mail_info->reserved = %d\n",im_mail_info->reserved);
  #endif
  #if 0
  if(!im_mail_info->reserved)
  {
    #ifndef WIN32
    OS_PRINTF("out here\n");
    return SUCCESS;
    #endif
  }
  #endif
  vid = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  if(db_dvbs_get_count(vid)<=0)
  {
    OS_PRINTF("no program ,so don't receive FORCE_EMAIL_MSG\n");
    return FALSE;
  }
  #endif
  if(fw_find_root_by_id(ROOT_ID_FORCE_EMAIL_CONTENT) != NULL)
  {
      manage_close_menu(ROOT_ID_FORCE_EMAIL_CONTENT,0,0);
  }
  g_is_uiokey_disable = TRUE;
  ui_close_all_mennus();
  #ifndef WIN32
  manage_open_menu(ROOT_ID_FORCE_EMAIL_CONTENT,0,(u32)im_mail_info);//para2);
  #else
  manage_open_menu(ROOT_ID_FORCE_EMAIL_CONTENT,0,(u32)&im_mail_info);//para2);
  #endif
  #ifdef WIN32
  fw_tmr_create(ROOT_ID_BACKGROUND,MSG_CA_TEST2,4000000,0);
  #endif
  return SUCCESS;
}

RET_CODE on_ca_cancel_important_email(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
#if (CONFIG_CAS_ID == CONFIG_CAS_ID_SV)
  cas_module_id_t ca_module;
  ca_module = para1;
  //for test
  OS_PRINTF("消去原吹那制消息\n");
#ifndef WIN32
  if(ca_module != CAS_ID_SV)
  {
    return SUCCESS;
  }
#endif
  if(fw_find_root_by_id(ROOT_ID_FORCE_EMAIL_CONTENT) != NULL)
  {
    manage_close_menu(ROOT_ID_FORCE_EMAIL_CONTENT,0,0);
  }
  g_is_uiokey_disable = FALSE;
  #if(CONFIG_CAS_ID  == CONFIG_CAS_ID_SV)
  g_mail_version = 0;
  #endif
#endif
  return SUCCESS;
}

BOOL get_uiokey_stayus(void)
{
  return g_is_uiokey_disable;
}

void set_uiokey_status(BOOL is_disable)
{
  g_is_uiokey_disable = is_disable;
}

BOOL get_force_channel_status(void)
{
  return g_is_force_channel;
}
RET_CODE on_ca_card_update(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;

  ui_close_all_mennus();
  ret = manage_open_menu(ROOT_ID_CA_CARD_UPDATE, 0, para2);

  return ret;
}

RET_CODE hotkey_blue_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if(cfg.customer == CUSTOMER_JINGHUICHENG_QZ)  /**tai hui not need****/
  {
    return pre_open_do_search(p_ctrl,msg,para1,para2);
  }
  if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
  	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
  	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id())) 
      {
          return manage_open_menu(ROOT_ID_GAME_INN, 0, 0);
      }
  }
  else if(cfg.customer == CUSTOMER_TAIHUI)
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
        return manage_open_menu(ROOT_ID_CHANNEL_EDIT, 0, 0);
    }
  }
 return SUCCESS;
}

RET_CODE hotkey_green_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  
 if((CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)||(CONFIG_CAS_ID == CONFIG_CAS_ID_DS))
  {
    ui_ca_get_info(CAS_CMD_IPPV_INFO_SYNC_GET, 0,0);
    return SUCCESS;
  }
  
  if(cfg.customer == CUSTOMER_JINGHUICHENG_QZ) /**tai hui not need****/
  {
    return on_ca_get_info(p_ctrl,msg,para1,para2);
  }
  else if((cfg.customer == CUSTOMER_FANTONG_KF)||(cfg.customer == CUSTOMER_FANTONG_KFAJX)
  	||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)||(cfg.customer == CUSTOMER_FANTONG_BYAJX)
  	||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
     if(ui_is_fullscreen_menu(fw_get_focus_id()))
     {
     	return manage_open_menu(ROOT_ID_EMAIL_MESS, 0, 0);
     }
  }
  else if(cfg.customer == CUSTOMER_TAIHUI)
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
       return  manage_open_menu(ROOT_ID_CALENDAR, 0, 0);
    }
  }
 return SUCCESS;
}
RET_CODE hotkey_yellow_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if(cfg.customer == CUSTOMER_FANTONG)
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
      return manage_open_menu(ROOT_ID_EMAIL_MESS, 0, 0);
    }
  }
  else if(cfg.customer == CUSTOMER_TAIHUI)
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
        return manage_open_menu(ROOT_ID_PREBOOK_MANAGE, 0, 0);
    }
  }
 return SUCCESS;
}
RET_CODE hotkey_red_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  get_customer_config(&cfg);
  if((cfg.customer == CUSTOMER_BOYUAN) ||(cfg.customer == CUSTOMER_PANLONGHU)
    || (cfg.customer == CUSTOMER_ZHILING) || (cfg.customer == CUSTOMER_ZHILING_KF) 
    || (cfg.customer == CUSTOMER_ZHILING_LQ) || (cfg.customer == CUSTOMER_SHENZHOU_QN))
  {
    return pre_open_set_freq(p_ctrl,msg,para1,para2);
  }
 if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
 	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
 	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
        return manage_open_menu(ROOT_ID_EPG, 0, 0);
    }
  }
 else if(cfg.customer == CUSTOMER_TAIHUI)
 {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
        return manage_open_menu(ROOT_ID_FAV_LIST, 0, 0);
    }
 }
 return SUCCESS;
}

RET_CODE hotkey_search_process(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  customer_cfg_t cfg = {0};
  u8 focus_id = 0;
  get_customer_config(&cfg);
   focus_id = fw_get_focus_id();
  if((focus_id != SN_ROOT_ID_MAINMENU) && (!ui_is_fullscreen_menu(focus_id)))
  {
      return SUCCESS;
  }
  if((cfg.customer == CUSTOMER_FANTONG)||(cfg.customer == CUSTOMER_FANTONG_KF)
  	||(cfg.customer == CUSTOMER_FANTONG_KFAJX)||(cfg.customer == CUSTOMER_FANTONG_KF_SZXC312)
  	||(cfg.customer == CUSTOMER_FANTONG_BYAJX)||(cfg.customer == CUSTOMER_FANTONG_XSMAJX))
  {
    ui_stop_play(STOP_PLAY_BLACK,TRUE);
    //ui_close_all_mennus();
    ui_clean_all_menus_do_nothing();
     return manage_open_menu(SN_ROOT_ID_SEARCH_RESULT, SCAN_TYPE_AUTO, 0);
  }
  return SUCCESS;
}

/*for yingji ca show overdue*/
RET_CODE show_overdue_window(u32 para1, u32 para2)
{

  rect_t overdue_notify = 
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };
  
  u8 *p_overdue_data = NULL;
  u16 uni_str[64] = {0};
  p_overdue_data = (u8 *)para2; 
  
  OS_PRINTF("show_overdue_window data: %s\n",(u8 *)para2);
  
  gb2312_to_unicode(p_overdue_data, 64,uni_str, 64);

  ui_set_notify(&overdue_notify,NOTIFY_TYPE_UNI,(u32)uni_str);
  return SUCCESS;
}

/*for yingji ca show overdue*/
RET_CODE show_overdue_data(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  OS_PRINTF("show_overdue_data!!!!!!!!!!!!!!!\n");
  if(ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    OS_PRINTF("show_overdue_data222222222222222222\n");
    show_overdue_window(para1,para2);
  }
  return SUCCESS;
}
static RET_CODE on_revert_ota_sw(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
#ifndef WIN32
  if(fw_get_focus_id() != SN_ROOT_ID_MAINMENU)
  {
    return SUCCESS;
  }
  OS_PRINTF("#####revert ota sw,it will ota");

  #ifdef OTA_DM_ON_STATIC_FLASH
   sys_static_set_status_ota_revert_sw_on_ap();
  #else
  sys_status_set_sw_version(0);
  #endif 
  mtos_task_delay_ms(1000);
  hal_pm_reset();
#endif
  return SUCCESS;
}

static RET_CODE on_printf_switch_onoff(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
#ifndef WIN32
  u8 switch_status;
  if(fw_get_focus_id() != SN_ROOT_ID_MAINMENU)
  {
    return SUCCESS;
  }
  OS_PRINTF("#####printf_switch_force");
  switch_status = get_printf_switch_flag();
  if(switch_status)
  {
    switch_status = 0;
    mtos_close_printk();
  }
  else
  {
    switch_status = 1;
    mtos_open_printk();
  }
  set_printf_switch_flag(switch_status);
#endif
  return SUCCESS;
}
static RET_CODE on_ca_stop_cur_pg(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u16 pg_id = 0;
  u32 index = 0;
  dvbs_prog_node_t pg = {0};
  u32 pid_param = (u32)*(u32*)para2;
  u16 ecm_pid = 0;
  u16 service_id = 0;
  u16 temp = 0;

  ecm_pid = (u16)(pid_param >> 16);
  service_id = (u16)(pid_param & 0xFFFF);

  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);
  
  //undescramble pg ecm_pid 0x1fff
  if((0x1FFF == ecm_pid) && (pg.s_id == service_id))
  {
    UI_PRINTF("@@@@  current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
                      temp, ecm_pid,pg.s_id,service_id);
    ui_stop_play(STOP_PLAY_BLACK, TRUE);
    return SUCCESS;
  }
  
  //descramble pg
  for(index = 0; index < pg.ecm_num; index++)
  {
    temp = pg.cas_ecm[index].ecm_pid;
    UI_PRINTF("@@@@ index = %d, current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
                      index, temp, ecm_pid,pg.s_id,service_id);
    if(((pg.cas_ecm[index].ecm_pid == ecm_pid) && ( pg.s_id == service_id) ))
    {
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
}
  return SUCCESS;
}

#if(CONFIG_CAS_ID != CONFIG_CAS_ID_ADT_MG)
static RET_CODE on_ca_real_time_book(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  menu_attr_t *p_attr;
  u16 pg_id = INVALIDID;
  dvbs_prog_node_t prog = {0};
  #ifdef WIN32
  static ipp_info_t p_ipp_info;
  p_ipp_info.booked_by_unit=3;
  p_ipp_info.booked_price=10000;
  p_ipp_info.current_price=10000;
  p_ipp_info.remain_time=1000;
  p_ipp_info.operator_id = 10;
  p_ipp_info.product_id = 10;
  p_ipp_info.burse_id = 5;
  strcpy(p_ipp_info.product_name,"hdhdhdhh");
  p_ipp_info.reserved = 0x0a;         
  p_ipp_info.book_state_flag = (p_ipp_info.reserved & 0x02) >> 1;
  p_ipp_info.type = 1;
  sprintf(p_ipp_info.start_time, "2012-12-19");
  p_ipp_info.remain_time = 100;
  p_ipp_info.booked_price = 1;
  p_ipp_info.booked_by_interval = 10;
  p_ipp_info.booked_by_unit = 3;
  strcpy(p_ipp_info.service_name,"fsfdsfdsfsfdsf");
  p_ipp_info.service_id= 12;
  p_ipp_info.current_price = 12;
  p_ipp_info.current_by_interval = 2;
  p_ipp_info.ippt_period = 2;
  para2=(u32)&p_ipp_info;
  #endif

  OS_PRINTF("enter real time book ipp prog!!!!!!\n");
  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &prog);
  
  p_attr = manage_get_curn_menu_attr();
  #ifndef WIN32
  if(!ui_is_fullscreen_menu(p_attr->root_id) || prog.s_id != ((ipp_info_t *)para2)->service_id)
  {
    return SUCCESS;
  }
  #else
   if(!ui_is_fullscreen_menu(p_attr->root_id))
  {
    return SUCCESS;
  }
  #endif
  ui_close_all_mennus();
  manage_open_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME,0,para2);
  return SUCCESS;
}
#endif
static RET_CODE on_ca_ope_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  #if((CONFIG_CAS_ID == CONFIG_CAS_ID_SV) || (CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB))
  cas_operators_info_t *oper_info = NULL;
  oper_info = (cas_operators_info_t *)para2;
  OS_PRINTF("!!!!desktop    operator max num : %d\n",oper_info->max_num);
  g_set_operator_info(oper_info);
  #endif
  return SUCCESS;
}

#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
static RET_CODE on_ipp_notify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(fw_find_root_by_id(ROOT_ID_CA_PAIR) != NULL)
  {
    return SUCCESS;
  }
  ui_ca_get_info(CAS_CMD_IPPV_INFO_SYNC_GET, 0,0);
  return SUCCESS;
}
static RET_CODE on_ca_pair_dlg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  if(fw_find_root_by_id(ROOT_ID_CA_PAIR) != NULL)
  {
    return SUCCESS;
  }
  manage_open_menu(ROOT_ID_CA_PAIR, 1,(u32)para2);
  return SUCCESS;
}

#endif
#ifdef WIN32
static RET_CODE on_test_1(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_SHOW_FINGER, 0, 0);
}
#endif

#if(CONFIG_CAS_ID==CONFIG_CAS_ID_TR)
static RET_CODE on_ca_del_mail_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  update_signal();
  return SUCCESS;
}

#endif

// start message mapping
BEGIN_MSGPROC(ui_desktop_proc_on_normal, cont_class_proc)
  ON_COMMAND(MSG_SIGNAL_CHECK, on_check_signal)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  #ifdef WIN32
  ON_COMMAND(MSG_TEST3, on_time_update)
  #endif
  ON_COMMAND(MSG_DESCRAMBLE_SUCCESS, on_update_descramble)
  ON_COMMAND(MSG_DESCRAMBLE_FAILED, on_update_descramble)
  ON_COMMAND(MSG_CLOSE_MENU, on_close_menu)
  ON_COMMAND(MSG_OPEN_MENU_IN_TAB, on_open_in_tab)
  //ON_COMMAND(MSG_MOTOR_DRVING, on_motor_drving)
  ON_COMMAND(MSG_CLOSE_ALL_MENUS, on_close_all_menus)
  ON_COMMAND(MSG_MOTOR_OUT_RANGE, on_motor_out_range)
  ON_COMMAND(MSG_MUTE, on_mute)
  ON_COMMAND(MSG_PAUSE, on_pause)
  ON_COMMAND(MSG_POWER_OFF, on_power_off)
  ON_COMMAND(MSG_POWER_ON, on_power_on)
  ON_COMMAND(MSG_SWITCH_SCART, on_scart_switch)
  ON_COMMAND(MSG_SCART_VCR_DETECTED, on_scart_vcr_detected)
  ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
  ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_TR)
  ON_COMMAND(MSG_CA_SHOW_FINGER, on_finger_print)
#else
  ON_COMMAND(MSG_CA_SHOW_FINGER, on_ca_finger_update)
#endif
  ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_finger_update)
  #if(CONFIG_ADS_ID == CONFIG_CAS_ID_DVBCA)
  ON_COMMAND(MSG_CA_SHOW_URGENT, on_ca_urgent_update)
  ON_COMMAND(MSG_CA_HIDE_URGENT, on_ca_urgent_update)
  #endif
  ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel_play)
  ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel_play)
  ON_COMMAND(MSG_CA_SHOW_ROLL_MSG, on_ca_rolling_msg_update) 
  ON_COMMAND(MSG_CA_HIDE_ROLL_MSG, on_ca_rolling_over)
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_YXSB)
  ON_COMMAND(MSG_CA_HIDE_ROLL_MSG_TOP, on_ca_rolling_over_stop_osd_top)
  ON_COMMAND(MSG_CA_HIDE_ROLL_MSG_BOTTOM,on_ca_rolling_over_stop_osd_bottom)
	ON_COMMAND(MSG_CA_CARD_PROGRESS, on_ca_card_update_process)
#endif 
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_ca_show_new_email)
  ON_COMMAND(MSG_CA_FORCE_MSG, on_ca_important_email)
  ON_COMMAND(MSG_CA_HIDE_FORCE_MSG, on_ca_cancel_important_email)
  ON_COMMAND(MSG_RECOVER_CA_PROMPT, on_ca_message_update)
  ON_COMMAND(MSG_CA_AUTHEN_EXPIRE_DAY, on_ca_authen_message_update)
  //ON_COMMAND(MSG_CA_CARD_UPDATE, on_ca_card_update)
  //ON_COMMAND(MSG_CA_CARD_NEED_UPDATE, on_ca_card_update)
  ON_COMMAND(MSG_CA_CARD_UPDATE_BEGIN, on_ca_card_update)

  ON_COMMAND(MSG_LNB_SHORT, on_lnb_short)
  ON_COMMAND(MSG_SLEEP_TMROUT, on_sleep_tmrout)
  ON_COMMAND(MSG_SWITCH_PN, on_switch_pn)
  ON_COMMAND(MSG_SWITCH_AUDIO, on_switch_audio)
  ON_COMMAND(MSG_SET_LANG, on_switch_lang)
  ON_COMMAND(MSG_PRO_CLASSIFY, on_pro_classify)
  ON_COMMAND(MSG_SWITCH_PREVIEW, on_switch_preview)
  ON_COMMAND(MSG_HEART_BEAT, on_heart_beat)
  ON_COMMAND(MSG_EXIT_CA_ROLLING, on_ca_rolling_timeout)
  ON_COMMAND(MSG_PB_NIT_FOUND, on_update_search)
  ON_COMMAND(MSG_CA_OPEN_IPPV_PPT, on_orde_ippv_info)
  ON_COMMAND(MSG_CA_HIDE_IPPV_PPT, on_hide_ippv_info) 
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
  ON_COMMAND(MSG_CA_SHOW_ERROR_PIN_CODE, on_ca_pair_dlg)
  #endif
  ON_COMMAND(MSG_HOTKEY_BLUE,hotkey_blue_process)
  ON_COMMAND(MSG_HOTKEY_RED,hotkey_red_process)
  ON_COMMAND(MSG_HOTKEY_GREEN,hotkey_green_process)
  ON_COMMAND(MSG_HOTKEY_YELLOW,hotkey_yellow_process)
   
  ON_COMMAND(MSG_HOTKEY_SEARCH,hotkey_search_process)
  ON_COMMAND(MSG_CA_CARD_INFO,on_ca_open_card_info)
  ON_COMMAND(MSG_CA_OVERDUE_DATA, show_overdue_data)
  ON_COMMAND(MSG_EXIT_AD_ROLLING, on_ca_rolling_over)
  ON_COMMAND(MSG_CA_STOP_CUR_PG, on_ca_stop_cur_pg)
  #ifdef OTA_CHECK_DEFAULT
  ON_COMMAND(MSG_OTA_MON_FORCE_OTA, on_force_ota)
  ON_COMMAND(MSG_OTA_MON_MAIN_FREQ_FORCE_OTA, on_main_freq_force_ota)
  ON_COMMAND(MSG_OTA_MON_MAIN_FREQ_TMOUT, on_main_freq_check_tmout)
  #endif
  ON_COMMAND(MSG_CA_CONDITION_SEARCH, on_desai_conditional_search)
  ON_COMMAND(MSG_NIT_OTA_UPGRADE, on_software_update)
  ON_COMMAND(MSG_REVERT_SW_ON_OTA, on_revert_ota_sw)
  ON_COMMAND(MSG_PRINTF_SWITCH, on_printf_switch_onoff)
  #ifdef WIN32
  ON_COMMAND(MSG_CA_TEST1, on_test_1)
  ON_COMMAND(MSG_CA_TEST2, on_ca_important_email)
#if(CONFIG_CAS_ID != CONFIG_CAS_ID_ADT_MG)
   ON_COMMAND(MSG_CA_TEST3, on_ca_real_time_book)
#endif
  ON_COMMAND(MSG_CA_TEST4, pre_open_osd)
  #endif
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)

  ON_COMMAND(MSG_CA_IPP_PROG_NOTIFY,on_ipp_notify)
#else
  ON_COMMAND(MSG_CA_IPP_PROG_NOTIFY,on_ca_real_time_book)
#endif
  ON_COMMAND(MSG_CA_OPE_INFO, on_ca_ope_update) 
#if(CONFIG_CAS_ID == CONFIG_CAS_ID_DVBCA)  
  ON_COMMAND(MSG_CA_DVB_RESUME_OSD, on_ca_rolling_resume_update)
#endif
  //ON_COMMAND(MSG_BROWSER_ENTER, on_browser_enter)
END_MSGPROC(ui_desktop_proc_on_normal, cont_class_proc);


// start key mapping on normal
BEGIN_KEYMAP(ui_desktop_keymap_on_normal, NULL)
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)
  ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
  //lvcm
  #ifdef WIN32
  ON_EVENT(V_KEY_PN, MSG_CA_OPEN_IPPV_PPT)
  ON_EVENT(V_KEY_PLAY, MSG_PAUSE)
  ON_EVENT(V_KEY_SAT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PRO_CLASSIFY)
  #endif
  #if(CONFIG_CAS_ID == CONFIG_CAS_ID_ADT_MG)
  ON_EVENT(V_KEY_SAT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PRO_CLASSIFY)
  #endif
  //lvcm end
  ON_EVENT(V_KEY_LANG, MSG_SET_LANG)
  ON_EVENT(V_KEY_TV_PLAYBACK, MSG_PRO_CLASSIFY)
  ON_EVENT(V_KEY_EXIT, MSG_OPEN_MENU_IN_TAB)//for sumavision close osd
  ON_EVENT(V_KEY_MENU, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_MAINMENU)
  ON_EVENT(V_KEY_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_CH_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_CH_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_PAGE_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_BACK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_RECALL, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_TVRADIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_TV, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_RADIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_F1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_ALERT_MESS)
  #ifndef WIN32
  ON_EVENT(V_KEY_1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #else
  //lvcm
  ON_EVENT(V_KEY_1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  //ON_EVENT(V_KEY_1, MSG_CA_TEST1)
  #endif
  #ifndef WIN32
  ON_EVENT(V_KEY_2, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #else
  ON_EVENT(V_KEY_2, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #endif
  #ifndef WIN32
  ON_EVENT(V_KEY_3, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #else
  ON_EVENT(V_KEY_3, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #endif
  #ifndef WIN32
  ON_EVENT(V_KEY_4, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #else
  ON_EVENT(V_KEY_4, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  #endif
  //lvcm end
  ON_EVENT(V_KEY_5, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  ON_EVENT(V_KEY_6, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  ON_EVENT(V_KEY_7, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  ON_EVENT(V_KEY_8, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  ON_EVENT(V_KEY_9, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
  ON_EVENT(V_KEY_0, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)

  ON_EVENT(V_KEY_LEFT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
  ON_EVENT(V_KEY_RIGHT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
  ON_EVENT(V_KEY_VDOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
  ON_EVENT(V_KEY_VUP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)

  #ifdef OTA_CHECK_DEFAULT
  ON_EVENT(V_KEY_OK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_LIST)
  #else
  //lvcm
  ON_EVENT(V_KEY_OK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_LIST)
  //lvcm end
  #endif
  ON_EVENT(V_KEY_PROGLIST, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_LIST)
  //#ifndef WIN32
  ON_EVENT(V_KEY_FAV, MSG_OPEN_MENU_IN_TAB | ROOT_ID_FAV_LIST)
  //#else
  //ON_EVENT(V_KEY_FAV, MSG_OPEN_MENU_IN_TAB | ROOT_ID_CONDITIONAL_ACCEPT_FEED_PASSIVE)
  //#endif
  ON_EVENT(V_KEY_EPG, MSG_OPEN_MENU_IN_TAB | ROOT_ID_EPG)
  
  ON_EVENT(V_KEY_GAME, MSG_OPEN_MENU_IN_TAB | ROOT_ID_GAME_INN)
 
  ON_EVENT(V_KEY_AUDIO, MSG_SWITCH_AUDIO)
  ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
  
#ifndef  CUS_TONGGUANG_HEBI
  ON_EVENT(V_KEY_INFO, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_PRO_INFO)
  ON_EVENT(V_KEY_GREEN,MSG_HOTKEY_GREEN)
  ON_EVENT(V_KEY_BLUE,MSG_HOTKEY_BLUE)
  ON_EVENT(V_KEY_RED,MSG_HOTKEY_RED) 
  ON_EVENT(V_KEY_YELLOW,MSG_HOTKEY_YELLOW)
#else
  ON_EVENT(V_KEY_INFO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_F1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_RED, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_F3, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_PRO_INFO)
  ON_EVENT(V_KEY_YELLOW, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_PRO_INFO)
  
  ON_EVENT(V_KEY_F4, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_SUBMENU)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_MENU_IN_TAB | SN_ROOT_ID_SUBMENU)
#endif
  ON_EVENT(V_KEY_SEARCH, MSG_HOTKEY_SEARCH)
  ON_EVENT(V_KEY_OTA_SW_REVERT, MSG_REVERT_SW_ON_OTA)
  ON_EVENT(V_KEY_PRINTF_SWITCH, MSG_PRINTF_SWITCH)
  //ON_EVENT(V_KEY_RED, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PRO_INFO)
  //ON_EVENT(V_KEY_F2, MSG_CA_SHOW_ROLL_MSG)

  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(ui_desktop_keymap_on_normal, NULL)

// start key mapping on standby
BEGIN_KEYMAP(ui_desktop_keymap_on_standby, NULL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_ON)
END_KEYMAP(ui_desktop_keymap_on_standby, NULL)


// start key mapping on vcr
BEGIN_KEYMAP(ui_desktop_keymap_on_vcr, NULL)
  ON_EVENT(V_KEY_F2, MSG_SWITCH_SCART)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(ui_desktop_keymap_on_vcr, NULL)

#if(CONFIG_CAS_ID==CONFIG_CAS_ID_TR)
BEGIN_KEYMAP(pwdlg_keymap_in_ca, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
END_KEYMAP(pwdlg_keymap_in_ca, NULL)

BEGIN_MSGPROC(pwdlg_proc_in_ca, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
  ON_COMMAND(MSG_CA_MAIL_DEL_RESULT,on_ca_del_mail_update)
END_MSGPROC(pwdlg_proc_in_ca, cont_class_proc)
#endif

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
  ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_stop)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)




