/******************************************************************************/
/******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

/*!
  The width of full screen for video layer (in pixels).
  */
#define DISP_FULLSCR_WIDTH            720
/*!
  The height of full screen for video layer in PAL standard (in pixels).
  */
#define DISP_FULLSCR_HEIGHT_PAL       576


/*!
  The supported vdec fw cmd 
  */
typedef enum
{
  /*!
    VDEC FW START
    NOTE start from 0x100 cause other process on av cpu will share cmd_id
    */
  VDEC_FW_CMD_START = 0x1,
  /*!
    STOP
    */
  VDEC_FW_CMD_STOP,  
  /*!
    PAUSE
    */
  VDEC_FW_CMD_PAUSE,  
  /*!
    RESUME
    */
  VDEC_FW_CMD_RESUME,  
  /*!
    FREEZE
    */
  VDEC_FW_CMD_FREEZE,
  /*!
    TRICK
    */
  VDEC_FW_CMD_TRICK,
  /*!
    DEC FRAME
    */
  VDEC_FW_CMD_DECFRAME,  
  //add by HY for autotest 2012-3-28 begin
  /*!
    GET INFO
    */
  VDEC_FW_CMD_GET_INFO,
  //add by HY for autotest 2012-3-28 end
  /*!
    SET INFO
    */
  VDEC_FW_CMD_SET_INFO,

}vdec_fw_cmd_t;

/*!
  The supported aud fw cmd 
  */
typedef enum
{
  /*!
    AUD FW ATTACH
    */
  AUD_FW_CMD_ATTACH,
  /*!
    AUD FW QUERY BUFFER
    */
  AUD_FW_CMD_QUERY_BUFFER,
  /*!
    AUD FW QUERY BUFFER CALLBACK
    */
  AUD_FW_CMD_QUERY_BUFFER_CB,
  /*!
    AUD FW INIT
    */
  AUD_FW_CMD_INIT,
  /*!
    AUD FW START
    */
  AUD_FW_CMD_START,
  /*!
    STOP
    */
  AUD_FW_CMD_STOP,  
  /*!
    PAUSE
    */
  AUD_FW_CMD_PAUSE,  
  /*!
    RESUME
    */
  AUD_FW_CMD_RESUME,
  /*!
    SET PCM PARAM
    */
  AUD_FW_CMD_SET_PCM_PARAM,
  /*!
    DEINIT
    */
  AUD_FW_CMD_DEINIT,
  /*!
    set output format
    */
  AUD_FW_CMD_SET_OUTPUT_FORMAT,
  /*!
    chimera cmd
    */
  AUD_FW_CMD_CHIMERA,
  /*!
    AUD FW DETACH
    */
  AUD_FW_CMD_DETACH

}aud_fw_cmd_t;

/*!
//defines for SRC&DST_ID On AP&AV CPU
  */
#define CPU_BASE_ID_AP 0x10000000

/*!
//defines for SRC&DST_ID On AP&AV CPU
  */
#define CPU_BASE_ID_AV 0x20000000

/*!
//example for SRC&DST_ID
//src_id of vdec dev on ap cpu is
  */
#define AP_SYS_DEV_VDEC (CPU_BASE_ID_AP + SYS_DEV_TYPE_VDEC_VSB)
/*!
//example for SRC&DST_ID
//dst_id of vdec dev on av cpu is
  */
#define AV_SYS_DEV_VDEC (CPU_BASE_ID_AV +  SYS_DEV_TYPE_VDEC_VSB)

/*!
//example for SRC&DST_ID
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_START ((VDEC_FW_CMD_START << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_STOP ((VDEC_FW_CMD_STOP << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_PAUSE ((VDEC_FW_CMD_PAUSE << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_RESUME ((VDEC_FW_CMD_RESUME << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_FREEZE ((VDEC_FW_CMD_FREEZE << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_TRICK ((VDEC_FW_CMD_TRICK << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
//add by HY for autotest 2012-3-28 begin
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_GET_INFO ((VDEC_FW_CMD_GET_INFO << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
//add by HY for autotest 2012-3-28 end
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_SET_INFO ((VDEC_FW_CMD_SET_INFO << 16) + \
                                      SYS_DEV_TYPE_VDEC_VSB)
/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_CMD_DECFRAME ((VDEC_FW_CMD_DECFRAME << 16) \
                                        + SYS_DEV_TYPE_VDEC_VSB)

/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_VDEC_FW_MSG_DEPTH 8

/*!
//example for SRC&DST_ID
//src_id of aud dev on ap cpu is
  */
#define AP_SYS_DEV_AUD (CPU_BASE_ID_AP + SYS_DEV_TYPE_AUDIO)
/*!
//example for SRC&DST_ID
//src_id of aud dev on ap cpu is
  */
#define AV_SYS_DEV_AUD (CPU_BASE_ID_AV + SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_ATTACH        ((AUD_FW_CMD_ATTACH << 16) + \
                                              SYS_DEV_TYPE_AUDIO)
/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_QUERY_BUFFER  ((AUD_FW_CMD_QUERY_BUFFER << 16) + \
                                            SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_QUERY_BUFFER_CB ((AUD_FW_CMD_QUERY_BUFFER_CB << 16) + \
                                            SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_INIT  ((AUD_FW_CMD_INIT << 16) + \
                                   SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_START ((AUD_FW_CMD_START << 16) + \
                                    SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_STOP ((AUD_FW_CMD_STOP << 16) + \
                                          SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_PAUSE ((AUD_FW_CMD_PAUSE << 16) + \
                                        SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_RESUME ((AUD_FW_CMD_RESUME << 16) + \
                                      SYS_DEV_TYPE_AUDIO)


/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_SET_PCM_PARAM ((AUD_FW_CMD_SET_PCM_PARAM << 16) + \
                                      SYS_DEV_TYPE_AUDIO)

/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_DEINIT ((AUD_FW_CMD_DEINIT << 16) + \
                                            SYS_DEV_TYPE_AUDIO)
/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_DETACH        ((AUD_FW_CMD_DETACH << 16) + \
                                          SYS_DEV_TYPE_AUDIO)

/*!
  chimera cmd
  */
#define IPC_MSG_AUD_FW_CMD_CHIMERA       ((AUD_FW_CMD_CHIMERA << 16) + \
                                          SYS_DEV_TYPE_AUDIO)



/*!
//dst_id of aud dev on av cpu is
  */
#define IPC_MSG_AUD_FW_CMD_SET_PCM_PARAM ((AUD_FW_CMD_SET_PCM_PARAM << 16) + \
                                      SYS_DEV_TYPE_AUDIO)


/*!
//dst_id of vdec dev on av cpu is
  */
#define IPC_MSG_AUD_FW_MSG_DEPTH 8

/*!
  This structure defines the pos
  */
typedef struct
{
  /*!
    X pos
    */
  u32 x;
  /*!
    Y pos
    */
  u32 y;
} pos_t;

/*!
  This structure defines the rect size
  */
typedef struct
{
  /*!
    width
    */
  u32 w;
  /*!
    height
    */
  u32 h;
} rect_size_t;

/*!
  This structure defines the rect for driver usage
  */
typedef struct
{
  /*!
    X pos
    */
  u32 x;
  /*!
    Y pos
    */
  u32 y;
  /*!
    width
    */
  u32 w;
  /*!
    height
    */
  u32 h;
} rect_vsb_t;

/*!
  This structure defines the colorspace
  */
typedef enum
{
  /*!
    rgb
    */
  COLOR_RGB,
  /*!
    yuv
    */
  COLOR_YUV,
  /*!
    jazz: cvbs+s-video
    */  
  COLOR_SVID_CVBS,
  /*!
    jazz: cvbs+YUV
    */  
  COLOR_CVBS_YUV,
  /*!
    jazz: cvbs+RGB
    */  
  COLOR_CVBS_RGB,
  /*!
    jazz: 2cvbs+s-video
    */  
  COLOR_2CVBS_SVID,
} colorspace_t;

/*!
  This structure defines the pixfmt
  */
typedef enum
{
  /*!
    1-bit RGB Palette index 
    */
  PIX_FMT_RGBPALETTE1,      
  /*!
    2-bit RGB Palette index 
    */
  PIX_FMT_RGBPALETTE2,       
  /*!
    4-bit RGB Palette index
    */
  PIX_FMT_RGBPALETTE4,      
  /*!
    8-bit RGB Palette index
    */
  PIX_FMT_RGBPALETTE8,       
  /*!
    1-bit YUV Palette index
    */
  PIX_FMT_YUVPALETTE1,      
  /*!
    2-bit YUV Palette index
    */
  PIX_FMT_YUVPALETTE2,     
  /*!
    4-bit YUV Palette index 
    */
  PIX_FMT_YUVPALETTE4,     
  /*!
    8-bit YUV Palette index
    */
  PIX_FMT_YUVPALETTE8,       
  /*!
    A1 and 1-bit RGB Palette inde
    */
  PIX_FMT_ARGBPALETTE11,     
  /*!
    A2 and 2-bit RGB Palette index
    */
  PIX_FMT_ARGBPALETTE22,    
  /*!
    A4 and 4-bit RGB Palette index
    */
  PIX_FMT_ARGBPALETTE44,     
  /*!
    A8 and 8-bit RGB Palette index
    */
  PIX_FMT_ARGBPALETTE88,     
  /*!
    A1 and 1-bit YUV Palette index
    */
  PIX_FMT_AYUVPALETTE11,    
  /*!
    A2 and 2-bit YUV Palette index
    */
  PIX_FMT_AYUVPALETTE22,     
  /*!
    A4 and 4-bit YUV Palette index 
    */
  PIX_FMT_AYUVPALETTE44,     
  /*!
    A8 and 8-bit YUV Palette index
    */
  PIX_FMT_AYUVPALETTE88,    
  /*!
    8-bit, no per-pixel alpha
    */
  PIX_FMT_RGB233,         
  /*!
    16-bit, no per-pixel alpha
    */
  PIX_FMT_RGB565,         
  /*!
    16-bit
    */
  PIX_FMT_ARGB1555,     
  /*!
    16-bit
    */
  PIX_FMT_RGBA5551,       
  /*!
    16-bit
    */
  PIX_FMT_ARGB4444,       
  /*!
    16-bit
    */
  PIX_FMT_RGBA4444,       
  /*!
    32-bit
    */
  PIX_FMT_ARGB8888,       
  /*!
    32-bit
    */
  PIX_FMT_RGBA8888,      
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_Y0CBY1CR8888,  
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_Y0CRY1CB8888,  
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_Y1CBY0CR8888,  
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_Y1CRY0CB8888,   
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_CBY0CRY18888,  
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_CBY1CRY08888,   
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_CRY1CBY08888,  
  /*!
    32-bit for 2 pixels, YCbCr422, 8-bit values
    */
  PIX_FMT_CRY0CBY18888,  
  /*!
    32-bit for 1 pixel,  YCbCr444, 10-bit values
    */
  PIX_FMT_X2C10Y10CB10,  
  /*!
    YCbCr444, 8-bit values
    */
  PIX_FMT_AYCBCR8888,    
  /*!
    YCbCr444, 8-bit values
    */
  PIX_FMT_CRCBYA8888,    
  /*!
    YCbCr444, 8-bit values
    */
  PIX_FMT_YCBCRA8888,    
  /*!
    YCbCr444, 8-bit values
    */
  PIX_FMT_ACRCBY8888,    
  /*!
    YCbCr444, 24-bit values
    */
  PIX_FMT_YCBCR444,       
  /*!
    YCbCr422, 16-bit values
    */
  PIX_FMT_YCBCR422,      
  /*!
    YCbCr420, 12-bit values
    */
  PIX_FMT_YCBCR420,       
  /*!
    max
    */
  PIX_FMT_MAX
} pix_fmt_t;

/*!
  This structure defines the video formats
  */
typedef enum
{
  /*!
    SD NTSC J mode
    */
  VID_SYS_NTSC_J,
  /*!
    SD NTSC M mode
    */
  VID_SYS_NTSC_M,
  /*!
    SD NTSC 443 mode
    */
  VID_SYS_NTSC_443,
  /*!
    SD normal PAL mode
    */
  VID_SYS_PAL,          
  /*!
    SD PAL N mode
    */
  VID_SYS_PAL_N,
  /*!
    SD PAL Nc mode
    */
  VID_SYS_PAL_NC,
  /*!
    SD PAL M mode
    */
  VID_SYS_PAL_M,
  /*!
    SD SECAM mode
    */
  VID_SYS_SECAM,
  /*!
    HD 1080i
    */
  VID_SYS_1080I,                 
  /*!
    HD 1080i 50Hz, 1125 sample per line, SMPTE 274M
    */
  VID_SYS_1080I_50HZ,           
  /*!
    HD 1080p 60/59.94Hz, SMPTE 274M-1998
    */
  VID_SYS_1080P,                 
  /*!
    HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998
    */
  VID_SYS_1080P_24HZ,           
  /*!
    HD 1080p 25Hz, 2640 sample per line, SMPTE 274M-1998
    */
  VID_SYS_1080P_25HZ,            
  /*!
    HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998
    */
  VID_SYS_1080P_30HZ,           
  /*!
    HD 1080p 50Hz
    */
  VID_SYS_1080P_50HZ,           
  /*!
    HD 1250i 50Hz, another 1080i_50hz standard SMPTE 295M
    */
  VID_SYS_1250I_50HZ,           
  /*!
    HD 720p
    */
  VID_SYS_720P,                  
  /*!
    HD 720p 23.976/24Hz, 750 line, SMPTE 296M
    */
  VID_SYS_720P_24HZ,            
  /*!
    HD 720p 25Hz, 750 line, SMPTE 296M
    */
  VID_SYS_720P_25HZ,             
  /*!
    HD 720p 30Hz, 750 line, SMPTE 296M
    */
  VID_SYS_720P_30HZ,             
  /*!
    HD 720p 50Hz (Australia)
    */
  VID_SYS_720P_50HZ,            
  /*!
    SD 576p 50Hz (Australia)
    */
  VID_SYS_576P_50HZ,             
  /*!
    HD 480p
    */
  VID_SYS_480P,                 
  /*!
    NTSC 240p
    */
  VID_SYS_240P_60HZ,             
  /*!
    PAL 288p
    */
  VID_SYS_288P_50HZ,             
  /*!
    AUTO
    */
  VID_SYS_AUTO,
  /*!
    MAX
    */
  VID_SYS_MAX
} disp_sys_t;

/*!
  The luma and chroma sampling formats
  */
typedef enum
{
  /*!
    4:2:0
    */
  YUV_420,
  /*!
    4:2:2
    */
  YUV_422,
  /*!
    4:4:4
    */
  YUV_444
}yuv_sample_t;

/*!
  The aspect ratio
  */
typedef enum
{
  /*!
     4:3
     */
  AR_43,
  /*!
     16:9
     */
  AR_169,
  /*!
    Square or 1:1
    */
  AR_SQUARE
}aspect_ratio_t;

/*!
  GPIO pin information, 1 Byte
  */
typedef struct
{
  /*!
    Polarity of GPIO, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH active(light)
    */
  u16 polar:1;
  /*!
    GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
    */
  u16 io   :1;
  /*!
    GPIO index, upto 64 GPIO, 63 means invalid
    */
  u16 pos  :6;
  /*!
    Definition of GPIO level low 
    */
  u16 polar_0 :4;
  /*!
    Definition of GPIO level high
    */
  u16 polar_1 :4;
}gpio_pin_t;

//add by HY for autotest 2012-3-28 begin
/*!
  Autotest info
  */
typedef struct
{
  /*!
    Key frame.
    */
  int is_key_frame[50];
  /*!
    luma address
    */
  unsigned int luma_addr[50];
  /*!
    chroma address
    */
  unsigned int chroma_addr[50];
  /*!
    length
    */
  unsigned int height;
  /*!
    width
    */
  unsigned int width;
}autotest_info;
//add by HY for autotest 2012-3-28 end

#endif //__COMMON_H__

