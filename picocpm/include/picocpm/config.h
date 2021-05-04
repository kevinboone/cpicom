/*=========================================================================

  cpicom

  picocpm/config

  Storage and manipulate of the system configuration (not much can be
  configured yet).

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

struct Config;
typedef struct _Config 
  {
  BOOL page;
  BOOL flow_control;
  } Config;

Config *config_create (void);
void config_destroy (Config *self);

