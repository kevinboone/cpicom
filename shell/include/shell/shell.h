/*=========================================================================

  cpicom

  shell/shell.h

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once
#include "picocpm/picocpm.h" 
#include "error/error.h" 
#include "console/console.h" 
#include "picocpm/config.h" 

extern void   shell_write_endl (ConsoleParams *console_params);
extern void   shell_write_error (Error error, ConsoleParams *console_params);
extern void   shell_write_error_filename (Error error, 
                 const char *filename, ConsoleParams *console_params);
extern void   shell_write_string (ConsoleParams *console_params, 
                  const char *fmt,...);

/** Reset the line count before outputting a large amount of screen data.
    This allows the shell's paging mechanism to work, if it is enabled. */
void shell_reset_linecount (void);

/** Write a single line, and pause for paging if the number of lines written
    exceeds the screen length */
extern Error  shell_writeln (ConsoleParams *console_params, 
                  const char *fmt,...);

extern void   shell_main (ConsoleParams *console_params, PicoCPM *picocpm);

extern Error  cmd_type_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_log_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_era_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_dir_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_yrecv_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_ysend_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_dump_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_setdef_run (int argc, char **argv, 
                  ConsoleParams *console_params, Config *config);
extern Error  cmd_rename_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_stat_run (int argc, char **argv, 
                  ConsoleParams *console_params);
extern Error  cmd_format_run (int argc, char **argv, 
                  ConsoleParams *console_params);

extern Error  shell_do_line (const char *buff, 
                 ConsoleParams *console_params, PicoCPM *picocmp);

