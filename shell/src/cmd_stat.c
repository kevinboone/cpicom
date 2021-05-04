/*=========================================================================
 
  cpicom

  shell/cmd_stat.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "log/log.h" 
#include "shell/shell.h" 
#include "console/console.h" 
#include "files/compat.h" 

//
// cmd_stat_run
//
Error cmd_stat_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  (void)argc; (void)argv;
  Error ret = 0;
  for (int i = 0; i < FILESYSTEM_MAX_MOUNTS; i++)
    {
    my_statfs statfs;   
    int err = filecontext_global_dstatfs (i, &statfs);
    if (err == 0)
      {
      uint32_t bytes_free = (int)statfs.block_size * (int)statfs.free_blocks;
      shell_writeln (console_params, 
        "%c: R/W, Space: %dk (%d of %d blocks of size %d)", i + 'A',
        bytes_free / 1000, (int)statfs.free_blocks, (int)statfs.total_blocks,
	(int)statfs.block_size);  
      }
    }
  return ret;
  }

