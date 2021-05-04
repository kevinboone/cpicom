/*=========================================================================
 
  cpicom

  files/flashblockdevice.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#if PICO_ON_DEVICE

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "hardware/sync.h"
#include "hardware/flash.h"
#include "files/flashblockdevice.h"
#include "log/log.h"

#define BLOCK_SIZE 4096
// Where the flash starts in the ARM memory map (this is where the
//   executable program is)
#define FLASH_START_MEM 0x10000000

extern uint32_t __flash_binary_end;

struct _FlashBlockDevice
  {
  uint32_t flash_start;
  uint32_t pages;
  uint32_t flash_storage_start_mem;
  };

// 
// flashblockdevice_create 
//
FlashBlockDevice *flashblockdevice_create (uint32_t flash_start, 
                            uint32_t pages)
  {
  FlashBlockDevice *self = malloc (sizeof (FlashBlockDevice));
  self->flash_start = flash_start;
  self->pages = pages;
  self->flash_storage_start_mem = FLASH_START_MEM + flash_start;
  return self;
  }

// 
// flashblockdevice_destroy
//
void flashblockdevice_destroy (FlashBlockDevice *self)
  {
  if (self)
    {
    free (self);
    }
  }

// 
// flashblockdevice_initialize
//
Error flashblockdevice_initialize (FlashBlockDevice *self)
  {
  Error ret = 0;
  if (self->flash_storage_start_mem < __flash_binary_end)
    log_error ("Flash storage start is in the program area!");
  return ret;
  }

// 
// flashblockdevice_sync_fn
//
Error flashblockdevice_sync_fn (void *context, int start_block, int count)
  {
  (void)context; (void)start_block; (void)count;
  // Nothing to do here
  return 0;
  }

Error flashblockdevice_read_fn
         (void *context, uint8_t *buffer, uint32_t block, uint32_t off, 
           uint32_t size)
  {
  FlashBlockDevice *self = (FlashBlockDevice *) context;
  log_debug ("FlashBD: read %d bytes from offset %d in block %d\n", 
    (int)size, (int)off, (int)block);
  char *mem = (char *)self->flash_storage_start_mem +
      ((int)block * (int)BLOCK_SIZE) + (int)off;
  memcpy (buffer, mem, size);
  return 0;
  } 

Error flashblockdevice_write_fn
         (void *context, const uint8_t *buffer, uint32_t block, uint32_t off, 
           uint32_t size)
  {
  FlashBlockDevice *self = (FlashBlockDevice *) context;
  log_debug ("FlashBD: write %d bytes at offset %d in block %d\n", 
    (int)size, (int)off, (int)block);
  int mem = self->flash_start +
      ((int)block * (int)BLOCK_SIZE) + (int)off;
  uint32_t ints = save_and_disable_interrupts();
  flash_range_program ((uint32_t)mem, buffer, size);
  restore_interrupts (ints);
  return 0;
  }

Error flashblockdevice_erase_fn
         (void *context, uint32_t block)
  {
  FlashBlockDevice *self = (FlashBlockDevice *) context;
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase (self->flash_start + (block * BLOCK_SIZE), BLOCK_SIZE);
  restore_interrupts (ints);
  return 0;
  }

// 
// flashblockdevice_get_params
//
void flashblockdevice_get_params (FlashBlockDevice *self, 
                             BlockDeviceParams *params)
  {
  strcpy (params->desc, "Flash");
  params->context = self;
  params->block_size = BLOCK_SIZE;
  params->block_count = self->pages;
  params->sync_fn = flashblockdevice_sync_fn;
  params->read_fn = flashblockdevice_read_fn;
  params->write_fn = flashblockdevice_write_fn;
  params->erase_fn = flashblockdevice_erase_fn;
  }

#endif

