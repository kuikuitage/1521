/******************************************************************************/
/******************************************************************************/
#include "string.h"

// system
#include "sys_types.h"
#include "sys_define.h"

#include "drv_dev.h"

#include "lib_util.h"
#include "class_factory.h"

//#include "../../kware/ufs/fat/ff.h"

//#include "fsioctl.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"

//#include "ufs.h"
#include "vfs.h"
#ifdef WIN32
#define MAX_DEVICE_CNT 24

/*!
  struct for vfs node
  */
typedef struct vfs_node
{
  /*!
    physical letter.
    */
  u8 physical_letter;
  /*!
    is linked.
    */
  BOOL is_linked;    
}vfs_node_t;

/*!
  vfs file handle
  */
typedef struct
{
  /*!
    ufs file handle
    */
  ufs_file_t ufs_file;
}vfs_file_t;

/*!
  vfs file handle
  */
typedef struct
{
  /*!
    ufs dir
    */
  ufs_dir_t ufs_dir;
}vfs_dir_t;

/*!
   vfs info
  */
typedef struct
{
  /*!
     vfs head.
    */
  vfs_node_t vfs_node[MAX_DEVICE_CNT];
  /*!
     vfs dev event.
    */
  vfs_dev_event_t dev_event;
  /*!
    ufs dev handle
  */
  void *p_usf;
}vfs_info_t;


/*!
  find vfs node by physical letter.

  \param[in] physical_letter  physical_letter.

  \return vfs node or NULL
  */
static vfs_node_t * _find_node_by_physical_letter(u8 physical_letter)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  vfs_info_t *p_vfs_info = NULL; 
  u8 i = 0;

  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  MT_ASSERT(p_vfs_info != NULL);

  for(i = 0; i < MAX_DEVICE_CNT; i++)
  {
    if((p_vfs_info->vfs_node[i].is_linked == TRUE)
      && (p_vfs_info->vfs_node[i].physical_letter == physical_letter))
    {
        return &(p_vfs_info->vfs_node[i]);
    }
  }


  return NULL;
}

/*!
  vfs change logical path to physical path.

  \param[in] p_logical_path  logical path.
  \param[in] p_physical_path physical path.

  \return TRUE or FALSE.
  */
static BOOL _logical_path_to_physical(u8 *p_logical_path, u8 *p_physical_path)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  vfs_info_t *p_vfs_info = NULL; 
  vfs_node_t *p_vfs_node = NULL;

  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  MT_ASSERT(p_vfs_info != NULL);
  MT_ASSERT(p_logical_path != NULL);
  MT_ASSERT(p_physical_path != NULL);
  
  memcpy(p_physical_path, p_logical_path, MAX_PATH_LEN * sizeof(u8));

  //low case logical letter.
  if((p_physical_path[0] < 'Z') && (p_physical_path[0] > 'A'))
  {
    p_physical_path[0] -= ('A' - 'a');
  }

  //to avoid illegal leter, 'c' ~ 'z' 
  if((p_physical_path[0] > 'z') || (p_physical_path[0] < 'c'))
  {
    return FALSE;
  }

  //if((p_physical_path[1] != ':') || (p_physical_path[2] != '\\'))
  if(p_physical_path[1] != ':')
  {
    return FALSE;
  }

  p_vfs_node = &(p_vfs_info->vfs_node[p_physical_path[0] - 'c']);

  if(p_vfs_node->is_linked == TRUE)
  {
    p_physical_path[0] = p_vfs_node->physical_letter;

    return TRUE;
  }

  return FALSE;
}


static RET_CODE _mount(u8 physical_letter)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  vfs_info_t *p_vfs_info = NULL; 
  vfs_node_t *p_vfs_node = NULL;
  u8 i = 0;
  
  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  MT_ASSERT(p_vfs_info != NULL);

  //to check if there are duplicate node.
  p_vfs_node = _find_node_by_physical_letter(physical_letter);

  if(p_vfs_node != NULL)
  {
    return ERR_FAILURE;
  }

  //to get a unused node.
  for(i = 0; i < MAX_DEVICE_CNT; i++)
  {
    if((p_vfs_info->vfs_node[i].is_linked == FALSE)
      && (p_vfs_info->vfs_node[i].physical_letter == 0))
    {
      break;
    }
  }

  if(i == MAX_DEVICE_CNT)
  {
    return ERR_FAILURE;
  }

  //mount device
  p_vfs_info->vfs_node[i].is_linked = TRUE;
  p_vfs_info->vfs_node[i].physical_letter = physical_letter;

  return SUCCESS;
}

static RET_CODE _unmount(u8 physical_letter)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  vfs_info_t *p_vfs_info = NULL; 
  vfs_node_t *p_vfs_node = NULL;
  
  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  MT_ASSERT(p_vfs_info != NULL);

  //to check if the node exist.
  p_vfs_node = _find_node_by_physical_letter(physical_letter);

  if(p_vfs_node == NULL)
  {
    return ERR_FAILURE;
  }

  //unmount the node.
  p_vfs_node->is_linked = FALSE;
  p_vfs_node->physical_letter = 0;
  
  return SUCCESS;
}

static u8 _get_partitions(u8 *p_letters, u8 length)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);  
  vfs_info_t *p_vfs_info = NULL; 
  u8 i = 0, cnt = 0;

  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;  
  
  MT_ASSERT(p_vfs_info != NULL);

  for(i = 0; i < MAX_DEVICE_CNT; i++)
  {
    if(p_vfs_info->vfs_node[i].is_linked == TRUE)
    {
      *(p_letters + cnt) = p_vfs_info->vfs_node[i].physical_letter;

      cnt++;

      if(cnt >= length)
      {
        break;
      }
    }
  }
  OS_PRINTF("\n##debug: _get_partitions [%d]\n", cnt);
  return cnt;
}

static RET_CODE _get_dev_info(u8 physical_letter, vfs_dev_info_t *p_dev_info)
{
  //todo....

  p_dev_info->total_size = 0x20000000;
  p_dev_info->free_size = 0x1023045;
  memset(p_dev_info->parttion_name, 0, sizeof(p_dev_info->parttion_name));
  strcpy((char *)p_dev_info->parttion_name, "Device");
  p_dev_info->fs_type = VFS_FAT32;
  
  return SUCCESS;
}

static RET_CODE _format(u8 partition, vfs_fs_t fs_type)
{
  vfs_t *p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  vfs_node_t *p_vfs_node = NULL;
  vfs_info_t *p_vfs_info = NULL; 
  u8 logic_partition = 0;
  u8 ufs_type = 0;

  MT_ASSERT(p_vfs != NULL);
  
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  
  if((partition < 'Z') && (partition > 'A'))
  {
    partition -= ('A' - 'a');
  }

  if((partition > 'z') || (partition < 'c'))
  {
    return ERR_FAILURE;
  }

  p_vfs_node = &(p_vfs_info->vfs_node[partition - 'c']);

  if(!p_vfs_node->is_linked)
  {
    return ERR_FAILURE;
  }

  logic_partition = p_vfs_node->physical_letter;
  
  switch(fs_type)
  {
    case VFS_FAT16:
      ufs_type = 0x06;
      break;
    
    case VFS_FAT32:
      ufs_type = 0x0B;
      break;

    default:
      MT_ASSERT(0);
      break;
  }
  ufs_format(logic_partition, ufs_type, 0, 512);

  return SUCCESS;
}

static hfile_t _open(u8 *p_path, u32 mode)
{
  u8 physical_path[MAX_PATH_LEN];
  vfs_file_t *p_vfs_file = NULL;
  u32 ufs_mode = UFS_OPEN;

  MT_ASSERT(p_path != NULL);

  OS_PRINTF("enter vfs open[%s]\n", p_path);

  p_vfs_file = (vfs_file_t *)mtos_malloc(sizeof(vfs_file_t));
  memset(p_vfs_file, 0, sizeof(vfs_file_t));

  if(_logical_path_to_physical(p_path, physical_path))
  {
    if(mode & VFS_READ)
    {
      ufs_mode |= UFS_READ;
    }

    if(mode & VFS_WRITE)
    {
      ufs_mode |= UFS_WRITE;
    }

    if(mode & VFS_NEW)
    {
      ufs_mode |= (UFS_CREATE_NEW_COVER | UFS_WRITE);
    }

    if(mode & VFS_SHARE)
    {
      ufs_mode |= (UFS_READ | UFS_WRITE);
    }

    OS_PRINTF(">>>>>>>>>>>\n");
    if (ufs_open(&p_vfs_file->ufs_file, p_path, ufs_mode))
    {
      mtos_free(p_vfs_file);
      p_vfs_file = NULL;
    }
    OS_PRINTF("#############\n");
  }

  return (hfile_t)p_vfs_file;
}

static u32 _read(hfile_t file, void *p_buf, u32 size)
{
  u32 actual_read = 0;
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;

  MT_ASSERT(p_vfs_file != NULL);
  MT_ASSERT(p_buf != NULL);
  
  ufs_read(&p_vfs_file->ufs_file, p_buf, size, &actual_read);

  return actual_read;
}

static u32 _write(hfile_t file, void *p_buf, u32 size)
{
  u32 actual_write = 0;
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;

  MT_ASSERT(p_vfs_file != NULL);
  MT_ASSERT(p_buf != NULL);
  
  ufs_write(&p_vfs_file->ufs_file, p_buf, size, &actual_write);

  mtos_printk("size %d actual %d\n",size,actual_write);
  return actual_write;
}

static void _close(hfile_t file)
{
  u8 ret = 0;
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;

  MT_ASSERT(p_vfs_file != NULL);
  
  ret = ufs_close(&p_vfs_file->ufs_file);
  OS_PRINTF("vfs close %d\n",ret);
  mtos_free(file);
}

static RET_CODE _delete(u8 *p_path)
{
  u8 physical_path[MAX_PATH_LEN];
  
  MT_ASSERT(p_path != NULL);

  if(_logical_path_to_physical(p_path, physical_path))
  {
    if (!ufs_delete(physical_path))
    {
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}

static RET_CODE _rename(u8 *p_old, u8 *p_new)
{
  if (!ufs_rename (p_old, p_new))
  {
    return SUCCESS;
  }

  return ERR_FAILURE;
}

static RET_CODE _seek(hfile_t file, u32 offset, vfs_seek_mode_t mode)
{
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;
  u32 frm_0_offset = 0;
  
  MT_ASSERT(p_vfs_file != NULL);

  switch(mode)
  {
    case VFS_SEEK_HEAD:
      frm_0_offset = offset;
      break;

    case VFS_SEEK_TAIL:
      frm_0_offset = p_vfs_file->ufs_file.file_size - offset;
      break;
      
    default:
      MT_ASSERT(0);
      break;
  }

  ufs_lseek(&p_vfs_file->ufs_file, frm_0_offset, 0);

  return SUCCESS;
}

static hdir_t _opendir(u8 *p_path)
{
  u8 physical_path[MAX_PATH_LEN];
  vfs_dir_t *p_vfs_dir = NULL;
  
  MT_ASSERT(p_path != NULL);

  p_vfs_dir = (vfs_dir_t *)mtos_malloc(sizeof(vfs_dir_t));
  
  if(_logical_path_to_physical(p_path, physical_path))
  {
    if((strlen((const char *)physical_path) <= 3) && physical_path[1] == ':')
    {
      ufs_chdrive(physical_path[0]);
      ufs_opendir((ufs_dir_t *)&p_vfs_dir->ufs_dir, (u8 *)"");
    }
    else
    {
      ufs_opendir((ufs_dir_t *)&p_vfs_dir->ufs_dir, p_path);
    }
  }

  return (hdir_t)p_vfs_dir;
}

static void _closedir(hdir_t dir)
{
  mtos_free((void *)dir);
}

static RET_CODE _readdir(hdir_t dir, vfs_dir_info_t *p_info)
{
  RET_CODE ret = SUCCESS;
  vfs_dir_t *p_vfs_dir = (vfs_dir_t *)dir;
  ufs_fileinfo_t ufs_fileinfo;
  u8 long_name[256] = {0};
  
  MT_ASSERT(p_vfs_dir != NULL);
  MT_ASSERT(p_info != NULL);
  
  ufs_fileinfo.fileinfo.lfname = (char *)long_name;
  ret = ufs_readdir(&p_vfs_dir->ufs_dir, &ufs_fileinfo);

  MT_ASSERT(ret == SUCCESS);
  
  p_info->fsize = ufs_fileinfo.fileinfo.fsize;
  p_info->fdate = ufs_fileinfo.fileinfo.fdate;
  p_info->fattrib = ufs_fileinfo.fileinfo.fattrib;

  p_info->ftime = ufs_fileinfo.fileinfo.ftime;

  if(ufs_fileinfo.fileinfo.lfname[0] == 0)
  {
    strcpy((char *)p_info->fname, ufs_fileinfo.fileinfo.fname);
  }
  else
  {
    strcpy((char *)p_info->fname, ufs_fileinfo.fileinfo.lfname);
  }
  return ret;
}

static RET_CODE _get_file_info(hfile_t file, vfs_file_info_t *p_info)
{
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;
  
  MT_ASSERT(p_vfs_file != NULL);
  MT_ASSERT(p_info != NULL);

  p_info->file_size = p_vfs_file->ufs_file.fs.p_fat_file->fsize;

  return SUCCESS;
}

static BOOL _process(vfs_dev_event_t *p_event)
{
  vfs_t *p_vfs = class_get_handle_by_id(VFS_CLASS_ID);
  vfs_info_t *p_info = (vfs_info_t *)p_vfs->p_priv;
  ufs_devic_event_t event = {0};
  u8 part_table[10] = {0};
  u8 num = 0;
  u8 i = 0;
  //OS_PRINTF("---------------_process-----------------\n");
  if(ufs_get_event(&event))
  {
  #ifndef WIN32
    p_info->p_usf = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)event.p_dev_name);;
    OS_PRINTF("===get event=====name=%s==event=%d===\n", event.p_dev_name, event.event);
  #else
    p_info->p_usf = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_WIN32_FAKE_DEV);
  #endif
    MT_ASSERT(p_info->p_usf != NULL);
    
    p_info->dev_event.evt = event.event;
    p_info->dev_event.p_name = event.p_dev_name;
    if(p_info->dev_event.evt != VFS_RESERVED)
    {
      if(p_info->dev_event.evt == VFS_PLUG_IN_EVENT 
        || p_info->dev_event.evt == VFS_READY_EVENT)
      {
        regist_new_device(p_info->p_usf, part_table, &num);
        for(i = 0; i < num; i++)
        {
          _mount(part_table[i]);//this physical letter should get from ufs
        }
      }
      else if(p_info->dev_event.evt == VFS_PLUG_OUT_EVENT)
      {
        unregist_device(p_info->p_usf, part_table, &num);
        for(i = 0; i < num; i++)
        {
          _unmount(part_table[i]);//this physical letter should get from ufs
        }
      }
      p_event->evt = p_info->dev_event.evt;
      p_event->p_name = p_info->dev_event.p_name;
      p_info->dev_event.evt = VFS_RESERVED;
      p_info->dev_event.p_name = NULL;
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

RET_CODE _ioctl (hfile_t file, vfs_cmd_t vfs_cmd, u32 para0, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  vfs_file_t *p_vfs_file = (vfs_file_t *)file;
  
  MT_ASSERT(p_vfs_file != NULL); 

  switch(vfs_cmd)
  {
    case VFS_CMD_CHDIR:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_CHDIR, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    case VFS_CMD_CHDRIVE:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_CHDRIVE, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    case VFS_CMD_FORMAT:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_FORMAT, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    case VFS_CMD_MKDIR:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_MKDIE, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    case VFS_CMD_MOUNT:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_MOUNT, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    case VFS_CMD_SHOW_PATH:
      ret = ufs_ioctl((struct drv_dev *)&p_vfs_file->ufs_file, 
        FS_SHOW_PATH, (u8 *)para0, (void *)para1, (void *)para2);
      break;

    default:
      break;
  }

  return SUCCESS;
}

void vfs_deinit(void)
{
  vfs_t *p_vfs = NULL;
  vfs_info_t *p_vfs_info = NULL;
  p_vfs = (vfs_t *)class_get_handle_by_id(VFS_CLASS_ID);
  p_vfs_info = (vfs_info_t *)p_vfs->p_priv;
  
 if(p_vfs != NULL)
  {
   if(p_vfs_info != NULL)
    {
      mtos_free(p_vfs_info);
    }
    mtos_free(p_vfs);
  }
  return;
}


void vfs_system_init(void)
{
  vfs_t *p_vfs = NULL;
  vfs_info_t *p_vfs_info = NULL;
  
  p_vfs = (vfs_t *)mtos_malloc(sizeof(vfs_t));
  MT_ASSERT(p_vfs != NULL);
  //extern u8 ufs_dev_init();
  ufs_dev_init();
  memset((void *)p_vfs, 0, sizeof(vfs_t));

  p_vfs->mount = _mount;
  p_vfs->unmount = _unmount;
  p_vfs->open = _open;
  p_vfs->read = _read;
  p_vfs->write = _write;
  p_vfs->close = _close;
  p_vfs->del = _delete;
  p_vfs->seek = _seek;
  p_vfs->get_file_info = _get_file_info;
  p_vfs->opendir = _opendir;
  p_vfs->closedir = _closedir;
  p_vfs->readdir = _readdir;
  p_vfs->process = _process;
  p_vfs->ioctl = _ioctl;
  p_vfs->format = _format;
  p_vfs->get_dev_info = _get_dev_info;
  p_vfs->get_partitions = _get_partitions;
  p_vfs->rename = _rename;

  p_vfs_info = (vfs_info_t *)mtos_malloc(sizeof(vfs_info_t));
  MT_ASSERT(p_vfs_info != NULL);

  memset((void *)p_vfs_info, 0, sizeof(vfs_info_t));

  p_vfs->p_priv = p_vfs_info;
  class_register(VFS_CLASS_ID, (class_handle_t)(p_vfs));
  OS_PRINTF("vfs init ok\n");
  return;
}
#endif
s64 vfs_tell(hfile_t file)
{
  return 0;
}

void vfs_close(hfile_t file)
{
  return;
}

u32 vfs_read(void * p_buf, u32 count, u32 size, hfile_t file)
{
  return 0;
}

void vfs_seek(hfile_t file, s64 offset, vfs_seek_mode_t mode)
{
  return;
}


hfile_t vfs_open(const char *p_path, const char *mode)
{

  return 0;
}