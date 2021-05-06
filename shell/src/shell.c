/*=========================================================================
 
  cpicom

  shell/shell.c

  This is the implementation of the main command processor, along with
  various utility functions that specific shell commands may use.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "error/error.h" 
#include "shell/shell.h" 
#include "klib/string.h" 
#include "console/console.h" 
#include "klib/list.h" 
#include "files/filecontext.h" 
#include "files/compat.h" 
#include "picocpm/config.h" 
#include "picocpm/picocpm.h" 

int shell_lines = 0;
int shell_max_lines = 24; // TODO - get from console

// Storing this here is ugly but, what the heck, there's only going to
//  be a single shell.
static Config *config;

//
// shell_reset_linecount
// 
void shell_reset_linecount (void)
  {
  shell_lines = 0;
  }

//
// shell_writeln
//
Error shell_writeln (ConsoleParams *console_params, 
    const char *fmt,...)
  {
  Error ret = 0;
  va_list ap;
  va_start (ap, fmt);
  console_params->console_out_string_v (console_params->context, fmt, ap);
  console_params->console_out_endl (console_params->context);
  va_end (ap);
  if (config->page) 
    {
    shell_lines++;
    if (shell_lines >= shell_max_lines - 1)
      {
      console_params->console_out_string (console_params->context, 
        "[Press any key...]");
      char c = console_params->console_get_char_timeout 
                  (console_params->context, -1); 
      if (c == 0x03) // interrupt. Nasty, nasty, ugh. Ewww...
        ret = ERROR_INTERRUPTED; 
      console_params->console_out_endl (console_params->context);
      shell_lines = 0;
      }
    }
  return ret;
  }

//
// shell_write_string_v
//
void shell_write_string (ConsoleParams *console_params, 
    const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  console_params->console_out_string_v (console_params->context, fmt, ap);
  va_end (ap);
  }

//
// shell_write_end
//
void shell_write_endl (ConsoleParams *console_params)
  {
  console_params->console_out_endl (console_params->context);
  }

//
// shell_write_error
//
void shell_write_error (Error error, ConsoleParams *console_params)
  {
  // TODO
  const char *msg = error_strerror (error);
  if (msg)
    console_params->console_out_string (console_params->context, msg);
  else
    console_params->console_out_string (console_params->context, 
       "Error %d", error);
  console_params->console_out_endl (console_params->context);
  }

//
// shell_write_error_filename
//
void shell_write_error_filename (Error error, const char *filename, 
                                   ConsoleParams *console_params)
  {
  // TODO
  console_params->console_out_string (console_params, "%s: ", filename);
  shell_write_error (error, console_params);
  }

//
// shell_find_executable_on_drive
//
static BOOL shell_find_executable_on_drive (char drive_letter, 
        const char *name83, char *ext83, char *exe_path)
  {
  char try_name[21];

  try_name[0] = drive_letter;
  try_name[1] = ':';
  try_name[2] = 0;
 
  if (ext83[0])
    {
    strcat (try_name, name83);
    strcat (try_name, ".");
    strcat (try_name, ext83);
    }
  else
    {
    // TODO check .sub
    strcat (try_name, name83);
    strcat (try_name, ".");
    strcat (try_name, "COM");
    }

  int fd = my_open (try_name, O_RDONLY);
  if (fd >= 0)
    {
    strcpy (exe_path, try_name);
    my_close (fd);
    return TRUE;
    }

  return FALSE;
  }

//
// Search for an executable (com or sub [not yet implemented])
//   that matches the name. The name may include a drive, in which
//   case the text is a simple match. name may be in any case, but
//   the match is case-insensitve, and the result will be
//   upper-case.
BOOL shell_find_executable (const char *name, char *exe_path)
  {
  BOOL ret = FALSE;

  char name83[9];
  char ext83[4];
  char drive_letter;
  compat_split_drive_name_ext_83 (name, &drive_letter, name83, ext83);

  if (drive_letter)
    {
    return shell_find_executable_on_drive (drive_letter, 
       name83, ext83, exe_path);
    }
  else
    {
    // TODO search path
    return shell_find_executable_on_drive ('A', name83, ext83, exe_path);
    }

  return ret;
  }

//
// shell_do_line_argv
//
Error shell_do_line_argv (int argc, char **argv, 
     ConsoleParams *console_params, PicoCPM *picocpm)
  {
  char exe_path [20]; // TODO
  Error ret = 0;
  if (argc == 0) return 0;

  if (strlen(argv[0]) == 2 && argv[0][1] == ':')
    {
    filecontext_global_set_current_drive (toupper(argv[0][0]) - 'A');
    }
  else if (strcasecmp (argv[0], "foo") == 0)
    {
    shell_reset_linecount ();
    for (int i = 0; i < 50; i++)
      {
      shell_writeln (console_params, "line %d", i);
      }
    }
  else if (strncasecmp (argv[0], "ren", 3) == 0)
    {
    ret = cmd_rename_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "typ", 3) == 0)
    {
    ret = cmd_type_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "era", 3) == 0)
    {
    ret = cmd_era_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "dir", 3) == 0)
    {
    ret = cmd_dir_run (argc, argv, console_params);
    }
  else if (strcasecmp (argv[0], "log") == 0)
    {
    ret = cmd_log_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "yrecv", 3) == 0)
    {
    ret = cmd_yrecv_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "ysend", 3) == 0)
    {
    ret = cmd_ysend_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "dump", 3) == 0)
    {
    ret = cmd_dump_run (argc, argv, console_params);
    }
  else if (strncasecmp (argv[0], "stat", 3) == 0)
    {
    ret = cmd_stat_run (argc, argv, console_params);
    }
  else if (strcasecmp (argv[0], "format") == 0)
    {
    ret = cmd_format_run (argc, argv, console_params);
    }
  else if (strcasecmp (argv[0], "setdef") == 0)
    {
    ret = cmd_setdef_run (argc, argv, console_params, config);
    }
  else if (shell_find_executable (argv[0], exe_path))
    {
    picocpm_run (picocpm, exe_path, argc, argv);
    ret = 0;
    }
  else
    {
    shell_write_string (console_params, argv[0]);
    shell_write_string (console_params, "?");
    shell_write_endl (console_params);
    }

  return ret;
  }


//
// shell_do_line
//
Error shell_do_line (const char *buff, ConsoleParams *console_params,
        PicoCPM *picocpm)
  {
  config = picocpm_get_config (picocpm);
  if (buff[0] == 0) return 0;

  Error ret = 0;
  String *sbuff = string_create (buff);
  List *args = string_tokenize (sbuff);
  int l = list_length (args);  
  char **argv = malloc ((l + 1) * sizeof (char *));

  if (argv)
    {
    for (int i = 0; i < l; i++)
      {
      const String *arg = list_get (args, i);
      argv[i] = strdup (string_cstr (arg));
      }
    argv[l] = NULL;

    int argc = l;
    if (strcasecmp (argv[0], "quit") == 0)
      ret = ERROR_ENDOFINPUT;
    else
      ret = shell_do_line_argv (argc, argv, console_params, picocpm);

    for (int i = 0; i < l; i++)
      {
      free (argv[i]);
      }

    free (argv);
    } 
  else
    {
    shell_write_error (ERROR_NOMEM, console_params);
    }

  list_destroy (args);
  string_destroy (sbuff);
  return ret;
  }

//
// shell_main 
//
void shell_main (ConsoleParams *console_params, PicoCPM *picocpm)
  {
  List *history = list_create (free);
  config = picocpm_get_config (picocpm);
  BOOL stop = FALSE;
  shell_writeln (console_params, "CPICOM version 0.1b");
  shell_writeln (console_params, "CP/M emulator for Raspberry Pi Pico");
  shell_writeln (console_params, "By Kevin Boone, and many others.");
  shell_writeln (console_params, "");
  while (!stop)
    {
    console_params->console_out_char 
       (console_params, filecontext_global_get_current_drive() + 'A');
    console_params->console_out_string (console_params->context, "> ");
    char buff[257];
    Error err = console_params->console_get_line (console_params->context, 
      buff, sizeof (buff) - 1, 10, history);
    if (err == 0)
      {
      err = shell_do_line (buff, console_params, picocpm);
      if (err == ERROR_ENDOFINPUT)
        stop = TRUE;
      }
    else if (err == ERROR_ENDOFINPUT)
      stop = TRUE;
    }

  list_destroy (history);
  }

