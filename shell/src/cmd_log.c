/*=========================================================================
 
  cpicom

  shell/cmd_log.c

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

Error cmd_log_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  if (argc < 2)
    {
    shell_write_error (ERROR_ARGCOUNT, console_params);
    }
  int l = atoi (argv[1]);
  log_set_level (l);
  return 0;
  }


