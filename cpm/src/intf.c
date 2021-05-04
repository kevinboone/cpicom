/*============================================================================

  cpicom/cmp

  intf.c

  Various functions from the Z80 emulator route here, for dealing with
  communication. To be honest, I'm not sure how often these functions
  are actually used (e.g., direct I/O on input port 0 to read a character).

 Copyright 1986-1988 by Parag Patel.  All Rights Reserved.
 Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved. 
 Now believed to be in the public domain. Modifications for CPICOM
 by Kevin Boone, 2021.

============================================================================*/

#include "files/compat.h"
#include "cpm/limits.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "cpm/defs.h"
#include "vt.h"

#include <unistd.h>

extern int errno;

/* input  --  z80 input instruction  --  this function is called whenever
   an input port is referenced from the z80 to handle the real I/O  --
   it returns a byte to the z80 just like the real I/O instruction  --
   the arguments represent the data on the bus as it would be for a real
   z80 - this routine is restarted later if there is no input pending,
   and we must wait for some to occur */

boolean input (z80info *z80, byte haddr, byte laddr, byte *val)
{
        (void)z80;
        ConsoleParams *cp = z80->console_params;
	unsigned int data;

	/* just uses the lower 8-bits of the I/O address for now... */
	switch (laddr)
	{

	/* return a character from the keyboard - wait for it if necessary  --
	   return "last" if we have already read in something via 0x01 */
	case 0x00:
		if (1)
		{
			fflush(stdout);
			data = kget (cp, 0);
			/* data = getchar(); */

			while ((data > 0x7f && errno == EINTR) ||
					data == INTR_CHAR)
			{
				data = kget (cp, 0);
				/* data = getchar(); */
			}
		}

		*val = data & 0x7F;
		break;

	/* return 0xFF if we have a character waiting to be read - save the
	   character in "last" for 0x00 above */
	case 0x01:
		fflush(stdout);

		if (constat (cp))
			*val = 0xFF;
		else
			*val = 0x00;

		break;

	/* default - prompt the user for an input byte */
	default:
		//resetterm();
		MYPRINTF("INPUT : addr = %X%X    DATA = ", haddr, laddr);
		fflush(stdout);
		scanf("%x", &data);
		//setterm();
		*val = data;
		break;
	}

	return TRUE;
}


/*-----------------------------------------------------------------------*\
 |  output  --  output the data at the specified I/O address
\*-----------------------------------------------------------------------*/

void
output(z80info *z80, byte haddr, byte laddr, byte data)
{
  ConsoleParams *cp = z80->console_params;

	if (laddr == 0xFF) {
		/* BIOS call - interrupt the z80 before the next instruction
		   since we may have to mess with the PC & other stuff -
		   otherwise we would do it right here */
		z80->event = TRUE;
		z80->halt = TRUE;
		z80->syscall = TRUE;
		z80->biosfn = data;

		if (z80->trace)
		{
			MYPRINTF("BIOS call %d\r\n", z80->biosfn);

		}
	} else if (laddr == 0) {
		/* output a character to the screen */
		/* putchar(data); */
		vt52 (cp, data);

	} else {
		/* Unimplemented -- just dump the data. */
		MYPRINTF ("Unimplemented OUTPUT: addr = %X%X  DATA = %X\r\n", haddr, laddr,data);
	}
}


/*-----------------------------------------------------------------------*\
 |  haltcpu  --  this is called after the z80 halts  --  it is used for
 |  tracing & such
\*-----------------------------------------------------------------------*/

void haltcpu (z80info *z80)
  {
  z80->halt = FALSE;

  /* we are tracing execution of the z80 */
  if (z80->trace)
    {
    /* re-enable tracing */
    z80->event = TRUE;
    z80->halt = TRUE;
    }

  /* a CP/M syscall - done here so tracing still works */
  if (z80->syscall)
    {
    z80->syscall = FALSE;
    bios (z80, z80->biosfn);
    }
  }

void undefinstr (z80info *z80, byte instr)
  {
  MYFPRINTFERR (stderr, "\r\nIllegal instruction 0x%.2X at PC=0x%.4X\r\n",
      instr, PC - 1);
  }

uint8_t getdrive (const z80info *z80)
  {
  (void)z80;
  return filecontext_global_get_current_drive ();
  }

uint8_t setdrive (const z80info *z80, uint8_t drive)
  {
  (void)z80;
  filecontext_global_set_current_drive (drive);
  return 0;
  }

int cpm_runprog (ConsoleParams *console_params, const char *cmdline, 
      z80info **z80_info)
  {
  char buff[CPM_MAX_CMDLINE + 1];
  strncpy (buff, cmdline, CPM_MAX_CMDLINE);
  buff [CPM_MAX_CMDLINE] = 0;
  stuff_cmd = buff;

  z80info *z80 = new_z80info();
  z80->console_params = console_params;
  *z80_info = z80;

  if (z80 == NULL)
    return -1;

  sysreset(z80);

  z80->finished = FALSE;
  while (!z80->finished)
    {
    z80_emulator (z80, 100000);
    }
  delete_z80info (z80);
  *z80_info = NULL;
  cleanupfp (); //KB -- cleanup file pointers that are still open
  return 0;
  }

