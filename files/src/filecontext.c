/*=========================================================================
 
  cpicom

  files/filecontext.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "files/filecontext.h"
#include "files/filesystemmount.h"
#include "log/log.h"

// Global filesystem context
FileContext *global_fc;

struct _FileContext 
  {
  FilesystemMount *mounts[FILESYSTEM_MAX_MOUNTS];
  OpenFileInfo open_files [MAX_OPEN_FILES];
  int current_drive;
  // char cwd [XXX]; // TODO -- working directory
  }; 

// 
// filecontext_create 
//
FileContext *filecontext_create (void)
  {
  FileContext *self = malloc (sizeof (FileContext));
  memset (self, 0, sizeof (FileContext));
  return self;
  }

// 
// filecontext_destroy
//
void filecontext_destroy (FileContext *self)
  {
  if (self)
    {
    filecontext_unmount_all (self);
    free (self);
    }
  }

// 
// filecontext_unmount_all
//
Error filecontext_unmount (FileContext *self, uint8_t drive_num)
  {
  Error ret = 0;
  FilesystemMount *mount = self->mounts[drive_num];
  ret = filesystemmount_unmount (mount); 
  if (ret == 0)
    {
    filesystemmount_destroy (mount);
    self->mounts[drive_num] = 0;
    log_debug ("Drv %d rmvd from mnt tab", drive_num);
    }
  else
    {
    log_debug ("Drv %d not rmvd from mnt tab", drive_num);
    }
  return ret;
  }

// 
// filecontext_unmount_all
//
void filecontext_unmount_all (FileContext *self)
  {
  log_info ("Unmounting all fs");
  for (int i = 0; i < FILESYSTEM_MAX_MOUNTS; i++)
    {
    if (self->mounts[i])
      filecontext_unmount (self, i);
    }
  log_info ("All fs unmounted");
  }

// TODO -- don't forget to unmount when mounting over existing drive


Error filecontext_mount (FileContext *self, uint8_t drive_num, 
        FilesystemParams *fs_params, 
        BlockDeviceParams *bd_params)
  {
  if (drive_num >= FILESYSTEM_MAX_MOUNTS) 
    return ERROR_BADDRIVELETTER;
  Error ret = 0;
  log_info ("Mnt (%s, %s) on drv %d", fs_params->desc, bd_params->desc, 
        drive_num);
  FilesystemMount *mount = filesystemmount_create (fs_params, bd_params);
  ret = filesystemmount_mount (mount);
  self->mounts[drive_num] = mount;
  // Note that a failed mount is still a mount -- the media might need to
  //  be formatted, for example
  if (ret == 0)
    {
    }
  else 
    {
    log_info ("Failed to mount (%s, %s) on drv %d, error %d",  
       fs_params->desc, bd_params->desc, drive_num, ret);
    }
  return ret;
  }

// 
// filecontext_format
//
Error filecontext_format (FileContext *self, uint8_t drive_num)
  {
  Error ret = 0;
  log_info ("Fmt drv %d", drive_num);
  FilesystemMount *mount = self->mounts[drive_num];
  if (mount)
    {
    ret = filesystemmount_format (mount);
    }
  else
    {
    log_error ("Can't format undefined drive %d", drive_num);
    ret = ERROR_UNDEFINED_DRIVE;
    } 
  return ret;
  }

static int filecontext_find_free_file_slot (const FileContext *self)
  {
  for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
    if (self->open_files[i].in_use == FALSE) return i;
    }
  return 0;
  }

//
// filecontext_delete
//
Error filecontext_delete (FileContext *self, uint8_t drive, 
         const char *filename)
  {
  int ret = 0;
  if (drive >= FILESYSTEM_MAX_MOUNTS) return ERROR_BADDRIVELETTER;
  FilesystemMount *mount = self->mounts [drive];
  if (mount)
    {
    ret = filesystemmount_delete (mount, filename);
    }
  else
    {
    log_debug 
        ("fc_delete: del file on undef drv"); 
    ret = ERROR_UNDEFINED_DRIVE;; 
    }
  return ret;
  }

//
// filecontext_open
//
int filecontext_open (FileContext *self, uint8_t drive, const char *name, 
      int flags)
  {
  if (drive >= FILESYSTEM_MAX_MOUNTS) return ERROR_BADDRIVELETTER;
  int ret = 0;
  int fd = filecontext_find_free_file_slot (self);
  log_debug ("fc_open: use fd slot %d", fd);
  if (fd >= 0 && fd < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      OpenFileInfo *ofi = &self->open_files[fd];
      Error err = filesystemmount_open (mount, name, flags, ofi);
      if (err == 0)
        {
        ofi->in_use = TRUE;
	ofi->drive = drive;
	ofi->path = strdup (name);
        ret = fd;
        }
      else
        {
        errno = err; 
        log_debug ("fc_open failed, errno = %d", errno);
        ret = -1;
        }
      }
    else
      {
      log_debug 
        ("fc_open: open file on undef drv"); 
      errno = ERROR_UNDEFINED_DRIVE; 
      ret = -1;
      }
    }
  else
    {
    errno = ERROR_MFILE; 
    log_error ("filecontext_open: too many open files", fd);
    }
  return ret;
  }

//
// filecontext_close
//
int filecontext_close (FileContext *self, int fd) 
  {
  log_debug ("Close %d", fd);
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_warn ("Closing bad fd: %d", fd);
    errno = ERROR_BADF;
    return -1;
    }

  int ret = -1;
  OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    Error err = filesystemmount_close (ofi->mount, ofi);
    if (err == 0)
      {
      ofi->in_use = FALSE;
      if (ofi->path) free (ofi->path);
      ret = 0;
      }
    else
      {
      ret = -1;
      errno = err;
      }
    }
  else
    {
    log_error ("Closing fd that is not in use: %d", fd);
    errno = ERROR_BADF;
    }
  return ret;
  }

//
// filecontext_write
//
ssize_t filecontext_write (FileContext *self, int fd, const void *buf, 
          size_t n)
  {
  log_debug ("Write %d", fd);
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_error ("Writing on bad file descriptor: %d", fd);
    errno = ERROR_BADF;
    return -1; 
    }

  int ret = -1;

  OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    uint32_t written;
    Error err = filesystemmount_write (ofi->mount, ofi, buf, n, &written);
    if (err == 0)
      {
      ret = written;
      errno = 0;
      }
    else
      {
      ret = -1;
      errno = err;
      }
    }
  else
    {
    log_error ("Writing on fd that is not in use: %d", fd);
    errno = ERROR_BADF;
    }
  return ret;
  }

//
// filecontext_read
//
ssize_t filecontext_read (FileContext *self, int fd, void *buf, 
          size_t n)
  {
  log_debug ("Read %d", fd);
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_error ("Reading from bad file descriptor: %d", fd);
    errno = ERROR_BADF;
    return -1; 
    }

  int ret = -1;

  OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    uint32_t read;
    Error err = filesystemmount_read (ofi->mount, ofi, buf, n, &read);
    if (err == 0)
      {
      ret = read;
      errno = 0;
      }
    else
      {
      ret = -1;
      errno = err;
      }
    }
  else
    {
    log_error ("Reading from fd that is not in use: %d", fd);
    errno = ERROR_BADF;
    }
  return ret;
  }

//
// filecontext_opendir
//
my_DIR *filecontext_opendir (FileContext *self, uint8_t drive, 
          const char *path)
  {
  my_DIR *ret = NULL;

  // TODO: if path does not start /, substitute CWD (when we implement
  //   a notion of CWD)
  if (path == NULL || path[0] == 0)
    path = "/";

  log_debug ("fc_opendir %d:%s", (int) drive, path);

  if (drive < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      ret = filesystemmount_opendir (mount, path);
      if (ret) ret->drive = drive;
      }
    else
      {
      log_debug 
        ("fc_open: open dir on undef drv"); 
      errno = ERROR_UNDEFINED_DRIVE; 
      }
    }
  else
    {
    errno = ERROR_BADDRIVELETTER;
    }

  return ret;
  }

//
// filecontext_closedir
//
Error filecontext_closedir (FileContext *self, my_DIR *dir)
  {
  uint8_t drive = (uint8_t)dir->drive;
  Error ret = 0;
  log_debug ("fc_closedir");

  if (drive < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      ret = filesystemmount_closedir (mount, dir);
      }
    else
      {
      log_debug 
        ("fc_open: open dir on undef drv"); 
      ret = ERROR_UNDEFINED_DRIVE; 
      }
    }
  else
    {
    ret = ERROR_BADDRIVELETTER;
    }
  return ret;
  }

//
// filecontext_readdir
//
struct my_dirent *filecontext_readdir (FileContext *self, 
          struct _my_DIR *dir)
  {
  uint8_t drive = (uint8_t)dir->drive;
  struct my_dirent *ret = NULL; 
  log_debug ("fc_readdir");

  if (drive < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      ret = filesystemmount_readdir (mount, dir);
      }
    else
      {
      log_debug 
        ("fc_readdir: on undef drv"); 
      errno = ERROR_UNDEFINED_DRIVE; 
      }
    }
  else
    {
    errno = ERROR_BADDRIVELETTER;
    }
  return ret;
  }

//
// filecontext_get_ofi
//
const OpenFileInfo *filecontext_get_ofi (const FileContext *self, int i)
  {
  return &self->open_files[i];
  }

//
// filecontext_lseek
//
uint32_t filecontext_lseek (FileContext *self, int fd, uint32_t offset, 
           int whence)
  {
  log_debug ("lseek %d %d %d", fd, (int)offset, whence);
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_error ("Reading from bad file descriptor: %d", fd);
    errno = ERROR_BADF;
    return -1; 
    }

  int ret = -1;

  OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    uint32_t newoff = filesystemmount_lseek (ofi->mount, ofi, 
                offset, whence);
    if (newoff == (uint32_t)-1)
      {
      ret = -1;
      }
    else
      {
      errno = 0;
      ret = newoff;
      }
    }
  else
    {
    log_error ("Reading from fd that is not in use: %d", fd);
    errno = ERROR_BADF;
    }

  return (uint32_t)ret;
  }

//
// filecontext_size
//
uint32_t filecontext_size (const FileContext *self, uint8_t drive, 
      const char *path)
  {
  uint32_t ret = (uint32_t)-1;
  if (drive >= FILESYSTEM_MAX_MOUNTS) 
    {
    errno = ERROR_BADDRIVELETTER;
    return ret;
    }

  FilesystemMount *mount = self->mounts [drive];
  if (mount)
    {
    ret = filesystemmount_size (mount, path);
    }
  else
    {
    errno = ERROR_UNDEFINED_DRIVE; 
    }
  return ret;
  }

//
// filecontext_fstat
// 
Error filecontext_fstat (const FileContext *self, int fd, struct stat *sb)
  {
  errno = 0;
  Error ret = 0;

  log_debug ("fstat %d", fd); 
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_error ("Reading from bad file descriptor: %d", fd);
    errno = ERROR_BADF;
    return -1; 
    }

  const OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    ret = filesystemmount_fstat (ofi->mount, ofi, sb); 
    sb->st_dev = ofi->drive;
    }
  else
    {
    log_error ("Reading from fd that is not in use: %d", fd);
    ret = ERROR_BADF;
    }

  return ret;
  }

//
// filecontext_ftruncate
//
Error filecontext_ftruncate (FileContext *self, int fd, 
        uint32_t length)
  {
  errno = 0;
  Error ret = 0;

  log_debug ("fstat %d", fd); 
  if (fd < 0 || fd >= MAX_OPEN_FILES)
    {
    log_error ("Reading from bad file descriptor: %d", fd);
    errno = ERROR_BADF;
    return -1; 
    }

  OpenFileInfo *ofi = &self->open_files [fd];
  if (ofi->in_use)
    {
    ret = filesystemmount_ftruncate (ofi->mount, ofi, length); 
    }
  else
    {
    log_error ("Reading from fd that is not in use: %d", fd);
    ret = ERROR_BADF;
    }

  return ret;
  }

//
// filecontext_rename
//
Error filecontext_rename (const FileContext *self,
                            uint8_t drive, 
                            const char *source_path, const char *target_path)
  {
  Error ret = 0;
  if (drive < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      ret = filesystemmount_rename (mount, source_path, target_path);
      }
    else
      {
      log_debug 
        ("fc_readdir: on undef drv"); 
      errno = ERROR_UNDEFINED_DRIVE; 
      }
    }
  else
    {
    ret = ERROR_BADDRIVELETTER;
    }
  return ret;
  }


 //
 // filecontext_dstatfs
 //
 // 0=A, 1=B...
Error filecontext_dstatfs (const FileContext *self, uint8_t drive, 
          struct _my_statfs *statfs)
  {
  Error ret = 0;
  if (drive < FILESYSTEM_MAX_MOUNTS)
    {
    FilesystemMount *mount = self->mounts [drive];
    if (mount)
      {
      ret = filesystemmount_statfs (mount, statfs);
      }
    else
      {
      log_debug 
        ("fc_readdir: on unmounted drv"); 
      ret = ERROR_UNDEFINED_DRIVE; 
      }
    }
  else
    {
    ret = ERROR_BADDRIVELETTER;
    }
  return ret;
  }


//
// filecontext_global_get_current_drive 
//
// 0=A, 1=B...
int filecontext_global_get_current_drive (void)
  {
  return global_fc->current_drive;
  }

//
// filecontext_global_set_current_drive 
//
void filecontext_global_set_current_drive (uint8_t drive)
  {
  if (drive < FILESYSTEM_MAX_MOUNTS) 
    global_fc->current_drive = drive;
  else
    global_fc->current_drive = 0;
  }

//
// filecontext_global_open
//
int filecontext_global_open (uint8_t drive, const char *name, int flags)
  {
  return filecontext_open (global_fc, drive, name, flags);
  }

//
// filecontext_global_close
//
int filecontext_global_close (int fd)
  {
  return filecontext_close (global_fc, fd);
  }

//
// filecontext_global_write
//
ssize_t filecontext_global_write (int fd, const void *buf, size_t n)
  {
  return filecontext_write (global_fc, fd, buf, n);
  }

//
// filecontext_global_read
//
ssize_t filecontext_global_read (int fd, void *buf, size_t n)
  {
  return filecontext_read (global_fc, fd, buf, n);
  }

//
// filecontext_global_delete
//
Error filecontext_global_delete (uint8_t drive, const char *filename)
  {
  return filecontext_delete (global_fc, drive, filename); 
  }

//
// filecontext_global_opendir
//
my_DIR *filecontext_global_opendir (uint8_t drive, const char *path)
  {
  return filecontext_opendir (global_fc, drive, path);
  }

//
// filecontext_global_closedir
//
Error filecontext_global_closedir (my_DIR *dir)
  {
  return filecontext_closedir (global_fc, dir);
  }

//
// filecontext_global_readdir
//
struct my_dirent *filecontext_global_readdir (struct _my_DIR *dir)
  {
  return filecontext_readdir (global_fc, dir);
  }

//
// filecontext_global_get_ofi
//
const OpenFileInfo *filecontext_global_get_ofi (int i)
  {
  return filecontext_get_ofi (global_fc, i);
  }

//
// filecontext_global_lseek
//
uint32_t filecontext_global_lseek (int fd, uint32_t offset, int whence)
  {
  return filecontext_lseek (global_fc, fd, offset, whence);
  }

//
// filecontext_global_lseek
//
uint32_t filecontext_global_size (uint8_t drive, const char *path)
  {
  return filecontext_size (global_fc, drive, path); 
  }

//
// filecontext_global_fstat
//
Error filecontext_global_fstat (int fd, struct stat *sb)
  {
  return filecontext_fstat (global_fc, fd, sb); 
  }

//
// filecontext_global_ftruncate
//
Error filecontext_global_ftruncate (int fd, uint32_t length)
  {
  return filecontext_ftruncate (global_fc, fd, length); 
  }

//
// filecontext_global_rename
//
Error filecontext_global_rename (uint8_t drive, 
        const char *source_path, const char *target_path)
  {
  return filecontext_rename (global_fc, drive, source_path, target_path); 
  }

//
// filecontext_global_dstatfs
//
Error filecontext_global_dstatfs (uint8_t drive, 
                            struct _my_statfs *statfs)
  {
  return filecontext_dstatfs (global_fc, drive, statfs);
  }

//
// filecontext_global_format
//
Error filecontext_global_format (uint8_t drive_num)
  {
  return filecontext_format (global_fc, drive_num);
  }


