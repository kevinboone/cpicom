/*=========================================================================
 
  cpicom

  files/filesystemmount.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "error/error.h"
#include "files/blockdevice.h"
#include "files/filesystemmount.h"
#include "files/filecontext.h"

struct _FilesystemMount 
  {
  BlockDeviceParams bd_params; 
  FilesystemParams fs_params; 
  }; 

// 
// filesystemmount_create 
//
FilesystemMount *filesystemmount_create 
                 (const FilesystemParams *fs_params, 
                  const BlockDeviceParams *bd_params)
  {
  FilesystemMount *self = malloc (sizeof (FilesystemMount));
  if (self) 
    {
    memcpy (&self->fs_params, fs_params, sizeof (FilesystemParams));
    memcpy (&self->bd_params, bd_params, sizeof (BlockDeviceParams));
    }
  return self;
  }

// 
// filesystemmount_destroy
//
void filesystemmount_destroy (FilesystemMount *self)
  {
  if (self)
    {
    free (self);
    }
  }

// 
// filesystemmount_unmount
//
Error filesystemmount_unmount (FilesystemMount *self)
  {
  Error ret = 0;
  ret = self->fs_params.unmount (&self->fs_params, &self->bd_params);
  return ret;
  }

// 
// filesystemmount_mount
//
Error filesystemmount_mount (FilesystemMount *self)
  {
  Error ret = 0;
  ret = self->fs_params.mount (&self->fs_params, &self->bd_params);
  return ret;
  }

// 
// filesystemmount_format
//
Error filesystemmount_format (FilesystemMount *self)
  {
  Error ret = 0;
  ret = self->fs_params.format (&self->fs_params, &self->bd_params);
  return ret;
  }

// 
// filesystemmount_open
//
Error filesystemmount_open (FilesystemMount *self, const char *name, int flags,
        OpenFileInfo *ofi)
  {
  Error ret = 0;

  ret = self->fs_params.open (&self->fs_params, &self->bd_params, 
          name, flags, ofi);
  if (ret == 0)
    {
    ofi->mount = self;
    }

  return ret;
  }
  
// 
// filesystemmount_close
//
Error filesystemmount_close (FilesystemMount *self, 
          struct _OpenFileInfo *ofi)
  {
  return self->fs_params.close (&self->fs_params, &self->bd_params, ofi);
  }

// 
// filesystemmount_write
//
Error filesystemmount_write (FilesystemMount *self, 
          struct _OpenFileInfo *ofi, const uint8_t *buff, int n, 
	  uint32_t *written)
  {
  return self->fs_params.write (&self->fs_params, &self->bd_params, ofi,
    buff, n, written);
  }

// 
// filesystemmount_read
//
Error filesystemmount_read (FilesystemMount *self, 
          struct _OpenFileInfo *ofi, uint8_t *buff, int n, 
	  uint32_t *read)
  {
  return self->fs_params.read (&self->fs_params, &self->bd_params, ofi,
    buff, n, read);
  }

// 
// filesystemmount_delete
//
Error filesystemmount_delete (FilesystemMount *self, 
        const char *path)
  {
  return self->fs_params.del (&self->fs_params, &self->bd_params, path);
  }

// 
// filesystemmount_opendir
//
my_DIR *filesystemmount_opendir (FilesystemMount *self, const char *path)
  {
  return self->fs_params.opendir (&self->fs_params, &self->bd_params, path);
  }

// 
// filesystemmount_closedir
//
Error filesystemmount_closedir (FilesystemMount *self, my_DIR *dir)
  {
  return self->fs_params.closedir (&self->fs_params, &self->bd_params, dir);
  }

// 
// filesystemmount_readdir
//
struct my_dirent *filesystemmount_readdir (FilesystemMount *self, 
          struct _my_DIR *dir)
  {
  return self->fs_params.readdir (&self->fs_params, &self->bd_params, dir);
  }

// 
// filesystemmount_lseek
//
uint32_t filesystemmount_lseek (FilesystemMount *self, 
    struct _OpenFileInfo *ofi, uint32_t offset, int whence) 
  {
  return self->fs_params.lseek (&self->fs_params, &self->bd_params, ofi,
         offset, whence);
  }

// 
// filesystemmount_size
//
uint32_t filesystemmount_size (FilesystemMount *self, const char *path)
  {
  return self->fs_params.size (&self->fs_params, &self->bd_params, path); 
  }

// 
// filesystemmount_stat
//
Error filesystemmount_stat (FilesystemMount *self, 
          const char *path, struct stat *sb)
  {
  return self->fs_params.stat (&self->fs_params, 
           &self->bd_params, path, sb); 
  }

// 
// filesystemmount_fstat
//
Error filesystemmount_fstat (FilesystemMount *self, 
          const struct _OpenFileInfo *ofi, struct stat *sb)
  {
  return filesystemmount_stat (self, ofi->path, sb); 
  }


// 
// filesystemmount_ftruncate
//
extern Error filesystemmount_ftruncate (FilesystemMount *self, 
          struct _OpenFileInfo *ofi, uint32_t length)
  {
  return self->fs_params.ftruncate (&self->fs_params, 
           &self->bd_params, ofi, length); 
  }

// 
// filesystemmount_rename
//
Error filesystemmount_rename (FilesystemMount *self, 
          const char *source, const char *target)
  {
  return self->fs_params.rename (&self->fs_params, 
           &self->bd_params, source, target); 
  }
         
// 
// filesystemmount_statfs
//
extern Error filesystemmount_statfs (const FilesystemMount *self, 
          struct _my_statfs *statfs)
  {
  return self->fs_params.statfs (&self->fs_params, 
           &self->bd_params, statfs);
  }

