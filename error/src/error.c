/*=========================================================================

  cpicom

  error/error.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#include <string.h> 
#include "error/error.h"

//
// error_strerror
// 
const char *error_strerror (Error error)
  {
  if (error < ERROR_MAX_ERRNO) return strerror (error);
  switch (error)
    {
    case ERROR_NOENT: case ERROR_NOMEM: case ERROR_MFILE:
    case ERROR_BADF:
      break; // Already done
    case ERROR_CORRUPT_FS: return "Corrupted filesystem";
    case ERROR_INTERRUPTED: return "Interrupted";
    case ERROR_ENDOFINPUT: return "End of input";
    case ERROR_BADDRIVELETTER: return "Bad drive letter";
    case ERROR_ARGCOUNT: return "Incorrect argument count";
    case ERROR_UNDEFINED_DRIVE: return "Undefined drive";
    case ERROR_NOMEDIUM: return "No medium in drive";
    case ERROR_EXISTS: return "File already exists";
    case ERROR_YMODEM: return "YModem transfer failed";
    case ERROR_EXE_TOO_LARGE: return "Executable too large";
    case ERROR_CMDLINE_TOO_LONG: return "Command line too long";
    case ERROR_UNIMPLEMENTED: return "Unimplemented feature";
    case ERROR_CMD_SYNTAX: return "Command syntax error";
    case ERROR_DRIVE_NOT_READY: return "Drive not ready";
    case ERROR_DISK_LOWLEVEL: return "Lowlevel disk error";
    case ERROR_FILENAME: return "Invalid filename";
    case ERROR_INTERNAL: return "Unknown internal error";
    }
  return NULL;
  }

