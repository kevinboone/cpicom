/*=========================================================================

  cpicom

  files/filecontext.h

  FileContext is the entry point for all the Pico file handling 
  functions. There are equivalents of many, but certainly not all,
  the standard POSIX functions. FileContext understands the idea of
  a numbered "drive". Each drive corresponds to a pairing between
  a fileystem (LittleFS, VFAT) and a specific block device. FileContext
  maintains a table of these drives, with their corresponding filesystem.
  The actual file operations are simple delegated to the filesystem, when
  the filesystem handler has been determined from the drive.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "klib/defs.h"
#include "error/error.h"
#include "files/compat.h"
#include "fs/filesystem.h"
#include "files/filesystemmount.h"
#include "blockdevice/blockdevice.h"

#define MAX_OPEN_FILES 64 
#define MAX_FNAME 255
#define MAX_PATH 255

#ifndef DT_DIR_
#define DT_DIR_ 4
#endif

#ifndef DT_REG
#define DT_REG_ 8
#endif

typedef struct _OpenFileInfo
  {
  BOOL in_use;
  struct _FilesystemMount *mount;
  char *path;
  int drive;
  void *opaque1;
  void *opaque2;
  } OpenFileInfo;

// my_dirent structure is a simple replacement for dirent, as used by
//   readdir, etc.
struct my_dirent
  {
  char d_name[256];
  uint8_t d_type;
  };

// my_DIR structure is a simple replacement for DIR, as used by
//   opendir, etc.
typedef struct _my_DIR
  {
  int drive;
  void *opaque1;
  struct my_dirent current_de;
  } my_DIR;


struct _FilesystemParams;
struct _FileContext;
struct _my_dirent;
struct _my_DIR;
struct _my_statfs;
typedef struct _FileContext FileContext;

// Global filesystem context
extern FileContext *global_fc;

extern FileContext *filecontext_create (void);
extern void         filecontext_destroy (FileContext *self);

extern Error        filecontext_unmount (FileContext *self, uint8_t drive_num);
extern void         filecontext_unmount_all (FileContext *self);

extern Error        filecontext_mount (FileContext *self, uint8_t drive_num, 
                       struct _FilesystemParams *fs_params, 
                       BlockDeviceParams *bd_params);

extern Error        filecontext_format (FileContext *self, uint8_t drive_num);

// Sets errno, returns fd or -1
extern int          filecontext_open (FileContext *self, uint8_t drive_num, 
                        const char *name, int flags);

// Sets errno, returns size or -1
extern ssize_t      filecontext_write (FileContext *self, int fd, 
                      const void *buf, size_t n);

// Sets errno, return 0 or -1
extern int          filecontext_close (FileContext *self, int fd);

extern Error        filecontext_delete (FileContext *self, uint8_t drive, 
                       const char *filename);

// Sets errno
extern my_DIR *     filecontext_opendir (FileContext *self, uint8_t drive, 
                       const char *path);

extern Error        filecontext_closedir (FileContext *self, 
                        my_DIR *dir);

extern struct my_dirent *filecontext_readdir (FileContext *self, 
                        struct _my_DIR *dir);

extern const OpenFileInfo *filecontext_get_ofi 
                        (const FileContext *self, int i);

/** Gets the size of a file, if it exists. 
    Sets errno, returns size or (uint32_t)-1 */
extern uint32_t     filecontext_size (const FileContext *self, 
                        uint8_t drive, const char *path);

extern Error        filecontext_fstat (const FileContext *self, int fd, 
                        struct stat *sb);

extern Error        filecontext_ftruncate (FileContext *self, int fd, 
                        uint32_t length);

extern Error        filecontext_rename (const FileContext *self,
                            uint8_t drive, 
                            const char *source_path, const char *target_path);

extern Error        filecontext_dstatfs (const FileContext *self, 
                            uint8_t drive, 
                            struct _my_statfs *statfs);

/** Prepares a drive for first use (if preparation is actually required).
    Checks that the drive is actually usable. Returns an error code if
    it isn't. */
extern Error        filecontext_activate_drive (FileContext *self, 
                            uint8_t drive_num);

extern Error        filecontext_stat (const FileContext *self, 
                       uint8_t drive_num, const char *path, struct stat *sb);

// Returns 0-16
extern int          filecontext_global_get_current_drive (void);
void                filecontext_global_set_current_drive (uint8_t drive);

// Sets errno, return fd or -1
extern int          filecontext_global_open (uint8_t drive, 
                            const char *path, int flags);

/** Gets the size of a file, if it exists. 
    Sets errno, returns size or (uint32_t)-1 */
extern uint32_t     filecontext_global_size (uint8_t drive, 
                             const char *path);

// Sets errno, return 0 or -1
extern int          filecontext_global_close (int fd);

extern ssize_t      filecontext_global_write (int fd, const void *buf, 
                      size_t n);

extern ssize_t      filecontext_global_read (int fd, void *buf, 
                      size_t n);

extern Error        filecontext_global_delete (uint8_t drive, 
                      const char *filename);

extern my_DIR *     filecontext_global_opendir (uint8_t drive, 
                       const char *path);

extern Error        filecontext_global_closedir (my_DIR *dir);

extern struct my_dirent   *filecontext_global_readdir (struct _my_DIR *dir);

extern const OpenFileInfo *filecontext_global_get_ofi (int i);

// Sets errno
extern uint32_t     filecontext_global_lseek (int fd, uint32_t offset, 
                      int whence);

extern Error        filecontext_global_fstat (int fd, struct stat *sb);

extern Error        filecontext_global_ftruncate (int fd, uint32_t length);

extern Error        filecontext_global_rename (uint8_t drive, 
                            const char *source_path, const char *target_path);

extern Error        filecontext_global_dstatfs (uint8_t drive, 
                            struct _my_statfs *statfs);

extern Error        filecontext_global_format (uint8_t drive_num);

/** See filecontext_activate_drive for details. */
extern Error        filecontext_global_activate_drive (uint8_t drive_num);

extern Error        filecontext_global_stat (uint8_t drive_num, 
                       const char *path, struct stat *sb);



