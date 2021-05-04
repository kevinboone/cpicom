/*=========================================================================

  cpicom

  picocpm/config.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "log/log.h" 
#include "picocpm/config.h" 

//
// config_create
//
Config *config_create (void)
  {
  log_debug ("Crting shl cfg");
  Config *self = malloc (sizeof (Config));
  self->page = FALSE;
  self->flow_control = TRUE;
  return self;
  }

//
// config_destroy
//
void config_destroy (Config *self)
  {
  log_debug ("Destry shl cfg");
  if (self) 
    {
    free (self);
    }
  }




