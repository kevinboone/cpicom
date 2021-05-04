/*=========================================================================
 
  cpicom

  shell/cmd_dir.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "log/log.h" 
#include "cpm/limits.h" 
#include "shell/shell.h" 
#include "console/console.h" 
#include "files/compat.h" 

Error cmd_dir_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  my_DIR *dir;
  Error ret = 0;
  const char *name; 
  if (argc > 1)
    name = argv[1];
  else
    name = ".";
  dir = my_opendir (name);
      
  // TODO limit to single file, if a filename is specified 

  if (dir)
    {
    shell_reset_linecount();
    struct my_dirent *de = my_readdir (dir);
    BOOL stop = FALSE;
    do
      {
      if (de->d_type == DT_REG_)
        {
	uint32_t size = filecontext_global_size (dir->drive, de->d_name);
	if (size == (uint32_t)-1) size = 0;
        int recs = (size + (BDOS_RECORD_SIZE - 1)) / BDOS_RECORD_SIZE;
        if (shell_writeln (console_params, 
	      "%c: %-15s %-4d %d", dir->drive + 'A', de->d_name, 
              (int)recs, (int)size) != 0) stop = TRUE;
	}
      de = my_readdir (dir);
      } while (de && !stop);
    my_closedir (dir);
    }
  else
    {
    shell_write_error_filename (errno, name, console_params);
    ret = errno;
    }

  return ret;
  }




