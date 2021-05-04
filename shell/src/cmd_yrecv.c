/*=========================================================================
 
  CPICOM

  shell/yrecv.c

  Copyright (c)2021 Kevin Boone, GPL v3.0

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
#include "ymodem/ymodem.h" 

// Largest file we are prepared to accept (essentially, large enough
// to fill a 720k floppy).
#define YMODEM_MAX 699904

//
// cmd_yrecv_usage
//
void cmd_yrecv_usage (ConsoleParams *console_params)
  {
  shell_write_string (console_params, "Usage: yrecv [filename]");
  shell_write_endl (console_params);
  }

// 
// cmd_yrecv_run
//
Error cmd_yrecv_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  Error ret = 0;
  // TODO options
/*
  if (argc > 1)
    dir = my_opendir (argv[1]);
  else
    dir = my_opendir ("");
*/      

  char *filename = NULL;
  if (argc >= 1)
    {
    filename = argv[1];
    }

  YmodemErr err = ymodem_receive (console_params, filename, YMODEM_MAX); 
  if (err != 0)
    {
    if (filename)
      shell_writeln (console_params, "YModem transfer failed: %s", 
         ymodem_strerror (err));
    else
      shell_writeln (console_params, 
        "YModem transfer failed for file %s: %s", filename,
	ymodem_strerror (err));
    ret = ERROR_YMODEM;
    }

  return ret;
  }





