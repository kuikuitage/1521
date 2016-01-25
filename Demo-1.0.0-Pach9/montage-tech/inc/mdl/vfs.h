/******************************************************************************/
/******************************************************************************/
#ifndef __VFS_H__
#define __VFS_H__
/*!
  max path langth
  */
#define MAX_PATH_LEN  256

/*!
  max file name length
  */
#define MAX_FILE_NAME 16

/*!
  hfile handle
  */
typedef void * hfile_t;
/*!
  hdir handle
  */
typedef void * hdir_t;

/*!
  structure for file information.
  */
typedef struct
{
  /*!
    file last modified date.
    */
  utc_time_t file_date;
  /*!
    file attribe.
    */
  u32 file_attr;
  /*!
    file size
    */
  u32 file_size;
  /*!
    file name
    */
  u8 file_name[MAX_FILE_NAME];
}file_info_t;   


/*!
  open exist.
  */
#define VFS_OPEN    0x0
/*!
  open for read.
  */
#define VFS_READ    0x01
/*!
  open for write.
  */
#define VFS_WRITE   0x02
/*!
  open new file.
  */
#define VFS_NEW     0x04
/*!
  shared open file
  */
#define VFS_SHARE   0x08

/*!
  enum for file system type
  */
typedef enum
{
  /*!
    fat 16
    */
  VFS_FAT16,
  /*!
    fat 32
    */
  VFS_FAT32,    
}vfs_fs_t;
/*!
  vfs seek mode
  */
typedef enum
{
  /*!
    seek from head.
    */
  VFS_SEEK_HEAD,
  /*!
    seek from tail.
    */
  VFS_SEEK_TAIL,
  /*!
    seek from curn.
    */
  VFS_SEEK_CURN,
}vfs_seek_mode_t;
/*!
  short file name length
  */
#define FILE_NAME_LEN 256

/*!
  vfs file attr
  */
typedef enum
{
  /*!
    Read only
    */
  VFS_RDO = 0x01,
  /*!
    hide
    */
  VFS_HID = 0x02,
  /*!
    system
    */
  VFS_SYS = 0x04,
  /*!
    volume label
    */
  VFS_VOL = 0x08,
  /*!
    lfn entry
    */
  VFS_LFN = 0x0F,
  /*!
    directory
    */
  VFS_DIR = 0x10,
  /*!
    archive
    */
  VFS_ARC = 0x20,
}vfs_fattr_t;

/*!
  vfs file handle
  */
typedef struct
{
  /*!
    file size
    */
  u32 fsize;
  /*!
    last modified date.
    */
  u16 fdate;
  /*!
    last modified time.
    */
  u16 ftime;
  /*!
    attribute
    */
  u8 fattrib;
  /*!
    short file name.
    */
  u8 fname[FILE_NAME_LEN];
}vfs_dir_info_t;

/*!
  vfs file info
  */
typedef struct
{
  /*!
    ufs file info
    */
  u32 file_size;
}vfs_file_info_t;

/*!
  vfs command
  */
typedef enum
{
  /*!
    mount
    */
  VFS_CMD_MOUNT = 0,
  /*!
    format
    */
  VFS_CMD_FORMAT,
  /*!
    change driver
    */
  VFS_CMD_CHDRIVE,
  /*!
    make directory
    */
  VFS_CMD_MKDIR,
  /*!
    change directory
    */
  VFS_CMD_CHDIR,
  /*!
    show path
    */
  VFS_CMD_SHOW_PATH,
}vfs_cmd_t;

/*!
   vfs device event
  */
typedef enum  {
/*!
  plug in
  */
    VFS_PLUG_IN_EVENT = 1,
/*!
  plug out
  */
    VFS_PLUG_OUT_EVENT,
/*!
  device  ready
  */
    VFS_READY_EVENT,
/*!
   reserved  
  */
    VFS_RESERVED
}dev_evt_t;

/*!
   struct of vfs device
  */
typedef struct
{
/*!
   dev name  
  */
  u8 *p_name;
/*!
   evt id  
  */
  dev_evt_t evt;  
}vfs_dev_event_t;

/*!
   struct of vfs device info
  */
typedef struct
{
  /*!
     total size.
    */
  u32 total_size;
  /*!
     free size.
    */
  u32 free_size;
  /*!
    partition used size
    */
  u32 used_size;
  /*!
    partition name
    */
  u8 parttion_name[13];
  /*!
     fs type.
    */
  vfs_fs_t fs_type;
}vfs_dev_info_t;

/*!
  vfs define
  */
typedef struct
{
/*!
  vfs mount device
  */
  RET_CODE (*mount)(u8 physical_letter);
/*!
  vfs umount device
  */
  RET_CODE (*unmount)(u8 physical_letter);
/*!
  vfs open file.
  */
  hfile_t (*open)(u8 *p_path, u32 mode);
/*!
  vfs read file.
  */
  u32 (*read)(hfile_t file, void *p_buf, u32 size);
/*!
  vfs write file.
  */
  u32 (*write)(hfile_t file, void *p_buf, u32 size);
/*!
  vfs close file.
  */
  void (*close)(hfile_t file);
/*!
  vfs delete file.
  */
  RET_CODE (*del)(u8 *p_path);
/*!
  vfs seek file.
  */
  RET_CODE (*seek)(hfile_t file, u32 offset, vfs_seek_mode_t mode);
/*!
  vfs seek file.
  */
  RET_CODE (*get_file_info)(hfile_t file, vfs_file_info_t *p_info);
/*!
  vfs open dir.
  */
  hdir_t (*opendir)(u8 *p_path);
/*!
  vfs close dir.
  */
  void (*closedir)(hdir_t dir);
/*!
  vfs read dir.
  */
  RET_CODE (*readdir)(hdir_t dir, vfs_dir_info_t *p_info);
/*!
  vfs format.
  */
  RET_CODE (*format)(u8 partition, vfs_fs_t fs_type);
/*!
  vfs get device info.
  */
  RET_CODE (*get_dev_info)(u8 partition, vfs_dev_info_t *p_dev_info);
/*!
  vfs get event.
  */
  BOOL (*process)(vfs_dev_event_t *p_event);
/*!
  vfs ioctl.
  */
  RET_CODE (*ioctl)(hfile_t file, vfs_cmd_t vfs_cmd, u32 para0, u32 para1, u32 para2);
/*!
  vfs get partitions.
  */
  u8 (*get_partitions)(u8 *p_letters, u8 length);
/*!
  rename file
  */
RET_CODE (*rename) (u8 *p_old, u8 *p_new);
/*!
  priv data of vfs
  */
void *p_priv;
}vfs_t;

/*!
  init virtual file system
  */
void vfs_system_init(void);

s64 vfs_tell(hfile_t file);

void vfs_close(hfile_t file);

u32 vfs_read(void * p_buf, u32 count, u32 size, hfile_t file);

void vfs_seek(hfile_t file, s64 offset, vfs_seek_mode_t mode);

hfile_t vfs_open(const char *p_path, const char *mode);

#endif
