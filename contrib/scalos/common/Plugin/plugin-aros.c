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

#include <aros/libcall.h>

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

#define FUNCNAME(NAME) PluginBase_0##_##NAME

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

struct Library *aroscbase;

//----------------------------------------------------------------------------

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(BPTR, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysBase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, base, 1, Plugin
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, Plugin
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, base, 3, Plugin
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, Plugin
);

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
	Plugin_1_Openlib,
	Plugin_2_Closelib,
	Plugin_3_Expungelib,
	Plugin_4_Extfunclib,
#if PLUGIN_TYPE == OOP
	Dummy_0_GetClassInfo,
#elif PLUGIN_TYPE == FILETYPE
	FT_INFOSTRING_AROS,
#endif
#if PLUGIN_TYPE == PREVIEW
	PV_GENERATE,
#endif
#if (PLUGIN_TYPE == PREFS)
	PluginBase_0_LIBSCAGetPrefsInfo,
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
	RTF_AUTOINIT,
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


//----------------------------------------------------------------------------

static AROS_UFH3(struct Library *, Initlib,
	 AROS_UFHA(struct Library *, libbase, D0),
	 AROS_UFHA(BPTR, seglist, A0),
	 AROS_UFHA(struct ExecBase *, sysBase, A6)
)
{
	AROS_USERFUNC_INIT

	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	(void)libVerString;
	SysBase = sysBase;

	PluginBase->pl_Initialized = FALSE;

	PluginBase->pl_LibNode.lib_Revision = LIB_REVISION;
	PluginBase->pl_SegList = seglist;

	aroscbase = OpenLibrary("arosc.library" ,0 );

#if PLUGIN_TYPE == OOP
	PluginBase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	return &PluginBase->pl_LibNode;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, Plugin
)
{
	AROS_LIBFUNC_INIT

	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	PluginBase->pl_LibNode.lib_OpenCnt++;
	PluginBase->pl_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!PluginBase->pl_Initialized)
		{
		if (!initPlugin(PluginBase))
			{
			Plugin_2_Closelib(&PluginBase->pl_LibNode);
			return NULL;
			}
		PluginBase->pl_Initialized = TRUE;
		}

	return &PluginBase->pl_LibNode;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, Plugin
)
{
	AROS_LIBFUNC_INIT

	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	PluginBase->pl_LibNode.lib_OpenCnt--;

	if (0 == PluginBase->pl_LibNode.lib_OpenCnt)
		{
		if (PluginBase->pl_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Plugin_3_Expungelib(&PluginBase->pl_LibNode, &PluginBase->pl_LibNode);
			}
		}

	return (BPTR)NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, Plugin
)
{
	AROS_LIBFUNC_INIT

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

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib, __unused struct Library *, libbase, 4, Plugin)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}


//----------------------------------------------------------------------------
