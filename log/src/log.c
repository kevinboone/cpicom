/*=========================================================================
 
  cpicom

  log/log.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "log/log.h"
#include "console/console.h"

static ConsoleParams *log_console;
static int log_level = LOGLEVEL_WARN;

void log_set_console (ConsoleParams *console_params)
  {
  log_console = console_params;
  }

void log_set_level (int level)
  {
  if (level >= 0 && level <= LOGLEVEL_TRACE)
    log_level = level;
  }

//
// log_error
//
void log_error (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  log_console->console_out_string (log_console->context, "ERROR: ");
  log_console->console_out_string_v (log_console->context, fmt, ap);
  log_console->console_out_endl (log_console->context);
  va_end (ap);
  }

void log_debug (const char *fmt,...)
  {
  if (log_level < LOGLEVEL_DEBUG) return;
  va_list ap;
  va_start (ap, fmt);
  log_console->console_out_string (log_console->context, "DEBUG: ");
  log_console->console_out_string_v (log_console->context, fmt, ap);
  log_console->console_out_endl (log_console->context);
  va_end (ap);
  }

void log_trace (const char *fmt,...)
  {
  if (log_level < LOGLEVEL_TRACE) return;
  va_list ap;
  va_start (ap, fmt);
  log_console->console_out_string (log_console->context, "TRACE: ");
  log_console->console_out_string_v (log_console->context, fmt, ap);
  log_console->console_out_endl (log_console->context);
  va_end (ap);
  }

void log_warn (const char *fmt,...)
  {
  if (log_level < LOGLEVEL_WARN) return;
  va_list ap;
  va_start (ap, fmt);
  log_console->console_out_string (log_console->context, "WARN: ");
  log_console->console_out_string_v (log_console->context, fmt, ap);
  log_console->console_out_endl (log_console->context);
  va_end (ap);
  }

void log_info (const char *fmt,...)
  {
  if (log_level < LOGLEVEL_INFO) return;
  va_list ap;
  va_start (ap, fmt);
  log_console->console_out_string (log_console->context, "INFO: ");
  log_console->console_out_string_v (log_console->context, fmt, ap);
  log_console->console_out_endl (log_console->context);
  va_end (ap);
  }



