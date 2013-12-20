// AmigaIconObject.h
// $Date$
// $Revision$


#ifndef	AMIGAICONOBJECT_H_INCLUDED
#define	AMIGAICONOBJECT_H_INCLUDED

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>

#include <defs.h>

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	16384
#define	MEMPOOL_THRESHSIZE	16384

//-----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	10

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//-----------------------------------------------------------------------------

struct AmigaIconObjectDtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	};

//----------------------------------------------------------------------------

ULONG InitDatatype(struct AmigaIconObjectDtLibBase *dtLib);
ULONG OpenDatatype(struct AmigaIconObjectDtLibBase *dtLib);
ULONG CloseDatatype(struct AmigaIconObjectDtLibBase *dtLib);

LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

//-----------------------------------------------------------------------------

struct InstanceData
	{
   	struct DiskObject	*aio_DiskObject;
   	struct DrawerData	*aio_myDrawerData;
   	struct Screen		*aio_Screen;

	UWORD			aio_ImageLeft;
	UWORD			aio_ImageTop;

	UBYTE			aio_DoNotFreeDiskObject;	//Flag: free icon on quit ?
	};

//---------------------------------------------------------------

// private and temporary Data during dtWrite

struct WriteData
	{
	STRPTR		aiowd_DefaultTool;
	STRPTR		*aiowd_ToolTypes;
	ULONG		aiowd_StackSize;
	struct IBox 	*aiowd_WindowRect;
	ULONG		aiowd_CurrentX;
	ULONG		aiowd_CurrentY;
	ULONG		aiowd_Flags;
	ULONG		aiowd_ViewModes;
	ULONG		aiowd_Type;
	};

/* ------------------------------------------------- */

#if !defined(__amigaos4__) && !defined(__AROS__)
VOID UpdateWorkbench(CONST_STRPTR name, BPTR parentlock, LONG action);

#if defined(__MORPHOS__)

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#define UpdateWorkbench(__p0, __p1, __p2) \
	LP3NR(30, UpdateWorkbench, \
		CONST_STRPTR , __p0, a0, \
		BPTR, __p1, a1, \
		LONG, __p2, d0, \
		, WB_BASE_NAME, 0, 0, 0, 0, 0, 0)

#else

#ifdef __SASC

#ifdef __CLIB_PRAGMA_LIBCALL
#pragma  libcall WorkbenchBase UpdateWorkbench        01e 09803
#endif /* __CLIB_PRAGMA_LIBCALL */

#else

#define WBInfo(lock, name, screen) \
	LP3NR(30, UpdateWorkbench, CONST_STRPTR, name, a0, BPTR, parentlock, a1, LONG, action, d0, \
	, WB_BASE_NAME)

#endif /* __SASC */

#endif /* __MORPHOS__ */

#endif /* !__amigaos4__ */

/* ------------------------------------------------- */

#define	Sizeof(x)	(sizeof(x) / sizeof(x[0]))

/* ------------------------------------------------- */

// from debug.lib
#ifdef __AROS__
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif


#define	d1(x)		;
#define	d2(x)		{ Forbid(); x; Permit(); }

/* ------------------------------------------------- */

#endif // AMIGAICONOBJECT_H_INCLUDED
