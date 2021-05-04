/*=========================================================================
 
  cpicom

  files/blockdevice.h

  Defines the function pointers for all the block device operations,
  and the BlockDevice structure that is filled in by a specific
  device handler.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include <stdint.h> 
#include "files/limits.h"
#include "error/error.h"

typedef Error (*blockdevice_sync_fn) 
         (void *context, int start_block, int count);
typedef Error (*blockdevice_read_fn)
         (void *context, uint8_t *buffer, uint32_t block, uint32_t off,  
           uint32_t size);
typedef Error (*blockdevice_write_fn)
         (void *context, const uint8_t *buffer, uint32_t block, uint32_t off, 
           uint32_t size);
typedef Error (*blockdevice_erase_fn)
         (void *context, uint32_t block);

typedef struct _BlockDeviceParams
  {
  char desc [BLOCKDEVICEPARAMS_MAX_DESC + 1];
  uint32_t block_size;
  uint32_t block_count;
  blockdevice_sync_fn sync_fn;
  blockdevice_read_fn read_fn;
  blockdevice_write_fn write_fn;
  blockdevice_erase_fn erase_fn;
  void *context;
  } BlockDeviceParams;

