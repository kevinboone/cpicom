/*=========================================================================
 
  CPICOM

  shell/ysend.c

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

//
// cmd_send_usage
//
void cmd_ysend_usage (ConsoleParams *console_params)
  {
  shell_write_string (console_params, "Usage: ysend {filename}");
  shell_write_endl (console_params);
  }

// 
// cmd_ysend_run
//
Error cmd_ysend_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  Error ret = 0;

  char *filename = NULL;
  if (argc >= 1)
    {
    filename = argv[1];
    }
 
  if (filename == NULL)
    {
    cmd_ysend_usage (console_params);
    return ERROR_CMD_SYNTAX;
    }

  YmodemErr err = ymodem_send (console_params, filename); 
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






