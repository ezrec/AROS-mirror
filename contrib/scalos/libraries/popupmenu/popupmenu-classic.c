// popupmenu-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>
#include <proto/iconobject.h>


#include "pmpriv.h"
#include "pmfind.h"

//----------------------------------------------------------------------------

// defined in pminit.c
extern struct ExecBase  *SysBase;

struct SegList;

//----------------------------------------------------------------------------
// Standard library functions

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

// defined in pmversion.c
extern char _LibID[];

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

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

	LIBPM_MakeMenuA,
	LIBPM_MakeItemA,
	LIBPM_FreePopupMenu,
	LIBPM_OpenPopupMenuA,
	LIBPM_MakeIDListA,
	LIBPM_ItemChecked,
	LIBPM_GetItemAttrsA,
	LIBPM_SetItemAttrsA,
	LIBPM_FindItem,
	LIBPM_AlterState,
	LIBPM_OBSOLETEFilterIMsgA,
	LIBPM_ExLstA,
	LIBPM_FilterIMsgA,
	LIBPM_InsertMenuItemA,
	LIBPM_RemoveMenuItem,
	LIBPM_AbortHook,
	LIBPM_GetVersion,
	LIBPM_ReloadPrefs,
	LIBPM_LayoutMenuA,
	LIBPM_RESERVED1,
	LIBPM_FreeIDList,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct PopupMenuBase),
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
	POPUPMENU_VERSION,
	NT_LIBRARY,
	0,
	POPUPMENU_NAME,
	_LibID,
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase))
{
	struct PopupMenuBase *PopupMenuBase = (struct PopupMenuBase *) libbase;

	SysBase = sysbase;

	PopupMenuBase->pmb_ExecBase = (struct Library *) SysBase;
	PopupMenuBase->pmb_Library.lib_Revision = POPUPMENU_REVISION;
	PopupMenuBase->pmb_SegList = (ULONG) seglist;

#if 0
	if (!IconObjectInit(PopupMenuBase))
		{
		CALLLIBFUNC(Expungelib, &PopupMenuBase->pmb_Library);
		PopupMenuBase = NULL;
		}
#endif

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  PopupMenuBase=%08lx\n", __LINE__, PopupMenuBase));

	return PopupMenuBase ? &PopupMenuBase->pmb_Library : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct PopupMenuBase *PopupMenuBase = (struct PopupMenuBase *) libbase;

	PopupMenuBase->pmb_Library.lib_OpenCnt++;
	PopupMenuBase->pmb_Library.lib_Flags &= ~LIBF_DELEXP;

	if (!PMLibInit(PopupMenuBase))
		{
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  fail\n", __LINE__));
		CALLLIBFUNC(Closelib, &PopupMenuBase->pmb_Library);
		return NULL;
		}

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  success\n", __LINE__));

	return &PopupMenuBase->pmb_Library;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct PopupMenuBase *PopupMenuBase = (struct PopupMenuBase *) libbase;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  OpenCnt=%lu\n", __LINE__, PopupMenuBase->pmb_Library.lib_OpenCnt));

	PopupMenuBase->pmb_Library.lib_OpenCnt--;

	if (0 == PopupMenuBase->pmb_Library.lib_OpenCnt)
		{
		if (PopupMenuBase->pmb_Library.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &PopupMenuBase->pmb_Library);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct PopupMenuBase *PopupMenuBase = (struct PopupMenuBase *) libbase;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  OpenCnt=%lu\n", __LINE__, PopupMenuBase->pmb_Library.lib_OpenCnt));

	if (0 == PopupMenuBase->pmb_Library.lib_OpenCnt)
		{
		ULONG size = PopupMenuBase->pmb_Library.lib_NegSize + PopupMenuBase->pmb_Library.lib_PosSize;
		UBYTE *ptr = (UBYTE *) PopupMenuBase - PopupMenuBase->pmb_Library.lib_NegSize;
		struct SegList *libseglist = (struct SegList *) PopupMenuBase->pmb_SegList;

		Remove((struct Node *) PopupMenuBase);
		PMLibCleanup(PopupMenuBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	PopupMenuBase->pmb_Library.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	return 0;
}
LIBFUNC_END


