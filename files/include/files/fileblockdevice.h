/*=========================================================================
 
  cpicom

  fileblockdevice.h

  Specification of the functions provided by the file block device
  handler. These functions satisfy the contract set out in
  blockdevice.h

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include "error/error.h"
#include "files/blockdevice.h"

struct _FileBlockDevice;
typedef struct _FileBlockDevice FileBlockDevice;

extern FileBlockDevice   *fileblockdevice_create (const char *filename);
extern void               fileblockdevice_destroy (FileBlockDevice *self);
extern Error              fileblockdevice_initialize (FileBlockDevice *self);
extern Error              fileblockdevice_sync_fn (void *self, 
                             int start, int end);
extern void               fileblockdevice_get_params 
                            (FileBlockDevice *self, 
                             BlockDeviceParams *params);


