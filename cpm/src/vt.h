#pragma once

#include "console/console.h"

/* Return true if input character available */
int constat (ConsoleParams *cp);

/* Get input character:
    w = 0: wait until we have a character
    w = 1: return -1 if we don' have one
    */
int kget(ConsoleParams *cp, int w);

/* Write character to terminal */
void vt52 (ConsoleParams *cp, int c);

#define INTR_CHAR	31	/* control-underscore */
