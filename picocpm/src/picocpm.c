/*=========================================================================
 
  cpicom

  picocpm

  This module provides the interface between the command-line processor,
  filesystem, and the sprawling complexity that is the CPM emulator.

  =========================================================================*/
#include <stdlib.h>
#include <string.h>
#include "picocpm/picocpm.h" 
#include "error/error.h" 
#include "log/log.h" 
#include "console/console.h" 
#include "cpm/limits.h" 
#include "shell/shell.h" 
#include "cpm/defs.h" 

// From com/intf.c
extern int cpm_runprog (ConsoleParams *cp, const char *cmdline, 
            z80info **z80_info);

struct _PicoCPM
  {
  ConsoleParams *console_params;
  z80info *z80_info;
  Config *config;
  };

//
// picocpm_create 
//
PicoCPM *picocpm_create (void)
  {
  PicoCPM *self = malloc (sizeof (PicoCPM));
  self->config = config_create();
  return self;
  }

//
// picocpm_set_console_params
//
void picocpm_set_console_params (PicoCPM *self, 
     ConsoleParams *console_params)
  {
  self->console_params = console_params;
  }

//
// picocpm_destroy
//
void picocpm_destroy (PicoCPM *self)
  {
  if (self)
    {
    config_destroy (self->config);
    free (self);
    }
  }

//
// picocpm_interrupt_handler
// 
static void picocpm_interrupt_handler (void *context)
  {
  PicoCPM *self = (PicoCPM *) context;
  z80info *z80_info = self->z80_info;
  // The PicoCPM object is long-lived, and might get interrupts when
  //  no CP/M program is running. We should ignore these -- presumably
  //  something else will be handling them.
  if (z80_info) 
    {
    z80_info->finished = TRUE;
    // We could launch a debugger or something here
    }
  }

//
// picocpm_run
//
Error picocpm_run (PicoCPM *self, const char *exe_path, int argc, char **argv)
  {
  Error ret = 0;
  char cmdline [CPM_MAX_CMDLINE + 1];
  strcpy (cmdline, exe_path);
  char *dotpos = strchr (cmdline, '.');
  // We might be passed a full filenane like "prog.com", but the BDOS
  //  impleemntation will choke on the .com extension. Sigh.
  if (dotpos) *dotpos = 0;
  BOOL too_long = FALSE;
  for (int i = 1; i < argc && !too_long; i++)
    {
    if (strlen (cmdline) + strlen (argv[i] + 1 + 1) < CPM_MAX_CMDLINE)
      {
      strcat (cmdline, " ");
      strcat (cmdline, argv[i]);
      }
    else
      too_long = TRUE;
    } 
  if (!too_long)
    {
    log_debug ("Running CP/M with cmdline '%s'", cmdline);
    self->console_params->console_set_interrupt_handler 
      (self->console_params->context, picocpm_interrupt_handler, self);
    int ret = cpm_runprog (self->console_params, cmdline, &self->z80_info);

    //console_params->set_interrupt_handler (NULL, NULL); ??
    (void) ret; // TODO -- if there ever is a return from runprog(), convert it
    }
  else
    {
    shell_write_string (self->console_params, 
       "Cmdline too long for CP/M\n"); // TODO -- console
    }
  return ret;
  }

//
// picocpm_get_config
// 
Config *picocpm_get_config (PicoCPM *self)
  {
  return self->config;
  }


