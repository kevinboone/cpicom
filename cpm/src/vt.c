/*============================================================================

  cpicom/cmp

  vt.c

  Functions for reading and writing the console. Many BIOS/BDOS functions
  route here. I don't know who the original author of this file is, but
  I'm guessing it's now PD. In any case, I've replaced most of the original
  logic with calls to functions in ConsoleParams.

============================================================================*/

#include "files/compat.h"
#include "console/console.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "vt.h"

int last = -1;

int constat (ConsoleParams *cp)
  {
  return (cp->console_peek_char (cp->context) >= 0);
  }

// Get a character, I'm not sure what the "w" param was originally intended
//   to do. A timeout, perhaps? Many BDOS/BIOS operations call this.
int kget (ConsoleParams *cp, int w)
  {
  (void)w;
  int c = cp->console_get_char_timeout (cp->context, -1);
  if (c == 10) c = 13; // Not sure about this
  return c; 
  }


void putch (ConsoleParams *cp, int c) 
  {
  cp->console_out_char (cp->context, c);
  }

void putmes (ConsoleParams *cp, const char *s) 
  {
  cp->console_out_string (cp->context, s);
  }

/** vt52 -- all character output from BDOS/BIOS comes through here. The
    function is so-named on the basis that most CP/M programs that do
    terminal output expect to see a VT52. If you actually have a VT52 or
    something that emulates one, this function can be reconfigured just
    to output direct to the console. If CP/M programs (the few of them
    that exist) do ANSI/VT-100, this could also be disabled, but there needs
    to be a way to do that selectably. In practice, it seems OK to leave
    this conversion in place all the time. */
void vt52 (ConsoleParams *cp, int c) {	
    //cp->console_out_char (cp->context, c);
    //return;
    static int state = 0, x, y;
    char buff[32];
    switch (state) {
    case 0:
	switch (c) {
#ifdef VBELL
        case 0x07:              /* BEL: flash screen */
            putmes (cp, "\033[?5h\033[?5l");
	    break;
#endif
	case 0x7f:		/* DEL: echo BS, space, BS */
	    putmes (cp, "\b \b");
	    break;
	case 0x1a:		/* adm3a clear screen */
	case 0x0c:		/* vt52 clear screen */
	    putmes (cp, "\033[H\033[2J");
	    break;
	case 0x1e:		/* adm3a cursor home */
	    putmes (cp, "\033[H");
	    break;
	case 0x1b:
	    state = 1;	/* esc-prefix */
	    break;
	case 1:
	    state = 2;	/* cursor motion prefix */
	    break;
	case 2:		/* insert line */
	    putmes (cp, "\033[L");
	    break;
	case 3:		/* delete line */
	    putmes (cp, "\033[M");
	    break;
	case 0x18: case 5:	/* clear to eol */
	    putmes (cp, "\033[K");
	    break;
	case 0x12: case 0x13:
	    break;
	default:
	    putch (cp, c);
	}
	break;
    case 1:	/* esc was sent */
	switch (c) {
        case 0x1b:
	    putch (cp, c);
	    break;
	case '=':
	case 'Y':
	    state = 2;
	    break;
	case 'E':	/* insert line */
	    putmes (cp, "\033[L");
	    break;
	case 'R':	/* delete line */
	    putmes (cp, "\033[M");
	    break;
	case 'B':	/* enable attribute */
	    state = 4;
	    break;
	case 'C':	/* disable attribute */
	    state = 5;
	    break;
        case 'L':       /* set line */
        case 'D':       /* delete line */
            state = 6;
            break;
	case '*':       /* set pixel */
	case ' ':       /* clear pixel */
	    state = 8;
	    break;
	default:		/* some true ANSI sequence? */
	    state = 0;
	    putch (cp, 0x1b);
	    putch (cp, c);
	}
	break;
    case 2:
	y = c - ' '+1;
	state = 3;
	break;
    case 3:
	x = c - ' '+1;
	state = 0;
	sprintf(buff, "\033[%d;%dH", y, x);
	putmes (cp, buff);
	break;
    case 4:	/* <ESC>+B prefix */
        state = 0;
        switch (c) {
	case '0': /* start reverse video */
	    putmes (cp, "\033[7m");
	    break;
	case '1': /* start half intensity */
	    putmes (cp, "\033[1m");
	    break;
	case '2': /* start blinking */
	    putmes (cp, "\033[5m");
	    break;
	case '3': /* start underlining */
	    putmes (cp, "\033[4m");
	    break;
	case '4': /* cursor on */
	    putmes (cp, "\033[?25h");
	    break;
	case '6': /* remember cursor position */
	    putmes (cp, "\033[s");
	    break;
	case '5': /* video mode on */
	case '7': /* preserve status line */
	    break;
	default:
	    putch (cp, 0x1b);
	    putch (cp, 'B');
	    putch (cp, c);
        }
	break;
    case 5:	/* <ESC>+C prefix */
        state = 0;
        switch (c) {
	case '0': /* stop reverse video */
	    putmes (cp, "\033[27m");
	    break;
	case '1': /* stop half intensity */
	    putmes (cp, "\033[m");
	    break;
	case '2': /* stop blinking */
	    putmes (cp, "\033[25m");
	    break;
	case '3': /* stop underlining */
	    putmes (cp, "\033[24m");
	    break;
	case '4': /* cursor off */
	    putmes (cp, "\033[?25l");
	    break;
	case '6': /* restore cursor position */
	    putmes (cp, "\033[u");
	    break;
	case '5': /* video mode off */
	case '7': /* don't preserve status line */
	    break;
	default:
	    putch (cp, 0x1b);
	    putch (cp, 'C');
	    putch (cp, c);
        }
	break;
/* set/clear line/point */
    case 6:
    case 7:
    case 8:
        state ++;
	break;
    case 9:
	state = 0;
    } 
}
