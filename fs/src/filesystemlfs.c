/*=========================================================================
 
  cpicom

  files/filesystemlfs.c 

  CPICOM-compatible wrapper for the LittleFS filesystem handler

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
#include "fs/filesystemlfs.h"
#include "fs/lfs.h"

struct _FilesystemLfs 
  {
  BlockDeviceParams bd_params; 
  lfs_t lfs;
  struct lfs_config cfg;
  }; 

// 
// filesystemlfs_create 
//
FilesystemLfs *filesystemlfs_create (void)
  {
  FilesystemLfs *self = malloc (sizeof (FilesystemLfs));
  memset (self, 0, sizeof (FilesystemLfs));
  return self;
  }

// 
// filesystemlfs_destroy
//
void filesystemlfs_destroy (FilesystemLfs *self)
  {
  if (self)
    {
    free (self);
    }
  }

static int filesystemlfs_lfs_read (const struct lfs_config *cfg,
        lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
  {
  FilesystemLfs *self = (FilesystemLfs *)cfg->context;

  return self->bd_params.read_fn (self->bd_params.context, 
    buffer, (uint32_t)block, (uint32_t)off, (uint32_t)size);
  }

static int filesystemlfs_lfs_sync (const struct lfs_config *cfg)
  {
  FilesystemLfs *self = (FilesystemLfs *)cfg->context;

  return self->bd_params.sync_fn (self->bd_params.context, 0, 0); // sync all
  }

static int filesystemlfs_lfs_erase (const struct lfs_config *cfg,
    lfs_block_t block)
  {
  FilesystemLfs *self = (FilesystemLfs *)cfg->context;

  return self->bd_params.erase_fn (self->bd_params.context, block); 
  }

static int filesystemlfs_lfs_prog (const struct lfs_config *cfg,
        lfs_block_t block, lfs_off_t off, const void *buffer,
        lfs_size_t size)
  {
  FilesystemLfs *self = (FilesystemLfs *)cfg->context;

  return self->bd_params.write_fn (self->bd_params.context, 
    buffer, (uint32_t) block, (uint32_t)off, (uint32_t)size);
  }

//
// filesystemlfs_set_block_device
//
void filesystemlfs_set_block_device (FilesystemLfs *self, 
                           BlockDeviceParams *bd_params)
  {
  memcpy (&self->bd_params, bd_params, sizeof (BlockDeviceParams));

  self->cfg.context = self; 
  self->cfg.read_size = bd_params->block_size;
  self->cfg.prog_size = bd_params->block_size;
  self->cfg.block_size = bd_params->block_size;
  self->cfg.cache_size = bd_params->block_size; 
  self->cfg.lookahead_size = bd_params->block_size; 
  self->cfg.block_count = bd_params->block_count; 
  self->cfg.block_cycles = 500;
  self->cfg.read = filesystemlfs_lfs_read;
  self->cfg.prog = filesystemlfs_lfs_prog;
  self->cfg.erase = filesystemlfs_lfs_erase;
  self->cfg.sync = filesystemlfs_lfs_sync;
  log_debug ("Set LFS blk sz to %d", self->cfg.read_size);
  log_debug ("Set LFS blk ct to %d", self->cfg.block_count);
  }

//
// filesystemlfs_convert_error
//
static Error filesystemlfs_convert_error (int lfs_error)
  {
  switch (lfs_error)
    {
    case 0: return 0;
    // TODO others -- we are kind of assuming the LFS errors are 
    //   negated versions of conventional errno's, but that might not
    //   always be true
    case LFS_ERR_CORRUPT: return ERROR_CORRUPT_FS; 
    }
  return -lfs_error;
  }

//
// filesystemlfs_mount
//
Error filesystemlfs_mount (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Mnt LFS fs");
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  int lfs_err = lfs_mount (&self->lfs, &self->cfg);
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_unmount
//
Error filesystemlfs_unmount (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Unmnt LFS fs");
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  int lfs_err = lfs_unmount (&self->lfs);
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_format
//
Error filesystemlfs_format (FilesystemParams *fs_params, 
       BlockDeviceParams *bd_params)
  {
  (void)bd_params;
  log_info ("Fmt LFS fs");
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  int lfs_err = lfs_format (&self->lfs, &self->cfg);
  if (lfs_err == 0)
    {
    lfs_err = lfs_mount (&self->lfs, &self->cfg);
    }
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_open
//
Error filesystemlfs_open (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, const char *name, int flags, 
          OpenFileInfo *ofi)
  {
  (void)bd_params;
  int lfs_flags = 0;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  if ((flags & O_ACCMODE) == O_RDONLY) lfs_flags = LFS_O_RDONLY;
  else if ((flags & O_ACCMODE) == O_WRONLY) lfs_flags = LFS_O_WRONLY;
  else lfs_flags = LFS_O_RDWR;
  if (flags & O_CREAT) lfs_flags |= LFS_O_CREAT;
  if (flags & O_EXCL) lfs_flags |= LFS_O_EXCL;
  if (flags & O_TRUNC) lfs_flags |= LFS_O_TRUNC;
  if (flags & O_APPEND) lfs_flags |= LFS_O_APPEND;
  log_debug ("LFS fs rq open %s flg=%04X lfs_flg=%04X", name, 
      flags, lfs_flags);
  lfs_file_t *file = malloc (sizeof (lfs_file_t));
  int lfs_err = lfs_file_open (&self->lfs, file, name, lfs_flags);
  if (lfs_err == 0)
    {
    ofi->opaque1 = file;
    }
  else
    {
    free (file);
    }
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_write
//
Error filesystemlfs_write (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi,
	  const uint8_t *buffer, uint32_t len, uint32_t *written)
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq wrt %s len=%d", 
    ofi->path, (int)len);
  lfs_file_t *file = ofi->opaque1;
  int lfs_err = lfs_file_write (&self->lfs, file, buffer, len);
  if (lfs_err < 0)
    {
    *written = 0;
    return filesystemlfs_convert_error (lfs_err);
    }
  else
    {
    *written = lfs_err;
    return 0;
    }
  }

//
// filesystemlfs_read
//
Error filesystemlfs_read (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi,
	  uint8_t *buffer, uint32_t len, uint32_t *read)
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq rd %s len=%d", 
    ofi->path, (int)len);
  lfs_file_t *file = ofi->opaque1;
  int lfs_err = lfs_file_read (&self->lfs, file, buffer, len);
  if (lfs_err < 0)
    {
    *read = 0;
    return filesystemlfs_convert_error (lfs_err);
    }
  else
    {
    *read = lfs_err;
    return 0;
    }
  }

//
// filesystemlfs_delete
//
Error filesystemlfs_delete (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, const char *path) 
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq del %s", path);
  int lfs_err = lfs_remove (&self->lfs, path);
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_close
//
Error filesystemlfs_close (FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, OpenFileInfo *ofi)
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq close %s", ofi->path);
  lfs_file_t *file = ofi->opaque1;
  int lfs_err = lfs_file_close (&self->lfs, file);
  free (file);
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_opendir
//
my_DIR *filesystemlfs_opendir (FilesystemParams *fs_params, 
             BlockDeviceParams *bd_params, const char *path)
  {
  log_debug ("LFS fs rq open dir %s", path); 
  (void)bd_params;
  errno = 0;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  my_DIR *ret = NULL;
  lfs_dir_t *dir = malloc (sizeof (lfs_dir_t));
  int lfs_err = lfs_dir_open (&self->lfs, dir, path);
  if (lfs_err == 0)
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
    errno = filesystemlfs_convert_error (lfs_err);
    }
  return ret;
  }

//
// filesystemlfs_closedir
//
Error filesystemlfs_closedir (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params,
           my_DIR *dir)
  {
  (void)bd_params;
  log_debug ("LFS fs rq close dir"); 
  Error ret = 0;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  lfs_dir_t *ldir = dir->opaque1; 
  int lfs_err = lfs_dir_close (&self->lfs, ldir);
  if (lfs_err == 0)
    {
    free (ldir);
    free (dir);
    }
  else
    {
    ret = -lfs_err;
    }
  return ret;
  }

//
// filesystemlfs_readdir
//
struct my_dirent *filesystemlfs_readdir 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _my_DIR *dir)
  {
  (void)bd_params;
  log_debug ("LFS fs rq read dir"); 
  struct my_dirent *ret = NULL;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  lfs_dir_t *ldir = dir->opaque1; 
  struct lfs_info info;
  int lfs_err = lfs_dir_read (&self->lfs, ldir, &info);
  if (lfs_err == 0) // End of dir
    {
    errno = 0;
    ret = NULL;
    }
  else if (lfs_err > 0) // More files to come
    {
    strncpy (dir->current_de.d_name, info.name, 
       sizeof (dir->current_de.d_name) - 1);
    if (info.type == LFS_TYPE_DIR)
      dir->current_de.d_type = DT_DIR_;
    else
      dir->current_de.d_type = DT_REG_;
    ret = &dir->current_de;
    }
  else // Error
    {
    errno = -lfs_err;
    ret = NULL;
    }
  return ret;
  }

//
// filesystemlfs_lseek
//
uint32_t filesystemlfs_lseek (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params, OpenFileInfo *ofi, 
	   uint32_t offset, int whence)
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq lseek %s off=%d whence=%d", 
    ofi->path, (int)offset, whence);
  lfs_file_t *file = ofi->opaque1;
  int lwhence;
  if (whence == SEEK_END) 
    lwhence = LFS_SEEK_END;
  else if (whence == SEEK_CUR) 
    lwhence = LFS_SEEK_CUR;
  else 
    lwhence = LFS_SEEK_SET;

  int newoff = lfs_file_seek (&self->lfs, file, offset, lwhence);
  if (newoff < 0)
    {
    errno =  filesystemlfs_convert_error (newoff);
    return -1;
    }
  else
    {
    return (uint32_t)newoff;
    }
  }

//
// filesystemlfs_size
//
uint32_t filesystemlfs_size (struct _FilesystemParams *fs_params, 
      BlockDeviceParams *bd_params, const char *path)
  {
  (void)bd_params;
  uint32_t ret = (uint32_t)-1;
  errno = 0;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq size %s", path);
  struct lfs_info info;
  int lfs_ret = lfs_stat (&self->lfs, path, &info);
  if (lfs_ret < 0)
    {
    errno =  filesystemlfs_convert_error (lfs_ret);
    }
  else
    {
    ret = (uint32_t)info.size;
    }
  return ret;
  }

//
// filesystemlfs_stat
//
Error filesystemlfs_stat (struct _FilesystemParams *fs_params, 
      BlockDeviceParams *bd_params, const char *path, struct stat *sb)
  {
  uint32_t ret = (uint32_t)-1;
  errno = 0;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  log_debug ("LFS fs rq stat %s", path);
  struct lfs_info info;
  int lfs_ret = lfs_stat (&self->lfs, path, &info);
  if (lfs_ret < 0)
    {
    errno =  filesystemlfs_convert_error (lfs_ret);
    }
  else
    {
    sb->st_size = info.size;
    if (info.type == LFS_TYPE_DIR)
      sb->st_mode = S_IFDIR | 0x0777; 
    else
      sb->st_mode = S_IFREG | 0x0777; 
    sb->st_blksize = bd_params->block_size;
    sb->st_blocks = bd_params->block_count;
    ret = 0; 
    }
  return ret;
  }

Error filesystemlfs_ftruncate (struct _FilesystemParams *fs_params, 
         BlockDeviceParams *bd_params, struct _OpenFileInfo *ofi, 
	 uint32_t length)
  {
  (void)bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  lfs_file_t *file = ofi->opaque1;
  int lfs_err = lfs_file_truncate (&self->lfs, file, length);
  return filesystemlfs_convert_error (lfs_err);
  }

//
// filesystemlfs_rename
//
Error filesystemlfs_rename (struct _FilesystemParams *fs_params, 
           BlockDeviceParams *bd_params, const char *source, 
           const char *target)
  {
  (void) bd_params;
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  int lfs_err = lfs_rename (&self->lfs, source, target);
  return filesystemlfs_convert_error (lfs_err);
  }


//
// filesystemlfs_statfs
//
Error filesystemlfs_statfs (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, struct _my_statfs *statfs) 
  {
  FilesystemLfs *self = (FilesystemLfs *)fs_params->context; 
  int lfs_size = (int)lfs_fs_size (&self->lfs);
  if (lfs_size > 0)
    {
    strcpy (statfs->fs_desc, fs_params->desc);
    strcpy (statfs->bd_desc, bd_params->desc);
    statfs->block_size = bd_params->block_size;
    statfs->total_blocks = bd_params->block_count;
    statfs->free_blocks = statfs->total_blocks - lfs_size; 
    return 0;
    }
  else
    return filesystemlfs_convert_error (lfs_size);
  }

//
// filesystemlfs_get_params
//
void filesystemlfs_get_params (FilesystemLfs *self, 
                           FilesystemParams *fs_params)
  {
  // TODO
  strcpy (fs_params->desc, "LFS");
  fs_params->context = self;
  fs_params->mount = filesystemlfs_mount;
  fs_params->unmount = filesystemlfs_unmount;
  fs_params->format = filesystemlfs_format;
  fs_params->open = filesystemlfs_open;
  fs_params->close = filesystemlfs_close;
  fs_params->write = filesystemlfs_write;
  fs_params->read = filesystemlfs_read;
  fs_params->del = filesystemlfs_delete;
  fs_params->opendir = filesystemlfs_opendir;
  fs_params->closedir = filesystemlfs_closedir;
  fs_params->readdir = filesystemlfs_readdir;
  fs_params->lseek = filesystemlfs_lseek;
  fs_params->stat = filesystemlfs_stat;
  fs_params->ftruncate = filesystemlfs_ftruncate;
  fs_params->rename = filesystemlfs_rename;
  fs_params->statfs = filesystemlfs_statfs;
  }


