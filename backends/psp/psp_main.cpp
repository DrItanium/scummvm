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
 
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>

#define	REAL_MAIN
#include <common/stdafx.h>
#include <common/scummsys.h>
#include <base/engine.h>
#include <base/gameDetector.h>
#include <base/plugins.h>

#include <pspgu.h>
#include "./trace.h"

PSP_MODULE_INFO("SCUMMVM-PSP", 0x1000, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(/*THREAD_ATTR_USER | THREAD_ATTR_VFPU*/ 0);

/* global quit flag, this is used to let the VM engine shutdown properly */
bool g_quit = false;

void MyExceptionHandler(PspDebugRegBlock *regs) {
	/* Do normal initial dump, setup screen etc */

	pspDebugScreenInit();

	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("Exception Details:\n");
	pspDebugDumpException(regs);

	while (1) ;
}


/* Exit callback */
SceKernelCallbackFunction exit_callback(int /*arg1*/, int /*arg2*/, void * /*common*/) {
	g_quit = true; //Set g_quit so our backend can shutdown the VM
	sceKernelDelayThread(1000);
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize /*size*/, void *arg) {
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (SceKernelCallbackFunction)exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


extern "C" int scummvm_main(int argc, char *argv[]);

int main(void)
{
	//PSPDebugTrace("Init debug screen\n");
	pspDebugScreenInit();

	//PSPDebugTrace("Setup callbacks\n");
	SetupCallbacks();

	//install exception handler
	pspDebugInstallErrorHandler(MyExceptionHandler);

	//check if the save directory exists
	SceUID fd = sceIoDopen(SCUMMVM_SAVEPATH);
	if (fd < 0) {
		//No? then let's create it.
		sceIoMkdir(SCUMMVM_SAVEPATH, 0777); 
	} else {
		//it exists, so close it again.
		sceIoDclose(fd);
	}

	static char *argv[] = { "scummvm", "--force-1x-overlay", NULL };
	static int argc = sizeof(argv)/sizeof(char *)-1;

	scummvm_main(argc, argv);
	
	sceKernelSleepThread();

	return 0;
}

