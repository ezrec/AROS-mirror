// popupmenu-aos4.c
// $Date$
// $Revision$

#include <exec/interfaces.h>
#include <exec/emulation.h>
#include <exec/exectags.h>
#include <exec/resident.h>
#include <intuition/intuition.h>

#include <proto/exec.h>

#ifdef __USE_INLINE__
#undef __USE_INLINE__
#endif

#include <interfaces/pm.h>

#include <stdarg.h>

#include "pmpriv.h"

//----------------------------------------------------------------------------

// defined in pminit.c
extern struct ExecBase  *SysBase;

// defined in pmversion.c
extern char _LibID[];

extern const CONST_APTR VecTable68K[];

//----------------------------------------------------------------------------

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

LIBFUNC_P1VA_PROTO(struct PopupMenu *, LIBPM_MakeMenu,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P1VA_PROTO(struct PopupMenu *, LIBPM_MakeItem,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P1VA_PROTO(struct PM_IDLst *, LIBPM_MakeIDList,
	A6, struct PopupMenuBase *, PopupMenuBase);

LIBFUNC_P2VA_PROTO(ULONG, LIBPM_OpenPopupMenu,
	A1, struct Window *, wnd,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P2VA_PROTO(LONG, LIBPM_GetItemAttrs,
	A2, struct PopupMenu *, item,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P2VA_PROTO(LONG, LIBPM_SetItemAttrs,
	A2, struct PopupMenu *, item,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P2VA_PROTO(struct PM_IDLst *, LIBPM_ExLst,
	A1, ULONG, id,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P4VA_PROTO(APTR, LIBPM_FilterIMsg,
	A0, struct Window *, window,
	A1, struct PopupMenu *, menu,
	A2, struct IntuiMessage *, imsg,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P3VA_PROTO(LONG, LIBPM_LayoutMenu,
	A0, struct Window *, window,
	A1, struct PopupMenu *, menu,
	A6, struct PopupMenuBase *, PopupMenuBase);
LIBFUNC_P2VA_PROTO(LONG, LIBPM_InsertMenuItem,
	A2, struct PopupMenu *, base,
	A6, struct PopupMenuBase *, l);

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
	LIBPM_MakeMenuA,
	LIBPM_MakeMenu,
	LIBPM_MakeItemA,
	LIBPM_MakeItem,
	LIBPM_FreePopupMenu,
	LIBPM_OpenPopupMenuA,
	LIBPM_OpenPopupMenu,
	LIBPM_MakeIDListA,
	LIBPM_MakeIDList,
	LIBPM_ItemChecked,
	LIBPM_GetItemAttrsA,
	LIBPM_GetItemAttrs,
	LIBPM_SetItemAttrsA,
	LIBPM_SetItemAttrs,
	LIBPM_FindItem,
	LIBPM_AlterState,
	LIBPM_OBSOLETEFilterIMsgA,
	LIBPM_ExLstA,
	LIBPM_ExLst,
	LIBPM_FilterIMsgA,
	LIBPM_FilterIMsg,
	LIBPM_InsertMenuItemA,
	LIBPM_InsertMenuItem,
	LIBPM_RemoveMenuItem,
	LIBPM_AbortHook,
	LIBPM_GetVersion,
	LIBPM_ReloadPrefs,
	LIBPM_LayoutMenuA,
	LIBPM_LayoutMenu,
	LIBPM_RESERVED1,
	LIBPM_FreeIDList,

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
	{CLT_DataSize, (ULONG)sizeof(struct PopupMenuBase)},
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
	POPUPMENU_VERSION,
	NT_LIBRARY,
	0,
	POPUPMENU_NAME,
	(char *)_LibID,
	(APTR)inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct PopupMenuBase *PMLibBase = (struct PopupMenuBase *) libbase;
//	  struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
//	  struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	PMLibBase->pmb_Library.lib_Revision = POPUPMENU_REVISION;
	PMLibBase->pmb_SegList = seglist;

	return PMLibBase ? &PMLibBase->pmb_Library : NULL;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct PopupMenuBase *PMLibBase = (struct PopupMenuBase *) Self->Data.LibBase;

	if (0 == PMLibBase->pmb_Library.lib_OpenCnt)
		{
		BPTR libseglist = PMLibBase->pmb_SegList;

		Remove((struct Node *) PMLibBase);
		PMLibCleanup(PMLibBase);
		DeleteLibrary((struct Library *)PMLibBase);

		return libseglist;
		}

	PMLibBase->pmb_Library.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct PopupMenuBase *PMLibBase = (struct PopupMenuBase *) Self->Data.LibBase;

	PMLibBase->pmb_Library.lib_OpenCnt++;
	PMLibBase->pmb_Library.lib_Flags &= ~LIBF_DELEXP;

	if (!PMLibInit(PMLibBase))
		{
		Closelib(Self);
		return NULL;
		}

	return (struct LibraryHeader *)&PMLibBase->pmb_Library;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct PopupMenuBase *PMLibBase = (struct PopupMenuBase *) Self->Data.LibBase;

	PMLibBase->pmb_Library.lib_OpenCnt--;
/*
	if (0 == PMLibBase->pmb_Library.lib_OpenCnt)
		{
		if (PMLibBase->pmb_Library.lib_Flags & LIBF_DELEXP)
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

/* ------------------- OS4 Varargs Functions ------------------------ */
LIBFUNC_P1VA(struct PopupMenu *, LIBPM_MakeMenu,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	struct PopupMenu *ret;
  	va_list args;
	va_startlinear(args, self); // self is always the first argument

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_MakeMenuA(va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P1VA(struct PopupMenu *, LIBPM_MakeItem,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	struct PopupMenu *ret;
  	va_list args;
	va_startlinear(args, self); // self is always the first argument

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_MakeItemA(va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P1VA(struct PM_IDLst *, LIBPM_MakeIDList,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	struct PM_IDLst	*ret;
  	va_list args;
	va_startlinear(args, self); // self is always the first argument

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_MakeIDListA(va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(ULONG, LIBPM_OpenPopupMenu,
	A1, struct Window *, wnd,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	ULONG ret;
  	va_list args;
	va_startlinear(args, wnd);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_OpenPopupMenuA(wnd, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(LONG, LIBPM_GetItemAttrs,
	A2, struct PopupMenu *, item,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	LONG ret;
  	va_list args;
	va_startlinear(args, item);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_GetItemAttrsA(item, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(LONG, LIBPM_SetItemAttrs,
	A2, struct PopupMenu *, item,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	LONG ret;
  	va_list args;
	va_startlinear(args, item);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_SetItemAttrsA(item, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(struct PM_IDLst *, LIBPM_ExLst,
	A1, ULONG, id,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	struct PM_IDLst *ret;
  	va_list args;
	va_startlinear(args, id);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_ExLstA(&id);

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P4VA(APTR, LIBPM_FilterIMsg,
	A0, struct Window *, window,
	A1, struct PopupMenu *, menu,
	A2, struct IntuiMessage *, imsg,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	APTR ret;
  	va_list args;
	va_startlinear(args, imsg);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_FilterIMsgA(window, menu, imsg, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P3VA(LONG, LIBPM_LayoutMenu,
	A0, struct Window *, window,
	A1, struct PopupMenu *, menu,
	A6, struct PopupMenuBase *, PopupMenuBase);
{
	LONG ret;
	va_list args;
	va_startlinear(args, menu);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_LayoutMenuA(window, menu, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(LONG, LIBPM_InsertMenuItem,
	A2, struct PopupMenu *, base,
	A6, struct PopupMenuBase *, PopupMenuBase)
{
	LONG ret;
  	va_list args;
	va_startlinear(args, base);

	(void)PopupMenuBase;

	ret = ((struct PopupMenuIFace *)self)->PM_InsertMenuItemA(base, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END


