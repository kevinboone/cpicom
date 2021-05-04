/*=========================================================================
 
  CPICOM

  main/main.c

  This is where it all starts.

   Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "log/log.h"
#include "files/filecontext.h" 
#include "pico/stdlib.h" 
#if PICO_ON_DEVICE
#include "files/flashblockdevice.h" 
#else
#include "files/fileblockdevice.h" 
#endif

#include "picocpm/picocpm.h" 
#include "files/filesystemlfs.h" 
#include "error/error.h" 
#include "shell/shell.h" 
#include "console/console_stdio_vt100.h" 
#include "console/console.h" 

//
// Start here
//
int main()
  {
  stdio_init_all();

  //log_set_level (LOGLEVEL_TRACE);
  log_set_level (LOGLEVEL_WARN);
  ConsoleStdioVT100 *csvt100 = consolestdiovt100_create();

  ConsoleParams console_params;
  consolestdiovt100_get_params (csvt100, &console_params); 
  log_set_console (&console_params);

  FileContext *fc = filecontext_create();
  global_fc = fc;

  BlockDeviceParams bd_params_a;
  BlockDeviceParams bd_params_b;
#if PICO_ON_DEVICE
  FlashBlockDevice *fbd_a = flashblockdevice_create (0x50000, 180);
  Error errA = flashblockdevice_initialize (fbd_a);
  flashblockdevice_get_params (fbd_a, &bd_params_a);
  FlashBlockDevice *fbd_b = flashblockdevice_create (0x106000, 180);
  Error errB = flashblockdevice_initialize (fbd_b);
  flashblockdevice_get_params (fbd_b, &bd_params_b);
#else
  FileBlockDevice *fbd_a = fileblockdevice_create ("/home/kevin/drive-a.dsk");
  Error errA = fileblockdevice_initialize (fbd_a);
  fileblockdevice_get_params (fbd_a, &bd_params_a);
  FileBlockDevice *fbd_b = fileblockdevice_create ("/home/kevin/drive-b.dsk");
  Error errB = fileblockdevice_initialize (fbd_b);
  fileblockdevice_get_params (fbd_b, &bd_params_b);
#endif

  FilesystemLfs *lfsA = filesystemlfs_create ();
  if (errA == 0)
    {
    filesystemlfs_set_block_device (lfsA, &bd_params_a);
    FilesystemParams fs_params_a;
    filesystemlfs_get_params (lfsA, &fs_params_a);

    Error err = filecontext_mount (fc, 0, &fs_params_a, &bd_params_a);
    if (err)
      {
      log_error ("Mount error %d", err);
      log_info ("Formatting drive 0");
      err = filecontext_format (fc, 0);
      if (err)
        log_error ("Format error %d", err); 
      }
    }
  else
    log_error ("Can't create block device, error %d", errA); 

  FilesystemLfs *lfsB = filesystemlfs_create ();
  if (errB == 0)
    {
    filesystemlfs_set_block_device (lfsB, &bd_params_b);
    FilesystemParams fs_params_b;
    filesystemlfs_get_params (lfsB, &fs_params_b);

    Error err = filecontext_mount (fc, 1, &fs_params_b, &bd_params_b);
    if (err)
      {
      log_error ("Mount error %d", err);
      log_info ("Formatting drive 1");
      err = filecontext_format (fc, 1);
      if (err)
        log_error ("Format error %d", err); 
      }
    }
  else
    log_error ("Can't create block device, error %d", errB); 

//=========
/*
  int f = open ("main.com", O_RDONLY);
  if (f < 0) 
    {
    fprintf (stderr, "can't open main.com\n");
    exit (0);
    }

  int l = lseek (f, 0, SEEK_END);
  lseek (f, 0, SEEK_SET);

  BYTE *buf = malloc (l);

  read (f, buf, l);

  close (f);

  picoz80_copy_to_mem (picoz80, 0x100, buf, l);
  free (buf);

  picoz80_run (picoz80);

*/
//=====

  PicoCPM *picocpm = picocpm_create ();
  picocpm_set_console_params (picocpm, &console_params);
  consolestdiovt100_set_config (csvt100, picocpm_get_config(picocpm));
  
  // Uncomment to have the program go straight to ymodem recv.
  //shell_do_line ("yrecv", &console_params, picocpm);
  //shell_do_line ("ysend te.com", &console_params, picocpm);

  shell_main (&console_params, picocpm);

  picocpm_destroy (picocpm);

  filecontext_destroy (fc);
  // Must destroyfilecontext first, as it unmounts. But it can't unmount
  //  if the individual handlers have been destroyed.
  if (lfsA) filesystemlfs_destroy (lfsA);
  if (lfsB) filesystemlfs_destroy (lfsB);

#if PICO_ON_DEVICE
  flashblockdevice_destroy (fbd_a);
  flashblockdevice_destroy (fbd_b);
#else
  fileblockdevice_destroy (fbd_a);
  fileblockdevice_destroy (fbd_b);
#endif
  consolestdiovt100_destroy (csvt100);
  }


