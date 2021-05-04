/*=========================================================================

  cpicom

  pciocpm/picocpm.h

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include "error/error.h"
#include "console/console.h"
#include "picocpm/config.h"

struct PicoCPM;
struct _ConsoleParams;
typedef struct _PicoCPM PicoCPM;

typedef void (*InterruptHandler) (void *interrupt_context);

extern PicoCPM  *picocpm_create (void);
extern void      picocpm_destroy (PicoCPM *self);
extern Error     picocpm_run (PicoCPM *self, const char *exe_path, 
                   int argc, char **argv);
extern void      picocpm_set_console_params (PicoCPM *self, 
                   struct _ConsoleParams *console_params);
extern Config   *picocpm_get_config (PicoCPM *self);

