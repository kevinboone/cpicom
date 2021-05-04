/*=========================================================================

  Ymodem

  xmodem/xmodem.h

  (c)2021 Kevin Boone, GPLv3.0

  Based on work placed into the public domain by 
  Fredrik Hederstierna, 2014

=========================================================================*/

#pragma once

#include <stdlib.h>
#include "console/console.h" 

typedef enum _YmodemErr
  {
  YmodemOK = 0,
  YmodemWriteFile,
  YmodemReadFile,
  YmodemTooBig,
  YmodemChecksum,
  YmodemCancelled,
  YmodemBadPacket,
  YmodemNoCRC
  } YmodemErr;


/** Receive to the specified filename. If this is NULL, use the
  filename in the ymodem header. And if there's nothing there, either,
  use "untitled.txt". Of course, it makes no sense to use transmit
  multiples files and _not_ supply filenames. Similarly, it makes no
  sense to invoke this method with a filename, when we expect multiple
  files. */
extern YmodemErr ymodem_receive (ConsoleParams *consoleParams, 
  const char *filename, uint32_t max_size);

extern YmodemErr ymodem_send (ConsoleParams *consoleParams, 
  const char *filename);

/** Get an English string corresponding to the error code. */
extern const char *ymodem_strerror (YmodemErr err);





