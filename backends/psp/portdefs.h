/* ScummVM - Scumm Interpreter
 * Copyright (C) 2005 The ScummVM project
 * Copyright (C) 2005 Joost Peters PSP Backend
 * Copyright (C) 2005 Thomas Mayer PSP Backend
 * Copyright (C) 2005 Paolo Costabel PSP Backend
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 
 *
 * $Header$
 *
 */

#ifndef PORTDEFS_H
#define PORTDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>

#include "trace.h"

#define	SCUMMVM_SAVEPATH	"ms0:/scummvm_savegames"

#define	BREAKPOINT	asm("break\n")


//#define printf  pspDebugScreenPrintf
#define exit(x) printf("exit() called\n"); sceKernelSleepThread();

#undef main

#ifndef REAL_MAIN
#define main scummvm_main
#endif

#endif /* PORTDEFS_H */


