/*
 *	MysticView library globals
 *	© 1997 TEK neoscientists
 */

#ifndef _MYSTIC_GLOBAL_H
#define _MYSTIC_GLOBAL_H 1

#define AQuote(string) #string								/* Put quotes around the whole thing */
#define AVersion(major,minor) AQuote(major ## . ## minor)	/* Concatenate the two version-strings */
#define AmVersion(major,minor) AVersion(major,minor)		/* We need to do it this way to elimate the blank spaces */

#define VERSION  5
#define REVISION 0
#define LIBVER AmVersion(VERSION,REVISION)
#define LIBNAME "mysticview.library"
#define LIBDATE "(10.05.2001)"

#ifdef __MORPHOS__
#define LIBTYPE " [MorphOS]"
#else
#define LIBTYPE
#endif


#define	USEPOOLS				1
#define TRACKMEMORY				1


#if defined(__MORPHOS__) || defined(__AROS__)
#define SAVE_DS
#define ASM
#define REG(x)
#define LIBENT
#else
#define SAVE_DS
#define ASM		__asm
#define REG(x)	register __ ## x
#define LIBENT	SAVE_DS ASM
#endif

#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/memory.h>

#include <dos/dosextens.h>

#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <clib/macros.h>

#ifdef __MORPHOS__
#include <emul/emulinterface.h>
#include <emul/emulregs.h>
#include <public/proto/libamiga/amiga_protos.h>

#define FUNC_HOOK(ret, name, htype, hook) \
	static ret name##_GATE(void); \
	static ret name##func(htype hook); \
	struct EmulLibEntry name = { TRAP_LIB, 0, (void (*)(void))name##_GATE }; \
	static ret name##_GATE(void) { return (name##func((htype)REG_A0)); } \
	static ret name##func(htype hook)
#else
#include <clib/alib_protos.h>
#endif


/*****************************************************************************/


struct MysticBase
{
	struct Library LibNode;
	struct ExecBase *ExecBase;
	BPTR LibSegment;
	struct SignalSemaphore LockSemaphore;
};

#include "mystic_internal.h"
#include "mystic_data.h"
#include "mystic_libprotos.h"


/*****************************************************************************/

#ifdef USEPOOLS
	extern APTR mainpool;
	extern struct SignalSemaphore memsemaphore;
	extern void *Malloc(unsigned long size);
	extern void *Malloclear(unsigned long size);
	extern void _Free(void *mem);
#else
	#ifdef TRACKMEMORY
		extern void *Malloc(unsigned long size);
		extern void *Malloclear(unsigned long size);
		extern void _Free(void *mem);
	#else
		#define Malloc(s) AllocVec((s), MEMF_PUBLIC)
		#define Malloclear(s) AllocVec((s), MEMF_PUBLIC + MEMF_CLEAR)
		#define _Free(s) FreeVec((s))
	#endif
#endif

extern char *_StrDup(char *s);
extern void MemStats(LONG *numalloc, LONG *numbytes);
extern LONG _Strcmp(char *s1, char *s2);


#endif
