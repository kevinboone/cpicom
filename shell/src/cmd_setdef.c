/*=========================================================================
 
  cpicom

  shell/setdef.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "log/log.h" 
#include "shell/shell.h" 
#include "picocpm/config.h" 
#include "console/console.h" 
#include "files/compat.h" 

//width height
//
// cmd_setdef_show
//
void cmd_setdef_show (ConsoleParams *console_params, Config *config)
  {
  shell_writeln (console_params, "%s", config->page ? "PAGE" : "NOPAGE");  
  shell_writeln (console_params, "%s", 
          config->flow_control ? "FLOWCONTROL" : "NOFLOWCONTROL");  
  }

//
// cmd_setdef_do_nvp
//
Error cmd_setdef_do_nvp (ConsoleParams *console_params, char *buff,
        Config *config)
  {
  (void)console_params;
  Error ret = 0;
  string_to_upper (buff);
  if (strcmp (buff, "PAGE") == 0)
    config->page = TRUE;
  else if (strcmp (buff, "NOPAGE") == 0)
    config->page = FALSE;
  if (strcmp (buff, "FLOWCONTROL") == 0)
    config->flow_control = TRUE;
  else if (strcmp (buff, "NOFLOWCONTROL") == 0)
    config->flow_control = FALSE;
  return ret;
  }

//
// cmd_setdef_do_drives
//
Error cmd_setdef_do_drives (ConsoleParams *console_params, char *buff,
        Config *config)
  {
  (void)config;
  (void)buff;
  Error ret = ERROR_UNIMPLEMENTED;
  shell_write_error (ret, console_params);
  return ret;
  }

//
// cmd_setdef_do_arg
//
  
Error cmd_setdef_do_arg (ConsoleParams *console_params, const char *arg,
        Config *config)
  {
  Error ret = 0;
  char buff[20];
  strncpy (buff, arg, sizeof (buff) - 1);
  buff[sizeof(buff) - 1] = 0;
  if (buff[0] == '[')
     {
     char *p = strchr (buff, ']');
     if (p) *p = 0;
     ret = cmd_setdef_do_nvp (console_params, buff + 1, config);
     }
  else
     {
     ret = cmd_setdef_do_drives (console_params, buff, config);
     }
  return ret;
  }

//
// cmd_setdef_run
//
Error cmd_setdef_run (int argc, char **argv, 
     ConsoleParams *console_params, Config *config)
  {
  Error ret = 0;
  if (argc == 1)
    {
    cmd_setdef_show (console_params, config);
    }
  else
    {
    for (int i = 1; i < argc; i++)
      {
      cmd_setdef_do_arg (console_params, argv[i], config);
      }
    }
  return ret;
  }





