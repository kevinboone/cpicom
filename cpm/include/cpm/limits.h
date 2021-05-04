#pragma once

// Longest command line. This is normally stored in the area from
//  0x80-0xFF, so that's 128 chars, minus one because the length is
//  stored at 0x80
#define CPM_MAX_CMDLINE 127

// Longest line that can be read by the read-line BDOS function. I don't
//  believe there is a limit, except that it's length will probably be
//  stored somewhere as 8 bytes. So call it 256. In any event, it has to
//  be as long as MAX_CMDLINE
#define CPM_MAX_READ_LINE 256

// Size of a CP/M record on disk
#define BDOS_RECORD_SIZE 128
