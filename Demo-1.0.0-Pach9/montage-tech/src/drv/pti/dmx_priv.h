/******************************************************************************/
/******************************************************************************/

#ifndef __DMX_PRIV_H__
#define __DMX_PRIV_H__

/*!
    invalid pid
  */
#define INVALID_PID                 0x1FFF

/*!
    invalid descramble key
  */
#define INVALID_DESC_KEY_INDEX      0xFF

/*!
    ABC
  */
#define SLOT_RAMIDX_ZERO            0x00
/*!
    ABC
  */
#define SLOT_RAMIDX_PIDOK           0x01
/*!
    ABC
  */
#define SLOT_RAMIDX_PIDNEW          0x02


/*!
    Section data
  */
#define SLOT_TYPE_SECTION           0x00
/*!
    Video ES
  */
#define SLOT_TYPE_VIDEO             0x01
/*!
    Audio ES
  */
#define SLOT_TYPE_AUDIO             0x02
/*!
    PCR
  */
#define SLOT_TYPE_PCR               0x03
/*!
    TS Packets
  */
#define SLOT_TYPE_TS_PKT            0x04

/*!
    PTI Memory pool singgle block size
  */
#define DMX_SINGLE_MEMPOOL_BLOCK 5
/*!
    PTI Memory pool multi block size
  */
#define DMX_MULTI_MEMPOOL_BLOCK 50

/*!
  This structure defines the type of the TS data to be parsed.
  */
typedef enum
{
    /*! 
    section mode
    */
    CH_TYPE_PSI_SI,
    /*! 
    record mode
    */
    CH_TYPE_REC,    
    /*! 
    video mode
    */
    CH_TYPE_VID,
    /*! 
    audio mode
    */
    CH_TYPE_AUD,
    /*! 
    pcr mode
    */
    CH_TYPE_PCR,
    /*! 
    other mode
    */
    CH_TYPE_UNKNOW
} chan_type_t;

/*!
  This structure defines the status of the slot.
  */
typedef enum
{
    /*! 
    open
    */
  SLOT_STATUS_OPEN,
    /*! 
    start
    */
  SLOT_STATUS_START,
    /*! 
    stop
    */
  SLOT_STATUS_STOP,
    /*! 
    close
    */
  SLOT_STATUS_CLOSE
}slot_status_type_t;

/*!
  This structure defines the status of the filter.
  */
typedef enum
{
    /*! 
    open
    */
  FILTER_STATUS_OPEN,
    /*! 
    start
    */
  FILTER_STATUS_START,
    /*! 
    stop
    */
  FILTER_STATUS_STOP,
    /*! 
    close
    */
  FILTER_STATUS_CLOSE
}filter_status_type_t;

#pragma pack(4)
/*!
  This structure defines the slot's structure.
  */
typedef struct
{
    /*!
      ABC
      */
  BOOL  used;
    /*!
      ABC
      */
  u8         index;
    /*!
      ABC
      */
  u8        desc_index;
    /*!
      ABC
      */
  u8        filter_index;
    /*!
      ABC
      */
  u8        filter_num;
    /*!
      ABC
      */
  u16       pid;
    /*!
      ABC
      */
  dmx_ch_type_t type;
    /*!
      TS Rec mode 
      */
  dmx_rec_mode_t mode;
    /*!
      Rec type
      */
  chan_type_t     chan_type;
    /*!
      stream input type
      */
  dmx_input_type_t chan_input;  
    /*! 
      record select
      */
  dmx_rec_path_t    rec_in;
    /*!
      slot status
      */
  slot_status_type_t status;
    /*!
      channel id
      */
  dmx_chanid_t  chanid;
}dmx_slot_t;

/*!
  ABC
  */
typedef struct dmx_user_data
{
    /*!
    ABC
    */
  BOOL used;
    /*!
      ABC
      */
  BOOL crc_flag;
    /*!
      ABC
      */
  BOOL gap_flag;
  /*!
    ABC
    */
  u8      *p_address;
    /*!
      ABC
      */    
  u32    size;
    /*!
      ABC
      */
  u16  pool_id;
    /*!
      ABC
      */
  u16  data_index;
    /*!
      ABC
      */
  u16 pid;
    /*!
      ABC
      */
  struct list_head data_list; 
    /*!
      ABC
      */
  struct dmx_user_data *p_next;
}dmx_user_data_t;

/*!
  dmx memory pool structure
  */
typedef struct dmx_mem_pool
{
     /*!
     pool mode
      */
    u32 pool_mode;   
    /*!
      left block size
      */
    u32 block_left_size;
    /*!
      total block size
      */
    u32 total_block_size;
    /*!
      user data struct
      */
    dmx_user_data_t *p_user_data;
    /*!
     memory pool iditify
      */
    u16 pool_id;
    /*!
     memory pool iditify
      */
    u16 pid;
    /*!
     next pointer
      */
    struct dmx_mem_pool *p_next;
}dmx_mem_pool_t;

/*!
  dmx memory pool manager structure
  */
typedef struct dmx_mem_pool_manager
{
    /*!
     pool mode
      */
    u32 pool_mode;
    /*!
     multi pool size
      */
    u32 multi_pool_size;
    /*!
     memory pool full function
      */
    BOOL (*dmx_mem_pool_is_full)(dmx_mem_pool_t *p_head);
    /*!
     memory pool create function
      */
    void (*dmx_mem_pool_create)(dmx_mem_pool_t *p_head);
    /*!
     memory pool destory function
      */
    void (*dmx_mem_pool_destory)(dmx_mem_pool_t *p_head,dmx_mem_pool_t *p_data);
    /*!
     memory pool unit alloc function
      */
    dmx_user_data_t *(*dmx_mem_pool_alloc)(dmx_mem_pool_t *p_head);
    /*!
     memory pool unit free function
      */
    void (*dmx_mem_pool_free)(dmx_mem_pool_t *p_head,dmx_user_data_t *p_data);
}dmx_mem_pool_manager_t;

/*!
  ABC
  */
typedef struct
{
    /*!
      ABC
      */
  dmx_mem_pool_t *p_user_mem;  
    /*!
      ABC
      */
  BOOL    used;
    /*!
      ABC
      */
  BOOL    en_crc;
    /*!
      ABC
      */
  BOOL    continuous;
    
    /*!
      ABC
      */
  u8         index;
    /*!
      ABC
      */
  u8         slot_index;
    /*!
      ABC
      */
  u8         desc_index;
    /*!
      ABC
      */
  u8         code[DMX_SECTION_FILTER_SIZE];
    /*!
      ABC
      */
     u8         mask[DMX_SECTION_FILTER_SIZE];

    /*!
      ABC
      */
  u8         *p_address;
    /*!
      ABC
      */
  u32       total_size;
    /*!
      ABC
      */
  u32       read_pointer;
    /*!
      ABC
      */
  u32       writer_pointer;
    /*!
      receive the ts or section data count
      */
  u32    sec_or_ts_num;
    /*!
      ABC
      */
  u16       pid;
    /*! 
    TS mode filter
    */
  dmx_ts_packet_number_t ts_packet_mode;
    /*!
      ABC
      */
  chan_type_t     chan_type;
    /*!
      ABC
      */
  filter_status_type_t  status;
    /*!
      ABC
      */
  dmx_user_data_t *p_user_data;
    /*!
      ABC
      */
  BOOL  b_sync;  
    /*!
      ABC
      */
  u32  alloc_count;  
    /*!
      ABC
      */
  u32  free_count;  
    /*!
      ABC
      */
  u32 debug_addr;
    /*!
      ABC
      */
  u32 debug_section_count;
    /*!
      ABC
      */
  struct list_head user_list; 
    /*!
        Back point to struct dvb_demux_feed struct
    */
    void *feed;
    /*!
      ABC
      */
  void (*dmx_buffer_mode)(u8 index,
                                            BOOL circle_flag,
                                            u32 data_len,
                                            BOOL crc_err);
}dmx_filter_hw_t;

/*!
  ABC
  */
typedef struct
{
    /*!
      ABC
      */
  BOOL used;
    /*!
      ABC
      */
  u8      index;
    /*!
      ABC
      */
  u8      slot_index;
}dmx_desc_t;

/*!
  record module structure
  */
typedef struct
{
    /*!
      ABC
      */
  BOOL  used;
    /*!
      writer pointer
      */
  u32 writer_pointer;
    /*!
      read pointer
      */
  u32 read_pointer;
    /*!
      buffer size
      */
  u32 buf_size;
    /*!
      buffer addr
      */
  u32 buf_addr;
    /*!
      record data length
      */
  u32 total_data_rec;
    /*!
      record ts stream bitrate
      */
  u32 data_bitrate;    
    /*!
    record data length
    */
  u32 total_rec_size;

}dmx_rec_t;

/*!
  ABC
  */
typedef struct
{
    /*!
      ABC
      */
    BOOL                  av_sync_flag;
    /*!
      ABC
      */
    u8                      slot_num;
    /*!
      ABC
      */
  u8                      filter_num;
    /*!
      ABC
      */
  u8                      desc_num;
    /*!
      ABC
      */
  u8                      rec_num;
    /*!
      ABC
      */
  dmx_slot_t      *p_dmx_slot;
    /*!
      ABC
      */
  dmx_filter_hw_t    *p_dmx_filter;  
    /*!
      ABC
      */
  dmx_desc_t     *p_dmx_desc;
    /*!
      record poniter structure
      */
  dmx_rec_t     *p_dmx_rec;
    /*!
      configure parameter
      */
  dmx_config_t   *p_dmx_cfg;  
    /*!
     dmx memory pool managerment pointer
      */
  dmx_mem_pool_manager_t *p_dmx_mem_pool;
#ifndef WIN32
    /*!
     dmx drviver service handle pointer
      */
  drvsvc_handle_t *p_drvsvc;
#endif
}dmx_chan_t;

/*!
  DMX
  */
typedef struct s_dmx
{
//priv:
    /*!
      priv
      */
    void *p_priv;

//public:

    /*!
      Open a demux channel,the same slot&diff filter,is the diff channel.
      */
    RET_CODE (*p_si_chan_open)(void *p_dmx
        , dmx_slot_setting_t *p_slot, dmx_chanid_t *p_channel);

    /*!
        Set hardware buffer for saving data.the buffer is controlled by hardware
      */
    RET_CODE (*p_si_chan_set_buffer)(void *p_dmx
        , dmx_chanid_t channel, u8 *p_buf, u32 size);

    /*!
      Set a filter for demux channel
      */
    RET_CODE (*p_si_chan_set_filter)(void *p_dmx
        ,dmx_chanid_t channel, dmx_filter_setting_t *p_filter);


    /*!
      Get data from demux channel.
      */
    RET_CODE (*p_si_chan_get_data)(void *p_dmx
        , dmx_chanid_t channel, u8 **pp_data, u32 *p_size);

    /*!
      Allocate a demux channel for play programe.
      */
    RET_CODE (*p_play_chan_open)(void *p_dmx
        , dmx_play_setting_t *p_play_t, dmx_chanid_t *p_channel);

    /*!
      Allocate a demux channel for record TS.
      */
    RET_CODE (*p_rec_chan_open)(void *p_dmx
        , dmx_rec_setting_t *p_rec_t,dmx_chanid_t *p_channel);

    /*!
        Set DDR buffer for user transfor data.  
      */
    RET_CODE (*p_rec_chan_set_buffer)(void *p_dmx
        ,u8 index, u8 *p_buf, u32 size);

    /*!
      Get data from record memory.
      */
    RET_CODE (*p_rec_get_data)(void *p_dmx, u8 index, u32 *p_size);

    /*!
      free a demux channel.
      */
    RET_CODE (*p_chan_close)(void *p_dmx, dmx_chanid_t channel);

    /*!
      start a demux channel.
      */
    RET_CODE (*p_chan_start)(void *p_dmx, dmx_chanid_t channel);

    /*!
      stop a demux channel.
      */
    RET_CODE (*p_chan_stop)(void *p_dmx, dmx_chanid_t channel);
    /*!
      get play channel id.
      */
     RET_CODE (*p_get_play_channel)(void *p_dmx,dmx_chanid_t *video_id, dmx_chanid_t *audio_id);

    /*!
      control the descrambler module.
      */
    RET_CODE (*p_descrambler_onoff)(void *p_dmx
        , dmx_chanid_t channel, BOOL enable);
    
    /*!
      set the odd keys.
      */
    RET_CODE (*p_descrambler_set_odd_keys)(void *p_dmx
        , dmx_chanid_t channel, u8 *p_key, s32 key_length);

    /*!
      set the even keys.
      */
    RET_CODE (*p_descrambler_set_even_keys)(void *p_dmx
        , dmx_chanid_t channel, u8 *p_key, s32 key_length);

    /*!
      set the dma parameter.
      */
    RET_CODE (*p_dma_config)(void *p_dmx,dmx_dma_config_t *p_dma_config);

    /*!
      Get the dma state.
      */
    BOOL (*p_dma_state)(void *p_dmx);

    /*!
      Get the channel id by pid
      */
    RET_CODE (*p_get_chanid_bypid)(void *p_dev, u16 pid, dmx_chanid_t *p_channel);  

    /*!
      av reset
      */
    RET_CODE (*p_av_reset)(void *p_dev);  

    /*!
      Get data from demux channel in safe mode.
      */
    RET_CODE (*p_si_chan_get_data_safe)(void *p_dmx
        , dmx_chanid_t channel,u32 maxBufLen, u8 **pp_data, u32 *p_size);
}  dmx_t;
#pragma pack()
#endif

