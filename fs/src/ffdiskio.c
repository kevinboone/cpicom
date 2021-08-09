/*=========================================================================
 
  cpicom

  fs/ffdiskio.c 

  Plumbing for the FAT filesystem driver, which expects to be able
  to call functions disk_read(), disk_ioctl(), etc.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "fs/ff.h"		
#include "fs/ffdiskio.h"	
#include "log/log.h"	

// 
// disk_status
//
DSTATUS disk_status (BYTE pdrv)
  {
  (void)pdrv;
  // At present we don't have a way to check the underlying drive
  //   status -- it is always ready.
  return 0;
  }

// 
// disk_initialize
//
DSTATUS disk_initialize (BYTE pdrv)
  {
  (void)pdrv;
  // At present we have nothing to initialize
  return 0;
  }

//
// disk_get_block_and_offset
//
static void disk_get_block_and_offset (BlockDeviceParams *bd_params, 
              LBA_t sector, int *block, int *offset)
  {
  if (FF_MIN_SS >= bd_params->block_size)
    {
    int bps = FF_MIN_SS / bd_params->block_size;  
    *block = bps * sector;
    *offset = 0;
    }
  else
    {
    // Example: my sector size = 512, disk block size = 4096
    //  We need sector 10. That's in block 01, two sector length in
    // Get disk block by dividing by blocks per sector -- 10 / 8 = 1;
    // To get the offset in sectors -- 10 % 8 = 2; 
    int spb = bd_params->block_size / FF_MIN_SS;  
    *block = sector / spb;
    *offset = (sector % spb) * FF_MIN_SS; 
    }
  }
               

//
// disk_read
//
DRESULT disk_read (BYTE pdrv,	BYTE *buff, LBA_t sector, UINT count)
  {
  BlockDeviceParams *bd_params = &bdp[pdrv];
  int block; int offset;
  disk_get_block_and_offset (bd_params, sector, &block, &offset);
  Error ret = bd_params->read_fn (bd_params->context, buff, block, offset, 
       count * FF_MIN_SS);

  return (ret ? 1 : 0); // TODO convert return to a DRESULT
  }


//
// disk_write
//
DRESULT disk_write (BYTE pdrv,	const BYTE *buff, LBA_t sector,	UINT count)
  {
  BlockDeviceParams *bd_params = &bdp[pdrv];
  int block; int offset;
  disk_get_block_and_offset (bd_params, sector, &block, &offset);
  Error ret = bd_params->write_fn (bd_params->context, buff, block, offset, 
       count * FF_MIN_SS);
  return (ret ? 1 : 0); // TODO convert return to a DRESULT
  }


//
// disk_ioctl
//
DRESULT disk_ioctl (BYTE pdrv,	BYTE cmd, void *buff)
  {
  (void)pdrv;
  (void)buff;
  switch (cmd)
    {
    case CTRL_SYNC:
      {
      BlockDeviceParams *bd_params = &bdp[pdrv];
      bd_params->sync_fn (bd_params->context, 0, 0); 
      return 0;
      }
    
    case GET_SECTOR_COUNT:
      {
      BlockDeviceParams *bd_params = &bdp[pdrv];
      uint32_t blocks = bd_params->block_count;

      log_debug ("disk_ioctl block count = %d", blocks);
      uint32_t sectors; 
      if (bd_params->block_size >= FF_MIN_SS)
        sectors = blocks * FF_MIN_SS / bd_params->block_size; 
      else
        sectors = blocks * bd_params->block_size / FF_MIN_SS; 

      uint32_t *r = (uint32_t*)buff;
      *r = sectors;

      log_debug ("disk_ioctl returning %d sectors", *r);

      return 0;
      }
    
    default:
      printf ("ioctl unhandled cmd %d\n", cmd);
      return RES_PARERR;
    }
  }

//
// disk_fattime
//
// We have no way to get a time -- return zero always
DWORD get_fattime (void)
  {
  return 0;
  }

