/*=========================================================================

  cpicom

  files/compat.h

  Prototypes for all the substitute file-handling commands that CPICOM's
  file compatibility layer provides. There are also macros like
  MYOPEN that can be used in client code, to make it easier to switch
  between using host file operations and simulated filesystem operations.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include "klib/string.h"
#include "klib/list.h"
#include "files/limits.h"
#include "files/filecontext.h"

// Space to allow for a BDOS filename (which won't have a directory)
// A:ABCDEFGH.IJK -- 14 characters
#define BDOS_MAX_FNAME 14

#define MYFILE struct myfile 
#define MYFOPEN my_fopen
#define MYFCLOSE my_fclose
#define MYFWRITE my_fwrite
#define MYFREAD my_fread
#define MYFTELL my_ftell
#define MYFSEEK my_fseek
#define MYPUTC my_putc
#define MYGETC my_getc
#define MYFGETC my_getc
#define MYPRINTF printf
#define MYFFLUSH my_fflush 
#define MYFILENO(fp) fp->fd
#define MYFPRINTFERR my_fprintferr
#define FFLUSH_STDOUT fflush(stdout)
#define MYFSTAT my_fstat 
#define MYFTRUNCATE my_ftruncate
#define MYRENAME my_rename

// If FILE_COMPAT_UPPERCASE is defined, all filenames are written
//   as uppercase
#define FILE_COMPAT_UPPERCASE

// Constants for my_fnmatch()
#define MYFNM_NOMATCH     -1
#define MYFNM_LEADING_DIR 0x0001 
#define MYFNM_NOESCAPE    0x0002 
#define MYFNM_PERIOD      0x0004 
#define MYFNM_FILE_NAME   0x0008 
#define MYFNM_CASEFOLD    0x0010 

#define MYFILE struct myfile 

struct _my_DIR;
typedef struct _my_DIR my_DIR;

struct myfile
  {
  int fd;
  };


typedef struct _my_statfs
  {
  char bd_desc[BLOCKDEVICEPARAMS_MAX_DESC + 1];
  char fs_desc[FILESYSTEMPARAMS_MAX_DESC + 1];
  uint32_t block_size;
  uint32_t total_blocks;
  uint32_t free_blocks;
  } my_statfs;


/** Get the drive number from the drive letter in the full path. If
    there is none, returns the current default drive. If a drive
    letter is present, it must be valid (but it isn't case-sensitive).
    Returns the drive number from 0 - FILESYSTEM_MAX_MOUNTS, or a
    negated error code if the drive is invalid. */
extern int compat_get_drive (const char *fullpath);

/** Get the drive number from the drive letter in the full path. If
    there is none, returns -1; */
extern int compat_get_drive_no_default (const char *fullpath);

/** Returns what ever is left after the drive letter. In principle, 
    the result could be empty, but it should never be null. */
extern const char *compat_get_path (const char *filename);

/** Split a filename into drive, name, extention, in 8.3 format.
    If there is no drive, this value is set to 0. *name and *ext
    must be of length at least 9 and 4 respectively. */
extern void compat_split_drive_name_ext_83 (const char *filename,
               char *drive_letter, char *name, char *ext);

extern int            my_open (const char *filename, int flags);
extern int            my_close (int fd);
extern ssize_t        my_write (int fd, const void *buf, size_t n);
extern ssize_t        my_read (int fd, void *buf, size_t n);
extern int            my_unlink (const char *path);
extern my_DIR        *my_opendir (const char *path);
extern int            my_closedir (my_DIR *dir);
extern struct my_dirent *my_readdir (struct _my_DIR *dir);
// my_create fails if the file already exists -- I'm not sure if
// that's the usual behaviour
extern int            my_creat (const char *path, int mode);
extern uint32_t       my_lseek (int fd, uint32_t offset, int whence);
extern int            my_fstat (int fd, struct stat *statbuf);
extern int            my_stat (const char *path, struct stat *statbuf);
extern int            my_ftruncate (int fd, uint32_t length);
extern struct myfile *my_fopen (const char *filename, const char *mode);
extern int32_t        my_fwrite (const void *ptr, int32_t size, 
                              int32_t nmemb, MYFILE *stream);
extern int32_t        my_fread (void *ptr, int32_t size, 
                              int32_t nmemb, MYFILE *f);
extern int            my_fclose (MYFILE *f);
extern uint32_t       my_ftell (MYFILE *f);
extern int            my_fseek (MYFILE *f, uint32_t offset, int whence); 
extern int            my_getc (MYFILE *f);
extern int            my_putc (int c, MYFILE *f);
extern void           my_fflush (MYFILE *f);
extern void           my_fprintferr (void *f, const char *fmt, ...);
extern int            my_rename (const char *source, const char *target);


// Substitute for the glibc globbing functions
extern int my_fnmatch (const char *pattern, const char *string, int flags);
extern void compat_globber (const char *pattern, List *list);



