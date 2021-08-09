/*=========================================================================

  cpicom

  files/filesystemfat.h

  Specification for a wrapper for the FatFS filesystem implementation.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include <stdint.h>
#include "blockdevice/blockdevice.h"
#include "filesystem.h"

struct _FilesystemFat;
typedef struct _FilesystemFat FilesystemFat;

extern FilesystemFat *filesystemfat_create (int ffvol);
extern void           filesystemfat_destroy (FilesystemFat *self);

extern void           filesystemfat_set_block_device (FilesystemFat *self, 
                           BlockDeviceParams *bd_params);

extern void           filesystemfat_get_params (FilesystemFat *self, 
                           FilesystemParams *fs_params);

