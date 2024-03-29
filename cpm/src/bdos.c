/* BDOS emulation */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "files/compat.h"
#include "console/console.h"
#include "cpm/limits.h"
#include "cpm/defs.h"
#include "vt.h"

#define BIOS 0xFE00
#define DPH0 (BIOS + 0x0036)
#define DPB0 (DPH0 + 0x0010)
#define DIRBUF 0xff80
#define CPMLIBDIR "./"

#define NUMFPS 100

static int storedfps = 0;
unsigned short usercode = 0x00;
int restricted_mode = 0;
int silent_exit = 0;
char *stuff_cmd = 0;
int trace_bdos = 0;

/* Kill CP/M command line prompt */

static void killprompt (ConsoleParams *cp)
  {
  vt52 (cp,'\b');
  vt52 (cp,' ');
  vt52 (cp,'\b');
  vt52 (cp,'\b');
  vt52 (cp,' ');
  vt52 (cp,'\b');
  }

char *rdcmdline (z80info *z80, int max, int ctrl_c_enable)
  {
  int i, c;
  static char s[CPM_MAX_READ_LINE + 3];
  ConsoleParams *cp = z80->console_params;

  FFLUSH_STDOUT; 
  max &= 0xff;
  i = 1;      /* number of next character */

  if (stuff_cmd) 
    {
    killprompt (cp);
    strcpy (s + i, stuff_cmd);
    i = 1 + strlen (s + i);
    stuff_cmd = 0;
    silent_exit = 1;
    s[0] = i-1;
    s[i] = 0;
    if (i <= max)
      s[i] = '\r';
    return s;
    //goto hit_rtn;
    } 

  Error err = cp->console_get_line (cp->context, s + 1, max, 0, NULL);
  if (err == ERROR_INTERRUPTED)
    {
    z80->regpc = BIOS + 3;
    s[0] = 0;
    }
  else
    s[0] = strlen (s);

  return s;

loop:
    c = kget (cp, 0);
    if (c == INTR_CHAR) {
        MYPRINTF ("INTERRUPT -- TODO");	
    	i = 1;
    	s[0] = i - 1;
    	s[i] = 0;
    	return s;
    } else if (c < ' ' || c == 0x7f) {
        switch (c) {
	case 3:
	    if (ctrl_c_enable) {
		vt52 (cp,'^');
		vt52 (cp,'C');
		z80->regpc = BIOS+3;
		s[0] = 0;
		return s;
	    }
	    break;
        case 8:
	case 0x7f:
	    if (i > 1) {
		--i;
		vt52 (cp,'\b');
		vt52 (cp,' ');
		vt52 (cp,'\b');
		FFLUSH_STDOUT;
	    }
	    break;
        case '\n':
        case '\r':
	    s[0] = i-1;
	    s[i] = 0;
	    if (!strcmp(s + 1, "bye")) {
	    	MYPRINTF("\r\n");
	    	finish(z80);
		goto finished;
	    }
	    if (i <= max)
		s[i] = '\r';
	    return s;
        }
        goto loop;
    } else if (i <= max) {
        s[i++] = c;
        vt52 (cp, c);
	FFLUSH_STDOUT;
    }
    goto loop;

finished:
  return "";
}


#if 0
static struct FCB {
    char drive;
    char name[11];
    char data[24];
} samplefcb;
#endif

static void FCB_to_filename(unsigned char *p, char *name) {
    int i;
    char *org = name;
    /* strcpy(name, "test/");
       name += 5; */
    for (i = 0; i < 8; ++i)
	if (p[i+1] != ' ')
	    *name++ = tolower(p[i+1]);
    if (p[9] != ' ') {
	*name++ = '.';
	for (i = 0; i < 3; ++i)
	    if (p[i+9] != ' ')
		*name++ = tolower(p[i+9]);
    }
    *name = '\0';
    if (trace_bdos)
    	MYPRINTF("File name is %s\r\n", org);
}

static void FCB_to_ufilename(unsigned char *p, char *name) {
    int i;
    char *org = name;
    /* strcpy(name, "test/");
       name += 5; */
    for (i = 0; i < 8; ++i)
	if (p[i+1] != ' ')
	    *name++ = toupper(p[i+1]);
    if (p[9] != ' ') {
	*name++ = '.';
	for (i = 0; i < 3; ++i)
	    if (p[i+9] != ' ')
		*name++ = toupper(p[i+9]);
    }
    *name = '\0';
    if (trace_bdos)
    	MYPRINTF("File name is %s\r\n", org);
}

static struct stfps {
    MYFILE *fp;
    unsigned where;
    char name[12];
} stfps[NUMFPS];


void cleanupfp (void) /* KB */
  {
  int i;
  for (i = 0; i < NUMFPS; i++)
    {
    if (stfps[i].where != 0 && stfps[i].where != 0xFFFFU)
      {
      MYFCLOSE (stfps[i].fp); 
      stfps[i].where = 0;
      }
    stfps[i].fp = NULL;
    }
  }


static void storefp(z80info *z80, MYFILE *fp, unsigned where) {
    int i;
    int ind = -1;
    for (i = 0; i < storedfps; ++i)
	if (stfps[i].where == 0xffffU)
	    ind = i;
	else if (stfps[i].where == where) {
	    ind = i;
	    goto putfp;
	}
    if (ind < 0) {
	if (++storedfps > NUMFPS) {
	    MYFPRINTFERR(stderr, "out of fp stores!\n");
            // TODO
	    exit(1);
	}
	ind = storedfps - 1;
    }
    stfps[ind].where = where;
 putfp:
    stfps[ind].fp = fp;
    memcpy(stfps[ind].name, z80->mem+z80->regde+1, 11);
    stfps[ind].name[11] = '\0';
}

/* Lookup an FCB to find the host file. */

static MYFILE *lookfp(z80info *z80, unsigned where) {
    int i;
    for (i = 0; i < storedfps; ++i)
	if (stfps[i].where == where)
            if (memcmp(stfps[i].name, z80->mem+z80->regde+1, 11) == 0)
	    return stfps[i].fp;
    /* fcb not found. maybe it has been moved? */
    for (i = 0; i < storedfps; ++i)
	if (stfps[i].where != 0xffffU &&
	    !memcmp(z80->mem+z80->regde+1, stfps[i].name, 11)) {
	    stfps[i].where = where;	/* moved FCB */
	    return stfps[i].fp;
	}
    return NULL;
}

static int flushallfps() {
    int i;
    for(i = 0; i < storedfps; ++i)
        if( stfps[i].fp !=0 ) {
	    int retv = my_fflush( stfps[i].fp ); 
	    if( 0 != retv ) {
		return retv;
            }
	}
    return 0 ;
}

/* Report an error finding an FCB. */

static void fcberr(z80info *z80, unsigned where) {
    int i;
    char name[9];
    char *ptr = (char*)(z80->mem+where+1);
    ptr = (ptr != NULL) ? ptr : "<NULL>";
    memcpy( name, ptr,8);
    name[8]=0;

    MYFPRINTFERR (stderr, "error: cannot find fp entry for FCB at %04x"
	    " fctn %d, FCB named %s\n", where, z80->regbc & 0xff, name );

    for (i = 0; i < storedfps; ++i)
	if (stfps[i].where != 0xffffU)
	    MYPRINTF("%s %04x\n", stfps[i].name, stfps[i].where);

    MYPRINTF("trying to keep going\n");
    return;
    //resetterm();
    // TODO
    //exit(1);
}

/* Get the host file for an FCB when it should be open. */

static MYFILE *getfp(z80info *z80, unsigned where) {
    MYFILE *fp;

    if (!(fp = lookfp(z80, where)))
        fcberr(z80, where);
    return fp;
}

static void delfp(z80info *z80, unsigned where) {
    int i;
    for (i = 0; i < storedfps; ++i)
	if (stfps[i].where == where) {
	    stfps[i].where = 0xffffU;
	    return;
	}
    fcberr(z80, where);
}

/* FCB fields */
#define FCB_DR 0
#define FCB_F1 1
#define FCB_T1 9
#define FCB_EX 12
#define FCB_S1 13
#define FCB_S2 14
#define FCB_RC 15
#define FCB_CR 32
#define FCB_R0 33
#define FCB_R1 34
#define FCB_R2 35

/* S2: only low 6 bits have extent number: upper bits are flags.. */
/* Upper bit: means file is open? */

/* Support 8MB files */

#define ADDRESS  (((long)z80->mem[z80->regde+FCB_R0] + \
		    (long)z80->mem[z80->regde+FCB_R1] * 256) * 128L)
/* (long)z80->mem[z80->regde+35] * 65536L; */


/* For sequential access, the record number is also in the FCB:
 *    EX + 32 * S2 is extent number (0 .. 8191).  Each extent has 16 blocks.  Each block is 1K.
 *          extent number = (file offset) / 16384
 *
 *    RC  has number of records in current extent: (0 .. 128)
 *          maybe just set to 128?
 *          otherwise: 
 *
 *    CR  is current record offset within current extent: (0 .. 127)
 *          ((file offset) % 16384) / 128
 */

/* Current extent number */
#define SEQ_EXT  ((long)z80->mem[DE + FCB_EX] + 32L * (long)(0x3F & z80->mem[DE + FCB_S2]))

/* Current byte offset */
#define SEQ_ADDRESS  (16384L * SEQ_EXT + 128L * (long)z80->mem[DE + FCB_CR])

/* Convert offset to CR */
#define SEQ_CR(n) (((n) % 16384) / 128)

/* Convert offset to extent number */
#define SEQ_EXTENT(n) ((n) / 16384)

/* Convert offset to low byte of extent number */
#define SEQ_EX(n) (SEQ_EXTENT(n) % 32)

/* Convert offset to high byte of extent number */
#define SEQ_S2(n) (SEQ_EXTENT(n) / 32)

static my_DIR *dp = NULL;
static unsigned sfn = 0;

char *bdos_decode(int n)
{
	switch (n) {
	    case  0: return "System Reset";
	    case 1: return "Console Input";
	    case 2: return "Console Output";
	    case 3: return "Reader input";
	    case 4: return "Punch output";
	    case 5: return "List output";
	    case 6: return "direct I/O";
	    case 7: return "get I/O byte";
	    case 8: return "set I/O byte";
	    case 9: return "Print String";
	    case 10: return "Read Command Line";
	    case 11: return "Console Status";
	    case 12: return "Return Version Number";
	    case 13: return "reset disk system";
	    case 14: return "select disk";
	    case 15: return "open file";
	    case 16: return "close file";
	    case 17: return "search for first";
	    case 18: return "search for next";
	    case 19: return "delete file (no wildcards yet)";
	    case 20: return "read sequential";
	    case 21: return "write sequential";
	    case 22: return "make file";
	    case 23: return "rename file";
	    case 24: return "return login vector";
	    case 25: return "return current disk";
	    case 26: return "Set DMA Address";
	    case 27: return "Get alloc addr";
	    case 28: return "Set r/o vector";
	    case 29: return "return r/o vector";
	    case 30: return "Set file attributes";
	    case 31: return "get disk parameters";
	    case 32: return "Get/Set User Code";
	    case 33: return "read random record";
	    case 34: return "write random record";
	    case 35: return "compute file size";
	    case 36: return "set random record";
	    case 41:
	    default: return "unknown";
	}
}

/* True if DE points to an FCB for a given BDOS call */

int bdos_fcb(int n)
{
	switch (n) {
	    case 15: return 1; /* "open file"; */
	    case 16: return 1; /* "close file"; */
	    case 17: return 1; /* "search for first"; */
	    case 18: return 1; /* "search for next"; */
	    case 19: return 1; /* "delete file (no wildcards yet)"; */
	    case 20: return 1; /* "read sequential"; */
	    case 21: return 1; /* "write sequential"; */
	    case 22: return 1; /* "make file"; */
	    case 23: return 1; /* "rename file"; */
	    case 30: return 1; /* set attribute */
	    case 33: return 1; /* "read random record"; */
	    case 34: return 1; /* "write random record"; */
	    case 35: return 1; /* "compute file size"; */
	    case 36: return 1; /* "set random record"; */
	    default: return 0;
	}
}

void bdos_fcb_dump(z80info *z80)
{
	char buf[80];
	MYPRINTF("FCB %x: ", DE);
	buf[0] = (0x7F & z80->mem[DE + 1]);
	buf[1] = (0x7F & z80->mem[DE + 2]);
	buf[2] = (0x7F & z80->mem[DE + 3]);
	buf[3] = (0x7F & z80->mem[DE + 4]);
	buf[4] = (0x7F & z80->mem[DE + 5]);
	buf[5] = (0x7F & z80->mem[DE + 6]);
	buf[6] = (0x7F & z80->mem[DE + 7]);
	buf[7] = (0x7F & z80->mem[DE + 8]);
	buf[8] = '.';
	buf[9] = (0x7F & z80->mem[DE + 9]);
	buf[10] = (0x7F & z80->mem[DE + 10]);
	buf[11] = (0x7F & z80->mem[DE + 11]);
	buf[12] = 0;
	MYPRINTF("DR=%x F='%s' EX=%x S1=%x S2=%x RC=%x CR=%x R0=%x R1=%x R2=%x\n",
	       z80->mem[DE + 0], buf, z80->mem[DE + 12], z80->mem[DE + 13],
	       z80->mem[DE + 14], z80->mem[DE + 15], z80->mem[DE + 32], z80->mem[DE + 33],
	       z80->mem[DE + 34], z80->mem[DE + 35]);
}

/* Get count of records in current extent */

int fixrc (z80info *z80, MYFILE *fp)
{

    // KB -- I've had to change the logic here completely, because we
    //  can't assume that the underlying filesystem will be able to
    //  report the size of the underlying file whilst it is open. 
    // So we need to fseek to the end and back again, rather than 
    //  relying on fstat.

    uint32_t old_pos = MYFTELL (fp);
    if (old_pos == (uint32_t)-1) return -1;
    MYFSEEK (fp, 0, SEEK_END);
    uint32_t fsize = MYFTELL (fp);
    MYFSEEK (fp, old_pos, SEEK_SET);

/*
   printf ("oldpos = %d\n", old_pos);
    struct stat stbuf;
    unsigned long size;
    unsigned long full;
    unsigned long ext;
    if (MYFSTAT (MYFILENO (fp), &stbuf) || !S_ISREG (stbuf.st_mode)) {
        return -1;
    }
*/

    //size = (stbuf.st_size + 127) >> 7; /* number of records in file */
    uint32_t size = (fsize + 127) >> 7; /* number of records in file */

    int32_t full = size - (size % 128); /* record number of first partially full extent */
    int ext = SEQ_EXT * 128; /* record number of current extent */

    if (ext < full)
        /* Current extent is full */
        z80->mem[DE + FCB_RC] = 128;
    else if (ext > full)
        /* We are pointing past the end */
        z80->mem[DE + FCB_RC] = 0;
    else
        /* We are pointing to a partial extent */
        z80->mem[DE + FCB_RC] = size - full;

    return 0;
}

/* emulation of BDOS calls */

void check_BDOS_hook (z80info *z80) {
    ConsoleParams *cp = z80->console_params;
    int i;
    char name[32];
    char name2[32];
    MYFILE *fp;
    char *s, *t;
    const char *mode;
    if (trace_bdos)
    {
        MYPRINTF("\r\nbdos %d %s (AF=%04x BC=%04x DE=%04x HL =%04x SP=%04x STACK=", C, bdos_decode(C), AF, BC, DE, HL, SP);
	for (i = 0; i < 8; ++i)
	    MYPRINTF(" %4x", z80->mem[SP + 2*i]
		   + 256 * z80->mem[SP + 2*i + 1]);
	MYPRINTF(")\r\n");
    }
    switch (C) {
    case  0:    /* System Reset */
	warmboot(z80);
	return;
#if 0
	for (i = 0; i < 0x1600; ++i)
	    z80->mem[i+BIOS-0x1600] = cpmsys[i];
	BC = 0;
	PC = BIOS-0x1600+3;
	SP = 0x80;
#endif
	break;
    case 1:     /* Console Input */
	HL = kget (cp, 0);
	B = H; A = L;
	if (A < ' ') {
	    switch(A) {
	    case '\r':
	    case '\n':
	    case '\t':
		vt52 (cp, A);
		break;
	    default:
		vt52 (cp,'^');
		vt52 (cp, (A & 0xff)+'@');
		if (A == 3) {	/* ctrl-C pressed */
		    /* PC = BIOS+3;
		       check_BIOS_hook(); */
		    warmboot(z80);
		    return;
		}
	    }
	} else {
	    vt52 (cp, A);
	}
	break;
    case 2:     /* Console Output */
	vt52 (cp, 0x7F & E);
	HL = 0;
        B = H; A = L;
	break;
    case 6:     /* direct I/O */
	switch (E) {
	case 0xff:  if (!constat (cp)) {
	    HL = 0;
            B = H; A = L;
	    F = 0;
	    break;
	} /* FALLTHRU */
	case 0xfd:  HL = kget(cp, 0);
            B = H; A = L;
	    F = 0;
	    break;
	case 0xfe:  HL = constat (cp) ? 0xff : 0;
            B = H; A = L;
	    F = 0;
	    break;
	default:    vt52 (cp, 0x7F & E);
            HL = 0;
            B = H; A = L;
	}
	break;
    case 9:	/* Print String */
	s = (char *)(z80->mem + DE);
	while (*s != '$')
	    vt52 (cp, 0x7F & *s++);
        HL = 0;
        B = H; A = L;
	break;
    case 10:    /* Read Command Line */
	s = rdcmdline (z80, *(unsigned char *)(t = (char *)(z80->mem + DE)), 1);
	if (z80->finished) return;
	if (PC == BIOS+3) { 	/* ctrl-C pressed */
	    /* check_BIOS_hook(); */		/* execute WBOOT */
	    warmboot(z80);
	    return;
	}
	++t;
	for (i = 0; i <= *(unsigned char *)s; ++i)
	    t[i] = s[i];
        HL = 0;
        B = H; A = L;
	break;
    case 12:    /* Return Version Number */
	HL = 0x22; /* Emulate CP/M 2.2 */
        B = H; A = L;
	F = 0;
	break;
    case 26:    /* Set DMA Address */
	z80->dma = DE;
	HL = 0;
        B = H; A = L;
	break;
    case 32:    /* Get/Set User Code */
	if (E == 0xff) {  /* Get Code */
	    HL = usercode;
            B = H; A = L;
	} else {
	    usercode = E;
            HL = 0; /* Or does it get usercode? */
            B = H; A = L;
        }
	break;

	/* dunno if these are correct */

    case 11:	/* Console Status */
	HL = (constat (cp) ? 0xff : 0x00);
        B = H; A = L;
	F = 0;
	break;

    case 13:	/* reset disk system */
	/* storedfps = 0; */	/* WS crashes then */
	HL = 0;
        B = H; A = L;
	if (dp)
	    my_closedir (dp);
	{   struct my_dirent *de;
            if ((dp = my_opendir("."))) {
                while ((de = my_readdir(dp))) {
                    if (strchr(de->d_name, '$')) {
                        A = 0xff;
                        break;
                    }
                }
                my_closedir(dp);
            }
        }
	dp = NULL;
	z80->dma = 0x80;
	/* select only A:, all r/w */
	break;
    case 14:	/* select disk */
        A = setdrive (z80, E);
	HL = A;
        B = H; 
	break;
    case 15:	/* open file */
	mode = "r+b";
    fileio:
        /* check if the file is already open */
        if (!(fp = lookfp(z80, DE))) {
            /* not already open - try lowercase */
            FCB_to_filename(z80->mem+DE, name);
	if (!(fp = MYFOPEN(name, mode))) {
	    FCB_to_ufilename(z80->mem+DE, name); /* Try all uppercase instead */
            if (!(fp = MYFOPEN(name, mode))) {
	            FCB_to_filename(z80->mem+DE, name);
		    if (*mode == 'r') {
			char ss[50];
			snprintf(ss, sizeof(ss), "%s/%s", CPMLIBDIR, name);
			fp = MYFOPEN(ss, mode);
			if (!fp)
			  fp = MYFOPEN(ss, "rb");
		    }
		    if (!fp) {
			/* still no success */
			HL = 0xFF;
                        B = H; A = L;
			F = 0;
			break;
		    }
            }
            }
            /* where to store fp? */
            storefp(z80, fp, DE);
	}
	/* success */

	/* memset(z80->mem + DE + 12, 0, 33-12); */

	/* User has to set EX, S2, and CR: don't change these- some set them to non-zero */
	z80->mem[DE + FCB_S1] = 0;
	/* memset(z80->mem + DE + 16, 0, 16); */ /* Clear D0-Dn */

	/* Should we clear R0 - R2? Nope: then we overlap the following area. */
	/* memset(z80->mem + DE + 33, 0, 3); */

	/* We need to set high bit of S2: means file is open? */
	z80->mem[DE + FCB_S2] |= 0x80;

	z80->mem[DE + FCB_RC] = 0;	/* rc field of FCB */

	if (fixrc(z80, fp)) { /* Not a real file? */
	    HL = 0xFF;
            B = H; A = L;
	    F = 0;
	    MYFCLOSE(fp);
            delfp(z80, DE);
	    break;
	}
	HL = 0;
        B = H; A = L;
	F = 0;
	break;
    case 16:	/* close file */
        {
            long host_size, host_exts;

	    if (!(fp = lookfp (z80, DE))) {
		/* if the FBC is unknown, return an error */
		HL = 0xFF;
		B = H, A = L;
		break;
	    }
            MYFSEEK (fp, 0, SEEK_END);
            host_size = MYFTELL (fp);
            host_exts = SEQ_EXTENT (host_size);
            if (host_exts == SEQ_EXT) {
                /* this is the last extent of the file so we allow the
                   CP/M program to truncate it by reducing RC */
                if (z80->mem[DE + FCB_RC] < SEQ_CR (host_size)) {
                    host_size = (16384L * SEQ_EXT + 128L * (long)z80->mem[DE + FCB_RC]);
                    MYFTRUNCATE (MYFILENO (fp), host_size);
                }
            }
	delfp(z80, DE);
	MYFCLOSE (fp);
        z80->mem[DE + FCB_S2] &= 0x7F; /* Clear high bit: indicates closed */
	HL = 0;
        B = H; A = L;
        }
	break;
    case 17:	/* search for first */
	if (dp)
	    my_closedir(dp);
	if (!(dp = my_opendir ("."))) {
	    printf ("errno=%d\n", errno);
	    MYFPRINTFERR(stderr, "opendir fails\n");
            //resetterm();
            // TODO
	    exit(1);
	}
	sfn = DE;
	/* fall through */
    case 18:	/* search for next */
	if (!dp)
	    goto retbad;
	{   struct my_dirent *de;
	    unsigned char *p;
	    const char *sr;
	nocpmname:
	    if (!(de = my_readdir(dp))) {
		my_closedir(dp);
		dp = NULL;
	    retbad:
	        HL = 0xff;
                B = H; A = L;
		F = 0;
		break;
	    }
	    /* compare data */
	    memset(p = z80->mem+z80->dma, 0, 128);	/* dmaaddr instead of DIRBUF!! */
	    if (*de->d_name == '.')
		goto nocpmname;
	    if (strchr(sr = de->d_name, '.')) {
		if (strlen(de->d_name) > 12)	/* POSIX: namlen */
		    goto nocpmname;
	    } else if (strlen(de->d_name) > 8)
		    goto nocpmname;
	    /* seems OK */
	    for (i = 0; i < 8; ++i)
		if (*sr != '.' && *sr) {
		    *++p = toupper(*(unsigned char *)sr); sr++;
		} else
		    *++p = ' ';
	    /* skip dot */
	    while (*sr && *sr != '.')
		++sr;
	    while (*sr == '.')
		++sr;
	    for (i = 0; i < 3; ++i)
		if (*sr != '.' && *sr) {
		    *++p = toupper(*(unsigned char *)sr); sr++;
		} else
		    *++p = ' ';
	    /* OK, fcb block is filled */
	    /* match name */
	    p -= 11;
	    sr = (char *)(z80->mem + sfn);
	    for (i = 1; i <= 12; ++i)
		if (sr[i] != '?' && sr[i] != p[i])
		    goto nocpmname;
	    /* yup, it matches */
	    HL = 0x00;	/* always at pos 0 */
            B = H; A = L;
	    F = 0;
	    p[32] = p[64] = p[96] = 0xe5;
	}
	break;
    case 19:	/* delete file (no wildcards yet) */
	FCB_to_filename(z80->mem + DE, name);
	unlink(name);
	HL = 0;
        B = H; A = L;
	break;
    case 20:	/* read sequential */
	fp = getfp(z80, DE);
    readseq:
	if (!MYFSEEK(fp, SEQ_ADDRESS, SEEK_SET) && ((i = MYFREAD(z80->mem+z80->dma, 1, 128, fp)) > 0)) {
	    long ofst = MYFTELL (fp) + 127;
	    if (i != 128)
		memset(z80->mem+z80->dma+i, 0x1a, 128-i);
	    z80->mem[DE + FCB_CR] = SEQ_CR(ofst);
	    z80->mem[DE + FCB_EX] = SEQ_EX(ofst);
	    z80->mem[DE + FCB_S2] = (0x80 | SEQ_S2(ofst));
	    fixrc(z80, fp);
	    HL = 0x00;
            B = H; A = L;
	} else {
	    HL = 0x1;	/* ff => pip error */
            B = H; A = L;
	}    
	break;
    case 21:	/* write sequential */
	fp = getfp (z80, DE);
    writeseq:
	if (!MYFSEEK (fp, SEQ_ADDRESS, SEEK_SET) && MYFWRITE (z80->mem+z80->dma, 1, 128, fp) == 128) {
	    long ofst = MYFTELL (fp);
	    z80->mem[DE + FCB_CR] = SEQ_CR(ofst);
	    z80->mem[DE + FCB_EX] = SEQ_EX(ofst);
	    z80->mem[DE + FCB_S2] = (0x80 | SEQ_S2(ofst));
            MYFFLUSH (fp);
	    fixrc(z80, fp);
	    HL = 0x00;
            B = H; A = L;
	} else {
	    HL = 0xff;
            B = H; A = L;
	}
	break;
    case 22:	/* make file */
	mode = "w+b";
	goto fileio;
    case 23:	/* rename file */
	FCB_to_filename(z80->mem + DE, name);
	FCB_to_filename(z80->mem + DE + 16, name2);
	MYRENAME (name, name2);
	HL = 0;
        B = H; A = L;
	break;
    case 24:	/* return login vector */
	HL = 1;	/* only A: online */
        B = H; A = L;
	F = 0;
	break;
    case 25:	/* return current disk */
	HL = getdrive (z80);
        B = H; A = L;
	F = 0;
	break;
    case 29:	/* return r/o vector */
	HL = 0;	/* none r/o */
        B = H; A = L;
	F = 0;
	break;
    case 30:    /* set attributes F_ATTRIB */
	fp = getfp (z80, DE);
	// sadly some apps call this after closing the file, so lets not crash
	if(fp) MYFATTRIB(fp);
	break;
    case 31:    /* get disk parameters */
        HL = DPB0;    /* only A: */
        B = H; A = L;
        break;
    case 33:	/* read random record */
        {
        long ofst;
	fp = getfp(z80, DE);
	ofst = ADDRESS;
        z80->mem[DE + FCB_CR] = SEQ_CR(ofst);
	z80->mem[DE + FCB_EX] = SEQ_EX(ofst);
	z80->mem[DE + FCB_S2] = (0x80 | SEQ_S2(ofst));
	goto readseq;
	}
    case 34:	/* write random record */
        {
        long ofst;
	fp = getfp(z80, DE);
	ofst = ADDRESS;
        z80->mem[DE + FCB_CR] = SEQ_CR(ofst);
	z80->mem[DE + FCB_EX] = SEQ_EX(ofst);
	z80->mem[DE + FCB_S2] = (0x80 | SEQ_S2(ofst));
	goto writeseq;
	}
    case 35:	/* compute file size */
	fp = getfp(z80, DE);
	MYFSEEK(fp, 0L, SEEK_END);
	/* fall through */
    case 36:	/* set random record */
	fp = getfp(z80, DE);
	{   
	    long ofst = MYFTELL (fp) + 127;
	    long pos = (ofst >> 7);
	    HL = 0x00;	/* dunno, if necessary */
            B = H; A = L;
	    z80->mem[DE + FCB_R0] = pos & 0xff;
	    z80->mem[DE + FCB_R1] = pos >> 8;
	    z80->mem[DE + FCB_R2] = pos >> 16;
            z80->mem[DE + FCB_CR] = SEQ_CR(ofst);
	    z80->mem[DE + FCB_EX] = SEQ_EX(ofst);
	    z80->mem[DE + FCB_S2] = (0x80 | SEQ_S2(ofst));
	    fixrc(z80, fp);
	}
	break;
    case 41:
        printf ("BIOS 41 NOT YET IMPLEMENTED");
	for (s = (char *)(z80->mem + DE); *s; ++s)
	    *s = tolower(*(unsigned char *)s);
	//HL = (restricted_mode || chdir((char  *)(z80->mem + DE))) ? 0xff : 0x00;
        HL = 0xFFFF;
        B = H; A = L;
	break;
    case 48:
	{
	    int err = flushallfps();
	    if( err == -1 ) {
		A = 0xFF;
		H = errno;
	    }
	    break;
	}
	
    default:
	MYPRINTF("\n\nUnrecognized BDOS-Function:\n");
	MYPRINTF("AF=%04x  BC=%04x  DE=%04x  HL=%04x  SP=%04x\nStack =",
	       AF, BC, DE, HL, SP);
	for (i = 0; i < 8; ++i)
		MYPRINTF(" %4x", z80->mem[SP + 2*i]
		   		+ 256 * z80->mem[SP + 2*i + 1]);
	MYPRINTF("\r\n");
	//resetterm();
        // TODO
	exit(1);
    }
    z80->mem[PC = DIRBUF-1] = 0xc9; /* Return instruction */
    return;
}
