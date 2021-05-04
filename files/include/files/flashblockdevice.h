/*=========================================================================
 
  cpicom

  files/flashblockdevice

  An implementation of a block device in Pico flash memory

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include "error/error.h"
#include "files/blockdevice.h"

struct _FlashBlockDevice;
typedef struct _FlashBlockDevice FlashBlockDevice;

extern FlashBlockDevice   *flashblockdevice_create (uint32_t flash_start, 
                            uint32_t pages);
extern void               flashblockdevice_destroy (FlashBlockDevice *self);
extern Error              flashblockdevice_initialize (FlashBlockDevice *self);
extern Error              flashblockdevice_sync_fn (void *self, 
                             int start, int end);
extern void               flashblockdevice_get_params 
                            (FlashBlockDevice *self, 
                             BlockDeviceParams *params);


