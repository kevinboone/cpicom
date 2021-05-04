/*=========================================================================
 
  cpicom

  shell/era.c

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
// cmd_era_erase_filespec
//
Error cmd_era_erase_filespec (const char *fullpath,
     ConsoleParams *console_params)
  {
  List *list = list_create (free);
  compat_globber (fullpath, list);
  int l = list_length (list);
  for (int i = 0; i < l; i++)
    {
    const char *file = list_get (list, i);
    if (my_unlink (file) != 0)
      {
      shell_write_error_filename (errno, file, console_params); 
      }
    }
  list_destroy (list);
  return 0;
  }

// 
// cmd_era_erase_run
//
Error cmd_era_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  for (int i = 1; i < argc; i++)
    {
    const char *filespec = argv[i];
    cmd_era_erase_filespec (filespec, console_params);
    // TODO
    }
  return 0;
  }



