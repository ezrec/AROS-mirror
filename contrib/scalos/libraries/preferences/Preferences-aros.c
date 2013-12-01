// Preferences-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>
#include <proto/preferences.h>

#include <aros/libcall.h>

#include "Preferences_base.h"
#include "Preferences.h"

//----------------------------------------------------------------------------
// Standard library functions

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libbase, 1, Preferences
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, Preferences
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, Preferences
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, Preferences
);


//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	Preferences_1_Openlib,
	Preferences_2_Closelib,
	Preferences_3_Expungelib,
	Preferences_4_Extfunclib,
	PreferencesBase_0_LIBAllocPrefsHandle,
	PreferencesBase_0_LIBFreePrefsHandle,
	PreferencesBase_0_LIBSetPreferences,
	PreferencesBase_0_LIBGetPreferences,
	PreferencesBase_0_LIBReadPrefsHandle,
	PreferencesBase_0_LIBWritePrefsHandle,
	PreferencesBase_0_LIBFindPreferences,
	PreferencesBase_0_LIBSetEntry,
	PreferencesBase_0_LIBGetEntry,
	PreferencesBase_0_LIBRemEntry,
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
	RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};


//----------------------------------------------------------------------------

        static AROS_UFH3(struct Library *, Initlib,
		 AROS_UFHA(struct Library *, libbase, D0),
		 AROS_UFHA(struct SegList *, seglist, A0),
		 AROS_UFHA(struct ExecBase *, sysbase, A6)
)
{
	AROS_USERFUNC_INIT

	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	PrefsLibBase->prb_LibNode.lib_Revision = LIB_REVISION;
	PrefsLibBase->prb_SegList = seglist;

	PrefsLibBase->prb_Initialized = FALSE;

	if (!PreferencesInit(PrefsLibBase))
		{
		Preferences_3_Expungelib(&PrefsLibBase->prb_LibNode, &PrefsLibBase->prb_LibNode);
		PrefsLibBase = NULL;
		}

	return PrefsLibBase ? &PrefsLibBase->prb_LibNode : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, Preferences
)
{
	AROS_LIBFUNC_INIT

	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt++;
	PrefsLibBase->prb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!PrefsLibBase->prb_Initialized)
		{
		if (!PreferencesOpen(PrefsLibBase))
			{
			Preferences_2_Closelib(&PrefsLibBase->prb_LibNode);
			return NULL;
			}

		PrefsLibBase->prb_Initialized = TRUE;
		}

	return &PrefsLibBase->prb_LibNode;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, Preferences
)
{
	AROS_LIBFUNC_INIT

	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt--;

	if (0 == PrefsLibBase->prb_LibNode.lib_OpenCnt)
		{
		if (PrefsLibBase->prb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Preferences_3_Expungelib(&PrefsLibBase->prb_LibNode, &PrefsLibBase->prb_LibNode);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, Preferences
)
{
	AROS_LIBFUNC_INIT

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

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, Preferences
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}

//----------------------------------------------------------------------------
