#include "stdafx.h"
#include <Izard/SystemConfigure/system_configure_native_interface.h>
#include <windows.h>
#include <stdarg.h>
#include "FlashParser.h"
///////////////////////////////////////////////////////////////////////////////
//uncomment following clause to enable log print
///////////////////////////////////////////////////////////////////////////////
#define PRINT_LOG_ENABLED

#define revert_endian_16(x)  (((x&0xff)<<8)|(x>>8))
#define revert_endian_32(x)  (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))
static bool g_bBigEdian = TRUE;

#define SYS_CONFIG_SW_VERSION_KEY               L"软件版本号"
#define SYS_CONFIG_MAIN_FREQ_KEY                L"主频点(MHz)"
#define SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY       L"备用频点1(MHz)"
#define SYS_CONFIG_UPGRADE_FREQ_KEY             L"升级频点(MHz)"
#define SYS_CONFIG_AD_FREQ_KEY                  L"广告频点(MHz)"
#define SYS_CONFIG_NVOD_FREQ_KEY                L"NVOD频点(MHz)"
#define SYS_CONFIG_PRINT_FLAG_KEY               L"打印信息"
#define SYS_CONFIG_LANGUAGE_SET_KEY             L"语言设置"
#define SYS_CONFIG_VIDEO_OUTPUT_KEY             L"视频输出设置"
#define SYS_CONFIG_VOLUME_MODE_KEY              L"音频模式"
#define SYS_CONFIG_GLOBAL_VOLUME                L"全局音量大小"
#define SYS_CONFIG_TRACK_MODE                   L"声道模式"
#define SYS_CONFIG_AUDIO_TRACK_KEY              L"音频声道"
#define SYS_CONFIG_OTA_OUI                      L"触发OUI"
#define SYS_CONFIG_OTA_HW_MOD_ID_KEY            L"硬件MOD ID"
#define SYS_CONFIG_OTA_SW_MOD_ID_KEY            L"软件MOD ID"
#define SYS_CONFIG_OTA_HW_VERSION_KEY               L"OTA硬件版本号"
#define SYS_CONFIG_OTA_SW_VERSION_KEY               L"OTA软件版本号"

typedef struct
{
  u32 oui;
  u16 manufacture_id;
  u16 hw_mod_id;
  u16 sw_mod_id;
  u16 hw_version;
  u16 sw_version;
  u16 reserved1;
  u32 reserved;
}ota_tdi_tmp_t;

typedef struct 
{
  char debug_info;
  char sig_str_ratio;
  char sig_str_max;
  char sig_qua_max;
  char str_l_qua;
  unsigned int str_0_gain;
  char bs_times;
  char spi_clk;
  char rd_mode;
  char standby_mode;
  char watchdog_mode;
  char str_poweron[10];
  char str_standby[10];
  char str_sw_version[20]; 
  char str_hw_version[20];
  char str_last_upgrade_date[20];
  ota_tdi_tmp_t ota_tdi;
}misc_options_t;

static sys_status_t g_status;
static misc_options_t g_misc_options;

FlashParser g_FlashParser;
wchar_t g_LastErrorMsg[256];


#ifdef PRINT_LOG_ENABLED
void LOGMSG(const char *fmt, ... )
{
	if (fmt==NULL || strlen(fmt)==0)
		return;
	
	char *msg = new char[1024];
	int   len = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);
	len = sprintf(msg, "%04d-%02d-%02d %02d:%02d:%02d.%03d, ",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);

	va_list ap;
	va_start(ap, fmt);
	vsprintf(msg+len, fmt, ap);
	va_end(ap);

	FILE *fp;
  fp = fopen("sys_config.log", "a");
  fprintf(fp, "%s\n", msg);
  fclose(fp);
	delete [] msg;
}
#else
#define LOGMSG
#endif

int CheckSWVersion(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  char szValue[MIDDLELENGTH];
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_SW_VERSION_KEY) != 0)
  {
    LOGMSG("INVLAID SW VERSION KEY");
    wcscpy(g_LastErrorMsg, L"软件版本号键值不正确");
    return 0;
  }
  if (property->value[0] != L'0' || (property->value[1] != L'x' && property->value[1] != L'X') || wcslen(property->value) != 10)
  {
    LOGMSG("[%s, %d] INVALID SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"软件版本号格式不正确（应是以小写0x开头的十六进制数字）");
    return 0;
  }
  for(u32 i=0; i<8; i++)
  {
    if (!iswxdigit(property->value[i+2]))
    {
      LOGMSG("[%s, %d] INVALID SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
      wcscpy(g_LastErrorMsg, L"软件版本号格式不正确（应是以小写0x开头的十六进制数字）");
      return 0;
    }
  }
  LOGMSG("[%s, %d] GOOD SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int isdigitnum(wchar_t *str)
{
  size_t size = wcslen(str);
  for(size_t i=0; i<size; i++)
  {
    if(!iswdigit(str[i]))
    {
      return 0;
    }
  }
  return 1;
}

int CheckFreqency(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  char szValue[MIDDLELENGTH];
  int nValue;

  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_MAIN_FREQ_KEY) != 0 
    && wcscmp(property->key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_UPGRADE_FREQ_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_AD_FREQ_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_NVOD_FREQ_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"频点键值不正确");
    return 0;
  }
  
  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"频点频率应介于45~999之间");
    return 0;
  }

  nValue = atoi(szValue);
  if (nValue<45 || nValue>999)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"频点频率应介于45~999之间");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckPrintFlag(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_PRINT_FLAG_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID PRINT FLAG KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"打印信息键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"关闭") != 0 
    && wcscmp(property->value, L"打开") != 0)
  {
    LOGMSG("[%s, %d] INVALID PRINT FLAG VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"打印信息设置不正确，应为打开或关闭");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD PRINT FLAG VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckLanguage(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_LANGUAGE_SET_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID LANGUAGE SET KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"语言设置键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"英语") != 0 
    && wcscmp(property->value, L"av_set") != 0)
  {
    LOGMSG("[%s, %d] INVALID LANGUAGE SET VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"语言设置不正确，应为英语或av_set");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD LANGUAGE SET VALUEs", __FUNCTION__, __LINE__);
  return 1;
}
int CheckVideoOutput(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_VIDEO_OUTPUT_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID VIDEO OUTPUT KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"视频输出键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"CVBS") != 0 
    && wcscmp(property->value, L"SVDIEO") != 0
    && wcscmp(property->value, L"RGB") != 0
    && wcscmp(property->value, L"YUV") != 0
    && wcscmp(property->value, L"CVBS_YUV") != 0
    && wcscmp(property->value, L"CVBS_SVDIEO") != 0
    && wcscmp(property->value, L"CVBS_CVBS") != 0)
  {
    LOGMSG("[%s, %d] INVALID VIDEO OUTPUT VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"视频输出设置不正确");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD VIDEO OUTPUT VALUEs", __FUNCTION__, __LINE__);
  return 1;
}
int CheckVolumeMode(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_VOLUME_MODE_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID VOLUME MODE KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"音频控制键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"单一音量") != 0 
    && wcscmp(property->value, L"全局音量") != 0)
  {
    LOGMSG("[%s, %d] INVALID VOLUME MODE VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"音频控制设置不正确，应为单一音量或全局音量");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD VOLUME MODE VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckGlobalVolume(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  char szValue[MIDDLELENGTH];
  int nValue;
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_GLOBAL_VOLUME) != 0)
  {
    LOGMSG("[%s, %d] INVALID GLOBAL VOLUME KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"全局音量大小键值不正确");
    return 0;
  }
  
  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID GLOBAL VOLUME value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"全局音量大小不正确,应介于0~32之间");
    return 0;
  }

  nValue = atoi(szValue);
  if (nValue<0 || nValue>32)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"全局音量大小不正确,应介于0~32之间");
    return 0;
  }

  LOGMSG("[%s, %d] GOOD GLOBAL VOLUME value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckTrackMode(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_TRACK_MODE) != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK MODE KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"声道控制键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"单一声道") != 0 
    && wcscmp(property->value, L"全局声道") != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK MODE VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"声道控制设置不正确，应为单一声道或全局声道");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD TRACK MODE VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckTrack(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_AUDIO_TRACK_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID AUDIO TRACK KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"音频声道键值不正确");
    return 0;
  }

  if (wcscmp(property->value, L"立体声") != 0 
    && wcscmp(property->value, L"左声道") != 0
    && wcscmp(property->value, L"右声道") != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"声道设置不正确，应为立体声、左声道或右声道");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD AUDIO TRACK VALUE", __FUNCTION__, __LINE__);
  return 1;
}

int CheckOui(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  char szValue[MIDDLELENGTH];
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_OTA_OUI) != 0)
  {
    LOGMSG("INVLAID OTA OUI KEY");
    wcscpy(g_LastErrorMsg, L"OTA OUI键值不正确");
    return 0;
  }
  if (property->value[0] != L'0' || (property->value[1] != L'x' && property->value[1] != L'X') || wcslen(property->value) != 10)
  {
    LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"OTA OUI格式不正确（应是以小写0x开头的十六进制数字）");
    return 0;
  }
  for(u32 i=0; i<4; i++)
  {
    if (!iswxdigit(property->value[i+2]))
    {
      LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
      wcscpy(g_LastErrorMsg, L"OTA OUI格式不正确（应是以小写0x开头的十六进制数字）");
      return 0;
    }
  }
  LOGMSG("[%s, %d] GOOD OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}
int CheckID(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  char szValue[MIDDLELENGTH];

  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_OTA_HW_MOD_ID_KEY) != 0
   && wcscmp(property->key, SYS_CONFIG_OTA_SW_MOD_ID_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA MOD ID KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"MOD ID键值不正确，应为十进制数字");
    return 0;
  }
  
  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA MOD ID value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"MOD ID正确");
    return 0;
  }

  LOGMSG("[%s, %d] GOOD OTA MOD ID value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckOTAHWVersion(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_OTA_HW_VERSION_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA HW VERSION KEY VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA硬件版本号键值不正确");
    return 0;
  }

  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA HW VERSION VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA硬件版本号格式不正确，应为十进制数字");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD OTA HW VERSION VALUE", __FUNCTION__, __LINE__);
  return 1;
}

int CheckOTASWVersion(struct NativeProperty* property, struct NativePropertyArray* all_propertys)
{
  if (wcscmp(property->key, SYS_CONFIG_OTA_SW_VERSION_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA SW VERSION KEY VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA软件版本号键值不正确");
    return 0;
  }

  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA SW VERSION VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA软件版本号格式不正确，应为十进制数字");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD OTA SW VERSION VALUE", __FUNCTION__, __LINE__);
  return 1;
}
int ProjectInitial(wchar_t* initial_parameter)
{
  //DO NOTHING HERE
  return 0;
}

void SSData_endian_convert()
{
  g_status.sw_version = revert_endian_32(g_status.sw_version);
  g_status.upgrade_tp.tp_freq = revert_endian_32(g_status.upgrade_tp.tp_freq);
  g_status.main_tp1.tp_freq = revert_endian_32(g_status.main_tp1.tp_freq);
  g_status.main_tp2.tp_freq = revert_endian_32(g_status.main_tp2.tp_freq);
  g_status.nvod_tp.tp_freq = revert_endian_32(g_status.nvod_tp.tp_freq);
  g_status.ad_tp.tp_freq = revert_endian_32(g_status.ad_tp.tp_freq);
}

int Load(wchar_t* file_path, struct NativePropertyArray* all_propertys)
{
  u32 size = 0;

  g_FlashParser.Clear();
  if(g_FlashParser.LoadBinFile(file_path) == FALSE)
  {
    return FALSE;
  }
  //Load SSData
  if (g_FlashParser.GetSSData((char *)&g_status, sizeof(g_status)) == FALSE)
  {
    return FALSE;
  }
  else if(g_bBigEdian == TRUE)
  {
    SSData_endian_convert();
  }
  //Load MiscOptions
  if (g_FlashParser.GetMiscOptions((char *)&g_misc_options, sizeof(g_misc_options)) == FALSE)
  {
    return FALSE;
  }

  //arrange properties
  //all_propertys->size = 10;
  all_propertys->first = new NativeProperty[50];
  memset(all_propertys->first, 0, sizeof(NativeProperty)*50);

  //-------------------------
  // Basic Settings
  //-------------------------
  //sw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0x%08x", g_status.sw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckSWVersion;
  size++;
  //main freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_MAIN_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp1.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckFreqency;
  size++;
  //main freq backup 1
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp2.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckFreqency;
  size++;
  //upgrade freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_UPGRADE_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.upgrade_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckFreqency;
  size++;
  //ad freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AD_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.ad_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckFreqency;
  size++;
  //nvod freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_NVOD_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.nvod_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckFreqency;
  size++;
  //print flag
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_PRINT_FLAG_KEY);
  switch(g_status.printf_switch_set.printf_flag)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"关闭");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"打开");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"关闭");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"关闭;打开;");
  all_propertys->first[size].m_validation = CheckPrintFlag;
  size++;
  //language set
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_LANGUAGE_SET_KEY);
  switch(g_status.lang_set.osd_text)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"英语");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"av_set");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"英语");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"英语;av_set;");
  all_propertys->first[size].m_validation = CheckLanguage;
  size++;
  //video output
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VIDEO_OUTPUT_KEY);
  switch(g_status.av_set.video_output)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"SVDIEO");
    break;
  case 2:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"RGB");
    break;
  case 3:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"YUV");
    break;
  case 4:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_YUV");
    break;
  case 5:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_SVDIEO");
    break;
  case 6:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_CVBS");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_YUV");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"CVBS;SVDIEO;RGB;YUV;CVBS_YUV;CVBS_SVDIEO;CVBS_CVBS;");
  all_propertys->first[size].m_validation = CheckVideoOutput;
  size++;
  //volume mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VOLUME_MODE_KEY);
  switch(g_status.audio_set.is_global_volume)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"单一音量");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"全局音量");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"单一音量");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"单一音量;全局音量;");
  all_propertys->first[size].m_validation = CheckVolumeMode;
  size++;
  //global volume
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_GLOBAL_VOLUME);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d",g_status.audio_set.global_volume);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  all_propertys->first[size].m_validation = CheckGlobalVolume;
  size++;
  //track mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TRACK_MODE);
  switch(g_status.audio_set.is_global_track)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"单一声道");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"全局声道");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"单一声道");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"单一声道;全局声道;");
  all_propertys->first[size].m_validation = CheckTrackMode;
  size++;
  //audio track
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AUDIO_TRACK_KEY);
  switch(g_status.audio_set.global_track)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"立体声");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"左声道");
    break;
  case 2:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"右声道");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"立体声");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"基本设置");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"立体声;左声道;右声道;");
  all_propertys->first[size].m_validation = CheckTrack;
  size++;

  //-------------------------
  //OTA UPGRADE
  //-------------------------
  //ota oui
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_OUI);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0x%04x", g_misc_options.ota_tdi.oui);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA升级参数设置");
  all_propertys->first[size].m_validation = CheckOui;
  size++;
  //hw mod id
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_HW_MOD_ID_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.hw_mod_id);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA升级参数设置");
  all_propertys->first[size].m_validation = CheckID;
  size++;
  //sw mod id
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_SW_MOD_ID_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.sw_mod_id);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA升级参数设置");
  all_propertys->first[size].m_validation = CheckID;
  size++;
  //hw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_HW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.hw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA升级参数设置");
  all_propertys->first[size].m_validation = CheckOTAHWVersion;
  size++;
  //sw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_SW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.sw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA升级参数设置");
  all_propertys->first[size].m_validation = CheckOTASWVersion;
  size++;
  all_propertys->size = size;
  return TRUE;
}

int Save(wchar_t* file_path, struct NativePropertyArray* all_propertys)
{
  char value[MIDDLELENGTH];  //the property's value

  for(unsigned int size=0; size<all_propertys->size; size++)
  {
    //sw version
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SW_VERSION_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      sscanf(value, "0x%08x", &g_status.sw_version);
    }

    //main freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_MAIN_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.main_tp1.tp_freq = atoi(value)*1000;
    }

    //main freq backup 1
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.main_tp2.tp_freq = atoi(value)*1000;
    }

    //upgrade freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_UPGRADE_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.upgrade_tp.tp_freq = atoi(value)*1000;
    }

    //ad freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AD_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.ad_tp.tp_freq = atoi(value)*1000;
    }

    //nvod freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_NVOD_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.nvod_tp.tp_freq = atoi(value)*1000;
    }

    //print flag
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_PRINT_FLAG_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"关闭") == 0)
      {
        g_status.printf_switch_set.printf_flag = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"打开") == 0)
      {
        g_status.printf_switch_set.printf_flag = 1;
      }
      else
      {
        g_status.printf_switch_set.printf_flag = 0;
      }
    }
    //language set
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_LANGUAGE_SET_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"英语") == 0)
      {
        g_status.lang_set.osd_text = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"av_set") == 0)
      {
        g_status.lang_set.osd_text = 1;
      }
      else
      {
        g_status.lang_set.osd_text = 0;
      }
    }
    //video output
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VIDEO_OUTPUT_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"CVBS") == 0)
      {
        g_status.av_set.video_output = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"SVDIEO") == 0)
      {
        g_status.av_set.video_output = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"RGB") == 0)
      {
        g_status.av_set.video_output = 2;
      }
      else if (wcscmp(all_propertys->first[size].value, L"YUV") == 0)
      {
        g_status.av_set.video_output = 3;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_YUV") == 0)
      {
        g_status.av_set.video_output = 4;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_SVDIEO") == 0)
      {
        g_status.av_set.video_output = 5;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_CVBS") == 0)
      {
        g_status.av_set.video_output = 6;
      }
      else
      {
        g_status.av_set.video_output = 4;
      }
    }
    //volume mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VOLUME_MODE_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"单一音量") == 0)
      {
        g_status.audio_set.is_global_volume = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"全局音量") == 0)
      {
        g_status.audio_set.is_global_volume = 1;
      }
      else
      {
        g_status.audio_set.is_global_volume = 0;
      }
    }
    //global volume
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_GLOBAL_VOLUME) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.audio_set.global_volume = (u8)atoi(value);
    }
    //track mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TRACK_MODE) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"单一声道") == 0)
      {
        g_status.audio_set.is_global_track = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"全局声道") == 0)
      {
        g_status.audio_set.is_global_track = 1;
      }
      else
      {
        g_status.audio_set.is_global_track = 0;
      }
    }
    //global track
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AUDIO_TRACK_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"立体声") == 0)
      {
        g_status.audio_set.global_track = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"左声道") == 0)
      {
        g_status.audio_set.global_track = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"右声道") == 0)
      {
        g_status.audio_set.global_track = 2;
      }
      else
      {
        g_status.audio_set.global_track = 0;
      }
    }

    //ota oui
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_OUI) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      sscanf(value, "0x%04x", &g_misc_options.ota_tdi.oui);
    }

    //hw mod id
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_HW_MOD_ID_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.hw_mod_id = (u16)atoi(value);
    }

    //sw mod id
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_SW_MOD_ID_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.sw_mod_id = (u16)atoi(value);
    }

    //hw version
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_HW_VERSION_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.hw_version = (u16)atoi(value);
    }

   //sw version
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_SW_VERSION_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.sw_version = (u16)atoi(value);
    }
  }
  if(g_bBigEdian == TRUE)
  {
    SSData_endian_convert();
  }
  g_FlashParser.SaveSSData((char *)&g_status, sizeof(g_status));
  g_FlashParser.SaveMiscOptions((char*)&g_misc_options, sizeof(g_misc_options));

  //write new bin file
  return g_FlashParser.SaveBinFile(file_path);
}

int Clean()
{
  return 0;
}

void GetLastError(wchar_t* info, size_t info_length)
{
  wcsncpy(info, g_LastErrorMsg, info_length);
}