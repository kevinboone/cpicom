/*=========================================================================
 
  cpicom

  files/compat.c 

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "files/compat.h" 
#include "error/error.h" 
#include "log/log.h" 
#include "files/filecontext.h" 
#include "files/filesystemmount.h" 

//#define DEBUG 1

//
// compat_get_drive 
//
int compat_get_drive (const char *filename)
  {
  const char *p = strchr (filename, ':');
  if (p)
    {
    // TODO check "drive" part is only one char and is in range
    int drive = toupper (filename[0]) - 'A';
    if (drive < 0 || drive > FILESYSTEM_MAX_MOUNTS)
      {
      return -ERROR_BADDRIVELETTER;
      }
    else
      return drive;
    }
  else
    {
    return filecontext_global_get_current_drive();
    }
  }

//
// compat_get_drive 
//
int compat_get_drive_no_default (const char *filename)
  {
  const char *p = strchr (filename, ':');
  if (p)
    {
    // TODO check "drive" part is only one char and is in range
    int drive = toupper (filename[0]) - 'A';
    if (drive < 0 || drive > FILESYSTEM_MAX_MOUNTS)
      {
      return -ERROR_BADDRIVELETTER;
      }
    else
      return drive;
    }
  else
    {
    return -1;
    }
  }

//
// compat_get_path
//
const char *compat_get_path (const char *filename)
  {
  const char *p = strchr (filename, ':');
  if (p) return p + 1;
  return filename;
  }

/** Split a filename into drive, name, extention, in 8.3 format.
    If there is no drive, this value is set to 0. *name and *ext
    must be of length at least 9 and 4 respectively. */
extern void compat_split_drive_name_ext_83 (const char *fullname,
               char *drive_letter, char *name, char *ext)
  {
  *drive_letter = 0;

  int drive = compat_get_drive_no_default (fullname);
  if (drive >= 0) 
    *drive_letter = drive + 'A';

  char uc_fullname [BDOS_MAX_FNAME + 1];
  memset (uc_fullname, 0, BDOS_MAX_FNAME + 1);
  strncpy (uc_fullname, compat_get_path (fullname), 12);
  uc_fullname[13] = 0;
  string_to_upper (uc_fullname);

  char *dotpos = strrchr (uc_fullname, '.'); 
  if (dotpos)
    {
    *dotpos = 0;
    strncpy (name, uc_fullname, 8);
    name[8] = 0;
    strncpy (ext, dotpos + 1, 3);
    ext[3] = 0;
    }
  else
    {
    strncpy (name, uc_fullname, 8);
    name[8] = 0;
    ext[0] = 0;
    }
  }


//
// my_unlink
//
int my_unlink (const char *filename)
  {
  char uc_filename [MAX_PATH + 1];
  strncpy (uc_filename, filename, MAX_PATH);
  uc_filename[MAX_PATH] = 0;
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (uc_filename);
#endif
  log_debug ("Request unlink %s", uc_filename); 
  int drive = compat_get_drive (uc_filename);
  if (drive < 0) 
    {
    errno = -drive;
    return -1;
    }

  const char *path = compat_get_path (uc_filename);

  Error error = filecontext_global_delete (drive, path);
  if (error)
    {
    errno = error;
    return -1;
    }
  else
    return 0;
  }

//
// my_open
//
int my_open (const char *filename, int flags)
  {
  log_debug ("Request open %s with flags %04X", filename, flags); 
  char uc_filename [MAX_PATH + 1];
  strncpy (uc_filename, filename, MAX_PATH);
  uc_filename[MAX_PATH] = 0;
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (uc_filename);
#endif
  int ret = 0;
  // TODO use compat_get_drive, etc.

  const char *name = uc_filename;
  const char *p = strchr (uc_filename, ':');
  int drive;
  if (p)
    {
    // TODO check "drive" part is only one char
    drive = toupper (uc_filename[0]) - 'A';
    name = p + 1;
    }
  else
    {
    drive = filecontext_global_get_current_drive();
    }

  log_debug ("Name %s on drive %d", name, drive);
  if (drive < 0 || drive > FILESYSTEM_MAX_MOUNTS)
    {
    errno = ERROR_BADDRIVELETTER;
    ret = -1; 
    }

  if (ret == 0)
    {
    // Sets errno
    ret = filecontext_global_open (drive, name, flags);
    }

  return ret;
  }

//
// my_close
//
int my_close (int fd)
  {
  // Sets errno
  return filecontext_global_close (fd);
  }

//
// my_write
//
ssize_t my_write (int fd, const void *buf, size_t n)
  {
  // Sets errno
  return filecontext_global_write (fd, buf, n);
  }

//
// my_read
//
ssize_t my_read (int fd, void *buf, size_t n)
  {
  // Sets errno
  return filecontext_global_read (fd, buf, n);
  }

#if PICO_ON_DEVICE
extern char *itoa (int num, char* str, int base);
#else
void swap (char *x, char *y) { char t = *x; *x = *y; *y = t; }

void reverse(char *buffer, int i, int j)
  {
  while (i < j)
    swap (&buffer[i++], &buffer[j--]);
  }

char *itoa (int num, char* str, int base)
  {
  int i = 0;
  BOOL isNegative = FALSE;

  if (num == 0)
    {
    str[i++] = '0';
    str[i] = '\0';
    return str;
    }

  if (num < 0 && base == 10)
    {
    isNegative = TRUE;
    num = -num;
    }

  while (num != 0)
    {
    int rem = num % base;
    str[i++] = (char) ((rem > 9)? (rem-10) + 'a' : rem + '0' );
    num = num/base;
    }

  if (isNegative)
    str[i++] = '-';

  str[i] = '\0'; 

  reverse (str, 0, i - 1);
  return str;
  }
#endif

/*==========================================================================

  my_fnmatch

==========================================================================*/
int my_fnmatch (const char *pattern, const char *string, int flags)
  {
  register const char *p = pattern, *n = string;
  register unsigned char c;

#define FOLD(c)	((flags & MYFNM_CASEFOLD) ? tolower (c) : (c))

  while ((c = *p++) != '\0')
    {
      c = FOLD (c);

      switch (c)
	{
	case '?':
	  if (*n == '\0')
	    return MYFNM_NOMATCH;
	  else if ((flags & MYFNM_FILE_NAME) && *n == '/')
	    return MYFNM_NOMATCH;
	  else if ((flags & MYFNM_PERIOD) && *n == '.' &&
		   (n == string || ((flags & MYFNM_FILE_NAME) && n[-1] == '/')))
	    return MYFNM_NOMATCH;
	  break;

	case '\\':
	  if (!(flags & MYFNM_NOESCAPE))
	    {
	      c = *p++;
	      c = FOLD (c);
	    }
	  if (FOLD ((unsigned char)*n) != c)
	    return MYFNM_NOMATCH;
	  break;

	case '*':
	  if ((flags & MYFNM_PERIOD) && *n == '.' &&
	      (n == string || ((flags & MYFNM_FILE_NAME) && n[-1] == '/')))
	    return MYFNM_NOMATCH;

	  for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
	    if (((flags & MYFNM_FILE_NAME) && *n == '/') ||
		(c == '?' && *n == '\0'))
	      return MYFNM_NOMATCH;

	  if (c == '\0')
	    return 0;

	  {
	    unsigned char c1 = (!(flags & MYFNM_NOESCAPE) && c == '\\') ? *p : c;
	    c1 = FOLD (c1);
	    for (--p; *n != '\0'; ++n)
	      if ((c == '[' || FOLD ((unsigned char)*n) == c1) &&
		  my_fnmatch (p, n, flags & ~MYFNM_PERIOD) == 0)
		return 0;
	    return MYFNM_NOMATCH;
	  }

	case '[':
	  {
	    /* Nonzero if the sense of the character class is inverted.  */
	    register int negate;

	    if (*n == '\0')
	      return MYFNM_NOMATCH;

	    if ((flags & MYFNM_PERIOD) && *n == '.' &&
		(n == string || ((flags & MYFNM_FILE_NAME) && n[-1] == '/')))
	      return MYFNM_NOMATCH;

	    negate = (*p == '!' || *p == '^');
	    if (negate)
	      ++p;

	    c = *p++;
	    for (;;)
	      {
		register unsigned char cstart = c, cend = c;

		if (!(flags & MYFNM_NOESCAPE) && c == '\\')
		  cstart = cend = *p++;

		cstart = cend = FOLD (cstart);

		if (c == '\0')
		  /* [ (unterminated) loses.  */
		  return MYFNM_NOMATCH;

		c = *p++;
		c = FOLD (c);

		if ((flags & MYFNM_FILE_NAME) && c == '/')
		  /* [/] can never match.  */
		  return MYFNM_NOMATCH;

		if (c == '-' && *p != ']')
		  {
		    cend = *p++;
		    if (!(flags & MYFNM_NOESCAPE) && cend == '\\')
		      cend = *p++;
		    if (cend == '\0')
		      return MYFNM_NOMATCH;
		    cend = FOLD (cend);

		    c = *p++;
		  }

		if (FOLD ((unsigned char)*n) >= cstart
		    && FOLD ((unsigned char)*n) <= cend)
		  goto matched;

		if (c == ']')
		  break;
	      }
	    if (!negate)
	      return MYFNM_NOMATCH;
	    break;

	  matched:;
	    /* Skip the rest of the [...] that already matched.  */
	    while (c != ']')
	      {
		if (c == '\0')
		  /* [... (unterminated) loses.  */
		  return MYFNM_NOMATCH;

		c = *p++;
		if (!(flags & MYFNM_NOESCAPE) && c == '\\')
		  /* XXX 1003.2d11 is unclear if this is right.  */
		  ++p;
	      }
	    if (negate)
	      return MYFNM_NOMATCH;
	  }
	  break;

	default:
	  if (c != FOLD ((unsigned char)*n))
	    return MYFNM_NOMATCH;
	}

      ++n;
    }

  if (*n == '\0')
    return 0;

  if ((flags & MYFNM_LEADING_DIR) && *n == '/')
    /* The MYFNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
    return 0;

  return MYFNM_NOMATCH;
  }

/*=========================================================================

  shell_glob_match

=========================================================================*/
BOOL shell_glob_match (const char *filename, const char *match)
  {
  return !my_fnmatch (match, filename, 0);
  }

/*=========================================================================

  compat_globber

  TODO -- this is not implemented at all yet. It just puts a copy
  of the supplied filename into the list of results.

=========================================================================*/
void compat_globber (const char *pattern, List *list) // List of char*
  {
  // a:/foo/bar/*.txt 
  
  char _pattern[MAX_FNAME + 1];
  strcpy (_pattern, pattern);
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (_pattern);
#endif

  const char *fname;
  char dir [MAX_FNAME + 1];
  BOOL has_path = FALSE;
  
  char *slpos = strrchr (_pattern, '/');
  if (slpos)
    {
    //  A:/foo/*.txt
    *slpos = 0;
    fname = slpos + 1;
    strcpy (dir, _pattern);
    has_path = TRUE;
    }
  else 
    {
    char *colpos = strrchr (_pattern, ':');
    if (colpos)
      {
      //  A:*.txt
      *colpos = 0; 
      fname = colpos + 1;
      strcpy (dir, _pattern);
      strcat (dir, ":");
      }
    else
      {
      //  *.txt
      fname = _pattern;
      dir[0] = 0;
      }
    }
  
  //printf ("dir=%s\n", dir);
  //printf ("fname=%s\n", fname);

  BOOL matched = FALSE;
  my_DIR *dirp = my_opendir (dir);
  if (dirp)
    {
    struct my_dirent *de = my_readdir (dirp);
    while (de)
      {
      if (shell_glob_match (de->d_name, fname))
        {
        char path [MAX_FNAME + 1];
        strcpy (path, dir);
        if (has_path)
          strcat (path, "/");
        strcat (path, de->d_name); 
        list_append (list, strdup (path));
        matched = TRUE;
        }
      de = my_readdir (dirp);
      } 
    my_closedir (dirp);
    }

  if (!matched) 
    list_append (list, strdup(pattern));
  }

//
// my_opendir 
//
my_DIR *my_opendir (const char *path_with_drive)
  {
  char uc_path_with_drive [MAX_PATH + 1];
  strncpy (uc_path_with_drive, path_with_drive, MAX_PATH);
  uc_path_with_drive[MAX_PATH] = 0;
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (uc_path_with_drive);
#endif
  log_debug ("Request opendir %s", uc_path_with_drive); 
  int drive = compat_get_drive (uc_path_with_drive);
  if (drive < 0) 
    {
    errno = -drive;
    return NULL; 
    }

  const char *path = compat_get_path (uc_path_with_drive);
  // sets errno
  return filecontext_global_opendir (drive, path);
  }

//
// my_closedir 
//
int my_closedir (my_DIR *dir)
  {
  int ret = 0;
  Error err = filecontext_global_closedir (dir);
  if (err)
    {
    errno = err;
    ret = -1;
    }
  return ret;
  }

//
// my_readdir 
//
struct my_dirent *my_readdir (struct _my_DIR *dir)
  {
  return filecontext_global_readdir (dir);
  }

//
// my_creat
//
int my_creat (const char *path, int mode)
  {
  (void)mode; // Not implemented yet
  printf ("Create %s\n", path);
  int fd = my_open (path, O_WRONLY | O_CREAT | O_EXCL);
  return fd;
  }

//
// my_lseek
//
uint32_t my_lseek (int fd, uint32_t offset, int whence)
  {
  return filecontext_global_lseek (fd, offset, whence);
  }

//
// my_fstat
//
int my_fstat (int fd, struct stat *statbuf)
  {
  Error ret = filecontext_global_fstat (fd, statbuf);
  if (ret != 0)
    {
    return -1;
    errno = ret;
    }
  return 0;
  }

//
// my_ftruncate
//
int my_ftruncate (int fd, uint32_t length)
  {
  Error ret = filecontext_global_ftruncate (fd, length);
  if (ret != 0)
    {
    return -1;
    errno = ret;
    }
  return 0;
  }

//
// my_fprinterr
//
// Nasty stub for calls to fprintf(stderr) that the Pico SDK can't
//  actually handle. I haven't just replaced them with "printf" calls
//  because, being errors, they might need to be treated differently.
//  ** TODO: this function's output needs to be fed through the 
//  console infrastructure
void my_fprintferr (void *f, const char *fmt, ...)
  {
  (void)f;
  // TODO
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  }

//
// my_fopen
//
struct myfile *my_fopen (const char *filename, const char *mode)
  {
#ifdef DEBUG 
  printf ("myfopen %s\n", filename);
#endif
  struct myfile *self = malloc (sizeof (struct myfile));

  if (self)
    {
    self->fd = -1;
    int flags = 0;
    char newmode[10]; // Should really make this dynamic
    newmode[0] = 0;
    // Strip the 'b' characters from the mode
    int l = strlen (mode);
    int j = 0;
    for (int i = 0; i < l; i++)
      {
      if (mode[i] != 'b')
        {
        newmode[j] = mode[i];
	j++;
	}
      newmode[j] = 0;
      }
    if (strcmp (newmode, "r") == 0) flags = O_RDONLY;
    else if (strcmp (newmode, "r+") == 0) flags = O_RDWR;
    else if (strcmp (newmode, "w") == 0) flags = O_WRONLY | O_CREAT | O_TRUNC;
    else if (strcmp (newmode, "w+") == 0) flags = O_RDWR | O_CREAT | O_TRUNC;
    else if (strcmp (newmode, "a") == 0) flags = O_WRONLY | O_CREAT | O_APPEND;
    else if (strcmp (newmode, "a+") == 0) flags = O_RDWR | O_CREAT | O_APPEND;

    int fd = my_open (filename, flags);
    if (fd >= 0)
      {
      self->fd = fd;
      }
    else
      {
      free (self);
      self = NULL;
      }
    }
  return self;
  }

//
// my_fwrite
//
int32_t my_fwrite (const void *ptr, int32_t size,
                              int32_t nmemb, MYFILE *fp)
  {
#ifdef DEBUG
  printf ("myfwrite %d\n", fp->fd);
#endif
  int32_t total = size * nmemb;
  int n = my_write (fp->fd, ptr, total);
  if (n < 0) return 0;
  return n / size; 
  }

//
// my_fread
//
int32_t my_fread (void *ptr, int32_t size,
                              int32_t nmemb, MYFILE *fp)
  {
#ifdef DEBUG
  printf ("myfread %d %d %d\n", fp->fd, (int)size, (int)nmemb);
#endif
  int32_t total = size * nmemb;
  int n = my_read (fp->fd, ptr, total);
#ifdef DEBUG
  printf ("myfread got %d\n", (int) n); 
#endif
  if (n < 0) return 0;
  return n / size; 
  }

//
// my_fclose
//
int my_fclose (MYFILE *fp)
  {
#ifdef DEBUG
  printf ("myclose %d\n", fp->fd);
#endif
  if (fp->fd >= 0)
    my_close (fp->fd);
  free (fp);
  return 0;
  }

//
// my_ftell
//
uint32_t my_ftell (MYFILE *fp)
  {
#ifdef DEBUG
  printf ("mytell %d\n", fp->fd);
#endif
  uint32_t ret = my_lseek (fp->fd, 0, SEEK_CUR);
#ifdef DEBUG
  printf ("mytell res %d\n", (int) ret);
#endif
  return ret;
  }

//
// my_fseek
//
int my_fseek (MYFILE *fp, uint32_t offset, int whence)
  {
#ifdef DEBUG
  printf ("myfseek %d\n", fp->fd);
#endif
  int ret = my_lseek (fp->fd, offset, whence);
#ifdef DEBUG
  printf ("fseek res= %d\n", ret);
#endif
  if (ret >= 0)
    return 0;
  else
    return -1;
  }

//
// my_getc
//
int my_getc (MYFILE *fp)
  {
#ifdef DEBUG
  printf ("mygetc\n");
#endif
  char c;
  int n = my_read (fp->fd, &c, 1);
  if (n == 1)
    return c;
  else
    return EOF;
  }

//
// my_putc
//
int my_putc (int c, MYFILE *fp)
  {
#ifdef DEBUG
  printf ("myputc\n");
#endif
  int n = my_write (fp->fd, &c, 1);
  return n;
  }

//
// my_fflush
//
void my_fflush (MYFILE *f)
  {
  (void)f;
  // Nothing to do at present -- no buffering :/
  }

//
// my_rename
//
int my_rename (const char *source, const char *target)
  {
  char uc_source_with_drive [MAX_PATH + 1];
  strncpy (uc_source_with_drive, source, MAX_PATH);
  uc_source_with_drive [MAX_PATH] = 0;
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (uc_source_with_drive);
#endif

  char uc_target_with_drive [MAX_PATH + 1];
  strncpy (uc_target_with_drive, target, MAX_PATH);
  uc_target_with_drive [MAX_PATH] = 0;
#ifdef FILE_COMPAT_UPPERCASE
  string_to_upper (uc_target_with_drive);
#endif

  int source_drive = compat_get_drive (uc_source_with_drive);
  if (source_drive < 0) 
    {
    errno = ERROR_BADDRIVELETTER;
    return -1; 
    }
  const char *source_path = compat_get_path (uc_source_with_drive);

  int target_drive = compat_get_drive (uc_target_with_drive);
  if (target_drive < 0) 
    {
    errno = ERROR_BADDRIVELETTER;
    return -1; 
    }
  const char *target_path = compat_get_path (uc_target_with_drive);

  log_debug ("my_rename: source %d : %s, target %d, %s\n", source_drive,
     source_path, target_drive, target_path);
  
  if (source_drive != target_drive)
    {
    errno = EXDEV;
    return -1;
    }
  
  int ret;

  Error error = filecontext_global_rename (source_drive, 
          source_path, target_path);
  if (error == 0)
    {
    errno = 0;
    ret = 0;
    }
  else
    {
    errno = error;
    ret = -1;
    }
  return ret; 
  }

//
// my_stat
//
int my_stat (const char *filename, struct stat *statbuf)
  {
  int ret;
  int drive = compat_get_drive (filename);
  if (drive >= 0)
    { 
    const char *path = compat_get_path (filename);
    Error err = filecontext_global_stat (drive, path, statbuf);
    if (err != 0)
      {
      ret = -1;
      errno = err;
      }
    else
      ret = 0;
    }
  else
    {
    ret = -1;
    errno = -drive;
    }
  return ret;
  }


