// Preferences-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/emulation.h>
#include <exec/exectags.h>
#include <exec/resident.h>

#include <proto/exec.h>
#include <interfaces/preferences.h>

#include <stdarg.h>

#include "Preferences_base.h"
#include "Preferences.h"

#include <defs.h>

int _start(void)
{
	return -1;
}

static struct Library *Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec);
static BPTR Expungelib(struct LibraryManagerInterface *Self);
static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version);
static BPTR Closelib(struct LibraryManagerInterface *Self);
static ULONG Obtainlib(struct LibraryManagerInterface *Self);
static ULONG Releaselib(struct LibraryManagerInterface *Self);

extern const CONST_APTR VecTable68K[];

/* OS4.0 Library */

/* ------------------- OS4 Manager Interface ------------------------ */
static const APTR managerfunctable[] =
{
  (APTR)Obtainlib,
  (APTR)Releaselib,
  (APTR)NULL,
  (APTR)NULL,
  (APTR)Openlib,
  (APTR)Closelib,
  (APTR)Expungelib,
  (APTR)NULL,
  (APTR)-1
};

static const struct TagItem managertags[] =
{
  {MIT_Name,             (ULONG)"__library"},
  {MIT_VectorTable,      (ULONG)managerfunctable},
  {MIT_Version,          1},
  {TAG_DONE,             0}
};

/* ---------------------- OS4 Main Interface ------------------------ */
static APTR functable[] =
	{
	Obtainlib,
	Releaselib,
	NULL,
	NULL,
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

static const struct TagItem maintags[] =
	{
	{MIT_Name,             (ULONG)"main"},
	{MIT_VectorTable,      (ULONG)functable},
	{MIT_Version,          1},
	{TAG_DONE,             0}
	};

/* Init table used in library initialization. */
static const ULONG interfaces[] =
{
   (ULONG)managertags,
   (ULONG)maintags,
   (ULONG)0
};

static const struct TagItem inittab[] =
	{
	{CLT_DataSize, (ULONG)sizeof(struct PreferencesBase)},
	{CLT_Interfaces, (ULONG) interfaces},
	{CLT_Vector68K,  (ULONG)VecTable68K},
	{CLT_InitFunc, (ULONG) Initlib},
	{TAG_DONE, 0}
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
	RTF_NATIVE | RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	(STRPTR)libName,
	(STRPTR)libIdString,
	(struct TagItem *)inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) libbase;
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	PrefsLibBase->prb_LibNode.lib_Revision = LIB_REVISION;
	PrefsLibBase->prb_SegList = (struct SegList *)seglist;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (!PreferencesInit(PrefsLibBase))
		{
		Expungelib(Self);
		PrefsLibBase = NULL;
		}

	d1(kprintf(__FILE__ "/%s/%ld: PrefsLibBase=%08lx\n", __FUNC__, __LINE__, PrefsLibBase));

	return PrefsLibBase ? &PrefsLibBase->prb_LibNode : NULL;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) Self->Data.LibBase;

	if (0 == PrefsLibBase->prb_LibNode.lib_OpenCnt)
		{
		struct SegList *libseglist = PrefsLibBase->prb_SegList;

		d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
		
		Remove((struct Node *) PrefsLibBase);
		PreferencesCleanup(PrefsLibBase);
		DeleteLibrary((struct Library *)PrefsLibBase);

		return (BPTR)libseglist;
		}

	PrefsLibBase->prb_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) Self->Data.LibBase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt++;
	PrefsLibBase->prb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	d1(kprintf(__FILE__ "/%s/%ld: OpenCnt=%ld\n", __FUNC__, __LINE__, PrefsLibBase->prb_LibNode.lib_OpenCnt));

	if (!PrefsLibBase->prb_Initialized)
		{
		if (!PreferencesOpen(PrefsLibBase))
			{
			d1(kprintf(__FILE__ "/%s/%ld: Initialization failed\n", __FUNC__, __LINE__));

			Closelib(Self);
			return NULL;
			}

		PrefsLibBase->prb_Initialized = TRUE;
		}

	d1(kprintf(__FILE__ "/%s/%ld: Initialization succeeded\n", __FUNC__, __LINE__));

	return (struct LibraryHeader *)&PrefsLibBase->prb_LibNode;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct PreferencesBase *PrefsLibBase = (struct PreferencesBase *) Self->Data.LibBase;

	PrefsLibBase->prb_LibNode.lib_OpenCnt--;

	d1(kprintf(__FILE__ "/%s/%ld: OpenCnt=%ld\n", __FUNC__, __LINE__, PrefsLibBase->prb_LibNode.lib_OpenCnt));
/*
	if (0 == PrefsLibBase->prb_LibNode.lib_OpenCnt)
		{
		if (PrefsLibBase->prb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(Self);
			}
		}
*/
	return (BPTR)NULL;
}

static ULONG Obtainlib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount++);
}

static ULONG Releaselib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount--);
}
