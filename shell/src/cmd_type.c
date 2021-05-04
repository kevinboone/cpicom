/*=========================================================================
 
  cpicom

  shell/type.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "shell/shell.h" 
#include "console/console.h" 
#include "files/compat.h" 

Error cmd_type_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  if (argc < 2)
    {
    shell_write_error (ERROR_ARGCOUNT, console_params);
    return ERROR_ARGCOUNT;
    }

  int f = my_open (argv[1], O_RDONLY);
  if (f >= 0)
    {
    char buff[257];
    int n = 1;
    while (n > 0)
      {
      n = my_read (f, buff, sizeof (buff) - 1); 
      if (n >= 0)
        {
        buff[n] = 0;
        // In CP/M, ascii files that don't occupy a fixed number of
        //   128-byte records are padded with ctrl-z's. In general,
        //   there's no way to know the true file length, other than 
        //   to the nearest block.
        char *zpos = strchr (buff, 26); // Truncate at ctrl-z
        if (zpos) *zpos = 0;
	shell_write_string (console_params, "%s", buff);
	}
      }
    my_close (f);
    }
  else
    shell_write_error_filename (errno, argv[1], console_params);
  return 0;
  }

