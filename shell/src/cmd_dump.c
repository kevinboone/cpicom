/*=========================================================================
 
  cpicom

  shell/cmd_dump.c

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

Error cmd_dump_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  if (argc < 2)
    {
    shell_write_error (ERROR_ARGCOUNT, console_params);
    return ERROR_ARGCOUNT;
    }
  errno = 0;
  const char *filename = argv[1];
  int f = my_open (filename, O_RDONLY);
  if (f >= 0)
    {
    char buff[16];
    int off = 0;
    int n = 1;
    shell_reset_linecount();
    BOOL stop = FALSE;
    while (n > 0 && !stop)
      {
      shell_write_string (console_params, "%08X ", off);
      n = my_read (f, buff, sizeof (buff)); 
      if (n >= 0)
        {
        for (int i = 0; i < n; i++)
	  {
	  shell_write_string (console_params, "%02X ", (unsigned char)
	    buff[i]);
	  }
	}
      if (shell_writeln (console_params, "") != 0) stop = TRUE;
      off += n;
      }
    my_close (f);
    }
  else
    shell_write_error_filename (errno, filename, console_params);
  return errno;
  }



