/*=========================================================================
 
  cpicom

  console/console_stdio_vt100.c

  A console for CPICOM based on the Pico's built-in stdin/stdout handlers,
  and assumed to be connected to something that emulates a VT100 terminal.

  Copyright (c)2001 Kevin Boone, GPL v3.0

  =========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log/log.h"
#include "klib/string.h"
#include <ctype.h>
#include "console/console.h"
#include "console/console_stdio_vt100.h"
#include "pico/stdlib.h" 
#include "picocpm/picocpm.h" 
#include "picocpm/config.h" 
#if PICO_ON_DEVICE
#else
#include <termios.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Define the ASCII code that the terminal will transmit to erase
//  the last character.
#if PICO_ON_DEVICE
#define I_BACKSPACE 8
#else
#define I_BACKSPACE 127
#endif

// Define the ASCII code that we transmit to the terminal to erase
//  the last character. This isn't necessarily the same as I_BACKSPACE
#if PICO_ON_DEVICE
#define O_BACKSPACE 8
#else
#define O_BACKSPACE 8 
#endif

// Define the ASCII code that the terminal will transmit to delete 
//  the last character (if it sends a code at all -- some transmit a
//  CSI sequence)
#define I_DEL 127

// Serial code for "interrupt" on the terminal. There usually isn't one,
// and we usually hit ctrl+c (ASCII 3)
#define I_INTR 3

// Serial code for "end of input" on the terminal. There usually isn't one,
// and we usually hit ctrl+d (ASCII 4)
#define I_EOI 4

// Serial code for "pause transmission", usually ctrl-S.
#define I_PAUSE 19

// Serial code for "resume transmission", usually ctrl-Q.
#define I_RESUME 17

// Define this if your terminal does not rub out the previous character
//   when it receives a backspace from the Pico. This setting has the
//   effect that PMBASIC will send backspace-space-backspace.
#define I_EMIT_DESTRUCTIVE_BACKSPACE

// Define how to send a end-of-line to the terminal. \n=10,
//  \r=13. Some terminals automatically expand a \n into \r\n, but most
//  do not by default.
#define I_ENDL "\r\n"

// The character that indicates an end-of-line -- usually 13 or 10 (or both)
#if PICO_ON_DEVICE
#define I_EOL 13
#else
#define I_EOL 10
#endif

// Time in milliseconds we wait after the escape key is pressed, to see
//   if it's just an escape, or the start of a CSI sequence. There is no
//   right answer here -- it depends on baud rate and other factors. Longer
//   is safer, but might irritate the user
#define I_ESC_TIMEOUT 100

// TODO put last_char in the Context when all the users of the console
//  functions are actually able to supply a context :/
int last_char = PICO_ERROR_TIMEOUT;

struct _ConsoleStdioVT100 
  {
  InterruptHandler interrupt_handler;
  void *interrupt_context;
  Config *config;
#if PICO_ON_DEVICE
#else
  struct termios orig_termios;
#endif
  };

ConsoleStdioVT100 *consolestdiovt100_create (void)
  {
  ConsoleStdioVT100 *self = malloc (sizeof (ConsoleStdioVT100));
  memset (self, 0, sizeof (ConsoleStdioVT100));
#if PICO_ON_DEVICE
#else
  tcgetattr (STDIN_FILENO, &self->orig_termios);
  struct termios raw = self->orig_termios;
  raw.c_iflag &= (unsigned int) ~(IXON);
  raw.c_lflag &= (unsigned int) ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VTIME] = 1;
  raw.c_cc[VMIN] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &raw);
#endif
  return self;
  }

// 
// consolestdiovt100_destroy
//
void consolestdiovt100_destroy (ConsoleStdioVT100 *self)
  {
  if (self)
    {
#if PICO_ON_DEVICE
#else
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &self->orig_termios);
#endif
    free (self);
    }
  }

// 
// consolestdiovt100_out_string
//
void consolestdiovt100_out_string (void *cp, const char *fmt,...)
  {
  (void)cp;
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  fflush (stdout);
  }

// 
// consolestdiovt100_out_string_v
//
void consolestdiovt100_out_string_v (void *cp, 
        const char *fmt, va_list ap)
  {
  (void)cp;
  vprintf (fmt, ap);
  fflush (stdout);
  }

// 
// consolestdiovt100_out_endl
//
void consolestdiovt100_out_endl (void *cp)
  {
  (void)cp;
  puts ("\r"); // \n should be automatic
  }

// 
// consolestdiovt100_peek_char
//
int consolestdiovt100_peek_char (void *context)
  {
  ConsoleStdioVT100 *self = (ConsoleStdioVT100 *)context;
  if (last_char < 0)
    {
    last_char = getchar_timeout_us (0); 
    sleep_us (250);
    // On the Pico we need _some_ value in here, or this function somehow
    // ends up return -1 all the time. But many programs epxect 
    // console-peeking to be fast, because it's done all the time. So
    // if the delay is too long, it seriously impacts speed.
    }
  if (last_char < -1) last_char = -1;
  if (last_char == I_INTR) 
    {
    if (self->interrupt_handler)
      self->interrupt_handler (self->interrupt_context); 
    }
  else if (last_char == I_PAUSE && self->config && self->config->flow_control) 
    {
    int c;
    last_char = -1;
    while ((c = getchar_timeout_us (250)) < 0){};
    if (c == I_INTR) 
      {
      if (self->interrupt_handler)
        self->interrupt_handler (self->interrupt_context); 
      }
    }
  return last_char;
  }

// 
// consolestdiovt100_out_char
//
void consolestdiovt100_out_char (void *context, int c)
  {
  (void)context;
  //ConsoleStdioVT100 *self = (ConsoleStdioVT100 *)context;
#if PICO_ON_DEVICE
  putchar (c);
#else
  putchar (c);
  fflush (stdout);
#endif
  //int cc = consolestdiovt100_peek_char (context);
  //if (cc == I_PAUSE)
  //  consolestdiv 
  }

// 
// consolestdiovt100_get_char
//
int consolestdiovt100_get_char (void *context)
  {
  (void)context;
  int c;
  while ((c = consolestdiovt100_peek_char (context)) < 0)
    {
    }
  last_char = PICO_ERROR_TIMEOUT;
  return c;
  }

// 
// consolestdiovt100_get_char_timeout
//
int consolestdiovt100_get_char_timeout (void *context, int msec)
  {
  (void)context;
  if (msec < 0) 
    {
    return consolestdiovt100_get_char (context);
    }
#if PICO_ON_DEVICE
  int c;
  int loops = 0;
  while ((c = getchar_timeout_us (0)) < 0 && loops < msec)
    {
    sleep_us (1000);
    loops++;
    }
  return c;
#else
  (void)msec;
  int c;
  int loops = 0;
  while ((c = getchar ()) < 0 && loops < msec)
    {
    usleep (1000);
    loops++;
    }
  return c;
#endif
  }

// 
// consolestdiovt100_get_key
//
int consolestdiovt100_get_key (void *context)
  {
  (void)context;
  char c = (char)consolestdiovt100_get_char (context);
  if (c == '\x1b') 
    {
    char c1 = (char)consolestdiovt100_get_char_timeout  
        (context, I_ESC_TIMEOUT);
    if (c1 == 0xFF) return VK_ESC;
    //printf ("c1 a =%d %c\n", c1, c1);
    if (c1 == '[') 
      {
      char c2 = (char) consolestdiovt100_get_char (context);
      //printf ("c2 a =%d %c\n", c2, c2);
      if (c2 >= '0' && c2 <= '9') 
        {
        char c3 = (char) consolestdiovt100_get_char (context);
        //printf ("c3 a =%d %c\n", c3, c3);
        if (c3 == '~') 
          {
          switch (c2) 
            {
            case '0': return VK_END;
            case '1': return VK_HOME;
            case '2': return VK_INS;
            case '3': return VK_DEL; // Usually the key marked "del"
            case '5': return VK_PGUP;
            case '6': return VK_PGDN;
            }
          }
        else if (c3 == ';')
          {
          if (c2 == '1') 
            {
            char c4 = (char) 
	       consolestdiovt100_get_char (context); // Modifier
            char c5 = (char) 
	       consolestdiovt100_get_char (context); // Direction
            //printf ("c4 b =%d %c\n", c4, c4);
            //printf ("c5 b =%d %c\n", c5, c5);
            if (c4 == '5') // ctrl
              {
              switch (c5) 
        	{
        	case 'A': return VK_CTRLUP;
        	case 'B': return VK_CTRLDOWN;
        	case 'C': return VK_CTRLRIGHT;
        	case 'D': return VK_CTRLLEFT;
        	case 'H': return VK_CTRLHOME;
        	case 'F': return VK_CTRLEND;
        	}
              }
            else if (c4 == '2') // shift 
              {
              switch (c5) 
        	{
        	case 'A': return VK_SHIFTUP;
        	case 'B': return VK_SHIFTDOWN;
        	case 'C': return VK_SHIFTRIGHT;
        	case 'D': return VK_SHIFTLEFT;
        	case 'H': return VK_SHIFTHOME;
        	case 'F': return VK_SHIFTEND;
        	}
              }
            else if (c4 == '6') // shift-ctrl
              {
              switch (c5) 
        	{
        	case 'A': return VK_CTRLSHIFTUP;
        	case 'B': return VK_CTRLSHIFTDOWN;
        	case 'C': return VK_CTRLSHIFTRIGHT;
        	case 'D': return VK_CTRLSHIFTLEFT;
        	case 'H': return VK_CTRLSHIFTHOME;
        	case 'F': return VK_CTRLSHIFTEND;
        	}
              }
            else 
              {
              // Any other modifier, we don't support. Just return
              //   the raw direction code
              switch (c5) 
        	{
        	case 'A': return VK_UP;
        	case 'B': return VK_DOWN;
        	case 'C': return VK_RIGHT;
        	case 'D': return VK_LEFT;
        	case 'H': return VK_HOME;
        	case 'F': return VK_END;
        	}
              }
            }
          else return VK_ESC;
          }
        }
      else
        {
        //printf ("c2 b =%d %c\n", c2, c2);
        switch (c2) 
          {
          case 'A': return VK_UP;
          case 'B': return VK_DOWN;
          case 'C': return VK_RIGHT;
          case 'D': return VK_LEFT;
          case 'H': return VK_HOME;
          case 'F': return VK_END;
          case 'Z': return VK_SHIFTTAB;
          }
        }
      }
    return '\x1b';
    } 
 else 
    {
    if (c == I_BACKSPACE) return VK_BACK;
    if (c == I_DEL) return VK_DEL;
    if (c == I_INTR) return VK_INTR;
    if (c == I_EOI) return VK_EOI;
    if (c == I_EOL) return VK_ENTER;
    return c;
    } 
  }

//
// consolestdiovt100_add_line_to_history 
//
void consolestdiovt100_add_line_to_history (List *history, int max_history, 
        const char *buff)
  {
  BOOL should_add = TRUE;
  int l = list_length (history);
  if (l < max_history)
    {
    for (int i = 0; i < l && should_add; i++)
      {
      if (strcmp (buff, list_get (history, i)) == 0)
        should_add = FALSE;
      }
    }

  if (should_add)
    {
    if (l >= max_history)
      {
      const char *s = list_get (history, 0);
      list_remove_object (history, s);
      }
    list_append (history, strdup (buff));
    }
  }

// 
// consolestdiovt100_get_line
//
Error consolestdiovt100_get_line (void *context, char *buff, int len, 
        int max_history, List *history)
  {
  ConsoleParams *cp = (ConsoleParams *)context;
  int pos = 0;
  BOOL done = 0;
  BOOL got_line = TRUE;
  BOOL interrupt = FALSE;
  // The main input buffer
  String *sbuff = string_create_empty ();
  // A copy of the main input buffer, taken when we up-arrow back
  //  into the history. We might need to restore this on a down-arrow
  char *tempstr = NULL;

  int histpos = -1;

  while (!done)
    {
    int c = consolestdiovt100_get_key (context);
    if (c == VK_INTR)
      {
      got_line = TRUE;
      done = TRUE;
      interrupt = TRUE;
      }
    else if (c == VK_EOI) 
      {
      got_line = FALSE;
      done = TRUE;
      }
    else if (c == VK_DEL || c == VK_BACK)
      {
      if (pos > 0) 
        {
        pos--;
        string_delete_c_at (sbuff, pos);
        consolestdiovt100_out_char (context, O_BACKSPACE);
        const char *s = string_cstr (sbuff);
        int l = string_length (sbuff);
        for (int i = pos; i < l; i++)
          {
          consolestdiovt100_out_char (context, s[i]);
          }
        consolestdiovt100_out_char (context, ' ');
        for (int i = pos; i <= l; i++)
          {
          consolestdiovt100_out_char (context, O_BACKSPACE);
          }
        }
      }
    else if (c == VK_ENTER)
      {
      //buff[pos] = 0;
      done = 1;
      }
    else if (c == VK_LEFT)
      {
      if (pos > 0)
        {
        pos--;
        consolestdiovt100_out_char (context, O_BACKSPACE);
        }
      }
    else if (c == VK_CTRLLEFT)
      {
      if (pos == 1)
        {
        pos = 0;
        consolestdiovt100_out_char (context, O_BACKSPACE);
        }
      else
        {
        const char *s = string_cstr (sbuff);
        while (pos > 0 && isspace (s[(pos - 1)]))
          {
          pos--;
          consolestdiovt100_out_char (context, O_BACKSPACE);
          }
        while (pos > 0 && !isspace (s[pos - 1]))
          {
          pos--;
          consolestdiovt100_out_char (context, O_BACKSPACE);
          }
        }
      }
    else if (c == VK_CTRLRIGHT)
      {
      const char *s = string_cstr (sbuff);

      while (s[pos] != 0 && !isspace (s[pos]))
        {
        consolestdiovt100_out_char (context, s[pos]);
        pos++;
        }
      while (s[pos] != 0 && isspace (s[pos]))
        {
        consolestdiovt100_out_char (context, s[pos]);
        pos++;
        }
      }
    else if (c == VK_RIGHT)
      {
      const char *s = string_cstr (sbuff);
      int l = string_length (sbuff);
      if (pos < l)
        {
        consolestdiovt100_out_char (context, s[pos]);
        pos++;
        }
      }
    else if (c == VK_UP)
      {
      if (!history) continue;
      if (histpos == 0) continue;
      int histlen = list_length (history);
      if (histlen == 0) continue;
      //printf ("histlen=%d histpos=%d\n", histlen, histpos);

      if (histpos == -1)
        {
        // We are stepping out of the main line, into the
        //  top of the history
        if (!tempstr)
            tempstr = strdup (string_cstr (sbuff));
        histpos = histlen - 1;
         }
      else
        {
        histpos --;
        }

      int oldlen = string_length (sbuff);
      const char *newline = list_get (history, histpos); 
      int newlen = strlen (newline);
      // Move to the start of the line 
       for (int i = 0; i < pos; i++)
        consolestdiovt100_out_char (context, O_BACKSPACE);
      // Write the new line
      for (int i = 0; i < newlen; i++)
        consolestdiovt100_out_char (context, newline[i]);
      // Erase from the end of the new line to the end of the old 
      for (int i = newlen; i < oldlen; i++)
        consolestdiovt100_out_char (context, ' ');
      for (int i = newlen; i < oldlen; i++)
        consolestdiovt100_out_char (context, O_BACKSPACE);
      pos = newlen;
      string_destroy (sbuff);
      sbuff = string_create (newline);
      }
    else if (c == VK_DOWN)
      {
      if (!history) continue;
      int histlen = list_length (history);
      if (histpos < 0) continue; 
      char *newline = "";
      BOOL restored_temp = FALSE;
      if (histpos == histlen - 1)
        {
        // We're about to move off the end of the history, back to 
        //   the main line. So restore the main line, if there is 
        //   one, or just set it to "" if there is not
        histpos = -1;
        if (tempstr)
          {
          newline = tempstr;
          restored_temp = TRUE;
          }
        }
      else
        {
        restored_temp = FALSE;
        histpos++;
        newline = list_get (history, histpos); 
        }

      int oldlen = string_length (sbuff);
      int newlen = strlen (newline);
      // Move to the start of the line 
       for (int i = 0; i < pos; i++)
          consolestdiovt100_out_char (context, O_BACKSPACE);
      // Write the new line
      for (int i = 0; i < newlen; i++)
          consolestdiovt100_out_char (context, newline[i]);
        // Erase from the end of the new line to the end of the old 
      for (int i = newlen; i < oldlen; i++)
          consolestdiovt100_out_char (context, ' ');
      for (int i = newlen; i < oldlen; i++)
          consolestdiovt100_out_char (context, O_BACKSPACE);
      pos = newlen;
      string_destroy (sbuff);
      sbuff = string_create (newline);
      if (restored_temp)
        {
        free (tempstr); 
        tempstr = NULL;
        }
      }
    else if (c == VK_HOME)
      {
      if (pos > 0)
        {
        for (int i = 0; i < pos; i++)
          consolestdiovt100_out_char (context, O_BACKSPACE);
        pos = 0;
        }
      }
    else if (c == VK_END)
      {
      const char *s = string_cstr (sbuff);
      int l = string_length (sbuff);
      for (int i = pos; i < l; i++)
          consolestdiovt100_out_char (context, s[i]);
      pos = l;
      }
    else
      {
      int l = string_length (sbuff);

      if (l < len - 1) // Leave room for null
        {
        if ((c < 256 && c >= 32) || (c == 8))
          {
          string_insert_c_at (sbuff, pos, (char)c);
          pos++;
          int l = string_length (sbuff);
          if (pos >= l - 1)
            consolestdiovt100_out_char (context, (char)c); 
          else
            {
            const char *s = string_cstr (sbuff);
            for (int i = pos - 1; i < l; i++)
              consolestdiovt100_out_char (context, s[i]); 
            for (int i = pos; i < l; i++)
              consolestdiovt100_out_char (context, O_BACKSPACE); 
            }
          }
        }
      }
    }

  strncpy (buff, string_cstr(sbuff), len);
  string_destroy (sbuff);
   //printf ("buff='%s'\n", buff);

  if (tempstr) free (tempstr);

  consolestdiovt100_out_endl (cp->context); 
  histpos = -1; 

  if (interrupt)
    return ERROR_INTERRUPTED;

  if (got_line)
    {
    if (history)
      {
      consolestdiovt100_add_line_to_history (history, max_history, buff);
      }
    return 0;
    }
  else
    return ERROR_ENDOFINPUT;
  }

//
// consolestdiovt100_set_interrupt_handler
//
void consolestdiovt100_set_interrupt_handler (void *context, 
       InterruptHandler handler, void *int_context)
  {
  ConsoleStdioVT100 *self = (ConsoleStdioVT100 *)context;
  self->interrupt_handler = handler;
  self->interrupt_context = int_context;
  }

// 
// consolestdiovt100_set_config
//
void consolestdiovt100_set_config (ConsoleStdioVT100 *self, Config *config)
  {
  self->config = config;
  }

// 
// consolestdiovt100_get_params
//
void consolestdiovt100_get_params (ConsoleStdioVT100 *self, 
       ConsoleParams *params) 
  {
  params->context = self;
  params->console_out_string = consolestdiovt100_out_string; 
  params->console_out_string_v = consolestdiovt100_out_string_v; 
  params->console_out_endl = consolestdiovt100_out_endl; 
  params->console_out_char = consolestdiovt100_out_char; 
  params->console_get_line = consolestdiovt100_get_line; 
  params->console_get_key = consolestdiovt100_get_key; 
  params->console_peek_char = consolestdiovt100_peek_char; 
  params->console_get_char_timeout = 
     consolestdiovt100_get_char_timeout; 
  params->console_set_interrupt_handler = 
     consolestdiovt100_set_interrupt_handler;
  }


