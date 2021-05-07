/*=========================================================================

  cpicom

  console/console.h 

  This header defines the abstract console control keys, and the
  ConsoleParams structure that all console implementations will have
  to implement.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/

#pragma once

#include <stdarg.h>
#include "error/error.h" 
#include "klib/list.h" 
#include "picocpm/picocpm.h" 

// Key codes for cursor movement, etc
#define VK_BACK     8
#define VK_TAB      9
#define VK_ENTER   10
#define VK_ESC     27
// Note that Linux console/terminal sends DEL when backspace is pressed
#define VK_DEL    127 
#define VK_DOWN  1000
#define VK_UP    1001 
#define VK_LEFT  1002 
#define VK_RIGHT 1003 
#define VK_PGUP  1004 
#define VK_PGDN  1005 
#define VK_HOME  1006 
#define VK_END   1007
#define VK_INS   1008
#define VK_CTRLUP 1009
#define VK_CTRLDOWN 1010
#define VK_CTRLLEFT 1011
#define VK_CTRLRIGHT 1012
#define VK_CTRLHOME 1013
#define VK_CTRLEND 1014
#define VK_SHIFTUP 1020
#define VK_SHIFTDOWN 1021
#define VK_SHIFTLEFT 1022
#define VK_SHIFTRIGHT 1023
#define VK_SHIFTHOME 1024
#define VK_SHIFTEND 1025
#define VK_SHIFTTAB 1026
#define VK_CTRLSHIFTUP 1030
#define VK_CTRLSHIFTDOWN 1031
#define VK_CTRLSHIFTLEFT 1032
#define VK_CTRLSHIFTRIGHT 1033
#define VK_CTRLSHIFTHOME 1034
#define VK_CTRLSHIFTEND 1035
#define VK_INTR 2000 
#define VK_EOI 2001 

//
// ConsoleProperties
// This structure specifies the size and capabilities of the terminal 
//
typedef struct _ConsoleProperties
  {
  int width;
  int height;
  } ConsoleProperties;

typedef void (*InterruptHandler) (void *interrupt_context);

typedef void (*console_out_string_fn) (void *context, const char *fmt,...);
typedef void (*console_out_string_v_fn) (void *context, 
                                           const char *fmt, va_list ap);
typedef void (*console_out_endl_fn) (void *context);
typedef void (*console_out_char_fn) (void *context, int c);
typedef Error (*console_get_line_fn) (void *context, char *buff, int max,
          int max_history, List *history);
typedef BOOL (*console_key_hit_fn) (void *context);
typedef int (*console_get_char_timeout_fn) (void *context, int timeout);
// Returns < 0 if no key waiting
typedef int (*console_peek_char_fn) (void *context);
typedef void (*console_set_interrupt_handler_fn) (void *context, 
               InterruptHandler handler, void *int_context);
typedef void (*console_get_properties_fn) (void *context, 
               ConsoleProperties *properties); 

/** Clear the screen; don't move the cursor. */
typedef void (*console_cls_fn) (void *context);

/** Move the cursor; don't change screen contents */
typedef void (*console_set_cursor_fn) (void *context, int row, int col);

// Get key NO ECHO
typedef int (*console_get_key_fn) (void *context);

//
// ConsoleParams
// This structure specifies the functions that all console handlers must
// implement (although implementations might, in some cases, be empty)
//
typedef struct _ConsoleParams 
  {
  void *context;
  console_out_string_fn console_out_string;
  console_out_string_v_fn console_out_string_v;
  console_out_endl_fn console_out_endl;
  console_out_char_fn console_out_char;
  console_get_line_fn console_get_line;
  console_get_key_fn console_get_key;
  console_get_char_timeout_fn console_get_char_timeout;
  console_peek_char_fn console_peek_char;
  console_set_interrupt_handler_fn console_set_interrupt_handler;
  console_get_properties_fn console_get_properties;
  console_cls_fn console_cls;
  console_set_cursor_fn console_set_cursor;
  } ConsoleParams;

