// Preferences-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>
#include <proto/preferences.h>


#include "Preferences_base.h"
#include "Preferences.h"

//----------------------------------------------------------------------------
// Standard library functions

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
	LIBAllocPrefsHandle,
	LIBFreePrefsHandle,
	LIBSetPreferences,
	LIBGetPreferences,
	LIBReadPrefsHandle,
	LIBWritePrefsHandle,
	LIBFindPreferences,
	LIBSetEntry,
	LIBGetEntry,
	LIBRemEntry,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct PreferencesBase),
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
#else
	RTF_AUTOINIT,
#endif
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase))
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	PrefsLibBase->prb_LibNode.lib_Revision = LIB_REVISION;
	PrefsLibBase->prb_SegList = seglist;

	PrefsLibBase->prb_Initialized = FALSE;

	if (!PreferencesInit(PrefsLibBase))
		{
		CALLLIBFUNC(Expungelib, &PrefsLibBase->prb_LibNode);
		PrefsLibBase = NULL;
		}

	return PrefsLibBase ? &PrefsLibBase->prb_LibNode : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt++;
	PrefsLibBase->prb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!PrefsLibBase->prb_Initialized)
		{
		if (!PreferencesOpen(PrefsLibBase))
			{
			CALLLIBFUNC(Closelib, &PrefsLibBase->prb_LibNode);
			return NULL;
			}

		PrefsLibBase->prb_Initialized = TRUE;
		}

	return &PrefsLibBase->prb_LibNode;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt--;

	if (0 == PrefsLibBase->prb_LibNode.lib_OpenCnt)
		{
		if (PrefsLibBase->prb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &PrefsLibBase->prb_LibNode);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	if (0 == PrefsLibBase->prb_LibNode.lib_OpenCnt)
		{
		ULONG size = PrefsLibBase->prb_LibNode.lib_NegSize + PrefsLibBase->prb_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) PrefsLibBase - PrefsLibBase->prb_LibNode.lib_NegSize;
		struct SegList *libseglist = PrefsLibBase->prb_SegList;

		Remove((struct Node *) PrefsLibBase);
		PreferencesCleanup(PrefsLibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	PrefsLibBase->prb_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	return 0;
}
LIBFUNC_END

//----------------------------------------------------------------------------

