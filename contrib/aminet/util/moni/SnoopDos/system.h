/*
 *		SYSTEM.H
 *
 *		System includes for SnoopDos							vi:ts=4
 */

#define INTUI_V36_NAMES_ONLY	1
#define __USE_SYSBASE			1
#define USE_BUILTIN_MATH		1

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/tasks.h>
#include <exec/execbase.h>
#include <devices/inputevent.h>
#include <devices/clipboard.h>
#include <graphics/gfx.h>
#include <graphics/gels.h>
#include <graphics/clip.h>
#include <graphics/rastport.h>
#include <graphics/view.h>
#include <graphics/gfxbase.h>
#include <graphics/layers.h>
#include <graphics/text.h>
#include <graphics/monitor.h>
#include <graphics/videocontrol.h>
#include <graphics/displayinfo.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <intuition/imageclass.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/datetime.h>
#include <dos/dostags.h>
#include <dos/var.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/diskfont.h>
#include <libraries/asl.h>
#include <libraries/locale.h>
#include <libraries/commodities.h>
#include <libraries/amigaguide.h>
#include <rexx/errors.h>
#include <rexx/storage.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include <proto/amigaguide.h>
#ifndef __AROS__
#include <proto/rexxsyslib.h>
#endif
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/layers.h>
#include <proto/wb.h>
#include <proto/diskfont.h>
#include <proto/commodities.h>
#include <proto/icon.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/commodities.h>
#include <proto/asl.h>
#include <proto/keymap.h>
#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef __AROS__
	extern struct WBStartup * WBenchMsg;
#include <aros/libcall.h>
#else
	/* for WBenchMsg */
	#include <dos.h>
#endif

#include "CompilerSpecific.h"

