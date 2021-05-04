/*=========================================================================
 
  cpicom

  files/blockdevice.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "files/fileblockdevice.h"
#include "log/log.h"

#if PICO_ON_DEVICE
#else

#define BLOCK_SIZE 256

struct _FileBlockDevice
  {
  char *filename;
  int fd;
  int size;
  };

// 
// fileblockdevice_create 
//
FileBlockDevice *fileblockdevice_create (const char *filename)
  {
  FileBlockDevice *self = malloc (sizeof (FileBlockDevice));
  self->filename = strdup (filename);
  self->fd = -1;
  return self;
  }

// 
// fileblockdevice_destroy
//
void fileblockdevice_destroy (FileBlockDevice *self)
  {
  if (self)
    {
    if (self->fd >= 0) close (self->fd);
    if (self->filename) free (self->filename);
    free (self);
    }
  }

// 
// fileblockdevice_initialize
//
Error fileblockdevice_initialize (FileBlockDevice *self)
  {
  Error ret = 0;

  self->fd = open (self->filename, O_RDWR);
  if (self->fd >= 0)
    {
    int l = lseek (self->fd, 0, SEEK_END);
    lseek (self->fd, 0, SEEK_SET);
    self->size = l;
    log_debug ("Block file '%s' has size %d", self->filename, 
      self->size);
    }
  else
    {
    log_warn ("Can't initialize block file '%s': %s", self->filename, 
      strerror (errno));
    ret = errno;
    }
  return ret;
  }

// 
// fileblockdevice_sync_fn
//
Error fileblockdevice_sync_fn (void *context, int start_block, int count)
  {
  (void)context; (void)start_block; (void)count;
  sync();
  return 0;
  }

Error fileblockdevice_read_fn
         (void *context, uint8_t *buffer, uint32_t block, uint32_t off, 
           uint32_t size)
  {
  FileBlockDevice *self = (FileBlockDevice *) context;
  log_debug ("FileBD: read %d bytes from offset %d in block %d", 
        (int)size, (int)off, (int)block);
  lseek (self->fd, (off_t) block * BLOCK_SIZE + (off_t)off, SEEK_SET);

  errno = 0;
  read (self->fd, buffer, (size_t)size);
  return errno;
  } 

Error fileblockdevice_write_fn
         (void *context, const uint8_t *buffer, uint32_t block, uint32_t off, 
           uint32_t size)
  {
  FileBlockDevice *self = (FileBlockDevice *) context;
  log_debug ("FileBD: write %d bytes at offset %d in block %d", 
    (int)size, (int)off, (int)block);
  lseek (self->fd, (off_t) block * BLOCK_SIZE + (off_t)off, SEEK_SET);

  errno = 0;
  write (self->fd, buffer, (size_t)size);
  return errno;
  }

Error fileblockdevice_erase_fn
         (void *context, uint32_t block)
  {
  (void)context; (void)block;
  // Not necessary
  return 0;
  }

// 
// fileblockdevice_get_params
//
void fileblockdevice_get_params (FileBlockDevice *self, 
                             BlockDeviceParams *params)
  {
  strcpy (params->desc, "Local file");
  params->context = self;
  params->block_size = BLOCK_SIZE;
  params->block_count = self->size / params->block_size;
  params->sync_fn = fileblockdevice_sync_fn;
  params->read_fn = fileblockdevice_read_fn;
  params->write_fn = fileblockdevice_write_fn;
  params->erase_fn = fileblockdevice_erase_fn;
  }

#endif

