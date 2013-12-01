// plugin-classic.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <exec/types.h>
#include <exec/resident.h>

#include <libraries/iffparse.h>
#include <scalos/scalos.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>

#include <defs.h>

#include "plugin.h"

#ifndef d
#define d(x)
#endif
#ifndef d1
#define d1(x)
#endif
#ifndef d2
#define d2(x) x
#endif

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

struct ExecBase *SysBase;

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, BPTR seglist), LIB_REG(a6, struct ExecBase *sysBase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, BPTR);
static LIBFUNC_PROTO(Expungelib, libbase, BPTR);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

#include "plugin-common.c"

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif /* __MORPHOS__ */
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
#if PLUGIN_TYPE == OOP
	GetClassInfo,
#elif PLUGIN_TYPE == FILETYPE
	FT_INFOSTRING,
#endif
#if PLUGIN_TYPE == PREVIEW
	PV_GENERATE,
#endif
#if (PLUGIN_TYPE == PREFS)
	LIBSCAGetPrefsInfo,
#endif
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(LIB_BASETYPE),
	(ULONG) functable,
	0,
	(ULONG) Initlib
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else /* __MORPHOS__ */
	RTF_AUTOINIT,
#endif /* __MORPHOS__ */
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
#ifdef LIB_IDSTRING
	libIdString,
#else /* LIB_IDSTRING */
	&libVerString[6],
#endif /* LIB_IDSTRING */
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif /* __MORPHOS__ */

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, BPTR seglist), LIB_REG(a6, struct ExecBase *sysBase))
{
	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	(void)libVerString;
	SysBase = sysBase;

	PluginBase->pl_Initialized = FALSE;

	PluginBase->pl_LibNode.lib_Revision = LIB_REVISION;
	PluginBase->pl_SegList = seglist;

#if PLUGIN_TYPE == OOP
	PluginBase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	return &PluginBase->pl_LibNode;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	PluginBase->pl_LibNode.lib_OpenCnt++;
	PluginBase->pl_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!PluginBase->pl_Initialized)
		{
		if (!initPlugin(PluginBase))
			{
			CALLLIBFUNC(Closelib, &PluginBase->pl_LibNode);
			return NULL;
			}
		PluginBase->pl_Initialized = TRUE;
		}

	return &PluginBase->pl_LibNode;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, BPTR)
{
	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	PluginBase->pl_LibNode.lib_OpenCnt--;

	if (0 == PluginBase->pl_LibNode.lib_OpenCnt)
		{
		if (PluginBase->pl_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &PluginBase->pl_LibNode);
			}
		}

	return (BPTR)NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, BPTR)
{
	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	if (0 == PluginBase->pl_LibNode.lib_OpenCnt)
		{
		ULONG size = PluginBase->pl_LibNode.lib_NegSize + PluginBase->pl_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) PluginBase - PluginBase->pl_LibNode.lib_NegSize;
		BPTR libseglist = PluginBase->pl_SegList;

		Remove((struct Node *) PluginBase);
		closePlugin(PluginBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	PluginBase->pl_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;
	return 0;
}
LIBFUNC_END


//----------------------------------------------------------------------------
