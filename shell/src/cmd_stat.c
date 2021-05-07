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
#include "cpm/limits.h" 

//
// cmd_stat_do_drive
//
Error cmd_stat_do_drive (ConsoleParams *cp, const char *spec, 
              uint8_t drive, const char *path)
  {
  Error ret = 0;
  if (path == NULL || path[0] == 0)
    {
    my_statfs statfs;   
    int err = filecontext_global_dstatfs (drive, &statfs);
    if (err == 0)
      {
      uint32_t bytes_free = (int)statfs.block_size * (int)statfs.free_blocks;
      shell_writeln (cp, 
        "%c: R/W, Space: %dk (%d of %d blocks of size %d)", drive + 'A',
        bytes_free / 1000, (int)statfs.free_blocks, (int)statfs.total_blocks,
	(int)statfs.block_size);  
      }
    else
      {
      shell_write_error (err, cp);
      ret = err;
      }
    }
  else
    {
    List *list = list_create (free);
    compat_globber (spec, list);
    int l = list_length (list);

    BOOL header_done = FALSE;
    for (int i = 0; i < l; i++)
      {
      const char *file = list_get (list, i);
      struct stat sb;
      if (my_stat (file, &sb) == 0)
        {
	if (S_ISREG (sb.st_mode))
	  {
	  if (!header_done)
	    {
	    shell_writeln (cp, "Recs  Bytes  Ext Acc");
	    header_done = TRUE;
	    }
	  uint32_t size = sb.st_size;
          int recs = (size + (BDOS_RECORD_SIZE - 1)) / BDOS_RECORD_SIZE;
          int extents = (size + (BDOS_EXTENT_SIZE - 1)) / BDOS_EXTENT_SIZE;
	  int kb = size / 1024;
	  shell_writeln (cp, "%4d%6dk%5d R/W %s", recs, kb, extents, file);
	  }
	}
      }

    list_destroy (list);
    cmd_stat_do_drive (cp, NULL, drive, NULL);
    }
  return ret;
  }

//
// cmd_stat_run
//
Error cmd_stat_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  Error ret = 0;
  if (argc == 1)
    {
    for (int i = 0; i < FILESYSTEM_MAX_MOUNTS; i++)
      {
      ret = filecontext_global_activate_drive (i);
      if (ret == 0)
        ret = cmd_stat_do_drive (console_params, NULL, (uint8_t) i, NULL);
      }
    }
  else
    {
    const char *spec = argv[1];
    int drive = compat_get_drive (spec); 
    const char *path = compat_get_path (spec);

    ret = cmd_stat_do_drive (console_params, spec, (uint8_t) drive, path);
    }
  return ret;
  }

