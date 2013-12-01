// IconObject-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>
#include <proto/iconobject.h>


#include "IconObject.h"

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
	LIBNewIconObject,
	LIBDisposeIconObject,
	LIBGetDefIconObject,
	LIBPutIconObject,
	LIBIsIconName,
	LIBConvert2IconObject,
	LIBConvert2IconObjectA,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct IconObjectBase),
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
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	SysBase = sysbase;
	IconObjLibBase->iob_LibNode.lib_Revision = LIB_REVISION;
	IconObjLibBase->iob_SegList = seglist;

	if (!IconObjectInit(IconObjLibBase))
		{
		CALLLIBFUNC(Expungelib, &IconObjLibBase->iob_LibNode);
		IconObjLibBase = NULL;
		}

	return IconObjLibBase ? &IconObjLibBase->iob_LibNode : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt++;
	IconObjLibBase->iob_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!IconObjectOpen(IconObjLibBase))
		{
		CALLLIBFUNC(Closelib, &IconObjLibBase->iob_LibNode);
		return NULL;
		}

	return &IconObjLibBase->iob_LibNode;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt--;

	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		if (IconObjLibBase->iob_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &IconObjLibBase->iob_LibNode);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		ULONG size = IconObjLibBase->iob_LibNode.lib_NegSize + IconObjLibBase->iob_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) IconObjLibBase - IconObjLibBase->iob_LibNode.lib_NegSize;
		struct SegList *libseglist = IconObjLibBase->iob_SegList;

		Remove((struct Node *) IconObjLibBase);
		IconObjectCleanup(IconObjLibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	IconObjLibBase->iob_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	return 0;
}
LIBFUNC_END


