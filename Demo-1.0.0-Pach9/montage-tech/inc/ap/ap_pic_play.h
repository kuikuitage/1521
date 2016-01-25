/******************************************************************************/
/******************************************************************************/
#ifndef __AP_PIC_PLAYER_H_
#define __AP_PIC_PLAYER_H_

/*!
  max gif count one time
  */
#define MAX_GIF_CNT (3)

/*!
  Application pic player evt enum
  */
typedef enum
{
  /*!
    file play end
    */
  PIC_PLAYER_EVT_DRAW_END = ((APP_PICTURE << 16)|ASYNC_EVT),
  /*!
    update frame, for gif
    */
  PIC_PLAYER_UPDATE_FRAME,
  /*!
    can not play the file
    */
  PIC_PLAYER_EVT_DRAW_FAIL,
  /*!
    picture play start
    */
  PIC_PLAYER_EVT_START = ((APP_PICTURE << 16)|SYNC_EVT),
  /*!
    picture play stoped
    */
  PIC_PLAYER_EVT_STOPED,
}pic_player_evt_t;

/*!
  Command definition in application player
  */
typedef enum
{
  /*!
    player start
    */
  PIC_CMD_START,
  /*!
    player stop
    */
  PIC_CMD_STOP,
  /*!
    player sync start
    */
  PIC_CMD_SYNC_START = SYNC_CMD,
  /*!
    player sync stop
    */
  PIC_CMD_SYNC_STOP,
  
}pic_cmd_t;

/*!
   ap_pic_policy_t
  */
typedef struct
{
  /*!
    the priority 
    */
  u32 pic_chain_priority[MAX_GIF_CNT];
  /*!
    max picture count one time
    */
  u32 max_pic_cnt_one_time;
} ap_pic_policy_t;

/*!
  tag_ap_jpeg_play_data
  */
typedef struct tag_ap_pic_play_data
{
  /*!
    play file name
    */
  char file_name[MAX_FILE_PATH];
  /*!
    file size
    */
  u32 file_size;
  /*!
    file or mem
    */
  BOOL file_or_mem;
  /*!
    buffer get
    */
  u32 buf_get;
  /*!
    mem
    */
  u8 *p_mem;
  /*!
    mem size
    */
  u32 mem_size;
  /*!
    rect
    */
  rect_t rect;
  /*!
    region handle
    */
  void *p_rgn;
  /*!
    pdec mode
    */
  dec_mode_t dec_mode;
  /*!
    is enable color key
    */
  BOOL enable_key;
  /*!
    color key
    */  
  u32 color_key;  
}ap_pic_data_t;

/*!
  Get the instance of application record
  \param[in] p_epg_policy implementation policy to be set
  */
app_t *construct_ap_pic_play(ap_pic_policy_t *p_policy);


#endif // End for __AP_PIC_PLAYER_H_

