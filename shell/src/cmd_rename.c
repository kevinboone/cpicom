/*=========================================================================
 
  cpicom

  shell/cmd_rename.c

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
// cmd_rename_source_target
//
Error cmd_rename_source_target (ConsoleParams *cp, const char *source, 
        const char *target)
  {
  Error ret = 0;
  log_debug ("cmd_ren_src_tgt: src=%s tgt=%s\n", source, target); 
  if (MYRENAME (source, target) != 0)
    {
    shell_writeln (cp, "Can't rename %s to %s: %s", source, target, 
            error_strerror (errno));
    ret = errno;
    }
  return ret;
  }

//
// cmd_rename_source_target_spec
//
Error cmd_rename_source_target_spec (ConsoleParams *cp, const char *source, 
        const char *target)
  {
  // TODO process wildcards here
  return cmd_rename_source_target (cp, source, target); 
  }

//
// cmd_rename_run
//
Error cmd_rename_prompt (ConsoleParams *cp)
  {
  Error ret = 0;

  char source[BDOS_MAX_FNAME + 1];
  char target[BDOS_MAX_FNAME + 1];
  shell_write_string (cp, "Enter new name: ");
  ret = cp->console_get_line (cp->context, target, BDOS_MAX_FNAME, 0, NULL);
  if (ret == 0 && target[0] != 0)
    {
    shell_write_string (cp, "Enter old name: ");
    //shell_writeln (cp, "");
    ret = cp->console_get_line (cp->context, source, BDOS_MAX_FNAME, 0, NULL);
    if (ret == 0 && source[0] != 0)
      {
      ret = cmd_rename_source_target_spec (cp, source, target);
      } 
    } 

  return ret;
  }

//
// cmd_rename_run
//
Error cmd_rename_do_spec (ConsoleParams *cp, const char *spec)
  {
  Error ret = 0;
  
  char buff[BDOS_MAX_FNAME * 2 + 2]; 
  strncpy (buff, spec, BDOS_MAX_FNAME * 2);
  buff[sizeof(buff) - 1] = 0;
  
  char *eq = strchr (buff, '=');
  if (eq)
    {
    *eq = 0;
    const char *target = buff;
    const char *source = eq + 1;
    if (strlen (source) > 0 && strlen (target) > 0)
      {
      ret = cmd_rename_source_target_spec (cp, source, target);
      }
    else
      {
      ret = ERROR_CMD_SYNTAX;
      shell_write_error_filename (ret, spec, cp);
      }
    }
  else
    {
    ret = ERROR_CMD_SYNTAX;
    shell_write_error_filename (ret, spec, cp);
    } 

  return ret;
  }

//
// cmd_rename_run
//
Error cmd_rename_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  Error ret = 0;
  if (argc == 1)
    {
    ret = cmd_rename_prompt (console_params); 
    }
  else
    {
    for (int i = 1; i < argc && ret == 0; i++)
      ret = cmd_rename_do_spec (console_params, argv[i]); 
    }
  return ret;
  }





