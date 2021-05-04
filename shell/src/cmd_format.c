/*=========================================================================
 
  cpicom

  shell/cmd_format.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "error/error.h" 
#include "log/log.h" 
#include "shell/shell.h" 
#include "console/console.h" 
#include "files/compat.h" 

//
// cmd_format_run
//
Error cmd_format_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  Error ret = 0;
  int drive_num;
  if (argc >= 2)
    drive_num = toupper (argv[1][0]) - 'A';
  else
    drive_num = filecontext_global_get_current_drive ();
  
  if (drive_num < FILESYSTEM_MAX_MOUNTS)
    {
    char drive_letter = drive_num + 'A';
    shell_write_string (console_params, "Delete all data on drive %c (y/n)? ",
      drive_letter);
    char buff[4];
    console_params->console_get_line 
       (console_params->context, buff, 3, 0, NULL);
    if (buff[0] == 'y' || buff[1] == 'Y')
      {
      ret = filecontext_global_format (drive_num);
      if (ret)
        {
        shell_write_error (ret, console_params);
        } 
      } 
    }
  else
    {
    ret = ERROR_BADDRIVELETTER;
    shell_write_error (ret, console_params);
    } 
  return ret;
  }


