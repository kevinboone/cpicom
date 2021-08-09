/*=========================================================================
 
  cpicom

  files/filesystemfat.c 

  CPICOM-compatible wrapper for the FatFS filesystem handler

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "error/error.h"
#include "files/compat.h"
#include "log/log.h"
#include "blockdevice/blockdevice.h"
#include "fs/filesystemfat.h"
#include "fs/ff.h"
#include "fs/ffdiskio.h"

struct _FilesystemFat
  {
  BlockDeviceParams bd_params; 
  int ffvol;
  FATFS fatfs;
  }; 

// 
// filesystemfat_create 
//
FilesystemFat *filesystemfat_create (int ffvol)
  {
  FilesystemFat *self = malloc (sizeof (FilesystemFat));
  memset (self, 0, sizeof (FilesystemFat));
  self->ffvol = ffvol;
  return self;
  }

// 
// filesystemfat_destroy
//
void filesystemfat_destroy (FilesystemFat *self)
  {
  if (self)
    {
    free (self);
    }
  }

//
// filesystemfat_prepend_drive_num
// 
char *filesystemfat_prepend_drive_num (const FilesystemFat *self, 
              const char *path)
  {
  char *s = malloc (strlen (path) + 4);
  sprintf (s, "%d:%s", self->ffvol, path);
  return s;
  }

//
// filesystemfat_convert_error
//
static Error filesystemfat_convert_error (int lfs_error)
  {
  switch (lfs_error)
    {
    case 0: return 0;
    case FR_DISK_ERR: return ERROR_DISK_LOWLEVEL;
    case FR_NOT_READY: return ERROR_DRIVE_NOT_READY;
    case FR_NO_FILE: return ENOENT;
    case FR_NO_PATH: return ENOENT;
    case FR_INVALID_NAME: return ERROR_FILENAME;
    case FR_DENIED: return EACCES;
    case FR_EXIST: return EACCES;
    case FR_WRITE_PROTECTED: return EROFS;
    case FR_INVALID_DRIVE: return ERROR_BADDRIVELETTER;
    case FR_NO_FILESYSTEM: return ERROR_CORRUPT_FS;
    case FR_TOO_MANY_OPEN_FILES: return EMFILE;
    default: return ERROR_INTERNAL;
    }
  }

//
// filesystemfat_format
//
Error filesystemfat_format (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Fmt LFS fs");
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  char *temp = filesystemfat_prepend_drive_num (self, "");
  int bufflen = 10 * FF_MIN_SS;
  void *buff = malloc (bufflen);
  MKFS_PARM opt;
  opt.fmt = FM_ANY;
  opt.au_size = 0; // Use default 
  opt.align = 1;
  opt.n_fat = 2;
  opt.n_root = 0;
  int faterr = f_mkfs (temp, &opt, buff, bufflen);
  free (temp);
  free (buff);
  return filesystemfat_convert_error (faterr);
  }


//
// filesystemfat_set_block_device
//
void filesystemfat_set_block_device (FilesystemFat *self, 
                           BlockDeviceParams *bd_params)
  {
  memcpy (&self->bd_params, bd_params, sizeof (BlockDeviceParams));

  //log_debug ("Set LFS blk sz to %d", self->cfg.read_size);
  }

//
// filesystemfat_mount
//
Error filesystemfat_mount (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Mnt FAT fs");
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 

  memcpy (&bdp[self->ffvol], bd_params, sizeof (BlockDeviceParams));
  char *temp = filesystemfat_prepend_drive_num (self, "");
  int faterr = f_mount (&self->fatfs, temp, 1); 
  free (temp);

  return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_unmount
//
Error filesystemfat_unmount (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Unmnt FAT fs");
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  char *temp = filesystemfat_prepend_drive_num (self, "");
  int faterr = f_unmount (temp);
  free (temp);
  return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_open
//
Error filesystemfat_open (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, const char *name, int flags, 
          OpenFileInfo *ofi)
  {
  (void)bd_params;
  uint8_t fat_flags = 0;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 

  if ((flags & O_ACCMODE) == O_RDONLY) fat_flags |= FA_READ;
  else if ((flags & O_ACCMODE) == O_WRONLY) fat_flags |= FA_WRITE;
  else fat_flags |= (FA_WRITE | FA_READ);
  if (flags & O_CREAT) fat_flags |= FA_OPEN_ALWAYS;
  //if (flags & O_EXCL) fat_flags |= LFS_O_EXCL;
  if (flags & O_TRUNC) fat_flags |= FA_CREATE_ALWAYS;
  if (flags & O_APPEND) fat_flags |= FA_OPEN_APPEND;

  log_debug ("FAT fs rq open %s flg=%04X fat_flg=%02X", name, 
      flags, fat_flags);

  FIL *fil = malloc (sizeof (FIL));
  char *temp = filesystemfat_prepend_drive_num (self, name);
  FRESULT faterr = f_open (fil, temp, fat_flags);
  free (temp);
  if (faterr == 0)
    {
    ofi->opaque1 = fil;
    }
  else
    {
    free (fil);
    }
  return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_close
//
Error filesystemfat_close (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi)
  {
  (void)bd_params; (void)fs_params;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("FAT fs rq close %s", ofi->path);
  FIL *fil = ofi->opaque1;
  int faterr = f_close (fil);
  free (fil);
  return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_read
//
Error filesystemfat_read (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi,
	  uint8_t *buffer, uint32_t len, uint32_t *read)
  {
  (void)bd_params; (void)fs_params;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("FAT fs rq rd %s len=%d", 
    ofi->path, (int)len);
  FIL *fil = ofi->opaque1;
  UINT n = 0;
  int faterr = f_read (fil, buffer, len, &n);
  if (faterr)
    {
    *read = 0;
    return filesystemfat_convert_error (faterr);
    }
  else
    {
    *read = n;
    return 0;
    }
  }

//
// filesystemfat_write
//
Error filesystemfat_write (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi,
	  const uint8_t *buffer, uint32_t len, uint32_t *written)
  {
  (void)bd_params; (void)fs_params;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("LFS fs rq wrt %s len=%d", 
    ofi->path, (int)len);
  FIL *fil = ofi->opaque1;
  UINT n = 0;
  int faterr = f_write (fil, buffer, len, &n);
  if (faterr)
    {
    *written = 0;
    return filesystemfat_convert_error (faterr);
    }
  else
    {
    *written = n;
    return 0;
    }
  }


//
// filesystemfat_closedir
//
Error filesystemfat_closedir (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params,
           my_DIR *dir)
  {
  (void)bd_params; (void)fs_params;
  log_debug ("VAT fs rq close dir"); 
  Error ret = 0;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  DIR *fdir = dir->opaque1; 
  int faterr = f_closedir (fdir);
  if (faterr == 0)
    {
    free (fdir);
    free (dir);
    }
  else
    {
    return filesystemfat_convert_error (faterr);
    }
  return ret;
  }

//
// filesystemfat_opendir
//
my_DIR *filesystemfat_opendir (FilesystemParams *fs_params, 
             BlockDeviceParams *bd_params, const char *path)
  {
  log_debug ("FAT fs rq open dir %s", path); 
  (void)bd_params;
  errno = 0;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  my_DIR *ret = NULL;
  DIR *dir = malloc (sizeof (DIR));
  char *temp = filesystemfat_prepend_drive_num (self, path);
  int faterr = f_opendir (dir, temp);
  free (temp);
  if (faterr == 0)
    {
    ret = malloc (sizeof (my_DIR));
    if (ret)
      {
      memset (&ret->current_de, 0, sizeof (struct my_dirent));
      ret->opaque1 = dir;
      errno = 0;
      }
    else
      errno = ERROR_NOMEM;
    }
  else
    {
    free (dir);
    errno = filesystemfat_convert_error (faterr);
    }
  return ret;
  }

//
// filesystemfat_readdir
//
struct my_dirent *filesystemfat_readdir 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _my_DIR *dir)
  {
  (void)bd_params; (void)fs_params;
  log_debug ("FAT fs rq read dir"); 
  struct my_dirent *ret = NULL;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  DIR *fdir = dir->opaque1; 
  FILINFO info; 
  int faterr = f_readdir (fdir, &info);
  if (info.fname[0] == 0) // End of dir
    {
    errno = 0;
    ret = NULL;
    }
  else if (faterr == 0) // More files to come
    {
    strncpy (dir->current_de.d_name, info.fname, 
       sizeof (dir->current_de.d_name) - 1);
    if (info.fattrib & AM_DIR) 
      dir->current_de.d_type = DT_DIR_;
    else
      dir->current_de.d_type = DT_REG_;
    ret = &dir->current_de;
    }
  else // Error
    {
    errno = faterr;
    ret = NULL;
    }
  return ret;
  }

//
// filesystemfat_statfs
//
Error filesystemfat_statfs (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, struct _my_statfs *statfs) 
  {
  FATFS *fatfs;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  uint32_t nclusters;
  char *temp = filesystemfat_prepend_drive_num (self, "");
  int faterr = f_getfree (temp, &nclusters, &fatfs); 
  free (temp);
  int free_sectors = nclusters * fatfs->csize;

  uint32_t free_blocks;
  if (bd_params->block_size >= FF_MIN_SS)
    free_blocks = free_sectors * bd_params->block_size / FF_MIN_SS; 
  else
    free_blocks = free_sectors * FF_MIN_SS / bd_params->block_size; 

  if (faterr == 0)
    {
    strcpy (statfs->fs_desc, fs_params->desc);
    strcpy (statfs->bd_desc, bd_params->desc);
    statfs->block_size = bd_params->block_size;
    statfs->total_blocks = bd_params->block_count;
    statfs->free_blocks = free_blocks; 
    return 0;
    }
  else
    return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_stat
//
Error filesystemfat_stat (struct _FilesystemParams *fs_params, 
      BlockDeviceParams *bd_params, const char *path, struct stat *sb)
  {
  uint32_t ret = (uint32_t)-1;
  errno = 0;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("FAT fs rq stat %s", path);
  FILINFO info;
  char *temp = filesystemfat_prepend_drive_num (self, path);
  int faterr = f_stat (path, &info);
  free (temp);
  if (faterr)
    errno =  filesystemfat_convert_error (faterr);
  else
    {
    sb->st_size = info.fsize;
    if (info.fattrib & AM_DIR) 
      sb->st_mode = S_IFDIR | 0x0777; 
    else
      sb->st_mode = S_IFREG | 0x0777; 
    sb->st_blksize = bd_params->block_size;
    sb->st_blocks = bd_params->block_count;
    ret = 0; 
    }
  return ret;
  }

//
// filesystemfat_delete
//
Error filesystemfat_delete (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, const char *path) 
  {
  (void)bd_params;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("FAT fs rq del %s", path);
  char *temp = filesystemfat_prepend_drive_num (self, path);
  int faterr = f_unlink (temp);
  free (temp);
  return filesystemfat_convert_error (faterr);
  }

//
// filesystemfat_lseek
//
uint32_t filesystemfat_lseek (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params, OpenFileInfo *ofi, 
	   uint32_t offset, int whence)
  {
  (void)bd_params; (void)fs_params;
  //FilesystemFat *self = (FilesystemFat *)fs_params->context; 
  log_debug ("FAT fs rq lseek %s off=%d whence=%d", 
    ofi->path, (int)offset, whence);
  FIL *fil = ofi->opaque1;
  uint32_t newoffset;
  if (whence == SEEK_END) 
    newoffset = f_size (fil) - offset; 
  else if (whence == SEEK_CUR) 
    newoffset = offset + f_tell (fil); 
  else 
    newoffset = offset; 

  int faterr = f_lseek (fil, newoffset); 
  if (faterr)
    {
    errno =  filesystemfat_convert_error (faterr);
    return -1;
    }
  else
    {
    return (uint32_t)f_tell (fil);
    }
  }

//
// filesystemfat_rename
//
Error filesystemfat_rename (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params, const char *source, 
           const char *target)
  {
  (void) bd_params;
  FilesystemFat *self = (FilesystemFat *)fs_params->context; 

  char *tempsource = filesystemfat_prepend_drive_num (self, source);
  char *temptarget = filesystemfat_prepend_drive_num (self, target);

  int faterr = f_rename (tempsource, temptarget); 

  free (temptarget);
  free (tempsource);

  return filesystemfat_convert_error (faterr);
  }


Error filesystemfat_ftruncate (struct _FilesystemParams *fs_params, 
         BlockDeviceParams *bd_params, struct _OpenFileInfo *ofi, 
	 uint32_t length)
  {
  (void)bd_params; (void)fs_params;
  FIL *fil = ofi->opaque1;
  f_lseek (fil, length);
  int faterr = f_truncate (fil);
  return filesystemfat_convert_error (faterr);
  }



//
// filesystemfat_get_params
//
void filesystemfat_get_params (FilesystemFat *self, 
                           FilesystemParams *fs_params)
  {
  // TODO
  memset (fs_params, 0, sizeof (FilesystemParams));
  strcpy (fs_params->desc, "FAT");
  fs_params->context = self;
  fs_params->mount = filesystemfat_mount;
  fs_params->unmount = filesystemfat_unmount;
  fs_params->open = filesystemfat_open;
  fs_params->close = filesystemfat_close;
  fs_params->read = filesystemfat_read;
  fs_params->closedir = filesystemfat_closedir;
  fs_params->opendir = filesystemfat_opendir;
  fs_params->readdir = filesystemfat_readdir;
  fs_params->statfs = filesystemfat_statfs;
  fs_params->stat = filesystemfat_stat;
  fs_params->del = filesystemfat_delete;
  fs_params->lseek = filesystemfat_lseek;
  fs_params->write = filesystemfat_write;
  fs_params->rename = filesystemfat_rename;
  fs_params->ftruncate = filesystemfat_ftruncate;
  fs_params->format = filesystemfat_format;
  }



