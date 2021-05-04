/*=========================================================================

  Files 

  files/fileutil.c

  (c)2021 Kevin Boone, GPLv3.0

=========================================================================*/

#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include "log/log.h" 
#include "files/fileutil.h" 
#include "files/compat.h" 

// 
// fileutil_write_file
// 
Error fileutil_write_file (const char *filename, 
                   uint8_t *buff, int len)
  {
  Error ret = 0;

  log_debug ("fileutil_write_file file=%s len=%d", filename, len);

  int fd = my_open (filename, O_WRONLY | O_CREAT | O_TRUNC);  
  if (fd >= 0)
    {
    int n = my_write (fd, buff, len);
    if (n != len) ret = errno;
    my_close (fd);
    }
  else
    ret = errno;

  return ret;
  }

// 
// fileutil_append_file
// 
Error fileutil_append_file (const char *filename, 
                   uint8_t *buff, int len)
  {
  Error ret = 0;

  log_debug ("fileutil_append_file file=%s len=%d", filename, len);

  int fd = my_open (filename, O_WRONLY | O_APPEND);  
  if (fd >= 0)
    {
    int n = my_write (fd, buff, len);
    if (n != len) ret = errno;
    my_close (fd);
    }
  else
    ret = errno;

  return ret;
  }

