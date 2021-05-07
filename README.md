# CPIOM

Version 0.1a

## What is this ?

CPICOM is a _proof-of-concept_ CP/M-80 (version 2.2) 
emulator for the Raspberry Pi Pico
microntroller, with a user interface provided by a terminal or
terminal emulator. By default, it CPICOM uses the USB port for USB-serial
communication with the terminal, but there are other possibilities.
This firmware allows the Pico to run certain popular CP/M applications.
My reference test applications are Zork1 and WordStar. So long as these
work, I take the proof of concept to be a success. Many other
CP/M program do run under CPICOM, but some do not, for reasons
I will explain later. 

CPICOM emulates dual 720k floppies in the Pico's flash memory -- there
is just room for the two floppy disk images in the 2Mb flash 
along with the program itself.
These images use the LittleFS filesystem, but the infrastructure is in
place to allow for other filesystem types, on external hardware, in
future development. For example, it shouldn't be too difficult to 
add an SD card and use it to simulate an external hard disk.
However, I wanted to use the flash memory as the primary storage initially, so
CPICOM can be used without any custom hardware. 

Right now, the only way to install CP/M software in the Pico's
flash is to
upload it using CPICOM's YModem implementation (see below for details). This 
can often be done directly 
from a terminal emulator, but most platforms have other, preferable ways
to do it. All the host operations I describe below are for Linux, using
the Minicom terminal emulator. I'm sure there are other terminal emulators,
for other platforms, but this is the platform I understand.

CPICOM includes a rudimentary command-line interface that replaces the
"CCP" interface that is usually provided by CP/M. This allows for the
provision of utilities like STAT and FORMAT that work on the emulated
disks. Where a common CP/M utility exists, such as `PIP` for copying files,
and that utility works, I have not attempted to provide my own 
implementation. 

CPICOM runs CP/M programs at about the same speed as real Z80 hardware
of the 1980s -- that is to say, slowly. The use of a terminal
as the display device reduces performance still further. It's an
authentic 80s computing experience ;) So, while CPICOM _will_ run
WordStar, it's scarcely usable -- it never was, really, on a terminal.

CPICOM is (or, at least, should be) easy to build from source using the
Pico SDK tools. Pre-built UF2 files, along with more general details
of the project, are available on my website:

https://kevinboone.me/cpicom.html

A good source of CP/M binaries is here:

http://cpmarchives.classiccmp.org/

## Building CPICOM for the Pico

CPICOM is implemented in C, and designed to be built using 
the documented method for the Raspberry Pi Pico C SDK. 
This method depends, for better
or worse, on the CMake utility. Setting up CMake, the SDK, and the ARM compiler
toolchain, is documented in the Raspberry Pi documentation. I'm not a fan
of CMake, but building for the Pico is well-nigh impossible any other way.
If you've set the SDK up according to the documentation, you should be able
to build like this: 

    $ mkdir build_pico
    $ cd build_pico
    $ cmake .. 
    $ make

The build process is configured to provide maximal warning logging,
and will throw out thousands of messages, of varying degrees of scariness. 
The result should be a file `cpicom.uf2`, that can be copied to the
Pico in bootloader mode.

When it starts, CPICOM will attempt to mount the two floppy disk images
in the Pico's flash. On the first start, when there is no data in
these locations, CPICOM will attempt to format the flash. In any event,
you should be aware that CPICOM will use the whole of the Pico's flash,
potentially erasing any other contents. 

For testing purposes, it should be possible to build a version of CPICOM 
that will work on a Linux workstation, like this:

    $ mkdir build_host
    $ cd build_host
    $ PICO_PLATFORM=host cmake ..
    $ make.

The Linux version uses two files as images of the two floppy drives. These
files must exist, and be of a suitable size, but they need not contain
any data -- they will be formatted on first use. You can create the
drives like this:

    $ dd if=/dev/zero of=$HOME/drive-a.dsk bs=1024 count=360
    $ dd if=/dev/zero of=$HOM/drive-b.dsk bs=1024 count=360

The Linux version is deliberately designed to have all the same 
flaws and limitations as the Pico version. However, it conceals some
problems that are evident when running on the Pico, because a modern
desktop computer is perhaps 30 times faster than the Pico.

## Running CPICOM on Pico

In bootloader mode, copy the `.UF2` file to the Pico. After installing
the UF2 file, the Pico will restart in USB serial mode.

To interact with CPICOM you'll need a terminal emulator 
running on some kind of workstation that
has a USB socket and can do USB-serial operations. All modern Linux
versions have this support built in. For Windows you may need additional
drivers -- this is described in the Pico SDK documentation.

All being well, you'll be presented with the good old "A>" prompt. At
this point, there will be no software on either of the emulated floppy
disks, so you'll need to upload something using YModem.

## Uploading and downloading CP/M files to and from CPICOM

The procedures below apply to Linux, and are probably not the only 
possibilities, even on Linux. I'm sure that similar procedures are
available for other platforms, but I don't know what they are. 

In all cases, you'll need to run

    setdef [noflowcontrol]

at the CP/M prompt before uploading or downloading, else the 
console handler will interpret
certain bytes in the data stream as terminal flow control characters.

### Uploading using YModem on Linux to the Pico

Files must be uploaded one at a time using the YModem protocol.
Connect the terminal (e.g., Minicom) as usual. Run `yrecv` at the
`A>` prompt.. Look for the 
row of "C" characters to be printed -- this is the start of the YModem
handshake. Now exit the terminal without resetting the modem
(e.g., ctrl-A,Q on Minicom). Then:

    $ sb --ymodem -k /path/to/file > /ttyACM0 < /dev/ttyACM0

You can do YModem directly from Minicom (and most terminal emulators, 
I guess) but setting the "-k" switch sets 1k block sizes, which I've found
is both faster and more robust. I have no idea why this is the case, and
it might not be so for everyone.

### Download using YModem on Linux from the Pico

If you need to download data from the Pico to a host computer, you
can use the `ysend` command. 
Connect the terminal (e.g., Minicom) as usual. Run 

    ysend [filename]

Look for the 
row of "C" characters. Now exit the terminal without resetting the modem
(e.g., ctrl-A,Q on Minicom). Then

    $ rb --ymodem [file_to_save] > /ttyACM0 < /dev/ttyACM0

Be aware that `rb` fails silently if asked to receive a file that already
exists -- this can be very confusing. 

If you want to download a text file, it's likely easier just to 
TYPE it, and cut-and-paste from the terminal emulator.

### Uploading using YModem on the Linux version

For completeness, here is how to upload CP/M files when running 
CPICOM under Linux.

    $ mkfifo /tmp/f1 /tmp/f2
    $./cpicom < /tmp/f2 > /tmp/f1

In a different terminal session:

    $ echo yrecv > /tmp/f2
    $ sb --ymodem -k /path/to/file > /tmp/f2 < /tmp/f1

Note that the order of redirections on the command lines _is_ significant
here -- things have to be set up in the right order.

### Downloading using YModem om the Linux version

Downloading on Linux is as follows:

    $ mkfifo /tmp/f1 /tmp/f2
    $./cpicom < /tmp/f2 > /tmp/f1

In a different terminal session:

    $ echo "ysend [filename]" > /tmp/f2
    $ rb --ymodem [file_to_save] > /tmp/f2 < /tmp/f1


## CPICOM shell commands

The built-in commands are: `rename`, `type`, `erase`, `dir`, `yrecv`, `stat`,
`dump`, `format`, and `setdef`. All except the last two can be abbreviated to
the first three characters.  For compatibility with CP/M commands that may be
provided as executable programs, all the built-in commands use CP/M syntax,
which is comparatively unfamiliar to most people under the age of 50.   

For example, to rename file X to file Y, both on drive A:, you need:

    A> ren Y=X

To "log on" to a different drive, enter the drive letter followed by a 
like this:

    B:

This changes the default drive (just like MS/DOS). The drive letter can be
entered in upper or lower case.

To run a CP/M program on drive A:, supplied as a `.COM` file, just 
enter its name (with or without the extension). To run a CP/M program
on a different drive, using the drive letter, like this:

   A> b:myprog

Please note that all the CP/M commands are cut-down versions of the 
originals, to reduce the size of the program and allow more space on
the Pico for storage.

## Installing the standard "transient" commands

Apart from the built-in commands, DIR, REN, etc., CP/M 2.2 was supplied
with "transient" commands that had to loaded from disk. These included
PIP (for copying files), and ED (the awful editor). The files are
provided in a tar archive in the `misc/` directory. To install them,
upload the file `cpm22disk.tar` using YModem as described above. Then,
at the `A>` prompt, run 

    A> untar cpm22disk.tar

You'll need to have enough space on the virtual disk to unpack the 
individual files, as well as the original archive. After unpacking, the
archive can be deleted.

## Technical details

CPICOM does not rely on any original CP/M files. However, since some
CP/M application do make calls to the CP/M BDOS in undocumented ways,
it does provide a minimal implementation of the CP/M BDOS and CCP
(command-line processor). These are taken from the P2DOS implementation
(see the "Legal" section below), rather than real CP/M. The 
BDOS is loaded into the emulated memory space in the same location it would
occupy in a real Z80 machine. Well-behaved CP/M programs will not really
use this BDOS -- they will make BDOS calls using the official, 
documented method (executing a subroutine call to memory address 5), and these
will be intercepted by CPICOM's BDOS implementation.

Although we can provide a more-or-less realistic BDOS image for applications
that use it, we can't really implement a realistic CP/M BIOS. The BIOS
handles the really low-level hardware interface -- reading specific sectors
on disks, for example. The reason we can't use a real BIOS is because 
the CP/M bios requires a specific, CP/M-compatible disk layout. I've designed
CPICOM to be extensible enough to be able to use real disk hardware 
with modern filesystems in due course,
and these won't be compatible with CP/M BIOS. As a result, although
CPICOM does handle some direct BIOS calls (like those that do terminal
input and output), most are not implemented. That's why disk management
tools like STAT.COM and FORMAT won't work, and CPICOM provides its
own alternatives. 


## Notes

Many CP/M applications just loop waiting to see if there is keyboard input.
There needs to be a delay when this happens, or the loop just burns CPU in
a way that is unsatisfactory on modern hardware, although it would have
been perfectly fine on a 4MHz Z80 in 1979. For better or worse, 
many CP/M applications 
check keyboard input all the time, even after writing a single character.
So the idle delay can't be too long, else the whole program gets slowed right
down. There is no clear solution to this problem, except to run on
real Z80 hardware, of course. I've tried to choose character 
polling timeouts that don't delay execution too much -- bearing in mind
that a realistic simulation will not be very fast, anyway.

Terminal interrupts are only checked when a program polls for 
keyboard input. Most
CP/M programs do this a lot, and ctrl+c is fairly responsive. However,
if the Z80 emulator gets stuck in an endless loop, ctrl+c won't fix it.
We could check more frequently, but checking for serial input is a 
comparatively slow operation with the Pico SDK.

CPICOM's line editing uses more modern key bindings, 
based on cursor (arrow) keys.
Up- and down-arrow retrieve previous commands from the (short) command history
list. As well as the ordinary arrow keys, the CPICOM line editor supports
ctrl+right and ctrl+left to move by words, and the usual "home" and
"end" keys. The same line editor is used when a CP/M program asks for
line input from BDOS, even though these are not idiomatic CP/M key bindings.
Not all CP/M apps use the BDOS "read line" call for line input. For those that 
do input on a character-by-character basis, line editing
will be at the discretion of the CP/M program. It might use traditional
CP/M bindings (e.g., ctrl+e is end-of-line), it might use
WordStar bindings (e..g, ctrl-s for cursor right), or it might provide
no line editing at all.  

Most terminals uses ctrl-S and ctrl-Q to pause and resume data flow.
These keystrokes may be typed by a user, or issued silently by the
terminal when its input buffer is full. This is generally referred to
as "terminal flow control". Unfortunately, many CP/M applications
use WordStar key bindings (including, of course, WordStar itself). WordStar
uses the flow control keys for navigation. So, to use WordStar
successfully, you'll need to turn off terminal flow control, using

    setdef [noflowcontrol]

Of course, you'll lose the ability to pause output by hitting ctrl+S.

Because I assume you're using a modern terminal emulator with a scroll-back
buffer, the CPICOM shell writes output without paging. If you want a 
more authentic CPM experience, enable paging by executing 

    setdef [page]

CP/M has a file management system based on fixed-size (128-byte) records.
Until CP/M 3, files did not even store an exact length. So if you use,
say, a text editor to create a file ten characters long, it will appear
to take one 128-byte record. CP/M will pad the last record with "ctrl+z"
characters. If you download a text file from CPICOM, you will always get
a file that is a multiple of 128 bytes in size, with this padding if it
is not an exact multiple. In addition CP/M, like MSDOS, uses "CRLF" as the
end-of-line marker in a test file, rather than the "LF" that Linux uses
Utilities like `dos2unix` can convert CP/M text files to and from Linux
format.

## Limitations

There are almost too many to list, let alone describe. There are the 
ones that seem to me to be most pressing.

### Only generic CP/M 2.2

CPICOM emulates a generic CP/M, not a specific machine. Any CP/M program
designed specifically for a particular machine will likely not work.
It almost goes without saying that, since I/O is terminal based, any
program that uses graphics won't work.

### Program search path

There is no drive search path -- executables are assumed to be on
drive A: To run a program on drive B, you'll need to say "b:prog"

### Limited built-in command set

I anticipate that proper utilities will be provided by CP/M executables,
uploaded to the emulated disk drives,
and I've only implemented very basic functionality in the CPICOM
shell -- the same commands that were built into CP/M's CCP command
processor, plus commands for uploading, downloading, and unpacking
files.

For example, the CPICOM shell provides no built-in way to copy files. 
The original CP/M
file copier `PIP.COM` works under CPICOM, and can installed as described
above. 

### Terminal issues 

CP/M has no concept of a terminal, in the sense of a controllable device.
All CP/M provides is a way for applications to write characters -- CP/M
does not care much what happens to them thereafter. 

Commercial CP/M applications had
built-in support for several different terminal types, which the user
could select. In practice, most modem terminal emulators implement 
something similar to the ANSI control code set, which is something 
similar to that used by the DEC VT100 terminal. My recollection is that
CP/M was rarely, if ever, used with VT100 terminals -- they did exist
at the same time, but only just.

What this means is that if you use a VT100 or ANSI terminal with a 
CP/M application, you're likely to have problems with screen-based
applications. Many CP/M applications assumed a VT52 terminal, so
CPICOM has built-in support for converting VT52 terminal codes to
VT100, to make it easier to use with contemporary terminals.

There is no similar support (yet) for converting VT100 keystrokes to
VT52. In practice, most CP/M software that allowed for on-screen
navigation used WordStar key bindings, which are based on control
characters. These are independent of the terminal.

A small amount of CP/M software has been written for, or converted to,
ANSI/VT100 terminals. For example, the "te" text editor by Miguel Garcia
has a VT100 version. This version, and others, are available here:

https://github.com/MiguelVis/te

In general, though, if your software allows you to choose the terminal
type, and VT100 is not available, try VT52, and let CPICOM try to 
convert the code.

I should point out that keyboard input that relies on multi-character
sequences, like the three-character cursor movement codes generated
by ANSI terminals, is inherently problematic on CP/M. That's because
there's no way for the program to tell the difference between the
multi-character sequence, and multiple key-presses, except by 
timing. And timing is notoriously imperfect on CP/M, because the
hardware provides no timing facilities. Sticking to WordStar
keystrokes is probably safer.

### YModem issues

The ymodem sender reads the whole file into memory before sending it
to the receiver. That's the Pico memory, not the Z80 memory, but
this still creates a limit of about 200kB on file sizes.

### Shell and CCP issues

The CPICOM shell provides the same built-in commands as CCP, plus some
additional support for uploading and downloading files, and unpacking
archives. This support is needed for getting a functioning CP/M system,
in an implementation that has no actual disk drives. 

CPICOM also includes a "real" CCP, as part of P2DOS. This CCP handles things
like setting up the file control blocks and parsing command lines. Otherwise,
because it does not contain the utilities needed for managing a system
like this, CPICOM tries to keep execution out of it. 

I could have just used the real CCP, but that would have meant implementing
all the extra features that CPICOM needs in Z80 assembler, and I just don't
have time for that.

Unfortunately, we have to have the real CCP, because sometimes CP/M utilities
will invoke it directly. Some will even replace it with other things, but 
expect it to be there. When that happens, execution can get stuck in the
original CCP. The CCP shell and the CPICOM shell look almost identical, 
and the only thing you'll likely notice is that the extra commands like
`yrecv` don't work. You can get out of CCP and back to the CPICOM shell
by hitting ctrl+c at the prompt.

## Author and legal

The Z80 CPU emulator engine is largely based on work by Parag Patel from the
1980s. Basic CP/M BDOS support was implemented for this emulator
 by Joseph Allen. I believe
that the source code for these components is freely distributable. They
are in the `cpm` directory of this repository.
The YModem implementation, in the `yodem` directory,
is based on work by Fredrik Hederstierna, now
believe to be in the public domain. The 
The "LittleFS" filesystem implementation (`files/*/lfs*`)
was developed by ARM, and is
open source under the terms of the BSD-3-Clause licence. 
The embeded P2DOS implementation of CP/M BDOS is not open source, but
its owners have given permission for it to be distributed in binary
form -- there is therefore no source for this component in this 
bundle, only a binary embedded in a C source.

The original CP/M 2.2 binaries (`PIP.COM`), etc., included in this
repository have been all been released to
the public domain. 

The Pico interface, filesystem management, filesystem compatibility 
layer, command-line interface (all other directories in the repository)
are copyright (c)2021 Kevin Boone, under the terms of the GNU Public
Licence v3.0. An overriding feature of all the licences for the various
components in this repository is unrestricted distribution, with
no warranty.

# Revision history
0.1a<br/>
May 2021<br/>
First working version

0.1b<br/>
May 2021<br/>
- Added flashing disk LED
- Various bug fixes

(untitled)<br/>
May 2021<br/>
- Added public-domain CP/M 2.2 transient commands as a tar file
- Added untar command to CPICOM shell



