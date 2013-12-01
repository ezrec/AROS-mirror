// deficons_library.c
// 13 Nov 2004 17:23:19


#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <exec/types.h>
#include <exec/resident.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>

#include <scalos/scalos.h>

#include <defs.h>
#include "deficons.h"

//----------------------------------------------------------------------------

struct DefIconsLibraryBase
	{
	struct Library deficons_LibNode;
	ULONG deficons_PlugID;
	struct SegList *deficons_SegList;
	};

//----------------------------------------------------------------------------

static SAVEDS(struct Library *) ASM INTERRUPT Initlib(REG(d0, struct Library *libbase),
	REG(a0, struct SegList *seglist), REG(a6, struct ExecBase *sysbase));
static SAVEDS(struct Library *) INTERRUPT ASM Openlib(REG(a6, struct Library *libbase));
static SAVEDS(struct SegList *) ASM INTERRUPT Closelib(REG(a6, struct Library *libbase));
static SAVEDS(struct SegList *) ASM INTERRUPT Expungelib(REG(a6, struct Library *libbase));
static SAVEDS(ULONG) ASM INTERRUPT Extfunclib(REG(a6, struct Library *libbase));
static SAVEDS(struct ScaClassInfo *) ASM INTERRUPT GetClassInfo(REG(a6, struct Library *libbase));

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

#define	LIB_VERSION	45
#define	LIB_REVISION	6

static char __aligned libName[] = "deficons.plugin";
static char __aligned libIdString[] = "$VER: deficons.plugin 45.6 (30 Dec 2001 16:12:22)";

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
	GetClassInfo,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct DefIconsLibraryBase),
	(ULONG) functable,
	0,
	(ULONG) Initlib
	};


/* The ROM tag */
struct Resident __aligned romtag =
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

static SAVEDS(struct Library *) ASM INTERRUPT Initlib(REG(d0, struct Library *libbase),
	REG(a0, struct SegList *seglist), REG(a6, struct ExecBase *sysbase))
{
	struct DefIconsLibraryBase *DefIconsLibBase = (struct DefIconsLibraryBase *) libbase;

	DefIconsLibBase->deficons_LibNode.lib_Revision = LIB_REVISION;
	DefIconsLibBase->deficons_SegList = seglist;

	DefIconsLibBase->deficons_PlugID = MAKE_ID('P','L','U','G');

	return &DefIconsLibBase->deficons_LibNode;
}


static SAVEDS(struct Library *) INTERRUPT ASM Openlib(REG(a6, struct Library *libbase))
{
	struct DefIconsLibraryBase *DefIconsLibBase = (struct DefIconsLibraryBase *) libbase;

	DefIconsLibBase->deficons_LibNode.lib_OpenCnt++;
	DefIconsLibBase->deficons_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!initDefIcons())
		{
		Closelib(&DefIconsLibBase->deficons_LibNode);
		return NULL;
		}

	return &DefIconsLibBase->deficons_LibNode;
}


static SAVEDS(struct SegList *) ASM INTERRUPT Closelib(REG(a6, struct Library *libbase))
{
	struct DefIconsLibraryBase *DefIconsLibBase = (struct DefIconsLibraryBase *) libbase;

	DefIconsLibBase->deficons_LibNode.lib_OpenCnt--;

	if (0 == DefIconsLibBase->deficons_LibNode.lib_OpenCnt)
		{
		if (DefIconsLibBase->deficons_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(&DefIconsLibBase->deficons_LibNode);
			}
		}

	return NULL;
}


static SAVEDS(struct SegList *) ASM INTERRUPT Expungelib(REG(a6, struct Library *libbase))
{
	struct DefIconsLibraryBase *DefIconsLibBase = (struct DefIconsLibraryBase *) libbase;

	if (0 == DefIconsLibBase->deficons_LibNode.lib_OpenCnt)
		{
		ULONG size = DefIconsLibBase->deficons_LibNode.lib_NegSize + DefIconsLibBase->deficons_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) DefIconsLibBase - DefIconsLibBase->deficons_LibNode.lib_NegSize;
		struct SegList *libseglist = DefIconsLibBase->deficons_SegList;

		Remove((struct Node *) DefIconsLibBase);
		cleanupDefIcons();
		FreeMem(ptr,size);

		return libseglist;
		}

	DefIconsLibBase->deficons_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;
}


static SAVEDS(ULONG) ASM INTERRUPT Extfunclib(REG(a6, struct Library *libbase))
{
	return 0;
}


static SAVEDS(struct ScaClassInfo *) ASM INTERRUPT GetClassInfo(REG(a6, struct Library *libbase))
{
	static struct ScaClassInfo LibClassInfo =
		{
		{ NULL, NULL, DefIconsHookFunc, NULL, NULL },

		-79,				// Priority (just after IconWindow.sca !!!)
		2 * 4,				// Instance Size
		41,
		0,
		"IconWindow.sca",
		"IconWindow.sca",
		"Scalos DefIcons Class",
		"This class gives files lacking an icon a filetype dependent icon" COMPILER_STRING,
		"Jürgen Lachmann",
		};

	return &LibClassInfo;
}

//----------------------------------------------------------------------------
