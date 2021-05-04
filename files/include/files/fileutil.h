/*=========================================================================
 
  cpicom

  Basic file handling utilities

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include <stdint.h>
#include "error/error.h" 

extern Error fileutil_write_file (const char *filename, 
                   uint8_t *buff, int len);

extern Error fileutil_append_file (const char *filename, 
                   uint8_t *buff, int len);


