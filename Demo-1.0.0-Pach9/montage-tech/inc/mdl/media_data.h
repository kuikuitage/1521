/******************************************************************************/
/******************************************************************************/
#ifndef __MEDIA_DATA_H_
#define __MEDIA_DATA_H_

/*!
  ts packet length
  */
#define TS_PACKET_LEN (188)

/*!
  ts packet id
  */
#define TS_PID(data) \
  ((u16)(((u8)(data[2])) | \
         (((u16)((u8)(data[1] & 0x1F))) << 8)))

/*!
  payload unit  start
  */
#define PAYLOAD_UNIT_START(data) ((((u8)data[0]) == 0x47) && ((((u8)data[1]) & 0x40) == 0x40))

/*!
  video pre start code
  */
#define PRE_START_CODE(x) \
  ((((u8)(x)[0] == 0x00)) && (((u8)(x)[1]) == 0x00) && (((u8)(x)[2]) == 0x01))

/*!
  is pic start code
  */
#define IS_PICTURE_START_CODE(x) \
  (PRE_START_CODE(x) && ((u8)(x)[3] == 0x00))

/*!
  record recorder
  */
typedef struct tag_rec_recorder
{
  /*!
    program name
    */
  u8 program[32];
  /*!
    start time
    */
  utc_time_t start;
  /*!
    duration time(seconds)
    */
  u32 drt_time;
  /*!
    service id
    */
  u16 svc_id;
  /*!
    video pid
    */
  u16 v_pid;
  /*!
    audio pid
    */
  u16 a_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
}rec_recorder_t;

/*!
  video key frame info
  */
typedef struct tag_vkey_frame_info
{
  /*!
    key frame offset
    */
  u32 offset[10];
  /*!
    offset cnt
    */
  u8 cnt;
}vkey_frame_info_t;

/*!
  dynamic rec info
  */
typedef struct tag_dynamic_rec_info
{
  /*!
    transformed_size
    */
  u32 transformed_size;
  /*!
    last transformed_size
    */
  u32 last_transformed_size;
  /*!
    cur payload
    */
  u32 payload;
  /*!
    recorded time 
    */
  u32 recorded_time;
  /*!
    video key frame info
    */
  vkey_frame_info_t *p_vkey_info;
}dy_rec_info_t;

/*!
  program record info
  */
typedef struct tag_program_rec_info
{
  /*!
    record program name
    */
  u8 program_name[24];
  /*!
    record date
    */
  u8 date[12];
  /*!
    record start to end time
    */
  u8 start_end[12];
  /*!
    video pid
    */
  u16 v_pid;
  /*!
    audio pid
    */
  u16 a_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    record duration time
    */
  u32 time_long;
}program_rec_info_t;

/*!
  record info
  */
typedef struct tag_player_rec_info
{
  /*!
    cur payload
    */
  u32 cur_payload;
  /*!
    cur rec info packet pos
    */
  u32 cur_pos;
  /*!
    played time
    */
  u32 played_time;
  /*!
    prev key frame pos
    */
  u32 pre_pos;
  /*!
    vkey frame offset
    */
  u32 offset[10];
  /*!
    vkey frame offset cnt
    */
  u8 cnt;
}player_rec_info_t;

/*!
  pack program rec info
  \param[in/out] p_static_rec_info buffer for add program rec info
  \param[in] p_recorder recorder
  */
void pack_program_rec_info(u8 *p_static_rec_info, rec_recorder_t *p_recorder);

/*!
  pack dynamic rec info
  \param[in/out] p_rec_info buffer for add rec info
  \param[in] p_static_rec_info static rec info
  \param[in] p_dy_rec_info dynamic rec info
  */
void pack_dynamic_rec_info(u8 *p_rec_info,
  u8 *p_static_rec_info, dy_rec_info_t *p_dy_rec_info);

/*!
  packet rec info
  \param[in/out] p_data packet for add rec info
  \param[in] p_info rec info
  \param[in] info_len rec info len
  */
void packet_rec_info(u8 *p_data, u8 *p_info, u8 info_len);

/*!
  packet is rec info or not
  \param[in] p_packet ts packet
  */
BOOL is_rec_info_packet(u8 *p_packet);

/*!
  unpacket rec info
  \param[in] p_packet rec info packet
  \param[in/out] p_rec_info player rec info for ts player
  */
void unpacket_dynamic_rec_info(u8 *p_packet, player_rec_info_t *p_rec_info);

/*!
  parse pcr
  \param[in] p_data ts packet
  \param[in/out] p_pcr pcr pointer
  */
BOOL parse_pcr(u8 *p_data, unsigned long long *p_pcr);

/*!
  parse pes data
  \param[in] p_ts_packet ts packet
  \param[in/out] p_pes_data pes data pointer
  \param[in/out] p_payload pes data payload
  */
void parse_pes_data(u8 *p_ts_packet, u8 **pp_pes_data, u32 *p_payload);

/*!
  parse stream
  \param[in] p_file_name file name to be parsed
  \param[in/out] p_a_pid audio pid parsed
  \param[in/out] p_v_pid video pid parsed
  \param[in/out] p_pcr_pid video pid parsed
  */
u32 parse_stream(u8 *p_file_name, u16 *p_a_pid, u16 *p_v_pid, u16 *p_pcr_pid);

/*!
  parse record info
  \param[in/out] p_rec_info record info parsed
  \param[in] p_name file name to be parsed
  */
BOOL parse_record_info(program_rec_info_t *p_rec_info, u8 *p_name);

/*!
  parse mp3 time long (seconds)
  \param[in] p_name file name to be parsed
  \param[in] file_size file size
  */
u32 parse_mp3_time_long(u8 *p_name);

/*!
  set record file lock flag
  \param[in] p_name rec file name
  \param[in] lock_flag lock flag
  */
void set_rec_file_lock_flag(u8 *p_name, BOOL lock_flag);

/*!
  set record file lock flag
  \param[in] p_name rec file name
  */
BOOL get_rec_file_lock_flag(u8 *p_name);

#endif // End of __MEDIA_DATA_H_

