/*=========================================================================

  cpicom

  error/error.h

  Definitions of CPICOM error codes. Note that all the standard Linux
  errno codes may also be used -- the codes specific to CPICOM start
  at value 1000.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include <errno.h>
#define ERROR_MAX_ERRNO 1000

typedef enum
  {
  ERROR_NOMEM = ENOMEM, 
  ERROR_NOENT = ENOENT, 
  ERROR_MFILE = EMFILE,
  ERROR_BADF = EBADF,
  ERROR_CORRUPT_FS = 1001,
  ERROR_UNDEFINED_DRIVE = 1002,
  ERROR_INTERRUPTED = 1003,
  ERROR_ENDOFINPUT = 1004,
  ERROR_BADDRIVELETTER = 1005,
  ERROR_ARGCOUNT = 1006,
  ERROR_NOMEDIUM = 1007,
  ERROR_EXISTS = 1008,
  ERROR_YMODEM = 1009,
  ERROR_EXE_TOO_LARGE = 1010,
  ERROR_CMDLINE_TOO_LONG = 1011,
  ERROR_UNIMPLEMENTED = 1012,
  ERROR_CMD_SYNTAX = 1013,
  ERROR_DRIVE_NOT_READY = 1014,
  ERROR_DISK_LOWLEVEL = 1015,
  ERROR_FILENAME = 1016,
  ERROR_INTERNAL = 1017
  } Error;

extern const char *error_strerror (Error error);
