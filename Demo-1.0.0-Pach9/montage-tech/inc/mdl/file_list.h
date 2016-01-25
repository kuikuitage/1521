/******************************************************************************/
/******************************************************************************/
#ifndef __FILE_LIST_H_
#define __FILE_LIST_H_

/*!
  Max file cnt
  */
#define MAX_FILE_CNT (500)


/*!
  media fav item 
  */
typedef struct tag_media_fav
{ 
  /*!
    cur media fav path
    */
  u8 path[MAX_FILE_PATH];
  /*!
    next media fav
    */
  struct tag_media_fav *p_next;
}media_fav_t;

/*!
  medai fav type
  */
typedef enum tag_media_fav_type
{
  /*!
    media audio fav
    */
  AUDIO_FAV,
  /*!
    media pic fav
    */
  PIC_FAV
}media_fav_type_t;

/*!
  partion
  */
typedef struct tag_partition
{
  /*!
    partion letter like (C:)
    */
  u8 letter[3];
  /*!
    partion name
    */
  u8 name[13];
  /*!
    partion total size
    */
  u32 toatl_size;
  /*!
    partion free size
    */
  u32 free_size;
}partition_t;

/*!
  media file type
  */
typedef enum 
{
  /*!
    directory
    */
  DIRECTORY,
  /*!
    not dir file
    */
  NOT_DIR_FILE,
  /*!
    unknow file
    */
  UNKNOW_FILE
}file_type_t;

/*!
  file list option
  */
typedef enum
{
  /*!
    file list unit first
    */
  FLIST_UNIT_FIRST,
  /*!
    file list unit next
    */
  FLIST_UNIT_NEXT,
  /*!
    file list unit previous
    */
  FLIST_UNIT_PREV,
  /*!
    file list unit last
    */
  FLIST_UNIT_LAST
}flist_option_t;

/*!
  media file
  */
typedef struct tag_media_file
{
  /*!
    file type
    */
  file_type_t type;
  /*!
    size
    */
  u32 size;
  /*!
    file name with path
    */
  u8 name[MAX_FILE_PATH];
  /*!
    file name no path
    */
  u8 *p_name;
}media_file_t;

/*!
  file list
  */
typedef struct tag_file_list
{
  /*!
    count of media file
    */
  u32 file_count;
  /*!
    media file
    */
  media_file_t *p_file;
}file_list_t;

/*!
  file list init
  */
void file_list_init(void);

/*!
  file list deinit
  */
void file_list_deinit(void);

/*!
  get available partition and count
  \param[in/out] pp_partion pointer to partition list
  */
u32 file_list_get_partition(partition_t **pp_partition);

/*!
  file list enter dir
  \param[in] p_filter media file type need
  \param[in] unit_cnt unit file list count
  \param[in] p_path directoty path
  */
BOOL file_list_enter_dir(u8 *p_filter, u16 unit_cnt, u8 *p_path);

/*!
  get file list by option
  \param[in] option get option
  \param[in/out] p_list file list for result
  */
BOOL file_list_get(flist_option_t option, file_list_t *p_list);

/*!
  file list leave dir
  */
void file_list_leave_dir(void);

/*!
  get current path
  */
u8 *file_list_get_cur_path(void);

/*!
  load fav list, return fav cnt
  \param[in] partition_letter partition letter
  \param[in] pp_media_fav loaded fav list
  \param[in] type media fav type
  */
u32 load_fav_list(u8 partition_letter, media_fav_t **pp_media_fav, media_fav_type_t type);

/*!
  unload fav list
  \param[in] pp_media_fav loaded fav list
  */
void unload_fav_list(media_fav_t *p_media_fav);

/*!
  save fav list
  \param[in] partition_letter partition letter
  \param[in] pp_media_fav loaded fav list
  \param[in] type media fav type
  */
void save_fav_list(u8 partition_letter, media_fav_t *p_media_fav, media_fav_type_t type);

/*!
  add one fav, return hanlde of added
  \param[in] p_name need add item
  \param[in] p_media_fav list to add
  */
media_fav_t *add_one_fav(u8 *p_name, media_fav_t **pp_media_fav);

/*!
  del one fav
  \param[in] p_fav_list fav list
  \param[in] p_fav_del fav need delete
  */
u8 del_one_fav(media_fav_t **pp_fav_list, media_fav_t *p_fav_del);

/*!
  get file size
  \param[in] p_name file path
  */
u32 file_list_get_file_size(u8 *p_name);

/*!
  rename file
  \param[in] p_old old name
  \param[in] p_new new name
  */
BOOL file_list_rename(u8 *p_old, u8 *p_new);

/*!
  delete file
  \param[in] p_name file to be delete
  */
BOOL file_list_delete_file(u8 *p_name);

#endif // End of __FILE_LIST_H_

