/*=========================================================================

  cpicom

  files/filesystemlfs.h

  Specification for a wrapper for the LitleFS filesystem implementation.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include <stdint.h>
#include "blockdevice.h"
#include "filesystem.h"

struct _FilesystemLfs;
typedef struct _FilesystemLfs FilesystemLfs;

extern FilesystemLfs *filesystemlfs_create (void);
extern void           filesystemlfs_destroy (FilesystemLfs *self);

extern void           filesystemlfs_set_block_device (FilesystemLfs *self, 
                           BlockDeviceParams *bd_params);

extern void           filesystemlfs_get_params (FilesystemLfs *self, 
                           FilesystemParams *fs_params);


