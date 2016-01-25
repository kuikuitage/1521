/******************************************************************************/
/******************************************************************************/
#ifndef __AP_UIO_H_
#define __AP_UIO_H_


/*!
  \file
  
  Application UIO translate hot keys from driver layer into 
  virtual keys and Notify the virtual key to UI layer
  */

/*!
  Max message number supported in AP UIO
  */
#define MAX_UIO_MSG_NUM (8)

/*!
  Invalid key
  */
#define INVALID_KEY 0xFFFF
/*!
  All supported virtual keys
  */
enum v_key
{
  /*!
    Invalid key
    */
  V_KEY_INVALID = 0,
  /*!
    Power key
    */
  V_KEY_POWER,
  /*!
    Mute key
    */
  V_KEY_MUTE,
  /*!
    Recall key 
    */
  V_KEY_RECALL,
  /*!
    TV/Radio switch
    */
  V_KEY_TVRADIO,
  /*!
    Key 0
    */
  V_KEY_0,
  /*!
    Key 1 
    */
  V_KEY_1,
  /*!
    Key 2 
    */
  V_KEY_2,
  /*!
    Key 3
    */
  V_KEY_3,
  /*!
    Key 4
    */
  V_KEY_4,
  /*!
    Key 5
    */
  V_KEY_5,
  /*!
    Key 6
    */
  V_KEY_6,
  /*!
    Key 7
    */
  V_KEY_7,
  /*!
    Key 8
    */
  V_KEY_8,
  /*!
    Key 9
    */
  V_KEY_9,
  /*!
    Cancel key
    */
  V_KEY_CANCEL,
  /*!
    OK key
    */
  V_KEY_OK,
  /*!
    Up key
    */
  V_KEY_UP,
  /*!
    Down key
    */
  V_KEY_DOWN,
  /*!
    Left key
    */
  V_KEY_LEFT,
  /*!
    Right key
    */
  V_KEY_RIGHT,
  /*!
    Menu key
    */
  V_KEY_MENU,
  /*!
    Porgram list key
    */
  V_KEY_PROGLIST,
  /*!
    Audio key
    */
  V_KEY_AUDIO,
  /*!
    Page up key
    */
  V_KEY_PAGE_UP,
  /*!
    Page down key
    */
  V_KEY_PAGE_DOWN,
  /*!
    Infor Key
    */
  V_KEY_INFO,
  /*!
    Favorite key
    */
  V_KEY_FAV,
  /*!
    Pause key
    */
  V_KEY_PAUSE,
  /*!
    Play key
    */
  V_KEY_PLAY,
  /*!
    9 Picture key
    */
  V_KEY_9PIC,
  /*!
    EPG key
    */
  V_KEY_EPG,
  /*!
    SI key
    */
  V_KEY_SI,
  /*!
    Gray key
    */
  V_KEY_GRAY,
  /*!
    Red key 
    */
  V_KEY_RED,
  /*!
    Green key
    */
  V_KEY_GREEN,
  /*!
    Yellow key
    */
  V_KEY_YELLOW,
  /*!
    Blue key
    */
  V_KEY_BLUE,
  /*!
    UPG key
    */
  V_KEY_UPG,
  /*!
    TTX key
    */
  V_KEY_TTX,
  /*!
    Test key
    */
  V_KEY_TEST,
  /*!
    I2C key
    */
  V_KEY_I2C,
  /*!
    F2 Key
    */
  V_KEY_F1,
  /*!
    F2 Key
    */
  V_KEY_F2,
  /*!
    F3 Key
    */
  V_KEY_F3,
  /*!
    F4 Key
    */
  V_KEY_F4,
  /*!
    F5 Key
    */
  V_KEY_F5,
  /*!
    STOCK Key
    */
  V_KEY_STOCK,
  /*!
    BOOK Key
    */
  V_KEY_BOOK,
  /*!
    TV Key
    */
  V_KEY_TV,
  /*!
    RADIO Key
    */
  V_KEY_RADIO,
  /*!
    OTA force key
    */
  V_KEY_OTA_FORCE,
  /*!
    SAT key
    */
  V_KEY_SAT,
  /*!
    ENTER ucas
    */
  V_KEY_UCAS,
  /*!
    ENTER satcode
    */
  V_KEY_SAT_CODE,
  /*!
    ctrl + 0
    */
  V_KEY_CTRL0,
  /*!
    ctrl + 1
    */
  V_KEY_CTRL1,
  /*!
    ctrl + 2
    */  
  V_KEY_CTRL2,
  /*!
    ctrl + 3
    */  
  V_KEY_CTRL3,
  /*!
    ctrl + 4
    */  
  V_KEY_CTRL4,
  /*!
    ctrl + 5
    */  
  V_KEY_CTRL5,
  /*!
    ctrl + 6
    */  
  V_KEY_CTRL6,
  /*!
    ctrl + 7
    */  
  V_KEY_CTRL7,
  /*!
    ctrl + 8
    */  
  V_KEY_CTRL8,
  /*!
    ctrl + 9
    */  
  V_KEY_CTRL9, 
  /*!
    ctrl + s0
    */
  V_KEY_CTRLS0,
  /*!
    ctrl + s1
    */
  V_KEY_CTRLS1,
  /*!
    ctrl + s2
    */  
  V_KEY_CTRLS2,
  /*!
    ctrl + s3
    */  
  V_KEY_CTRLS3,
  /*!
    ctrl + s4
    */  
  V_KEY_CTRLS4,
  /*!
    ctrl + s5
    */  
  V_KEY_CTRLS5,
  /*!
    ctrl + s6
    */  
  V_KEY_CTRLS6,
  /*!
    ctrl + s7
    */  
  V_KEY_CTRLS7,
  /*!
    ctrl + s8
    */  
  V_KEY_CTRLS8,
  /*!
    ctrl + s9
    */  
  V_KEY_CTRLS9,
  /*!
    pos
    */  
  V_KEY_POS,
  /*!
    rec
    */  
  V_KEY_REC,
  /*!
    pn
    */  
  V_KEY_PN,
  /*!
    ts
    */  
  V_KEY_TS,
  /*!
    back2
    */  
  V_KEY_BACK2,
  /*!
    forw2
    */  
  V_KEY_FORW2,
  /*!
    back
    */  
  V_KEY_BACK,
  /*!
    forw
    */  
  V_KEY_FORW,
  /*!
    revslow
    */  
  V_KEY_REVSLOW,
  /*!
    slow
    */  
  V_KEY_SLOW,
  /*!
    stop
    */  
  V_KEY_STOP,
  /*!
    vdown
    */  
  V_KEY_VDOWN,
  /*!
    vup
    */  
  V_KEY_VUP,
  /*!
    subt
    */  
  V_KEY_SUBT,
  /*!
    zoom
    */  
  V_KEY_ZOOM,
  /*!
    tvsat
    */  
  V_KEY_TVSAT,
  /*!
    find
    */  
  V_KEY_FIND,
  /*!
    d
    */  
  V_KEY_D,
  /*!
    tvav
    */  
  V_KEY_TVAV,
  /*!
    twin port
    */
  V_KEY_TWIN_PORT,
  /*!
    CA Hide Menu
    */
  V_KEY_CA_HIDE_MENU,
  /*!
    colorbar
    */
  V_KEY_COLORBAR,
  /*!
    scan
    */
  V_KEY_SCAN,
  /*!
    tp list
    */
  V_KEY_TPLIST,
  /*!
    scan
    */
  V_KEY_VOLUP,
  /*!
    tp list
    */
  V_KEY_VOLDOWN,
  /*!
    language switch
    */
  V_KEY_LANG,
  /*!
    exit
    */
  V_KEY_EXIT,
  /*!
    sleep
    */
  V_KEY_SLEEP,
  /*!
    video mode
    */
  V_KEY_VIDEO_MODE,
  /*!
    game
    */
  V_KEY_GAME,  
  /*!
    tv playback
    */
  V_KEY_TV_PLAYBACK,  
  /*!
    p2p
  */
  V_KEY_P2P,
  /*!
    VBI_INSERTER
  */
  V_KEY_VBI_INSERTER,
  /*!
    ASPECT_MODE
  */
  V_KEY_ASPECT_MODE,
  /*!
    DISPLAY_MODE
  */
  V_KEY_DISPLAY_MODE,
  /*!
    LANGUAGE_SWITCH
  */
  V_KEY_LANGUAGE_SWITCH,
  /*!
    PLAY_TYPE
  */
  V_KEY_PLAY_TYPE,
  /*!
    BEEPER
  */
  V_KEY_BEEPER,
  /*!
    CHANNEL_CHANGE
  */
  V_KEY_CHANNEL_CHANGE,
  /*!
    LNB_POWER
  */
  V_KEY_LNB_POWER,
  /*!
    OSD_TRANSPARENCY
  */
  V_KEY_OSD_TRANSPARENCY,
  /*!
    LOOP_THROUGH
  */
  V_KEY_LOOP_THROUGH,
  /*!
    BISS_KEY
  */
  V_KEY_BISS_KEY,
  /*!
    SEARCH key
    */
  V_KEY_SEARCH,
  /*!
    SEARCH key
    */
  V_KEY_MAIL,
  /*!
    FACTORY key
    */
  V_KEY_FACTORY,
  /*!
    FavUp key
    */
  V_KEY_FAVUP,
  /*!
    FavDown key
    */
  V_KEY_FAVDOWN,
  /*!
    NVOD key
    */
  V_KEY_NVOD,
  /*!
    Data broadcast key
    */
  V_KEY_DATA_BROADCAST,
      /*!
    Goto key
    */
  V_KEY_GOTO,
    /*!
    CA Info key
    */
  V_KEY_CA_INFO,
   /*!
    CH Up
    */
  V_KEY_CH_UP,
   /*!
    CH Down
    */
  V_KEY_CH_DOWN,
  
  /*!
    Unkown key
    */
  V_KEY_UNKNOW
};

/*!
  UIO commands definition
  */
typedef enum
{
  /*!
    UIO task start command
    */
  AP_UIO_CMD_START = 0,
  /*!
    Front pannel display commands
    */
  AP_UIO_CMD_FP_DISPLAY,
  /*!
    UIO end command
    */
  AP_UIO_CMD_END
} ap_uio_cmd_t;

/*!
  UIO event defintion
  */
typedef enum
{
  /*!
    UIO task start running
    */
  UIO_EVT_BEGIN = (APP_UIO << 16),
  /*!
    UIO key notify
    */
  UIO_EVT_KEY
} uio_evt_t;

/*!
  Key type definition
  */
typedef enum
{
  /*!
    IRDA key
    */
  IRDA_KEY = 0,
  /*!
    Front pannel key
    */
  FP_KEY,
  /*!
    Both FP key and IRDA input is valid  
    */
  ALL_KEY_TYPE
} key_type_t;

/*!
  Parameter define for virtual key 
  */
typedef struct
{
  /*!
    Remote id
    */
  u8  usr;
  /*!
    Virtual key value
    */
  u16 v_key;
  /*!
    Virtual key value 2
    */
  u16 v_key_2;
  /*!
    Virtual key value 3
    */
  u16 v_key_3;
  /*!
    Virtual key value 4
    */
  u16 v_key_4;  
  /*!
    Key type
    */
  key_type_t type;
} v_key_t;

/*!
  Key map
  */
typedef struct
{
  /*!
    Hot key
    */
  u8 h_key;
  /*!
    Virtual key
    */
  u16 v_key;
} key_map_t;


/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
    Initialize key map
    */
  void (*p_init_kep_map)(void);
  /*!
    Translate key and return key translation complete status
    \param[in] key parameter for translation 
    */
  BOOL (*p_translate_key)(v_key_t *p_key);
  /*!
    set irda led when ir push
    */
  void (*p_set_irda_led)(void);
}ap_uio_policy_t;

/*!
  Get UIO instance and return UIO instance
  \param[in] p_uio_policy UIO application implementation policy
  */
app_t *construct_ap_uio(ap_uio_policy_t *p_uio_policy);

#endif // End for __AP_UIO_H_

