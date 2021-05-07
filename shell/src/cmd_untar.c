/*=========================================================================
 
  cpicom

  shell/cmd_untar.c

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error/error.h" 
#include "log/log.h" 
#include "shell/shell.h" 
#include "console/console.h" 
#include "files/compat.h" 

//
// cmd_untar_parse_octal
//
static int cmd_untar_parse_octal (const char *p, size_t n)
  {
  int i = 0;

  while ((*p < '0' || *p > '7') && n > 0) 
    {
    ++p;
    --n;
    }

  while (*p >= '0' && *p <= '7' && n > 0) 
    {
    i *= 8;
    i += *p - '0';
    ++p;
    --n;
    }

  return i;
  }

//
// cmd_untar_verify_checksum
//
static BOOL cmd_untar_verify_checksum (const char *p)
  {
  int u = 0;
  for (int n = 0; n < 512; ++n) 
    {
    if (n < 148 || n > 155)
       u += ((unsigned char *)p)[n];
    else
       u += 0x20;
    }

  int stored_cs = cmd_untar_parse_octal (p + 148, 8);
  return (u == stored_cs);
  }

//
// cmd_untar_is_end_of_archive
//
static BOOL cmd_untar_is_end_of_archive (const char *p)
  {
  for (int n = 511; n >= 0; --n)
    if (p[n] != '\0')
      return  FALSE;
  return TRUE;
  }

//
// cmd_untar_do
//
static void cmd_untar_do (ConsoleParams *cp, MYFILE *a)
  {
  char buff[512];
  MYFILE *f = NULL;

  for (;;) 
    {
    size_t bytes_read = my_fread (buff, 1, sizeof (buff), a);
    if (bytes_read < sizeof (buff)) 
      {
      shell_writeln (cp, "Short read: expected %d, got %d",
         sizeof (buff), (int)bytes_read);
         return;
      }
    if (cmd_untar_is_end_of_archive (buff)) 
      {
      return;
      }
    if (!cmd_untar_verify_checksum (buff)) 
      {
      shell_writeln (cp, "Checksum failure");
         return;
      }
    int filesize = cmd_untar_parse_octal (buff + 124, 12);
    switch (buff[156]) 
      {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
        shell_writeln (cp, "Ignoring %s", buff);
        break;
      default:
        shell_writeln (cp, "Extracting file %s", buff);
        f = my_fopen (buff, "w");
        break;
      }
    while (filesize > 0) 
      {
      bytes_read = my_fread (buff, 1, sizeof(buff), a);
      if (bytes_read < sizeof (buff)) 
          {
          shell_writeln (cp, "Short read: Expected %d, got %d",
             sizeof (buff), (int)bytes_read);
           return;
           }
      if (filesize < (int)sizeof (buff))
            bytes_read = filesize;
      if (f) 
        {
        if ((int)my_fwrite (buff, 1, bytes_read, f) != (int)bytes_read)
          {
          shell_writeln (cp, "Failed write");
          my_fclose (f);
          f = NULL;
          }
        }
      filesize -= bytes_read;
      }
    if (f) 
      {
      my_fclose (f);
      f = NULL;
      }
    }
  }

//
// cmd_untar_run
//
Error cmd_untar_run (int argc, char **argv, 
     ConsoleParams *console_params)
  {
  if (argc < 2)
    {
    shell_write_error (ERROR_ARGCOUNT, console_params);
    return ERROR_ARGCOUNT;
    }
  errno = 0;
  char *filename = argv[1];
  MYFILE *f = my_fopen (filename, "r");
  if (f)
    {
    cmd_untar_do (console_params, f);
    my_fclose (f);
    }
  else
    shell_write_error_filename (errno, filename, console_params);
  return errno;
  }




