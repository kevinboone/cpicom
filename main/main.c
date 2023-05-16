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
#include "blockdevice/flashblockdevice.h" 
#else
#include "blockdevice/fileblockdevice.h" 
#endif

//#define TEST_FAT 1

#include "picocpm/picocpm.h" 
#include "fs/filesystemlfs.h" 

#ifdef TEST_FAT
#include "fs/filesystemfat.h" 
#endif

#include "error/error.h" 
#include "shell/shell.h" 
#include "console/console_stdio_vt100.h" 
#include "console/console.h" 

#if defined(PICO_W)
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
// get this from cpicom.elf.map, search for __flash_binary_end
// then round up to 4K boundary
#define PROGRAM_SIZE 0x78000
#define BLOCKS_PER_DRIVE 189
#else
#define PROGRAM_SIZE 0x50000
#define BLOCKS_PER_DRIVE 180
#endif

#define FLASH_SIZE 0x200000

extern uint32_t __flash_binary_end;

//
// Start here
//
int main()
  {
  stdio_init_all();

#if defined(PICO_W)
  if (cyw43_arch_init()) {
      printf("Wi-Fi init failed");
      return -1;
  }
  // seems like this is required here to keep it from crashing
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

/*  
  int cnt = 1 ;
  while(cnt > 0 ) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
        cnt--;
  }
  */
#endif

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
#ifdef TEST_FAT
  BlockDeviceParams bd_params_c;
#endif

#if PICO_ON_DEVICE
  if( (FLASH_SIZE - PROGRAM_SIZE)/2/4096 < BLOCKS_PER_DRIVE ) {
    printf("ERROR:There is %d bytes of FLASH available\n", (unsigned int)(FLASH_SIZE - PROGRAM_SIZE) );
    printf("That means maximum of %d 4K blocks per drive\n",  (unsigned int)(FLASH_SIZE - PROGRAM_SIZE)/2/4096);
  } 
  FlashBlockDevice *fbd_a = flashblockdevice_create (PROGRAM_SIZE, BLOCKS_PER_DRIVE);
  Error errA = flashblockdevice_initialize (fbd_a);
  flashblockdevice_get_params (fbd_a, &bd_params_a);
  FlashBlockDevice *fbd_b = flashblockdevice_create (PROGRAM_SIZE + ((BLOCKS_PER_DRIVE+1) * 4096) , BLOCKS_PER_DRIVE );
  Error errB = flashblockdevice_initialize (fbd_b);
  flashblockdevice_get_params (fbd_b, &bd_params_b);
#else
  const char *home = getenv ("HOME");
  char path[MAX_FNAME];
  sprintf (path, "%s/drive-a.dsk", home);
  FileBlockDevice *fbd_a = fileblockdevice_create (path);
  Error errA = fileblockdevice_initialize (fbd_a);
  fileblockdevice_get_params (fbd_a, &bd_params_a);
  sprintf (path, "%s/drive-b.dsk", home);
  FileBlockDevice *fbd_b = fileblockdevice_create (path);
  Error errB = fileblockdevice_initialize (fbd_b);
  fileblockdevice_get_params (fbd_b, &bd_params_b);
#ifdef TEST_FAT
  sprintf (path, "%s/drive-c.dsk", home);
  FileBlockDevice *fbd_c = fileblockdevice_create (path);
  Error errC = fileblockdevice_initialize (fbd_c);
  fileblockdevice_get_params (fbd_c, &bd_params_c);
#endif
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

#ifdef TEST_FAT
  FilesystemFat *fatC = filesystemfat_create (0);
  if (errC == 0)
    {
    filesystemfat_set_block_device (fatC, &bd_params_c);
    FilesystemParams fs_params_c;
    filesystemfat_get_params (fatC, &fs_params_c);

    Error err = filecontext_mount (fc, 2, &fs_params_c, &bd_params_c);
    if (err)
      {
      log_error ("Mount error %d", err);
      }

    }
  else
    log_error ("Can't create block device, error %d", errC); 
#endif

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
  //shell_do_line ("ysend main.com", &console_params, picocpm);

#if PICO_ON_DEVICE
  // On the Pico, don't allow a ctrl+d to halt the board -- just reset
  while (TRUE) {
#endif
  shell_main (&console_params, picocpm);
#if PICO_ON_DEVICE

  }
#endif

  picocpm_destroy (picocpm);

  filecontext_destroy (fc);
  // Must destroyfilecontext first, as it unmounts. But it can't unmount
  //  if the individual handlers have been destroyed.
  if (lfsA) filesystemlfs_destroy (lfsA);
  if (lfsB) filesystemlfs_destroy (lfsB);
#ifdef TEST_FAT
  if (fatC) filesystemfat_destroy (fatC);
#endif

#if PICO_ON_DEVICE
  flashblockdevice_destroy (fbd_a);
  flashblockdevice_destroy (fbd_b);
#else
  fileblockdevice_destroy (fbd_a);
  fileblockdevice_destroy (fbd_b);

#ifdef TEST_FAT
  fileblockdevice_destroy (fbd_c);
#endif

#endif


#if defined(PICO_W)  
  cyw43_arch_deinit(); 
#endif

  consolestdiovt100_destroy (csvt100);
  }


