/*=========================================================================
 
  cpicom

  log/log.h 

  Logging functions

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include "console/console.h"

#define LOGLEVEL_ERROR 0
#define LOGLEVEL_WARN  1
#define LOGLEVEL_INFO  2
#define LOGLEVEL_DEBUG 3
#define LOGLEVEL_TRACE 4

extern void log_set_console (ConsoleParams *console_params);
extern void log_set_level (int level);

extern void log_trace (const char *fmt,...);
extern void log_debug (const char *fmt,...);
extern void log_info (const char *fmt,...);
extern void log_warn (const char *fmt,...);
extern void log_error (const char *fmt,...);

