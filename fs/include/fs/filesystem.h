/*=========================================================================

  cpicom

  files/filesystem.h

  Typedefs for all the functions that a specific filesystem handler
  must implement, and the FilesystemParams structure in which they
  are stored.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include "error/error.h"
#include "fs/filesystem.h"
#include "blockdevice/blockdevice.h"
#include "files/filecontext.h"

struct _FilesystemParams;
struct _OpenFileInfo;
struct _my_DIR;
struct _my_statfs;

typedef Error (*filesystem_open_fn) (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, const char *name, int flags, 
          struct _OpenFileInfo *ofi);

typedef Error (*filesystem_write_fn) (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, struct _OpenFileInfo *ofi,
	  const uint8_t*buffer, uint32_t len, uint32_t *written);

typedef Error (*filesystem_read_fn) (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, struct _OpenFileInfo *ofi,
	  uint8_t*buffer, uint32_t len, uint32_t *read);

typedef Error (*filesystem_close_fn) (struct _FilesystemParams *fs_params, 
          BlockDeviceParams *bd_params, struct _OpenFileInfo *ofi);

typedef Error (*filesystem_mount_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params);

typedef Error (*filesystem_unmount_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params);

typedef Error (*filesystem_format_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params);

typedef Error (*filesystem_stat_fn) 
          (struct _FilesystemParams *fs_params, 
	   BlockDeviceParams *bd_params, const char *path, struct stat *sb);

typedef Error (*filesystem_delete_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           const char *filename);

typedef struct _my_DIR *(*filesystem_opendir_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           const char *path);

typedef Error (*filesystem_closedir_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _my_DIR *dir);

typedef struct my_dirent *(*filesystem_readdir_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _my_DIR *dir);

typedef uint32_t (*filesystem_lseek_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _OpenFileInfo *ofi, uint32_t offset, int whence);

typedef Error (*filesystem_ftruncate_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           struct _OpenFileInfo *ofi, uint32_t length); 

typedef uint32_t (*filesystem_size_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           const char *path);

typedef Error (*filesystem_rename_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
           const char *source, const char *target);

typedef Error (*filesystem_statfs_fn) 
          (struct _FilesystemParams *fs_params, BlockDeviceParams *bd_params,
            struct _my_statfs *statfs); 

typedef struct _FilesystemParams
  {
  char desc [FILESYSTEMPARAMS_MAX_DESC + 1];
  filesystem_mount_fn mount;
  filesystem_unmount_fn unmount;
  filesystem_format_fn format;
  filesystem_open_fn open;
  filesystem_close_fn close;
  filesystem_write_fn write;
  filesystem_read_fn read;
  filesystem_delete_fn del;
  filesystem_opendir_fn opendir;
  filesystem_closedir_fn closedir;
  filesystem_readdir_fn readdir;
  filesystem_lseek_fn lseek;
  filesystem_size_fn size;
  filesystem_stat_fn stat;
  filesystem_ftruncate_fn ftruncate;
  filesystem_rename_fn rename;
  filesystem_statfs_fn statfs;
  void *context;
  } FilesystemParams;

