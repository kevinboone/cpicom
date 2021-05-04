/*=========================================================================
 
  cpicom

  files/filesystemmount.h

  A FilesystemMount is a tuple of the drive number, the filesystem handler
  for the drive, and the underlying block device. The FileContext
  object maintains a table of these mounts.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include <sys/types.h> 
#include <sys/stat.h> 
#include "files/blockdevice.h"
#include "files/filesystem.h"
#include "files/filecontext.h"

// Number of filesystems that can be mounted (A: to P:)
#define FILESYSTEM_MAX_MOUNTS 16

struct _OpenFileInfo;
struct _FilesystemMount;
struct _FilesystemParams;
struct _my_statfs;
struct _my_DIR;

typedef struct _FilesystemMount FilesystemMount;

extern FilesystemMount *filesystemmount_create 
                 (const struct _FilesystemParams *fs_params, 
                  const BlockDeviceParams *bd_params);
extern void filesystemmount_destroy (FilesystemMount *self);

extern Error filesystemmount_mount (FilesystemMount *self);

/** Unmount a filesystem. It is envisaged that this operation simply
    flushes outstanding operations -- it does not physically eject
    harware or free any resources. Although it can return an error code, 
    the only likely reasons for failure are that the medium has been removed,
    or the filesystem is corrupt. */
extern Error filesystemmount_unmount (FilesystemMount *self); 

extern Error filesystemmount_format (FilesystemMount *self);

extern Error filesystemmount_open (FilesystemMount *mount, 
          const char *name, int flags, struct _OpenFileInfo *ofi);

extern Error filesystemmount_close (FilesystemMount *mount, 
          struct _OpenFileInfo *ofi);

extern Error filesystemmount_write (FilesystemMount *mount, 
          struct _OpenFileInfo *ofi, const uint8_t *buff, int n, 
	  uint32_t *written);

extern Error filesystemmount_read (FilesystemMount *mount, 
          struct _OpenFileInfo *ofi, uint8_t *buff, int n, 
	  uint32_t *read);

extern Error filesystemmount_delete (FilesystemMount *self, 
          const char *path);

extern struct _my_DIR *filesystemmount_opendir (FilesystemMount *self, 
          const char *path);

extern Error filesystemmount_closedir (FilesystemMount *self, 
          struct _my_DIR *dir);

extern struct my_dirent *filesystemmount_readdir (FilesystemMount *self, 
          struct _my_DIR *dir);

extern uint32_t filesystemmount_lseek (FilesystemMount *self, 
          struct _OpenFileInfo *ofi, uint32_t offset, int whence);

extern uint32_t filesystemmount_size (FilesystemMount *self, 
          const char *path);

extern Error filesystemmount_fstat (FilesystemMount *self, 
          const struct _OpenFileInfo *ofi, struct stat *sb);

extern Error filesystemmount_stat (FilesystemMount *self, 
          const char *path, struct stat *sb);

extern Error filesystemmount_ftruncate (FilesystemMount *self, 
          struct _OpenFileInfo *ofi, uint32_t length);
          

extern Error filesystemmount_rename (FilesystemMount *self, 
          const char *source, const char *target); 
          
extern Error filesystemmount_statfs (const FilesystemMount *self, 
          struct _my_statfs *statfs);
          

