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

  ConsoleProperties cprops;
  console_params->console_get_properties (console_params->context, &cprops);
  int space = 14; // 8 + 3 + separators
  int num_across = cprops.width / space;
  if (num_across == 0) num_across = 1;

  dir = my_opendir (name);
      
  // TODO limit to single file, if a filename is specified 

  if (dir)
    {
    shell_reset_linecount();
    struct my_dirent *de = my_readdir (dir);
    BOOL stop = FALSE;
    int across = 0;
    do
      {
      if (de->d_type == DT_REG_)
        {
	shell_write_string (console_params, "%14s", de->d_name);
	if (across == num_across - 1)
	  {
	  shell_writeln (console_params, "");
	  across = 0;
	  }
	across++;
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




