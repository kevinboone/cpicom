/*=========================================================================

  cpicom

  console/console.h 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include <stdint.h>

#include "console/console.h"
#include "picocpm/config.h"

typedef struct _ConsoleStdioVT100 ConsoleStdioVT100;

extern ConsoleStdioVT100 *consolestdiovt100_create (void);
extern void               consolestdiovt100_destroy (ConsoleStdioVT100 *self);

extern void               consolestdiovt100_get_params 
                             (ConsoleStdioVT100 *self, ConsoleParams *params);

extern void              consolestdiovt100_set_config 
                             (ConsoleStdioVT100 *self, Config *config);

